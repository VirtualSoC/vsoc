/**
 * vSoC video decoder device
 * 
 * Copyright (c) 2023 Jiaxing Qiu <jx.qiu@outlook.com>
 * Copyright (c) 2023 Zijie Zhou <zijiezhou017@outlook.com>
 */

// #define STD_DEBUG_LOG

// todo: do not uncomment the following line as it is unusable for now.
// #define STD_DEBUG_INDEPENDENT_WINDOW

#include "hw/teleport-express/express_log.h"
#include "hw/express-codec/dcodec_video.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/glv3_status.h"
#include "hw/express-mem/express_sync.h"
#include "hw/express-mem/express_mem.h"

#include "hw/express-codec/device_cuda.h"

#define MAX_SW_VIDEO_DIMENSION 1280

static const struct VideoCodingMapEntry {
    OMX_VIDEO_CODINGTYPE mCodingType;
    enum AVCodecID mCodecID;
} sCodingMap[] = {
    { (OMX_VIDEO_CODINGTYPE) OMX_VIDEO_CodingDIVX, AV_CODEC_ID_MPEG4 },
    { (OMX_VIDEO_CODINGTYPE) OMX_VIDEO_CodingFLV1, AV_CODEC_ID_FLV1 },
    { OMX_VIDEO_CodingAVC, AV_CODEC_ID_H264 },
    { OMX_VIDEO_CodingH263, AV_CODEC_ID_H263 },
    { OMX_VIDEO_CodingHEVC, AV_CODEC_ID_HEVC },
    { OMX_VIDEO_CodingMPEG2, AV_CODEC_ID_MPEG2VIDEO },
    { OMX_VIDEO_CodingMPEG4, AV_CODEC_ID_MPEG4 },
    { OMX_VIDEO_CodingRV, AV_CODEC_ID_RV40 },
    { (OMX_VIDEO_CODINGTYPE) OMX_VIDEO_CodingVC1, AV_CODEC_ID_VC1 },
    { OMX_VIDEO_CodingVP8, AV_CODEC_ID_VP8 },
    { OMX_VIDEO_CodingVP9, AV_CODEC_ID_VP9 },
    { OMX_VIDEO_CodingAutoDetect, AV_CODEC_ID_NONE },
    { OMX_VIDEO_CodingWMV, AV_CODEC_ID_WMV2 },
};

static const size_t sCodingMapLen = (sizeof(sCodingMap) / sizeof(sCodingMap[0]));

// async tasks should use their own private sws contexts
__thread struct SwsContext * g_sws_ctx;
__thread uint8_t *g_videobuf;

static int setup_decoder(DCodecVideo *context);
static int open_codec(DCodecComponent *_context);
static int empty_one_input_buffer(DCodecComponent *_context);
static int decode_video(DCodecVideo *context, BufferDesc *desc);
static int fill_one_output_buffer(DCodecComponent *_context);

static void free_avpacket(gpointer pkt) {
    if (pkt != NULL) {
        av_packet_free((AVPacket **)&pkt);
    }
}

DCodecComponent* dcodec_vdec_init_component(enum OMX_VIDEO_CODINGTYPE codingType, NotifyCallbackFunc notify) {
    DCodecVideo *context = g_malloc0(sizeof(DCodecVideo));
    LOGI("dcodec_vdec_init_component() component %p codingType %x", context, codingType);

    enum AVCodecID codec_id = AV_CODEC_ID_NONE;

    for (int i = 0; i < sCodingMapLen; i++) {
        if (sCodingMap[i].mCodingType == codingType) {
            codec_id = sCodingMap[i].mCodecID;
        }
    }

    // init base component
    int ret = dcodec_init_component((DCodecComponent *)context, notify);
    if (ret != ERR_OK) {
        LOGE("error %d when creating dcodec base component", ret);
        g_free(context);
        return NULL;
    }

    context->base.reset_component = dcodec_vdec_reset_component;
    context->base.destroy_component = dcodec_vdec_destroy_component;
    context->base.get_parameter = dcodec_vdec_get_parameter;
    context->base.set_parameter = dcodec_vdec_set_parameter;
    context->base.open_codec = open_codec;
    context->base.empty_one_input_buffer = empty_one_input_buffer;
    context->base.fill_one_output_buffer = fill_one_output_buffer;
    context->base.fill_eos_output_buffer = dcodec_fill_eos_output_buffer;

    context->mIsDecoder = true;
    context->mIsAdaptive = false;
    context->mIsLowLatency = false;
    context->mAdaptiveMaxWidth = 0;
    context->mAdaptiveMaxHeight = 0;
    context->mWidth = 1920;
    context->mHeight = 1080;
    context->mImageFormat = OMX_COLOR_Format32BitRGBA8888;

    AVCodecContext *mCtx = context->base.mCtx;

    mCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    mCtx->codec_id = codec_id;
    mCtx->extradata_size = 0;
    mCtx->extradata = NULL;
    mCtx->width = context->mWidth;
    mCtx->height = context->mHeight;

    return (DCodecComponent *)context;
}

OMX_ERRORTYPE dcodec_vdec_reset_component(DCodecComponent *_context) {
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    THREAD_CONTROL_BEGIN
    glfwHideWindow(context->window);
    THREAD_CONTROL_END
#endif

    return dcodec_reset_component(_context);
}

OMX_ERRORTYPE dcodec_vdec_destroy_component(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;

    if (g_sws_ctx) {
        sws_freeContext(g_sws_ctx);
        g_sws_ctx = NULL;
    }

    if (g_videobuf) {
        av_free(g_videobuf);
        g_videobuf = NULL;
    }

    if (context->mCsConv) {
        cs_deinit_cuda(context->mCsConv);
        cs_deinit(context->mCsConv);
    }

    if (context->window) {
        // the decoder has been opened before
        glDeleteFramebuffers(1 /* num */, &context->mDebugFbo);
        glDeleteTextures(1, &context->mDebugTexture);
        egl_makeCurrent(NULL);
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
        release_native_opengl_context(context->window, DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT);
#else
        release_native_opengl_context(context->window, 0);
#endif
    }

    if (context->mInputMap) {
        g_hash_table_destroy(context->mInputMap);
    }

    dcodec_deinit_component(_context);
    g_free(context);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_vdec_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecVideo *context = (DCodecVideo *)_context;
    size_t videoPortIndex = context->mIsDecoder ? CODEC_INPUT_PORT_INDEX : CODEC_OUTPUT_PORT_INDEX;
    LOGD("dcodec_vdec_get_parameter index:0x%x", index);

    switch ((int)index) {
        case OMX_IndexParamVideoWmv:
        {
            OMX_VIDEO_PARAM_WMVTYPE *profile =
                (OMX_VIDEO_PARAM_WMVTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            profile->eFormat = OMX_VIDEO_WMVFormatUnused;
            break;
        }

        case OMX_IndexParamVideoRv:
        {
            OMX_VIDEO_PARAM_RVTYPE *profile =
                (OMX_VIDEO_PARAM_RVTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            profile->eFormat = OMX_VIDEO_RVFormatUnused;
            break;
        }

        default: {
            LOGE("dcodec_vdec_get_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_vdec_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecVideo *context = (DCodecVideo *)_context;
    const int32_t indexFull = index;
    size_t videoPortIndex = context->mIsDecoder ? CODEC_INPUT_PORT_INDEX : CODEC_OUTPUT_PORT_INDEX;
    size_t imagePortIndex = context->mIsDecoder ? CODEC_OUTPUT_PORT_INDEX : CODEC_INPUT_PORT_INDEX;
    LOGD("dcodec_vdec_set_parameter index:0x%x", index);

    switch (indexFull) {
        case OMX_IndexParamVideoDcodecDefinition:
        {
            OMX_VIDEO_DCODECDEFINITIONTYPE *newParams =
                (OMX_VIDEO_DCODECDEFINITIONTYPE *)params;

            uint32_t newWidth = newParams->nFrameWidth;
            uint32_t newHeight = newParams->nFrameHeight;
            if (newParams->nPortIndex == imagePortIndex) {
                context->mWidth = newWidth;
                context->mHeight = newHeight;
                context->mImageFormat = (OMX_COLOR_FORMATTYPE)newParams->eColorFormat;
            } else {
                // For input port, we only set nFrameWidth and nFrameHeight. Buffer size
                // is updated when configuring the output port using the max-frame-size,
                // though client can still request a larger size.
                _context->mCtx->width = newWidth;
                _context->mCtx->height = newHeight;
            }
            context->mIsLowLatency = newParams->bLowLatency ? true : false;

            LOGI("set OMX_IndexParamVideoDcodecDefinition on port %d width=%d height=%d format=0x%x low_latency %d", newParams->nPortIndex, newWidth, newHeight, newParams->eColorFormat, newParams->bLowLatency);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamVideoWmv:
        {
            OMX_VIDEO_PARAM_WMVTYPE *profile =
                (OMX_VIDEO_PARAM_WMVTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            if (profile->eFormat == OMX_VIDEO_WMVFormat7) {
                _context->mCtx->codec_id = AV_CODEC_ID_WMV1;
            } else if (profile->eFormat == OMX_VIDEO_WMVFormat8) {
                _context->mCtx->codec_id = AV_CODEC_ID_WMV2;
            } else if (profile->eFormat == OMX_VIDEO_WMVFormat9) {
                _context->mCtx->codec_id = AV_CODEC_ID_WMV3;
            } else {
                _context->mCtx->codec_id = AV_CODEC_ID_VC1;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamVideoRv:
        {
            OMX_VIDEO_PARAM_RVTYPE *profile =
                (OMX_VIDEO_PARAM_RVTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            if (profile->eFormat == OMX_VIDEO_RVFormatG2) {
                _context->mCtx->codec_id = AV_CODEC_ID_RV20;
            } else if (profile->eFormat == OMX_VIDEO_RVFormat8) {
                _context->mCtx->codec_id = AV_CODEC_ID_RV30;
            } else if (profile->eFormat == OMX_VIDEO_RVFormat9) {
                _context->mCtx->codec_id = AV_CODEC_ID_RV40;
            } else {
                LOGE("unsupported rv codec: 0x%x", profile->eFormat);
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamVideoFFmpeg:
        {
            OMX_VIDEO_PARAM_FFMPEGTYPE *profile =
                (OMX_VIDEO_PARAM_FFMPEGTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            _context->mCtx->codec_id = (enum AVCodecID)profile->eCodecId;
            _context->mCtx->width    = profile->nWidth;
            _context->mCtx->height   = profile->nHeight;

            LOGD("got OMX_IndexParamVideoFFmpeg, eCodecId:%d(%s), width:%u, height:%u",
                profile->eCodecId, avcodec_get_name(_context->mCtx->codec_id),
                profile->nWidth, profile->nHeight);

            return OMX_ErrorNone;
        }

        default: {
            LOGE("dcodec_vdec_set_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
    return OMX_ErrorNone;
}

/**
 * finds hw/sw codecs matching the given codec_id and use the hw one if faster.
 * returns ERR_OK if successful, ERR_HWACCEL_FAILED if hwaccel setup fails and we fallback to sw
 */
static int setup_decoder(DCodecVideo *context) {
    AVCodecContext *mCtx = context->base.mCtx;
    enum AVHWDeviceType device_type = AV_HWDEVICE_TYPE_NONE;
    enum AVPixelFormat hw_pix_fmt = AV_PIX_FMT_NONE;
    const AVCodec *codec = avcodec_find_decoder(mCtx->codec_id);

    mCtx->codec = codec;

    if (context->mIsLowLatency && mCtx->width <= MAX_SW_VIDEO_DIMENSION && mCtx->height <= MAX_SW_VIDEO_DIMENSION) {
        // CPU decoding is faster when the video is small
        return ERR_OK;
    }

    // use the hw device
    for (int i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(codec, i);
        if (!config) {
            // no hw device available, return sw codec
            return ERR_OK;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && config->device_type == AV_HWDEVICE_TYPE_CUDA) {
            device_type = config->device_type;
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }

    AVBufferRef *hw_device_ref;
    // use primary context to share device ptrs
    if (av_hwdevice_ctx_create(&hw_device_ref, device_type,
                                      NULL, NULL, 1 /* AV_CUDA_USE_PRIMARY_CONTEXT */) < 0) {
        LOGW("failed to create HW device type %s pix_fmt %s", av_hwdevice_get_type_name(device_type), av_get_pix_fmt_name(hw_pix_fmt));
        return ERR_HWACCEL_FAILED;
    }

    // nvenc only supports nv12
    context->mCsConv = cs_init(pixel_format_omx_to_av(context->mImageFormat), AV_PIX_FMT_NV12, context->mWidth, context->mHeight);

    // CHECK_CU(cu->cuGLGetDevices(&deviceNum, &dummy, 1, CU_GL_DEVICE_LIST_ALL));
    // if (deviceNum == 0) {
    //     LOGW("no available cuda devices for the current gl context. this will result in degraded decoding performance");
    //     av_buffer_unref(&hw_device_ref);
    //     return ERR_HWACCEL_FAILED;
    // }

    mCtx->hw_device_ctx = hw_device_ref;

    LOGD("hw decoder %s pix_fmt %s setup complete", av_hwdevice_get_type_name(device_type), av_get_pix_fmt_name(hw_pix_fmt));
    return ERR_OK;
}

static int open_codec(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    if (avcodec_is_open(mCtx)) {
        return ERR_OK;
    }

    // inform express-gpu to create shared child window
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    context->window = get_native_opengl_context(DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT);
#else
    context->window = get_native_opengl_context(0);
#endif
    if (!context->window) {
        LOGE("create shared child window failed!");
        return ERR_CODEC_OPEN_FAILED;
    }
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
    glfwMakeContextCurrent(context->window);
    glfwSwapInterval(0);

    glGenTextures(1, &context->mDebugTexture);
    glGenFramebuffers(1 /* num */, &context->mDebugFbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, context->mDebugFbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, context->mDebugTexture, 0);

    THREAD_CONTROL_BEGIN
    glfwSetWindowSize(context->window, context->mWidth, context->mHeight);
    // required, since get_native_opengl_context() creates a 1x1 window by default
    glViewport(0, 0, context->mWidth, context->mHeight);
    glfwShowWindow(context->window);
    THREAD_CONTROL_END
#else
    egl_makeCurrent(context->window);
#endif

    // find decoder
    int err = setup_decoder(context);
    if (err < ERR_OK || mCtx->codec == NULL) {
        return ERR_CODEC_NOT_FOUND;
    }

    // qsv doesn't play nice with h264 / h265 extradata, use workaround
    if ((strstr(mCtx->codec->name, "h264_qsv") == NULL) && (strstr(mCtx->codec->name, "h265_qsv") == NULL)) {
        // only accept extradata configs before we open the decoder
        if (dcodec_handle_extradata(_context) != ERR_OK) {
            return ERR_EXTRADATA_FAILED;
        }
    } 

    // set default ctx params
    mCtx->workaround_bugs   = FF_BUG_AUTODETECT;
    mCtx->idct_algo         = FF_IDCT_AUTO;
    mCtx->skip_frame        = AVDISCARD_DEFAULT;
    mCtx->skip_idct         = AVDISCARD_DEFAULT;
    mCtx->skip_loop_filter  = AVDISCARD_DEFAULT;
    mCtx->flags2 |= AV_CODEC_FLAG2_FAST;
    // mCtx->error_concealment = 3;

    if (mCtx->pkt_timebase.num == 0 || mCtx->pkt_timebase.den == 0) {
        // default timebase: microsecond
        mCtx->pkt_timebase = AV_TIME_BASE_Q;
    }

#ifdef STD_DEBUG_LOG
    mCtx->debug = 1;
#endif

    AVDictionary *options = NULL;

    if (context->mIsLowLatency) {
        mCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
        av_dict_set(&options, "tune", "zerolatency", 0);
        av_dict_set(&options, "preset", "ultrafast", 0);
    }

    LOGD("open ffmpeg video decoder (%s), width %d height %d",
           mCtx->codec->name, mCtx->width, mCtx->height);

    err = avcodec_open2(mCtx, mCtx->codec, &options);
    av_dict_free(&options);
    if (err < 0) {
        LOGE("ffmpeg video decoder failed to initialize (%s).", av_err2str(err));
        return ERR_CODEC_OPEN_FAILED;
    }

    LOGI("open ffmpeg video decoder (%s) success, width %d height %d",
            mCtx->codec->name, mCtx->width, mCtx->height);

    context->mInputMap = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free_avpacket);

    return ERR_OK;
}

/**
 * @brief decode a video frame using the first buffer in the input queue.
*/
static int empty_one_input_buffer(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    int ret = 0;

    BufferDesc *desc = g_queue_peek_head(_context->input_buffers);

    // 1. dcodec_handle_extradata() 参考487这个while循环 改成拼接extradata
    // 2. 这个函数里特判一下是不是h2645_qsv，如果是的话发第一个包的时候把extradata拼在第一个包前面，参考496行
    if ((strstr(mCtx->codec->name, "h264_qsv") != NULL) || (strstr(mCtx->codec->name, "h265_qsv") != NULL)) {
        uint8_t *extra_buf = av_mallocz(1);
        int extra_bufsize = 0;

        while (desc->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
            CHECK(desc->type & CODEC_BUFFER_TYPE_GUEST_MEM);
            extra_buf = av_realloc(extra_buf, extra_bufsize + desc->nFilledLen);
            read_from_guest_mem(desc->data, extra_buf + extra_bufsize, 0, desc->nFilledLen);
            extra_bufsize = extra_bufsize + desc->nFilledLen;
            dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
            desc = g_queue_peek_head(_context->input_buffers);
        }
        AVPacket *new_packet = av_packet_alloc();
        BufferDesc *new_desc = g_malloc(sizeof(BufferDesc));
        if (extra_bufsize != 0) {
            extra_buf = av_realloc(extra_buf, extra_bufsize + desc->nFilledLen);
            read_from_guest_mem(desc->data, extra_buf + extra_bufsize, 0, desc->nFilledLen);
            extra_bufsize = extra_bufsize + desc->nFilledLen;
            av_packet_from_data(new_packet, extra_buf, extra_bufsize);
            // send input packet to codec
            new_desc->type = CODEC_BUFFER_TYPE_INPUT | CODEC_BUFFER_TYPE_AVPACKET;
            new_desc->id = desc->id;
            new_desc->data = new_packet;
            new_desc->nAllocLen = extra_bufsize;
            new_desc->nFilledLen = extra_bufsize;
            new_desc->nOffset = 0;
            new_desc->nTimeStamp = desc->nTimeStamp;
            new_desc->nFlags = desc->nFlags;
            desc = new_desc;
        }

        ret = decode_video(context, desc);

        g_free(new_desc);
        av_packet_free(&new_packet);
    }
    else {
        if (desc->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
            LOGW("extradata config ignored when the decoder is open");
            dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
            return ERR_OK;
        }
        ret = decode_video(context, desc);
    }

    // a negative error code is returned if an error occurred during decoding
    if (ret < 0) {
        dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
        return ret;
    }
    else if (ret == ERR_INPUT_QUEUE_FULL) {
        return ret;
    }

    LOGD("empty_one_input_buffer() on buffer type %x id %" PRIx64 " nAllocLen %u "
         "nFilledLen %u nOffset %u nTimeStamp %lld nFlags %x sync_id %d",
         desc->type, desc->id, desc->nAllocLen, desc->nFilledLen, desc->nOffset,
         desc->nTimeStamp, desc->nFlags, desc->sync_id);

    if (desc->nFlags & OMX_BUFFERFLAG_EOS) {
        LOGD("input eos seen, flushing buffers");

        if (mCtx->codec->capabilities & AV_CODEC_CAP_DELAY) {
            LOGD("codec capability AV_CODEC_CAP_DELAY detected, sending EOS packet.");
            ret = decode_video(context, NULL);
            CHECK_EQ(ret, ERR_OK);
        }
        _context->mStatus = INPUT_EOS_SEEN;
    }

    dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
    return ERR_OK;
}

static int decode_video(DCodecVideo *context, BufferDesc *desc) {
    AVPacket *mPkt = context->base.mPkt;
    AVCodecContext *mCtx = context->base.mCtx;
    int ret = ERR_OK;

    if (desc == NULL) {
        mPkt->data = NULL;
        mPkt->size = 0;
        mPkt->pts = AV_NOPTS_VALUE;
        mPkt->dts = AV_NOPTS_VALUE;
    }
    else if (desc->type & CODEC_BUFFER_TYPE_AVPACKET) {
        mPkt = (AVPacket *)desc->data;
    }
    else if (desc->type & CODEC_BUFFER_TYPE_GUEST_MEM) {
        mPkt = (AVPacket *)g_hash_table_lookup(context->mInputMap, (gpointer)desc->id);
        if (!mPkt) {
            mPkt = av_packet_alloc();
            uint8_t *buf = av_malloc(min(desc->nFilledLen * 2, desc->nAllocLen));
            read_from_guest_mem(desc->data, buf, desc->nOffset, desc->nFilledLen);
            av_packet_from_data(mPkt, buf, desc->nFilledLen);
            g_hash_table_insert(context->mInputMap, (gpointer)desc->id, (gpointer)mPkt);
        }
        else {
            if (av_buffer_is_writable(mPkt->buf)) {
                if (mPkt->buf->size < desc->nFilledLen) {
                    int newSize = min(desc->nFilledLen * 2, desc->nAllocLen) + AV_INPUT_BUFFER_PADDING_SIZE;
                    LOGD("av_buffer_realloc %d -> %d", mPkt->buf->size, newSize);
                    av_buffer_realloc(&mPkt->buf, newSize);
                    mPkt->data = mPkt->buf->data;
                }
                read_from_guest_mem(desc->data, mPkt->buf->data, desc->nOffset, desc->nFilledLen);
                mPkt->size = desc->nFilledLen;
            }
            else {
                LOGE("error! av_buffer id %" PRIx64 " is not writable!", desc->id);
            }
        }
        mPkt->pts = desc->nTimeStamp;
        mPkt->dts = desc->nTimeStamp;
    }
    else {
        LOGE("input buffer %" PRIx64 " type %x not supported!", desc->id, desc->type);
        return ERR_NO_FRM;
    }

    if (mPkt->size == 0 && desc) { // empty packets will cause mischief with ffmpeg
        return ERR_NO_FRM;
    }

    LOGD("avcodec_send_packet pkt size %d pts %lld desc %p", mPkt->size, mPkt->pts, desc);

    ret = avcodec_send_packet(mCtx, mPkt);
    if (ret == AVERROR_EOF || ret == AVERROR_INVALIDDATA) {
        // hope that the next packet will make it right
        return ERR_OK;
    }
    else if (ret == AVERROR(EAGAIN)) {
        return ERR_INPUT_QUEUE_FULL;
    }
    else if (ret != 0) {
        LOGE("avcodec_send_packet error %d", ret);
        return ERR_CODING_FAILED;
    }

    return ERR_OK;
}

static void swscale_task_cb(MemTransferTask *task, void *mapped_addr) {
    DCodecVideo *context = task->private_data;
    DCodecComponent *_context = (DCodecComponent *)context;
    AVFrame *mFrame = task->src_data;
    enum AVPixelFormat avdstfmt = pixel_format_omx_to_av(context->mImageFormat);
    uint8_t *data[4] = { mapped_addr };
    int linesize[4] = { 0 };

    if (mFrame->format == AV_PIX_FMT_CUDA) {
        AVFrame *swFrame = av_frame_alloc();
        av_hwframe_transfer_data(swFrame, mFrame, 0);
        av_frame_free(&mFrame);
        mFrame = swFrame;
    }

    g_sws_ctx = sws_getCachedContext(g_sws_ctx,
           mFrame->width, mFrame->height, mFrame->format, context->mWidth, context->mHeight,
           avdstfmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if (task->dst_dev == EXPRESS_MEM_TYPE_GBUFFER_HOST_MEM) {
        Hardware_Buffer *gbuffer = (Hardware_Buffer *)task->dst_data;
        gbuffer->host_data = g_realloc(gbuffer->host_data, task->dst_len);
        data[0] = (uint8_t *)gbuffer->host_data;
    }
    else if (task->dst_dev == EXPRESS_MEM_TYPE_HOST_MEM) {
        data[0] = (uint8_t *)task->dst_data;
    } 
    else if (task->dst_dev == EXPRESS_MEM_TYPE_GUEST_OPAQUE) {
        g_videobuf = av_realloc(g_videobuf, task->dst_len);
        data[0] = g_videobuf;
    }

    if (pixel_format_to_swscale_param(context->mImageFormat, context->mWidth, context->mHeight, data, linesize) < 0) {
        return;
    }

    LOGD("sws_scale frame_width=%d frame_height=%d ctx_width=%d ctx_height=%d mIsAdaptive=%d src_format=%s tgt_format=%s",
        mFrame->width, mFrame->height, context->mWidth, context->mHeight, context->mIsAdaptive, av_get_pix_fmt_name(mFrame->format), av_get_pix_fmt_name(avdstfmt));

    sws_scale(g_sws_ctx, (const uint8_t * const*)mFrame->data, mFrame->linesize, 0, mFrame->height, data, linesize);

    if (task->dst_dev == EXPRESS_MEM_TYPE_GUEST_OPAQUE) {
        BufferDesc *desc = (BufferDesc *)task->dst_data;
        write_to_guest_mem((Guest_Mem *)desc->data, g_videobuf, 0, desc->nFilledLen);
        _context->notify(_context, (CodecCallbackData){ .event = OMX_EventFillBufferDone, .data1 = desc->nFilledLen, .data2 = desc->nTimeStamp, .data = desc->id, .flags = desc->nFlags });
    }

#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    if (task->dst_dev == EXPRESS_MEM_TYPE_TEXTURE) {
        glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gbuffer->data_texture, 0);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, context->mDebugTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, glIntFmt, context->mWidth, context->mHeight, 0, glPixFmt, glPixType, data[0]);
        glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, context->mDebugTexture, 0);
    }
    glBlitFramebuffer(0, 0, context->mWidth, context->mHeight, 0, 0, context->mWidth, context->mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    THREAD_CONTROL_BEGIN
    glfwSwapBuffers(context->window);
    THREAD_CONTROL_END
#endif
    av_frame_free(&mFrame);
}

static int fill_one_output_buffer(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    AVFrame *mFrame = av_frame_alloc();
    BufferDesc *desc = g_queue_peek_head(_context->output_buffers);

    if (mem_transfer_is_busy()) {
        return ERR_OK;
    }

    // read one frame at a time
    int ret = avcodec_receive_frame(mCtx, mFrame);
    if (ret == AVERROR_EOF && _context->mStatus == INPUT_EOS_SEEN) {
        _context->fill_eos_output_buffer(_context);
        _context->mStatus = OUTPUT_EOS_SENT;
        av_frame_free(&mFrame);
        return ERR_OK;
    }
    else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        av_frame_free(&mFrame);
        return ERR_NO_FRM;
    }
    else if (ret < 0) {
        LOGE("avcodec_receive_frame error %d", ret);
        av_frame_free(&mFrame);
        return ERR_CODING_FAILED;
    }

    uint32_t bufferWidth = max(context->mIsAdaptive ? context->mAdaptiveMaxWidth : 0, context->mWidth);
    uint32_t bufferHeight = max(context->mIsAdaptive ? context->mAdaptiveMaxHeight : 0, context->mHeight);

    // process timestamps
    int64_t pts = mFrame->best_effort_timestamp;
    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    desc->nTimeStamp = pts;

    enum AVPixelFormat avDstFmt = pixel_format_omx_to_av(context->mImageFormat);
    int outputSize = av_image_get_buffer_size(avDstFmt, bufferWidth, bufferHeight, 1);
    CHECK_GE(desc->nAllocLen, outputSize);
    desc->nFilledLen = outputSize;
    if (mFrame->flags & AV_FRAME_FLAG_KEY) {
        desc->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
    }

    LOGD("fill_one_output_buffer() on buffer type %x id %" PRIx64 " nAllocLen %u "
         "nFilledLen %u nOffset %u nTimeStamp %lld nFlags %x sync_id %d",
         desc->type, desc->id, desc->nAllocLen, desc->nFilledLen, desc->nOffset,
         desc->nTimeStamp, desc->nFlags, desc->sync_id);

    LOGD("mFrame pts: %lld pkt_dts: %lld best effort: %lld. used %lld", mFrame->pts, mFrame->pkt_dts, mFrame->best_effort_timestamp, pts);

    desc = g_queue_pop_head(_context->output_buffers);

    if (desc->type & CODEC_BUFFER_TYPE_GUEST_MEM) {
        mem_transfer_async(EXPRESS_MEM_TYPE_GUEST_OPAQUE, EXPRESS_MEM_TYPE_HOST_OPAQUE, desc, mFrame, desc->nAllocLen, outputSize, desc->sync_id, swscale_task_cb, NULL, context);
        // desc is freed by the async thread
        // since the async thread needs to send omx fill events
    }
    else if (desc->type & CODEC_BUFFER_TYPE_GBUFFER) {
        int glIntFmt = GL_RGB8;
        GLenum glPixFmt = GL_RGB, glPixType = GL_UNSIGNED_BYTE;
        if (pixel_format_to_tex_format(context->mImageFormat, &glIntFmt, &glPixFmt, &glPixType) < 0) {
            av_frame_free(&mFrame);
            return ERR_SWS_FAILED;
        }

        Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(desc->id);
        if (gbuffer == NULL) {
            LOGD("create_gbuffer with id %llx width %d height %d pixtype %x pixfmt %x intfmt %x", desc->id, context->mWidth, context->mHeight, glPixType, glPixFmt, glIntFmt);
            gbuffer = create_gbuffer(context->mWidth, context->mHeight, 0 /* sampler num */,
                          glPixFmt,
                          glPixType,
                          glIntFmt,
                          0,
                          0,
                          desc->id);

            glGenFramebuffers(1, &gbuffer->data_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->data_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->data_texture, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            add_gbuffer_to_global(gbuffer);
        }

        uint32_t block_time;
        ExpressMemType pred_phy_dev;

        if (mFrame->format == AV_PIX_FMT_CUDA) {
            // hw pix fmt, do in-GPU colorspace conversion
            LOGD("hw frame %p received, data %p size %d", mFrame, mFrame->data[0], mFrame->linesize[0]);
            cs_map_cuda(context->mCsConv, (CUdeviceptr *)mFrame->data, mFrame->linesize);
            cs_convert(context->mCsConv, gbuffer->data_fbo);
            av_frame_free(&mFrame);
            pred_phy_dev = mem_predict_prefetch(gbuffer, EXPRESS_CODEC_DEVICE_ID, EXPRESS_MEM_TYPE_TEXTURE, &block_time);
            if (pred_phy_dev == EXPRESS_MEM_TYPE_UNKNOWN) pred_phy_dev = EXPRESS_MEM_TYPE_TEXTURE; // default to texture
            mem_transfer_async(pred_phy_dev, EXPRESS_MEM_TYPE_TEXTURE, gbuffer, gbuffer, outputSize, outputSize, desc->sync_id, NULL, NULL, context);
        } else {
            // host mem, need to use swscale by CPU
            pred_phy_dev = mem_predict_prefetch(gbuffer, EXPRESS_CODEC_DEVICE_ID, EXPRESS_MEM_TYPE_GBUFFER_HOST_MEM, &block_time);
            if (pred_phy_dev == EXPRESS_MEM_TYPE_UNKNOWN) pred_phy_dev = EXPRESS_MEM_TYPE_TEXTURE; // default to texture
            mem_transfer_async(pred_phy_dev, EXPRESS_MEM_TYPE_HOST_OPAQUE, gbuffer, mFrame, outputSize, outputSize, desc->sync_id, swscale_task_cb, NULL, context);
            // mFrame is freed by swscale_task_cb
            // av_frame_free(&mFrame);
        }
        // notify the guest ahead of time
        _context->notify(_context, (CodecCallbackData) { .event = OMX_EventFillBufferDone, .data1 = desc->nFilledLen, .data2 = desc->nTimeStamp, .data = desc->id, .flags = desc->nFlags, .extra = block_time });
        dcodec_free_buffer_desc(desc);
    }
    else {
        LOGE("output buffer type %x not supported yet!", desc->type);
        dcodec_free_buffer_desc(desc);
        av_frame_free(&mFrame);
    }

    return ERR_OK;
}
