

#ifndef DEFINE_GL_H
#define DEFINE_GL_H

//android
#include <GLES/glplatform.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2platform.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>


# define EXPRESS_GPU_FUN_ID ((unsigned long long)1)


#define MAX_OUT_BUF_LEN 4096

#define FUNID_glClientWaitSync ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+1)
GLenum r_glClientWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);
GLenum d_glClientWaitSync(void *context, GLsyncconst* sync, GLbitfieldconst* flags, GLuint64const* timeout);

#define FUNID_glTestInt1 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+2)
GLint r_glTestInt1(void *context, GLint a, GLuint b);
GLint d_glTestInt1(void *context, GLintconst* a, GLuintconst* b);

#define FUNID_glTestInt2 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+3)
GLuint r_glTestInt2(void *context, GLint a, GLuint b);
GLuint d_glTestInt2(void *context, GLintconst* a, GLuintconst* b);

#define FUNID_glTestInt3 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+4)
GLint64 r_glTestInt3(void *context, GLint64 a, GLuint64 b);
GLint64 d_glTestInt3(void *context, GLint64const* a, GLuint64const* b);

#define FUNID_glTestInt4 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+5)
GLuint64 r_glTestInt4(void *context, GLint64 a, GLuint64 b);
GLuint64 d_glTestInt4(void *context, GLint64const* a, GLuint64const* b);

#define FUNID_glTestInt5 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+6)
GLfloat r_glTestInt5(void *context, GLint a, GLuint b);
GLfloat d_glTestInt5(void *context, GLintconst* a, GLuintconst* b);

#define FUNID_glTestInt6 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+7)
GLdouble r_glTestInt6(void *context, GLint a, GLuint b);
GLdouble d_glTestInt6(void *context, GLintconst* a, GLuintconst* b);

#define FUNID_glTestPointer1 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+8)
void r_glTestPointer1(void *context, GLint a, const GLint* b);
void d_glTestPointer1(void *context, GLintconst* a, const GLint*const* b);

#define FUNID_glTestPointer2 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+9)
void r_glTestPointer2(void *context, GLint a, const GLint* b, GLint* c);
void d_glTestPointer2(void *context, GLintconst* a, const GLint*const* b, GLint*const* c);

#define FUNID_glTestPointer4 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+10)
GLint r_glTestPointer4(void *context, GLint a, const GLint* b, GLint* c);
GLint d_glTestPointer4(void *context, GLintconst* a, const GLint*const* b, GLint*const* c);

#define FUNID_glTestString ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+11)
void r_glTestString(void *context, GLint a, GLint count, const GLchar*const* strings, GLint buf_len, GLchar* char_buf);
void d_glTestString(void *context, GLintconst* a, GLintconst* count, const GLchar*const* strings, GLintconst* buf_len, GLchar*const* char_buf);

#define FUNID_glIsBuffer ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+12)
GLboolean r_glIsBuffer(void *context, GLuint buffer);
GLboolean d_glIsBuffer(void *context, GLuintconst* buffer);

#define FUNID_glIsEnabled ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+13)
GLboolean r_glIsEnabled(void *context, GLenum cap);
GLboolean d_glIsEnabled(void *context, GLenumconst* cap);

#define FUNID_glIsFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+14)
GLboolean r_glIsFramebuffer(void *context, GLuint framebuffer);
GLboolean d_glIsFramebuffer(void *context, GLuintconst* framebuffer);

#define FUNID_glIsProgram ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+15)
GLboolean r_glIsProgram(void *context, GLuint program);
GLboolean d_glIsProgram(void *context, GLuintconst* program);

#define FUNID_glIsRenderbuffer ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+16)
GLboolean r_glIsRenderbuffer(void *context, GLuint renderbuffer);
GLboolean d_glIsRenderbuffer(void *context, GLuintconst* renderbuffer);

#define FUNID_glIsShader ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+17)
GLboolean r_glIsShader(void *context, GLuint shader);
GLboolean d_glIsShader(void *context, GLuintconst* shader);

#define FUNID_glIsTexture ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+18)
GLboolean r_glIsTexture(void *context, GLuint texture);
GLboolean d_glIsTexture(void *context, GLuintconst* texture);

#define FUNID_glIsQuery ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+19)
GLboolean r_glIsQuery(void *context, GLuint id);
GLboolean d_glIsQuery(void *context, GLuintconst* id);

#define FUNID_glIsVertexArray ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+20)
GLboolean r_glIsVertexArray(void *context, GLuint array);
GLboolean d_glIsVertexArray(void *context, GLuintconst* array);

#define FUNID_glIsSampler ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+21)
GLboolean r_glIsSampler(void *context, GLuint sampler);
GLboolean d_glIsSampler(void *context, GLuintconst* sampler);

#define FUNID_glIsTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+22)
GLboolean r_glIsTransformFeedback(void *context, GLuint id);
GLboolean d_glIsTransformFeedback(void *context, GLuintconst* id);

#define FUNID_glGetError ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+23)
GLenum r_glGetError(void *context);
GLenum d_glGetError(void *context);

#define FUNID_glGetString ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+24)
void r_glGetString(void *context, GLenum name, GLubyte* buffer);
void d_glGetString(void *context, GLenumconst* name, GLubyte*const* buffer);

#define FUNID_glGetStringi ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+25)
void r_glGetStringi(void *context, GLenum name, GLuint index, GLubyte* buffer);
void d_glGetStringi(void *context, GLenumconst* name, GLuintconst* index, GLubyte*const* buffer);

#define FUNID_glCheckFramebufferStatus ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+26)
GLenum r_glCheckFramebufferStatus(void *context, GLenum target);
GLenum d_glCheckFramebufferStatus(void *context, GLenumconst* target);

#define FUNID_glQueryMatrixxOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+27)
GLbitfield r_glQueryMatrixxOES(void *context, GLfixed* mantissa, GLint* exponent);
GLbitfield d_glQueryMatrixxOES(void *context, GLfixed*const* mantissa, GLint*const* exponent);

#define FUNID_glGetFramebufferAttachmentParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+28)
void r_glGetFramebufferAttachmentParameteriv(void *context, GLenum target, GLenum attachment, GLenum pname, GLint* params);
void d_glGetFramebufferAttachmentParameteriv(void *context, GLenumconst* target, GLenumconst* attachment, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetProgramInfoLog ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+29)
void r_glGetProgramInfoLog(void *context, GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void d_glGetProgramInfoLog(void *context, GLuintconst* program, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* infoLog);

#define FUNID_glGetRenderbufferParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+30)
void r_glGetRenderbufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetRenderbufferParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetShaderInfoLog ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+31)
void r_glGetShaderInfoLog(void *context, GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void d_glGetShaderInfoLog(void *context, GLuintconst* shader, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* infoLog);

#define FUNID_glGetShaderPrecisionFormat ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+32)
void r_glGetShaderPrecisionFormat(void *context, GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
void d_glGetShaderPrecisionFormat(void *context, GLenumconst* shadertype, GLenumconst* precisiontype, GLint*const* range, GLint*const* precision);

#define FUNID_glGetShaderSource ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+33)
void r_glGetShaderSource(void *context, GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
void d_glGetShaderSource(void *context, GLuintconst* shader, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* source);

#define FUNID_glGetTexParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+34)
void r_glGetTexParameterfv(void *context, GLenum target, GLenum pname, GLfloat* params);
void d_glGetTexParameterfv(void *context, GLenumconst* target, GLenumconst* pname, GLfloat*const* params);

#define FUNID_glGetTexParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+35)
void r_glGetTexParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetTexParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetQueryiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+36)
void r_glGetQueryiv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetQueryiv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetQueryObjectuiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+37)
void r_glGetQueryObjectuiv(void *context, GLuint id, GLenum pname, GLuint* params);
void d_glGetQueryObjectuiv(void *context, GLuintconst* id, GLenumconst* pname, GLuint*const* params);

#define FUNID_glGetTransformFeedbackVarying ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+38)
void r_glGetTransformFeedbackVarying(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name);
void d_glGetTransformFeedbackVarying(void *context, GLuintconst* program, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLsizei*const* size, GLenum*const* type, GLchar*const* name);

#define FUNID_glGetActiveUniformsiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+39)
void r_glGetActiveUniformsiv(void *context, GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
void d_glGetActiveUniformsiv(void *context, GLuintconst* program, GLsizeiconst* uniformCount, const GLuint*const* uniformIndices, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetActiveUniformBlockiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+40)
void r_glGetActiveUniformBlockiv(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
void d_glGetActiveUniformBlockiv(void *context, GLuintconst* program, GLuintconst* uniformBlockIndex, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetActiveUniformBlockName ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+41)
void r_glGetActiveUniformBlockName(void *context, GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);
void d_glGetActiveUniformBlockName(void *context, GLuintconst* program, GLuintconst* uniformBlockIndex, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* uniformBlockName);

#define FUNID_glGetSamplerParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+42)
void r_glGetSamplerParameteriv(void *context, GLuint sampler, GLenum pname, GLint* params);
void d_glGetSamplerParameteriv(void *context, GLuintconst* sampler, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetSamplerParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+43)
void r_glGetSamplerParameterfv(void *context, GLuint sampler, GLenum pname, GLfloat* params);
void d_glGetSamplerParameterfv(void *context, GLuintconst* sampler, GLenumconst* pname, GLfloat*const* params);

#define FUNID_glGetProgramBinary ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+44)
void r_glGetProgramBinary(void *context, GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary);
void d_glGetProgramBinary(void *context, GLuintconst* program, GLsizeiconst* bufSize, GLsizei*const* length, GLenum*const* binaryFormat, void*const* binary);

#define FUNID_glGetInternalformativ ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+45)
void r_glGetInternalformativ(void *context, GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint* params);
void d_glGetInternalformativ(void *context, GLenumconst* target, GLenumconst* internalformat, GLenumconst* pname, GLsizeiconst* count, GLint*const* params);

#define FUNID_glGetClipPlanexOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+46)
void r_glGetClipPlanexOES(void *context, GLenum plane, GLfixed* equation);
void d_glGetClipPlanexOES(void *context, GLenumconst* plane, GLfixed*const* equation);

#define FUNID_glGetFixedvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+47)
void r_glGetFixedvOES(void *context, GLenum pname, GLfixed* params);
void d_glGetFixedvOES(void *context, GLenumconst* pname, GLfixed*const* params);

#define FUNID_glGetTexEnvxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+48)
void r_glGetTexEnvxvOES(void *context, GLenum target, GLenum pname, GLfixed* params);
void d_glGetTexEnvxvOES(void *context, GLenumconst* target, GLenumconst* pname, GLfixed*const* params);

#define FUNID_glGetTexParameterxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+49)
void r_glGetTexParameterxvOES(void *context, GLenum target, GLenum pname, GLfixed* params);
void d_glGetTexParameterxvOES(void *context, GLenumconst* target, GLenumconst* pname, GLfixed*const* params);

#define FUNID_glGetLightxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+50)
void r_glGetLightxvOES(void *context, GLenum light, GLenum pname, GLfixed* params);
void d_glGetLightxvOES(void *context, GLenumconst* light, GLenumconst* pname, GLfixed*const* params);

#define FUNID_glGetMaterialxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+51)
void r_glGetMaterialxvOES(void *context, GLenum face, GLenum pname, GLfixed* params);
void d_glGetMaterialxvOES(void *context, GLenumconst* face, GLenumconst* pname, GLfixed*const* params);

#define FUNID_glGetTexGenxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+52)
void r_glGetTexGenxvOES(void *context, GLenum coord, GLenum pname, GLfixed* params);
void d_glGetTexGenxvOES(void *context, GLenumconst* coord, GLenumconst* pname, GLfixed*const* params);

#define FUNID_glGetFramebufferParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+53)
void r_glGetFramebufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetFramebufferParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetProgramInterfaceiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+54)
void r_glGetProgramInterfaceiv(void *context, GLuint program, GLenum programInterface, GLenum pname, GLint* params);
void d_glGetProgramInterfaceiv(void *context, GLuintconst* program, GLenumconst* programInterface, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetProgramResourceName ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+55)
void r_glGetProgramResourceName(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name);
void d_glGetProgramResourceName(void *context, GLuintconst* program, GLenumconst* programInterface, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* name);

#define FUNID_glGetProgramResourceiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+56)
void r_glGetProgramResourceiv(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei bufSize, GLsizei* length, GLint* params);
void d_glGetProgramResourceiv(void *context, GLuintconst* program, GLenumconst* programInterface, GLuintconst* index, GLsizeiconst* propCount, const GLenum*const* props, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* params);

#define FUNID_glGetProgramPipelineiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+57)
void r_glGetProgramPipelineiv(void *context, GLuint pipeline, GLenum pname, GLint* params);
void d_glGetProgramPipelineiv(void *context, GLuintconst* pipeline, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetProgramPipelineInfoLog ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+58)
void r_glGetProgramPipelineInfoLog(void *context, GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void d_glGetProgramPipelineInfoLog(void *context, GLuintconst* pipeline, GLsizeiconst* bufSize, GLsizei*const* length, GLchar*const* infoLog);

#define FUNID_glGetMultisamplefv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+59)
void r_glGetMultisamplefv(void *context, GLenum pname, GLuint index, GLfloat* val);
void d_glGetMultisamplefv(void *context, GLenumconst* pname, GLuintconst* index, GLfloat*const* val);

#define FUNID_glGetTexLevelParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+60)
void r_glGetTexLevelParameteriv(void *context, GLenum target, GLint level, GLenum pname, GLint* params);
void d_glGetTexLevelParameteriv(void *context, GLenumconst* target, GLintconst* level, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetTexLevelParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+61)
void r_glGetTexLevelParameterfv(void *context, GLenum target, GLint level, GLenum pname, GLfloat* params);
void d_glGetTexLevelParameterfv(void *context, GLenumconst* target, GLintconst* level, GLenumconst* pname, GLfloat*const* params);

#define FUNID_glGetSynciv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+62)
void r_glGetSynciv(void *context, GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values);
void d_glGetSynciv(void *context, GLsyncconst* sync, GLenumconst* pname, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* values);

#define FUNID_glGetAttribLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+63)
GLint r_glGetAttribLocation(void *context, GLuint program, const GLchar* name);
GLint d_glGetAttribLocation(void *context, GLuintconst* program, const GLchar*const* name);

#define FUNID_glGetUniformLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+64)
GLint r_glGetUniformLocation(void *context, GLuint program, const GLchar* name);
GLint d_glGetUniformLocation(void *context, GLuintconst* program, const GLchar*const* name);

#define FUNID_glGetFragDataLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+65)
GLint r_glGetFragDataLocation(void *context, GLuint program, const GLchar* name);
GLint d_glGetFragDataLocation(void *context, GLuintconst* program, const GLchar*const* name);

#define FUNID_glGetUniformBlockIndex ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+66)
GLuint r_glGetUniformBlockIndex(void *context, GLuint program, const GLchar* uniformBlockName);
GLuint d_glGetUniformBlockIndex(void *context, GLuintconst* program, const GLchar*const* uniformBlockName);

#define FUNID_glGetProgramResourceIndex ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+67)
GLuint r_glGetProgramResourceIndex(void *context, GLuint program, GLenum programInterface, const GLchar* name);
GLuint d_glGetProgramResourceIndex(void *context, GLuintconst* program, GLenumconst* programInterface, const GLchar*const* name);

#define FUNID_glGetProgramResourceLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+68)
GLint r_glGetProgramResourceLocation(void *context, GLuint program, GLenum programInterface, const GLchar* name);
GLint d_glGetProgramResourceLocation(void *context, GLuintconst* program, GLenumconst* programInterface, const GLchar*const* name);

#define FUNID_glGetActiveAttrib ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+69)
void r_glGetActiveAttrib(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
void d_glGetActiveAttrib(void *context, GLuintconst* program, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* size, GLenum*const* type, GLchar*const* name);

#define FUNID_glGetActiveUniform ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+70)
void r_glGetActiveUniform(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
void d_glGetActiveUniform(void *context, GLuintconst* program, GLuintconst* index, GLsizeiconst* bufSize, GLsizei*const* length, GLint*const* size, GLenum*const* type, GLchar*const* name);

#define FUNID_glGetAttachedShaders ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+71)
void r_glGetAttachedShaders(void *context, GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
void d_glGetAttachedShaders(void *context, GLuintconst* program, GLsizeiconst* maxCount, GLsizei*const* count, GLuint*const* shaders);

#define FUNID_glGetProgramiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+72)
void r_glGetProgramiv(void *context, GLuint program, GLenum pname, GLint* params);
void d_glGetProgramiv(void *context, GLuintconst* program, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetShaderiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+73)
void r_glGetShaderiv(void *context, GLuint shader, GLenum pname, GLint* params);
void d_glGetShaderiv(void *context, GLuintconst* shader, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetUniformfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+74)
void r_glGetUniformfv(void *context, GLuint program, GLint location, GLfloat* params);
void d_glGetUniformfv(void *context, GLuintconst* program, GLintconst* location, GLfloat*const* params);

#define FUNID_glGetUniformiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+75)
void r_glGetUniformiv(void *context, GLuint program, GLint location, GLint* params);
void d_glGetUniformiv(void *context, GLuintconst* program, GLintconst* location, GLint*const* params);

#define FUNID_glGetUniformuiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+76)
void r_glGetUniformuiv(void *context, GLuint program, GLint location, GLuint* params);
void d_glGetUniformuiv(void *context, GLuintconst* program, GLintconst* location, GLuint*const* params);

#define FUNID_glGetUniformIndices ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+77)
void r_glGetUniformIndices(void *context, GLuint program, GLsizei uniformCount, const GLchar*const* uniformNames, GLuint* uniformIndices);
void d_glGetUniformIndices(void *context, GLuintconst* program, GLsizeiconst* uniformCount, const GLchar*const* uniformNames, GLuint*const* uniformIndices);

#define FUNID_glGetVertexAttribfv_origin ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+78)
void r_glGetVertexAttribfv_origin(void *context, GLuint index, GLenum pname, GLfloat* params);
void d_glGetVertexAttribfv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLfloat*const* params);

#define FUNID_glGetVertexAttribiv_origin ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+79)
void r_glGetVertexAttribiv_origin(void *context, GLuint index, GLenum pname, GLint* params);
void d_glGetVertexAttribiv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetVertexAttribIiv_origin ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+80)
void r_glGetVertexAttribIiv_origin(void *context, GLuint index, GLenum pname, GLint* params);
void d_glGetVertexAttribIiv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetVertexAttribIuiv_origin ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+81)
void r_glGetVertexAttribIuiv_origin(void *context, GLuint index, GLenum pname, GLuint* params);
void d_glGetVertexAttribIuiv_origin(void *context, GLuintconst* index, GLenumconst* pname, GLuint*const* params);

#define FUNID_glGetBufferParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+82)
void r_glGetBufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetBufferParameteriv(void *context, GLenumconst* target, GLenumconst* pname, GLint*const* params);

#define FUNID_glGetBufferParameteri64v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+83)
void r_glGetBufferParameteri64v(void *context, GLenum target, GLenum pname, GLint64* params);
void d_glGetBufferParameteri64v(void *context, GLenumconst* target, GLenumconst* pname, GLint64*const* params);

#define FUNID_glGetBooleanv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+84)
void r_glGetBooleanv(void *context, GLenum pname, GLboolean* data);
void d_glGetBooleanv(void *context, GLenumconst* pname, GLboolean*const* data);

#define FUNID_glGetBooleani_v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+85)
void r_glGetBooleani_v(void *context, GLenum target, GLuint index, GLboolean* data);
void d_glGetBooleani_v(void *context, GLenumconst* target, GLuintconst* index, GLboolean*const* data);

#define FUNID_glGetFloatv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+86)
void r_glGetFloatv(void *context, GLenum pname, GLfloat* data);
void d_glGetFloatv(void *context, GLenumconst* pname, GLfloat*const* data);

#define FUNID_glGetIntegerv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+87)
void r_glGetIntegerv(void *context, GLenum pname, GLint* data);
void d_glGetIntegerv(void *context, GLenumconst* pname, GLint*const* data);

#define FUNID_glGetIntegeri_v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+88)
void r_glGetIntegeri_v(void *context, GLenum target, GLuint index, GLint* data);
void d_glGetIntegeri_v(void *context, GLenumconst* target, GLuintconst* index, GLint*const* data);

#define FUNID_glGetInteger64v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+89)
void r_glGetInteger64v(void *context, GLenum pname, GLint64* data);
void d_glGetInteger64v(void *context, GLenumconst* pname, GLint64*const* data);

#define FUNID_glGetInteger64i_v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+90)
void r_glGetInteger64i_v(void *context, GLenum target, GLuint index, GLint64* data);
void d_glGetInteger64i_v(void *context, GLenumconst* target, GLuintconst* index, GLint64*const* data);

#define FUNID_glMapBufferRange_read ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+91)
void r_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void* mem_buf);
void d_glMapBufferRange_read(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* length, GLbitfieldconst* access, void*const* mem_buf);

#define FUNID_glReadPixels_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+92)
void r_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void* pixels);
void d_glReadPixels_without_bound(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, void*const* pixels);

#define FUNID_glTestPointer3 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+93)
GLint r_glTestPointer3(void *context, GLint a, const GLint* b, GLint* c);
GLint d_glTestPointer3(void *context, GLintconst* a, const GLint*const* b, GLint*const* c);

#define FUNID_glFlush ((EXPRESS_GPU_FUN_ID<<32u)+94)
void r_glFlush(void *context);
void d_glFlush(void *context);

#define FUNID_glFinish ((EXPRESS_GPU_FUN_ID<<32u)+95)
void r_glFinish(void *context);
void d_glFinish(void *context);

#define FUNID_glBeginQuery ((EXPRESS_GPU_FUN_ID<<32u)+96)
void r_glBeginQuery(void *context, GLenum target, GLuint id);
void d_glBeginQuery(void *context, GLenumconst* target, GLuintconst* id);

#define FUNID_glEndQuery ((EXPRESS_GPU_FUN_ID<<32u)+97)
void r_glEndQuery(void *context, GLenum target);
void d_glEndQuery(void *context, GLenumconst* target);

#define FUNID_glViewport ((EXPRESS_GPU_FUN_ID<<32u)+98)
void r_glViewport(void *context, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glViewport(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glTexStorage2D ((EXPRESS_GPU_FUN_ID<<32u)+99)
void r_glTexStorage2D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void d_glTexStorage2D(void *context, GLenumconst* target, GLsizeiconst* levels, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glTexStorage3D ((EXPRESS_GPU_FUN_ID<<32u)+100)
void r_glTexStorage3D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
void d_glTexStorage3D(void *context, GLenumconst* target, GLsizeiconst* levels, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth);

#define FUNID_glTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+101)
void r_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels);
void d_glTexImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels);

#define FUNID_glTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+102)
void r_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);
void d_glTexSubImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels);

#define FUNID_glTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+103)
void r_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels);
void d_glTexImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels);

#define FUNID_glTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+104)
void r_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels);
void d_glTexSubImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels);

#define FUNID_glReadPixels_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+105)
void r_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);
void d_glReadPixels_with_bound(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintptrconst* pixels);

#define FUNID_glCompressedTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+106)
void r_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data);
void d_glCompressedTexImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLsizeiconst* imageSize, GLintptrconst* data);

#define FUNID_glCompressedTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+107)
void r_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data);
void d_glCompressedTexSubImage2D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLsizeiconst* imageSize, GLintptrconst* data);

#define FUNID_glCompressedTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+108)
void r_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data);
void d_glCompressedTexImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLsizeiconst* imageSize, GLintptrconst* data);

#define FUNID_glCompressedTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+109)
void r_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data);
void d_glCompressedTexSubImage3D_with_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLsizeiconst* imageSize, GLintptrconst* data);

#define FUNID_glCopyTexImage2D ((EXPRESS_GPU_FUN_ID<<32u)+110)
void r_glCopyTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void d_glCopyTexImage2D(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border);

#define FUNID_glCopyTexSubImage2D ((EXPRESS_GPU_FUN_ID<<32u)+111)
void r_glCopyTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glCopyTexSubImage2D(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glCopyTexSubImage3D ((EXPRESS_GPU_FUN_ID<<32u)+112)
void r_glCopyTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glCopyTexSubImage3D(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glVertexAttribPointer_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+113)
void r_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer);
void d_glVertexAttribPointer_with_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLbooleanconst* normalized, GLsizeiconst* stride, GLintptrconst* pointer);

#define FUNID_glVertexAttribPointer_offset ((EXPRESS_GPU_FUN_ID<<32u)+114)
void r_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset);
void d_glVertexAttribPointer_offset(void *context, GLuintconst* index, GLuintconst* size, GLenumconst* type, GLbooleanconst* normalized, GLsizeiconst* stride, GLuintconst* index_father, GLintptrconst* offset);

#define FUNID_glMapBufferRange_write ((EXPRESS_GPU_FUN_ID<<32u)+115)
void r_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
void d_glMapBufferRange_write(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* length, GLbitfieldconst* access);

#define FUNID_glUnmapBuffer_special ((EXPRESS_GPU_FUN_ID<<32u)+116)
void r_glUnmapBuffer_special(void *context, GLenum target);
void d_glUnmapBuffer_special(void *context, GLenumconst* target);

#define FUNID_glWaitSync ((EXPRESS_GPU_FUN_ID<<32u)+117)
void r_glWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);
void d_glWaitSync(void *context, GLsyncconst* sync, GLbitfieldconst* flags, GLuint64const* timeout);

#define FUNID_glShaderBinary ((EXPRESS_GPU_FUN_ID<<32u)+118)
void r_glShaderBinary(void *context, GLsizei count, const GLuint* shaders, GLenum binaryFormat, const void* binary, GLsizei length);
void d_glShaderBinary(void *context, GLsizeiconst* count, const GLuint*const* shaders, GLenumconst* binaryFormat, const void*const* binary, GLsizeiconst* length);

#define FUNID_glProgramBinary ((EXPRESS_GPU_FUN_ID<<32u)+119)
void r_glProgramBinary(void *context, GLuint program, GLenum binaryFormat, const void* binary, GLsizei length);
void d_glProgramBinary(void *context, GLuintconst* program, GLenumconst* binaryFormat, const void*const* binary, GLsizeiconst* length);

#define FUNID_glDrawBuffers ((EXPRESS_GPU_FUN_ID<<32u)+120)
void r_glDrawBuffers(void *context, GLsizei n, const GLenum* bufs);
void d_glDrawBuffers(void *context, GLsizeiconst* n, const GLenum*const* bufs);

#define FUNID_glDrawArrays_origin ((EXPRESS_GPU_FUN_ID<<32u)+121)
void r_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count);
void d_glDrawArrays_origin(void *context, GLenumconst* mode, GLintconst* first, GLsizeiconst* count);

#define FUNID_glDrawArraysInstanced_origin ((EXPRESS_GPU_FUN_ID<<32u)+122)
void r_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
void d_glDrawArraysInstanced_origin(void *context, GLenumconst* mode, GLintconst* first, GLsizeiconst* count, GLsizeiconst* instancecount);

#define FUNID_glDrawElementsInstanced_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+123)
void r_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount);
void d_glDrawElementsInstanced_with_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, GLsizeiptrconst* indices, GLsizeiconst* instancecount);

#define FUNID_glDrawElements_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+124)
void r_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawElements_with_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, GLsizeiptrconst* indices);

#define FUNID_glDrawRangeElements_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+125)
void r_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawRangeElements_with_bound(void *context, GLenumconst* mode, GLuintconst* start, GLuintconst* end, GLsizeiconst* count, GLenumconst* type, GLsizeiptrconst* indices);

#define FUNID_glTestIntAsyn ((EXPRESS_GPU_FUN_ID<<32u)+126)
void r_glTestIntAsyn(void *context, GLint a, GLuint b, GLfloat c, GLdouble d);
void d_glTestIntAsyn(void *context, GLintconst* a, GLuintconst* b, GLfloatconst* c, GLdoubleconst* d);

#define FUNID_glPrintfAsyn ((EXPRESS_GPU_FUN_ID<<32u)+127)
void r_glPrintfAsyn(void *context, GLint a, GLuint size, GLdouble c, const GLchar* out_string);
void d_glPrintfAsyn(void *context, GLintconst* a, GLuintconst* size, GLdoubleconst* c, const GLchar*const* out_string);

#define FUNID_glEGLImageTargetTexture2DOES ((EXPRESS_GPU_FUN_ID<<32u)+128)
void r_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES imageSize);
void d_glEGLImageTargetTexture2DOES(void *context, GLenumconst* target, GLeglImageOESconst* imageSize);

#define FUNID_glEGLImageTargetRenderbufferStorageOES ((EXPRESS_GPU_FUN_ID<<32u)+129)
void r_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image);
void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenumconst* target, GLeglImageOESconst* image);

#define FUNID_glGenBuffers_special ((EXPRESS_GPU_FUN_ID<<32u)+130)
void r_glGenBuffers_special(void *context, GLsizei n, const GLuint* buffers);
void d_glGenBuffers_special(void *context, GLsizeiconst* n, const GLuint*const* buffers);

#define FUNID_glGenRenderbuffers_special ((EXPRESS_GPU_FUN_ID<<32u)+131)
void r_glGenRenderbuffers_special(void *context, GLsizei n, const GLuint* renderbuffers);
void d_glGenRenderbuffers_special(void *context, GLsizeiconst* n, const GLuint*const* renderbuffers);

#define FUNID_glGenTextures_special ((EXPRESS_GPU_FUN_ID<<32u)+132)
void r_glGenTextures_special(void *context, GLsizei n, const GLuint* textures);
void d_glGenTextures_special(void *context, GLsizeiconst* n, const GLuint*const* textures);

#define FUNID_glGenSamplers_special ((EXPRESS_GPU_FUN_ID<<32u)+133)
void r_glGenSamplers_special(void *context, GLsizei count, const GLuint* samplers);
void d_glGenSamplers_special(void *context, GLsizeiconst* count, const GLuint*const* samplers);

#define FUNID_glCreateProgram_special ((EXPRESS_GPU_FUN_ID<<32u)+134)
void r_glCreateProgram_special(void *context, GLuint program);
void d_glCreateProgram_special(void *context, GLuintconst* program);

#define FUNID_glCreateShader_special ((EXPRESS_GPU_FUN_ID<<32u)+135)
void r_glCreateShader_special(void *context, GLenum type, GLuint shader);
void d_glCreateShader_special(void *context, GLenumconst* type, GLuintconst* shader);

#define FUNID_glFenceSync_special ((EXPRESS_GPU_FUN_ID<<32u)+136)
void r_glFenceSync_special(void *context, GLenum condition, GLbitfield flags, GLsync sync);
void d_glFenceSync_special(void *context, GLenumconst* condition, GLbitfieldconst* flags, GLsyncconst* sync);

#define FUNID_glCreateShaderProgramv ((EXPRESS_GPU_FUN_ID<<32u)+137)
void r_glCreateShaderProgramv(void *context, GLenum type, GLsizei count, const GLchar*const* strings, GLuint program);
void d_glCreateShaderProgramv(void *context, GLenumconst* type, GLsizeiconst* count, const GLchar*const* strings, GLuintconst* program);

#define FUNID_glGenFramebuffers_special ((EXPRESS_GPU_FUN_ID<<32u)+138)
void r_glGenFramebuffers_special(void *context, GLsizei n, const GLuint* framebuffers);
void d_glGenFramebuffers_special(void *context, GLsizeiconst* n, const GLuint*const* framebuffers);

#define FUNID_glGenProgramPipelines_special ((EXPRESS_GPU_FUN_ID<<32u)+139)
void r_glGenProgramPipelines_special(void *context, GLsizei n, const GLuint* pipelines);
void d_glGenProgramPipelines_special(void *context, GLsizeiconst* n, const GLuint*const* pipelines);

#define FUNID_glGenTransformFeedbacks_special ((EXPRESS_GPU_FUN_ID<<32u)+140)
void r_glGenTransformFeedbacks_special(void *context, GLsizei n, const GLuint* ids);
void d_glGenTransformFeedbacks_special(void *context, GLsizeiconst* n, const GLuint*const* ids);

#define FUNID_glGenVertexArrays_special ((EXPRESS_GPU_FUN_ID<<32u)+141)
void r_glGenVertexArrays_special(void *context, GLsizei n, const GLuint* arrays);
void d_glGenVertexArrays_special(void *context, GLsizeiconst* n, const GLuint*const* arrays);

#define FUNID_glGenQueries_special ((EXPRESS_GPU_FUN_ID<<32u)+142)
void r_glGenQueries_special(void *context, GLsizei n, const GLuint* ids);
void d_glGenQueries_special(void *context, GLsizeiconst* n, const GLuint*const* ids);

#define FUNID_glDeleteBuffers_origin ((EXPRESS_GPU_FUN_ID<<32u)+143)
void r_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint* buffers);
void d_glDeleteBuffers_origin(void *context, GLsizeiconst* n, const GLuint*const* buffers);

#define FUNID_glDeleteRenderbuffers ((EXPRESS_GPU_FUN_ID<<32u)+144)
void r_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint* renderbuffers);
void d_glDeleteRenderbuffers(void *context, GLsizeiconst* n, const GLuint*const* renderbuffers);

#define FUNID_glDeleteTextures ((EXPRESS_GPU_FUN_ID<<32u)+145)
void r_glDeleteTextures(void *context, GLsizei n, const GLuint* textures);
void d_glDeleteTextures(void *context, GLsizeiconst* n, const GLuint*const* textures);

#define FUNID_glDeleteSamplers ((EXPRESS_GPU_FUN_ID<<32u)+146)
void r_glDeleteSamplers(void *context, GLsizei count, const GLuint* samplers);
void d_glDeleteSamplers(void *context, GLsizeiconst* count, const GLuint*const* samplers);

#define FUNID_glDeleteProgram_origin ((EXPRESS_GPU_FUN_ID<<32u)+147)
void r_glDeleteProgram_origin(void *context, GLuint program);
void d_glDeleteProgram_origin(void *context, GLuintconst* program);

#define FUNID_glDeleteShader ((EXPRESS_GPU_FUN_ID<<32u)+148)
void r_glDeleteShader(void *context, GLuint shader);
void d_glDeleteShader(void *context, GLuintconst* shader);

#define FUNID_glDeleteSync ((EXPRESS_GPU_FUN_ID<<32u)+149)
void r_glDeleteSync(void *context, GLsync sync);
void d_glDeleteSync(void *context, GLsyncconst* sync);

#define FUNID_glDeleteFramebuffers ((EXPRESS_GPU_FUN_ID<<32u)+150)
void r_glDeleteFramebuffers(void *context, GLsizei n, const GLuint* framebuffers);
void d_glDeleteFramebuffers(void *context, GLsizeiconst* n, const GLuint*const* framebuffers);

#define FUNID_glDeleteProgramPipelines ((EXPRESS_GPU_FUN_ID<<32u)+151)
void r_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint* pipelines);
void d_glDeleteProgramPipelines(void *context, GLsizeiconst* n, const GLuint*const* pipelines);

#define FUNID_glDeleteTransformFeedbacks ((EXPRESS_GPU_FUN_ID<<32u)+152)
void r_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint* ids);
void d_glDeleteTransformFeedbacks(void *context, GLsizeiconst* n, const GLuint*const* ids);

#define FUNID_glDeleteVertexArrays_origin ((EXPRESS_GPU_FUN_ID<<32u)+153)
void r_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint* arrays);
void d_glDeleteVertexArrays_origin(void *context, GLsizeiconst* n, const GLuint*const* arrays);

#define FUNID_glDeleteQueries ((EXPRESS_GPU_FUN_ID<<32u)+154)
void r_glDeleteQueries(void *context, GLsizei n, const GLuint* ids);
void d_glDeleteQueries(void *context, GLsizeiconst* n, const GLuint*const* ids);

#define FUNID_glLinkProgram_origin ((EXPRESS_GPU_FUN_ID<<32u)+155)
void r_glLinkProgram_origin(void *context, GLuint program);
void d_glLinkProgram_origin(void *context, GLuintconst* program);

#define FUNID_glPixelStorei_origin ((EXPRESS_GPU_FUN_ID<<32u)+156)
void r_glPixelStorei_origin(void *context, GLenum pname, GLint param);
void d_glPixelStorei_origin(void *context, GLenumconst* pname, GLintconst* param);

#define FUNID_glDisableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID<<32u)+157)
void r_glDisableVertexAttribArray_origin(void *context, GLuint index);
void d_glDisableVertexAttribArray_origin(void *context, GLuintconst* index);

#define FUNID_glEnableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID<<32u)+158)
void r_glEnableVertexAttribArray_origin(void *context, GLuint index);
void d_glEnableVertexAttribArray_origin(void *context, GLuintconst* index);

#define FUNID_glReadBuffer_special ((EXPRESS_GPU_FUN_ID<<32u)+159)
void r_glReadBuffer_special(void *context, GLenum src);
void d_glReadBuffer_special(void *context, GLenumconst* src);

#define FUNID_glVertexAttribDivisor_origin ((EXPRESS_GPU_FUN_ID<<32u)+160)
void r_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor);
void d_glVertexAttribDivisor_origin(void *context, GLuintconst* index, GLuintconst* divisor);

#define FUNID_glShaderSource_origin ((EXPRESS_GPU_FUN_ID<<32u)+161)
void r_glShaderSource_origin(void *context, GLuint shader, GLsizei count, const GLint* length, const GLchar*const* string);
void d_glShaderSource_origin(void *context, GLuintconst* shader, GLsizeiconst* count, const GLint*const* length, const GLchar*const* string);

#define FUNID_glVertexAttribIPointer_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+162)
void r_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer);
void d_glVertexAttribIPointer_with_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLsizeiconst* stride, GLintptrconst* pointer);

#define FUNID_glVertexAttribIPointer_offset ((EXPRESS_GPU_FUN_ID<<32u)+163)
void r_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset);
void d_glVertexAttribIPointer_offset(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLsizeiconst* stride, GLuintconst* index_father, GLintptrconst* offset);

#define FUNID_glBindVertexArray_special ((EXPRESS_GPU_FUN_ID<<32u)+164)
void r_glBindVertexArray_special(void *context, GLuint array);
void d_glBindVertexArray_special(void *context, GLuintconst* array);

#define FUNID_glBindBuffer_origin ((EXPRESS_GPU_FUN_ID<<32u)+165)
void r_glBindBuffer_origin(void *context, GLenum target, GLuint buffer);
void d_glBindBuffer_origin(void *context, GLenumconst* target, GLuintconst* buffer);

#define FUNID_glBeginTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+166)
void r_glBeginTransformFeedback(void *context, GLenum primitiveMode);
void d_glBeginTransformFeedback(void *context, GLenumconst* primitiveMode);

#define FUNID_glEndTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+167)
void r_glEndTransformFeedback(void *context);
void d_glEndTransformFeedback(void *context);

#define FUNID_glPauseTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+168)
void r_glPauseTransformFeedback(void *context);
void d_glPauseTransformFeedback(void *context);

#define FUNID_glResumeTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+169)
void r_glResumeTransformFeedback(void *context);
void d_glResumeTransformFeedback(void *context);

#define FUNID_glBindBufferRange ((EXPRESS_GPU_FUN_ID<<32u)+170)
void r_glBindBufferRange(void *context, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
void d_glBindBufferRange(void *context, GLenumconst* target, GLuintconst* index, GLuintconst* buffer, GLintptrconst* offset, GLsizeiptrconst* size);

#define FUNID_glBindBufferBase ((EXPRESS_GPU_FUN_ID<<32u)+171)
void r_glBindBufferBase(void *context, GLenum target, GLuint index, GLuint buffer);
void d_glBindBufferBase(void *context, GLenumconst* target, GLuintconst* index, GLuintconst* buffer);

#define FUNID_glBindTexture ((EXPRESS_GPU_FUN_ID<<32u)+172)
void r_glBindTexture(void *context, GLenum target, GLuint texture);
void d_glBindTexture(void *context, GLenumconst* target, GLuintconst* texture);

#define FUNID_glBindRenderbuffer ((EXPRESS_GPU_FUN_ID<<32u)+173)
void r_glBindRenderbuffer(void *context, GLenum target, GLuint renderbuffer);
void d_glBindRenderbuffer(void *context, GLenumconst* target, GLuintconst* renderbuffer);

#define FUNID_glBindSampler ((EXPRESS_GPU_FUN_ID<<32u)+174)
void r_glBindSampler(void *context, GLuint unit, GLuint sampler);
void d_glBindSampler(void *context, GLuintconst* unit, GLuintconst* sampler);

#define FUNID_glBindFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+175)
void r_glBindFramebuffer(void *context, GLenum target, GLuint framebuffer);
void d_glBindFramebuffer(void *context, GLenumconst* target, GLuintconst* framebuffer);

#define FUNID_glBindProgramPipeline ((EXPRESS_GPU_FUN_ID<<32u)+176)
void r_glBindProgramPipeline(void *context, GLuint pipeline);
void d_glBindProgramPipeline(void *context, GLuintconst* pipeline);

#define FUNID_glBindTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+177)
void r_glBindTransformFeedback(void *context, GLenum target, GLuint id);
void d_glBindTransformFeedback(void *context, GLenumconst* target, GLuintconst* id);

#define FUNID_glSamplerParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+178)
void r_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint* param);
void d_glSamplerParameteriv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLint*const* param);

#define FUNID_glSamplerParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+179)
void r_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat* param);
void d_glSamplerParameterfv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLfloat*const* param);

#define FUNID_glActiveTexture ((EXPRESS_GPU_FUN_ID<<32u)+180)
void r_glActiveTexture(void *context, GLenum texture);
void d_glActiveTexture(void *context, GLenumconst* texture);

#define FUNID_glAttachShader ((EXPRESS_GPU_FUN_ID<<32u)+181)
void r_glAttachShader(void *context, GLuint program, GLuint shader);
void d_glAttachShader(void *context, GLuintconst* program, GLuintconst* shader);

#define FUNID_glBlendColor ((EXPRESS_GPU_FUN_ID<<32u)+182)
void r_glBlendColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void d_glBlendColor(void *context, GLfloatconst* red, GLfloatconst* green, GLfloatconst* blue, GLfloatconst* alpha);

#define FUNID_glBlendEquation ((EXPRESS_GPU_FUN_ID<<32u)+183)
void r_glBlendEquation(void *context, GLenum mode);
void d_glBlendEquation(void *context, GLenumconst* mode);

#define FUNID_glBlendEquationSeparate ((EXPRESS_GPU_FUN_ID<<32u)+184)
void r_glBlendEquationSeparate(void *context, GLenum modeRGB, GLenum modeAlpha);
void d_glBlendEquationSeparate(void *context, GLenumconst* modeRGB, GLenumconst* modeAlpha);

#define FUNID_glBlendFunc ((EXPRESS_GPU_FUN_ID<<32u)+185)
void r_glBlendFunc(void *context, GLenum sfactor, GLenum dfactor);
void d_glBlendFunc(void *context, GLenumconst* sfactor, GLenumconst* dfactor);

#define FUNID_glBlendFuncSeparate ((EXPRESS_GPU_FUN_ID<<32u)+186)
void r_glBlendFuncSeparate(void *context, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
void d_glBlendFuncSeparate(void *context, GLenumconst* sfactorRGB, GLenumconst* dfactorRGB, GLenumconst* sfactorAlpha, GLenumconst* dfactorAlpha);

#define FUNID_glClear ((EXPRESS_GPU_FUN_ID<<32u)+187)
void r_glClear(void *context, GLbitfield mask);
void d_glClear(void *context, GLbitfieldconst* mask);

#define FUNID_glClearColor ((EXPRESS_GPU_FUN_ID<<32u)+188)
void r_glClearColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void d_glClearColor(void *context, GLfloatconst* red, GLfloatconst* green, GLfloatconst* blue, GLfloatconst* alpha);

#define FUNID_glClearDepthf ((EXPRESS_GPU_FUN_ID<<32u)+189)
void r_glClearDepthf(void *context, GLfloat d);
void d_glClearDepthf(void *context, GLfloatconst* d);

#define FUNID_glClearStencil ((EXPRESS_GPU_FUN_ID<<32u)+190)
void r_glClearStencil(void *context, GLint s);
void d_glClearStencil(void *context, GLintconst* s);

#define FUNID_glColorMask ((EXPRESS_GPU_FUN_ID<<32u)+191)
void r_glColorMask(void *context, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void d_glColorMask(void *context, GLbooleanconst* red, GLbooleanconst* green, GLbooleanconst* blue, GLbooleanconst* alpha);

#define FUNID_glCompileShader ((EXPRESS_GPU_FUN_ID<<32u)+192)
void r_glCompileShader(void *context, GLuint shader);
void d_glCompileShader(void *context, GLuintconst* shader);

#define FUNID_glCullFace ((EXPRESS_GPU_FUN_ID<<32u)+193)
void r_glCullFace(void *context, GLenum mode);
void d_glCullFace(void *context, GLenumconst* mode);

#define FUNID_glDepthFunc ((EXPRESS_GPU_FUN_ID<<32u)+194)
void r_glDepthFunc(void *context, GLenum func);
void d_glDepthFunc(void *context, GLenumconst* func);

#define FUNID_glDepthMask ((EXPRESS_GPU_FUN_ID<<32u)+195)
void r_glDepthMask(void *context, GLboolean flag);
void d_glDepthMask(void *context, GLbooleanconst* flag);

#define FUNID_glDepthRangef ((EXPRESS_GPU_FUN_ID<<32u)+196)
void r_glDepthRangef(void *context, GLfloat n, GLfloat f);
void d_glDepthRangef(void *context, GLfloatconst* n, GLfloatconst* f);

#define FUNID_glDetachShader ((EXPRESS_GPU_FUN_ID<<32u)+197)
void r_glDetachShader(void *context, GLuint program, GLuint shader);
void d_glDetachShader(void *context, GLuintconst* program, GLuintconst* shader);

#define FUNID_glDisable ((EXPRESS_GPU_FUN_ID<<32u)+198)
void r_glDisable(void *context, GLenum cap);
void d_glDisable(void *context, GLenumconst* cap);

#define FUNID_glEnable ((EXPRESS_GPU_FUN_ID<<32u)+199)
void r_glEnable(void *context, GLenum cap);
void d_glEnable(void *context, GLenumconst* cap);

#define FUNID_glFramebufferRenderbuffer ((EXPRESS_GPU_FUN_ID<<32u)+200)
void r_glFramebufferRenderbuffer(void *context, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void d_glFramebufferRenderbuffer(void *context, GLenumconst* target, GLenumconst* attachment, GLenumconst* renderbuffertarget, GLuintconst* renderbuffer);

#define FUNID_glFramebufferTexture2D ((EXPRESS_GPU_FUN_ID<<32u)+201)
void r_glFramebufferTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void d_glFramebufferTexture2D(void *context, GLenumconst* target, GLenumconst* attachment, GLenumconst* textarget, GLuintconst* texture, GLintconst* level);

#define FUNID_glFrontFace ((EXPRESS_GPU_FUN_ID<<32u)+202)
void r_glFrontFace(void *context, GLenum mode);
void d_glFrontFace(void *context, GLenumconst* mode);

#define FUNID_glGenerateMipmap ((EXPRESS_GPU_FUN_ID<<32u)+203)
void r_glGenerateMipmap(void *context, GLenum target);
void d_glGenerateMipmap(void *context, GLenumconst* target);

#define FUNID_glHint ((EXPRESS_GPU_FUN_ID<<32u)+204)
void r_glHint(void *context, GLenum target, GLenum mode);
void d_glHint(void *context, GLenumconst* target, GLenumconst* mode);

#define FUNID_glLineWidth ((EXPRESS_GPU_FUN_ID<<32u)+205)
void r_glLineWidth(void *context, GLfloat width);
void d_glLineWidth(void *context, GLfloatconst* width);

#define FUNID_glPolygonOffset ((EXPRESS_GPU_FUN_ID<<32u)+206)
void r_glPolygonOffset(void *context, GLfloat factor, GLfloat units);
void d_glPolygonOffset(void *context, GLfloatconst* factor, GLfloatconst* units);

#define FUNID_glReleaseShaderCompiler ((EXPRESS_GPU_FUN_ID<<32u)+207)
void r_glReleaseShaderCompiler(void *context);
void d_glReleaseShaderCompiler(void *context);

#define FUNID_glRenderbufferStorage ((EXPRESS_GPU_FUN_ID<<32u)+208)
void r_glRenderbufferStorage(void *context, GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
void d_glRenderbufferStorage(void *context, GLenumconst* target, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glSampleCoverage ((EXPRESS_GPU_FUN_ID<<32u)+209)
void r_glSampleCoverage(void *context, GLfloat value, GLboolean invert);
void d_glSampleCoverage(void *context, GLfloatconst* value, GLbooleanconst* invert);

#define FUNID_glScissor ((EXPRESS_GPU_FUN_ID<<32u)+210)
void r_glScissor(void *context, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glScissor(void *context, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glStencilFunc ((EXPRESS_GPU_FUN_ID<<32u)+211)
void r_glStencilFunc(void *context, GLenum func, GLint ref, GLuint mask);
void d_glStencilFunc(void *context, GLenumconst* func, GLintconst* ref, GLuintconst* mask);

#define FUNID_glStencilFuncSeparate ((EXPRESS_GPU_FUN_ID<<32u)+212)
void r_glStencilFuncSeparate(void *context, GLenum face, GLenum func, GLint ref, GLuint mask);
void d_glStencilFuncSeparate(void *context, GLenumconst* face, GLenumconst* func, GLintconst* ref, GLuintconst* mask);

#define FUNID_glStencilMask ((EXPRESS_GPU_FUN_ID<<32u)+213)
void r_glStencilMask(void *context, GLuint mask);
void d_glStencilMask(void *context, GLuintconst* mask);

#define FUNID_glStencilMaskSeparate ((EXPRESS_GPU_FUN_ID<<32u)+214)
void r_glStencilMaskSeparate(void *context, GLenum face, GLuint mask);
void d_glStencilMaskSeparate(void *context, GLenumconst* face, GLuintconst* mask);

#define FUNID_glStencilOp ((EXPRESS_GPU_FUN_ID<<32u)+215)
void r_glStencilOp(void *context, GLenum fail, GLenum zfail, GLenum zpass);
void d_glStencilOp(void *context, GLenumconst* fail, GLenumconst* zfail, GLenumconst* zpass);

#define FUNID_glStencilOpSeparate ((EXPRESS_GPU_FUN_ID<<32u)+216)
void r_glStencilOpSeparate(void *context, GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
void d_glStencilOpSeparate(void *context, GLenumconst* face, GLenumconst* sfail, GLenumconst* dpfail, GLenumconst* dppass);

#define FUNID_glTexParameterf ((EXPRESS_GPU_FUN_ID<<32u)+217)
void r_glTexParameterf(void *context, GLenum target, GLenum pname, GLfloat param);
void d_glTexParameterf(void *context, GLenumconst* target, GLenumconst* pname, GLfloatconst* param);

#define FUNID_glTexParameteri ((EXPRESS_GPU_FUN_ID<<32u)+218)
void r_glTexParameteri(void *context, GLenum target, GLenum pname, GLint param);
void d_glTexParameteri(void *context, GLenumconst* target, GLenumconst* pname, GLintconst* param);

#define FUNID_glUniform1f ((EXPRESS_GPU_FUN_ID<<32u)+219)
void r_glUniform1f(void *context, GLint location, GLfloat v0);
void d_glUniform1f(void *context, GLintconst* location, GLfloatconst* v0);

#define FUNID_glUniform1i ((EXPRESS_GPU_FUN_ID<<32u)+220)
void r_glUniform1i(void *context, GLint location, GLint v0);
void d_glUniform1i(void *context, GLintconst* location, GLintconst* v0);

#define FUNID_glUniform2f ((EXPRESS_GPU_FUN_ID<<32u)+221)
void r_glUniform2f(void *context, GLint location, GLfloat v0, GLfloat v1);
void d_glUniform2f(void *context, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1);

#define FUNID_glUniform2i ((EXPRESS_GPU_FUN_ID<<32u)+222)
void r_glUniform2i(void *context, GLint location, GLint v0, GLint v1);
void d_glUniform2i(void *context, GLintconst* location, GLintconst* v0, GLintconst* v1);

#define FUNID_glUniform3f ((EXPRESS_GPU_FUN_ID<<32u)+223)
void r_glUniform3f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void d_glUniform3f(void *context, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2);

#define FUNID_glUniform3i ((EXPRESS_GPU_FUN_ID<<32u)+224)
void r_glUniform3i(void *context, GLint location, GLint v0, GLint v1, GLint v2);
void d_glUniform3i(void *context, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2);

#define FUNID_glUniform4f ((EXPRESS_GPU_FUN_ID<<32u)+225)
void r_glUniform4f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void d_glUniform4f(void *context, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2, GLfloatconst* v3);

#define FUNID_glUniform4i ((EXPRESS_GPU_FUN_ID<<32u)+226)
void r_glUniform4i(void *context, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void d_glUniform4i(void *context, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2, GLintconst* v3);

#define FUNID_glUseProgram ((EXPRESS_GPU_FUN_ID<<32u)+227)
void r_glUseProgram(void *context, GLuint program);
void d_glUseProgram(void *context, GLuintconst* program);

#define FUNID_glValidateProgram ((EXPRESS_GPU_FUN_ID<<32u)+228)
void r_glValidateProgram(void *context, GLuint program);
void d_glValidateProgram(void *context, GLuintconst* program);

#define FUNID_glVertexAttrib1f ((EXPRESS_GPU_FUN_ID<<32u)+229)
void r_glVertexAttrib1f(void *context, GLuint index, GLfloat x);
void d_glVertexAttrib1f(void *context, GLuintconst* index, GLfloatconst* x);

#define FUNID_glVertexAttrib2f ((EXPRESS_GPU_FUN_ID<<32u)+230)
void r_glVertexAttrib2f(void *context, GLuint index, GLfloat x, GLfloat y);
void d_glVertexAttrib2f(void *context, GLuintconst* index, GLfloatconst* x, GLfloatconst* y);

#define FUNID_glVertexAttrib3f ((EXPRESS_GPU_FUN_ID<<32u)+231)
void r_glVertexAttrib3f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z);
void d_glVertexAttrib3f(void *context, GLuintconst* index, GLfloatconst* x, GLfloatconst* y, GLfloatconst* z);

#define FUNID_glVertexAttrib4f ((EXPRESS_GPU_FUN_ID<<32u)+232)
void r_glVertexAttrib4f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void d_glVertexAttrib4f(void *context, GLuintconst* index, GLfloatconst* x, GLfloatconst* y, GLfloatconst* z, GLfloatconst* w);

#define FUNID_glBlitFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+233)
void r_glBlitFramebuffer(void *context, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
void d_glBlitFramebuffer(void *context, GLintconst* srcX0, GLintconst* srcY0, GLintconst* srcX1, GLintconst* srcY1, GLintconst* dstX0, GLintconst* dstY0, GLintconst* dstX1, GLintconst* dstY1, GLbitfieldconst* mask, GLenumconst* filter);

#define FUNID_glRenderbufferStorageMultisample ((EXPRESS_GPU_FUN_ID<<32u)+234)
void r_glRenderbufferStorageMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
void d_glRenderbufferStorageMultisample(void *context, GLenumconst* target, GLsizeiconst* samples, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glFramebufferTextureLayer ((EXPRESS_GPU_FUN_ID<<32u)+235)
void r_glFramebufferTextureLayer(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
void d_glFramebufferTextureLayer(void *context, GLenumconst* target, GLenumconst* attachment, GLuintconst* texture, GLintconst* level, GLintconst* layer);

#define FUNID_glVertexAttribI4i ((EXPRESS_GPU_FUN_ID<<32u)+236)
void r_glVertexAttribI4i(void *context, GLuint index, GLint x, GLint y, GLint z, GLint w);
void d_glVertexAttribI4i(void *context, GLuintconst* index, GLintconst* x, GLintconst* y, GLintconst* z, GLintconst* w);

#define FUNID_glVertexAttribI4ui ((EXPRESS_GPU_FUN_ID<<32u)+237)
void r_glVertexAttribI4ui(void *context, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
void d_glVertexAttribI4ui(void *context, GLuintconst* index, GLuintconst* x, GLuintconst* y, GLuintconst* z, GLuintconst* w);

#define FUNID_glUniform1ui ((EXPRESS_GPU_FUN_ID<<32u)+238)
void r_glUniform1ui(void *context, GLint location, GLuint v0);
void d_glUniform1ui(void *context, GLintconst* location, GLuintconst* v0);

#define FUNID_glUniform2ui ((EXPRESS_GPU_FUN_ID<<32u)+239)
void r_glUniform2ui(void *context, GLint location, GLuint v0, GLuint v1);
void d_glUniform2ui(void *context, GLintconst* location, GLuintconst* v0, GLuintconst* v1);

#define FUNID_glUniform3ui ((EXPRESS_GPU_FUN_ID<<32u)+240)
void r_glUniform3ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2);
void d_glUniform3ui(void *context, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2);

#define FUNID_glUniform4ui ((EXPRESS_GPU_FUN_ID<<32u)+241)
void r_glUniform4ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
void d_glUniform4ui(void *context, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2, GLuintconst* v3);

#define FUNID_glClearBufferfi ((EXPRESS_GPU_FUN_ID<<32u)+242)
void r_glClearBufferfi(void *context, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
void d_glClearBufferfi(void *context, GLenumconst* buffer, GLintconst* drawbuffer, GLfloatconst* depth, GLintconst* stencil);

#define FUNID_glCopyBufferSubData ((EXPRESS_GPU_FUN_ID<<32u)+243)
void r_glCopyBufferSubData(void *context, GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
void d_glCopyBufferSubData(void *context, GLenumconst* readTarget, GLenumconst* writeTarget, GLintptrconst* readOffset, GLintptrconst* writeOffset, GLsizeiptrconst* size);

#define FUNID_glUniformBlockBinding ((EXPRESS_GPU_FUN_ID<<32u)+244)
void r_glUniformBlockBinding(void *context, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
void d_glUniformBlockBinding(void *context, GLuintconst* program, GLuintconst* uniformBlockIndex, GLuintconst* uniformBlockBinding);

#define FUNID_glSamplerParameteri ((EXPRESS_GPU_FUN_ID<<32u)+245)
void r_glSamplerParameteri(void *context, GLuint sampler, GLenum pname, GLint param);
void d_glSamplerParameteri(void *context, GLuintconst* sampler, GLenumconst* pname, GLintconst* param);

#define FUNID_glSamplerParameterf ((EXPRESS_GPU_FUN_ID<<32u)+246)
void r_glSamplerParameterf(void *context, GLuint sampler, GLenum pname, GLfloat param);
void d_glSamplerParameterf(void *context, GLuintconst* sampler, GLenumconst* pname, GLfloatconst* param);

#define FUNID_glProgramParameteri ((EXPRESS_GPU_FUN_ID<<32u)+247)
void r_glProgramParameteri(void *context, GLuint program, GLenum pname, GLint value);
void d_glProgramParameteri(void *context, GLuintconst* program, GLenumconst* pname, GLintconst* value);

#define FUNID_glAlphaFuncxOES ((EXPRESS_GPU_FUN_ID<<32u)+248)
void r_glAlphaFuncxOES(void *context, GLenum func, GLfixed ref);
void d_glAlphaFuncxOES(void *context, GLenumconst* func, GLfixedconst* ref);

#define FUNID_glClearColorxOES ((EXPRESS_GPU_FUN_ID<<32u)+249)
void r_glClearColorxOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
void d_glClearColorxOES(void *context, GLfixedconst* red, GLfixedconst* green, GLfixedconst* blue, GLfixedconst* alpha);

#define FUNID_glClearDepthxOES ((EXPRESS_GPU_FUN_ID<<32u)+250)
void r_glClearDepthxOES(void *context, GLfixed depth);
void d_glClearDepthxOES(void *context, GLfixedconst* depth);

#define FUNID_glColor4xOES ((EXPRESS_GPU_FUN_ID<<32u)+251)
void r_glColor4xOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
void d_glColor4xOES(void *context, GLfixedconst* red, GLfixedconst* green, GLfixedconst* blue, GLfixedconst* alpha);

#define FUNID_glDepthRangexOES ((EXPRESS_GPU_FUN_ID<<32u)+252)
void r_glDepthRangexOES(void *context, GLfixed n, GLfixed f);
void d_glDepthRangexOES(void *context, GLfixedconst* n, GLfixedconst* f);

#define FUNID_glFogxOES ((EXPRESS_GPU_FUN_ID<<32u)+253)
void r_glFogxOES(void *context, GLenum pname, GLfixed param);
void d_glFogxOES(void *context, GLenumconst* pname, GLfixedconst* param);

#define FUNID_glFrustumxOES ((EXPRESS_GPU_FUN_ID<<32u)+254)
void r_glFrustumxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);
void d_glFrustumxOES(void *context, GLfixedconst* l, GLfixedconst* r, GLfixedconst* b, GLfixedconst* t, GLfixedconst* n, GLfixedconst* f);

#define FUNID_glLightModelxOES ((EXPRESS_GPU_FUN_ID<<32u)+255)
void r_glLightModelxOES(void *context, GLenum pname, GLfixed param);
void d_glLightModelxOES(void *context, GLenumconst* pname, GLfixedconst* param);

#define FUNID_glLightxOES ((EXPRESS_GPU_FUN_ID<<32u)+256)
void r_glLightxOES(void *context, GLenum light, GLenum pname, GLfixed param);
void d_glLightxOES(void *context, GLenumconst* light, GLenumconst* pname, GLfixedconst* param);

#define FUNID_glLineWidthxOES ((EXPRESS_GPU_FUN_ID<<32u)+257)
void r_glLineWidthxOES(void *context, GLfixed width);
void d_glLineWidthxOES(void *context, GLfixedconst* width);

#define FUNID_glMaterialxOES ((EXPRESS_GPU_FUN_ID<<32u)+258)
void r_glMaterialxOES(void *context, GLenum face, GLenum pname, GLfixed param);
void d_glMaterialxOES(void *context, GLenumconst* face, GLenumconst* pname, GLfixedconst* param);

#define FUNID_glMultiTexCoord4xOES ((EXPRESS_GPU_FUN_ID<<32u)+259)
void r_glMultiTexCoord4xOES(void *context, GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
void d_glMultiTexCoord4xOES(void *context, GLenumconst* texture, GLfixedconst* s, GLfixedconst* t, GLfixedconst* r, GLfixedconst* q);

#define FUNID_glNormal3xOES ((EXPRESS_GPU_FUN_ID<<32u)+260)
void r_glNormal3xOES(void *context, GLfixed nx, GLfixed ny, GLfixed nz);
void d_glNormal3xOES(void *context, GLfixedconst* nx, GLfixedconst* ny, GLfixedconst* nz);

#define FUNID_glOrthoxOES ((EXPRESS_GPU_FUN_ID<<32u)+261)
void r_glOrthoxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);
void d_glOrthoxOES(void *context, GLfixedconst* l, GLfixedconst* r, GLfixedconst* b, GLfixedconst* t, GLfixedconst* n, GLfixedconst* f);

#define FUNID_glPointSizexOES ((EXPRESS_GPU_FUN_ID<<32u)+262)
void r_glPointSizexOES(void *context, GLfixed size);
void d_glPointSizexOES(void *context, GLfixedconst* size);

#define FUNID_glPolygonOffsetxOES ((EXPRESS_GPU_FUN_ID<<32u)+263)
void r_glPolygonOffsetxOES(void *context, GLfixed factor, GLfixed units);
void d_glPolygonOffsetxOES(void *context, GLfixedconst* factor, GLfixedconst* units);

#define FUNID_glRotatexOES ((EXPRESS_GPU_FUN_ID<<32u)+264)
void r_glRotatexOES(void *context, GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
void d_glRotatexOES(void *context, GLfixedconst* angle, GLfixedconst* x, GLfixedconst* y, GLfixedconst* z);

#define FUNID_glScalexOES ((EXPRESS_GPU_FUN_ID<<32u)+265)
void r_glScalexOES(void *context, GLfixed x, GLfixed y, GLfixed z);
void d_glScalexOES(void *context, GLfixedconst* x, GLfixedconst* y, GLfixedconst* z);

#define FUNID_glTexEnvxOES ((EXPRESS_GPU_FUN_ID<<32u)+266)
void r_glTexEnvxOES(void *context, GLenum target, GLenum pname, GLfixed param);
void d_glTexEnvxOES(void *context, GLenumconst* target, GLenumconst* pname, GLfixedconst* param);

#define FUNID_glTranslatexOES ((EXPRESS_GPU_FUN_ID<<32u)+267)
void r_glTranslatexOES(void *context, GLfixed x, GLfixed y, GLfixed z);
void d_glTranslatexOES(void *context, GLfixedconst* x, GLfixedconst* y, GLfixedconst* z);

#define FUNID_glPointParameterxOES ((EXPRESS_GPU_FUN_ID<<32u)+268)
void r_glPointParameterxOES(void *context, GLenum pname, GLfixed param);
void d_glPointParameterxOES(void *context, GLenumconst* pname, GLfixedconst* param);

#define FUNID_glSampleCoveragexOES ((EXPRESS_GPU_FUN_ID<<32u)+269)
void r_glSampleCoveragexOES(void *context, GLclampx value, GLboolean invert);
void d_glSampleCoveragexOES(void *context, GLclampxconst* value, GLbooleanconst* invert);

#define FUNID_glTexGenxOES ((EXPRESS_GPU_FUN_ID<<32u)+270)
void r_glTexGenxOES(void *context, GLenum coord, GLenum pname, GLfixed param);
void d_glTexGenxOES(void *context, GLenumconst* coord, GLenumconst* pname, GLfixedconst* param);

#define FUNID_glClearDepthfOES ((EXPRESS_GPU_FUN_ID<<32u)+271)
void r_glClearDepthfOES(void *context, GLclampf depth);
void d_glClearDepthfOES(void *context, GLclampfconst* depth);

#define FUNID_glDepthRangefOES ((EXPRESS_GPU_FUN_ID<<32u)+272)
void r_glDepthRangefOES(void *context, GLclampf n, GLclampf f);
void d_glDepthRangefOES(void *context, GLclampfconst* n, GLclampfconst* f);

#define FUNID_glFrustumfOES ((EXPRESS_GPU_FUN_ID<<32u)+273)
void r_glFrustumfOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
void d_glFrustumfOES(void *context, GLfloatconst* l, GLfloatconst* r, GLfloatconst* b, GLfloatconst* t, GLfloatconst* n, GLfloatconst* f);

#define FUNID_glOrthofOES ((EXPRESS_GPU_FUN_ID<<32u)+274)
void r_glOrthofOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
void d_glOrthofOES(void *context, GLfloatconst* l, GLfloatconst* r, GLfloatconst* b, GLfloatconst* t, GLfloatconst* n, GLfloatconst* f);

#define FUNID_glRenderbufferStorageMultisampleEXT ((EXPRESS_GPU_FUN_ID<<32u)+275)
void r_glRenderbufferStorageMultisampleEXT(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
void d_glRenderbufferStorageMultisampleEXT(void *context, GLenumconst* target, GLsizeiconst* samples, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glUseProgramStages ((EXPRESS_GPU_FUN_ID<<32u)+276)
void r_glUseProgramStages(void *context, GLuint pipeline, GLbitfield stages, GLuint program);
void d_glUseProgramStages(void *context, GLuintconst* pipeline, GLbitfieldconst* stages, GLuintconst* program);

#define FUNID_glActiveShaderProgram ((EXPRESS_GPU_FUN_ID<<32u)+277)
void r_glActiveShaderProgram(void *context, GLuint pipeline, GLuint program);
void d_glActiveShaderProgram(void *context, GLuintconst* pipeline, GLuintconst* program);

#define FUNID_glProgramUniform1i ((EXPRESS_GPU_FUN_ID<<32u)+278)
void r_glProgramUniform1i(void *context, GLuint program, GLint location, GLint v0);
void d_glProgramUniform1i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0);

#define FUNID_glProgramUniform2i ((EXPRESS_GPU_FUN_ID<<32u)+279)
void r_glProgramUniform2i(void *context, GLuint program, GLint location, GLint v0, GLint v1);
void d_glProgramUniform2i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0, GLintconst* v1);

#define FUNID_glProgramUniform3i ((EXPRESS_GPU_FUN_ID<<32u)+280)
void r_glProgramUniform3i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
void d_glProgramUniform3i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2);

#define FUNID_glProgramUniform4i ((EXPRESS_GPU_FUN_ID<<32u)+281)
void r_glProgramUniform4i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void d_glProgramUniform4i(void *context, GLuintconst* program, GLintconst* location, GLintconst* v0, GLintconst* v1, GLintconst* v2, GLintconst* v3);

#define FUNID_glProgramUniform1ui ((EXPRESS_GPU_FUN_ID<<32u)+282)
void r_glProgramUniform1ui(void *context, GLuint program, GLint location, GLuint v0);
void d_glProgramUniform1ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0);

#define FUNID_glProgramUniform2ui ((EXPRESS_GPU_FUN_ID<<32u)+283)
void r_glProgramUniform2ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1);
void d_glProgramUniform2ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0, GLuintconst* v1);

#define FUNID_glProgramUniform3ui ((EXPRESS_GPU_FUN_ID<<32u)+284)
void r_glProgramUniform3ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
void d_glProgramUniform3ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2);

#define FUNID_glProgramUniform4ui ((EXPRESS_GPU_FUN_ID<<32u)+285)
void r_glProgramUniform4ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
void d_glProgramUniform4ui(void *context, GLuintconst* program, GLintconst* location, GLuintconst* v0, GLuintconst* v1, GLuintconst* v2, GLuintconst* v3);

#define FUNID_glProgramUniform1f ((EXPRESS_GPU_FUN_ID<<32u)+286)
void r_glProgramUniform1f(void *context, GLuint program, GLint location, GLfloat v0);
void d_glProgramUniform1f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0);

#define FUNID_glProgramUniform2f ((EXPRESS_GPU_FUN_ID<<32u)+287)
void r_glProgramUniform2f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1);
void d_glProgramUniform2f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1);

#define FUNID_glProgramUniform3f ((EXPRESS_GPU_FUN_ID<<32u)+288)
void r_glProgramUniform3f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void d_glProgramUniform3f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2);

#define FUNID_glProgramUniform4f ((EXPRESS_GPU_FUN_ID<<32u)+289)
void r_glProgramUniform4f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void d_glProgramUniform4f(void *context, GLuintconst* program, GLintconst* location, GLfloatconst* v0, GLfloatconst* v1, GLfloatconst* v2, GLfloatconst* v3);

#define FUNID_glTransformFeedbackVaryings ((EXPRESS_GPU_FUN_ID<<32u)+290)
void r_glTransformFeedbackVaryings(void *context, GLuint program, GLsizei count, const GLchar*const* varyings, GLenum bufferMode);
void d_glTransformFeedbackVaryings(void *context, GLuintconst* program, GLsizeiconst* count, const GLchar*const* varyings, GLenumconst* bufferMode);

#define FUNID_glTexParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+291)
void r_glTexParameterfv(void *context, GLenum target, GLenum pname, const GLfloat* params);
void d_glTexParameterfv(void *context, GLenumconst* target, GLenumconst* pname, const GLfloat*const* params);

#define FUNID_glTexParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+292)
void r_glTexParameteriv(void *context, GLenum target, GLenum pname, const GLint* params);
void d_glTexParameteriv(void *context, GLenumconst* target, GLenumconst* pname, const GLint*const* params);

#define FUNID_glUniform1fv ((EXPRESS_GPU_FUN_ID<<32u)+293)
void r_glUniform1fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform1fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glUniform1iv ((EXPRESS_GPU_FUN_ID<<32u)+294)
void r_glUniform1iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform1iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glUniform2fv ((EXPRESS_GPU_FUN_ID<<32u)+295)
void r_glUniform2fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform2fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glUniform2iv ((EXPRESS_GPU_FUN_ID<<32u)+296)
void r_glUniform2iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform2iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glUniform3fv ((EXPRESS_GPU_FUN_ID<<32u)+297)
void r_glUniform3fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform3fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glUniform3iv ((EXPRESS_GPU_FUN_ID<<32u)+298)
void r_glUniform3iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform3iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glUniform4fv ((EXPRESS_GPU_FUN_ID<<32u)+299)
void r_glUniform4fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform4fv(void *context, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glUniform4iv ((EXPRESS_GPU_FUN_ID<<32u)+300)
void r_glUniform4iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform4iv(void *context, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glVertexAttrib1fv ((EXPRESS_GPU_FUN_ID<<32u)+301)
void r_glVertexAttrib1fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib1fv(void *context, GLuintconst* index, const GLfloat*const* v);

#define FUNID_glVertexAttrib2fv ((EXPRESS_GPU_FUN_ID<<32u)+302)
void r_glVertexAttrib2fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib2fv(void *context, GLuintconst* index, const GLfloat*const* v);

#define FUNID_glVertexAttrib3fv ((EXPRESS_GPU_FUN_ID<<32u)+303)
void r_glVertexAttrib3fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib3fv(void *context, GLuintconst* index, const GLfloat*const* v);

#define FUNID_glVertexAttrib4fv ((EXPRESS_GPU_FUN_ID<<32u)+304)
void r_glVertexAttrib4fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib4fv(void *context, GLuintconst* index, const GLfloat*const* v);

#define FUNID_glUniformMatrix2fv ((EXPRESS_GPU_FUN_ID<<32u)+305)
void r_glUniformMatrix2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix2fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix3fv ((EXPRESS_GPU_FUN_ID<<32u)+306)
void r_glUniformMatrix3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix3fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix4fv ((EXPRESS_GPU_FUN_ID<<32u)+307)
void r_glUniformMatrix4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix4fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID<<32u)+308)
void r_glUniformMatrix2x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix2x3fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID<<32u)+309)
void r_glUniformMatrix3x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix3x2fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID<<32u)+310)
void r_glUniformMatrix2x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix2x4fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID<<32u)+311)
void r_glUniformMatrix4x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix4x2fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID<<32u)+312)
void r_glUniformMatrix3x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix3x4fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID<<32u)+313)
void r_glUniformMatrix4x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix4x3fv(void *context, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glVertexAttribI4iv ((EXPRESS_GPU_FUN_ID<<32u)+314)
void r_glVertexAttribI4iv(void *context, GLuint index, const GLint* v);
void d_glVertexAttribI4iv(void *context, GLuintconst* index, const GLint*const* v);

#define FUNID_glVertexAttribI4uiv ((EXPRESS_GPU_FUN_ID<<32u)+315)
void r_glVertexAttribI4uiv(void *context, GLuint index, const GLuint* v);
void d_glVertexAttribI4uiv(void *context, GLuintconst* index, const GLuint*const* v);

#define FUNID_glUniform1uiv ((EXPRESS_GPU_FUN_ID<<32u)+316)
void r_glUniform1uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform1uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glUniform2uiv ((EXPRESS_GPU_FUN_ID<<32u)+317)
void r_glUniform2uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform2uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glUniform3uiv ((EXPRESS_GPU_FUN_ID<<32u)+318)
void r_glUniform3uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform3uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glUniform4uiv ((EXPRESS_GPU_FUN_ID<<32u)+319)
void r_glUniform4uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform4uiv(void *context, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glClearBufferiv ((EXPRESS_GPU_FUN_ID<<32u)+320)
void r_glClearBufferiv(void *context, GLenum buffer, GLint drawbuffer, const GLint* value);
void d_glClearBufferiv(void *context, GLenumconst* buffer, GLintconst* drawbuffer, const GLint*const* value);

#define FUNID_glClearBufferuiv ((EXPRESS_GPU_FUN_ID<<32u)+321)
void r_glClearBufferuiv(void *context, GLenum buffer, GLint drawbuffer, const GLuint* value);
void d_glClearBufferuiv(void *context, GLenumconst* buffer, GLintconst* drawbuffer, const GLuint*const* value);

#define FUNID_glClearBufferfv ((EXPRESS_GPU_FUN_ID<<32u)+322)
void r_glClearBufferfv(void *context, GLenum buffer, GLint drawbuffer, const GLfloat* value);
void d_glClearBufferfv(void *context, GLenumconst* buffer, GLintconst* drawbuffer, const GLfloat*const* value);

#define FUNID_glSamplerParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+323)
void r_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint* param);
void d_glSamplerParameteriv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLint*const* param);

#define FUNID_glSamplerParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+324)
void r_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat* param);
void d_glSamplerParameterfv(void *context, GLuintconst* sampler, GLenumconst* pname, const GLfloat*const* param);

#define FUNID_glInvalidateFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+325)
void r_glInvalidateFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum* attachments);
void d_glInvalidateFramebuffer(void *context, GLenumconst* target, GLsizeiconst* numAttachments, const GLenum*const* attachments);

#define FUNID_glInvalidateSubFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+326)
void r_glInvalidateSubFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glInvalidateSubFramebuffer(void *context, GLenumconst* target, GLsizeiconst* numAttachments, const GLenum*const* attachments, GLintconst* x, GLintconst* y, GLsizeiconst* width, GLsizeiconst* height);

#define FUNID_glClipPlanexOES ((EXPRESS_GPU_FUN_ID<<32u)+327)
void r_glClipPlanexOES(void *context, GLenum plane, const GLfixed* equation);
void d_glClipPlanexOES(void *context, GLenumconst* plane, const GLfixed*const* equation);

#define FUNID_glFogxvOES ((EXPRESS_GPU_FUN_ID<<32u)+328)
void r_glFogxvOES(void *context, GLenum pname, const GLfixed* param);
void d_glFogxvOES(void *context, GLenumconst* pname, const GLfixed*const* param);

#define FUNID_glLightModelxvOES ((EXPRESS_GPU_FUN_ID<<32u)+329)
void r_glLightModelxvOES(void *context, GLenum pname, const GLfixed* param);
void d_glLightModelxvOES(void *context, GLenumconst* pname, const GLfixed*const* param);

#define FUNID_glLightxvOES ((EXPRESS_GPU_FUN_ID<<32u)+330)
void r_glLightxvOES(void *context, GLenum light, GLenum pname, const GLfixed* params);
void d_glLightxvOES(void *context, GLenumconst* light, GLenumconst* pname, const GLfixed*const* params);

#define FUNID_glLoadMatrixxOES ((EXPRESS_GPU_FUN_ID<<32u)+331)
void r_glLoadMatrixxOES(void *context, const GLfixed* m);
void d_glLoadMatrixxOES(void *context, const GLfixed*const* m);

#define FUNID_glMaterialxvOES ((EXPRESS_GPU_FUN_ID<<32u)+332)
void r_glMaterialxvOES(void *context, GLenum face, GLenum pname, const GLfixed* param);
void d_glMaterialxvOES(void *context, GLenumconst* face, GLenumconst* pname, const GLfixed*const* param);

#define FUNID_glMultMatrixxOES ((EXPRESS_GPU_FUN_ID<<32u)+333)
void r_glMultMatrixxOES(void *context, const GLfixed* m);
void d_glMultMatrixxOES(void *context, const GLfixed*const* m);

#define FUNID_glPointParameterxvOES ((EXPRESS_GPU_FUN_ID<<32u)+334)
void r_glPointParameterxvOES(void *context, GLenum pname, const GLfixed* params);
void d_glPointParameterxvOES(void *context, GLenumconst* pname, const GLfixed*const* params);

#define FUNID_glTexEnvxvOES ((EXPRESS_GPU_FUN_ID<<32u)+335)
void r_glTexEnvxvOES(void *context, GLenum target, GLenum pname, const GLfixed* params);
void d_glTexEnvxvOES(void *context, GLenumconst* target, GLenumconst* pname, const GLfixed*const* params);

#define FUNID_glClipPlanefOES ((EXPRESS_GPU_FUN_ID<<32u)+336)
void r_glClipPlanefOES(void *context, GLenum plane, const GLfloat* equation);
void d_glClipPlanefOES(void *context, GLenumconst* plane, const GLfloat*const* equation);

#define FUNID_glTexGenxvOES ((EXPRESS_GPU_FUN_ID<<32u)+337)
void r_glTexGenxvOES(void *context, GLenum coord, GLenum pname, const GLfixed* params);
void d_glTexGenxvOES(void *context, GLenumconst* coord, GLenumconst* pname, const GLfixed*const* params);

#define FUNID_glProgramUniform1iv ((EXPRESS_GPU_FUN_ID<<32u)+338)
void r_glProgramUniform1iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform1iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glProgramUniform2iv ((EXPRESS_GPU_FUN_ID<<32u)+339)
void r_glProgramUniform2iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform2iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glProgramUniform3iv ((EXPRESS_GPU_FUN_ID<<32u)+340)
void r_glProgramUniform3iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform3iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glProgramUniform4iv ((EXPRESS_GPU_FUN_ID<<32u)+341)
void r_glProgramUniform4iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform4iv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLint*const* value);

#define FUNID_glProgramUniform1uiv ((EXPRESS_GPU_FUN_ID<<32u)+342)
void r_glProgramUniform1uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform1uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glProgramUniform2uiv ((EXPRESS_GPU_FUN_ID<<32u)+343)
void r_glProgramUniform2uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform2uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glProgramUniform3uiv ((EXPRESS_GPU_FUN_ID<<32u)+344)
void r_glProgramUniform3uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform3uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glProgramUniform4uiv ((EXPRESS_GPU_FUN_ID<<32u)+345)
void r_glProgramUniform4uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform4uiv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLuint*const* value);

#define FUNID_glProgramUniform1fv ((EXPRESS_GPU_FUN_ID<<32u)+346)
void r_glProgramUniform1fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform1fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glProgramUniform2fv ((EXPRESS_GPU_FUN_ID<<32u)+347)
void r_glProgramUniform2fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glProgramUniform3fv ((EXPRESS_GPU_FUN_ID<<32u)+348)
void r_glProgramUniform3fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glProgramUniform4fv ((EXPRESS_GPU_FUN_ID<<32u)+349)
void r_glProgramUniform4fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix2fv ((EXPRESS_GPU_FUN_ID<<32u)+350)
void r_glProgramUniformMatrix2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix3fv ((EXPRESS_GPU_FUN_ID<<32u)+351)
void r_glProgramUniformMatrix3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix4fv ((EXPRESS_GPU_FUN_ID<<32u)+352)
void r_glProgramUniformMatrix4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID<<32u)+353)
void r_glProgramUniformMatrix2x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix2x3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID<<32u)+354)
void r_glProgramUniformMatrix3x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix3x2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID<<32u)+355)
void r_glProgramUniformMatrix2x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix2x4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID<<32u)+356)
void r_glProgramUniformMatrix4x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix4x2fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID<<32u)+357)
void r_glProgramUniformMatrix3x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix3x4fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glProgramUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID<<32u)+358)
void r_glProgramUniformMatrix4x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix4x3fv(void *context, GLuintconst* program, GLintconst* location, GLsizeiconst* count, GLbooleanconst* transpose, const GLfloat*const* value);

#define FUNID_glBindAttribLocation ((EXPRESS_GPU_FUN_ID<<32u)+359)
void r_glBindAttribLocation(void *context, GLuint program, GLuint index, const GLchar* name);
void d_glBindAttribLocation(void *context, GLuintconst* program, GLuintconst* index, const GLchar*const* name);

#define FUNID_glVertexAttribIPointer_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+360)
void r_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLsizei length, const void* pointer);
void d_glVertexAttribIPointer_without_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLsizeiconst* stride, GLuintconst* offset, GLsizeiconst* length, const void*const* pointer);

#define FUNID_glVertexAttribPointer_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+361)
void r_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void* pointer);
void d_glVertexAttribPointer_without_bound(void *context, GLuintconst* index, GLintconst* size, GLenumconst* type, GLbooleanconst* normalized, GLsizeiconst* stride, GLuintconst* offset, GLuintconst* length, const void*const* pointer);

#define FUNID_glDrawElements_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+362)
void r_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices);
void d_glDrawElements_without_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, const void*const* indices);

#define FUNID_glDrawElementsInstanced_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+363)
void r_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
void d_glDrawElementsInstanced_without_bound(void *context, GLenumconst* mode, GLsizeiconst* count, GLenumconst* type, const void*const* indices, GLsizeiconst* instancecount);

#define FUNID_glDrawRangeElements_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+364)
void r_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
void d_glDrawRangeElements_without_bound(void *context, GLenumconst* mode, GLuintconst* start, GLuintconst* end, GLsizeiconst* count, GLenumconst* type, const void*const* indices);

#define FUNID_glFlushMappedBufferRange_special ((EXPRESS_GPU_FUN_ID<<32u)+365)
void r_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void* data);
void d_glFlushMappedBufferRange_special(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* length, const void*const* data);

#define FUNID_glBufferData_custom ((EXPRESS_GPU_FUN_ID<<32u)+366)
void r_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void* data, GLenum usage);
void d_glBufferData_custom(void *context, GLenumconst* target, GLsizeiptrconst* size, const void*const* data, GLenumconst* usage);

#define FUNID_glBufferSubData_custom ((EXPRESS_GPU_FUN_ID<<32u)+367)
void r_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
void d_glBufferSubData_custom(void *context, GLenumconst* target, GLintptrconst* offset, GLsizeiptrconst* size, const void*const* data);

#define FUNID_glCompressedTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+368)
void r_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
void d_glCompressedTexImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLsizeiconst* imageSize, const void*const* data);

#define FUNID_glCompressedTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+369)
void r_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
void d_glCompressedTexSubImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLsizeiconst* imageSize, const void*const* data);

#define FUNID_glCompressedTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+370)
void r_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
void d_glCompressedTexImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLenumconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLsizeiconst* imageSize, const void*const* data);

#define FUNID_glCompressedTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+371)
void r_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
void d_glCompressedTexSubImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLsizeiconst* imageSize, const void*const* data);

#define FUNID_glTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+372)
void r_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels);

#define FUNID_glTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+373)
void r_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* internalformat, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLintconst* border, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels);

#define FUNID_glTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+374)
void r_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexSubImage2D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLsizeiconst* width, GLsizeiconst* height, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels);

#define FUNID_glTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+375)
void r_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexSubImage3D_without_bound(void *context, GLenumconst* target, GLintconst* level, GLintconst* xoffset, GLintconst* yoffset, GLintconst* zoffset, GLsizeiconst* width, GLsizeiconst* height, GLsizeiconst* depth, GLenumconst* format, GLenumconst* type, GLintconst* buf_len, const void*const* pixels);

#define FUNID_glPrintf ((EXPRESS_GPU_FUN_ID<<32u)+376)
void r_glPrintf(void *context, GLint buf_len, const GLchar* out_string);
void d_glPrintf(void *context, GLintconst* buf_len, const GLchar*const* out_string);










#endif
