#pragma once
#include "hw/vsoc/teleport_express.h"
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"


#define EXPRESS_DEVICE_INIT(device_name, info)                                       \
    static void __attribute__((constructor)) express_thread_init_##device_name(void) \
    {                                                                                \
        express_device_init_common(info);                                            \
    }


/**
 * @brief 自定义的Queue_Elem结构体，用来接收guest端传输过来的数据元信息
 *
 */
typedef struct Teleport_Express_Queue_Elem
{
    VirtQueueElement elem;

    //该数据的对外指针
    void *para;

    //数据的长度
    size_t len;

    struct Teleport_Express_Queue_Elem *next;
} Teleport_Express_Queue_Elem;


typedef bool (*EXPRESS_DECODE_FUN)(void *, uint64_t, const Call_Para *, int);

extern Device_Log_Setting_Info express_device_log_setting_info;

extern char *kernel_load_express_driver_names;
extern int kernel_load_express_driver_num;

void express_device_init_common(Express_Device_Info *info);

Express_Device_Info *get_express_device_info(unsigned int device_id);
Express_Device_Info *get_express_device_info_by_name(const char *name);

void cluster_decode_invoke(Teleport_Express_Call *call, void *context, EXPRESS_DECODE_FUN decode_fun);
