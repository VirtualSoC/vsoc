#pragma once

/* gles 3.2 symbols and functions All In One */

// helpers

// deal with messy constant names
#ifndef EXPRESS_GPU_FUN_ID
#define EXPRESS_GPU_FUN_ID EXPRESS_GPU_DEVICE_ID
#endif

// batch define d_ and r_ function variants
#define NEED_SYNC (((unsigned long long)0x2) << 24u)
#define NEED_FLUSH (((unsigned long long)0x1) << 24u)
#define NO_NEED_FLUSH 0
#define DGLAPIENTRY(fun_id, need_flush, min_param, return_value, func_name, ...)  \
    static const unsigned long long FUNID_##func_name =                        \
        ((EXPRESS_GPU_FUN_ID << 32u) + need_flush + fun_id);                   \
    static const unsigned int PARA_NUM_MIN_##func_name = min_param;            \
    return_value r_##func_name __VA_ARGS__;                                    \
    return_value d_##func_name __VA_ARGS__;

// GL_KHR_robustness

void d_glReadnPixels(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                        GLenum type, GLsizei bufSize, void *data);
void d_glReadnPixelsEXT(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                        GLenum type, GLsizei bufSize, void *data);
void d_glReadnPixelsKHR(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                        GLenum type, GLsizei bufSize, void *data);
void d_glGetnUniformfv(void *context, GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
void d_glGetnUniformfvEXT(void *context, GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
void d_glGetnUniformfvKHR(void *context, GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
void d_glGetnUniformiv(void *context, GLuint program, GLint location, GLsizei bufSize, GLint *params);
void d_glGetnUniformivEXT(void *context, GLuint program, GLint location, GLsizei bufSize, GLint *params);
void d_glGetnUniformivKHR(void *context, GLuint program, GLint location, GLsizei bufSize, GLint *params);
void d_glGetnUniformuiv(void *context, GLuint program, GLint location, GLsizei bufSize, GLuint *params);
void d_glGetnUniformuivKHR(void *context, GLuint program, GLint location, GLsizei bufSize, GLuint *params);

DGLAPIENTRY(412, NEED_FLUSH, 2, void, glReadnPixels_without_bound, (void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, GLsizei bufSize, void *pixels))
DGLAPIENTRY(413, NO_NEED_FLUSH, 1, void, glReadnPixels_with_bound, (void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, GLintptr pixels))
DGLAPIENTRY(414, NEED_FLUSH, 2, void, glGetnUniformfv, (void *context, GLuint program, GLint location, GLsizei bufSize, GLfloat *params))
DGLAPIENTRY(415, NEED_FLUSH, 2, void, glGetnUniformiv, (void *context, GLuint program, GLint location, GLsizei bufSize, GLint *params))
DGLAPIENTRY(416, NEED_FLUSH, 2, void, glGetnUniformuiv, (void *context, GLuint program, GLint location, GLsizei bufSize, GLuint *params))

GLenum d_glGetGraphicsResetStatus(void *context);
GLenum d_glGetGraphicsResetStatusEXT(void *context);
GLenum d_glGetGraphicsResetStatusKHR(void *context);

DGLAPIENTRY(417, NEED_FLUSH, 1, GLenum, glGetGraphicsResetStatus, (void *context))

// GL_KHR_blend_equation_advanced

void d_glBlendBarrier(void *context);
void d_glBlendBarrierKHR(void *context);
void d_glBlendBarrierNV(void *context);

DGLAPIENTRY(418, NO_NEED_FLUSH, 1, void, glBlendBarrier, (void *context))

// GL_OES_sample_shading

void d_glMinSampleShading(void *context, GLfloat value);
void d_glMinSampleShadingOES(void *context, GLfloat value);

DGLAPIENTRY(419, NO_NEED_FLUSH, 1, void, glMinSampleShading, (void *context, GLfloat value))

// GL_EXT_draw_buffers_indexed

void d_glEnablei(void *context, GLenum target, GLuint index);
void d_glEnableiEXT(void *context, GLenum target, GLuint index);
void d_glEnableiNV(void *context, GLenum target, GLuint index);
void d_glEnableiOES(void *context, GLenum target, GLuint index);
void d_glEnablei_special(void *context, GLenum target, GLuint index);
void d_glDisablei(void *context, GLenum target, GLuint index);
void d_glDisableiEXT(void *context, GLenum target, GLuint index);
void d_glDisableiNV(void *context, GLenum target, GLuint index);
void d_glDisableiOES(void *context, GLenum target, GLuint index);
void d_glDisablei_special(void *context, GLenum target, GLuint index);
void d_glBlendEquationi(void *context, GLuint buf, GLenum mode);
void d_glBlendEquationiEXT(void *context, GLuint buf, GLenum mode);
void d_glBlendEquationiOES(void *context, GLuint buf, GLenum mode);
// void d_glBlendEquationSeparatei(void *context, GLuint buf, GLenum modeRGB, GLenum modeAlpha);
// void d_glBlendEquationSeparateiEXT(void *context, GLuint buf, GLenum modeRGB, GLenum modeAlpha);
// void d_glBlendEquationSeparateiOES(void *context, GLuint buf, GLenum modeRGB, GLenum modeAlpha);
void d_glBlendFunci(void *context, GLuint buf, GLenum src, GLenum dst);
void d_glBlendFunciEXT(void *context, GLuint buf, GLenum src, GLenum dst);
void d_glBlendFunciOES(void *context, GLuint buf, GLenum src, GLenum dst);
// void d_glBlendFuncSeparatei(void *context, GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
// void d_glBlendFuncSeparateiEXT(void *context, GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
// void d_glBlendFuncSeparateiOES(void *context, GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
// void d_glColorMaski(void *context, GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
// void d_glColorMaskiEXT(void *context, GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
// void d_glColorMaskiOES(void *context, GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
GLboolean d_glIsEnabledi(void *context, GLenum target, GLuint index);
GLboolean d_glIsEnablediEXT(void *context, GLenum target, GLuint index);
GLboolean d_glIsEnablediNV(void *context, GLenum target, GLuint index);
GLboolean d_glIsEnablediOES(void *context, GLenum target, GLuint index);

DGLAPIENTRY(420, NO_NEED_FLUSH, 1, void, glEnablei, (void *context, GLenum target, GLuint index))
DGLAPIENTRY(421, NO_NEED_FLUSH, 1, void, glDisablei, (void *context, GLenum target, GLuint index))
DGLAPIENTRY(422, NO_NEED_FLUSH, 1, void, glBlendEquationi, (void *context, GLuint buf, GLenum mode))
DGLAPIENTRY(423, NO_NEED_FLUSH, 1, void, glBlendFunci, (void *context, GLuint buf, GLenum src, GLenum dst))
// DGLAPIENTRY(424, NO_NEED_FLUSH, 0, GLboolean, glIsEnabledi, (void *context, GLenum target, GLuint index))

// GL_OES_texture_storage_multisample_2d_array

void d_glTexStorage3DMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
void d_glTexStorage3DMultisampleOES(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);

DGLAPIENTRY(425, NO_NEED_FLUSH, 1, void, glTexStorage3DMultisample, (void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations))

// GL_EXT_geometry_shader

void d_glFramebufferTexture(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level);
void d_glFramebufferTextureEXT(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level);
void d_glFramebufferTextureOES(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level);
void d_glFramebufferTexture_special(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level);

DGLAPIENTRY(426, NO_NEED_FLUSH, 1, void, glFramebufferTexture, (void *context, GLenum target, GLenum attachment, GLuint texture, GLint level))

// glDraw* with base vertex

void d_glDrawElementsBaseVertex(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void d_glDrawElementsBaseVertexEXT(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void d_glDrawElementsBaseVertexOES(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void d_glDrawRangeElementsBaseVertex(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void d_glDrawRangeElementsBaseVertexEXT(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void d_glDrawRangeElementsBaseVertexOES(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
void d_glDrawElementsInstancedBaseVertex(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
void d_glDrawElementsInstancedBaseVertexEXT(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
void d_glDrawElementsInstancedBaseVertexOES(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);

DGLAPIENTRY(427, NO_NEED_FLUSH, 1, void, glDrawElementsBaseVertex_with_bound, (void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLint basevertex))
DGLAPIENTRY(428, NEED_FLUSH, 2, void, glDrawElementsBaseVertex_without_bound, (void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex))
DGLAPIENTRY(429, NO_NEED_FLUSH, 1, void, glDrawRangeElementsBaseVertex_with_bound, (void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices, GLint basevertex))
DGLAPIENTRY(430, NEED_FLUSH, 2, void, glDrawRangeElementsBaseVertex_without_bound, (void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex))
DGLAPIENTRY(431, NO_NEED_FLUSH, 1, void, glDrawElementsInstancedBaseVertex_with_bound, (void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount, GLint basevertex))
DGLAPIENTRY(432, NEED_FLUSH, 2, void, glDrawElementsInstancedBaseVertex_without_bound, (void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex))

// GL_EXT_texture_border_clamp

void d_glTexParameterIiv(void *context, GLenum target, GLenum pname, const GLint *params);
void d_glTexParameterIivEXT(void *context, GLenum target, GLenum pname, const GLint *params);
void d_glTexParameterIivOES(void *context, GLenum target, GLenum pname, const GLint *params);
void d_glTexParameterIuiv(void *context, GLenum target, GLenum pname, const GLuint *params);
void d_glTexParameterIuivEXT(void *context, GLenum target, GLenum pname, const GLuint *params);
void d_glTexParameterIuivOES(void *context, GLenum target, GLenum pname, const GLuint *params);
void d_glGetTexParameterIiv(void *context, GLenum target, GLenum pname, GLint *params);
void d_glGetTexParameterIivEXT(void *context, GLenum target, GLenum pname, GLint *params);
void d_glGetTexParameterIivOES(void *context, GLenum target, GLenum pname, GLint *params);
void d_glGetTexParameterIuiv(void *context, GLenum target, GLenum pname, GLuint *params);
void d_glGetTexParameterIuivEXT(void *context, GLenum target, GLenum pname, GLuint *params);
void d_glGetTexParameterIuivOES(void *context, GLenum target, GLenum pname, GLuint *params);
void d_glSamplerParameterIiv(void *context, GLuint sampler, GLenum pname, const GLint *param);
void d_glSamplerParameterIivEXT(void *context, GLuint sampler, GLenum pname, const GLint *param);
void d_glSamplerParameterIivOES(void *context, GLuint sampler, GLenum pname, const GLint *param);
void d_glSamplerParameterIuiv(void *context, GLuint sampler, GLenum pname, const GLuint *param);
void d_glSamplerParameterIuivEXT(void *context, GLuint sampler, GLenum pname, const GLuint *param);
void d_glSamplerParameterIuivOES(void *context, GLuint sampler, GLenum pname, const GLuint *param);
void d_glGetSamplerParameterIiv(void *context, GLuint sampler, GLenum pname, GLint *params);
void d_glGetSamplerParameterIivEXT(void *context, GLuint sampler, GLenum pname, GLint *params);
void d_glGetSamplerParameterIivOES(void *context, GLuint sampler, GLenum pname, GLint *params);
void d_glGetSamplerParameterIuiv(void *context, GLuint sampler, GLenum pname, GLuint *params);
void d_glGetSamplerParameterIuivEXT(void *context, GLuint sampler, GLenum pname, GLuint *params);
void d_glGetSamplerParameterIuivOES(void *context, GLuint sampler, GLenum pname, GLuint *params);

DGLAPIENTRY(433, NO_NEED_FLUSH, 1, void, glTexParameterIiv, (void *context, GLenum target, GLenum pname, const GLint *params))
DGLAPIENTRY(434, NO_NEED_FLUSH, 1, void, glTexParameterIuiv, (void *context, GLenum target, GLenum pname, const GLuint *params))
DGLAPIENTRY(435, NO_NEED_FLUSH, 2, void, glGetTexParameterIiv, (void *context, GLenum target, GLenum pname, GLint *params))
DGLAPIENTRY(436, NO_NEED_FLUSH, 2, void, glGetTexParameterIuiv, (void *context, GLenum target, GLenum pname, GLuint *params))
DGLAPIENTRY(437, NO_NEED_FLUSH, 1, void, glSamplerParameterIiv, (void *context, GLuint sampler, GLenum pname, const GLint *params))
DGLAPIENTRY(438, NO_NEED_FLUSH, 1, void, glSamplerParameterIuiv, (void *context, GLuint sampler, GLenum pname, const GLuint *params))
DGLAPIENTRY(439, NO_NEED_FLUSH, 2, void, glGetSamplerParameterIiv, (void *context, GLuint sampler, GLenum pname, GLint *params))
DGLAPIENTRY(440, NO_NEED_FLUSH, 2, void, glGetSamplerParameterIuiv, (void *context, GLuint sampler, GLenum pname, GLuint *params))

// GL_EXT_tessellation_shader

void d_glPatchParameteri(void *context, GLenum pname, GLint value);
void d_glPatchParameteriOES(void *context, GLenum pname, GLint value);
void d_glPatchParameteriEXT(void *context, GLenum pname, GLint value);

DGLAPIENTRY(441, NO_NEED_FLUSH, 1, void, glPatchParameteri, (void *context, GLenum pname, GLint value))

// GL_EXT_copy_image

void d_glCopyImageSubData(void *context, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
void d_glCopyImageSubDataEXT(void *context, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
void d_glCopyImageSubDataOES(void *context, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);

DGLAPIENTRY(442, NO_NEED_FLUSH, 1, void, glCopyImageSubData, (void *context, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth))

// GL_EXT_primitive_bounding_box

void d_glPrimitiveBoundingBox(void *context, GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW);
void d_glPrimitiveBoundingBoxEXT(void *context, GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW);
void d_glPrimitiveBoundingBoxOES(void *context, GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW);

// GL_KHR_debug

typedef struct DebugMessageDesc {
    GLenum source;
    GLenum type;
    GLuint id;
    GLenum severity;
    GLsizei length; // message字符串实际长度（不包含\0）
} __attribute__((packed, aligned(4))) DebugMessageDesc;

typedef struct RingBufferDesc {
    size_t block_count; // message block的总数量
    size_t block_size;  // 每个message block的大小
    int read_idx;       // 在buffer中的实际位置是read_idx % block_count
    int write_idx;      // 和read_idx类似
} __attribute__((packed, aligned(4))) RingBufferDesc;

typedef struct Debug_Status { // guest only
    void *debug_message_buffer;
    // guest端注册的debug callback函数以及user_param
    GLDEBUGPROC callback;
    void *user_param;
} Debug_Status;


// FIXME: potential collision with future GL context flags
#define DGL_CONTEXT_INDEPENDENT_MODE 0x00ffffff
#define DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT 0x10000000

DebugMessageDesc *get_next_debug_message(RingBufferDesc *buffer);
void process_debug_message(void *context);
void d_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
void d_eglCreateDebugMessageBuffer(void *context, EGLContext gl_context, void *buffer);

DGLAPIENTRY(10043, NEED_FLUSH, 2, void, eglCreateDebugMessageBuffer, (void *context, void *gl_context, void *buffer))
DGLAPIENTRY(10044, NEED_FLUSH, 2, void, eglDestroyDebugMessageBuffer, (void *context, void *gl_context, void *buffer))

void d_glDebugMessageControl(void *context, GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
void d_glDebugMessageControlKHR(void *context, GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
void d_glDebugMessageInsert(void *context, GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
void d_glDebugMessageInsertKHR(void *context, GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
void d_glDebugMessageCallback(void *context, GLDEBUGPROC callback, const void *userParam);
void d_glDebugMessageCallbackKHR(void *context, GLDEBUGPROC callback, const void *userParam);
GLuint d_glGetDebugMessageLog(void *context, GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
GLuint d_glGetDebugMessageLogKHR(void *context, GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
void d_glPushDebugGroup(void *context, GLenum source, GLuint id, GLsizei length, const GLchar *message);
void d_glPushDebugGroupKHR(void *context, GLenum source, GLuint id, GLsizei length, const GLchar *message);
void d_glPopDebugGroup(void *context);
void d_glPopDebugGroupKHR(void *context);
void d_glObjectLabel(void *context, GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
void d_glObjectLabelKHR(void *context, GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
void d_glGetObjectLabel(void *context, GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glGetObjectLabelEXT(void *context, GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glGetObjectLabelKHR(void *context, GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glObjectPtrLabel(void *context, const void *ptr, GLsizei length, const GLchar *label);
void d_glObjectPtrLabelKHR(void *context, const void *ptr, GLsizei length, const GLchar *label);
void d_glGetObjectPtrLabel(void *context, const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glGetObjectPtrLabelKHR(void *context, const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glGetPointerv(void *context, GLenum pname, void **params);
void d_glGetPointervKHR(void *context, GLenum pname, void **params);

DGLAPIENTRY(443, NO_NEED_FLUSH, 1, void, glDebugMessageControl, (void *context, GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled))
DGLAPIENTRY(444, NO_NEED_FLUSH, 1, void, glDebugMessageInsert, (void *context, GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf))
DGLAPIENTRY(445, NO_NEED_FLUSH, 1, void, glPushDebugGroup, (void *context, GLenum source, GLuint id, GLsizei length, const GLchar *message))
DGLAPIENTRY(446, NO_NEED_FLUSH, 1, void, glPopDebugGroup, (void *context))
DGLAPIENTRY(447, NO_NEED_FLUSH, 1, void, glObjectLabel, (void *context, GLenum identifier, GLuint name, GLsizei length, const GLchar *label))
DGLAPIENTRY(448, NEED_FLUSH, 2, void, glGetObjectLabel, (void *context, GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label))
DGLAPIENTRY(449, NO_NEED_FLUSH, 1, void, glObjectPtrLabel, (void *context, const void *ptr, GLsizei length, const GLchar *label))
DGLAPIENTRY(450, NEED_FLUSH, 2, void, glGetObjectPtrLabel, (void *context, const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label))
