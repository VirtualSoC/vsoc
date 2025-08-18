#pragma once
#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_platform.h"


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


extern Device_Log_Setting_Info express_device_log_setting_info;

extern bool express_gpu_gl_debug_enable;
extern bool express_gpu_enable_windowed_mode;
extern bool express_device_input_window_enable;
extern bool teleport_express_save_snapshot;

extern bool express_gpu_keep_window_scale;

extern int express_gpu_window_width;
extern int express_gpu_window_height;

extern int *express_touchscreen_size;

extern bool express_touchscreen_scroll_is_zoom;
extern bool express_touchscreen_right_click_is_two_finger;
extern int express_touchscreen_scroll_ratio;

extern bool express_keyboard_finger_replay;

extern char *kernel_load_express_driver_names;
extern int kernel_load_express_driver_num;

extern int express_display_pixel_width;
extern int express_display_pixel_height;
extern int express_display_refresh_rate;
extern uint64_t express_display_count;
extern char *express_display_options;
extern int express_keyboard_count;

extern bool express_display_headless_mode;

extern bool express_gpu_open_shader_binary;

extern char *express_ruim_file;

void express_device_init_common(Express_Device_Info *info);

Express_Device_Info *get_express_device_info(unsigned int device_id);
Express_Device_Info *get_express_device_info_by_name(const char *name);

void cluster_decode_invoke(Teleport_Express_Call *call, void *context, EXPRESS_DECODE_FUN decode_fun);
