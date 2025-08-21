#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_subprocess.h"
void qemu_system_killed(int signal, pid_t pid);
/**
 * @brief 所有的express设备共用的init函数，这个函数会在main函数前调用
 *
 * @param info
 */
void express_device_init_common(Express_Device_Info *info)
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

void monitor_log(Monitor *mon, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LOGW("warning! monitoring not implemented for device: %s", fmt);
    va_end(args);
}

void qemu_system_killed(int signal, pid_t pid) {
    LOGI("subprocess qemu killed: signal=%d pid=%d", signal, pid);
}