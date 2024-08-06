/**
 * vSoC audio codec device
 * 
 * Copyright (c) 2023 Jiaxing Qiu <jx.qiu@outlook.com>
 */

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"
#include "hw/express-codec/dcodec_audio.h"
#include "hw/express-codec/dcodec_shared.h"

static const struct AudioCodingMapEntry {
    OMX_AUDIO_CODINGTYPE mCodingType;
    enum AVCodecID mCodecID;
} sCodingMap[] = {
    { OMX_AUDIO_CodingAAC, AV_CODEC_ID_AAC },
    { (OMX_AUDIO_CODINGTYPE) OMX_AUDIO_CodingAC3, AV_CODEC_ID_AC3 },
    { (OMX_AUDIO_CODINGTYPE) OMX_AUDIO_CodingAPE, AV_CODEC_ID_APE },
    { OMX_AUDIO_CodingAutoDetect, AV_CODEC_ID_NONE },
    { (OMX_AUDIO_CODINGTYPE) OMX_AUDIO_CodingDTS, AV_CODEC_ID_DTS },
    { OMX_AUDIO_CodingFLAC, AV_CODEC_ID_FLAC },
    { (OMX_AUDIO_CODINGTYPE) OMX_AUDIO_CodingMP2, AV_CODEC_ID_MP2 },
    { OMX_AUDIO_CodingMP3, AV_CODEC_ID_MP3 },
    { OMX_AUDIO_CodingRA, AV_CODEC_ID_COOK },
    { OMX_AUDIO_CodingPCM, AV_CODEC_ID_PCM_S16LE },
    { OMX_AUDIO_CodingVORBIS, AV_CODEC_ID_VORBIS },
    { OMX_AUDIO_CodingWMA, AV_CODEC_ID_WMAV2 },
    { (OMX_AUDIO_CODINGTYPE) OMX_AUDIO_CodingALAC, AV_CODEC_ID_ALAC },
};

static const size_t sCodingMapLen = (sizeof(sCodingMap) / sizeof(sCodingMap[0]));

static int open_codec(DCodecComponent *_context);
static int empty_one_input_buffer(DCodecComponent *_context);
static int decode_audio(DCodecAudio *context, BufferDesc *desc);
static int resample_audio(DCodecAudio *context);
static int fill_one_output_buffer(DCodecComponent *_context);
static void adjust_audio_params(DCodecAudio *context);
static bool get_omx_channel_mapping(uint32_t numChannels, OMX_AUDIO_CHANNELTYPE map[]);

DCodecComponent* dcodec_audio_init_component(enum OMX_AUDIO_CODINGTYPE codingType, NotifyCallbackFunc notify) {
    DCodecAudio *context = g_malloc0(sizeof(DCodecAudio));
    LOGI("dcodec_audio_init_component() component %p codingType %d", context, codingType);

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

    context->base.reset_component = dcodec_audio_reset_component;
    context->base.destroy_component = dcodec_audio_destroy_component;
    context->base.get_parameter = dcodec_audio_get_parameter;
    context->base.set_parameter = dcodec_audio_set_parameter;
    context->base.open_codec = open_codec;
    context->base.empty_one_input_buffer = empty_one_input_buffer;
    context->base.fill_one_output_buffer = fill_one_output_buffer;
    context->base.fill_eos_output_buffer = dcodec_fill_eos_output_buffer;

    AVCodecContext *mCtx = context->base.mCtx;

    mCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    mCtx->codec_id = codec_id;

    // invalidate ctx
    mCtx->ch_layout.nb_channels = 2;
    mCtx->sample_rate = 44100;
    mCtx->bit_rate = 0;
    mCtx->sample_fmt = AV_SAMPLE_FMT_NONE;

    mCtx->extradata = NULL;
    mCtx->extradata_size = 0;

    context->mAudioSrcChannels = context->mAudioTgtChannels = 2;
    context->mAudioSrcFreq = context->mAudioTgtFreq = 44100;
    context->mAudioSrcFmt = AV_SAMPLE_FMT_NONE;
    context->mAudioTgtFmt = AV_SAMPLE_FMT_S16; // default to pcm_s16le
    av_channel_layout_default(&context->mAudioSrcChannelLayout, context->mAudioSrcChannels);
    av_channel_layout_default(&context->mAudioTgtChannelLayout, context->mAudioTgtChannels);

    memset(context->mSilenceBuffer, 0, CODEC_AUDIO_OUTPUT_BUFFER_SIZE);

    return (DCodecComponent *)context;
}

OMX_ERRORTYPE dcodec_audio_reset_component(DCodecComponent *_context) {
    DCodecAudio *context = (DCodecAudio *)_context;

#ifdef STD_DEBUG_LOG
    if (context->raw_fd) {
        fclose(context->raw_fd);
    }
#endif

    context->mReconfiguring = false;
    context->mResampledData = NULL;
    context->mResampledDataSize = 0;

    return dcodec_reset_component(_context);
}

OMX_ERRORTYPE dcodec_audio_destroy_component(DCodecComponent *_context) {
    DCodecAudio *context = (DCodecAudio *)_context;

#ifdef STD_DEBUG_LOG
    if (context->raw_fd) {
        fclose(context->raw_fd);
    }
#endif
    av_channel_layout_uninit(&context->mAudioSrcChannelLayout);
    av_channel_layout_uninit(&context->mAudioTgtChannelLayout);

    if (context->mSwrCtx) {
        swr_free(&context->mSwrCtx);
        context->mSwrCtx = NULL;
    }

    dcodec_deinit_component(_context);
    g_free(context);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_audio_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecAudio *context = (DCodecAudio *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    LOGD("dcodec_audio_get_parameter index:0x%x", index);

    switch ((int)index) {
        case OMX_IndexParamAudioPcm:
        {
            OMX_AUDIO_PARAM_PCMMODETYPE *profile = 
                (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            // pcm_s16le by default
            profile->bInterleaved = OMX_TRUE;
            profile->ePCMMode = OMX_AUDIO_PCMModeLinear;
            profile->eNumData = OMX_NumericalDataSigned;
            profile->eEndian = OMX_EndianLittle;
			profile->nBitPerSample = 16;

            if (profile->nPortIndex == CODEC_INPUT_PORT_INDEX) {
                switch (mCtx->codec_id) {
                    case AV_CODEC_ID_PCM_U8:
                        profile->nBitPerSample = 8;
                        profile->eNumData = OMX_NumericalDataUnsigned;
                        profile->eEndian = OMX_EndianLittle;
                        break;
                    case AV_CODEC_ID_PCM_S16LE:
                        profile->nBitPerSample = 16;
                        profile->eNumData = OMX_NumericalDataSigned;
                        profile->eEndian = OMX_EndianLittle;
                        break;
                    case AV_CODEC_ID_PCM_S24LE:
                        profile->nBitPerSample = 24;
                        profile->eNumData = OMX_NumericalDataSigned;
                        profile->eEndian = OMX_EndianLittle;
                        break;
                    case AV_CODEC_ID_PCM_S32LE:
                        profile->nBitPerSample = 32;
                        profile->eNumData = OMX_NumericalDataSigned;
                        profile->eEndian = OMX_EndianLittle;
                        break;
                    case AV_CODEC_ID_PCM_F32LE:
                        profile->nBitPerSample = 32;
                        profile->eNumData = OMX_NumericalDataFloat;
                        profile->eEndian = OMX_EndianLittle;
                        break;
                    case AV_CODEC_ID_PCM_S16BE:
                        profile->nBitPerSample = 16;
                        profile->eNumData = OMX_NumericalDataSigned;
                        profile->eEndian = OMX_EndianBig;
                        break;
                    case AV_CODEC_ID_PCM_S24BE:
                        profile->nBitPerSample = 24;
                        profile->eNumData = OMX_NumericalDataSigned;
                        profile->eEndian = OMX_EndianBig;
                        break;
                    case AV_CODEC_ID_PCM_S32BE:
                        profile->nBitPerSample = 32;
                        profile->eNumData = OMX_NumericalDataSigned;
                        profile->eEndian = OMX_EndianBig;
                        break;
                    case AV_CODEC_ID_PCM_F32BE:
                        profile->nBitPerSample = 32;
                        profile->eNumData = OMX_NumericalDataFloat;
                        profile->eEndian = OMX_EndianBig;
                        break;
                    default:
                        LOGE("error! input codec_id %x is not a supported pcm format!", mCtx->codec_id);
                        break;
                }
                profile->nChannels = mCtx->ch_layout.nb_channels;
                profile->nSamplingRate = mCtx->sample_rate;

                get_omx_channel_mapping(mCtx->ch_layout.nb_channels, profile->eChannelMapping);
            }
            else if (profile->nPortIndex == CODEC_OUTPUT_PORT_INDEX) {
                if (mCtx->ch_layout.nb_channels > 0) { // is configured
                    switch (context->mAudioTgtFmt) {
                        case AV_SAMPLE_FMT_U8:
                            profile->nBitPerSample = 8;
                            profile->eNumData = OMX_NumericalDataUnsigned;
                            break;
                        case AV_SAMPLE_FMT_S16:
                            profile->nBitPerSample = 16;
                            profile->eNumData = OMX_NumericalDataSigned;
                            break;
                        case AV_SAMPLE_FMT_S32:
                            profile->nBitPerSample = 32;
                            profile->eNumData = OMX_NumericalDataSigned;
                            break;
                        default:
                            profile->nBitPerSample = 16;
                            profile->eNumData = OMX_NumericalDataSigned;
                            break;
                    }
                    get_omx_channel_mapping(context->mAudioTgtChannels, profile->eChannelMapping);
                }

                profile->nChannels = context->mAudioTgtChannels;
                profile->nSamplingRate = context->mAudioTgtFreq;
            }
            else {
                return OMX_ErrorBadPortIndex;
            }

            // mCtx has been updated(adjust_audio_params)!
            LOGD("get pcm params on port %d, nChannels:%u, nSamplingRate:%u, nBitPerSample:%u, interleaved:%d, eNumData:%d, eEndian:%d, ePCMMode:%d",
                   profile->nPortIndex, profile->nChannels, profile->nSamplingRate, profile->nBitPerSample, profile->bInterleaved, profile->eNumData, profile->eEndian, profile->ePCMMode);

            break;
        }

        case OMX_IndexParamAudioAac:
        {
            OMX_AUDIO_PARAM_AACPROFILETYPE *profile =
                (OMX_AUDIO_PARAM_AACPROFILETYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nBitRate = 0;
            profile->nAudioBandWidth = 0;
            profile->nAACtools = 0;
            profile->nAACERtools = 0;
            profile->eAACProfile = OMX_AUDIO_AACObjectMain;
            profile->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4FF;
            profile->eChannelMode = OMX_AUDIO_ChannelModeStereo;

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSampleRate = mCtx->sample_rate;

            break;
                        }

        case OMX_IndexParamAudioMp3:
        {
            OMX_AUDIO_PARAM_MP3TYPE *profile =
                (OMX_AUDIO_PARAM_MP3TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nBitRate = 0;
            profile->nAudioBandWidth = 0;
            profile->eChannelMode = OMX_AUDIO_ChannelModeStereo;
            profile->eFormat = OMX_AUDIO_MP3StreamFormatMP1Layer3;

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSampleRate = mCtx->sample_rate;

            break;
        }
        case OMX_IndexParamAudioVorbis:
        {
            OMX_AUDIO_PARAM_VORBISTYPE *profile =
                (OMX_AUDIO_PARAM_VORBISTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nBitRate = 0;
            profile->nMinBitRate = 0;
            profile->nMaxBitRate = 0;
            profile->nAudioBandWidth = 0;
            profile->nQuality = 3;
            profile->bManaged = OMX_FALSE;
            profile->bDownmix = OMX_FALSE;

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSampleRate = mCtx->sample_rate;

            break;
        }

        case OMX_IndexParamAudioWma:
        {
            OMX_AUDIO_PARAM_WMATYPE *profile =
                (OMX_AUDIO_PARAM_WMATYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->eFormat = OMX_AUDIO_WMAFormatUnused;

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSamplingRate = mCtx->sample_rate;

            profile->nBlockAlign = mCtx->block_align;
            profile->nBitRate = mCtx->bit_rate;

            break;
        }

        case OMX_IndexParamAudioRa:
        {
            OMX_AUDIO_PARAM_RATYPE *profile =
                (OMX_AUDIO_PARAM_RATYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->eFormat = OMX_AUDIO_RAFormatUnused;

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSamplingRate = mCtx->sample_rate;

            profile->nNumRegions = mCtx->block_align;

            break;
        }

        case OMX_IndexParamAudioFlac:
        {
            OMX_AUDIO_PARAM_FLACTYPE *profile =
                (OMX_AUDIO_PARAM_FLACTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSampleRate = mCtx->sample_rate;
            profile->nCompressionLevel = mCtx->bits_per_raw_sample;

            break;
        }

        case OMX_IndexParamAudioMp2:
        {
            OMX_AUDIO_PARAM_MP2TYPE *profile =
                (OMX_AUDIO_PARAM_MP2TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSampleRate = mCtx->sample_rate;

            break;
        }

        case OMX_IndexParamAudioAndroidAc3:
        {
            OMX_AUDIO_PARAM_ANDROID_AC3TYPE *profile =
                (OMX_AUDIO_PARAM_ANDROID_AC3TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSampleRate = mCtx->sample_rate;

            break;
        }


        case OMX_IndexParamAudioAc3:
        {
            OMX_AUDIO_PARAM_AC3TYPE *profile =
                (OMX_AUDIO_PARAM_AC3TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSamplingRate = mCtx->sample_rate;

            break;
        }

        case OMX_IndexParamAudioAlac:
        {
            OMX_AUDIO_PARAM_ALACTYPE *profile =
                (OMX_AUDIO_PARAM_ALACTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSamplingRate = mCtx->sample_rate;

            profile->nBitsPerSample = mCtx->bits_per_coded_sample;

            break;
        }

        case OMX_IndexParamAudioApe:
        {
            OMX_AUDIO_PARAM_APETYPE *profile =
                (OMX_AUDIO_PARAM_APETYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSamplingRate = mCtx->sample_rate;

            profile->nBitsPerSample = mCtx->bits_per_coded_sample;

            break;
        }

        case OMX_IndexParamAudioDts:
        {
            OMX_AUDIO_PARAM_DTSTYPE *profile =
                (OMX_AUDIO_PARAM_DTSTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSamplingRate = mCtx->sample_rate;

            break;
        }

        case OMX_IndexParamAudioFFmpeg:
        {
            OMX_AUDIO_PARAM_FFMPEGTYPE *profile =
                (OMX_AUDIO_PARAM_FFMPEGTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            profile->eCodecId = mCtx->codec_id;
            profile->nBitRate = mCtx->bit_rate;
            profile->nBlockAlign = mCtx->block_align;

            profile->nBitsPerSample = mCtx->bits_per_raw_sample;
            profile->eSampleFormat = mCtx->sample_fmt;

            profile->nChannels = mCtx->ch_layout.nb_channels;
            profile->nSampleRate = mCtx->sample_rate;

            break;
        }
        default: {
            LOGE("dcodec_audio_get_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE dcodec_audio_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params) {
    DCodecAudio *context = (DCodecAudio *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    LOGD("dcodec_audio_set_parameter index:0x%x", index);

    switch ((int)index) {
        case OMX_IndexParamAudioPcm:
        {
            const OMX_AUDIO_PARAM_PCMMODETYPE *profile =
                (const OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            enum AVSampleFormat format = AV_SAMPLE_FMT_NONE;
            enum AVCodecID pcm_id = AV_CODEC_ID_PCM_S16LE;
            switch (profile->nBitPerSample) {
                case 8:
                    format = AV_SAMPLE_FMT_U8;
                    pcm_id = AV_CODEC_ID_PCM_U8;
                    break;
                case 16:
                    format = AV_SAMPLE_FMT_S16;
                    pcm_id = profile->eEndian == OMX_EndianLittle ? AV_CODEC_ID_PCM_S16LE: AV_CODEC_ID_PCM_S16BE;
                    break;
                case 24:
                    format = AV_SAMPLE_FMT_S32;
                    pcm_id = profile->eEndian == OMX_EndianLittle ? AV_CODEC_ID_PCM_S24LE: AV_CODEC_ID_PCM_S24BE;
                    break;
                case 32:
                    if (profile->eNumData == OMX_NumericalDataFloat) {
                        format = AV_SAMPLE_FMT_FLT;
                        pcm_id = profile->eEndian == OMX_EndianLittle ? AV_CODEC_ID_PCM_F32LE : AV_CODEC_ID_PCM_F32BE;
                    }
                    else {
                        format = AV_SAMPLE_FMT_S32;
                        pcm_id = profile->eEndian == OMX_EndianLittle ? AV_CODEC_ID_PCM_S32LE : AV_CODEC_ID_PCM_S32BE;
                    }
                    break;
                default:
                    LOGW("Unknown PCM encoding, assuming pcm_s16le");
                    format = AV_SAMPLE_FMT_S16;
                    pcm_id = AV_CODEC_ID_PCM_S16LE;
            }

            if (profile->nPortIndex == CODEC_INPUT_PORT_INDEX) {
                mCtx->codec_id = pcm_id;
                mCtx->sample_rate = profile->nSamplingRate;
                mCtx->ch_layout.nb_channels = profile->nChannels;
            }
            else if (profile->nPortIndex == CODEC_OUTPUT_PORT_INDEX) {
                context->mAudioTgtFmt = format;
                context->mAudioTgtFreq = profile->nSamplingRate;
                context->mAudioTgtChannels = profile->nChannels;
            }
            else {
                return OMX_ErrorBadPortIndex;
            }

            LOGD("set OMX_IndexParamAudioPcm on port %d, nChannels:%u, "
                    "nSampleRate:%u, nBitPerSample:%u, interleaved:%d, eNumData:%d, eEndian:%d, ePCMMode:%d", profile->nPortIndex,
                    profile->nChannels, profile->nSamplingRate,
                    profile->nBitPerSample, profile->bInterleaved, profile->eNumData, profile->eEndian, profile->ePCMMode);
            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAac:
        {
            const OMX_AUDIO_PARAM_AACPROFILETYPE *profile =
                (const OMX_AUDIO_PARAM_AACPROFILETYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSampleRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioAac, nChannels:%u, nSampleRate:%u",
                profile->nChannels, profile->nSampleRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioMp3:
        {
            const OMX_AUDIO_PARAM_MP3TYPE *profile =
                (const OMX_AUDIO_PARAM_MP3TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSampleRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioMp3, nChannels:%u, nSampleRate:%u",
                profile->nChannels, profile->nSampleRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioVorbis:
        {
            const OMX_AUDIO_PARAM_VORBISTYPE *profile =
                (const OMX_AUDIO_PARAM_VORBISTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSampleRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioVorbis, "
                    "nChannels=%u, nSampleRate=%u, nBitRate=%u, "
                    "nMinBitRate=%u, nMaxBitRate=%u",
                profile->nChannels, profile->nSampleRate,
                profile->nBitRate, profile->nMinBitRate,
                profile->nMaxBitRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioWma:
        {
            OMX_AUDIO_PARAM_WMATYPE *profile =
                (OMX_AUDIO_PARAM_WMATYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            if (profile->eFormat == OMX_AUDIO_WMAFormat7) {
               mCtx->codec_id = AV_CODEC_ID_WMAV2;
            } else if (profile->eFormat == OMX_AUDIO_WMAFormat8) {
               mCtx->codec_id = AV_CODEC_ID_WMAPRO;
            } else if (profile->eFormat == OMX_AUDIO_WMAFormat9) {
               mCtx->codec_id = AV_CODEC_ID_WMALOSSLESS;
            } else {
                LOGE("unsupported wma codec: 0x%x", profile->eFormat);
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSamplingRate;

            // wmadec needs bitrate, block_align
            mCtx->bit_rate = profile->nBitRate;
            mCtx->block_align = profile->nBlockAlign;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioWma, nChannels:%u, "
                    "nSampleRate:%u, nBitRate:%u, nBlockAlign:%u",
                profile->nChannels, profile->nSamplingRate,
                profile->nBitRate, profile->nBlockAlign);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioRa:
        {
            OMX_AUDIO_PARAM_RATYPE *profile =
                (OMX_AUDIO_PARAM_RATYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSamplingRate;

            // FIXME, HACK!!!, I use the nNumRegions parameter pass blockAlign!!!
            // the cook audio codec need blockAlign!
            mCtx->block_align = profile->nNumRegions;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioRa, nChannels:%u, "
                    "nSampleRate:%u, nBlockAlign:%d",
                profile->nChannels, profile->nSamplingRate, mCtx->block_align);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioFlac:
        {
            OMX_AUDIO_PARAM_FLACTYPE *profile =
                (OMX_AUDIO_PARAM_FLACTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSampleRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioFlac, nChannels:%u, nSampleRate:%u ",
                profile->nChannels, profile->nSampleRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioMp2:
        {
            OMX_AUDIO_PARAM_MP2TYPE *profile =
                (OMX_AUDIO_PARAM_MP2TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSampleRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioMp2, nChannels:%u, nSampleRate:%u",
                profile->nChannels, profile->nSampleRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAc3:
        {
            OMX_AUDIO_PARAM_AC3TYPE *profile =
                (OMX_AUDIO_PARAM_AC3TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSamplingRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioAc3, nChannels:%u, nSampleRate:%u",
                profile->nChannels, profile->nSamplingRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAndroidAc3:
        {
            OMX_AUDIO_PARAM_ANDROID_AC3TYPE *profile =
                (OMX_AUDIO_PARAM_ANDROID_AC3TYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSampleRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioAndroidAc3, nChannels:%u, nSampleRate:%u",
                profile->nChannels, profile->nSampleRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAlac:
        {
            OMX_AUDIO_PARAM_ALACTYPE *profile =
                (OMX_AUDIO_PARAM_ALACTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSamplingRate;
            mCtx->bits_per_coded_sample = profile->nBitsPerSample;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioAlac, nChannels:%u, "
                    "nSampleRate:%u, nBitsPerSample:%u",
                profile->nChannels, profile->nSamplingRate,
                profile->nBitsPerSample);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioApe:
        {
            OMX_AUDIO_PARAM_APETYPE *profile =
                (OMX_AUDIO_PARAM_APETYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSamplingRate;
            mCtx->bits_per_coded_sample = profile->nBitsPerSample;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioApe, nChannels:%u, "
                    "nSampleRate:%u, nBitsPerSample:%u",
                profile->nChannels, profile->nSamplingRate,
                profile->nBitsPerSample);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioDts:
        {
            OMX_AUDIO_PARAM_DTSTYPE *profile =
                (OMX_AUDIO_PARAM_DTSTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->sample_rate = profile->nSamplingRate;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioDts, nChannels:%u, nSampleRate:%u",
                profile->nChannels, profile->nSamplingRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioFFmpeg:
        {
            OMX_AUDIO_PARAM_FFMPEGTYPE *profile =
                (OMX_AUDIO_PARAM_FFMPEGTYPE *)params;

            if (profile->nPortIndex != CODEC_INPUT_PORT_INDEX) {
                return OMX_ErrorUndefined;
            }

            mCtx->codec_id = (enum AVCodecID)profile->eCodecId;
            mCtx->ch_layout.nb_channels = profile->nChannels;
            mCtx->bit_rate = profile->nBitRate;
            mCtx->sample_rate = profile->nSampleRate;
            mCtx->block_align = profile->nBlockAlign;
            mCtx->bits_per_coded_sample = profile->nBitsPerSample;
            mCtx->sample_fmt = (enum AVSampleFormat)profile->eSampleFormat;

            adjust_audio_params(context);

            LOGD("set OMX_IndexParamAudioFFmpeg, "
                "eCodecId:%d(%s), nChannels:%u, nBitRate:%u, "
                "nBitsPerSample:%u, nSampleRate:%u, "
                "nBlockAlign:%u, eSampleFormat:%u(%s)",
                profile->eCodecId, avcodec_get_name(mCtx->codec_id),
                profile->nChannels, profile->nBitRate,
                profile->nBitsPerSample, profile->nSampleRate,
                profile->nBlockAlign, profile->eSampleFormat,
                av_get_sample_fmt_name(mCtx->sample_fmt));
            return OMX_ErrorNone;
        }

        default: {
            LOGE("dcodec_audio_set_parameter unrecognized index 0x%x!", index);
            return OMX_ErrorUnsupportedIndex;
        }
    }
}

static int open_codec(DCodecComponent *_context) {
    DCodecAudio *context = (DCodecAudio *)_context;
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
        LOGE("ffmpeg audio decoder failed to find codec");
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

#ifdef STD_DEBUG_LOG
    mCtx->debug = 1;
#endif

    av_channel_layout_default(&mCtx->ch_layout, mCtx->ch_layout.nb_channels);

    LOGD("open ffmpeg audio decoder (%s), mCtx sample_rate: %d, channels: %d",
           avcodec_get_name(mCtx->codec_id),
           mCtx->sample_rate, mCtx->ch_layout.nb_channels);

    int err = avcodec_open2(mCtx, mCtx->codec, NULL);
    if (err < 0) {
        LOGE("ffmpeg audio decoder failed to initialize (%s).", av_err2str(err));
        return ERR_CODEC_OPEN_FAILED;
    }

    LOGI("open ffmpeg audio decoder (%s) success, mCtx sample_rate: %d, "
            "channels: %d, sample_fmt: %s, bits_per_coded_sample: %d, bits_per_raw_sample: %d",
            avcodec_get_name(mCtx->codec_id),
            mCtx->sample_rate, mCtx->ch_layout.nb_channels,
            av_get_sample_fmt_name(mCtx->sample_fmt),
            mCtx->bits_per_coded_sample, mCtx->bits_per_raw_sample);

    context->mAudioSrcFmt = mCtx->sample_fmt;
    context->mAudioSrcFreq = mCtx->sample_rate;
    context->mAudioSrcChannels = mCtx->ch_layout.nb_channels;
    av_channel_layout_copy(&context->mAudioSrcChannelLayout, &mCtx->ch_layout);

#ifdef STD_DEBUG_LOG
    // print some extra info about the decoder
    char buf[32] = {0};
    av_channel_layout_describe(&mCtx->ch_layout, buf, sizeof(buf));
    LOGD("  channel layout %s, time base {%d, %d}", buf, mCtx->time_base.num, mCtx->time_base.den);

    char name[64] = {0};
    snprintf(name, 64, "log/audio-%x-%s-%d-%s-%d.pcm", mCtx->codec_id, av_get_sample_fmt_name(mCtx->sample_fmt), mCtx->sample_rate, buf, (int)CURRENT_TID());
    context->raw_fd = fopen(name, "wb");
    if (context->raw_fd == NULL) {
        LOGE("error opening file %s for writing!", name);
    }
#endif

    return ERR_OK;
}

/**
 * @brief decode an audio frame using the first buffer in the input queue.
*/
static int empty_one_input_buffer(DCodecComponent *_context) {
    DCodecAudio *context = (DCodecAudio *)_context;
    AVCodecContext *mCtx = _context->mCtx;
    int ret = 0;

    BufferDesc *desc = g_queue_peek_head(_context->input_buffers);

    if (desc->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
        LOGW("extradata config ignored when the decoder is open");
        dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
        return ERR_OK;
    }

    ret = decode_audio(context, desc);

    // a negative error code is returned if an error occurred during decoding
    if (ret < 0) {
        LOGW("audio decoder error %d, we skip the frame and play silence instead", ret);
        context->mResampledData = context->mSilenceBuffer;
        context->mResampledDataSize = CODEC_AUDIO_OUTPUT_BUFFER_SIZE;
        if (desc->nTimeStamp != AV_NOPTS_VALUE) {
            context->mAudioClock = desc->nTimeStamp;
        }
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
            ret = decode_audio(context, NULL);
            CHECK_EQ(ret, ERR_OK);
        }
        avcodec_flush_buffers(mCtx);
        _context->mStatus = INPUT_EOS_SEEN;
    }

    dcodec_return_buffer(_context, g_queue_pop_head(_context->input_buffers));
    return ERR_OK;
}

/**
 * @brief decode and resample (if necessary) an audio frame
 * @param desc the buffer descriptor. If NULL, the ffmpeg codec will be flushed.
 * @return ERR_OK on success, or a non-zero error code.
*/
static int decode_audio(DCodecAudio *context, BufferDesc *desc) {
    AVPacket *mPkt = context->base.mPkt;
    AVCodecContext *mCtx = context->base.mCtx;
    int ret = ERR_OK;

    if (desc == NULL) {
        mPkt->data = NULL;
        mPkt->size = 0;
    }
    else if (desc->nFilledLen == 0) { // empty packets will cause mischief with ffmpeg
        return ERR_NO_FRM;
    }
    else {
        CHECK(desc->type & CODEC_BUFFER_TYPE_GUEST_MEM);
        CHECK_LE(desc->nFilledLen, sizeof(context->mAudioBuffer));
        read_from_guest_mem(desc->data, context->mAudioBuffer, desc->nOffset, desc->nFilledLen); // avoid memcpys caused by EAGAIN
        mPkt->data = context->mAudioBuffer;
        mPkt->size = desc->nFilledLen;
        mPkt->pts = desc->nTimeStamp;
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
        return ERR_CODING_FAILED;
    }

// #ifdef STD_DEBUG_LOG
//     if (context->raw_fd > 0) {
//         fwrite(context->mAudioBuffer, 1, desc->nFilledLen, context->raw_fd);
//     }
// #endif

    return ERR_OK;
}

static int resample_audio(DCodecAudio *context) {
    AVFrame *mFrame = context->base.mFrame;
    size_t dataSize = 0;
    // todo: check if we really need to resample
    if (context->mAudioTgtFmt < 0 || context->mAudioTgtFmt > AV_SAMPLE_FMT_NB) {
        LOGE("resample_audio: error! target format 0x%x not supported!", context->mAudioTgtFmt);
        return ERR_INVALID_PARAM;
    }

    dataSize = av_samples_get_buffer_size(NULL, mFrame->ch_layout.nb_channels,
            mFrame->nb_samples, (enum AVSampleFormat)mFrame->format, 1);

    // create if we're reconfiguring, if the format changed mid-stream, or
    // if the output format is actually different
    if ((context->mReconfiguring && context->mSwrCtx) || (!(context->mSwrCtx)
            && (mFrame->format != context->mAudioSrcFmt
                || (unsigned int)mFrame->sample_rate != context->mAudioSrcFreq
                || context->mAudioSrcFmt != context->mAudioTgtFmt
                || context->mAudioSrcFreq != context->mAudioTgtFreq))) {
        LOGI("format/configuration change detected, configuring audio resampler.");

        swr_alloc_set_opts2(&context->mSwrCtx, &context->mAudioTgtChannelLayout, context->mAudioTgtFmt, context->mAudioTgtFreq, &mFrame->ch_layout, (enum AVSampleFormat)mFrame->format, mFrame->sample_rate, 0, NULL);

        if (!(context->mSwrCtx) || swr_init(context->mSwrCtx) < 0) {
            LOGE("Cannot create sample rate converter for conversion "
                    "of %d Hz %s %d channels to %d Hz %s %d channels!",
                    mFrame->sample_rate,
                    av_get_sample_fmt_name((enum AVSampleFormat)mFrame->format),
                    mFrame->ch_layout.nb_channels,
                    context->mAudioTgtFreq,
                    av_get_sample_fmt_name(context->mAudioTgtFmt),
                    context->mAudioTgtChannels);
            return ERR_SWR_INIT_FAILED;
        }

        LOGI("Create sample rate converter for conversion "
                "of %d Hz %s %d channels "
                "to %d Hz %s %d channels!",
                mFrame->sample_rate,
                av_get_sample_fmt_name((enum AVSampleFormat)mFrame->format),
                mFrame->ch_layout.nb_channels,
                context->mAudioTgtFreq,
                av_get_sample_fmt_name(context->mAudioTgtFmt),
                context->mAudioTgtChannels);

#ifdef STD_DEBUG_LOG
        char src_layout_name[32] = {0};
        char tgt_layout_name[32] = {0};
        av_channel_layout_describe(&mCtx->ch_layout, src_layout_name, sizeof(src_layout_name));
        av_channel_layout_describe(&context->mAudioTgtChannelLayout, tgt_layout_name, sizeof(tgt_layout_name));
        LOGD("  channel layout: %s -> %s", src_layout_name, tgt_layout_name);
#endif
        // source format change detected, update context to the new format
        context->mAudioSrcFreq = mFrame->sample_rate;
        context->mAudioSrcFmt = (enum AVSampleFormat)mFrame->format;
        context->mAudioSrcChannels = mFrame->ch_layout.nb_channels;
        av_channel_layout_copy(&context->mAudioSrcChannelLayout, &mFrame->ch_layout);
        context->mReconfiguring = false;
    }

    struct SwrContext *mSwrCtx = context->mSwrCtx;

    if (mSwrCtx) {
        const uint8_t **in = (const uint8_t **)mFrame->extended_data;
        uint8_t *out[] = {context->mAudioBuffer};
        int out_count = sizeof(context->mAudioBuffer) / context->mAudioTgtChannels / av_get_bytes_per_sample(context->mAudioTgtFmt);
        int out_size  = av_samples_get_buffer_size(NULL, context->mAudioTgtChannels, out_count, context->mAudioTgtFmt, 0);
        int len2 = 0;
        if (out_size < 0) {
            LOGE("av_samples_get_buffer_size() failed");
            return ERR_INVALID_PARAM;
        }

        len2 = swr_convert(mSwrCtx, out, out_count, in, mFrame->nb_samples);
        if (len2 < 0) {
            LOGE("audio_resample() failed");
            return ERR_RESAMPLE_FAILED;
        }
        if (len2 == out_count) {
            LOGE("warning: audio buffer is probably too small");
            swr_init(mSwrCtx);
        }
        context->mResampledData = context->mAudioBuffer;
        context->mResampledDataSize = len2 * context->mAudioTgtChannels * av_get_bytes_per_sample(context->mAudioTgtFmt);

        LOGD("audio decoder w/ resample, mFrame->nb_samples:%d, len2:%d, mResampledDataSize:%d, "
                "src channel:%u, src fmt:%s, tgt channel:%u, tgt fmt:%s",
                mFrame->nb_samples, len2, context->mResampledDataSize,
                mFrame->ch_layout.nb_channels,
                av_get_sample_fmt_name((enum AVSampleFormat)mFrame->format),
                context->mAudioTgtChannels,
                av_get_sample_fmt_name(context->mAudioTgtFmt));
    } else {
        context->mResampledData = mFrame->data[0];
        context->mResampledDataSize = dataSize;

        LOGD("audio decoder w/o resample, "
            "nb_samples (before resample): %d, mResampledDataSize: %d",
            mFrame->nb_samples, context->mResampledDataSize);
    }

    return ERR_OK;
}

static int fill_one_output_buffer(DCodecComponent *_context) {
    DCodecAudio *context = (DCodecAudio *)_context;
    AVCodecContext *mCtx = context->base.mCtx;
    AVFrame *mFrame = context->base.mFrame;
    int ret = ERR_OK;

    if (context->mResampledDataSize == 0) {
        // read a new frame
        ret = avcodec_receive_frame(mCtx, mFrame);
        if (ret == AVERROR_EOF && _context->mStatus != OUTPUT_EOS_SENT) {
            _context->fill_eos_output_buffer(_context);
            _context->mStatus = OUTPUT_EOS_SENT;
            return ERR_OK;
        }
        else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return ERR_NO_FRM;
        }
        else if (ret < 0) {
            LOGW("avcodec_receive_frame error %d, we skip the frame and play silence instead", ret);
            context->mResampledData = context->mSilenceBuffer;
            context->mResampledDataSize = CODEC_AUDIO_OUTPUT_BUFFER_SIZE;
        }
        if (mFrame->pts != AV_NOPTS_VALUE) {
            context->mAudioClock = mFrame->pts;
        }
        ret = resample_audio(context);
        if (ret < 0) {
            LOGW("resample_audio error %d, we skip the frame and play silence instead", ret);
            context->mResampledData = context->mSilenceBuffer;
            context->mResampledDataSize = CODEC_AUDIO_OUTPUT_BUFFER_SIZE;
        }
    }

    if (context->mResampledData == 0) { // in case the audio resampler fails
        return ERR_NO_FRM;
    }

    BufferDesc *desc = g_queue_pop_head(_context->output_buffers);
    size_t copy = min(context->mResampledDataSize, desc->nAllocLen);

    desc->nOffset = 0;
    desc->nFilledLen = copy;
    desc->nTimeStamp = context->mAudioClock;
    write_to_guest_mem(desc->data, context->mResampledData, 0, copy);

#ifdef STD_DEBUG_LOG
    if (context->raw_fd > 0) {
        fwrite(context->mResampledData, 1, copy, context->raw_fd);
    }
#endif

    //update mResampledSize
    context->mResampledData += copy;
    context->mResampledDataSize -= copy;

    //update audio pts
    size_t samples = copy / av_get_bytes_per_sample(context->mAudioTgtFmt) / context->mAudioTgtChannels;
    context->mAudioClock = context->mAudioClock + samples * 1000000ll / context->mAudioTgtFreq;

    LOGD("fill_one_output_buffer() on buffer type %x id %" PRIx64 " nAllocLen %u "
         "nFilledLen %u nOffset %u nTimeStamp %lld nFlags %x",
         desc->type, desc->id, desc->nAllocLen, desc->nFilledLen, desc->nOffset,
         desc->nTimeStamp, desc->nFlags);

    dcodec_return_buffer(_context, desc);
    return ERR_OK;
}

static void adjust_audio_params(DCodecAudio *context) {
    AVCodecContext *mCtx = context->base.mCtx;
    context->mReconfiguring = mCtx->ch_layout.nb_channels > 0; // is configured

    // let android audio mixer to downmix if there is no multichannel output
    // and use number of channels from the source file, useful for HDMI/offload output
    context->mAudioTgtChannels = mCtx->ch_layout.nb_channels;
    context->mAudioTgtFreq = mCtx->sample_rate;

    av_channel_layout_default(&context->mAudioTgtChannelLayout, context->mAudioTgtChannels);

    LOGD("adjust_audio_params: target pcm format [channels=%d freq=%d fmt=%s]",
            mCtx->ch_layout.nb_channels, mCtx->sample_rate, av_get_sample_fmt_name(context->mAudioTgtFmt));
}

// c.f. android ACodec::getOMXChannelMapping
static bool get_omx_channel_mapping(uint32_t numChannels, OMX_AUDIO_CHANNELTYPE map[]) {
    switch (numChannels) {
        case 1:
            map[0] = OMX_AUDIO_ChannelCF;
            break;
        case 2:
            map[0] = OMX_AUDIO_ChannelLF;
            map[1] = OMX_AUDIO_ChannelRF;
            break;
        case 3:
            map[0] = OMX_AUDIO_ChannelLF;
            map[1] = OMX_AUDIO_ChannelRF;
            map[2] = OMX_AUDIO_ChannelCF;
            break;
        case 4:
            map[0] = OMX_AUDIO_ChannelLF;
            map[1] = OMX_AUDIO_ChannelRF;
            map[2] = OMX_AUDIO_ChannelLR;
            map[3] = OMX_AUDIO_ChannelRR;
            break;
        case 5:
            map[0] = OMX_AUDIO_ChannelLF;
            map[1] = OMX_AUDIO_ChannelRF;
            map[2] = OMX_AUDIO_ChannelCF;
            map[3] = OMX_AUDIO_ChannelLR;
            map[4] = OMX_AUDIO_ChannelRR;
            break;
        case 6:
            map[0] = OMX_AUDIO_ChannelLF;
            map[1] = OMX_AUDIO_ChannelRF;
            map[2] = OMX_AUDIO_ChannelCF;
            map[3] = OMX_AUDIO_ChannelLFE;
            map[4] = OMX_AUDIO_ChannelLR;
            map[5] = OMX_AUDIO_ChannelRR;
            break;
        case 7:
            map[0] = OMX_AUDIO_ChannelLF;
            map[1] = OMX_AUDIO_ChannelRF;
            map[2] = OMX_AUDIO_ChannelCF;
            map[3] = OMX_AUDIO_ChannelLFE;
            map[4] = OMX_AUDIO_ChannelLR;
            map[5] = OMX_AUDIO_ChannelRR;
            map[6] = OMX_AUDIO_ChannelCS;
            break;
        case 8:
            map[0] = OMX_AUDIO_ChannelLF;
            map[1] = OMX_AUDIO_ChannelRF;
            map[2] = OMX_AUDIO_ChannelCF;
            map[3] = OMX_AUDIO_ChannelLFE;
            map[4] = OMX_AUDIO_ChannelLR;
            map[5] = OMX_AUDIO_ChannelRR;
            map[6] = OMX_AUDIO_ChannelLS;
            map[7] = OMX_AUDIO_ChannelRS;
            break;
        default:
            LOGE("error! get_omx_channel_mapping channel count %d not supported!", numChannels);
            return false;
    }
    return true;
}
