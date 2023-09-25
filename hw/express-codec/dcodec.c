// #define STD_DEBUG_LOG
#include "hw/express-codec/OMX/OMX_Core.h"
#include "hw/express-codec/express_codec.h"
#include "hw/express-codec/dcodec.h"
#include "hw/express-codec/dcodec_component.h"
#include "hw/express-codec/dcodec_shared.h"
#include "hw/express-codec/dcodec_video.h"
#include "hw/express-codec/dcodec_audio.h"

#include "hw/teleport-express/express_log.h"

static char *para_to_ptr(Call_Para para, int *need_free) {
    size_t ptr_len = 0;
    unsigned char *ptr = NULL;

    ptr_len = para.data_len;

    int null_flag = 0;
    ptr = get_direct_ptr(para.data, &null_flag);
    if (unlikely(ptr == NULL)) {
        if (ptr_len != 0 && null_flag == 0) {
            ptr = g_malloc(ptr_len);
            *need_free = 1;
            read_from_guest_mem(para.data, ptr, 0, para.data_len);
        }
    }

    return ptr;
}

void dcodec_master_switch(struct Thread_Context *_context,
                          Teleport_Express_Call *call) {
    DCodecComponent *component = ((Codec_Thread_Context *)_context)->component;

    Call_Para all_para[MAX_PARA_NUM];
    OMX_ERRORTYPE error = OMX_ErrorNone;

    int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

    if (GET_FUN_ID(call->id) <= DCODEC_MAX_FUNID) {
        LOGD("function %s called!",
               DCODEC_ID_MAPPING[GET_FUN_ID(call->id)]);
    }
    else {
        LOGE("error! codec function id %d not recognized!", GET_FUN_ID(call->id));
        return;
    }

    if (component == NULL && GET_FUN_ID(call->id) != DCODEC_FUN_InitComponent) {
        LOGE("error! function %s called on null component!", DCODEC_ID_MAPPING[GET_FUN_ID(call->id)]);
        return;
    }

    switch (GET_FUN_ID(call->id)) {

    case DCODEC_FUN_InitComponent: {
        int isVideo = 0;
        int codingType = 0;

        int need_free = 0;
        char *_ptr;
        _ptr = para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        isVideo = *(int *)(_ptr + _idx);
        _idx += 4;

        codingType = *(int *)(_ptr + _idx);
        _idx += 4;

        if (need_free)
            g_free(_ptr);

        if (isVideo) {
            ((Codec_Thread_Context *)_context)->component = dcodec_video_init_component(codingType);
        }
        else {
            ((Codec_Thread_Context *)_context)->component = dcodec_audio_init_component(codingType);
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
        _ptr = para_to_ptr(all_para[0], &need_free);
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
        _ptr = para_to_ptr(all_para[0], &need_free);
        int _idx = 0;

        index = *(int *)(_ptr + _idx);
        _idx += 4;

        if (need_free)
            g_free(_ptr);

        uint32_t param_size = get_omx_param_size(index);
        CHECK_EQ(param_size, all_para[1].data_len);
        char params[param_size];

        read_from_guest_mem(all_para[1].data, params, 0, param_size);

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
        _ptr = para_to_ptr(all_para[0], &need_free);
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
        desc->data = copy_guest_mem_from_call(call, 2);

        error = dcodec_process_this_buffer(component, desc);

    } break;

    default: {
        LOGE("error! codec function id %d not recognized!", GET_FUN_ID(call->id));
    } break;

    }
    if (error != OMX_ErrorNone) {
        LOGE("error! host call returned error %d", error);
    }
}
