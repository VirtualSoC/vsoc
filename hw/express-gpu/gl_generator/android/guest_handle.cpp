
#include "define_gl.h"
#include "glv3_utils.h"

//#include "all_gl.h"

#include <cstring>

/******* file '1-1' *******/

GLenum d_glClientWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout)
{

    return r_glClientWaitSync(context, sync, flags, timeout);
}

GLint d_glTestInt1(void *context, GLint a, GLuint b)
{

    return r_glTestInt1(context, a, b);
}

GLuint d_glTestInt2(void *context, GLint a, GLuint b)
{

    return r_glTestInt2(context, a, b);
}

GLint64 d_glTestInt3(void *context, GLint64 a, GLuint64 b)
{

    return r_glTestInt3(context, a, b);
}

GLuint64 d_glTestInt4(void *context, GLint64 a, GLuint64 b)
{

    return r_glTestInt4(context, a, b);
}

GLfloat d_glTestInt5(void *context, GLint a, GLuint b)
{

    return r_glTestInt5(context, a, b);
}

GLdouble d_glTestInt6(void *context, GLint a, GLuint b)
{

    return r_glTestInt6(context, a, b);
}

void d_glTestPointer1(void *context, GLint a, const GLint *b)
{

    r_glTestPointer1(context, a, b);
}

void d_glTestPointer2(void *context, GLint a, const GLint *b, GLint *c)
{

    r_glTestPointer2(context, a, b, c);
}

GLint d_glTestPointer4(void *context, GLint a, const GLint *b, GLint *c)
{

    return r_glTestPointer4(context, a, b, c);
}

void d_glTestString(void *context, GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf)
{

    r_glTestString(context, a, count, strings, buf_len, char_buf);
}

/******* file '1-1-1' *******/

GLboolean d_glIsBuffer(void *context, GLuint buffer)
{

    return r_glIsBuffer(context, buffer);
}

GLboolean d_glIsEnabled(void *context, GLenum cap)
{

    return r_glIsEnabled(context, cap);
}

GLboolean d_glIsFramebuffer(void *context, GLuint framebuffer)
{

    return r_glIsFramebuffer(context, framebuffer);
}

GLboolean d_glIsProgram(void *context, GLuint program)
{

    return r_glIsProgram(context, program);
}

GLboolean d_glIsRenderbuffer(void *context, GLuint renderbuffer)
{

    return r_glIsRenderbuffer(context, renderbuffer);
}

GLboolean d_glIsShader(void *context, GLuint shader)
{

    return r_glIsShader(context, shader);
}

GLboolean d_glIsTexture(void *context, GLuint texture)
{

    return r_glIsTexture(context, texture);
}

GLboolean d_glIsQuery(void *context, GLuint id)
{

    return r_glIsQuery(context, id);
}

GLboolean d_glIsVertexArray(void *context, GLuint array)
{

    return r_glIsVertexArray(context, array);
}

GLboolean d_glIsSampler(void *context, GLuint sampler)
{

    return r_glIsSampler(context, sampler);
}

GLboolean d_glIsTransformFeedback(void *context, GLuint id)
{

    return r_glIsTransformFeedback(context, id);
}

GLboolean d_glIsSync(void *context, GLsync sync)
{

    return r_glIsSync(context, sync);
}

/******* file '1-1-2' *******/

GLenum d_glGetError(void *context)
{

    return r_glGetError(context);
}

void d_glGetString(void *context, GLenum name, GLubyte *buffer)
{

    r_glGetString_special(context, name, buffer);
}

void d_glGetStringi(void *context, GLenum name, GLuint index, GLubyte *buffer)
{

    r_glGetStringi_special(context, name, index, buffer);
}

GLenum d_glCheckFramebufferStatus(void *context, GLenum target)
{
    {
        if (target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER && target != GL_FRAMEBUFFER)
        {
            set_gl_error(context, GL_INVALID_ENUM);
            return 0;
        }
    }
    return r_glCheckFramebufferStatus(context, target);
}

GLbitfield d_glQueryMatrixxOES(void *context, GLfixed *mantissa, GLint *exponent)
{

    return r_glQueryMatrixxOES(context, mantissa, exponent);
}

void d_glGetFramebufferAttachmentParameteriv(void *context, GLenum target, GLenum attachment, GLenum pname, GLint *params)
{

    r_glGetFramebufferAttachmentParameteriv(context, target, attachment, pname, params);
}

void d_glGetProgramInfoLog(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetProgramInfoLog(context, program, bufSize, length, infoLog);
}

void d_glGetRenderbufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{

    r_glGetRenderbufferParameteriv(context, target, pname, params);
}

void d_glGetShaderInfoLog(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetShaderInfoLog(context, shader, bufSize, length, infoLog);
}

void d_glGetShaderPrecisionFormat(void *context, GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
{

    r_glGetShaderPrecisionFormat(context, shadertype, precisiontype, range, precision);
}

void d_glGetShaderSource(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetShaderSource(context, shader, bufSize, length, source);
}

void d_glGetTexParameterfv(void *context, GLenum target, GLenum pname, GLfloat *params)
{

    r_glGetTexParameterfv(context, target, pname, params);
}

void d_glGetTexParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{

    r_glGetTexParameteriv(context, target, pname, params);
}

void d_glGetQueryiv(void *context, GLenum target, GLenum pname, GLint *params)
{

    r_glGetQueryiv(context, target, pname, params);
}

void d_glGetQueryObjectuiv(void *context, GLuint id, GLenum pname, GLuint *params)
{

    r_glGetQueryObjectuiv(context, id, pname, params);
}

void d_glGetTransformFeedbackVarying(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetTransformFeedbackVarying(context, program, index, bufSize, length, size, type, name);
}

void d_glGetActiveUniformsiv(void *context, GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
{

    r_glGetActiveUniformsiv(context, program, uniformCount, uniformIndices, pname, params);
}

void d_glGetActiveUniformBlockiv(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{

    r_glGetActiveUniformBlockiv(context, program, uniformBlockIndex, pname, params);
}

void d_glGetActiveUniformBlockName(void *context, GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetActiveUniformBlockName(context, program, uniformBlockIndex, bufSize, length, uniformBlockName);
}

void d_glGetSamplerParameteriv(void *context, GLuint sampler, GLenum pname, GLint *params)
{

    r_glGetSamplerParameteriv(context, sampler, pname, params);
}

void d_glGetSamplerParameterfv(void *context, GLuint sampler, GLenum pname, GLfloat *params)
{

    r_glGetSamplerParameterfv(context, sampler, pname, params);
}

void d_glGetProgramBinary(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetProgramBinary(context, program, bufSize, length, binaryFormat, binary);
}

void d_glGetInternalformativ(void *context, GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params)
{

    r_glGetInternalformativ(context, target, internalformat, pname, count, params);
}

void d_glGetClipPlanexOES(void *context, GLenum plane, GLfixed *equation)
{

    r_glGetClipPlanexOES(context, plane, equation);
}

void d_glGetFixedvOES(void *context, GLenum pname, GLfixed *params)
{

    r_glGetFixedvOES(context, pname, params);
}

void d_glGetTexEnvxvOES(void *context, GLenum target, GLenum pname, GLfixed *params)
{

    r_glGetTexEnvxvOES(context, target, pname, params);
}

void d_glGetTexParameterxvOES(void *context, GLenum target, GLenum pname, GLfixed *params)
{

    r_glGetTexParameterxvOES(context, target, pname, params);
}

void d_glGetLightxvOES(void *context, GLenum light, GLenum pname, GLfixed *params)
{

    r_glGetLightxvOES(context, light, pname, params);
}

void d_glGetMaterialxvOES(void *context, GLenum face, GLenum pname, GLfixed *params)
{

    r_glGetMaterialxvOES(context, face, pname, params);
}

void d_glGetTexGenxvOES(void *context, GLenum coord, GLenum pname, GLfixed *params)
{

    r_glGetTexGenxvOES(context, coord, pname, params);
}

void d_glGetFramebufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{

    r_glGetFramebufferParameteriv(context, target, pname, params);
}

void d_glGetProgramInterfaceiv(void *context, GLuint program, GLenum programInterface, GLenum pname, GLint *params)
{

    r_glGetProgramInterfaceiv(context, program, programInterface, pname, params);
}

void d_glGetProgramResourceName(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetProgramResourceName(context, program, programInterface, index, bufSize, length, name);
}

void d_glGetProgramResourceiv(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetProgramResourceiv(context, program, programInterface, index, propCount, props, bufSize, length, params);
}

void d_glGetProgramPipelineiv(void *context, GLuint pipeline, GLenum pname, GLint *params)
{

    r_glGetProgramPipelineiv(context, pipeline, pname, params);
}

void d_glGetProgramPipelineInfoLog(void *context, GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetProgramPipelineInfoLog(context, pipeline, bufSize, length, infoLog);
}

void d_glGetMultisamplefv(void *context, GLenum pname, GLuint index, GLfloat *val)
{

    r_glGetMultisamplefv(context, pname, index, val);
}

void d_glGetTexLevelParameteriv(void *context, GLenum target, GLint level, GLenum pname, GLint *params)
{

    r_glGetTexLevelParameteriv(context, target, level, pname, params);
}

void d_glGetTexLevelParameterfv(void *context, GLenum target, GLint level, GLenum pname, GLfloat *params)
{

    r_glGetTexLevelParameterfv(context, target, level, pname, params);
}

void d_glGetSynciv(void *context, GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetSynciv(context, sync, pname, bufSize, length, values);
}

GLint d_glGetAttribLocation(void *context, GLuint program, const GLchar *name)
{

    return r_glGetAttribLocation(context, program, name);
}

GLint d_glGetUniformLocation(void *context, GLuint program, const GLchar *name)
{

    return r_glGetUniformLocation(context, program, name);
}

GLint d_glGetFragDataLocation(void *context, GLuint program, const GLchar *name)
{

    return r_glGetFragDataLocation(context, program, name);
}

GLuint d_glGetUniformBlockIndex(void *context, GLuint program, const GLchar *uniformBlockName)
{

    return r_glGetUniformBlockIndex(context, program, uniformBlockName);
}

GLuint d_glGetProgramResourceIndex(void *context, GLuint program, GLenum programInterface, const GLchar *name)
{

    return r_glGetProgramResourceIndex(context, program, programInterface, name);
}

GLint d_glGetProgramResourceLocation(void *context, GLuint program, GLenum programInterface, const GLchar *name)
{

    return r_glGetProgramResourceLocation(context, program, programInterface, name);
}

void d_glGetActiveAttrib(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetActiveAttrib(context, program, index, bufSize, length, size, type, name);
}

void d_glGetActiveUniform(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGetActiveUniform(context, program, index, bufSize, length, size, type, name);
}

void d_glGetAttachedShaders(void *context, GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders)
{

    r_glGetAttachedShaders(context, program, maxCount, count, shaders);
}

void d_glGetProgramiv(void *context, GLuint program, GLenum pname, GLint *params)
{

    r_glGetProgramiv(context, program, pname, params);
}

void d_glGetShaderiv(void *context, GLuint shader, GLenum pname, GLint *params)
{

    r_glGetShaderiv(context, shader, pname, params);
}

void d_glGetUniformfv(void *context, GLuint program, GLint location, GLfloat *params)
{

    r_glGetUniformfv(context, program, location, params);
}

void d_glGetUniformiv(void *context, GLuint program, GLint location, GLint *params)
{

    r_glGetUniformiv(context, program, location, params);
}

void d_glGetUniformuiv(void *context, GLuint program, GLint location, GLuint *params)
{

    r_glGetUniformuiv(context, program, location, params);
}

void d_glGetUniformIndices(void *context, GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices)
{

    r_glGetUniformIndices(context, program, uniformCount, uniformNames, uniformIndices);
}

void d_glGetVertexAttribfv(void *context, GLuint index, GLenum pname, GLfloat *params)
{

    r_glGetVertexAttribfv(context, index, pname, params);
}

void d_glGetVertexAttribiv(void *context, GLuint index, GLenum pname, GLint *params)
{

    r_glGetVertexAttribiv(context, index, pname, params);
}

void d_glGetVertexAttribIiv(void *context, GLuint index, GLenum pname, GLint *params)
{

    r_glGetVertexAttribIiv(context, index, pname, params);
}

void d_glGetVertexAttribIuiv(void *context, GLuint index, GLenum pname, GLuint *params)
{

    r_glGetVertexAttribIuiv(context, index, pname, params);
}

void d_glGetBufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{

    r_glGetBufferParameteriv(context, target, pname, params);
}

void d_glGetBufferParameteri64v(void *context, GLenum target, GLenum pname, GLint64 *params)
{

    r_glGetBufferParameteri64v(context, target, pname, params);
}

void d_glGetBooleanv(void *context, GLenum pname, GLboolean *data)
{

    r_glGetBooleanv(context, pname, data);
}

void d_glGetBooleani_v(void *context, GLenum target, GLuint index, GLboolean *data)
{

    r_glGetBooleani_v(context, target, index, data);
}

void d_glGetFloatv(void *context, GLenum pname, GLfloat *data)
{

    r_glGetFloatv(context, pname, data);
}

void d_glGetIntegerv(void *context, GLenum pname, GLint *data)
{

    r_glGetIntegerv(context, pname, data);
}

void d_glGetIntegeri_v(void *context, GLenum target, GLuint index, GLint *data)
{

    r_glGetIntegeri_v(context, target, index, data);
}

void d_glGetInteger64v(void *context, GLenum pname, GLint64 *data)
{

    r_glGetInteger64v(context, pname, data);
}

void d_glGetInteger64i_v(void *context, GLenum target, GLuint index, GLint64 *data)
{

    r_glGetInteger64i_v(context, target, index, data);
}

/******* file '1-2' *******/

void d_glMapBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{

    r_glMapBufferRange_read(context, target, offset, length, access, mem_buf);
}

void d_glReadPixels(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels)
{

    r_glReadPixels_without_bound(context, x, y, width, height, format, type, buf_len, pixels);
}

GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c)
{

    return r_glTestPointer3(context, a, b, c);
}

/******* file '2-1' *******/

void d_glFlush(void *context)
{

    r_glFlush(context);
}

void d_glFinish(void *context)
{

    r_glFinish(context);
}

void d_glBeginQuery(void *context, GLenum target, GLuint id)
{

    r_glBeginQuery(context, target, id);
}

void d_glEndQuery(void *context, GLenum target)
{

    r_glEndQuery(context, target);
}

void d_glViewport(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{

    r_glViewport(context, x, y, width, height);
}

void d_glTexStorage2D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{

    r_glTexStorage2D(context, target, levels, internalformat, width, height);
}

void d_glTexStorage3D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{

    r_glTexStorage3D(context, target, levels, internalformat, width, height, depth);
}

void d_glTexImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{

    r_glTexImage2D_with_bound(context, target, level, internalformat, width, height, border, format, type, pixels);
}

void d_glTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{

    r_glTexSubImage2D_with_bound(context, target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void d_glTexImage3D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels)
{

    r_glTexImage3D_with_bound(context, target, level, internalformat, width, height, depth, border, format, type, pixels);
}

void d_glTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{

    r_glTexSubImage3D_with_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

void d_glReadPixels(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{

    r_glReadPixels_with_bound(context, x, y, width, height, format, type, pixels);
}

void d_glCompressedTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glCompressedTexImage2D_with_bound(context, target, level, internalformat, width, height, border, imageSize, data);
}

void d_glCompressedTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glCompressedTexSubImage2D_with_bound(context, target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void d_glCompressedTexImage3D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glCompressedTexImage3D_with_bound(context, target, level, internalformat, width, height, depth, border, imageSize, data);
}

void d_glCompressedTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{

    r_glCompressedTexSubImage3D_with_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

void d_glCopyTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{

    r_glCopyTexImage2D(context, target, level, internalformat, x, y, width, height, border);
}

void d_glCopyTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{

    r_glCopyTexSubImage2D(context, target, level, xoffset, yoffset, x, y, width, height);
}

void d_glCopyTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{

    r_glCopyTexSubImage3D(context, target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

void d_glVertexAttribPointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{

    r_glVertexAttribPointer_with_bound(context, index, size, type, normalized, stride, pointer);
}

void d_glVertexAttribPointer(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset)
{

    r_glVertexAttribPointer_offset(context, index, size, type, normalized, stride, index_father, offset);
}

void d_glMapBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{

    r_glMapBufferRange_write(context, target, offset, length, access);
}

void d_glUnmapBuffer(void *context, GLenum target)
{

    r_glUnmapBuffer_special(context, target);
}

void d_glWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout)
{

    r_glWaitSync(context, sync, flags, timeout);
}

void d_glShaderBinary(void *context, GLsizei count, const GLuint *shaders, GLenum binaryFormat, const void *binary, GLsizei length)
{

    r_glShaderBinary(context, count, shaders, binaryFormat, binary, length);
}

void d_glProgramBinary(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
{

    r_glProgramBinary(context, program, binaryFormat, binary, length);
}

void d_glDrawBuffers(void *context, GLsizei n, const GLenum *bufs)
{

    r_glDrawBuffers(context, n, bufs);
}

void d_glDrawArrays(void *context, GLenum mode, GLint first, GLsizei count)
{

    r_glDrawArrays_origin(context, mode, first, count);
}

void d_glDrawArraysInstanced(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{

    r_glDrawArraysInstanced_origin(context, mode, first, count, instancecount);
}

void d_glDrawElementsInstanced(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{

    r_glDrawElementsInstanced_with_bound(context, mode, count, type, indices, instancecount);
}

void d_glDrawElements(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{

    r_glDrawElements_with_bound(context, mode, count, type, indices);
}

void d_glDrawRangeElements(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{

    r_glDrawRangeElements_with_bound(context, mode, start, end, count, type, indices);
}

void d_glTestIntAsyn(void *context, GLint a, GLuint b, GLfloat c, GLdouble d)
{

    r_glTestIntAsyn(context, a, b, c, d);
}

void d_glPrintfAsyn(void *context, GLint a, GLuint size, GLdouble c, const GLchar *out_string)
{

    r_glPrintfAsyn(context, a, size, c, out_string);
}

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES imageSize)
{

    r_glEGLImageTargetTexture2DOES(context, target, imageSize);
}

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{

    r_glEGLImageTargetRenderbufferStorageOES(context, target, image);
}

/******* file '2-1-1' *******/

void d_glGenBuffers(void *context, GLsizei n, const GLuint *buffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenBuffers(context, n, buffers);
}

void d_glGenRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenRenderbuffers(context, n, renderbuffers);
}

void d_glGenTextures(void *context, GLsizei n, const GLuint *textures)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenTextures(context, n, textures);
}

void d_glGenSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    {
        if (count < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenSamplers(context, count, samplers);
}

void d_glCreateProgram(void *context, GLuint program)
{

    r_glCreateProgram(context, program);
}

void d_glCreateShader(void *context, GLenum type, GLuint shader)
{
    if (type != GL_COMPUTE_SHADER && type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER)
    {
        set_gl_error(context, GL_INVALID_ENUM);
        return 0;
    }
    r_glCreateShader(context, type, shader);
}

void d_glFenceSync(void *context, GLenum condition, GLbitfield flags, GLsync sync)
{

    r_glFenceSync(context, condition, flags, sync);
}

void d_glCreateShaderProgramv(void *context, GLenum type, GLsizei count, const GLchar *const *strings, GLuint program)
{
    {
        if (type != GL_COMPUTE_SHADER && type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER)
        {
            set_gl_error(context, GL_INVALID_ENUM);
            return 0;
        }
        if (count < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return 0;
        }
    }
    r_glCreateShaderProgramv(context, type, count, strings, program);
}

void d_glGenFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenFramebuffers(context, n, framebuffers);
}

void d_glGenProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenProgramPipelines(context, n, pipelines);
}

void d_glGenTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenTransformFeedbacks(context, n, ids);
}

void d_glGenVertexArrays(void *context, GLsizei n, const GLuint *arrays)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenVertexArrays(context, n, arrays);
}

void d_glGenQueries(void *context, GLsizei n, const GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glGenQueries(context, n, ids);
}

void d_glDeleteBuffers(void *context, GLsizei n, const GLuint *buffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteBuffers_origin(context, n, buffers);
}

void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteRenderbuffers(context, n, renderbuffers);
}

void d_glDeleteTextures(void *context, GLsizei n, const GLuint *textures)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteTextures(context, n, textures);
}

void d_glDeleteSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    {
        if (count < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteSamplers(context, count, samplers);
}

void d_glDeleteProgram(void *context, GLuint program)
{

    r_glDeleteProgram_origin(context, program);
}

void d_glDeleteShader(void *context, GLuint shader)
{

    r_glDeleteShader(context, shader);
}

void d_glDeleteSync(void *context, GLsync sync)
{

    r_glDeleteSync(context, sync);
}

void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteFramebuffers(context, n, framebuffers);
}

void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteProgramPipelines(context, n, pipelines);
}

void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteTransformFeedbacks(context, n, ids);
}

void d_glDeleteVertexArrays(void *context, GLsizei n, const GLuint *arrays)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteVertexArrays_origin(context, n, arrays);
}

void d_glDeleteQueries(void *context, GLsizei n, const GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glDeleteQueries(context, n, ids);
}

/******* file '2-1-2' *******/

void d_glLinkProgram(void *context, GLuint program)
{

    r_glLinkProgram_origin(context, program);
}

void d_glPixelStorei(void *context, GLenum pname, GLint param)
{

    r_glPixelStorei_origin(context, pname, param);
}

void d_glDisableVertexAttribArray(void *context, GLuint index)
{

    r_glDisableVertexAttribArray_origin(context, index);
}

void d_glEnableVertexAttribArray(void *context, GLuint index)
{

    r_glEnableVertexAttribArray_origin(context, index);
}

void d_glReadBuffer(void *context, GLenum src)
{

    r_glReadBuffer_special(context, src);
}

void d_glVertexAttribDivisor(void *context, GLuint index, GLuint divisor)
{

    r_glVertexAttribDivisor_origin(context, index, divisor);
}

void d_glShaderSource(void *context, GLuint shader, GLsizei count, const GLint *length, const GLchar *const *string)
{

    r_glShaderSource_origin(context, shader, count, length, string);
}

void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{

    r_glVertexAttribIPointer_with_bound(context, index, size, type, stride, pointer);
}

void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset)
{

    r_glVertexAttribIPointer_offset(context, index, size, type, stride, index_father, offset);
}

void d_glBindVertexArray(void *context, GLuint array)
{

    r_glBindVertexArray_special(context, array);
}

void d_glBindBuffer(void *context, GLenum target, GLuint buffer)
{

    r_glBindBuffer_origin(context, target, buffer);
}

void d_glBeginTransformFeedback(void *context, GLenum primitiveMode)
{

    r_glBeginTransformFeedback(context, primitiveMode);
}

void d_glEndTransformFeedback(void *context)
{

    r_glEndTransformFeedback(context);
}

void d_glPauseTransformFeedback(void *context)
{

    r_glPauseTransformFeedback(context);
}

void d_glResumeTransformFeedback(void *context)
{

    r_glResumeTransformFeedback(context);
}

void d_glBindBufferRange(void *context, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{

    r_glBindBufferRange(context, target, index, buffer, offset, size);
}

void d_glBindBufferBase(void *context, GLenum target, GLuint index, GLuint buffer)
{

    r_glBindBufferBase(context, target, index, buffer);
}

void d_glBindTexture(void *context, GLenum target, GLuint texture)
{

    r_glBindTexture(context, target, texture);
}

void d_glBindRenderbuffer(void *context, GLenum target, GLuint renderbuffer)
{

    r_glBindRenderbuffer(context, target, renderbuffer);
}

void d_glBindSampler(void *context, GLuint unit, GLuint sampler)
{

    r_glBindSampler(context, unit, sampler);
}

void d_glBindFramebuffer(void *context, GLenum target, GLuint framebuffer)
{

    r_glBindFramebuffer(context, target, framebuffer);
}

void d_glBindProgramPipeline(void *context, GLuint pipeline)
{

    r_glBindProgramPipeline(context, pipeline);
}

void d_glBindTransformFeedback(void *context, GLenum target, GLuint feedback_id)
{

    r_glBindTransformFeedback(context, target, feedback_id);
}

void d_glActiveTexture(void *context, GLenum texture)
{

    r_glActiveTexture(context, texture);
}

void d_glAttachShader(void *context, GLuint program, GLuint shader)
{

    r_glAttachShader(context, program, shader);
}

void d_glBlendColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{

    r_glBlendColor(context, red, green, blue, alpha);
}

void d_glBlendEquation(void *context, GLenum mode)
{

    r_glBlendEquation(context, mode);
}

void d_glBlendEquationSeparate(void *context, GLenum modeRGB, GLenum modeAlpha)
{

    r_glBlendEquationSeparate(context, modeRGB, modeAlpha);
}

void d_glBlendFunc(void *context, GLenum sfactor, GLenum dfactor)
{

    r_glBlendFunc(context, sfactor, dfactor);
}

void d_glBlendFuncSeparate(void *context, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{

    r_glBlendFuncSeparate(context, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void d_glClear(void *context, GLbitfield mask)
{

    r_glClear(context, mask);
}

void d_glClearColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{

    r_glClearColor(context, red, green, blue, alpha);
}

void d_glClearDepthf(void *context, GLfloat d)
{

    r_glClearDepthf(context, d);
}

void d_glClearStencil(void *context, GLint s)
{

    r_glClearStencil(context, s);
}

void d_glColorMask(void *context, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{

    r_glColorMask(context, red, green, blue, alpha);
}

void d_glCompileShader(void *context, GLuint shader)
{

    r_glCompileShader(context, shader);
}

void d_glCullFace(void *context, GLenum mode)
{

    r_glCullFace(context, mode);
}

void d_glDepthFunc(void *context, GLenum func)
{

    r_glDepthFunc(context, func);
}

void d_glDepthMask(void *context, GLboolean flag)
{

    r_glDepthMask(context, flag);
}

void d_glDepthRangef(void *context, GLfloat n, GLfloat f)
{

    r_glDepthRangef(context, n, f);
}

void d_glDetachShader(void *context, GLuint program, GLuint shader)
{

    r_glDetachShader(context, program, shader);
}

void d_glDisable(void *context, GLenum cap)
{

    r_glDisable(context, cap);
}

void d_glEnable(void *context, GLenum cap)
{

    r_glEnable(context, cap);
}

void d_glFramebufferRenderbuffer(void *context, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{

    r_glFramebufferRenderbuffer(context, target, attachment, renderbuffertarget, renderbuffer);
}

void d_glFramebufferTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{

    r_glFramebufferTexture2D(context, target, attachment, textarget, texture, level);
}

void d_glFrontFace(void *context, GLenum mode)
{

    r_glFrontFace(context, mode);
}

void d_glGenerateMipmap(void *context, GLenum target)
{

    r_glGenerateMipmap(context, target);
}

void d_glHint(void *context, GLenum target, GLenum mode)
{

    r_glHint(context, target, mode);
}

void d_glLineWidth(void *context, GLfloat width)
{

    r_glLineWidth(context, width);
}

void d_glPolygonOffset(void *context, GLfloat factor, GLfloat units)
{

    r_glPolygonOffset(context, factor, units);
}

void d_glReleaseShaderCompiler(void *context)
{

    r_glReleaseShaderCompiler(context);
}

void d_glRenderbufferStorage(void *context, GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{

    r_glRenderbufferStorage(context, target, internalformat, width, height);
}

void d_glSampleCoverage(void *context, GLfloat value, GLboolean invert)
{

    r_glSampleCoverage(context, value, invert);
}

void d_glScissor(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{

    r_glScissor(context, x, y, width, height);
}

void d_glStencilFunc(void *context, GLenum func, GLint ref, GLuint mask)
{

    r_glStencilFunc(context, func, ref, mask);
}

void d_glStencilFuncSeparate(void *context, GLenum face, GLenum func, GLint ref, GLuint mask)
{

    r_glStencilFuncSeparate(context, face, func, ref, mask);
}

void d_glStencilMask(void *context, GLuint mask)
{

    r_glStencilMask(context, mask);
}

void d_glStencilMaskSeparate(void *context, GLenum face, GLuint mask)
{

    r_glStencilMaskSeparate(context, face, mask);
}

void d_glStencilOp(void *context, GLenum fail, GLenum zfail, GLenum zpass)
{

    r_glStencilOp(context, fail, zfail, zpass);
}

void d_glStencilOpSeparate(void *context, GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{

    r_glStencilOpSeparate(context, face, sfail, dpfail, dppass);
}

void d_glTexParameterf(void *context, GLenum target, GLenum pname, GLfloat param)
{

    r_glTexParameterf(context, target, pname, param);
}

void d_glTexParameteri(void *context, GLenum target, GLenum pname, GLint param)
{

    r_glTexParameteri(context, target, pname, param);
}

void d_glUniform1f(void *context, GLint location, GLfloat v0)
{

    r_glUniform1f(context, location, v0);
}

void d_glUniform1i(void *context, GLint location, GLint v0)
{

    r_glUniform1i(context, location, v0);
}

void d_glUniform2f(void *context, GLint location, GLfloat v0, GLfloat v1)
{

    r_glUniform2f(context, location, v0, v1);
}

void d_glUniform2i(void *context, GLint location, GLint v0, GLint v1)
{

    r_glUniform2i(context, location, v0, v1);
}

void d_glUniform3f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{

    r_glUniform3f(context, location, v0, v1, v2);
}

void d_glUniform3i(void *context, GLint location, GLint v0, GLint v1, GLint v2)
{

    r_glUniform3i(context, location, v0, v1, v2);
}

void d_glUniform4f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{

    r_glUniform4f(context, location, v0, v1, v2, v3);
}

void d_glUniform4i(void *context, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{

    r_glUniform4i(context, location, v0, v1, v2, v3);
}

void d_glUseProgram(void *context, GLuint program)
{

    r_glUseProgram(context, program);
}

void d_glValidateProgram(void *context, GLuint program)
{

    r_glValidateProgram(context, program);
}

void d_glVertexAttrib1f(void *context, GLuint index, GLfloat x)
{

    r_glVertexAttrib1f(context, index, x);
}

void d_glVertexAttrib2f(void *context, GLuint index, GLfloat x, GLfloat y)
{

    r_glVertexAttrib2f(context, index, x, y);
}

void d_glVertexAttrib3f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z)
{

    r_glVertexAttrib3f(context, index, x, y, z);
}

void d_glVertexAttrib4f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{

    r_glVertexAttrib4f(context, index, x, y, z, w);
}

void d_glBlitFramebuffer(void *context, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{

    r_glBlitFramebuffer(context, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void d_glRenderbufferStorageMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{

    r_glRenderbufferStorageMultisample(context, target, samples, internalformat, width, height);
}

void d_glFramebufferTextureLayer(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{

    r_glFramebufferTextureLayer(context, target, attachment, texture, level, layer);
}

void d_glVertexAttribI4i(void *context, GLuint index, GLint x, GLint y, GLint z, GLint w)
{

    r_glVertexAttribI4i(context, index, x, y, z, w);
}

void d_glVertexAttribI4ui(void *context, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{

    r_glVertexAttribI4ui(context, index, x, y, z, w);
}

void d_glUniform1ui(void *context, GLint location, GLuint v0)
{

    r_glUniform1ui(context, location, v0);
}

void d_glUniform2ui(void *context, GLint location, GLuint v0, GLuint v1)
{

    r_glUniform2ui(context, location, v0, v1);
}

void d_glUniform3ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2)
{

    r_glUniform3ui(context, location, v0, v1, v2);
}

void d_glUniform4ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{

    r_glUniform4ui(context, location, v0, v1, v2, v3);
}

void d_glClearBufferfi(void *context, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{

    r_glClearBufferfi(context, buffer, drawbuffer, depth, stencil);
}

void d_glCopyBufferSubData(void *context, GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{

    r_glCopyBufferSubData(context, readTarget, writeTarget, readOffset, writeOffset, size);
}

void d_glUniformBlockBinding(void *context, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{

    r_glUniformBlockBinding(context, program, uniformBlockIndex, uniformBlockBinding);
}

void d_glSamplerParameteri(void *context, GLuint sampler, GLenum pname, GLint param)
{

    r_glSamplerParameteri(context, sampler, pname, param);
}

void d_glSamplerParameterf(void *context, GLuint sampler, GLenum pname, GLfloat param)
{

    r_glSamplerParameterf(context, sampler, pname, param);
}

void d_glProgramParameteri(void *context, GLuint program, GLenum pname, GLint value)
{

    r_glProgramParameteri(context, program, pname, value);
}

void d_glAlphaFuncxOES(void *context, GLenum func, GLfixed ref)
{

    r_glAlphaFuncxOES(context, func, ref);
}

void d_glClearColorxOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{

    r_glClearColorxOES(context, red, green, blue, alpha);
}

void d_glClearDepthxOES(void *context, GLfixed depth)
{

    r_glClearDepthxOES(context, depth);
}

void d_glColor4xOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{

    r_glColor4xOES(context, red, green, blue, alpha);
}

void d_glDepthRangexOES(void *context, GLfixed n, GLfixed f)
{

    r_glDepthRangexOES(context, n, f);
}

void d_glFogxOES(void *context, GLenum pname, GLfixed param)
{

    r_glFogxOES(context, pname, param);
}

void d_glFrustumxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{

    r_glFrustumxOES(context, l, r, b, t, n, f);
}

void d_glLightModelxOES(void *context, GLenum pname, GLfixed param)
{

    r_glLightModelxOES(context, pname, param);
}

void d_glLightxOES(void *context, GLenum light, GLenum pname, GLfixed param)
{

    r_glLightxOES(context, light, pname, param);
}

void d_glLineWidthxOES(void *context, GLfixed width)
{

    r_glLineWidthxOES(context, width);
}

void d_glMaterialxOES(void *context, GLenum face, GLenum pname, GLfixed param)
{

    r_glMaterialxOES(context, face, pname, param);
}

void d_glMultiTexCoord4xOES(void *context, GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{

    r_glMultiTexCoord4xOES(context, texture, s, t, r, q);
}

void d_glNormal3xOES(void *context, GLfixed nx, GLfixed ny, GLfixed nz)
{

    r_glNormal3xOES(context, nx, ny, nz);
}

void d_glOrthoxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{

    r_glOrthoxOES(context, l, r, b, t, n, f);
}

void d_glPointSizexOES(void *context, GLfixed size)
{

    r_glPointSizexOES(context, size);
}

void d_glPolygonOffsetxOES(void *context, GLfixed factor, GLfixed units)
{

    r_glPolygonOffsetxOES(context, factor, units);
}

void d_glRotatexOES(void *context, GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{

    r_glRotatexOES(context, angle, x, y, z);
}

void d_glScalexOES(void *context, GLfixed x, GLfixed y, GLfixed z)
{

    r_glScalexOES(context, x, y, z);
}

void d_glTexEnvxOES(void *context, GLenum target, GLenum pname, GLfixed param)
{

    r_glTexEnvxOES(context, target, pname, param);
}

void d_glTranslatexOES(void *context, GLfixed x, GLfixed y, GLfixed z)
{

    r_glTranslatexOES(context, x, y, z);
}

void d_glPointParameterxOES(void *context, GLenum pname, GLfixed param)
{

    r_glPointParameterxOES(context, pname, param);
}

void d_glSampleCoveragexOES(void *context, GLclampx value, GLboolean invert)
{

    r_glSampleCoveragexOES(context, value, invert);
}

void d_glTexGenxOES(void *context, GLenum coord, GLenum pname, GLfixed param)
{

    r_glTexGenxOES(context, coord, pname, param);
}

void d_glClearDepthfOES(void *context, GLclampf depth)
{

    r_glClearDepthfOES(context, depth);
}

void d_glDepthRangefOES(void *context, GLclampf n, GLclampf f)
{

    r_glDepthRangefOES(context, n, f);
}

void d_glFrustumfOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{

    r_glFrustumfOES(context, l, r, b, t, n, f);
}

void d_glOrthofOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{

    r_glOrthofOES(context, l, r, b, t, n, f);
}

void d_glRenderbufferStorageMultisampleEXT(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{

    r_glRenderbufferStorageMultisampleEXT(context, target, samples, internalformat, width, height);
}

void d_glUseProgramStages(void *context, GLuint pipeline, GLbitfield stages, GLuint program)
{

    r_glUseProgramStages(context, pipeline, stages, program);
}

void d_glActiveShaderProgram(void *context, GLuint pipeline, GLuint program)
{

    r_glActiveShaderProgram(context, pipeline, program);
}

void d_glProgramUniform1i(void *context, GLuint program, GLint location, GLint v0)
{

    r_glProgramUniform1i(context, program, location, v0);
}

void d_glProgramUniform2i(void *context, GLuint program, GLint location, GLint v0, GLint v1)
{

    r_glProgramUniform2i(context, program, location, v0, v1);
}

void d_glProgramUniform3i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2)
{

    r_glProgramUniform3i(context, program, location, v0, v1, v2);
}

void d_glProgramUniform4i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{

    r_glProgramUniform4i(context, program, location, v0, v1, v2, v3);
}

void d_glProgramUniform1ui(void *context, GLuint program, GLint location, GLuint v0)
{

    r_glProgramUniform1ui(context, program, location, v0);
}

void d_glProgramUniform2ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1)
{

    r_glProgramUniform2ui(context, program, location, v0, v1);
}

void d_glProgramUniform3ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2)
{

    r_glProgramUniform3ui(context, program, location, v0, v1, v2);
}

void d_glProgramUniform4ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{

    r_glProgramUniform4ui(context, program, location, v0, v1, v2, v3);
}

void d_glProgramUniform1f(void *context, GLuint program, GLint location, GLfloat v0)
{

    r_glProgramUniform1f(context, program, location, v0);
}

void d_glProgramUniform2f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1)
{

    r_glProgramUniform2f(context, program, location, v0, v1);
}

void d_glProgramUniform3f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{

    r_glProgramUniform3f(context, program, location, v0, v1, v2);
}

void d_glProgramUniform4f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{

    r_glProgramUniform4f(context, program, location, v0, v1, v2, v3);
}

void d_glTransformFeedbackVaryings(void *context, GLuint program, GLsizei count, const GLchar *const *varyings, GLenum bufferMode)
{

    r_glTransformFeedbackVaryings(context, program, count, varyings, bufferMode);
}

void d_glTexParameterfv(void *context, GLenum target, GLenum pname, const GLfloat *params)
{

    r_glTexParameterfv(context, target, pname, params);
}

void d_glTexParameteriv(void *context, GLenum target, GLenum pname, const GLint *params)
{

    r_glTexParameteriv(context, target, pname, params);
}

void d_glUniform1fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    r_glUniform1fv(context, location, count, value);
}

void d_glUniform1iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    r_glUniform1iv(context, location, count, value);
}

void d_glUniform2fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    r_glUniform2fv(context, location, count, value);
}

void d_glUniform2iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    r_glUniform2iv(context, location, count, value);
}

void d_glUniform3fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    r_glUniform3fv(context, location, count, value);
}

void d_glUniform3iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    r_glUniform3iv(context, location, count, value);
}

void d_glUniform4fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    r_glUniform4fv(context, location, count, value);
}

void d_glUniform4iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    r_glUniform4iv(context, location, count, value);
}

void d_glVertexAttrib1fv(void *context, GLuint index, const GLfloat *v)
{

    r_glVertexAttrib1fv(context, index, v);
}

void d_glVertexAttrib2fv(void *context, GLuint index, const GLfloat *v)
{

    r_glVertexAttrib2fv(context, index, v);
}

void d_glVertexAttrib3fv(void *context, GLuint index, const GLfloat *v)
{

    r_glVertexAttrib3fv(context, index, v);
}

void d_glVertexAttrib4fv(void *context, GLuint index, const GLfloat *v)
{

    r_glVertexAttrib4fv(context, index, v);
}

void d_glUniformMatrix2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix2fv(context, location, count, transpose, value);
}

void d_glUniformMatrix3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix3fv(context, location, count, transpose, value);
}

void d_glUniformMatrix4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix4fv(context, location, count, transpose, value);
}

void d_glUniformMatrix2x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix2x3fv(context, location, count, transpose, value);
}

void d_glUniformMatrix3x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix3x2fv(context, location, count, transpose, value);
}

void d_glUniformMatrix2x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix2x4fv(context, location, count, transpose, value);
}

void d_glUniformMatrix4x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix4x2fv(context, location, count, transpose, value);
}

void d_glUniformMatrix3x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix3x4fv(context, location, count, transpose, value);
}

void d_glUniformMatrix4x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glUniformMatrix4x3fv(context, location, count, transpose, value);
}

void d_glVertexAttribI4iv(void *context, GLuint index, const GLint *v)
{

    r_glVertexAttribI4iv(context, index, v);
}

void d_glVertexAttribI4uiv(void *context, GLuint index, const GLuint *v)
{

    r_glVertexAttribI4uiv(context, index, v);
}

void d_glUniform1uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    r_glUniform1uiv(context, location, count, value);
}

void d_glUniform2uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    r_glUniform2uiv(context, location, count, value);
}

void d_glUniform3uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    r_glUniform3uiv(context, location, count, value);
}

void d_glUniform4uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    r_glUniform4uiv(context, location, count, value);
}

void d_glClearBufferiv(void *context, GLenum buffer, GLint drawbuffer, const GLint *value)
{

    r_glClearBufferiv(context, buffer, drawbuffer, value);
}

void d_glClearBufferuiv(void *context, GLenum buffer, GLint drawbuffer, const GLuint *value)
{

    r_glClearBufferuiv(context, buffer, drawbuffer, value);
}

void d_glClearBufferfv(void *context, GLenum buffer, GLint drawbuffer, const GLfloat *value)
{

    r_glClearBufferfv(context, buffer, drawbuffer, value);
}

void d_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint *param)
{

    r_glSamplerParameteriv(context, sampler, pname, param);
}

void d_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat *param)
{

    r_glSamplerParameterfv(context, sampler, pname, param);
}

void d_glInvalidateFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments)
{

    r_glInvalidateFramebuffer(context, target, numAttachments, attachments);
}

void d_glInvalidateSubFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{

    r_glInvalidateSubFramebuffer(context, target, numAttachments, attachments, x, y, width, height);
}

void d_glClipPlanexOES(void *context, GLenum plane, const GLfixed *equation)
{

    r_glClipPlanexOES(context, plane, equation);
}

void d_glFogxvOES(void *context, GLenum pname, const GLfixed *param)
{

    r_glFogxvOES(context, pname, param);
}

void d_glLightModelxvOES(void *context, GLenum pname, const GLfixed *param)
{

    r_glLightModelxvOES(context, pname, param);
}

void d_glLightxvOES(void *context, GLenum light, GLenum pname, const GLfixed *params)
{

    r_glLightxvOES(context, light, pname, params);
}

void d_glLoadMatrixxOES(void *context, const GLfixed *m)
{

    r_glLoadMatrixxOES(context, m);
}

void d_glMaterialxvOES(void *context, GLenum face, GLenum pname, const GLfixed *param)
{

    r_glMaterialxvOES(context, face, pname, param);
}

void d_glMultMatrixxOES(void *context, const GLfixed *m)
{

    r_glMultMatrixxOES(context, m);
}

void d_glPointParameterxvOES(void *context, GLenum pname, const GLfixed *params)
{

    r_glPointParameterxvOES(context, pname, params);
}

void d_glTexEnvxvOES(void *context, GLenum target, GLenum pname, const GLfixed *params)
{

    r_glTexEnvxvOES(context, target, pname, params);
}

void d_glClipPlanefOES(void *context, GLenum plane, const GLfloat *equation)
{

    r_glClipPlanefOES(context, plane, equation);
}

void d_glTexGenxvOES(void *context, GLenum coord, GLenum pname, const GLfixed *params)
{

    r_glTexGenxvOES(context, coord, pname, params);
}

void d_glProgramUniform1iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    r_glProgramUniform1iv(context, program, location, count, value);
}

void d_glProgramUniform2iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    r_glProgramUniform2iv(context, program, location, count, value);
}

void d_glProgramUniform3iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    r_glProgramUniform3iv(context, program, location, count, value);
}

void d_glProgramUniform4iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    r_glProgramUniform4iv(context, program, location, count, value);
}

void d_glProgramUniform1uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    r_glProgramUniform1uiv(context, program, location, count, value);
}

void d_glProgramUniform2uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    r_glProgramUniform2uiv(context, program, location, count, value);
}

void d_glProgramUniform3uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    r_glProgramUniform3uiv(context, program, location, count, value);
}

void d_glProgramUniform4uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    r_glProgramUniform4uiv(context, program, location, count, value);
}

void d_glProgramUniform1fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    r_glProgramUniform1fv(context, program, location, count, value);
}

void d_glProgramUniform2fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    r_glProgramUniform2fv(context, program, location, count, value);
}

void d_glProgramUniform3fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    r_glProgramUniform3fv(context, program, location, count, value);
}

void d_glProgramUniform4fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    r_glProgramUniform4fv(context, program, location, count, value);
}

void d_glProgramUniformMatrix2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix2fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix3fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix4fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix2x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix2x3fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix3x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix3x2fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix2x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix2x4fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix4x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix4x2fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix3x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix3x4fv(context, program, location, count, transpose, value);
}

void d_glProgramUniformMatrix4x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    r_glProgramUniformMatrix4x3fv(context, program, location, count, transpose, value);
}

void d_glBindAttribLocation(void *context, GLuint program, GLuint index, const GLchar *name)
{

    r_glBindAttribLocation(context, program, index, name);
}

void d_glTexEnvf(void *context, GLenum target, GLenum pname, GLfloat param)
{

    r_glTexEnvf(context, target, pname, param);
}

void d_glTexEnvi(void *context, GLenum target, GLenum pname, GLint param)
{

    r_glTexEnvi(context, target, pname, param);
}

void d_glTexEnvx(void *context, GLenum target, GLenum pname, GLfixed param)
{

    r_glTexEnvx(context, target, pname, param);
}

void d_glTexParameterx(void *context, GLenum target, GLenum pname, GLint param)
{

    r_glTexParameterx(context, target, pname, param);
}

void d_glShadeModel(void *context, GLenum mode)
{

    r_glShadeModel(context, mode);
}

void d_glDrawTexiOES(void *context, GLint x, GLint y, GLint z, GLint width, GLint height)
{

    r_glDrawTexiOES(context, x, y, z, width, height);
}

/******* file '2-2' *******/

void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLsizei length, const void *pointer)
{

    r_glVertexAttribIPointer_without_bound(context, index, size, type, stride, offset, length, pointer);
}

void d_glVertexAttribPointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{

    r_glVertexAttribPointer_without_bound(context, index, size, type, normalized, stride, offset, length, pointer);
}

void d_glDrawElements(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{

    r_glDrawElements_without_bound(context, mode, count, type, indices);
}

void d_glDrawElementsInstanced(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    r_glDrawElementsInstanced_without_bound(context, mode, count, type, indices, instancecount);
}

void d_glDrawRangeElements(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{

    r_glDrawRangeElements_without_bound(context, mode, start, end, count, type, indices);
}

void d_glFlushMappedBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data)
{

    r_glFlushMappedBufferRange_special(context, target, offset, length, data);
}

void d_glBufferData(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{

    r_glBufferData_custom(context, target, size, data, usage);
}

void d_glBufferSubData(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{

    r_glBufferSubData_custom(context, target, offset, size, data);
}

void d_glCompressedTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glCompressedTexImage2D_without_bound(context, target, level, internalformat, width, height, border, imageSize, data);
}

void d_glCompressedTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glCompressedTexSubImage2D_without_bound(context, target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void d_glCompressedTexImage3D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glCompressedTexImage3D_without_bound(context, target, level, internalformat, width, height, depth, border, imageSize, data);
}

void d_glCompressedTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }
    r_glCompressedTexSubImage3D_without_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

void d_glTexImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    r_glTexImage2D_without_bound(context, target, level, internalformat, width, height, border, format, type, buf_len, pixels);
}

void d_glTexImage3D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    r_glTexImage3D_without_bound(context, target, level, internalformat, width, height, depth, border, format, type, buf_len, pixels);
}

void d_glTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    r_glTexSubImage2D_without_bound(context, target, level, xoffset, yoffset, width, height, format, type, buf_len, pixels);
}

void d_glTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    r_glTexSubImage3D_without_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, buf_len, pixels);
}

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string)
{

    r_glPrintf(context, buf_len, out_string);
}
