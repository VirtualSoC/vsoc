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
#include "direct-express/direct_express_distribute.h"

#include "direct-express/express_log.h"

#include "express-gpu/express_gpu_opengl.h"
#include "express-gpu/offscreen_render_thread.h"

#include "express-gpu/glv3_trans.h"
#include "express-gpu/egl_trans.h"
#include "express-gpu/test_trans.h"

//用于保存draw线程信息的hash表，方便分发到相应的线程
static GHashTable *render_thread_contexts = NULL;

static QemuThread render_thread;

//这三个函数不提供外部调用接口
Thread_Context *get_render_thread_context(uint64_t type_id,uint64_t thread_id,uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);
void render_context_init(Thread_Context *context);

void decode_invoke(Thread_Context *context, Direct_Express_Call *call);

/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 * 
 * @param call 
 */
void decode_invoke(Thread_Context *context, Direct_Express_Call *call)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *buffer_context = &(render_context->render_double_buffer);
    Opengl_Context *opengl_context = &(render_context->opengl_context);

    express_printf("enter decode invoke\n");

    uint64_t fun_id = GET_FUN_ID(call->id);


    if(fun_id>=200000){
        express_printf("test decode invoke\n");

        test_decode_invoke(render_context,call);
    }
    else if(fun_id>10000){
        express_printf("egl decode invoke\n");

        egl_decode_invoke(render_context,call);
    }else{

        express_printf("gl decode invoke\n");

        if(buffer_context->has_init&&opengl_context->has_init){
            express_printf("gl3 decode invoke\n");

            gl3_decode_invoke(render_context,call);
        }else{
            call->callback(call,0);
        }
    }

    return;
}

/**
 * @brief 向
 * 
 * @param context 
 */
void real_egl_swap_buffer(Render_Thread_Context *context)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *buffer_context = &(render_context->render_double_buffer);

    //post是异步的，发送完消息后就返回了
    
    PostMessage(draw_native_window, WM_USER_PAINT, 0, (LPARAM)buffer_context );

    express_printf("real egl swap buffer\n");

    return;
}

void render_windows_create(Render_Thread_Context *context)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *buffer_context = &(render_context->render_double_buffer);
    Opengl_Context *opengl_context = &(render_context->opengl_context);

    if (!buffer_context->has_init && !opengl_context->has_init)
    {
        //send是同步的，发送完消息需要等待消息处理完
        //调用egl_context_create
        SendMessage(draw_native_window, WM_USER_CREATE, 0, (LPARAM)buffer_context);

    }

    return;
}




Thread_Context *get_render_thread_context(uint64_t type_id,uint64_t thread_id,uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info){
    if(render_thread_contexts==NULL){
        render_thread_contexts=g_hash_table_new(g_direct_hash,g_direct_equal);
    }
    Thread_Context *context = g_hash_table_lookup(render_thread_contexts, GINT_TO_POINTER(thread_id));
    // express_printf("g_hash table lookup\n");
    //没有context就新建线程
    if(context==NULL){
        // @todo 需要支持context状态转移
        // express_printf("create new thread\n");
        express_printf("create new context thread opengl\n");
        context = thread_context_create(thread_id, type_id, sizeof(Render_Thread_Context), info);

        g_hash_table_insert(render_thread_contexts, GINT_TO_POINTER(thread_id), (gpointer)context);
    }
    return context;
}

void remove_render_thread_context(uint64_t type_id, uint64_t thread_id, struct Express_Device_Info *inf)
{
    // Thread_Context *context=g_hash_table_lookup(render_thread_contexts,GINT_TO_POINTER(thread_id));
    // if(context!=NULL){
    g_hash_table_remove(render_thread_contexts, GINT_TO_POINTER(thread_id));
    // }
}

void render_context_init(Thread_Context *context)
{

    express_printf("render context init!\n");
    //这个render线程只能创建一次，且其他线程必须等待该线程运行成功
    if (atomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        express_printf("create native window\n");
        qemu_thread_create(&render_thread, "handle_thread", native_window_thread, context->direct_express_device, QEMU_THREAD_JOINABLE);
    }

    if (native_render_run == 1)
    {
        do
        {
            g_usleep(5000);
        } while (native_render_run != 2);
    }

    // Render_Thread_Context *render_context=(Render_Thread_Context *)context;
    // Opengl_Context *opengl_context = &(render_context->opengl_context);

    // express_printf("ready to init context\n");
    // opengl_context_init(opengl_context);
    // // express_printf("init context ok\n");
}

void render_context_destroy(Thread_Context *context)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Opengl_Context *opengl_context = &(render_context->opengl_context);
    Double_Buffer *double_buf = &(render_context->render_double_buffer);
    opengl_context_destroy(opengl_context);
    egl_context_destroy(double_buf);
    express_printf("render context destroy!\n");
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
    .name = "express-gpu",
    .type_id = EXPRESS_GPU_FUN_ID,
    .context_init = render_context_init,
    .context_destory = render_context_destroy,
    .call_handle = decode_invoke,
    .get_context = get_render_thread_context,
    .remove_context = remove_render_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_GPU, &express_gpu_info)