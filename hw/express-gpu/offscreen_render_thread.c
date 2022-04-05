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
#include "express-gpu/express_gpu_render.h"

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

void cluster_decode_invoke(Thread_Context *context, Direct_Express_Call *call);

void release_call_special(Direct_Express_Call *call, int notify);

int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Direct_Express_Call *pre_call, Direct_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data);

// static void g_window_surface_map_destroy(gpointer data);

static gboolean g_window_Surface_destroy(gpointer key, gpointer data, gpointer user_data);

static void g_p_surface_map_destroy(gpointer data);

static void g_context_map_destroy(gpointer data);

static void g_image_map_destroy(gpointer data);

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
    else if (fun_id == 9999)
    {
        express_printf("cluster decode invoke %llu\n", fun_id);

        cluster_decode_invoke(context, call);
    }
    else
    {

        express_printf("gl decode invoke %llu\n", fun_id);
        gl3_decode_invoke(render_context, call);
    }
    if (render_context->opengl_context != NULL)
    {
        // GLenum error_code = glGetError();
        // if(error_code!=GL_NO_ERROR){
        //     printf("#fun_id %llu get error %lx\n",fun_id,error_code);
        // }
    }
    return;
}

/**
 * @brief 把聚合好的数据解包，分解成不同的call，用于继续调用invoke函数
 * 
 * @param context 
 * @param call 
 */
void cluster_decode_invoke(Thread_Context *context, Direct_Express_Call *call)
{
    Call_Para all_para[MAX_PARA_NUM];

    unsigned char *send_async_buf;
    int send_async_buf_len;

    unsigned char *save_buf;

    // unsigned char temp_buf[1024];

    //把保存的两个参数数据取出来

    int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
    if (para_num != 2)
    {
        call->callback(call, 0);
        return;
    }

    size_t temp_len = 0;
    unsigned char *temp = NULL;

    temp_len = all_para[0].data_len;
    send_async_buf_len = temp_len;

    if (temp_len % 8 != 0)
    {
        call->callback(call, 0);
        return;
    }

    send_async_buf = g_malloc(temp_len);

    int null_flag = 0;
    temp = get_direct_ptr(all_para[0].data, &null_flag);

    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {
            // temp = temp_buf;
            guest_write(all_para[0].data, send_async_buf, 0, all_para[0].data_len);
        }
        else
        {
            call->callback(call, 0);
            g_free(send_async_buf);
            return;
        }
    }
    else
    {
        memcpy(send_async_buf, temp, temp_len);
    }

    temp_len = all_para[1].data_len;

    save_buf = g_malloc(temp_len);

    null_flag = 0;
    temp = get_direct_ptr(all_para[1].data, &null_flag);
    // printf("get direct ptr %llx\n",temp);
    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {
            // temp = temp_buf;
            guest_write(all_para[1].data, save_buf, 0, all_para[1].data_len);
        }
        else
        {
            call->callback(call, 0);
            g_free(send_async_buf);
            g_free(save_buf);
            return;
        }
    }
    else
    {
        memcpy(save_buf, temp, temp_len);
    }

    Direct_Express_Call unpack_call;
    unpack_call.vq = NULL;
    unpack_call.vdev = NULL;
    unpack_call.callback = release_call_special;
    unpack_call.is_end = 0;

    unpack_call.spend_time = 0;
    unpack_call.next = NULL;

    Direct_Express_Queue_Elem pre_elem[MAX_PARA_NUM + 1];
    Guest_Mem pre_mem[MAX_PARA_NUM + 1];
    Scatter_Data pre_s_data[MAX_PARA_NUM + 1];

    //依次从两个数组数据中取出数据，创建call
    int buf_loc = 0;
    int create_ret;
    while (buf_loc < send_async_buf_len)
    {
        create_ret = create_call_from_cluster((uint64_t *)(send_async_buf + buf_loc), save_buf, &unpack_call, pre_elem, pre_mem, pre_s_data);
        if (create_ret == 0)
        {
            break;
        }
        //解包的几个id还是原来的id
        unpack_call.thread_id = call->thread_id;
        unpack_call.process_id = call->process_id;
        unpack_call.unique_id = call->unique_id;

        buf_loc += (unpack_call.para_num * 2 + 2) * 8;
        if (buf_loc > send_async_buf_len)
        {
            //防止有的call有问题
            break;
        }

        decode_invoke(context, &unpack_call);
    }
    //所有调用完成后，这个call要回收
    call->callback(call, 1);

    g_free(send_async_buf);
    g_free(save_buf);
    return;
}

/**
 * @brief 从聚合的数据中取出信息，创建一个call，用于之后调用
 * 
 * @param send_buf 原始的发送数据
 * @param save_buf 保存的指针数据
 * @return  
 */
int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Direct_Express_Call *pre_call, Direct_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data)
{

    pre_call->id = send_buf[0];

    //用9999作为聚合调用的id
    if (GET_FUN_ID(pre_call->id) == 9999)
    {
        return 0;
    }

    pre_call->para_num = send_buf[1];
    pre_call->elem_header = NULL;

    //第一个elem是用于存储各种id的，这个解包的call用不到。但是也得占位
    // Direct_Express_Queue_Elem *elem = g_malloc(sizeof(Direct_Express_Queue_Elem));
    pre_call->elem_header = &(pre_elem[0]);
    Direct_Express_Queue_Elem *last_elem = &(pre_elem[0]);
    for (int i = 0; i < pre_call->para_num; i++)
    {
        //需要把这个pre_elem[i+1]中能填充的部分给填充起来

        // Guest_Mem *guest_mem = g_malloc(sizeof(Guest_Mem));
        // Scatter_Data *scatter_data = g_malloc(sizeof(Scatter_Data));

        if (send_buf[i * 2 + 2 + 1] != 0)
        {
            pre_scatter_data[i].len = send_buf[i * 2 + 2];
            pre_scatter_data[i].data = save_buf + send_buf[i * 2 + 2 + 1];
        }
        else
        {
            pre_scatter_data[i].len = 0;
            pre_scatter_data[i].data = NULL;
        }

        pre_guest_mem[i].scatter_data = &(pre_scatter_data[i]);
        pre_guest_mem[i].num = 1;
        pre_guest_mem[i].all_len = pre_scatter_data[i].len;

        pre_elem[i + 1].para = &(pre_guest_mem[i]);
        pre_elem[i + 1].len = send_buf[i * 2 + 2];
        pre_elem[i + 1].next = NULL;

        last_elem->next = &(pre_elem[i + 1]);
        last_elem = &(pre_elem[i + 1]);
    }
    pre_call->elem_tail = &(pre_elem[pre_call->para_num]);

    //因为这个call是解包的call，所以不能调用原先的callback，只能调用新的callback，这个里面会释放前面申请的各种数据
    //所以不论是vdev还是vq都用不上，不用设置来着

    return 1;
}

/**
 * @brief 用于call使用完成之后的回调
 *
 * @param call
 * @param notify
 */
void release_call_special(Direct_Express_Call *call, int notify)
{
    // Direct_Express_Queue_Elem *elem = call->elem_header;

    // Direct_Express_Queue_Elem *delete_elem = elem;
    // elem = elem->next;

    // //第一个elem里面是空的啥也没有
    // g_free(delete_elem);

    // for (int i = 0; i < call->para_num; i++)
    // {
    //     delete_elem = elem;
    //     Guest_Mem *guest_mem = (Guest_Mem *)elem->para;
    //     Scatter_Data *scatter_data = guest_mem->scatter_data;

    //     g_free(scatter_data);
    //     g_free(guest_mem);

    //     elem = elem->next;

    //     g_free(delete_elem);
    // }

    // //最后要自己释放掉这个call，因为这个不会推送给轮询线程来释放
    // g_free(call);
    return;
}

Thread_Context *get_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (render_thread_contexts == NULL)
    {
        render_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

        render_process_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    Thread_Context *context = g_hash_table_lookup(render_thread_contexts, GUINT_TO_POINTER(unique_id));
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
        Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
        if (process == NULL)
        {
            express_printf("create new process context\n");
            process = g_malloc(sizeof(Process_Context));
            process->context_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_context_map_destroy);
            //注意，从surface_map删除的时候不一定需要删除surface，所以这里为空，但是从native_window中删除却需要
            process->surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_p_surface_map_destroy);
            // process->native_window_surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_window_surface_map_destroy);
            process->native_window_surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
            process->gbuffer_image_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_image_map_destroy);
            process->egl_sync_resource = g_malloc(sizeof(Resource_Map_Status));
            process->egl_sync_resource->map_size = 0;
            process->egl_sync_resource->max_id = 0;
            process->egl_sync_resource->resource_id_map = NULL;
            process->thread_cnt = 0;

            g_hash_table_insert(render_process_contexts, GUINT_TO_POINTER(process_id), (gpointer)process);
        }
        process->thread_cnt += 1;
        thread_context->process_context = process;
        g_hash_table_insert(render_thread_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

void remove_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *inf)
{
    Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
    if (process != NULL)
    {
        if (process->thread_cnt == 1)
        {
            g_hash_table_remove(render_process_contexts, GUINT_TO_POINTER(process_id));
        }
    }

    g_hash_table_remove(render_thread_contexts, GUINT_TO_POINTER(unique_id));
}

void render_context_init(Thread_Context *context)
{

    express_printf("render context init!\n");
    //这个render线程只能创建一次，且其他线程必须等待该线程运行成功
    if (atomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        express_printf("create native window\n");
        qemu_thread_create(&render_thread, "handle_thread", native_window_thread, context->direct_express_device, QEMU_THREAD_DETACHED);
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

// static void g_window_surface_map_destroy(gpointer data)
// {
//     printf("remove window_surface %llx\n",data);
//     Window_Buffer *real_surface = (Window_Buffer *)data;
//     if (real_surface->type == WINDOW_SURFACE)
//     {
//         render_surface_destroy(real_surface);
//     }
// }

static gboolean g_window_Surface_destroy(gpointer key, gpointer data, gpointer user_data)
{
    printf("remove window_surface %llx\n",data);
    Window_Buffer *real_surface = (Window_Buffer *)data;
    if (real_surface->type == WINDOW_SURFACE)
    {
        render_surface_destroy(real_surface);
    }
    return true;
}

static void g_p_surface_map_destroy(gpointer data)
{
    Window_Buffer *real_surface = (Window_Buffer *)data;
    if (real_surface->type == P_SURFACE)
    {
        // printf("remove p_surface %llx\n",data);
        render_surface_destroy(real_surface);
    }
}

static void g_context_map_destroy(gpointer data)
{
    Opengl_Context *real_context = (Opengl_Context *)data;
    if (real_context->is_current)
    {
        //假如当前的context正在被使用，则需要等到context没有被使用了才能删除
        real_context->need_destroy = 1;
    }
    else
    {
        //实际上是到主窗口调用opengl_context_destroy了
        // PostMessage(draw_native_window, WM_USER_CONTEXT_DESTROY, 0, (LPARAM)real_context);
#ifdef USE_GLFW_AS_WGL
        // if(real_context->window != NULL)
        // {
        //     printf("destroy windows when remove\n");
        //     glfwSetWindowShouldClose(real_context->window, 1);
        //     glfwPollEvents();
        //     glfwDestroyWindow(real_context->window);
        // }
#else
        egl_destroyContext(real_context->window);
#endif
        send_message_to_main_window(MAIN_DESTROY_CONTEXT, real_context);
    }
}

static void g_image_map_destroy(gpointer data)
{
    EGL_Image *real_image = (EGL_Image *)data;
    // printf("destroy image invoke\n");
    // PostMessage(draw_native_window, WM_USER_IMAGE_DESTROY, 0, (LPARAM)real_image);
    // send_message_to_main_window(MAIN_DESTROY_IMAGE, real_image);


    if(real_image->target != EGL_GL_TEXTURE_2D)
    {
        if(real_image->fbo_texture != 0)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_TEXTURE, real_image->fbo_texture);
        }
        if(real_image->fbo_texture_reverse != 0)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_TEXTURE, real_image->fbo_texture_reverse);
        }
    }


    if (real_image->fbo_sync != NULL)
    {
        send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, real_image->fbo_sync);
    }
    if (real_image->fbo_sync_need_delete != NULL)
    {
        send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, real_image->fbo_sync_need_delete);
    }

    set_gbuffer_id_image(real_image->gbuffer_id, real_image, NULL);

    g_free(real_image);
    return;

}

void render_context_destroy(Thread_Context *context)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

#ifdef USE_GLFW_AS_WGL
    glfwMakeContextCurrent(NULL);
#else
    egl_makeCurrent(NULL);
#endif

    //保证都不是current状态，确保能够删除成功
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
        express_printf("render context destroy %llx\n", thread_context->opengl_context);
        // if(thread_context->opengl_context->window != NULL)
        // {
        //     printf("destroy windows when destroy all\n");
        //     glfwSetWindowShouldClose(thread_context->opengl_context->window, 1);
        //     glfwPollEvents();
        //     glfwDestroyWindow(thread_context->opengl_context->window);
        // }
        // thread_context->opengl_context->window = NULL;

        thread_context->opengl_context->is_current = 0;
    }

    process_context->thread_cnt -= 1;

    if (process_context->thread_cnt == 0)
    {
        // printf("process destroy everything\n");
        g_hash_table_destroy(process_context->context_map);
        //surface_map这个是删除p_surface
        g_hash_table_destroy(process_context->surface_map);
        //native的这个map是删除Window_Surface
        g_hash_table_foreach_remove(process_context->native_window_surface_map, g_window_Surface_destroy, NULL);
        g_hash_table_destroy(process_context->native_window_surface_map);

        //image删除，这里主要是为了释放gbuffer映射
        g_hash_table_destroy(process_context->gbuffer_image_map);

        send_message_to_main_window(MAIN_DESTROY_ALL_EGLSYNC, process_context->egl_sync_resource);

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