#ifndef EXPRESS_GPU_H
#define EXPRESS_GPU_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_call.h"

#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/egl_display.h"
#include "hw/express-gpu/egl_surface.h"
// #include "hw/express-gpu/express_gpu_render.h"


// #define SEND_ASYNC_BUF_SIZE 4096
// #define SAVE_BUF_SIZE 8192


typedef struct Process_Context{
    GHashTable *surface_map;
    GHashTable *context_map;
    
    GHashTable *gbuffer_map;

    int thread_cnt;
} Process_Context;

typedef struct
{
    Thread_Context context;
    Process_Context *process_context;
    GHashTable *thread_unique_ids;

    //好像没见到过read和draw不一样的情况
    Window_Buffer *render_double_buffer_read;
    Window_Buffer *render_double_buffer_draw;

    Opengl_Context *opengl_context;
    Egl_Display *egl_display;
} Render_Thread_Context;


#define FUNID_GPU_Gbuffer_Download ((EXPRESS_GPU_DEVICE_ID << 32u) + 5001)

#define PARA_NUM_MIN_GPU_Gbuffer_Download (1)

#define FUNID_GPU_Gbuffer_Upload ((EXPRESS_GPU_DEVICE_ID << 32u) + 5002)

#define PARA_NUM_MIN_GPU_Gbuffer_Upload (1)

#define FUNID_GPU_Alloc_Gbuffer ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 25u) + 5003)

#define PARA_NUM_MIN_GPU_Alloc_Gbuffer (2)


#endif