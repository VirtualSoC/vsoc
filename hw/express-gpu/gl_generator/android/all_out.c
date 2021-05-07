
#include "glad.h"
#include "glext.h"
#include "gl3.h"
#include "gl.h"

#include "define.h"

typedef khronos_int32_t GLclampx;

/******* file '1-1' *******/

/* readline: "glMapBufferRange_read GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf#length" */
/* func name: "glMapBufferRange_read" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'access', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'void*', 'name': 'mem_buf', 'ptr': 'out', 'ptr_len': 'length', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 24;
    unsigned char *save_buf;

    unsigned char local_save_buf[24];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    *(GLsizeiptr *)ptr = length;
    ptr += 8;

    *(GLbitfield *)ptr = access;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glMapBufferRange_read;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)mem_buf;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "GLenum glCheckFramebufferStatus GLenum target" */
/* func name: "glCheckFramebufferStatus" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLenum" */
/* type: "0" */

GLenum d_glCheckFramebufferStatus(void *context, GLenum target)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCheckFramebufferStatus;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLenum ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLenum)ret_long;
    return ret;
}

/* readline: "GLuint glCreateProgram void" */
/* func name: "glCreateProgram" */
/* args: [] */
/* ret: "GLuint" */
/* type: "0" */

GLuint d_glCreateProgram(void *context)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;

    send_buf_len -= 16;
    para_num -= 1;

    send_buf_len += 16;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCreateProgram;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    GLuint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLuint)ret_long;
    return ret;
}

/* readline: "GLuint glCreateShader GLenum type" */
/* func name: "glCreateShader" */
/* args: [{'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLuint" */
/* type: "0" */

GLuint d_glCreateShader(void *context, GLenum type)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCreateShader;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLuint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLuint)ret_long;
    return ret;
}

/* readline: "GLenum glGetError void" */
/* func name: "glGetError" */
/* args: [] */
/* ret: "GLenum" */
/* type: "0" */

GLenum d_glGetError(void *context)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;

    send_buf_len -= 16;
    para_num -= 1;

    send_buf_len += 16;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetError;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    GLenum ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLenum)ret_long;
    return ret;
}

/* readline: "GLboolean glIsBuffer GLuint buffer" */
/* func name: "glIsBuffer" */
/* args: [{'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsBuffer(void *context, GLuint buffer)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = buffer;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsBuffer;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsEnabled GLenum cap" */
/* func name: "glIsEnabled" */
/* args: [{'type': 'GLenum', 'name': 'cap', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsEnabled(void *context, GLenum cap)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = cap;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsEnabled;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsFramebuffer GLuint framebuffer" */
/* func name: "glIsFramebuffer" */
/* args: [{'type': 'GLuint', 'name': 'framebuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsFramebuffer(void *context, GLuint framebuffer)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = framebuffer;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsFramebuffer;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsProgram GLuint program" */
/* func name: "glIsProgram" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsProgram(void *context, GLuint program)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsProgram;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsRenderbuffer GLuint renderbuffer" */
/* func name: "glIsRenderbuffer" */
/* args: [{'type': 'GLuint', 'name': 'renderbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsRenderbuffer(void *context, GLuint renderbuffer)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = renderbuffer;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsRenderbuffer;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsShader GLuint shader" */
/* func name: "glIsShader" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsShader(void *context, GLuint shader)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsShader;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsTexture GLuint texture" */
/* func name: "glIsTexture" */
/* args: [{'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsTexture(void *context, GLuint texture)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = texture;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsTexture;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsQuery GLuint id" */
/* func name: "glIsQuery" */
/* args: [{'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsQuery(void *context, GLuint id)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = id;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsQuery;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsVertexArray GLuint array" */
/* func name: "glIsVertexArray" */
/* args: [{'type': 'GLuint', 'name': 'array', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsVertexArray(void *context, GLuint array)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = array;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsVertexArray;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsSampler GLuint sampler" */
/* func name: "glIsSampler" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsSampler(void *context, GLuint sampler)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsSampler;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLboolean glIsTransformFeedback GLuint id" */
/* func name: "glIsTransformFeedback" */
/* args: [{'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsTransformFeedback(void *context, GLuint id)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = id;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsTransformFeedback;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "GLint glGetAttribLocation GLuint program, const GLchar *name#strlen(name)+1" */
/* func name: "glGetAttribLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "0" */

GLint d_glGetAttribLocation(void *context, GLuint program, const GLchar *name)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetAttribLocation;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = strlen(name) + 1;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    GLint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLint)ret_long;
    return ret;
}

/* readline: "GLint glGetUniformLocation GLuint program, const GLchar *name#strlen(name)+1" */
/* func name: "glGetUniformLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "0" */

GLint d_glGetUniformLocation(void *context, GLuint program, const GLchar *name)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetUniformLocation;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = strlen(name) + 1;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    GLint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLint)ret_long;
    return ret;
}

/* readline: "GLint glGetFragDataLocation GLuint program, const GLchar *name#strlen(name)+1" */
/* func name: "glGetFragDataLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "0" */

GLint d_glGetFragDataLocation(void *context, GLuint program, const GLchar *name)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetFragDataLocation;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = strlen(name) + 1;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    GLint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLint)ret_long;
    return ret;
}

/* readline: "GLuint glGetUniformBlockIndex GLuint program, const GLchar *uniformBlockName#strlen(uniformBlockName)+1" */
/* func name: "glGetUniformBlockIndex" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'uniformBlockName', 'ptr': 'in', 'ptr_len': 'strlen(uniformBlockName)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLuint" */
/* type: "0" */

GLuint d_glGetUniformBlockIndex(void *context, GLuint program, const GLchar *uniformBlockName)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetUniformBlockIndex;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = strlen(uniformBlockName) + 1;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)uniformBlockName;
    ptr += sizeof(unsigned long);

    GLuint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLuint)ret_long;
    return ret;
}

/* readline: "GLuint glGetProgramResourceIndex GLuint program, GLenum programInterface, const GLchar *name#strlen(name)+1" */
/* func name: "glGetProgramResourceIndex" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLuint" */
/* type: "0" */

GLuint d_glGetProgramResourceIndex(void *context, GLuint program, GLenum programInterface, const GLchar *name)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = programInterface;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramResourceIndex;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = strlen(name) + 1;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    GLuint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLuint)ret_long;
    return ret;
}

/* readline: "GLint glGetProgramResourceLocation GLuint program, GLenum programInterface, const GLchar *name#strlen(name)+1" */
/* func name: "glGetProgramResourceLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "0" */

GLint d_glGetProgramResourceLocation(void *context, GLuint program, GLenum programInterface, const GLchar *name)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = programInterface;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramResourceLocation;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = strlen(name) + 1;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    GLint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLint)ret_long;
    return ret;
}

/* readline: "GLuint glCreateShaderProgramv GLenum type, GLsizei count, const GLchar *const*strings#count|strlen(strings[i])+1" */
/* func name: "glCreateShaderProgramv" */
/* args: [{'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'strings', 'ptr': 'in', 'ptr_len': 'count|strlen(strings[i])+1', 'loc': 2, 'ptr_ptr': True}] */
/* ret: "GLuint" */
/* type: "0" */

GLuint d_glCreateShaderProgramv(void *context, GLenum type, GLsizei count, const GLchar *const *strings)
{
    unsigned char *send_buf = (char *)malloc(32 + 16 + 16 * (0 + count));
    size_t send_buf_len = 32 + 16 * (0 + count);
    size_t para_num = 1 + 0 + count;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCreateShaderProgramv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    for (int i = 0; i < count; i++)
    {
        *(unsigned long *)ptr = (unsigned long)strlen(strings[i]) + 1;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)strings[i];
        ptr += sizeof(unsigned long);
    }

    GLuint ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    free(send_buf);

    ret = (GLuint)ret_long;
    return ret;
}

/* readline: "GLbitfield glQueryMatrixxOES GLfixed *mantissa#sizeof(GLint)*16, GLint *exponent#sizeof(GLint)*16" */
/* func name: "glQueryMatrixxOES" */
/* args: [{'type': 'GLfixed*', 'name': 'mantissa', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)*16', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'exponent', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)*16', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLbitfield" */
/* type: "0" */

GLbitfield d_glQueryMatrixxOES(void *context, GLfixed *mantissa, GLint *exponent)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;

    send_buf_len -= 16;
    para_num -= 1;

    send_buf_len += 16;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glQueryMatrixxOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint) * 16;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)mantissa;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint) * 16;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)exponent;
    ptr += sizeof(unsigned long);

    GLbitfield ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLbitfield)ret_long;
    return ret;
}

/* readline: "glGenBuffers GLsizei n, GLuint *buffers#n*sizeof(GLuint)" */
/* func name: "glGenBuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'buffers', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenBuffers(void *context, GLsizei n, GLuint *buffers)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenBuffers;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)buffers;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenFramebuffers GLsizei n, GLuint *framebuffers#n*sizeof(GLuint)" */
/* func name: "glGenFramebuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'framebuffers', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenFramebuffers(void *context, GLsizei n, GLuint *framebuffers)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenFramebuffers;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)framebuffers;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenRenderbuffers GLsizei n, GLuint *renderbuffers#n*sizeof(GLuint)" */
/* func name: "glGenRenderbuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'renderbuffers', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenRenderbuffers(void *context, GLsizei n, GLuint *renderbuffers)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenRenderbuffers;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)renderbuffers;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenTextures GLsizei n, GLuint *textures#n*sizeof(GLuint)" */
/* func name: "glGenTextures" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'textures', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenTextures(void *context, GLsizei n, GLuint *textures)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenTextures;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)textures;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetActiveAttrib GLuint program, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLint *size#sizeof(GLint), GLenum *type#sizeof(GLenum), GLchar *name#bufSize" */
/* func name: "glGetActiveAttrib" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'size', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'type', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveAttrib(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    unsigned char send_buf[32 + 16 + 4 * 16];
    size_t send_buf_len = 32 + 4 * 16;
    size_t para_num = 1 + 4;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetActiveAttrib;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)size;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLenum);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)type;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetActiveUniform GLuint program, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLint *size#sizeof(GLint), GLenum *type#sizeof(GLenum), GLchar *name#bufSize" */
/* func name: "glGetActiveUniform" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'size', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'type', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveUniform(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    unsigned char send_buf[32 + 16 + 4 * 16];
    size_t send_buf_len = 32 + 4 * 16;
    size_t para_num = 1 + 4;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetActiveUniform;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)size;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLenum);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)type;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetAttachedShaders GLuint program, GLsizei maxCount, GLsizei *count#sizeof(GLsizei), GLuint *shaders#maxCount*sizeof(GLuint)" */
/* func name: "glGetAttachedShaders" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'maxCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'count', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'shaders', 'ptr': 'out', 'ptr_len': 'maxCount*sizeof(GLuint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetAttachedShaders(void *context, GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = maxCount;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetAttachedShaders;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)count;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)maxCount * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)shaders;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetBooleanv GLenum pname, GLboolean *data#gl_pname_size(pname)*sizeof(GLboolean)" */
/* func name: "glGetBooleanv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLboolean)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetBooleanv(void *context, GLenum pname, GLboolean *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetBooleanv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLboolean);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetBufferParameteriv GLenum target, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetBufferParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetBufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetBufferParameteriv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetFloatv GLenum pname, GLfloat *data#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetFloatv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetFloatv(void *context, GLenum pname, GLfloat *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetFloatv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfloat);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetFramebufferAttachmentParameteriv GLenum target, GLenum attachment, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetFramebufferAttachmentParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetFramebufferAttachmentParameteriv(void *context, GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = attachment;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetFramebufferAttachmentParameteriv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetIntegerv GLenum pname, GLint *data#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetIntegerv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetIntegerv(void *context, GLenum pname, GLint *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetIntegerv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramiv GLuint program, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetProgramiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramiv(void *context, GLuint program, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramInfoLog GLuint program, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *infoLog#bufSize" */
/* func name: "glGetProgramInfoLog" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'infoLog', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramInfoLog(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramInfoLog;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)infoLog;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetRenderbufferParameteriv GLenum target, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetRenderbufferParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetRenderbufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetRenderbufferParameteriv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetShaderiv GLuint shader, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetShaderiv" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetShaderiv(void *context, GLuint shader, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetShaderiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetShaderInfoLog GLuint shader, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *infoLog#bufSize" */
/* func name: "glGetShaderInfoLog" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'infoLog', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetShaderInfoLog(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetShaderInfoLog;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)infoLog;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetShaderPrecisionFormat GLenum shadertype, GLenum precisiontype, GLint *range#2*sizeof(GLint), GLint *precision#sizeof(GLint)" */
/* func name: "glGetShaderPrecisionFormat" */
/* args: [{'type': 'GLenum', 'name': 'shadertype', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'precisiontype', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'range', 'ptr': 'out', 'ptr_len': '2*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'precision', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetShaderPrecisionFormat(void *context, GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = shadertype;
    ptr += 4;

    *(GLenum *)ptr = precisiontype;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetShaderPrecisionFormat;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)2 * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)range;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)precision;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetShaderSource GLuint shader, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *source#bufSize" */
/* func name: "glGetShaderSource" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'source', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetShaderSource(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetShaderSource;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)source;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTexParameterfv GLenum target, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetTexParameterfv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexParameterfv(void *context, GLenum target, GLenum pname, GLfloat *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTexParameterfv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfloat);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTexParameteriv GLenum target, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetTexParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTexParameteriv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetUniformfv GLuint program, GLint location, GLfloat *params#gl_get_program_uniform_size(context,program,location)" */
/* func name: "glGetUniformfv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_program_uniform_size(context,program,location)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetUniformfv(void *context, GLuint program, GLint location, GLfloat *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetUniformfv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_get_program_uniform_size(context, program, location);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetUniformiv GLuint program, GLint location, GLint *params#gl_get_program_uniform_size(context,program,location)" */
/* func name: "glGetUniformiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_program_uniform_size(context,program,location)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetUniformiv(void *context, GLuint program, GLint location, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetUniformiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_get_program_uniform_size(context, program, location);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetUniformuiv GLuint program, GLint location, GLuint *params#gl_get_program_uniform_size(context,program,location)" */
/* func name: "glGetUniformuiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_program_uniform_size(context,program,location)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetUniformuiv(void *context, GLuint program, GLint location, GLuint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetUniformuiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_get_program_uniform_size(context, program, location);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetUniformIndices GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames#uniformCount|strlen(uniformNames)+1, GLuint *uniformIndices#uniformCount*sizeof(GLuint)" */
/* func name: "glGetUniformIndices" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'uniformCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'uniformNames', 'ptr': 'in', 'ptr_len': 'uniformCount|strlen(uniformNames)+1', 'loc': 2, 'ptr_ptr': True}, {'type': 'GLuint*', 'name': 'uniformIndices', 'ptr': 'out', 'ptr_len': 'uniformCount*sizeof(GLuint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetUniformIndices(void *context, GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices)
{
    unsigned char *send_buf = (char *)malloc(32 + 16 + 16 * (1 + uniformCount));
    size_t send_buf_len = 32 + 16 * (1 + uniformCount);
    size_t para_num = 1 + 1 + uniformCount;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = uniformCount;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetUniformIndices;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    for (int i = 0; i < uniformCount; i++)
    {
        *(unsigned long *)ptr = (unsigned long)strlen(uniformNames) + 1;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)uniformNames[i];
        ptr += sizeof(unsigned long);
    }

    *(unsigned long *)ptr = (unsigned long)uniformCount * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)uniformIndices;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    free(send_buf);
}

/* readline: "glGetVertexAttribfv_origin GLuint index, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetVertexAttribfv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetVertexAttribfv_origin(void *context, GLuint index, GLenum pname, GLfloat *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetVertexAttribfv_origin;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfloat);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetVertexAttribiv_origin GLuint index, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetVertexAttribiv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetVertexAttribiv_origin(void *context, GLuint index, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetVertexAttribiv_origin;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glReadPixels_without_bound GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels#gl_pixel_data_size(context,width,height,format,type,1)" */
/* func name: "glReadPixels_without_bound" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'void*', 'name': 'pixels', 'ptr': 'out', 'ptr_len': 'gl_pixel_data_size(context,width,height,format,type,1)', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 24;
    unsigned char *save_buf;

    unsigned char local_save_buf[24];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glReadPixels_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pixel_data_size(context, width, height, format, type, 1);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pixels;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glReadPixels_with_bound GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glReadPixels_with_bound" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 32;
    unsigned char *save_buf;

    unsigned char local_save_buf[32];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLintptr *)ptr = pixels;
    ptr += 8;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glReadPixels_with_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenQueries GLsizei n, GLuint *ids#n*sizeof(GLuint)" */
/* func name: "glGenQueries" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'ids', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenQueries(void *context, GLsizei n, GLuint *ids)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenQueries;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)ids;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetQueryiv GLenum target, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetQueryiv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetQueryiv(void *context, GLenum target, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetQueryiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetQueryObjectuiv GLuint id, GLenum pname, GLuint *params#gl_pname_size(pname)*sizeof(GLuint)" */
/* func name: "glGetQueryObjectuiv" */
/* args: [{'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLuint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetQueryObjectuiv(void *context, GLuint id, GLenum pname, GLuint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = id;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetQueryObjectuiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenVertexArrays_origin GLsizei n, GLuint *arrays#n*sizeof(GLuint)" */
/* func name: "glGenVertexArrays_origin" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'arrays', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenVertexArrays_origin(void *context, GLsizei n, GLuint *arrays)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenVertexArrays_origin;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)arrays;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetIntegeri_v GLenum target, GLuint index, GLint *data#sizeof(GLint)" */
/* func name: "glGetIntegeri_v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetIntegeri_v(void *context, GLenum target, GLuint index, GLint *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetIntegeri_v;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTransformFeedbackVarying GLuint program, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLsizei *size#sizeof(GLsizei), GLenum *type#sizeof(GLenum), GLchar *name#bufSize" */
/* func name: "glGetTransformFeedbackVarying" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'size', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'type', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTransformFeedbackVarying(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
{
    unsigned char send_buf[32 + 16 + 4 * 16];
    size_t send_buf_len = 32 + 4 * 16;
    size_t para_num = 1 + 4;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTransformFeedbackVarying;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)size;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLenum);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)type;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetVertexAttribIiv_origin GLuint index, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetVertexAttribIiv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetVertexAttribIiv_origin(void *context, GLuint index, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetVertexAttribIiv_origin;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetVertexAttribIuiv_origin GLuint index, GLenum pname, GLuint *params#gl_pname_size(pname)*sizeof(GLuint)" */
/* func name: "glGetVertexAttribIuiv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLuint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetVertexAttribIuiv_origin(void *context, GLuint index, GLenum pname, GLuint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetVertexAttribIuiv_origin;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetActiveUniformsiv GLuint program, GLsizei uniformCount, const GLuint *uniformIndices#uniformCount*sizeof(GLuint), GLenum pname, GLint *params#uniformCount*sizeof(GLint)" */
/* func name: "glGetActiveUniformsiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'uniformCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'uniformIndices', 'ptr': 'in', 'ptr_len': 'uniformCount*sizeof(GLuint)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'uniformCount*sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveUniformsiv(void *context, GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = uniformCount;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetActiveUniformsiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = uniformCount * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)uniformIndices;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)uniformCount * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetActiveUniformBlockiv GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params#gl_get_uniform_block_para_size(context,program,uniformBlockIndex,pname)*sizeof(GLint)" */
/* func name: "glGetActiveUniformBlockiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockIndex', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_uniform_block_para_size(context,program,uniformBlockIndex,pname)*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveUniformBlockiv(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = uniformBlockIndex;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetActiveUniformBlockiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_get_uniform_block_para_size(context, program, uniformBlockIndex, pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetActiveUniformBlockName GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *uniformBlockName#bufSize" */
/* func name: "glGetActiveUniformBlockName" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockIndex', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'uniformBlockName', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveUniformBlockName(void *context, GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = uniformBlockIndex;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetActiveUniformBlockName;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)uniformBlockName;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetInteger64v GLenum pname, GLint64 *data#gl_pname_size(pname)*sizeof(GLint64)" */
/* func name: "glGetInteger64v" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint64*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint64)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetInteger64v(void *context, GLenum pname, GLint64 *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetInteger64v;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint64);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetInteger64i_v GLenum target, GLuint index, GLint64 *data#sizeof(GLint64)" */
/* func name: "glGetInteger64i_v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint64*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'sizeof(GLint64)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetInteger64i_v(void *context, GLenum target, GLuint index, GLint64 *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetInteger64i_v;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLint64);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetBufferParameteri64v GLenum target, GLenum pname, GLint64 *params#gl_pname_size(pname)*sizeof(GLint64)" */
/* func name: "glGetBufferParameteri64v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint64*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint64)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetBufferParameteri64v(void *context, GLenum target, GLenum pname, GLint64 *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetBufferParameteri64v;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint64);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenSamplers GLsizei count, GLuint *samplers#count*sizeof(GLuint)" */
/* func name: "glGenSamplers" */
/* args: [{'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'samplers', 'ptr': 'out', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenSamplers(void *context, GLsizei count, GLuint *samplers)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenSamplers;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)count * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)samplers;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetSamplerParameteriv GLuint sampler, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetSamplerParameteriv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetSamplerParameteriv(void *context, GLuint sampler, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetSamplerParameteriv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetSamplerParameterfv GLuint sampler, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetSamplerParameterfv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetSamplerParameterfv(void *context, GLuint sampler, GLenum pname, GLfloat *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetSamplerParameterfv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfloat);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenTransformFeedbacks GLsizei n, GLuint *ids#n*sizeof(GLuint)" */
/* func name: "glGenTransformFeedbacks" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'ids', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenTransformFeedbacks(void *context, GLsizei n, GLuint *ids)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenTransformFeedbacks;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)ids;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramBinary GLuint program, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLenum *binaryFormat#sizeof(GLenum), void *binary#bufSize" */
/* func name: "glGetProgramBinary" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'binaryFormat', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 3, 'ptr_ptr': False}, {'type': 'void*', 'name': 'binary', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramBinary(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary)
{
    unsigned char send_buf[32 + 16 + 3 * 16];
    size_t send_buf_len = 32 + 3 * 16;
    size_t para_num = 1 + 3;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramBinary;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLenum);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)binaryFormat;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)binary;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetInternalformativ GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params#count*sizeof(GLint)" */
/* func name: "glGetInternalformativ" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'count*sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetInternalformativ(void *context, GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetInternalformativ;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)count * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetClipPlanexOES GLenum plane, GLfixed *equation#4*sizeof(GLfixed)" */
/* func name: "glGetClipPlanexOES" */
/* args: [{'type': 'GLenum', 'name': 'plane', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'equation', 'ptr': 'out', 'ptr_len': '4*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetClipPlanexOES(void *context, GLenum plane, GLfixed *equation)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = plane;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetClipPlanexOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)4 * sizeof(GLfixed);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)equation;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetFixedvOES GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetFixedvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetFixedvOES(void *context, GLenum pname, GLfixed *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetFixedvOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfixed);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTexEnvxvOES GLenum target, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetTexEnvxvOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexEnvxvOES(void *context, GLenum target, GLenum pname, GLfixed *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTexEnvxvOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfixed);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTexParameterxvOES GLenum target, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetTexParameterxvOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexParameterxvOES(void *context, GLenum target, GLenum pname, GLfixed *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTexParameterxvOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfixed);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetLightxvOES GLenum light, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetLightxvOES" */
/* args: [{'type': 'GLenum', 'name': 'light', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetLightxvOES(void *context, GLenum light, GLenum pname, GLfixed *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = light;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetLightxvOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfixed);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetMaterialxvOES GLenum face, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetMaterialxvOES" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetMaterialxvOES(void *context, GLenum face, GLenum pname, GLfixed *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = face;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetMaterialxvOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfixed);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTexGenxvOES GLenum coord, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetTexGenxvOES" */
/* args: [{'type': 'GLenum', 'name': 'coord', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexGenxvOES(void *context, GLenum coord, GLenum pname, GLfixed *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = coord;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTexGenxvOES;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfixed);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glShaderSource GLuint shader, GLsizei count, const GLchar *const*string#count|strlen(string[i])+1, const GLint *length#count*sizeof(GLint)" */
/* func name: "glShaderSource" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'string', 'ptr': 'in', 'ptr_len': 'count|strlen(string[i])+1', 'loc': 2, 'ptr_ptr': True}, {'type': 'const GLint*', 'name': 'length', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glShaderSource(void *context, GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
    unsigned char *send_buf = (char *)malloc(32 + 16 + 16 * (0 + count + 1));
    size_t send_buf_len = 32 + 16 * (0 + count + 1);
    size_t para_num = 1 + 0 + count + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glShaderSource;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    for (int i = 0; i < count; i++)
    {
        *(unsigned long *)ptr = (unsigned long)strlen(string[i]) + 1;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)string[i];
        ptr += sizeof(unsigned long);
    }

    *(unsigned long *)ptr = count * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    free(send_buf);
}

/* readline: "glTransformFeedbackVaryings GLuint program, GLsizei count, const GLchar *const*varyings#count|strlen(varyings[i])+1, GLenum bufferMode" */
/* func name: "glTransformFeedbackVaryings" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'varyings', 'ptr': 'in', 'ptr_len': 'count|strlen(varyings[i])+1', 'loc': 2, 'ptr_ptr': True}, {'type': 'GLenum', 'name': 'bufferMode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTransformFeedbackVaryings(void *context, GLuint program, GLsizei count, const GLchar *const *varyings, GLenum bufferMode)
{
    unsigned char *send_buf = (char *)malloc(32 + 16 + 16 * (0 + count));
    size_t send_buf_len = 32 + 16 * (0 + count);
    size_t para_num = 1 + 0 + count;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = bufferMode;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glTransformFeedbackVaryings;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    for (int i = 0; i < count; i++)
    {
        *(unsigned long *)ptr = (unsigned long)strlen(varyings[i]) + 1;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)varyings[i];
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    free(send_buf);
}

/* readline: "glGetFramebufferParameteriv GLenum target, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetFramebufferParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetFramebufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetFramebufferParameteriv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramInterfaceiv GLuint program, GLenum programInterface, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetProgramInterfaceiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramInterfaceiv(void *context, GLuint program, GLenum programInterface, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = programInterface;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramInterfaceiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramResourceName GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *name#bufSize" */
/* func name: "glGetProgramResourceName" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramResourceName(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = programInterface;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramResourceName;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramResourceiv GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props#propCount*sizeof(GLenum), GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLint *params#bufSize*sizeof(GLint)" */
/* func name: "glGetProgramResourceiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'propCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'props', 'ptr': 'in', 'ptr_len': 'propCount*sizeof(GLenum)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'bufSize*sizeof(GLint)', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramResourceiv(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params)
{
    unsigned char send_buf[32 + 16 + 3 * 16];
    size_t send_buf_len = 32 + 3 * 16;
    size_t para_num = 1 + 3;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 20;
    unsigned char *save_buf;

    unsigned char local_save_buf[20];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = programInterface;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLsizei *)ptr = propCount;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramResourceiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = propCount * sizeof(GLenum);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)props;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGenProgramPipelines GLsizei n, GLuint *pipelines#n*sizeof(GLuint)" */
/* func name: "glGenProgramPipelines" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'pipelines', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGenProgramPipelines(void *context, GLsizei n, GLuint *pipelines)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGenProgramPipelines;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)n * sizeof(GLuint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pipelines;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramPipelineiv GLuint pipeline, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetProgramPipelineiv" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramPipelineiv(void *context, GLuint pipeline, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = pipeline;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramPipelineiv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetProgramPipelineInfoLog GLuint pipeline, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *infoLog#bufSize" */
/* func name: "glGetProgramPipelineInfoLog" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'infoLog', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramPipelineInfoLog(void *context, GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = pipeline;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetProgramPipelineInfoLog;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)bufSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)infoLog;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetBooleani_v GLenum target, GLuint index, GLboolean *data#sizeof(GLboolean)" */
/* func name: "glGetBooleani_v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'sizeof(GLboolean)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetBooleani_v(void *context, GLenum target, GLuint index, GLboolean *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetBooleani_v;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLboolean);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetMultisamplefv GLenum pname, GLuint index, GLfloat *val#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetMultisamplefv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'val', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetMultisamplefv(void *context, GLenum pname, GLuint index, GLfloat *val)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetMultisamplefv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfloat);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)val;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTexLevelParameteriv GLenum target, GLint level, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetTexLevelParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexLevelParameteriv(void *context, GLenum target, GLint level, GLenum pname, GLint *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTexLevelParameteriv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetTexLevelParameterfv GLenum target, GLint level, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetTexLevelParameterfv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexLevelParameterfv(void *context, GLenum target, GLint level, GLenum pname, GLfloat *params)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetTexLevelParameterfv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)gl_pname_size(pname) * sizeof(GLfloat);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)params;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "GLsync glFenceSync GLenum condition, GLbitfield flags" */
/* func name: "glFenceSync" */
/* args: [{'type': 'GLenum', 'name': 'condition', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLsync" */
/* type: "0" */

GLsync d_glFenceSync(void *context, GLenum condition, GLbitfield flags)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = condition;
    ptr += 4;

    *(GLbitfield *)ptr = flags;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glFenceSync;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLsync ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLsync)ret_long;
    return ret;
}

/* readline: "GLboolean glIsSync GLsync sync" */
/* func name: "glIsSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsSync(void *context, GLsync sync)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glIsSync;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "glGetSynciv GLsync sync, GLenum pname, GLsizei count, GLsizei *length#sizeof(GLsizei), GLint *values#count*sizeof(GLint)" */
/* func name: "glGetSynciv" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'values', 'ptr': 'out', 'ptr_len': 'count*sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetSynciv(void *context, GLsync sync, GLenum pname, GLsizei count, GLsizei *length, GLint *values)
{
    unsigned char send_buf[32 + 16 + 2 * 16];
    size_t send_buf_len = 32 + 2 * 16;
    size_t para_num = 1 + 2;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glGetSynciv;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)sizeof(GLsizei);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)count * sizeof(GLint);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)values;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "GLenum glClientWaitSync GLsync sync, GLbitfield flags, GLuint64 timeout" */
/* func name: "glClientWaitSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint64', 'name': 'timeout', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLenum" */
/* type: "0" */

GLenum d_glClientWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 20;
    unsigned char *save_buf;

    unsigned char local_save_buf[20];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    *(GLbitfield *)ptr = flags;
    ptr += 4;

    *(GLuint64 *)ptr = timeout;
    ptr += 8;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glClientWaitSync;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLenum ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLenum)ret_long;
    return ret;
}

/* readline: "glFinish void" */
/* func name: "glFinish" */
/* args: [] */
/* ret: "" */
/* type: "0" */

void d_glFinish(void *context)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;

    send_buf_len -= 16;
    para_num -= 1;

    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glFinish;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "GLboolean glUnmapBuffer_special GLenum target" */
/* func name: "glUnmapBuffer_special" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glUnmapBuffer_special(void *context, GLenum target)
{
    unsigned char send_buf[32 + 16 + 0 * 16];
    size_t send_buf_len = 32 + 0 * 16;
    size_t para_num = 1 + 0;
    send_buf_len += 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glUnmapBuffer_special;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    GLboolean ret = 0;
    unsigned long long ret_long = 0;
    *(unsigned long *)ptr = (unsigned long)sizeof(unsigned long long);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long long)&ret_long;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);

    ret = (GLboolean)ret_long;
    return ret;
}

/* readline: "glBindAttribLocation GLuint program, GLuint index, const GLchar *name#strlen(name)+1" */
/* func name: "glBindAttribLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glBindAttribLocation(void *context, GLuint program, GLuint index, const GLchar *name)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glBindAttribLocation;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = strlen(name) + 1;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)name;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glBufferData_custom GLenum target, GLsizeiptr size, const void *data#size, GLenum usage" */
/* func name: "glBufferData_custom" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'size', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'usage', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLsizeiptr *)ptr = size;
    ptr += 8;

    *(GLenum *)ptr = usage;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glBufferData_custom;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = size;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glBufferSubData_custom GLenum target, GLintptr offset, GLsizeiptr size, const void *data#size" */
/* func name: "glBufferSubData_custom" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'size', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 20;
    unsigned char *save_buf;

    unsigned char local_save_buf[20];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    *(GLsizeiptr *)ptr = size;
    ptr += 8;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glBufferSubData_custom;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = size;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexImage2D_without_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data#imageSize" */
/* func name: "glCompressedTexImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 28;
    unsigned char *save_buf;

    unsigned char local_save_buf[28];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCompressedTexImage2D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = imageSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexSubImage2D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data#imageSize" */
/* func name: "glCompressedTexSubImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 32;
    unsigned char *save_buf;

    unsigned char local_save_buf[32];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCompressedTexSubImage2D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = imageSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glDrawElements_without_bound GLenum mode, GLsizei count, GLenum type, const void *indices#count*gl_sizeof(type)" */
/* func name: "glDrawElements_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'indices', 'ptr': 'in', 'ptr_len': 'count*gl_sizeof(type)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glDrawElements_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = count * gl_sizeof(type);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)indices;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexImage2D_without_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels#gl_pixel_data_size(context,width,height,format,type,0)" */
/* func name: "glTexImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'gl_pixel_data_size(context,width,height,format,type,0)', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 32;
    unsigned char *save_buf;

    unsigned char local_save_buf[32];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glTexImage2D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = gl_pixel_data_size(context, width, height, format, type, 0);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pixels;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glDrawRangeElements_without_bound GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices#count*gl_sizeof(type)" */
/* func name: "glDrawRangeElements_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'start', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'end', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'indices', 'ptr': 'in', 'ptr_len': 'count*gl_sizeof(type)', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 20;
    unsigned char *save_buf;

    unsigned char local_save_buf[20];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLuint *)ptr = start;
    ptr += 4;

    *(GLuint *)ptr = end;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glDrawRangeElements_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = count * gl_sizeof(type);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)indices;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexImage3D_without_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data#imageSize" */
/* func name: "glCompressedTexImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 32;
    unsigned char *save_buf;

    unsigned char local_save_buf[32];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCompressedTexImage3D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = imageSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexSubImage3D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data#imageSize" */
/* func name: "glCompressedTexSubImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 40;
    unsigned char *save_buf;

    unsigned char local_save_buf[40];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLint *)ptr = zoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glCompressedTexSubImage3D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = imageSize;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)data;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexSubImage2D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels#gl_pixel_data_size(context,width,height,format,type,0)" */
/* func name: "glTexSubImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'gl_pixel_data_size(context,width,height,format,type,0)', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 32;
    unsigned char *save_buf;

    unsigned char local_save_buf[32];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glTexSubImage2D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = gl_pixel_data_size(context, width, height, format, type, 0);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pixels;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexImage3D_without_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels#gl_pixel_data_3d_size(context,width,height,depth,format,type,0)" */
/* func name: "glTexImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'gl_pixel_data_3d_size(context,width,height,depth,format,type,0)', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 36;
    unsigned char *save_buf;

    unsigned char local_save_buf[36];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glTexImage3D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = gl_pixel_data_3d_size(context, width, height, depth, format, type, 0);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pixels;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexSubImage3D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels#gl_pixel_data_3d_size(context,width,height,depth,format,type,0)" */
/* func name: "glTexSubImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'gl_pixel_data_3d_size(context,width,height,depth,format,type,0)', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 40;
    unsigned char *save_buf;

    unsigned char local_save_buf[40];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLint *)ptr = zoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glTexSubImage3D_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = gl_pixel_data_3d_size(context, width, height, depth, format, type, 0);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pixels;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glDrawElementsInstanced_without_bound GLenum mode, GLsizei count, GLenum type, const void *indices#count*gl_sizeof(type), GLsizei instancecount" */
/* func name: "glDrawElementsInstanced_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'indices', 'ptr': 'in', 'ptr_len': 'count*gl_sizeof(type)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'instancecount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{
    unsigned char send_buf[32 + 16 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    size_t para_num = 1 + 1;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizei *)ptr = instancecount;
    ptr += 4;

    ptr = send_buf;
    *(unsigned long *)ptr = FUNID_glDrawElementsInstanced_without_bound;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)para_num;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = count * gl_sizeof(type);
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)indices;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/******* end of file '1-1', 113/113 functions *******/

/******* file '1-2' *******/

/******* end of file '1-2', 0/113 functions *******/

/******* file '2-1' *******/

/* readline: "glActiveTexture GLenum texture" */
/* func name: "glActiveTexture" */
/* args: [{'type': 'GLenum', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glActiveTexture(void *context, GLenum texture)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = texture;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glActiveTexture;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glAttachShader GLuint program, GLuint shader" */
/* func name: "glAttachShader" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glAttachShader(void *context, GLuint program, GLuint shader)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glAttachShader;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindBuffer_origin GLenum target, GLuint buffer" */
/* func name: "glBindBuffer_origin" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = buffer;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindBuffer_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindFramebuffer GLenum target, GLuint framebuffer" */
/* func name: "glBindFramebuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'framebuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindFramebuffer(void *context, GLenum target, GLuint framebuffer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = framebuffer;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindFramebuffer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindRenderbuffer GLenum target, GLuint renderbuffer" */
/* func name: "glBindRenderbuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'renderbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindRenderbuffer(void *context, GLenum target, GLuint renderbuffer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = renderbuffer;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindRenderbuffer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindTexture GLenum target, GLuint texture" */
/* func name: "glBindTexture" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindTexture(void *context, GLenum target, GLuint texture)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = texture;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindTexture;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBlendColor GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha" */
/* func name: "glBlendColor" */
/* args: [{'type': 'GLfloat', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBlendColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = red;
    ptr += 4;

    *(GLfloat *)ptr = green;
    ptr += 4;

    *(GLfloat *)ptr = blue;
    ptr += 4;

    *(GLfloat *)ptr = alpha;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBlendColor;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBlendEquation GLenum mode" */
/* func name: "glBlendEquation" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBlendEquation(void *context, GLenum mode)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBlendEquation;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBlendEquationSeparate GLenum modeRGB, GLenum modeAlpha" */
/* func name: "glBlendEquationSeparate" */
/* args: [{'type': 'GLenum', 'name': 'modeRGB', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'modeAlpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBlendEquationSeparate(void *context, GLenum modeRGB, GLenum modeAlpha)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = modeRGB;
    ptr += 4;

    *(GLenum *)ptr = modeAlpha;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBlendEquationSeparate;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBlendFunc GLenum sfactor, GLenum dfactor" */
/* func name: "glBlendFunc" */
/* args: [{'type': 'GLenum', 'name': 'sfactor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dfactor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBlendFunc(void *context, GLenum sfactor, GLenum dfactor)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = sfactor;
    ptr += 4;

    *(GLenum *)ptr = dfactor;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBlendFunc;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBlendFuncSeparate GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha" */
/* func name: "glBlendFuncSeparate" */
/* args: [{'type': 'GLenum', 'name': 'sfactorRGB', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dfactorRGB', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'sfactorAlpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dfactorAlpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBlendFuncSeparate(void *context, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = sfactorRGB;
    ptr += 4;

    *(GLenum *)ptr = dfactorRGB;
    ptr += 4;

    *(GLenum *)ptr = sfactorAlpha;
    ptr += 4;

    *(GLenum *)ptr = dfactorAlpha;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBlendFuncSeparate;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClear GLbitfield mask" */
/* func name: "glClear" */
/* args: [{'type': 'GLbitfield', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClear(void *context, GLbitfield mask)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLbitfield *)ptr = mask;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClear;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearColor GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha" */
/* func name: "glClearColor" */
/* args: [{'type': 'GLfloat', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = red;
    ptr += 4;

    *(GLfloat *)ptr = green;
    ptr += 4;

    *(GLfloat *)ptr = blue;
    ptr += 4;

    *(GLfloat *)ptr = alpha;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearColor;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearDepthf GLfloat d" */
/* func name: "glClearDepthf" */
/* args: [{'type': 'GLfloat', 'name': 'd', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearDepthf(void *context, GLfloat d)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = d;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearDepthf;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearStencil GLint s" */
/* func name: "glClearStencil" */
/* args: [{'type': 'GLint', 'name': 's', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearStencil(void *context, GLint s)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = s;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearStencil;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glColorMask GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha" */
/* func name: "glColorMask" */
/* args: [{'type': 'GLboolean', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glColorMask(void *context, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLboolean *)ptr = red;
    ptr += 4;

    *(GLboolean *)ptr = green;
    ptr += 4;

    *(GLboolean *)ptr = blue;
    ptr += 4;

    *(GLboolean *)ptr = alpha;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glColorMask;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCompileShader GLuint shader" */
/* func name: "glCompileShader" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCompileShader(void *context, GLuint shader)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCompileShader;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCopyTexImage2D GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border" */
/* func name: "glCopyTexImage2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCopyTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 32;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCopyTexImage2D;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCopyTexSubImage2D GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glCopyTexSubImage2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCopyTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 32;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCopyTexSubImage2D;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCullFace GLenum mode" */
/* func name: "glCullFace" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCullFace(void *context, GLenum mode)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCullFace;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteProgram_origin GLuint program" */
/* func name: "glDeleteProgram_origin" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteProgram_origin(void *context, GLuint program)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteProgram_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteShader GLuint shader" */
/* func name: "glDeleteShader" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteShader(void *context, GLuint shader)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteShader;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDepthFunc GLenum func" */
/* func name: "glDepthFunc" */
/* args: [{'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDepthFunc(void *context, GLenum func)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = func;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDepthFunc;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDepthMask GLboolean flag" */
/* func name: "glDepthMask" */
/* args: [{'type': 'GLboolean', 'name': 'flag', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDepthMask(void *context, GLboolean flag)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLboolean *)ptr = flag;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDepthMask;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDepthRangef GLfloat n, GLfloat f" */
/* func name: "glDepthRangef" */
/* args: [{'type': 'GLfloat', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDepthRangef(void *context, GLfloat n, GLfloat f)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = n;
    ptr += 4;

    *(GLfloat *)ptr = f;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDepthRangef;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDetachShader GLuint program, GLuint shader" */
/* func name: "glDetachShader" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDetachShader(void *context, GLuint program, GLuint shader)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDetachShader;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDisable GLenum cap" */
/* func name: "glDisable" */
/* args: [{'type': 'GLenum', 'name': 'cap', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDisable(void *context, GLenum cap)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = cap;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDisable;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDisableVertexAttribArray_origin GLuint index" */
/* func name: "glDisableVertexAttribArray_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDisableVertexAttribArray_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDrawArrays_origin GLenum mode, GLint first, GLsizei count" */
/* func name: "glDrawArrays_origin" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'first', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLint *)ptr = first;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDrawArrays_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glEnable GLenum cap" */
/* func name: "glEnable" */
/* args: [{'type': 'GLenum', 'name': 'cap', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glEnable(void *context, GLenum cap)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = cap;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glEnable;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glEnableVertexAttribArray_origin GLuint index" */
/* func name: "glEnableVertexAttribArray_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glEnableVertexAttribArray_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFlush void" */
/* func name: "glFlush" */
/* args: [] */
/* ret: "" */
/* type: "2" */

void d_glFlush(void *context)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 0;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFlush;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFramebufferRenderbuffer GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer" */
/* func name: "glFramebufferRenderbuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'renderbuffertarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'renderbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFramebufferRenderbuffer(void *context, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = attachment;
    ptr += 4;

    *(GLenum *)ptr = renderbuffertarget;
    ptr += 4;

    *(GLuint *)ptr = renderbuffer;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFramebufferRenderbuffer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFramebufferTexture2D GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level" */
/* func name: "glFramebufferTexture2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'textarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFramebufferTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = attachment;
    ptr += 4;

    *(GLenum *)ptr = textarget;
    ptr += 4;

    *(GLuint *)ptr = texture;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFramebufferTexture2D;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFrontFace GLenum mode" */
/* func name: "glFrontFace" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFrontFace(void *context, GLenum mode)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFrontFace;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glGenerateMipmap GLenum target" */
/* func name: "glGenerateMipmap" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGenerateMipmap(void *context, GLenum target)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glGenerateMipmap;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glHint GLenum target, GLenum mode" */
/* func name: "glHint" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glHint(void *context, GLenum target, GLenum mode)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = mode;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glHint;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLineWidth GLfloat width" */
/* func name: "glLineWidth" */
/* args: [{'type': 'GLfloat', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLineWidth(void *context, GLfloat width)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = width;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLineWidth;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLinkProgram_origin GLuint program" */
/* func name: "glLinkProgram_origin" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLinkProgram_origin(void *context, GLuint program)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLinkProgram_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glPixelStorei_origin GLenum pname, GLint param" */
/* func name: "glPixelStorei_origin" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLint *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glPixelStorei_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glPolygonOffset GLfloat factor, GLfloat units" */
/* func name: "glPolygonOffset" */
/* args: [{'type': 'GLfloat', 'name': 'factor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'units', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glPolygonOffset(void *context, GLfloat factor, GLfloat units)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = factor;
    ptr += 4;

    *(GLfloat *)ptr = units;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glPolygonOffset;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glReleaseShaderCompiler void" */
/* func name: "glReleaseShaderCompiler" */
/* args: [] */
/* ret: "" */
/* type: "2" */

void d_glReleaseShaderCompiler(void *context)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 0;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glReleaseShaderCompiler;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glRenderbufferStorage GLenum target, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glRenderbufferStorage" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glRenderbufferStorage(void *context, GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glRenderbufferStorage;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glSampleCoverage GLfloat value, GLboolean invert" */
/* func name: "glSampleCoverage" */
/* args: [{'type': 'GLfloat', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'invert', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glSampleCoverage(void *context, GLfloat value, GLboolean invert)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = value;
    ptr += 4;

    *(GLboolean *)ptr = invert;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glSampleCoverage;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glScissor GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glScissor" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glScissor(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glScissor;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glStencilFunc GLenum func, GLint ref, GLuint mask" */
/* func name: "glStencilFunc" */
/* args: [{'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'ref', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glStencilFunc(void *context, GLenum func, GLint ref, GLuint mask)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = func;
    ptr += 4;

    *(GLint *)ptr = ref;
    ptr += 4;

    *(GLuint *)ptr = mask;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glStencilFunc;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glStencilFuncSeparate GLenum face, GLenum func, GLint ref, GLuint mask" */
/* func name: "glStencilFuncSeparate" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'ref', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glStencilFuncSeparate(void *context, GLenum face, GLenum func, GLint ref, GLuint mask)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = face;
    ptr += 4;

    *(GLenum *)ptr = func;
    ptr += 4;

    *(GLint *)ptr = ref;
    ptr += 4;

    *(GLuint *)ptr = mask;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glStencilFuncSeparate;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glStencilMask GLuint mask" */
/* func name: "glStencilMask" */
/* args: [{'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glStencilMask(void *context, GLuint mask)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = mask;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glStencilMask;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glStencilMaskSeparate GLenum face, GLuint mask" */
/* func name: "glStencilMaskSeparate" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glStencilMaskSeparate(void *context, GLenum face, GLuint mask)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = face;
    ptr += 4;

    *(GLuint *)ptr = mask;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glStencilMaskSeparate;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glStencilOp GLenum fail, GLenum zfail, GLenum zpass" */
/* func name: "glStencilOp" */
/* args: [{'type': 'GLenum', 'name': 'fail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'zfail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'zpass', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glStencilOp(void *context, GLenum fail, GLenum zfail, GLenum zpass)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = fail;
    ptr += 4;

    *(GLenum *)ptr = zfail;
    ptr += 4;

    *(GLenum *)ptr = zpass;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glStencilOp;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glStencilOpSeparate GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass" */
/* func name: "glStencilOpSeparate" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'sfail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dpfail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dppass', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glStencilOpSeparate(void *context, GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = face;
    ptr += 4;

    *(GLenum *)ptr = sfail;
    ptr += 4;

    *(GLenum *)ptr = dpfail;
    ptr += 4;

    *(GLenum *)ptr = dppass;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glStencilOpSeparate;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameterf GLenum target, GLenum pname, GLfloat param" */
/* func name: "glTexParameterf" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexParameterf(void *context, GLenum target, GLenum pname, GLfloat param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfloat *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexParameterf;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameteri GLenum target, GLenum pname, GLint param" */
/* func name: "glTexParameteri" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexParameteri(void *context, GLenum target, GLenum pname, GLint param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLint *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexParameteri;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1f GLint location, GLfloat v0" */
/* func name: "glUniform1f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform1f(void *context, GLint location, GLfloat v0)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform1f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1i GLint location, GLint v0" */
/* func name: "glUniform1i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform1i(void *context, GLint location, GLint v0)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform1i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2f GLint location, GLfloat v0, GLfloat v1" */
/* func name: "glUniform2f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform2f(void *context, GLint location, GLfloat v0, GLfloat v1)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    *(GLfloat *)ptr = v1;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform2f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2i GLint location, GLint v0, GLint v1" */
/* func name: "glUniform2i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform2i(void *context, GLint location, GLint v0, GLint v1)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    *(GLint *)ptr = v1;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform2i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3f GLint location, GLfloat v0, GLfloat v1, GLfloat v2" */
/* func name: "glUniform3f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform3f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    *(GLfloat *)ptr = v1;
    ptr += 4;

    *(GLfloat *)ptr = v2;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform3f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3i GLint location, GLint v0, GLint v1, GLint v2" */
/* func name: "glUniform3i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform3i(void *context, GLint location, GLint v0, GLint v1, GLint v2)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    *(GLint *)ptr = v1;
    ptr += 4;

    *(GLint *)ptr = v2;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform3i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4f GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3" */
/* func name: "glUniform4f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform4f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    *(GLfloat *)ptr = v1;
    ptr += 4;

    *(GLfloat *)ptr = v2;
    ptr += 4;

    *(GLfloat *)ptr = v3;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform4f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4i GLint location, GLint v0, GLint v1, GLint v2, GLint v3" */
/* func name: "glUniform4i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform4i(void *context, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    *(GLint *)ptr = v1;
    ptr += 4;

    *(GLint *)ptr = v2;
    ptr += 4;

    *(GLint *)ptr = v3;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform4i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUseProgram GLuint program" */
/* func name: "glUseProgram" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUseProgram(void *context, GLuint program)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUseProgram;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glValidateProgram GLuint program" */
/* func name: "glValidateProgram" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glValidateProgram(void *context, GLuint program)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glValidateProgram;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib1f GLuint index, GLfloat x" */
/* func name: "glVertexAttrib1f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib1f(void *context, GLuint index, GLfloat x)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLfloat *)ptr = x;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib1f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib2f GLuint index, GLfloat x, GLfloat y" */
/* func name: "glVertexAttrib2f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib2f(void *context, GLuint index, GLfloat x, GLfloat y)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLfloat *)ptr = x;
    ptr += 4;

    *(GLfloat *)ptr = y;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib2f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib3f GLuint index, GLfloat x, GLfloat y, GLfloat z" */
/* func name: "glVertexAttrib3f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib3f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLfloat *)ptr = x;
    ptr += 4;

    *(GLfloat *)ptr = y;
    ptr += 4;

    *(GLfloat *)ptr = z;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib3f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib4f GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w" */
/* func name: "glVertexAttrib4f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'w', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib4f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLfloat *)ptr = x;
    ptr += 4;

    *(GLfloat *)ptr = y;
    ptr += 4;

    *(GLfloat *)ptr = z;
    ptr += 4;

    *(GLfloat *)ptr = w;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib4f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glViewport GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glViewport" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glViewport(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glViewport;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glReadBuffer GLenum src" */
/* func name: "glReadBuffer" */
/* args: [{'type': 'GLenum', 'name': 'src', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glReadBuffer(void *context, GLenum src)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = src;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glReadBuffer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCopyTexSubImage3D GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glCopyTexSubImage3D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCopyTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 36;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLint *)ptr = zoffset;
    ptr += 4;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCopyTexSubImage3D;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBeginQuery GLenum target, GLuint id" */
/* func name: "glBeginQuery" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBeginQuery(void *context, GLenum target, GLuint id)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = id;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBeginQuery;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glEndQuery GLenum target" */
/* func name: "glEndQuery" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glEndQuery(void *context, GLenum target)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glEndQuery;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBlitFramebuffer GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter" */
/* func name: "glBlitFramebuffer" */
/* args: [{'type': 'GLint', 'name': 'srcX0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'srcY0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'srcX1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'srcY1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstX0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstY0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstX1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstY1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'filter', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBlitFramebuffer(void *context, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 40;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = srcX0;
    ptr += 4;

    *(GLint *)ptr = srcY0;
    ptr += 4;

    *(GLint *)ptr = srcX1;
    ptr += 4;

    *(GLint *)ptr = srcY1;
    ptr += 4;

    *(GLint *)ptr = dstX0;
    ptr += 4;

    *(GLint *)ptr = dstY0;
    ptr += 4;

    *(GLint *)ptr = dstX1;
    ptr += 4;

    *(GLint *)ptr = dstY1;
    ptr += 4;

    *(GLbitfield *)ptr = mask;
    ptr += 4;

    *(GLenum *)ptr = filter;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBlitFramebuffer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glRenderbufferStorageMultisample GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glRenderbufferStorageMultisample" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'samples', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glRenderbufferStorageMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLsizei *)ptr = samples;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glRenderbufferStorageMultisample;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFramebufferTextureLayer GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer" */
/* func name: "glFramebufferTextureLayer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'layer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFramebufferTextureLayer(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = attachment;
    ptr += 4;

    *(GLuint *)ptr = texture;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = layer;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFramebufferTextureLayer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindVertexArray_origin GLuint array" */
/* func name: "glBindVertexArray_origin" */
/* args: [{'type': 'GLuint', 'name': 'array', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindVertexArray_origin(void *context, GLuint array)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = array;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindVertexArray_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBeginTransformFeedback GLenum primitiveMode" */
/* func name: "glBeginTransformFeedback" */
/* args: [{'type': 'GLenum', 'name': 'primitiveMode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBeginTransformFeedback(void *context, GLenum primitiveMode)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = primitiveMode;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBeginTransformFeedback;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glEndTransformFeedback void" */
/* func name: "glEndTransformFeedback" */
/* args: [] */
/* ret: "" */
/* type: "2" */

void d_glEndTransformFeedback(void *context)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 0;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glEndTransformFeedback;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindBufferRange GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size" */
/* func name: "glBindBufferRange" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindBufferRange(void *context, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 28;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLuint *)ptr = buffer;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    *(GLsizeiptr *)ptr = size;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindBufferRange;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindBufferBase GLenum target, GLuint index, GLuint buffer" */
/* func name: "glBindBufferBase" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindBufferBase(void *context, GLenum target, GLuint index, GLuint buffer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLuint *)ptr = buffer;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindBufferBase;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4i GLuint index, GLint x, GLint y, GLint z, GLint w" */
/* func name: "glVertexAttribI4i" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'w', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribI4i(void *context, GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLint *)ptr = z;
    ptr += 4;

    *(GLint *)ptr = w;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribI4i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4ui GLuint index, GLuint x, GLuint y, GLuint z, GLuint w" */
/* func name: "glVertexAttribI4ui" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'w', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribI4ui(void *context, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLuint *)ptr = x;
    ptr += 4;

    *(GLuint *)ptr = y;
    ptr += 4;

    *(GLuint *)ptr = z;
    ptr += 4;

    *(GLuint *)ptr = w;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribI4ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1ui GLint location, GLuint v0" */
/* func name: "glUniform1ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform1ui(void *context, GLint location, GLuint v0)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform1ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2ui GLint location, GLuint v0, GLuint v1" */
/* func name: "glUniform2ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform2ui(void *context, GLint location, GLuint v0, GLuint v1)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    *(GLuint *)ptr = v1;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform2ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3ui GLint location, GLuint v0, GLuint v1, GLuint v2" */
/* func name: "glUniform3ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform3ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    *(GLuint *)ptr = v1;
    ptr += 4;

    *(GLuint *)ptr = v2;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform3ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4ui GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3" */
/* func name: "glUniform4ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform4ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    *(GLuint *)ptr = v1;
    ptr += 4;

    *(GLuint *)ptr = v2;
    ptr += 4;

    *(GLuint *)ptr = v3;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform4ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferfi GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil" */
/* func name: "glClearBufferfi" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'stencil', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearBufferfi(void *context, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = buffer;
    ptr += 4;

    *(GLint *)ptr = drawbuffer;
    ptr += 4;

    *(GLfloat *)ptr = depth;
    ptr += 4;

    *(GLint *)ptr = stencil;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearBufferfi;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCopyBufferSubData GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size" */
/* func name: "glCopyBufferSubData" */
/* args: [{'type': 'GLenum', 'name': 'readTarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'writeTarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'readOffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'writeOffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCopyBufferSubData(void *context, GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 32;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = readTarget;
    ptr += 4;

    *(GLenum *)ptr = writeTarget;
    ptr += 4;

    *(GLintptr *)ptr = readOffset;
    ptr += 8;

    *(GLintptr *)ptr = writeOffset;
    ptr += 8;

    *(GLsizeiptr *)ptr = size;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCopyBufferSubData;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformBlockBinding GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding" */
/* func name: "glUniformBlockBinding" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockIndex', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockBinding', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformBlockBinding(void *context, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = uniformBlockIndex;
    ptr += 4;

    *(GLuint *)ptr = uniformBlockBinding;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformBlockBinding;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDrawArraysInstanced_origin GLenum mode, GLint first, GLsizei count, GLsizei instancecount" */
/* func name: "glDrawArraysInstanced_origin" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'first', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'instancecount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLint *)ptr = first;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLsizei *)ptr = instancecount;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDrawArraysInstanced_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindSampler GLuint unit, GLuint sampler" */
/* func name: "glBindSampler" */
/* args: [{'type': 'GLuint', 'name': 'unit', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindSampler(void *context, GLuint unit, GLuint sampler)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = unit;
    ptr += 4;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindSampler;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameteri GLuint sampler, GLenum pname, GLint param" */
/* func name: "glSamplerParameteri" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glSamplerParameteri(void *context, GLuint sampler, GLenum pname, GLint param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLint *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glSamplerParameteri;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameterf GLuint sampler, GLenum pname, GLfloat param" */
/* func name: "glSamplerParameterf" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glSamplerParameterf(void *context, GLuint sampler, GLenum pname, GLfloat param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfloat *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glSamplerParameterf;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribDivisor_origin GLuint index, GLuint divisor" */
/* func name: "glVertexAttribDivisor_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'divisor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLuint *)ptr = divisor;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribDivisor_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glBindTransformFeedback GLenum target, GLuint id" */
/* func name: "glBindTransformFeedback" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glBindTransformFeedback(void *context, GLenum target, GLuint id)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = id;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glBindTransformFeedback;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glPauseTransformFeedback void" */
/* func name: "glPauseTransformFeedback" */
/* args: [] */
/* ret: "" */
/* type: "2" */

void d_glPauseTransformFeedback(void *context)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 0;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glPauseTransformFeedback;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glResumeTransformFeedback void" */
/* func name: "glResumeTransformFeedback" */
/* args: [] */
/* ret: "" */
/* type: "2" */

void d_glResumeTransformFeedback(void *context)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 0;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glResumeTransformFeedback;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramParameteri GLuint program, GLenum pname, GLint value" */
/* func name: "glProgramParameteri" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramParameteri(void *context, GLuint program, GLenum pname, GLint value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLint *)ptr = value;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramParameteri;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexStorage2D GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glTexStorage2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'levels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexStorage2D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLsizei *)ptr = levels;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexStorage2D;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexStorage3D GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth" */
/* func name: "glTexStorage3D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'levels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexStorage3D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLsizei *)ptr = levels;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexStorage3D;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glAlphaFuncxOES GLenum func, GLfixed ref" */
/* func name: "glAlphaFuncxOES" */
/* args: [{'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'ref', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glAlphaFuncxOES(void *context, GLenum func, GLfixed ref)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = func;
    ptr += 4;

    *(GLfixed *)ptr = ref;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glAlphaFuncxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearColorxOES GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha" */
/* func name: "glClearColorxOES" */
/* args: [{'type': 'GLfixed', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearColorxOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = red;
    ptr += 4;

    *(GLfixed *)ptr = green;
    ptr += 4;

    *(GLfixed *)ptr = blue;
    ptr += 4;

    *(GLfixed *)ptr = alpha;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearColorxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearDepthxOES GLfixed depth" */
/* func name: "glClearDepthxOES" */
/* args: [{'type': 'GLfixed', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearDepthxOES(void *context, GLfixed depth)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = depth;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearDepthxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glColor4xOES GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha" */
/* func name: "glColor4xOES" */
/* args: [{'type': 'GLfixed', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glColor4xOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = red;
    ptr += 4;

    *(GLfixed *)ptr = green;
    ptr += 4;

    *(GLfixed *)ptr = blue;
    ptr += 4;

    *(GLfixed *)ptr = alpha;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glColor4xOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDepthRangexOES GLfixed n, GLfixed f" */
/* func name: "glDepthRangexOES" */
/* args: [{'type': 'GLfixed', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDepthRangexOES(void *context, GLfixed n, GLfixed f)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = n;
    ptr += 4;

    *(GLfixed *)ptr = f;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDepthRangexOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFogxOES GLenum pname, GLfixed param" */
/* func name: "glFogxOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFogxOES(void *context, GLenum pname, GLfixed param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfixed *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFogxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFrustumxOES GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f" */
/* func name: "glFrustumxOES" */
/* args: [{'type': 'GLfixed', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFrustumxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = l;
    ptr += 4;

    *(GLfixed *)ptr = r;
    ptr += 4;

    *(GLfixed *)ptr = b;
    ptr += 4;

    *(GLfixed *)ptr = t;
    ptr += 4;

    *(GLfixed *)ptr = n;
    ptr += 4;

    *(GLfixed *)ptr = f;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFrustumxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLightModelxOES GLenum pname, GLfixed param" */
/* func name: "glLightModelxOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLightModelxOES(void *context, GLenum pname, GLfixed param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfixed *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLightModelxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLightxOES GLenum light, GLenum pname, GLfixed param" */
/* func name: "glLightxOES" */
/* args: [{'type': 'GLenum', 'name': 'light', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLightxOES(void *context, GLenum light, GLenum pname, GLfixed param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = light;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfixed *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLightxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLineWidthxOES GLfixed width" */
/* func name: "glLineWidthxOES" */
/* args: [{'type': 'GLfixed', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLineWidthxOES(void *context, GLfixed width)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = width;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLineWidthxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glMaterialxOES GLenum face, GLenum pname, GLfixed param" */
/* func name: "glMaterialxOES" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glMaterialxOES(void *context, GLenum face, GLenum pname, GLfixed param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = face;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfixed *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glMaterialxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glMultiTexCoord4xOES GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q" */
/* func name: "glMultiTexCoord4xOES" */
/* args: [{'type': 'GLenum', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 's', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'q', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glMultiTexCoord4xOES(void *context, GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = texture;
    ptr += 4;

    *(GLfixed *)ptr = s;
    ptr += 4;

    *(GLfixed *)ptr = t;
    ptr += 4;

    *(GLfixed *)ptr = r;
    ptr += 4;

    *(GLfixed *)ptr = q;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glMultiTexCoord4xOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glNormal3xOES GLfixed nx, GLfixed ny, GLfixed nz" */
/* func name: "glNormal3xOES" */
/* args: [{'type': 'GLfixed', 'name': 'nx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'ny', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'nz', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glNormal3xOES(void *context, GLfixed nx, GLfixed ny, GLfixed nz)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = nx;
    ptr += 4;

    *(GLfixed *)ptr = ny;
    ptr += 4;

    *(GLfixed *)ptr = nz;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glNormal3xOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glOrthoxOES GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f" */
/* func name: "glOrthoxOES" */
/* args: [{'type': 'GLfixed', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glOrthoxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = l;
    ptr += 4;

    *(GLfixed *)ptr = r;
    ptr += 4;

    *(GLfixed *)ptr = b;
    ptr += 4;

    *(GLfixed *)ptr = t;
    ptr += 4;

    *(GLfixed *)ptr = n;
    ptr += 4;

    *(GLfixed *)ptr = f;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glOrthoxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glPointSizexOES GLfixed size" */
/* func name: "glPointSizexOES" */
/* args: [{'type': 'GLfixed', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glPointSizexOES(void *context, GLfixed size)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = size;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glPointSizexOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glPolygonOffsetxOES GLfixed factor, GLfixed units" */
/* func name: "glPolygonOffsetxOES" */
/* args: [{'type': 'GLfixed', 'name': 'factor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'units', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glPolygonOffsetxOES(void *context, GLfixed factor, GLfixed units)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = factor;
    ptr += 4;

    *(GLfixed *)ptr = units;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glPolygonOffsetxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glRotatexOES GLfixed angle, GLfixed x, GLfixed y, GLfixed z" */
/* func name: "glRotatexOES" */
/* args: [{'type': 'GLfixed', 'name': 'angle', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glRotatexOES(void *context, GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = angle;
    ptr += 4;

    *(GLfixed *)ptr = x;
    ptr += 4;

    *(GLfixed *)ptr = y;
    ptr += 4;

    *(GLfixed *)ptr = z;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glRotatexOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glScalexOES GLfixed x, GLfixed y, GLfixed z" */
/* func name: "glScalexOES" */
/* args: [{'type': 'GLfixed', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glScalexOES(void *context, GLfixed x, GLfixed y, GLfixed z)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = x;
    ptr += 4;

    *(GLfixed *)ptr = y;
    ptr += 4;

    *(GLfixed *)ptr = z;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glScalexOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexEnvxOES GLenum target, GLenum pname, GLfixed param" */
/* func name: "glTexEnvxOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexEnvxOES(void *context, GLenum target, GLenum pname, GLfixed param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfixed *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexEnvxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTranslatexOES GLfixed x, GLfixed y, GLfixed z" */
/* func name: "glTranslatexOES" */
/* args: [{'type': 'GLfixed', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTranslatexOES(void *context, GLfixed x, GLfixed y, GLfixed z)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = x;
    ptr += 4;

    *(GLfixed *)ptr = y;
    ptr += 4;

    *(GLfixed *)ptr = z;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTranslatexOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glPointParameterxOES GLenum pname, GLfixed param" */
/* func name: "glPointParameterxOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glPointParameterxOES(void *context, GLenum pname, GLfixed param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfixed *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glPointParameterxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glSampleCoveragexOES GLclampx value, GLboolean invert" */
/* func name: "glSampleCoveragexOES" */
/* args: [{'type': 'GLclampx', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'invert', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glSampleCoveragexOES(void *context, GLclampx value, GLboolean invert)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLclampx *)ptr = value;
    ptr += 4;

    *(GLboolean *)ptr = invert;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glSampleCoveragexOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexGenxOES GLenum coord, GLenum pname, GLfixed param" */
/* func name: "glTexGenxOES" */
/* args: [{'type': 'GLenum', 'name': 'coord', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexGenxOES(void *context, GLenum coord, GLenum pname, GLfixed param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = coord;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLfixed *)ptr = param;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexGenxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearDepthfOES GLclampf depth" */
/* func name: "glClearDepthfOES" */
/* args: [{'type': 'GLclampf', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearDepthfOES(void *context, GLclampf depth)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLclampf *)ptr = depth;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearDepthfOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDepthRangefOES GLclampf n, GLclampf f" */
/* func name: "glDepthRangefOES" */
/* args: [{'type': 'GLclampf', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLclampf', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDepthRangefOES(void *context, GLclampf n, GLclampf f)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLclampf *)ptr = n;
    ptr += 4;

    *(GLclampf *)ptr = f;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDepthRangefOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFrustumfOES GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f" */
/* func name: "glFrustumfOES" */
/* args: [{'type': 'GLfloat', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFrustumfOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = l;
    ptr += 4;

    *(GLfloat *)ptr = r;
    ptr += 4;

    *(GLfloat *)ptr = b;
    ptr += 4;

    *(GLfloat *)ptr = t;
    ptr += 4;

    *(GLfloat *)ptr = n;
    ptr += 4;

    *(GLfloat *)ptr = f;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFrustumfOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glOrthofOES GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f" */
/* func name: "glOrthofOES" */
/* args: [{'type': 'GLfloat', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glOrthofOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = l;
    ptr += 4;

    *(GLfloat *)ptr = r;
    ptr += 4;

    *(GLfloat *)ptr = b;
    ptr += 4;

    *(GLfloat *)ptr = t;
    ptr += 4;

    *(GLfloat *)ptr = n;
    ptr += 4;

    *(GLfloat *)ptr = f;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glOrthofOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glRenderbufferStorageMultisampleEXT GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glRenderbufferStorageMultisampleEXT" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'samples', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glRenderbufferStorageMultisampleEXT(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLsizei *)ptr = samples;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glRenderbufferStorageMultisampleEXT;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUseProgramStages GLuint pipeline, GLbitfield stages, GLuint program" */
/* func name: "glUseProgramStages" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'stages', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUseProgramStages(void *context, GLuint pipeline, GLbitfield stages, GLuint program)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = pipeline;
    ptr += 4;

    *(GLbitfield *)ptr = stages;
    ptr += 4;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUseProgramStages;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glActiveShaderProgram GLuint pipeline, GLuint program" */
/* func name: "glActiveShaderProgram" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glActiveShaderProgram(void *context, GLuint pipeline, GLuint program)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = pipeline;
    ptr += 4;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glActiveShaderProgram;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1i GLuint program, GLint location, GLint v0" */
/* func name: "glProgramUniform1i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform1i(void *context, GLuint program, GLint location, GLint v0)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform1i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2i GLuint program, GLint location, GLint v0, GLint v1" */
/* func name: "glProgramUniform2i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform2i(void *context, GLuint program, GLint location, GLint v0, GLint v1)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    *(GLint *)ptr = v1;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform2i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3i GLuint program, GLint location, GLint v0, GLint v1, GLint v2" */
/* func name: "glProgramUniform3i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform3i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    *(GLint *)ptr = v1;
    ptr += 4;

    *(GLint *)ptr = v2;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform3i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4i GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3" */
/* func name: "glProgramUniform4i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform4i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLint *)ptr = v0;
    ptr += 4;

    *(GLint *)ptr = v1;
    ptr += 4;

    *(GLint *)ptr = v2;
    ptr += 4;

    *(GLint *)ptr = v3;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform4i;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1ui GLuint program, GLint location, GLuint v0" */
/* func name: "glProgramUniform1ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform1ui(void *context, GLuint program, GLint location, GLuint v0)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform1ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2ui GLuint program, GLint location, GLuint v0, GLuint v1" */
/* func name: "glProgramUniform2ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform2ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    *(GLuint *)ptr = v1;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform2ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3ui GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2" */
/* func name: "glProgramUniform3ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform3ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    *(GLuint *)ptr = v1;
    ptr += 4;

    *(GLuint *)ptr = v2;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform3ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4ui GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3" */
/* func name: "glProgramUniform4ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform4ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLuint *)ptr = v0;
    ptr += 4;

    *(GLuint *)ptr = v1;
    ptr += 4;

    *(GLuint *)ptr = v2;
    ptr += 4;

    *(GLuint *)ptr = v3;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform4ui;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1f GLuint program, GLint location, GLfloat v0" */
/* func name: "glProgramUniform1f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform1f(void *context, GLuint program, GLint location, GLfloat v0)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform1f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2f GLuint program, GLint location, GLfloat v0, GLfloat v1" */
/* func name: "glProgramUniform2f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform2f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    *(GLfloat *)ptr = v1;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform2f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3f GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2" */
/* func name: "glProgramUniform3f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform3f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    *(GLfloat *)ptr = v1;
    ptr += 4;

    *(GLfloat *)ptr = v2;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform3f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4f GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3" */
/* func name: "glProgramUniform4f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform4f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLfloat *)ptr = v0;
    ptr += 4;

    *(GLfloat *)ptr = v1;
    ptr += 4;

    *(GLfloat *)ptr = v2;
    ptr += 4;

    *(GLfloat *)ptr = v3;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform4f;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteBuffers_origin GLsizei n, const GLuint *buffers#n*sizeof(GLuint)" */
/* func name: "glDeleteBuffers_origin" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'buffers', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint *buffers)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t buffers_len = n * sizeof(GLuint);
    save_buf_len += buffers_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)buffers, buffers_len);
    ptr += buffers_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteBuffers_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteFramebuffers GLsizei n, const GLuint *framebuffers#n*sizeof(GLuint)" */
/* func name: "glDeleteFramebuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'framebuffers', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t framebuffers_len = n * sizeof(GLuint);
    save_buf_len += framebuffers_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)framebuffers, framebuffers_len);
    ptr += framebuffers_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteFramebuffers;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteRenderbuffers GLsizei n, const GLuint *renderbuffers#n*sizeof(GLuint)" */
/* func name: "glDeleteRenderbuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'renderbuffers', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t renderbuffers_len = n * sizeof(GLuint);
    save_buf_len += renderbuffers_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)renderbuffers, renderbuffers_len);
    ptr += renderbuffers_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteRenderbuffers;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteTextures GLsizei n, const GLuint *textures#n*sizeof(GLuint)" */
/* func name: "glDeleteTextures" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'textures', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteTextures(void *context, GLsizei n, const GLuint *textures)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t textures_len = n * sizeof(GLuint);
    save_buf_len += textures_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)textures, textures_len);
    ptr += textures_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteTextures;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDrawElements_with_bound GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices" */
/* func name: "glDrawElements_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'indices', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizeiptr *)ptr = indices;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDrawElements_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glShaderBinary GLsizei count, const GLuint *shaders#count*sizeof(GLuint), GLenum binaryFormat, const void *binary#length, GLsizei length" */
/* func name: "glShaderBinary" */
/* args: [{'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'shaders', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'binaryFormat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'binary', 'ptr': 'in', 'ptr_len': 'length', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glShaderBinary(void *context, GLsizei count, const GLuint *shaders, GLenum binaryFormat, const void *binary, GLsizei length)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t shaders_len = count * sizeof(GLuint);
    save_buf_len += shaders_len;

    size_t binary_len = length;
    save_buf_len += binary_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = binaryFormat;
    ptr += 4;

    *(GLsizei *)ptr = length;
    ptr += 4;

    memcpy(ptr, (unsigned char *)shaders, shaders_len);
    ptr += shaders_len;

    memcpy(ptr, (unsigned char *)binary, binary_len);
    ptr += binary_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glShaderBinary;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexSubImage2D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data" */
/* func name: "glCompressedTexSubImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 40;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    *(GLintptr *)ptr = data;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCompressedTexSubImage2D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexImage2D_with_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 40;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLintptr *)ptr = pixels;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexImage2D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameterfv GLenum target, GLenum pname, const GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glTexParameterfv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexParameterfv(void *context, GLenum target, GLenum pname, const GLfloat *params)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfloat);
    save_buf_len += params_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)params, params_len);
    ptr += params_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexParameterfv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameteriv GLenum target, GLenum pname, const GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glTexParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexParameteriv(void *context, GLenum target, GLenum pname, const GLint *params)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLint);
    save_buf_len += params_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)params, params_len);
    ptr += params_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexParameteriv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexSubImage2D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexSubImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 40;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLintptr *)ptr = pixels;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexSubImage2D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*1" */
/* func name: "glUniform1fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform1fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 1;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform1fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*1" */
/* func name: "glUniform1iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform1iv(void *context, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 1;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform1iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*2" */
/* func name: "glUniform2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*2', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform2fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 2;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*2" */
/* func name: "glUniform2iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*2', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform2iv(void *context, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform2iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*3" */
/* func name: "glUniform3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*3', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform3fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 3;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*3" */
/* func name: "glUniform3iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*3', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform3iv(void *context, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform3iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glUniform4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform4fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*4" */
/* func name: "glUniform4iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*4', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform4iv(void *context, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform4iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib1fv GLuint index, const GLfloat *v#sizeof(GLfloat)*1" */
/* func name: "glVertexAttrib1fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib1fv(void *context, GLuint index, const GLfloat *v)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 1;
    save_buf_len += v_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    memcpy(ptr, (unsigned char *)v, v_len);
    ptr += v_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib1fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib2fv GLuint index, const GLfloat *v#sizeof(GLfloat)*2" */
/* func name: "glVertexAttrib2fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*2', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib2fv(void *context, GLuint index, const GLfloat *v)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 2;
    save_buf_len += v_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    memcpy(ptr, (unsigned char *)v, v_len);
    ptr += v_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib3fv GLuint index, const GLfloat *v#sizeof(GLfloat)*3" */
/* func name: "glVertexAttrib3fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*3', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib3fv(void *context, GLuint index, const GLfloat *v)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 3;
    save_buf_len += v_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    memcpy(ptr, (unsigned char *)v, v_len);
    ptr += v_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib4fv GLuint index, const GLfloat *v#sizeof(GLfloat)*4" */
/* func name: "glVertexAttrib4fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttrib4fv(void *context, GLuint index, const GLfloat *v)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 4;
    save_buf_len += v_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    memcpy(ptr, (unsigned char *)v, v_len);
    ptr += v_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttrib4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribPointer_with_bound GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer" */
/* func name: "glVertexAttribPointer_with_bound" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'normalized', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pointer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 28;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLint *)ptr = size;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLboolean *)ptr = normalized;
    ptr += 4;

    *(GLsizei *)ptr = stride;
    ptr += 4;

    *(GLintptr *)ptr = pointer;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribPointer_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribPointer_offset GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer" */
/* func name: "glVertexAttribPointer_offset" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'normalized', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'min_index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'max_index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index_father', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'divisor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'enabled', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pointer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 48;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLuint *)ptr = size;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLboolean *)ptr = normalized;
    ptr += 4;

    *(GLsizei *)ptr = stride;
    ptr += 4;

    *(GLuint *)ptr = min_index;
    ptr += 4;

    *(GLuint *)ptr = max_index;
    ptr += 4;

    *(GLuint *)ptr = index_father;
    ptr += 4;

    *(GLuint *)ptr = divisor;
    ptr += 4;

    *(GLboolean *)ptr = enabled;
    ptr += 4;

    *(GLintptr *)ptr = pointer;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribPointer_offset;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDrawRangeElements_with_bound GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices" */
/* func name: "glDrawRangeElements_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'start', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'end', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'indices', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 28;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLuint *)ptr = start;
    ptr += 4;

    *(GLuint *)ptr = end;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizeiptr *)ptr = indices;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDrawRangeElements_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexImage3D_with_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 44;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLintptr *)ptr = pixels;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexImage3D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexSubImage3D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexSubImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 48;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLint *)ptr = zoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLintptr *)ptr = pixels;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexSubImage3D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexImage3D_with_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data" */
/* func name: "glCompressedTexImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 40;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    *(GLintptr *)ptr = data;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCompressedTexImage3D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexSubImage3D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data" */
/* func name: "glCompressedTexSubImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 48;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLint *)ptr = xoffset;
    ptr += 4;

    *(GLint *)ptr = yoffset;
    ptr += 4;

    *(GLint *)ptr = zoffset;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLsizei *)ptr = depth;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    *(GLintptr *)ptr = data;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCompressedTexSubImage3D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexImage2D_with_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data" */
/* func name: "glCompressedTexImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 36;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLint *)ptr = level;
    ptr += 4;

    *(GLenum *)ptr = internalformat;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLsizei *)ptr = imageSize;
    ptr += 4;

    *(GLintptr *)ptr = data;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glCompressedTexImage2D_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteQueries GLsizei n, const GLuint *ids#n*sizeof(GLuint)" */
/* func name: "glDeleteQueries" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'ids', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteQueries(void *context, GLsizei n, const GLuint *ids)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t ids_len = n * sizeof(GLuint);
    save_buf_len += ids_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)ids, ids_len);
    ptr += ids_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteQueries;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDrawBuffers GLsizei n, const GLenum *bufs#n*sizeof(GLenum)" */
/* func name: "glDrawBuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'bufs', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLenum)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDrawBuffers(void *context, GLsizei n, const GLenum *bufs)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t bufs_len = n * sizeof(GLenum);
    save_buf_len += bufs_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)bufs, bufs_len);
    ptr += bufs_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDrawBuffers;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix2fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glUniformMatrix2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix3fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*9" */
/* func name: "glUniformMatrix3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*9', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 9;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix4fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*16" */
/* func name: "glUniformMatrix4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*16', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 16;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix2x3fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glUniformMatrix2x3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix2x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix2x3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix3x2fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glUniformMatrix3x2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix3x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix3x2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix2x4fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glUniformMatrix2x4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix2x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix2x4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix4x2fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glUniformMatrix4x2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix4x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix4x2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix3x4fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glUniformMatrix3x4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix3x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix3x4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix4x3fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glUniformMatrix4x3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniformMatrix4x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniformMatrix4x3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteVertexArrays_origin GLsizei n, const GLuint *arrays#n*sizeof(GLuint)" */
/* func name: "glDeleteVertexArrays_origin" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'arrays', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint *arrays)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t arrays_len = n * sizeof(GLuint);
    save_buf_len += arrays_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)arrays, arrays_len);
    ptr += arrays_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteVertexArrays_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribIPointer_with_bound GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer" */
/* func name: "glVertexAttribIPointer_with_bound" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pointer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLint *)ptr = size;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizei *)ptr = stride;
    ptr += 4;

    *(GLintptr *)ptr = pointer;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribIPointer_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribIPointer_offset GLuint index, GLint size, GLenum type, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer" */
/* func name: "glVertexAttribIPointer_offset" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'min_index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'max_index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index_father', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'divisor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'enabled', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pointer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 44;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLint *)ptr = size;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizei *)ptr = stride;
    ptr += 4;

    *(GLuint *)ptr = min_index;
    ptr += 4;

    *(GLuint *)ptr = max_index;
    ptr += 4;

    *(GLuint *)ptr = index_father;
    ptr += 4;

    *(GLuint *)ptr = divisor;
    ptr += 4;

    *(GLboolean *)ptr = enabled;
    ptr += 4;

    *(GLintptr *)ptr = pointer;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribIPointer_offset;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4iv GLuint index, const GLint *v#sizeof(GLint)*4" */
/* func name: "glVertexAttribI4iv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLint)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribI4iv(void *context, GLuint index, const GLint *v)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLint) * 4;
    save_buf_len += v_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    memcpy(ptr, (unsigned char *)v, v_len);
    ptr += v_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribI4iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4uiv GLuint index, const GLuint *v#sizeof(GLuint)*4" */
/* func name: "glVertexAttribI4uiv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLuint)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glVertexAttribI4uiv(void *context, GLuint index, const GLuint *v)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLuint) * 4;
    save_buf_len += v_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    memcpy(ptr, (unsigned char *)v, v_len);
    ptr += v_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribI4uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*1" */
/* func name: "glUniform1uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform1uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 1;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform1uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*2" */
/* func name: "glUniform2uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*2', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform2uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform2uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*3" */
/* func name: "glUniform3uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*3', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform3uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform3uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*4" */
/* func name: "glUniform4uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*4', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glUniform4uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glUniform4uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferiv GLenum buffer, GLint drawbuffer, const GLint *value#(buffer==GL_COLOR?4*sizeof(GLint):1*sizeof(GLint))" */
/* func name: "glClearBufferiv" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': '(buffer==GL_COLOR?4*sizeof(GLint):1*sizeof(GLint))', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearBufferiv(void *context, GLenum buffer, GLint drawbuffer, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = (buffer == GL_COLOR ? 4 * sizeof(GLint) : 1 * sizeof(GLint));
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = buffer;
    ptr += 4;

    *(GLint *)ptr = drawbuffer;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearBufferiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferuiv GLenum buffer, GLint drawbuffer, const GLuint *value#(buffer==GL_COLOR?4*sizeof(GLuint):1*sizeof(GLuint))" */
/* func name: "glClearBufferuiv" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': '(buffer==GL_COLOR?4*sizeof(GLuint):1*sizeof(GLuint))', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearBufferuiv(void *context, GLenum buffer, GLint drawbuffer, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = (buffer == GL_COLOR ? 4 * sizeof(GLuint) : 1 * sizeof(GLuint));
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = buffer;
    ptr += 4;

    *(GLint *)ptr = drawbuffer;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearBufferuiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferfv GLenum buffer, GLint drawbuffer, const GLfloat *value#(buffer==GL_COLOR?4*sizeof(GLfloat):1*sizeof(GLfloat))" */
/* func name: "glClearBufferfv" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': '(buffer==GL_COLOR?4*sizeof(GLfloat):1*sizeof(GLfloat))', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClearBufferfv(void *context, GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = (buffer == GL_COLOR ? 4 * sizeof(GLfloat) : 1 * sizeof(GLfloat));
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = buffer;
    ptr += 4;

    *(GLint *)ptr = drawbuffer;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClearBufferfv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDrawElementsInstanced_with_bound GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount" */
/* func name: "glDrawElementsInstanced_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'indices', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'instancecount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizeiptr *)ptr = indices;
    ptr += 8;

    *(GLsizei *)ptr = instancecount;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDrawElementsInstanced_with_bound;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteSamplers GLsizei count, const GLuint *samplers#count*sizeof(GLuint)" */
/* func name: "glDeleteSamplers" */
/* args: [{'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'samplers', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t samplers_len = count * sizeof(GLuint);
    save_buf_len += samplers_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)samplers, samplers_len);
    ptr += samplers_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteSamplers;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameteriv GLuint sampler, GLenum pname, const GLint *param#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glSamplerParameteriv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint *param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLint);
    save_buf_len += param_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)param, param_len);
    ptr += param_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glSamplerParameteriv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameterfv GLuint sampler, GLenum pname, const GLfloat *param#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glSamplerParameterfv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat *param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfloat);
    save_buf_len += param_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)param, param_len);
    ptr += param_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glSamplerParameterfv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteTransformFeedbacks GLsizei n, const GLuint *ids#n*sizeof(GLuint)" */
/* func name: "glDeleteTransformFeedbacks" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'ids', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t ids_len = n * sizeof(GLuint);
    save_buf_len += ids_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)ids, ids_len);
    ptr += ids_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteTransformFeedbacks;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramBinary GLuint program, GLenum binaryFormat, const void *binary#length, GLsizei length" */
/* func name: "glProgramBinary" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'binaryFormat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'binary', 'ptr': 'in', 'ptr_len': 'length', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramBinary(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t binary_len = length;
    save_buf_len += binary_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = binaryFormat;
    ptr += 4;

    *(GLsizei *)ptr = length;
    ptr += 4;

    memcpy(ptr, (unsigned char *)binary, binary_len);
    ptr += binary_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramBinary;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glInvalidateFramebuffer GLenum target, GLsizei numAttachments, const GLenum *attachments#numAttachments*sizeof(GLenum)" */
/* func name: "glInvalidateFramebuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'numAttachments', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'attachments', 'ptr': 'in', 'ptr_len': 'numAttachments*sizeof(GLenum)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glInvalidateFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t attachments_len = numAttachments * sizeof(GLenum);
    save_buf_len += attachments_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLsizei *)ptr = numAttachments;
    ptr += 4;

    memcpy(ptr, (unsigned char *)attachments, attachments_len);
    ptr += attachments_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glInvalidateFramebuffer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glInvalidateSubFramebuffer GLenum target, GLsizei numAttachments, const GLenum *attachments#numAttachments*sizeof(GLenum), GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glInvalidateSubFramebuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'numAttachments', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'attachments', 'ptr': 'in', 'ptr_len': 'numAttachments*sizeof(GLenum)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glInvalidateSubFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 24;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t attachments_len = numAttachments * sizeof(GLenum);
    save_buf_len += attachments_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLsizei *)ptr = numAttachments;
    ptr += 4;

    *(GLint *)ptr = x;
    ptr += 4;

    *(GLint *)ptr = y;
    ptr += 4;

    *(GLsizei *)ptr = width;
    ptr += 4;

    *(GLsizei *)ptr = height;
    ptr += 4;

    memcpy(ptr, (unsigned char *)attachments, attachments_len);
    ptr += attachments_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glInvalidateSubFramebuffer;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClipPlanexOES GLenum plane, const GLfixed *equation#sizeof(GLfixed)*4" */
/* func name: "glClipPlanexOES" */
/* args: [{'type': 'GLenum', 'name': 'plane', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'equation', 'ptr': 'in', 'ptr_len': 'sizeof(GLfixed)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClipPlanexOES(void *context, GLenum plane, const GLfixed *equation)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t equation_len = sizeof(GLfixed) * 4;
    save_buf_len += equation_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = plane;
    ptr += 4;

    memcpy(ptr, (unsigned char *)equation, equation_len);
    ptr += equation_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClipPlanexOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFogxvOES GLenum pname, const GLfixed *param#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glFogxvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFogxvOES(void *context, GLenum pname, const GLfixed *param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += param_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)param, param_len);
    ptr += param_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFogxvOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLightModelxvOES GLenum pname, const GLfixed *param#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glLightModelxvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLightModelxvOES(void *context, GLenum pname, const GLfixed *param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += param_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)param, param_len);
    ptr += param_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLightModelxvOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLightxvOES GLenum light, GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glLightxvOES" */
/* args: [{'type': 'GLenum', 'name': 'light', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLightxvOES(void *context, GLenum light, GLenum pname, const GLfixed *params)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = light;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)params, params_len);
    ptr += params_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLightxvOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glLoadMatrixxOES const GLfixed *m#16*sizeof(GLfixed)" */
/* func name: "glLoadMatrixxOES" */
/* args: [{'type': 'const GLfixed*', 'name': 'm', 'ptr': 'in', 'ptr_len': '16*sizeof(GLfixed)', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glLoadMatrixxOES(void *context, const GLfixed *m)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 0;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t m_len = 16 * sizeof(GLfixed);
    save_buf_len += m_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    memcpy(ptr, (unsigned char *)m, m_len);
    ptr += m_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glLoadMatrixxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glMaterialxvOES GLenum face, GLenum pname, const GLfixed *param#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glMaterialxvOES" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glMaterialxvOES(void *context, GLenum face, GLenum pname, const GLfixed *param)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += param_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = face;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)param, param_len);
    ptr += param_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glMaterialxvOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glMultMatrixxOES const GLfixed *m#16*sizeof(GLfixed)" */
/* func name: "glMultMatrixxOES" */
/* args: [{'type': 'const GLfixed*', 'name': 'm', 'ptr': 'in', 'ptr_len': '16*sizeof(GLfixed)', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glMultMatrixxOES(void *context, const GLfixed *m)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 0;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t m_len = 16 * sizeof(GLfixed);
    save_buf_len += m_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    memcpy(ptr, (unsigned char *)m, m_len);
    ptr += m_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glMultMatrixxOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glPointParameterxvOES GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glPointParameterxvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glPointParameterxvOES(void *context, GLenum pname, const GLfixed *params)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)params, params_len);
    ptr += params_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glPointParameterxvOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexEnvxvOES GLenum target, GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glTexEnvxvOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexEnvxvOES(void *context, GLenum target, GLenum pname, const GLfixed *params)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)params, params_len);
    ptr += params_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexEnvxvOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glClipPlanefOES GLenum plane, const GLfloat *equation#sizeof(GLfloat)*4" */
/* func name: "glClipPlanefOES" */
/* args: [{'type': 'GLenum', 'name': 'plane', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'equation', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glClipPlanefOES(void *context, GLenum plane, const GLfloat *equation)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t equation_len = sizeof(GLfloat) * 4;
    save_buf_len += equation_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = plane;
    ptr += 4;

    memcpy(ptr, (unsigned char *)equation, equation_len);
    ptr += equation_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glClipPlanefOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glTexGenxvOES GLenum coord, GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glTexGenxvOES" */
/* args: [{'type': 'GLenum', 'name': 'coord', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glTexGenxvOES(void *context, GLenum coord, GLenum pname, const GLfixed *params)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = coord;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    memcpy(ptr, (unsigned char *)params, params_len);
    ptr += params_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glTexGenxvOES;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteProgramPipelines GLsizei n, const GLuint *pipelines#n*sizeof(GLuint)" */
/* func name: "glDeleteProgramPipelines" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'pipelines', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 4;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t pipelines_len = n * sizeof(GLuint);
    save_buf_len += pipelines_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    memcpy(ptr, (unsigned char *)pipelines, pipelines_len);
    ptr += pipelines_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteProgramPipelines;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)" */
/* func name: "glProgramUniform1iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform1iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint);
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform1iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*2" */
/* func name: "glProgramUniform2iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*2', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform2iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform2iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*3" */
/* func name: "glProgramUniform3iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*3', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform3iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform3iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*4" */
/* func name: "glProgramUniform4iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform4iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform4iv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)" */
/* func name: "glProgramUniform1uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform1uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint);
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform1uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*2" */
/* func name: "glProgramUniform2uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*2', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform2uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform2uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*3" */
/* func name: "glProgramUniform3uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*3', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform3uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform3uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*4" */
/* func name: "glProgramUniform4uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform4uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform4uiv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)" */
/* func name: "glProgramUniform1fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform1fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat);
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform1fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*2" */
/* func name: "glProgramUniform2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*2', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform2fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 2;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*3" */
/* func name: "glProgramUniform3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*3', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform3fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 3;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glProgramUniform4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniform4fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 12;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniform4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix2fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glProgramUniformMatrix2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix3fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*9" */
/* func name: "glProgramUniformMatrix3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*9', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 9;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix4fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*16" */
/* func name: "glProgramUniformMatrix4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*16', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 16;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix2x3fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glProgramUniformMatrix2x3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix2x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix2x3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix3x2fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glProgramUniformMatrix3x2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix3x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix3x2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix2x4fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glProgramUniformMatrix2x4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix2x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix2x4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix4x2fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glProgramUniformMatrix4x2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix4x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix4x2fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix3x4fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glProgramUniformMatrix3x4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix3x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix3x4fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix4x3fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glProgramUniformMatrix4x3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glProgramUniformMatrix4x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 16;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLboolean *)ptr = transpose;
    ptr += 4;

    memcpy(ptr, (unsigned char *)value, value_len);
    ptr += value_len;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glProgramUniformMatrix4x3fv;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glFlushMappedBufferRange_origin GLenum target, GLintptr offset, GLsizeiptr length" */
/* func name: "glFlushMappedBufferRange_origin" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glFlushMappedBufferRange_origin(void *context, GLenum target, GLintptr offset, GLsizeiptr length)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    *(GLsizeiptr *)ptr = length;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glFlushMappedBufferRange_origin;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteSync GLsync sync" */
/* func name: "glDeleteSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glDeleteSync(void *context, GLsync sync)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 8;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glDeleteSync;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/* readline: "glWaitSync GLsync sync, GLbitfield flags, GLuint64 timeout" */
/* func name: "glWaitSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint64', 'name': 'timeout', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    unsigned char send_buf[32];
    size_t send_buf_len = 32;
    unsigned long save_buf_len = 20;
    unsigned char local_save_buf[1024];
    unsigned char *save_buf;

    if (save_buf_len > 1000)
    {
        save_buf = malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    *(GLbitfield *)ptr = flags;
    ptr += 4;

    *(GLuint64 *)ptr = timeout;
    ptr += 8;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glWaitSync;
    ptr += sizeof(unsigned long);
    if (save_buf_len == 0)
    {
        *(unsigned long *)ptr = 0;
        ptr += sizeof(unsigned long);
    }
    else
    {
        *(unsigned long *)ptr = 1;
        ptr += sizeof(unsigned long);

        *(unsigned long *)ptr = (unsigned long)save_buf_len;
        ptr += sizeof(unsigned long);
        *(unsigned long *)ptr = (unsigned long)save_buf;
        ptr += sizeof(unsigned long);
    }

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > 1000)
    {
        free(save_buf);
    }
}

/******* end of file '2-1', 240/353 functions *******/

/******* file '2-2' *******/

/* readline: "glVertexAttribPointer_data GLuint index, GLuint length, const void *pointer#length" */
/* func name: "glVertexAttribPointer_data" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pointer', 'ptr': 'in', 'ptr_len': 'length', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribPointer_data(void *context, GLuint index, GLuint length, const void *pointer)
{
    unsigned char send_buf[32 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLuint *)ptr = length;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribPointer_data;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)(1 + 1);
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pointer;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glVertexAttribIPointer_data GLuint index, GLuint size, const void *pointer#size" */
/* func name: "glVertexAttribIPointer_data" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pointer', 'ptr': 'in', 'ptr_len': 'size', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribIPointer_data(void *context, GLuint index, GLuint size, const void *pointer)
{
    unsigned char send_buf[32 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLuint *)ptr = size;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glVertexAttribIPointer_data;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)(1 + 1);
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)size;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)pointer;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glMapBufferRange_write GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, const void *mem_buf#length" */
/* func name: "glMapBufferRange_write" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'access', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'mem_buf', 'ptr': 'in', 'ptr_len': 'length', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, const void *mem_buf)
{
    unsigned char send_buf[32 + 1 * 16];
    size_t send_buf_len = 32 + 1 * 16;
    unsigned char *ptr = NULL;

    unsigned long save_buf_len = 24;
    unsigned char *save_buf;

    unsigned char local_save_buf[24];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    *(GLsizeiptr *)ptr = length;
    ptr += 8;

    *(GLbitfield *)ptr = access;
    ptr += 4;

    ptr = send_buf;

    *(unsigned long *)ptr = FUNID_glMapBufferRange_write;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)(1 + 1);
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)save_buf_len;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)save_buf;
    ptr += sizeof(unsigned long);

    *(unsigned long *)ptr = (unsigned long)length;
    ptr += sizeof(unsigned long);
    *(unsigned long *)ptr = (unsigned long)mem_buf;
    ptr += sizeof(unsigned long);

    send_to_host(context, send_buf, send_buf_len);
}

/******* end of file '2-2', 3/356 functions *******/
