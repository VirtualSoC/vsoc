#include "express-gpu/egl_display.h"
#include "express-gpu/egl_config.h"

const unsigned int config_attrs[NUM_ATTRS] = {
    EGL_BUFFER_SIZE,
    EGL_RED_SIZE,
    EGL_GREEN_SIZE,
    EGL_BLUE_SIZE,
    EGL_ALPHA_SIZE,
    EGL_BIND_TO_TEXTURE_RGB,
    EGL_BIND_TO_TEXTURE_RGBA,
    EGL_CONFIG_CAVEAT,
    EGL_CONFIG_ID,
    EGL_LEVEL,
    EGL_DEPTH_SIZE,
    EGL_MAX_PBUFFER_WIDTH,
    EGL_MAX_PBUFFER_HEIGHT,
    EGL_MAX_PBUFFER_PIXELS,
    EGL_MAX_SWAP_INTERVAL,
    EGL_MIN_SWAP_INTERVAL,
    EGL_NATIVE_RENDERABLE,
    EGL_RENDERABLE_TYPE,
    EGL_NATIVE_VISUAL_ID,
    EGL_NATIVE_VISUAL_TYPE,
    EGL_SAMPLE_BUFFERS,
    EGL_SAMPLES,
    EGL_STENCIL_SIZE,
    EGL_LUMINANCE_SIZE,
    EGL_BUFFER_SIZE,
    EGL_SURFACE_TYPE,
    EGL_TRANSPARENT_TYPE,
    EGL_TRANSPARENT_RED_VALUE,
    EGL_TRANSPARENT_GREEN_VALUE,
    EGL_TRANSPARENT_BLUE_VALUE,
    EGL_CONFORMANT,
    EGL_COLOR_BUFFER_TYPE};

Egl_Display *default_egl_display;

EGLint d_eglGetEGLConfigParam(void *context, EGLint *num_configs)
{
    *num_configs = g_hash_table_size(default_egl_display->egl_config_set);
    return sizeof(config_attrs) / sizeof(config_attrs[0]);
}

EGLint d_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list)
{
    GList *config_list = g_hash_table_get_values(default_egl_display->egl_config_set);

    int index = 0;
    memcpy(attr_list, config_attrs, NUM_ATTRS * sizeof(unsigned int));
    index++;

    for (GList *it = config_list; it; it = it->next)
    {
        eglConfig *config = (eglConfig *)it->data;
        memcpy(attr_list + index * NUM_ATTRS, config, NUM_ATTRS * sizeof(unsigned int));
        index++;
    }

    return index - 1;
}

EGLBoolean d_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    //@todo 可能要在render_thread_context里加点啥，方便之后建立窗口的时候设置上去
    //记得加点啥后要初始化
}

void d_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display)
{
    //@todo留着为以后有多个display的情况的处理
}

size_t get_attrib_list_len(const EGLint *attrib_list)
{
    size_t i = 0;
    while (attrib_list[i] != EGL_NONE)
    {
        i++;
    }
    return i + 1;
}