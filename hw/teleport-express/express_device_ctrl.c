#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_ctrl.h"

int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Teleport_Express_Call *pre_call, Teleport_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data);
void release_call_none(Teleport_Express_Call *call, int notify);

void express_device_ctrl_invoke(Teleport_Express_Call *call)
{

    Call_Para all_para[MAX_PARA_NUM];
    express_printf("get ctrl invoke %llx\n", call->id);
    switch (call->id)
    {

    case FUNID_getExpressDeviceNum:
    {
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int temp_len = all_para[0].data_len;
        if (temp_len != 8)
        {
            printf("error len %d para_num %d FUNID_getExpressDeviceNum\n", temp_len, para_num);
            break;
        }
        uint64_t ret_data = (((uint64_t)kernel_load_express_driver_num) << 32) + (uint64_t)(strlen(kernel_load_express_driver_names) + 1);
        write_to_guest_mem(all_para[0].data, &ret_data, 0, 8);
    }
    break;

    case FUNID_getExpressDeviceNames:
    {
        if (kernel_load_express_driver_num == 0)
        {
            break;
        }
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int temp_len = all_para[0].data_len;
        int name_len = strlen(kernel_load_express_driver_names) + 1;
        if (temp_len < name_len)
        {
            printf("error len %d need len %d para_num %d FUNID_getExpressDeviceNames\n", temp_len, name_len, para_num);
            break;
        }
        write_to_guest_mem(all_para[0].data, (void *)kernel_load_express_driver_names, 0, name_len);
    }
    break;

    case FUNID_getExpressDeviceLogSettingInfo:
    {
        if (kernel_load_express_driver_num == 0)
        {
            break;
        }
        int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        int temp_len = all_para[0].data_len;
        if (temp_len > sizeof(express_device_log_setting_info))
        {
            printf("error len %d need len %d para_num %d FUNID_getExpressDeviceLogSettingInfo\n", temp_len, (int)sizeof(express_device_log_setting_info), para_num);
            break;
        }
        write_to_guest_mem(all_para[0].data, (void *)&express_device_log_setting_info, 0, temp_len);
    }
    break;

    default:
    {
        printf("unknow funid %llx with device-id 0\n", call->id);
    }
    break;
    }

    call->callback(call, true);

    return;
}

/**
 * @brief 把聚合好的数据解包，分解成不同的call，用于继续调用invoke函数
 *
 * @param context
 * @param call
 */
void cluster_decode_invoke(Teleport_Express_Call *call, void *context, EXPRESS_DECODE_FUN real_decode_fun)
{
    Call_Para all_para[MAX_PARA_NUM];

    unsigned char *send_async_buf;
    int send_async_buf_len;

    unsigned char *save_buf;

    // unsigned char temp_buf[1024];

    //把保存的两个参数数据取出来

    int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
    if (para_num != 2)
    {
        call->callback(call, 0);
        return;
    }

    size_t temp_len = 0;
    unsigned char *temp = NULL;

    temp_len = all_para[0].data_len;
    send_async_buf_len = temp_len;

    if (temp_len % 8 != 0)
    {
        call->callback(call, 0);
        return;
    }

    send_async_buf = g_malloc(temp_len);

    int null_flag = 0;
    temp = get_direct_ptr(all_para[0].data, &null_flag);

    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {
            // temp = temp_buf;
            read_from_guest_mem(all_para[0].data, send_async_buf, 0, all_para[0].data_len);
        }
        else
        {
            call->callback(call, 0);
            g_free(send_async_buf);
            return;
        }
    }
    else
    {
        memcpy(send_async_buf, temp, temp_len);
    }

    temp_len = all_para[1].data_len;

    save_buf = g_malloc(temp_len);

    null_flag = 0;
    temp = get_direct_ptr(all_para[1].data, &null_flag);
    // printf("get direct ptr %llx\n",temp);
    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {
            // temp = temp_buf;
            read_from_guest_mem(all_para[1].data, save_buf, 0, all_para[1].data_len);
        }
        else
        {
            call->callback(call, 0);
            g_free(send_async_buf);
            g_free(save_buf);
            return;
        }
    }
    else
    {
        memcpy(save_buf, temp, temp_len);
    }

    Teleport_Express_Call unpack_call;
    unpack_call.vq = NULL;
    unpack_call.vdev = NULL;
    unpack_call.callback = release_call_none;
    unpack_call.is_end = 0;

    unpack_call.spend_time = 0;
    unpack_call.next = NULL;

    Teleport_Express_Queue_Elem pre_elem[MAX_PARA_NUM + 1];
    Guest_Mem pre_mem[MAX_PARA_NUM + 1];
    Scatter_Data pre_s_data[MAX_PARA_NUM + 1];

    //依次从两个数组数据中取出数据，创建call
    int buf_loc = 0;
    int create_ret;
    while (buf_loc < send_async_buf_len)
    {
        create_ret = create_call_from_cluster((uint64_t *)(send_async_buf + buf_loc), save_buf, &unpack_call, pre_elem, pre_mem, pre_s_data);
        if (create_ret == 0)
        {
            break;
        }
        //解包的几个id还是原来的id
        unpack_call.thread_id = call->thread_id;
        unpack_call.process_id = call->process_id;
        unpack_call.unique_id = call->unique_id;

        buf_loc += (unpack_call.para_num * 2 + 2) * 8;
        if (buf_loc > send_async_buf_len)
        {
            //防止有的call有问题
            break;
        }

        real_decode_fun(context, &unpack_call);
    }
    //所有调用完成后，这个call要回收
    call->callback(call, 1);

    g_free(send_async_buf);
    g_free(save_buf);
    return;
}

/**
 * @brief 从聚合的数据中取出信息，创建一个call，用于之后调用
 *
 * @param send_buf 原始的发送数据
 * @param save_buf 保存的指针数据
 * @return
 */
int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Teleport_Express_Call *pre_call, Teleport_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data)
{

    pre_call->id = send_buf[0];

    //用9999作为聚合调用的id
    if (GET_FUN_ID(pre_call->id) == 9999)
    {
        return 0;
    }

    pre_call->para_num = send_buf[1];
    pre_call->elem_header = NULL;
    // assert(pre_call->para_num < 10);
    //第一个elem是用于存储各种id的，这个解包的call用不到。但是也得占位
    // Teleport_Express_Queue_Elem *elem = g_malloc(sizeof(Teleport_Express_Queue_Elem));
    pre_call->elem_header = &(pre_elem[0]);
    Teleport_Express_Queue_Elem *last_elem = &(pre_elem[0]);
    for (int i = 0; i < pre_call->para_num; i++)
    {
        //需要把这个pre_elem[i+1]中能填充的部分给填充起来

        // Guest_Mem *guest_mem = g_malloc(sizeof(Guest_Mem));
        // Scatter_Data *scatter_data = g_malloc(sizeof(Scatter_Data));

        if (send_buf[i * 2 + 2 + 1] != 0)
        {
            pre_scatter_data[i].len = send_buf[i * 2 + 2];
            pre_scatter_data[i].data = save_buf + send_buf[i * 2 + 2 + 1];
        }
        else
        {
            pre_scatter_data[i].len = 0;
            pre_scatter_data[i].data = NULL;
        }

        pre_guest_mem[i].scatter_data = &(pre_scatter_data[i]);
        pre_guest_mem[i].num = 1;
        pre_guest_mem[i].all_len = pre_scatter_data[i].len;

        pre_elem[i + 1].para = &(pre_guest_mem[i]);
        pre_elem[i + 1].len = send_buf[i * 2 + 2];
        pre_elem[i + 1].next = NULL;

        last_elem->next = &(pre_elem[i + 1]);
        last_elem = &(pre_elem[i + 1]);
    }
    pre_call->elem_tail = &(pre_elem[pre_call->para_num]);

    //因为这个call是解包的call，所以不能调用原先的callback，只能调用新的callback，这个里面会释放前面申请的各种数据
    //所以不论是vdev还是vq都用不上，不用设置来着

    return 1;
}

/**
 * @brief 用于call使用完成之后的回调
 *
 * @param call
 * @param notify
 */
void release_call_none(Teleport_Express_Call *call, int notify)
{
    return;
}
