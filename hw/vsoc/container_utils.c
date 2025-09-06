// #define DEBUG_LOG
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/container_utils.h"

#include "hw/vsoc/display/express_display.h"
#include "hw/vsoc/input/express_touchscreen.h"
#include "hw/vsoc/input/express_keyboard.h"

bool device_needs_proxy(int device_id) {
    return 
        device_id == EXPRESS_GPU_DEVICE_ID || 
        device_id == EXPRESS_SYNC_DEVICE_ID || 
        device_id == EXPRESS_DISPLAY_DEVICE_ID ||
        device_id == EXPRESS_MEM_DEVICE_ID  ||
        device_id == EXPRESS_TOUCHSCREEN_DEVICE_ID || 
        device_id == EXPRESS_KEYBOARD_DEVICE_ID;
}

// Map Android UID to worker ID
int wid_from_uid(uint64_t uid) {
    if (uid < 100000) return 0;
    int wid = uid / 100000 - 9;
    if (wid > 0 && wid < g_ops.express_display_count) {
        return wid;
    }
    return 0;
}

int wid_from_ids(uint64_t device_id, uint64_t unique_id, uint64_t uid) {
    switch (device_id) {
        case EXPRESS_DISPLAY_DEVICE_ID:
        case EXPRESS_TOUCHSCREEN_DEVICE_ID:
        case EXPRESS_KEYBOARD_DEVICE_ID: {
            return unique_id;
        }
    }
    return wid_from_uid(uid);
}

int wid_from_thread_context(Thread_Context *context) {
    if (!context) return -1;
    switch (context->device_id) {
        case EXPRESS_DISPLAY_DEVICE_ID: {
            Display_Context *dc = (Display_Context *)context;
            return dc->unique_id;
        } break;
        default: {
            return wid_from_uid(context->user_id);
        }
    }
}

int wid_from_device_context(Device_Context *context) {
    if (!context) return -1;
    switch (context->device_info->device_id) {
        case EXPRESS_TOUCHSCREEN_DEVICE_ID: {
            Touchscreen_Context *ctx = (Touchscreen_Context *)context;
            return ctx->id;
        } break;
        case EXPRESS_KEYBOARD_DEVICE_ID: {
            Keyboard_Context *ctx = (Keyboard_Context *)context;
            return ctx->id;
        } break;
        default: {
            return 0;
        }
    }
}