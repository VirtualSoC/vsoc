#ifndef EXPRESS_GPU_OPENGL_H
#define EXPRESS_GPU_OPENGL_H


#include "direct-express/express_device_common.h"
#include "direct-express/express_log.h"
#include "express-gpu/express_gpu_render.h"


#define MAX_VERTEX_ATTRIBS_NUM 16

typedef struct Pixel_Store_Status
{
    //表示数据对齐的标准
    int unpack_alignment;
    int pack_alignment;

    //表示对于每个图片，每一行有多少个像素点，为0的时候像素点的个数就是传入的width
    int unpack_row_length;
    int pack_row_length;

    //表示对于这个图片的读取要跳过多少行像素
    int unpack_skip_rows;
    int pack_skip_rows;

    //表示对于这个图片的这一行的读取要跳过多少个像素
    int unpack_skip_pixels;
    int pack_skip_pixels;

    //表示对于一堆图片的读取要跳过多少张图片
    int unpack_skip_images;

    //表示每张图片的高度
    int unpack_image_height;
} Pixel_Store_Status;

typedef struct Buffer_Status
{
    GLint array_buffer;         //vbo
    GLint element_array_buffer; //ebo
    GLint copy_read_buffer;
    GLint copy_write_buffer;
    GLint pixel_pack_buffer;
    GLint pixel_unpack_buffer;
    GLint transform_feedback_buffer;
    GLint uniform_buffer;
    GLint atomic_counter_buffer;
    GLint dispatch_indirect_buffer;
    GLint draw_indirect_buffer;
    GLint shader_storage_buffer;
    GLint vertex_array_buffer; //vao

} Buffer_Status;


typedef struct Attrib_Point
{
    
    
    //father指示其的数据在哪，然后offset指示了在数组中的偏移
    // GLint father[MAX_VERTEX_ATTRIBS_NUM];
    // GLintptr offset[MAX_VERTEX_ATTRIBS_NUM];


    //顶点的常规属性
    // GLint size[MAX_VERTEX_ATTRIBS_NUM];
    // GLenum type[MAX_VERTEX_ATTRIBS_NUM];
    // GLsizei stride[MAX_VERTEX_ATTRIBS_NUM];
    // GLboolean normalized[MAX_VERTEX_ATTRIBS_NUM];
    // GLuint divisor[MAX_VERTEX_ATTRIBS_NUM];
    // GLenum invoke_type[MAX_VERTEX_ATTRIBS_NUM];
    // GLuint min_index;
    // GLuint max_index;

    GLuint buffer_object[MAX_VERTEX_ATTRIBS_NUM];
    GLint buffer_loc[MAX_VERTEX_ATTRIBS_NUM];

    GLint remain_buffer_len[MAX_VERTEX_ATTRIBS_NUM];
    GLint buffer_len[MAX_VERTEX_ATTRIBS_NUM];

    GLuint indices_buffer_object;
    GLint indices_buffer_len;

    GLint remain_indices_buffer_len;

    // GLint buffer_num;

    // GLboolean in_buffer[MAX_VERTEX_ATTRIBS_NUM];


    //表示顶点属性是否启用顶点数组
    // GLboolean enabled[MAX_VERTEX_ATTRIBS_NUM];

} Attrib_Point;



typedef struct Bound_Buffer
{

    //这个buffer_status指针是指向实际vao_status里的值
    Buffer_Status *buffer_status;
    Attrib_Point *attrib_point;

    // std::map<GLint, GLenum> buffer_type;
    GHashTable *buffer_type;

    //std::map<GLint, Buffer_Status *> vao_status;
    GHashTable *vao_status;

    //std::map<GLint, Attrib_Point *> vao_point_data;
    GHashTable *vao_point_data;

    GLint asyn_unpack_texture_buffer;
    GLint asyn_pack_texture_buffer;


    //std::map<GLint, Element_Array_Buffer *> ebo_buffer;

} Bound_Buffer;

typedef struct Opengl_Context
{
    Pixel_Store_Status pixel_store_status;
    Bound_Buffer bound_buffer_status;

    GHashTable *buffer_map;

    int has_init;

} Opengl_Context;




typedef struct Guest_Host_Map{
    GLubyte *host_data;
    unsigned long map_len;
    GLenum target;
    GLbitfield access;

}Guest_Host_Map;




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


void prepare_unpack_texture(void *context,Scatter_Data *s_data,int start_loc,int end_loc);

void gl_pixel_data_loc(void *store_status, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack, int *start_loc,int *end_loc);

void gl_pixel_data_3d_loc(void *store_status, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack, int *start_loc,int *end_loc);


void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer);


void d_glDeleteProgram_origin(void *context, GLuint program);

void d_glLinkProgram_origin(void *context, GLuint program);

void d_glShaderSource_origin(void *context,GLuint shader, GLsizei count, const GLint *length, const GLchar *const*string);



void opengl_context_create(void *context);



void opengl_context_destroy(void *context);












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

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string);


void d_glInOutTest(void *context, GLint a, GLint b, const GLchar *e, GLint *c, GLdouble *d, GLsizei buf_len, GLchar *f);




#endif