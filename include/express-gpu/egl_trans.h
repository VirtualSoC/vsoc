#ifndef EGL_TRANS_H
#define EGL_TRANS_H

#include "direct-express/express_device_common.h"

#include "direct-express/direct_express_distribute.h"


#include "express-gpu/offscreen_render_thread.h"

#include "express-gpu/egl_define.h"






#define FUNID_eglSwapBuffers_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x8+0x4)<<24u)+100000)
#define FUNID_eglMakeCurrent_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x8+0x4)<<24u)+100001)
#define FUNID_eglTerminate_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x8+0x4)<<24u)+100002)








#define PARA_NUM_MIN_eglSwapBuffers_special (1)
#define PARA_NUM_MIN_eglMakeCurrent_special (1)
#define PARA_NUM_MIN_eglTerminate_special (1)




void egl_decode_invoke(Render_Thread_Context *context, Direct_Express_Call *call);



// void egl_decode_invoke(Dire_Call *call);


#endif