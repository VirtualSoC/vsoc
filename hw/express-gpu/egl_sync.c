#include "hw/express-gpu/egl_sync.h"
#include "hw/express-gpu/glv3_resource.h"

EGLBoolean d_eglWaitGL(void *context)
{
    // 实际不会调用到这里来
    glFinish();
    return EGL_TRUE;
}

EGLBoolean d_eglWaitNative(void *context, EGLint engine)
{
    // 实际会调用到这里来
    glFinish();
    return EGL_TRUE;
}

EGLBoolean d_eglWaitClient(void *context)
{
    // 实际不会调用到这里来
    glFinish();
    return EGL_TRUE;
}


// 下面这五个都不会调用到
EGLBoolean d_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLint *value)
{

    return EGL_TRUE;
}

void d_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLint *attrib_list, EGLSync guest_sync)
{

    return;
}

EGLBoolean d_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync)
{
    return EGL_TRUE;
}

EGLBoolean d_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags)
{
    return EGL_FALSE;
}

EGLint d_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{
    // LOGI("eglClientWaitSync guest_sync %d no host sync", guest_sync_int);
    return EGL_FALSE;
}