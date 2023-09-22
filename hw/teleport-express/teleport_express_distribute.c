/**
 * @file teleport_express_distribute.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 使用轮询实现取call，分发给其他draw线程，同时负责回收数据
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */
// #define STD_DEBUG_LOG

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/teleport-express/express_handle_thread.h"

#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_device_ctrl.h"
#include "hw/teleport-express/express_event.h"

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

static VirtIODevice *teleport_express_device;

//用于回收call的队列，实现了无锁的入队，这里将它的大小设置为CALL_BUF_SIZE+2是为了保证队列不会爆，大小一定满足要求
//这里设置volatile是为了保证其在不同线程间同步不会受到缓存的影响
static Teleport_Express_Call *call_recycle_queue[(CALL_BUF_SIZE + 2)];
static volatile int call_recycle_queue_header;
static volatile int call_recycle_queue_tail;

int atomic_distribute_thread_running = 0;

static void push_free_callback(Teleport_Express_Call *call, int notify);
void push_to_thread(Teleport_Express_Call *call);
void init_distribute_event(void);
void distribute_wait(void);

//用于通知回收的事件，这里对于平台兼容性的部分尚未完成
typedef struct
{
#ifdef _WIN32
    HANDLE win_event;
#else
    void *win_event;
#endif
} RECYCLE_EVENT;

RECYCLE_EVENT recycle_event;

// /**
//  * @brief 释放Draw_Call这个结构体本身占用的空间，并将其占用的vring空间部分返还给guest
//  *
//  * @param out_call 需要释放的Teleport_Express_Call
//  */
// static void release_call(Teleport_Express_Call *out_call)
// {

//     VirtQueue *vq = out_call->vq;
//     VIRTIO_ELEM_PUSH_ALL(vq, Teleport_Express_Queue_Elem, out_call->elem_header, 1, next);
//     TELEPORT_EXPRESS_QUEUE_ELEMS_FREE(out_call->elem_header);

//     release_one_call(out_call);
//     return;
// }

/**
 * @brief 创建一个thread_context，并根据这个context新建一个线程
 *
 * @param context 需要初始化的线程context
 */
Thread_Context *thread_context_create(unsigned long long thread_id, unsigned long long device_id, unsigned long long len, Express_Device_Info *info)
{

    Thread_Context *context = g_malloc0(len);
    memset(context, 0, len);
    context->device_id = device_id;
    context->thread_id = thread_id;

    //环形缓冲区初始化
    memset(context->call_buf, 0, (CALL_BUF_SIZE + 2) * sizeof(Teleport_Express_Call *));

    context->read_loc = 0;
    context->write_loc = 0;
    // context->atomic_event_lock = 0;
    context->init = 0;
    context->thread_run = 1;

    context->context_init = info->context_init;
    context->context_destroy = info->context_destroy;
    context->call_handle = info->call_handle;

    context->teleport_express_device = teleport_express_device;

//线程缓冲区事件初始化
// qemu_event_init(&(context->data_event), false);
#ifdef _WIN32
    context->data_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
    context->data_event = create_event(0, 0);
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
void push_to_thread(Teleport_Express_Call *call)
{

    uint64_t thread_id = call->thread_id;
    uint64_t process_id = call->process_id;
    uint64_t unique_id = call->unique_id;

    uint64_t device_id = GET_DEVICE_ID(call->id);
    uint64_t fun_id = GET_FUN_ID(call->id);

    if (device_id == EXPRESS_CTRL_DEVICE_ID)
    {
        express_device_ctrl_invoke(call);
        return;
    }

    Express_Device_Info *device_info = get_express_device_info(device_id);
    if (device_info == NULL || (device_info->device_type & OUTPUT_DEVICE_TYPE) == 0)
    {
        express_printf("something bad happened %llu %llu\n", device_id, fun_id);
        call->callback(call, 0);
        return;
    }
    express_printf("push to %s thread_id %llu %08x fun id%llu %llu %08x unique id %08x\n", device_info->name, call->thread_id, call->thread_id, device_id, fun_id, call->id, call->unique_id);

    Thread_Context *context = (Thread_Context *)device_info->get_context(device_id, thread_id, process_id, unique_id, device_info);

    //找得到相应的设备处理时才把他推送到相应的设备线程
    if (context != NULL)
    {
        if (fun_id == EXPRESS_TERMINATE_FUN_ID)
        {
            if (device_info->remove_context)
            {
                if(device_info->remove_context(device_id, thread_id, process_id, unique_id, device_info))
                {
                    call->is_end = 1;
                    context->init = 0;
                }
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
    if (recycle_event.win_event == NULL)
    {
        recycle_event.win_event = create_event(0, 0);
    }
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
    if (recycle_event.win_event != NULL)
    {
        set_event(recycle_event.win_event);
    }
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
    if (recycle_event.win_event != NULL)
    {
        wait_event(recycle_event.win_event, 1);
    }
#endif
}

// int push_cnt = 0;
/**
 * @brief 真正用于取出vring上传过来的数据，然后调用相关解码的线程
 *
 * @param opaque 这个传入的是VirtIODevice
 * @return void*
 */
void *call_distribute_thread(void *opaque)
{
    VirtIODevice *vdev = opaque;
    Teleport_Express *e = TELEPORT_EXPRESS(vdev);
    VirtQueue *vq = e->out_data_queue;

    teleport_express_device = vdev;

    // Teleport_Express_Call *call = NULL;

    memset(call_recycle_queue, 0, sizeof(call_recycle_queue));
    call_recycle_queue_header = 0;
    call_recycle_queue_tail = 0;

    //已经释放，但是还没有通知对方的call数量
    int release_cnt = 0;

    init_distribute_event();

    int pop_cnt = 0;
    int in_handle_num = 0;

    e->distribute_thread_run = 2;

    //这个挪到了线程建立之前
    // guest_null_ptr_init(vq);

    // int64_t spend_time_all = 0;
    // int64_t call_num = 0;

    // return;
#ifdef DISTRIBUTE_WHEN_VM_EXIT
    while (qatomic_cmpxchg(&atomic_distribute_thread_running, 0, 1) != 0)
        ;
#endif
    while (e->distribute_thread_run && !teleport_express_should_stop)
    {

        int has_handle_flag = 0;
        int pop_flag = 1;
        int recycle_flag = 1;
        int need_irq = 0;
        virtqueue_data_distribute_and_recycle(vq, &pop_flag, &recycle_flag, &need_irq);

        if (pop_flag != 0)
        {
            pop_cnt += 1;
            in_handle_num += 1;
            has_handle_flag = 1;
        }
        if (recycle_flag != 0)
        {
            in_handle_num -= 1;
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

#ifdef DISTRIBUTE_WHEN_VM_EXIT
            qatomic_set(&atomic_distribute_thread_running, 0);
#endif
            //休眠采用可以被其他线程打断的休眠，主要是被处理线程打断，打断的目的也是为了减小延迟
            distribute_wait();
            if (teleport_express_should_stop)
            {
                return NULL;
            }

#ifdef DISTRIBUTE_WHEN_VM_EXIT
            if (qatomic_cmpxchg(&atomic_distribute_thread_running, 0, 1) == 1)
            {
                //人家在跑着，我得告诉他我准备好了，他得赶紧结束
                if (qatomic_cmpxchg(&atomic_distribute_thread_running, 1, 2) == 1)
                {
                    int cnt_lock = 0;
                    while (qatomic_cmpxchg(&atomic_distribute_thread_running, 0, 1) != 0)
                    {
                        cnt_lock++;
                        // if(cnt_lock%100 == 0)
                        // {
                        //     printf("lock %d %d\n",cnt_lock,qatomic_read(&atomic_distribute_thread_running));
                        // }
                    }
                }
                else
                {
                    // 还没设置2表示我准备好了，结果人家就结束了，当然是接着继续运行了
                    // printf("lock fail %d\n",qatomic_read(&atomic_distribute_thread_running));
                }
            }
#endif
        }

        //下面这个不需要，因为高负载下，并不依赖与中断注入来回收数据
        //高负载下依赖flag标志来回收数据
        if (release_cnt >= 128)
        {
            // express_printf("recycle 128\n");
            //平均一个call占用的空间为2左右，所以queue里理论上最大有1024/2=512个call，保留一定量的余量空间
            //剩下的空间里留一部分给处理过程消耗，因此假设留给释放的call大概在128左右
            //所以这时需要赶紧释放空间，防止queue满了
            //  express_printf("%s notify 128 with %d\n",get_now_time(),release_cnt);
            release_cnt = 0;
            virtio_notify(VIRTIO_DEVICE(vdev), vq);
        }
    }

    return NULL;
}

/**
 * @brief 分发线程处理virtqueue的call的函数
 *
 * @param vq Teleport_Express和VirtQueue
 * @param pop_flag 是否有取出call的flag
 * @param recycle_flag 是否有回收call的flag
 * @return void
 */
void virtqueue_data_distribute_and_recycle(VirtQueue *vq, int *pop_flag, int *recycle_flag, int *need_irq)
{
    Teleport_Express_Call *call = NULL;
    int origin_pop_flag = *pop_flag;
    int origin_recycle_flag = *recycle_flag;
    *pop_flag = 0;
    *recycle_flag = 0;
    if (origin_pop_flag == 1 && (call = pack_call_from_queue(vq, 0)) != NULL)
    {
        //从queue中打包调用，假如打包失败的话，失败的部分也还是会还给guest
        express_printf("virtio has data push\n");
        // draw_call的其他部分都已经初始化过了
        call->vdev = teleport_express_device;
        call->callback = push_free_callback;
        call->is_end = 0;
        push_to_thread(call);
        if (GET_DEVICE_ID(call->id) == EXPRESS_CTRL_DEVICE_ID && FUN_NEED_SYNC(call->id))
        {
            *need_irq = 1;
        }
        *pop_flag = 1;
    }
    //这里之前是else if，高负载下导致大量call被堆积到这里，一直没法回收，影响了性能，因此这里进行修改
    //改为一次取数据对应着一次回收数据
    if (origin_recycle_flag == 1 && call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] != NULL)
    {
        //将回收的部分和分发的部分放到一起是为了减小延迟

        //出队直接把队头后面的数据交换出来，队头那里没有放数据，数据都是放在后面一个了
        //这里没有使用无锁的方式是因为就这一个地方会出队，所以不存在并发问题
        Teleport_Express_Call *out_call = call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)];
        call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] = NULL;
        // Teleport_Express_Call *out_call=atomic_xchg(&call_recycle_queue[(call_recycle_queue_header+1)%(CALL_BUF_SIZE+2)],NULL);
        call_recycle_queue_header = (call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2);
        if (FUN_NEED_SYNC(out_call->id))
        {
            *need_irq = 1;
        }
        release_one_call(out_call, false);
        *recycle_flag = 1;
    }

    return;
}

/**
 * @brief 在处理线程使用完数据后的回调函数，将调用完成的call送给回收线程，使用无锁队列实现入队，同时，在传回之前，会将相关数据复制回去，同时设置好guest会读取的flag
 *
 * @param call 需要回收的call
 * @param notify 指示是否需要通知回收线程快速回收
 */
void push_free_callback(Teleport_Express_Call *call, int notify)
{

    common_call_callback(call);

    //无锁入队
    int origin_tail = call_recycle_queue_tail;
    int t = origin_tail;
    do
    {
        while (call_recycle_queue[(t + 1) % (CALL_BUF_SIZE + 2)] != NULL)
        {
            t = (t + 1) % (CALL_BUF_SIZE + 2);
        }
    } while (qatomic_cmpxchg(&(call_recycle_queue[(t + 1) % (CALL_BUF_SIZE + 2)]), NULL, call) != NULL);

    qatomic_cmpxchg(&call_recycle_queue_tail, origin_tail, (t + 1) % (CALL_BUF_SIZE + 2));

    if (notify)
    {
        //入队后要尝试中断掉分发回收线程的休眠（轮询过程中的休眠）
        wake_up_distribute();
    }
}
