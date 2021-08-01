
#include "define_egl.h"

/******* file '1-1' *******/

EGLBoolean d_eglGetConfigAttrib(void *context, EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{

    return d_eglGetConfigAttrib_special(context, dpy, config, attribute, value);
}

EGLBoolean d_eglGetConfigs(void *context, EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{

    return d_eglGetConfigs_special(context, dpy, configs, config_size, num_config);
}

EGLDisplay d_eglGetCurrentDisplay(void *context)
{

    return d_eglGetCurrentDisplay_special(context);
}

EGLSurface d_eglGetCurrentSurface(void *context, EGLint readdraw)
{

    return d_eglGetCurrentSurface_special(context, readdraw);
}

EGLint d_eglGetError(void *context)
{

    return d_eglGetError_special(context);
}

EGLBoolean d_eglInitialize(void *context, EGLDisplay dpy, EGLint *major, EGLint *minor)
{

    return d_eglInitialize_special(context, dpy, major, minor);
}

EGLBoolean d_eglQueryContext(void *context, EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value)
{

    return d_eglQueryContext_special(context, dpy, ctx, attribute, value);
}

EGLBoolean d_eglQuerySurface(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{

    return d_eglQuerySurface_special(context, dpy, surface, attribute, value);
}

const char *d_eglQueryString(void *context, EGLDisplay dpy, EGLint name)
{

    return d_eglQueryString_special(context, dpy, name);
}

__eglMustCastToProperFunctionPointerType d_eglGetProcAddress(void *context, const char *procname)
{

    return d_eglGetProcAddress_special(context, procname);
}

EGLBoolean d_eglBindAPI(void *context, EGLenum api)
{

    return d_eglBindAPI_special(context, api);
}

EGLenum d_eglQueryAPI(void *context)
{

    return d_eglQueryAPI_special(context);
}

EGLContext d_eglGetCurrentContext(void *context)
{

    return d_eglGetCurrentContext_special(context);
}

/******* file '1-2' *******/

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{

    return d_eglMakeCurrent_special(context, dpy, draw, read, ctx);
}

EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface)
{

    return d_eglSwapBuffers_sync_special(context, dpy, surface);
}

EGLBoolean d_eglWaitGL(void *context)
{

    return d_eglWaitGL_special(context);
}

EGLBoolean d_eglWaitNative(void *context, EGLint engine)
{

    return d_eglWaitNative_special(context, engine);
}

EGLBoolean d_eglWaitClient(void *context)
{

    return d_eglWaitClient_special(context);
}

EGLint d_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{

    return d_eglClientWaitSync_special(context, dpy, sync, flags, timeout);
}

EGLBoolean d_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value)
{

    return d_eglGetSyncAttrib_special(context, dpy, sync, attribute, value);
}

EGLint d_eglGetEGLConfigParam(void *context, EGLint *num_configs)
{

    return d_eglGetEGLConfigParam_special(context, num_configs);
}

EGLint d_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list)
{

    return d_eglGetEGLConfigs_special(context, num_attrs, list_len, attr_list);
}

/******* file '2-1' *******/

EGLBoolean d_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{

    return d_eglChooseConfig_special(context, dpy, attrib_list, configs, config_size, num_config);
}

EGLSurface d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface surface)
{

    return d_eglCreatePbufferSurface_special(context, dpy, config, attrib_list, surface);
}

EGLSurface d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface surface)
{

    return d_eglCreateWindowSurface_special(context, dpy, config, win, attrib_list, surface);
}

EGLContext d_eglCreateContext(void *context, EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list, EGLContext egl_context)
{

    return d_eglCreateContext_special(context, dpy, config, share_context, attrib_list, egl_context);
}

EGLBoolean d_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx)
{

    return d_eglDestroyContext_special(context, dpy, ctx);
}

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{

    return d_eglDestroySurface_special(context, dpy, surface);
}

EGLBoolean d_eglTerminate(void *context, EGLDisplay dpy)
{

    return d_eglTerminate_special(context, dpy);
}

void d_eglIamComposer(void *context, EGLSurface surface)
{

    d_eglIamComposer_special(context, surface);
}

EGLDisplay d_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display)
{

    return d_eglGetDisplay_special(context, display_id, guest_display);
}

EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{

    return d_eglBindTexImage_special(context, dpy, surface, buffer);
}

EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{

    return d_eglReleaseTexImage_special(context, dpy, surface, buffer);
}

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{

    return d_eglSurfaceAttrib_special(context, dpy, surface, attribute, value);
}

EGLBoolean d_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval)
{

    return d_eglSwapInterval_special(context, dpy, interval);
}

EGLBoolean d_eglReleaseThread(void *context)
{

    return d_eglReleaseThread_special(context);
}

EGLSync d_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list, EGLSync sync)
{

    return d_eglCreateSync_special(context, dpy, type, attrib_list, sync);
}

EGLBoolean d_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync)
{

    return d_eglDestroySync_special(context, dpy, sync);
}

EGLImage d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list, EGLImage image)
{

    return d_eglCreateImage_special(context, dpy, ctx, target, buffer, attrib_list, image);
}

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{

    return d_eglDestroyImage_special(context, dpy, image);
}

EGLBoolean d_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags)
{

    return d_eglWaitSync_special(context, dpy, sync, flags);
}

EGLClientBuffer d_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer buffer)
{

    return d_eglGetNativeClientBufferANDROID_special(context, buffer);
}

/******* file '2-2' *******/

void d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, EGLint *ret_flag)
{

    d_eglSwapBuffers_special(context, dpy, surface, ret_flag);
}

/******* file '3' *******/

EGLBoolean d_eglDestroySyncKHR(void *context, EGLDisplay dpy, EGLSyncKHR sync)
{

    return d_eglDestroySync(context, dpy, sync);
}

EGLint d_eglClientWaitSyncKHR(void *context, EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout)
{

    return d_eglClientWaitSync(context, dpy, sync, flags, timeout);
}

EGLImageKHR d_eglCreateImageKHR(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list)
{

    return d_eglCreateImage(context, dpy, ctx, target, buffer, attrib_list);
}

EGLSyncKHR d_eglCreateSyncKHR(void *context, EGLDisplay dpy, EGLenum type, const EGLint *attrib_list)
{

    return d_eglCreateSync(context, dpy, type, attrib_list);
}

EGLBoolean d_eglDestroyImageKHR(void *context, EGLDisplay dpy, EGLImageKHR img)
{

    return d_eglDestroyImage(context, dpy, img);
}

EGLBoolean d_eglGetSyncAttribKHR(void *context, EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value)
{

    return d_eglGetSyncAttrib(context, dpy, sync, attribute, value);
}
