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
#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_handle_thread.h"
#include "direct-express/express_log.h"
// #include <EGL/eglplatform.h>

// #define express_printf null_printf
#include <winsock2.h>
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


typedef struct VRing_Used
{
    unsigned int num;
    unsigned int num_default;
    unsigned int align;
    hwaddr desc;
    hwaddr avail;
    hwaddr used;
} VRing_Used;


static GHashTable *device_thread_info=NULL;


static VirtIODevice *direct_express_device;


//用于回收的队列缓冲区的头和尾指针，方便实现无锁的入队出队
// static Direct_Express_Call *call_recycle_header = NULL;
// static Direct_Express_Call *call_recycle_tail = NULL;


static volatile Direct_Express_Call *call_recycle_queue[(CALL_BUF_SIZE+2)];
static int call_recycle_queue_header;
static volatile int call_recycle_queue_tail;


static void release_call(Direct_Express_Call *out_call);
void push_free_callback(Direct_Express_Call *call);


typedef struct {
    #ifdef _WIN32
        HANDLE win_event;   
    #else
           
    #endif
} RECYCLE_EVENT;

RECYCLE_EVENT recycle_event;


/**
 * @brief 根据iov获得一个连续内存块，内存块内数据为iov数组的复制结果，这个函数一般用于有多个iov时
 * 
 * @param sg iov存放的位置
 * @param num iov的数目
 * @param all_cnt 需要回写的变量，表示到底复制了多少数据
 * @return void* 返回内存块的指针
 */
static void *alloc_buf_from_iov(struct iovec *sg, unsigned int num, size_t *all_cnt)
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

/**
 * @brief 检查调用的fun_id与参数数目的对应关系是否满足，防止瞎传
 * 
 * @param id 调用的函数id
 * @param num 参数的数目
 * @return int 返回检查结果是否正确，1表示正确，0表示错误
 */
static int check_fun_id_para_num(int id, int num)
{
    // express_printf("mygpu get invoke %d %d\n", id, num);
    return 1;
    //todo
}

/**
 * @brief 将Direct_Express_Queue_Elem内的数据填充完毕，也就是初始化Direct_Express_Queue_Elem中除了
 * VirtQueueElement的其他部分，例如para指针，type类型等
 * 
 * @param elem 需要填充的elem数据
 * @param id 需要回传的函数调用id（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @param num 需要回传的参数数目（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @return int 返回填充是否完成，1表示完成，0表示失败
 */
static int fill_direct_express_queue_elem(Direct_Express_Queue_Elem *elem, int *id, int *thread_id, size_t *num)
{
    VirtQueueElement *v_elem = &elem->elem;
    if ((v_elem->out_num != 0 && v_elem->in_num != 0) || (v_elem->out_num == 0 && v_elem->in_num == 0))
    {
        return 0;
    }
    elem->para = NULL;
    elem->next = NULL;
    elem->type = 0;
    //sg数目多余1个就要单独复制出来
    if (v_elem->out_num > 1)
    {
        elem->para = alloc_buf_from_iov(v_elem->out_sg, v_elem->out_num, &elem->len);
        elem->type |= COPY_PARA;
    }
    else if (v_elem->out_num == 1)
    {
        //只有一个sg的情况下直接传递指针
        elem->para = v_elem->out_sg->iov_base;
        elem->len = v_elem->out_sg->iov_len;
        elem->type |= DIRECT_PARA;
    }

    if (v_elem->in_num > 1)
    {
        elem->para = alloc_buf_from_iov(v_elem->in_sg, v_elem->in_num, &elem->len);
        elem->type |= COPY_PARA;
    }
    else if (v_elem->in_num == 1)
    {
        elem->para = v_elem->in_sg->iov_base;
        elem->len = v_elem->in_sg->iov_len;
        elem->type |= DIRECT_PARA;
    }
    if (elem->para == NULL)
    {
        return 0;
    }
    if (id != NULL && num != NULL && thread_id != NULL)
    {
        //在设置了id和num指针的情况下才传出数据
        //这种情况还要先检查是不是in_buf
        if (v_elem->in_num != 1 || v_elem->out_num != 0)
        {
            return 0;
        }
        Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)elem->para;
        *id = flag_buf->id;
        *num = flag_buf->para_num;
        *thread_id = flag_buf->thread_id;
        if (!check_fun_id_para_num(*id, *num))
        {
            return 0;
        }
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

    Direct_Express_Queue_Elem *elem;

    Direct_Express_Call *call;

    size_t para_num;
    int draw_id;
    int thread_id;

    volatile hwaddr *ving_avail= &(((VRing_Used *)vq)->avail);
    if(*ving_avail==0){
        return NULL;
    }


    elem = virtqueue_pop(vq, sizeof(Direct_Express_Queue_Elem));
    while (elem)
    {

        // //debug test
        // {
        //     fill_mygpu_queue_elem(elem, &draw_id, &para_num);
        //     // express_printf("fill ok %d\n",elem==&elem->elem);
        //     // virtqueue_push(vq, elem, 1);
        //     // for (MYGPU_Queue_Elem *a = (MYGPU_Queue_Elem *)(elem); a != NULL; a = a->next)
        //     // {
        //     //     virtqueue_push(vq, a, 1);
        //     // }
        //     VIRTIO_ELEM_PUSH_ALL(vq, MYGPU_Queue_Elem, elem, 1, next);
        //     express_printf("push\n");
        //     MYGPU_QUEUE_ELEMS_FREE(elem);
        //     // virtio_notify(VIRTIO_DEVICE(vdev), vq);
        //     express_printf("pop next\n");
        //     // return;
        //     elem = virtqueue_pop(vq, sizeof(MYGPU_Queue_Elem));
        //     fill_mygpu_queue_elem(elem, NULL, NULL);
        //     express_printf("mygpu get user data %s\n", elem->para);
        //     VIRTIO_ELEM_PUSH_ALL(vq, MYGPU_Queue_Elem, elem, 1, next);
        //     MYGPU_QUEUE_ELEMS_FREE(elem);
        //     virtio_notify(VIRTIO_DEVICE(vdev), vq);
        //     express_printf("notify\n");
        //     elem = virtqueue_pop(vq, sizeof(MYGPU_Queue_Elem));
        //     continue;
        // }

        if (unlikely(fill_direct_express_queue_elem(elem, &draw_id, &thread_id, &para_num) == 0))
        {
            //第一个elem检查出错，说明不是一个调用，因此将这个elem释放掉，然后继续获取下一个
            VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, elem, 1, next);
            DIRECT_EXPRESS_QUEUE_ELEMS_FREE(elem);

            return NULL;
        }

        call = g_malloc(sizeof(Direct_Express_Call));
        call->elem_header = elem;
        call->elem_tail = elem;
        call->vq = vq;


        call->para_num = para_num;
        call->fun_id = draw_id;
        call->thread_id = thread_id;

        call->next = NULL;

        //会有para_num个传入参数，这些elem本应该都是out类型
        for (int i = 0; i < para_num; i++)
        {
            elem = virtqueue_pop(vq, sizeof(Direct_Express_Queue_Elem));
            if (unlikely(elem == NULL || elem->elem.in_num != 0 || elem->elem.out_num == 0 || fill_direct_express_queue_elem(elem, NULL, NULL, NULL) == 0))
            {
                //要么是数据复制有问题，要么是这个elem是个in的类型，破坏了调用结构
                //因此将已经保存的数据抛弃，将这个elem作为第一个elem重新尝试fill，所以是break后continue
                VIRTIO_ELEM_PUSH_ALL(vq, Direct_Express_Queue_Elem, call->elem_header, 1, next);
                DIRECT_EXPRESS_QUEUE_ELEMS_FREE(call->elem_header);
                g_free(call);
                call = NULL;
                break;
            }
            call->elem_tail->next = elem;
            call->elem_tail = elem;
        }
        //不让用goto就得break后continue
        if (call == NULL)
        {
            continue;
        }

        //返回call，elem的空间释放由call里面的回调函数实现，call结构体由渲染线程释放
        return call;
    }

    return NULL;
}

// /**
//  * @brief 临时的debug调用函数，后续不需要
//  * 
//  * @param call 
//  */
// static void draw_invoke(Direct_Express_Draw_Call *call)
// {
//     express_printf("draw invoke\n");
//     int cnt1 = 0;
//     for (Direct_Express_Queue_Elem *i = call->elem_header; i != NULL; i = i->next)
//     {
//         cnt1++;
//         if (cnt1 == 1)
//         {
//             Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)i->para;
//             flag_buf->ret = 2;
//             express_printf("call para %d call fun_id %d thread id %d\n", flag_buf->para_num, flag_buf->id, call->thread_id);
//             continue;
//         }
//         char *temp = (char *)i->para;
//         if (call->fun_id == 0 && cnt1 == 2)
//         {
//             express_printf("call message express_printf: %s\n", temp);
//         }
//         else if (call->fun_id == 1 || call->fun_id == 2 || call->fun_id == 3)
//         {
//             if (cnt1 == 2)
//             {
//                 int *t = (int *)temp;
//                 express_printf("call message int %d %d %d\n", t[0], t[1], t[2]);
//             }
//         }
//     }
//     if (call->fun_id == 4)
//     {
//         char *t = call->elem_tail->para;
//         for (int i = 0; i < call->elem_tail->len - 1; i++)
//         {
//             t[i] = 'c';
//         }
//     }
//     // call->callback(call);
//     g_free(call);
// }

/**
 * @brief 创建一个thread_context，后续需要拿着这个context新建线程
 * 
 * @param context 需要初始化的线程context
 */
Thread_Context *thread_context_create(int thread_id,int type_id,unsigned int len,Express_Device_Info *info)
{

    Thread_Context *context = g_malloc(len);
    memset(context,0,len);

    context->thread_id = thread_id;
    context->type_id = type_id;

    //环形缓冲区初始化
    memset(context->call_buf, 0, (CALL_BUF_SIZE+2) * sizeof(Direct_Express_Call *));

    context->read_loc = 0;
    context->write_loc = 0;
    context->init=0;
    context->thread_run=1;

    context->context_init=info->context_init;
    context->call_handle=info->call_handle;

    context->direct_express_device=direct_express_device;


    //线程缓冲区事件初始化
    qemu_event_init(&(context->data_event), false);

    if(context->context_init!=NULL){
        context->context_init(context);
    }
    qemu_thread_create(&context->this_thread,"handle_thread",handle_thread_run,context,QEMU_THREAD_JOINABLE);

    return context;
}


/**
 * @brief 把包装好的call推送到相应的线程
 * 
 * @param call 
 */
void push_to_thread(Direct_Express_Call *call){
    
    //express_printf("push to thread\n");
    int thread_id=call->thread_id;
    int fun_id=GET_FUN_ID(call->fun_id);
    int device_type_id=GET_DEVICE_ID(call->fun_id);

    assert(device_thread_info!=NULL);
    Express_Device_Info *device_info=(Express_Device_Info *)g_hash_table_lookup(device_thread_info,GINT_TO_POINTER(device_type_id));
    if(device_info==NULL){
        return;
    }


    Thread_Context *context=device_info->get_context(device_type_id,thread_id,device_info);

    if(context!=NULL){
        call_push(context,call);
    }
    return;
}


/**
 * @brief 初始化分发线程休眠唤醒的事件
 * 
 */
void init_distribute_event(){
    #ifdef _WIN32
        if(recycle_event.win_event==NULL){
            recycle_event.win_event=CreateEvent(NULL, FALSE, FALSE, NULL);
        }
    #else
           
    #endif
}


/**
 * @brief 唤醒分发线程，告知其应该回收了
 * 
 */
void wake_up_distribute(){
    #ifdef _WIN32
        if(recycle_event.win_event!=NULL){
            SetEvent(recycle_event.win_event);
        }
    #else
        
    #endif
}


/**
 * @brief 分发线程等待回收事件，超时时间为1ms，实际可能超时时间在1.8ms左右
 * 
 */
void distribute_wait(){
    #ifdef _WIN32
        if(recycle_event.win_event!=NULL){
            WaitForSingleObject(recycle_event.win_event,1);
        }
    #else
        
    #endif
}


void *time_out_thread(void *t){
    
    SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    fd_set dummy;
    while(1){
        struct timeval wait_time;
        
        wait_time.tv_sec = 0;                     //秒
        wait_time.tv_usec = 500;       //微妙
        FD_ZERO(&dummy);
        FD_SET(s, &dummy);
        int ret = select(0, NULL, NULL, &dummy, &wait_time);
        wake_up_distribute();
    }
}

int push_cnt=0;
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


    direct_express_device=vdev;


    Direct_Express_Call *call = NULL;


    //null_header是默认的链表头，这个头是不会出队的，出队是出这个头的next
    // Direct_Express_Call *null_header = g_malloc(sizeof(Direct_Express_Call));
    // null_header->next = NULL;
    // call_recycle_header = null_header;
    // call_recycle_tail = null_header;

    memset(call_recycle_queue,0,sizeof(call_recycle_queue));
    call_recycle_queue_header=0;
    call_recycle_queue_tail=0;


    //已经释放，但是还没有通知对方的call数量
    int release_cnt=0;

    init_distribute_event();


    int pop_cnt=0;
    int in_handle_num=0;
    
    QemuThread t;

    // for(int i=0;i<10;i++){
    //     qemu_thread_create(&t,"timeout",time_out_thread,NULL,QEMU_THREAD_JOINABLE);
    // }
    
    int cnt=0;

    // unsigned long usleep_time = 1;
    // unsigned long long cnt_time=0;
    // int sleep_cnt=0;
    while (e->thread_run)
    {
        if ((call = pack_call_from_queue(vq)) != NULL)
        {
            //从queue中打包调用，假如打包失败的话，失败的部分也还是会还给guest
            pop_cnt+=1;
            in_handle_num+=1;
            atomic_add(&push_cnt,1);
            
            //express_printf("virtio has data\n");
            //draw_call的其他部分都已经初始化过了
            call->vdev = vdev;
            call->callback = push_free_callback;
            push_to_thread(call);
            // draw_call_printf(draw_call);
            // Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)draw_call->elem_header->para;
            // flag_buf->flag=1;
            // release_call(draw_call);
            // virtio_notify(VIRTIO_DEVICE(vdev), vq);
            // push_free_callback(draw_call);

        }
        else if (call_recycle_queue[(call_recycle_queue_header+1)%(CALL_BUF_SIZE+2)] != NULL)
        {
            //将回收的部分和分发的部分放到一起是为了减小延迟

            Direct_Express_Call *out_call=atomic_xchg(&call_recycle_queue[(call_recycle_queue_header+1)%(CALL_BUF_SIZE+2)],NULL);
            call_recycle_queue_header=(call_recycle_queue_header+1)%(CALL_BUF_SIZE+2);

            // Direct_Express_Call *out_call = call_recycle_header->next;
            // call_recycle_header->next = out_call->next;
            // //更新tail，假如tail就是刚才出队的节点，则tail=header
            // atomic_cmpxchg(&call_recycle_tail, out_call, call_recycle_header);

            //express_printf("recycle one %s\n",(char *)out_call->elem_tail->para);
            in_handle_num-=1;
            release_call(out_call);
            release_cnt+=1;
        }
        else
        {

              //休眠前注入中断，通知对方，防止部分call的延迟过大
            if(release_cnt!=0){
                //express_printf("notify before sleep\n");
                release_cnt=0;
                virtio_notify(VIRTIO_DEVICE(vdev), vq);
            }

            // if(pop_cnt!=0){
            //     printf("pop %d call before sleep %lu\n",pop_cnt,usleep_time);
            // }
            pop_cnt=0;
            // gint64 s1=g_get_real_time();
            // struct timeval wait_time;
            // wait_time.tv_sec = 0;                     //秒
            // wait_time.tv_usec = 500;       //微妙
            // FD_ZERO(&dummy);
            //  FD_SET(s, &dummy);
            // int ret = select(0, NULL, NULL, &dummy, &wait_time);

            //休眠采用可以被其他线程打断的休眠，主要是被处理线程打断，打断的目的也是为了减小延迟
            distribute_wait();

            // gint64 s2=g_get_real_time();
            // if(in_handle_num!=0){
            //     cnt_time+=s2-s1;
            //     sleep_cnt+=1;
            //     if(sleep_cnt%100==0){
            //         printf("sleep cnt %d time %lld avg %lld\n",sleep_cnt,cnt_time,cnt_time/sleep_cnt);
            //     }
            // }
            cnt++;
            if(cnt%100==0){
                printf("sleep 100 *1ms %d %d %d\n",in_handle_num,release_cnt,push_cnt);
            }   
            
        }
        if(release_cnt>=128){
            //express_printf("recycle 128\n");
            //平均一个call占用的空间为3左右，所以queue里理论上最大有1024/3=341个call，保留一定量的余量空间
            //剩下的空间里留一部分给处理过程消耗，因此假设留给释放的call大概在128左右
            //所以这时需要赶紧释放空间，防止queue满了
            release_cnt=0;
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
    
    g_free(out_call);
    return;
}

/**
 * @brief 在处理线程使用完数据后的回调函数，将调用完成的call送给回收线程，使用无锁队列实现入队，同时，在传回之前，会将相关数据复制回去，同时设置好guest会读取的flag
 * 
 * @param call 
 */
void push_free_callback(Direct_Express_Call *call)
{
    
    atomic_sub(&push_cnt,1);
    //复制回去相关数据
    //todo 使用其他内存映射完成复制
    Direct_Express_Queue_Elem *para_elem;
    for (para_elem = call->elem_header; para_elem != NULL; para_elem = para_elem->next)
    {
        //RET_PARA由调用者修改
        if (((para_elem->type) & RET_PARA) != 0 && ((para_elem->type) & COPY_PARA) != 0 && para_elem->para != NULL)
        {
            //para来自于复制操作，并且是将要返回的数据，就要将数据写回去
            //express_printf("need copy back\n");
            int loc = 0;
            for (int i = 0; i < para_elem->elem.in_num; i++)
            {
                memcpy(para_elem->elem.in_sg->iov_base, para_elem->para + loc, para_elem->elem.in_sg->iov_len);
                loc += para_elem->elem.in_sg->iov_len;
            }
        }
    }

    //将这个flag设置为1，方便guest检查到相应内存区域内的数据变为1而快速返回，减小延迟
    Direct_Express_Flag_Buf *flag_buf = (Direct_Express_Flag_Buf *)call->elem_header->para;
    flag_buf->flag=1;

    //无锁入队


    // int tail=0;
    // int next=0;
    // while(1){
    //     tail=call_recycle_queue_tail;
    //     next=(call_recycle_queue_tail+1)%(CALL_BUF_SIZE+2);
    //     if(tail!=call_recycle_queue_tail){
    //         continue;
    //     }
    //     if(call_recycle_queue[next]!=NULL){
    //         atomic_cmpxchg(&call_recycle_queue_tail, tail, next);
    //         continue;
    //     }
    //     if(atomic_cmpxchg(&(call_recycle_queue[(call_recycle_queue_tail+1)%(CALL_BUF_SIZE+2)]), call_recycle_queue[next], call)==NULL){
    //         break;
    //     }
    // }
    // atomic_cmpxchg(&(call_recycle_queue_tail), tail, next);


    int origin_tail=call_recycle_queue_tail;
    int t=origin_tail;
    do{
        while(call_recycle_queue[(t+1)%(CALL_BUF_SIZE+2)]!=NULL){
            t=(t+1)%(CALL_BUF_SIZE+2);
        }
    }while(atomic_cmpxchg(&(call_recycle_queue[(t+1)%(CALL_BUF_SIZE+2)]), NULL, call) != NULL);

    atomic_cmpxchg(&call_recycle_queue_tail, origin_tail, (t+1)%(CALL_BUF_SIZE+2));

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

    //入队后要尝试中断掉分发回收线程的休眠（轮询过程中的休眠）
    wake_up_distribute();
}




void express_device_init_common(Express_Device_Info *info){
    if(device_thread_info==NULL){
         device_thread_info=g_hash_table_new(g_direct_hash,g_direct_equal);
    }

    g_hash_table_insert(device_thread_info,GINT_TO_POINTER(info->type_id),(gconstpointer)info);
}