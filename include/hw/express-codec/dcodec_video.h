#pragma once

#include "dcodec_component.h"

#include "hw/express-gpu/express_gpu_render.h"

typedef struct CodecProfileLevel {
    OMX_U32 mProfile;
    OMX_U32 mLevel;
} CodecProfileLevel;

enum {
    kNotSupported,
    kPreferBitstream,
    kPreferContainer,
};

enum CropSettingsMode {
    kCropUnSet = 0,
    kCropSet,
    kCropChanged,
};

typedef struct DCodecVideo {
    DCodecComponent base;

    // output format
    bool mIsAdaptive;
    uint32_t mAdaptiveMaxWidth, mAdaptiveMaxHeight;
    uint32_t mWidth, mHeight;
    OMX_COLOR_FORMATTYPE mTgtPixelFormat;

    const CodecProfileLevel *mProfileLevels;
    size_t mNumProfileLevels;

    struct SwsContext *mImgConvertCtx;

    uint8_t *mVideoBuffer;
    GHashTable *mPacketMap;

    GLFWwindow* window;
    GLuint mUnpackBuffer;
    GLint mUnpackBufferSize;
    GLsync mUnpackBufferSync;
    GLuint mDebugTexture;
    GLuint mDebugFbo;
} DCodecVideo;

DCodecComponent* dcodec_video_init_component(enum OMX_VIDEO_CODINGTYPE codingType, NotifyCallbackFunc notify);
OMX_ERRORTYPE dcodec_video_reset_component(DCodecComponent *_context);
OMX_ERRORTYPE dcodec_video_destroy_component(DCodecComponent *_context);
OMX_ERRORTYPE dcodec_video_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
OMX_ERRORTYPE dcodec_video_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
OMX_ERRORTYPE dcodec_video_process_this_buffer(DCodecComponent *_context, OMX_INOUT BufferDesc *desc);
