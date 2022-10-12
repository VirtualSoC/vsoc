#ifndef GLV3_PROGRAM_H
#define GLV3_PROGRAM_H

#include "direct-express/express_device_common.h"
#include "direct-express/express_log.h"
#include "express-gpu/express_gpu_render.h"


extern GHashTable *program_is_external_map;
// extern GHashTable *to_external_texture_id_map;
extern GHashTable *program_data_map;



void get_program_data(GLuint program, int buf_len, GLchar *program_data);

int init_program_data(GLuint program);

// void prepare_unpack_texture(void *context,Scatter_Data *s_data,int start_loc,int end_loc);

// void gl_pixel_data_loc(void *store_status, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack, int *start_loc,int *end_loc);

// void gl_pixel_data_3d_loc(void *store_status, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack, int *start_loc,int *end_loc);


void d_glLinkProgram_special(void *context, GLuint program, int *program_data_len);

void d_glProgramBinary_special(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length, int *program_data_len);

void d_glGetProgramData(void *context, GLuint program, int buf_len, void *program_data);

void d_glShaderSource_special(void *context, GLuint shader, GLsizei count, GLint *length, const GLchar **string);



void d_glUseProgram_special(void *context, GLuint program);

void change_GLSL_version(char *start, char *end, int try_cnt);

void d_glCompileShader_special(void *context, GLuint id);

#endif