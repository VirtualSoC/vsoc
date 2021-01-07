/**
 * @file offscreen_render_thread.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */

// #define STD_DEBUG_LOG
#include "qemu/osdep.h"
#include "qemu/thread.h"
#include "express-gpu/offscreen_render_thread.h"

#include "direct-express/express_log.h"





//用于保存draw线程信息的hash表，方便分发到相应的线程
static GHashTable *render_thread_contexts=NULL;

static int render_thread;

/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 * 
 * @param call 
 */
void decode_invoke(Direct_Express_Call *call)
{

    if (call->fun_id == 1||call->fun_id == 0)
    {
        //draw_id为0表示debug输出消息
        // draw_call_printf(call);
    }
    // else if (call->fun_id < 1000)
    // {
    //     egl_decode_invoke(call);
    // }
    // else if (call->fun_id < 2000)
    // {
    //     //opengl1.0不兼容任何东西，所以单独列出来
    //     gl1_decode_invoke(call);
    // }
    // else
    // {
    //     //由于现阶段3.0版本的opengl能兼容2.0，所以暂时先这样，出了事情再说
    //     gl3_decode_invoke(call);
    // }
    call->callback(call,1);
    return;
}



/**
 * @brief 向
 * 
 * @param context 
 */
void real_egl_swapbuf(Thread_Context *context)
{
    PostMessage(draw_native_window, WM_USER_PAINT, 0, (LPARAM)context );

    return;

}


Thread_Context *get_render_thread_context(int type_id,int thread_id,struct Express_Device_Info *info){
    if(render_thread_contexts==NULL){
        render_thread_contexts=g_hash_table_new(g_direct_hash,g_direct_equal);
    }
    Thread_Context *context=g_hash_table_lookup(render_thread_contexts,GINT_TO_POINTER(thread_id));
    // express_printf("g_hash table lookup\n");
    //没有context就新建线程
    if(context==NULL){
        // express_printf("create new thread\n");
        context=thread_context_create(thread_id,type_id,sizeof(Render_Thread_Context),info);
        g_hash_table_insert(render_thread_contexts,GINT_TO_POINTER(thread_id),(gconstpointer)context);
    }
}


void create_render_window(Thread_Context *context){
    if(!native_render_run){
        native_render_run=1;
        qemu_thread_create(&render_thread,"handle_thread",native_window_thread,context->direct_express_device,QEMU_THREAD_JOINABLE);
    }
}



static const Express_Device_Info express_gpu_info = {
    .name="express-gpu",
    .type_id=EXPRESS_GPU_FUN_ID,
    .context_init=create_render_window,
    .call_handle=decode_invoke,
    .get_context=get_render_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_GPU,&express_gpu_info)