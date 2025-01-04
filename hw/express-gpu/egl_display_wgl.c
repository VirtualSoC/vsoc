// #define STD_DEBUG_LOG
#include "hw/express-gpu/egl_display_wgl.h"
#include "hw/teleport-express/express_log.h"
#include <wingdi.h>

Egl_Display_WGL default_wgl_display;

// 加载WGL函数
#define LOAD_WGL_FUNCS(return_type, func_name, param)                                                                          \
    wgl_display->wgl_ext->func_name = (return_type(WINAPI *) param)GetProcAddress(wgl_display->wgl_ext->instance, #func_name); \
    if (!(wgl_display->wgl_ext->func_name))                                                                                    \
    {                                                                                                                          \
        LOGE("Fail to load %s", #func_name);                                                                                 \
    }

// 加载WGL Extension函数
#define LOAD_WGL_EXT_FUNCS(return_type, func_name, param)                                                               \
    wgl_display->wgl_ext->func_name = (return_type(WINAPI *) param)wgl_display->wgl_ext->wglGetProcAddress(#func_name); \
    if (!(wgl_display->wgl_ext->func_name))                                                                             \
    {                                                                                                                   \
        LOGE("Fail to load %s", #func_name);                                                                          \
    }


/**
 * @brief 初始化Egl_Display
 *
 * @param display_point 待初始化的Egl_Display
 */
void init_display(Egl_Display **display_point)
{
    default_egl_display = g_malloc0(sizeof(Egl_Display));

    LOGD("init display");
    init_configs(default_egl_display);

    default_egl_display->guest_ver_major = 1;
    default_egl_display->guest_ver_minor = 5;

    default_egl_display->is_init = true;
}

/**
 * @brief 初始化EGL configuration并将所有可用configuration保存到全局的hash表中
 *
 * @param display 待初始化的Egl_Display
 */
void init_configs(Egl_Display *display)
{
    if (display->egl_config_set == NULL)
    {
        display->egl_config_set = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    // 添加一些与窗口无关的配置
    add_simple_config(display);
    add_window_independent_config(display, EGL_DEPTH_SIZE, depth_vals, NUM_DEPTH_VAL);
    add_window_independent_config(display, EGL_STENCIL_SIZE, stencil_vals, NUM_STENCILE_VAL);
    add_window_independent_config(display, EGL_SAMPLES, sample_vals, NUM_SAMPLE_VAL);
}
