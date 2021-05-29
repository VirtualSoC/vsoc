#ifndef EGL_CONFIG_H
#define EGL_CONFIG_H

#include "egl.h"
#include "qemu/osdep.h"

typedef struct
{
    EGLint      buffer_size;
    EGLint      red_size;
    EGLint      green_size;
    EGLint      blue_size;
    EGLint      alpha_size;
    EGLBoolean  bind_to_tex_rgb;
    EGLBoolean  bind_to_tex_rgba;
    EGLenum     caveat;
    EGLint            config_id;
    EGLint      frame_buffer_level;
    EGLint      depth_size;
    EGLint      max_pbuffer_width;
    EGLint      max_pbuffer_height;
    EGLint      max_pbuffer_size;
    EGLint      max_swap_interval;
    EGLint      min_swap_interval;
    EGLBoolean  native_renderable;
    EGLint      renderable_type;
    EGLint      native_visual_id;
    EGLint      native_visual_type;
    EGLint      sample_buffers_num;
    EGLint      samples_per_pixel;
    EGLint      stencil_size;
    EGLint      luminance_size;
    EGLint      wanted_buffer_size;
    EGLint      surface_type;
    EGLenum     transparent_type;
    EGLint      trans_red_val;
    EGLint      trans_green_val;
    EGLint      trans_blue_val;
    EGLenum     conformant;
    EGLint      color_buffer_type;

    void*       pixel_format;
} eglConfig;

#endif