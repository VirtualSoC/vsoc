#include "hw/teleport-express/express_platform.h"
#include "hw/express-gpu/express_gpu_main_window.h"

#include "hw/teleport-express/express_device.h"

ExpressPlatformOps g_ops;

static bool should_stop = false;

void init_express_platform(ExpressPlatformOps ops) {
    g_ops = ops;
}

void init_express_device(Express_Device_Info *info) {
    express_device_init_common(info);
}

void deinit_express_device(void) {
    should_stop = true;
    express_gpu_shutdown_notify_callback();
    // deinit
}

bool platform_device_should_stop(void) {
    return should_stop;
}

// Duplicate a Guest_Mem (deep copy scatter list) for persistence beyond handler scope
Guest_Mem *duplicate_guest_mem(Guest_Mem *orig) {
    if (!orig) return NULL;
    Guest_Mem *cpy = g_malloc(sizeof(Guest_Mem));
    cpy->num = orig->num;
    cpy->all_len = orig->all_len;
    cpy->scatter_data = g_malloc(sizeof(Scatter_Data) * cpy->num);
    memcpy(cpy->scatter_data, orig->scatter_data, sizeof(Scatter_Data) * cpy->num);
    return cpy;
}

void free_duplicated_guest_mem(Guest_Mem *mem) {
    if (mem) {
        g_free(mem->scatter_data);
        g_free(mem);
    }
}