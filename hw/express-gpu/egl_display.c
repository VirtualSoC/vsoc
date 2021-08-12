#include "express-gpu/egl_display.h"


Egl_Display *default_egl_display;

EGLBoolean add_config(Egl_Display *display, eglConfig *config) {
    // 过滤掉一些奇葩值，另外不需要抗锯齿
    if (config->red_size > 8 ||
        config->green_size > 8 ||
        config->blue_size > 8 ||
        config->depth_size < 24 ||
        config->stencil_size < 8 ||
        config->samples_per_pixel > 0) {
        return EGL_FALSE;
    }

    if (!is_config_in_table(config, display->egl_config_set)) {
        config->config_id = g_hash_table_size(display->egl_config_set) + 1;
        g_hash_table_insert(display->egl_config_set, GINT_TO_POINTER(config->config_id), (gpointer)config);
        return EGL_TRUE;
    } else {
        return EGL_FALSE;
    }
}