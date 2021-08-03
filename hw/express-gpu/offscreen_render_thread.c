/**
 * @file offscreen_render_thread.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 这个文件是用于定义express_gpu设备的文件，包含了direct用来调用的接口和涉及到的专用结构体
 * @version 0.1
 * @date 2020-12-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */

// #define STD_DEBUG_LOG
#include "direct-express/direct_express_distribute.h"

#include "direct-express/express_log.h"

#include "express-gpu/glv3_context.h"
#include "express-gpu/offscreen_render_thread.h"

#include "express-gpu/glv3_trans.h"
#include "express-gpu/egl_trans.h"
#include "express-gpu/test_trans.h"

// #include "express-gpu/egl_surface.h"
// #include "express-gpu/egl_context.h"

//用于保存draw线程信息的hash表，方便分发到相应的线程
static GHashTable *render_thread_contexts = NULL;

static GHashTable *render_process_contexts = NULL;

static QemuThread render_thread;

//这些函数不提供外部调用接口
Thread_Context *get_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);
void render_context_init(Thread_Context *context);

void decode_invoke(Thread_Context *context, Direct_Express_Call *call);

void render_context_destroy(Thread_Context *context);

static void g_surface_map_destroy(gpointer data);

static void g_context_map_destroy(gpointer data);

/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 * 
 * @param call 
 */
void decode_invoke(Thread_Context *context, Direct_Express_Call *call)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;

    express_printf("enter decode invoke\n");

    uint64_t fun_id = GET_FUN_ID(call->id);

    if (fun_id >= 200000)
    {
        express_printf("test decode invoke\n");

        test_decode_invoke(render_context, call);
    }
    else if (fun_id > 10000)
    {
        express_printf("egl decode invoke %llu\n", fun_id);

        egl_decode_invoke(render_context, call);
    }
    else
    {

        express_printf("gl decode invoke %llu\n", fun_id);
        gl3_decode_invoke(render_context, call);
    }

    return;
}

Thread_Context *get_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (render_thread_contexts == NULL)
    {
        render_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

        render_process_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    Thread_Context *context = g_hash_table_lookup(render_thread_contexts, GINT_TO_POINTER(unique_id));
    // express_printf("g_hash table lookup\n");
    //没有context就新建线程
    if (context == NULL)
    {
        // express_printf("create new thread\n");
        express_printf("create new thread context\n");
        context = thread_context_create(thread_id, type_id, sizeof(Render_Thread_Context), info);
        Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
        //处理好process_context与thread_context的关系
        //新建进程上下文
        Process_Context *process = g_hash_table_lookup(render_process_contexts, GINT_TO_POINTER(process_id));
        if (process == NULL)
        {
            express_printf("create new process context\n");
            process = g_malloc(sizeof(Process_Context));
            process->context_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_context_map_destroy);
            process->surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_surface_map_destroy);
            process->egl_image_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
            process->egl_sync_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
            process->thread_cnt = 0;

            g_hash_table_insert(render_process_contexts, GINT_TO_POINTER(process_id), (gpointer)process);
        }
        process->thread_cnt += 1;
        thread_context->process_context = process;
        g_hash_table_insert(render_thread_contexts, GINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

void remove_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *inf)
{
    Process_Context *process = g_hash_table_lookup(render_process_contexts, GINT_TO_POINTER(process_id));
    if (process != NULL)
    {
        if (process->thread_cnt == 1)
        {
            g_hash_table_remove(render_process_contexts, GINT_TO_POINTER(process_id));
        }
    }

    g_hash_table_remove(render_thread_contexts, GINT_TO_POINTER(thread_id));
}

void render_context_init(Thread_Context *context)
{

    express_printf("render context init!\n");
    //这个render线程只能创建一次，且其他线程必须等待该线程运行成功
    if (atomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        express_printf("create native window\n");
        qemu_thread_create(&render_thread, "handle_thread", native_window_thread, context->direct_express_device, QEMU_THREAD_JOINABLE);
        init_display(&default_egl_display);
    }

    if (native_render_run == 1)
    {
        do
        {
            g_usleep(5000);
        } while (native_render_run != 2);
    }
}

static void g_surface_map_destroy(gpointer data)
{
    Double_Buffer *real_surface = (Double_Buffer *)data;
    render_surface_destroy(real_surface);
}

static void g_context_map_destroy(gpointer data)
{
    Opengl_Context *real_context = (Opengl_Context *)data;
    if (real_context->is_current)
    {
        real_context->need_destroy = 1;
    }
    else
    {
        //实际上是到主窗口调用opengl_context_destroy了
        PostMessage(draw_native_window, WM_USER_CONTEXT_DESTROY, 0, (LPARAM)real_context);
    }
}

void render_context_destroy(Thread_Context *context)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    glfwMakeContextCurrent(NULL);

    if (thread_context->render_double_buffer_read != NULL)
    {
        thread_context->render_double_buffer_read->is_current = 0;
    }
    if (thread_context->render_double_buffer_draw != NULL)
    {
        thread_context->render_double_buffer_draw->is_current = 0;
    }
    if (thread_context->opengl_context != NULL)
    {
        thread_context->opengl_context->is_current = 0;
    }

    g_hash_table_remove_all(process_context->context_map);
    g_hash_table_remove_all(process_context->surface_map);
    g_hash_table_remove_all(process_context->egl_image_map);
    g_hash_table_remove_all(process_context->egl_sync_map);

    process_context->thread_cnt -= 1;

    if (process_context->thread_cnt == 0)
    {
        g_hash_table_destroy(process_context->context_map);
        g_hash_table_destroy(process_context->surface_map);
        g_hash_table_destroy(process_context->egl_image_map);
        g_hash_table_destroy(process_context->egl_sync_map);
        g_free(process_context);
    }
}

static Express_Device_Info express_gpu_info = {
    .name = "express-gpu",
    .type_id = EXPRESS_GPU_FUN_ID,
    .context_init = render_context_init,
    .context_destroy = render_context_destroy,
    .call_handle = decode_invoke,
    .get_context = get_render_thread_context,
    .remove_context = remove_render_thread_context,
};

EXPRESS_DEVICE_INIT(EXPRESS_GPU, &express_gpu_info)