
#include "define_gl.h"

#include "glv3_mem.h"
#include "glv3_status.h"
#include "glv3_texture.h"
#include "glv3_utils.h"
#include "glv3_vertex.h"

#include "../context_common.h"

/******* file '1-1-1' *******/

/* readline: "GLenum glCheckFramebufferStatus GLenum target @{if(target!=GL_DRAW_FRAMEBUFFER&&target!= GL_READ_FRAMEBUFFER&&target!=GL_FRAMEBUFFER){set_gl_error(context,GL_INVALID_ENUM);return 0;}}" */
/* func name: "glCheckFramebufferStatus" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLenum" */
/* type: "0" */

GLenum d_glCheckFramebufferStatus(void *context, GLenum target)
{
    {
        if (target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER && target != GL_FRAMEBUFFER)
        {
            set_gl_error(context, GL_INVALID_ENUM);
            return 0;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glCheckFramebufferStatus;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLenum);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLenum ret = *(GLenum *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLuint glCreateShaderProgramv GLenum type, GLsizei count, const GLchar *const*strings#count|strlen(strings[i])+1 @{if(type!=GL_COMPUTE_SHADER&&type!=GL_VERTEX_SHADER&&type!=GL_FRAGMENT_SHADER){set_gl_error(context,GL_INVALID_ENUM);return 0;}if(count<0){set_gl_error(context,GL_INVALID_VALUE);return 0;}}" */
/* func name: "glCreateShaderProgramv" */
/* args: [{'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'strings', 'ptr': 'in', 'ptr_len': 'count|strlen(strings[i])+1', 'loc': 2, 'ptr_ptr': True}] */
/* ret: "GLuint" */
/* type: "0" */

GLuint d_glCreateShaderProgramv(void *context, GLenum type, GLsizei count, const GLchar *const *strings)
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

    unsigned char *send_buf = (unsigned char *)malloc(16 + 16 * (2 + count));
    size_t send_buf_len = 16 + 16 * (2 + count);
    size_t para_num = 2 + count;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glCreateShaderProgramv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    for (int i = 0; i < count; i++)
    {
        *(uint64_t *)ptr = (uint64_t)strlen(strings[i]) + 1;
        ptr += sizeof(uint64_t);
        *(uint64_t *)ptr = (uint64_t)strings[i];
        ptr += sizeof(uint64_t);
    }

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLuint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    free(send_buf);
    out_buf_ptr = out_buf;
    GLuint ret = *(GLuint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLbitfield glQueryMatrixxOES GLfixed *mantissa#sizeof(GLint)*16, GLint *exponent#sizeof(GLint)*16" */
/* func name: "glQueryMatrixxOES" */
/* args: [{'type': 'GLfixed*', 'name': 'mantissa', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)*16', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'exponent', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)*16', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLbitfield" */
/* type: "0" */

GLbitfield d_glQueryMatrixxOES(void *context, GLfixed *mantissa, GLint *exponent)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glQueryMatrixxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t mantissa_len = (uint64_t)sizeof(GLint) * 16;
    out_buf_len += mantissa_len;

    uint64_t exponent_len = (uint64_t)sizeof(GLint) * 16;
    out_buf_len += exponent_len;

    out_buf_len += sizeof(GLbitfield);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (mantissa != NULL)
    {

        memcpy(mantissa, (GLfixed *)out_buf_ptr, mantissa_len);
    }
    out_buf_ptr += mantissa_len;

    if (exponent != NULL)
    {

        memcpy(exponent, (GLint *)out_buf_ptr, exponent_len);
    }
    out_buf_ptr += exponent_len;

    GLbitfield ret = *(GLbitfield *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "glGetFramebufferAttachmentParameteriv GLenum target, GLenum attachment, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetFramebufferAttachmentParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetFramebufferAttachmentParameteriv(void *context, GLenum target, GLenum attachment, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetFramebufferAttachmentParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramInfoLog GLuint program, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *infoLog#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetProgramInfoLog" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'infoLog', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramInfoLog(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetProgramInfoLog;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t infoLog_len = (uint64_t)bufSize;
    out_buf_len += infoLog_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (infoLog != NULL)
    {

        memcpy(infoLog, (GLchar *)out_buf_ptr, infoLog_len);
    }
    out_buf_ptr += infoLog_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetRenderbufferParameteriv GLenum target, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetRenderbufferParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetRenderbufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetRenderbufferParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetShaderInfoLog GLuint shader, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *infoLog#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetShaderInfoLog" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'infoLog', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetShaderInfoLog(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetShaderInfoLog;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t infoLog_len = (uint64_t)bufSize;
    out_buf_len += infoLog_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (infoLog != NULL)
    {

        memcpy(infoLog, (GLchar *)out_buf_ptr, infoLog_len);
    }
    out_buf_ptr += infoLog_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetShaderPrecisionFormat GLenum shadertype, GLenum precisiontype, GLint *range#2*sizeof(GLint), GLint *precision#sizeof(GLint)" */
/* func name: "glGetShaderPrecisionFormat" */
/* args: [{'type': 'GLenum', 'name': 'shadertype', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'precisiontype', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'range', 'ptr': 'out', 'ptr_len': '2*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'precision', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetShaderPrecisionFormat(void *context, GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = shadertype;
    ptr += 4;

    *(GLenum *)ptr = precisiontype;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetShaderPrecisionFormat;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t range_len = (uint64_t)2 * sizeof(GLint);
    out_buf_len += range_len;

    uint64_t precision_len = (uint64_t)sizeof(GLint);
    out_buf_len += precision_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (range != NULL)
    {

        memcpy(range, (GLint *)out_buf_ptr, range_len);
    }
    out_buf_ptr += range_len;

    if (precision != NULL)
    {

        memcpy(precision, (GLint *)out_buf_ptr, precision_len);
    }
    out_buf_ptr += precision_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetShaderSource GLuint shader, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *source#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetShaderSource" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'source', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetShaderSource(void *context, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetShaderSource;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t source_len = (uint64_t)bufSize;
    out_buf_len += source_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (source != NULL)
    {

        memcpy(source, (GLchar *)out_buf_ptr, source_len);
    }
    out_buf_ptr += source_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTexParameterfv GLenum target, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetTexParameterfv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexParameterfv(void *context, GLenum target, GLenum pname, GLfloat *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetTexParameterfv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfloat);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfloat *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTexParameteriv GLenum target, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetTexParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetTexParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetQueryiv GLenum target, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetQueryiv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetQueryiv(void *context, GLenum target, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetQueryiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetQueryObjectuiv GLuint id, GLenum pname, GLuint *params#gl_pname_size(pname)*sizeof(GLuint)" */
/* func name: "glGetQueryObjectuiv" */
/* args: [{'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLuint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetQueryObjectuiv(void *context, GLuint id, GLenum pname, GLuint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = id;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetQueryObjectuiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLuint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLuint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTransformFeedbackVarying GLuint program, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLsizei *size#sizeof(GLsizei), GLenum *type#sizeof(GLenum), GLchar *name#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetTransformFeedbackVarying" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'size', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'type', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTransformFeedbackVarying(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetTransformFeedbackVarying;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t size_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += size_len;

    uint64_t type_len = (uint64_t)sizeof(GLenum);
    out_buf_len += type_len;

    uint64_t name_len = (uint64_t)bufSize;
    out_buf_len += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (size != NULL)
    {

        memcpy(size, (GLsizei *)out_buf_ptr, size_len);
    }
    out_buf_ptr += size_len;

    if (type != NULL)
    {

        memcpy(type, (GLenum *)out_buf_ptr, type_len);
    }
    out_buf_ptr += type_len;

    if (name != NULL)
    {

        memcpy(name, (GLchar *)out_buf_ptr, name_len);
    }
    out_buf_ptr += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetActiveUniformsiv GLuint program, GLsizei uniformCount, const GLuint *uniformIndices#uniformCount*sizeof(GLuint), GLenum pname, GLint *params#uniformCount*sizeof(GLint)" */
/* func name: "glGetActiveUniformsiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'uniformCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'uniformIndices', 'ptr': 'in', 'ptr_len': 'uniformCount*sizeof(GLuint)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'uniformCount*sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveUniformsiv(void *context, GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetActiveUniformsiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = uniformCount * sizeof(GLuint);
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)uniformIndices;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)uniformCount * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetActiveUniformBlockiv GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params#gl_get_uniform_block_para_size(context,program,uniformBlockIndex,pname)*sizeof(GLint)" */
/* func name: "glGetActiveUniformBlockiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockIndex', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_uniform_block_para_size(context,program,uniformBlockIndex,pname)*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveUniformBlockiv(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetActiveUniformBlockiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_get_uniform_block_para_size(context, program, uniformBlockIndex, pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetActiveUniformBlockName GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *uniformBlockName#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetActiveUniformBlockName" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockIndex', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'uniformBlockName', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetActiveUniformBlockName(void *context, GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetActiveUniformBlockName;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t uniformBlockName_len = (uint64_t)bufSize;
    out_buf_len += uniformBlockName_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (uniformBlockName != NULL)
    {

        memcpy(uniformBlockName, (GLchar *)out_buf_ptr, uniformBlockName_len);
    }
    out_buf_ptr += uniformBlockName_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetSamplerParameteriv GLuint sampler, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetSamplerParameteriv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetSamplerParameteriv(void *context, GLuint sampler, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetSamplerParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetSamplerParameterfv GLuint sampler, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetSamplerParameterfv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetSamplerParameterfv(void *context, GLuint sampler, GLenum pname, GLfloat *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetSamplerParameterfv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfloat);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfloat *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramBinary GLuint program, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLenum *binaryFormat#sizeof(GLenum), void *binary#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetProgramBinary" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'binaryFormat', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 3, 'ptr_ptr': False}, {'type': 'void*', 'name': 'binary', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramBinary(void *context, GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetProgramBinary;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t binaryFormat_len = (uint64_t)sizeof(GLenum);
    out_buf_len += binaryFormat_len;

    uint64_t binary_len = (uint64_t)bufSize;
    out_buf_len += binary_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (binaryFormat != NULL)
    {

        memcpy(binaryFormat, (GLenum *)out_buf_ptr, binaryFormat_len);
    }
    out_buf_ptr += binaryFormat_len;

    if (binary != NULL)
    {

        memcpy(binary, (void *)out_buf_ptr, binary_len);
    }
    out_buf_ptr += binary_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetInternalformativ GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params#count*sizeof(GLint)" */
/* func name: "glGetInternalformativ" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'count*sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetInternalformativ(void *context, GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
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
    *(uint64_t *)ptr = FUNID_glGetInternalformativ;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)count * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetClipPlanexOES GLenum plane, GLfixed *equation#4*sizeof(GLfixed)" */
/* func name: "glGetClipPlanexOES" */
/* args: [{'type': 'GLenum', 'name': 'plane', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'equation', 'ptr': 'out', 'ptr_len': '4*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetClipPlanexOES(void *context, GLenum plane, GLfixed *equation)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = plane;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetClipPlanexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t equation_len = (uint64_t)4 * sizeof(GLfixed);
    out_buf_len += equation_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (equation != NULL)
    {

        memcpy(equation, (GLfixed *)out_buf_ptr, equation_len);
    }
    out_buf_ptr += equation_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetFixedvOES GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetFixedvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetFixedvOES(void *context, GLenum pname, GLfixed *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetFixedvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfixed);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfixed *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTexEnvxvOES GLenum target, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetTexEnvxvOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexEnvxvOES(void *context, GLenum target, GLenum pname, GLfixed *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetTexEnvxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfixed);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfixed *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTexParameterxvOES GLenum target, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetTexParameterxvOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexParameterxvOES(void *context, GLenum target, GLenum pname, GLfixed *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetTexParameterxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfixed);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfixed *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetLightxvOES GLenum light, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetLightxvOES" */
/* args: [{'type': 'GLenum', 'name': 'light', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetLightxvOES(void *context, GLenum light, GLenum pname, GLfixed *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = light;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetLightxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfixed);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfixed *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetMaterialxvOES GLenum face, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetMaterialxvOES" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetMaterialxvOES(void *context, GLenum face, GLenum pname, GLfixed *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = face;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetMaterialxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfixed);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfixed *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTexGenxvOES GLenum coord, GLenum pname, GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glGetTexGenxvOES" */
/* args: [{'type': 'GLenum', 'name': 'coord', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexGenxvOES(void *context, GLenum coord, GLenum pname, GLfixed *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = coord;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetTexGenxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfixed);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfixed *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glReadPixels_with_bound GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glReadPixels_with_bound" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 32;
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
    *(uint64_t *)ptr = FUNID_glReadPixels_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glGetFramebufferParameteriv GLenum target, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetFramebufferParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetFramebufferParameteriv(void *context, GLenum target, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetFramebufferParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramInterfaceiv GLuint program, GLenum programInterface, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetProgramInterfaceiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramInterfaceiv(void *context, GLuint program, GLenum programInterface, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetProgramInterfaceiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramResourceName GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *name#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetProgramResourceName" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramResourceName(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
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
    *(uint64_t *)ptr = FUNID_glGetProgramResourceName;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t name_len = (uint64_t)bufSize;
    out_buf_len += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (name != NULL)
    {

        memcpy(name, (GLchar *)out_buf_ptr, name_len);
    }
    out_buf_ptr += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramResourceiv GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props#propCount*sizeof(GLenum), GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLint *params#bufSize*sizeof(GLint) @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetProgramResourceiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'propCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'props', 'ptr': 'in', 'ptr_len': 'propCount*sizeof(GLenum)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'bufSize*sizeof(GLint)', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramResourceiv(void *context, GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 20;
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
    *(uint64_t *)ptr = FUNID_glGetProgramResourceiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = propCount * sizeof(GLenum);
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)props;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t params_len = (uint64_t)bufSize * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramPipelineiv GLuint pipeline, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetProgramPipelineiv" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramPipelineiv(void *context, GLuint pipeline, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = pipeline;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetProgramPipelineiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramPipelineInfoLog GLuint pipeline, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLchar *infoLog#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetProgramPipelineInfoLog" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'infoLog', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetProgramPipelineInfoLog(void *context, GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = pipeline;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetProgramPipelineInfoLog;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t infoLog_len = (uint64_t)bufSize;
    out_buf_len += infoLog_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (infoLog != NULL)
    {

        memcpy(infoLog, (GLchar *)out_buf_ptr, infoLog_len);
    }
    out_buf_ptr += infoLog_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetMultisamplefv GLenum pname, GLuint index, GLfloat *val#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetMultisamplefv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'val', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetMultisamplefv(void *context, GLenum pname, GLuint index, GLfloat *val)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetMultisamplefv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t val_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfloat);
    out_buf_len += val_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (val != NULL)
    {

        memcpy(val, (GLfloat *)out_buf_ptr, val_len);
    }
    out_buf_ptr += val_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTexLevelParameteriv GLenum target, GLint level, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetTexLevelParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexLevelParameteriv(void *context, GLenum target, GLint level, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetTexLevelParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetTexLevelParameterfv GLenum target, GLint level, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetTexLevelParameterfv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetTexLevelParameterfv(void *context, GLenum target, GLint level, GLenum pname, GLfloat *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetTexLevelParameterfv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfloat);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfloat *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "GLsync glFenceSync GLenum condition, GLbitfield flags" */
/* func name: "glFenceSync" */
/* args: [{'type': 'GLenum', 'name': 'condition', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLsync" */
/* type: "0" */

GLsync d_glFenceSync(void *context, GLenum condition, GLbitfield flags)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = condition;
    ptr += 4;

    *(GLbitfield *)ptr = flags;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glFenceSync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLsync);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLsync ret = *(GLsync *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsSync GLsync sync" */
/* func name: "glIsSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "0" */

GLboolean d_glIsSync(void *context, GLsync sync)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsSync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "glGetSynciv GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLint *values#bufSize*sizeof(GLint) @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetSynciv" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'values', 'ptr': 'out', 'ptr_len': 'bufSize*sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glGetSynciv(void *context, GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    *(GLenum *)ptr = pname;
    ptr += 4;

    *(GLsizei *)ptr = bufSize;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetSynciv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t values_len = (uint64_t)bufSize * sizeof(GLint);
    out_buf_len += values_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (values != NULL)
    {

        memcpy(values, (GLint *)out_buf_ptr, values_len);
    }
    out_buf_ptr += values_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "GLenum glClientWaitSync GLsync sync, GLbitfield flags, GLuint64 timeout" */
/* func name: "glClientWaitSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint64', 'name': 'timeout', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLenum" */
/* type: "0" */

GLenum d_glClientWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 20;
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
    *(uint64_t *)ptr = FUNID_glClientWaitSync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLenum);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLenum ret = *(GLenum *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "glFinish void" */
/* func name: "glFinish" */
/* args: [] */
/* ret: "" */
/* type: "0" */

void d_glFinish(void *context)
{

    unsigned char send_buf[16 + 0 * 16];
    size_t send_buf_len = 16 + 0 * 16;
    size_t para_num = 0;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glFinish;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "GLint glTestInt1 GLint a, GLuint b" */
/* func name: "glTestInt1" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "0" */

GLint d_glTestInt1(void *context, GLint a, GLuint b)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    *(GLuint *)ptr = b;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestInt1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLint ret = *(GLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLuint glTestInt2 GLint a, GLuint b" */
/* func name: "glTestInt2" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLuint" */
/* type: "0" */

GLuint d_glTestInt2(void *context, GLint a, GLuint b)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    *(GLuint *)ptr = b;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestInt2;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLuint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLuint ret = *(GLuint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLint64 glTestInt3 GLint64 a, GLuint64 b" */
/* func name: "glTestInt3" */
/* args: [{'type': 'GLint64', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint64', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint64" */
/* type: "0" */

GLint64 d_glTestInt3(void *context, GLint64 a, GLuint64 b)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint64 *)ptr = a;
    ptr += 8;

    *(GLuint64 *)ptr = b;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestInt3;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLint64);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLint64 ret = *(GLint64 *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLuint64 glTestInt4 GLint64 a, GLuint64 b" */
/* func name: "glTestInt4" */
/* args: [{'type': 'GLint64', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint64', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLuint64" */
/* type: "0" */

GLuint64 d_glTestInt4(void *context, GLint64 a, GLuint64 b)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
    unsigned char *save_buf;

    unsigned char local_save_buf[16];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint64 *)ptr = a;
    ptr += 8;

    *(GLuint64 *)ptr = b;
    ptr += 8;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestInt4;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLuint64);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLuint64 ret = *(GLuint64 *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLfloat glTestInt5 GLint a, GLuint b" */
/* func name: "glTestInt5" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLfloat" */
/* type: "0" */

GLfloat d_glTestInt5(void *context, GLint a, GLuint b)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    *(GLuint *)ptr = b;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestInt5;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLfloat);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLfloat ret = *(GLfloat *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLdouble glTestInt6 GLint a, GLuint b" */
/* func name: "glTestInt6" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLdouble" */
/* type: "0" */

GLdouble d_glTestInt6(void *context, GLint a, GLuint b)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    *(GLuint *)ptr = b;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestInt6;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLdouble);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLdouble ret = *(GLdouble *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "glTestPointer1 GLint a, const GLint *b#sizeof(GLint)*10" */
/* func name: "glTestPointer1" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'b', 'ptr': 'in', 'ptr_len': 'sizeof(GLint)*10', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTestPointer1(void *context, GLint a, const GLint *b)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestPointer1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = sizeof(GLint) * 10;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)b;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTestPointer2 GLint a, const GLint *b#sizeof(GLint)*10, GLint *c#sizeof(GLint)*10" */
/* func name: "glTestPointer2" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'b', 'ptr': 'in', 'ptr_len': 'sizeof(GLint)*10', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'c', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)*10', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTestPointer2(void *context, GLint a, const GLint *b, GLint *c)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestPointer2;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = sizeof(GLint) * 10;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)b;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t c_len = (uint64_t)sizeof(GLint) * 10;
    out_buf_len += c_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (c != NULL)
    {

        memcpy(c, (GLint *)out_buf_ptr, c_len);
    }
    out_buf_ptr += c_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "GLint glTestPointer4 GLint a, const GLint *b#sizeof(GLint)*1000, GLint *c#sizeof(GLint)*1000" */
/* func name: "glTestPointer4" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'b', 'ptr': 'in', 'ptr_len': 'sizeof(GLint)*1000', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'c', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)*1000', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "0" */

GLint d_glTestPointer4(void *context, GLint a, const GLint *b, GLint *c)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestPointer4;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = sizeof(GLint) * 1000;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)b;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t c_len = (uint64_t)sizeof(GLint) * 1000;
    out_buf_len += c_len;

    out_buf_len += sizeof(GLint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (c != NULL)
    {

        memcpy(c, (GLint *)out_buf_ptr, c_len);
    }
    out_buf_ptr += c_len;

    GLint ret = *(GLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "glTestString GLint a, GLint count, const GLchar *const*strings#count|strlen(strings[i])+1, GLint buf_len, GLchar *char_buf#buf_len" */
/* func name: "glTestString" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'strings', 'ptr': 'in', 'ptr_len': 'count|strlen(strings[i])+1', 'loc': 2, 'ptr_ptr': True}, {'type': 'GLint', 'name': 'buf_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'char_buf', 'ptr': 'out', 'ptr_len': 'buf_len', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "0" */

void d_glTestString(void *context, GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf)
{

    unsigned char *send_buf = (unsigned char *)malloc(16 + 16 * (2 + count));
    size_t send_buf_len = 16 + 16 * (2 + count);
    size_t para_num = 2 + count;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
    unsigned char *save_buf;

    unsigned char local_save_buf[12];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    *(GLint *)ptr = count;
    ptr += 4;

    *(GLint *)ptr = buf_len;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestString;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    for (int i = 0; i < count; i++)
    {
        *(uint64_t *)ptr = (uint64_t)strlen(strings[i]) + 1;
        ptr += sizeof(uint64_t);
        *(uint64_t *)ptr = (uint64_t)strings[i];
        ptr += sizeof(uint64_t);
    }

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t char_buf_len = (uint64_t)buf_len;
    out_buf_len += char_buf_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    free(send_buf);
    out_buf_ptr = out_buf;
    if (char_buf != NULL)
    {

        memcpy(char_buf, (GLchar *)out_buf_ptr, char_buf_len);
    }
    out_buf_ptr += char_buf_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/******* end of file '1-1-1', 54/53 functions *******/

/******* file '1-1-2' *******/

/* readline: "glMapBufferRange_read GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf#length" */
/* func name: "glMapBufferRange_read" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'access', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'void*', 'name': 'mem_buf', 'ptr': 'out', 'ptr_len': 'length', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "1" */

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 24;
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
    *(uint64_t *)ptr = FUNID_glMapBufferRange_read;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)length;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)mem_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glReadPixels_without_bound GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels#buf_len" */
/* func name: "glReadPixels_without_bound" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'buf_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'void*', 'name': 'pixels', 'ptr': 'out', 'ptr_len': 'buf_len', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "1" */

void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 28;
    unsigned char *save_buf;

    unsigned char local_save_buf[28];
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

    *(GLint *)ptr = buf_len;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glReadPixels_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)pixels;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "GLint glTestPointer3 GLint a, const GLint *b#sizeof(GLint)*a, GLint *c#sizeof(GLint)*a" */
/* func name: "glTestPointer3" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'b', 'ptr': 'in', 'ptr_len': 'sizeof(GLint)*a', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'c', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)*a', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "1" */

GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c)
{

    unsigned char send_buf[16 + 4 * 16];
    size_t send_buf_len = 16 + 4 * 16;
    size_t para_num = 4;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glTestPointer3;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = sizeof(GLint) * a;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)b;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)sizeof(GLint) * a;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)c;
    ptr += sizeof(uint64_t);

    GLint ret = 0;
    *(uint64_t *)ptr = (uint64_t)sizeof(GLint);
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)&ret;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    return ret;
}

/******* end of file '1-1-2', 4/56 functions *******/

/******* file '1-2' *******/

/* readline: "GLenum glGetError void" */
/* func name: "glGetError" */
/* args: [] */
/* ret: "GLenum" */
/* type: "2" */

GLenum d_glGetError_origin(void *context)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetError;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLenum);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLenum ret = *(GLenum *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsBuffer GLuint buffer" */
/* func name: "glIsBuffer" */
/* args: [{'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsBuffer_origin(void *context, GLuint buffer)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = buffer;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsBuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsEnabled GLenum cap" */
/* func name: "glIsEnabled" */
/* args: [{'type': 'GLenum', 'name': 'cap', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsEnabled_origin(void *context, GLenum cap)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = cap;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsEnabled;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsFramebuffer GLuint framebuffer" */
/* func name: "glIsFramebuffer" */
/* args: [{'type': 'GLuint', 'name': 'framebuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsFramebuffer_origin(void *context, GLuint framebuffer)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = framebuffer;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsFramebuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsProgram GLuint program" */
/* func name: "glIsProgram" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsProgram_origin(void *context, GLuint program)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsProgram;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsRenderbuffer GLuint renderbuffer" */
/* func name: "glIsRenderbuffer" */
/* args: [{'type': 'GLuint', 'name': 'renderbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsRenderbuffer_origin(void *context, GLuint renderbuffer)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = renderbuffer;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsRenderbuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsShader GLuint shader" */
/* func name: "glIsShader" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsShader_origin(void *context, GLuint shader)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsShader;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsTexture GLuint texture" */
/* func name: "glIsTexture" */
/* args: [{'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsTexture_origin(void *context, GLuint texture)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = texture;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsTexture;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsQuery GLuint id" */
/* func name: "glIsQuery" */
/* args: [{'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsQuery_origin(void *context, GLuint id)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = id;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsQuery;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsVertexArray GLuint array" */
/* func name: "glIsVertexArray" */
/* args: [{'type': 'GLuint', 'name': 'array', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsVertexArray_origin(void *context, GLuint array)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = array;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsVertexArray;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsSampler GLuint sampler" */
/* func name: "glIsSampler" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsSampler_origin(void *context, GLuint sampler)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = sampler;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsSampler;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLboolean glIsTransformFeedback GLuint id" */
/* func name: "glIsTransformFeedback" */
/* args: [{'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "2" */

GLboolean d_glIsTransformFeedback_origin(void *context, GLuint id)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = id;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glIsTransformFeedback;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLboolean);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLboolean ret = *(GLboolean *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLint glGetAttribLocation GLuint program, const GLchar *name#strlen(name)+1" */
/* func name: "glGetAttribLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "2" */

GLint d_glGetAttribLocation_origin(void *context, GLuint program, const GLchar *name)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetAttribLocation;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = strlen(name) + 1;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)name;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLint ret = *(GLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLint glGetUniformLocation GLuint program, const GLchar *name#strlen(name)+1" */
/* func name: "glGetUniformLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "2" */

GLint d_glGetUniformLocation_origin(void *context, GLuint program, const GLchar *name)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetUniformLocation;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = strlen(name) + 1;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)name;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLint ret = *(GLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLint glGetFragDataLocation GLuint program, const GLchar *name#strlen(name)+1" */
/* func name: "glGetFragDataLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "2" */

GLint d_glGetFragDataLocation_origin(void *context, GLuint program, const GLchar *name)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetFragDataLocation;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = strlen(name) + 1;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)name;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLint ret = *(GLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLuint glGetUniformBlockIndex GLuint program, const GLchar *uniformBlockName#strlen(uniformBlockName)+1" */
/* func name: "glGetUniformBlockIndex" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'uniformBlockName', 'ptr': 'in', 'ptr_len': 'strlen(uniformBlockName)+1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "GLuint" */
/* type: "2" */

GLuint d_glGetUniformBlockIndex_origin(void *context, GLuint program, const GLchar *uniformBlockName)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetUniformBlockIndex;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = strlen(uniformBlockName) + 1;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)uniformBlockName;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLuint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLuint ret = *(GLuint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLuint glGetProgramResourceIndex GLuint program, GLenum programInterface, const GLchar *name#strlen(name)+1" */
/* func name: "glGetProgramResourceIndex" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLuint" */
/* type: "2" */

GLuint d_glGetProgramResourceIndex_origin(void *context, GLuint program, GLenum programInterface, const GLchar *name)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = programInterface;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetProgramResourceIndex;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = strlen(name) + 1;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)name;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLuint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLuint ret = *(GLuint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLint glGetProgramResourceLocation GLuint program, GLenum programInterface, const GLchar *name#strlen(name)+1" */
/* func name: "glGetProgramResourceLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'programInterface', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "GLint" */
/* type: "2" */

GLint d_glGetProgramResourceLocation_origin(void *context, GLuint program, GLenum programInterface, const GLchar *name)
{

    unsigned char send_buf[16 + 3 * 16];
    size_t send_buf_len = 16 + 3 * 16;
    size_t para_num = 3;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = programInterface;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetProgramResourceLocation;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = strlen(name) + 1;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)name;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLint ret = *(GLint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "glGetActiveAttrib GLuint program, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLint *size#sizeof(GLint), GLenum *type#sizeof(GLenum), GLchar *name#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetActiveAttrib" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'size', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'type', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetActiveAttrib_origin(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetActiveAttrib;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t size_len = (uint64_t)sizeof(GLint);
    out_buf_len += size_len;

    uint64_t type_len = (uint64_t)sizeof(GLenum);
    out_buf_len += type_len;

    uint64_t name_len = (uint64_t)bufSize;
    out_buf_len += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (size != NULL)
    {

        memcpy(size, (GLint *)out_buf_ptr, size_len);
    }
    out_buf_ptr += size_len;

    if (type != NULL)
    {

        memcpy(type, (GLenum *)out_buf_ptr, type_len);
    }
    out_buf_ptr += type_len;

    if (name != NULL)
    {

        memcpy(name, (GLchar *)out_buf_ptr, name_len);
    }
    out_buf_ptr += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetActiveUniform GLuint program, GLuint index, GLsizei bufSize, GLsizei *length#sizeof(GLsizei), GLint *size#sizeof(GLint), GLenum *type#sizeof(GLenum), GLchar *name#bufSize @{if(bufSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGetActiveUniform" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'bufSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'length', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'size', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLenum*', 'name': 'type', 'ptr': 'out', 'ptr_len': 'sizeof(GLenum)', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLchar*', 'name': 'name', 'ptr': 'out', 'ptr_len': 'bufSize', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetActiveUniform_origin(void *context, GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    {
        if (bufSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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
    *(uint64_t *)ptr = FUNID_glGetActiveUniform;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t length_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += length_len;

    uint64_t size_len = (uint64_t)sizeof(GLint);
    out_buf_len += size_len;

    uint64_t type_len = (uint64_t)sizeof(GLenum);
    out_buf_len += type_len;

    uint64_t name_len = (uint64_t)bufSize;
    out_buf_len += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (length != NULL)
    {

        memcpy(length, (GLsizei *)out_buf_ptr, length_len);
    }
    out_buf_ptr += length_len;

    if (size != NULL)
    {

        memcpy(size, (GLint *)out_buf_ptr, size_len);
    }
    out_buf_ptr += size_len;

    if (type != NULL)
    {

        memcpy(type, (GLenum *)out_buf_ptr, type_len);
    }
    out_buf_ptr += type_len;

    if (name != NULL)
    {

        memcpy(name, (GLchar *)out_buf_ptr, name_len);
    }
    out_buf_ptr += name_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetAttachedShaders GLuint program, GLsizei maxCount, GLsizei *count#sizeof(GLsizei), GLuint *shaders#maxCount*sizeof(GLuint)" */
/* func name: "glGetAttachedShaders" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'maxCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei*', 'name': 'count', 'ptr': 'out', 'ptr_len': 'sizeof(GLsizei)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'shaders', 'ptr': 'out', 'ptr_len': 'maxCount*sizeof(GLuint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetAttachedShaders_origin(void *context, GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = maxCount;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetAttachedShaders;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t count_len = (uint64_t)sizeof(GLsizei);
    out_buf_len += count_len;

    uint64_t shaders_len = (uint64_t)maxCount * sizeof(GLuint);
    out_buf_len += shaders_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (count != NULL)
    {

        memcpy(count, (GLsizei *)out_buf_ptr, count_len);
    }
    out_buf_ptr += count_len;

    if (shaders != NULL)
    {

        memcpy(shaders, (GLuint *)out_buf_ptr, shaders_len);
    }
    out_buf_ptr += shaders_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetProgramiv GLuint program, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetProgramiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetProgramiv_origin(void *context, GLuint program, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetProgramiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetShaderiv GLuint shader, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetShaderiv" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetShaderiv_origin(void *context, GLuint shader, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetShaderiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetUniformfv GLuint program, GLint location, GLfloat *params#gl_get_program_uniform_size(context,program,location)" */
/* func name: "glGetUniformfv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_program_uniform_size(context,program,location)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetUniformfv_origin(void *context, GLuint program, GLint location, GLfloat *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetUniformfv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_get_program_uniform_size(context, program, location);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfloat *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetUniformiv GLuint program, GLint location, GLint *params#gl_get_program_uniform_size(context,program,location)" */
/* func name: "glGetUniformiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_program_uniform_size(context,program,location)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetUniformiv_origin(void *context, GLuint program, GLint location, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetUniformiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_get_program_uniform_size(context, program, location);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetUniformuiv GLuint program, GLint location, GLuint *params#gl_get_program_uniform_size(context,program,location)" */
/* func name: "glGetUniformuiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_get_program_uniform_size(context,program,location)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetUniformuiv_origin(void *context, GLuint program, GLint location, GLuint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLint *)ptr = location;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetUniformuiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_get_program_uniform_size(context, program, location);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLuint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetUniformIndices GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames#uniformCount|strlen(uniformNames[i])+1, GLuint *uniformIndices#uniformCount*sizeof(GLuint)" */
/* func name: "glGetUniformIndices" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'uniformCount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'uniformNames', 'ptr': 'in', 'ptr_len': 'uniformCount|strlen(uniformNames[i])+1', 'loc': 2, 'ptr_ptr': True}, {'type': 'GLuint*', 'name': 'uniformIndices', 'ptr': 'out', 'ptr_len': 'uniformCount*sizeof(GLuint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetUniformIndices_origin(void *context, GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices)
{

    unsigned char *send_buf = (unsigned char *)malloc(16 + 16 * (2 + uniformCount));
    size_t send_buf_len = 16 + 16 * (2 + uniformCount);
    size_t para_num = 2 + uniformCount;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = uniformCount;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetUniformIndices;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    for (int i = 0; i < uniformCount; i++)
    {
        *(uint64_t *)ptr = (uint64_t)strlen(uniformNames[i]) + 1;
        ptr += sizeof(uint64_t);
        *(uint64_t *)ptr = (uint64_t)uniformNames[i];
        ptr += sizeof(uint64_t);
    }

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t uniformIndices_len = (uint64_t)uniformCount * sizeof(GLuint);
    out_buf_len += uniformIndices_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    free(send_buf);
    out_buf_ptr = out_buf;
    if (uniformIndices != NULL)
    {

        memcpy(uniformIndices, (GLuint *)out_buf_ptr, uniformIndices_len);
    }
    out_buf_ptr += uniformIndices_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetVertexAttribfv_origin GLuint index, GLenum pname, GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetVertexAttribfv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetVertexAttribfv_origin(void *context, GLuint index, GLenum pname, GLfloat *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetVertexAttribfv_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfloat);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLfloat *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetVertexAttribiv_origin GLuint index, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetVertexAttribiv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetVertexAttribiv_origin(void *context, GLuint index, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetVertexAttribiv_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetVertexAttribIiv_origin GLuint index, GLenum pname, GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetVertexAttribIiv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetVertexAttribIiv_origin(void *context, GLuint index, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetVertexAttribIiv_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetVertexAttribIuiv_origin GLuint index, GLenum pname, GLuint *params#gl_pname_size(pname)*sizeof(GLuint)" */
/* func name: "glGetVertexAttribIuiv_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLuint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetVertexAttribIuiv_origin(void *context, GLuint index, GLenum pname, GLuint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetVertexAttribIuiv_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLuint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLuint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetBufferParameteriv GLenum target, GLenum pname, GLint *params#sizeof(GLint)" */
/* func name: "glGetBufferParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetBufferParameteriv_origin(void *context, GLenum target, GLenum pname, GLint *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetBufferParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)sizeof(GLint);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetBufferParameteri64v GLenum target, GLenum pname, GLint64 *params#gl_pname_size(pname)*sizeof(GLint64)" */
/* func name: "glGetBufferParameteri64v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint64*', 'name': 'params', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint64)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetBufferParameteri64v_origin(void *context, GLenum target, GLenum pname, GLint64 *params)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetBufferParameteri64v;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t params_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint64);
    out_buf_len += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (params != NULL)
    {

        memcpy(params, (GLint64 *)out_buf_ptr, params_len);
    }
    out_buf_ptr += params_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetBooleanv GLenum pname, GLboolean *data#gl_pname_size(pname)*sizeof(GLboolean)" */
/* func name: "glGetBooleanv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLboolean)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetBooleanv_origin(void *context, GLenum pname, GLboolean *data)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetBooleanv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t data_len = (uint64_t)gl_pname_size(pname) * sizeof(GLboolean);
    out_buf_len += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (data != NULL)
    {

        memcpy(data, (GLboolean *)out_buf_ptr, data_len);
    }
    out_buf_ptr += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetBooleani_v GLenum target, GLuint index, GLboolean *data#sizeof(GLboolean)" */
/* func name: "glGetBooleani_v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'sizeof(GLboolean)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetBooleani_v_origin(void *context, GLenum target, GLuint index, GLboolean *data)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetBooleani_v;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t data_len = (uint64_t)sizeof(GLboolean);
    out_buf_len += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (data != NULL)
    {

        memcpy(data, (GLboolean *)out_buf_ptr, data_len);
    }
    out_buf_ptr += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetFloatv GLenum pname, GLfloat *data#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glGetFloatv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetFloatv_origin(void *context, GLenum pname, GLfloat *data)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetFloatv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t data_len = (uint64_t)gl_pname_size(pname) * sizeof(GLfloat);
    out_buf_len += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (data != NULL)
    {

        memcpy(data, (GLfloat *)out_buf_ptr, data_len);
    }
    out_buf_ptr += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetIntegerv GLenum pname, GLint *data#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glGetIntegerv" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetIntegerv_origin(void *context, GLenum pname, GLint *data)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetIntegerv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t data_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint);
    out_buf_len += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (data != NULL)
    {

        memcpy(data, (GLint *)out_buf_ptr, data_len);
    }
    out_buf_ptr += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetIntegeri_v GLenum target, GLuint index, GLint *data#sizeof(GLint)" */
/* func name: "glGetIntegeri_v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetIntegeri_v_origin(void *context, GLenum target, GLuint index, GLint *data)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetIntegeri_v;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t data_len = (uint64_t)sizeof(GLint);
    out_buf_len += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (data != NULL)
    {

        memcpy(data, (GLint *)out_buf_ptr, data_len);
    }
    out_buf_ptr += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetInteger64v GLenum pname, GLint64 *data#gl_pname_size(pname)*sizeof(GLint64)" */
/* func name: "glGetInteger64v" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint64*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint64)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetInteger64v_origin(void *context, GLenum pname, GLint64 *data)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = pname;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetInteger64v;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t data_len = (uint64_t)gl_pname_size(pname) * sizeof(GLint64);
    out_buf_len += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (data != NULL)
    {

        memcpy(data, (GLint64 *)out_buf_ptr, data_len);
    }
    out_buf_ptr += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGetInteger64i_v GLenum target, GLuint index, GLint64 *data#sizeof(GLint64)" */
/* func name: "glGetInteger64i_v" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint64*', 'name': 'data', 'ptr': 'out', 'ptr_len': 'sizeof(GLint64)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "2" */

void d_glGetInteger64i_v_origin(void *context, GLenum target, GLuint index, GLint64 *data)
{

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 8;
    unsigned char *save_buf;

    unsigned char local_save_buf[8];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGetInteger64i_v;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t data_len = (uint64_t)sizeof(GLint64);
    out_buf_len += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (data != NULL)
    {

        memcpy(data, (GLint64 *)out_buf_ptr, data_len);
    }
    out_buf_ptr += data_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/******* end of file '1-2', 41/96 functions *******/

/******* file '2-1-1' *******/

/* readline: "glActiveTexture GLenum texture" */
/* func name: "glActiveTexture" */
/* args: [{'type': 'GLenum', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glActiveTexture(void *context, GLenum texture)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = texture;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glActiveTexture;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glAttachShader GLuint program, GLuint shader" */
/* func name: "glAttachShader" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glAttachShader(void *context, GLuint program, GLuint shader)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glAttachShader;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindBuffer_origin GLenum target, GLuint buffer" */
/* func name: "glBindBuffer_origin" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindBuffer_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindFramebuffer_special GLenum target, GLuint framebuffer" */
/* func name: "glBindFramebuffer_special" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'framebuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindFramebuffer_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindRenderbuffer GLenum target, GLuint renderbuffer" */
/* func name: "glBindRenderbuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'renderbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindRenderbuffer(void *context, GLenum target, GLuint renderbuffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindRenderbuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindTexture GLenum target, GLuint texture" */
/* func name: "glBindTexture" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindTexture(void *context, GLenum target, GLuint texture)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindTexture;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBlendColor GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha" */
/* func name: "glBlendColor" */
/* args: [{'type': 'GLfloat', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBlendColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBlendColor;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBlendEquation GLenum mode" */
/* func name: "glBlendEquation" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBlendEquation(void *context, GLenum mode)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glBlendEquation;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBlendEquationSeparate GLenum modeRGB, GLenum modeAlpha" */
/* func name: "glBlendEquationSeparate" */
/* args: [{'type': 'GLenum', 'name': 'modeRGB', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'modeAlpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBlendEquationSeparate(void *context, GLenum modeRGB, GLenum modeAlpha)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBlendEquationSeparate;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBlendFunc GLenum sfactor, GLenum dfactor" */
/* func name: "glBlendFunc" */
/* args: [{'type': 'GLenum', 'name': 'sfactor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dfactor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBlendFunc(void *context, GLenum sfactor, GLenum dfactor)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBlendFunc;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBlendFuncSeparate GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha" */
/* func name: "glBlendFuncSeparate" */
/* args: [{'type': 'GLenum', 'name': 'sfactorRGB', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dfactorRGB', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'sfactorAlpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dfactorAlpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBlendFuncSeparate(void *context, GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBlendFuncSeparate;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClear GLbitfield mask" */
/* func name: "glClear" */
/* args: [{'type': 'GLbitfield', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClear(void *context, GLbitfield mask)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLbitfield *)ptr = mask;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glClear;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearColor GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha" */
/* func name: "glClearColor" */
/* args: [{'type': 'GLfloat', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearColor(void *context, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClearColor;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearDepthf GLfloat d" */
/* func name: "glClearDepthf" */
/* args: [{'type': 'GLfloat', 'name': 'd', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearDepthf(void *context, GLfloat d)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = d;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glClearDepthf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearStencil GLint s" */
/* func name: "glClearStencil" */
/* args: [{'type': 'GLint', 'name': 's', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearStencil(void *context, GLint s)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = s;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glClearStencil;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glColorMask GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha" */
/* func name: "glColorMask" */
/* args: [{'type': 'GLboolean', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glColorMask(void *context, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glColorMask;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCompileShader GLuint shader" */
/* func name: "glCompileShader" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCompileShader(void *context, GLuint shader)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glCompileShader;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCopyTexImage2D GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border" */
/* func name: "glCopyTexImage2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCopyTexImage2D(void *context, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 32;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCopyTexImage2D;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCopyTexSubImage2D GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glCopyTexSubImage2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCopyTexSubImage2D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 32;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCopyTexSubImage2D;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCullFace GLenum mode" */
/* func name: "glCullFace" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCullFace(void *context, GLenum mode)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glCullFace;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteProgram_origin GLuint program" */
/* func name: "glDeleteProgram_origin" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteProgram_origin(void *context, GLuint program)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glDeleteProgram_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteShader GLuint shader" */
/* func name: "glDeleteShader" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteShader(void *context, GLuint shader)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glDeleteShader;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDepthFunc GLenum func" */
/* func name: "glDepthFunc" */
/* args: [{'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDepthFunc(void *context, GLenum func)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = func;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glDepthFunc;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDepthMask GLboolean flag" */
/* func name: "glDepthMask" */
/* args: [{'type': 'GLboolean', 'name': 'flag', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDepthMask(void *context, GLboolean flag)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLboolean *)ptr = flag;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glDepthMask;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDepthRangef GLfloat n, GLfloat f" */
/* func name: "glDepthRangef" */
/* args: [{'type': 'GLfloat', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDepthRangef(void *context, GLfloat n, GLfloat f)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDepthRangef;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDetachShader GLuint program, GLuint shader" */
/* func name: "glDetachShader" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDetachShader(void *context, GLuint program, GLuint shader)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDetachShader;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDisable GLenum cap" */
/* func name: "glDisable" */
/* args: [{'type': 'GLenum', 'name': 'cap', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDisable(void *context, GLenum cap)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = cap;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glDisable;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDisableVertexAttribArray_origin GLuint index" */
/* func name: "glDisableVertexAttribArray_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glDisableVertexAttribArray_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDrawArrays_origin GLenum mode, GLint first, GLsizei count" */
/* func name: "glDrawArrays_origin" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'first', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDrawArrays_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glEnable GLenum cap" */
/* func name: "glEnable" */
/* args: [{'type': 'GLenum', 'name': 'cap', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glEnable(void *context, GLenum cap)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = cap;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glEnable;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glEnableVertexAttribArray_origin GLuint index" */
/* func name: "glEnableVertexAttribArray_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glEnableVertexAttribArray_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFlush void" */
/* func name: "glFlush" */
/* args: [] */
/* ret: "" */
/* type: "3" */

void d_glFlush(void *context)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glFlush;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFramebufferRenderbuffer GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer" */
/* func name: "glFramebufferRenderbuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'renderbuffertarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'renderbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFramebufferRenderbuffer(void *context, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glFramebufferRenderbuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFramebufferTexture2D GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level" */
/* func name: "glFramebufferTexture2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'textarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFramebufferTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glFramebufferTexture2D;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFrontFace GLenum mode" */
/* func name: "glFrontFace" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFrontFace(void *context, GLenum mode)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = mode;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glFrontFace;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glGenerateMipmap GLenum target" */
/* func name: "glGenerateMipmap" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glGenerateMipmap(void *context, GLenum target)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glGenerateMipmap;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glHint GLenum target, GLenum mode" */
/* func name: "glHint" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glHint(void *context, GLenum target, GLenum mode)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glHint;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLineWidth GLfloat width" */
/* func name: "glLineWidth" */
/* args: [{'type': 'GLfloat', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLineWidth(void *context, GLfloat width)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfloat *)ptr = width;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glLineWidth;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLinkProgram_origin GLuint program" */
/* func name: "glLinkProgram_origin" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLinkProgram_origin(void *context, GLuint program)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glLinkProgram_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPixelStorei_origin GLenum pname, GLint param" */
/* func name: "glPixelStorei_origin" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glPixelStorei_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPolygonOffset GLfloat factor, GLfloat units" */
/* func name: "glPolygonOffset" */
/* args: [{'type': 'GLfloat', 'name': 'factor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'units', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glPolygonOffset(void *context, GLfloat factor, GLfloat units)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glPolygonOffset;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glReleaseShaderCompiler void" */
/* func name: "glReleaseShaderCompiler" */
/* args: [] */
/* ret: "" */
/* type: "3" */

void d_glReleaseShaderCompiler(void *context)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glReleaseShaderCompiler;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glRenderbufferStorage GLenum target, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glRenderbufferStorage" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glRenderbufferStorage(void *context, GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glRenderbufferStorage;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glSampleCoverage GLfloat value, GLboolean invert" */
/* func name: "glSampleCoverage" */
/* args: [{'type': 'GLfloat', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'invert', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glSampleCoverage(void *context, GLfloat value, GLboolean invert)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glSampleCoverage;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glScissor GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glScissor" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glScissor(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glScissor;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glStencilFunc GLenum func, GLint ref, GLuint mask" */
/* func name: "glStencilFunc" */
/* args: [{'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'ref', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glStencilFunc(void *context, GLenum func, GLint ref, GLuint mask)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glStencilFunc;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glStencilFuncSeparate GLenum face, GLenum func, GLint ref, GLuint mask" */
/* func name: "glStencilFuncSeparate" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'ref', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glStencilFuncSeparate(void *context, GLenum face, GLenum func, GLint ref, GLuint mask)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glStencilFuncSeparate;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glStencilMask GLuint mask" */
/* func name: "glStencilMask" */
/* args: [{'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glStencilMask(void *context, GLuint mask)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = mask;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glStencilMask;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glStencilMaskSeparate GLenum face, GLuint mask" */
/* func name: "glStencilMaskSeparate" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glStencilMaskSeparate(void *context, GLenum face, GLuint mask)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glStencilMaskSeparate;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glStencilOp GLenum fail, GLenum zfail, GLenum zpass" */
/* func name: "glStencilOp" */
/* args: [{'type': 'GLenum', 'name': 'fail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'zfail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'zpass', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glStencilOp(void *context, GLenum fail, GLenum zfail, GLenum zpass)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glStencilOp;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glStencilOpSeparate GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass" */
/* func name: "glStencilOpSeparate" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'sfail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dpfail', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'dppass', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glStencilOpSeparate(void *context, GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glStencilOpSeparate;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameterf GLenum target, GLenum pname, GLfloat param" */
/* func name: "glTexParameterf" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexParameterf(void *context, GLenum target, GLenum pname, GLfloat param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexParameterf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameteri GLenum target, GLenum pname, GLint param" */
/* func name: "glTexParameteri" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexParameteri(void *context, GLenum target, GLenum pname, GLint param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexParameteri;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1f GLint location, GLfloat v0" */
/* func name: "glUniform1f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform1f(void *context, GLint location, GLfloat v0)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform1f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1i GLint location, GLint v0" */
/* func name: "glUniform1i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform1i(void *context, GLint location, GLint v0)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform1i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2f GLint location, GLfloat v0, GLfloat v1" */
/* func name: "glUniform2f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform2f(void *context, GLint location, GLfloat v0, GLfloat v1)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform2f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2i GLint location, GLint v0, GLint v1" */
/* func name: "glUniform2i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform2i(void *context, GLint location, GLint v0, GLint v1)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform2i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3f GLint location, GLfloat v0, GLfloat v1, GLfloat v2" */
/* func name: "glUniform3f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform3f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform3f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3i GLint location, GLint v0, GLint v1, GLint v2" */
/* func name: "glUniform3i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform3i(void *context, GLint location, GLint v0, GLint v1, GLint v2)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform3i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4f GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3" */
/* func name: "glUniform4f" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform4f(void *context, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform4f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4i GLint location, GLint v0, GLint v1, GLint v2, GLint v3" */
/* func name: "glUniform4i" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform4i(void *context, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform4i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUseProgram GLuint program" */
/* func name: "glUseProgram" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUseProgram(void *context, GLuint program)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glUseProgram;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glValidateProgram GLuint program" */
/* func name: "glValidateProgram" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glValidateProgram(void *context, GLuint program)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glValidateProgram;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib1f GLuint index, GLfloat x" */
/* func name: "glVertexAttrib1f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib1f(void *context, GLuint index, GLfloat x)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib1f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib2f GLuint index, GLfloat x, GLfloat y" */
/* func name: "glVertexAttrib2f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib2f(void *context, GLuint index, GLfloat x, GLfloat y)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib2f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib3f GLuint index, GLfloat x, GLfloat y, GLfloat z" */
/* func name: "glVertexAttrib3f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib3f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib3f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib4f GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w" */
/* func name: "glVertexAttrib4f" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'w', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib4f(void *context, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib4f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glViewport GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glViewport" */
/* args: [{'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glViewport(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glViewport;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glReadBuffer_special GLenum src" */
/* func name: "glReadBuffer_special" */
/* args: [{'type': 'GLenum', 'name': 'src', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glReadBuffer_special(void *context, GLenum src)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = src;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glReadBuffer_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCopyTexSubImage3D GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glCopyTexSubImage3D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCopyTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 36;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCopyTexSubImage3D;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBeginQuery GLenum target, GLuint id" */
/* func name: "glBeginQuery" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBeginQuery(void *context, GLenum target, GLuint id)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBeginQuery;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glEndQuery GLenum target" */
/* func name: "glEndQuery" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glEndQuery(void *context, GLenum target)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glEndQuery;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBlitFramebuffer GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter" */
/* func name: "glBlitFramebuffer" */
/* args: [{'type': 'GLint', 'name': 'srcX0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'srcY0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'srcX1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'srcY1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstX0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstY0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstX1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'dstY1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'mask', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'filter', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBlitFramebuffer(void *context, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 40;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBlitFramebuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glRenderbufferStorageMultisample GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glRenderbufferStorageMultisample" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'samples', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glRenderbufferStorageMultisample(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glRenderbufferStorageMultisample;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFramebufferTextureLayer GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer" */
/* func name: "glFramebufferTextureLayer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'attachment', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'layer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFramebufferTextureLayer(void *context, GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glFramebufferTextureLayer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindVertexArray_origin GLuint array" */
/* func name: "glBindVertexArray_origin" */
/* args: [{'type': 'GLuint', 'name': 'array', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindVertexArray_origin(void *context, GLuint array)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = array;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glBindVertexArray_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBeginTransformFeedback GLenum primitiveMode" */
/* func name: "glBeginTransformFeedback" */
/* args: [{'type': 'GLenum', 'name': 'primitiveMode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBeginTransformFeedback(void *context, GLenum primitiveMode)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = primitiveMode;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glBeginTransformFeedback;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glEndTransformFeedback void" */
/* func name: "glEndTransformFeedback" */
/* args: [] */
/* ret: "" */
/* type: "3" */

void d_glEndTransformFeedback(void *context)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glEndTransformFeedback;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindBufferRange GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size" */
/* func name: "glBindBufferRange" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindBufferRange(void *context, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 28;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindBufferRange;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindBufferBase GLenum target, GLuint index, GLuint buffer" */
/* func name: "glBindBufferBase" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindBufferBase(void *context, GLenum target, GLuint index, GLuint buffer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindBufferBase;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4i GLuint index, GLint x, GLint y, GLint z, GLint w" */
/* func name: "glVertexAttribI4i" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'w', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribI4i(void *context, GLuint index, GLint x, GLint y, GLint z, GLint w)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttribI4i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4ui GLuint index, GLuint x, GLuint y, GLuint z, GLuint w" */
/* func name: "glVertexAttribI4ui" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'w', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribI4ui(void *context, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttribI4ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1ui GLint location, GLuint v0" */
/* func name: "glUniform1ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform1ui(void *context, GLint location, GLuint v0)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform1ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2ui GLint location, GLuint v0, GLuint v1" */
/* func name: "glUniform2ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform2ui(void *context, GLint location, GLuint v0, GLuint v1)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform2ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3ui GLint location, GLuint v0, GLuint v1, GLuint v2" */
/* func name: "glUniform3ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform3ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform3ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4ui GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3" */
/* func name: "glUniform4ui" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform4ui(void *context, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform4ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferfi GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil" */
/* func name: "glClearBufferfi" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'stencil', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearBufferfi(void *context, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClearBufferfi;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCopyBufferSubData GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size" */
/* func name: "glCopyBufferSubData" */
/* args: [{'type': 'GLenum', 'name': 'readTarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'writeTarget', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'readOffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'writeOffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCopyBufferSubData(void *context, GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 32;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCopyBufferSubData;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformBlockBinding GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding" */
/* func name: "glUniformBlockBinding" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockIndex', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'uniformBlockBinding', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformBlockBinding(void *context, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformBlockBinding;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDrawArraysInstanced_origin GLenum mode, GLint first, GLsizei count, GLsizei instancecount" */
/* func name: "glDrawArraysInstanced_origin" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'first', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'instancecount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDrawArraysInstanced_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindSampler GLuint unit, GLuint sampler" */
/* func name: "glBindSampler" */
/* args: [{'type': 'GLuint', 'name': 'unit', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindSampler(void *context, GLuint unit, GLuint sampler)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindSampler;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameteri GLuint sampler, GLenum pname, GLint param" */
/* func name: "glSamplerParameteri" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glSamplerParameteri(void *context, GLuint sampler, GLenum pname, GLint param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glSamplerParameteri;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameterf GLuint sampler, GLenum pname, GLfloat param" */
/* func name: "glSamplerParameterf" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glSamplerParameterf(void *context, GLuint sampler, GLenum pname, GLfloat param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glSamplerParameterf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribDivisor_origin GLuint index, GLuint divisor" */
/* func name: "glVertexAttribDivisor_origin" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'divisor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttribDivisor_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindTransformFeedback GLenum target, GLuint id" */
/* func name: "glBindTransformFeedback" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'id', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindTransformFeedback(void *context, GLenum target, GLuint id)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glBindTransformFeedback;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPauseTransformFeedback void" */
/* func name: "glPauseTransformFeedback" */
/* args: [] */
/* ret: "" */
/* type: "3" */

void d_glPauseTransformFeedback(void *context)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glPauseTransformFeedback;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glResumeTransformFeedback void" */
/* func name: "glResumeTransformFeedback" */
/* args: [] */
/* ret: "" */
/* type: "3" */

void d_glResumeTransformFeedback(void *context)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glResumeTransformFeedback;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramParameteri GLuint program, GLenum pname, GLint value" */
/* func name: "glProgramParameteri" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramParameteri(void *context, GLuint program, GLenum pname, GLint value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramParameteri;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexStorage2D GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glTexStorage2D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'levels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexStorage2D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexStorage2D;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexStorage3D GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth" */
/* func name: "glTexStorage3D" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'levels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexStorage3D(void *context, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexStorage3D;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glAlphaFuncxOES GLenum func, GLfixed ref" */
/* func name: "glAlphaFuncxOES" */
/* args: [{'type': 'GLenum', 'name': 'func', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'ref', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glAlphaFuncxOES(void *context, GLenum func, GLfixed ref)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glAlphaFuncxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearColorxOES GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha" */
/* func name: "glClearColorxOES" */
/* args: [{'type': 'GLfixed', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearColorxOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClearColorxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearDepthxOES GLfixed depth" */
/* func name: "glClearDepthxOES" */
/* args: [{'type': 'GLfixed', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearDepthxOES(void *context, GLfixed depth)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = depth;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glClearDepthxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glColor4xOES GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha" */
/* func name: "glColor4xOES" */
/* args: [{'type': 'GLfixed', 'name': 'red', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'green', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'blue', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'alpha', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glColor4xOES(void *context, GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glColor4xOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDepthRangexOES GLfixed n, GLfixed f" */
/* func name: "glDepthRangexOES" */
/* args: [{'type': 'GLfixed', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDepthRangexOES(void *context, GLfixed n, GLfixed f)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDepthRangexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFogxOES GLenum pname, GLfixed param" */
/* func name: "glFogxOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFogxOES(void *context, GLenum pname, GLfixed param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glFogxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFrustumxOES GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f" */
/* func name: "glFrustumxOES" */
/* args: [{'type': 'GLfixed', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFrustumxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glFrustumxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLightModelxOES GLenum pname, GLfixed param" */
/* func name: "glLightModelxOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLightModelxOES(void *context, GLenum pname, GLfixed param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glLightModelxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLightxOES GLenum light, GLenum pname, GLfixed param" */
/* func name: "glLightxOES" */
/* args: [{'type': 'GLenum', 'name': 'light', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLightxOES(void *context, GLenum light, GLenum pname, GLfixed param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glLightxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLineWidthxOES GLfixed width" */
/* func name: "glLineWidthxOES" */
/* args: [{'type': 'GLfixed', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLineWidthxOES(void *context, GLfixed width)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = width;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glLineWidthxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glMaterialxOES GLenum face, GLenum pname, GLfixed param" */
/* func name: "glMaterialxOES" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glMaterialxOES(void *context, GLenum face, GLenum pname, GLfixed param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glMaterialxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glMultiTexCoord4xOES GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q" */
/* func name: "glMultiTexCoord4xOES" */
/* args: [{'type': 'GLenum', 'name': 'texture', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 's', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'q', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glMultiTexCoord4xOES(void *context, GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glMultiTexCoord4xOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glNormal3xOES GLfixed nx, GLfixed ny, GLfixed nz" */
/* func name: "glNormal3xOES" */
/* args: [{'type': 'GLfixed', 'name': 'nx', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'ny', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'nz', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glNormal3xOES(void *context, GLfixed nx, GLfixed ny, GLfixed nz)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glNormal3xOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glOrthoxOES GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f" */
/* func name: "glOrthoxOES" */
/* args: [{'type': 'GLfixed', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glOrthoxOES(void *context, GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glOrthoxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPointSizexOES GLfixed size" */
/* func name: "glPointSizexOES" */
/* args: [{'type': 'GLfixed', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glPointSizexOES(void *context, GLfixed size)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLfixed *)ptr = size;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glPointSizexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPolygonOffsetxOES GLfixed factor, GLfixed units" */
/* func name: "glPolygonOffsetxOES" */
/* args: [{'type': 'GLfixed', 'name': 'factor', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'units', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glPolygonOffsetxOES(void *context, GLfixed factor, GLfixed units)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glPolygonOffsetxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glRotatexOES GLfixed angle, GLfixed x, GLfixed y, GLfixed z" */
/* func name: "glRotatexOES" */
/* args: [{'type': 'GLfixed', 'name': 'angle', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glRotatexOES(void *context, GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glRotatexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glScalexOES GLfixed x, GLfixed y, GLfixed z" */
/* func name: "glScalexOES" */
/* args: [{'type': 'GLfixed', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glScalexOES(void *context, GLfixed x, GLfixed y, GLfixed z)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glScalexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexEnvxOES GLenum target, GLenum pname, GLfixed param" */
/* func name: "glTexEnvxOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexEnvxOES(void *context, GLenum target, GLenum pname, GLfixed param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexEnvxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTranslatexOES GLfixed x, GLfixed y, GLfixed z" */
/* func name: "glTranslatexOES" */
/* args: [{'type': 'GLfixed', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'z', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTranslatexOES(void *context, GLfixed x, GLfixed y, GLfixed z)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTranslatexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPointParameterxOES GLenum pname, GLfixed param" */
/* func name: "glPointParameterxOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glPointParameterxOES(void *context, GLenum pname, GLfixed param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glPointParameterxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glSampleCoveragexOES GLclampx value, GLboolean invert" */
/* func name: "glSampleCoveragexOES" */
/* args: [{'type': 'GLclampx', 'name': 'value', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'invert', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glSampleCoveragexOES(void *context, GLclampx value, GLboolean invert)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glSampleCoveragexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexGenxOES GLenum coord, GLenum pname, GLfixed param" */
/* func name: "glTexGenxOES" */
/* args: [{'type': 'GLenum', 'name': 'coord', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfixed', 'name': 'param', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexGenxOES(void *context, GLenum coord, GLenum pname, GLfixed param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexGenxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearDepthfOES GLclampf depth" */
/* func name: "glClearDepthfOES" */
/* args: [{'type': 'GLclampf', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearDepthfOES(void *context, GLclampf depth)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLclampf *)ptr = depth;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glClearDepthfOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDepthRangefOES GLclampf n, GLclampf f" */
/* func name: "glDepthRangefOES" */
/* args: [{'type': 'GLclampf', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLclampf', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDepthRangefOES(void *context, GLclampf n, GLclampf f)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDepthRangefOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFrustumfOES GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f" */
/* func name: "glFrustumfOES" */
/* args: [{'type': 'GLfloat', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFrustumfOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glFrustumfOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glOrthofOES GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f" */
/* func name: "glOrthofOES" */
/* args: [{'type': 'GLfloat', 'name': 'l', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'r', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 't', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'f', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glOrthofOES(void *context, GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glOrthofOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glRenderbufferStorageMultisampleEXT GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height" */
/* func name: "glRenderbufferStorageMultisampleEXT" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'samples', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glRenderbufferStorageMultisampleEXT(void *context, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glRenderbufferStorageMultisampleEXT;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUseProgramStages GLuint pipeline, GLbitfield stages, GLuint program" */
/* func name: "glUseProgramStages" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'stages', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUseProgramStages(void *context, GLuint pipeline, GLbitfield stages, GLuint program)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUseProgramStages;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glActiveShaderProgram GLuint pipeline, GLuint program" */
/* func name: "glActiveShaderProgram" */
/* args: [{'type': 'GLuint', 'name': 'pipeline', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glActiveShaderProgram(void *context, GLuint pipeline, GLuint program)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glActiveShaderProgram;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1i GLuint program, GLint location, GLint v0" */
/* func name: "glProgramUniform1i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform1i(void *context, GLuint program, GLint location, GLint v0)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform1i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2i GLuint program, GLint location, GLint v0, GLint v1" */
/* func name: "glProgramUniform2i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform2i(void *context, GLuint program, GLint location, GLint v0, GLint v1)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform2i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3i GLuint program, GLint location, GLint v0, GLint v1, GLint v2" */
/* func name: "glProgramUniform3i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform3i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform3i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4i GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3" */
/* func name: "glProgramUniform4i" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform4i(void *context, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform4i;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1ui GLuint program, GLint location, GLuint v0" */
/* func name: "glProgramUniform1ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform1ui(void *context, GLuint program, GLint location, GLuint v0)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform1ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2ui GLuint program, GLint location, GLuint v0, GLuint v1" */
/* func name: "glProgramUniform2ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform2ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform2ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3ui GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2" */
/* func name: "glProgramUniform3ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform3ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform3ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4ui GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3" */
/* func name: "glProgramUniform4ui" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform4ui(void *context, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform4ui;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1f GLuint program, GLint location, GLfloat v0" */
/* func name: "glProgramUniform1f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform1f(void *context, GLuint program, GLint location, GLfloat v0)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform1f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2f GLuint program, GLint location, GLfloat v0, GLfloat v1" */
/* func name: "glProgramUniform2f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform2f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform2f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3f GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2" */
/* func name: "glProgramUniform3f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform3f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform3f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4f GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3" */
/* func name: "glProgramUniform4f" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v0', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v1', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v2', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'v3', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform4f(void *context, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform4f;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glShaderSource_origin GLuint shader, GLsizei count, const GLint *length#count*sizeof(GLint), const GLchar *const*string#count|length[i]" */
/* func name: "glShaderSource_origin" */
/* args: [{'type': 'GLuint', 'name': 'shader', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'length', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'string', 'ptr': 'in', 'ptr_len': 'count|length[i]', 'loc': 3, 'ptr_ptr': True}] */
/* ret: "" */
/* type: "3" */

void d_glShaderSource_origin(void *context, GLuint shader, GLsizei count, const GLint *length, const GLchar *const *string)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t length_len = count * sizeof(GLint);
    save_buf_len += length_len;

    size_t *string_len = (size_t *)malloc(count * sizeof(size_t));
    for (int i = 0; i < count; i++)
    {
        string_len[i] = length[i];
        save_buf_len += string_len[i];
    }

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = shader;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    memcpy(ptr, (unsigned char *)length, length_len);
    ptr += length_len;

    for (int i = 0; i < count; i++)
    {
        memcpy(ptr, (unsigned char *)string[i], string_len[i]);
        ptr += string_len[i];
    }

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glShaderSource_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTransformFeedbackVaryings GLuint program, GLsizei count, const GLchar *const*varyings#count|strlen(varyings[i])+1, GLenum bufferMode" */
/* func name: "glTransformFeedbackVaryings" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'varyings', 'ptr': 'in', 'ptr_len': 'count|strlen(varyings[i])+1', 'loc': 2, 'ptr_ptr': True}, {'type': 'GLenum', 'name': 'bufferMode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTransformFeedbackVaryings(void *context, GLuint program, GLsizei count, const GLchar *const *varyings, GLenum bufferMode)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t *varyings_len = (size_t *)malloc(count * sizeof(size_t));
    for (int i = 0; i < count; i++)
    {
        varyings_len[i] = strlen(varyings[i]) + 1;
        save_buf_len += varyings_len[i];
    }

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLsizei *)ptr = count;
    ptr += 4;

    *(GLenum *)ptr = bufferMode;
    ptr += 4;

    for (int i = 0; i < count; i++)
    {
        memcpy(ptr, (unsigned char *)varyings[i], varyings_len[i]);
        ptr += varyings_len[i];
    }

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glTransformFeedbackVaryings;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteBuffers_origin GLsizei n, const GLuint *buffers#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteBuffers_origin" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'buffers', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint *buffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t buffers_len = n * sizeof(GLuint);
    save_buf_len += buffers_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteBuffers_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteFramebuffers GLsizei n, const GLuint *framebuffers#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteFramebuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'framebuffers', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t framebuffers_len = n * sizeof(GLuint);
    save_buf_len += framebuffers_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteFramebuffers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteRenderbuffers GLsizei n, const GLuint *renderbuffers#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteRenderbuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'renderbuffers', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t renderbuffers_len = n * sizeof(GLuint);
    save_buf_len += renderbuffers_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteRenderbuffers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteTextures GLsizei n, const GLuint *textures#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteTextures" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'textures', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteTextures(void *context, GLsizei n, const GLuint *textures)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t textures_len = n * sizeof(GLuint);
    save_buf_len += textures_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteTextures;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDrawElements_with_bound GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices" */
/* func name: "glDrawElements_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'indices', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDrawElements_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glShaderBinary GLsizei count, const GLuint *shaders#count*sizeof(GLuint), GLenum binaryFormat, const void *binary#length, GLsizei length" */
/* func name: "glShaderBinary" */
/* args: [{'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'shaders', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'binaryFormat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'binary', 'ptr': 'in', 'ptr_len': 'length', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glShaderBinary(void *context, GLsizei count, const GLuint *shaders, GLenum binaryFormat, const void *binary, GLsizei length)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t shaders_len = count * sizeof(GLuint);
    save_buf_len += shaders_len;

    size_t binary_len = length;
    save_buf_len += binary_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glShaderBinary;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexSubImage2D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data  @{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glCompressedTexSubImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 40;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCompressedTexSubImage2D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexImage2D_with_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 40;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexImage2D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameterfv GLenum target, GLenum pname, const GLfloat *params#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glTexParameterfv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexParameterfv(void *context, GLenum target, GLenum pname, const GLfloat *params)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfloat);
    save_buf_len += params_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexParameterfv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexParameteriv GLenum target, GLenum pname, const GLint *params#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glTexParameteriv" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexParameteriv(void *context, GLenum target, GLenum pname, const GLint *params)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLint);
    save_buf_len += params_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexSubImage2D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexSubImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 40;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexSubImage2D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*1" */
/* func name: "glUniform1fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform1fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 1;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform1fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*1" */
/* func name: "glUniform1iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform1iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 1;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform1iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*2" */
/* func name: "glUniform2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*2', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform2fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 2;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*2" */
/* func name: "glUniform2iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*2', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform2iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform2iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*3" */
/* func name: "glUniform3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*3', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform3fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 3;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*3" */
/* func name: "glUniform3iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*3', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform3iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform3iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4fv GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glUniform4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform4fv(void *context, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4iv GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*4" */
/* func name: "glUniform4iv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*4', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform4iv(void *context, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform4iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib1fv GLuint index, const GLfloat *v#sizeof(GLfloat)*1" */
/* func name: "glVertexAttrib1fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*1', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib1fv(void *context, GLuint index, const GLfloat *v)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 1;
    save_buf_len += v_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib1fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib2fv GLuint index, const GLfloat *v#sizeof(GLfloat)*2" */
/* func name: "glVertexAttrib2fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*2', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib2fv(void *context, GLuint index, const GLfloat *v)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 2;
    save_buf_len += v_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib3fv GLuint index, const GLfloat *v#sizeof(GLfloat)*3" */
/* func name: "glVertexAttrib3fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*3', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib3fv(void *context, GLuint index, const GLfloat *v)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 3;
    save_buf_len += v_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttrib4fv GLuint index, const GLfloat *v#sizeof(GLfloat)*4" */
/* func name: "glVertexAttrib4fv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttrib4fv(void *context, GLuint index, const GLfloat *v)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLfloat) * 4;
    save_buf_len += v_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttrib4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribPointer_with_bound GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer" */
/* func name: "glVertexAttribPointer_with_bound" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'normalized', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pointer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 28;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttribPointer_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribPointer_offset GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset" */
/* func name: "glVertexAttribPointer_offset" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'normalized', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index_father', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 32;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(GLuint *)ptr = index_father;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glVertexAttribPointer_offset;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDrawRangeElements_with_bound GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices" */
/* func name: "glDrawRangeElements_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'start', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'end', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'indices', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 28;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDrawRangeElements_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexImage3D_with_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 44;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexImage3D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexSubImage3D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels" */
/* func name: "glTexSubImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pixels', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 48;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexSubImage3D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexImage3D_with_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data @{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glCompressedTexImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 40;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCompressedTexImage3D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexSubImage3D_with_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data" */
/* func name: "glCompressedTexSubImage3D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 48;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCompressedTexSubImage3D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glCompressedTexImage2D_with_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data @{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glCompressedTexImage2D_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'data', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 36;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glCompressedTexImage2D_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteQueries GLsizei n, const GLuint *ids#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteQueries" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'ids', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteQueries(void *context, GLsizei n, const GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t ids_len = n * sizeof(GLuint);
    save_buf_len += ids_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteQueries;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDrawBuffers GLsizei n, const GLenum *bufs#n*sizeof(GLenum)" */
/* func name: "glDrawBuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'bufs', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLenum)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDrawBuffers(void *context, GLsizei n, const GLenum *bufs)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t bufs_len = n * sizeof(GLenum);
    save_buf_len += bufs_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDrawBuffers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix2fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glUniformMatrix2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix3fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*9" */
/* func name: "glUniformMatrix3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*9', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 9;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix4fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*16" */
/* func name: "glUniformMatrix4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*16', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 16;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix2x3fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glUniformMatrix2x3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix2x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix2x3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix3x2fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glUniformMatrix3x2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix3x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix3x2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix2x4fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glUniformMatrix2x4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix2x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix2x4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix4x2fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glUniformMatrix4x2fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix4x2fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix4x2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix3x4fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glUniformMatrix3x4fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix3x4fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix3x4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniformMatrix4x3fv GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glUniformMatrix4x3fv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniformMatrix4x3fv(void *context, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniformMatrix4x3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteVertexArrays_origin GLsizei n, const GLuint *arrays#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteVertexArrays_origin" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'arrays', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint *arrays)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t arrays_len = n * sizeof(GLuint);
    save_buf_len += arrays_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteVertexArrays_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribIPointer_with_bound GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer" */
/* func name: "glVertexAttribIPointer_with_bound" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'pointer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttribIPointer_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribIPointer_offset GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset" */
/* func name: "glVertexAttribIPointer_offset" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index_father', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 28;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(GLuint *)ptr = index_father;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glVertexAttribIPointer_offset;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4iv GLuint index, const GLint *v#sizeof(GLint)*4" */
/* func name: "glVertexAttribI4iv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLint)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribI4iv(void *context, GLuint index, const GLint *v)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLint) * 4;
    save_buf_len += v_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttribI4iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glVertexAttribI4uiv GLuint index, const GLuint *v#sizeof(GLuint)*4" */
/* func name: "glVertexAttribI4uiv" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'v', 'ptr': 'in', 'ptr_len': 'sizeof(GLuint)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glVertexAttribI4uiv(void *context, GLuint index, const GLuint *v)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t v_len = sizeof(GLuint) * 4;
    save_buf_len += v_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glVertexAttribI4uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform1uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*1" */
/* func name: "glUniform1uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform1uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 1;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform1uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform2uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*2" */
/* func name: "glUniform2uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*2', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform2uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform2uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform3uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*3" */
/* func name: "glUniform3uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*3', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform3uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform3uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glUniform4uiv GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*4" */
/* func name: "glUniform4uiv" */
/* args: [{'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*4', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glUniform4uiv(void *context, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glUniform4uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferiv GLenum buffer, GLint drawbuffer, const GLint *value#(buffer==GL_COLOR?4*sizeof(GLint):1*sizeof(GLint))" */
/* func name: "glClearBufferiv" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': '(buffer==GL_COLOR?4*sizeof(GLint):1*sizeof(GLint))', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearBufferiv(void *context, GLenum buffer, GLint drawbuffer, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = (buffer == GL_COLOR ? 4 * sizeof(GLint) : 1 * sizeof(GLint));
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClearBufferiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferuiv GLenum buffer, GLint drawbuffer, const GLuint *value#(buffer==GL_COLOR?4*sizeof(GLuint):1*sizeof(GLuint))" */
/* func name: "glClearBufferuiv" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': '(buffer==GL_COLOR?4*sizeof(GLuint):1*sizeof(GLuint))', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearBufferuiv(void *context, GLenum buffer, GLint drawbuffer, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = (buffer == GL_COLOR ? 4 * sizeof(GLuint) : 1 * sizeof(GLuint));
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClearBufferuiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClearBufferfv GLenum buffer, GLint drawbuffer, const GLfloat *value#(buffer==GL_COLOR?4*sizeof(GLfloat):1*sizeof(GLfloat))" */
/* func name: "glClearBufferfv" */
/* args: [{'type': 'GLenum', 'name': 'buffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'drawbuffer', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': '(buffer==GL_COLOR?4*sizeof(GLfloat):1*sizeof(GLfloat))', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClearBufferfv(void *context, GLenum buffer, GLint drawbuffer, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = (buffer == GL_COLOR ? 4 * sizeof(GLfloat) : 1 * sizeof(GLfloat));
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClearBufferfv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDrawElementsInstanced_with_bound GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount" */
/* func name: "glDrawElementsInstanced_with_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'indices', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'instancecount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDrawElementsInstanced_with_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteSamplers GLsizei count, const GLuint *samplers#count*sizeof(GLuint) @{if(count<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteSamplers" */
/* args: [{'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'samplers', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    {
        if (count < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t samplers_len = count * sizeof(GLuint);
    save_buf_len += samplers_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteSamplers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameteriv GLuint sampler, GLenum pname, const GLint *param#gl_pname_size(pname)*sizeof(GLint)" */
/* func name: "glSamplerParameteriv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLint)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glSamplerParameteriv(void *context, GLuint sampler, GLenum pname, const GLint *param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLint);
    save_buf_len += param_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glSamplerParameteriv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glSamplerParameterfv GLuint sampler, GLenum pname, const GLfloat *param#gl_pname_size(pname)*sizeof(GLfloat)" */
/* func name: "glSamplerParameterfv" */
/* args: [{'type': 'GLuint', 'name': 'sampler', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfloat)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glSamplerParameterfv(void *context, GLuint sampler, GLenum pname, const GLfloat *param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfloat);
    save_buf_len += param_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glSamplerParameterfv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteTransformFeedbacks GLsizei n, const GLuint *ids#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteTransformFeedbacks" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'ids', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t ids_len = n * sizeof(GLuint);
    save_buf_len += ids_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteTransformFeedbacks;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramBinary GLuint program, GLenum binaryFormat, const void *binary#length, GLsizei length" */
/* func name: "glProgramBinary" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'binaryFormat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'binary', 'ptr': 'in', 'ptr_len': 'length', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramBinary(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t binary_len = length;
    save_buf_len += binary_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramBinary;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glInvalidateFramebuffer GLenum target, GLsizei numAttachments, const GLenum *attachments#numAttachments*sizeof(GLenum)" */
/* func name: "glInvalidateFramebuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'numAttachments', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'attachments', 'ptr': 'in', 'ptr_len': 'numAttachments*sizeof(GLenum)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glInvalidateFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attachments_len = numAttachments * sizeof(GLenum);
    save_buf_len += attachments_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glInvalidateFramebuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glInvalidateSubFramebuffer GLenum target, GLsizei numAttachments, const GLenum *attachments#numAttachments*sizeof(GLenum), GLint x, GLint y, GLsizei width, GLsizei height" */
/* func name: "glInvalidateSubFramebuffer" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'numAttachments', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLenum*', 'name': 'attachments', 'ptr': 'in', 'ptr_len': 'numAttachments*sizeof(GLenum)', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'x', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'y', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glInvalidateSubFramebuffer(void *context, GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t attachments_len = numAttachments * sizeof(GLenum);
    save_buf_len += attachments_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glInvalidateSubFramebuffer;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClipPlanexOES GLenum plane, const GLfixed *equation#sizeof(GLfixed)*4" */
/* func name: "glClipPlanexOES" */
/* args: [{'type': 'GLenum', 'name': 'plane', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'equation', 'ptr': 'in', 'ptr_len': 'sizeof(GLfixed)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClipPlanexOES(void *context, GLenum plane, const GLfixed *equation)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t equation_len = sizeof(GLfixed) * 4;
    save_buf_len += equation_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClipPlanexOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glFogxvOES GLenum pname, const GLfixed *param#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glFogxvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glFogxvOES(void *context, GLenum pname, const GLfixed *param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += param_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glFogxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLightModelxvOES GLenum pname, const GLfixed *param#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glLightModelxvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLightModelxvOES(void *context, GLenum pname, const GLfixed *param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += param_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glLightModelxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLightxvOES GLenum light, GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glLightxvOES" */
/* args: [{'type': 'GLenum', 'name': 'light', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLightxvOES(void *context, GLenum light, GLenum pname, const GLfixed *params)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glLightxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glLoadMatrixxOES const GLfixed *m#16*sizeof(GLfixed)" */
/* func name: "glLoadMatrixxOES" */
/* args: [{'type': 'const GLfixed*', 'name': 'm', 'ptr': 'in', 'ptr_len': '16*sizeof(GLfixed)', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glLoadMatrixxOES(void *context, const GLfixed *m)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t m_len = 16 * sizeof(GLfixed);
    save_buf_len += m_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    memcpy(ptr, (unsigned char *)m, m_len);
    ptr += m_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glLoadMatrixxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glMaterialxvOES GLenum face, GLenum pname, const GLfixed *param#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glMaterialxvOES" */
/* args: [{'type': 'GLenum', 'name': 'face', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'param', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glMaterialxvOES(void *context, GLenum face, GLenum pname, const GLfixed *param)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t param_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += param_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glMaterialxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glMultMatrixxOES const GLfixed *m#16*sizeof(GLfixed)" */
/* func name: "glMultMatrixxOES" */
/* args: [{'type': 'const GLfixed*', 'name': 'm', 'ptr': 'in', 'ptr_len': '16*sizeof(GLfixed)', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glMultMatrixxOES(void *context, const GLfixed *m)
{

    unsigned char send_buf[16 + 16 * 0];
    size_t send_buf_len = 16 + 16 * 0;
    uint64_t save_buf_len = 0;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t m_len = 16 * sizeof(GLfixed);
    save_buf_len += m_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    memcpy(ptr, (unsigned char *)m, m_len);
    ptr += m_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glMultMatrixxOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 0;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPointParameterxvOES GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glPointParameterxvOES" */
/* args: [{'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glPointParameterxvOES(void *context, GLenum pname, const GLfixed *params)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glPointParameterxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexEnvxvOES GLenum target, GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glTexEnvxvOES" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexEnvxvOES(void *context, GLenum target, GLenum pname, const GLfixed *params)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexEnvxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glClipPlanefOES GLenum plane, const GLfloat *equation#sizeof(GLfloat)*4" */
/* func name: "glClipPlanefOES" */
/* args: [{'type': 'GLenum', 'name': 'plane', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'equation', 'ptr': 'in', 'ptr_len': 'sizeof(GLfloat)*4', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glClipPlanefOES(void *context, GLenum plane, const GLfloat *equation)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t equation_len = sizeof(GLfloat) * 4;
    save_buf_len += equation_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glClipPlanefOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glTexGenxvOES GLenum coord, GLenum pname, const GLfixed *params#gl_pname_size(pname)*sizeof(GLfixed)" */
/* func name: "glTexGenxvOES" */
/* args: [{'type': 'GLenum', 'name': 'coord', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'pname', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLfixed*', 'name': 'params', 'ptr': 'in', 'ptr_len': 'gl_pname_size(pname)*sizeof(GLfixed)', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTexGenxvOES(void *context, GLenum coord, GLenum pname, const GLfixed *params)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t params_len = gl_pname_size(pname) * sizeof(GLfixed);
    save_buf_len += params_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glTexGenxvOES;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteProgramPipelines GLsizei n, const GLuint *pipelines#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glDeleteProgramPipelines" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'pipelines', 'ptr': 'in', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t pipelines_len = n * sizeof(GLuint);
    save_buf_len += pipelines_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glDeleteProgramPipelines;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)" */
/* func name: "glProgramUniform1iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform1iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint);
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform1iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*2" */
/* func name: "glProgramUniform2iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*2', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform2iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform2iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*3" */
/* func name: "glProgramUniform3iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*3', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform3iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform3iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4iv GLuint program, GLint location, GLsizei count, const GLint *value#count*sizeof(GLint)*4" */
/* func name: "glProgramUniform4iv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLint)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform4iv(void *context, GLuint program, GLint location, GLsizei count, const GLint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform4iv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)" */
/* func name: "glProgramUniform1uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform1uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint);
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform1uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*2" */
/* func name: "glProgramUniform2uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*2', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform2uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 2;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform2uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*3" */
/* func name: "glProgramUniform3uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*3', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform3uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 3;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform3uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4uiv GLuint program, GLint location, GLsizei count, const GLuint *value#count*sizeof(GLuint)*4" */
/* func name: "glProgramUniform4uiv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLuint*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLuint)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform4uiv(void *context, GLuint program, GLint location, GLsizei count, const GLuint *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLuint) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform4uiv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform1fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)" */
/* func name: "glProgramUniform1fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform1fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat);
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform1fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform2fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*2" */
/* func name: "glProgramUniform2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*2', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform2fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 2;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform3fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*3" */
/* func name: "glProgramUniform3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*3', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform3fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 3;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniform4fv GLuint program, GLint location, GLsizei count, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glProgramUniform4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniform4fv(void *context, GLuint program, GLint location, GLsizei count, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 12;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniform4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix2fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*4" */
/* func name: "glProgramUniformMatrix2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*4', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 4;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix3fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*9" */
/* func name: "glProgramUniformMatrix3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*9', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 9;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix4fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*16" */
/* func name: "glProgramUniformMatrix4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*16', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 16;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix2x3fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glProgramUniformMatrix2x3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix2x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix2x3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix3x2fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*6" */
/* func name: "glProgramUniformMatrix3x2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*6', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix3x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 6;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix3x2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix2x4fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glProgramUniformMatrix2x4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix2x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix2x4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix4x2fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*8" */
/* func name: "glProgramUniformMatrix4x2fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*8', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix4x2fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 8;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix4x2fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix3x4fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glProgramUniformMatrix3x4fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix3x4fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix3x4fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glProgramUniformMatrix4x3fv GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value#count*sizeof(GLfloat)*12" */
/* func name: "glProgramUniformMatrix4x3fv" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'location', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'transpose', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'const GLfloat*', 'name': 'value', 'ptr': 'in', 'ptr_len': 'count*sizeof(GLfloat)*12', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glProgramUniformMatrix4x3fv(void *context, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t value_len = count * sizeof(GLfloat) * 12;
    save_buf_len += value_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glProgramUniformMatrix4x3fv;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glDeleteSync GLsync sync" */
/* func name: "glDeleteSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glDeleteSync(void *context, GLsync sync)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLsync *)ptr = sync;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glDeleteSync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glWaitSync GLsync sync, GLbitfield flags, GLuint64 timeout" */
/* func name: "glWaitSync" */
/* args: [{'type': 'GLsync', 'name': 'sync', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'flags', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint64', 'name': 'timeout', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glWaitSync(void *context, GLsync sync, GLbitfield flags, GLuint64 timeout)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(uint64_t *)ptr = FUNID_glWaitSync;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glBindAttribLocation GLuint program, GLuint index, const GLchar *name#strlen(name)+1" */
/* func name: "glBindAttribLocation" */
/* args: [{'type': 'GLuint', 'name': 'program', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'name', 'ptr': 'in', 'ptr_len': 'strlen(name)+1', 'loc': 2, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glBindAttribLocation(void *context, GLuint program, GLuint index, const GLchar *name)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 8;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t name_len = strlen(name) + 1;
    save_buf_len += name_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLuint *)ptr = program;
    ptr += 4;

    *(GLuint *)ptr = index;
    ptr += 4;

    memcpy(ptr, (unsigned char *)name, name_len);
    ptr += name_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glBindAttribLocation;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glMapBufferRange_write GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access" */
/* func name: "glMapBufferRange_write" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLbitfield', 'name': 'access', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 24;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
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

    *(GLbitfield *)ptr = access;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glMapBufferRange_write;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "GLboolean glUnmapBuffer_special GLenum target" */
/* func name: "glUnmapBuffer_special" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLboolean" */
/* type: "3" */

GLboolean d_glUnmapBuffer_special(void *context, GLenum target)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 4;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glUnmapBuffer_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }

    return GL_TRUE;
}

/* readline: "glTestIntAsyn GLint a, GLuint b, GLfloat c, GLdouble d" */
/* func name: "glTestIntAsyn" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'b', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLfloat', 'name': 'c', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLdouble', 'name': 'd', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glTestIntAsyn(void *context, GLint a, GLuint b, GLfloat c, GLdouble d)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 20;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    *(GLuint *)ptr = b;
    ptr += 4;

    *(GLfloat *)ptr = c;
    ptr += 4;

    *(GLdouble *)ptr = d;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glTestIntAsyn;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/* readline: "glPrintfAsyn GLint a, GLuint size, GLdouble c, const GLchar *out_string#strlen(out_string)+1" */
/* func name: "glPrintfAsyn" */
/* args: [{'type': 'GLint', 'name': 'a', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLdouble', 'name': 'c', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'out_string', 'ptr': 'in', 'ptr_len': 'strlen(out_string)+1', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "3" */

void d_glPrintfAsyn(void *context, GLint a, GLuint size, GLdouble c, const GLchar *out_string)
{

    unsigned char send_buf[16 + 16 * 1];
    size_t send_buf_len = 16 + 16 * 1;
    uint64_t save_buf_len = 16;
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;

    size_t out_string_len = strlen(out_string) + 1;
    save_buf_len += out_string_len;

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        save_buf = (unsigned char *)malloc(save_buf_len);
    }
    else
    {
        save_buf = local_save_buf;
    }

    unsigned char *ptr = save_buf;

    *(GLint *)ptr = a;
    ptr += 4;

    *(GLuint *)ptr = size;
    ptr += 4;

    *(GLdouble *)ptr = c;
    ptr += 8;

    memcpy(ptr, (unsigned char *)out_string, out_string_len);
    ptr += out_string_len;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glPrintfAsyn;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = 1;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    if (save_buf_len > MAX_OUT_BUF_LEN)
    {
        free(save_buf);
    }
}

/******* end of file '2-1-1', 247/342 functions *******/

/******* file '2-1-2' *******/

/* readline: "glDrawElements_without_bound GLenum mode, GLsizei count, GLenum type, const void *indices#count*gl_sizeof(type)" */
/* func name: "glDrawElements_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'indices', 'ptr': 'in', 'ptr_len': 'count*gl_sizeof(type)', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 12;
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

    *(uint64_t *)ptr = FUNID_glDrawElements_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)count * gl_sizeof(type);
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)indices;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glVertexAttribIPointer_without_bound GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLsizei length, const void *pointer#length" */
/* func name: "glVertexAttribIPointer_without_bound" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pointer', 'ptr': 'in', 'ptr_len': 'length', 'loc': 6, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLsizei length, const void *pointer)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 24;
    unsigned char *save_buf;

    unsigned char local_save_buf[24];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLuint *)ptr = index;
    ptr += 4;

    *(GLint *)ptr = size;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLsizei *)ptr = stride;
    ptr += 4;

    *(GLuint *)ptr = offset;
    ptr += 4;

    *(GLsizei *)ptr = length;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glVertexAttribIPointer_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)length;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)pointer;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glVertexAttribPointer_without_bound GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer#length" */
/* func name: "glVertexAttribPointer_without_bound" */
/* args: [{'type': 'GLuint', 'name': 'index', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLboolean', 'name': 'normalized', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'stride', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pointer', 'ptr': 'in', 'ptr_len': 'length', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 28;
    unsigned char *save_buf;

    unsigned char local_save_buf[28];
    save_buf = local_save_buf;
    ptr = save_buf;

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

    *(GLuint *)ptr = offset;
    ptr += 4;

    *(GLuint *)ptr = length;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glVertexAttribPointer_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)length;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)pointer;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glDrawElementsInstanced_without_bound GLenum mode, GLsizei count, GLenum type, const void *indices#count*gl_sizeof(type), GLsizei instancecount" */
/* func name: "glDrawElementsInstanced_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'indices', 'ptr': 'in', 'ptr_len': 'count*gl_sizeof(type)', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'instancecount', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
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

    *(uint64_t *)ptr = FUNID_glDrawElementsInstanced_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)count * gl_sizeof(type);
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)indices;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glBufferData_custom GLenum target, GLsizeiptr size, const void *data#size, GLenum usage" */
/* func name: "glBufferData_custom" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'size', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'usage', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 16;
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

    *(uint64_t *)ptr = FUNID_glBufferData_custom;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)size;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)data;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glBufferSubData_custom GLenum target, GLintptr offset, GLsizeiptr size, const void *data#size" */
/* func name: "glBufferSubData_custom" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'size', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'size', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 20;
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

    *(uint64_t *)ptr = FUNID_glBufferSubData_custom;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)size;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)data;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexImage2D_without_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data#imageSize  @{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glCompressedTexImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 7, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 28;
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

    *(uint64_t *)ptr = FUNID_glCompressedTexImage2D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)imageSize;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)data;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexSubImage2D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data#imageSize @{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glCompressedTexSubImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 32;
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

    *(uint64_t *)ptr = FUNID_glCompressedTexSubImage2D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)imageSize;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)data;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexImage2D_without_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels#buf_len" */
/* func name: "glTexImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'buf_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'buf_len', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 36;
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

    *(GLint *)ptr = border;
    ptr += 4;

    *(GLenum *)ptr = format;
    ptr += 4;

    *(GLenum *)ptr = type;
    ptr += 4;

    *(GLint *)ptr = buf_len;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glTexImage2D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)pixels;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glDrawRangeElements_without_bound GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices#count*gl_sizeof(type)" */
/* func name: "glDrawRangeElements_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'mode', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'start', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLuint', 'name': 'end', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'indices', 'ptr': 'in', 'ptr_len': 'count*gl_sizeof(type)', 'loc': 5, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 20;
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

    *(uint64_t *)ptr = FUNID_glDrawRangeElements_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)count * gl_sizeof(type);
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)indices;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexImage3D_without_bound GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data#imageSize @{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glCompressedTexImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 8, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 32;
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

    *(uint64_t *)ptr = FUNID_glCompressedTexImage3D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)imageSize;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)data;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glCompressedTexSubImage3D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data#imageSize @{if(imageSize<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glCompressedTexSubImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'imageSize', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'imageSize', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
    {
        if (imageSize < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 40;
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

    *(uint64_t *)ptr = FUNID_glCompressedTexSubImage3D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)imageSize;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)data;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexSubImage2D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels#buf_len" */
/* func name: "glTexSubImage2D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'buf_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'buf_len', 'loc': 9, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 36;
    unsigned char *save_buf;

    unsigned char local_save_buf[36];
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

    *(GLint *)ptr = buf_len;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glTexSubImage2D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)pixels;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexImage3D_without_bound GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels#buf_len" */
/* func name: "glTexImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'internalformat', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'border', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'buf_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'buf_len', 'loc': 10, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 40;
    unsigned char *save_buf;

    unsigned char local_save_buf[40];
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

    *(GLint *)ptr = buf_len;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glTexImage3D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)pixels;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glTexSubImage3D_without_bound GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels#buf_len" */
/* func name: "glTexSubImage3D_without_bound" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'level', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'xoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'yoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 3, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'zoffset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 4, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'width', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 5, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'height', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 6, 'ptr_ptr': False}, {'type': 'GLsizei', 'name': 'depth', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 7, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'format', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 8, 'ptr_ptr': False}, {'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 9, 'ptr_ptr': False}, {'type': 'GLint', 'name': 'buf_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 10, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'pixels', 'ptr': 'in', 'ptr_len': 'buf_len', 'loc': 11, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 44;
    unsigned char *save_buf;

    unsigned char local_save_buf[44];
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

    *(GLint *)ptr = buf_len;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glTexSubImage3D_without_bound;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)pixels;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glFlushMappedBufferRange_special GLenum target, GLintptr offset, GLsizeiptr length, const void *data#length" */
/* func name: "glFlushMappedBufferRange_special" */
/* args: [{'type': 'GLenum', 'name': 'target', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLintptr', 'name': 'offset', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 1, 'ptr_ptr': False}, {'type': 'GLsizeiptr', 'name': 'length', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 2, 'ptr_ptr': False}, {'type': 'const void*', 'name': 'data', 'ptr': 'in', 'ptr_len': 'length', 'loc': 3, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 20;
    unsigned char *save_buf;

    unsigned char local_save_buf[20];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = target;
    ptr += 4;

    *(GLintptr *)ptr = offset;
    ptr += 8;

    *(GLsizeiptr *)ptr = length;
    ptr += 8;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glFlushMappedBufferRange_special;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)length;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)data;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/* readline: "glPrintf GLint buf_len, const GLchar *out_string#buf_len" */
/* func name: "glPrintf" */
/* args: [{'type': 'GLint', 'name': 'buf_len', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'const GLchar*', 'name': 'out_string', 'ptr': 'in', 'ptr_len': 'buf_len', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "4" */

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string)
{

    unsigned char send_buf[16 + (2) * 16];
    size_t send_buf_len = 16 + (2) * 16;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLint *)ptr = buf_len;
    ptr += 4;

    ptr = send_buf;

    *(uint64_t *)ptr = FUNID_glPrintf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)(1 + 1);
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_string;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);
}

/******* end of file '2-1-2', 18/359 functions *******/

/******* file '2-2' *******/

/* readline: "glGenBuffers GLsizei n, GLuint *buffers#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenBuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'buffers', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenBuffers_origin(void *context, GLsizei n, GLuint *buffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenBuffers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t buffers_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += buffers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (buffers != NULL)
    {

        memcpy(buffers, (GLuint *)out_buf_ptr, buffers_len);
    }
    out_buf_ptr += buffers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenFramebuffers GLsizei n, GLuint *framebuffers#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenFramebuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'framebuffers', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenFramebuffers_origin(void *context, GLsizei n, GLuint *framebuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenFramebuffers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t framebuffers_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += framebuffers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (framebuffers != NULL)
    {

        memcpy(framebuffers, (GLuint *)out_buf_ptr, framebuffers_len);
    }
    out_buf_ptr += framebuffers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenRenderbuffers GLsizei n, GLuint *renderbuffers#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenRenderbuffers" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'renderbuffers', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenRenderbuffers_origin(void *context, GLsizei n, GLuint *renderbuffers)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenRenderbuffers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t renderbuffers_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += renderbuffers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (renderbuffers != NULL)
    {

        memcpy(renderbuffers, (GLuint *)out_buf_ptr, renderbuffers_len);
    }
    out_buf_ptr += renderbuffers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenTextures GLsizei n, GLuint *textures#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenTextures" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'textures', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenTextures_origin(void *context, GLsizei n, GLuint *textures)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenTextures;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t textures_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += textures_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (textures != NULL)
    {

        memcpy(textures, (GLuint *)out_buf_ptr, textures_len);
    }
    out_buf_ptr += textures_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenQueries GLsizei n, GLuint *ids#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenQueries" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'ids', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenQueries_origin(void *context, GLsizei n, GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenQueries;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t ids_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += ids_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (ids != NULL)
    {

        memcpy(ids, (GLuint *)out_buf_ptr, ids_len);
    }
    out_buf_ptr += ids_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenVertexArrays_origin GLsizei n, GLuint *arrays#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenVertexArrays_origin" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'arrays', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenVertexArrays_origin(void *context, GLsizei n, GLuint *arrays)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenVertexArrays_origin;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t arrays_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += arrays_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (arrays != NULL)
    {

        memcpy(arrays, (GLuint *)out_buf_ptr, arrays_len);
    }
    out_buf_ptr += arrays_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenSamplers GLsizei count, GLuint *samplers#count*sizeof(GLuint) @{if(count<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenSamplers" */
/* args: [{'type': 'GLsizei', 'name': 'count', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'samplers', 'ptr': 'out', 'ptr_len': 'count*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenSamplers_origin(void *context, GLsizei count, GLuint *samplers)
{
    {
        if (count < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = count;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenSamplers;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t samplers_len = (uint64_t)count * sizeof(GLuint);
    out_buf_len += samplers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (samplers != NULL)
    {

        memcpy(samplers, (GLuint *)out_buf_ptr, samplers_len);
    }
    out_buf_ptr += samplers_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenTransformFeedbacks GLsizei n, GLuint *ids#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenTransformFeedbacks" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'ids', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenTransformFeedbacks_origin(void *context, GLsizei n, GLuint *ids)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenTransformFeedbacks;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t ids_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += ids_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (ids != NULL)
    {

        memcpy(ids, (GLuint *)out_buf_ptr, ids_len);
    }
    out_buf_ptr += ids_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "glGenProgramPipelines GLsizei n, GLuint *pipelines#n*sizeof(GLuint) @{if(n<0){ set_gl_error(context,GL_INVALID_VALUE); return; }}" */
/* func name: "glGenProgramPipelines" */
/* args: [{'type': 'GLsizei', 'name': 'n', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}, {'type': 'GLuint*', 'name': 'pipelines', 'ptr': 'out', 'ptr_len': 'n*sizeof(GLuint)', 'loc': 1, 'ptr_ptr': False}] */
/* ret: "" */
/* type: "5" */

void d_glGenProgramPipelines_origin(void *context, GLsizei n, GLuint *pipelines)
{
    {
        if (n < 0)
        {
            set_gl_error(context, GL_INVALID_VALUE);
            return;
        }
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLsizei *)ptr = n;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glGenProgramPipelines;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    uint64_t pipelines_len = (uint64_t)n * sizeof(GLuint);
    out_buf_len += pipelines_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    if (pipelines != NULL)
    {

        memcpy(pipelines, (GLuint *)out_buf_ptr, pipelines_len);
    }
    out_buf_ptr += pipelines_len;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }
}

/* readline: "GLuint glCreateProgram void" */
/* func name: "glCreateProgram" */
/* args: [] */
/* ret: "GLuint" */
/* type: "5" */

GLuint d_glCreateProgram_origin(void *context)
{

    unsigned char send_buf[16 + 1 * 16];
    size_t send_buf_len = 16 + 1 * 16;
    size_t para_num = 1;
    unsigned char *ptr = NULL;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glCreateProgram;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLuint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLuint ret = *(GLuint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/* readline: "GLuint glCreateShader GLenum type @if(type!=GL_COMPUTE_SHADER&&type!=GL_VERTEX_SHADER&&type!=GL_FRAGMENT_SHADER){set_gl_error(context,GL_INVALID_ENUM);return 0;}" */
/* func name: "glCreateShader" */
/* args: [{'type': 'GLenum', 'name': 'type', 'ptr': 'NA', 'ptr_len': 'NA', 'loc': 0, 'ptr_ptr': False}] */
/* ret: "GLuint" */
/* type: "5" */

GLuint d_glCreateShader_origin(void *context, GLenum type)
{
    if (type != GL_COMPUTE_SHADER && type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER)
    {
        set_gl_error(context, GL_INVALID_ENUM);
        return 0;
    }

    unsigned char send_buf[16 + 2 * 16];
    size_t send_buf_len = 16 + 2 * 16;
    size_t para_num = 2;
    unsigned char *ptr = NULL;

    uint64_t save_buf_len = 4;
    unsigned char *save_buf;

    unsigned char local_save_buf[4];
    save_buf = local_save_buf;
    ptr = save_buf;

    *(GLenum *)ptr = type;
    ptr += 4;

    ptr = send_buf;
    *(uint64_t *)ptr = FUNID_glCreateShader;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)para_num;
    ptr += sizeof(uint64_t);

    *(uint64_t *)ptr = (uint64_t)save_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)save_buf;
    ptr += sizeof(uint64_t);

    unsigned char *out_buf;
    unsigned char *out_buf_ptr;
    uint64_t out_buf_len = 0;
    unsigned char out_buf_local[MAX_OUT_BUF_LEN];

    out_buf_len += sizeof(GLuint);

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        out_buf = (unsigned char *)malloc(out_buf_len);
    }
    else
    {
        out_buf = out_buf_local;
    }

    *(uint64_t *)ptr = (uint64_t)out_buf_len;
    ptr += sizeof(uint64_t);
    *(uint64_t *)ptr = (uint64_t)out_buf;
    ptr += sizeof(uint64_t);

    send_to_host(context, send_buf, send_buf_len);

    out_buf_ptr = out_buf;
    GLuint ret = *(GLuint *)out_buf_ptr;

    if (out_buf_len > MAX_OUT_BUF_LEN)
    {
        {
            free(out_buf);
        }
    }

    return ret;
}

/******* end of file '2-2', 12/370 functions *******/
