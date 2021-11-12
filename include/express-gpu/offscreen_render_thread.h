#ifndef OFFSCREEN_RENDER_THREAD_H
#define OFFSCREEN_RENDER_THREAD_H

#include "direct-express/express_device_common.h"
#include "express-gpu/glv3_context.h"
#include "express-gpu/egl_display.h"
#include "express-gpu/egl_surface.h"
// #include "express-gpu/express_gpu_render.h"


// #define SEND_ASYNC_BUF_SIZE 4096
// #define SAVE_BUF_SIZE 8192


typedef struct Process_Context{
    GHashTable *surface_map;
    GHashTable *context_map;
    
    GHashTable *native_window_surface_map;

    GHashTable *gbuffer_image_map;

    Resource_Map_Status *egl_sync_resource;

    int thread_cnt;
} Process_Context;

typedef struct
{
    Thread_Context context;
    Process_Context *process_context;

    //好像没见到过read和draw不一样的情况
    Window_Buffer *render_double_buffer_read;
    Window_Buffer *render_double_buffer_draw;

    Opengl_Context *opengl_context;
    Egl_Display *egl_display;
} Render_Thread_Context;

void real_egl_swap_buffer(Render_Thread_Context *context);



#endif