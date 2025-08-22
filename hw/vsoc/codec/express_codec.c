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

#include "hw/vsoc/codec/OMX/OMX_Core.h"
#include "hw/vsoc/mem/express_sync.h"
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/codec/express_codec.h"
#include "hw/vsoc/codec/dcodec_component.h"
#include "hw/vsoc/codec/dcodec_shared.h"
#include "hw/vsoc/codec/dcodec_video.h"
#include "hw/vsoc/codec/dcodec_audio.h"

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

static bool dcodec_call_handler(Thread_Context *_context, uint64_t id, const Call_Para *all_para, int para_num) {
    DCodecComponent *component = ((Codec_Thread_Context *)_context)->component;
    uint64_t fun_id = GET_FUN_ID(id);
    OMX_ERRORTYPE error = OMX_ErrorNone;
    int64_t unique_id = ((Codec_Thread_Context *)_context)->unique_id;

    if (fun_id > DCODEC_MAX_FUNID) {
        LOGE("codec uid %" PRId64 ": unknown function id %llu", unique_id, (unsigned long long)fun_id);
        return false;
    }
    LOGD("codec uid %" PRId64 ": function %s called with %d params", unique_id, DCODEC_ID_MAPPING[fun_id], para_num);

    if (component == NULL && fun_id != DCODEC_FUN_InitComponent) {
        LOGE("codec uid %" PRId64 ": function %s on null component", unique_id, DCODEC_ID_MAPPING[fun_id]);
        return false;
    }

    switch (fun_id) {
    case DCODEC_FUN_InitComponent: {
        if (para_num < 1 || all_para[0].data_len < 8) return false; // need isVideo + codingType
        int need_free = 0; 
        char *_ptr = call_para_to_ptr(all_para[0], &need_free); 
        if (!_ptr) {
            if (need_free) g_free(_ptr);
            return false;
        }
        int _idx = 0; 
        int isVideo = *(int *)(_ptr + _idx); _idx += 4; 
        int codingType = *(int *)(_ptr + _idx); _idx += 4;
        if (need_free) g_free(_ptr);
        if (isVideo == 0) {
            ((Codec_Thread_Context *)_context)->component = dcodec_audio_init_component(codingType, dcodec_notify_guest);
        } else if (isVideo == 1) {
            ((Codec_Thread_Context *)_context)->component = dcodec_vdec_init_component(codingType, dcodec_notify_guest);
        } else if (isVideo == 2) {
            ((Codec_Thread_Context *)_context)->component = dcodec_venc_init_component(codingType, dcodec_notify_guest);
        } else {
            LOGE("codec uid %" PRId64 ": unrecognized codec type %d", unique_id, isVideo);
            return false;
        }
        return true;
    }
    case DCODEC_FUN_ResetComponent: {
        error = component->reset_component(component);
        return (error == OMX_ErrorNone);
    }
    case DCODEC_FUN_DestroyComponent: {
        component->destroy_component(component);
        ((Codec_Thread_Context *)_context)->component = NULL;
        return true;
    }
    case DCODEC_FUN_SendCommand: {
        if (para_num < 1 || all_para[0].data_len < (int)(4 + 4 + 8)) return false;
        int need_free = 0; 
        char *_ptr = call_para_to_ptr(all_para[0], &need_free); 
        if (!_ptr) { 
            if (need_free) g_free(_ptr);
            return false; 
        }
        int _idx = 0; 
        OMX_COMMANDTYPE cmd = *(OMX_COMMANDTYPE *)(_ptr + _idx);
        _idx += 4; 
        OMX_U32 param = *(OMX_U32 *)(_ptr + _idx);
        _idx += 4; 
        OMX_U64 data = *(OMX_U64 *)(_ptr + _idx); 
        _idx += 8; 
        if (need_free) g_free(_ptr);
        error = dcodec_send_command(component, cmd, param, data); 
        return (error == OMX_ErrorNone);
    }
    case DCODEC_FUN_GetParameter: {
        if (para_num < 2 || all_para[0].data_len < 4 || !all_para[1].data) return false;
        int need_free = 0; 
        char *_ptr = call_para_to_ptr(all_para[0], &need_free); 
        if (!_ptr) { 
            if (need_free) g_free(_ptr); 
            return false; 
        }
        int index = *(int *)_ptr; 
        if (need_free) g_free(_ptr);
        uint32_t param_size = get_omx_param_size(index); 
        if (param_size != all_para[1].data_len) { 
            LOGE("codec uid %" PRId64 ": get_parameter size mismatch %u vs %zu", unique_id, param_size, all_para[1].data_len); 
            return false; 
        }
        char *params = g_malloc(param_size);
        read_from_guest_mem(all_para[1].data, params, 0, param_size);
        error = component->get_parameter(component, index, params);
        if (error == OMX_ErrorNone) { 
            write_to_guest_mem(all_para[1].data, params, 0, param_size); 
        }
        g_free(params);
        return (error == OMX_ErrorNone);
    }
    case DCODEC_FUN_SetParameter: {
        if (para_num < 1 || all_para[0].data_len < 4) return false;
        int need_free = 0; 
        char *_ptr = call_para_to_ptr(all_para[0], &need_free); 
        if (!_ptr) { 
            if (need_free) g_free(_ptr); 
            return false; 
        }
        int _idx = 0; 
        OMX_INDEXTYPE index = *(int *)(_ptr + _idx); 
        _idx += 4; 
        size_t expected = _idx + get_omx_param_size(index); 
        if (expected != all_para[0].data_len) { 
            LOGE("codec uid %" PRId64 ": set_parameter size mismatch exp %zu got %zu", unique_id, expected, all_para[0].data_len); 
            if (need_free) g_free(_ptr); 
            return false; 
        }
        void *params = _ptr + _idx; 
        error = component->set_parameter(component, index, params); 
        if (need_free) g_free(_ptr); 
        return (error == OMX_ErrorNone);
    }
    case DCODEC_FUN_ProcessThisBuffer: {
        if (para_num < 1 || !all_para[0].data) return false;
        BufferDesc *desc = g_malloc0(sizeof(BufferDesc));
        read_from_guest_mem(all_para[0].data, desc, 0, all_para[0].data_len);
        if ((desc->type & CODEC_BUFFER_TYPE_GUEST_MEM)) {
            if (para_num < 2 || !all_para[1].data) { 
                g_free(desc); 
                return false; 
            }
            desc->data = duplicate_guest_mem(all_para[1].data);
        }
        error = dcodec_process_this_buffer(component, desc); 
        return (error == OMX_ErrorNone);
    }
    case DCODEC_FUN_SetSync: {
        if (para_num < 1 || all_para[0].data_len < 8) return false; 
        int need_free = 0; 
        char *_ptr = call_para_to_ptr(all_para[0], &need_free); 
        if(!_ptr){ 
            if(need_free) g_free(_ptr); 
            return false; 
        } 
        uint64_t sync_id = *(uint64_t *)_ptr; 
        if (need_free) g_free(_ptr); 
        signal_express_sync((int)sync_id, false); 
        return true; 
    }
    case DCODEC_FUN_WaitSync: {
        if (para_num < 1 || all_para[0].data_len < 8) return false; 
        int need_free = 0; 
        char *_ptr = call_para_to_ptr(all_para[0], &need_free); 
        if(!_ptr){ 
            if(need_free) g_free(_ptr); 
            return false; 
        } 
        uint64_t sync_id = *(uint64_t *)_ptr; 
        if (need_free) g_free(_ptr); 
        wait_for_express_sync((int)sync_id, false); 
        return true; 
    }
    default:
        LOGE("codec uid %" PRId64 ": unreachable unknown id %llu", unique_id, (unsigned long long)fun_id); return false;
    }
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

        LOGD("codec uid %" PRId64 " create context", unique_id);
        g_hash_table_insert(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static Thread_Context *remove_codec_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    Codec_Thread_Context *thread_context = (Codec_Thread_Context *)g_hash_table_lookup(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id));

    if (thread_context && thread_context->component && thread_context->component->dma_buf) {
        free_duplicated_guest_mem(thread_context->component->dma_buf);
    }

    if (thread_context && thread_context->component) {
        thread_context->component->destroy_component(thread_context->component);
        thread_context->component = NULL;
    }

    LOGD("codec uid %" PRId64 " remove context", unique_id);
    g_hash_table_remove(g_codec_thread_contexts, GUINT_TO_POINTER(unique_id));
    return (Thread_Context *)thread_context;
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
        free_duplicated_guest_mem(thread_context->component->dma_buf);
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

    .call_handler = dcodec_call_handler,
    .get_context = get_codec_context,
    .get_device_context = get_codec_device_context,
    .remove_context = remove_codec_context,
    .buffer_register = codec_buffer_register,

    .static_prop = NULL,
    .static_prop_size = 0,

};

EXPRESS_DEVICE_INIT(express_codec, &express_codec_info)
