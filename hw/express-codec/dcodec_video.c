// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

#include "hw/express-codec/dcodec_video.h"

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

static int open_decoder(DCodecComponent *_context);
static int empty_one_input_buffer(DCodecComponent *_context);
static int decode_video(DCodecVideo *context, BufferDesc *desc);
static int fill_one_output_buffer(DCodecComponent *_context);
static void fill_eos_output_buffer(DCodecComponent *_context);

DCodecComponent* dcodec_video_init_component(enum OMX_VIDEO_CODINGTYPE codingType) {
    DCodecVideo *context = g_malloc0(sizeof(DCodecVideo));
    LOGI("dcodec_video_init_component() component %p codingType %x", context, codingType);

    bool found = false;
    enum AVCodecID codec_id = AV_CODEC_ID_NONE;

    for (int i = 0; i < sCodingMapLen; i++) {
        if (sCodingMap[i].mCodingType == codingType) {
            codec_id = sCodingMap[i].mCodecID;
            found = true;
        }
    }
    if (!found) {
        LOGW("warning! OMX coding type %x not supported by us!", codingType);
    }

    // init base component
    int ret = dcodec_init_component((DCodecComponent *)context);
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

    context->mCodingType = codingType;
    context->mIsAdaptive = false;
    context->mAdaptiveMaxWidth = 0;
    context->mAdaptiveMaxHeight = 0;
    context->mWidth = 352;
    context->mHeight = 288;
    context->mCropLeft = 0;
    context->mCropTop = 0;
    context->mCropWidth = 352;
    context->mCropHeight = 288;
    context->mOutputFormat = OMX_COLOR_FormatYUV420Planar;
    context->mMinInputBufferSize = 384; // arbitrary, using one uncompressed macroblock
    context->mMinCompressionRatio = 1;  // max input size is normally the output size
    context->mProfileLevels = codec_profile_levels;
    context->mNumProfileLevels = codec_array_size;
    context->mImgConvertCtx = NULL;
    context->mStride = 320;

    AVCodecContext *mCtx = context->base.mCtx;

    mCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    mCtx->codec_id = codec_id;
    mCtx->extradata_size = 0;
    mCtx->extradata = NULL;
    mCtx->width = context->mWidth;
    mCtx->height = context->mHeight;

    return (DCodecComponent *)context;
}

OMX_ERRORTYPE dcodec_video_reset_component(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;

    return dcodec_reset_component(_context);
}

OMX_ERRORTYPE dcodec_video_destroy_component(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;

    dcodec_deinit_component(_context);
    g_free(context);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_video_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecVideo *context = (DCodecVideo *)_context;
    AVCodecContext *mCtx = _context->mCtx;
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
    AVCodecContext *mCtx = _context->mCtx;
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
            } else {
                // For input port, we only set nFrameWidth and nFrameHeight. Buffer size
                // is updated when configuring the output port using the max-frame-size,
                // though client can still request a larger size.
                _context->mCtx->width = newWidth;
                _context->mCtx->height = newHeight;
            }
            LOGI("set OMX_IndexParamAudioDcodecDefinition on port %d width=%d height=%d", newParams->nPortIndex, newWidth, newHeight);
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

        case kPrepareForAdaptivePlaybackIndex:
        {
            // todo: use metadata
            // const PrepareForAdaptivePlaybackParams* adaptivePlaybackParams =
            //         (const PrepareForAdaptivePlaybackParams *)params;

            // context->mIsAdaptive = adaptivePlaybackParams->bEnable;
            // if (context->mIsAdaptive) {
            //     context->mAdaptiveMaxWidth = adaptivePlaybackParams->nMaxFrameWidth;
            //     context->mAdaptiveMaxHeight = adaptivePlaybackParams->nMaxFrameHeight;
            //     context->mWidth = context->mAdaptiveMaxWidth;
            //     context->mHeight = context->mAdaptiveMaxHeight;
            // } else {
            //     context->mAdaptiveMaxWidth = 0;
            //     context->mAdaptiveMaxHeight = 0;
            // }
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

    //find decoder
    mCtx->codec = avcodec_find_decoder(mCtx->codec_id);
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
    mCtx->flags |= AV_CODEC_FLAG_BITEXACT;
    // mCtx->error_concealment = 3;
    // mCtx->flags2 |= AV_CODEC_FLAG2_FAST;

    LOGD("open ffmpeg video decoder(%s), width %d height %d",
           avcodec_get_name(mCtx->codec_id), mCtx->width, mCtx->height);

    int err = avcodec_open2(mCtx, mCtx->codec, NULL);
    if (err < 0) {
        LOGE("ffmpeg video decoder failed to initialize (%s).", av_err2str(err));
        return ERR_DECODER_OPEN_FAILED;
    }

    LOGD("open ffmpeg video decoder(%s) success, width %d height %d",
            avcodec_get_name(mCtx->codec_id), mCtx->width, mCtx->height);

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
        dcodec_return_buffer_to_guest(_context, g_queue_pop_head(_context->input_buffers));
        return ERR_OK;
    }

    ret = decode_video(context, desc);

    // a negative error code is returned if an error occurred during decoding
    if (ret < 0) {
        // todo: play black frame instead
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
        LOGD("input eos seen, switching to INPUT_EOS_SEEN.");
        _context->mStatus = INPUT_EOS_SEEN;
        if (mCtx->codec->capabilities & AV_CODEC_CAP_DELAY) {
            LOGD("codec capability AV_CODEC_CAP_DELAY detected, sending EOS packet.");
            ret = decode_video(context, NULL);
            CHECK_EQ(ret, ERR_OK);
        }
    }

    dcodec_return_buffer_to_guest(_context, g_queue_pop_head(_context->input_buffers));
    return ERR_OK;
}

static int decode_video(DCodecVideo *context, BufferDesc *desc) {
    AVPacket *mPkt = context->base.mPkt;
    AVCodecContext *mCtx = context->base.mCtx;
    AVFrame *mFrame = context->base.mFrame;
    int ret = ERR_OK;

    if (desc == NULL) {
        mPkt->data = NULL;
        mPkt->size = 0;
        mPkt->pts = AV_NOPTS_VALUE;
    }
    else {
        read_from_guest_mem(desc->data, context->mVideoBuffer, desc->nOffset, desc->nFilledLen);
        mPkt->data = context->mVideoBuffer;
        mPkt->size = desc->nFilledLen;
        mPkt->pts = desc->nTimeStamp;
        mPkt->dts = desc->nTimeStamp;
    }

    ret = avcodec_send_packet(mCtx, mPkt);
    if (ret == AVERROR_EOF) {
        return ERR_OK;
    }
    else if (ret == AVERROR(EAGAIN)) {
        return ERR_INPUT_QUEUE_FULL;
    }
    else if (ret != 0) {
        LOGE("avcodec_send_packet error %d", ret);
        return ERR_DECODE_FAILED;
    }

    LOGD("input pkt size:%d, pts:%lld", mPkt->size, mPkt->pts);

    return ERR_OK;
}

static int fill_one_output_buffer(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    AVFrame *mFrame = _context->mFrame;

    // read one frame at a time
    int ret = avcodec_receive_frame(mCtx, mFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return ERR_NO_FRM;
    }
    else if (ret < 0) {
        LOGE("avcodec_receive_frame error %d", ret);
        return ERR_DECODE_FAILED;
    }

    uint8_t *data[4];
    int linesize[4];

    int64_t pts = AV_NOPTS_VALUE;
    uint8_t *dst = context->mVideoBuffer;

    uint32_t bufferWidth = max(context->mIsAdaptive ? context->mAdaptiveMaxWidth : 0, context->mWidth);
    uint32_t bufferHeight = max(context->mIsAdaptive ? context->mAdaptiveMaxHeight : 0, context->mHeight);

    data[0] = dst;
    data[1] = dst + bufferWidth * bufferHeight;
    data[2] = data[1] + (bufferWidth / 2  * bufferHeight / 2);
    linesize[0] = bufferWidth;
    linesize[1] = bufferWidth / 2;
    linesize[2] = bufferWidth / 2;

    LOGD("fill_one_output_buffer: frame_width=%d frame_height=%d buffer_width=%d buffer_height=%d ctx_width=%d ctx_height=%d mIsAdaptive=%d src_format=%d tgt_format=%d",
          mFrame->width, mFrame->height, bufferWidth, bufferHeight, mCtx->width, mCtx->height, context->mIsAdaptive, mFrame->format, AV_PIX_FMT_YUV420P);

    int sws_flags = SWS_BICUBIC;
    context->mImgConvertCtx = sws_getCachedContext(context->mImgConvertCtx,
           mFrame->width, mFrame->height, (enum AVPixelFormat)mFrame->format, mFrame->width, mFrame->height,
           AV_PIX_FMT_YUV420P, sws_flags, NULL, NULL, NULL);
    if (context->mImgConvertCtx == NULL) {
        LOGE("Cannot initialize the conversion context");
        return ERR_SWS_FAILED;
    }
    sws_scale(context->mImgConvertCtx, mFrame->data, mFrame->linesize,
            0, mFrame->height, data, linesize);

    BufferDesc *desc = g_queue_pop_head(_context->output_buffers);

    CHECK_GE(desc->nAllocLen, (bufferWidth * bufferHeight * 3) / 2);

    desc->nFilledLen = (bufferWidth * bufferHeight * 3) / 2;

    if (mFrame->key_frame) {
        desc->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
    }

    //process timestamps
    pts = mFrame->best_effort_timestamp;
    // pts = mFrame->pkt_dts;

    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    desc->nTimeStamp = pts;

    LOGD("mFrame pts: %lld pkt_dts: %lld best effort: %lld. used %lld", mFrame->pts, mFrame->pkt_dts, mFrame->best_effort_timestamp, pts);

    write_to_guest_mem(desc->data, context->mVideoBuffer, 0, desc->nFilledLen);
    dcodec_return_buffer_to_guest(_context, desc);
    return ERR_OK;
}

static void fill_eos_output_buffer(DCodecComponent *_context) {
    DCodecVideo *context = (DCodecVideo *)_context;
    BufferDesc *desc = g_queue_pop_head(_context->output_buffers);

    LOGD("video decoder fill eos outbuf");

    desc->nTimeStamp = 0;
    desc->nFilledLen = 0;
    desc->nFlags |= OMX_BUFFERFLAG_EOS;

    dcodec_return_buffer_to_guest(_context, desc);
}
