#ifndef GLV3_VERTEX_H
#define GLV3_VERTEX_H

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"

#include "express-gpu/express_gpu_opengl.h"

void safe_release_point_data_data(Attrib_Point *point_data,int index);

// void set_attrib_point_index(void *context,Scatter_Data *data,int len);
void set_attrib_point_index(void *context,void *data,int len);


void set_attrib_point(void *context,GLsizei instancecount);


void flush_array_buffer(Attrib_Point *point_data, GLint instancecount);



void d_glVertexAttribIPointer_data(void *context,const void *int_data, const void *pointer);
void d_glVertexAttribPointer_data(void *context,const void *int_data, const void *pointer);

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer);
void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer);


void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer);
void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer);

void d_glGenVertexArrays_origin(void *context, GLsizei n, GLuint *arrays);
void d_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint *arrays);
void d_glBindVertexArray_origin(void *context, GLuint array);

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor);
void d_glGetVertexAttribIiv_origin(void *context, GLuint index, GLenum pname, GLint *params);
void d_glGetVertexAttribIuiv_origin(void *context, GLuint index, GLenum pname, GLuint *params);
void d_glGetVertexAttribfv_origin(void *context, GLuint index, GLenum pname, GLfloat *params);
void d_glGetVertexAttribiv_origin(void *context, GLuint index, GLenum pname, GLint *params);

void d_glDisableVertexAttribArray_origin(void *context, GLuint index);
void d_glEnableVertexAttribArray_origin(void *context, GLuint index);


void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count);
void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices);

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount);

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices);
void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);




#endif