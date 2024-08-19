/**
 * vSoC ISP device
 * 
 * Copyright (c) 2024 Jiaxing Qiu <jx.qiu@outlook.com>
 */

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"
#include "hw/express-codec/colorspace.h"


static bool isInterleaved(enum AVPixelFormat format) {
    switch (format) {
    case AV_PIX_FMT_NV12:
        return true;
    case AV_PIX_FMT_YUV420P:
        return false;
    default:
        LOGE("isInterleaved invalid format %d", format);
        return false;
    }
}

// getYUVOffsets(), given a YUV-formatted buffer that is arranged
// according to the spec
// https://developer.android.com/reference/android/graphics/ImageFormat.html#YUV
// In particular, Android YUV widths are aligned to 16 pixels.
// Inputs:
// |yv12|: the YUV-formatted buffer
// Outputs:
// |yOffsetBytes|: offset into |yv12| of the start of the Y component
// |uOffsetBytes|: offset into |yv12| of the start of the U component
// |vOffsetBytes|: offset into |yv12| of the start of the V component
static int getYUVOffsets(int width,
                          int height,
                          enum AVPixelFormat format,
                          uint32_t* yWidth,
                          uint32_t* yHeight,
                          uint32_t* yOffsetBytes,
                          uint32_t* yStridePixels,
                          uint32_t* yStrideBytes,
                          uint32_t* uWidth,
                          uint32_t* uHeight,
                          uint32_t* uOffsetBytes,
                          uint32_t* uStridePixels,
                          uint32_t* uStrideBytes,
                          uint32_t* vWidth,
                          uint32_t* vHeight,
                          uint32_t* vOffsetBytes,
                          uint32_t* vStridePixels,
                          uint32_t* vStrideBytes) {
    switch (format) {
        case AV_PIX_FMT_YUV420P: {
            *yWidth = width;
            *yHeight = height;
            *yOffsetBytes = 0;
            *yStridePixels = width;
            *yStrideBytes = *yStridePixels;

            *uWidth = width / 2;
            *uHeight = height / 2;
            *uOffsetBytes = (*yStrideBytes) * (*yHeight);
            *uStridePixels = (*yStridePixels) / 2;
            *uStrideBytes = *uStridePixels;

            *vWidth = width / 2;
            *vHeight = height / 2;
            *vOffsetBytes = (*uOffsetBytes) + ((*uStrideBytes) * (*uHeight));
            *vStridePixels = (*yStridePixels) / 2;
            *vStrideBytes = (*vStridePixels);
            break;
        }
        case AV_PIX_FMT_NV12: {
            *yWidth = width;
            *yHeight = height;
            *yOffsetBytes = 0;
            *yStridePixels = width;
            *yStrideBytes = *yStridePixels;

            *uWidth = width / 2;
            *uHeight = height / 2;
            *uOffsetBytes = (*yStrideBytes) * (*yHeight);
            *uStridePixels = (*yStridePixels) / 2;
            *uStrideBytes = *uStridePixels;

            *vWidth = width / 2;
            *vHeight = height / 2;
            *vOffsetBytes = (*uOffsetBytes) + 1;
            *vStridePixels = (*yStridePixels) / 2;
            *vStrideBytes = (*vStridePixels);
            break;
        }
        case AV_PIX_FMT_P010: {
            *yWidth = width;
            *yHeight = height;
            *yOffsetBytes = 0;
            *yStridePixels = width;
            *yStrideBytes = (*yStridePixels) * /*bytes per pixel=*/2;

            *uWidth = width / 2;
            *uHeight = height / 2;
            *uOffsetBytes = (*yStrideBytes) * (*yHeight);
            *uStridePixels = (*uWidth);
            *uStrideBytes = *uStridePixels  * /*bytes per pixel=*/2;

            *vWidth = width / 2;
            *vHeight = height / 2;
            *vOffsetBytes = (*uOffsetBytes) + 2;
            *vStridePixels = (*vWidth);
            *vStrideBytes = (*vStridePixels)  * /*bytes per pixel=*/2;
            break;
        }
        default: {
            LOGE("Unknown format: 0x%x", format);
            return ERR_INVALID_PARAM;
        }
    }
    return ERR_OK;
}

static void getGLTextureFormat(YUVPlane plane, GLint *internalformat, GLenum *format, GLenum *type) {
    switch (plane) {
    case YUVPlane_Y: {
        *internalformat = GL_R8;
        *format = GL_RED;
        *type = GL_UNSIGNED_BYTE;
        return;
    }
    case YUVPlane_U:
    case YUVPlane_V:
    case YUVPlane_UV: {
        *internalformat = GL_RG8;
        *format = GL_RG;
        *type = GL_UNSIGNED_BYTE;
        return;
    }
    default:
        LOGE("Invalid plane %d", plane);
    }
}

// Allocates an OpenGL texture that is large enough for a single plane of
// a YUV buffer of the given format and returns the texture name in the
// `outTextureName` argument.
static void createYUVGLTex(GLenum textureUnit,
                    GLsizei width,
                    GLsizei height,
                    enum AVPixelFormat format,
                    YUVPlane plane,
                    GLuint* outTextureName) {
    LOGD("w:%d h:%d format:%d plane:%d", width, height, format, plane);

    glActiveTexture(textureUnit);
    glGenTextures(1, outTextureName);
    glBindTexture(GL_TEXTURE_2D, *outTextureName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLint unprevAlignment = 0;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unprevAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLint textureFormat;
    GLenum pixelFormat, pixelType;
    getGLTextureFormat(plane, &textureFormat, &pixelFormat, &pixelType);
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, pixelFormat, pixelType, NULL);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unprevAlignment);
    glActiveTexture(GL_TEXTURE0);
}

static void createYUVGLShader(CsConverter *conv) {
    static const char _kVertShader[] =
        "#version 330\n"
        "precision highp float;\n"
        "attribute mediump vec4 aPosition;\n"
        "attribute highp vec2 aTexCoord;\n"
        "varying highp vec2 vTexCoord;\n"
        "void main(void) {\n"
        "  gl_Position = aPosition;\n"
        "  vTexCoord = aTexCoord;\n"
        "}\n";
    static const char *kVertShader = _kVertShader;

    static const char kFragShaderVersion3[] = "#version 330\n";

    static const char kFragShaderBegin[] =
        "#version 330\n"
        "precision highp float;\n"

        "varying highp vec2 vTexCoord;\n"

        "uniform highp float uYWidthCutoff;\n"
        "uniform highp float uUVWidthCutoff;\n";

    static const char kFragShaderBeginVersion3[] =
        "precision highp float;\n"

        "layout (location = 0) out vec4 FragColor;\n"
        "in highp vec2 vTexCoord;\n"

        "uniform highp float uYWidthCutoff;\n"
        "uniform highp float uUVWidthCutoff;\n";

    static const char kSamplerUniforms[] =
        "uniform sampler2D uSamplerY;\n"
        "uniform sampler2D uSamplerU;\n"
        "uniform sampler2D uSamplerV;\n";
    static const char kSamplerUniformsUint[] =
        "uniform highp usampler2D uSamplerY;\n"
        "uniform highp usampler2D uSamplerU;\n"
        "uniform highp usampler2D uSamplerV;\n";

    static const char kFragShaderMainBegin[] =
        "void main(void) {\n"
        "    highp vec2 yTexCoords = vTexCoord;\n"
        "    highp vec2 uvTexCoords = vTexCoord;\n"
        "    // For textures with extra padding for alignment (e.g. YV12 pads to 16),\n"
        "    // scale the coordinates to only sample from the non-padded area.\n"
        "    yTexCoords.x *= uYWidthCutoff;\n"
        "    uvTexCoords.x *= uUVWidthCutoff;\n"
        "    highp vec3 yuv;\n";

    static const char kSampleY[] =
        "yuv[0] = texture2D(uSamplerY, yTexCoords).r;\n";

    static const char kSampleUV[] =
        "yuv[1] = texture2D(uSamplerU, uvTexCoords).r;\n"
        "yuv[2] = texture2D(uSamplerV, uvTexCoords).r;\n";

    static const char kSampleInterleavedUV[] =
        "// Note: uSamplerU and vSamplerV refer to the same texture.\n"
        "yuv[1] = texture2D(uSamplerU, uvTexCoords).r;\n"
        "yuv[2] = texture2D(uSamplerV, uvTexCoords).g;\n";

    static const char kSampleInterleavedVU[] =
        "// Note: uSamplerU and vSamplerV refer to the same texture.\n"
        "yuv[1] = texture2D(uSamplerU, uvTexCoords).g;\n"
        "yuv[2] = texture2D(uSamplerV, uvTexCoords).r;\n";


    static const char kSampleP010[] =
        "uint yRaw = texture(uSamplerY, yTexCoords).r;\n"
        "uint uRaw = texture(uSamplerU, uvTexCoords).r;\n"
        "uint vRaw = texture(uSamplerV, uvTexCoords).g;\n"

        "// P010 values are stored in the upper 10-bits of 16-bit unsigned shorts.\n"
        "yuv[0] = float(yRaw >> 6) / 1023.0;\n"
        "yuv[1] = float(uRaw >> 6) / 1023.0;\n"
        "yuv[2] = float(vRaw >> 6) / 1023.0;\n";


    // default
    // limited range (2) 601 (4) sRGB transfer (3)
    static const char kFragShaderMain_2_4_3[] =
        "yuv[0] = yuv[0] - 0.0625;\n"
        "yuv[1] = (yuv[1] - 0.5);\n"
        "yuv[2] = (yuv[2] - 0.5);\n"

        "highp float yscale = 1.1643835616438356;\n"
        "highp vec3 rgb = mat3(            yscale,               yscale,            yscale,\n"
        "                                       0, -0.39176229009491365, 2.017232142857143,\n"
        "                      1.5960267857142856,  -0.8129676472377708,                 0) * yuv;\n";


    // full range (1) 601 (4) sRGB transfer (3)
    static const char kFragShaderMain_1_4_3[] =
        "yuv[0] = yuv[0];\n"
        "yuv[1] = (yuv[1] - 0.5);\n"
        "yuv[2] = (yuv[2] - 0.5);\n"

        "highp float yscale = 1.0;\n"
        "highp vec3 rgb = mat3(            yscale,               yscale,            yscale,\n"
        "                                       0, -0.344136* yscale, 1.772* yscale,\n"
        "                      yscale*1.402,  -0.714136* yscale,                 0) * yuv;\n";


    // limited range (2) 709 (1) sRGB transfer (3)
    static const char kFragShaderMain_2_1_3[] =
        "highp float xscale = 219.0/ 224.0;\n"
        "yuv[0] = yuv[0] - 0.0625;\n"
        "yuv[1] = xscale* (yuv[1] - 0.5);\n"
        "yuv[2] = xscale* (yuv[2] - 0.5);\n"

        "highp float yscale = 255.0/219.0;\n"
        "highp vec3 rgb = mat3(            yscale,               yscale,            yscale,\n"
        "                                       0, -0.1873* yscale, 1.8556* yscale,\n"
        "                      yscale*1.5748,  -0.4681* yscale,                 0) * yuv;\n";


    static const char kFragShaderMainEnd[] =
            "gl_FragColor = vec4(rgb, 1.0);\n"
        "}\n";


    static const char kFragShaderMainEndVersion3[] =
            "FragColor = vec4(rgb, 1.0);\n"
        "}\n";

    GString* fragShaderSource = g_string_new(NULL);

    if (conv->mFormat == AV_PIX_FMT_P010) {
        g_string_append(fragShaderSource, kFragShaderVersion3);
        g_string_append(fragShaderSource, kFragShaderBeginVersion3);
    } else {
        g_string_append(fragShaderSource, kFragShaderBegin);
    }

    if (conv->mFormat == AV_PIX_FMT_P010) {
        g_string_append(fragShaderSource, kSamplerUniformsUint);
    } else {
        g_string_append(fragShaderSource, kSamplerUniforms);
    }

    g_string_append(fragShaderSource, kFragShaderMainBegin);

    switch (conv->mFormat) {
    case AV_PIX_FMT_NV12:
    case AV_PIX_FMT_YUV420P:
        g_string_append(fragShaderSource, kSampleY);
        if (isInterleaved(conv->mFormat)) {
            g_string_append(fragShaderSource, kSampleInterleavedUV);
        } else {
            g_string_append(fragShaderSource, kSampleUV);
        }
        break;
    case AV_PIX_FMT_P010:
        g_string_append(fragShaderSource, kSampleP010);
        break;
    default:
        LOGE("%s: invalid format:%d", __FUNCTION__, conv->mFormat);
        return;
    }

    g_string_append(fragShaderSource, kFragShaderMain_2_4_3);

    if (conv->mFormat == AV_PIX_FMT_P010) {
        g_string_append(fragShaderSource, kFragShaderMainEndVersion3);
    } else {
        g_string_append(fragShaderSource, kFragShaderMainEnd);
    }

    const GLchar* const fragShaderSourceChars = fragShaderSource->str;

    // LOGD("format:%d vert-source:%s frag-source:%s", conv->mFormat, kVertShader, fragShaderSourceChars);

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertShader, 1, &kVertShader, NULL);
    glShaderSource(fragShader, 1, &fragShaderSourceChars, NULL);
    glCompileShader(vertShader);
    glCompileShader(fragShader);

    g_string_free(fragShaderSource, TRUE);

    for (GLuint shader = vertShader; shader != fragShader;
         shader = fragShader) {
        GLint status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLchar error[1024];
            glGetShaderInfoLog(shader, sizeof(error), NULL, &error[0]);
            LOGE("Failed to compile YUV conversion shader: %s", error);
            glDeleteShader(shader);
            return;
        }
    }

    conv->mProgram = glCreateProgram();
    glAttachShader(conv->mProgram, vertShader);
    glAttachShader(conv->mProgram, fragShader);
    glLinkProgram(conv->mProgram);

    GLint status = GL_FALSE;
    glGetProgramiv(conv->mProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar error[1024];
        glGetProgramInfoLog(conv->mProgram, sizeof(error), 0, &error[0]);
        LOGE("Failed to link YUV conversion program: %s", error);
        glDeleteProgram(conv->mProgram);
        conv->mProgram = 0;
        return;
    }

    conv->mUniformLocYWidthCutoff = glGetUniformLocation(conv->mProgram, "uYWidthCutoff");
    conv->mUniformLocUVWidthCutoff = glGetUniformLocation(conv->mProgram, "uUVWidthCutoff");
    conv->mUniformLocSamplerY = glGetUniformLocation(conv->mProgram, "uSamplerY");
    conv->mUniformLocSamplerU = glGetUniformLocation(conv->mProgram, "uSamplerU");
    conv->mUniformLocSamplerV = glGetUniformLocation(conv->mProgram, "uSamplerV");
    conv->mAttributeLocPos = glGetAttribLocation(conv->mProgram, "aPosition");
    conv->mAttributeLocTexCoord = glGetAttribLocation(conv->mProgram, "aTexCoord");

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

static void createYUVGLFullscreenQuad(CsConverter *conv) {
    glGenBuffers(1, &conv->mQuadVertexBuffer);
    glGenBuffers(1, &conv->mQuadIndexBuffer);

    static const float kVertices[] = {
        +1, -1, +0, +1, +0,
        +1, +1, +0, +1, +1,
        -1, +1, +0, +0, +1,
        -1, -1, +0, +0, +0,
    };

    static const GLubyte kIndices[] = { 0, 1, 2, 2, 3, 0 };

    glBindBuffer(GL_ARRAY_BUFFER, conv->mQuadVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, conv->mQuadIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);
}

/**
 * constructs and returns a colorspace converter.
 * the converter assumes a current gl context.
*/
CsConverter *cs_init(const enum AVPixelFormat dst_fmt, const enum AVPixelFormat src_fmt, int width, int height) {
    CsConverter *conv = g_malloc0(sizeof(CsConverter));
    conv->mFormat = src_fmt;

    uint32_t yWidth = 0, yHeight = 0, yOffsetBytes = 0, yStridePixels = 0, yStrideBytes = 0;
    uint32_t uWidth = 0, uHeight = 0, uOffsetBytes = 0, uStridePixels = 0, uStrideBytes = 0;
    uint32_t vWidth = 0, vHeight = 0, vOffsetBytes = 0, vStridePixels = 0, vStrideBytes = 0;
    getYUVOffsets(width, height, src_fmt,
                  &yWidth, &yHeight, &yOffsetBytes, &yStridePixels, &yStrideBytes,
                  &uWidth, &uHeight, &uOffsetBytes, &uStridePixels, &uStrideBytes,
                  &vWidth, &vHeight, &vOffsetBytes, &vStridePixels, &vStrideBytes);
    conv->mWidth = width;
    conv->mHeight = height;

    LOGD("cs_init() "
                  "width:%d height:%d "
                  "yWidth:%d yHeight:%d yOffsetBytes:%d yStridePixels:%d yStrideBytes:%d "
                  "uWidth:%d uHeight:%d uOffsetBytes:%d uStridePixels:%d uStrideBytes:%d "
                  "vWidth:%d vHeight:%d vOffsetBytes:%d vStridePixels:%d vStrideBytes:%d ",
                width, height,
                  yWidth, yHeight, yOffsetBytes, yStridePixels, yStrideBytes,
                  uWidth, uHeight, uOffsetBytes, uStridePixels, uStrideBytes,
                  vWidth, vHeight, vOffsetBytes, vStridePixels, vStrideBytes);

    if (!conv->mTextureY) {
        createYUVGLTex(GL_TEXTURE0, yStridePixels, yHeight, src_fmt, YUVPlane_Y, &conv->mTextureY);
    }
    if (isInterleaved(src_fmt)) {
        if (!conv->mTextureU) {
            createYUVGLTex(GL_TEXTURE1, uStridePixels, uHeight, src_fmt, YUVPlane_UV, &conv->mTextureU);
            conv->mTextureV = conv->mTextureU;
        }
    } else {
        if (!conv->mTextureU) {
            createYUVGLTex(GL_TEXTURE1, uStridePixels, uHeight, src_fmt, YUVPlane_U, &conv->mTextureU);
        }
        if (!conv->mTextureV) {
            createYUVGLTex(GL_TEXTURE2, vStridePixels, vHeight, src_fmt, YUVPlane_V, &conv->mTextureV);
        }
    }

    createYUVGLShader(conv);
    createYUVGLFullscreenQuad(conv);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGW("cs_init: gl error %x", err);
    }

    LOGI("created isp with colorspace %s -> %s width %d height %d",
         av_get_pix_fmt_name(src_fmt), av_get_pix_fmt_name(dst_fmt), width,
         height);
    return conv;
}

/**
 * deconstructs the colorspace converter.
 * the converter assumes a current gl context.
*/
void cs_deinit(CsConverter *conv) {
    if (conv->mQuadIndexBuffer) glDeleteBuffers(1, &conv->mQuadIndexBuffer);
    if (conv->mQuadVertexBuffer) glDeleteBuffers(1, &conv->mQuadVertexBuffer);
    if (conv->mProgram) glDeleteProgram(conv->mProgram);
    if (conv->mTextureY) glDeleteTextures(1, &conv->mTextureY);
    if (isInterleaved(conv->mFormat)) {
        if (conv->mTextureU) glDeleteTextures(1, &conv->mTextureU);
    } else {
        if (conv->mTextureU) glDeleteTextures(1, &conv->mTextureU);
        if (conv->mTextureV) glDeleteTextures(1, &conv->mTextureV);
    }
    g_free(conv);
}

static void do_convert_draw(GLuint program,
                                GLint uniformLocYWidthCutoff,
                                GLint uniformLocUVWidthCutoff,
                                GLint uniformLocYSampler,
                                GLint uniformLocUSampler,
                                GLint uniformLocVSampler,
                                GLint attributeLocTexCoord,
                                GLint attributeLocPos,
                                GLuint quadVertexBuffer,
                                GLuint quadIndexBuffer) {
    const GLsizei kVertexAttribStride = 5 * sizeof(GL_FLOAT);
    const GLvoid* kVertexAttribPosOffset = (GLvoid*)0;
    const GLvoid* kVertexAttribCoordOffset = (GLvoid*)(3 * sizeof(GL_FLOAT));

    glUseProgram(program);

    glUniform1f(uniformLocYWidthCutoff, 1.0f);
    glUniform1f(uniformLocUVWidthCutoff, 1.0f);

    glUniform1i(uniformLocYSampler, 0);
    glUniform1i(uniformLocUSampler, 1);
    glUniform1i(uniformLocVSampler, 2);

    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    glEnableVertexAttribArray(attributeLocPos);
    glEnableVertexAttribArray(attributeLocTexCoord);

    glVertexAttribPointer(attributeLocPos, 3, GL_FLOAT, false,
                                  kVertexAttribStride,
                                  kVertexAttribPosOffset);
    glVertexAttribPointer(attributeLocTexCoord, 2, GL_FLOAT, false,
                                  kVertexAttribStride,
                                  kVertexAttribCoordOffset);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndexBuffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    glDisableVertexAttribArray(attributeLocPos);
    glDisableVertexAttribArray(attributeLocTexCoord);
}

// Updates dst_tex plane texture at the coordinates
// (x, y, width, height), with the raw YUV data (with an offset) in src_buf.
static void updateYUVGLTex(GLenum texture_unit,
                            YUVPlane plane,
                            GLuint dst_tex,
                            GLuint src_buf,
                            int x,
                            int y,
                            int width,
                            int height,
                            int offset) {
    LOGD("x:%d y:%d w:%d h:%d", x, y, width, height);
    GLint textureFormat;
    GLenum pixelFormat, pixelType;
    getGLTextureFormat(plane, &textureFormat, &pixelFormat, &pixelType);

    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, dst_tex);
    GLint unprevAlignment = 0;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unprevAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, src_buf);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, pixelFormat, pixelType, (void *)(intptr_t)offset);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unprevAlignment);
    glActiveTexture(GL_TEXTURE0);
}

/**
 * converts the source tex lines into the destination fbo.
*/
int cs_convert(const CsConverter *conv, GLuint dst_fbo) {
    glViewport(0, 0, conv->mWidth, conv->mHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, conv->mTextureY);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, conv->mTextureU);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, conv->mTextureV);

    glBindFramebuffer(GL_FRAMEBUFFER, dst_fbo);

    do_convert_draw(conv->mProgram,
                    conv->mUniformLocYWidthCutoff,
                    conv->mUniformLocUVWidthCutoff,
                    conv->mUniformLocSamplerY,
                    conv->mUniformLocSamplerU,
                    conv->mUniformLocSamplerV,
                    conv->mAttributeLocTexCoord,
                    conv->mAttributeLocPos,
                    conv->mQuadVertexBuffer,
                    conv->mQuadIndexBuffer);

    return ERR_OK;
}
