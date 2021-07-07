

#ifndef DEFINE_GL_H
#define DEFINE_GL_H

//android
#include <GLES/glplatform.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2platform.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#define EXPRESS_GPU_FUN_ID ((unsigned long long)1)

#define MAX_OUT_BUF_LEN 4096

#define FUNID_glCheckFramebufferStatus ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 1)
GLenum d_glCheckFramebufferStatus(void *context, GLenum target);

#define FUNID_glCreateShaderProgramv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 2)
GLuint d_glCreateShaderProgramv(void *context, GLenum type, GLsizei count, const GLchar *const *strings);

#define FUNID_glQueryMatrixxOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 3)
GLbitfield d_glQueryMatrixxOES(void *context, GLfixed *mantissa, GLint *exponent);

#define FUNID_glGetFramebufferAttachmentParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 4)
void d_glGetFramebufferAttachmentParameteriv(void *context, GLenum target, GLenum attachment, GLenum pname, GLint *params);

#define FUNID_glGetProgramInfoLog ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 5)
void d_glGetProgramInfoLog(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

#define FUNID_glGetRenderbufferParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 6)
void d_glGetRenderbufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params);

#define FUNID_glGetShaderInfoLog ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 7)
void d_glGetShaderInfoLog(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

#define FUNID_glGetShaderPrecisionFormat ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 8)
void d_glGetShaderPrecisionFormat(void *context, GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);

#define FUNID_glGetShaderSource ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 9)
void d_glGetShaderSource(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);

#define FUNID_glGetTexParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10)
void d_glGetTexParameterfv(void *context, GLenum target, GLenum pname, GLfloat *params);

#define FUNID_glGetTexParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 11)
void d_glGetTexParameteriv(void *context, GLenum target, GLenum pname, GLint *params);

#define FUNID_glGetQueryiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 12)
void d_glGetQueryiv(void *context, GLenum target, GLenum pname, GLint *params);

#define FUNID_glGetQueryObjectuiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 13)
void d_glGetQueryObjectuiv(void *context, GLuint id, GLenum pname, GLuint *params);

#define FUNID_glGetTransformFeedbackVarying ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 14)
void d_glGetTransformFeedbackVarying(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);

#define FUNID_glGetActiveUniformsiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 15)
void d_glGetActiveUniformsiv(void *context, GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);

#define FUNID_glGetActiveUniformBlockiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 16)
void d_glGetActiveUniformBlockiv(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);

#define FUNID_glGetActiveUniformBlockName ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 17)
void d_glGetActiveUniformBlockName(void *context, GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);

#define FUNID_glGetSamplerParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 18)
void d_glGetSamplerParameteriv(void *context, GLuint sampler, GLenum pname, GLint *params);

#define FUNID_glGetSamplerParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 19)
void d_glGetSamplerParameterfv(void *context, GLuint sampler, GLenum pname, GLfloat *params);

#define FUNID_glGetProgramBinary ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 20)
void d_glGetProgramBinary(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);

#define FUNID_glGetInternalformativ ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 21)
void d_glGetInternalformativ(void *context, GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params);

#define FUNID_glGetClipPlanexOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 22)
void d_glGetClipPlanexOES(void *context, GLenum plane, GLfixed *equation);

#define FUNID_glGetFixedvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 23)
void d_glGetFixedvOES(void *context, GLenum pname, GLfixed *params);

#define FUNID_glGetTexEnvxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 24)
void d_glGetTexEnvxvOES(void *context, GLenum target, GLenum pname, GLfixed *params);

#define FUNID_glGetTexParameterxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 25)
void d_glGetTexParameterxvOES(void *context, GLenum target, GLenum pname, GLfixed *params);

#define FUNID_glGetLightxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 26)
void d_glGetLightxvOES(void *context, GLenum light, GLenum pname, GLfixed *params);

#define FUNID_glGetMaterialxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 27)
void d_glGetMaterialxvOES(void *context, GLenum face, GLenum pname, GLfixed *params);

#define FUNID_glGetTexGenxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 28)
void d_glGetTexGenxvOES(void *context, GLenum coord, GLenum pname, GLfixed *params);

#define FUNID_glReadPixels_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 29)
void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);

#define FUNID_glGetFramebufferParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 30)
void d_glGetFramebufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params);

#define FUNID_glGetProgramInterfaceiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 31)
void d_glGetProgramInterfaceiv(void *context, GLuint program, GLenum programInterface, GLenum pname, GLint *params);

#define FUNID_glGetProgramResourceName ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 32)
void d_glGetProgramResourceName(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);

#define FUNID_glGetProgramResourceiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 33)
void d_glGetProgramResourceiv(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params);

#define FUNID_glGetProgramPipelineiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 34)
void d_glGetProgramPipelineiv(void *context, GLuint pipeline, GLenum pname, GLint *params);

#define FUNID_glGetProgramPipelineInfoLog ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 35)
void d_glGetProgramPipelineInfoLog(void *context, GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

#define FUNID_glGetMultisamplefv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 36)
void d_glGetMultisamplefv(void *context, GLenum pname, GLuint index, GLfloat *val);

#define FUNID_glGetTexLevelParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 37)
void d_glGetTexLevelParameteriv(void *context, GLenum target, GLint level, GLenum pname, GLint *params);

#define FUNID_glGetTexLevelParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 38)
void d_glGetTexLevelParameterfv(void *context, GLenum target, GLint level, GLenum pname, GLfloat *params);

#define FUNID_glFenceSync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 39)
GLsync d_glFenceSync(void *context, GLenum condition, GLbitfield flags);

#define FUNID_glIsSync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 40)
GLboolean d_glIsSync(void *context, GLsync sync);

#define FUNID_glGetSynciv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 41)
void d_glGetSynciv(void *context, GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);

#define FUNID_glClientWaitSync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 42)
GLenum d_glClientWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);

#define FUNID_glFinish ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 43)
void d_glFinish(void *context);

#define FUNID_glTestInt1 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 44)
GLint d_glTestInt1(void *context, GLint a, GLuint b);

#define FUNID_glTestInt2 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 45)
GLuint d_glTestInt2(void *context, GLint a, GLuint b);

#define FUNID_glTestInt3 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 46)
GLint64 d_glTestInt3(void *context, GLint64 a, GLuint64 b);

#define FUNID_glTestInt4 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 47)
GLuint64 d_glTestInt4(void *context, GLint64 a, GLuint64 b);

#define FUNID_glTestInt5 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 48)
GLfloat d_glTestInt5(void *context, GLint a, GLuint b);

#define FUNID_glTestInt6 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 49)
GLdouble d_glTestInt6(void *context, GLint a, GLuint b);

#define FUNID_glTestPointer1 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 50)
void d_glTestPointer1(void *context, GLint a, const GLint *b);

#define FUNID_glTestPointer2 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 51)
void d_glTestPointer2(void *context, GLint a, const GLint *b, GLint *c);

#define FUNID_glTestPointer4 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 52)
GLint d_glTestPointer4(void *context, GLint a, const GLint *b, GLint *c);

#define FUNID_glTestString ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 53)
void d_glTestString(void *context, GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf);

#define FUNID_glMapBufferRange_read ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 54)
void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf);

#define FUNID_glReadPixels_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 55)
void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels);

#define FUNID_glTestPointer3 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 56)
GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c);

#define FUNID_glGetError ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 57)
GLenum d_glGetError_origin(void *context);
GLenum d_glGetError(void *context);

#define FUNID_glIsBuffer ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 58)
GLboolean d_glIsBuffer_origin(void *context, GLuint buffer);
GLboolean d_glIsBuffer(void *context, GLuint buffer);

#define FUNID_glIsEnabled ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 59)
GLboolean d_glIsEnabled_origin(void *context, GLenum cap);
GLboolean d_glIsEnabled(void *context, GLenum cap);

#define FUNID_glIsFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 60)
GLboolean d_glIsFramebuffer_origin(void *context, GLuint framebuffer);
GLboolean d_glIsFramebuffer(void *context, GLuint framebuffer);

#define FUNID_glIsProgram ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 61)
GLboolean d_glIsProgram_origin(void *context, GLuint program);
GLboolean d_glIsProgram(void *context, GLuint program);

#define FUNID_glIsRenderbuffer ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 62)
GLboolean d_glIsRenderbuffer_origin(void *context, GLuint renderbuffer);
GLboolean d_glIsRenderbuffer(void *context, GLuint renderbuffer);

#define FUNID_glIsShader ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 63)
GLboolean d_glIsShader_origin(void *context, GLuint shader);
GLboolean d_glIsShader(void *context, GLuint shader);

#define FUNID_glIsTexture ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 64)
GLboolean d_glIsTexture_origin(void *context, GLuint texture);
GLboolean d_glIsTexture(void *context, GLuint texture);

#define FUNID_glIsQuery ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 65)
GLboolean d_glIsQuery_origin(void *context, GLuint id);
GLboolean d_glIsQuery(void *context, GLuint id);

#define FUNID_glIsVertexArray ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 66)
GLboolean d_glIsVertexArray_origin(void *context, GLuint array);
GLboolean d_glIsVertexArray(void *context, GLuint array);

#define FUNID_glIsSampler ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 67)
GLboolean d_glIsSampler_origin(void *context, GLuint sampler);
GLboolean d_glIsSampler(void *context, GLuint sampler);

#define FUNID_glIsTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 68)
GLboolean d_glIsTransformFeedback_origin(void *context, GLuint id);
GLboolean d_glIsTransformFeedback(void *context, GLuint id);

#define FUNID_glGetAttribLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 69)
GLint d_glGetAttribLocation_origin(void *context, GLuint program, const GLchar *name);
GLint d_glGetAttribLocation(void *context, GLuint program, const GLchar *name);

#define FUNID_glGetUniformLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 70)
GLint d_glGetUniformLocation_origin(void *context, GLuint program, const GLchar *name);
GLint d_glGetUniformLocation(void *context, GLuint program, const GLchar *name);

#define FUNID_glGetFragDataLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 71)
GLint d_glGetFragDataLocation_origin(void *context, GLuint program, const GLchar *name);
GLint d_glGetFragDataLocation(void *context, GLuint program, const GLchar *name);

#define FUNID_glGetUniformBlockIndex ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 72)
GLuint d_glGetUniformBlockIndex_origin(void *context, GLuint program, const GLchar *uniformBlockName);
GLuint d_glGetUniformBlockIndex(void *context, GLuint program, const GLchar *uniformBlockName);

#define FUNID_glGetProgramResourceIndex ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 73)
GLuint d_glGetProgramResourceIndex_origin(void *context, GLuint program, GLenum programInterface, const GLchar *name);
GLuint d_glGetProgramResourceIndex(void *context, GLuint program, GLenum programInterface, const GLchar *name);

#define FUNID_glGetProgramResourceLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 74)
GLint d_glGetProgramResourceLocation_origin(void *context, GLuint program, GLenum programInterface, const GLchar *name);
GLint d_glGetProgramResourceLocation(void *context, GLuint program, GLenum programInterface, const GLchar *name);

#define FUNID_glGetActiveAttrib ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 75)
void d_glGetActiveAttrib_origin(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void d_glGetActiveAttrib(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);

#define FUNID_glGetActiveUniform ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 76)
void d_glGetActiveUniform_origin(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void d_glGetActiveUniform(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);

#define FUNID_glGetAttachedShaders ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 77)
void d_glGetAttachedShaders_origin(void *context, GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
void d_glGetAttachedShaders(void *context, GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);

#define FUNID_glGetProgramiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 78)
void d_glGetProgramiv_origin(void *context, GLuint program, GLenum pname, GLint *params);
void d_glGetProgramiv(void *context, GLuint program, GLenum pname, GLint *params);

#define FUNID_glGetShaderiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 79)
void d_glGetShaderiv_origin(void *context, GLuint shader, GLenum pname, GLint *params);
void d_glGetShaderiv(void *context, GLuint shader, GLenum pname, GLint *params);

#define FUNID_glGetUniformfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 80)
void d_glGetUniformfv_origin(void *context, GLuint program, GLint location, GLfloat *params);
void d_glGetUniformfv(void *context, GLuint program, GLint location, GLfloat *params);

#define FUNID_glGetUniformiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 81)
void d_glGetUniformiv_origin(void *context, GLuint program, GLint location, GLint *params);
void d_glGetUniformiv(void *context, GLuint program, GLint location, GLint *params);

#define FUNID_glGetUniformuiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 82)
void d_glGetUniformuiv_origin(void *context, GLuint program, GLint location, GLuint *params);
void d_glGetUniformuiv(void *context, GLuint program, GLint location, GLuint *params);

#define FUNID_glGetUniformIndices ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 83)
void d_glGetUniformIndices_origin(void *context, GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices);
void d_glGetUniformIndices(void *context, GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices);

#define FUNID_glGetVertexAttribfv_origin ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 84)
void d_glGetVertexAttribfv_origin(void *context, GLuint index, GLenum pname, GLfloat *params);

#define FUNID_glGetVertexAttribiv_origin ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 85)
void d_glGetVertexAttribiv_origin(void *context, GLuint index, GLenum pname, GLint *params);

#define FUNID_glGetVertexAttribIiv_origin ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 86)
void d_glGetVertexAttribIiv_origin(void *context, GLuint index, GLenum pname, GLint *params);

#define FUNID_glGetVertexAttribIuiv_origin ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 87)
void d_glGetVertexAttribIuiv_origin(void *context, GLuint index, GLenum pname, GLuint *params);

#define FUNID_glGetBufferParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 88)
void d_glGetBufferParameteriv_origin(void *context, GLenum target, GLenum pname, GLint *params);
void d_glGetBufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params);

#define FUNID_glGetBufferParameteri64v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 89)
void d_glGetBufferParameteri64v_origin(void *context, GLenum target, GLenum pname, GLint64 *params);
void d_glGetBufferParameteri64v(void *context, GLenum target, GLenum pname, GLint64 *params);

#define FUNID_glGetBooleanv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 90)
void d_glGetBooleanv_origin(void *context, GLenum pname, GLboolean *data);
void d_glGetBooleanv(void *context, GLenum pname, GLboolean *data);

#define FUNID_glGetBooleani_v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 91)
void d_glGetBooleani_v_origin(void *context, GLenum target, GLuint index, GLboolean *data);
void d_glGetBooleani_v(void *context, GLenum target, GLuint index, GLboolean *data);

#define FUNID_glGetFloatv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 92)
void d_glGetFloatv_origin(void *context, GLenum pname, GLfloat *data);
void d_glGetFloatv(void *context, GLenum pname, GLfloat *data);

#define FUNID_glGetIntegerv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 93)
void d_glGetIntegerv_origin(void *context, GLenum pname, GLint *data);
void d_glGetIntegerv(void *context, GLenum pname, GLint *data);

#define FUNID_glGetIntegeri_v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 94)
void d_glGetIntegeri_v_origin(void *context, GLenum target, GLuint index, GLint *data);
void d_glGetIntegeri_v(void *context, GLenum target, GLuint index, GLint *data);

#define FUNID_glGetInteger64v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 95)
void d_glGetInteger64v_origin(void *context, GLenum pname, GLint64 *data);
void d_glGetInteger64v(void *context, GLenum pname, GLint64 *data);

#define FUNID_glGetInteger64i_v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 96)
void d_glGetInteger64i_v_origin(void *context, GLenum target, GLuint index, GLint64 *data);
void d_glGetInteger64i_v(void *context, GLenum target, GLuint index, GLint64 *data);

#define FUNID_glActiveTexture ((EXPRESS_GPU_FUN_ID << 32u) + 97)
void d_glActiveTexture(void *context, GLenum texture);

#define FUNID_glAttachShader ((EXPRESS_GPU_FUN_ID << 32u) + 98)
void d_glAttachShader(void *context, GLuint program, GLuint shader);

#define FUNID_glBindBuffer_origin ((EXPRESS_GPU_FUN_ID << 32u) + 99)
void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer);

#define FUNID_glBindFramebuffer_special ((EXPRESS_GPU_FUN_ID << 32u) + 100)
void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer);

#define FUNID_glBindRenderbuffer ((EXPRESS_GPU_FUN_ID << 32u) + 101)
void d_glBindRenderbuffer(void *context, GLenum target, GLuint renderbuffer);

#define FUNID_glBindTexture ((EXPRESS_GPU_FUN_ID << 32u) + 102)
void d_glBindTexture(void *context, GLenum target, GLuint texture);

#define FUNID_glBlendColor ((EXPRESS_GPU_FUN_ID << 32u) + 103)
void d_glBlendColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

#define FUNID_glBlendEquation ((EXPRESS_GPU_FUN_ID << 32u) + 104)
void d_glBlendEquation(void *context, GLenum mode);

#define FUNID_glBlendEquationSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 105)
void d_glBlendEquationSeparate(void *context, GLenum modeRGB, GLenum modeAlpha);

#define FUNID_glBlendFunc ((EXPRESS_GPU_FUN_ID << 32u) + 106)
void d_glBlendFunc(void *context, GLenum sfactor, GLenum dfactor);

#define FUNID_glBlendFuncSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 107)
void d_glBlendFuncSeparate(void *context, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

#define FUNID_glClear ((EXPRESS_GPU_FUN_ID << 32u) + 108)
void d_glClear(void *context, GLbitfield mask);

#define FUNID_glClearColor ((EXPRESS_GPU_FUN_ID << 32u) + 109)
void d_glClearColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

#define FUNID_glClearDepthf ((EXPRESS_GPU_FUN_ID << 32u) + 110)
void d_glClearDepthf(void *context, GLfloat d);

#define FUNID_glClearStencil ((EXPRESS_GPU_FUN_ID << 32u) + 111)
void d_glClearStencil(void *context, GLint s);

#define FUNID_glColorMask ((EXPRESS_GPU_FUN_ID << 32u) + 112)
void d_glColorMask(void *context, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

#define FUNID_glCompileShader ((EXPRESS_GPU_FUN_ID << 32u) + 113)
void d_glCompileShader(void *context, GLuint shader);

#define FUNID_glCopyTexImage2D ((EXPRESS_GPU_FUN_ID << 32u) + 114)
void d_glCopyTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);

#define FUNID_glCopyTexSubImage2D ((EXPRESS_GPU_FUN_ID << 32u) + 115)
void d_glCopyTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);

#define FUNID_glCullFace ((EXPRESS_GPU_FUN_ID << 32u) + 116)
void d_glCullFace(void *context, GLenum mode);

#define FUNID_glDeleteProgram_origin ((EXPRESS_GPU_FUN_ID << 32u) + 117)
void d_glDeleteProgram_origin(void *context, GLuint program);

#define FUNID_glDeleteShader ((EXPRESS_GPU_FUN_ID << 32u) + 118)
void d_glDeleteShader(void *context, GLuint shader);

#define FUNID_glDepthFunc ((EXPRESS_GPU_FUN_ID << 32u) + 119)
void d_glDepthFunc(void *context, GLenum func);

#define FUNID_glDepthMask ((EXPRESS_GPU_FUN_ID << 32u) + 120)
void d_glDepthMask(void *context, GLboolean flag);

#define FUNID_glDepthRangef ((EXPRESS_GPU_FUN_ID << 32u) + 121)
void d_glDepthRangef(void *context, GLfloat n, GLfloat f);

#define FUNID_glDetachShader ((EXPRESS_GPU_FUN_ID << 32u) + 122)
void d_glDetachShader(void *context, GLuint program, GLuint shader);

#define FUNID_glDisable ((EXPRESS_GPU_FUN_ID << 32u) + 123)
void d_glDisable(void *context, GLenum cap);

#define FUNID_glDisableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID << 32u) + 124)
void d_glDisableVertexAttribArray_origin(void *context, GLuint index);

#define FUNID_glDrawArrays_origin ((EXPRESS_GPU_FUN_ID << 32u) + 125)
void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count);

#define FUNID_glEnable ((EXPRESS_GPU_FUN_ID << 32u) + 126)
void d_glEnable(void *context, GLenum cap);

#define FUNID_glEnableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID << 32u) + 127)
void d_glEnableVertexAttribArray_origin(void *context, GLuint index);

#define FUNID_glFlush ((EXPRESS_GPU_FUN_ID << 32u) + 128)
void d_glFlush(void *context);

#define FUNID_glFramebufferRenderbuffer ((EXPRESS_GPU_FUN_ID << 32u) + 129)
void d_glFramebufferRenderbuffer(void *context, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

#define FUNID_glFramebufferTexture2D ((EXPRESS_GPU_FUN_ID << 32u) + 130)
void d_glFramebufferTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

#define FUNID_glFrontFace ((EXPRESS_GPU_FUN_ID << 32u) + 131)
void d_glFrontFace(void *context, GLenum mode);

#define FUNID_glGenerateMipmap ((EXPRESS_GPU_FUN_ID << 32u) + 132)
void d_glGenerateMipmap(void *context, GLenum target);

#define FUNID_glHint ((EXPRESS_GPU_FUN_ID << 32u) + 133)
void d_glHint(void *context, GLenum target, GLenum mode);

#define FUNID_glLineWidth ((EXPRESS_GPU_FUN_ID << 32u) + 134)
void d_glLineWidth(void *context, GLfloat width);

#define FUNID_glLinkProgram_origin ((EXPRESS_GPU_FUN_ID << 32u) + 135)
void d_glLinkProgram_origin(void *context, GLuint program);

#define FUNID_glPixelStorei_origin ((EXPRESS_GPU_FUN_ID << 32u) + 136)
void d_glPixelStorei_origin(void *context, GLenum pname, GLint param);

#define FUNID_glPolygonOffset ((EXPRESS_GPU_FUN_ID << 32u) + 137)
void d_glPolygonOffset(void *context, GLfloat factor, GLfloat units);

#define FUNID_glReleaseShaderCompiler ((EXPRESS_GPU_FUN_ID << 32u) + 138)
void d_glReleaseShaderCompiler(void *context);

#define FUNID_glRenderbufferStorage ((EXPRESS_GPU_FUN_ID << 32u) + 139)
void d_glRenderbufferStorage(void *context, GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

#define FUNID_glSampleCoverage ((EXPRESS_GPU_FUN_ID << 32u) + 140)
void d_glSampleCoverage(void *context, GLfloat value, GLboolean invert);

#define FUNID_glScissor ((EXPRESS_GPU_FUN_ID << 32u) + 141)
void d_glScissor(void *context, GLint x, GLint y, GLsizei width, GLsizei height);

#define FUNID_glStencilFunc ((EXPRESS_GPU_FUN_ID << 32u) + 142)
void d_glStencilFunc(void *context, GLenum func, GLint ref, GLuint mask);

#define FUNID_glStencilFuncSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 143)
void d_glStencilFuncSeparate(void *context, GLenum face, GLenum func, GLint ref, GLuint mask);

#define FUNID_glStencilMask ((EXPRESS_GPU_FUN_ID << 32u) + 144)
void d_glStencilMask(void *context, GLuint mask);

#define FUNID_glStencilMaskSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 145)
void d_glStencilMaskSeparate(void *context, GLenum face, GLuint mask);

#define FUNID_glStencilOp ((EXPRESS_GPU_FUN_ID << 32u) + 146)
void d_glStencilOp(void *context, GLenum fail, GLenum zfail, GLenum zpass);

#define FUNID_glStencilOpSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 147)
void d_glStencilOpSeparate(void *context, GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);

#define FUNID_glTexParameterf ((EXPRESS_GPU_FUN_ID << 32u) + 148)
void d_glTexParameterf(void *context, GLenum target, GLenum pname, GLfloat param);

#define FUNID_glTexParameteri ((EXPRESS_GPU_FUN_ID << 32u) + 149)
void d_glTexParameteri(void *context, GLenum target, GLenum pname, GLint param);

#define FUNID_glUniform1f ((EXPRESS_GPU_FUN_ID << 32u) + 150)
void d_glUniform1f(void *context, GLint location, GLfloat v0);

#define FUNID_glUniform1i ((EXPRESS_GPU_FUN_ID << 32u) + 151)
void d_glUniform1i(void *context, GLint location, GLint v0);

#define FUNID_glUniform2f ((EXPRESS_GPU_FUN_ID << 32u) + 152)
void d_glUniform2f(void *context, GLint location, GLfloat v0, GLfloat v1);

#define FUNID_glUniform2i ((EXPRESS_GPU_FUN_ID << 32u) + 153)
void d_glUniform2i(void *context, GLint location, GLint v0, GLint v1);

#define FUNID_glUniform3f ((EXPRESS_GPU_FUN_ID << 32u) + 154)
void d_glUniform3f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);

#define FUNID_glUniform3i ((EXPRESS_GPU_FUN_ID << 32u) + 155)
void d_glUniform3i(void *context, GLint location, GLint v0, GLint v1, GLint v2);

#define FUNID_glUniform4f ((EXPRESS_GPU_FUN_ID << 32u) + 156)
void d_glUniform4f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

#define FUNID_glUniform4i ((EXPRESS_GPU_FUN_ID << 32u) + 157)
void d_glUniform4i(void *context, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

#define FUNID_glUseProgram ((EXPRESS_GPU_FUN_ID << 32u) + 158)
void d_glUseProgram(void *context, GLuint program);

#define FUNID_glValidateProgram ((EXPRESS_GPU_FUN_ID << 32u) + 159)
void d_glValidateProgram(void *context, GLuint program);

#define FUNID_glVertexAttrib1f ((EXPRESS_GPU_FUN_ID << 32u) + 160)
void d_glVertexAttrib1f(void *context, GLuint index, GLfloat x);

#define FUNID_glVertexAttrib2f ((EXPRESS_GPU_FUN_ID << 32u) + 161)
void d_glVertexAttrib2f(void *context, GLuint index, GLfloat x, GLfloat y);

#define FUNID_glVertexAttrib3f ((EXPRESS_GPU_FUN_ID << 32u) + 162)
void d_glVertexAttrib3f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z);

#define FUNID_glVertexAttrib4f ((EXPRESS_GPU_FUN_ID << 32u) + 163)
void d_glVertexAttrib4f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

#define FUNID_glViewport ((EXPRESS_GPU_FUN_ID << 32u) + 164)
void d_glViewport(void *context, GLint x, GLint y, GLsizei width, GLsizei height);

#define FUNID_glReadBuffer_special ((EXPRESS_GPU_FUN_ID << 32u) + 165)
void d_glReadBuffer_special(void *context, GLenum src);

#define FUNID_glCopyTexSubImage3D ((EXPRESS_GPU_FUN_ID << 32u) + 166)
void d_glCopyTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

#define FUNID_glBeginQuery ((EXPRESS_GPU_FUN_ID << 32u) + 167)
void d_glBeginQuery(void *context, GLenum target, GLuint id);

#define FUNID_glEndQuery ((EXPRESS_GPU_FUN_ID << 32u) + 168)
void d_glEndQuery(void *context, GLenum target);

#define FUNID_glBlitFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + 169)
void d_glBlitFramebuffer(void *context, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

#define FUNID_glRenderbufferStorageMultisample ((EXPRESS_GPU_FUN_ID << 32u) + 170)
void d_glRenderbufferStorageMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

#define FUNID_glFramebufferTextureLayer ((EXPRESS_GPU_FUN_ID << 32u) + 171)
void d_glFramebufferTextureLayer(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);

#define FUNID_glBindVertexArray_origin ((EXPRESS_GPU_FUN_ID << 32u) + 172)
void d_glBindVertexArray_origin(void *context, GLuint array);

#define FUNID_glBeginTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 173)
void d_glBeginTransformFeedback(void *context, GLenum primitiveMode);

#define FUNID_glEndTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 174)
void d_glEndTransformFeedback(void *context);

#define FUNID_glBindBufferRange ((EXPRESS_GPU_FUN_ID << 32u) + 175)
void d_glBindBufferRange(void *context, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);

#define FUNID_glBindBufferBase ((EXPRESS_GPU_FUN_ID << 32u) + 176)
void d_glBindBufferBase(void *context, GLenum target, GLuint index, GLuint buffer);

#define FUNID_glVertexAttribI4i ((EXPRESS_GPU_FUN_ID << 32u) + 177)
void d_glVertexAttribI4i(void *context, GLuint index, GLint x, GLint y, GLint z, GLint w);

#define FUNID_glVertexAttribI4ui ((EXPRESS_GPU_FUN_ID << 32u) + 178)
void d_glVertexAttribI4ui(void *context, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);

#define FUNID_glUniform1ui ((EXPRESS_GPU_FUN_ID << 32u) + 179)
void d_glUniform1ui(void *context, GLint location, GLuint v0);

#define FUNID_glUniform2ui ((EXPRESS_GPU_FUN_ID << 32u) + 180)
void d_glUniform2ui(void *context, GLint location, GLuint v0, GLuint v1);

#define FUNID_glUniform3ui ((EXPRESS_GPU_FUN_ID << 32u) + 181)
void d_glUniform3ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2);

#define FUNID_glUniform4ui ((EXPRESS_GPU_FUN_ID << 32u) + 182)
void d_glUniform4ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);

#define FUNID_glClearBufferfi ((EXPRESS_GPU_FUN_ID << 32u) + 183)
void d_glClearBufferfi(void *context, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);

#define FUNID_glCopyBufferSubData ((EXPRESS_GPU_FUN_ID << 32u) + 184)
void d_glCopyBufferSubData(void *context, GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);

#define FUNID_glUniformBlockBinding ((EXPRESS_GPU_FUN_ID << 32u) + 185)
void d_glUniformBlockBinding(void *context, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

#define FUNID_glDrawArraysInstanced_origin ((EXPRESS_GPU_FUN_ID << 32u) + 186)
void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

#define FUNID_glBindSampler ((EXPRESS_GPU_FUN_ID << 32u) + 187)
void d_glBindSampler(void *context, GLuint unit, GLuint sampler);

#define FUNID_glSamplerParameteri ((EXPRESS_GPU_FUN_ID << 32u) + 188)
void d_glSamplerParameteri(void *context, GLuint sampler, GLenum pname, GLint param);

#define FUNID_glSamplerParameterf ((EXPRESS_GPU_FUN_ID << 32u) + 189)
void d_glSamplerParameterf(void *context, GLuint sampler, GLenum pname, GLfloat param);

#define FUNID_glVertexAttribDivisor_origin ((EXPRESS_GPU_FUN_ID << 32u) + 190)
void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor);

#define FUNID_glBindTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 191)
void d_glBindTransformFeedback(void *context, GLenum target, GLuint id);

#define FUNID_glPauseTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 192)
void d_glPauseTransformFeedback(void *context);

#define FUNID_glResumeTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 193)
void d_glResumeTransformFeedback(void *context);

#define FUNID_glProgramParameteri ((EXPRESS_GPU_FUN_ID << 32u) + 194)
void d_glProgramParameteri(void *context, GLuint program, GLenum pname, GLint value);

#define FUNID_glTexStorage2D ((EXPRESS_GPU_FUN_ID << 32u) + 195)
void d_glTexStorage2D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);

#define FUNID_glTexStorage3D ((EXPRESS_GPU_FUN_ID << 32u) + 196)
void d_glTexStorage3D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

#define FUNID_glAlphaFuncxOES ((EXPRESS_GPU_FUN_ID << 32u) + 197)
void d_glAlphaFuncxOES(void *context, GLenum func, GLfixed ref);

#define FUNID_glClearColorxOES ((EXPRESS_GPU_FUN_ID << 32u) + 198)
void d_glClearColorxOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);

#define FUNID_glClearDepthxOES ((EXPRESS_GPU_FUN_ID << 32u) + 199)
void d_glClearDepthxOES(void *context, GLfixed depth);

#define FUNID_glColor4xOES ((EXPRESS_GPU_FUN_ID << 32u) + 200)
void d_glColor4xOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);

#define FUNID_glDepthRangexOES ((EXPRESS_GPU_FUN_ID << 32u) + 201)
void d_glDepthRangexOES(void *context, GLfixed n, GLfixed f);

#define FUNID_glFogxOES ((EXPRESS_GPU_FUN_ID << 32u) + 202)
void d_glFogxOES(void *context, GLenum pname, GLfixed param);

#define FUNID_glFrustumxOES ((EXPRESS_GPU_FUN_ID << 32u) + 203)
void d_glFrustumxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);

#define FUNID_glLightModelxOES ((EXPRESS_GPU_FUN_ID << 32u) + 204)
void d_glLightModelxOES(void *context, GLenum pname, GLfixed param);

#define FUNID_glLightxOES ((EXPRESS_GPU_FUN_ID << 32u) + 205)
void d_glLightxOES(void *context, GLenum light, GLenum pname, GLfixed param);

#define FUNID_glLineWidthxOES ((EXPRESS_GPU_FUN_ID << 32u) + 206)
void d_glLineWidthxOES(void *context, GLfixed width);

#define FUNID_glMaterialxOES ((EXPRESS_GPU_FUN_ID << 32u) + 207)
void d_glMaterialxOES(void *context, GLenum face, GLenum pname, GLfixed param);

#define FUNID_glMultiTexCoord4xOES ((EXPRESS_GPU_FUN_ID << 32u) + 208)
void d_glMultiTexCoord4xOES(void *context, GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q);

#define FUNID_glNormal3xOES ((EXPRESS_GPU_FUN_ID << 32u) + 209)
void d_glNormal3xOES(void *context, GLfixed nx, GLfixed ny, GLfixed nz);

#define FUNID_glOrthoxOES ((EXPRESS_GPU_FUN_ID << 32u) + 210)
void d_glOrthoxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);

#define FUNID_glPointSizexOES ((EXPRESS_GPU_FUN_ID << 32u) + 211)
void d_glPointSizexOES(void *context, GLfixed size);

#define FUNID_glPolygonOffsetxOES ((EXPRESS_GPU_FUN_ID << 32u) + 212)
void d_glPolygonOffsetxOES(void *context, GLfixed factor, GLfixed units);

#define FUNID_glRotatexOES ((EXPRESS_GPU_FUN_ID << 32u) + 213)
void d_glRotatexOES(void *context, GLfixed angle, GLfixed x, GLfixed y, GLfixed z);

#define FUNID_glScalexOES ((EXPRESS_GPU_FUN_ID << 32u) + 214)
void d_glScalexOES(void *context, GLfixed x, GLfixed y, GLfixed z);

#define FUNID_glTexEnvxOES ((EXPRESS_GPU_FUN_ID << 32u) + 215)
void d_glTexEnvxOES(void *context, GLenum target, GLenum pname, GLfixed param);

#define FUNID_glTranslatexOES ((EXPRESS_GPU_FUN_ID << 32u) + 216)
void d_glTranslatexOES(void *context, GLfixed x, GLfixed y, GLfixed z);

#define FUNID_glPointParameterxOES ((EXPRESS_GPU_FUN_ID << 32u) + 217)
void d_glPointParameterxOES(void *context, GLenum pname, GLfixed param);

#define FUNID_glSampleCoveragexOES ((EXPRESS_GPU_FUN_ID << 32u) + 218)
void d_glSampleCoveragexOES(void *context, GLclampx value, GLboolean invert);

#define FUNID_glTexGenxOES ((EXPRESS_GPU_FUN_ID << 32u) + 219)
void d_glTexGenxOES(void *context, GLenum coord, GLenum pname, GLfixed param);

#define FUNID_glClearDepthfOES ((EXPRESS_GPU_FUN_ID << 32u) + 220)
void d_glClearDepthfOES(void *context, GLclampf depth);

#define FUNID_glDepthRangefOES ((EXPRESS_GPU_FUN_ID << 32u) + 221)
void d_glDepthRangefOES(void *context, GLclampf n, GLclampf f);

#define FUNID_glFrustumfOES ((EXPRESS_GPU_FUN_ID << 32u) + 222)
void d_glFrustumfOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);

#define FUNID_glOrthofOES ((EXPRESS_GPU_FUN_ID << 32u) + 223)
void d_glOrthofOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);

#define FUNID_glRenderbufferStorageMultisampleEXT ((EXPRESS_GPU_FUN_ID << 32u) + 224)
void d_glRenderbufferStorageMultisampleEXT(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

#define FUNID_glUseProgramStages ((EXPRESS_GPU_FUN_ID << 32u) + 225)
void d_glUseProgramStages(void *context, GLuint pipeline, GLbitfield stages, GLuint program);

#define FUNID_glActiveShaderProgram ((EXPRESS_GPU_FUN_ID << 32u) + 226)
void d_glActiveShaderProgram(void *context, GLuint pipeline, GLuint program);

#define FUNID_glProgramUniform1i ((EXPRESS_GPU_FUN_ID << 32u) + 227)
void d_glProgramUniform1i(void *context, GLuint program, GLint location, GLint v0);

#define FUNID_glProgramUniform2i ((EXPRESS_GPU_FUN_ID << 32u) + 228)
void d_glProgramUniform2i(void *context, GLuint program, GLint location, GLint v0, GLint v1);

#define FUNID_glProgramUniform3i ((EXPRESS_GPU_FUN_ID << 32u) + 229)
void d_glProgramUniform3i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2);

#define FUNID_glProgramUniform4i ((EXPRESS_GPU_FUN_ID << 32u) + 230)
void d_glProgramUniform4i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

#define FUNID_glProgramUniform1ui ((EXPRESS_GPU_FUN_ID << 32u) + 231)
void d_glProgramUniform1ui(void *context, GLuint program, GLint location, GLuint v0);

#define FUNID_glProgramUniform2ui ((EXPRESS_GPU_FUN_ID << 32u) + 232)
void d_glProgramUniform2ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1);

#define FUNID_glProgramUniform3ui ((EXPRESS_GPU_FUN_ID << 32u) + 233)
void d_glProgramUniform3ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);

#define FUNID_glProgramUniform4ui ((EXPRESS_GPU_FUN_ID << 32u) + 234)
void d_glProgramUniform4ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);

#define FUNID_glProgramUniform1f ((EXPRESS_GPU_FUN_ID << 32u) + 235)
void d_glProgramUniform1f(void *context, GLuint program, GLint location, GLfloat v0);

#define FUNID_glProgramUniform2f ((EXPRESS_GPU_FUN_ID << 32u) + 236)
void d_glProgramUniform2f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1);

#define FUNID_glProgramUniform3f ((EXPRESS_GPU_FUN_ID << 32u) + 237)
void d_glProgramUniform3f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);

#define FUNID_glProgramUniform4f ((EXPRESS_GPU_FUN_ID << 32u) + 238)
void d_glProgramUniform4f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

#define FUNID_glShaderSource_origin ((EXPRESS_GPU_FUN_ID << 32u) + 239)
void d_glShaderSource_origin(void *context, GLuint shader, GLsizei count, const GLint *length, const GLchar *const *string);

#define FUNID_glTransformFeedbackVaryings ((EXPRESS_GPU_FUN_ID << 32u) + 240)
void d_glTransformFeedbackVaryings(void *context, GLuint program, GLsizei count, const GLchar *const *varyings, GLenum bufferMode);

#define FUNID_glDeleteBuffers_origin ((EXPRESS_GPU_FUN_ID << 32u) + 241)
void d_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint *buffers);

#define FUNID_glDeleteFramebuffers ((EXPRESS_GPU_FUN_ID << 32u) + 242)
void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint *framebuffers);

#define FUNID_glDeleteRenderbuffers ((EXPRESS_GPU_FUN_ID << 32u) + 243)
void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers);

#define FUNID_glDeleteTextures ((EXPRESS_GPU_FUN_ID << 32u) + 244)
void d_glDeleteTextures(void *context, GLsizei n, const GLuint *textures);

#define FUNID_glDrawElements_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 245)
void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices);

#define FUNID_glShaderBinary ((EXPRESS_GPU_FUN_ID << 32u) + 246)
void d_glShaderBinary(void *context, GLsizei count, const GLuint *shaders, GLenum binaryFormat, const void *binary, GLsizei length);

#define FUNID_glCompressedTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 247)
void d_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data);

#define FUNID_glTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 248)
void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels);

#define FUNID_glTexParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + 249)
void d_glTexParameterfv(void *context, GLenum target, GLenum pname, const GLfloat *params);

#define FUNID_glTexParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + 250)
void d_glTexParameteriv(void *context, GLenum target, GLenum pname, const GLint *params);

#define FUNID_glTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 251)
void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);

#define FUNID_glUniform1fv ((EXPRESS_GPU_FUN_ID << 32u) + 252)
void d_glUniform1fv(void *context, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glUniform1iv ((EXPRESS_GPU_FUN_ID << 32u) + 253)
void d_glUniform1iv(void *context, GLint location, GLsizei count, const GLint *value);

#define FUNID_glUniform2fv ((EXPRESS_GPU_FUN_ID << 32u) + 254)
void d_glUniform2fv(void *context, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glUniform2iv ((EXPRESS_GPU_FUN_ID << 32u) + 255)
void d_glUniform2iv(void *context, GLint location, GLsizei count, const GLint *value);

#define FUNID_glUniform3fv ((EXPRESS_GPU_FUN_ID << 32u) + 256)
void d_glUniform3fv(void *context, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glUniform3iv ((EXPRESS_GPU_FUN_ID << 32u) + 257)
void d_glUniform3iv(void *context, GLint location, GLsizei count, const GLint *value);

#define FUNID_glUniform4fv ((EXPRESS_GPU_FUN_ID << 32u) + 258)
void d_glUniform4fv(void *context, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glUniform4iv ((EXPRESS_GPU_FUN_ID << 32u) + 259)
void d_glUniform4iv(void *context, GLint location, GLsizei count, const GLint *value);

#define FUNID_glVertexAttrib1fv ((EXPRESS_GPU_FUN_ID << 32u) + 260)
void d_glVertexAttrib1fv(void *context, GLuint index, const GLfloat *v);

#define FUNID_glVertexAttrib2fv ((EXPRESS_GPU_FUN_ID << 32u) + 261)
void d_glVertexAttrib2fv(void *context, GLuint index, const GLfloat *v);

#define FUNID_glVertexAttrib3fv ((EXPRESS_GPU_FUN_ID << 32u) + 262)
void d_glVertexAttrib3fv(void *context, GLuint index, const GLfloat *v);

#define FUNID_glVertexAttrib4fv ((EXPRESS_GPU_FUN_ID << 32u) + 263)
void d_glVertexAttrib4fv(void *context, GLuint index, const GLfloat *v);

#define FUNID_glVertexAttribPointer_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 264)
void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer);

#define FUNID_glVertexAttribPointer_offset ((EXPRESS_GPU_FUN_ID << 32u) + 265)
void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset);

#define FUNID_glDrawRangeElements_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 266)
void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices);

#define FUNID_glTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 267)
void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels);

#define FUNID_glTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 268)
void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels);

#define FUNID_glCompressedTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 269)
void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data);

#define FUNID_glCompressedTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 270)
void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data);

#define FUNID_glCompressedTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 271)
void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data);

#define FUNID_glDeleteQueries ((EXPRESS_GPU_FUN_ID << 32u) + 272)
void d_glDeleteQueries(void *context, GLsizei n, const GLuint *ids);

#define FUNID_glDrawBuffers ((EXPRESS_GPU_FUN_ID << 32u) + 273)
void d_glDrawBuffers(void *context, GLsizei n, const GLenum *bufs);

#define FUNID_glUniformMatrix2fv ((EXPRESS_GPU_FUN_ID << 32u) + 274)
void d_glUniformMatrix2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix3fv ((EXPRESS_GPU_FUN_ID << 32u) + 275)
void d_glUniformMatrix3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix4fv ((EXPRESS_GPU_FUN_ID << 32u) + 276)
void d_glUniformMatrix4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 277)
void d_glUniformMatrix2x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 278)
void d_glUniformMatrix3x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 279)
void d_glUniformMatrix2x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 280)
void d_glUniformMatrix4x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 281)
void d_glUniformMatrix3x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 282)
void d_glUniformMatrix4x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glDeleteVertexArrays_origin ((EXPRESS_GPU_FUN_ID << 32u) + 283)
void d_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint *arrays);

#define FUNID_glVertexAttribIPointer_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 284)
void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer);

#define FUNID_glVertexAttribIPointer_offset ((EXPRESS_GPU_FUN_ID << 32u) + 285)
void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset);

#define FUNID_glVertexAttribI4iv ((EXPRESS_GPU_FUN_ID << 32u) + 286)
void d_glVertexAttribI4iv(void *context, GLuint index, const GLint *v);

#define FUNID_glVertexAttribI4uiv ((EXPRESS_GPU_FUN_ID << 32u) + 287)
void d_glVertexAttribI4uiv(void *context, GLuint index, const GLuint *v);

#define FUNID_glUniform1uiv ((EXPRESS_GPU_FUN_ID << 32u) + 288)
void d_glUniform1uiv(void *context, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glUniform2uiv ((EXPRESS_GPU_FUN_ID << 32u) + 289)
void d_glUniform2uiv(void *context, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glUniform3uiv ((EXPRESS_GPU_FUN_ID << 32u) + 290)
void d_glUniform3uiv(void *context, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glUniform4uiv ((EXPRESS_GPU_FUN_ID << 32u) + 291)
void d_glUniform4uiv(void *context, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glClearBufferiv ((EXPRESS_GPU_FUN_ID << 32u) + 292)
void d_glClearBufferiv(void *context, GLenum buffer, GLint drawbuffer, const GLint *value);

#define FUNID_glClearBufferuiv ((EXPRESS_GPU_FUN_ID << 32u) + 293)
void d_glClearBufferuiv(void *context, GLenum buffer, GLint drawbuffer, const GLuint *value);

#define FUNID_glClearBufferfv ((EXPRESS_GPU_FUN_ID << 32u) + 294)
void d_glClearBufferfv(void *context, GLenum buffer, GLint drawbuffer, const GLfloat *value);

#define FUNID_glDrawElementsInstanced_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 295)
void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount);

#define FUNID_glDeleteSamplers ((EXPRESS_GPU_FUN_ID << 32u) + 296)
void d_glDeleteSamplers(void *context, GLsizei count, const GLuint *samplers);

#define FUNID_glSamplerParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + 297)
void d_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint *param);

#define FUNID_glSamplerParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + 298)
void d_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat *param);

#define FUNID_glDeleteTransformFeedbacks ((EXPRESS_GPU_FUN_ID << 32u) + 299)
void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint *ids);

#define FUNID_glProgramBinary ((EXPRESS_GPU_FUN_ID << 32u) + 300)
void d_glProgramBinary(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);

#define FUNID_glInvalidateFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + 301)
void d_glInvalidateFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments);

#define FUNID_glInvalidateSubFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + 302)
void d_glInvalidateSubFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);

#define FUNID_glClipPlanexOES ((EXPRESS_GPU_FUN_ID << 32u) + 303)
void d_glClipPlanexOES(void *context, GLenum plane, const GLfixed *equation);

#define FUNID_glFogxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 304)
void d_glFogxvOES(void *context, GLenum pname, const GLfixed *param);

#define FUNID_glLightModelxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 305)
void d_glLightModelxvOES(void *context, GLenum pname, const GLfixed *param);

#define FUNID_glLightxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 306)
void d_glLightxvOES(void *context, GLenum light, GLenum pname, const GLfixed *params);

#define FUNID_glLoadMatrixxOES ((EXPRESS_GPU_FUN_ID << 32u) + 307)
void d_glLoadMatrixxOES(void *context, const GLfixed *m);

#define FUNID_glMaterialxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 308)
void d_glMaterialxvOES(void *context, GLenum face, GLenum pname, const GLfixed *param);

#define FUNID_glMultMatrixxOES ((EXPRESS_GPU_FUN_ID << 32u) + 309)
void d_glMultMatrixxOES(void *context, const GLfixed *m);

#define FUNID_glPointParameterxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 310)
void d_glPointParameterxvOES(void *context, GLenum pname, const GLfixed *params);

#define FUNID_glTexEnvxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 311)
void d_glTexEnvxvOES(void *context, GLenum target, GLenum pname, const GLfixed *params);

#define FUNID_glClipPlanefOES ((EXPRESS_GPU_FUN_ID << 32u) + 312)
void d_glClipPlanefOES(void *context, GLenum plane, const GLfloat *equation);

#define FUNID_glTexGenxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 313)
void d_glTexGenxvOES(void *context, GLenum coord, GLenum pname, const GLfixed *params);

#define FUNID_glDeleteProgramPipelines ((EXPRESS_GPU_FUN_ID << 32u) + 314)
void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint *pipelines);

#define FUNID_glProgramUniform1iv ((EXPRESS_GPU_FUN_ID << 32u) + 315)
void d_glProgramUniform1iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value);

#define FUNID_glProgramUniform2iv ((EXPRESS_GPU_FUN_ID << 32u) + 316)
void d_glProgramUniform2iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value);

#define FUNID_glProgramUniform3iv ((EXPRESS_GPU_FUN_ID << 32u) + 317)
void d_glProgramUniform3iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value);

#define FUNID_glProgramUniform4iv ((EXPRESS_GPU_FUN_ID << 32u) + 318)
void d_glProgramUniform4iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value);

#define FUNID_glProgramUniform1uiv ((EXPRESS_GPU_FUN_ID << 32u) + 319)
void d_glProgramUniform1uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glProgramUniform2uiv ((EXPRESS_GPU_FUN_ID << 32u) + 320)
void d_glProgramUniform2uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glProgramUniform3uiv ((EXPRESS_GPU_FUN_ID << 32u) + 321)
void d_glProgramUniform3uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glProgramUniform4uiv ((EXPRESS_GPU_FUN_ID << 32u) + 322)
void d_glProgramUniform4uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value);

#define FUNID_glProgramUniform1fv ((EXPRESS_GPU_FUN_ID << 32u) + 323)
void d_glProgramUniform1fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glProgramUniform2fv ((EXPRESS_GPU_FUN_ID << 32u) + 324)
void d_glProgramUniform2fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glProgramUniform3fv ((EXPRESS_GPU_FUN_ID << 32u) + 325)
void d_glProgramUniform3fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glProgramUniform4fv ((EXPRESS_GPU_FUN_ID << 32u) + 326)
void d_glProgramUniform4fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value);

#define FUNID_glProgramUniformMatrix2fv ((EXPRESS_GPU_FUN_ID << 32u) + 327)
void d_glProgramUniformMatrix2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix3fv ((EXPRESS_GPU_FUN_ID << 32u) + 328)
void d_glProgramUniformMatrix3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix4fv ((EXPRESS_GPU_FUN_ID << 32u) + 329)
void d_glProgramUniformMatrix4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 330)
void d_glProgramUniformMatrix2x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 331)
void d_glProgramUniformMatrix3x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 332)
void d_glProgramUniformMatrix2x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 333)
void d_glProgramUniformMatrix4x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 334)
void d_glProgramUniformMatrix3x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glProgramUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 335)
void d_glProgramUniformMatrix4x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define FUNID_glDeleteSync ((EXPRESS_GPU_FUN_ID << 32u) + 336)
void d_glDeleteSync(void *context, GLsync sync);

#define FUNID_glWaitSync ((EXPRESS_GPU_FUN_ID << 32u) + 337)
void d_glWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);

#define FUNID_glBindAttribLocation ((EXPRESS_GPU_FUN_ID << 32u) + 338)
void d_glBindAttribLocation(void *context, GLuint program, GLuint index, const GLchar *name);

#define FUNID_glMapBufferRange_write ((EXPRESS_GPU_FUN_ID << 32u) + 339)
void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);

#define FUNID_glUnmapBuffer_special ((EXPRESS_GPU_FUN_ID << 32u) + 340)
GLboolean d_glUnmapBuffer_special(void *context, GLenum target);

#define FUNID_glTestIntAsyn ((EXPRESS_GPU_FUN_ID << 32u) + 341)
void d_glTestIntAsyn(void *context, GLint a, GLuint b, GLfloat c, GLdouble d);

#define FUNID_glPrintfAsyn ((EXPRESS_GPU_FUN_ID << 32u) + 342)
void d_glPrintfAsyn(void *context, GLint a, GLuint size, GLdouble c, const GLchar *out_string);

#define FUNID_glDrawElements_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 343)
void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices);

#define FUNID_glVertexAttribIPointer_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 344)
void d_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLsizei length, const void *pointer);

#define FUNID_glVertexAttribPointer_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 345)
void d_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer);

#define FUNID_glDrawElementsInstanced_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 346)
void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);

#define FUNID_glBufferData_custom ((EXPRESS_GPU_FUN_ID << 32u) + 347)
void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage);

#define FUNID_glBufferSubData_custom ((EXPRESS_GPU_FUN_ID << 32u) + 348)
void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data);

#define FUNID_glCompressedTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 349)
void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);

#define FUNID_glCompressedTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 350)
void d_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);

#define FUNID_glTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 351)
void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels);

#define FUNID_glDrawRangeElements_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 352)
void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);

#define FUNID_glCompressedTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 353)
void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);

#define FUNID_glCompressedTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 354)
void d_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);

#define FUNID_glTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 355)
void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels);

#define FUNID_glTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 356)
void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels);

#define FUNID_glTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 357)
void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels);

#define FUNID_glFlushMappedBufferRange_special ((EXPRESS_GPU_FUN_ID << 32u) + 358)
void d_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data);

#define FUNID_glPrintf ((EXPRESS_GPU_FUN_ID << 32u) + 359)
void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string);

#define FUNID_glGenBuffers ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 360)
void d_glGenBuffers_origin(void *context, GLsizei n, GLuint *buffers);
void d_glGenBuffers(void *context, GLsizei n, GLuint *buffers);

#define FUNID_glGenFramebuffers ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 361)
void d_glGenFramebuffers_origin(void *context, GLsizei n, GLuint *framebuffers);
void d_glGenFramebuffers(void *context, GLsizei n, GLuint *framebuffers);

#define FUNID_glGenRenderbuffers ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 362)
void d_glGenRenderbuffers_origin(void *context, GLsizei n, GLuint *renderbuffers);
void d_glGenRenderbuffers(void *context, GLsizei n, GLuint *renderbuffers);

#define FUNID_glGenTextures ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 363)
void d_glGenTextures_origin(void *context, GLsizei n, GLuint *textures);
void d_glGenTextures(void *context, GLsizei n, GLuint *textures);

#define FUNID_glGenQueries ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 364)
void d_glGenQueries_origin(void *context, GLsizei n, GLuint *ids);
void d_glGenQueries(void *context, GLsizei n, GLuint *ids);

#define FUNID_glGenVertexArrays_origin ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 365)
void d_glGenVertexArrays_origin(void *context, GLsizei n, GLuint *arrays);

#define FUNID_glGenSamplers ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 366)
void d_glGenSamplers_origin(void *context, GLsizei count, GLuint *samplers);
void d_glGenSamplers(void *context, GLsizei count, GLuint *samplers);

#define FUNID_glGenTransformFeedbacks ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 367)
void d_glGenTransformFeedbacks_origin(void *context, GLsizei n, GLuint *ids);
void d_glGenTransformFeedbacks(void *context, GLsizei n, GLuint *ids);

#define FUNID_glGenProgramPipelines ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 368)
void d_glGenProgramPipelines_origin(void *context, GLsizei n, GLuint *pipelines);
void d_glGenProgramPipelines(void *context, GLsizei n, GLuint *pipelines);

#define FUNID_glCreateProgram ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 369)
GLuint d_glCreateProgram_origin(void *context);
GLuint d_glCreateProgram(void *context);

#define FUNID_glCreateShader ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 370)
GLuint d_glCreateShader_origin(void *context, GLenum type);
GLuint d_glCreateShader(void *context, GLenum type);

#endif
