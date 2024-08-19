

#ifndef EGL_TRANS_H
#define EGL_TRANS_H

#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/express_device_common.h"
#include "hw/express-gpu/express_gpu.h"

#define MAX_OUT_BUF_LEN 4096
#define FUNID_eglMakeCurrent ((EXPRESS_GPU_DEVICE_ID << 32u) + (((uint64_t)0x1) << 24u) + 10013)

#define PARA_NUM_MIN_eglMakeCurrent (1)

#define FUNID_eglSwapBuffers_sync ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10014)

#define PARA_NUM_MIN_eglSwapBuffers_sync (2)

#define FUNID_eglWaitGL ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10015)

#define PARA_NUM_MIN_eglWaitGL (1)

#define FUNID_eglWaitNative ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10016)

#define PARA_NUM_MIN_eglWaitNative (2)

#define FUNID_eglWaitClient ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10017)

#define PARA_NUM_MIN_eglWaitClient (1)

#define FUNID_eglClientWaitSync ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10018)

#define PARA_NUM_MIN_eglClientWaitSync (2)

#define FUNID_eglGetSyncAttrib ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10019)

#define PARA_NUM_MIN_eglGetSyncAttrib (2)

#define FUNID_eglGetEGLConfigParam ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10020)

#define PARA_NUM_MIN_eglGetEGLConfigParam (1)

#define FUNID_eglGetEGLConfigs ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10021)

#define PARA_NUM_MIN_eglGetEGLConfigs (2)

#define FUNID_eglChooseConfig ((EXPRESS_GPU_DEVICE_ID << 32u) + 10022)

#define PARA_NUM_MIN_eglChooseConfig (1)

#define FUNID_eglCreatePbufferSurface ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10023)

#define PARA_NUM_MIN_eglCreatePbufferSurface (1)

#define FUNID_eglCreateWindowSurface ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10024)

#define PARA_NUM_MIN_eglCreateWindowSurface (1)

#define FUNID_eglCreateContext ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10025)

#define PARA_NUM_MIN_eglCreateContext (1)

#define FUNID_eglDestroyContext ((EXPRESS_GPU_DEVICE_ID << 32u) + 10026)

#define PARA_NUM_MIN_eglDestroyContext (1)

#define FUNID_eglDestroySurface ((EXPRESS_GPU_DEVICE_ID << 32u) + 10027)

#define PARA_NUM_MIN_eglDestroySurface (1)

#define FUNID_eglTerminate ((EXPRESS_GPU_DEVICE_ID << 32u) + 10028)

#define PARA_NUM_MIN_eglTerminate (1)

#define FUNID_eglIamComposer ((EXPRESS_GPU_DEVICE_ID << 32u) + 10029)

#define PARA_NUM_MIN_eglIamComposer (1)

#define FUNID_eglGetDisplay ((EXPRESS_GPU_DEVICE_ID << 32u) + 10030)

#define PARA_NUM_MIN_eglGetDisplay (1)

#define FUNID_eglBindTexImage ((EXPRESS_GPU_DEVICE_ID << 32u) + 10031)

#define PARA_NUM_MIN_eglBindTexImage (1)

#define FUNID_eglReleaseTexImage ((EXPRESS_GPU_DEVICE_ID << 32u) + 10032)

#define PARA_NUM_MIN_eglReleaseTexImage (1)

#define FUNID_eglSurfaceAttrib ((EXPRESS_GPU_DEVICE_ID << 32u) + 10033)

#define PARA_NUM_MIN_eglSurfaceAttrib (1)

#define FUNID_eglSwapInterval ((EXPRESS_GPU_DEVICE_ID << 32u) + 10034)

#define PARA_NUM_MIN_eglSwapInterval (1)

#define FUNID_eglReleaseThread ((EXPRESS_GPU_DEVICE_ID << 32u) + 10035)

#define PARA_NUM_MIN_eglReleaseThread (0)

#define FUNID_eglCreateSync ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10036)

#define PARA_NUM_MIN_eglCreateSync (1)

#define FUNID_eglDestroySync ((EXPRESS_GPU_DEVICE_ID << 32u) + 10037)

#define PARA_NUM_MIN_eglDestroySync (1)

//image create的地方和使用的地方很可能不在一个线程上，所以这个要进行同步操作，同时也需要flush
#define FUNID_eglCreateImage ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10038)

#define PARA_NUM_MIN_eglCreateImage (1)

#define FUNID_eglDestroyImage ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10039)

#define PARA_NUM_MIN_eglDestroyImage (1)

#define FUNID_eglWaitSync ((EXPRESS_GPU_DEVICE_ID << 32u) + 10040)

#define PARA_NUM_MIN_eglWaitSync (1)

#define FUNID_eglGetNativeClientBufferANDROID ((EXPRESS_GPU_DEVICE_ID << 32u) + 10041)

#define PARA_NUM_MIN_eglGetNativeClientBufferANDROID (1)

#define FUNID_eglSwapBuffers ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x2) << 24u) + 10042)

#define PARA_NUM_MIN_eglSwapBuffers (3)

// #define FUNID_eglQueueBuffer ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10043)
#define FUNID_eglQueueBuffer ((EXPRESS_GPU_DEVICE_ID << 32u) + 10043)

#define PARA_NUM_MIN_eglQueueBuffer (1)

#define FUNID_eglTP ((EXPRESS_GPU_DEVICE_ID << 32u) + 10044)

#define PARA_NUM_MIN_eglTP (1)

#define FUNID_EGL_Set_Sync_Flag (DEVICE_FUN_ID(EXPRESS_GPU_DEVICE_ID, (10050)))

#define FUNID_EGL_Wait_Sync (DEVICE_FUN_ID(EXPRESS_GPU_DEVICE_ID, (10051)))

void egl_decode_invoke(Render_Thread_Context *context, Teleport_Express_Call *call);

#endif
