// #define STD_DEBUG_LOG

#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <limits.h> /* INT_MAX */
#include <time.h>
#include <string.h>

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"
#include "hw/express-codec/express_codec.h"
#include "hw/express-codec/dcodec.h"

static GHashTable *codec_thread_contexts = NULL;

static void codec_output_call_handle(struct Thread_Context *context, Teleport_Express_Call *call)
{

    Call_Para all_para[1];
    get_para_from_call(call, all_para, 1);

    dcodec_master_switch(context, call);

    call->callback(call, 1);
}

static Thread_Context *get_codec_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (codec_thread_contexts == NULL)
    {
        codec_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    Thread_Context *context = (Thread_Context *)g_hash_table_lookup(codec_thread_contexts, GUINT_TO_POINTER(unique_id));

    // 没有context就新建线程
    if (context == NULL)
    {
        express_printf("create new codec thread context\n");
        context = thread_context_create(thread_id, device_id, sizeof(Codec_Thread_Context), info);

        Codec_Thread_Context *b_context = (Codec_Thread_Context *)context;
        b_context->unique_id = unique_id;
        b_context->thread_id = thread_id;

        g_hash_table_insert(codec_thread_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static bool remove_codec_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Codec_Thread_Context *thread_context = g_hash_table_lookup(codec_thread_contexts, GUINT_TO_POINTER(unique_id));

    if (thread_context && thread_context->component && thread_context->component->dma_buf) {
        free_copied_guest_mem(thread_context->component->dma_buf);
    }

    if (thread_context && thread_context->component) {
        thread_context->component->destroy_component(thread_context->component);
        thread_context->component = NULL;
    }

    g_hash_table_remove(codec_thread_contexts, GUINT_TO_POINTER(unique_id));
    return true;
}

static Device_Context *get_codec_device_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Codec_Thread_Context *thread_context = g_hash_table_lookup(codec_thread_contexts, GUINT_TO_POINTER(unique_id));
    if (thread_context == NULL)
    {
        return NULL;
    }
    return (Device_Context *)thread_context->component;
}

static void codec_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    Codec_Thread_Context *thread_context = g_hash_table_lookup(codec_thread_contexts, GUINT_TO_POINTER(unique_id));

    if (!thread_context || !thread_context->component) {
        LOGE("error! attempt to register codec dma buffer on null context!");
        return;
    }

    if (thread_context->component->dma_buf != NULL)
    {
        LOGW("codec dmabuf registered twice!");
        free_copied_guest_mem(thread_context->component->dma_buf);
    }

    thread_context->component->dma_buf = data;
}

static Express_Device_Info express_codec_info = {
    .enable_default = true,
    .name = "express-codec",
    .option_name = "codec",
    .driver_name = "express_codec",
    .device_id = EXPRESS_CODEC_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE | OUTPUT_DEVICE_TYPE,

    .call_handle = codec_output_call_handle,
    .get_context = get_codec_context,
    .get_device_context = get_codec_device_context,
    .remove_context = remove_codec_context,
    .buffer_register = codec_buffer_register,

    .static_prop = NULL,
    .static_prop_size = 0,

};

EXPRESS_DEVICE_INIT(express_codec, &express_codec_info)
