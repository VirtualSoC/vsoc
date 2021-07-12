

#ifndef DEFINE_EGL_H
#define DEFINE_EGL_H

#include <EGL/egl.h>


# define EXPRESS_GPU_FUN_ID ((unsigned long long)1)


#define MAX_OUT_BUF_LEN 4096


#define FUNID_eglSwapBuffers_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+100000)
EGLBoolean d_eglSwapBuffers_special(void *context, EGLDisplay dpy, EGLSurface surface);

#define FUNID_eglMakeCurrent_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+100001)
EGLBoolean d_eglMakeCurrent_special(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);

#define FUNID_eglTerminate_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+100002)
EGLBoolean d_eglTerminate_special(void *context, EGLDisplay dpy);

#define FUNID_getEGLConfigParam_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+100003)
int d_getEGLConfigParam_special(void *context, int* num_configs);

#define FUNID_getEGLConfigs_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+100004)
EGLint d_getEGLConfigs_special(void *context, EGLint num_attrs, EGLint* attr_list);

#define FUNID_getEGLVersion_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+100005)
EGLBoolean d_getEGLVersion_special(void *context, EGLint* ver_major, EGLint* ver_minor);









#endif
