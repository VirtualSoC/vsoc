#pragma once

#include "dcodec_component.h"

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // Deprecated in ffmpeg

typedef struct DCodecAudio {
    DCodecComponent base;

    // if the omx client requests a format change mid-stream
    bool mReconfiguring;

    // "Fatal signal 7 (SIGBUS)"!!! SIGBUS is because of an alignment exception
    // Don't malloc mAudioBuffer", because "NEON optimised stereo fltp to s16
    // conversion" require 16 byte alignment.
    uint8_t mAudioBuffer[AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];
    uint8_t mSilenceBuffer[CODEC_AUDIO_OUTPUT_BUFFER_SIZE];
    uint8_t *mResampledData;
    int32_t mResampledDataSize;

    // c.f. OMX specification: A buffer timestamp associates a presentation time in microseconds 
    // with the data in the buffer used to time the rendering of that data.
    OMX_TICKS mAudioClock;
    uint32_t mAudioSrcFreq;
    uint32_t mAudioTgtFreq;
    uint32_t mAudioSrcChannels;
    uint32_t mAudioTgtChannels;
    AVChannelLayout mAudioSrcChannelLayout;
    AVChannelLayout mAudioTgtChannelLayout;
    enum AVSampleFormat mAudioSrcFmt;
    enum AVSampleFormat mAudioTgtFmt;

    struct SwrContext *mSwrCtx;

#ifdef STD_DEBUG_LOG
    FILE *raw_fd;
#endif

} DCodecAudio;

DCodecComponent* dcodec_audio_init_component(enum OMX_AUDIO_CODINGTYPE codingType, NotifyCallbackFunc notify);
OMX_ERRORTYPE dcodec_audio_reset_component(DCodecComponent *_context);
OMX_ERRORTYPE dcodec_audio_destroy_component(DCodecComponent *_context);
OMX_ERRORTYPE dcodec_audio_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
OMX_ERRORTYPE dcodec_audio_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
OMX_ERRORTYPE dcodec_audio_process_this_buffer(DCodecComponent *_context, OMX_INOUT BufferDesc *desc);
