#include "glad.h"


void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer);

void d_glDeleteProgram_origin(void *context, GLuint program);

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count);

void d_glLinkProgram_origin(void *context, GLuint program);
void d_glPixelStorei_origin(void *context, GLenum pname, GLint param);


void d_glBindVertexArray_origin(void *context, GLuint array);

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor);


void d_glGetVertexAttribPointerv_with_bound(void *context,GLuint index, GLenum pname, GLintptr pointer);

void d_glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params);
void d_glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params);
void d_glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params);
void d_glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params);


void d_glDisableVertexAttribArray_origin(void *context,GLuint index);
void d_glEnableVertexAttribArray_origin(void *context,GLuint index);


void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void* data, GLenum usage);
void glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void* data);

void d_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint* buffers);

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices);

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels);

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer);
void d_glVertexAttribPointer_data(void *context, GLuint index, GLuint length, const void* pointer);
void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint length, GLenum type, GLboolean normalized, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer);
void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);
void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels);
void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels);

void d_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint* arrays);
void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer);
void d_glVertexAttribIPointer_data(void *context, GLuint index, GLuint size, const void* pointer);
void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor,  GLboolean enabled, GLintptr pointer);

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount);

void glGetVertexAttribPointerv_with_bound(GLuint index, GLenum pname, GLintptr *pointer);

void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels);

void d_glGenVertexArrays_origin(void *context, GLsizei n, GLuint* arrays);

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void* mem_buf);
void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, const void* mem_buf);
GLboolean d_glUnmapBuffer_special(void *context, GLenum target);
void d_glFlushMappedBufferRange_origin(void *context, GLenum target, GLintptr offset, GLsizeiptr length);



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

GLint glTestPointer3(GLint a, const GLint *b, GLint *c);

GLint glTestPointer4(GLint a, const GLint *b, GLint *c);


void glTestString(GLint a, GLint count, const GLchar *const*strings, GLint buf_len, GLchar *char_buf);

void glPrintf(GLint buf_len, GLchar *out_string);