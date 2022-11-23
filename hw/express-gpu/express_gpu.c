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

#define STD_DEBUG_LOG
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

#include "qemu/atomic.h"

//用于保存draw线程信息的hash表，方便分发到相应的线程
static GHashTable *render_thread_contexts = NULL;

static GHashTable *render_process_contexts = NULL;

static QemuThread render_thread;


bool express_gpu_gl_debug_enable = false;
bool express_gpu_independ_window_enable = false;
bool express_device_input_window_enable = false;

//这些函数不提供外部调用接口
Thread_Context *get_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);
void render_context_init(Thread_Context *context);

void decode_invoke(Thread_Context *context, Teleport_Express_Call *call);

void render_context_destroy(Thread_Context *context);

void remove_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *inf);

// void cluster_decode_invoke(Thread_Context *context, Teleport_Express_Call *call);

// void release_call_special(Teleport_Express_Call *call, int notify);

// int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Teleport_Express_Call *pre_call, Teleport_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data);

static void g_surface_map_destroy(gpointer data);

static void g_context_map_destroy(gpointer data);

static void gbuffer_map_destroy(gpointer data);

/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 *
 * @param call
 */
void decode_invoke(Thread_Context *context, Teleport_Express_Call *call)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;

    // express_printf("enter decode invoke\n");

    uint64_t fun_id = GET_FUN_ID(call->id);

    if (fun_id >= 200000)
    {
        // express_printf("test decode invoke\n");

        test_decode_invoke(render_context, call);
    }
    else if (fun_id > 10000)
    {
        // express_printf("egl decode invoke %llu\n", fun_id);

        egl_decode_invoke(render_context, call);
    }
    else if (fun_id == EXPRESS_CLUSTER_FUN_ID)
    {
        // express_printf("cluster decode invoke %llu\n", fun_id);

        cluster_decode_invoke(call, context, (EXPRESS_DECODE_FUN)decode_invoke);
    }
    else
    {

        // express_printf("gl decode invoke %llu context %llx\n", fun_id, render_context->opengl_context);
        gl3_decode_invoke(render_context, call);
    }
    if (render_context->opengl_context != NULL && express_gpu_gl_debug_enable)
    {
        GLenum error_code = glGetError();
        if (error_code != GL_NO_ERROR)
        {
            printf("#fun_id %llu context %llx get error %x\n", fun_id, (uint64_t)render_context->opengl_context, error_code);
        }
    }
    return;
}

// /**
//  * @brief 把聚合好的数据解包，分解成不同的call，用于继续调用invoke函数
//  *
//  * @param context
//  * @param call
//  */
// void cluster_decode_invoke(Thread_Context *context, Teleport_Express_Call *call)
// {
//     Call_Para all_para[MAX_PARA_NUM];

//     unsigned char *send_async_buf;
//     int send_async_buf_len;

//     unsigned char *save_buf;

//     // unsigned char temp_buf[1024];

//     //把保存的两个参数数据取出来

//     int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
//     if (para_num != 2)
//     {
//         call->callback(call, 0);
//         return;
//     }

//     size_t temp_len = 0;
//     unsigned char *temp = NULL;

//     temp_len = all_para[0].data_len;
//     send_async_buf_len = temp_len;

//     if (temp_len % 8 != 0)
//     {
//         call->callback(call, 0);
//         return;
//     }

//     send_async_buf = g_malloc(temp_len);

//     int null_flag = 0;
//     temp = get_direct_ptr(all_para[0].data, &null_flag);

//     if (temp == NULL)
//     {
//         if (temp_len != 0 && null_flag == 0)
//         {
//             // temp = temp_buf;
//             read_from_guest_mem(all_para[0].data, send_async_buf, 0, all_para[0].data_len);
//         }
//         else
//         {
//             call->callback(call, 0);
//             g_free(send_async_buf);
//             return;
//         }
//     }
//     else
//     {
//         memcpy(send_async_buf, temp, temp_len);
//     }

//     temp_len = all_para[1].data_len;

//     save_buf = g_malloc(temp_len);

//     null_flag = 0;
//     temp = get_direct_ptr(all_para[1].data, &null_flag);
//     // printf("get direct ptr %llx\n",temp);
//     if (temp == NULL)
//     {
//         if (temp_len != 0 && null_flag == 0)
//         {
//             // temp = temp_buf;
//             read_from_guest_mem(all_para[1].data, save_buf, 0, all_para[1].data_len);
//         }
//         else
//         {
//             call->callback(call, 0);
//             g_free(send_async_buf);
//             g_free(save_buf);
//             return;
//         }
//     }
//     else
//     {
//         memcpy(save_buf, temp, temp_len);
//     }

//     Teleport_Express_Call unpack_call;
//     unpack_call.vq = NULL;
//     unpack_call.vdev = NULL;
//     unpack_call.callback = release_call_special;
//     unpack_call.is_end = 0;

//     unpack_call.spend_time = 0;
//     unpack_call.next = NULL;

//     Teleport_Express_Queue_Elem pre_elem[MAX_PARA_NUM + 1];
//     Guest_Mem pre_mem[MAX_PARA_NUM + 1];
//     Scatter_Data pre_s_data[MAX_PARA_NUM + 1];

//     //依次从两个数组数据中取出数据，创建call
//     int buf_loc = 0;
//     int create_ret;
//     while (buf_loc < send_async_buf_len)
//     {
//         create_ret = create_call_from_cluster((uint64_t *)(send_async_buf + buf_loc), save_buf, &unpack_call, pre_elem, pre_mem, pre_s_data);
//         if (create_ret == 0)
//         {
//             break;
//         }
//         //解包的几个id还是原来的id
//         unpack_call.thread_id = call->thread_id;
//         unpack_call.process_id = call->process_id;
//         unpack_call.unique_id = call->unique_id;

//         buf_loc += (unpack_call.para_num * 2 + 2) * 8;
//         if (buf_loc > send_async_buf_len)
//         {
//             //防止有的call有问题
//             break;
//         }

//         decode_invoke(context, &unpack_call);
//     }
//     //所有调用完成后，这个call要回收
//     call->callback(call, 1);

//     g_free(send_async_buf);
//     g_free(save_buf);
//     return;
// }

// /**
//  * @brief 从聚合的数据中取出信息，创建一个call，用于之后调用
//  *
//  * @param send_buf 原始的发送数据
//  * @param save_buf 保存的指针数据
//  * @return
//  */
// int create_call_from_cluster(uint64_t *send_buf, unsigned char *save_buf, Teleport_Express_Call *pre_call, Teleport_Express_Queue_Elem *pre_elem, Guest_Mem *pre_guest_mem, Scatter_Data *pre_scatter_data)
// {

//     pre_call->id = send_buf[0];

//     //用9999作为聚合调用的id
//     if (GET_FUN_ID(pre_call->id) == 9999)
//     {
//         return 0;
//     }

//     pre_call->para_num = send_buf[1];
//     pre_call->elem_header = NULL;
//     // assert(pre_call->para_num < 10);
//     //第一个elem是用于存储各种id的，这个解包的call用不到。但是也得占位
//     // Teleport_Express_Queue_Elem *elem = g_malloc(sizeof(Teleport_Express_Queue_Elem));
//     pre_call->elem_header = &(pre_elem[0]);
//     Teleport_Express_Queue_Elem *last_elem = &(pre_elem[0]);
//     for (int i = 0; i < pre_call->para_num; i++)
//     {
//         //需要把这个pre_elem[i+1]中能填充的部分给填充起来

//         // Guest_Mem *guest_mem = g_malloc(sizeof(Guest_Mem));
//         // Scatter_Data *scatter_data = g_malloc(sizeof(Scatter_Data));

//         if (send_buf[i * 2 + 2 + 1] != 0)
//         {
//             pre_scatter_data[i].len = send_buf[i * 2 + 2];
//             pre_scatter_data[i].data = save_buf + send_buf[i * 2 + 2 + 1];
//         }
//         else
//         {
//             pre_scatter_data[i].len = 0;
//             pre_scatter_data[i].data = NULL;
//         }

//         pre_guest_mem[i].scatter_data = &(pre_scatter_data[i]);
//         pre_guest_mem[i].num = 1;
//         pre_guest_mem[i].all_len = pre_scatter_data[i].len;

//         pre_elem[i + 1].para = &(pre_guest_mem[i]);
//         pre_elem[i + 1].len = send_buf[i * 2 + 2];
//         pre_elem[i + 1].next = NULL;

//         last_elem->next = &(pre_elem[i + 1]);
//         last_elem = &(pre_elem[i + 1]);
//     }
//     pre_call->elem_tail = &(pre_elem[pre_call->para_num]);

//     //因为这个call是解包的call，所以不能调用原先的callback，只能调用新的callback，这个里面会释放前面申请的各种数据
//     //所以不论是vdev还是vq都用不上，不用设置来着

//     return 1;
// }

// /**
//  * @brief 用于call使用完成之后的回调
//  *
//  * @param call
//  * @param notify
//  */
// void release_call_special(Teleport_Express_Call *call, int notify)
// {
//     // Teleport_Express_Queue_Elem *elem = call->elem_header;

//     // Teleport_Express_Queue_Elem *delete_elem = elem;
//     // elem = elem->next;

//     // //第一个elem里面是空的啥也没有
//     // g_free(delete_elem);

//     // for (int i = 0; i < call->para_num; i++)
//     // {
//     //     delete_elem = elem;
//     //     Guest_Mem *guest_mem = (Guest_Mem *)elem->para;
//     //     Scatter_Data *scatter_data = guest_mem->scatter_data;

//     //     g_free(scatter_data);
//     //     g_free(guest_mem);

//     //     elem = elem->next;

//     //     g_free(delete_elem);
//     // }

//     // //最后要自己释放掉这个call，因为这个不会推送给轮询线程来释放
//     // g_free(call);
//     return;
// }

Thread_Context *get_render_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
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
        context = thread_context_create(thread_id, device_id, sizeof(Render_Thread_Context), info);
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
            process->surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_surface_map_destroy);
            // process->native_window_surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_window_surface_map_destroy);
            // process->native_window_surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
            process->gbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gbuffer_map_destroy);
            process->egl_sync_resource = g_malloc0(sizeof(Resource_Map_Status));
            process->egl_sync_resource->map_size = 0;
            process->egl_sync_resource->max_id = 0;
            process->egl_sync_resource->resource_id_map = NULL;
            process->thread_cnt = 0;

            g_hash_table_insert(render_process_contexts, GUINT_TO_POINTER(process_id), (gpointer)process);
        }
        qatomic_inc(&(process->thread_cnt));
        // process->thread_cnt += 1;
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
    if (qatomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        express_printf("create native window\n");
        qemu_thread_create(&render_thread, "handle_thread", native_window_thread, context->teleport_express_device, QEMU_THREAD_DETACHED);
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
        //假如当前的context正在被使用，则需要等到context没有被使用了才能删除
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
    Graphic_Buffer *gbuffer = (Graphic_Buffer *)data;

    printf("destroy map gbuffer %llx type %d ptr %llx width %d height %d format %x type %d\n", gbuffer->gbuffer_id, gbuffer->usage_type, (uint64_t)gbuffer, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->usage_type);

    //@todo 没有context时，能不能delete sync？所以暂时让主线程去释放sync
    if (gbuffer->usage_type == GBUFFER_TYPE_TEXTURE)
    {
        if (gbuffer->data_sync != NULL)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, gbuffer->data_sync);
            // glDeleteSync(gbuffer->data_sync);
        }
        if (gbuffer->delete_sync != NULL)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, gbuffer->delete_sync);
            // glDeleteSync(gbuffer->delete_sync);
        }
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_NONE);
        g_free(gbuffer);
    }
    else
    {
        ATOMIC_LOCK(gbuffer->is_lock);
        gbuffer->remain_life_time = (gbuffer->usage_type == GBUFFER_TYPE_BITMAP ? MAX_BITMAP_LIFE_TIME : MAX_WINDOW_LIFE_TIME);
        gbuffer->is_using = 0;
        if (gbuffer->is_dying == 0)
        {
            gbuffer->is_dying = 1;
            send_message_to_main_window(MAIN_DESTROY_GBUFFER, gbuffer);
        }
        ATOMIC_UNLOCK(gbuffer->is_lock);
    }
    // printf("send destroy gbuffer %llx message\n",gbuffer->gbuffer_id);

    return;
}

void render_context_destroy(Thread_Context *context)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    //这个函数的出现表示文件close了，通道都关掉了
    //目前通道关掉只有一种可能，就是进程退出了

    //保证都不是current状态，确保能够删除成功
    if (thread_context->opengl_context != NULL)
    {
        d_eglMakeCurrent(thread_context, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
    }
    // if (thread_context->render_double_buffer_read != NULL)
    // {
    //     thread_context->render_double_buffer_read->is_current = 0;

    //     Graphic_Buffer *old_draw_gbuffer = thread_context->render_double_buffer_draw->gbuffer;
    //     express_printf("makecurrent free draw surface %llx\n",(uint64_t)thread_context->render_double_buffer_draw);
    //     if(thread_context->render_double_buffer_draw->type == WINDOW_SURFACE && old_draw_gbuffer->gbuffer_id != gbuffer_id)
    //     {
    //         ATOMIC_LOCK(old_draw_gbuffer->is_lock);
    //         old_draw_gbuffer->remain_life_time = MAX_WINDOW_LIFE_TIME;
    //         if(old_draw_gbuffer->is_using == 0 && old_draw_gbuffer->is_dying == 0)
    //         {
    //             old_draw_gbuffer->is_dying = 1;
    //             send_message_to_main_window(MAIN_DESTROY_GBUFFER, old_draw_gbuffer);
    //         }
    //         ATOMIC_UNLOCK(old_draw_gbuffer->is_lock);
    //     }

    // }
    // if (thread_context->render_double_buffer_draw != NULL && thread_context->render_double_buffer_draw != thread_context->render_double_buffer_read)
    // {
    //     thread_context->render_double_buffer_draw->is_current = 0;
    // }
    // if (thread_context->opengl_context != NULL)
    // {
    //     express_printf("render context destroy thread %llx context %llx guest %llx when current window %llx\n", (uint64_t)thread_context, (uint64_t)thread_context->opengl_context,(uint64_t)thread_context->opengl_context->guest_context, (uint64_t)thread_context->opengl_context->window);

    //     thread_context->opengl_context->is_current = 0;
    //     g_hash_table_remove(process_context->context_map, GUINT_TO_POINTER(thread_context->opengl_context->guest_context));

    //     egl_makeCurrent(NULL);

    // }

    // process_context->thread_cnt -= 1;
    express_printf("process %llx destroy cnt %d\n", (uint64_t)process_context, process_context->thread_cnt);
    if (qatomic_dec_fetch(&(process_context->thread_cnt)) == 0)
    {
        //由最后一个退出的线程清空资源
        express_printf("process %llx destroy everything\n", (uint64_t)process_context);
        g_hash_table_destroy(process_context->context_map);

        g_hash_table_destroy(process_context->surface_map);

        // image删除，这里主要是为了释放gbuffer映射
        //  printf("destroy process context\n");
        g_hash_table_destroy(process_context->gbuffer_map);

        send_message_to_main_window(MAIN_DESTROY_ALL_EGLSYNC, process_context->egl_sync_resource);

        g_free(process_context);
    }
}

static Express_Device_Info express_gpu_info = {
    .enable_default = true,
    .name = "express-gpu",
    .option_name = "gl",
    .driver_name = "express_cdev",
    .device_id = EXPRESS_GPU_DEVICE_ID,
    .device_type = OUTPUT_DEVICE_TYPE,
    .context_init = render_context_init,
    .context_destroy = render_context_destroy,
    .call_handle = decode_invoke,
    .get_context = get_render_thread_context,
    .remove_context = remove_render_thread_context,
};

EXPRESS_DEVICE_INIT(express_gpu, &express_gpu_info)
