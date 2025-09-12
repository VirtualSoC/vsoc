#include "hw/vsoc/teleport_express_call.h"
#include "hw/vsoc/express_device.h"
#include "hw/vsoc/express_log.h"

#include "hw/vsoc/express_device_ctrl.h"

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
