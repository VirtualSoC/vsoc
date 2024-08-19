#ifndef EGL_SYNC_H
#define EGL_SYNC_H

#include "hw/teleport-express/teleport_express_call.h"
#include "hw/express-gpu/express_gpu_render.h"
#include "hw/express-gpu/express_gpu.h"

EGLBoolean d_eglWaitGL(void *context);

EGLBoolean d_eglWaitNative(void *context, EGLint engine);

EGLBoolean d_eglWaitClient(void *context);


EGLint d_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout);

EGLBoolean d_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLint *value);

void d_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLint *attrib_list,EGLSync guest_sync);

EGLBoolean d_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync);

EGLBoolean d_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags);


#endif