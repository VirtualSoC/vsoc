
#include "define_gl.h"
#include "utils_gl.h"


//#include "all_gl.h"

#include <cstring>
//typedef khronos_int32_t  GLclampx;



/******* file '1-1-1' *******/




/******* file '1-1-2' *******/




/******* file '1-2' *******/


GLenum d_glGetError(void *context)
{
return d_glGetError_origin(context);

}


GLboolean d_glIsBuffer(void *context, GLuint buffer)
{
return d_glIsBuffer_origin(context, buffer);

}


GLboolean d_glIsEnabled(void *context, GLenum cap)
{
return d_glIsEnabled_origin(context, cap);

}


GLboolean d_glIsFramebuffer(void *context, GLuint framebuffer)
{
return d_glIsFramebuffer_origin(context, framebuffer);

}


GLboolean d_glIsProgram(void *context, GLuint program)
{
return d_glIsProgram_origin(context, program);

}


GLboolean d_glIsRenderbuffer(void *context, GLuint renderbuffer)
{
return d_glIsRenderbuffer_origin(context, renderbuffer);

}


GLboolean d_glIsShader(void *context, GLuint shader)
{
return d_glIsShader_origin(context, shader);

}


GLboolean d_glIsTexture(void *context, GLuint texture)
{
return d_glIsTexture_origin(context, texture);

}


GLboolean d_glIsQuery(void *context, GLuint id)
{
return d_glIsQuery_origin(context, id);

}


GLboolean d_glIsVertexArray(void *context, GLuint array)
{
return d_glIsVertexArray_origin(context, array);

}


GLboolean d_glIsSampler(void *context, GLuint sampler)
{
return d_glIsSampler_origin(context, sampler);

}


GLboolean d_glIsTransformFeedback(void *context, GLuint id)
{
return d_glIsTransformFeedback_origin(context, id);

}


GLint d_glGetAttribLocation(void *context, GLuint program, const GLchar* name)
{
return d_glGetAttribLocation_origin(context, program, name);

}


GLint d_glGetUniformLocation(void *context, GLuint program, const GLchar* name)
{
return d_glGetUniformLocation_origin(context, program, name);

}


GLint d_glGetFragDataLocation(void *context, GLuint program, const GLchar* name)
{
return d_glGetFragDataLocation_origin(context, program, name);

}


GLuint d_glGetUniformBlockIndex(void *context, GLuint program, const GLchar* uniformBlockName)
{
return d_glGetUniformBlockIndex_origin(context, program, uniformBlockName);

}


GLuint d_glGetProgramResourceIndex(void *context, GLuint program, GLenum programInterface, const GLchar* name)
{
return d_glGetProgramResourceIndex_origin(context, program, programInterface, name);

}


GLint d_glGetProgramResourceLocation(void *context, GLuint program, GLenum programInterface, const GLchar* name)
{
return d_glGetProgramResourceLocation_origin(context, program, programInterface, name);

}


void d_glGetActiveAttrib(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
d_glGetActiveAttrib_origin(context, program, index, bufSize, length, size, type, name);

}


void d_glGetActiveUniform(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
d_glGetActiveUniform_origin(context, program, index, bufSize, length, size, type, name);

}


void d_glGetAttachedShaders(void *context, GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders)
{
d_glGetAttachedShaders_origin(context, program, maxCount, count, shaders);

}


void d_glGetProgramiv(void *context, GLuint program, GLenum pname, GLint* params)
{
d_glGetProgramiv_origin(context, program, pname, params);

}


void d_glGetShaderiv(void *context, GLuint shader, GLenum pname, GLint* params)
{
d_glGetShaderiv_origin(context, shader, pname, params);

}


void d_glGetUniformfv(void *context, GLuint program, GLint location, GLfloat* params)
{
d_glGetUniformfv_origin(context, program, location, params);

}


void d_glGetUniformiv(void *context, GLuint program, GLint location, GLint* params)
{
d_glGetUniformiv_origin(context, program, location, params);

}


void d_glGetUniformuiv(void *context, GLuint program, GLint location, GLuint* params)
{
d_glGetUniformuiv_origin(context, program, location, params);

}


void d_glGetUniformIndices(void *context, GLuint program, GLsizei uniformCount, const GLchar*const* uniformNames, GLuint* uniformIndices)
{
d_glGetUniformIndices_origin(context, program, uniformCount, uniformNames, uniformIndices);

}


void d_glGetBufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params)
{
d_glGetBufferParameteriv_origin(context, target, pname, params);

}


void d_glGetBufferParameteri64v(void *context, GLenum target, GLenum pname, GLint64* params)
{
d_glGetBufferParameteri64v_origin(context, target, pname, params);

}


void d_glGetBooleanv(void *context, GLenum pname, GLboolean* data)
{
d_glGetBooleanv_origin(context, pname, data);

}


void d_glGetBooleani_v(void *context, GLenum target, GLuint index, GLboolean* data)
{
d_glGetBooleani_v_origin(context, target, index, data);

}


void d_glGetFloatv(void *context, GLenum pname, GLfloat* data)
{
d_glGetFloatv_origin(context, pname, data);

}


void d_glGetIntegerv(void *context, GLenum pname, GLint* data)
{
d_glGetIntegerv_origin(context, pname, data);

}


void d_glGetIntegeri_v(void *context, GLenum target, GLuint index, GLint* data)
{
d_glGetIntegeri_v_origin(context, target, index, data);

}


void d_glGetInteger64v(void *context, GLenum pname, GLint64* data)
{
d_glGetInteger64v_origin(context, pname, data);

}


void d_glGetInteger64i_v(void *context, GLenum target, GLuint index, GLint64* data)
{
d_glGetInteger64i_v_origin(context, target, index, data);

}




/******* file '2-1-1' *******/




/******* file '2-1-2' *******/




/******* file '2-2' *******/


void d_glGenBuffers(void *context, GLsizei n, GLuint* buffers)
{
d_glGenBuffers_origin(context, n, buffers);

}


void d_glGenFramebuffers(void *context, GLsizei n, GLuint* framebuffers)
{
d_glGenFramebuffers_origin(context, n, framebuffers);

}


void d_glGenRenderbuffers(void *context, GLsizei n, GLuint* renderbuffers)
{
d_glGenRenderbuffers_origin(context, n, renderbuffers);

}


void d_glGenTextures(void *context, GLsizei n, GLuint* textures)
{
d_glGenTextures_origin(context, n, textures);

}


void d_glGenQueries(void *context, GLsizei n, GLuint* ids)
{
d_glGenQueries_origin(context, n, ids);

}


void d_glGenSamplers(void *context, GLsizei count, GLuint* samplers)
{
d_glGenSamplers_origin(context, count, samplers);

}


void d_glGenTransformFeedbacks(void *context, GLsizei n, GLuint* ids)
{
d_glGenTransformFeedbacks_origin(context, n, ids);

}


void d_glGenProgramPipelines(void *context, GLsizei n, GLuint* pipelines)
{
d_glGenProgramPipelines_origin(context, n, pipelines);

}


GLuint d_glCreateProgram(void *context)
{
return d_glCreateProgram_origin(context);

}


GLuint d_glCreateShader(void *context, GLenum type)
{
return d_glCreateShader_origin(context, type);

}


