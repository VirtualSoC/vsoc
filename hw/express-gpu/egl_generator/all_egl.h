
#ifndef ALL_EGL_H
#define ALL_EGL_H

#include <EGL/egl.h>
EGLBoolean d_eglSwapBuffers_special(void *context, EGLDisplay dpy, EGLSurface surface);
EGLBoolean d_eglMakeCurrent_special(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
EGLBoolean d_eglTerminate_special(void *context, EGLDisplay dpy);
int d_getEGLConfigParam_special(void *context, int* num_configs);
