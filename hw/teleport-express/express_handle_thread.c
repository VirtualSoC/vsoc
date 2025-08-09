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

#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_handle_thread.h"
#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/express_event.h"

/**
 * @brief 从context的环形缓冲区中pop出一个call，若没有call，则会阻塞直到下一个call到达，这个只在thread运行函数中使用
 *
 * @param context
 * @return Teleport_Express_Call*
 */
Teleport_Express_Call *call_pop(Thread_Context *context)
{
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

        if (teleport_express_should_stop)
        {
            return NULL;
        }
    }
    Teleport_Express_Call *ret = context->call_buf[context->read_loc];

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
void call_push(Thread_Context *context, Teleport_Express_Call *call)
{
    LOGD("doing call push device id %llu thread_id %llu %08x fun id %llu unique id %08x thread run %d %d %d", context->device_id, call->thread_id, call->thread_id, call->id, call->unique_id, context->thread_run, context->write_loc, context->read_loc);
    while ((context->write_loc + 1) % CALL_BUF_SIZE == context->read_loc)
    {
//缓冲区为满
        if (context->data_event != NULL)
        {
            wait_event(context->data_event, 0xffffffff);
        }

        if (teleport_express_should_stop)
        {
            return;
        }
    }
    if (context->call_buf[context->write_loc] != NULL)
    {
        printf("error push find not null\n");
    }
    //LOGI("pushing call event id %d",call->id);
    context->call_buf[context->write_loc] = call;

    context->write_loc = (context->write_loc + 1) % CALL_BUF_SIZE;

    LOGD("finish call push device id %llu thread_id %llu %08x fun id %llu unique id %08x loc %d %d", context->device_id, call->thread_id, call->thread_id, call->id, call->unique_id, context->write_loc, context->read_loc);

    //通知已经非空
    if (context->data_event != NULL)
    {
        set_event(context->data_event);
    }

    return;
}

/**
 * @brief 处理线程运行函数，分发线程会分发call到这个线程，然后调用call_handle进行处理
 *
 * @param opaque
 * @return void*
 */
void *handle_thread_run(void *opaque) //初始化后运行的新qemu thread
{

    Thread_Context *context = (Thread_Context *)opaque;

    if (context->context_init != NULL)
    {
        context->context_init(context);
    }
    context->thread_run = 2;
    context->init = 1;
    while (context->thread_run)
    {
        Teleport_Express_Call *call = call_pop(context);

        if (teleport_express_should_stop)
        {
            break;
        }

        if (call == NULL)
        {
            continue;
        }

        if (call->is_end)
        {
            LOGD("thread context %llx call end thread_id %lld process_id %lld", (uint64_t)context, call->thread_id, call->process_id);
            call->callback(call, 0);
            context->thread_run = 0;
            break;
        }

        //实际对每个call调用的操作
        if (context->call_handle != NULL)
        {
            LOGD("handle thread call_handle devid %llu callid %llu", context->device_id, call->id);
            context->call_handle(context, call);
        }
    }

    delete_event(context->data_event);

    if (context->context_destroy != NULL)
    {
        context->context_destroy(context);
    }

    LOGD("handle thread exit %llu", context->thread_id);
    g_free(context);
    return NULL;
}
