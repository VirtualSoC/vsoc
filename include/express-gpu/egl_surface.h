#ifndef EGL_SURFACE_H
#define EGL_SURFACE_H


#include "direct-express/direct_express_distribute.h"
#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"


#include "express-gpu/egl_define.h"

#define VALIDATE_DISPLAY \
     Egl_Display *display = &(context->egl_display); \
     if (!display->is_init) { init_display(display); } \

EGLBoolean d_eglSwapBuffers_special(Render_Thread_Context *context,EGLDisplay dpy, EGLSurface surface);
EGLBoolean d_eglMakeCurrent_special(Render_Thread_Context *context,EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
EGLBoolean d_eglTerminate_special(Render_Thread_Context *context, EGLDisplay dpy);
int d_getEGLConfigParam_special(Render_Thread_Context *context, int* num_configs);
EGLint d_getEGLConfigs_special(Render_Thread_Context *context, EGLint num_attrs, EGLint* attr_list);

#endif