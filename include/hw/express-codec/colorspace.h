#pragma once

#include "dcodec_component.h"

typedef enum YUVPlane {
    YUVPlane_Y = 0,
    YUVPlane_U = 1,
    YUVPlane_V = 2,
    YUVPlane_UV = 3,
} YUVPlane;

// The purpose of YUVConverter is to use
// OpenGL shaders to convert YUV images to RGB
// images that can be displayed on screen.
// Doing this on the GPU can be much faster than
// on the CPU.
typedef struct CsConverter {
    int mWidth;
    int mHeight;
    // format of the source
    enum AVPixelFormat mFormat;
    GLuint mProgram;
    GLuint mQuadVertexBuffer;
    GLuint mQuadIndexBuffer;
    GLuint mTextureY;
    GLuint mTextureU;
    GLuint mTextureV;
    GLint mUniformLocYWidthCutoff;
    GLint mUniformLocUVWidthCutoff;
    GLint mUniformLocSamplerY;
    GLint mUniformLocSamplerU;
    GLint mUniformLocSamplerV;
    GLint mAttributeLocPos;
    GLint mAttributeLocTexCoord;
    void *private; // private pointer for cuda devices
} CsConverter;

CsConverter *cs_init(const enum AVPixelFormat dst_fmt,
                     const enum AVPixelFormat src_fmt, int width, int height);
void cs_deinit(CsConverter *conv);
int cs_convert(const CsConverter *conv, GLuint dst_fbo);
