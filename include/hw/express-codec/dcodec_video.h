#pragma once

#include "dcodec_component.h"
#include "colorspace.h"

#include "hw/express-gpu/express_gpu_render.h"

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

    bool mIsDecoder;
    bool mIsAdaptive;
    bool mIsLowLatency;
    uint32_t mAdaptiveMaxWidth, mAdaptiveMaxHeight;
    uint32_t mSrcWidth, mSrcHeight;
    uint32_t mWidth, mHeight;
    OMX_COLOR_FORMATTYPE mImageFormat;
    int mFramerate;

    GHashTable *mInputMap;

    CsConverter *mCsConv;

    GLFWwindow* window;
    GLuint mDebugTexture;
    GLuint mDebugFbo;
} DCodecVideo;

DCodecComponent* dcodec_vdec_init_component(enum OMX_VIDEO_CODINGTYPE codingType, NotifyCallbackFunc notify);
OMX_ERRORTYPE dcodec_vdec_reset_component(DCodecComponent *_context);
OMX_ERRORTYPE dcodec_vdec_destroy_component(DCodecComponent *_context);
OMX_ERRORTYPE dcodec_vdec_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
OMX_ERRORTYPE dcodec_vdec_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);

DCodecComponent* dcodec_venc_init_component(enum OMX_VIDEO_CODINGTYPE codingType, NotifyCallbackFunc notify);
OMX_ERRORTYPE dcodec_venc_get_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
OMX_ERRORTYPE dcodec_venc_set_parameter(DCodecComponent *_context, OMX_IN OMX_INDEXTYPE index, OMX_PTR params);
