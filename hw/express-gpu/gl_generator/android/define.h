

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
#define FUNID_glTestInt1 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+2)
#define FUNID_glTestInt2 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+3)
#define FUNID_glTestInt3 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+4)
#define FUNID_glTestInt4 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+5)
#define FUNID_glTestInt5 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+6)
#define FUNID_glTestInt6 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+7)
#define FUNID_glTestPointer1 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+8)
#define FUNID_glTestPointer2 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+9)
#define FUNID_glTestPointer4 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+10)
#define FUNID_glTestString ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+11)
#define FUNID_glIsBuffer ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+12)
#define FUNID_glIsEnabled ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+13)
#define FUNID_glIsFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+14)
#define FUNID_glIsProgram ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+15)
#define FUNID_glIsRenderbuffer ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+16)
#define FUNID_glIsShader ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+17)
#define FUNID_glIsTexture ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+18)
#define FUNID_glIsQuery ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+19)
#define FUNID_glIsVertexArray ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+20)
#define FUNID_glIsSampler ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+21)
#define FUNID_glIsTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+22)
#define FUNID_glIsSync ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+23)
#define FUNID_glGetError ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+24)
#define FUNID_glGetString_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+25)
#define FUNID_glGetStringi_special ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+26)
#define FUNID_glCheckFramebufferStatus ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+27)
#define FUNID_glQueryMatrixxOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+28)
#define FUNID_glGetFramebufferAttachmentParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+29)
#define FUNID_glGetProgramInfoLog ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+30)
#define FUNID_glGetRenderbufferParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+31)
#define FUNID_glGetShaderInfoLog ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+32)
#define FUNID_glGetShaderPrecisionFormat ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+33)
#define FUNID_glGetShaderSource ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+34)
#define FUNID_glGetTexParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+35)
#define FUNID_glGetTexParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+36)
#define FUNID_glGetQueryiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+37)
#define FUNID_glGetQueryObjectuiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+38)
#define FUNID_glGetTransformFeedbackVarying ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+39)
#define FUNID_glGetActiveUniformsiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+40)
#define FUNID_glGetActiveUniformBlockiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+41)
#define FUNID_glGetActiveUniformBlockName ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+42)
#define FUNID_glGetSamplerParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+43)
#define FUNID_glGetSamplerParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+44)
#define FUNID_glGetProgramBinary ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+45)
#define FUNID_glGetInternalformativ ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+46)
#define FUNID_glGetClipPlanexOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+47)
#define FUNID_glGetFixedvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+48)
#define FUNID_glGetTexEnvxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+49)
#define FUNID_glGetTexParameterxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+50)
#define FUNID_glGetLightxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+51)
#define FUNID_glGetMaterialxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+52)
#define FUNID_glGetTexGenxvOES ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+53)
#define FUNID_glGetFramebufferParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+54)
#define FUNID_glGetProgramInterfaceiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+55)
#define FUNID_glGetProgramResourceName ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+56)
#define FUNID_glGetProgramResourceiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+57)
#define FUNID_glGetProgramPipelineiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+58)
#define FUNID_glGetProgramPipelineInfoLog ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+59)
#define FUNID_glGetMultisamplefv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+60)
#define FUNID_glGetTexLevelParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+61)
#define FUNID_glGetTexLevelParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+62)
#define FUNID_glGetSynciv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+63)
#define FUNID_glGetAttribLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+64)
#define FUNID_glGetUniformLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+65)
#define FUNID_glGetFragDataLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+66)
#define FUNID_glGetUniformBlockIndex ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+67)
#define FUNID_glGetProgramResourceIndex ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+68)
#define FUNID_glGetProgramResourceLocation ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+69)
#define FUNID_glGetActiveAttrib ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+70)
#define FUNID_glGetActiveUniform ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+71)
#define FUNID_glGetAttachedShaders ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+72)
#define FUNID_glGetProgramiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+73)
#define FUNID_glGetShaderiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+74)
#define FUNID_glGetUniformfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+75)
#define FUNID_glGetUniformiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+76)
#define FUNID_glGetUniformuiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+77)
#define FUNID_glGetUniformIndices ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+78)
#define FUNID_glGetVertexAttribfv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+79)
#define FUNID_glGetVertexAttribiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+80)
#define FUNID_glGetVertexAttribIiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+81)
#define FUNID_glGetVertexAttribIuiv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+82)
#define FUNID_glGetBufferParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+83)
#define FUNID_glGetBufferParameteri64v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+84)
#define FUNID_glGetBooleanv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+85)
#define FUNID_glGetBooleani_v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+86)
#define FUNID_glGetFloatv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+87)
#define FUNID_glGetIntegerv ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+88)
#define FUNID_glGetIntegeri_v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+89)
#define FUNID_glGetInteger64v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+90)
#define FUNID_glGetInteger64i_v ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+91)
#define FUNID_glMapBufferRange_read ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+92)
#define FUNID_glReadPixels_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+93)
#define FUNID_glTestPointer3 ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long)0x1)<<24u)+94)
#define FUNID_glFlush ((EXPRESS_GPU_FUN_ID<<32u)+95)
#define FUNID_glFinish ((EXPRESS_GPU_FUN_ID<<32u)+96)
#define FUNID_glBeginQuery ((EXPRESS_GPU_FUN_ID<<32u)+97)
#define FUNID_glEndQuery ((EXPRESS_GPU_FUN_ID<<32u)+98)
#define FUNID_glViewport ((EXPRESS_GPU_FUN_ID<<32u)+99)
#define FUNID_glTexStorage2D ((EXPRESS_GPU_FUN_ID<<32u)+100)
#define FUNID_glTexStorage3D ((EXPRESS_GPU_FUN_ID<<32u)+101)
#define FUNID_glTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+102)
#define FUNID_glTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+103)
#define FUNID_glTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+104)
#define FUNID_glTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+105)
#define FUNID_glReadPixels_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+106)
#define FUNID_glCompressedTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+107)
#define FUNID_glCompressedTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+108)
#define FUNID_glCompressedTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+109)
#define FUNID_glCompressedTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+110)
#define FUNID_glCopyTexImage2D ((EXPRESS_GPU_FUN_ID<<32u)+111)
#define FUNID_glCopyTexSubImage2D ((EXPRESS_GPU_FUN_ID<<32u)+112)
#define FUNID_glCopyTexSubImage3D ((EXPRESS_GPU_FUN_ID<<32u)+113)
#define FUNID_glVertexAttribPointer_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+114)
#define FUNID_glVertexAttribPointer_offset ((EXPRESS_GPU_FUN_ID<<32u)+115)
#define FUNID_glMapBufferRange_write ((EXPRESS_GPU_FUN_ID<<32u)+116)
#define FUNID_glUnmapBuffer_special ((EXPRESS_GPU_FUN_ID<<32u)+117)
#define FUNID_glWaitSync ((EXPRESS_GPU_FUN_ID<<32u)+118)
#define FUNID_glShaderBinary ((EXPRESS_GPU_FUN_ID<<32u)+119)
#define FUNID_glProgramBinary ((EXPRESS_GPU_FUN_ID<<32u)+120)
#define FUNID_glDrawBuffers ((EXPRESS_GPU_FUN_ID<<32u)+121)
#define FUNID_glDrawArrays_origin ((EXPRESS_GPU_FUN_ID<<32u)+122)
#define FUNID_glDrawArraysInstanced_origin ((EXPRESS_GPU_FUN_ID<<32u)+123)
#define FUNID_glDrawElementsInstanced_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+124)
#define FUNID_glDrawElements_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+125)
#define FUNID_glDrawRangeElements_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+126)
#define FUNID_glTestIntAsyn ((EXPRESS_GPU_FUN_ID<<32u)+127)
#define FUNID_glPrintfAsyn ((EXPRESS_GPU_FUN_ID<<32u)+128)
#define FUNID_glEGLImageTargetTexture2DOES ((EXPRESS_GPU_FUN_ID<<32u)+129)
#define FUNID_glEGLImageTargetRenderbufferStorageOES ((EXPRESS_GPU_FUN_ID<<32u)+130)
#define FUNID_glGenBuffers ((EXPRESS_GPU_FUN_ID<<32u)+131)
#define FUNID_glGenRenderbuffers ((EXPRESS_GPU_FUN_ID<<32u)+132)
#define FUNID_glGenTextures ((EXPRESS_GPU_FUN_ID<<32u)+133)
#define FUNID_glGenSamplers ((EXPRESS_GPU_FUN_ID<<32u)+134)
#define FUNID_glCreateProgram ((EXPRESS_GPU_FUN_ID<<32u)+135)
#define FUNID_glCreateShader ((EXPRESS_GPU_FUN_ID<<32u)+136)
#define FUNID_glFenceSync ((EXPRESS_GPU_FUN_ID<<32u)+137)
#define FUNID_glCreateShaderProgramv ((EXPRESS_GPU_FUN_ID<<32u)+138)
#define FUNID_glGenFramebuffers ((EXPRESS_GPU_FUN_ID<<32u)+139)
#define FUNID_glGenProgramPipelines ((EXPRESS_GPU_FUN_ID<<32u)+140)
#define FUNID_glGenTransformFeedbacks ((EXPRESS_GPU_FUN_ID<<32u)+141)
#define FUNID_glGenVertexArrays ((EXPRESS_GPU_FUN_ID<<32u)+142)
#define FUNID_glGenQueries ((EXPRESS_GPU_FUN_ID<<32u)+143)
#define FUNID_glDeleteBuffers_origin ((EXPRESS_GPU_FUN_ID<<32u)+144)
#define FUNID_glDeleteRenderbuffers ((EXPRESS_GPU_FUN_ID<<32u)+145)
#define FUNID_glDeleteTextures ((EXPRESS_GPU_FUN_ID<<32u)+146)
#define FUNID_glDeleteSamplers ((EXPRESS_GPU_FUN_ID<<32u)+147)
#define FUNID_glDeleteProgram_origin ((EXPRESS_GPU_FUN_ID<<32u)+148)
#define FUNID_glDeleteShader ((EXPRESS_GPU_FUN_ID<<32u)+149)
#define FUNID_glDeleteSync ((EXPRESS_GPU_FUN_ID<<32u)+150)
#define FUNID_glDeleteFramebuffers ((EXPRESS_GPU_FUN_ID<<32u)+151)
#define FUNID_glDeleteProgramPipelines ((EXPRESS_GPU_FUN_ID<<32u)+152)
#define FUNID_glDeleteTransformFeedbacks ((EXPRESS_GPU_FUN_ID<<32u)+153)
#define FUNID_glDeleteVertexArrays_origin ((EXPRESS_GPU_FUN_ID<<32u)+154)
#define FUNID_glDeleteQueries ((EXPRESS_GPU_FUN_ID<<32u)+155)
#define FUNID_glLinkProgram_origin ((EXPRESS_GPU_FUN_ID<<32u)+156)
#define FUNID_glPixelStorei_origin ((EXPRESS_GPU_FUN_ID<<32u)+157)
#define FUNID_glDisableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID<<32u)+158)
#define FUNID_glEnableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID<<32u)+159)
#define FUNID_glReadBuffer_special ((EXPRESS_GPU_FUN_ID<<32u)+160)
#define FUNID_glVertexAttribDivisor_origin ((EXPRESS_GPU_FUN_ID<<32u)+161)
#define FUNID_glShaderSource_origin ((EXPRESS_GPU_FUN_ID<<32u)+162)
#define FUNID_glVertexAttribIPointer_with_bound ((EXPRESS_GPU_FUN_ID<<32u)+163)
#define FUNID_glVertexAttribIPointer_offset ((EXPRESS_GPU_FUN_ID<<32u)+164)
#define FUNID_glBindVertexArray_special ((EXPRESS_GPU_FUN_ID<<32u)+165)
#define FUNID_glBindBuffer_origin ((EXPRESS_GPU_FUN_ID<<32u)+166)
#define FUNID_glBeginTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+167)
#define FUNID_glEndTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+168)
#define FUNID_glPauseTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+169)
#define FUNID_glResumeTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+170)
#define FUNID_glBindBufferRange ((EXPRESS_GPU_FUN_ID<<32u)+171)
#define FUNID_glBindBufferBase ((EXPRESS_GPU_FUN_ID<<32u)+172)
#define FUNID_glBindTexture ((EXPRESS_GPU_FUN_ID<<32u)+173)
#define FUNID_glBindRenderbuffer ((EXPRESS_GPU_FUN_ID<<32u)+174)
#define FUNID_glBindSampler ((EXPRESS_GPU_FUN_ID<<32u)+175)
#define FUNID_glBindFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+176)
#define FUNID_glBindProgramPipeline ((EXPRESS_GPU_FUN_ID<<32u)+177)
#define FUNID_glBindTransformFeedback ((EXPRESS_GPU_FUN_ID<<32u)+178)
#define FUNID_glActiveTexture ((EXPRESS_GPU_FUN_ID<<32u)+179)
#define FUNID_glAttachShader ((EXPRESS_GPU_FUN_ID<<32u)+180)
#define FUNID_glBlendColor ((EXPRESS_GPU_FUN_ID<<32u)+181)
#define FUNID_glBlendEquation ((EXPRESS_GPU_FUN_ID<<32u)+182)
#define FUNID_glBlendEquationSeparate ((EXPRESS_GPU_FUN_ID<<32u)+183)
#define FUNID_glBlendFunc ((EXPRESS_GPU_FUN_ID<<32u)+184)
#define FUNID_glBlendFuncSeparate ((EXPRESS_GPU_FUN_ID<<32u)+185)
#define FUNID_glClear ((EXPRESS_GPU_FUN_ID<<32u)+186)
#define FUNID_glClearColor ((EXPRESS_GPU_FUN_ID<<32u)+187)
#define FUNID_glClearDepthf ((EXPRESS_GPU_FUN_ID<<32u)+188)
#define FUNID_glClearStencil ((EXPRESS_GPU_FUN_ID<<32u)+189)
#define FUNID_glColorMask ((EXPRESS_GPU_FUN_ID<<32u)+190)
#define FUNID_glCompileShader ((EXPRESS_GPU_FUN_ID<<32u)+191)
#define FUNID_glCullFace ((EXPRESS_GPU_FUN_ID<<32u)+192)
#define FUNID_glDepthFunc ((EXPRESS_GPU_FUN_ID<<32u)+193)
#define FUNID_glDepthMask ((EXPRESS_GPU_FUN_ID<<32u)+194)
#define FUNID_glDepthRangef ((EXPRESS_GPU_FUN_ID<<32u)+195)
#define FUNID_glDetachShader ((EXPRESS_GPU_FUN_ID<<32u)+196)
#define FUNID_glDisable ((EXPRESS_GPU_FUN_ID<<32u)+197)
#define FUNID_glEnable ((EXPRESS_GPU_FUN_ID<<32u)+198)
#define FUNID_glFramebufferRenderbuffer ((EXPRESS_GPU_FUN_ID<<32u)+199)
#define FUNID_glFramebufferTexture2D ((EXPRESS_GPU_FUN_ID<<32u)+200)
#define FUNID_glFrontFace ((EXPRESS_GPU_FUN_ID<<32u)+201)
#define FUNID_glGenerateMipmap ((EXPRESS_GPU_FUN_ID<<32u)+202)
#define FUNID_glHint ((EXPRESS_GPU_FUN_ID<<32u)+203)
#define FUNID_glLineWidth ((EXPRESS_GPU_FUN_ID<<32u)+204)
#define FUNID_glPolygonOffset ((EXPRESS_GPU_FUN_ID<<32u)+205)
#define FUNID_glReleaseShaderCompiler ((EXPRESS_GPU_FUN_ID<<32u)+206)
#define FUNID_glRenderbufferStorage ((EXPRESS_GPU_FUN_ID<<32u)+207)
#define FUNID_glSampleCoverage ((EXPRESS_GPU_FUN_ID<<32u)+208)
#define FUNID_glScissor ((EXPRESS_GPU_FUN_ID<<32u)+209)
#define FUNID_glStencilFunc ((EXPRESS_GPU_FUN_ID<<32u)+210)
#define FUNID_glStencilFuncSeparate ((EXPRESS_GPU_FUN_ID<<32u)+211)
#define FUNID_glStencilMask ((EXPRESS_GPU_FUN_ID<<32u)+212)
#define FUNID_glStencilMaskSeparate ((EXPRESS_GPU_FUN_ID<<32u)+213)
#define FUNID_glStencilOp ((EXPRESS_GPU_FUN_ID<<32u)+214)
#define FUNID_glStencilOpSeparate ((EXPRESS_GPU_FUN_ID<<32u)+215)
#define FUNID_glTexParameterf ((EXPRESS_GPU_FUN_ID<<32u)+216)
#define FUNID_glTexParameteri ((EXPRESS_GPU_FUN_ID<<32u)+217)
#define FUNID_glUniform1f ((EXPRESS_GPU_FUN_ID<<32u)+218)
#define FUNID_glUniform1i ((EXPRESS_GPU_FUN_ID<<32u)+219)
#define FUNID_glUniform2f ((EXPRESS_GPU_FUN_ID<<32u)+220)
#define FUNID_glUniform2i ((EXPRESS_GPU_FUN_ID<<32u)+221)
#define FUNID_glUniform3f ((EXPRESS_GPU_FUN_ID<<32u)+222)
#define FUNID_glUniform3i ((EXPRESS_GPU_FUN_ID<<32u)+223)
#define FUNID_glUniform4f ((EXPRESS_GPU_FUN_ID<<32u)+224)
#define FUNID_glUniform4i ((EXPRESS_GPU_FUN_ID<<32u)+225)
#define FUNID_glUseProgram ((EXPRESS_GPU_FUN_ID<<32u)+226)
#define FUNID_glValidateProgram ((EXPRESS_GPU_FUN_ID<<32u)+227)
#define FUNID_glVertexAttrib1f ((EXPRESS_GPU_FUN_ID<<32u)+228)
#define FUNID_glVertexAttrib2f ((EXPRESS_GPU_FUN_ID<<32u)+229)
#define FUNID_glVertexAttrib3f ((EXPRESS_GPU_FUN_ID<<32u)+230)
#define FUNID_glVertexAttrib4f ((EXPRESS_GPU_FUN_ID<<32u)+231)
#define FUNID_glBlitFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+232)
#define FUNID_glRenderbufferStorageMultisample ((EXPRESS_GPU_FUN_ID<<32u)+233)
#define FUNID_glFramebufferTextureLayer ((EXPRESS_GPU_FUN_ID<<32u)+234)
#define FUNID_glVertexAttribI4i ((EXPRESS_GPU_FUN_ID<<32u)+235)
#define FUNID_glVertexAttribI4ui ((EXPRESS_GPU_FUN_ID<<32u)+236)
#define FUNID_glUniform1ui ((EXPRESS_GPU_FUN_ID<<32u)+237)
#define FUNID_glUniform2ui ((EXPRESS_GPU_FUN_ID<<32u)+238)
#define FUNID_glUniform3ui ((EXPRESS_GPU_FUN_ID<<32u)+239)
#define FUNID_glUniform4ui ((EXPRESS_GPU_FUN_ID<<32u)+240)
#define FUNID_glClearBufferfi ((EXPRESS_GPU_FUN_ID<<32u)+241)
#define FUNID_glCopyBufferSubData ((EXPRESS_GPU_FUN_ID<<32u)+242)
#define FUNID_glUniformBlockBinding ((EXPRESS_GPU_FUN_ID<<32u)+243)
#define FUNID_glSamplerParameteri ((EXPRESS_GPU_FUN_ID<<32u)+244)
#define FUNID_glSamplerParameterf ((EXPRESS_GPU_FUN_ID<<32u)+245)
#define FUNID_glProgramParameteri ((EXPRESS_GPU_FUN_ID<<32u)+246)
#define FUNID_glAlphaFuncxOES ((EXPRESS_GPU_FUN_ID<<32u)+247)
#define FUNID_glClearColorxOES ((EXPRESS_GPU_FUN_ID<<32u)+248)
#define FUNID_glClearDepthxOES ((EXPRESS_GPU_FUN_ID<<32u)+249)
#define FUNID_glColor4xOES ((EXPRESS_GPU_FUN_ID<<32u)+250)
#define FUNID_glDepthRangexOES ((EXPRESS_GPU_FUN_ID<<32u)+251)
#define FUNID_glFogxOES ((EXPRESS_GPU_FUN_ID<<32u)+252)
#define FUNID_glFrustumxOES ((EXPRESS_GPU_FUN_ID<<32u)+253)
#define FUNID_glLightModelxOES ((EXPRESS_GPU_FUN_ID<<32u)+254)
#define FUNID_glLightxOES ((EXPRESS_GPU_FUN_ID<<32u)+255)
#define FUNID_glLineWidthxOES ((EXPRESS_GPU_FUN_ID<<32u)+256)
#define FUNID_glMaterialxOES ((EXPRESS_GPU_FUN_ID<<32u)+257)
#define FUNID_glMultiTexCoord4xOES ((EXPRESS_GPU_FUN_ID<<32u)+258)
#define FUNID_glNormal3xOES ((EXPRESS_GPU_FUN_ID<<32u)+259)
#define FUNID_glOrthoxOES ((EXPRESS_GPU_FUN_ID<<32u)+260)
#define FUNID_glPointSizexOES ((EXPRESS_GPU_FUN_ID<<32u)+261)
#define FUNID_glPolygonOffsetxOES ((EXPRESS_GPU_FUN_ID<<32u)+262)
#define FUNID_glRotatexOES ((EXPRESS_GPU_FUN_ID<<32u)+263)
#define FUNID_glScalexOES ((EXPRESS_GPU_FUN_ID<<32u)+264)
#define FUNID_glTexEnvxOES ((EXPRESS_GPU_FUN_ID<<32u)+265)
#define FUNID_glTranslatexOES ((EXPRESS_GPU_FUN_ID<<32u)+266)
#define FUNID_glPointParameterxOES ((EXPRESS_GPU_FUN_ID<<32u)+267)
#define FUNID_glSampleCoveragexOES ((EXPRESS_GPU_FUN_ID<<32u)+268)
#define FUNID_glTexGenxOES ((EXPRESS_GPU_FUN_ID<<32u)+269)
#define FUNID_glClearDepthfOES ((EXPRESS_GPU_FUN_ID<<32u)+270)
#define FUNID_glDepthRangefOES ((EXPRESS_GPU_FUN_ID<<32u)+271)
#define FUNID_glFrustumfOES ((EXPRESS_GPU_FUN_ID<<32u)+272)
#define FUNID_glOrthofOES ((EXPRESS_GPU_FUN_ID<<32u)+273)
#define FUNID_glRenderbufferStorageMultisampleEXT ((EXPRESS_GPU_FUN_ID<<32u)+274)
#define FUNID_glUseProgramStages ((EXPRESS_GPU_FUN_ID<<32u)+275)
#define FUNID_glActiveShaderProgram ((EXPRESS_GPU_FUN_ID<<32u)+276)
#define FUNID_glProgramUniform1i ((EXPRESS_GPU_FUN_ID<<32u)+277)
#define FUNID_glProgramUniform2i ((EXPRESS_GPU_FUN_ID<<32u)+278)
#define FUNID_glProgramUniform3i ((EXPRESS_GPU_FUN_ID<<32u)+279)
#define FUNID_glProgramUniform4i ((EXPRESS_GPU_FUN_ID<<32u)+280)
#define FUNID_glProgramUniform1ui ((EXPRESS_GPU_FUN_ID<<32u)+281)
#define FUNID_glProgramUniform2ui ((EXPRESS_GPU_FUN_ID<<32u)+282)
#define FUNID_glProgramUniform3ui ((EXPRESS_GPU_FUN_ID<<32u)+283)
#define FUNID_glProgramUniform4ui ((EXPRESS_GPU_FUN_ID<<32u)+284)
#define FUNID_glProgramUniform1f ((EXPRESS_GPU_FUN_ID<<32u)+285)
#define FUNID_glProgramUniform2f ((EXPRESS_GPU_FUN_ID<<32u)+286)
#define FUNID_glProgramUniform3f ((EXPRESS_GPU_FUN_ID<<32u)+287)
#define FUNID_glProgramUniform4f ((EXPRESS_GPU_FUN_ID<<32u)+288)
#define FUNID_glTransformFeedbackVaryings ((EXPRESS_GPU_FUN_ID<<32u)+289)
#define FUNID_glTexParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+290)
#define FUNID_glTexParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+291)
#define FUNID_glUniform1fv ((EXPRESS_GPU_FUN_ID<<32u)+292)
#define FUNID_glUniform1iv ((EXPRESS_GPU_FUN_ID<<32u)+293)
#define FUNID_glUniform2fv ((EXPRESS_GPU_FUN_ID<<32u)+294)
#define FUNID_glUniform2iv ((EXPRESS_GPU_FUN_ID<<32u)+295)
#define FUNID_glUniform3fv ((EXPRESS_GPU_FUN_ID<<32u)+296)
#define FUNID_glUniform3iv ((EXPRESS_GPU_FUN_ID<<32u)+297)
#define FUNID_glUniform4fv ((EXPRESS_GPU_FUN_ID<<32u)+298)
#define FUNID_glUniform4iv ((EXPRESS_GPU_FUN_ID<<32u)+299)
#define FUNID_glVertexAttrib1fv ((EXPRESS_GPU_FUN_ID<<32u)+300)
#define FUNID_glVertexAttrib2fv ((EXPRESS_GPU_FUN_ID<<32u)+301)
#define FUNID_glVertexAttrib3fv ((EXPRESS_GPU_FUN_ID<<32u)+302)
#define FUNID_glVertexAttrib4fv ((EXPRESS_GPU_FUN_ID<<32u)+303)
#define FUNID_glUniformMatrix2fv ((EXPRESS_GPU_FUN_ID<<32u)+304)
#define FUNID_glUniformMatrix3fv ((EXPRESS_GPU_FUN_ID<<32u)+305)
#define FUNID_glUniformMatrix4fv ((EXPRESS_GPU_FUN_ID<<32u)+306)
#define FUNID_glUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID<<32u)+307)
#define FUNID_glUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID<<32u)+308)
#define FUNID_glUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID<<32u)+309)
#define FUNID_glUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID<<32u)+310)
#define FUNID_glUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID<<32u)+311)
#define FUNID_glUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID<<32u)+312)
#define FUNID_glVertexAttribI4iv ((EXPRESS_GPU_FUN_ID<<32u)+313)
#define FUNID_glVertexAttribI4uiv ((EXPRESS_GPU_FUN_ID<<32u)+314)
#define FUNID_glUniform1uiv ((EXPRESS_GPU_FUN_ID<<32u)+315)
#define FUNID_glUniform2uiv ((EXPRESS_GPU_FUN_ID<<32u)+316)
#define FUNID_glUniform3uiv ((EXPRESS_GPU_FUN_ID<<32u)+317)
#define FUNID_glUniform4uiv ((EXPRESS_GPU_FUN_ID<<32u)+318)
#define FUNID_glClearBufferiv ((EXPRESS_GPU_FUN_ID<<32u)+319)
#define FUNID_glClearBufferuiv ((EXPRESS_GPU_FUN_ID<<32u)+320)
#define FUNID_glClearBufferfv ((EXPRESS_GPU_FUN_ID<<32u)+321)
#define FUNID_glSamplerParameteriv ((EXPRESS_GPU_FUN_ID<<32u)+322)
#define FUNID_glSamplerParameterfv ((EXPRESS_GPU_FUN_ID<<32u)+323)
#define FUNID_glInvalidateFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+324)
#define FUNID_glInvalidateSubFramebuffer ((EXPRESS_GPU_FUN_ID<<32u)+325)
#define FUNID_glClipPlanexOES ((EXPRESS_GPU_FUN_ID<<32u)+326)
#define FUNID_glFogxvOES ((EXPRESS_GPU_FUN_ID<<32u)+327)
#define FUNID_glLightModelxvOES ((EXPRESS_GPU_FUN_ID<<32u)+328)
#define FUNID_glLightxvOES ((EXPRESS_GPU_FUN_ID<<32u)+329)
#define FUNID_glLoadMatrixxOES ((EXPRESS_GPU_FUN_ID<<32u)+330)
#define FUNID_glMaterialxvOES ((EXPRESS_GPU_FUN_ID<<32u)+331)
#define FUNID_glMultMatrixxOES ((EXPRESS_GPU_FUN_ID<<32u)+332)
#define FUNID_glPointParameterxvOES ((EXPRESS_GPU_FUN_ID<<32u)+333)
#define FUNID_glTexEnvxvOES ((EXPRESS_GPU_FUN_ID<<32u)+334)
#define FUNID_glClipPlanefOES ((EXPRESS_GPU_FUN_ID<<32u)+335)
#define FUNID_glTexGenxvOES ((EXPRESS_GPU_FUN_ID<<32u)+336)
#define FUNID_glProgramUniform1iv ((EXPRESS_GPU_FUN_ID<<32u)+337)
#define FUNID_glProgramUniform2iv ((EXPRESS_GPU_FUN_ID<<32u)+338)
#define FUNID_glProgramUniform3iv ((EXPRESS_GPU_FUN_ID<<32u)+339)
#define FUNID_glProgramUniform4iv ((EXPRESS_GPU_FUN_ID<<32u)+340)
#define FUNID_glProgramUniform1uiv ((EXPRESS_GPU_FUN_ID<<32u)+341)
#define FUNID_glProgramUniform2uiv ((EXPRESS_GPU_FUN_ID<<32u)+342)
#define FUNID_glProgramUniform3uiv ((EXPRESS_GPU_FUN_ID<<32u)+343)
#define FUNID_glProgramUniform4uiv ((EXPRESS_GPU_FUN_ID<<32u)+344)
#define FUNID_glProgramUniform1fv ((EXPRESS_GPU_FUN_ID<<32u)+345)
#define FUNID_glProgramUniform2fv ((EXPRESS_GPU_FUN_ID<<32u)+346)
#define FUNID_glProgramUniform3fv ((EXPRESS_GPU_FUN_ID<<32u)+347)
#define FUNID_glProgramUniform4fv ((EXPRESS_GPU_FUN_ID<<32u)+348)
#define FUNID_glProgramUniformMatrix2fv ((EXPRESS_GPU_FUN_ID<<32u)+349)
#define FUNID_glProgramUniformMatrix3fv ((EXPRESS_GPU_FUN_ID<<32u)+350)
#define FUNID_glProgramUniformMatrix4fv ((EXPRESS_GPU_FUN_ID<<32u)+351)
#define FUNID_glProgramUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID<<32u)+352)
#define FUNID_glProgramUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID<<32u)+353)
#define FUNID_glProgramUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID<<32u)+354)
#define FUNID_glProgramUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID<<32u)+355)
#define FUNID_glProgramUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID<<32u)+356)
#define FUNID_glProgramUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID<<32u)+357)
#define FUNID_glBindAttribLocation ((EXPRESS_GPU_FUN_ID<<32u)+358)
#define FUNID_glTexEnvf ((EXPRESS_GPU_FUN_ID<<32u)+359)
#define FUNID_glTexEnvi ((EXPRESS_GPU_FUN_ID<<32u)+360)
#define FUNID_glTexEnvx ((EXPRESS_GPU_FUN_ID<<32u)+361)
#define FUNID_glTexParameterx ((EXPRESS_GPU_FUN_ID<<32u)+362)
#define FUNID_glShadeModel ((EXPRESS_GPU_FUN_ID<<32u)+363)
#define FUNID_glDrawTexiOES ((EXPRESS_GPU_FUN_ID<<32u)+364)
#define FUNID_glVertexAttribIPointer_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+365)
#define FUNID_glVertexAttribPointer_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+366)
#define FUNID_glDrawElements_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+367)
#define FUNID_glDrawElementsInstanced_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+368)
#define FUNID_glDrawRangeElements_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+369)
#define FUNID_glFlushMappedBufferRange_special ((EXPRESS_GPU_FUN_ID<<32u)+370)
#define FUNID_glBufferData_custom ((EXPRESS_GPU_FUN_ID<<32u)+371)
#define FUNID_glBufferSubData_custom ((EXPRESS_GPU_FUN_ID<<32u)+372)
#define FUNID_glCompressedTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+373)
#define FUNID_glCompressedTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+374)
#define FUNID_glCompressedTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+375)
#define FUNID_glCompressedTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+376)
#define FUNID_glTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+377)
#define FUNID_glTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+378)
#define FUNID_glTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+379)
#define FUNID_glTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID<<32u)+380)
#define FUNID_glPrintf ((EXPRESS_GPU_FUN_ID<<32u)+381)
GLenum r_glClientWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);
GLenum d_glClientWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);

GLint r_glTestInt1(void *context, GLint a, GLuint b);
GLint d_glTestInt1(void *context, GLint a, GLuint b);

GLuint r_glTestInt2(void *context, GLint a, GLuint b);
GLuint d_glTestInt2(void *context, GLint a, GLuint b);

GLint64 r_glTestInt3(void *context, GLint64 a, GLuint64 b);
GLint64 d_glTestInt3(void *context, GLint64 a, GLuint64 b);

GLuint64 r_glTestInt4(void *context, GLint64 a, GLuint64 b);
GLuint64 d_glTestInt4(void *context, GLint64 a, GLuint64 b);

GLfloat r_glTestInt5(void *context, GLint a, GLuint b);
GLfloat d_glTestInt5(void *context, GLint a, GLuint b);

GLdouble r_glTestInt6(void *context, GLint a, GLuint b);
GLdouble d_glTestInt6(void *context, GLint a, GLuint b);

void r_glTestPointer1(void *context, GLint a, const GLint* b);
void d_glTestPointer1(void *context, GLint a, const GLint* b);

void r_glTestPointer2(void *context, GLint a, const GLint* b, GLint* c);
void d_glTestPointer2(void *context, GLint a, const GLint* b, GLint* c);

GLint r_glTestPointer4(void *context, GLint a, const GLint* b, GLint* c);
GLint d_glTestPointer4(void *context, GLint a, const GLint* b, GLint* c);

void r_glTestString(void *context, GLint a, GLint count, const GLchar*const* strings, GLint buf_len, GLchar* char_buf);
void d_glTestString(void *context, GLint a, GLint count, const GLchar*const* strings, GLint buf_len, GLchar* char_buf);

GLboolean r_glIsBuffer(void *context, GLuint buffer);
GLboolean d_glIsBuffer(void *context, GLuint buffer);

GLboolean r_glIsEnabled(void *context, GLenum cap);
GLboolean d_glIsEnabled(void *context, GLenum cap);

GLboolean r_glIsFramebuffer(void *context, GLuint framebuffer);
GLboolean d_glIsFramebuffer(void *context, GLuint framebuffer);

GLboolean r_glIsProgram(void *context, GLuint program);
GLboolean d_glIsProgram(void *context, GLuint program);

GLboolean r_glIsRenderbuffer(void *context, GLuint renderbuffer);
GLboolean d_glIsRenderbuffer(void *context, GLuint renderbuffer);

GLboolean r_glIsShader(void *context, GLuint shader);
GLboolean d_glIsShader(void *context, GLuint shader);

GLboolean r_glIsTexture(void *context, GLuint texture);
GLboolean d_glIsTexture(void *context, GLuint texture);

GLboolean r_glIsQuery(void *context, GLuint id);
GLboolean d_glIsQuery(void *context, GLuint id);

GLboolean r_glIsVertexArray(void *context, GLuint array);
GLboolean d_glIsVertexArray(void *context, GLuint array);

GLboolean r_glIsSampler(void *context, GLuint sampler);
GLboolean d_glIsSampler(void *context, GLuint sampler);

GLboolean r_glIsTransformFeedback(void *context, GLuint id);
GLboolean d_glIsTransformFeedback(void *context, GLuint id);

GLboolean r_glIsSync(void *context, GLsync sync);
GLboolean d_glIsSync(void *context, GLsync sync);

GLenum r_glGetError(void *context);
GLenum d_glGetError(void *context);

void r_glGetString_special(void *context, GLenum name, GLubyte* buffer);
void d_glGetString(void *context, GLenum name, GLubyte* buffer);

void r_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte* buffer);
void d_glGetStringi(void *context, GLenum name, GLuint index, GLubyte* buffer);

GLenum r_glCheckFramebufferStatus(void *context, GLenum target);
GLenum d_glCheckFramebufferStatus(void *context, GLenum target);

GLbitfield r_glQueryMatrixxOES(void *context, GLfixed* mantissa, GLint* exponent);
GLbitfield d_glQueryMatrixxOES(void *context, GLfixed* mantissa, GLint* exponent);

void r_glGetFramebufferAttachmentParameteriv(void *context, GLenum target, GLenum attachment, GLenum pname, GLint* params);
void d_glGetFramebufferAttachmentParameteriv(void *context, GLenum target, GLenum attachment, GLenum pname, GLint* params);

void r_glGetProgramInfoLog(void *context, GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void d_glGetProgramInfoLog(void *context, GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

void r_glGetRenderbufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetRenderbufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);

void r_glGetShaderInfoLog(void *context, GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void d_glGetShaderInfoLog(void *context, GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

void r_glGetShaderPrecisionFormat(void *context, GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
void d_glGetShaderPrecisionFormat(void *context, GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);

void r_glGetShaderSource(void *context, GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
void d_glGetShaderSource(void *context, GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);

void r_glGetTexParameterfv(void *context, GLenum target, GLenum pname, GLfloat* params);
void d_glGetTexParameterfv(void *context, GLenum target, GLenum pname, GLfloat* params);

void r_glGetTexParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetTexParameteriv(void *context, GLenum target, GLenum pname, GLint* params);

void r_glGetQueryiv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetQueryiv(void *context, GLenum target, GLenum pname, GLint* params);

void r_glGetQueryObjectuiv(void *context, GLuint id, GLenum pname, GLuint* params);
void d_glGetQueryObjectuiv(void *context, GLuint id, GLenum pname, GLuint* params);

void r_glGetTransformFeedbackVarying(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name);
void d_glGetTransformFeedbackVarying(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name);

void r_glGetActiveUniformsiv(void *context, GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
void d_glGetActiveUniformsiv(void *context, GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);

void r_glGetActiveUniformBlockiv(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
void d_glGetActiveUniformBlockiv(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);

void r_glGetActiveUniformBlockName(void *context, GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);
void d_glGetActiveUniformBlockName(void *context, GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);

void r_glGetSamplerParameteriv(void *context, GLuint sampler, GLenum pname, GLint* params);
void d_glGetSamplerParameteriv(void *context, GLuint sampler, GLenum pname, GLint* params);

void r_glGetSamplerParameterfv(void *context, GLuint sampler, GLenum pname, GLfloat* params);
void d_glGetSamplerParameterfv(void *context, GLuint sampler, GLenum pname, GLfloat* params);

void r_glGetProgramBinary(void *context, GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary);
void d_glGetProgramBinary(void *context, GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary);

void r_glGetInternalformativ(void *context, GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint* params);
void d_glGetInternalformativ(void *context, GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint* params);

void r_glGetClipPlanexOES(void *context, GLenum plane, GLfixed* equation);
void d_glGetClipPlanexOES(void *context, GLenum plane, GLfixed* equation);

void r_glGetFixedvOES(void *context, GLenum pname, GLfixed* params);
void d_glGetFixedvOES(void *context, GLenum pname, GLfixed* params);

void r_glGetTexEnvxvOES(void *context, GLenum target, GLenum pname, GLfixed* params);
void d_glGetTexEnvxvOES(void *context, GLenum target, GLenum pname, GLfixed* params);

void r_glGetTexParameterxvOES(void *context, GLenum target, GLenum pname, GLfixed* params);
void d_glGetTexParameterxvOES(void *context, GLenum target, GLenum pname, GLfixed* params);

void r_glGetLightxvOES(void *context, GLenum light, GLenum pname, GLfixed* params);
void d_glGetLightxvOES(void *context, GLenum light, GLenum pname, GLfixed* params);

void r_glGetMaterialxvOES(void *context, GLenum face, GLenum pname, GLfixed* params);
void d_glGetMaterialxvOES(void *context, GLenum face, GLenum pname, GLfixed* params);

void r_glGetTexGenxvOES(void *context, GLenum coord, GLenum pname, GLfixed* params);
void d_glGetTexGenxvOES(void *context, GLenum coord, GLenum pname, GLfixed* params);

void r_glGetFramebufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetFramebufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);

void r_glGetProgramInterfaceiv(void *context, GLuint program, GLenum programInterface, GLenum pname, GLint* params);
void d_glGetProgramInterfaceiv(void *context, GLuint program, GLenum programInterface, GLenum pname, GLint* params);

void r_glGetProgramResourceName(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name);
void d_glGetProgramResourceName(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name);

void r_glGetProgramResourceiv(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei bufSize, GLsizei* length, GLint* params);
void d_glGetProgramResourceiv(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei bufSize, GLsizei* length, GLint* params);

void r_glGetProgramPipelineiv(void *context, GLuint pipeline, GLenum pname, GLint* params);
void d_glGetProgramPipelineiv(void *context, GLuint pipeline, GLenum pname, GLint* params);

void r_glGetProgramPipelineInfoLog(void *context, GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void d_glGetProgramPipelineInfoLog(void *context, GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

void r_glGetMultisamplefv(void *context, GLenum pname, GLuint index, GLfloat* val);
void d_glGetMultisamplefv(void *context, GLenum pname, GLuint index, GLfloat* val);

void r_glGetTexLevelParameteriv(void *context, GLenum target, GLint level, GLenum pname, GLint* params);
void d_glGetTexLevelParameteriv(void *context, GLenum target, GLint level, GLenum pname, GLint* params);

void r_glGetTexLevelParameterfv(void *context, GLenum target, GLint level, GLenum pname, GLfloat* params);
void d_glGetTexLevelParameterfv(void *context, GLenum target, GLint level, GLenum pname, GLfloat* params);

void r_glGetSynciv(void *context, GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values);
void d_glGetSynciv(void *context, GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values);

GLint r_glGetAttribLocation(void *context, GLuint program, const GLchar* name);
GLint d_glGetAttribLocation(void *context, GLuint program, const GLchar* name);

GLint r_glGetUniformLocation(void *context, GLuint program, const GLchar* name);
GLint d_glGetUniformLocation(void *context, GLuint program, const GLchar* name);

GLint r_glGetFragDataLocation(void *context, GLuint program, const GLchar* name);
GLint d_glGetFragDataLocation(void *context, GLuint program, const GLchar* name);

GLuint r_glGetUniformBlockIndex(void *context, GLuint program, const GLchar* uniformBlockName);
GLuint d_glGetUniformBlockIndex(void *context, GLuint program, const GLchar* uniformBlockName);

GLuint r_glGetProgramResourceIndex(void *context, GLuint program, GLenum programInterface, const GLchar* name);
GLuint d_glGetProgramResourceIndex(void *context, GLuint program, GLenum programInterface, const GLchar* name);

GLint r_glGetProgramResourceLocation(void *context, GLuint program, GLenum programInterface, const GLchar* name);
GLint d_glGetProgramResourceLocation(void *context, GLuint program, GLenum programInterface, const GLchar* name);

void r_glGetActiveAttrib(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
void d_glGetActiveAttrib(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);

void r_glGetActiveUniform(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
void d_glGetActiveUniform(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);

void r_glGetAttachedShaders(void *context, GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
void d_glGetAttachedShaders(void *context, GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);

void r_glGetProgramiv(void *context, GLuint program, GLenum pname, GLint* params);
void d_glGetProgramiv(void *context, GLuint program, GLenum pname, GLint* params);

void r_glGetShaderiv(void *context, GLuint shader, GLenum pname, GLint* params);
void d_glGetShaderiv(void *context, GLuint shader, GLenum pname, GLint* params);

void r_glGetUniformfv(void *context, GLuint program, GLint location, GLfloat* params);
void d_glGetUniformfv(void *context, GLuint program, GLint location, GLfloat* params);

void r_glGetUniformiv(void *context, GLuint program, GLint location, GLint* params);
void d_glGetUniformiv(void *context, GLuint program, GLint location, GLint* params);

void r_glGetUniformuiv(void *context, GLuint program, GLint location, GLuint* params);
void d_glGetUniformuiv(void *context, GLuint program, GLint location, GLuint* params);

void r_glGetUniformIndices(void *context, GLuint program, GLsizei uniformCount, const GLchar*const* uniformNames, GLuint* uniformIndices);
void d_glGetUniformIndices(void *context, GLuint program, GLsizei uniformCount, const GLchar*const* uniformNames, GLuint* uniformIndices);

void r_glGetVertexAttribfv(void *context, GLuint index, GLenum pname, GLfloat* params);
void d_glGetVertexAttribfv(void *context, GLuint index, GLenum pname, GLfloat* params);

void r_glGetVertexAttribiv(void *context, GLuint index, GLenum pname, GLint* params);
void d_glGetVertexAttribiv(void *context, GLuint index, GLenum pname, GLint* params);

void r_glGetVertexAttribIiv(void *context, GLuint index, GLenum pname, GLint* params);
void d_glGetVertexAttribIiv(void *context, GLuint index, GLenum pname, GLint* params);

void r_glGetVertexAttribIuiv(void *context, GLuint index, GLenum pname, GLuint* params);
void d_glGetVertexAttribIuiv(void *context, GLuint index, GLenum pname, GLuint* params);

void r_glGetBufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);
void d_glGetBufferParameteriv(void *context, GLenum target, GLenum pname, GLint* params);

void r_glGetBufferParameteri64v(void *context, GLenum target, GLenum pname, GLint64* params);
void d_glGetBufferParameteri64v(void *context, GLenum target, GLenum pname, GLint64* params);

void r_glGetBooleanv(void *context, GLenum pname, GLboolean* data);
void d_glGetBooleanv(void *context, GLenum pname, GLboolean* data);

void r_glGetBooleani_v(void *context, GLenum target, GLuint index, GLboolean* data);
void d_glGetBooleani_v(void *context, GLenum target, GLuint index, GLboolean* data);

void r_glGetFloatv(void *context, GLenum pname, GLfloat* data);
void d_glGetFloatv(void *context, GLenum pname, GLfloat* data);

void r_glGetIntegerv(void *context, GLenum pname, GLint* data);
void d_glGetIntegerv(void *context, GLenum pname, GLint* data);

void r_glGetIntegeri_v(void *context, GLenum target, GLuint index, GLint* data);
void d_glGetIntegeri_v(void *context, GLenum target, GLuint index, GLint* data);

void r_glGetInteger64v(void *context, GLenum pname, GLint64* data);
void d_glGetInteger64v(void *context, GLenum pname, GLint64* data);

void r_glGetInteger64i_v(void *context, GLenum target, GLuint index, GLint64* data);
void d_glGetInteger64i_v(void *context, GLenum target, GLuint index, GLint64* data);

void r_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void* mem_buf);
void d_glMapBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void* mem_buf);

void r_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void* pixels);
void d_glReadPixels(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void* pixels);

GLint r_glTestPointer3(void *context, GLint a, const GLint* b, GLint* c);
GLint d_glTestPointer3(void *context, GLint a, const GLint* b, GLint* c);

void r_glFlush(void *context);
void d_glFlush(void *context);

void r_glFinish(void *context);
void d_glFinish(void *context);

void r_glBeginQuery(void *context, GLenum target, GLuint id);
void d_glBeginQuery(void *context, GLenum target, GLuint id);

void r_glEndQuery(void *context, GLenum target);
void d_glEndQuery(void *context, GLenum target);

void r_glViewport(void *context, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glViewport(void *context, GLint x, GLint y, GLsizei width, GLsizei height);

void r_glTexStorage2D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void d_glTexStorage2D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);

void r_glTexStorage3D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
void d_glTexStorage3D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

void r_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels);
void d_glTexImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels);

void r_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);
void d_glTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);

void r_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels);
void d_glTexImage3D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels);

void r_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels);
void d_glTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels);

void r_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);
void d_glReadPixels(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);

void r_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data);
void d_glCompressedTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data);

void r_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data);
void d_glCompressedTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data);

void r_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data);
void d_glCompressedTexImage3D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data);

void r_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data);
void d_glCompressedTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data);

void r_glCopyTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void d_glCopyTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);

void r_glCopyTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glCopyTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);

void r_glCopyTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glCopyTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

void r_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer);
void d_glVertexAttribPointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer);

void r_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset);
void d_glVertexAttribPointer(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset);

void r_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
void d_glMapBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);

void r_glUnmapBuffer_special(void *context, GLenum target);
void d_glUnmapBuffer(void *context, GLenum target);

void r_glWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);
void d_glWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout);

void r_glShaderBinary(void *context, GLsizei count, const GLuint* shaders, GLenum binaryFormat, const void* binary, GLsizei length);
void d_glShaderBinary(void *context, GLsizei count, const GLuint* shaders, GLenum binaryFormat, const void* binary, GLsizei length);

void r_glProgramBinary(void *context, GLuint program, GLenum binaryFormat, const void* binary, GLsizei length);
void d_glProgramBinary(void *context, GLuint program, GLenum binaryFormat, const void* binary, GLsizei length);

void r_glDrawBuffers(void *context, GLsizei n, const GLenum* bufs);
void d_glDrawBuffers(void *context, GLsizei n, const GLenum* bufs);

void r_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count);
void d_glDrawArrays(void *context, GLenum mode, GLint first, GLsizei count);

void r_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
void d_glDrawArraysInstanced(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

void r_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount);
void d_glDrawElementsInstanced(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount);

void r_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawElements(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices);

void r_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawRangeElements(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices);

void r_glTestIntAsyn(void *context, GLint a, GLuint b, GLfloat c, GLdouble d);
void d_glTestIntAsyn(void *context, GLint a, GLuint b, GLfloat c, GLdouble d);

void r_glPrintfAsyn(void *context, GLint a, GLuint size, GLdouble c, const GLchar* out_string);
void d_glPrintfAsyn(void *context, GLint a, GLuint size, GLdouble c, const GLchar* out_string);

void r_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES imageSize);
void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES imageSize);

void r_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image);
void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image);

void r_glGenBuffers(void *context, GLsizei n, const GLuint* buffers);
void d_glGenBuffers(void *context, GLsizei n, const GLuint* buffers);

void r_glGenRenderbuffers(void *context, GLsizei n, const GLuint* renderbuffers);
void d_glGenRenderbuffers(void *context, GLsizei n, const GLuint* renderbuffers);

void r_glGenTextures(void *context, GLsizei n, const GLuint* textures);
void d_glGenTextures(void *context, GLsizei n, const GLuint* textures);

void r_glGenSamplers(void *context, GLsizei count, const GLuint* samplers);
void d_glGenSamplers(void *context, GLsizei count, const GLuint* samplers);

void r_glCreateProgram(void *context, GLuint program);
void d_glCreateProgram(void *context, GLuint program);

void r_glCreateShader(void *context, GLenum type, GLuint shader);
void d_glCreateShader(void *context, GLenum type, GLuint shader);

void r_glFenceSync(void *context, GLenum condition, GLbitfield flags, GLsync sync);
void d_glFenceSync(void *context, GLenum condition, GLbitfield flags, GLsync sync);

void r_glCreateShaderProgramv(void *context, GLenum type, GLsizei count, const GLchar*const* strings, GLuint program);
void d_glCreateShaderProgramv(void *context, GLenum type, GLsizei count, const GLchar*const* strings, GLuint program);

void r_glGenFramebuffers(void *context, GLsizei n, const GLuint* framebuffers);
void d_glGenFramebuffers(void *context, GLsizei n, const GLuint* framebuffers);

void r_glGenProgramPipelines(void *context, GLsizei n, const GLuint* pipelines);
void d_glGenProgramPipelines(void *context, GLsizei n, const GLuint* pipelines);

void r_glGenTransformFeedbacks(void *context, GLsizei n, const GLuint* ids);
void d_glGenTransformFeedbacks(void *context, GLsizei n, const GLuint* ids);

void r_glGenVertexArrays(void *context, GLsizei n, const GLuint* arrays);
void d_glGenVertexArrays(void *context, GLsizei n, const GLuint* arrays);

void r_glGenQueries(void *context, GLsizei n, const GLuint* ids);
void d_glGenQueries(void *context, GLsizei n, const GLuint* ids);

void r_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint* buffers);
void d_glDeleteBuffers(void *context, GLsizei n, const GLuint* buffers);

void r_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint* renderbuffers);
void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint* renderbuffers);

void r_glDeleteTextures(void *context, GLsizei n, const GLuint* textures);
void d_glDeleteTextures(void *context, GLsizei n, const GLuint* textures);

void r_glDeleteSamplers(void *context, GLsizei count, const GLuint* samplers);
void d_glDeleteSamplers(void *context, GLsizei count, const GLuint* samplers);

void r_glDeleteProgram_origin(void *context, GLuint program);
void d_glDeleteProgram(void *context, GLuint program);

void r_glDeleteShader(void *context, GLuint shader);
void d_glDeleteShader(void *context, GLuint shader);

void r_glDeleteSync(void *context, GLsync sync);
void d_glDeleteSync(void *context, GLsync sync);

void r_glDeleteFramebuffers(void *context, GLsizei n, const GLuint* framebuffers);
void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint* framebuffers);

void r_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint* pipelines);
void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint* pipelines);

void r_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint* ids);
void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint* ids);

void r_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint* arrays);
void d_glDeleteVertexArrays(void *context, GLsizei n, const GLuint* arrays);

void r_glDeleteQueries(void *context, GLsizei n, const GLuint* ids);
void d_glDeleteQueries(void *context, GLsizei n, const GLuint* ids);

void r_glLinkProgram_origin(void *context, GLuint program);
void d_glLinkProgram(void *context, GLuint program);

void r_glPixelStorei_origin(void *context, GLenum pname, GLint param);
void d_glPixelStorei(void *context, GLenum pname, GLint param);

void r_glDisableVertexAttribArray_origin(void *context, GLuint index);
void d_glDisableVertexAttribArray(void *context, GLuint index);

void r_glEnableVertexAttribArray_origin(void *context, GLuint index);
void d_glEnableVertexAttribArray(void *context, GLuint index);

void r_glReadBuffer_special(void *context, GLenum src);
void d_glReadBuffer(void *context, GLenum src);

void r_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor);
void d_glVertexAttribDivisor(void *context, GLuint index, GLuint divisor);

void r_glShaderSource_origin(void *context, GLuint shader, GLsizei count, const GLint* length, const GLchar*const* string);
void d_glShaderSource(void *context, GLuint shader, GLsizei count, const GLint* length, const GLchar*const* string);

void r_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer);
void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer);

void r_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset);
void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset);

void r_glBindVertexArray_special(void *context, GLuint array);
void d_glBindVertexArray(void *context, GLuint array);

void r_glBindBuffer_origin(void *context, GLenum target, GLuint buffer);
void d_glBindBuffer(void *context, GLenum target, GLuint buffer);

void r_glBeginTransformFeedback(void *context, GLenum primitiveMode);
void d_glBeginTransformFeedback(void *context, GLenum primitiveMode);

void r_glEndTransformFeedback(void *context);
void d_glEndTransformFeedback(void *context);

void r_glPauseTransformFeedback(void *context);
void d_glPauseTransformFeedback(void *context);

void r_glResumeTransformFeedback(void *context);
void d_glResumeTransformFeedback(void *context);

void r_glBindBufferRange(void *context, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
void d_glBindBufferRange(void *context, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);

void r_glBindBufferBase(void *context, GLenum target, GLuint index, GLuint buffer);
void d_glBindBufferBase(void *context, GLenum target, GLuint index, GLuint buffer);

void r_glBindTexture(void *context, GLenum target, GLuint texture);
void d_glBindTexture(void *context, GLenum target, GLuint texture);

void r_glBindRenderbuffer(void *context, GLenum target, GLuint renderbuffer);
void d_glBindRenderbuffer(void *context, GLenum target, GLuint renderbuffer);

void r_glBindSampler(void *context, GLuint unit, GLuint sampler);
void d_glBindSampler(void *context, GLuint unit, GLuint sampler);

void r_glBindFramebuffer(void *context, GLenum target, GLuint framebuffer);
void d_glBindFramebuffer(void *context, GLenum target, GLuint framebuffer);

void r_glBindProgramPipeline(void *context, GLuint pipeline);
void d_glBindProgramPipeline(void *context, GLuint pipeline);

void r_glBindTransformFeedback(void *context, GLenum target, GLuint feedback_id);
void d_glBindTransformFeedback(void *context, GLenum target, GLuint feedback_id);

void r_glActiveTexture(void *context, GLenum texture);
void d_glActiveTexture(void *context, GLenum texture);

void r_glAttachShader(void *context, GLuint program, GLuint shader);
void d_glAttachShader(void *context, GLuint program, GLuint shader);

void r_glBlendColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void d_glBlendColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

void r_glBlendEquation(void *context, GLenum mode);
void d_glBlendEquation(void *context, GLenum mode);

void r_glBlendEquationSeparate(void *context, GLenum modeRGB, GLenum modeAlpha);
void d_glBlendEquationSeparate(void *context, GLenum modeRGB, GLenum modeAlpha);

void r_glBlendFunc(void *context, GLenum sfactor, GLenum dfactor);
void d_glBlendFunc(void *context, GLenum sfactor, GLenum dfactor);

void r_glBlendFuncSeparate(void *context, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
void d_glBlendFuncSeparate(void *context, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

void r_glClear(void *context, GLbitfield mask);
void d_glClear(void *context, GLbitfield mask);

void r_glClearColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void d_glClearColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

void r_glClearDepthf(void *context, GLfloat d);
void d_glClearDepthf(void *context, GLfloat d);

void r_glClearStencil(void *context, GLint s);
void d_glClearStencil(void *context, GLint s);

void r_glColorMask(void *context, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void d_glColorMask(void *context, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

void r_glCompileShader(void *context, GLuint shader);
void d_glCompileShader(void *context, GLuint shader);

void r_glCullFace(void *context, GLenum mode);
void d_glCullFace(void *context, GLenum mode);

void r_glDepthFunc(void *context, GLenum func);
void d_glDepthFunc(void *context, GLenum func);

void r_glDepthMask(void *context, GLboolean flag);
void d_glDepthMask(void *context, GLboolean flag);

void r_glDepthRangef(void *context, GLfloat n, GLfloat f);
void d_glDepthRangef(void *context, GLfloat n, GLfloat f);

void r_glDetachShader(void *context, GLuint program, GLuint shader);
void d_glDetachShader(void *context, GLuint program, GLuint shader);

void r_glDisable(void *context, GLenum cap);
void d_glDisable(void *context, GLenum cap);

void r_glEnable(void *context, GLenum cap);
void d_glEnable(void *context, GLenum cap);

void r_glFramebufferRenderbuffer(void *context, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void d_glFramebufferRenderbuffer(void *context, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

void r_glFramebufferTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void d_glFramebufferTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

void r_glFrontFace(void *context, GLenum mode);
void d_glFrontFace(void *context, GLenum mode);

void r_glGenerateMipmap(void *context, GLenum target);
void d_glGenerateMipmap(void *context, GLenum target);

void r_glHint(void *context, GLenum target, GLenum mode);
void d_glHint(void *context, GLenum target, GLenum mode);

void r_glLineWidth(void *context, GLfloat width);
void d_glLineWidth(void *context, GLfloat width);

void r_glPolygonOffset(void *context, GLfloat factor, GLfloat units);
void d_glPolygonOffset(void *context, GLfloat factor, GLfloat units);

void r_glReleaseShaderCompiler(void *context);
void d_glReleaseShaderCompiler(void *context);

void r_glRenderbufferStorage(void *context, GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
void d_glRenderbufferStorage(void *context, GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

void r_glSampleCoverage(void *context, GLfloat value, GLboolean invert);
void d_glSampleCoverage(void *context, GLfloat value, GLboolean invert);

void r_glScissor(void *context, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glScissor(void *context, GLint x, GLint y, GLsizei width, GLsizei height);

void r_glStencilFunc(void *context, GLenum func, GLint ref, GLuint mask);
void d_glStencilFunc(void *context, GLenum func, GLint ref, GLuint mask);

void r_glStencilFuncSeparate(void *context, GLenum face, GLenum func, GLint ref, GLuint mask);
void d_glStencilFuncSeparate(void *context, GLenum face, GLenum func, GLint ref, GLuint mask);

void r_glStencilMask(void *context, GLuint mask);
void d_glStencilMask(void *context, GLuint mask);

void r_glStencilMaskSeparate(void *context, GLenum face, GLuint mask);
void d_glStencilMaskSeparate(void *context, GLenum face, GLuint mask);

void r_glStencilOp(void *context, GLenum fail, GLenum zfail, GLenum zpass);
void d_glStencilOp(void *context, GLenum fail, GLenum zfail, GLenum zpass);

void r_glStencilOpSeparate(void *context, GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
void d_glStencilOpSeparate(void *context, GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);

void r_glTexParameterf(void *context, GLenum target, GLenum pname, GLfloat param);
void d_glTexParameterf(void *context, GLenum target, GLenum pname, GLfloat param);

void r_glTexParameteri(void *context, GLenum target, GLenum pname, GLint param);
void d_glTexParameteri(void *context, GLenum target, GLenum pname, GLint param);

void r_glUniform1f(void *context, GLint location, GLfloat v0);
void d_glUniform1f(void *context, GLint location, GLfloat v0);

void r_glUniform1i(void *context, GLint location, GLint v0);
void d_glUniform1i(void *context, GLint location, GLint v0);

void r_glUniform2f(void *context, GLint location, GLfloat v0, GLfloat v1);
void d_glUniform2f(void *context, GLint location, GLfloat v0, GLfloat v1);

void r_glUniform2i(void *context, GLint location, GLint v0, GLint v1);
void d_glUniform2i(void *context, GLint location, GLint v0, GLint v1);

void r_glUniform3f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void d_glUniform3f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);

void r_glUniform3i(void *context, GLint location, GLint v0, GLint v1, GLint v2);
void d_glUniform3i(void *context, GLint location, GLint v0, GLint v1, GLint v2);

void r_glUniform4f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void d_glUniform4f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

void r_glUniform4i(void *context, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void d_glUniform4i(void *context, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

void r_glUseProgram(void *context, GLuint program);
void d_glUseProgram(void *context, GLuint program);

void r_glValidateProgram(void *context, GLuint program);
void d_glValidateProgram(void *context, GLuint program);

void r_glVertexAttrib1f(void *context, GLuint index, GLfloat x);
void d_glVertexAttrib1f(void *context, GLuint index, GLfloat x);

void r_glVertexAttrib2f(void *context, GLuint index, GLfloat x, GLfloat y);
void d_glVertexAttrib2f(void *context, GLuint index, GLfloat x, GLfloat y);

void r_glVertexAttrib3f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z);
void d_glVertexAttrib3f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z);

void r_glVertexAttrib4f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void d_glVertexAttrib4f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

void r_glBlitFramebuffer(void *context, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
void d_glBlitFramebuffer(void *context, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

void r_glRenderbufferStorageMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
void d_glRenderbufferStorageMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

void r_glFramebufferTextureLayer(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
void d_glFramebufferTextureLayer(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);

void r_glVertexAttribI4i(void *context, GLuint index, GLint x, GLint y, GLint z, GLint w);
void d_glVertexAttribI4i(void *context, GLuint index, GLint x, GLint y, GLint z, GLint w);

void r_glVertexAttribI4ui(void *context, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
void d_glVertexAttribI4ui(void *context, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);

void r_glUniform1ui(void *context, GLint location, GLuint v0);
void d_glUniform1ui(void *context, GLint location, GLuint v0);

void r_glUniform2ui(void *context, GLint location, GLuint v0, GLuint v1);
void d_glUniform2ui(void *context, GLint location, GLuint v0, GLuint v1);

void r_glUniform3ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2);
void d_glUniform3ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2);

void r_glUniform4ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
void d_glUniform4ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);

void r_glClearBufferfi(void *context, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
void d_glClearBufferfi(void *context, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);

void r_glCopyBufferSubData(void *context, GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
void d_glCopyBufferSubData(void *context, GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);

void r_glUniformBlockBinding(void *context, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
void d_glUniformBlockBinding(void *context, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

void r_glSamplerParameteri(void *context, GLuint sampler, GLenum pname, GLint param);
void d_glSamplerParameteri(void *context, GLuint sampler, GLenum pname, GLint param);

void r_glSamplerParameterf(void *context, GLuint sampler, GLenum pname, GLfloat param);
void d_glSamplerParameterf(void *context, GLuint sampler, GLenum pname, GLfloat param);

void r_glProgramParameteri(void *context, GLuint program, GLenum pname, GLint value);
void d_glProgramParameteri(void *context, GLuint program, GLenum pname, GLint value);

void r_glAlphaFuncxOES(void *context, GLenum func, GLfixed ref);
void d_glAlphaFuncxOES(void *context, GLenum func, GLfixed ref);

void r_glClearColorxOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
void d_glClearColorxOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);

void r_glClearDepthxOES(void *context, GLfixed depth);
void d_glClearDepthxOES(void *context, GLfixed depth);

void r_glColor4xOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
void d_glColor4xOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);

void r_glDepthRangexOES(void *context, GLfixed n, GLfixed f);
void d_glDepthRangexOES(void *context, GLfixed n, GLfixed f);

void r_glFogxOES(void *context, GLenum pname, GLfixed param);
void d_glFogxOES(void *context, GLenum pname, GLfixed param);

void r_glFrustumxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);
void d_glFrustumxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);

void r_glLightModelxOES(void *context, GLenum pname, GLfixed param);
void d_glLightModelxOES(void *context, GLenum pname, GLfixed param);

void r_glLightxOES(void *context, GLenum light, GLenum pname, GLfixed param);
void d_glLightxOES(void *context, GLenum light, GLenum pname, GLfixed param);

void r_glLineWidthxOES(void *context, GLfixed width);
void d_glLineWidthxOES(void *context, GLfixed width);

void r_glMaterialxOES(void *context, GLenum face, GLenum pname, GLfixed param);
void d_glMaterialxOES(void *context, GLenum face, GLenum pname, GLfixed param);

void r_glMultiTexCoord4xOES(void *context, GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
void d_glMultiTexCoord4xOES(void *context, GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q);

void r_glNormal3xOES(void *context, GLfixed nx, GLfixed ny, GLfixed nz);
void d_glNormal3xOES(void *context, GLfixed nx, GLfixed ny, GLfixed nz);

void r_glOrthoxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);
void d_glOrthoxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);

void r_glPointSizexOES(void *context, GLfixed size);
void d_glPointSizexOES(void *context, GLfixed size);

void r_glPolygonOffsetxOES(void *context, GLfixed factor, GLfixed units);
void d_glPolygonOffsetxOES(void *context, GLfixed factor, GLfixed units);

void r_glRotatexOES(void *context, GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
void d_glRotatexOES(void *context, GLfixed angle, GLfixed x, GLfixed y, GLfixed z);

void r_glScalexOES(void *context, GLfixed x, GLfixed y, GLfixed z);
void d_glScalexOES(void *context, GLfixed x, GLfixed y, GLfixed z);

void r_glTexEnvxOES(void *context, GLenum target, GLenum pname, GLfixed param);
void d_glTexEnvxOES(void *context, GLenum target, GLenum pname, GLfixed param);

void r_glTranslatexOES(void *context, GLfixed x, GLfixed y, GLfixed z);
void d_glTranslatexOES(void *context, GLfixed x, GLfixed y, GLfixed z);

void r_glPointParameterxOES(void *context, GLenum pname, GLfixed param);
void d_glPointParameterxOES(void *context, GLenum pname, GLfixed param);

void r_glSampleCoveragexOES(void *context, GLclampx value, GLboolean invert);
void d_glSampleCoveragexOES(void *context, GLclampx value, GLboolean invert);

void r_glTexGenxOES(void *context, GLenum coord, GLenum pname, GLfixed param);
void d_glTexGenxOES(void *context, GLenum coord, GLenum pname, GLfixed param);

void r_glClearDepthfOES(void *context, GLclampf depth);
void d_glClearDepthfOES(void *context, GLclampf depth);

void r_glDepthRangefOES(void *context, GLclampf n, GLclampf f);
void d_glDepthRangefOES(void *context, GLclampf n, GLclampf f);

void r_glFrustumfOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
void d_glFrustumfOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);

void r_glOrthofOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
void d_glOrthofOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);

void r_glRenderbufferStorageMultisampleEXT(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
void d_glRenderbufferStorageMultisampleEXT(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

void r_glUseProgramStages(void *context, GLuint pipeline, GLbitfield stages, GLuint program);
void d_glUseProgramStages(void *context, GLuint pipeline, GLbitfield stages, GLuint program);

void r_glActiveShaderProgram(void *context, GLuint pipeline, GLuint program);
void d_glActiveShaderProgram(void *context, GLuint pipeline, GLuint program);

void r_glProgramUniform1i(void *context, GLuint program, GLint location, GLint v0);
void d_glProgramUniform1i(void *context, GLuint program, GLint location, GLint v0);

void r_glProgramUniform2i(void *context, GLuint program, GLint location, GLint v0, GLint v1);
void d_glProgramUniform2i(void *context, GLuint program, GLint location, GLint v0, GLint v1);

void r_glProgramUniform3i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
void d_glProgramUniform3i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2);

void r_glProgramUniform4i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
void d_glProgramUniform4i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

void r_glProgramUniform1ui(void *context, GLuint program, GLint location, GLuint v0);
void d_glProgramUniform1ui(void *context, GLuint program, GLint location, GLuint v0);

void r_glProgramUniform2ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1);
void d_glProgramUniform2ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1);

void r_glProgramUniform3ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
void d_glProgramUniform3ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);

void r_glProgramUniform4ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
void d_glProgramUniform4ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);

void r_glProgramUniform1f(void *context, GLuint program, GLint location, GLfloat v0);
void d_glProgramUniform1f(void *context, GLuint program, GLint location, GLfloat v0);

void r_glProgramUniform2f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1);
void d_glProgramUniform2f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1);

void r_glProgramUniform3f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void d_glProgramUniform3f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);

void r_glProgramUniform4f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void d_glProgramUniform4f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

void r_glTransformFeedbackVaryings(void *context, GLuint program, GLsizei count, const GLchar*const* varyings, GLenum bufferMode);
void d_glTransformFeedbackVaryings(void *context, GLuint program, GLsizei count, const GLchar*const* varyings, GLenum bufferMode);

void r_glTexParameterfv(void *context, GLenum target, GLenum pname, const GLfloat* params);
void d_glTexParameterfv(void *context, GLenum target, GLenum pname, const GLfloat* params);

void r_glTexParameteriv(void *context, GLenum target, GLenum pname, const GLint* params);
void d_glTexParameteriv(void *context, GLenum target, GLenum pname, const GLint* params);

void r_glUniform1fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform1fv(void *context, GLint location, GLsizei count, const GLfloat* value);

void r_glUniform1iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform1iv(void *context, GLint location, GLsizei count, const GLint* value);

void r_glUniform2fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform2fv(void *context, GLint location, GLsizei count, const GLfloat* value);

void r_glUniform2iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform2iv(void *context, GLint location, GLsizei count, const GLint* value);

void r_glUniform3fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform3fv(void *context, GLint location, GLsizei count, const GLfloat* value);

void r_glUniform3iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform3iv(void *context, GLint location, GLsizei count, const GLint* value);

void r_glUniform4fv(void *context, GLint location, GLsizei count, const GLfloat* value);
void d_glUniform4fv(void *context, GLint location, GLsizei count, const GLfloat* value);

void r_glUniform4iv(void *context, GLint location, GLsizei count, const GLint* value);
void d_glUniform4iv(void *context, GLint location, GLsizei count, const GLint* value);

void r_glVertexAttrib1fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib1fv(void *context, GLuint index, const GLfloat* v);

void r_glVertexAttrib2fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib2fv(void *context, GLuint index, const GLfloat* v);

void r_glVertexAttrib3fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib3fv(void *context, GLuint index, const GLfloat* v);

void r_glVertexAttrib4fv(void *context, GLuint index, const GLfloat* v);
void d_glVertexAttrib4fv(void *context, GLuint index, const GLfloat* v);

void r_glUniformMatrix2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix2x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix2x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix3x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix3x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix2x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix2x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix4x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix4x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix3x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix3x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glUniformMatrix4x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glUniformMatrix4x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glVertexAttribI4iv(void *context, GLuint index, const GLint* v);
void d_glVertexAttribI4iv(void *context, GLuint index, const GLint* v);

void r_glVertexAttribI4uiv(void *context, GLuint index, const GLuint* v);
void d_glVertexAttribI4uiv(void *context, GLuint index, const GLuint* v);

void r_glUniform1uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform1uiv(void *context, GLint location, GLsizei count, const GLuint* value);

void r_glUniform2uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform2uiv(void *context, GLint location, GLsizei count, const GLuint* value);

void r_glUniform3uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform3uiv(void *context, GLint location, GLsizei count, const GLuint* value);

void r_glUniform4uiv(void *context, GLint location, GLsizei count, const GLuint* value);
void d_glUniform4uiv(void *context, GLint location, GLsizei count, const GLuint* value);

void r_glClearBufferiv(void *context, GLenum buffer, GLint drawbuffer, const GLint* value);
void d_glClearBufferiv(void *context, GLenum buffer, GLint drawbuffer, const GLint* value);

void r_glClearBufferuiv(void *context, GLenum buffer, GLint drawbuffer, const GLuint* value);
void d_glClearBufferuiv(void *context, GLenum buffer, GLint drawbuffer, const GLuint* value);

void r_glClearBufferfv(void *context, GLenum buffer, GLint drawbuffer, const GLfloat* value);
void d_glClearBufferfv(void *context, GLenum buffer, GLint drawbuffer, const GLfloat* value);

void r_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint* param);
void d_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint* param);

void r_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat* param);
void d_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat* param);

void r_glInvalidateFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum* attachments);
void d_glInvalidateFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum* attachments);

void r_glInvalidateSubFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
void d_glInvalidateSubFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);

void r_glClipPlanexOES(void *context, GLenum plane, const GLfixed* equation);
void d_glClipPlanexOES(void *context, GLenum plane, const GLfixed* equation);

void r_glFogxvOES(void *context, GLenum pname, const GLfixed* param);
void d_glFogxvOES(void *context, GLenum pname, const GLfixed* param);

void r_glLightModelxvOES(void *context, GLenum pname, const GLfixed* param);
void d_glLightModelxvOES(void *context, GLenum pname, const GLfixed* param);

void r_glLightxvOES(void *context, GLenum light, GLenum pname, const GLfixed* params);
void d_glLightxvOES(void *context, GLenum light, GLenum pname, const GLfixed* params);

void r_glLoadMatrixxOES(void *context, const GLfixed* m);
void d_glLoadMatrixxOES(void *context, const GLfixed* m);

void r_glMaterialxvOES(void *context, GLenum face, GLenum pname, const GLfixed* param);
void d_glMaterialxvOES(void *context, GLenum face, GLenum pname, const GLfixed* param);

void r_glMultMatrixxOES(void *context, const GLfixed* m);
void d_glMultMatrixxOES(void *context, const GLfixed* m);

void r_glPointParameterxvOES(void *context, GLenum pname, const GLfixed* params);
void d_glPointParameterxvOES(void *context, GLenum pname, const GLfixed* params);

void r_glTexEnvxvOES(void *context, GLenum target, GLenum pname, const GLfixed* params);
void d_glTexEnvxvOES(void *context, GLenum target, GLenum pname, const GLfixed* params);

void r_glClipPlanefOES(void *context, GLenum plane, const GLfloat* equation);
void d_glClipPlanefOES(void *context, GLenum plane, const GLfloat* equation);

void r_glTexGenxvOES(void *context, GLenum coord, GLenum pname, const GLfixed* params);
void d_glTexGenxvOES(void *context, GLenum coord, GLenum pname, const GLfixed* params);

void r_glProgramUniform1iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform1iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);

void r_glProgramUniform2iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform2iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);

void r_glProgramUniform3iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform3iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);

void r_glProgramUniform4iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);
void d_glProgramUniform4iv(void *context, GLuint program, GLint location, GLsizei count, const GLint* value);

void r_glProgramUniform1uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform1uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);

void r_glProgramUniform2uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform2uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);

void r_glProgramUniform3uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform3uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);

void r_glProgramUniform4uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);
void d_glProgramUniform4uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint* value);

void r_glProgramUniform1fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform1fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);

void r_glProgramUniform2fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform2fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);

void r_glProgramUniform3fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform3fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);

void r_glProgramUniform4fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);
void d_glProgramUniform4fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat* value);

void r_glProgramUniformMatrix2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix2x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix2x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix3x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix3x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix2x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix2x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix4x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix4x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix3x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix3x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glProgramUniformMatrix4x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void d_glProgramUniformMatrix4x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

void r_glBindAttribLocation(void *context, GLuint program, GLuint index, const GLchar* name);
void d_glBindAttribLocation(void *context, GLuint program, GLuint index, const GLchar* name);

void r_glTexEnvf(void *context, GLenum target, GLenum pname, GLfloat param);
void d_glTexEnvf(void *context, GLenum target, GLenum pname, GLfloat param);

void r_glTexEnvi(void *context, GLenum target, GLenum pname, GLint param);
void d_glTexEnvi(void *context, GLenum target, GLenum pname, GLint param);

void r_glTexEnvx(void *context, GLenum target, GLenum pname, GLfixed param);
void d_glTexEnvx(void *context, GLenum target, GLenum pname, GLfixed param);

void r_glTexParameterx(void *context, GLenum target, GLenum pname, GLint param);
void d_glTexParameterx(void *context, GLenum target, GLenum pname, GLint param);

void r_glShadeModel(void *context, GLenum mode);
void d_glShadeModel(void *context, GLenum mode);

void r_glDrawTexiOES(void *context, GLint x, GLint y, GLint z, GLint width, GLint height);
void d_glDrawTexiOES(void *context, GLint x, GLint y, GLint z, GLint width, GLint height);

void r_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLsizei length, const void* pointer);
void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLsizei length, const void* pointer);

void r_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void* pointer);
void d_glVertexAttribPointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void* pointer);

void r_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices);
void d_glDrawElements(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices);

void r_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
void d_glDrawElementsInstanced(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);

void r_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
void d_glDrawRangeElements(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);

void r_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void* data);
void d_glFlushMappedBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void* data);

void r_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void* data, GLenum usage);
void d_glBufferData(void *context, GLenum target, GLsizeiptr size, const void* data, GLenum usage);

void r_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
void d_glBufferSubData(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void* data);

void r_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
void d_glCompressedTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);

void r_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
void d_glCompressedTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);

void r_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
void d_glCompressedTexImage3D(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);

void r_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
void d_glCompressedTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);

void r_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void* pixels);

void r_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexImage3D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void* pixels);

void r_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void* pixels);

void r_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void* pixels);
void d_glTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void* pixels);

void r_glPrintf(void *context, GLint buf_len, const GLchar* out_string);
void d_glPrintf(void *context, GLint buf_len, const GLchar* out_string);










#endif
