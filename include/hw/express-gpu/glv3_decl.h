#pragma once

/* gles 3.2 symbols and functions All In One */

// deal with messy constant names
#ifndef EXPRESS_GPU_DEVICE_ID
#define EXPRESS_GPU_DEVICE_ID ((uint64_t)1)
#endif


#define FUNID_glReadnPixels_without_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 412)
#define PARA_NUM_MIN_glReadnPixels_without_bound 2
void r_glReadnPixels_without_bound(void *context, GLint x, GLint y,
                                   GLsizei width, GLsizei height, GLenum format,
                                   GLenum type, GLint buf_len, GLsizei bufSize,
                                   void *pixels);
void d_glReadnPixels_without_bound(void *context, GLint x, GLint y,
                                   GLsizei width, GLsizei height, GLenum format,
                                   GLenum type, GLint buf_len, GLsizei bufSize,
                                   void *pixels);
void d_glReadnPixels(void *context, GLint x, GLint y, GLsizei width,
                     GLsizei height, GLenum format, GLenum type,
                     GLsizei bufSize, void *data);
void d_glReadnPixelsEXT(void *context, GLint x, GLint y, GLsizei width,
                        GLsizei height, GLenum format, GLenum type,
                        GLsizei bufSize, void *data);
void d_glReadnPixelsKHR(void *context, GLint x, GLint y, GLsizei width,
                        GLsizei height, GLenum format, GLenum type,
                        GLsizei bufSize, void *data);

#define FUNID_glReadnPixels_with_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 413)
#define PARA_NUM_MIN_glReadnPixels_with_bound 1
void r_glReadnPixels_with_bound(void *context, GLint x, GLint y, GLsizei width,
                                GLsizei height, GLenum format, GLenum type,
                                GLsizei bufSize, GLintptr pixels);
void d_glReadnPixels_with_bound(void *context, GLint x, GLint y, GLsizei width,
                                GLsizei height, GLenum format, GLenum type,
                                GLsizei bufSize, GLintptr pixels);

#define FUNID_glGetnUniformfv ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 414)
#define PARA_NUM_MIN_glGetnUniformfv 2
void r_glGetnUniformfv(void *context, GLuint program, GLint location,
                       GLsizei bufSize, GLfloat *params);
void d_glGetnUniformfv(void *context, GLuint program, GLint location,
                       GLsizei bufSize, GLfloat *params);
void d_glGetnUniformfvEXT(void *context, GLuint program, GLint location,
                          GLsizei bufSize, GLfloat *params);
void d_glGetnUniformfvKHR(void *context, GLuint program, GLint location,
                          GLsizei bufSize, GLfloat *params);

#define FUNID_glGetnUniformiv ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 415)
#define PARA_NUM_MIN_glGetnUniformiv 2
void r_glGetnUniformiv(void *context, GLuint program, GLint location,
                       GLsizei bufSize, GLint *params);
void d_glGetnUniformiv(void *context, GLuint program, GLint location,
                       GLsizei bufSize, GLint *params);
void d_glGetnUniformivEXT(void *context, GLuint program, GLint location,
                          GLsizei bufSize, GLint *params);
void d_glGetnUniformivKHR(void *context, GLuint program, GLint location,
                          GLsizei bufSize, GLint *params);

#define FUNID_glGetnUniformuiv ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 416)
#define PARA_NUM_MIN_glGetnUniformuiv 2
void r_glGetnUniformuiv(void *context, GLuint program, GLint location,
                        GLsizei bufSize, GLuint *params);
void d_glGetnUniformuiv(void *context, GLuint program, GLint location,
                        GLsizei bufSize, GLuint *params);
void d_glGetnUniformuivKHR(void *context, GLuint program, GLint location,
                           GLsizei bufSize, GLuint *params);

#define FUNID_glGetGraphicsResetStatus ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 417)
#define PARA_NUM_MIN_glGetGraphicsResetStatus 1
GLenum r_glGetGraphicsResetStatus(void *context);
GLenum d_glGetGraphicsResetStatus(void *context);
GLenum d_glGetGraphicsResetStatusEXT(void *context);
GLenum d_glGetGraphicsResetStatusKHR(void *context);

#define FUNID_glBlendBarrier ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 418)
#define PARA_NUM_MIN_glBlendBarrier 1
void r_glBlendBarrier(void *context);
void d_glBlendBarrier(void *context);
void d_glBlendBarrierKHR(void *context);
void d_glBlendBarrierNV(void *context);

#define FUNID_glMinSampleShading ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 419)
#define PARA_NUM_MIN_glMinSampleShading 1
void r_glMinSampleShading(void *context, GLfloat value);
void d_glMinSampleShading(void *context, GLfloat value);
void d_glMinSampleShadingOES(void *context, GLfloat value);

GLboolean d_glIsEnabledi(void *context, GLenum target, GLuint index);
GLboolean d_glIsEnablediEXT(void *context, GLenum target, GLuint index);
GLboolean d_glIsEnablediNV(void *context, GLenum target, GLuint index);
GLboolean d_glIsEnablediOES(void *context, GLenum target, GLuint index);

#define FUNID_glEnablei ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 420)
#define PARA_NUM_MIN_glEnablei 1
void r_glEnablei(void *context, GLenum target, GLuint index);
void d_glEnablei(void *context, GLenum target, GLuint index);
void d_glEnableiEXT(void *context, GLenum target, GLuint index);
void d_glEnableiNV(void *context, GLenum target, GLuint index);
void d_glEnableiOES(void *context, GLenum target, GLuint index);
void d_glEnablei_special(void *context, GLenum target, GLuint index);

#define FUNID_glDisablei ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 421)
#define PARA_NUM_MIN_glDisablei 1
void r_glDisablei(void *context, GLenum target, GLuint index);
void d_glDisablei(void *context, GLenum target, GLuint index);
void d_glDisableiEXT(void *context, GLenum target, GLuint index);
void d_glDisableiNV(void *context, GLenum target, GLuint index);
void d_glDisableiOES(void *context, GLenum target, GLuint index);
void d_glDisablei_special(void *context, GLenum target, GLuint index);

#define FUNID_glBlendEquationi ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 422)
#define PARA_NUM_MIN_glBlendEquationi 1
void r_glBlendEquationi(void *context, GLuint buf, GLenum mode);
void d_glBlendEquationi(void *context, GLuint buf, GLenum mode);
void d_glBlendEquationiEXT(void *context, GLuint buf, GLenum mode);
void d_glBlendEquationiOES(void *context, GLuint buf, GLenum mode);

#define FUNID_glBlendFunci ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 423)
#define PARA_NUM_MIN_glBlendFunci 1
void r_glBlendFunci(void *context, GLuint buf, GLenum src, GLenum dst);
void d_glBlendFunci(void *context, GLuint buf, GLenum src, GLenum dst);
void d_glBlendFunciEXT(void *context, GLuint buf, GLenum src, GLenum dst);
void d_glBlendFunciOES(void *context, GLuint buf, GLenum src, GLenum dst);

#define FUNID_glTexStorage3DMultisample ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 425)
#define PARA_NUM_MIN_glTexStorage3DMultisample 1
void r_glTexStorage3DMultisample(void *context, GLenum target, GLsizei samples,
                                 GLenum internalformat, GLsizei width,
                                 GLsizei height, GLsizei depth,
                                 GLboolean fixedsamplelocations);
void d_glTexStorage3DMultisample(void *context, GLenum target, GLsizei samples,
                                 GLenum internalformat, GLsizei width,
                                 GLsizei height, GLsizei depth,
                                 GLboolean fixedsamplelocations);
void d_glTexStorage3DMultisampleOES(void *context, GLenum target,
                                    GLsizei samples, GLenum internalformat,
                                    GLsizei width, GLsizei height,
                                    GLsizei depth,
                                    GLboolean fixedsamplelocations);

#define FUNID_glFramebufferTexture ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 426)
#define PARA_NUM_MIN_glFramebufferTexture 1
void r_glFramebufferTexture(void *context, GLenum target, GLenum attachment,
                            GLuint texture, GLint level);
void d_glFramebufferTexture(void *context, GLenum target, GLenum attachment,
                            GLuint texture, GLint level);
void d_glFramebufferTextureEXT(void *context, GLenum target, GLenum attachment,
                               GLuint texture, GLint level);
void d_glFramebufferTextureOES(void *context, GLenum target, GLenum attachment,
                               GLuint texture, GLint level);
void d_glFramebufferTexture_special(void *context, GLenum target,
                                    GLenum attachment, GLuint texture,
                                    GLint level);

#define FUNID_glDrawElementsBaseVertex_with_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 427)
#define PARA_NUM_MIN_glDrawElementsBaseVertex_with_bound 1
void r_glDrawElementsBaseVertex_with_bound(void *context, GLenum mode,
                                           GLsizei count, GLenum type,
                                           GLsizeiptr indices,
                                           GLint basevertex);
void d_glDrawElementsBaseVertex_with_bound(void *context, GLenum mode,
                                           GLsizei count, GLenum type,
                                           GLsizeiptr indices,
                                           GLint basevertex);

#define FUNID_glDrawElementsBaseVertex_without_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 428)
static const unsigned int PARA_NUM_MIN_glDrawElementsBaseVertex_without_bound =
    2;
void r_glDrawElementsBaseVertex_without_bound(void *context, GLenum mode,
                                              GLsizei count, GLenum type,
                                              const void *indices,
                                              GLint basevertex);
void d_glDrawElementsBaseVertex_without_bound(void *context, GLenum mode,
                                              GLsizei count, GLenum type,
                                              const void *indices,
                                              GLint basevertex);

void d_glDrawElementsBaseVertex(void *context, GLenum mode, GLsizei count,
                                GLenum type, const void *indices,
                                GLint basevertex);
void d_glDrawElementsBaseVertexEXT(void *context, GLenum mode, GLsizei count,
                                   GLenum type, const void *indices,
                                   GLint basevertex);
void d_glDrawElementsBaseVertexOES(void *context, GLenum mode, GLsizei count,
                                   GLenum type, const void *indices,
                                   GLint basevertex);

#define FUNID_glDrawRangeElementsBaseVertex_with_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 429)
static const unsigned int
    PARA_NUM_MIN_glDrawRangeElementsBaseVertex_with_bound = 1;
void r_glDrawRangeElementsBaseVertex_with_bound(void *context, GLenum mode,
                                                GLuint start, GLuint end,
                                                GLsizei count, GLenum type,
                                                GLsizeiptr indices,
                                                GLint basevertex);
void d_glDrawRangeElementsBaseVertex_with_bound(void *context, GLenum mode,
                                                GLuint start, GLuint end,
                                                GLsizei count, GLenum type,
                                                GLsizeiptr indices,
                                                GLint basevertex);

#define FUNID_glDrawRangeElementsBaseVertex_without_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 430)
#define PARA_NUM_MIN_glDrawRangeElementsBaseVertex_without_bound 2
void r_glDrawRangeElementsBaseVertex_without_bound(void *context, GLenum mode,
                                                   GLuint start, GLuint end,
                                                   GLsizei count, GLenum type,
                                                   const void *indices,
                                                   GLint basevertex);
void d_glDrawRangeElementsBaseVertex_without_bound(void *context, GLenum mode,
                                                   GLuint start, GLuint end,
                                                   GLsizei count, GLenum type,
                                                   const void *indices,
                                                   GLint basevertex);

void d_glDrawRangeElementsBaseVertex(void *context, GLenum mode, GLuint start,
                                     GLuint end, GLsizei count, GLenum type,
                                     const void *indices, GLint basevertex);
void d_glDrawRangeElementsBaseVertexEXT(void *context, GLenum mode,
                                        GLuint start, GLuint end, GLsizei count,
                                        GLenum type, const void *indices,
                                        GLint basevertex);
void d_glDrawRangeElementsBaseVertexOES(void *context, GLenum mode,
                                        GLuint start, GLuint end, GLsizei count,
                                        GLenum type, const void *indices,
                                        GLint basevertex);

#define FUNID_glDrawElementsInstancedBaseVertex_with_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 431)
#define PARA_NUM_MIN_glDrawElementsInstancedBaseVertex_with_bound 1
void r_glDrawElementsInstancedBaseVertex_with_bound(void *context, GLenum mode,
                                                    GLsizei count, GLenum type,
                                                    GLsizeiptr indices,
                                                    GLsizei instancecount,
                                                    GLint basevertex);
void d_glDrawElementsInstancedBaseVertex_with_bound(void *context, GLenum mode,
                                                    GLsizei count, GLenum type,
                                                    GLsizeiptr indices,
                                                    GLsizei instancecount,
                                                    GLint basevertex);

#define FUNID_glDrawElementsInstancedBaseVertex_without_bound ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 432)
#define PARA_NUM_MIN_glDrawElementsInstancedBaseVertex_without_bound 2
void r_glDrawElementsInstancedBaseVertex_without_bound(
    void *context, GLenum mode, GLsizei count, GLenum type, const void *indices,
    GLsizei instancecount, GLint basevertex);
void d_glDrawElementsInstancedBaseVertex_without_bound(
    void *context, GLenum mode, GLsizei count, GLenum type, const void *indices,
    GLsizei instancecount, GLint basevertex);

void d_glDrawElementsInstancedBaseVertex(void *context, GLenum mode,
                                         GLsizei count, GLenum type,
                                         const void *indices,
                                         GLsizei instancecount,
                                         GLint basevertex);
void d_glDrawElementsInstancedBaseVertexEXT(void *context, GLenum mode,
                                            GLsizei count, GLenum type,
                                            const void *indices,
                                            GLsizei instancecount,
                                            GLint basevertex);
void d_glDrawElementsInstancedBaseVertexOES(void *context, GLenum mode,
                                            GLsizei count, GLenum type,
                                            const void *indices,
                                            GLsizei instancecount,
                                            GLint basevertex);

#define FUNID_glTexParameterIiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 433)
#define PARA_NUM_MIN_glTexParameterIiv 1
void r_glTexParameterIiv(void *context, GLenum target, GLenum pname,
                         const GLint *params);
void d_glTexParameterIiv(void *context, GLenum target, GLenum pname,
                         const GLint *params);
void d_glTexParameterIivEXT(void *context, GLenum target, GLenum pname,
                            const GLint *params);
void d_glTexParameterIivOES(void *context, GLenum target, GLenum pname,
                            const GLint *params);

#define FUNID_glTexParameterIuiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 434)
#define PARA_NUM_MIN_glTexParameterIuiv 1
void r_glTexParameterIuiv(void *context, GLenum target, GLenum pname,
                          const GLuint *params);
void d_glTexParameterIuiv(void *context, GLenum target, GLenum pname,
                          const GLuint *params);
void d_glTexParameterIuivEXT(void *context, GLenum target, GLenum pname,
                             const GLuint *params);
void d_glTexParameterIuivOES(void *context, GLenum target, GLenum pname,
                             const GLuint *params);

#define FUNID_glGetTexParameterIiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 435)
#define PARA_NUM_MIN_glGetTexParameterIiv 2
void r_glGetTexParameterIiv(void *context, GLenum target, GLenum pname,
                            GLint *params);
void d_glGetTexParameterIiv(void *context, GLenum target, GLenum pname,
                            GLint *params);
void d_glGetTexParameterIivEXT(void *context, GLenum target, GLenum pname,
                               GLint *params);
void d_glGetTexParameterIivOES(void *context, GLenum target, GLenum pname,
                               GLint *params);

#define FUNID_glGetTexParameterIuiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 436)
#define PARA_NUM_MIN_glGetTexParameterIuiv 2
void r_glGetTexParameterIuiv(void *context, GLenum target, GLenum pname,
                             GLuint *params);
void d_glGetTexParameterIuiv(void *context, GLenum target, GLenum pname,
                             GLuint *params);
void d_glGetTexParameterIuivEXT(void *context, GLenum target, GLenum pname,
                                GLuint *params);
void d_glGetTexParameterIuivOES(void *context, GLenum target, GLenum pname,
                                GLuint *params);

#define FUNID_glSamplerParameterIiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 437)
#define PARA_NUM_MIN_glSamplerParameterIiv 1
void r_glSamplerParameterIiv(void *context, GLuint sampler, GLenum pname,
                             const GLint *params);
void d_glSamplerParameterIiv(void *context, GLuint sampler, GLenum pname,
                             const GLint *params);
void d_glSamplerParameterIivEXT(void *context, GLuint sampler, GLenum pname,
                                const GLint *param);
void d_glSamplerParameterIivOES(void *context, GLuint sampler, GLenum pname,
                                const GLint *param);

#define FUNID_glSamplerParameterIuiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 438)
#define PARA_NUM_MIN_glSamplerParameterIuiv 1
void r_glSamplerParameterIuiv(void *context, GLuint sampler, GLenum pname,
                              const GLuint *params);
void d_glSamplerParameterIuiv(void *context, GLuint sampler, GLenum pname,
                              const GLuint *params);
void d_glSamplerParameterIuivEXT(void *context, GLuint sampler, GLenum pname,
                                 const GLuint *param);
void d_glSamplerParameterIuivOES(void *context, GLuint sampler, GLenum pname,
                                 const GLuint *param);

#define FUNID_glGetSamplerParameterIiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 439)
#define PARA_NUM_MIN_glGetSamplerParameterIiv 2
void r_glGetSamplerParameterIiv(void *context, GLuint sampler, GLenum pname,
                                GLint *params);
void d_glGetSamplerParameterIiv(void *context, GLuint sampler, GLenum pname,
                                GLint *params);
void d_glGetSamplerParameterIivEXT(void *context, GLuint sampler, GLenum pname,
                                   GLint *params);
void d_glGetSamplerParameterIivOES(void *context, GLuint sampler, GLenum pname,
                                   GLint *params);

#define FUNID_glGetSamplerParameterIuiv ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 440)
#define PARA_NUM_MIN_glGetSamplerParameterIuiv 2
void r_glGetSamplerParameterIuiv(void *context, GLuint sampler, GLenum pname,
                                 GLuint *params);
void d_glGetSamplerParameterIuiv(void *context, GLuint sampler, GLenum pname,
                                 GLuint *params);
void d_glGetSamplerParameterIuivEXT(void *context, GLuint sampler, GLenum pname,
                                    GLuint *params);
void d_glGetSamplerParameterIuivOES(void *context, GLuint sampler, GLenum pname,
                                    GLuint *params);

#define FUNID_glPatchParameteri ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 441)
#define PARA_NUM_MIN_glPatchParameteri 1
void r_glPatchParameteri(void *context, GLenum pname, GLint value);
void d_glPatchParameteri(void *context, GLenum pname, GLint value);
void d_glPatchParameteriOES(void *context, GLenum pname, GLint value);
void d_glPatchParameteriEXT(void *context, GLenum pname, GLint value);

#define FUNID_glCopyImageSubData ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 442)
#define PARA_NUM_MIN_glCopyImageSubData 1
void r_glCopyImageSubData(void *context, GLuint srcName, GLenum srcTarget,
                          GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
                          GLuint dstName, GLenum dstTarget, GLint dstLevel,
                          GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth,
                          GLsizei srcHeight, GLsizei srcDepth);
void d_glCopyImageSubData(void *context, GLuint srcName, GLenum srcTarget,
                          GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
                          GLuint dstName, GLenum dstTarget, GLint dstLevel,
                          GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth,
                          GLsizei srcHeight, GLsizei srcDepth);
void d_glCopyImageSubDataEXT(void *context, GLuint srcName, GLenum srcTarget,
                             GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
                             GLuint dstName, GLenum dstTarget, GLint dstLevel,
                             GLint dstX, GLint dstY, GLint dstZ,
                             GLsizei srcWidth, GLsizei srcHeight,
                             GLsizei srcDepth);
void d_glCopyImageSubDataOES(void *context, GLuint srcName, GLenum srcTarget,
                             GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
                             GLuint dstName, GLenum dstTarget, GLint dstLevel,
                             GLint dstX, GLint dstY, GLint dstZ,
                             GLsizei srcWidth, GLsizei srcHeight,
                             GLsizei srcDepth);

void d_glPrimitiveBoundingBox(void *context, GLfloat minX, GLfloat minY,
                              GLfloat minZ, GLfloat minW, GLfloat maxX,
                              GLfloat maxY, GLfloat maxZ, GLfloat maxW);
void d_glPrimitiveBoundingBoxEXT(void *context, GLfloat minX, GLfloat minY,
                                 GLfloat minZ, GLfloat minW, GLfloat maxX,
                                 GLfloat maxY, GLfloat maxZ, GLfloat maxW);
void d_glPrimitiveBoundingBoxOES(void *context, GLfloat minX, GLfloat minY,
                                 GLfloat minZ, GLfloat minW, GLfloat maxX,
                                 GLfloat maxY, GLfloat maxZ, GLfloat maxW);

typedef struct DebugMessageDesc {
    GLenum source;
    GLenum type;
    GLuint id;
    GLenum severity;
    GLsizei length;
} __attribute__((packed, aligned(4))) DebugMessageDesc;

typedef struct RingBufferDesc {
    int block_count;
    int block_size;
    int read_idx;
    int write_idx;
} __attribute__((packed, aligned(4))) RingBufferDesc;

typedef struct Debug_Status {
    void *debug_message_buffer;

    GLDEBUGPROC callback;
    void *user_param;
} Debug_Status;

// FIXME: potential collision with future GL context flags
#define DGL_CONTEXT_INDEPENDENT_MODE 0x00ffffff
#define DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT 0x10000000

DebugMessageDesc *get_next_debug_message(RingBufferDesc *buffer);
void process_debug_message(void *context);
void d_debug_message_callback(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length,
                              const GLchar *message, const void *userParam);
void d_glDebugMessageCallback(void *context, GLDEBUGPROC callback,
                              const void *userParam);
void d_glDebugMessageCallbackKHR(void *context, GLDEBUGPROC callback,
                                 const void *userParam);
GLuint d_glGetDebugMessageLog(void *context, GLuint count, GLsizei bufSize,
                              GLenum *sources, GLenum *types, GLuint *ids,
                              GLenum *severities, GLsizei *lengths,
                              GLchar *messageLog);
GLuint d_glGetDebugMessageLogKHR(void *context, GLuint count, GLsizei bufSize,
                                 GLenum *sources, GLenum *types, GLuint *ids,
                                 GLenum *severities, GLsizei *lengths,
                                 GLchar *messageLog);

#define FUNID_eglCreateDebugMessageBuffer ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10043)
#define PARA_NUM_MIN_eglCreateDebugMessageBuffer 2
void r_eglCreateDebugMessageBuffer(void *context, void *gl_context,
                                   void *buffer);
void d_eglCreateDebugMessageBuffer(void *context, void *gl_context,
                                   void *buffer);

#define FUNID_eglDestroyDebugMessageBuffer ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10044)
#define PARA_NUM_MIN_eglDestroyDebugMessageBuffer 2
void r_eglDestroyDebugMessageBuffer(void *context, void *gl_context,
                                    void *buffer);
void d_eglDestroyDebugMessageBuffer(void *context, void *gl_context,
                                    void *buffer);

#define FUNID_glDebugMessageControl ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 443)
#define PARA_NUM_MIN_glDebugMessageControl 1
void r_glDebugMessageControl(void *context, GLenum source, GLenum type,
                             GLenum severity, GLsizei count, const GLuint *ids,
                             GLboolean enabled);
void d_glDebugMessageControl(void *context, GLenum source, GLenum type,
                             GLenum severity, GLsizei count, const GLuint *ids,
                             GLboolean enabled);
void d_glDebugMessageControlKHR(void *context, GLenum source, GLenum type,
                                GLenum severity, GLsizei count,
                                const GLuint *ids, GLboolean enabled);

#define FUNID_glDebugMessageInsert ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 444)
#define PARA_NUM_MIN_glDebugMessageInsert 1
void r_glDebugMessageInsert(void *context, GLenum source, GLenum type,
                            GLuint id, GLenum severity, GLsizei length,
                            const GLchar *buf);
void d_glDebugMessageInsert(void *context, GLenum source, GLenum type,
                            GLuint id, GLenum severity, GLsizei length,
                            const GLchar *buf);
void d_glDebugMessageInsertKHR(void *context, GLenum source, GLenum type,
                               GLuint id, GLenum severity, GLsizei length,
                               const GLchar *buf);

#define FUNID_glPushDebugGroup ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 445)
#define PARA_NUM_MIN_glPushDebugGroup 1
void r_glPushDebugGroup(void *context, GLenum source, GLuint id, GLsizei length,
                        const GLchar *message);
void d_glPushDebugGroup(void *context, GLenum source, GLuint id, GLsizei length,
                        const GLchar *message);
void d_glPushDebugGroupKHR(void *context, GLenum source, GLuint id,
                           GLsizei length, const GLchar *message);

#define FUNID_glPopDebugGroup ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 446)
#define PARA_NUM_MIN_glPopDebugGroup 1
void r_glPopDebugGroup(void *context);
void d_glPopDebugGroup(void *context);
void d_glPopDebugGroupKHR(void *context);

#define FUNID_glObjectLabel ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 447)
#define PARA_NUM_MIN_glObjectLabel 1
void r_glObjectLabel(void *context, GLenum identifier, GLuint name,
                     GLsizei length, const GLchar *label);
void d_glObjectLabel(void *context, GLenum identifier, GLuint name,
                     GLsizei length, const GLchar *label);
void d_glObjectLabelKHR(void *context, GLenum identifier, GLuint name,
                        GLsizei length, const GLchar *label);

#define FUNID_glGetObjectLabel ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 448)
#define PARA_NUM_MIN_glGetObjectLabel 2
void r_glGetObjectLabel(void *context, GLenum identifier, GLuint name,
                        GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glGetObjectLabel(void *context, GLenum identifier, GLuint name,
                        GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glGetObjectLabelEXT(void *context, GLenum identifier, GLuint name,
                           GLsizei bufSize, GLsizei *length, GLchar *label);
void d_glGetObjectLabelKHR(void *context, GLenum identifier, GLuint name,
                           GLsizei bufSize, GLsizei *length, GLchar *label);

#define FUNID_glObjectPtrLabel ((EXPRESS_GPU_DEVICE_ID << 32u) + 0 + 449)
#define PARA_NUM_MIN_glObjectPtrLabel 1
void r_glObjectPtrLabel(void *context, const void *ptr, GLsizei length,
                        const GLchar *label);
void d_glObjectPtrLabel(void *context, const void *ptr, GLsizei length,
                        const GLchar *label);
void d_glObjectPtrLabelKHR(void *context, const void *ptr, GLsizei length,
                           const GLchar *label);

#define FUNID_glGetObjectPtrLabel ((EXPRESS_GPU_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 450)
#define PARA_NUM_MIN_glGetObjectPtrLabel 2
void r_glGetObjectPtrLabel(void *context, const void *ptr, GLsizei bufSize,
                           GLsizei *length, GLchar *label);
void d_glGetObjectPtrLabel(void *context, const void *ptr, GLsizei bufSize,
                           GLsizei *length, GLchar *label);
void d_glGetObjectPtrLabelKHR(void *context, const void *ptr, GLsizei bufSize,
                              GLsizei *length, GLchar *label);

void d_glGetPointerv(void *context, GLenum pname, void **params);
void d_glGetPointervKHR(void *context, GLenum pname, void **params);
