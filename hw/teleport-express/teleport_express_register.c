#include "hw/teleport-express/express_device_common.h"

#include "hw/teleport-express/teleport_express_register.h"

#include "qemu/atomic.h"

#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_event.h"

static VirtIODevice *in_teleport_express = NULL;

static Teleport_Express_Call *call_recycle_queue[(CALL_BUF_SIZE + 2)];
static volatile int call_recycle_queue_header = 0;
static volatile int call_recycle_queue_tail = 0;

static bool need_send_irq = false;

bool now_can_set_event = true;
#ifdef _WIN32
HANDLE input_event = NULL;
#else
void *input_event = NULL;
#endif

void send_express_device_irq(Teleport_Express_Call *irq_call, int buf_index, int len);
void common_device_irq_register(Device_Context *device_context, Teleport_Express_Call *irq_call);
void common_device_irq_release(Device_Context *device_context);

static void send_device_prop_to_guest(Express_Device_Info *device_info, Teleport_Express_Call *call)
{
    Call_Para paras[10];
    int para_num = get_para_from_call(call, paras, 10);
    if (unlikely(para_num != 1 || paras[0].data_len < device_info->static_prop_size))
    {
        printf("error! get_device_prop get %d para_num id %u data_len %d prop_size %d\n", para_num, GET_FUN_ID(call->id), (int)paras[0].data_len, device_info->static_prop_size);
        return;
    }

    write_to_guest_mem(paras[0].data, (void *)device_info->static_prop, 0, device_info->static_prop_size);

    call->callback(call, 1);
}

/**
 * @brief 把包装好的call推送到相应的设备
 *
 * @param call
 */
static void push_to_device(Teleport_Express_Call *call)
{

    uint64_t thread_id = call->thread_id;
    uint64_t process_id = call->process_id;
    uint64_t unique_id = call->unique_id;

    uint64_t device_id = GET_DEVICE_ID(call->id);
    uint64_t fun_id = GET_FUN_ID(call->id);

    Express_Device_Info *device_info = get_express_device_info(device_id);
    if (device_info == NULL || (device_info->device_type & INPUT_DEVICE_TYPE) == 0)
    {
        printf("something bad happened(when input) %llu %llu\n", device_id, fun_id);
        call->callback(call, 0);
        return;
    }
    express_printf("push to %s device %llx id %llx\n", device_info->name, device_id, call->id);

    Device_Context *device_context = device_info->get_device_context(device_id, thread_id, process_id, unique_id, device_info);
    if(unlikely(device_context == NULL))
    {
        LOGD("device %s: input call received with null device context! call id %llx", device_info->name, call->id);
        call->callback(call, 0);
        return;
    }
    
    if (unlikely(device_context->device_info == NULL))
    {
        device_context->device_info = device_info;
    }

    if (fun_id == EXPRESS_REGISTER_BUFFER_FUN_ID)
    {
        Guest_Mem *data = copy_guest_mem_from_call(call, 1);
        device_info->buffer_register(data, thread_id, process_id, unique_id);
        call->callback(call, 0);
    }
    else if (fun_id == EXPRESS_IRQ_FUN_ID)
    {
        common_device_irq_register(device_context, call);
    }
    else if (fun_id == EXPRESS_GET_PROP_FUN_ID && device_info->static_prop != NULL && device_info->static_prop_size != 0)
    {
        send_device_prop_to_guest(device_info, call);
    }
    else if (fun_id == EXPRESS_RELEASE_IRQ_FUN_ID)
    {
        common_device_irq_release(device_context);
        call->callback(call, 1);
    }
    else
    {
        printf("unknow fun id %llu device %llu\n", fun_id, device_id);
        call->callback(call, 0);
    }

    return;
}

/**
 * @brief 在处理线程使用完数据后的回调函数，将调用完成的call送给回收线程，使用无锁队列实现入队，同时，在传回之前，会将相关数据复制回去，同时设置好guest会读取的flag
 *
 * @param call 需要回收的call
 * @param notify 指示是否需要通知回收线程快速回收
 */
static void input_call_release(Teleport_Express_Call *call, int notify)
{
    // 设置guest端的flag标志，防止中断丢失
    common_call_callback(call);

    // 无锁入队
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

    // release_one_call(call, (bool)notify);

    // need_send_irq = true;
    if (input_event != NULL && now_can_set_event)
    {
#ifdef _WIN32
        SetEvent(input_event);
#else
        set_event(input_event);
#endif
        express_printf("slow input_event!\n");
    }
    else
    {
        express_printf("qucik input_event!\n");
    }

    return;
}

void register_input_buffer_call(VirtIODevice *vdev, VirtQueue *vq)
{
    // Teleport_Express_Call *call = get_one_call_from_input_queue(vq);
    if (unlikely(in_teleport_express == NULL))
    {
        in_teleport_express = vdev;
    }

    Teleport_Express_Call *call = pack_call_from_queue(vq, 1);

    // if (call == NULL)
    // {
    //     printf("register get no call\n");
    // }

    while (call != NULL)
    {
        call->callback = input_call_release;
        call->is_end = 0;
        call->vdev = in_teleport_express;
        push_to_device(call);

        call = pack_call_from_queue(vq, 1);
    }

    express_input_device_sync();

    return;
}

void send_express_device_irq(Teleport_Express_Call *irq_call, int buf_index, int len)
{

    Guest_Mem *mem = irq_call->elem_header->para;

    unsigned long long t_data = ((((uint64_t)buf_index) << 32) + (uint64_t)len);
    write_to_guest_mem(mem, &t_data, __builtin_offsetof(Teleport_Express_Flag_Buf, ret_data), 8);

    irq_call->callback(irq_call, 0);
}

void *input_sync_thread(void *opaque)
{

#ifdef _WIN32
    input_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
    input_event = create_event(0,0);
#endif
    while (!teleport_express_should_stop)
    {
        #ifdef _WIN32
        // 有一个中断时，之后的1ms超时内的中断都不再使能中断的打断，以防止中断过于频繁
        DWORD ret = WaitForSingleObject(input_event, 1);
        if (ret == WAIT_TIMEOUT)
        {
            now_can_set_event = true;
        }
        else
        {
            express_printf("intrupted by event\n");
            now_can_set_event = false;
        }
    #else    
        int ret=wait_event(input_event,1);
        if(ret == 0){
            now_can_set_event = true;
        }
        else{
            now_can_set_event = false;
        }
    #endif
        Teleport_Express *g = TELEPORT_EXPRESS(in_teleport_express);
        if (qatomic_cmpxchg(&(g->register_input_vq_locker), 0, 1) == 0)
        {
            // Teleport_Express *g = TELEPORT_EXPRESS(in_teleport_express);
            // if (qatomic_cmpxchg(&(g->register_input_vq_locker), 0, 1) == 0)
            // {
            //     register_input_buffer_call(in_teleport_express, g->in_data_queue);
            //     qatomic_set(&(g->register_input_vq_locker), 0);
            // }
            register_input_buffer_call(in_teleport_express, g->in_data_queue);
            qatomic_set(&(g->register_input_vq_locker), 0);

        }
    }
#ifdef _WIN32
    CloseHandle(input_event);
#else
    delete_event(input_event);
#endif
    return NULL;
}

void express_input_device_sync(void)
{
    while (call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] != NULL)
    {
        Teleport_Express_Call *out_call = call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)];
        call_recycle_queue[(call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2)] = NULL;
        // Teleport_Express_Call *out_call=atomic_xchg(&call_recycle_queue[(call_recycle_queue_header+1)%(CALL_BUF_SIZE+2)],NULL);
        call_recycle_queue_header = (call_recycle_queue_header + 1) % (CALL_BUF_SIZE + 2);

        release_one_call(out_call, false);

        need_send_irq = true;
    }

    if (need_send_irq)
    {
        virtio_notify(VIRTIO_DEVICE(in_teleport_express), TELEPORT_EXPRESS(in_teleport_express)->in_data_queue);
        need_send_irq = false;
        // printf("input sync\n");
    }
}

void common_device_irq_register(Device_Context *device_context, Teleport_Express_Call *irq_call)
{
    express_printf("irq register %s\n", device_context->device_info->name);

    Teleport_Express_Call *origin_call = NULL;
    if ((origin_call = qatomic_xchg(&device_context->irq_call, irq_call)) != NULL)
    {
        if (origin_call == (void *)1)
        {
            // 此时已经release过了，所以此时需要直接发送call
            // 但是可能此时继续产生send irq的中断请求，只是send出去的不会进行重置，所以这里进行二次交换，假如换到NULL，说明irq call被input函数发送出去了，就不用管了
            if ((origin_call = qatomic_xchg(&device_context->irq_call, NULL)) != NULL)
            {
                // 这里origin_call不可能再次为1，因为已经release过一次了
                if (origin_call == (void *)1)
                {
                    LOGE("error! %s register with half-released status get one release 1!\n", device_context->device_info->name);
                    return;
                }
                send_express_device_irq(origin_call, 0, 0);
                printf("%s release bewteen send and reset\n", device_context->device_info->name);
                return;
            }
        }
    }
    device_context->irq_enabled = true;
    if(device_context->device_info->irq_register != NULL)
    {
        device_context->device_info->irq_register(device_context);
    }
}

void common_device_irq_release(Device_Context *device_context)
{
    device_context->irq_enabled = false;

    LOGI("irq release %s", device_context->device_info->name);

    Teleport_Express_Call *origin_call = NULL;
    if ((origin_call = qatomic_xchg(&device_context->irq_call, 1)) != NULL)
    {
        if (origin_call != (void *)1)
        {
            send_express_device_irq(origin_call, 0, 0);

            // 在irq_call被release函数获取时，不可能存在进一步的中断注入，因而也不可能出现中断的重置，所以可以放心设置为NULL
            // 其他情况意味着在等待下一次中断重置过程中
            qatomic_xchg(&device_context->irq_call, NULL);
            LOGD("%s irq_release", device_context->device_info->name);
        }
        else
        {
            LOGE("error! %s release twice!", device_context->device_info->name);
        }
    }

    if(device_context->device_info->irq_release != NULL)
    {
        device_context->device_info->irq_release(device_context);
    }
}


int set_express_device_irq(Device_Context *device_context, int buf_index, int len)
{
    if (!device_context->irq_enabled)
    {
        LOGW("%s irq is not enabled!", device_context->device_info->name);
        return IRQ_NOT_ENABLE;
    }

    Teleport_Express_Call *origin_call = NULL;
    if ((origin_call = qatomic_xchg(&device_context->irq_call, NULL)) == NULL)
    {
        LOGW("%s irq not ok!", device_context->device_info->name);
        return IRQ_NOT_READY;
    }

    if (origin_call == (void *)1)
    {
        LOGW("%s has been released!", device_context->device_info->name);
        return IRQ_RELEASED;
    }

    express_printf("%s irq send ok!\n", device_context->device_info->name);
    send_express_device_irq(origin_call, buf_index, len);

    return IRQ_SET_OK;
}