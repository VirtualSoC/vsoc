/**
 * vSoC codec device base component
 * 
 * Copyright (c) 2023 Jiaxing Qiu <jx.qiu@outlook.com>
 */


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

static void dcodec_av_log_callback(void *ptr, int level, const char *fmt,
                                   va_list vl);
static void sanitize(uint8_t *line);

/**
 * @brief Initializes the given dcodec component.
 * @return ERR_OK if success, or a non-zero error code on error.
 */
int dcodec_init_component(DCodecComponent *context, NotifyCallbackFunc notify) {
    context->input_buffers = g_queue_new();
    context->output_buffers = g_queue_new();

#ifdef STD_DEBUG_LOG
    av_log_set_level(AV_LOG_VERBOSE);
#else
    av_log_set_level(AV_LOG_ERROR);
#endif
    av_log_set_callback(dcodec_av_log_callback);

    AVCodecContext *mCtx = avcodec_alloc_context3(NULL);
    if (!mCtx) {
        LOGE("avcodec_alloc_context3 failed.");
        return ERR_OOM;
    }
    mCtx->opaque = context; // we can retrieve codec context from the opaque pointer

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

    if (notify != NULL) {
        context->notify = notify;
    }
    else {
        context->notify = dcodec_notify_null;
    }

    return ERR_OK;
}

/**
 * @brief resets the codec component to idle state
 * @return ERR_OK if success, or a non-zero error code on error.
 */
int dcodec_reset_component(DCodecComponent *context) {
    if (context->mCtx && avcodec_is_open(context->mCtx)) {
        // make sure that the next buffer output does not depend on fragments from the last one decoded
        avcodec_flush_buffers(context->mCtx);
    }
    context->mStatus = AWAITING_INPUT;

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
        if (mCtx->extradata) {
            av_free(mCtx->extradata);
            mCtx->extradata = NULL;
            mCtx->extradata_size = 0;
        }
        if (mCtx->hw_device_ctx) {
            av_buffer_unref(&mCtx->hw_device_ctx);
        }
        if (avcodec_is_open(mCtx)) {
            avcodec_flush_buffers(mCtx);
            avcodec_close(mCtx);
        }
        avcodec_free_context(&mCtx);
        context->mCtx = NULL;
    }
    if (context->mFrame) {
        av_frame_free(&context->mFrame);
    }
    if (context->mPkt) {
        av_packet_free(&context->mPkt);
    }
}

/**
 * @brief notify the guest and releases the host-side buffer descriptor.
*/
void dcodec_return_buffer(DCodecComponent *context, BufferDesc *desc) {
    if (desc->type & CODEC_BUFFER_TYPE_INPUT) {
        context->notify(context, (CodecCallbackData){ .event = OMX_EventEmptyBufferDone, .data1 = 0, .data2 = desc->nTimeStamp, .data = desc->id, .flags = desc->nFlags });
    }
    if (desc->type & CODEC_BUFFER_TYPE_OUTPUT) {
        context->notify(context, (CodecCallbackData){ .event = OMX_EventFillBufferDone, .data1 = desc->nFilledLen, .data2 = desc->nTimeStamp, .data = desc->id, .flags = desc->nFlags });
    }
    dcodec_free_buffer_desc(desc);
}

/**
 * Returns all the buffers in the queue specified.
 * if (type & CODEC_BUFFER_TYPE_INPUT), flushes all input buffers;
 * if (type & CODEC_BUFFER_TYPE_OUTPUT), flushes all output buffers.
 */
void dcodec_flush_buffers(DCodecComponent *context, int type) {
    if (type & CODEC_BUFFER_TYPE_INPUT) {
        while (!g_queue_is_empty(context->input_buffers)) {
            dcodec_return_buffer(context, g_queue_pop_head(context->input_buffers));
        }
    }
    if (type & CODEC_BUFFER_TYPE_OUTPUT) {
        while (!g_queue_is_empty(context->output_buffers)) {
            dcodec_return_buffer(context, g_queue_pop_head(context->output_buffers));
        }
    }
}

void dcodec_free_buffer_desc(void *_desc) {
    BufferDesc *desc = _desc;
    if (!desc) {
        return;
    }
    if (desc->data && (desc->type & CODEC_BUFFER_TYPE_GUEST_MEM)) {
        free_copied_guest_mem(desc->data);
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
                dcodec_flush_buffers(context, CODEC_BUFFER_TYPE_INPUT);
                dcodec_reset_component(context); // flush codec in case the user seeks 
                context->notify(context, (CodecCallbackData){ .event = OMX_EventCmdComplete, .data1 = OMX_CommandFlush, .data2 = CODEC_INPUT_PORT_INDEX });
            }
            if (param == OMX_ALL || param == CODEC_OUTPUT_PORT_INDEX) {
                dcodec_flush_buffers(context, CODEC_BUFFER_TYPE_OUTPUT);
                context->notify(context, (CodecCallbackData) { .event = OMX_EventCmdComplete, .data1 = OMX_CommandFlush, .data2 = CODEC_OUTPUT_PORT_INDEX });
            }
            break;
        }
        case OMX_CommandStateSet: {
            if (param != OMX_StateIdle && param != OMX_StateExecuting) {
                LOGE("error! attempt to set component to unsupported state %x", param);
                break;
            }
            if (param == OMX_StateExecuting) {
                // for now, nothing needs to be done
                break;
            }
            dcodec_flush_buffers(context, CODEC_BUFFER_TYPE_INPUT | CODEC_BUFFER_TYPE_OUTPUT);
            dcodec_reset_component(context);
            context->notify(context, (CodecCallbackData) { .event = OMX_EventCmdComplete, .data1 = OMX_CommandStateSet, .data2 = param });
            break;
        }

        default: {
            LOGE("dcodec_send_command unrecognized command %x param %u data %" PRIx64 "!", cmd, param, data);
            return OMX_ErrorBadParameter;
        }
    }
    return OMX_ErrorNone;
}

/**
 * convenient wrapper for throwing an error to the client 
*/
void dcodec_notify_error(DCodecComponent *context, OMX_ERRORTYPE type) {
    context->notify(context, (CodecCallbackData){ .event = OMX_EventError, .data1 = type } );
}

/**
 * the default callback handler for a decoder without any callbacks specified
 * it just prints a debug message and exits
*/
void dcodec_notify_null(DCodecComponent *context, CodecCallbackData ccd) {
    LOGD("codec null notify event %x data1 %d data2 %d ptr %" PRIx64 " flags %x extra %u", ccd.event, ccd.data1, ccd.data2, ccd.data, ccd.flags, ccd.extra);
}

/**
 * the default callback handler for a guest-initiated decoder
 * notifies the guest of the event
*/
void dcodec_notify_guest(DCodecComponent *context, CodecCallbackData ccd) {
    int header[3]; // size, host_idx, guest_idx;

    if (!context->dma_buf) {
        LOGE("dcodec_notify on null dma_buf!");
        return;
    }

    g_mutex_lock(&context->dma_buf_mutex);

    while (true) {
        read_from_guest_mem(context->dma_buf, header, 0, 3 * sizeof(int));
        if (header[0] != sizeof(CodecDMABuffer)) {
            LOGE("error! dma buffer size does not match! host %llu guest %d", sizeof(CodecDMABuffer), header[0]);
            g_mutex_unlock(&context->dma_buf_mutex);
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

    LOGD("codec notify (guest idx %d host %d+1) event %x data1 %d data2 %d ptr %" PRIx64 " flags %x extra %u", header[2], header[1], ccd.event, ccd.data1, ccd.data2, ccd.data, ccd.flags, ccd.extra);

    write_to_guest_mem(context->dma_buf, &ccd, __builtin_offsetof(CodecDMABuffer, callbacks) + (header[1] % CODEC_CALLBACK_BUFFER_LEN) * sizeof(CodecCallbackData), sizeof(CodecCallbackData));
    header[1] += 1;
    write_to_guest_mem(context->dma_buf, header + 1, __builtin_offsetof(CodecDMABuffer, host_idx), sizeof(int));
    g_mutex_unlock(&context->dma_buf_mutex);

    // guest-side already has polling, but polling can be laggy
    // use interrupts on important events to reduce delay
    if (ccd.event == OMX_EventCmdComplete) {
        set_express_device_irq((Device_Context *)context, header[1], sizeof(CodecCallbackData));
    }
}

OMX_ERRORTYPE dcodec_process_this_buffer(DCodecComponent *context, OMX_INOUT BufferDesc *desc) {

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

void dcodec_process_buffers(DCodecComponent *context) {
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

    while ((context->mStatus != AWAITING_INPUT && context->mStatus != ERROR_SIGNALED) && did_something) {
        // if nothing is done in this iteration, break the loop and exit
        did_something = false;

        // empty one input buffer
        if (!g_queue_is_empty(input_buffers)) {
            if (context->open_codec(context) != ERR_OK) {
                context->mStatus = ERROR_SIGNALED;
                dcodec_notify_error((DCodecComponent *)context, OMX_ErrorUndefined);
                break;
            }

            err = context->empty_one_input_buffer(context);
            if (err < ERR_OK) {
                LOGE("error %d occurred while emptying one input buffer!", err);
                context->mStatus = ERROR_SIGNALED;
                dcodec_notify_error((DCodecComponent *)context, OMX_ErrorUndefined);
                break;
            }
            else if (err == ERR_OK) {
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

        // in the following cases, it is possible that we will not receive any more buffers
        // therefore we have to wait and fill the buffers to avoid deadlock
        // because of the single-threaded nature of the decoder
        // case 1: slow decoder, input/output queue are both full
        // case 2: eos frame has not finished decoding yet
        if (g_queue_get_length(output_buffers) != 0 && 
           (g_queue_get_length(input_buffers) != 0 || context->mStatus == INPUT_EOS_SEEN)) {
            g_usleep(1000);
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

    uint8_t *extra_buf = av_mallocz(1);
    int extra_bufsize = 0;
    while (desc && (desc->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
        CHECK(desc->type & CODEC_BUFFER_TYPE_GUEST_MEM);

        if (mCtx->codec_id == AV_CODEC_ID_VORBIS) {
            uint8_t *header = (uint8_t *)av_malloc(desc->nFilledLen);
            if (!header) {
                LOGE("error allocating memory for vorbis extradata");
                dcodec_return_buffer(context, g_queue_pop_head(context->input_buffers));
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
                dcodec_return_buffer(context, g_queue_pop_head(context->input_buffers));
                return ERR_INVALID_PARAM;
            }
            vorbisHeaderStart[index] = header;
            vorbisHeaderLen[index] = desc->nFilledLen;
        }
        else {
            extra_buf = av_realloc(extra_buf, extra_bufsize + desc->nFilledLen);
            read_from_guest_mem(desc->data, extra_buf + extra_bufsize, 0, desc->nFilledLen);
            extra_bufsize = extra_bufsize + desc->nFilledLen;
            mCtx->extradata_size = extra_bufsize;
            mCtx->extradata = extra_buf;
        }
        dcodec_return_buffer(context, g_queue_pop_head(context->input_buffers));
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

void dcodec_fill_eos_output_buffer(DCodecComponent *_context) {
    BufferDesc *desc = g_queue_pop_head(_context->output_buffers);

    LOGD("video codec fill eos outbuf");

    desc->nTimeStamp = 0;
    desc->nFilledLen = 0;
    desc->nFlags |= OMX_BUFFERFLAG_EOS;

    dcodec_return_buffer(_context, desc);
}

OMX_COLOR_FORMATTYPE pixel_format_av_to_omx(enum AVPixelFormat format) {
    switch (format) {
        case AV_PIX_FMT_RGB24: return OMX_COLOR_Format24bitRGB888;
        case AV_PIX_FMT_RGBA: return OMX_COLOR_Format32BitRGBA8888;
        case AV_PIX_FMT_RGB565: return OMX_COLOR_Format16bitRGB565;
        case AV_PIX_FMT_YUV420P: return OMX_COLOR_FormatYUV420Planar;
        case AV_PIX_FMT_YUYV422: return OMX_COLOR_FormatYCbYCr;
        case AV_PIX_FMT_NV12: return OMX_COLOR_FormatYUV420SemiPlanar;
        default: {
            LOGE("unknown pixel format %d", format);
            return OMX_COLOR_FormatUnused;
        }
    }
}

enum AVPixelFormat pixel_format_omx_to_av(OMX_COLOR_FORMATTYPE format) {
    switch (format) {
        case OMX_COLOR_Format24bitRGB888: return AV_PIX_FMT_RGB24;
        case OMX_COLOR_Format32BitRGBA8888: return AV_PIX_FMT_RGBA;
        case OMX_COLOR_Format32bitBGRA8888: return AV_PIX_FMT_BGRA;
        case OMX_COLOR_Format16bitRGB565: return AV_PIX_FMT_RGB565;
        case OMX_COLOR_FormatYUV420Planar: return AV_PIX_FMT_YUV420P;
        case OMX_COLOR_FormatYCbYCr: return AV_PIX_FMT_YUYV422;
        case OMX_COLOR_FormatYUV420SemiPlanar: return AV_PIX_FMT_NV12;
        default: {
            LOGE("unknown pixel format 0x%x", format);
            return AV_PIX_FMT_NONE;
        }
    }
}

/**
 * gets the opengl texture format corresponding to the pixel format.
*/
int pixel_format_to_tex_format(const OMX_COLOR_FORMATTYPE format,
                                int *glIntFmt, 
                                GLenum *glPixFmt, 
                                GLenum *glPixType) {
    switch (format) {
        case OMX_COLOR_Format24bitRGB888: {
            *glIntFmt = GL_RGB8;
            *glPixFmt = GL_RGB;
            *glPixType = GL_UNSIGNED_BYTE;
            break;
        }
        case OMX_COLOR_Format32BitRGBA8888: {
            *glPixFmt = GL_RGBA;
            *glPixType = GL_UNSIGNED_BYTE;
            *glIntFmt = GL_RGBA8;
            break;
        }
        case OMX_COLOR_Format32bitBGRA8888: {
            *glPixFmt = GL_BGRA;
            *glPixType = GL_UNSIGNED_BYTE;
            *glIntFmt = GL_RGBA8;
            break;
        }
        case OMX_COLOR_Format16bitRGB565: {
            *glPixFmt = GL_RGB;
            *glPixType = GL_UNSIGNED_SHORT_5_6_5;
            *glIntFmt = GL_RGB8;
            break;
        }
        // desktop GL does not support yuv targets
        case OMX_COLOR_FormatYUV420Planar:
        default: {
            LOGE("pixel_format_to_tex_format error! target omx pixel format %d not supported!", format);
            return ERR_INVALID_PARAM;
        }
    }
    return ERR_OK;
}

/**
 * gets the swscale parameters corresponding to the pixel format.
*/
int pixel_format_to_swscale_param(const OMX_COLOR_FORMATTYPE format,
                                int width,
                                int height,
                                uint8_t **data,
                                int *linesize) {
    switch (format) {
        case OMX_COLOR_Format24bitRGB888: {
            linesize[0] = width * 3;
            break;
        }
        case OMX_COLOR_Format32BitRGBA8888: {
            linesize[0] = width * 4;
            break;
        }
        case OMX_COLOR_Format32bitBGRA8888: {
            linesize[0] = width * 4;
            break;
        }
        case OMX_COLOR_Format16bitRGB565: {
            linesize[0] = width * 2;
            break;
        }
        case OMX_COLOR_FormatYUV420Planar: {
            data[1] = data[0] + width * height;
            data[2] = data[1] + (width / 2  * height / 2);
            linesize[0] = width;
            linesize[1] = width / 2;
            linesize[2] = width / 2;
            break;
        }
        default: {
            LOGE("pixel_format_to_tex_format error! target omx pixel format %d not supported!", format);
            return ERR_INVALID_PARAM;
        }
    }
    return ERR_OK;
}