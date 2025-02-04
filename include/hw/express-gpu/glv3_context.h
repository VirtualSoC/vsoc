#ifndef GLV3_CONTEXT_H
#define GLV3_CONTEXT_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/express_log.h"
#include "hw/express-gpu/express_gpu_render.h"

#include "hw/express-gpu/glv3_status.h"
#include "migration/qemu-file.h"


#define MAX_VERTEX_ATTRIBS_NUM 16


typedef struct Attrib_Point
{
    //这些也都主要用于处理with_bound的情况
    GLuint buffer_object[MAX_VERTEX_ATTRIBS_NUM]; //vbo，不同index对应着色器中不同的采样点layout(location = 0/1/2...)
    GLint buffer_loc[MAX_VERTEX_ATTRIBS_NUM];

    GLint remain_buffer_len[MAX_VERTEX_ATTRIBS_NUM];
    GLint buffer_len[MAX_VERTEX_ATTRIBS_NUM]; //这几个是glVertexAttribPointer等一系列函数的时候用的，和下面那些含义差不多

    GLuint indices_buffer_object; //默认的ebo(ibo),指向在set_indices_data里绑定的ebo，处理guest没有绑定ebo的情况
    GLint indices_buffer_len; //用来算ebo要从哪开始映射

    GLint remain_indices_buffer_len; //用来算ebo要从哪里开始映射 这几个是gldrawelements等一系列函数的时候用的

    // GLint buffer_num;
    GLint element_array_buffer; //指示现在被指定绑定的ebo

} Attrib_Point;

typedef struct Bound_Buffer
{

    Attrib_Point *attrib_point;

    GHashTable *vao_point_data;

    GLuint asyn_unpack_texture_buffer;
    GLuint asyn_pack_texture_buffer;

    Buffer_Status buffer_status; //这个就是记录具体的bound情况的,特别是DSA的情况

    int has_init;
} Bound_Buffer; //关注一下，有点意思




typedef struct Resource_Map_Status
{
    unsigned int max_id;
    unsigned int map_size;
    // unsigned int now_map_len;
    long long *resource_id_map;
    char *resource_is_init; //texture用的，是1代表init了，是2代表已经关联了gbuffer。
    
    unsigned int gbuffer_map_max_size; //整个按照maxsize全存下来吧
    Hardware_Buffer **gbuffer_ptr_map; //相当于一个存储Hardware_Buffer* 的数组, 暂未存下来。得存下来！
    
} Resource_Map_Status;

typedef struct Share_Resources
{

    int counter;

    //Regular objects
    Resource_Map_Status texture_resource;
    Resource_Map_Status buffer_resource;
    Resource_Map_Status render_buffer_resource;
    Resource_Map_Status sample_resource;

    //GLSL objects, include program,shader
    Resource_Map_Status program_resource;
    Resource_Map_Status shader_resource;

    //Sync objects
    Resource_Map_Status sync_resource;

} Share_Resources;

typedef struct Exclusive_Resources
{

    //Container objects
    Resource_Map_Status frame_buffer_resource;
    Resource_Map_Status program_pipeline_resource;
    Resource_Map_Status transform_feedback_resource;
    Resource_Map_Status vertex_array_resource;

    //query objects
    Resource_Map_Status query_resource;

} Exclusive_Resources;

typedef struct Resource_Context
{
    // Regular objects
    Resource_Map_Status *texture_resource;
    Resource_Map_Status *buffer_resource;
    Resource_Map_Status *render_buffer_resource;
    Resource_Map_Status *sampler_resource;

    //GLSL objects, include program,shader
    Resource_Map_Status *shader_resource;
    Resource_Map_Status *program_resource;

    //Sync objects
    Resource_Map_Status *sync_resource;

    //Container objects
    Resource_Map_Status *frame_buffer_resource;
    Resource_Map_Status *program_pipeline_resource;
    Resource_Map_Status *transform_feedback_resource;
    Resource_Map_Status *vertex_array_resource;

    // query objects
    Resource_Map_Status *query_resource;

    // objects sharable among contexts
    Share_Resources *share_resources;
    
    // container objects exclusive to each opengl context
    Exclusive_Resources *exclusive_resources;

} Resource_Context;

typedef struct Opengl_Context
{
    //注意，这个window必须得放到opengl_context这边，因为opengl的环境保存在这边了
    void *window;

    Bound_Buffer bound_buffer_status;

    Resource_Context resource_status;


    void *share_context;
    GHashTable *buffer_map;
    GLuint draw_fbo0; //默认读取/绘制的地方 ztodo:Gluint用32位够不够
    GLuint read_fbo0;

    GLuint current_read_fbo;
    GLuint current_write_fbo;

    GLuint current_program;
    GHashTable *framebuffer_map;


    GLuint vao0;

    GLint view_x;
    GLint view_y;

    GLsizei view_w;
    GLsizei view_h;

    int is_current;
    int need_destroy;
    EGLContext guest_context;


    Texture_Binding_Status texture_binding_status;
    // external_texture不受到当前激活的纹理影响，只要绑定了就能用
    GLuint is_using_external_program;

    GLuint enable_scissor;

    GLint context_flags;

    GLuint draw_texi_vao;
    GLuint draw_texi_vbo;
    GLuint draw_texi_ebo;

    GLenum blendfunc_sfactor;
    GLenum blendfunc_dfactor;

    void *debug_message_buffer;
} Opengl_Context;

typedef struct Guest_Host_Map
{
    GLubyte *host_data;
    unsigned long map_len;
    GLenum target;
    GLbitfield access;

} Guest_Host_Map;

void d_glGetString_special(void *context, GLenum name, GLubyte *buffer);

void d_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte *buffer);

void resource_context_init(Resource_Context *resources, Share_Resources *share_resources);

void resource_context_destroy(Resource_Context *resources);

Opengl_Context *opengl_context_create(Opengl_Context *share_context, int context_flags);

void opengl_context_init(Opengl_Context *context);

void opengl_context_destroy(Opengl_Context *context);

void *get_native_opengl_context(int context_flags);

void release_native_opengl_context(void *native_context, int context_flags);

int get_window_id(void *window);

void save_native_context_pool(QEMUFile *f);

void load_native_context_pool(QEMUFile *f);


#endif