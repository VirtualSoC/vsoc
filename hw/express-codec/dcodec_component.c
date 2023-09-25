#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <limits.h> /* INT_MAX */
#include <time.h>
#include <stdbool.h>

#undef strncpy
#include <string.h>

// #define STD_DEBUG_LOG
#include "glib.h"
#include "hw/teleport-express/teleport_express_register.h"
#include "hw/teleport-express/express_log.h"
#include "hw/express-codec/dcodec_component.h"

#include "libavutil/hwcontext.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
// #ifdef LIBAV_CONFIG_H
// #include "avtools/avconv.h"
// #else
// #include "ffmpeg.h"
// #endif

static void dcodec_process_buffers(DCodecComponent *context);
static void dcodec_av_log_callback(void *ptr, int level, const char *fmt,
                                   va_list vl);
static void sanitize(uint8_t *line);

/**
 * @brief Initializes the given dcodec component.
 * @return ERR_OK if success, or a non-zero error code on error.
 */
int dcodec_init_component(DCodecComponent *context) {
    context->input_buffers = g_queue_new();
    context->output_buffers = g_queue_new();

    av_log_set_level(AV_LOG_INFO);
    av_log_set_callback(dcodec_av_log_callback);

    AVCodecContext *mCtx = avcodec_alloc_context3(NULL);
    if (!mCtx) {
        LOGE("avcodec_alloc_context3 failed.");
        return ERR_OOM;
    }

    AVFrame *mFrame = av_frame_alloc();
    if (!mFrame) {
        LOGE("oom for AVFrame");
        return ERR_OOM;
    }

    AVPacket *mPkt = av_packet_alloc();
    if (!mPkt) {
        LOGE("oom for AVPacket");
        return ERR_OOM;
    }

    context->mCtx = mCtx;
    context->mFrame = mFrame;
    context->mPkt = mPkt;
    context->mStatus = AWAITING_INPUT;
    context->mSignalledError = false;

    return ERR_OK;
}

/**
 * @brief Resets the dcodec component to idle state.
 * @return ERR_OK if success, or a non-zero error code on error.
 */
int dcodec_reset_component(DCodecComponent *context) {
    dcodec_return_all_buffers_to_guest(context);
    if (context->mCtx && avcodec_is_open(context->mCtx)) {
        // make sure that the next buffer output does not depend on fragments from the last one decoded
        avcodec_flush_buffers(context->mCtx);
    }
    context->mStatus = AWAITING_INPUT;
    context->mSignalledError = false;

    return ERR_OK;
}

/**
 * @brief Prepares the dcodec component for deletion.
 * This function only frees internal structures; it does not free the component itself.
 */
void dcodec_deinit_component(DCodecComponent *context) {
    LOGI("dcodec_deinit_component %p", context);
    g_queue_free_full(context->input_buffers, dcodec_free_buffer_desc);
    g_queue_free_full(context->output_buffers, dcodec_free_buffer_desc);
    context->input_buffers = NULL;
    context->output_buffers = NULL;

    AVCodecContext *mCtx = context->mCtx;
    if (mCtx) {
        if (!(mCtx->extradata)) {
            av_free(mCtx->extradata);
            mCtx->extradata = NULL;
            mCtx->extradata_size = 0;
        }

        if (avcodec_is_open(mCtx)) {
            avcodec_flush_buffers(mCtx);
            avcodec_close(mCtx);
        }
        av_free(mCtx);
        context->mCtx = NULL;
    }
    if (context->mFrame) {
        av_frame_free(&context->mFrame);
        context->mFrame = NULL;
    }
    if (context->mPkt) {
        av_packet_free(&context->mPkt);
        context->mPkt = NULL;
    }
#ifdef LIBAV_CONFIG_H
#else
    if (context->mSwrCtx) {
        swr_free(&context->mSwrCtx);
        context->mSwrCtx = NULL;
    }
#endif
}

/**
 * @brief notify the guest and releases the host-side buffer descriptor.
*/
void dcodec_return_buffer_to_guest(DCodecComponent *context, BufferDesc *desc) {
    if (desc->type & CODEC_BUFFER_TYPE_INPUT) {
        dcodec_notify(context, OMX_EventEmptyBufferDone, 0, desc->nTimeStamp, desc->id, desc->nFlags);
    }
    if (desc->type & CODEC_BUFFER_TYPE_OUTPUT) {
        dcodec_notify(context, OMX_EventFillBufferDone, desc->nFilledLen, desc->nTimeStamp, desc->id, desc->nFlags);
    }
    dcodec_free_buffer_desc(desc);
}

void dcodec_return_all_buffers_to_guest(DCodecComponent *context) {
    LOGD("returning all buffers, current queue length: input %d output %d", g_queue_get_length(context->input_buffers), g_queue_get_length(context->output_buffers));

    while (!g_queue_is_empty(context->input_buffers)) {
        dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->input_buffers));
    }
    while (!g_queue_is_empty(context->output_buffers)) {
        dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->output_buffers));
    }
}

void dcodec_free_buffer_desc(void *desc) {
    BufferDesc *_desc = desc;
    if (!_desc) {
        return;
    }
    if (_desc->data) {
        free_copied_guest_mem(_desc->data);
    }
    g_free(_desc);
}

OMX_ERRORTYPE dcodec_send_command(DCodecComponent *context, OMX_COMMANDTYPE cmd, OMX_U32 param, OMX_U64 data) {
    LOGD("dcodec_send_command %x param %u data %" PRIx64 "", cmd, param, data);

    switch (cmd) {
        case OMX_CommandFlush: {
            /*  For each port that the component successfully flushes, the component shall send an 
                OMX_EventCmdComplete event, indicating OMX_CommandFlush for nData1 
                and the individual port index for nData2, even if the flush resulted from using a value 
                of OMX_ALL for nParam. */
            if (param == OMX_ALL || param == CODEC_INPUT_PORT_INDEX) {
                while (!g_queue_is_empty(context->input_buffers)) {
                    dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->input_buffers));
                }
                dcodec_notify(context, OMX_EventCmdComplete, OMX_CommandFlush, CODEC_INPUT_PORT_INDEX, 0, 0);
            }
            if (param == OMX_ALL || param == CODEC_OUTPUT_PORT_INDEX) {
                while (!g_queue_is_empty(context->output_buffers)) {
                    dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->output_buffers));
                }
                dcodec_notify(context, OMX_EventCmdComplete, OMX_CommandFlush, CODEC_OUTPUT_PORT_INDEX, 0, 0);
            }
            break;
        }
        case OMX_CommandStateSet: {
            if (param != OMX_StateIdle && param != OMX_StateLoaded) {
                LOGE("error! attempt to set component to unsupported state %x", param);
                break;
            }
            dcodec_return_all_buffers_to_guest(context);
            if (param == OMX_StateLoaded) { // further reset ffmpeg
                dcodec_reset_component(context);
            }
            dcodec_notify(context, OMX_EventCmdComplete, OMX_CommandStateSet, param, 0, 0);
            break;
        }

        default: {
            LOGE("dcodec_send_command unrecognized command %x param %u data %" PRIx64 "!", cmd, param, data);
            return OMX_ErrorBadParameter;
        }
    }
    return OMX_ErrorNone;
}

void dcodec_notify_error(DCodecComponent *context, OMX_ERRORTYPE type) {
    dcodec_notify(context, OMX_EventError, type, 0, 0, 0);
}

void dcodec_notify(DCodecComponent *context, OMX_EVENTTYPE event, OMX_U32 data1, OMX_U32 data2, OMX_U64 data, OMX_U32 flags) {
    int header[3]; // size, host_idx, guest_idx;

    while (!context->dma_buf) {
        g_usleep(1000);
        LOGD("waiting for dma_buf to be registered...");
    }

    while (true) {
        read_from_guest_mem(context->dma_buf, header, 0, 3 * sizeof(int));
        if (header[0] != sizeof(CodecDMABuffer)) {
            LOGE("error! dma buffer size does not match! host %d guest %d", sizeof(CodecDMABuffer), header[0]);
            return;
        }
        // "+ 1" must be present in the following code to avoid racing conditions
        if (header[1] - header[2] + 1 >= CODEC_CALLBACK_BUFFER_LEN) {
            LOGW("codec callback buffer full (%d), waiting for guest to empty...", header[1] - header[2]);
            g_usleep(1000);
            continue;
        }
        break;
    }

    CodecCallbackData callback;
    callback.event = event;
    callback.data1 = data1;
    callback.data2 = data2;
    callback.data = data;
    callback.flags = flags;

    LOGD("codec notify (guest idx %d host %d+1) event %x data1 %d data2 %d ptr %" PRIx64 " flags %x", header[2], header[1], event, data1, data2, data, flags);

    write_to_guest_mem(context->dma_buf, &callback, __builtin_offsetof(CodecDMABuffer, callbacks) + (header[1] % CODEC_CALLBACK_BUFFER_LEN) * sizeof(CodecCallbackData), sizeof(CodecCallbackData));
    header[1] += 1;
    write_to_guest_mem(context->dma_buf, header + 1, __builtin_offsetof(CodecDMABuffer, host_idx), sizeof(int));

    // guest-side already has polling, but polling can be laggy
    // use interrupts on important events to reduce delay
    if (event == (OMX_EVENTTYPE)OMX_EventFillBufferDone && event == OMX_EventCmdComplete) {
        set_express_device_irq((Device_Context *)context, header[1], sizeof(CodecCallbackData));
    }
}

OMX_ERRORTYPE dcodec_process_this_buffer(DCodecComponent *context, OMX_INOUT BufferDesc *desc) {

    if (context->mStatus == OUTPUT_EOS_SENT) {
        LOGW("context status is OUTPUT_EOS_SENT, buffer %" PRIx64 " will not be processed!", desc->id);
    }

    if (desc->type & CODEC_BUFFER_TYPE_INPUT) { // input buffer
        g_queue_push_tail(context->input_buffers, desc);
        LOGD("input buffer %" PRIx64 " queued, current queue length: input %d output %d", desc->id, g_queue_get_length(context->input_buffers), g_queue_get_length(context->output_buffers));
    }
    else { // output buffer
        g_queue_push_tail(context->output_buffers, desc);
        LOGD("output buffer %" PRIx64 " queued, current queue length: input %d output %d", desc->id, g_queue_get_length(context->input_buffers), g_queue_get_length(context->output_buffers));
    }

    dcodec_process_buffers(context);
    return OMX_ErrorNone;
}

static void dcodec_process_buffers(DCodecComponent *context) {
    GQueue *input_buffers = context->input_buffers;
    GQueue *output_buffers = context->output_buffers;
    bool did_something = true;
    int err = ERR_OK;

    if (context->mStatus == AWAITING_INPUT && !g_queue_is_empty(input_buffers)) {
        BufferDesc *desc = g_queue_peek_tail(input_buffers);
        if (!(desc->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
            LOGD("first data buffer received, switching to state INPUT_DATA_AVAILABLE.");
            context->mStatus = INPUT_DATA_AVAILABLE;
        }
        else {
            // wait till the first data buffer, indicating that the all the config buffers have arrived
            return;
        }
    }

    // todo: should we allow decoding to continue even if an error is signaled?
    // todo: allow decoding to continue when the user seeks back
    while ((context->mStatus == INPUT_DATA_AVAILABLE || context->mStatus == INPUT_EOS_SEEN) && did_something) {
        // if nothing is done in this iteration, break the loop and exit
        did_something = false;

        // empty one input buffer
        if (context->mStatus == INPUT_DATA_AVAILABLE && !g_queue_is_empty(input_buffers)) {
            if (context->open_decoder(context) != ERR_OK) {
                context->mStatus = ERROR_SIGNALED;
                dcodec_notify_error((DCodecComponent *)context, OMX_ErrorUndefined);
                return;
            }

            err = context->empty_one_input_buffer(context);
            if (err < ERR_OK) {
                LOGE("error %d occurred while emptying one input buffer!", err);
                context->mStatus = ERROR_SIGNALED;
                dcodec_notify_error((DCodecComponent *)context, OMX_ErrorUndefined);
                return;
            }
            if (err == ERR_OK) {
                did_something = true;
            }
        }

        // fill output buffers with decoded data
        err = ERR_OK;
        while (!g_queue_is_empty(output_buffers)) {
            err = context->fill_one_output_buffer(context);
            if (err < ERR_OK) {
                LOGE("error %d occurred while filling one output buffer!", err);
                context->mStatus = ERROR_SIGNALED;
                dcodec_notify_error((DCodecComponent *)context, OMX_ErrorUndefined);
                return;
            }
            else if (err > ERR_OK) {
                break;
            }
            did_something = true;
        }

        if (context->mStatus == INPUT_EOS_SEEN && g_queue_is_empty(input_buffers) && !g_queue_is_empty(output_buffers)) {
            context->fill_eos_output_buffer(context);
            dcodec_return_all_buffers_to_guest(context);
            context->mStatus = OUTPUT_EOS_SENT;
            LOGD("eos output buffer sent, switching to state OUTPUT_EOS_SENT.");
            did_something = true;
        }
    }
}

static void sanitize(uint8_t *line){
    while(*line){
        if(*line < 0x08 || (*line > 0x0D && *line < 0x20))
            *line='?';
        line++;
    }
}

// TODO, remove static variables to support multi-instances
static void dcodec_av_log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    static int print_prefix = 1;
    static int count;
    static char prev[1024];
    char line[1024];

    if (level > av_log_get_level())
        return;
    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);

    if (print_prefix && !strcmp(line, prev)){
        count++;
        return;
    }
    if (count > 0) {
        LOGI("Last message repeated %d times", count);
        count = 0;
    }
    strcpy(prev, line);
    sanitize((uint8_t *)line);

#if 1
    LOGI("%s", line);
#else
#define LOG_BUF_SIZE 1024
    static char g_msg[LOG_BUF_SIZE];
    static int g_msg_len = 0;

    int saw_lf, check_len;

    do {
        check_len = g_msg_len + strlen(line) + 1;
        if (check_len <= LOG_BUF_SIZE) {
            /* lf: Line feed ('\n') */
            saw_lf = (strchr(line, '\n') != NULL) ? 1 : 0;
            strncpy(g_msg + g_msg_len, line, strlen(line));
            g_msg_len += strlen(line);
            if (!saw_lf) {
               /* skip */
               return;
            } else {
               /* attach the line feed */
               g_msg_len += 1;
               g_msg[g_msg_len] = '\n';
            }
        } else {
            /* trace is fragmented */
            g_msg_len += 1;
            g_msg[g_msg_len] = '\n';
        }
        LOGI("%s", g_msg);
        /* reset g_msg and g_msg_len */
        memset(g_msg, 0, LOG_BUF_SIZE);
        g_msg_len = 0;
     } while (check_len > LOG_BUF_SIZE);
#endif
}

int dcodec_handle_extradata(DCodecComponent *context) {
    AVCodecContext *mCtx = context->mCtx;
    uint8_t *vorbisHeaderStart[3];
    int vorbisHeaderLen[3];
    BufferDesc *desc = g_queue_peek_head(context->input_buffers);

    memset(vorbisHeaderStart, 0, sizeof(vorbisHeaderStart));
    memset(vorbisHeaderLen, 0, sizeof(vorbisHeaderLen));

    while (desc && (desc->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
        if (mCtx->codec_id == AV_CODEC_ID_VORBIS) {
            uint8_t *header = (uint8_t *)av_mallocz(desc->nFilledLen);
            if (!header) {
                LOGE("error allocating memory for vorbis extradata");
                dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->input_buffers));
                return ERR_OOM;
            }

            read_from_guest_mem(desc->data, header, desc->nOffset, desc->nFilledLen);

            // vorbis header handling
            // c.f. https://xiph.org/vorbis/doc/Vorbis_I_spec.html#x1-610004.2
            int index = 0;
            if (header[0] == 1) {
                index = 0;
            } 
            else if (header[0] == 3) {
                index = 1;
            } 
            else if (header[0] == 5) {
                index = 2;
            } 
            else {
                LOGE("error! invalid vorbis extradata config index %d", header[0]);
                dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->input_buffers));
                return ERR_INVALID_PARAM;
            }
            vorbisHeaderStart[index] = header;
            vorbisHeaderLen[index] = desc->nFilledLen;
        }
        else {
            int orig_extradata_size = mCtx->extradata_size;
            mCtx->extradata_size += desc->nFilledLen;
            mCtx->extradata = (uint8_t *)av_realloc(mCtx->extradata,
                mCtx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
            if (unlikely(!mCtx->extradata)) {
                LOGE("ffmpeg failed to alloc extradata memory.");
                dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->input_buffers));
                return ERR_OOM;
            }

            read_from_guest_mem(desc->data, mCtx->extradata + orig_extradata_size, desc->nOffset, desc->nFilledLen);
            memset(mCtx->extradata + mCtx->extradata_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
            LOGD("extradata setup complete size %d", mCtx->extradata_size);
        }
        dcodec_return_buffer_to_guest(context, g_queue_pop_head(context->input_buffers));
        desc = g_queue_peek_head(context->input_buffers);
    }

    if (mCtx->codec_id == AV_CODEC_ID_VORBIS) {
        int len = vorbisHeaderLen[0] + vorbisHeaderLen[1] + vorbisHeaderLen[2];
        uint8_t *extradata = av_mallocz(len + len/255 + AV_INPUT_BUFFER_PADDING_SIZE);
        uint8_t *p = extradata;
        *p++ = 2;
        p += av_xiphlacing(p, vorbisHeaderLen[0]);
        p += av_xiphlacing(p, vorbisHeaderLen[1]);
        for (int i = 0; i < 3; i++) {
            if (vorbisHeaderLen[i] > 0) {
                memcpy(p, vorbisHeaderStart[i], vorbisHeaderLen[i]);
                p += vorbisHeaderLen[i];
            }
        }
        mCtx->extradata = extradata;
        mCtx->extradata_size = p - extradata;
        LOGD("vorbis header setup complete; length %d %d %d, total %d",
             vorbisHeaderLen[0], vorbisHeaderLen[1], vorbisHeaderLen[2],
             (int)(p - extradata));
    }

    return ERR_OK;
}
