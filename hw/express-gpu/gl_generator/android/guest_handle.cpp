
#include "define_gl.h"
#include "utils_gl.h"


//#include "all_gl.h"

#include <cstring>




/******* file '1-1' *******/


GLenum d_glClientWaitSync(void *context, GLsyncconst* sync, GLbitfieldconst* flags, GLuint64const* timeout)
{

return r_glClientWaitSync(context, sync, flags, timeout);

}


GLint d_glTestInt1(void *context, GLintconst* a, GLuintconst* b)
{

return r_glTestInt1(context, a, b);

}


GLuint d_glTestInt2(void *context, GLintconst* a, GLuintconst* b)
{

return r_glTestInt2(context, a, b);

}


GLint64 d_glTestInt3(void *context, GLint64const* a, GLuint64const* b)
{

return r_glTestInt3(context, a, b);

}


GLuint64 d_glTestInt4(void *context, GLint64const* a, GLuint64const* b)
{

return r_glTestInt4(context, a, b);

}


GLfloat d_glTestInt5(void *context, GLintconst* a, GLuintconst* b)
{

return r_glTestInt5(context, a, b);

}


GLdouble d_glTestInt6(void *context, GLintconst* a, GLuintconst* b)
{

return r_glTestInt6(context, a, b);

}


void d_glTestPointer1(void *context, GLintconst* a, const GLint*const* b)
{

r_glTestPointer1(context, a, b);

}


void d_glTestPointer2(void *context, GLintconst* a, const GLint*const* b, GLint*const* c)
{

r_glTestPointer2(context, a, b, c);

}


GLint d_glTestPointer4(void *context, GLintconst* a, const GLint*const* b, GLint*const* c)
{

return r_glTestPointer4(context, a, b, c);

}


void d_glTestString(void *context, GLintconst* a, GLintconst* count, const GLchar*const* strings, GLintconst* buf_len, GLchar*const* char_buf)
{

r_glTestString(context, a, count, strings, buf_len, char_buf);

}




/******* file '1-1-1' *******/


GLboolean d_glIsBuffer(void *context, GLuintconst* buffer)
{

return r_glIsBuffer(context, buffer);

}


GLboolean d_glIsEnabled(void *context, GLenumconst* cap)
{

return r_glIsEnabled(context, cap);

}


GLboolean d_glIsFramebuffer(void *context, GLuintconst* framebuffer)
{

return r_glIsFramebuffer(context, framebuffer);

}


GLboolean d_glIsProgram(void *context, GLuintconst* program)
{

return r_glIsProgram(context, program);

}


GLboolean d_glIsRenderbuffer(void *context, GLuintconst* renderbuffer)
{

return r_glIsRenderbuffer(context, renderbuffer);

}


GLboolean d_glIsShader(void *context, GLuintconst* shader)
{

return r_glIsShader(context, shader);

}


GLboolean d_glIsTexture(void *context, GLuintconst* texture)
{

return r_glIsTexture(context, texture);

}


GLboolean d_glIsQuery(void *context, GLuintconst* id)
{

return r_glIsQuery(context, id);

}


GLboolean d_glIsVertexArray(void *context, GLuintconst* array)
{

return r_glIsVertexArray(context, array);

}


GLboolean d_glIsSampler(void *context, GLuintconst* sampler)
{

return r_glIsSampler(context, sampler);

}


GLboolean d_glIsTransformFeedback(void *context, GLuintconst* id)
{

return r_glIsTransformFeedback(context, id);

}




/******* file '1-1-2' *******/


GLenum d_glGetError(void *context)
{

return r_glGetError(context);

}


void d_glGetString(void *context, GLenumconst* name, GLubyte*const* buffer)
{

r_glGetString(context, name, buffer);

}


void d_glGetStringi(void *context, GLenumconst* name, GLuintconst* index, GLubyte*const* buffer)
{

r_glGetStringi(context, name, index, buffer);

}


GLenum d_glCheckFramebufferStatus(void *context, GLenumconst* target)
{
{if(target!=GL_DRAW_FRAMEBUFFER&&target!= GL_READ_FRAMEBUFFER&&target!=GL_FRAMEBUFFER){set_gl_error(context,GL_INVALID_ENUM);return 0;}}
return r_glCheckFramebufferStatus(context, target);

}


GLbitfield d_glQueryMatrixxOES(void *context, GLfixed*const* mantissa, GLint*const* exponent)
{

return r_glQueryMatrixxOES(context, mantissa, exponent);

}


void d_glGetFramebufferAttachmentParameteriv(void *context, GLenumconst* target, GLenumconst* attachment, GLenumconst* pname, GLint*const* params)
{

r_glGetFramebufferAttachmentParameteriv(context, target, attachment, pname, params);

}


void d_glGetProgramInfoLog(void *context, GLuintconst* program, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* infoLog)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetProgramInfoLog(context, program, bufSize, length, infoLog);

}


void d_glGetRenderbufferParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params)
{

r_glGetRenderbufferParameteriv(context, target, pname, params);

}


void d_glGetShaderInfoLog(void *context, GLuintconst* shader, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* infoLog)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetShaderInfoLog(context, shader, bufSize, length, infoLog);

}


void d_glGetShaderPrecisionFormat(void *context, GLenumconst* shadertype, GLenumconst* precisiontype, GLint*const* range, GLint*const* precision)
{

r_glGetShaderPrecisionFormat(context, shadertype, precisiontype, range, precision);

}


void d_glGetShaderSource(void *context, GLuintconst* shader, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* source)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetShaderSource(context, shader, bufSize, length, source);

}


void d_glGetTexParameterfv(void *context, GLenumconst* target, GLenumconst* pname, GLfloat*const* params)
{

r_glGetTexParameterfv(context, target, pname, params);

}


void d_glGetTexParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params)
{

r_glGetTexParameteriv(context, target, pname, params);

}


void d_glGetQueryiv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params)
{

r_glGetQueryiv(context, target, pname, params);

}


void d_glGetQueryObjectuiv(void *context, GLuintconst* id, GLenumconst* pname, GLuint*const* params)
{

r_glGetQueryObjectuiv(context, id, pname, params);

}


void d_glGetTransformFeedbackVarying(void *context, GLuintconst* program, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLsizei*const* size, GLenum*const* type, GLchar*const* name)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetTransformFeedbackVarying(context, program, index, bufSize, length, size, type, name);

}


void d_glGetActiveUniformsiv(void *context, GLuintconst* program, GLsizeiconst* uniformCount, const GLuint*const* uniformIndices, GLenumconst* pname, GLint*const* params)
{

r_glGetActiveUniformsiv(context, program, uniformCount, uniformIndices, pname, params);

}


void d_glGetActiveUniformBlockiv(void *context, GLuintconst* program, GLuintconst* uniformBlockIndex, GLenumconst* pname, GLint*const* params)
{

r_glGetActiveUniformBlockiv(context, program, uniformBlockIndex, pname, params);

}


void d_glGetActiveUniformBlockName(void *context, GLuintconst* program, GLuintconst* uniformBlockIndex, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* uniformBlockName)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetActiveUniformBlockName(context, program, uniformBlockIndex, bufSize, length, uniformBlockName);

}


void d_glGetSamplerParameteriv(void *context, GLuintconst* sampler, GLenumconst* pname, GLint*const* params)
{

r_glGetSamplerParameteriv(context, sampler, pname, params);

}


void d_glGetSamplerParameterfv(void *context, GLuintconst* sampler, GLenumconst* pname, GLfloat*const* params)
{

r_glGetSamplerParameterfv(context, sampler, pname, params);

}


void d_glGetProgramBinary(void *context, GLuintconst* program, GLsizeiconst* bufSize, GLsizei*const* length, GLenum*const* binaryFormat, void*const* binary)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetProgramBinary(context, program, bufSize, length, binaryFormat, binary);

}


void d_glGetInternalformativ(void *context, GLenumconst* target, GLenumconst* internalformat, GLenumconst* pname, GLsizeiconst* count, GLint*const* params)
{

r_glGetInternalformativ(context, target, internalformat, pname, count, params);

}


void d_glGetClipPlanexOES(void *context, GLenumconst* plane, GLfixed*const* equation)
{

r_glGetClipPlanexOES(context, plane, equation);

}


void d_glGetFixedvOES(void *context, GLenumconst* pname, GLfixed*const* params)
{

r_glGetFixedvOES(context, pname, params);

}


void d_glGetTexEnvxvOES(void *context, GLenumconst* target, GLenumconst* pname, GLfixed*const* params)
{

r_glGetTexEnvxvOES(context, target, pname, params);

}


void d_glGetTexParameterxvOES(void *context, GLenumconst* target, GLenumconst* pname, GLfixed*const* params)
{

r_glGetTexParameterxvOES(context, target, pname, params);

}


void d_glGetLightxvOES(void *context, GLenumconst* light, GLenumconst* pname, GLfixed*const* params)
{

r_glGetLightxvOES(context, light, pname, params);

}


void d_glGetMaterialxvOES(void *context, GLenumconst* face, GLenumconst* pname, GLfixed*const* params)
{

r_glGetMaterialxvOES(context, face, pname, params);

}


void d_glGetTexGenxvOES(void *context, GLenumconst* coord, GLenumconst* pname, GLfixed*const* params)
{

r_glGetTexGenxvOES(context, coord, pname, params);

}


void d_glGetFramebufferParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params)
{

r_glGetFramebufferParameteriv(context, target, pname, params);

}


void d_glGetProgramInterfaceiv(void *context, GLuintconst* program, GLenumconst* programInterface, GLenumconst* pname, GLint*const* params)
{

r_glGetProgramInterfaceiv(context, program, programInterface, pname, params);

}


void d_glGetProgramResourceName(void *context, GLuintconst* program, GLenumconst* programInterface, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* name)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetProgramResourceName(context, program, programInterface, index, bufSize, length, name);

}


void d_glGetProgramResourceiv(void *context, GLuintconst* program, GLenumconst* programInterface, GLuintconst* index, GLsizeiconst* propCount, const GLenum*const* props, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* params)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetProgramResourceiv(context, program, programInterface, index, propCount, props, bufSize, length, params);

}


void d_glGetProgramPipelineiv(void *context, GLuintconst* pipeline, GLenumconst* pname, GLint*const* params)
{

r_glGetProgramPipelineiv(context, pipeline, pname, params);

}


void d_glGetProgramPipelineInfoLog(void *context, GLuintconst* pipeline, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* infoLog)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetProgramPipelineInfoLog(context, pipeline, bufSize, length, infoLog);

}


void d_glGetMultisamplefv(void *context, GLenumconst* pname, GLuintconst* index, GLfloat*const* val)
{

r_glGetMultisamplefv(context, pname, index, val);

}


void d_glGetTexLevelParameteriv(void *context, GLenumconst* target, GLintconst* level, GLenumconst* pname, GLint*const* params)
{

r_glGetTexLevelParameteriv(context, target, level, pname, params);

}


void d_glGetTexLevelParameterfv(void *context, GLenumconst* target, GLintconst* level, GLenumconst* pname, GLfloat*const* params)
{

r_glGetTexLevelParameterfv(context, target, level, pname, params);

}


void d_glGetSynciv(void *context, GLsyncconst* sync, GLenumconst* pname, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* values)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetSynciv(context, sync, pname, bufSize, length, values);

}


GLint d_glGetAttribLocation(void *context, GLuintconst* program, const GLchar*const* name)
{

return r_glGetAttribLocation(context, program, name);

}


GLint d_glGetUniformLocation(void *context, GLuintconst* program, const GLchar*const* name)
{

return r_glGetUniformLocation(context, program, name);

}


GLint d_glGetFragDataLocation(void *context, GLuintconst* program, const GLchar*const* name)
{

return r_glGetFragDataLocation(context, program, name);

}


GLuint d_glGetUniformBlockIndex(void *context, GLuintconst* program, const GLchar*const* uniformBlockName)
{

return r_glGetUniformBlockIndex(context, program, uniformBlockName);

}


GLuint d_glGetProgramResourceIndex(void *context, GLuintconst* program, GLenumconst* programInterface, const GLchar*const* name)
{

return r_glGetProgramResourceIndex(context, program, programInterface, name);

}


GLint d_glGetProgramResourceLocation(void *context, GLuintconst* program, GLenumconst* programInterface, const GLchar*const* name)
{

return r_glGetProgramResourceLocation(context, program, programInterface, name);

}


void d_glGetActiveAttrib(void *context, GLuintconst* program, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* size, GLenum*const* type, GLchar*const* name)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetActiveAttrib(context, program, index, bufSize, length, size, type, name);

}


void d_glGetActiveUniform(void *context, GLuintconst* program, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* size, GLenum*const* type, GLchar*const* name)
{
{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGetActiveUniform(context, program, index, bufSize, length, size, type, name);

}


void d_glGetAttachedShaders(void *context, GLuintconst* program, GLsizeiconst* maxCount, GLsizei*const* count, GLuint*const* shaders)
{

r_glGetAttachedShaders(context, program, maxCount, count, shaders);

}


void d_glGetProgramiv(void *context, GLuintconst* program, GLenumconst* pname, GLint*const* params)
{

r_glGetProgramiv(context, program, pname, params);

}


void d_glGetShaderiv(void *context, GLuintconst* shader, GLenumconst* pname, GLint*const* params)
{

r_glGetShaderiv(context, shader, pname, params);

}


void d_glGetUniformfv(void *context, GLuintconst* program, GLintconst* location, GLfloat*const* params)
{

r_glGetUniformfv(context, program, location, params);

}


void d_glGetUniformiv(void *context, GLuintconst* program, GLintconst* location, GLint*const* params)
{

r_glGetUniformiv(context, program, location, params);

}


void d_glGetUniformuiv(void *context, GLuintconst* program, GLintconst* location, GLuint*const* params)
{

r_glGetUniformuiv(context, program, location, params);

}


void d_glGetUniformIndices(void *context, GLuintconst* program, GLsizeiconst* uniformCount, const GLchar*const* uniformNames, GLuint*const* uniformIndices)
{

r_glGetUniformIndices(context, program, uniformCount, uniformNames, uniformIndices);

}


void d_glGetVertexAttribfv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLfloat*const* params)
{

r_glGetVertexAttribfv_origin(context, index, pname, params);

}


void d_glGetVertexAttribiv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLint*const* params)
{

r_glGetVertexAttribiv_origin(context, index, pname, params);

}


void d_glGetVertexAttribIiv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLint*const* params)
{

r_glGetVertexAttribIiv_origin(context, index, pname, params);

}


void d_glGetVertexAttribIuiv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLuint*const* params)
{

r_glGetVertexAttribIuiv_origin(context, index, pname, params);

}


void d_glGetBufferParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params)
{

r_glGetBufferParameteriv(context, target, pname, params);

}


void d_glGetBufferParameteri64v(void *context, GLenumconst* target, GLenumconst* pname, GLint64*const* params)
{

r_glGetBufferParameteri64v(context, target, pname, params);

}


void d_glGetBooleanv(void *context, GLenumconst* pname, GLboolean*const* data)
{

r_glGetBooleanv(context, pname, data);

}


void d_glGetBooleani_v(void *context, GLenumconst* target, GLuintconst* index, GLboolean*const* data)
{

r_glGetBooleani_v(context, target, index, data);

}


void d_glGetFloatv(void *context, GLenumconst* pname, GLfloat*const* data)
{

r_glGetFloatv(context, pname, data);

}


void d_glGetIntegerv(void *context, GLenumconst* pname, GLint*const* data)
{

r_glGetIntegerv(context, pname, data);

}


void d_glGetIntegeri_v(void *context, GLenumconst* target, GLuintconst* index, GLint*const* data)
{

r_glGetIntegeri_v(context, target, index, data);

}


void d_glGetInteger64v(void *context, GLenumconst* pname, GLint64*const* data)
{

r_glGetInteger64v(context, pname, data);

}


void d_glGetInteger64i_v(void *context, GLenumconst* target, GLuintconst* index, GLint64*const* data)
{

r_glGetInteger64i_v(context, target, index, data);

}




/******* file '1-2' *******/


void d_glMapBufferRange_read(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* length, GLbitfieldconst* access, void*const* mem_buf)
{

r_glMapBufferRange_read(context, target, offset, length, access, mem_buf);

}


void d_glReadPixels_without_bound(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, void*const* pixels)
{

r_glReadPixels_without_bound(context, x, y, width, height, format, type, buf_len, pixels);

}


GLint d_glTestPointer3(void *context, GLintconst* a, const GLint*const* b, GLint*const* c)
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


void d_glBeginQuery(void *context, GLenumconst* target, GLuintconst* id)
{

r_glBeginQuery(context, target, id);

}


void d_glEndQuery(void *context, GLenumconst* target)
{

r_glEndQuery(context, target);

}


void d_glViewport(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height)
{

r_glViewport(context, x, y, width, height);

}


void d_glTexStorage2D(void *context, GLenumconst* target, GLsizeiconst* levels, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height)
{

r_glTexStorage2D(context, target, levels, internalformat, width, height);

}


void d_glTexStorage3D(void *context, GLenumconst* target, GLsizeiconst* levels, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth)
{

r_glTexStorage3D(context, target, levels, internalformat, width, height, depth);

}


void d_glTexImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels)
{

r_glTexImage2D_with_bound(context, target, level, internalformat, width, height, border, format, type, pixels);

}


void d_glTexSubImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels)
{

r_glTexSubImage2D_with_bound(context, target, level, xoffset, yoffset, width, height, format, type, pixels);

}


void d_glTexImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels)
{

r_glTexImage3D_with_bound(context, target, level, internalformat, width, height, depth, border, format, type, pixels);

}


void d_glTexSubImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels)
{

r_glTexSubImage3D_with_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);

}


void d_glReadPixels_with_bound(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels)
{

r_glReadPixels_with_bound(context, x, y, width, height, format, type, pixels);

}


void d_glCompressedTexImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLsizeiconst* imageSize, GLintptrconst* data)
{
{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glCompressedTexImage2D_with_bound(context, target, level, internalformat, width, height, border, imageSize, data);

}


void d_glCompressedTexSubImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLsizeiconst* imageSize, GLintptrconst* data)
{
{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glCompressedTexSubImage2D_with_bound(context, target, level, xoffset, yoffset, width, height, format, imageSize, data);

}


void d_glCompressedTexImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLsizeiconst* imageSize, GLintptrconst* data)
{
{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glCompressedTexImage3D_with_bound(context, target, level, internalformat, width, height, depth, border, imageSize, data);

}


void d_glCompressedTexSubImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLsizeiconst* imageSize, GLintptrconst* data)
{

r_glCompressedTexSubImage3D_with_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);

}


void d_glCopyTexImage2D(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border)
{

r_glCopyTexImage2D(context, target, level, internalformat, x, y, width, height, border);

}


void d_glCopyTexSubImage2D(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height)
{

r_glCopyTexSubImage2D(context, target, level, xoffset, yoffset, x, y, width, height);

}


void d_glCopyTexSubImage3D(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height)
{

r_glCopyTexSubImage3D(context, target, level, xoffset, yoffset, zoffset, x, y, width, height);

}


void d_glVertexAttribPointer_with_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLbooleanconst* normalized, GLsizeiconst* stride, GLintptrconst* pointer)
{

r_glVertexAttribPointer_with_bound(context, index, size, type, normalized, stride, pointer);

}


void d_glVertexAttribPointer_offset(void *context, GLuintconst* index, GLuintconst* size, GLenumconst* type, GLbooleanconst* normalized, GLsizeiconst* stride, GLuintconst* index_father, GLintptrconst* offset)
{

r_glVertexAttribPointer_offset(context, index, size, type, normalized, stride, index_father, offset);

}


void d_glMapBufferRange_write(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* length, GLbitfieldconst* access)
{

r_glMapBufferRange_write(context, target, offset, length, access);

}


void d_glUnmapBuffer_special(void *context, GLenumconst* target)
{

r_glUnmapBuffer_special(context, target);

}


void d_glWaitSync(void *context, GLsyncconst* sync, GLbitfieldconst* flags, GLuint64const* timeout)
{

r_glWaitSync(context, sync, flags, timeout);

}


void d_glShaderBinary(void *context, GLsizeiconst* count, const GLuint*const* shaders, GLenumconst* binaryFormat, const void*const* binary, GLsizeiconst* length)
{

r_glShaderBinary(context, count, shaders, binaryFormat, binary, length);

}


void d_glProgramBinary(void *context, GLuintconst* program, GLenumconst* binaryFormat, const void*const* binary, GLsizeiconst* length)
{

r_glProgramBinary(context, program, binaryFormat, binary, length);

}


void d_glDrawBuffers(void *context, GLsizeiconst* n, const GLenum*const* bufs)
{

r_glDrawBuffers(context, n, bufs);

}


void d_glDrawArrays_origin(void *context, GLenumconst* mode, GLintconst* first, GLsizeiconst* count)
{

r_glDrawArrays_origin(context, mode, first, count);

}


void d_glDrawArraysInstanced_origin(void *context, GLenumconst* mode, GLintconst* first, GLsizeiconst* count, GLsizeiconst* instancecount)
{

r_glDrawArraysInstanced_origin(context, mode, first, count, instancecount);

}


void d_glDrawElementsInstanced_with_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, GLsizeiptrconst* indices, GLsizeiconst* instancecount)
{

r_glDrawElementsInstanced_with_bound(context, mode, count, type, indices, instancecount);

}


void d_glDrawElements_with_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, GLsizeiptrconst* indices)
{

r_glDrawElements_with_bound(context, mode, count, type, indices);

}


void d_glDrawRangeElements_with_bound(void *context, GLenumconst* mode, GLuintconst* start, GLuintconst* end, GLsizeiconst* count, GLenumconst* type, GLsizeiptrconst* indices)
{

r_glDrawRangeElements_with_bound(context, mode, start, end, count, type, indices);

}


void d_glTestIntAsyn(void *context, GLintconst* a, GLuintconst* b, GLfloatconst* c, GLdoubleconst* d)
{

r_glTestIntAsyn(context, a, b, c, d);

}


void d_glPrintfAsyn(void *context, GLintconst* a, GLuintconst* size, GLdoubleconst* c, const GLchar*const* out_string)
{

r_glPrintfAsyn(context, a, size, c, out_string);

}


void d_glEGLImageTargetTexture2DOES(void *context, GLenumconst* target, GLeglImageOESconst* imageSize)
{

r_glEGLImageTargetTexture2DOES(context, target, imageSize);

}


void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenumconst* target, GLeglImageOESconst* image)
{

r_glEGLImageTargetRenderbufferStorageOES(context, target, image);

}




/******* file '2-1-1' *******/


void d_glGenBuffers_special(void *context, GLsizeiconst* n, const GLuint*const* buffers)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenBuffers_special(context, n, buffers);

}


void d_glGenRenderbuffers_special(void *context, GLsizeiconst* n, const GLuint*const* renderbuffers)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenRenderbuffers_special(context, n, renderbuffers);

}


void d_glGenTextures_special(void *context, GLsizeiconst* n, const GLuint*const* textures)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenTextures_special(context, n, textures);

}


void d_glGenSamplers_special(void *context, GLsizeiconst* count, const GLuint*const* samplers)
{
{if(count<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenSamplers_special(context, count, samplers);

}


void d_glCreateProgram_special(void *context, GLuintconst* program)
{

r_glCreateProgram_special(context, program);

}


void d_glCreateShader_special(void *context, GLenumconst* type, GLuintconst* shader)
{
if(type!=GL_COMPUTE_SHADER&&type!=GL_VERTEX_SHADER&&type!=GL_FRAGMENT_SHADER){set_gl_error(context,GL_INVALID_ENUM);return 0;}
r_glCreateShader_special(context, type, shader);

}


void d_glFenceSync_special(void *context, GLenumconst* condition, GLbitfieldconst* flags, GLsyncconst* sync)
{

r_glFenceSync_special(context, condition, flags, sync);

}


void d_glCreateShaderProgramv(void *context, GLenumconst* type, GLsizeiconst* count, const GLchar*const* strings, GLuintconst* program)
{
{if(type!=GL_COMPUTE_SHADER&&type!=GL_VERTEX_SHADER&&type!=GL_FRAGMENT_SHADER){set_gl_error(context,GL_INVALID_ENUM);return 0;}if(count<0){set_gl_error(context,GL_INVALID_VALUE);return 0;}}
r_glCreateShaderProgramv(context, type, count, strings, program);

}


void d_glGenFramebuffers_special(void *context, GLsizeiconst* n, const GLuint*const* framebuffers)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenFramebuffers_special(context, n, framebuffers);

}


void d_glGenProgramPipelines_special(void *context, GLsizeiconst* n, const GLuint*const* pipelines)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenProgramPipelines_special(context, n, pipelines);

}


void d_glGenTransformFeedbacks_special(void *context, GLsizeiconst* n, const GLuint*const* ids)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenTransformFeedbacks_special(context, n, ids);

}


void d_glGenVertexArrays_special(void *context, GLsizeiconst* n, const GLuint*const* arrays)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenVertexArrays_special(context, n, arrays);

}


void d_glGenQueries_special(void *context, GLsizeiconst* n, const GLuint*const* ids)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glGenQueries_special(context, n, ids);

}


void d_glDeleteBuffers_origin(void *context, GLsizeiconst* n, const GLuint*const* buffers)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteBuffers_origin(context, n, buffers);

}


void d_glDeleteRenderbuffers(void *context, GLsizeiconst* n, const GLuint*const* renderbuffers)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteRenderbuffers(context, n, renderbuffers);

}


void d_glDeleteTextures(void *context, GLsizeiconst* n, const GLuint*const* textures)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteTextures(context, n, textures);

}


void d_glDeleteSamplers(void *context, GLsizeiconst* count, const GLuint*const* samplers)
{
{if(count<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteSamplers(context, count, samplers);

}


void d_glDeleteProgram_origin(void *context, GLuintconst* program)
{

r_glDeleteProgram_origin(context, program);

}


void d_glDeleteShader(void *context, GLuintconst* shader)
{

r_glDeleteShader(context, shader);

}


void d_glDeleteSync(void *context, GLsyncconst* sync)
{

r_glDeleteSync(context, sync);

}


void d_glDeleteFramebuffers(void *context, GLsizeiconst* n, const GLuint*const* framebuffers)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteFramebuffers(context, n, framebuffers);

}


void d_glDeleteProgramPipelines(void *context, GLsizeiconst* n, const GLuint*const* pipelines)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteProgramPipelines(context, n, pipelines);

}


void d_glDeleteTransformFeedbacks(void *context, GLsizeiconst* n, const GLuint*const* ids)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteTransformFeedbacks(context, n, ids);

}


void d_glDeleteVertexArrays_origin(void *context, GLsizeiconst* n, const GLuint*const* arrays)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteVertexArrays_origin(context, n, arrays);

}


void d_glDeleteQueries(void *context, GLsizeiconst* n, const GLuint*const* ids)
{
{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glDeleteQueries(context, n, ids);

}




/******* file '2-1-2' *******/


void d_glLinkProgram_origin(void *context, GLuintconst* program)
{

r_glLinkProgram_origin(context, program);

}


void d_glPixelStorei_origin(void *context, GLenumconst* pname, GLintconst* param)
{

r_glPixelStorei_origin(context, pname, param);

}


void d_glDisableVertexAttribArray_origin(void *context, GLuintconst* index)
{

r_glDisableVertexAttribArray_origin(context, index);

}


void d_glEnableVertexAttribArray_origin(void *context, GLuintconst* index)
{

r_glEnableVertexAttribArray_origin(context, index);

}


void d_glReadBuffer_special(void *context, GLenumconst* src)
{

r_glReadBuffer_special(context, src);

}


void d_glVertexAttribDivisor_origin(void *context, GLuintconst* index, GLuintconst* divisor)
{

r_glVertexAttribDivisor_origin(context, index, divisor);

}


void d_glShaderSource_origin(void *context, GLuintconst* shader, GLsizeiconst* count, const GLint*const* length, const GLchar*const* string)
{

r_glShaderSource_origin(context, shader, count, length, string);

}


void d_glVertexAttribIPointer_with_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLsizeiconst* stride, GLintptrconst* pointer)
{

r_glVertexAttribIPointer_with_bound(context, index, size, type, stride, pointer);

}


void d_glVertexAttribIPointer_offset(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLsizeiconst* stride, GLuintconst* index_father, GLintptrconst* offset)
{

r_glVertexAttribIPointer_offset(context, index, size, type, stride, index_father, offset);

}


void d_glBindVertexArray_special(void *context, GLuintconst* array)
{

r_glBindVertexArray_special(context, array);

}


void d_glBindBuffer_origin(void *context, GLenumconst* target, GLuintconst* buffer)
{

r_glBindBuffer_origin(context, target, buffer);

}


void d_glBeginTransformFeedback(void *context, GLenumconst* primitiveMode)
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


void d_glBindBufferRange(void *context, GLenumconst* target, GLuintconst* index, GLuintconst* buffer, GLintptrconst* offset, GLsizeiptrconst* size)
{

r_glBindBufferRange(context, target, index, buffer, offset, size);

}


void d_glBindBufferBase(void *context, GLenumconst* target, GLuintconst* index, GLuintconst* buffer)
{

r_glBindBufferBase(context, target, index, buffer);

}


void d_glBindTexture(void *context, GLenumconst* target, GLuintconst* texture)
{

r_glBindTexture(context, target, texture);

}


void d_glBindRenderbuffer(void *context, GLenumconst* target, GLuintconst* renderbuffer)
{

r_glBindRenderbuffer(context, target, renderbuffer);

}


void d_glBindSampler(void *context, GLuintconst* unit, GLuintconst* sampler)
{

r_glBindSampler(context, unit, sampler);

}


void d_glBindFramebuffer(void *context, GLenumconst* target, GLuintconst* framebuffer)
{

r_glBindFramebuffer(context, target, framebuffer);

}


void d_glBindProgramPipeline(void *context, GLuintconst* pipeline)
{

r_glBindProgramPipeline(context, pipeline);

}


void d_glBindTransformFeedback(void *context, GLenumconst* target, GLuintconst* id)
{

r_glBindTransformFeedback(context, target, id);

}


void d_glSamplerParameteriv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLint*const* param)
{

r_glSamplerParameteriv(context, sampler, pname, param);

}


void d_glSamplerParameterfv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLfloat*const* param)
{

r_glSamplerParameterfv(context, sampler, pname, param);

}


void d_glActiveTexture(void *context, GLenumconst* texture)
{

r_glActiveTexture(context, texture);

}


void d_glAttachShader(void *context, GLuintconst* program, GLuintconst* shader)
{

r_glAttachShader(context, program, shader);

}


void d_glBlendColor(void *context, GLfloatconst* red, GLfloatconst* green, GLfloatconst* blue, GLfloatconst* alpha)
{

r_glBlendColor(context, red, green, blue, alpha);

}


void d_glBlendEquation(void *context, GLenumconst* mode)
{

r_glBlendEquation(context, mode);

}


void d_glBlendEquationSeparate(void *context, GLenumconst* modeRGB, GLenumconst* modeAlpha)
{

r_glBlendEquationSeparate(context, modeRGB, modeAlpha);

}


void d_glBlendFunc(void *context, GLenumconst* sfactor, GLenumconst* dfactor)
{

r_glBlendFunc(context, sfactor, dfactor);

}


void d_glBlendFuncSeparate(void *context, GLenumconst* sfactorRGB, GLenumconst* dfactorRGB, GLenumconst* sfactorAlpha, GLenumconst* dfactorAlpha)
{

r_glBlendFuncSeparate(context, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);

}


void d_glClear(void *context, GLbitfieldconst* mask)
{

r_glClear(context, mask);

}


void d_glClearColor(void *context, GLfloatconst* red, GLfloatconst* green, GLfloatconst* blue, GLfloatconst* alpha)
{

r_glClearColor(context, red, green, blue, alpha);

}


void d_glClearDepthf(void *context, GLfloatconst* d)
{

r_glClearDepthf(context, d);

}


void d_glClearStencil(void *context, GLintconst* s)
{

r_glClearStencil(context, s);

}


void d_glColorMask(void *context, GLbooleanconst* red, GLbooleanconst* green, GLbooleanconst* blue, GLbooleanconst* alpha)
{

r_glColorMask(context, red, green, blue, alpha);

}


void d_glCompileShader(void *context, GLuintconst* shader)
{

r_glCompileShader(context, shader);

}


void d_glCullFace(void *context, GLenumconst* mode)
{

r_glCullFace(context, mode);

}


void d_glDepthFunc(void *context, GLenumconst* func)
{

r_glDepthFunc(context, func);

}


void d_glDepthMask(void *context, GLbooleanconst* flag)
{

r_glDepthMask(context, flag);

}


void d_glDepthRangef(void *context, GLfloatconst* n, GLfloatconst* f)
{

r_glDepthRangef(context, n, f);

}


void d_glDetachShader(void *context, GLuintconst* program, GLuintconst* shader)
{

r_glDetachShader(context, program, shader);

}


void d_glDisable(void *context, GLenumconst* cap)
{

r_glDisable(context, cap);

}


void d_glEnable(void *context, GLenumconst* cap)
{

r_glEnable(context, cap);

}


void d_glFramebufferRenderbuffer(void *context, GLenumconst* target, GLenumconst* attachment, GLenumconst* renderbuffertarget, GLuintconst* renderbuffer)
{

r_glFramebufferRenderbuffer(context, target, attachment, renderbuffertarget, renderbuffer);

}


void d_glFramebufferTexture2D(void *context, GLenumconst* target, GLenumconst* attachment, GLenumconst* textarget, GLuintconst* texture, GLintconst* level)
{

r_glFramebufferTexture2D(context, target, attachment, textarget, texture, level);

}


void d_glFrontFace(void *context, GLenumconst* mode)
{

r_glFrontFace(context, mode);

}


void d_glGenerateMipmap(void *context, GLenumconst* target)
{

r_glGenerateMipmap(context, target);

}


void d_glHint(void *context, GLenumconst* target, GLenumconst* mode)
{

r_glHint(context, target, mode);

}


void d_glLineWidth(void *context, GLfloatconst* width)
{

r_glLineWidth(context, width);

}


void d_glPolygonOffset(void *context, GLfloatconst* factor, GLfloatconst* units)
{

r_glPolygonOffset(context, factor, units);

}


void d_glReleaseShaderCompiler(void *context)
{

r_glReleaseShaderCompiler(context);

}


void d_glRenderbufferStorage(void *context, GLenumconst* target, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height)
{

r_glRenderbufferStorage(context, target, internalformat, width, height);

}


void d_glSampleCoverage(void *context, GLfloatconst* value, GLbooleanconst* invert)
{

r_glSampleCoverage(context, value, invert);

}


void d_glScissor(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height)
{

r_glScissor(context, x, y, width, height);

}


void d_glStencilFunc(void *context, GLenumconst* func, GLintconst* ref, GLuintconst* mask)
{

r_glStencilFunc(context, func, ref, mask);

}


void d_glStencilFuncSeparate(void *context, GLenumconst* face, GLenumconst* func, GLintconst* ref, GLuintconst* mask)
{

r_glStencilFuncSeparate(context, face, func, ref, mask);

}


void d_glStencilMask(void *context, GLuintconst* mask)
{

r_glStencilMask(context, mask);

}


void d_glStencilMaskSeparate(void *context, GLenumconst* face, GLuintconst* mask)
{

r_glStencilMaskSeparate(context, face, mask);

}


void d_glStencilOp(void *context, GLenumconst* fail, GLenumconst* zfail, GLenumconst* zpass)
{

r_glStencilOp(context, fail, zfail, zpass);

}


void d_glStencilOpSeparate(void *context, GLenumconst* face, GLenumconst* sfail, GLenumconst* dpfail, GLenumconst* dppass)
{

r_glStencilOpSeparate(context, face, sfail, dpfail, dppass);

}


void d_glTexParameterf(void *context, GLenumconst* target, GLenumconst* pname, GLfloatconst* param)
{

r_glTexParameterf(context, target, pname, param);

}


void d_glTexParameteri(void *context, GLenumconst* target, GLenumconst* pname, GLintconst* param)
{

r_glTexParameteri(context, target, pname, param);

}


void d_glUniform1f(void *context, GLintconst* location, GLfloatconst* v0)
{

r_glUniform1f(context, location, v0);

}


void d_glUniform1i(void *context, GLintconst* location, GLintconst* v0)
{

r_glUniform1i(context, location, v0);

}


void d_glUniform2f(void *context, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1)
{

r_glUniform2f(context, location, v0, v1);

}


void d_glUniform2i(void *context, GLintconst* location, GLintconst* v0, GLintconst* v1)
{

r_glUniform2i(context, location, v0, v1);

}


void d_glUniform3f(void *context, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2)
{

r_glUniform3f(context, location, v0, v1, v2);

}


void d_glUniform3i(void *context, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2)
{

r_glUniform3i(context, location, v0, v1, v2);

}


void d_glUniform4f(void *context, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2, GLfloatconst* v3)
{

r_glUniform4f(context, location, v0, v1, v2, v3);

}


void d_glUniform4i(void *context, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2, GLintconst* v3)
{

r_glUniform4i(context, location, v0, v1, v2, v3);

}


void d_glUseProgram(void *context, GLuintconst* program)
{

r_glUseProgram(context, program);

}


void d_glValidateProgram(void *context, GLuintconst* program)
{

r_glValidateProgram(context, program);

}


void d_glVertexAttrib1f(void *context, GLuintconst* index, GLfloatconst* x)
{

r_glVertexAttrib1f(context, index, x);

}


void d_glVertexAttrib2f(void *context, GLuintconst* index, GLfloatconst* x, GLfloatconst* y)
{

r_glVertexAttrib2f(context, index, x, y);

}


void d_glVertexAttrib3f(void *context, GLuintconst* index, GLfloatconst* x, GLfloatconst* y, GLfloatconst* z)
{

r_glVertexAttrib3f(context, index, x, y, z);

}


void d_glVertexAttrib4f(void *context, GLuintconst* index, GLfloatconst* x, GLfloatconst* y, GLfloatconst* z, GLfloatconst* w)
{

r_glVertexAttrib4f(context, index, x, y, z, w);

}


void d_glBlitFramebuffer(void *context, GLintconst* srcX0, GLintconst* srcY0, GLintconst* srcX1, GLintconst* srcY1, GLintconst* dstX0, GLintconst* dstY0, GLintconst* dstX1, GLintconst* dstY1, GLbitfieldconst* mask, GLenumconst* filter)
{

r_glBlitFramebuffer(context, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);

}


void d_glRenderbufferStorageMultisample(void *context, GLenumconst* target, GLsizeiconst* samples, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height)
{

r_glRenderbufferStorageMultisample(context, target, samples, internalformat, width, height);

}


void d_glFramebufferTextureLayer(void *context, GLenumconst* target, GLenumconst* attachment, GLuintconst* texture, GLintconst* level, GLintconst* layer)
{

r_glFramebufferTextureLayer(context, target, attachment, texture, level, layer);

}


void d_glVertexAttribI4i(void *context, GLuintconst* index, GLintconst* x, GLintconst* y, GLintconst* z, GLintconst* w)
{

r_glVertexAttribI4i(context, index, x, y, z, w);

}


void d_glVertexAttribI4ui(void *context, GLuintconst* index, GLuintconst* x, GLuintconst* y, GLuintconst* z, GLuintconst* w)
{

r_glVertexAttribI4ui(context, index, x, y, z, w);

}


void d_glUniform1ui(void *context, GLintconst* location, GLuintconst* v0)
{

r_glUniform1ui(context, location, v0);

}


void d_glUniform2ui(void *context, GLintconst* location, GLuintconst* v0, GLuintconst* v1)
{

r_glUniform2ui(context, location, v0, v1);

}


void d_glUniform3ui(void *context, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2)
{

r_glUniform3ui(context, location, v0, v1, v2);

}


void d_glUniform4ui(void *context, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2, GLuintconst* v3)
{

r_glUniform4ui(context, location, v0, v1, v2, v3);

}


void d_glClearBufferfi(void *context, GLenumconst* buffer, GLintconst* drawbuffer, GLfloatconst* depth, GLintconst* stencil)
{

r_glClearBufferfi(context, buffer, drawbuffer, depth, stencil);

}


void d_glCopyBufferSubData(void *context, GLenumconst* readTarget, GLenumconst* writeTarget, GLintptrconst* readOffset, GLintptrconst* writeOffset, GLsizeiptrconst* size)
{

r_glCopyBufferSubData(context, readTarget, writeTarget, readOffset, writeOffset, size);

}


void d_glUniformBlockBinding(void *context, GLuintconst* program, GLuintconst* uniformBlockIndex, GLuintconst* uniformBlockBinding)
{

r_glUniformBlockBinding(context, program, uniformBlockIndex, uniformBlockBinding);

}


void d_glSamplerParameteri(void *context, GLuintconst* sampler, GLenumconst* pname, GLintconst* param)
{

r_glSamplerParameteri(context, sampler, pname, param);

}


void d_glSamplerParameterf(void *context, GLuintconst* sampler, GLenumconst* pname, GLfloatconst* param)
{

r_glSamplerParameterf(context, sampler, pname, param);

}


void d_glProgramParameteri(void *context, GLuintconst* program, GLenumconst* pname, GLintconst* value)
{

r_glProgramParameteri(context, program, pname, value);

}


void d_glAlphaFuncxOES(void *context, GLenumconst* func, GLfixedconst* ref)
{

r_glAlphaFuncxOES(context, func, ref);

}


void d_glClearColorxOES(void *context, GLfixedconst* red, GLfixedconst* green, GLfixedconst* blue, GLfixedconst* alpha)
{

r_glClearColorxOES(context, red, green, blue, alpha);

}


void d_glClearDepthxOES(void *context, GLfixedconst* depth)
{

r_glClearDepthxOES(context, depth);

}


void d_glColor4xOES(void *context, GLfixedconst* red, GLfixedconst* green, GLfixedconst* blue, GLfixedconst* alpha)
{

r_glColor4xOES(context, red, green, blue, alpha);

}


void d_glDepthRangexOES(void *context, GLfixedconst* n, GLfixedconst* f)
{

r_glDepthRangexOES(context, n, f);

}


void d_glFogxOES(void *context, GLenumconst* pname, GLfixedconst* param)
{

r_glFogxOES(context, pname, param);

}


void d_glFrustumxOES(void *context, GLfixedconst* l, GLfixedconst* r, GLfixedconst* b, GLfixedconst* t, GLfixedconst* n, GLfixedconst* f)
{

r_glFrustumxOES(context, l, r, b, t, n, f);

}


void d_glLightModelxOES(void *context, GLenumconst* pname, GLfixedconst* param)
{

r_glLightModelxOES(context, pname, param);

}


void d_glLightxOES(void *context, GLenumconst* light, GLenumconst* pname, GLfixedconst* param)
{

r_glLightxOES(context, light, pname, param);

}


void d_glLineWidthxOES(void *context, GLfixedconst* width)
{

r_glLineWidthxOES(context, width);

}


void d_glMaterialxOES(void *context, GLenumconst* face, GLenumconst* pname, GLfixedconst* param)
{

r_glMaterialxOES(context, face, pname, param);

}


void d_glMultiTexCoord4xOES(void *context, GLenumconst* texture, GLfixedconst* s, GLfixedconst* t, GLfixedconst* r, GLfixedconst* q)
{

r_glMultiTexCoord4xOES(context, texture, s, t, r, q);

}


void d_glNormal3xOES(void *context, GLfixedconst* nx, GLfixedconst* ny, GLfixedconst* nz)
{

r_glNormal3xOES(context, nx, ny, nz);

}


void d_glOrthoxOES(void *context, GLfixedconst* l, GLfixedconst* r, GLfixedconst* b, GLfixedconst* t, GLfixedconst* n, GLfixedconst* f)
{

r_glOrthoxOES(context, l, r, b, t, n, f);

}


void d_glPointSizexOES(void *context, GLfixedconst* size)
{

r_glPointSizexOES(context, size);

}


void d_glPolygonOffsetxOES(void *context, GLfixedconst* factor, GLfixedconst* units)
{

r_glPolygonOffsetxOES(context, factor, units);

}


void d_glRotatexOES(void *context, GLfixedconst* angle, GLfixedconst* x, GLfixedconst* y, GLfixedconst* z)
{

r_glRotatexOES(context, angle, x, y, z);

}


void d_glScalexOES(void *context, GLfixedconst* x, GLfixedconst* y, GLfixedconst* z)
{

r_glScalexOES(context, x, y, z);

}


void d_glTexEnvxOES(void *context, GLenumconst* target, GLenumconst* pname, GLfixedconst* param)
{

r_glTexEnvxOES(context, target, pname, param);

}


void d_glTranslatexOES(void *context, GLfixedconst* x, GLfixedconst* y, GLfixedconst* z)
{

r_glTranslatexOES(context, x, y, z);

}


void d_glPointParameterxOES(void *context, GLenumconst* pname, GLfixedconst* param)
{

r_glPointParameterxOES(context, pname, param);

}


void d_glSampleCoveragexOES(void *context, GLclampxconst* value, GLbooleanconst* invert)
{

r_glSampleCoveragexOES(context, value, invert);

}


void d_glTexGenxOES(void *context, GLenumconst* coord, GLenumconst* pname, GLfixedconst* param)
{

r_glTexGenxOES(context, coord, pname, param);

}


void d_glClearDepthfOES(void *context, GLclampfconst* depth)
{

r_glClearDepthfOES(context, depth);

}


void d_glDepthRangefOES(void *context, GLclampfconst* n, GLclampfconst* f)
{

r_glDepthRangefOES(context, n, f);

}


void d_glFrustumfOES(void *context, GLfloatconst* l, GLfloatconst* r, GLfloatconst* b, GLfloatconst* t, GLfloatconst* n, GLfloatconst* f)
{

r_glFrustumfOES(context, l, r, b, t, n, f);

}


void d_glOrthofOES(void *context, GLfloatconst* l, GLfloatconst* r, GLfloatconst* b, GLfloatconst* t, GLfloatconst* n, GLfloatconst* f)
{

r_glOrthofOES(context, l, r, b, t, n, f);

}


void d_glRenderbufferStorageMultisampleEXT(void *context, GLenumconst* target, GLsizeiconst* samples, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height)
{

r_glRenderbufferStorageMultisampleEXT(context, target, samples, internalformat, width, height);

}


void d_glUseProgramStages(void *context, GLuintconst* pipeline, GLbitfieldconst* stages, GLuintconst* program)
{

r_glUseProgramStages(context, pipeline, stages, program);

}


void d_glActiveShaderProgram(void *context, GLuintconst* pipeline, GLuintconst* program)
{

r_glActiveShaderProgram(context, pipeline, program);

}


void d_glProgramUniform1i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0)
{

r_glProgramUniform1i(context, program, location, v0);

}


void d_glProgramUniform2i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0, GLintconst* v1)
{

r_glProgramUniform2i(context, program, location, v0, v1);

}


void d_glProgramUniform3i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2)
{

r_glProgramUniform3i(context, program, location, v0, v1, v2);

}


void d_glProgramUniform4i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2, GLintconst* v3)
{

r_glProgramUniform4i(context, program, location, v0, v1, v2, v3);

}


void d_glProgramUniform1ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0)
{

r_glProgramUniform1ui(context, program, location, v0);

}


void d_glProgramUniform2ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0, GLuintconst* v1)
{

r_glProgramUniform2ui(context, program, location, v0, v1);

}


void d_glProgramUniform3ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2)
{

r_glProgramUniform3ui(context, program, location, v0, v1, v2);

}


void d_glProgramUniform4ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2, GLuintconst* v3)
{

r_glProgramUniform4ui(context, program, location, v0, v1, v2, v3);

}


void d_glProgramUniform1f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0)
{

r_glProgramUniform1f(context, program, location, v0);

}


void d_glProgramUniform2f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1)
{

r_glProgramUniform2f(context, program, location, v0, v1);

}


void d_glProgramUniform3f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2)
{

r_glProgramUniform3f(context, program, location, v0, v1, v2);

}


void d_glProgramUniform4f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2, GLfloatconst* v3)
{

r_glProgramUniform4f(context, program, location, v0, v1, v2, v3);

}


void d_glTransformFeedbackVaryings(void *context, GLuintconst* program, GLsizeiconst* count, const GLchar*const* varyings, GLenumconst* bufferMode)
{

r_glTransformFeedbackVaryings(context, program, count, varyings, bufferMode);

}


void d_glTexParameterfv(void *context, GLenumconst* target, GLenumconst* pname, const GLfloat*const* params)
{

r_glTexParameterfv(context, target, pname, params);

}


void d_glTexParameteriv(void *context, GLenumconst* target, GLenumconst* pname, const GLint*const* params)
{

r_glTexParameteriv(context, target, pname, params);

}


void d_glUniform1fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glUniform1fv(context, location, count, value);

}


void d_glUniform1iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glUniform1iv(context, location, count, value);

}


void d_glUniform2fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glUniform2fv(context, location, count, value);

}


void d_glUniform2iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glUniform2iv(context, location, count, value);

}


void d_glUniform3fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glUniform3fv(context, location, count, value);

}


void d_glUniform3iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glUniform3iv(context, location, count, value);

}


void d_glUniform4fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glUniform4fv(context, location, count, value);

}


void d_glUniform4iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glUniform4iv(context, location, count, value);

}


void d_glVertexAttrib1fv(void *context, GLuintconst* index, const GLfloat*const* v)
{

r_glVertexAttrib1fv(context, index, v);

}


void d_glVertexAttrib2fv(void *context, GLuintconst* index, const GLfloat*const* v)
{

r_glVertexAttrib2fv(context, index, v);

}


void d_glVertexAttrib3fv(void *context, GLuintconst* index, const GLfloat*const* v)
{

r_glVertexAttrib3fv(context, index, v);

}


void d_glVertexAttrib4fv(void *context, GLuintconst* index, const GLfloat*const* v)
{

r_glVertexAttrib4fv(context, index, v);

}


void d_glUniformMatrix2fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix2fv(context, location, count, transpose, value);

}


void d_glUniformMatrix3fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix3fv(context, location, count, transpose, value);

}


void d_glUniformMatrix4fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix4fv(context, location, count, transpose, value);

}


void d_glUniformMatrix2x3fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix2x3fv(context, location, count, transpose, value);

}


void d_glUniformMatrix3x2fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix3x2fv(context, location, count, transpose, value);

}


void d_glUniformMatrix2x4fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix2x4fv(context, location, count, transpose, value);

}


void d_glUniformMatrix4x2fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix4x2fv(context, location, count, transpose, value);

}


void d_glUniformMatrix3x4fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix3x4fv(context, location, count, transpose, value);

}


void d_glUniformMatrix4x3fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glUniformMatrix4x3fv(context, location, count, transpose, value);

}


void d_glVertexAttribI4iv(void *context, GLuintconst* index, const GLint*const* v)
{

r_glVertexAttribI4iv(context, index, v);

}


void d_glVertexAttribI4uiv(void *context, GLuintconst* index, const GLuint*const* v)
{

r_glVertexAttribI4uiv(context, index, v);

}


void d_glUniform1uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glUniform1uiv(context, location, count, value);

}


void d_glUniform2uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glUniform2uiv(context, location, count, value);

}


void d_glUniform3uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glUniform3uiv(context, location, count, value);

}


void d_glUniform4uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glUniform4uiv(context, location, count, value);

}


void d_glClearBufferiv(void *context, GLenumconst* buffer, GLintconst* drawbuffer, const GLint*const* value)
{

r_glClearBufferiv(context, buffer, drawbuffer, value);

}


void d_glClearBufferuiv(void *context, GLenumconst* buffer, GLintconst* drawbuffer, const GLuint*const* value)
{

r_glClearBufferuiv(context, buffer, drawbuffer, value);

}


void d_glClearBufferfv(void *context, GLenumconst* buffer, GLintconst* drawbuffer, const GLfloat*const* value)
{

r_glClearBufferfv(context, buffer, drawbuffer, value);

}


void d_glSamplerParameteriv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLint*const* param)
{

r_glSamplerParameteriv(context, sampler, pname, param);

}


void d_glSamplerParameterfv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLfloat*const* param)
{

r_glSamplerParameterfv(context, sampler, pname, param);

}


void d_glInvalidateFramebuffer(void *context, GLenumconst* target, GLsizeiconst* numAttachments, const GLenum*const* attachments)
{

r_glInvalidateFramebuffer(context, target, numAttachments, attachments);

}


void d_glInvalidateSubFramebuffer(void *context, GLenumconst* target, GLsizeiconst* numAttachments, const GLenum*const* attachments, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height)
{

r_glInvalidateSubFramebuffer(context, target, numAttachments, attachments, x, y, width, height);

}


void d_glClipPlanexOES(void *context, GLenumconst* plane, const GLfixed*const* equation)
{

r_glClipPlanexOES(context, plane, equation);

}


void d_glFogxvOES(void *context, GLenumconst* pname, const GLfixed*const* param)
{

r_glFogxvOES(context, pname, param);

}


void d_glLightModelxvOES(void *context, GLenumconst* pname, const GLfixed*const* param)
{

r_glLightModelxvOES(context, pname, param);

}


void d_glLightxvOES(void *context, GLenumconst* light, GLenumconst* pname, const GLfixed*const* params)
{

r_glLightxvOES(context, light, pname, params);

}


void d_glLoadMatrixxOES(void *context, const GLfixed*const* m)
{

r_glLoadMatrixxOES(context, m);

}


void d_glMaterialxvOES(void *context, GLenumconst* face, GLenumconst* pname, const GLfixed*const* param)
{

r_glMaterialxvOES(context, face, pname, param);

}


void d_glMultMatrixxOES(void *context, const GLfixed*const* m)
{

r_glMultMatrixxOES(context, m);

}


void d_glPointParameterxvOES(void *context, GLenumconst* pname, const GLfixed*const* params)
{

r_glPointParameterxvOES(context, pname, params);

}


void d_glTexEnvxvOES(void *context, GLenumconst* target, GLenumconst* pname, const GLfixed*const* params)
{

r_glTexEnvxvOES(context, target, pname, params);

}


void d_glClipPlanefOES(void *context, GLenumconst* plane, const GLfloat*const* equation)
{

r_glClipPlanefOES(context, plane, equation);

}


void d_glTexGenxvOES(void *context, GLenumconst* coord, GLenumconst* pname, const GLfixed*const* params)
{

r_glTexGenxvOES(context, coord, pname, params);

}


void d_glProgramUniform1iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glProgramUniform1iv(context, program, location, count, value);

}


void d_glProgramUniform2iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glProgramUniform2iv(context, program, location, count, value);

}


void d_glProgramUniform3iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glProgramUniform3iv(context, program, location, count, value);

}


void d_glProgramUniform4iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value)
{

r_glProgramUniform4iv(context, program, location, count, value);

}


void d_glProgramUniform1uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glProgramUniform1uiv(context, program, location, count, value);

}


void d_glProgramUniform2uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glProgramUniform2uiv(context, program, location, count, value);

}


void d_glProgramUniform3uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glProgramUniform3uiv(context, program, location, count, value);

}


void d_glProgramUniform4uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value)
{

r_glProgramUniform4uiv(context, program, location, count, value);

}


void d_glProgramUniform1fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glProgramUniform1fv(context, program, location, count, value);

}


void d_glProgramUniform2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glProgramUniform2fv(context, program, location, count, value);

}


void d_glProgramUniform3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glProgramUniform3fv(context, program, location, count, value);

}


void d_glProgramUniform4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value)
{

r_glProgramUniform4fv(context, program, location, count, value);

}


void d_glProgramUniformMatrix2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix2fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix3fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix4fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix2x3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix2x3fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix3x2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix3x2fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix2x4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix2x4fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix4x2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix4x2fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix3x4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix3x4fv(context, program, location, count, transpose, value);

}


void d_glProgramUniformMatrix4x3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value)
{

r_glProgramUniformMatrix4x3fv(context, program, location, count, transpose, value);

}


void d_glBindAttribLocation(void *context, GLuintconst* program, GLuintconst* index, const GLchar*const* name)
{

r_glBindAttribLocation(context, program, index, name);

}




/******* file '2-2' *******/


void d_glVertexAttribIPointer_without_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLsizeiconst* stride, GLuintconst* offset, GLsizeiconst* length, const void*const* pointer)
{

r_glVertexAttribIPointer_without_bound(context, index, size, type, stride, offset, length, pointer);

}


void d_glVertexAttribPointer_without_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLbooleanconst* normalized, GLsizeiconst* stride, GLuintconst* offset, GLuintconst* length, const void*const* pointer)
{

r_glVertexAttribPointer_without_bound(context, index, size, type, normalized, stride, offset, length, pointer);

}


void d_glDrawElements_without_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, const void*const* indices)
{

r_glDrawElements_without_bound(context, mode, count, type, indices);

}


void d_glDrawElementsInstanced_without_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, const void*const* indices, GLsizeiconst* instancecount)
{

r_glDrawElementsInstanced_without_bound(context, mode, count, type, indices, instancecount);

}


void d_glDrawRangeElements_without_bound(void *context, GLenumconst* mode, GLuintconst* start, GLuintconst* end, GLsizeiconst* count, GLenumconst* type, const void*const* indices)
{

r_glDrawRangeElements_without_bound(context, mode, start, end, count, type, indices);

}


void d_glFlushMappedBufferRange_special(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* length, const void*const* data)
{

r_glFlushMappedBufferRange_special(context, target, offset, length, data);

}


void d_glBufferData_custom(void *context, GLenumconst* target, GLsizeiptrconst* size, const void*const* data, GLenumconst* usage)
{

r_glBufferData_custom(context, target, size, data, usage);

}


void d_glBufferSubData_custom(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* size, const void*const* data)
{

r_glBufferSubData_custom(context, target, offset, size, data);

}


void d_glCompressedTexImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLsizeiconst* imageSize, const void*const* data)
{
{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glCompressedTexImage2D_without_bound(context, target, level, internalformat, width, height, border, imageSize, data);

}


void d_glCompressedTexSubImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLsizeiconst* imageSize, const void*const* data)
{
{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glCompressedTexSubImage2D_without_bound(context, target, level, xoffset, yoffset, width, height, format, imageSize, data);

}


void d_glCompressedTexImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLsizeiconst* imageSize, const void*const* data)
{
{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glCompressedTexImage3D_without_bound(context, target, level, internalformat, width, height, depth, border, imageSize, data);

}


void d_glCompressedTexSubImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLsizeiconst* imageSize, const void*const* data)
{
{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}
r_glCompressedTexSubImage3D_without_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);

}


void d_glTexImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels)
{

r_glTexImage2D_without_bound(context, target, level, internalformat, width, height, border, format, type, buf_len, pixels);

}


void d_glTexImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels)
{

r_glTexImage3D_without_bound(context, target, level, internalformat, width, height, depth, border, format, type, buf_len, pixels);

}


void d_glTexSubImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels)
{

r_glTexSubImage2D_without_bound(context, target, level, xoffset, yoffset, width, height, format, type, buf_len, pixels);

}


void d_glTexSubImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels)
{

r_glTexSubImage3D_without_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, buf_len, pixels);

}


void d_glPrintf(void *context, GLintconst* buf_len, const GLchar*const* out_string)
{

r_glPrintf(context, buf_len, out_string);

}


