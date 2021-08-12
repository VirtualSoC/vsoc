#include "express-gpu/egl_display.h"
#include "express-gpu/egl_config.h"
#include <GLFW/glfw3.h>

const unsigned int config_attrs[NUM_ATTRS] = {
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
    EGL_COLOR_BUFFER_TYPE,
    EGL_ALPHA_MASK_SIZE,
    EGL_RECORDABLE_ANDROID,
    EGL_FRAMEBUFFER_TARGET_ANDROID};

const int64_t config_hints[NUM_HINTS] = {
    GLFW_RED_BITS,
    GLFW_GREEN_BITS,
    GLFW_BLUE_BITS,
    GLFW_ALPHA_BITS,
    GLFW_STENCIL_BITS,
    GLFW_DEPTH_BITS};

EGLBoolean is_config_in_table(eglConfig *config, GHashTable *table)
{
    GList *config_list = g_hash_table_get_values(table);
    for (GList *it = config_list; it; it = it->next)
    {
        eglConfig *cur_config = (eglConfig *)it->data;
        if (is_config_equaled(config, cur_config))
            return EGL_TRUE;
    }
    return EGL_FALSE;
}

EGLBoolean is_config_equaled(eglConfig *config, eglConfig *other)
{
    return ATTRIB_EQ(buffer_size) &&
           ATTRIB_EQ(red_size) &&
           ATTRIB_EQ(green_size) &&
           ATTRIB_EQ(blue_size) &&
           ATTRIB_EQ(alpha_size) &&
           ATTRIB_EQ(bind_to_tex_rgb) &&
           ATTRIB_EQ(bind_to_tex_rgba) &&
           ATTRIB_EQ(caveat) &&
           // ATTRIB_EQ(config_id) &&
           ATTRIB_EQ(frame_buffer_level) &&
           ATTRIB_EQ(depth_size) &&
           ATTRIB_EQ(max_pbuffer_width) &&
           ATTRIB_EQ(max_pbuffer_height) &&
           ATTRIB_EQ(max_pbuffer_size) &&
           ATTRIB_EQ(max_swap_interval) &&
           ATTRIB_EQ(min_swap_interval) &&
           ATTRIB_EQ(native_renderable) &&
           ATTRIB_EQ(renderable_type) &&
           // ATTRIB_EQ(native_visual_id) &&
           // ATTRIB_EQ(native_visual_type) &&
           ATTRIB_EQ(sample_buffers_num) &&
           ATTRIB_EQ(samples_per_pixel) &&
           ATTRIB_EQ(stencil_size) &&
           ATTRIB_EQ(luminance_size) &&
           ATTRIB_EQ(surface_type) &&
           ATTRIB_EQ(transparent_type) &&
           ATTRIB_EQ(trans_red_val) &&
           ATTRIB_EQ(trans_green_val) &&
           ATTRIB_EQ(trans_blue_val) &&
           ATTRIB_EQ(recordable_android) &&
           ATTRIB_EQ(framebuffer_target_android) &&
           ATTRIB_EQ(conformant) &&
           ATTRIB_EQ(color_buffer_type);
}

EGLBoolean config_to_hints(EGLConfig cfg, GLFWHints *hints)
{
    eglConfig *config = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GINT_TO_POINTER(cfg));
    for (int i = 0; i < NUM_HINTS; i++)
    {
        hints->hints[i * 2] = config_hints[i];
        hints->hints[i * 2 + 1] = get_hint_by_config(config, config_hints[i]);
    }
    hints->hints[HINTS_LEN - 1] = GLFW_DONT_CARE;
}

EGLint get_hint_by_config(eglConfig *config, int64_t hint_enum)
{

    switch (hint_enum)
    {
    case GLFW_RED_BITS:
        return config->red_size;
    case GLFW_GREEN_BITS:
        return config->green_size;
    case GLFW_BLUE_BITS:
        return config->blue_size;
    case GLFW_ALPHA_BITS:
        return config->alpha_size;
    case GLFW_STENCIL_BITS:
        return config->stencil_size;
    case GLFW_DEPTH_BITS:
        return config->depth_size;
    default:
        return 0;
    }
}

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
    return EGL_TRUE;
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
    return (i + 1) * sizeof(EGLint);
}