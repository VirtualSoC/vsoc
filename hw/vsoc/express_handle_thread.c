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

#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/express_event.h"

/**
 * @brief 从context的环形缓冲区中pop出一个call，若没有call，则会阻塞直到下一个call到达，这个只在thread运行函数中使用
 *
 * @param context
 * @return Teleport_Express_Call*
 */
void *call_pop(Thread_Context *context)
{
    if (context->proxy) {
        LOGE("attempt to call_pop on a proxied context!");
        return NULL;
    }

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
    if (context->proxy) {
        // proxy context, just directly call the handler
        // assuming that proxy calls are quick
        invoke_call_handler(context, call);
        return;
    }

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

    context->context_init = info->context_init;
    context->context_destroy = info->context_destroy;
    context->call_handler = info->call_handler;

    if (info->proxy) {
        context->proxy = true;
    }

    if (!context->proxy) {
        //线程缓冲区事件初始化
        context->data_event = create_event(0, 0);
    
        char thread_name[32];
        snprintf(thread_name, sizeof(thread_name), "%s_handle_thread", info->name);
    
        context->thread_run = 1;
    
        qemu_thread_create(&context->this_thread, thread_name, handle_thread_run, context, QEMU_THREAD_JOINABLE);
    }

    return context;
}