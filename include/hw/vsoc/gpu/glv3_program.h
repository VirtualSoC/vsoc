#ifndef GLV3_PROGRAM_H
#define GLV3_PROGRAM_H

#include "hw/vsoc/express_log.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"

extern GHashTable *program_is_external_map;
extern GMutex program_is_external_map_mutex;
extern GHashTable *program_data_map;
extern GMutex program_data_map_mutex;

void get_program_data(GLuint program, int buf_len, GLchar *program_data);

int init_program_data(GLuint program);

void d_glLinkProgram_special(void *context, GLuint program, int *program_data_len);

void d_glProgramBinary_special(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length, int *program_data_len);

void d_glGetProgramData(void *context, GLuint program, int buf_len, void *program_data);

void d_glShaderSource_special(void *context, GLuint shader, GLsizei count, GLint *length, GLchar **string);

void d_glUseProgram_special(void *context, GLuint program);

void change_GLSL_version(char *start, char *end, int try_cnt);

void d_glCompileShader_special(void *context, GLuint id);

#endif