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
#include "qemu/osdep.h"
#include "qemu/thread.h"
#include "direct-express/express_handle_thread.h"

#include "direct-express/express_log.h"
// #define express_printf null_printf

Direct_Express_Call *call_pop(Thread_Context *context);

/**
 * @brief 从context的环形缓冲区中pop出一个call，若没有call，则会阻塞直到下一个call到达，这个只在thread运行函数中使用
 * 
 * @param context 
 * @return Direct_Express_Call* 
 */
Direct_Express_Call *call_pop(Thread_Context *context)
{
    while (context->write_loc == context->read_loc)
    {
//缓冲区为空
// qemu_event_reset(&(context->data_event));
// qemu_event_wait(&(context->data_event));
#ifdef _WIN32
        if (context->data_event != NULL)
        {
            WaitForSingleObject(context->data_event, INFINITE);
        }
#else

#endif

        if(direct_express_should_stop)
        {
            return NULL;
        }
    }
    Direct_Express_Call *ret = context->call_buf[context->read_loc];

    context->read_loc = (context->read_loc + 1) % CALL_BUF_SIZE;

//通知已经非满
// qemu_event_set(&(context->data_event));
#ifdef _WIN32
    if (context->data_event != NULL)
    {
        SetEvent(context->data_event);
    }
#else

#endif

    // express_printf("call buf get one\n");
    return ret;
}

/**
 * @brief 将call加入到线程context的缓冲区中
 * 
 * @param context 
 * @param call 
 */
void call_push(Thread_Context *context, Direct_Express_Call *call)
{
    // express_printf("call push\n");

    while ((context->write_loc + 1) % CALL_BUF_SIZE == context->read_loc)
    {
//缓冲区为满
// qemu_event_reset(&(context->data_event));
// qemu_event_wait(&(context->data_event));
#ifdef _WIN32
        if (context->data_event != NULL)
        {
            WaitForSingleObject(context->data_event, INFINITE);
        }
#else

#endif

        if(direct_express_should_stop)
        {
            return;
        }
    }
    context->call_buf[context->write_loc] = call;

    context->write_loc = (context->write_loc + 1) % CALL_BUF_SIZE;

//通知已经非空
//qemu_event_set(&(context->data_event));
#ifdef _WIN32
    if (context->data_event != NULL)
    {
        SetEvent(context->data_event);
    }
#else

#endif

    // express_printf("call push\n");
    // express_printf("call buf set\n");
    return;
}

/**
 * @brief 处理线程运行函数，分发线程会分发call到这个线程，然后调用call_handle进行处理
 * 
 * @param opaque 
 * @return void* 
 */
void *handle_thread_run(void *opaque)
{

    Thread_Context *context = (Thread_Context *)opaque;
    // my_print(NULL);

    if (context->context_init != NULL)
    {
        context->context_init(context);
    }
    context->thread_run = 1;
    context->init = 1;
    while (context->thread_run)
    {
        Direct_Express_Call *call = call_pop(context);

        if(direct_express_should_stop)
        {
            return NULL;
        }

        if (call->is_end)
        {
            call->callback(call, 0);
            context->thread_run = 0;
            continue;
        }

        //my_print(NULL);
        // express_printf("call pop\n");
        //实际对每个call调用的操作
        if (context->call_handle != NULL)
        {
            express_printf("handle thread call handle\n");
            context->call_handle(context, call);
        }

        // decode_invoke(call);
        // if(call->id==-1){
        //     // my_print(NULL);
        //     express_printf("error return\n");
        //     return NULL;
        // }

        // call->callback(call);
    }

// qemu_event_destroy(&(context->data_event));
#ifdef _WIN32
    CloseHandle(context->data_event);
#else

#endif

    if (context->context_destroy != NULL)
    {
        context->context_destroy(context);
    }

    express_printf("handle thread exit %llu\n", context->thread_id);
    g_free(context);
    return NULL;
}
