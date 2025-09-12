/**
 * @file express_handle_thread.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 重构后的处理线程相关的函数
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020
 *
 */

// #define STD_DEBUG_LOG
// #define THREADED_PROXY_CONTEXT

#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/teleport_express_call.h"

/**
 * @brief 从context的环形缓冲区中pop出一个call，若没有call，则会阻塞直到下一个call到达，这个只在thread运行函数中使用
 *
 * @param context
 * @return Teleport_Express_Call*
 */
void *call_pop(Thread_Context *context)
{
#ifndef THREADED_PROXY_CONTEXT
    if (context->proxy) {
        LOGE("attempt to call_pop on a proxied context!");
        return NULL;
    }
#endif

    while (context->write_loc == context->read_loc) //因为是竞争关系所以要用while循环
    {
//缓冲区为空
//注意：不要用qemu_event*相关的函数，这系列函数在并发时有bug，会导致event丢失
// qemu_event_reset(&(context->data_event));
// qemu_event_wait(&(context->data_event));
        if (context->data_event != NULL)
        {
            wait_event(context->data_event, 0xffffffff);
        }

        if (platform_should_stop())
        {
            return NULL;
        }
    }
    void *ret = context->call_buf[context->read_loc];

    if (ret == NULL) {
        LOGW("call_pop obtained empty call!");
        return NULL;
    }

    context->call_buf[context->read_loc] = NULL;

    context->read_loc = (context->read_loc + 1) % CALL_BUF_SIZE;

    // 通知已经非满
    if (context->data_event != NULL)
    {
        set_event(context->data_event);
    }

    return ret;
}

/**
 * @brief 将call加入到线程context的缓冲区中
 *
 * @param context
 * @param call
 */
void call_push(Thread_Context *context, void *call)
{
#ifndef THREADED_PROXY_CONTEXT
    if (context->proxy) {
        // proxy context, just directly call the handler
        // assuming that proxy calls are quick
        invoke_call_handler(context, call);
        return;
    }
#endif

    while ((context->write_loc + 1) % CALL_BUF_SIZE == context->read_loc)
    {
//缓冲区为满
        if (context->data_event != NULL)
        {
            wait_event(context->data_event, 0xffffffff);
        }

        if (platform_should_stop())
        {
            return;
        }
    }
    if (context->call_buf[context->write_loc] != NULL)
    {
        printf("error push find not null\n");
    }
    context->call_buf[context->write_loc] = call;

    context->write_loc = (context->write_loc + 1) % CALL_BUF_SIZE;

    //通知已经非空
    if (context->data_event != NULL)
    {
        set_event(context->data_event);
    }

    return;
}

/**
 * @brief 创建一个thread_context，并根据这个context新建一个线程
 *
 * @param context 需要初始化的线程context
 */
Thread_Context *thread_context_create(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t user_id, uint64_t len, Express_Device_Info *info)
{
    Thread_Context *context = g_malloc0(len);
    context->device_id = device_id;
    context->thread_id = thread_id;
    context->process_id = process_id;
    context->user_id = user_id;

    context->context_init = info->context_init;
    context->context_destroy = info->context_destroy;
    context->call_handler = info->call_handler;

    if (info->proxy) {
        context->proxy = true;
    }

#ifndef THREADED_PROXY_CONTEXT
    if (!context->proxy) {
#endif
        //线程缓冲区事件初始化
        context->data_event = create_event(0, 0);
    
        char thread_name[32];
        snprintf(thread_name, sizeof(thread_name), "%s_handle_thread", info->name);
    
        context->thread_run = 1;
    
        qemu_thread_create(&context->this_thread, thread_name, handle_thread_run, context, QEMU_THREAD_JOINABLE);

#ifndef THREADED_PROXY_CONTEXT
    }
#endif

    return context;
}


/**
 * @brief 解码聚合调用：从父调用的参数 all_para 重建子调用参数并依次分发
 *
 * 输入参数约定：
 *  - all_para[0] 指向 uint64_t 数组，布局为 [id, num] + 每个参数 [len, off]
 *  - all_para[1] 指向保存的参数数据 blob，off/len 均基于该 blob
 * 返回值：
 *  - true 表示解码流程正常执行（不代表每个子调用都成功），false 表示输入不合法
 */
bool cluster_decode_invoke(Thread_Context *context, const Call_Para *all_para, int para_num)
{
    if (!context || !all_para || para_num != 2) return false;
    if (!context->call_handler) return false;

    unsigned char *send_async_buf;
    int send_async_buf_len;

    unsigned char *save_buf;
    int save_buf_len;

    // 从 all_para[0] 和 all_para[1] 取出两个缓冲区

    size_t temp_len = 0;
    unsigned char *temp = NULL;

    temp_len = all_para[0].data_len;
    send_async_buf_len = temp_len;

    if (temp_len % 8 != 0)
    {
        return false;
    }

    send_async_buf = g_malloc(temp_len);

    int null_flag = 0;
    temp = get_direct_ptr(all_para[0].data, &null_flag);

    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {
            // temp = temp_buf;
            g_ops.read_from_guest_mem(all_para[0].data, send_async_buf, 0, all_para[0].data_len);
        }
        else
        {
            g_free(send_async_buf);
            return false;
        }
    }
    else
    {
        memcpy(send_async_buf, temp, temp_len);
    }

    temp_len = all_para[1].data_len;
    save_buf_len = (int)temp_len;

    save_buf = g_malloc(temp_len);

    null_flag = 0;
    temp = get_direct_ptr(all_para[1].data, &null_flag);
    if (temp == NULL)
    {
        if (temp_len != 0 && null_flag == 0)
        {
            g_ops.read_from_guest_mem(all_para[1].data, save_buf, 0, all_para[1].data_len);
        }
        else
        {
            g_free(send_async_buf);
            g_free(save_buf);
            return false;
        }
    }
    else
    {
        memcpy(save_buf, temp, temp_len);
    }

    // 直接根据聚合的描述信息构造每个子调用的参数数组，并调用真实的 decode 函数
    int buf_loc = 0;
    while (buf_loc + (int)(2 * sizeof(uint64_t)) <= send_async_buf_len)
    {
        uint64_t *hdr = (uint64_t *)(send_async_buf + buf_loc);
        uint64_t sub_id = hdr[0];
        uint64_t sub_para_num = hdr[1];

        // 用9999作为聚合调用的id，遇到则停止
        if (GET_FUN_ID(sub_id) == EXPRESS_CLUSTER_FUN_ID)
        {
            break;
        }

        if (sub_para_num > MAX_PARA_NUM)
        {
            // 非法的参数个数，停止解码
            break;
        }

        size_t desc_qwords = 2 + sub_para_num * 2; // [id, num] + N * [len, off]
        size_t need_bytes = desc_qwords * sizeof(uint64_t);
        if (buf_loc + (int)need_bytes > send_async_buf_len)
        {
            // 残缺的描述信息，停止解码
            break;
        }

        // 为本次子调用构造参数
        Scatter_Data sgs[MAX_PARA_NUM];
        Guest_Mem gms[MAX_PARA_NUM];
        Call_Para paras[MAX_PARA_NUM];

        for (uint64_t i = 0; i < sub_para_num; ++i)
        {
            uint64_t len = hdr[2 + i * 2];
            uint64_t off = hdr[2 + i * 2 + 1];

            if (off != 0 && len != 0 && (off + len) <= (uint64_t)save_buf_len)
            {
                sgs[i].iov_base = save_buf + off;
                sgs[i].iov_len = (size_t)len;
                gms[i].scatter_data = &sgs[i];
                gms[i].num = 1;
                gms[i].all_len = (int)len;
                gms[i].is_gpa = false;
                paras[i].data = &gms[i];
                paras[i].data_len = (size_t)len;
            }
            else
            {
                // 空指针或越界，视为无效参数
                sgs[i].iov_base = NULL;
                sgs[i].iov_len = 0;
                gms[i].scatter_data = &sgs[i];
                gms[i].num = 1;
                gms[i].all_len = 0;
                gms[i].is_gpa = false;
                paras[i].data = &gms[i];
                paras[i].data_len = 0;
            }
        }

        // 调用实际的设备解码函数
        context->call_handler((Thread_Context *)context, sub_id, paras, (int)sub_para_num);

        buf_loc += (int)need_bytes;
    }
    g_free(send_async_buf);
    g_free(save_buf);
    return true;
}
