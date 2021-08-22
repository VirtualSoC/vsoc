#include "express-gpu/egl_sync.h"

EGLBoolean d_eglWaitGL(void *context)
{
    //实际不会调用到这里来
    glFinish();
    return EGL_TRUE;
}

EGLBoolean d_eglWaitNative(void *context, EGLint engine)
{
    //实际会调用到这里来
    glFinish();
    return EGL_TRUE;
}

EGLBoolean d_eglWaitClient(void *context)
{
    //实际不会调用到这里来
    glFinish();
    return EGL_TRUE;
}



//下面这几个客户端都会调用到gl的sync去，所以不会到这里来
EGLint d_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{
    return 0;
}

EGLBoolean d_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value)
{
    return 0;
}

void d_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list, EGLSync guest_sync)
{
    return;
}

EGLBoolean d_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync)
{
    return 0;
}

EGLBoolean d_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags)
{
    return 0;
}