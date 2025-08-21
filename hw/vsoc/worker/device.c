#include "hw/vsoc/express_log.h"
#include "hw/vsoc/worker/device.h"

void init_express_device(Express_Device_Info *info)
{
    LOGI("express device init %s", info->name);
}

/**
 * @brief 处理线程运行函数，分发线程会分发call到这个线程，然后调用call_handler进行处理
 *
 * @param opaque
 * @return void*
 */
void *handle_thread_run(void *opaque)
{
    LOGE("error! handle thread not implemented!");
    return NULL;
}