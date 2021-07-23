#ifndef EGL_TRANS_H
#define EGL_TRANS_H

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/offscreen_render_thread.h"
#include "express-gpu/egl_define.h"

#define MAX_OUT_BUF_LEN 4096
#define FUNID_eglSwapBuffers_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 100000)
#define FUNID_eglMakeCurrent_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 100001)
#define FUNID_eglTerminate_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 100002)
#define FUNID_getEGLConfigParam_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 100003)
#define FUNID_getEGLConfigs_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 100004)
#define FUNID_getEGLVersion_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 100005)

#define PARA_NUM_MIN_eglSwapBuffers_special (1)
#define PARA_NUM_MIN_eglMakeCurrent_special (1)
#define PARA_NUM_MIN_eglTerminate_special (1)
#define PARA_NUM_MIN_getEGLConfigParam_special (1)
#define PARA_NUM_MIN_getEGLConfigs_special (2)
#define PARA_NUM_MIN_getEGLVersion_special (1)

void egl_decode_invoke(Render_Thread_Context *context, Direct_Express_Call *call);

#endif
