#ifndef EGL_SURFACE_H
#define EGL_SURFACE_H


#include "direct-express/direct_express_distribute.h"
#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"


#include "express-gpu/egl_define.h"




EGLBoolean d_eglSwapBuffers_special(Render_Thread_Context *context,EGLDisplay dpy, EGLSurface surface);

EGLBoolean d_eglMakeCurrent_special(Render_Thread_Context *context,EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);


EGLBoolean d_eglTerminate_special(Render_Thread_Context *context, EGLDisplay dpy);

void d_getEGLConfigParam_special(Render_Thread_Context *context, int *num_configs, int *num_config_attrs);

#endif