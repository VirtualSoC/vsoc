#include "express-gpu/egl_sync.h"

EGLBoolean d_eglWaitGL(void *context) {}

EGLBoolean d_eglWaitNative(void *context, EGLint engine) {}

EGLBoolean d_eglWaitClient(void *context) {}

EGLint d_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout) {}

EGLBoolean d_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value) {}

void d_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list, EGLSync guest_sync) {}

EGLBoolean d_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync) {}

EGLBoolean d_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags) {}