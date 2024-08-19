/**
 * @file express_gpu.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 这个文件是用于定义express_gpu设备的文件，包含了direct用来调用的接口和涉及到的专用结构体
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020
 *
 */

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_device_common.h"

#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/express_gpu.h"
#include "hw/express-gpu/express_gpu_render.h"
#include "hw/express-gpu/egl_draw.h"

#include "hw/express-gpu/glv3_trans.h"
#include "hw/express-gpu/egl_trans.h"
#include "hw/express-gpu/test_trans.h"
#include "hw/express-gpu/device_interface_window.h"

#include "hw/express-gpu/express_display.h"

#include "qemu/atomic.h"

// 用于保存draw线程信息的hash表，方便分发到相应的线程
static GHashTable *render_thread_contexts = NULL;

static GHashTable *render_process_contexts = NULL;

bool express_gpu_gl_debug_enable = false;
bool express_gpu_independ_window_enable = false;
bool express_device_input_window_enable = false;

static void g_surface_map_destroy(gpointer data);

static void g_context_map_destroy(gpointer data);

static void gbuffer_map_destroy(gpointer data);

/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 *
 * @param call
 */
static void decode_invoke(Thread_Context *context, Teleport_Express_Call *call)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;

    uint64_t fun_id = GET_FUN_ID(call->id);

    express_printf("enter gpu decode invoke id %llu\n", fun_id);

    if (fun_id >= 200000)
    {
        test_decode_invoke(render_context, call);
    }
    else if (fun_id > 10000)
    {
        egl_decode_invoke(render_context, call);
    }
    else if (fun_id == EXPRESS_CLUSTER_FUN_ID)
    {
        cluster_decode_invoke(call, context, (EXPRESS_DECODE_FUN)decode_invoke);
    }
    else
    {
        gl3_decode_invoke(render_context, call);
    }
    if (express_gpu_gl_debug_enable && render_context->opengl_context != NULL && render_context->opengl_context->is_current)
    {
        GLenum error_code = glGetError();
        while (error_code != GL_NO_ERROR)
        {
            LOGE("#fun_id %llu context %llx gl error %x", fun_id, (uint64_t)render_context->opengl_context, error_code);
            error_code = glGetError();
        }
    }
    return;
}

static Thread_Context *get_render_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (render_thread_contexts == NULL)
    {
        render_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

        render_process_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    Render_Thread_Context *thread_context = (Render_Thread_Context *)g_hash_table_lookup(render_thread_contexts, GUINT_TO_POINTER(thread_id));
    // 没有context就新建线程
    if (thread_context == NULL)
    {
        // express_printf("create new thread\n");
        express_printf("create new thread context\n");
        thread_context = (Render_Thread_Context *)thread_context_create(thread_id, device_id, sizeof(Render_Thread_Context), info);
        thread_context->thread_unique_ids = g_hash_table_new(g_direct_hash, g_direct_equal);

        // 处理好process_context与thread_context的关系
        // 新建进程上下文
        Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
        if (process == NULL)
        {
            express_printf("create new process context\n");
            process = g_malloc(sizeof(Process_Context));
            process->context_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_context_map_destroy);
            // 注意，从surface_map删除的时候不一定需要删除surface，所以这里为空，但是从native_window中删除却需要
            process->surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_surface_map_destroy);

            process->gbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gbuffer_map_destroy);
            process->thread_cnt = 0;

            g_hash_table_insert(render_process_contexts, GUINT_TO_POINTER(process_id), (gpointer)process);
        }
        qatomic_inc(&(process->thread_cnt));
        thread_context->process_context = process;
        g_hash_table_insert(render_thread_contexts, GUINT_TO_POINTER(thread_id), (gpointer)thread_context);
    }

    // 这里是只要是新的unique_id就都给加上，所以需要额外排除那种不是专门针对gpu device的调用
    // 例如更新显存数据时，显示指定了一次gpu device调用，但是其实其本质是display device调用，
    // 因此那种情况下，需要显示的删除该unique_id
    g_hash_table_insert(thread_context->thread_unique_ids, GUINT_TO_POINTER(unique_id), (gpointer)1);
    return (Thread_Context *)thread_context;
}

static bool remove_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *inf)
{
    Render_Thread_Context *render_context = (Render_Thread_Context *)g_hash_table_lookup(render_thread_contexts, GUINT_TO_POINTER(thread_id));

    g_hash_table_remove(render_context->thread_unique_ids, GUINT_TO_POINTER(unique_id));

    if (g_hash_table_size(render_context->thread_unique_ids) == 0)
    {
        g_hash_table_remove(render_thread_contexts, GUINT_TO_POINTER(thread_id));
        Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
        if (process != NULL)
        {
            if (process->thread_cnt == 1)
            {
                g_hash_table_remove(render_process_contexts, GUINT_TO_POINTER(process_id));
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

static void render_context_init(Thread_Context *context)
{

    express_printf("render context init!\n");
    // 这个render线程只能创建一次，且其他线程必须等待该线程运行成功
    if (qatomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        express_printf("create native window\n");
        qemu_thread_create(&native_window_render_thread, "handle_thread", native_window_thread, context->teleport_express_device, QEMU_THREAD_DETACHED);
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
    Window_Buffer *real_surface = (Window_Buffer *)data;
    express_printf("try free surface %llx\n", (uint64_t)real_surface);
    if (real_surface->is_current)
    {
        real_surface->need_destroy = 1;
    }
    else
    {
        render_surface_destroy(real_surface);
    }
}

static void g_context_map_destroy(gpointer data)
{
    Opengl_Context *real_context = (Opengl_Context *)data;
    if (real_context->is_current)
    {
        // 假如当前的context正在被使用，则需要等到context没有被使用了才能删除
        express_printf("context %llx guest %llx is using\n", (uint64_t)real_context, (uint64_t)real_context->guest_context);
        real_context->need_destroy = 1;
    }
    else
    {
        express_printf("destroy context %llx\n", (uint64_t)real_context);
        opengl_context_destroy(real_context);
        g_free(real_context);
    }
}

static void gbuffer_map_destroy(gpointer data)
{
    Hardware_Buffer *gbuffer = (Hardware_Buffer *)data;

    LOGI("destroy map gbuffer %llx type %d ptr %llx width %d height %d format %x type %d", gbuffer->gbuffer_id, gbuffer->usage_type, (uint64_t)gbuffer, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->usage_type);

    //@todo 没有context时，能不能delete sync？所以暂时让主线程去释放sync
    if (gbuffer->usage_type == GBUFFER_TYPE_TEXTURE)
    {
        if (gbuffer->data_sync != NULL)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, gbuffer->data_sync);
        }
        if (gbuffer->delete_sync != NULL)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, gbuffer->delete_sync);
        }
        g_free(gbuffer);
    }
    else
    {
        // 其他类型的gbuffer真实释放由mem线程完成
        LOGE("non_texture gbuffer in process context id %" PRIx64 " ptr %p", gbuffer->gbuffer_id, gbuffer);
    }

    return;
}

static void render_context_destroy(Thread_Context *context)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    // 这个函数的出现表示文件close了，通道都关掉了
    // 目前通道关掉只有一种可能，就是进程退出了

    g_hash_table_destroy(thread_context->thread_unique_ids);

    // 保证都不是current状态，确保能够删除成功
    if (thread_context->opengl_context != NULL)
    {
        d_eglMakeCurrent(thread_context, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
    }

    // process_context->thread_cnt -= 1;
    express_printf("process %llx destroy cnt %d\n", (uint64_t)process_context, process_context->thread_cnt);
    if (qatomic_dec_fetch(&(process_context->thread_cnt)) == 0)
    {
        // 由最后一个退出的线程清空资源
        express_printf("process %llx destroy everything\n", (uint64_t)process_context);
        g_hash_table_destroy(process_context->context_map);

        g_hash_table_destroy(process_context->surface_map);

        // image删除，这里主要是为了释放gbuffer映射
        g_hash_table_destroy(process_context->gbuffer_map);

        g_free(process_context);
    }
}

static Express_Device_Info express_gpu_info = {
    .enable_default = true,
    .name = "express-gpu",
    .option_name = "gl",
    .driver_name = "express_gpu",
    .device_id = EXPRESS_GPU_DEVICE_ID,
    .device_type = OUTPUT_DEVICE_TYPE,
    .context_init = render_context_init,
    .context_destroy = render_context_destroy,
    .call_handle = decode_invoke,
    .get_context = get_render_thread_context,
    .remove_context = remove_render_thread_context,
};

EXPRESS_DEVICE_INIT(express_gpu, &express_gpu_info)
