#ifndef OFFSCREEN_RENDER_THREAD_H
#define OFFSCREEN_RENDER_THREAD_H


#include "direct-express/direct_express_distribute.h"

#include "express-gpu/express_gpu_render.h"




typedef struct{
    Thread_Context context;
    Double_Buffer render_double_buffer;
} Render_Thread_Context;


#endif