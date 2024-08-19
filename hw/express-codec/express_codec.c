/**
 * vSoC video codec instance
 * 
 * Copyright (c) 2023 Jiaxing Qiu <jx.qiu@outlook.com>
 */

// #define STD_DEBUG_LOG

#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <limits.h> /* INT_MAX */
#include <time.h>
#include <string.h>

#include "hw/express-codec/OMX/OMX_Core.h"
#include "hw/express-mem/express_sync.h"
#include "hw/teleport-express/express_log.h"
#include "hw/express-codec/express_codec.h"
#include "hw/express-codec/dcodec_component.h"
#include "hw/express-codec/dcodec_shared.h"
#include "hw/express-codec/dcodec_video.h"
#include "hw/express-codec/dcodec_audio.h"

static GHashTable *g_codec_thread_contexts = NULL;

static const char *DCODEC_ID_MAPPING[] = {
    "Reserved",
    "InitComponent",
    "ResetComponent",
    "DestroyComponent",
    "SendCommand",
    "GetParameter",
    "SetParameter",
    "ProcessThisBuffer",
    "SetCallbacks",
};

static void dcodec_master_switch(struct Thread_Context *_context,
                          Teleport_Express_Call *call) {
    DCodecComponent *component = ((Codec_Thread_Context *)_context)->component;

    Call_Para all_para[MAX_PARA_NUM];
    OMX_ERRORTYPE error = OMX_ErrorNone;

    int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

    if (GET_FUN_ID(call->id) <= DCODEC_MAX_FUNID) {
        LOGD("unique id %" PRId64 ": function %s called with %d params!", call->unique_id,
               DCODEC_ID_MAPPING[GET_FUN_ID(call->id)], para_num);
    }
    else {
        LOGE("unique id %" PRId64 ": error! codec function id %d not recognized!", call->unique_id, GET_FUN_ID(call->id));
        return;
    }

    if (component == NULL && GET_FUN_ID(call->id) != DCODEC_FUN_InitComponent) {
        LOGE("unique id %" PRId64 ": error! function %s called on null component!", call->unique_id, DCODEC_ID_MAPPING[GET_FUN_ID(call->id)]);
        return;
    }

    switch (GET_FUN_ID(call->id)) {

    case DCODEC_FUN_InitComponent: {
        int isVideo = 0;
        int codingType = 0;

        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        isVideo = *(int *)(_ptr + _idx);
        _idx += 4;

        codingType = *(int *)(_ptr + _idx);
        _idx += 4;

        if (need_free)
            g_free(_ptr);

        if (isVideo == 0) {
            ((Codec_Thread_Context *)_context)->component = dcodec_audio_init_component(codingType, dcodec_notify_guest);
        } 
        else if (isVideo == 1) {
            ((Codec_Thread_Context *)_context)->component = dcodec_vdec_init_component(codingType, dcodec_notify_guest);
        }
        else if (isVideo == 2) {
            ((Codec_Thread_Context *)_context)->component = dcodec_venc_init_component(codingType, dcodec_notify_guest);
        }
        else {
            LOGE("error! unrecognized codec type %d", isVideo);
        }

    } break;

    case DCODEC_FUN_ResetComponent: {

        error = component->reset_component(component);

    } break;

    case DCODEC_FUN_DestroyComponent: {

        component->destroy_component(component);
        ((Codec_Thread_Context *)_context)->component = NULL;

    } break;

    case DCODEC_FUN_SendCommand: {
        OMX_COMMANDTYPE cmd;
        OMX_U32 param;
        OMX_U64 data;

        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        cmd = *(OMX_COMMANDTYPE *)(_ptr + _idx);
        _idx += 4;

        param = *(OMX_U32 *)(_ptr + _idx);
        _idx += 4;

        data = *(OMX_U64 *)(_ptr + _idx);
        _idx += 8;

        if (need_free)
            g_free(_ptr);

        error = dcodec_send_command(component, cmd, param, data);
    } break;

    case DCODEC_FUN_GetParameter: {
        OMX_INDEXTYPE index = 0;

        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        index = *(int *)(_ptr + _idx);
        _idx += 4;

        if (need_free)
            g_free(_ptr);

        uint32_t param_size = get_omx_param_size(index);
        CHECK_EQ(param_size, all_para[1].data_len);
        char params[all_para[1].data_len];

        read_from_guest_mem(all_para[1].data, params, 0, all_para[1].data_len);

        error = component->get_parameter(component, index, params);

        if (error == OMX_ErrorNone) {
            write_to_guest_mem(all_para[1].data, params, 0, param_size);
        }

    } break;

    case DCODEC_FUN_SetParameter: {
        OMX_INDEXTYPE index = 0;
        void *params = NULL;

        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        index = *(int *)(_ptr + _idx);
        _idx += 4;

        params = _ptr + _idx;
        CHECK_EQ(_idx + get_omx_param_size(index), all_para[0].data_len);

        error = component->set_parameter(component, index, params);

        if (need_free)
            g_free(_ptr);

    } break;

    case DCODEC_FUN_ProcessThisBuffer: {
        BufferDesc *desc = g_malloc0(sizeof(BufferDesc));
        read_from_guest_mem(all_para[0].data, desc, 0, all_para[0].data_len);
        if (desc->type & CODEC_BUFFER_TYPE_GUEST_MEM) {
            desc->data = copy_guest_mem_from_call(call, 2);
        }

        error = dcodec_process_this_buffer(component, desc);

    } break;

    case DCODEC_FUN_SetSync:
    {
        uint64_t sync_id;

        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        sync_id = *(uint64_t *)(_ptr + _idx);
        _idx += 8;

        if (need_free)
            g_free(_ptr);

        signal_express_sync((int)sync_id, false);
    }
    break;

    case DCODEC_FUN_WaitSync:
    {
        uint64_t sync_id;

        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        sync_id = *(uint64_t *)(_ptr + _idx);
        _idx += 8;

        if (need_free)
            g_free(_ptr);

        wait_for_express_sync((int)sync_id, false);
    }
    break;

    default: {
        LOGE("error! codec function id %d not recognized!", GET_FUN_ID(call->id));
    } break;

    }
    if (error != OMX_ErrorNone) {
        LOGE("error! host call returned error %x", error);
    }
}

static void codec_output_call_handle(struct Thread_Context *context, Teleport_Express_Call *call)
{

    Call_Para all_para[1];
    get_para_from_call(call, all_para, 1);

    dcodec_master_switch(context, call);

    call->callback(call, 1);
}

static Thread_Context *get_codec_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (g_codec_thread_contexts == NULL) // init
    {
        g_codec_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

        // list all hwaccel devices
        enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
        LOGI("Supported hw codec devices:")
        while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
            LOGI("  %s", av_hwdevice_get_type_name(type));
        }
    }

    Thread_Context *context = (Thread_Context *)g_hash_table_lookup(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id));

    // 没有context就新建线程
    if (context == NULL)
    {
        context = thread_context_create(thread_id, device_id, sizeof(Codec_Thread_Context), info);

        Codec_Thread_Context *b_context = (Codec_Thread_Context *)context;
        b_context->unique_id = unique_id;
        b_context->thread_id = thread_id;

        LOGD("codec uid %" PRId64 " create context", unique_id);
        g_hash_table_insert(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static bool remove_codec_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Codec_Thread_Context *thread_context = g_hash_table_lookup(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id));

    if (thread_context && thread_context->component && thread_context->component->dma_buf) {
        free_copied_guest_mem(thread_context->component->dma_buf);
    }

    if (thread_context && thread_context->component) {
        thread_context->component->destroy_component(thread_context->component);
        thread_context->component = NULL;
    }

    LOGD("codec uid %" PRId64 " remove context", unique_id);
    // g_hash_table_remove(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id));
    return true;
}

static Device_Context *get_codec_device_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Codec_Thread_Context *thread_context = g_hash_table_lookup(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id));
    if (thread_context == NULL)
    {
        LOGW("codec uid %" PRId64 " -> null device context", unique_id);
        return NULL;
    }
    if ((Device_Context *)thread_context->component == NULL) {
        LOGW("codec uid %" PRId64 " null component!", unique_id);
    }
    return (Device_Context *)thread_context->component;
}

static void codec_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    Codec_Thread_Context *thread_context = g_hash_table_lookup(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id));

    if (!thread_context || !thread_context->component) {
        LOGE("error! attempt to register codec dma buffer on null context!");
        return;
    }

    if (thread_context->component->dma_buf != NULL)
    {
        LOGW("codec dmabuf registered twice!");
        free_copied_guest_mem(thread_context->component->dma_buf);
    }

    LOGD("codec uid %" PRId64 " dmabuf register complete", unique_id);

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
