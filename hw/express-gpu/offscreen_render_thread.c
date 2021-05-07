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
#include "express-gpu/glv3_trans.h"

#include "express-gpu/express_gpu_opengl.h"


//用于保存draw线程信息的hash表，方便分发到相应的线程
static GHashTable *render_thread_contexts=NULL;

static QemuThread render_thread;


Thread_Context *get_render_thread_context(unsigned long long type_id,unsigned long long thread_id,struct Express_Device_Info *info);
void render_context_init(Thread_Context *context);



void decode_invoke(Thread_Context *context,Direct_Express_Call *call);

/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 * 
 * @param call 
 */
void decode_invoke(Thread_Context *context,Direct_Express_Call *call)
{

    Render_Thread_Context *render_context=(Render_Thread_Context *)context;

    // if (call->fun_id == 1||call->fun_id == 0)
    // {
    //     //draw_id为0表示debug输出消息
    //     // draw_call_printf(call);
    // }
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
    // }
    express_printf("enter decode invoke\n");
    gl3_decode_invoke(render_context,call);
    // call->callback(call,1);
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


Thread_Context *get_render_thread_context(unsigned long long type_id,unsigned long long thread_id,struct Express_Device_Info *info){
    if(render_thread_contexts==NULL){
        render_thread_contexts=g_hash_table_new(g_direct_hash,g_direct_equal);
    }
    Thread_Context *context=g_hash_table_lookup(render_thread_contexts,GINT_TO_POINTER(thread_id));
    // express_printf("g_hash table lookup\n");
    //没有context就新建线程
    if(context==NULL){
        // express_printf("create new thread\n");
        express_printf("create new context thread opengl\n");
        context=thread_context_create(thread_id,type_id,sizeof(Render_Thread_Context),info);

        g_hash_table_insert(render_thread_contexts,GINT_TO_POINTER(thread_id),(gpointer)context);
    }
    return context;
}


void render_context_init(Thread_Context *context){
    if(!native_render_run){
        native_render_run=1;
        qemu_thread_create(&render_thread,"handle_thread",native_window_thread,context->direct_express_device,QEMU_THREAD_JOINABLE);
    }

    Render_Thread_Context *render_context=(Render_Thread_Context *)context;
    Opengl_Context *opengl_context = &(render_context->opengl_context);
    
    express_printf("ready to init context\n");
    context_init(opengl_context);
    express_printf("init context ok\n");


}


// void gpu_mem_map(Render_Thread_Context *render_context,GLenum target,GLintptr offset,GLsizeiptr length,GLbitfield access,void *buf){
    
//     void *gpu_mem = glMapBufferRange(target, offset, length, access);
//     memcpy(buf,gpu_mem,length);
//     g_hash_table_insert(render_context->mem_map,GINT_TO_POINTER(target),(gpointer)gpu_mem);
//     g_hash_table_insert(render_context->mem_map_len,GINT_TO_POINTER(target),(gpointer)length);

// }

// GLboolean gpu_mem_unmap(Render_Thread_Context *render_context,GLenum target,void *guest_mem){
//     void *gpu_mem=g_hash_table_lookup(render_context->mem_map,GINT_TO_POINTER(target));
//     if(gpu_mem==NULL){
//         return GL_FALSE;
//     }
//     GLsizeiptr length=g_hash_table_lookup(render_context->mem_map_len,GINT_TO_POINTER(target));
//     memcpy(gpu_mem,guest_mem,length);
//     glUnmapBuffer(target);
//     g_hash_table_remove(render_context->mem_map,GINT_TO_POINTER(target));
//     g_hash_table_remove(render_context->mem_map_len,GINT_TO_POINTER(target));

// }


// void gpu_mem_flush(Render_Thread_Context *render_context, GLenum target, GLintptr offset, GLsizeiptr length,void *guest_mem){
//     void *gpu_mem=g_hash_table_lookup(render_context->mem_map,GINT_TO_POINTER(target));
//     if(gpu_mem==NULL){
//         return;
//     }
//     GLsizeiptr all_length=g_hash_table_lookup(render_context->mem_map_len,GINT_TO_POINTER(target));
//     ssize_t write_len=0;
//     if(all_length<offset+length){
//         write_len=all_length-offset;
//     }else{
//         write_len=length;
//     }
//     memcpy(gpu_mem+offset,guest_mem,write_len);
    
//     glFlushMappedBufferRange(target, offset, length);
// }






// void *get_gpu_mem_map(Render_Thread_Context *render_context,unsigned int target){
//     return g_hash_table_lookup(render_context->mem_map,GINT_TO_POINTER(target));
// }



static Express_Device_Info express_gpu_info = {
    .name="express-gpu",
    .type_id=EXPRESS_GPU_FUN_ID,
    .context_init=render_context_init,
    .call_handle=decode_invoke,
    .get_context=get_render_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_GPU,&express_gpu_info)