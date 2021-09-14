

#ifndef DEFINE_EGL_H
#define DEFINE_EGL_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#define EXPRESS_GPU_FUN_ID ((unsigned long long)1)

#define MAX_OUT_BUF_LEN 4096

#define FUNID_eglMakeCurrent ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10013)
#define FUNID_eglSwapBuffers_sync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10014)
#define FUNID_eglWaitGL ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10015)
#define FUNID_eglWaitNative ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10016)
#define FUNID_eglWaitClient ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10017)
#define FUNID_eglClientWaitSync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10018)
#define FUNID_eglGetSyncAttrib ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10019)
#define FUNID_eglGetEGLConfigParam ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10020)
#define FUNID_eglGetEGLConfigs ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10021)
#define FUNID_eglChooseConfig ((EXPRESS_GPU_FUN_ID << 32u) + 10022)
#define FUNID_eglCreatePbufferSurface ((EXPRESS_GPU_FUN_ID << 32u) + 10023)
#define FUNID_eglCreateWindowSurface ((EXPRESS_GPU_FUN_ID << 32u) + 10024)
#define FUNID_eglCreateContext ((EXPRESS_GPU_FUN_ID << 32u) + 10025)
#define FUNID_eglDestroyContext ((EXPRESS_GPU_FUN_ID << 32u) + 10026)
#define FUNID_eglDestroySurface ((EXPRESS_GPU_FUN_ID << 32u) + 10027)
#define FUNID_eglTerminate ((EXPRESS_GPU_FUN_ID << 32u) + 10028)
#define FUNID_eglIamComposer ((EXPRESS_GPU_FUN_ID << 32u) + 10029)
#define FUNID_eglGetDisplay ((EXPRESS_GPU_FUN_ID << 32u) + 10030)
#define FUNID_eglBindTexImage ((EXPRESS_GPU_FUN_ID << 32u) + 10031)
#define FUNID_eglReleaseTexImage ((EXPRESS_GPU_FUN_ID << 32u) + 10032)
#define FUNID_eglSurfaceAttrib ((EXPRESS_GPU_FUN_ID << 32u) + 10033)
#define FUNID_eglSwapInterval ((EXPRESS_GPU_FUN_ID << 32u) + 10034)
#define FUNID_eglReleaseThread ((EXPRESS_GPU_FUN_ID << 32u) + 10035)
#define FUNID_eglCreateSync ((EXPRESS_GPU_FUN_ID << 32u) + 10036)
#define FUNID_eglDestroySync ((EXPRESS_GPU_FUN_ID << 32u) + 10037)
#define FUNID_eglCreateImage ((EXPRESS_GPU_FUN_ID << 32u) + 10038)
#define FUNID_eglDestroyImage ((EXPRESS_GPU_FUN_ID << 32u) + 10039)
#define FUNID_eglWaitSync ((EXPRESS_GPU_FUN_ID << 32u) + 10040)
#define FUNID_eglGetNativeClientBufferANDROID ((EXPRESS_GPU_FUN_ID << 32u) + 10041)
#define FUNID_eglSwapBuffers ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x2) << 24u) + 10042)

EGLBoolean d_eglGetConfigAttrib(void *context, EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value);

EGLBoolean d_eglGetConfigs(void *context, EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config);

EGLDisplay d_eglGetCurrentDisplay(void *context);

EGLSurface d_eglGetCurrentSurface(void *context, EGLint readdraw);

EGLint d_eglGetError(void *context);

EGLBoolean d_eglInitialize(void *context, EGLDisplay dpy, EGLint *major, EGLint *minor);

EGLBoolean d_eglQueryContext(void *context, EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value);

EGLBoolean d_eglQuerySurface(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);

const char *d_eglQueryString(void *context, EGLDisplay dpy, EGLint name);

__eglMustCastToProperFunctionPointerType d_eglGetProcAddress(void *context, const char *procname);

EGLBoolean d_eglBindAPI(void *context, EGLenum api);

EGLenum d_eglQueryAPI(void *context);

EGLContext d_eglGetCurrentContext(void *context);

EGLBoolean r_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id);
EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);

EGLBoolean r_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface);
EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean r_eglWaitGL(void *context);
EGLBoolean d_eglWaitGL(void *context);

EGLBoolean r_eglWaitNative(void *context, EGLint engine);
EGLBoolean d_eglWaitNative(void *context, EGLint engine);

EGLBoolean r_eglWaitClient(void *context);
EGLBoolean d_eglWaitClient(void *context);

EGLint r_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout);
EGLint d_eglClientWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout);

EGLBoolean r_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value);
EGLBoolean d_eglGetSyncAttrib(void *context, EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value);

EGLint r_eglGetEGLConfigParam(void *context, EGLint *num_configs);
EGLint d_eglGetEGLConfigParam(void *context, EGLint *num_configs);

EGLint r_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list);
EGLint d_eglGetEGLConfigs(void *context, EGLint num_attrs, EGLint list_len, EGLint *attr_list);

EGLBoolean r_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
EGLBoolean d_eglChooseConfig(void *context, EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);

EGLSurface r_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface surface);
EGLSurface d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface surface);

EGLSurface r_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface surface);
EGLSurface d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface surface);

EGLContext r_eglCreateContext(void *context, EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list, EGLContext egl_context);
EGLContext d_eglCreateContext(void *context, EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list, EGLContext egl_context);

EGLBoolean r_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx);
EGLBoolean d_eglDestroyContext(void *context, EGLDisplay dpy, EGLContext ctx);

EGLBoolean r_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface);
EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean r_eglTerminate(void *context, EGLDisplay dpy);
EGLBoolean d_eglTerminate(void *context, EGLDisplay dpy);

void r_eglIamComposer(void *context, EGLSurface surface);
void d_eglIamComposer(void *context, EGLSurface surface);

EGLDisplay r_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display);
EGLDisplay d_eglGetDisplay(void *context, EGLNativeDisplayType display_id, EGLDisplay guest_display);

EGLBoolean r_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGLBoolean r_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGLBoolean r_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);

EGLBoolean r_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval);
EGLBoolean d_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval);

EGLBoolean r_eglReleaseThread(void *context);
EGLBoolean d_eglReleaseThread(void *context);

EGLSync r_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list, EGLSync sync);
EGLSync d_eglCreateSync(void *context, EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list, EGLSync sync);

EGLBoolean r_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync);
EGLBoolean d_eglDestroySync(void *context, EGLDisplay dpy, EGLSync sync);

EGLImage r_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list, EGLImage image);
EGLImage d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list, EGLImage image);

EGLBoolean r_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);
EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);

EGLBoolean r_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags);
EGLBoolean d_eglWaitSync(void *context, EGLDisplay dpy, EGLSync sync, EGLint flags);

EGLClientBuffer r_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer buffer);
EGLClientBuffer d_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer buffer);

void r_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, EGLint *ret_flag);
void d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, EGLint *ret_flag);

EGLBoolean d_eglDestroySyncKHR(void *context, EGLDisplay dpy, EGLSyncKHR sync);

EGLint d_eglClientWaitSyncKHR(void *context, EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);

EGLImageKHR d_eglCreateImageKHR(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);

EGLSyncKHR d_eglCreateSyncKHR(void *context, EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);

EGLBoolean d_eglDestroyImageKHR(void *context, EGLDisplay dpy, EGLImageKHR img);

EGLBoolean d_eglGetSyncAttribKHR(void *context, EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value);

#endif
