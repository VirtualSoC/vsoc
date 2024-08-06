#ifndef EGL_DISPLAY_H
#define EGL_DISPLAY_H

#include "hw/express-gpu/egl_config.h"

#define RENDERABLE_SUPPORT 0 | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT;
#define PBUFFER_MAX_WIDTH 32767
#define PBUFFER_MAX_HEIGHT 32767
#define PBUFFER_MAX_PIXELS (PBUFFER_MAX_WIDTH * PBUFFER_MAX_HEIGHT)
#define MIN_SWAP_INTERVAL 0
#define MAX_SWAP_INTERVAL 10

typedef struct
{
    GHashTable *egl_config_set;
    EGLint guest_ver_major;
    EGLint guest_ver_minor;
    int is_init;
} Egl_Display;

extern Egl_Display *default_egl_display;

void init_display(Egl_Display **display);
void init_configs(Egl_Display *display);
EGLBoolean add_config(Egl_Display *display, eglConfig *config);
void add_window_independent_config(Egl_Display *display, EGLint attr_enum, const EGLint *vals, EGLint val_size);
void add_simple_config(Egl_Display *display);

#endif