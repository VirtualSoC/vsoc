#ifndef OFFSCREEN_RENDER_THREAD_H
#define OFFSCREEN_RENDER_THREAD_H


#include "direct-express/express_device_common.h"
#include "express-gpu/express_gpu_render.h"
#include "express-gpu/express_gpu_opengl.h"

typedef struct{
    Thread_Context context;
    Double_Buffer render_double_buffer;
    Opengl_Context opengl_context;

} Render_Thread_Context;

void real_egl_swapbuf(Thread_Context *context);


#endif