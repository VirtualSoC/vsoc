// #define STD_DEBUG_LOG
// #define STD_DEBUG_INDEPENDENT_WINDOW
#include "hw/teleport-express/express_log.h"

#include "hw/express-codec/dcodec_video.h"

#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/glv3_status.h"
#include "hw/express-gpu/express_sync.h"

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

static const CodecProfileLevel kM4VProfileLevels[] = {
    { OMX_VIDEO_MPEG4ProfileSimple, OMX_VIDEO_MPEG4Level5 },
    { OMX_VIDEO_MPEG4ProfileAdvancedSimple, OMX_VIDEO_MPEG4Level5 },
};

static const CodecProfileLevel kAVCProfileLevels[] = {
    // Only declare the highest level for each supported profile
    { OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel52 },
    { OMX_VIDEO_AVCProfileMain,     OMX_VIDEO_AVCLevel52 },
    { OMX_VIDEO_AVCProfileHigh,     OMX_VIDEO_AVCLevel52 },
};

static AVCodec *find_decoder(enum AVCodecID codec_id);
static int open_decoder(DCodecComponent *_context);
static int empty_one_input_buffer(DCodecComponent *_context);
static int decode_video(DCodecVideo *context, BufferDesc *desc);
static int fill_one_output_buffer(DCodecComponent *_context);
static void fill_eos_output_buffer(DCodecComponent *_context);

DCodecComponent* dcodec_video_init_component(enum OMX_VIDEO_CODINGTYPE codingType, NotifyCallbackFunc notify) {
    DCodecVideo *context = g_malloc0(sizeof(DCodecVideo));
    LOGI("dcodec_video_init_component() component %p codingType %x", context, codingType);

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

    context->base.reset_component = dcodec_video_reset_component;
    context->base.destroy_component = dcodec_video_destroy_component;
    context->base.get_parameter = dcodec_video_get_parameter;
    context->base.set_parameter = dcodec_video_set_parameter;
    context->base.open_decoder = open_decoder;
    context->base.empty_one_input_buffer = empty_one_input_buffer;
    context->base.fill_one_output_buffer = fill_one_output_buffer;
    context->base.fill_eos_output_buffer = fill_eos_output_buffer;

    const CodecProfileLevel *codec_profile_levels;
    size_t codec_array_size;
    if (codec_id == AV_CODEC_ID_MPEG4) {
        codec_profile_levels = kM4VProfileLevels;
        codec_array_size = ARRAY_SIZE(kM4VProfileLevels);
    } 
    else if (codec_id == AV_CODEC_ID_H264) {
        codec_profile_levels = kAVCProfileLevels;
        codec_array_size = ARRAY_SIZE(kAVCProfileLevels);
    } 
    else {
        codec_profile_levels = NULL;
        codec_array_size = 0;
    }

    context->mIsAdaptive = false;
    context->mAdaptiveMaxWidth = 0;
    context->mAdaptiveMaxHeight = 0;
    context->mWidth = 1920;
    context->mHeight = 1080;
    context->mCropLeft = 0;
    context->mCropTop = 0;
    context->mCropWidth = 1920;
    context->mCropHeight = 1080;
    context->mProfileLevels = codec_profile_levels;
    context->mNumProfileLevels = codec_array_size;
    context->mImgConvertCtx = NULL;
    context->mStride = context->mWidth;
    context->mTgtPixelFormat = OMX_COLOR_Format24bitRGB888;

    AVCodecContext *mCtx = context->base.mCtx;

    mCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    mCtx->codec_id = codec_id;
    mCtx->extradata_size = 0;
    mCtx->extradata = NULL;
    mCtx->width = context->mWidth;
    mCtx->height = context->mHeight;

    // inform express-gpu to create shared child window
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    context->window = get_native_opengl_context(DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT);
#else
    context->window = get_native_opengl_context(0);
#endif
    if (!context->window) {
        LOGE("create shared child window failed!");
    }
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
    glfwMakeContextCurrent(context->window);
    glfwSwapInterval(0);

    glGenTextures(1, &context->mDebugTexture);
    glGenFramebuffers(1 /* num */, &context->mDebugFbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, context->mDebugFbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, context->mDebugTexture, 0);
#else
    egl_makeCurrent(context->window);
#endif

    glGenBuffers(1, &context->mUnpackBuffer);

    return (DCodecComponent *)context;
}

OMX_ERRORTYPE dcodec_video_reset_component(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    THREAD_CONTROL_BEGIN
    glfwHideWindow(context->window);
    THREAD_CONTROL_END
#endif

    return dcodec_reset_component(_context);
}

OMX_ERRORTYPE dcodec_video_destroy_component(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    THREAD_CONTROL_BEGIN
    glfwHideWindow(context->window);
    THREAD_CONTROL_END
#endif

    if (context->mUnpackBufferSync != NULL)
    {
        glDeleteSync(context->mUnpackBufferSync);
        context->mUnpackBufferSync = NULL;
    }

    glDeleteFramebuffers(1 /* num */, &context->mDebugFbo);
    glDeleteTextures(1, &context->mDebugTexture);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glDeleteBuffers(1, &context->mUnpackBuffer);
    egl_makeCurrent(NULL);
    release_native_opengl_context(context->window, 0);

    av_free(context->mVideoBuffer);

    dcodec_deinit_component(_context);
    g_free(context);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_video_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecVideo *context = (DCodecVideo *)_context;
    LOGD("dcodec_video_get_parameter index:0x%x", index);

    switch ((int)index) {
        case OMX_IndexParamVideoWmv:
        {
            OMX_VIDEO_PARAM_WMVTYPE *profile =
                (OMX_VIDEO_PARAM_WMVTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->eFormat = OMX_VIDEO_WMVFormatUnused;
            break;
        }

        case OMX_IndexParamVideoRv:
        {
            OMX_VIDEO_PARAM_RVTYPE *profile =
                (OMX_VIDEO_PARAM_RVTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->eFormat = OMX_VIDEO_RVFormatUnused;
            break;
        }

        case OMX_IndexParamVideoProfileLevelQuerySupported:
        {
            OMX_VIDEO_PARAM_PROFILELEVELTYPE *profileLevel =
                  (OMX_VIDEO_PARAM_PROFILELEVELTYPE *) params;

            if (profileLevel->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                LOGE("Invalid port index: %" PRIu32, profileLevel->nPortIndex);
                return OMX_ErrorUnsupportedIndex;
            }

            if (profileLevel->nProfileIndex >= context->mNumProfileLevels) {
                return OMX_ErrorNoMore;
            }

            profileLevel->eProfile = context->mProfileLevels[profileLevel->nProfileIndex].mProfile;
            profileLevel->eLevel   = context->mProfileLevels[profileLevel->nProfileIndex].mLevel;
            break;
        }

        default: {
            LOGE("dcodec_video_get_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_video_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecVideo *context = (DCodecVideo *)_context;
    const int32_t indexFull = index;
    LOGD("dcodec_video_set_parameter index:0x%x", index);

    switch (indexFull) {
        case OMX_IndexParamVideoDcodecDefinition:
        {
            OMX_VIDEO_DCODECDEFINITIONTYPE *newParams =
                (OMX_VIDEO_DCODECDEFINITIONTYPE *)params;

            uint32_t newWidth = newParams->nFrameWidth;
            uint32_t newHeight = newParams->nFrameHeight;
            bool outputPort = (newParams->nPortIndex == CODEC_OUTPUT_PORT_INDEX);
            if (outputPort) {
                context->mWidth = newWidth;
                context->mHeight = newHeight;
                context->mTgtPixelFormat = (OMX_COLOR_FORMATTYPE)newParams->eColorFormat;
            } else {
                // For input port, we only set nFrameWidth and nFrameHeight. Buffer size
                // is updated when configuring the output port using the max-frame-size,
                // though client can still request a larger size.
                _context->mCtx->width = newWidth;
                _context->mCtx->height = newHeight;
            }
            LOGI("set OMX_IndexParamVideoDcodecDefinition on port %d width=%d height=%d format=%d", newParams->nPortIndex, newWidth, newHeight, newParams->eColorFormat);
            return OMX_ErrorNone;
        }

        case OMX_IndexParamVideoWmv:
        {
            OMX_VIDEO_PARAM_WMVTYPE *profile =
                (OMX_VIDEO_PARAM_WMVTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
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

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
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

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
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
            LOGE("dcodec_video_set_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
}

/**
 * finds hw/sw codecs matching the given codec_id and return the hw one if possible
 * adapted from av_find_codec_by_name()
*/
static AVCodec* find_decoder(enum AVCodecID codec_id) {
    static const char *hwaccels[] = {
        "d3d11va",     // windows
        "vaapi",       // linux
        "videotoolbox" // macOS
    };

    static const int hwaccels_len = sizeof(hwaccels) / sizeof(hwaccels[0]);

    AVCodec *codec_hw = NULL, *codec_sw = NULL, *codec = NULL;
    void *i = 0;

    while ((codec = av_codec_iterate(&i))) {
        if (codec->id != codec_id || !av_codec_is_decoder(codec)) {
            continue;
        }
        for (int j = 0; j < hwaccels_len; j++) {
            if (strstr(codec->name, hwaccels[j]) != NULL) {
                codec_hw = codec;
                break;
            }
        }
        codec_sw = codec;
    }
    return codec_hw ? codec_hw : codec_sw;
}

static int open_decoder(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    if (avcodec_is_open(mCtx)) {
        return ERR_OK;
    }

    // only accept extradata configs before we open the decoder
    if (dcodec_handle_extradata(_context) != ERR_OK) {
        return ERR_EXTRADATA_FAILED;
    }

    // find decoder
    mCtx->codec = find_decoder(mCtx->codec_id);
    if (!(mCtx->codec)) {
        LOGE("ffmpeg video decoder failed to find codec");
        return ERR_CODEC_NOT_FOUND;
    }

    // set default ctx params
    mCtx->workaround_bugs   = 1;
    mCtx->idct_algo         = 0;
    mCtx->skip_frame        = AVDISCARD_DEFAULT;
    mCtx->skip_idct         = AVDISCARD_DEFAULT;
    mCtx->skip_loop_filter  = AVDISCARD_DEFAULT;
    mCtx->flags2 |= AV_CODEC_FLAG2_FAST;
    // mCtx->error_concealment = 3;

#ifdef STD_DEBUG_LOG
    mCtx->debug = 1;
#endif
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
    THREAD_CONTROL_BEGIN
    glfwSetWindowSize(context->window, context->mWidth, context->mHeight);
    // required, since get_native_opengl_context() creates a 1x1 window by default
    glViewport(0, 0, context->mWidth, context->mHeight);
    glfwShowWindow(context->window);
    THREAD_CONTROL_END
#endif

    LOGD("open ffmpeg video decoder (%s), width %d height %d",
           mCtx->codec->name, mCtx->width, mCtx->height);

    int err = avcodec_open2(mCtx, mCtx->codec, NULL);
    if (err < 0) {
        LOGE("ffmpeg video decoder failed to initialize (%s).", av_err2str(err));
        return ERR_DECODER_OPEN_FAILED;
    }

    context->mVideoBuffer = av_realloc(context->mVideoBuffer, av_image_get_buffer_size(pixel_format_omx_to_av(context->mTgtPixelFormat), context->mWidth, context->mHeight, 1));

    LOGD("open ffmpeg video decoder (%s) success, width %d height %d",
            mCtx->codec->name, mCtx->width, mCtx->height);

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
        LOGW("extradata config ignored when the decoder is open");
        dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
        return ERR_OK;
    }

    ret = decode_video(context, desc);

    // a negative error code is returned if an error occurred during decoding
    if (ret < 0) {
        return ret;
    }
    else if (ret == ERR_INPUT_QUEUE_FULL) {
        return ret;
    }

    LOGD("empty_one_input_buffer() on buffer type %x id %" PRIx64 " nAllocLen %u "
         "nFilledLen %u nOffset %u nTimeStamp %lld nFlags %x",
         desc->type, desc->id, desc->nAllocLen, desc->nFilledLen, desc->nOffset,
         desc->nTimeStamp, desc->nFlags);

    if (desc->nFlags & OMX_BUFFERFLAG_EOS) {
        LOGD("input eos seen, flushing buffers");

        if (mCtx->codec->capabilities & AV_CODEC_CAP_DELAY) {
            LOGD("codec capability AV_CODEC_CAP_DELAY detected, sending EOS packet.");
            ret = decode_video(context, NULL);
            CHECK_EQ(ret, ERR_OK);
        }
        avcodec_flush_buffers(mCtx);
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
        read_from_guest_mem(desc->data, context->mVideoBuffer, desc->nOffset, desc->nFilledLen);
        mPkt->data = context->mVideoBuffer;
        mPkt->size = desc->nFilledLen;
        mPkt->pts = desc->nTimeStamp;
        mPkt->dts = desc->nTimeStamp;
    }
    else {
        LOGE("input buffer %" PRIx64 " type %x not supported!", desc->id, desc->type);
    }

    if (mPkt->size == 0 && desc && !(desc->nFlags & OMX_BUFFERFLAG_ENDOFFRAME)) { // empty packets will cause mischief with ffmpeg
        return ERR_NO_FRM;
    }

    LOGD("avcodec_send_packet pkt size %d pts %lld", mPkt->size, mPkt->pts);

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
        return ERR_DECODE_FAILED;
    }

    return ERR_OK;
}

static int fill_one_output_buffer(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    AVFrame *mFrame = _context->mFrame;
    BufferDesc *desc = g_queue_peek_head(_context->output_buffers);

    // read one frame at a time
    int ret = avcodec_receive_frame(mCtx, mFrame);
    if (ret == AVERROR_EOF && _context->mStatus != OUTPUT_EOS_SENT) {
        fill_eos_output_buffer(_context);
        _context->mStatus = OUTPUT_EOS_SENT;
        return ERR_OK;
    }
    else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return ERR_NO_FRM;
    }
    else if (ret < 0) {
        LOGE("avcodec_receive_frame error %d", ret);
        return ERR_DECODE_FAILED;
    }

    int64_t pts = AV_NOPTS_VALUE;
    
    //process timestamps
    pts = mFrame->best_effort_timestamp;
    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    LOGD("mFrame pts: %lld pkt_dts: %lld best effort: %lld. used %lld", mFrame->pts, mFrame->pkt_dts, mFrame->best_effort_timestamp, pts);

    uint8_t *dst = context->mVideoBuffer;
    uint32_t bufferWidth = max(context->mIsAdaptive ? context->mAdaptiveMaxWidth : 0, context->mWidth);
    uint32_t bufferHeight = max(context->mIsAdaptive ? context->mAdaptiveMaxHeight : 0, context->mHeight);
    uint8_t *data[4] = {0};
    int linesize[4] = {0};
    enum AVPixelFormat avDstFmt = pixel_format_omx_to_av(context->mTgtPixelFormat);
    int glPixFmt = GL_RGB;
    int glPixType = GL_UNSIGNED_BYTE;
    int glIntFmt = GL_RGB8;
    switch (context->mTgtPixelFormat) {
        case OMX_COLOR_Format24bitRGB888: {
            glPixFmt = GL_RGB;
            glPixType = GL_UNSIGNED_BYTE;
            glIntFmt = GL_RGB8;
            data[0] = dst;
            linesize[0] = bufferWidth * 3;
            break;
        }
        case OMX_COLOR_Format32BitRGBA8888: {
            glPixFmt = GL_RGBA;
            glPixType = GL_UNSIGNED_BYTE;
            glIntFmt = GL_RGBA8;
            data[0] = dst;
            linesize[0] = bufferWidth * 4;
            break;
        }
        case OMX_COLOR_Format16bitRGB565: {
            glPixFmt = GL_RGB;
            glPixType = GL_UNSIGNED_SHORT_5_6_5;
            glIntFmt = GL_RGB8;
            data[0] = dst;
            linesize[0] = bufferWidth * 2;
            break;
        }
        case OMX_COLOR_FormatYUV420Planar: { // android guests only support yuv buffers in sw mode
            data[0] = dst;
            data[1] = dst + bufferWidth * bufferHeight;
            data[2] = data[1] + (bufferWidth / 2  * bufferHeight / 2);
            linesize[0] = bufferWidth;
            linesize[1] = bufferWidth / 2;
            linesize[2] = bufferWidth / 2;
            break;
        }
        default: {
            LOGE("scaling error! target omx pixel format %d not supported!", context->mTgtPixelFormat);
            return ERR_SWS_FAILED;
        }
    }

    LOGD("sws_scale frame_width=%d frame_height=%d buffer_width=%d buffer_height=%d ctx_width=%d ctx_height=%d mIsAdaptive=%d src_format=%s tgt_format=%s",
          mFrame->width, mFrame->height, bufferWidth, bufferHeight, mCtx->width, mCtx->height, context->mIsAdaptive, av_get_pix_fmt_name(mFrame->format), av_get_pix_fmt_name(avDstFmt));

    int outputSize = av_image_get_buffer_size(avDstFmt, bufferWidth, bufferHeight, 1);
    int sws_flags = SWS_FAST_BILINEAR;
    context->mImgConvertCtx = sws_getCachedContext(context->mImgConvertCtx,
           mFrame->width, mFrame->height, (enum AVPixelFormat)mFrame->format, mCtx->width, mCtx->height,
           avDstFmt, sws_flags, NULL, NULL, NULL);
    if (context->mImgConvertCtx == NULL) {
        LOGE("Cannot initialize the conversion context");
        return ERR_SWS_FAILED;
    }
    sws_scale(context->mImgConvertCtx, mFrame->data, mFrame->linesize,
            0, mFrame->height, data, linesize);

    CHECK_GE(desc->nAllocLen, outputSize);
    desc->nFilledLen = outputSize;
    desc->nTimeStamp = pts;
    if (mFrame->key_frame) {
        desc->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
    }

    LOGD("fill_one_output_buffer() on buffer type %x id %" PRIx64 " nAllocLen %u "
         "nFilledLen %u nOffset %u nTimeStamp %lld nFlags %x",
         desc->type, desc->id, desc->nAllocLen, desc->nFilledLen, desc->nOffset,
         desc->nTimeStamp, desc->nFlags);

    // write data to output buffer
    if (desc->type & CODEC_BUFFER_TYPE_GUEST_MEM) {
        write_to_guest_mem(desc->data, context->mVideoBuffer, 0, desc->nFilledLen);
#ifdef STD_DEBUG_INDEPENDENT_WINDOW
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, context->mDebugTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, glIntFmt, context->mWidth, context->mHeight, 0, glPixFmt, glPixType, context->mVideoBuffer);
        glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, context->mDebugTexture, 0);
        glBlitFramebuffer(0, 0, context->mWidth, context->mHeight, 0, 0, context->mWidth, context->mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        THREAD_CONTROL_BEGIN
        glfwSwapBuffers(context->window);
        THREAD_CONTROL_END
#endif
    }
    else if (desc->type & CODEC_BUFFER_TYPE_GBUFFER) {
        Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(desc->id);
        if (gbuffer == NULL) {
            LOGD("create_gbuffer with id %llx width %d height %d pixtype %x pixfmt %x intfmt %x", desc->id, context->mWidth, context->mHeight, glPixType, glPixFmt, glIntFmt);
            gbuffer = create_gbuffer(context->mWidth, context->mHeight, 0 /* sampler num */,
                          glPixFmt,
                          glPixType,
                          glIntFmt,
                          0,
                          0,
                          desc->id);

            add_gbuffer_to_global(gbuffer);
        }

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, context->mUnpackBuffer);

        GLint sync_status = GL_SIGNALED;
        if (context->mUnpackBufferSync)
        {
            glGetSynciv(context->mUnpackBufferSync, GL_SYNC_STATUS, sizeof(GLint), NULL, &sync_status);
            glDeleteSync(context->mUnpackBufferSync);
        }

        if (sync_status != GL_SIGNALED || context->mUnpackBufferSize < outputSize)
        {
            glBufferData(GL_PIXEL_UNPACK_BUFFER, outputSize, NULL, GL_STREAM_DRAW);
            context->mUnpackBufferSize = outputSize;
        }

        // upload data to GPU
        GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, outputSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        memcpy(map_pointer, context->mVideoBuffer, outputSize);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, glIntFmt, context->mWidth, context->mHeight, 0, glPixFmt, glPixType, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        set_express_sync_id(desc->sync_id, true);

#ifdef STD_DEBUG_INDEPENDENT_WINDOW
        glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gbuffer->data_texture, 0);
        glBlitFramebuffer(0, 0, context->mWidth, context->mHeight, 0, 0, context->mWidth, context->mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        THREAD_CONTROL_BEGIN
        glfwSwapBuffers(context->window);
        THREAD_CONTROL_END
#endif

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            LOGE("codec gl error %x!", error);
        }
    }
    else {
        LOGE("output buffer type %x not supported yet!", desc->type);
    }
    dcodec_return_buffer(_context, g_queue_pop_head(_context->output_buffers));
    return ERR_OK;
}

static void fill_eos_output_buffer(DCodecComponent *_context) {
    BufferDesc *desc = g_queue_pop_head(_context->output_buffers);

    LOGD("video decoder fill eos outbuf");

    desc->nTimeStamp = 0;
    desc->nFilledLen = 0;
    desc->nFlags |= OMX_BUFFERFLAG_EOS;

    dcodec_return_buffer(_context, desc);
}
