#include "hw/teleport-express/express_device_common.h"

#include "hw/teleport-express/teleport_express_register.h"

#include "hw/teleport-express/express_log.h"

static VirtIODevice *in_teleport_express = NULL;

// /**
//  * @brief 从queue中打包出一个draw调用
//  *
//  * @param vq
//  * @return Teleport_Express_Draw_Call*
//  */
// static Teleport_Express_Call *get_one_call_from_input_queue(VirtQueue *vq)
// {

//     // static int pack_cnt = 0;
//     Teleport_Express_Queue_Elem *elem;

//     Teleport_Express_Call *call;

//     // unsigned long long para_num;
//     // unsigned long long fun_id;
//     // unsigned long long thread_id;
//     // unsigned long long process_id;
//     // unsigned long long unique_id;

//     elem = (Teleport_Express_Queue_Elem *)virtqueue_pop(vq, sizeof(Teleport_Express_Queue_Elem));

//     if (elem)
//     {
//         VirtQueueElement *v_elem = &elem->elem;
//         printf("get elem call num %u %u\n",v_elem->out_num,v_elem->in_num);
//         if ((v_elem->out_num != 0 && v_elem->in_num != 0) || (v_elem->out_num == 0 && v_elem->in_num == 0))
//         {
//             return NULL;
//         }
//         elem->para = NULL;
//         elem->next = NULL;

//         Guest_Mem *guest_mem = g_malloc(sizeof(Guest_Mem));

//         if (v_elem->out_num != 0)
//         {
//             guest_mem->scatter_data = (Scatter_Data *)v_elem->out_sg;
//             guest_mem->num = v_elem->out_num;
//         }

//         if (v_elem->in_num != 0)
//         {
//             guest_mem->scatter_data = (Scatter_Data *)v_elem->in_sg;
//             guest_mem->num = v_elem->in_num;
//         }

//         int buf_len = 0;
//         for (int i = 0; i < guest_mem->num; i++)
//         {
//             buf_len += guest_mem->scatter_data[i].len;
//             // express_printf("guest_mem %d i %d len %d now %d\n",num,i, guest_mem->scatter_data[i].len, buf_len);
//         }

//         guest_mem->all_len = buf_len;
//         elem->len = buf_len;

//         elem->para = guest_mem;

//         call = g_malloc0(sizeof(Teleport_Express_Call));
//         call->elem_header = elem;
//         call->elem_tail = elem;
//         call->vq = vq;

//         call->spend_time = 0;
//         call->next = NULL;

//         elem = (Teleport_Express_Queue_Elem *)virtqueue_pop(vq, sizeof(Teleport_Express_Queue_Elem));

//         int null_flag = 0;
//         Teleport_Express_Flag_Buf *flag_buf = get_direct_ptr(guest_mem, &null_flag);
//         if (null_flag != 0)
//         {
//             call->id = flag_buf->id;
//             call->process_id = flag_buf->process_id;
//             call->thread_id = flag_buf->thread_id;
//             call->para_num = flag_buf->para_num;
//             call->unique_id = flag_buf->unique_id;
//         }
//         else
//         {
//             Teleport_Express_Flag_Buf flag_buf_temp;
//             read_from_guest_mem(guest_mem, &flag_buf_temp, 0, sizeof(Teleport_Express_Flag_Buf));
//             call->id = flag_buf_temp.id;
//             call->process_id = flag_buf_temp.process_id;
//             call->thread_id = flag_buf_temp.thread_id;
//             call->para_num = flag_buf_temp.para_num;
//             call->unique_id = flag_buf->unique_id;
//         }

//         return call;
//     }
//     else
//     {
//         return NULL;
//     }
// }

static void send_device_prop_to_guest(Express_Device_Info *device_info, Teleport_Express_Call *call)
{
    Call_Para paras[10];
    int para_num = get_para_from_call(call, paras, 10);
    if (unlikely(para_num != 1 || paras[0].data_len < device_info->static_prop_size))
    {
        printf("error! get_device_prop get %d para_num id %llx data_len %d prop_size %d\n", para_num, GET_FUN_ID(call->id), (int)paras[0].data_len, device_info->static_prop_size);
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
    express_printf("\033[31mpush to %s device %llx id %llx \033[0m\n", device_info->name, device_id, call->id);

    if (fun_id == EXPRESS_REGISTER_BUFFER_FUN_ID)
    {
        Guest_Mem *data = copy_guest_mem_from_call(call, 1);
        device_info->buffer_register(data, thread_id, process_id, unique_id);
        call->callback(call, 0);
    }
    else if (fun_id == EXPRESS_IRQ_FUN_ID)
    {
        device_info->irq_register(call);
    }
    else if (fun_id == EXPRESS_GET_PROP_FUN_ID)
    {
        send_device_prop_to_guest(device_info, call);
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
    //设置guest端的flag标志，防止中断丢失
    common_call_callback(call);

    release_one_call(call, (bool)notify);

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

    if (call == NULL)
    {
        printf("register get no call\n");
    }

    while (call != NULL)
    {
        call->callback = input_call_release;
        call->is_end = 0;
        call->vdev = in_teleport_express;
        push_to_device(call);

        call = pack_call_from_queue(vq, 1);
    }
    return;
}

void send_express_device_irq(Teleport_Express_Call *irq_call, int buf_index, int len)
{

    Guest_Mem *mem = irq_call->elem_header->para;

    unsigned long long t_data = ((((uint64_t)buf_index) << 32) + (uint64_t)len);
    write_to_guest_mem(mem, &t_data, __builtin_offsetof(Teleport_Express_Flag_Buf, ret_data), 8);

    irq_call->callback(irq_call, 0);
}

void express_input_device_sync(void)
{
    virtio_notify(VIRTIO_DEVICE(in_teleport_express), TELEPORT_EXPRESS(in_teleport_express)->in_data_queue);
}
