#ifndef EGL_DRAW_H
#define EGL_DRAW_H

#include "direct-express/direct_express_distribute.h"
#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"

#include "express-gpu/egl_define.h"




EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read,
                                    EGLContext ctx);

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);

void d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target,
                                  EGLClientBuffer buffer, const EGLAttrib *attrib_list,EGLImage guest_image);

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);

//EGLClientBuffer d_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer *buffer);

EGLBoolean d_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval);


EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, int64_t invoke_time, int64_t *ret_invoke_time, int64_t *swap_time);

#endif