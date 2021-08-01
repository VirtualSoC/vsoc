#ifndef EGL_CONFIG_H
#define EGL_CONFIG_H

#include "egl.h"
#include "qemu/osdep.h"

#include "express-gpu/egl_display.h"

#define NUM_ATTRS 32
// 顺序不能变
typedef struct
{
    EGLint buffer_size;
    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint alpha_size;
    EGLBoolean bind_to_tex_rgb;
    EGLBoolean bind_to_tex_rgba;
    EGLenum caveat;
    EGLint config_id;
    EGLint frame_buffer_level;
    EGLint depth_size;
    EGLint max_pbuffer_width;
    EGLint max_pbuffer_height;
    EGLint max_pbuffer_size;
    EGLint max_swap_interval;
    EGLint min_swap_interval;
    EGLBoolean native_renderable;
    EGLint renderable_type;
    EGLint native_visual_id;
    EGLint native_visual_type;
    EGLint sample_buffers_num;
    EGLint samples_per_pixel;
    EGLint stencil_size;
    EGLint luminance_size;
    EGLint wanted_buffer_size;
    EGLint surface_type;
    EGLenum transparent_type;
    EGLint trans_red_val;
    EGLint trans_green_val;
    EGLint trans_blue_val;
    EGLenum conformant;
    EGLint color_buffer_type;

    void *pixel_format;
} eglConfig;

extern const unsigned int config_attrs[NUM_ATTRS];


EGLint d_eglGetEGLConfigParam(void *context, EGLint *num_configs);

EGLint d_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list);

EGLBoolean d_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list,
                             EGLConfig *configs, EGLint config_size, EGLint *num_config);

void d_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display);


size_t get_attrib_list_len(const EGLint *attrib_list);

#endif