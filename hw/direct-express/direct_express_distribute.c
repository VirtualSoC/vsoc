/**
 * @file direct_express_distribute.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 使用轮询实现取call，分发给其他draw线程，同时负责回收数据
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */
// #define STD_DEBUG_LOG

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_handle_thread.h"
#include "direct-express/express_log.h"
#include "direct-express/express_device_common.h"
// #include <EGL/eglplatform.h>

// #define express_printf null_printf
// #include <winsock2.h>
// #include <windows.h>

// #include "ui/console.h"

//这是VirtQueueElement里面的实际东西
// typedef struct VirtQueueElement
// {
//     //elem的翻译
//     unsigned int index; // header
//     unsigned int len;  //packe模式用的
//     unsigned int ndescs; //消耗的desc数目
//     unsigned int out_num; //out数组长度
//     unsigned int in_num;  //in数组长度
//     hwaddr *in_addr;  //desc中放的实际值（物理地址）
//     hwaddr *out_addr; //
//     struct iovec *in_sg;
//     struct iovec *out_sg; //实际地址和长度
// } VirtQueueElement;

static GHashTable *device_thread_info = NULL;

static VirtIODevice *direct_express_device;

//用于回收的队列缓冲区的头和尾指针，方便实现无锁的入队出队
// static Direct_Express_Call *call_recycle_header = NULL;
// static Direct_Express_Call *call_recycle_tail = NULL;

//用于回收call的队列，实现了无锁的入队，这里将它的大小设置为CALL_BUF_SIZE+2是为了保证队列不会爆，大小一定满足要求
//这里设置volatile是为了保证其在不同线程间同步不会受到缓存的影响
static Direct_Express_Call *call_recycle_queue[(CALL_BUF_SIZE + 2)];
static int call_recycle_queue_header;
static volatile int call_recycle_queue_tail;

static void *guest_null_ptr = NULL;

static void release_call(Direct_Express_Call *out_call);
static void push_free_callback(Direct_Express_Call *call, int notify);
void push_to_thread(Direct_Express_Call *call);
void init_distribute_event(void);
void distribute_wait(void);
void *alloc_buf_from_iov(struct iovec *sg, unsigned int num, size_t *all_cnt);


Direct_Express_Call *alloc_one_call();
void release_one_call(Direct_Express_Call *call);
Guest_Mem *alloc_one_guest_mem();
void release_one_guest_mem(Guest_Mem *mem);

//用于通知回收的事件，这里对于平台兼容性的部分尚未完成
typedef struct
{
#ifdef _WIN32
    HANDLE win_event;
#else

#endif
} RECYCLE_EVENT;

RECYCLE_EVENT recycle_event;

static Direct_Express_Call pre_alloc_call[CALL_BUF_SIZE * 2];
static bool pre_alloc_call_flag[CALL_BUF_SIZE * 2];

static int pre_alloc_call_loc = 0;

static Guest_Mem pre_guest_mem[CALL_BUF_SIZE * 2 * MAX_PARA_NUM];
static bool pre_guest_mem_flag[CALL_BUF_SIZE * 2 * MAX_PARA_NUM];

static int pre_guest_mem_loc = 0;

Direct_Express_Call *alloc_one_call()
{
    int cnt = 0;
    while (pre_alloc_call_flag[pre_alloc_call_loc] == true)
    {
        pre_alloc_call_loc = (pre_alloc_call_loc + 1) % (CALL_BUF_SIZE * 2);
        cnt++;
        if (cnt > CALL_BUF_SIZE * 2)
        {
            return NULL;
        }
    }
    pre_alloc_call_flag[pre_alloc_call_loc] = true;
    return pre_alloc_call + pre_alloc_call_loc;
}

void release_one_call(Direct_Express_Call *call)
{
    int loc = (int)(call - pre_alloc_call);
    if (loc < 0 || loc >= CALL_BUF_SIZE * 2)
    {
        return;
    }
    pre_alloc_call_flag[loc] = false;
}

Guest_Mem *alloc_one_guest_mem()
{
    int cnt = 0;
    while (pre_guest_mem_flag[pre_guest_mem_loc] == true)
    {
        pre_guest_mem_loc = (pre_guest_mem_loc + 1) % (CALL_BUF_SIZE * 2 * MAX_PARA_NUM);
        cnt++;
        if (cnt > CALL_BUF_SIZE * 2 * MAX_PARA_NUM)
        {
            return NULL;
        }
    }
    pre_guest_mem_flag[pre_guest_mem_loc] = true;
    return pre_guest_mem + pre_guest_mem_loc;
}

void release_one_guest_mem(Guest_Mem *mem)
{
    int loc = (int)(mem - pre_guest_mem);
    if (loc < 0 || loc >= CALL_BUF_SIZE * 2 * MAX_PARA_NUM)
    {
        return;
    }
    pre_guest_mem_flag[loc] = false;
}

// void guest_mem_copy(Guest_Mem *dst_guest_mem, Guest_Mem *src_guest_mem)
// {
//     dst_guest_mem->all_len = src_guest_mem->all_len;
//     dst_guest_mem->num = src_guest_mem->num;
//     if (dst_guest_mem->scatter_data != NULL)
//     {
//         g_free(dst_guest_mem->scatter_data);
//         dst_guest_mem->scatter_data = NULL;
//     }
//     dst_guest_mem->scatter_data = g_malloc(src_guest_mem->num * sizeof(Scatter_Data));
//     memcpy(dst_guest_mem->scatter_data, src_guest_mem->scatter_data, src_guest_mem->num * sizeof(Scatter_Data));
//     return;
// }

/**
 * @brief 获取直接的guest端指针，flag表示是否获取到了，返回guest端的指针，可能为NULL，因为当初传入的指针可能真的为NULL
 * 
 * @param guest_mem 
 * @param flag 
 * @return void* 
 */
void *get_direct_ptr(Guest_Mem *guest_mem, int *flag)
{
    if (guest_mem->num == 1)
    {
        Scatter_Data *guest_data = guest_mem->scatter_data;
        *flag = 1;
        //这里也可能返回NULL，所以以flag来区分
        return guest_data->data;
    }
    *flag = 0;
    return NULL;
}

/**
 * @brief guest向host写入数据
 * 
 * @param guest guest端数据，指向一个Guest_Mem结构体
 * @param host host端内存，指向一个host内存
 * @param start_loc 读取guest端的开始位置
 * @param length 读取guest的数据长度
 */
void guest_write(Guest_Mem *guest, void *host, size_t start_loc, size_t length)
{
    if (guest == NULL)
    {
        return;
    }
    express_printf("guest_write length %llu all_len %d\n", length, guest->all_len);
    Scatter_Data *guest_data = guest->scatter_data;
    if (length == 0 || host == NULL || length > guest->all_len)
    {
        printf("guest write error host %lx len %d %lld\n", host, guest->all_len, length);
        return;
    }
    // gint64 start_time=0;
    // int32_t spend_time=0;
    // if(length>5*1024*1024){
    //     start_time=g_get_real_time();
    // }

    host_guest_buffer_exchange(guest_data, (unsigned char *)host, start_loc, length, 1);

    // if(length>5*1024*1024){
    //     spend_time = (int32_t)(g_get_real_time()-start_time);
    //     if(spend_time>0){
    //         uint32_t mem_speed=1024*1024*24/(uint32_t)spend_time;

    //         express_printf("mem cpy speed %u\n", mem_speed);
    //         if(mem_speed>100 && mem_speed<1000000){
    //             *(uint32_t *)guest_null_ptr=mem_speed;
    //         }

    //     }
    // }
}

/**
 * @brief guest从host读入数据
 * 
 * @param guest guest端数据，指向一个Guest_Mem结构体
 * @param host host端内存，指向一个host内存
 * @param start_loc 写入guest端的开始位置
 * @param length 写入guest的数据长度
 */
void guest_read(Guest_Mem *guest, void *host, size_t start_loc, size_t length)
{
    if (guest == NULL)
    {
        return;
    }
    express_printf("guest_read length %llu all_len %d\n", length, guest->all_len);

    Scatter_Data *guest_data = guest->scatter_data;
    if (length == 0 || host == NULL || length > guest->all_len)
    {
        return;
    }
    express_printf("read %llu,%llu\n", start_loc, length);
    host_guest_buffer_exchange(guest_data, (unsigned char *)host, start_loc, length, 0);
}

/** 
 * @brief 交换scatter的guest数据和host数据
 * 
 * @param guest_data guest数据，指向一个Scatter_Data数组
 * @param host_data host数据，为正常内存指针
 * @param start_loc 需要交换的guest数据的开始位置
 * @param length 需要交换的长度
 * @param is_guest_to_host 
 */
void host_guest_buffer_exchange(Scatter_Data *guest_data, unsigned char *host_data, size_t start_loc, size_t length, int is_guest_to_host)
{

    // int walk_loc = 0;
    if (guest_data == NULL || host_data == NULL)
    {
        return;
    }

    size_t remain_len = length;
    // express_printf("memcpy data len %llu,%d\n",length,remain_len);
    int guest_loc = start_loc;
    int host_loc = 0;
    // int cpy_len = 0;
    int guest_index = 0;
    char *last_data = NULL;
    while (remain_len > 0 && remain_len < 100000000000)
    {
        if (guest_data[guest_index].len == 0 || guest_data[guest_index].data == NULL)
        {
            break;
        }
        if (guest_data[guest_index].len > guest_loc)
        {
            //一直找到start_loc所在的那个区块
            if (remain_len < guest_data[guest_index].len - guest_loc)
            {
                if (is_guest_to_host)
                {
                    memcpy(host_data + host_loc, guest_data[guest_index].data + guest_loc, remain_len);
                }
                else
                {

                    express_printf("memcpy data %lx index %d loc %d host %lx loc %d remain %llu\n", guest_data[guest_index].data, guest_index, guest_loc, host_data, host_loc, remain_len);
                    memcpy(guest_data[guest_index].data + guest_loc, host_data + host_loc, remain_len);
                }
                break;
            }
            else
            {
                if (is_guest_to_host)
                {
                    memcpy(host_data + host_loc, guest_data[guest_index].data + guest_loc, guest_data[guest_index].len - guest_loc);
                }
                else
                {

                    express_printf("memcpy data %lx index %d loc %d len %llu,host %lx loc %d remain %llu\n", guest_data[guest_index].data, guest_index, guest_loc, guest_data[guest_index].len, host_data, host_loc, remain_len);

                    if (last_data != guest_data[guest_index].data)
                    {
                        last_data = guest_data[guest_index].data;
                    }
                    else
                    {
                        printf("error map data! same scatter data pointer");
                    }

                    memcpy(guest_data[guest_index].data + guest_loc, host_data + host_loc, guest_data[guest_index].len - guest_loc);
                }
                host_loc += guest_data[guest_index].len - guest_loc;
                remain_len -= guest_data[guest_index].len - guest_loc;
            }
            //只要复制了一次之后guest_loc都为0，因为这个时候后面的都是从下一段内存的刚开始的位置开始（因为内存连续）
            guest_loc = 0;
        }
        else
        {
            guest_loc -= guest_data[guest_index].len;
        }
        guest_index++;
    }
}

/**
 * @brief 根据iov获得一个连续内存块，内存块内数据为iov数组的复制结果，这个函数一般用于有多个iov时
 * 
 * @param sg iov存放的位置
 * @param num iov的数目
 * @param all_cnt 需要回写的变量，表示到底复制了多少数据
 * @return void* 返回内存块的指针
 */
void *alloc_buf_from_iov(struct iovec *sg, unsigned int num, size_t *all_cnt)
{
    int buf_len = 0;
    for (int i = 0; i < num; i++)
    {
        buf_len += sg[i].iov_len;
    }
    /**
     * @todo 使用内存映射来实现buf
     * 
     */

    void *buf = g_malloc(buf_len);
    size_t ret = iov_to_buf(sg, num, 0, buf, buf_len);
    if (ret != buf_len)
    {
        //express_printf("error iov_to_buf\n");
        g_free(buf);
        return NULL;
    }
    *all_cnt = ret;
    return buf;
}

// /**
//  * @brief 检查调用的fun_id与参数数目的对应关系是否满足，防止瞎传
//  *
//  * @param id 调用的函数id
//  * @param num 参数的数目
//  * @return int 返回检查结果是否正确，1表示正确，0表示错误
//  */
// static int check_fun_id_para_num(int id, int num)
// {
//     // express_printf("mygpu get invoke %d %d\n", id, num);
//     return 1;
//     //todo
// }

/**
 * @brief 将Direct_Express_Queue_Elem内的数据填充完毕，也就是初始化Direct_Express_Queue_Elem中除了
 * VirtQueueElement的其他部分，例如para指针，type类型等
 * 
 * @param elem 需要填充的elem数据
 * @param id 需要回传的函数调用id（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @param thread_id 需要回传的线程id（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @param process_id 需要回传的进程id（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @param unique_id 需要回传的通道文件唯一id（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @param num 需要回传的参数数目（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @return int 返回填充是否完成，1表示完成，0表示失败
 */
static int fill_direct_express_queue_elem(Direct_Express_Queue_Elem *elem, unsigned long long *id, unsigned long long *thread_id, unsigned long long *process_id, unsigned long long *unique_id, unsigned long long *num)
{
    VirtQueueElement *v_elem = &elem->elem;
    // printf("fill elem num %u %u\n",v_elem->out_num,v_elem->in_num);
    if ((v_elem->out_num != 0 && v_elem->in_num != 0) || (v_elem->out_num == 0 && v_elem->in_num == 0))
    {
        return 0;
    }
    elem->para = NULL;
    elem->next = NULL;
    elem->type = 0;

    Guest_Mem *guest_mem = alloc_one_guest_mem();

    if(guest_mem == NULL)
    {
        printf("error! guest_mem alloc return NULL!\n");
    }

    if (v_elem->out_num != 0)
    {
        guest_mem->scatter_data = (Scatter_Data *)v_elem->out_sg;
        guest_mem->num = v_elem->out_num;
    }

    if (v_elem->in_num != 0)
    {
        guest_mem->scatter_data = (Scatter_Data *)v_elem->in_sg;
        guest_mem->num = v_elem->in_num;
    }

    int buf_len = 0;
    for (int i = 0; i < guest_mem->num; i++)
    {
        if (guest_mem->scatter_data[i].len == 4 && guest_mem->scatter_data[i].data == guest_null_ptr && v_elem->out_num == 1 && v_elem->in_num == 0)
        {
            express_printf("find null prt!!!\n");
            guest_mem->scatter_data[i].data = NULL;
            guest_mem->scatter_data[i].len = 0;
        }
        buf_len += guest_mem->scatter_data[i].len;

        // express_printf("guest_mem %d i %d len %d now %d\n",num,i, guest_mem->scatter_data[i].len, buf_len);
    }

    guest_mem->all_len = buf_len;
    elem->len = buf_len;

    elem->para = guest_mem;

    // //sg数目多余1个就要单独复制出来
    // if (v_elem->out_num > 1)
    // {
    //     elem->para = alloc_buf_from_iov(v_elem->out_sg, v_elem->out_num, &elem->len);
    //     elem->type |= COPY_PARA;
    // }
    // else if (v_elem->out_num == 1)
    // {
    //     //只有一个sg的情况下直接传递指针
    //     elem->para = v_elem->out_sg->iov_base;
    //     elem->len = v_elem->out_sg->iov_len;
    //     elem->type |= DIRECT_PARA;
    // }
    // if (v_elem->in_num > 1)
    // {
    //     elem->para = alloc_buf_from_iov(v_elem->in_sg, v_elem->in_num, &elem->len);
    //     elem->type |= COPY_PARA;
    // }
    // else if (v_elem->in_num == 1)
    // {
    //     elem->para = v_elem->in_sg->iov_base;
    //     elem->len = v_elem->in_sg->iov_len;
    //     elem->type |= DIRECT_PARA;
    // }
    // if (elem->para == NULL)
    // {
    //     return 0;
    // }
    if (id != NULL && num != NULL && thread_id != NULL && process_id != NULL && unique_id != NULL)
    {
        //在设置了id和num指针的情况下才传出数据
        //这种情况还要先检查是不是in_buf
        if (v_elem->in_num != 1 || v_elem->out_num != 0)
        {
            return 0;
        }
        //这里scatter_data数组就一个，所以直接可以当指针开取数据
        // Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)guest_mem->scatter_data->data;

        int null_flag = 0;
        Direct_Express_Flag_Buf *flag_buf = get_direct_ptr(guest_mem, &null_flag);
        if (null_flag != 0)
        {
            if (flag_buf == NULL)
            {
                return 0;
            }
            *id = flag_buf->id;
            *process_id = flag_buf->process_id;
            *thread_id = flag_buf->thread_id;
            *num = flag_buf->para_num;
            *unique_id = flag_buf->unique_id;
        }
        else
        {
            Direct_Express_Flag_Buf flag_buf_temp;
            guest_write(guest_mem, &flag_buf_temp, 0, sizeof(Direct_Express_Flag_Buf));
            *id = flag_buf_temp.id;
            *process_id = flag_buf_temp.process_id;
            *thread_id = flag_buf_temp.thread_id;
            *num = flag_buf_temp.para_num;
            *unique_id = flag_buf->unique_id;
        }
        // if (!check_fun_id_para_num(*id, *num))
        // {
        //     return 0;
        // }
    }
    return 1;
}

/**
 * @brief 从queue中打包出一个draw调用
 * 
 * @param vq 
 * @return Direct_Express_Draw_Call* 返回为NULL表示queue中没有数据，或者有数据但是数据不对
 */
static Direct_Express_Call *pack_call_from_queue(VirtQueue *vq)
{

    // static int pack_cnt = 0;
    Direct_Express_Queue_Elem *elem;

    Direct_Express_Call *call;

    unsigned long long para_num;
    unsigned long long fun_id;
    unsigned long long thread_id;
    unsigned long long process_id;
    unsigned long long unique_id;

    elem = virtqueue_pop(vq, sizeof(Direct_Express_Queue_Elem));
    while (elem)
    {

        if (unlikely(fill_direct_express_queue_elem(elem, &fun_id, &thread_id, &process_id, &unique_id, &para_num) == 0))
        {
            //第一个elem检查出错，说明不是一个调用，因此将这个elem释放掉，然后继续获取下一个
            VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, elem, 1, next);
            DIRECT_EXPRESS_QUEUE_ELEMS_FREE(elem);
            express_printf("fill error %u %u\n", elem->elem.in_num, elem->elem.out_num);
            return NULL;
        }

        call = alloc_one_call();
        if(call == NULL)
        {
            printf("error! alloc call return NULL!\n");
        }
        call->elem_header = elem;
        call->elem_tail = elem;
        call->vq = vq;

        call->para_num = para_num;
        call->id = fun_id;
        call->thread_id = thread_id;
        call->process_id = process_id;
        call->unique_id = unique_id;
        call->spend_time = 0;
        call->next = NULL;
        // gint64 start_time=g_get_real_time();

        //会有para_num个传入参数，这些elem本应该都是out类型
        for (int i = 0; i < para_num; i++)
        {
            //由于有时候取数据取的过快，安卓那边还没把剩下的一个大数据放进去vring内，这个时候pop会pop一个空的
            //所以要在这里搞个循环，循环的取。但是循环时间又不能过长，以免影响其他数据的传输
            //因此这里使用了一个循环计数机制，50000000基本相当于50ms左右，这个时间不够的话还要继续加
            int cnt_timeout = 0;
            // gint64 t_int = g_get_real_time();

            elem = virtqueue_pop(vq, sizeof(Direct_Express_Queue_Elem));

            while (elem == NULL && cnt_timeout < 10000000)
            {
                // t_int = g_get_real_time();
                // start_time = g_get_real_time();

                elem = virtqueue_pop(vq, sizeof(Direct_Express_Queue_Elem));
                cnt_timeout++;
            }

            if (unlikely(elem == NULL || elem->elem.in_num != 0 || elem->elem.out_num == 0 || fill_direct_express_queue_elem(elem, NULL, NULL, NULL, NULL, NULL) == 0))
            {
                //要么是数据复制有问题，要么是这个elem是个in的类型，破坏了调用结构
                //因此将已经保存的数据抛弃，将这个elem作为第一个elem重新尝试fill，所以是break后continue
                VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, call->elem_header, 1, next);
                DIRECT_EXPRESS_QUEUE_ELEMS_FREE(call->elem_header);
                release_one_call(call);
                call = NULL;
                if (elem == NULL)
                {
                    express_printf(YELLOW("fill para error NULL\n"));
                }
                else
                {
                    express_printf(YELLOW("fill para error %u,%u\n"), elem->elem.in_num, elem->elem.out_num);
                }
                break;
            }
            call->elem_tail->next = elem;
            call->elem_tail = elem;

            // gint64 spend_time = g_get_real_time()-t_int;

            // static gint64 all_spend_time=0;
            // static int now_len=4096;
            // static int now_cnt=1;
            // // printf("now len %u\n",elem->len);
            // if(elem->len%4096==0){
            //     if(now_len!=elem->len){
            //         printf("size %d cnt %d all_time %lld avg time %lld 4k avg time %lld\n",now_len,now_cnt,all_spend_time,all_spend_time/now_cnt,all_spend_time/now_cnt/(now_len/4096));
            //         now_len=elem->len;
            //         now_cnt=1;
            //         all_spend_time=spend_time;
            //     }else{
            //         all_spend_time+=spend_time;
            //         now_cnt+=1;
            //     }
            // }
        }
        // gint64 spend_time = g_get_real_time()-start_time;

        // if(spend_time>0){
        //     Guest_Mem *mem = call->elem_header->para;
        //     // guest_read(mem,&ret,(size_t)((char *)&(((Direct_Express_Flag_Buf *)0)->ret)-(char *)0),8);

        //     guest_read(mem, &spend_time, __builtin_offsetof(Direct_Express_Flag_Buf, map_spend_time), 8);
        //     // guest_write(mem, &t_flag, __builtin_offsetof(Direct_Express_Flag_Buf, id), 8);
        // }

        //不让用goto就得break后continue
        if (call == NULL)
        {
            continue;
        }

        //返回call，elem的空间释放由call里面的回调函数实现
        return call;
    }

    return NULL;
}

/**
 * @brief 创建一个thread_context，并根据这个context新建一个线程
 * 
 * @param context 需要初始化的线程context
 */
Thread_Context *thread_context_create(unsigned long long thread_id, unsigned long long type_id, unsigned long long len, Express_Device_Info *info)
{

    Thread_Context *context = g_malloc(len);
    memset(context, 0, len);

    context->thread_id = thread_id;
    context->type_id = type_id;

    //环形缓冲区初始化
    memset(context->call_buf, 0, (CALL_BUF_SIZE + 2) * sizeof(Direct_Express_Call *));

    context->read_loc = 0;
    context->write_loc = 0;
    context->atomic_event_lock = 0;
    context->init = 0;
    context->thread_run = 1;

    context->context_init = info->context_init;
    context->context_destroy = info->context_destroy;
    context->call_handle = info->call_handle;

    context->direct_express_device = direct_express_device;

//线程缓冲区事件初始化
//qemu_event_init(&(context->data_event), false);
#ifdef _WIN32
    context->data_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else

#endif

    express_printf("ready to create thread\n");
    qemu_thread_create(&context->this_thread, "handle_thread", handle_thread_run, context, QEMU_THREAD_JOINABLE);

    return context;
}

/**
 * @brief 把包装好的call推送到相应的线程
 * 
 * @param call 
 */
void push_to_thread(Direct_Express_Call *call)
{

    //express_printf("push to thread\n");
    uint64_t thread_id = call->thread_id;
    uint64_t process_id = call->process_id;
    uint64_t unique_id = call->unique_id;

    // unsigned long fun_id = GET_FUN_ID(call->id);
    uint64_t device_type_id = GET_DEVICE_ID(call->id);
    uint64_t fun_id = GET_FUN_ID(call->id);

    assert(device_thread_info != NULL);
    Express_Device_Info *device_info = (Express_Device_Info *)g_hash_table_lookup(device_thread_info, GINT_TO_POINTER(device_type_id));
    if (device_info == NULL)
    {
        express_printf("something bad happened %llu %llu\n", device_type_id, fun_id);
        call->callback(call, 0);
        return;
    }
    express_printf("\033[31mpush to %s thread_id %llu %08x fun id%llu %llu %08x unique id %08x\033[0m\n", device_info->name, call->thread_id, call->thread_id, device_type_id, fun_id, call->id, call->unique_id);

    Thread_Context *context = device_info->get_context(device_type_id, thread_id, process_id, unique_id, device_info);

    //找得到相应的设备处理时才把他推送到相应的设备线程
    if (context != NULL)
    {
        if (fun_id == TERMINATE_FUN_ID)
        {
            if (device_info->remove_context)
            {
                device_info->remove_context(device_type_id, thread_id, process_id, unique_id, device_info);
                call->is_end = 1;
                context->init = 0;
            }
        }
        call_push(context, call);
    }
    else
    {
        call->callback(call, 0);
    }
    return;
}

/**
 * @brief 初始化分发线程休眠唤醒的事件
 * 
 */
void init_distribute_event(void)
{
#ifdef _WIN32
    if (recycle_event.win_event == NULL)
    {
        recycle_event.win_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
#else

#endif
}

/**
 * @brief 唤醒分发线程，告知其应该回收了
 * 
 */
void wake_up_distribute(void)
{
#ifdef _WIN32
    if (recycle_event.win_event != NULL)
    {
        SetEvent(recycle_event.win_event);
    }
#else

#endif
}

/**
 * @brief 分发线程等待回收事件，超时时间为1ms，实际可能超时时间在1.8ms左右
 * 
 */
void distribute_wait(void)
{
#ifdef _WIN32
    if (recycle_event.win_event != NULL)
    {
        WaitForSingleObject(recycle_event.win_event, 1);
    }
#else

#endif
}

// #include "express-gpu/express_gpu_render.h"

int push_cnt = 0;
/**
 * @brief 真正用于取出vring上传过来的数据，然后调用相关解码的线程
 * 
 * @param opaque 这个传入的是VirtIODevice
 * @return void* 
 */
void *call_distribute_thread(void *opaque)
{
    VirtIODevice *vdev = opaque;
    Direct_Express *e = DIRECT_EXPRESS(vdev);
    VirtQueue *vq = e->data_queue;

    direct_express_device = vdev;

    Direct_Express_Call *call = NULL;

    memset(call_recycle_queue, 0, sizeof(call_recycle_queue));
    call_recycle_queue_header = 0;
    call_recycle_queue_tail = 0;

    //已经释放，但是还没有通知对方的call数量
    int release_cnt = 0;

    init_distribute_event();

    int pop_cnt = 0;
    int in_handle_num = 0;

    // QemuThread t;

    // int cnt = 0;

    // int sync_flag = 1;
    // unsigned long usleep_time = 1;
    // unsigned long long cnt_time=0;
    // int sleep_cnt=0;
    VirtQueueElement *elem;

    express_printf("wait for pop\n");
    // QemuThread render_thread;
    // qemu_thread_create(&render_thread,"handle_thread",native_window_thread,vdev,QEMU_THREAD_JOINABLE);
    elem = virtqueue_pop(vq, sizeof(VirtQueueElement));
    while (elem == NULL)
    {
        elem = virtqueue_pop(vq, sizeof(VirtQueueElement));
        express_printf("error elem is NULL\n");
    }
    express_printf("get first one ptr %llu %llu %llu\n", elem->out_sg->iov_len, elem->out_num, elem->in_num);

    if (elem->out_sg->iov_len == 4 && elem->out_num == 1 && elem->in_num == 0)
    {
        guest_null_ptr = elem->out_sg->iov_base;

        express_printf("null ptr %llu\n", (unsigned long)guest_null_ptr);

        //计算内存复制速度
        char *temp1 = g_malloc(1024 * 1024 * 24);
        char *temp2 = g_malloc(1024 * 1024 * 24);
        memset(temp1, 0, 1024 * 1024 * 24);
        // memset(temp2,1,1024*1024*24);
        gint64 t_start = g_get_real_time();
        memcpy(temp1, temp2, 1024 * 1024 * 24);
        uint32_t t_spend = (uint32_t)(g_get_real_time() - t_start);
        uint32_t mem_speed = 1024 * 1024 * 24 / t_spend;

        express_printf("mem cpy speed %u\n", mem_speed);

        *(uint32_t *)guest_null_ptr = mem_speed;

        g_free(temp1);
        g_free(temp2);
    }
    else
    {
        express_printf("error! null ptr cannot be init!\n");
    }
    virtqueue_push(vq, elem, 1);
    // int release_cnt_debug=0;
    // int pop_cnt_debug=0;

    int64_t spend_time_all = 0;
    int64_t call_num = 0;
    while (e->thread_run)
    {

        int has_handle_flag = 0;
        if ((call = pack_call_from_queue(vq)) != NULL)
        {
            //从queue中打包调用，假如打包失败的话，失败的部分也还是会还给guest
            pop_cnt += 1;
            in_handle_num += 1;
            // atomic_add(&push_cnt, 1);
            // sync_flag = call->fun_id;
            //express_printf("virtio has data\n");
            express_printf("virtio has data push\n");
            //draw_call的其他部分都已经初始化过了
            call->vdev = vdev;
            call->callback = push_free_callback;
            call->is_end = 0;
            push_to_thread(call);
            has_handle_flag = 1;
            // pop_cnt_debug++;
            // printf("pop %d\n",pop_cnt_debug);

            // draw_call_printf(draw_call);
            // Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)draw_call->elem_header->para;
            // flag_buf->flag=1;
            // release_call(draw_call);
            // virtio_notify(VIRTIO_DEVICE(vdev), vq);
            // push_free_callback(draw_call);
        }
        //这里之前是else if，高负载下导致大量call被堆积到这里，一直没法回收，影响了性能，因此这里进行修改
        //改为一次取数据对应着一次回收数据
        if (call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] != NULL)
        {
            //将回收的部分和分发的部分放到一起是为了减小延迟

            //出队直接把队头后面的数据交换出来，队头那里没有放数据，数据都是放在后面一个了
            //这里没有使用无锁的方式是因为就这一个地方会出队，所以不存在并发问题
            Direct_Express_Call *out_call = call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)];
            call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] = NULL;
            // Direct_Express_Call *out_call=atomic_xchg(&call_recycle_queue[(call_recycle_queue_header+1)%(CALL_BUF_SIZE+2)],NULL);
            call_recycle_queue_header = (call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2);
            // express_printf("recycle one\n");
            //express_printf("recycle one %s\n",(char *)out_call->elem_tail->para);
            in_handle_num -= 1;
            // gint64 start_time =g_get_real_time();
            // uint64_t thread_id=out_call->thread_id;
            release_call(out_call);
            // spend_time_all += g_get_real_time()-start_time;
            // call_num+=1;
            // express_printf("call release time %lld %lld %lld %llu\n",spend_time_all/call_num,spend_time_all,call_num,thread_id);

            // release_cnt_debug+=1;
            // printf("%d %d %d\n",release_cnt_debug,call_recycle_queue_header,call_recycle_queue_tail);
            release_cnt += 1;
            has_handle_flag = 1;
        }

        //前面两个改为if后，这里也改为判断前面两个if有没有进入
        if (!has_handle_flag)
        // else
        {
            //休眠前注入中断，通知对方，防止部分call的延迟过大
            if (release_cnt != 0)
            {

                release_cnt = 0;
                express_printf("notify guest\n");
                virtio_notify(VIRTIO_DEVICE(vdev), vq);
            }

            pop_cnt = 0;

            //休眠采用可以被其他线程打断的休眠，主要是被处理线程打断，打断的目的也是为了减小延迟
            distribute_wait();

            // gint64 s2=g_get_real_time();
            // if(in_handle_num!=0){
            //     cnt_time+=s2-s1;
            //     sleep_cnt+=1;
            //     if(sleep_cnt%100==0){
            //         express_printf("sleep cnt %d time %lld avg %lld\n",sleep_cnt,cnt_time,cnt_time/sleep_cnt);
            //     }
            // }
        }

        //下面这个不需要，因为高负载下，并不依赖与中断注入来回收数据
        //高负载下依赖flag标志来回收数据
        if (release_cnt >= 128)
        {
            //express_printf("recycle 128\n");
            //平均一个call占用的空间为2左右，所以queue里理论上最大有1024/2=512个call，保留一定量的余量空间
            //剩下的空间里留一部分给处理过程消耗，因此假设留给释放的call大概在128左右
            //所以这时需要赶紧释放空间，防止queue满了
            // express_printf("%s notify 128 with %d\n",get_now_time(),release_cnt);
            release_cnt = 0;
            virtio_notify(VIRTIO_DEVICE(vdev), vq);
        }
    }

    return NULL;
}

// /**
//  * @brief 回收已经执行了的指令的线程
//  *
//  * @param opaque 这个实际上是VirtIODevice，能从中找到Direct_Express和VirtQueue
//  * @return void*
//  */
// void *call_recycle_thread(void *opaque)
// {
//     //队列里始终有一个队列头，以防止队列为空时，需要麻烦的设置NULL
//     VirtIODevice *vdev = opaque;
//     Direct_Express *g = DIRECT_EXPRESS(vdev);
//     VirtQueue *vq = g->data_queue;
//     //null_header是默认的链表头，这个头是不会出队的，出队是出这个头的next
//     Direct_Express_Draw_Call *null_header = g_malloc(sizeof(Direct_Express_Draw_Call));
//     null_header->next = NULL;
//     call_recycle_header = null_header;
//     call_recycle_tail = null_header;
//     int release_cnt=0;
//     while (g->thread_run)
//     {
//         if (call_recycle_header->next != NULL)
//         {
//             Direct_Express_Draw_Call *out_call = call_recycle_header->next;
//             call_recycle_header->next = out_call->next;
//             //更新tail，假如tail就是刚才出队的节点，则tail=header
//             atomic_cmpxchg(&call_recycle_tail, out_call, call_recycle_header);
//             express_printf("recycle one %s\n",(char *)out_call->elem_tail->para);
//             release_call(out_call);
//             release_cnt+=1;
//         }
//         else
//         {
//             //休眠前注入中断，通知对方，防止部分call的延迟过大
//             if(release_cnt!=0){
//                 express_printf("notify before sleep\n");
//                 release_cnt=0;
//                 virtio_notify(VIRTIO_DEVICE(vdev), vq);
//             }
//             /**
//              * @todo 这个时间需要测试
//              *
//              */
//             g_usleep(1);
//         }
//         if(release_cnt>=128){
//             express_printf("recycle 128\n");
//             //平均一个call占用的空间为3左右，所以queue里理论上最大有1024/3=341个call，保留一定量的余量空间
//             //剩下的空间里留一部分给处理过程消耗，因此假设留给释放的call大概在128左右
//             //所以这时需要赶紧释放空间，防止queue满了
//             release_cnt=0;
//             virtio_notify(VIRTIO_DEVICE(vdev), vq);
//         }
//     }
//     //todo:释放链表上的空间
//     g_free(null_header);
//     return NULL;
// }

/**
 * @brief 释放Draw_Call这个结构体本身占用的空间，并将其占用的vring空间部分返还给guest
 * 
 * @param out_call 需要释放的Direct_Express_Call
 */
static void release_call(Direct_Express_Call *out_call)
{

    VirtQueue *vq = out_call->vq;
    VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, out_call->elem_header, 1, next);
    DIRECT_EXPRESS_QUEUE_ELEMS_FREE(out_call->elem_header);

    release_one_call(out_call);
    return;
}

// /**
//  * @brief 获取一些这个call相关的线程、进程、vring的状态等的信息
//  *
//  * @param call 需要寻找的call
//  * @param fun_id 返回的函数调用id
//  * @param process_id 返回的进程id
//  * @param thread_id 返回的线程id
//  * @param num_free 返回的vring当前的可用空间
//  */
// void get_process_mess(Direct_Express_Call *call,int *fun_id,int *process_id,int *thread_id,int *num_free){
//     Direct_Express_Queue_Elem *header=call->elem_header;

//     Direct_Express_Flag_Buf *flag_buf=get_direct_ptr(header->para);
//     if(flag_buf!=NULL){
//         *fun_id=flag_buf->id;
//         *process_id=flag_buf->process_id;
//         *thread_id=flag_buf->thread_id;
//         *num_free=flag_buf->num_free;
//     }else{
//         Direct_Express_Flag_Buf flag_buf_temp;
//         guest_write(header->para,&flag_buf_temp,0,sizeof(Direct_Express_Flag_Buf));
//         *fun_id=flag_buf_temp.id;
//         *process_id=flag_buf_temp.process_id;
//         *thread_id=flag_buf_temp.thread_id;
//         *num_free=flag_buf_temp.num_free;
//     }
//     // Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)header->para;

//     return;
// }

/**
 * @brief 将call中的第loc个（传入）参数标记为返回值，便于在数据被切分后再复制回去
 * 
 * @param call 需要标记的调用call
 * @param loc 返回值的位置，从1开始
 */
void mark_call_return(Direct_Express_Call *call, int loc)
{
    Direct_Express_Queue_Elem *header = call->elem_header;
    Direct_Express_Queue_Elem *now_elem = header->next;
    for (int i = 0; i < loc - 1; i++)
    {
        if (now_elem == NULL)
        {
            return;
        }
        now_elem = now_elem->next;
    }
    if (now_elem != NULL)
    {

        now_elem->type |= RET_PARA;
    }

    return;
}

// void set_call_return_val(Direct_Express_Call *call, unsigned char *ret_ptr, size_t len)
// {

//     // Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)call->elem_header->para;

//     Guest_Mem *mem = call->elem_header->para;
//     // guest_read(mem,&ret,(size_t)((char *)&(((Direct_Express_Flag_Buf *)0)->ret)-(char *)0),8);

//     // unsigned long long t=0;
//     // guest_write(mem,&t,__builtin_offsetof(Direct_Express_Flag_Buf,ret),8);

//     express_printf("return value %llu\n",*(unsigned long long*)ret_ptr);
//     // Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)mem->scatter_data[0].data;
//     guest_read(mem, ret_ptr, __builtin_offsetof(Direct_Express_Flag_Buf, ret), len);
//     // flag_buf->ret=*(uint64_t *)ret_ptr;

//     // express_printf("set return value %llu %llu -- %llu\n",ret_ptr);

//     return;
// }

/**
 * @brief 从call中获得其保存的参数，并返回参数数目，假如返回的是0，则说明获取失败
 * 
 * @param call 用于提取参数的call
 * @param call_para 传入的用于设置参数信息的call_para数组指针，其中每一个元素都有data和len两个值，其中，data是Guest_Mem指针
 * @param para_num 传入的参数数目，会验证是否和call中的数目是否过大
 * @return int 
 */
int get_para_from_call(Direct_Express_Call *call, Call_Para *call_para, unsigned long max_para_num)
{

    Direct_Express_Queue_Elem *header = call->elem_header;
    Direct_Express_Queue_Elem *now_elem = header->next;
    if (max_para_num < call->para_num)
    {
        return 0;
    }
    call->spend_time = g_get_real_time();

    // if (now_elem == NULL)
    // {
    //     //调用没有传入参数
    //     call_para[0].data = NULL;
    //     call_para[0].data_len = 0;
    //     return 0;
    // }
    // Call_Para *ret_para=g_malloc(*para_num*sizeof(Call_Para));
    // memset(ret_para,0,*para_num*sizeof(Call_Para));
    for (int i = 0; i < call->para_num; i++)
    {
        if (now_elem == NULL)
        {
            // g_free(ret_para);
            return 0;
        }
        // Guest_Mem *mem=now_elem->para;
        call_para[i].data = now_elem->para;

        call_para[i].data_len = now_elem->len;
        now_elem = now_elem->next;
    }
    return call->para_num;
}

/**
 * @brief 在处理线程使用完数据后的回调函数，将调用完成的call送给回收线程，使用无锁队列实现入队，同时，在传回之前，会将相关数据复制回去，同时设置好guest会读取的flag
 * 
 * @param call 需要回收的call
 * @param notify 指示是否需要通知回收线程快速回收
 */
void push_free_callback(Direct_Express_Call *call, int notify)
{

    // atomic_sub(&push_cnt, 1);

    // Direct_Express_Queue_Elem *para_elem;
    // for (para_elem = call->elem_header; para_elem != NULL; para_elem = para_elem->next)
    // {
    //     //RET_PARA由调用者修改
    //     if (((para_elem->type) & RET_PARA) != 0 && ((para_elem->type) & COPY_PARA) != 0 && para_elem->para != NULL)
    //     {
    //         //para来自于复制操作，并且是将要返回的数据，就要将数据写回去
    //         //express_printf("need copy back\n");
    //         int loc = 0;
    //         for (int i = 0; i < para_elem->elem.in_num; i++)
    //         {
    //             memcpy(para_elem->elem.in_sg->iov_base, para_elem->para + loc, para_elem->elem.in_sg->iov_len);
    //             loc += para_elem->elem.in_sg->iov_len;
    //         }
    //     }
    // }

    //将这个flag设置为1，方便guest检查到相应内存区域内的数据变为1而快速返回，减小延迟
    // Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)call->elem_header->para;
    // flag_buf->flag = 1;

    if (call->spend_time != 0)
    {
        call->spend_time = g_get_real_time() - call->spend_time;
    }

    //设置guest端的flag标志，防止中断丢失
    Guest_Mem *mem = call->elem_header->para;
    // guest_read(mem,&ret,(size_t)((char *)&(((Direct_Express_Flag_Buf *)0)->ret)-(char *)0),8);
    unsigned long long t_flag = 1;
    guest_read(mem, &t_flag, __builtin_offsetof(Direct_Express_Flag_Buf, flag), 8);
    guest_read(mem, &(call->spend_time), __builtin_offsetof(Direct_Express_Flag_Buf, mem_spend_time), 8);

    // guest_write(mem, &t_flag, __builtin_offsetof(Direct_Express_Flag_Buf, id), 8);
    express_printf("write flag id %llu %llu\n", t_flag, call->thread_id);

    // printf("push cnt %d\n",push_cnt);

    //无锁入队
    int origin_tail = call_recycle_queue_tail;
    int t = origin_tail;
    do
    {
        while (call_recycle_queue[(t + 1) % (CALL_BUF_SIZE + 2)] != NULL)
        {
            t = (t + 1) % (CALL_BUF_SIZE + 2);
        }
    } while (atomic_cmpxchg(&(call_recycle_queue[(t + 1) % (CALL_BUF_SIZE + 2)]), NULL, call) != NULL);

    atomic_cmpxchg(&call_recycle_queue_tail, origin_tail, (t + 1) % (CALL_BUF_SIZE + 2));

    // Direct_Express_Call *tail=NULL;
    // Direct_Express_Call *next = NULL;

    // while(1){
    //     tail=call_recycle_tail;
    //     next=call_recycle_tail->next;

    //     if(tail!=call_recycle_tail){
    //         continue;
    //     }

    //     if(next!=NULL){
    //         atomic_cmpxchg(&call_recycle_tail, tail, next);
    //         continue;
    //     }

    //     if(atomic_cmpxchg(&(call_recycle_tail->next), next, call)==next){
    //         break;
    //     }

    // }
    // atomic_cmpxchg(&(call_recycle_tail), tail, call);

    // Direct_Express_Call *origin_tail=call_recycle_tail;
    // Direct_Express_Call *t = origin_tail;

    // do
    // {
    //     while (t->next != NULL)
    //     {
    //         t = t->next;
    //     }
    // } while (atomic_cmpxchg(&(t->next), NULL, call) != NULL);

    // atomic_cmpxchg(&call_recycle_tail, origin_tail, call);

    if (notify)
    {
        //入队后要尝试中断掉分发回收线程的休眠（轮询过程中的休眠）
        wake_up_distribute();
    }
}

/**
 * @brief 所有的express设备共用的init函数，这个函数会在main函数前调用
 * 
 * @param info 
 */
void express_device_init_common(Express_Device_Info *info)
{
    if (device_thread_info == NULL)
    {
        device_thread_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
    // printf("type id %llu\n", info->type_id);
    g_hash_table_insert(device_thread_info, GINT_TO_POINTER(info->type_id), (gpointer)info);
}