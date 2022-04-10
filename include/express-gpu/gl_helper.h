#ifndef GL_HELPER_H
#define GL_HELPER_H

#include "direct-express/express_device_common.h"
#include "direct-express/express_log.h"
#include "express-gpu/express_gpu_render.h"


/**
 * @brief 根据像素格式和类型计算一个像素所占的空间的字节大小
 * 
 * @param format 像素格式
 * @param type 像素类型
 * @return int 
 */
int pixel_size_calc(GLenum format, GLenum type);

/**
 * @brief opengl各种类型数据的sizeof函数
 * 
 * @param type 
 * @return size_t 
 */
size_t gl_sizeof(GLenum type);

size_t gl_pname_size(GLenum pname);


void glTestIntAsyn(GLint a, GLuint b, GLfloat c, GLdouble d);

void glPrintfAsyn(GLint a, GLuint size, GLdouble c, const GLchar *out_string);

GLint glTestInt1(GLint a, GLuint b);
GLuint glTestInt2(GLint a, GLuint b);

GLint64 glTestInt3(GLint64 a, GLuint64 b);
GLuint64 glTestInt4(GLint64 a, GLuint64 b);

GLfloat glTestInt5(GLint a, GLuint b);
GLdouble glTestInt6(GLint a, GLuint b);

void glTestPointer1(GLint a, const GLint *b);

void glTestPointer2(GLint a, const GLint *b, GLint *c);

GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c);

GLint glTestPointer4(GLint a, const GLint *b, GLint *c);

void glTestString(GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf);

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string);

void d_glInOutTest(void *context, GLint a, GLint b, const GLchar *e, GLint *c, GLdouble *d, GLsizei buf_len, GLchar *f);

#endif