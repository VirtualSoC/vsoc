#ifndef EGL_CONTEXT_H
#define EGL_CONTEXT_H

#include "hw/direct-express/direct_express_distribute.h"
#include "hw/express-gpu/express_gpu_render.h"
#include "hw/express-gpu/offscreen_render_thread.h"

#include "hw/express-gpu/egl_define.h"


EGLBoolean d_eglTerminate(void *context, EGLDisplay dpy);


EGLBoolean d_eglReleaseThread(void *context);



void d_eglCreateContext(void *context, EGLDisplay dpy, EGLConfig config,EGLContext share_context, const EGLint *attrib_list,EGLContext guest_context);

EGLBoolean d_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx);


#endif