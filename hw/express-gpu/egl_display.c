#include "express-gpu/egl_display.h"

Egl_Display *default_egl_display;

EGLBoolean add_config(Egl_Display *display, eglConfig *config)
{
    // 过滤掉一些奇葩值，另外不需要抗锯齿
    // if (config->red_size > 8 ||
    //     config->green_size > 8 ||
    //     config->blue_size > 8 ||
    //     config->depth_size < 24 ||
    //     config->stencil_size < 8 ||
    //     config->samples_per_pixel > 0) {
    //     return EGL_FALSE;
    // }

    if (!is_config_in_table(config, display->egl_config_set))
    {
        config->config_id = g_hash_table_size(display->egl_config_set) + 1;
        g_hash_table_insert(display->egl_config_set, GINT_TO_POINTER(config->config_id), (gpointer)config);
        return EGL_TRUE;
    }
    else
    {
        return EGL_FALSE;
    }
}

/**
 * @brief 添加与窗口无关的配置信息，可以指定某个配置属性，添加一系列值
 * 
 * @param display 需要添加配置的display
 * @param attr_enum 需要添加的配置属性的Enum，比如EGL_RED_SIZE
 * @param vals 需要添加的属性值数组
 * @param val_size 属性值数组的长度
 */
void add_window_independent_config(Egl_Display *display, EGLint attr_enum, EGLint *vals, EGLint val_size)
{
    int config_set_size = g_hash_table_size(display->egl_config_set);
    for (int i = 0; i < config_set_size; i++)
    {
        eglConfig *config = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GINT_TO_POINTER(i + 1));
        for (int j = 0; j < val_size; j++)
        {
            eglConfig *new_config = (eglConfig *)malloc(sizeof(eglConfig));
            memcpy(new_config, config, sizeof(eglConfig));
            set_val_by_enum(new_config, vals[j], attr_enum);
            if (!add_config(display, new_config))
            {
                free(new_config);
            }
        }
    }
}