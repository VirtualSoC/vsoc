
#ifndef ALL_EGL_H
#define ALL_EGL_H

#include <EGL/egl.h>
EGLBoolean d_eglSwapBuffers_special(void *context, EGLDisplay dpy, EGLSurface surface);
EGLBoolean d_eglMakeCurrent_special(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
EGLBoolean d_eglTerminate_special(void *context, EGLDisplay dpy);
int d_getEGLConfigParam_special(void *context, int* num_configs);
EGLint d_getEGLConfigs_special(void *context, EGLint num_attrs, EGLint* attr_list);
EGLBoolean d_getEGLVersion_special(void *context, EGLint* ver_major, EGLint* ver_minor);

#endif