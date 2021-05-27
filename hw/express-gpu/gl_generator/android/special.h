

#include <map>
#include <string>
#include <algorithm>
extern "C"
{

#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "utils.h"
}

// #define MAX_ASYN_BUF_SIZE 2500



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



typedef struct Attrib_Point
{
    //因为opengles3.0至少是16个，这个数量由硬件决定，用32个应该够用了（实际上google模拟器也只是用了16个）
    // GLuint index[32];
    GLboolean has_update;
    //用来表示当前顶点数组对应的内存区域有没有被get_page，假如有，那么在切换到新的内存区域时要put_page
    GLboolean get_page[32];
    GLboolean location[32];
    GLboolean normalized[32];
    GLint size[32];
    GLenum type[32];
    GLsizei stride[32];

    GLboolean enabled[32];
    GLboolean is_offset[32];

    //当前顶点数组的最大长度
    GLsizei max_len[32];
    GLuint divisor[32];
    GLenum invoke_type[32];
    //注意由于指针指向的数据会发生变化，因此每次draw都需要复制数据过去，因此也需要想办法减少复制的代价，在数据为结构数组的情况下进行优化
    const void *data[32];

    //用于保存上面的data对应的index和length数据，这些数据也需要进行长时保存，之后put_page掉
    unsigned char data_index_length[32][8];

    //用于聚类计算各个指针之间距离的数组
    uint64_t data_dis[32][32];

    //用于指示这个属性指针到底跟哪个属性是同一个数组
    GLint data_real_index[32];

} Attrib_Point;


typedef struct Element_Array_Buffer
{

    char *data;
    size_t size;
    GLenum type;
    //用st算法解决rmq问题
    GLboolean has_update;

    GLuint **dp_max;
    GLuint **dp_min;
    GLuint dp_num;
    // GLuint *ebo_mm;
} Element_Array_Buffer;

typedef struct Program_Info
{
    std::map<GLint, GLenum> uniform_loction_type;
    std::map<std::string, GLint> uniform_name_location;
    std::map<GLint, GLenum> attrib_loction_type;
    std::map<std::string, GLint> attrib_name_location;
    std::map<GLint, GLint> uniform_block_index_num;

    int active_uniform_block_count;
    int transform_feedback_varyings;
    int uniform_num;
    int attrib_num;
    // GLint link_status;
} Program_info;


typedef struct Buffer_Status{
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
}Buffer_Status;


typedef struct Bound_Buffer
{

    //这个buffer_status指针是指向实际vao_status里的值
    Buffer_Status *buffer_status;

    std::map<GLint, GLenum> buffer_type;

    std::map<GLint, Buffer_Status *> vao_status;

    std::map<GLint, Attrib_Point *> vao_point_data;

    std::map<GLint, Element_Array_Buffer *> ebo_buffer;

    //打表表示索引下标最少是2的多少幂次
    GLuint *ebo_mm;
    GLuint ebo_mm_num;

} Bound_Buffer;

typedef struct Map_Result
{
    GLenum target;
    GLintptr offset;
    GLsizeiptr length;
    GLbitfield access;
    char map_data_info[32];
    char *map_data;
} Map_Result;


typedef struct Buffer_Mapped
{
    //3.0支持的几个缓冲区
    Map_Result array_buf_ptr;
    Map_Result elem_array_buf_ptr;
    Map_Result copy_read_buf_ptr;
    Map_Result copy_write_buf_ptr;
    Map_Result pixel_pack_buf_ptr;
    Map_Result pixel_unpack_buf_ptr;
    Map_Result trans_feedback_buf_ptr;
    Map_Result uniform_buf_ptr;

    //es3.2
    Map_Result texture_buf_ptr;
    Map_Result shader_storage_buf_ptr;
    Map_Result dispatch_indirect_buf_ptr;
    Map_Result draw_indirect_buf_ptr;
    Map_Result atomic_counter_buf_ptr;

} Buffer_Mapped;


typedef struct Direct_GPU_Context
{
    Pixel_Store_Status pixel_store_status;
    Bound_Buffer bound_buffer_status;

    // Attrib_Point attrib_point;

    std::map<GLint, Program_Info *> all_program_info;

    Buffer_Mapped buffer_ptr;


    //这里用open产生的文件句柄，是因为open对应的write是无缓存写入，一次写入能带来一次系统调用，假如用fwrite的话不对，因为它自带用户态缓存
    //这里为了缓存异步调用，减少系统调用次数，手动设了个缓存

    // std::vector<char *> need_free_bufs;

    // char asyn_buf[MAX_ASYN_BUF_SIZE];
    // int asyn_buf_loc;
    
    int direct_express; 

} Direct_GPU_Context;













GLint get_bound_buffer(void *context, GLenum target);

void get_max_min_point_index_direct(void *context, const void *data, GLsizei count, GLenum type, GLuint *min_index, GLuint *max_index);

void send_point_attrib_data_indices(void *context, GLsizei count, GLenum type, const void *indices, GLuint instancecount);

void get_max_min_point_index_rmq(void *context, uint64_t offset, GLsizei count, GLenum type, GLuint *min_index, GLuint *max_index);

void send_point_attrib_data(void *context, GLuint min_index, GLuint max_index, GLuint instancecount);

uint64_t gl_get_program_uniform_size(void *context,GLuint program,GLint location);


void d_glGenVertexArrays(void *context, GLsizei n, GLuint *arrays);

void d_glBindVertexArray(void *context, GLuint array);

void d_glDeleteVertexArrays(void *context, GLsizei n, const GLuint *arrays);

void d_glGenVertexArraysOES(void *context, GLsizei n, GLuint *arrays);

void d_glBindVertexArrayOES(void *context, GLuint array);

void d_glDeleteVertexArraysOES(void *context, GLsizei n, const GLuint *arrays);

void d_glBindBuffer(void *context, GLenum target, GLint id);

void d_glPixelStorei(void *context, GLenum pname, GLint param);




int gl_get_uniform_block_para_size(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname);

size_t get_program_uniform_size(void *context, GLuint program, GLint location);

uint64_t gl_pixel_data_size(void *context, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack);

uint64_t gl_pixel_data_3d_size(void *context, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack);

void send_to_host(void *context, unsigned char *send_buf, size_t send_buf_len);

void put_page_host(void *context, void *buf,size_t buf_len);



void d_glTexImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);

void d_glTexSubImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);

void d_glTexImage3D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);

void d_glTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);

void d_glReadPixels(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);

void d_glLinkProgram(void *context, GLuint program);

void d_glDeleteProgram(void *context, GLuint program);

void d_glDrawArrays(void *context, GLenum mode, GLint first, GLsizei count);

void d_glDrawArraysInstanced(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

void d_glDrawElements(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices);

void d_glDrawRangeElements(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);

void d_glDrawElementsInstanced(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);


void init_vertex_attrib_offset(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLenum invoke_type,const void *pointer);


void init_vertex_attrib_pointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLenum invoke_type,const void *pointer);




void d_glVertexAttribPointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);



void d_glGetVertexAttribPointerv(void *context,GLuint index, GLenum pname, void **pointer);

int get_vertex_attrib(void *context,GLuint index, GLenum pname,void *param);


void d_glGetVertexAttribIiv(void *context, GLuint index, GLenum pname, GLint *params);
void d_glGetVertexAttribIuiv(void *context, GLuint index, GLenum pname, GLuint *params);

void d_glGetVertexAttribfv(void *context, GLuint index, GLenum pname, GLfloat *params);
void d_glGetVertexAttribiv(void *context, GLuint index, GLenum pname, GLint *params);


void d_glDisableVertexAttribArray(void *context,GLuint index);
void d_glEnableVertexAttribArray(void *context,GLuint index);


void d_glVertexAttribDivisor(void *context, GLuint index, GLuint divisor);

void d_glGetBufferPointerv(void *context, GLenum target, GLenum pname, void **params);

void d_glBufferData(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage);

void d_glBufferSubData(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data);

void d_glDeleteBuffers(void *context, GLsizei n, const GLuint *buffers);



Map_Result *get_map_result(void *context, GLenum target);


void *d_glMapBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);

GLboolean d_glUnmapBuffer(void *context, GLenum target);

void d_glFlushMappedBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length);

void * context_init(void);




const GLubyte* d_glGetStringi(GLenum name, GLuint index);
const GLubyte* d_glGetString(GLenum name);