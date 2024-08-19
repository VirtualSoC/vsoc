/**
 * vSoC video encoder device
 * 
 * Copyright (c) 2024 Jiaxing Qiu <jx.qiu@outlook.com>
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
    { OMX_VIDEO_CodingAVC, AV_CODEC_ID_H264 },
    { OMX_VIDEO_CodingHEVC, AV_CODEC_ID_HEVC },
};

static const size_t sCodingMapLen = (sizeof(sCodingMap) / sizeof(sCodingMap[0]));

// async tasks should use their own private sws contexts
extern __thread struct SwsContext * g_sws_ctx;
extern __thread uint8_t *g_videobuf;

static int setup_encoder(DCodecVideo *context);
static int open_encoder(DCodecComponent *_context);
static int empty_one_input_buffer(DCodecComponent *_context);
static int encode_video(DCodecVideo *context, BufferDesc *desc);
static int fill_one_output_buffer(DCodecComponent *_context);

static void free_avframe(gpointer frame) {
    if (frame != NULL) {
        av_frame_free((AVFrame **)&frame);
    }
}

DCodecComponent* dcodec_venc_init_component(enum OMX_VIDEO_CODINGTYPE codingType, NotifyCallbackFunc notify) {
    DCodecVideo *context = g_malloc0(sizeof(DCodecVideo));
    LOGI("dcodec_venc_init_component() component %p codingType %x", context, codingType);

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
    context->base.get_parameter = dcodec_venc_get_parameter;
    context->base.set_parameter = dcodec_venc_set_parameter;
    context->base.open_codec = open_encoder;
    context->base.empty_one_input_buffer = empty_one_input_buffer;
    context->base.fill_one_output_buffer = fill_one_output_buffer;
    context->base.fill_eos_output_buffer = dcodec_fill_eos_output_buffer;

    context->mIsDecoder = false;
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

OMX_ERRORTYPE dcodec_venc_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecVideo *context = (DCodecVideo *)_context;
    size_t videoPortIndex = context->mIsDecoder ? CODEC_INPUT_PORT_INDEX : CODEC_OUTPUT_PORT_INDEX;
    LOGD("dcodec_venc_get_parameter index:0x%x", index);

    switch ((int)index) {
        case OMX_IndexParamVideoAvc:
        {
            OMX_VIDEO_PARAM_AVCTYPE *profile =
                (OMX_VIDEO_PARAM_AVCTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            profile->eProfile = OMX_VIDEO_AVCProfileHigh;
            profile->eLevel = OMX_VIDEO_AVCLevel52;
            break;
        }

        case OMX_IndexParamVideoHevc:
        {
            OMX_VIDEO_PARAM_HEVCTYPE *profile =
                (OMX_VIDEO_PARAM_HEVCTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            profile->eProfile = OMX_VIDEO_HEVCProfileMain;
            profile->eLevel = OMX_VIDEO_HEVCMainTierLevel62;
            break;
        }

        case OMX_IndexParamVideoBitrate:
        {
            OMX_VIDEO_PARAM_BITRATETYPE *profile =
                (OMX_VIDEO_PARAM_BITRATETYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            break;
        }

        default: {
            LOGE("dcodec_venc_get_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_venc_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecVideo *context = (DCodecVideo *)_context;
    const int32_t indexFull = index;
    size_t videoPortIndex = context->mIsDecoder ? CODEC_INPUT_PORT_INDEX : CODEC_OUTPUT_PORT_INDEX;
    LOGD("dcodec_venc_set_parameter index:0x%x", index);

    switch (indexFull) {
        case OMX_IndexParamVideoDcodecDefinition:
        {
            OMX_VIDEO_DCODECDEFINITIONTYPE *newParams =
                (OMX_VIDEO_DCODECDEFINITIONTYPE *)params;

            uint32_t newWidth = newParams->nFrameWidth;
            uint32_t newHeight = newParams->nFrameHeight;
            if (newParams->nPortIndex != videoPortIndex) {
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

            LOGI("set OMX_IndexParamVideoDcodecDefinition on port %d width=%d height=%d format=0x%x framerate %d low_latency %d", newParams->nPortIndex, newWidth, newHeight, newParams->eColorFormat, newParams->xFramerate, newParams->bLowLatency);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamVideoAvc:
        {
            OMX_VIDEO_PARAM_AVCTYPE *profile =
                (OMX_VIDEO_PARAM_AVCTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            if (profile->eProfile == OMX_VIDEO_AVCProfileBaseline) {
                _context->mCtx->profile = AV_PROFILE_H264_BASELINE;
            } else if (profile->eProfile == OMX_VIDEO_AVCProfileMain) {
                _context->mCtx->profile = AV_PROFILE_H264_MAIN;
            } else if (profile->eProfile == OMX_VIDEO_AVCProfileHigh) {
                _context->mCtx->profile = AV_PROFILE_H264_HIGH;
            }
            _context->mCtx->level = 52;
            _context->mCtx->gop_size = profile->nPFrames + profile->nBFrames + 1;
            if (profile->nBFrames == 0) {
                _context->mCtx->max_b_frames = 0;
            }

            LOGD("set OMX_IndexParamVideoAvc on port %d profile %d level %d", profile->nPortIndex, profile->eProfile, profile->eLevel);

            break;
        }

        case OMX_IndexParamVideoHevc:
        {
            OMX_VIDEO_PARAM_HEVCTYPE *profile =
                (OMX_VIDEO_PARAM_HEVCTYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            if (profile->eProfile == OMX_VIDEO_HEVCProfileMain) {
                _context->mCtx->profile = AV_PROFILE_HEVC_MAIN;
            } else if (profile->eProfile == OMX_VIDEO_HEVCProfileMain10 || 
                       profile->eProfile == OMX_VIDEO_HEVCProfileMain10HDR10) {
                _context->mCtx->profile = AV_PROFILE_HEVC_MAIN_10;
            } else if (profile->eProfile == OMX_VIDEO_HEVCProfileMainStill) {
                _context->mCtx->profile = AV_PROFILE_HEVC_MAIN_STILL_PICTURE;
            }
            _context->mCtx->gop_size = profile->nKeyFrameInterval;

            LOGD("set OMX_IndexParamVideoAvc on port %d profile %d level %d", profile->nPortIndex, profile->eProfile, profile->eLevel);

            break;
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

        case OMX_IndexParamVideoBitrate:
        {
            OMX_VIDEO_PARAM_BITRATETYPE *profile =
                (OMX_VIDEO_PARAM_BITRATETYPE *)params;

            if (profile->nPortIndex != videoPortIndex) {
                return OMX_ErrorUndefined;
            }

            _context->mCtx->bit_rate = profile->nTargetBitrate;
            if (profile->eControlRate == OMX_Video_ControlRateConstant) {
                _context->mCtx->rc_max_rate = profile->nTargetBitrate;
            }
            if (profile->eControlRate == OMX_Video_ControlRateVariableSkipFrames ||
                profile->eControlRate == OMX_Video_ControlRateConstantSkipFrames) {
                LOGW("encoder skip frames is not supported, ignoring.");
            }

            LOGD("set OMX_IndexParamVideoBitrate on port %d control %d bitrate %u", profile->nPortIndex, profile->eControlRate, profile->nTargetBitrate);

            break;
        }

        default: {
            LOGE("dcodec_venc_set_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
    return OMX_ErrorNone;
}

/**
 * finds hw/sw codecs matching the given codec_id and use the hw one if faster.
 * returns ERR_OK if successful, ERR_HWACCEL_FAILED if hwaccel setup fails and we fallback to sw
 */
static int setup_encoder(DCodecVideo *context) {
    AVCodecContext *mCtx = context->base.mCtx;
    enum AVHWDeviceType device_type = AV_HWDEVICE_TYPE_NONE;
    enum AVPixelFormat hw_pix_fmt = AV_PIX_FMT_NONE;
    const AVCodec *codec = NULL;

    if (mCtx->codec_id == AV_CODEC_ID_H264) {
        // HACK: force nvenc for now. libav SUCKS at finding hw codecs.
        codec = avcodec_find_encoder_by_name("h264_nvenc");
        // codec = avcodec_find_encoder_by_name("libx264");
    }
    else if (mCtx->codec_id == AV_CODEC_ID_H265) {
        codec = avcodec_find_encoder_by_name("hevc_nvenc");
    }
    else {
        codec = avcodec_find_encoder(mCtx->codec_id);
    }
    mCtx->codec = codec;
    mCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    // use the hw device
    for (int i = 0;; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(codec, i);
        if (!config) {
            // no hw device available, return sw codec
            return ERR_OK;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX && config->device_type == AV_HWDEVICE_TYPE_CUDA) {
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

    AVBufferRef *hw_frames_ref;
    AVHWFramesContext *frames_ctx = NULL;
    int err = 0;
    if (!(hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ref))) {
        LOGE("Failed to allocate hw_frames_ctx err %s", av_err2str(err));
        return ERR_HWACCEL_FAILED;
    }
    frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
    frames_ctx->format    = AV_PIX_FMT_CUDA;
    frames_ctx->sw_format = pixel_format_omx_to_av(context->mImageFormat);
    frames_ctx->width     = context->mWidth;
    frames_ctx->height    = context->mHeight;
    frames_ctx->initial_pool_size = 4;
    if ((err = av_hwframe_ctx_init(hw_frames_ref)) < 0) {
        LOGE("Failed to initialize hw_frames_ctx err %s", av_err2str(err));
        av_buffer_unref(&hw_frames_ref);
        return ERR_HWACCEL_FAILED;
    }

    mCtx->hw_device_ctx = hw_device_ref;
    mCtx->hw_frames_ctx = hw_frames_ref;
    mCtx->pix_fmt = hw_pix_fmt;

    LOGD("hw encoder %s pix_fmt %s setup complete", av_hwdevice_get_type_name(device_type), av_get_pix_fmt_name(hw_pix_fmt));
    return ERR_OK;
}

static int open_encoder(DCodecComponent *_context) {
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

    // find encoder
    int err = setup_encoder(context);
    if (err < ERR_OK || mCtx->codec == NULL) {
        return ERR_CODEC_NOT_FOUND;
    }

    // set default ctx params
    mCtx->workaround_bugs   = FF_BUG_AUTODETECT;
    mCtx->idct_algo         = FF_IDCT_AUTO;
    mCtx->skip_frame        = AVDISCARD_DEFAULT;
    mCtx->skip_idct         = AVDISCARD_DEFAULT;
    mCtx->skip_loop_filter  = AVDISCARD_DEFAULT;
    mCtx->flags2 |= AV_CODEC_FLAG2_FAST;
    // mCtx->error_concealment = 3;

    if (mCtx->time_base.num == 0 || mCtx->time_base.den == 0) {
        // default timebase: microsecond
        mCtx->time_base = AV_TIME_BASE_Q;
    }
    mCtx->sample_aspect_ratio = (AVRational){1, 1};

    // c.f. libx264-ultrafast.ffpreset
    mCtx->rc_buffer_size = 0;
    mCtx->me_cmp = 1;
    mCtx->me_range = 16;
    mCtx->qmin = 10;
    mCtx->qmax = 51;
    mCtx->i_quant_factor = 0.71;
    mCtx->qcompress = 0.6;
    mCtx->max_qdiff = 4;
    mCtx->trellis = 1;
    mCtx->flags |= AV_CODEC_FLAG_LOOP_FILTER;

#ifdef STD_DEBUG_LOG
    mCtx->debug = 1;
#endif

    AVDictionary *options = NULL;

    if (context->mIsLowLatency) {
        mCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
        // nvenc
        av_dict_set(&options, "zerolatency", "1", 0);
        av_dict_set(&options, "preset", "fast", 0);
        // libx264
        // av_dict_set(&options, "preset", "ultrafast", 0);
        // av_dict_set(&options, "tune", "zerolatency", 0);
    }

    LOGD("open ffmpeg video encoder (%s), src %dx%d pix_fmt %s tgt %dx%d "
         "pix_fmt %s; low_latency %d bit_rate %" PRId64 " gop_size %d b_frames %d",
         mCtx->codec->name, context->mWidth, context->mHeight,
         av_get_pix_fmt_name(pixel_format_omx_to_av(context->mImageFormat)),
         mCtx->width, mCtx->height, av_get_pix_fmt_name(mCtx->pix_fmt),
         context->mIsLowLatency, mCtx->bit_rate, mCtx->gop_size, mCtx->max_b_frames);

    err = avcodec_open2(mCtx, mCtx->codec, &options);
    av_dict_free(&options);
    if (err < 0) {
        LOGE("ffmpeg video encoder failed to initialize (%s).", av_err2str(err));
        return ERR_CODEC_OPEN_FAILED;
    }

    LOGI("open ffmpeg video encoder (%s) success, width %d height %d pix_fmt %s",
            mCtx->codec->name, mCtx->width, mCtx->height, av_get_pix_fmt_name(mCtx->pix_fmt));

    context->mInputMap = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free_avframe);

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

    if (desc->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
        LOGW("extradata config ignored when the encoder is open");
        dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
        return ERR_OK;
    }
    ret = encode_video(context, desc);

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
            ret = encode_video(context, NULL);
            CHECK_EQ(ret, ERR_OK);
        }
        _context->mStatus = INPUT_EOS_SEEN;
    }

    dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
    return ERR_OK;
}

static int encode_video(DCodecVideo *context, BufferDesc *desc) {
    AVFrame *mFrame = context->base.mFrame;
    AVCodecContext *mCtx = context->base.mCtx;
    int ret = ERR_OK;

    if (desc == NULL) {
        mFrame->data[0] = NULL;
        mFrame->linesize[0] = 0;
        mFrame->pts = AV_NOPTS_VALUE;
    }
    else if (desc->type & CODEC_BUFFER_TYPE_AVFRAME) {
        mFrame = (AVFrame *)desc->data;
    }
    else if (desc->type & CODEC_BUFFER_TYPE_GBUFFER) {
        Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(desc->id);
        if (gbuffer == NULL) {
            LOGE("error! encoder source gbuffer %" PRIx64 " is null!", desc->id);
            return ERR_CODING_FAILED;
        }
        update_gbuffer_phy_usage(gbuffer, EXPRESS_MEM_TYPE_TEXTURE, false);
        mFrame = (AVFrame *)g_hash_table_lookup(context->mInputMap, (gpointer)desc->id);
        if (!mFrame) {
            mFrame = av_frame_alloc();
            mFrame->width = gbuffer->width;
            mFrame->height = gbuffer->height;
            if (mCtx->pix_fmt == AV_PIX_FMT_CUDA) {
                mFrame->format = mCtx->pix_fmt;
            }
            else {
                mFrame->format = pixel_format_omx_to_av(context->mImageFormat);
            }
            av_hwframe_get_buffer(mCtx->hw_frames_ctx, mFrame, 0);
            g_hash_table_insert(context->mInputMap, (gpointer)desc->id, (gpointer)mFrame);
        }
        if (!av_buffer_is_writable(mFrame->buf[0])) {
            LOGE("error! av_buffer id %" PRIx64 " is not writable!", desc->id);
        }

        if (mCtx->pix_fmt == AV_PIX_FMT_CUDA) {
            copy_tex_to_cuda((CUdeviceptr)mFrame->data[0], gbuffer->data_texture, mFrame->linesize);
        }
        else {
            glReadPixels(0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, mFrame->data[0]);
        }
        signal_express_sync(desc->sync_id, true);
        mFrame->pts = desc->nTimeStamp;
    }
    else if (desc->type & CODEC_BUFFER_TYPE_GUEST_MEM) {
        mFrame = (AVFrame *)g_hash_table_lookup(context->mInputMap, (gpointer)desc->id);
        if (!mFrame) {
            mFrame = av_frame_alloc();
            mFrame->width = context->mWidth;
            mFrame->height = context->mHeight;
            mFrame->format = mCtx->pix_fmt;
            av_frame_get_buffer(mFrame, 0);
            g_hash_table_insert(context->mInputMap, (gpointer)desc->id, (gpointer)mFrame);
        }
        if (!av_buffer_is_writable(mFrame->buf[0])) {
            LOGE("error! av_buffer id %" PRIx64 " is not writable!", desc->id);
        }

        uint8_t *data[4] = { 0 };
        int linesize[4] = { 0 };

        g_sws_ctx = sws_getCachedContext(g_sws_ctx,
            mFrame->width, mFrame->height, pixel_format_omx_to_av(context->mImageFormat), context->mWidth, context->mHeight,
            mFrame->format, SWS_FAST_BILINEAR, NULL, NULL, NULL);

        g_videobuf = av_realloc(g_videobuf, desc->nFilledLen);
        read_from_guest_mem((Guest_Mem *)desc->data, g_videobuf, 0, desc->nFilledLen);
        data[0] = g_videobuf;

        if (pixel_format_to_swscale_param(context->mImageFormat, context->mWidth, context->mHeight, data, linesize) < 0) {
            return ERR_SWS_FAILED;
        }

        sws_scale(g_sws_ctx, (const uint8_t * const*)data, linesize, 0, mFrame->height, mFrame->data, mFrame->linesize);

        mFrame->pts = desc->nTimeStamp;
    }
    else {
        LOGE("input buffer %" PRIx64 " type %x not supported!", desc->id, desc->type);
        return ERR_NO_FRM;
    }

    if (mFrame->linesize[0] == 0 && desc) { // empty frames will cause mischief with ffmpeg
        return ERR_NO_FRM;
    }

    LOGD("avcodec_send_frame linesize %d %d %d pts %lld desc %p", mFrame->linesize[0], mFrame->linesize[1], mFrame->linesize[2], mFrame->pts, desc);

    ret = avcodec_send_frame(mCtx, mFrame);
    if (ret == AVERROR_EOF || ret == AVERROR_INVALIDDATA) {
        // hope that the next frame will make it right
        return ERR_OK;
    }
    else if (ret == AVERROR(EAGAIN)) {
        return ERR_INPUT_QUEUE_FULL;
    }
    else if (ret != 0) {
        LOGE("avcodec_send_frame error %d", ret);
        return ERR_CODING_FAILED;
    }

    return ERR_OK;
}

static int find_nal_preamble_idx(uint8_t *data, int offset, int size) {
    // nal header max 4 bytes
    for (int i = 0; offset + i + 4 < size; i++) {
        if (*((uint32_t *)(data + offset + i)) == 0x01000000) { // nal unit preamble
            return offset + i;
        }
    }
    return size;
}

/**
 * parses pps and sps from the header.
*/
static int parse_pps_sps(DCodecComponent *_context, uint8_t *data, int size) {
    BufferDesc *desc = g_queue_peek_head(_context->output_buffers);
    int seek_idx = 0, end_idx = 0, write_idx = 0;

    seek_idx = find_nal_preamble_idx(data, seek_idx, size);
    while (seek_idx < size) {
        char utype = data[seek_idx + 4] & 0x1F; // unit type
        if (utype == 7 || utype == 8) { // sps or pps
            end_idx = find_nal_preamble_idx(data, seek_idx + 1, size);
            CHECK(desc->type & CODEC_BUFFER_TYPE_GUEST_MEM);
            CHECK_LE(write_idx + end_idx - seek_idx, desc->nAllocLen);
            LOGD("parsed sps/pps type %d size %d", utype, end_idx - seek_idx);
            write_to_guest_mem((Guest_Mem *)desc->data, data + seek_idx, write_idx, end_idx - seek_idx);
            write_idx += end_idx - seek_idx;
            seek_idx = end_idx;
        } else {
            seek_idx = find_nal_preamble_idx(data, seek_idx + 1, size);
        }
    }

    if (write_idx > 0) {
        desc = g_queue_pop_head(_context->output_buffers);
        desc->nFilledLen = write_idx;
        desc->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
        dcodec_return_buffer(_context, desc);
        return ERR_OK;
    }
    return ERR_NO_FRM;
}

static int fill_one_output_buffer(DCodecComponent *_context) {
    AVCodecContext *mCtx = _context->mCtx;
    AVPacket *mPkt = _context->mPkt;
    static __thread bool _has_sent_config;

    // read one packet at a time
    int ret = avcodec_receive_packet(mCtx, mPkt);
    if (ret == AVERROR_EOF && _context->mStatus == INPUT_EOS_SEEN) {
        _context->fill_eos_output_buffer(_context);
        _context->mStatus = OUTPUT_EOS_SENT;
        av_packet_unref(mPkt);
        return ERR_OK;
    }
    else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        av_packet_unref(mPkt);
        return ERR_NO_FRM;
    }
    else if (ret < 0) {
        LOGE("avcodec_receive_packet error %d", ret);
        av_packet_unref(mPkt);
        return ERR_CODING_FAILED;
    }

    if (_has_sent_config == false) {
        CHECK_GE(g_queue_get_length(_context->output_buffers), 2);
        if (parse_pps_sps(_context, mPkt->data, mPkt->size) == ERR_OK) {
            _has_sent_config = true;
        }
    }

    BufferDesc *desc = g_queue_pop_head(_context->output_buffers);

    // process timestamps
    int64_t pts = mPkt->pts;
    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    desc->nTimeStamp = pts;

    CHECK_GE(desc->nAllocLen, mPkt->size);
    desc->nFilledLen = mPkt->size;
    if (mPkt->flags & AV_PKT_FLAG_KEY) {
        desc->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
    }

    LOGD("fill_one_output_buffer() on buffer type %x id %" PRIx64 " nAllocLen %u "
         "nFilledLen %u nOffset %u nTimeStamp %lld nFlags %x sync_id %d",
         desc->type, desc->id, desc->nAllocLen, desc->nFilledLen, desc->nOffset,
         desc->nTimeStamp, desc->nFlags, desc->sync_id);

    LOGD("mPkt pts: %lld flags 0x%x", mPkt->pts, mPkt->flags);

    if (desc->type & CODEC_BUFFER_TYPE_GUEST_MEM) {
        write_to_guest_mem((Guest_Mem *)desc->data, mPkt->data, 0, desc->nFilledLen);
    }
    else {
        LOGE("output buffer type %x not supported yet!", desc->type);
    }

    dcodec_return_buffer(_context, desc);
    av_packet_unref(mPkt);

    return ERR_OK;
}
