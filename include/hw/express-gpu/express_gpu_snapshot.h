#ifndef EXPRESS_GPU_SNAPSHOT_H
#define EXPRESS_GPU_SNAPSHOT_H

// #include "migration/qemu-file.h"
// #include "hw/teleport-express/express_device_common.h"
// #include "hw/teleport-express/teleport_express_call.h"

// #include "hw/express-gpu/glv3_context.h"
// #include "hw/express-gpu/egl_display.h"
// #include "hw/express-gpu/egl_surface.h"
#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/teleport-express/teleport_express_register.h"

#include "hw/teleport-express/express_log.h"

#include "hw/virtio/virtio.h"

#include "migration/qemu-file.h"
#include "hw/express-gpu/express_gpu.h"


#define NUM_RESOURCES 13
#define RESOURCE_TYPE_SHADER 0
#define RESOURCE_TYPE_TEXTURE 1
#define RESOURCE_TYPE_BUFFER 2
#define RESOURCE_TYPE_FRAMEBUFFER 3
#define RESOURCE_TYPE_RENDERBUFFER 4
#define RESOURCE_TYPE_PROGRAM 5
#define RESOURCE_TYPE_VERTEX_ARRAY 6
#define RESOURCE_TYPE_SHADER_PROGRAM 7 //ztodo:这是啥来着,没这玩意吧
#define RESOURCE_TYPE_SYNC 8
#define RESOURCE_TYPE_SAMPLER 9
#define RESOURCE_TYPE_PROGRAM_PIPELINE 10
#define RESOURCE_TYPE_TRANSFORM_FEEDBACK 11
#define RESOURCE_TYPE_QUERY 12

#define MAX_CUBEMAP_MIPMAP_LEVELS 6  // ztodo:确认数值
#define MAX_TEXTURE_UNITS 16
typedef struct Express_Native_Shader {
    GLint id;
    GLenum type;
    GLboolean deleteStatus;
    // GLboolean compileStatus;
    // GLint sourceLength;
    // char* source;
} Express_Native_Shader;

// program处理方法：（program_id<<32）| shader_id

typedef struct Express_Native_Sampler {
    GLuint sampler_id;
    GLint min_filter;
    GLint mag_filter;
    GLint wrap_s;
    GLint wrap_t;
    GLint wrap_r;
    GLint compare_mode;
    GLint compare_func;
    GLint max_anisotropy;
    GLint min_lod;
    GLint max_lod;
    GLint lod_bias;
    GLint border_color[4];
    GLint swizzle_r;
    GLint swizzle_g;
    GLint swizzle_b;
    GLint swizzle_a;
} Express_Native_Sampler;


typedef struct Express_Native_Program_Shader{
    GLuint shader_id;
    GLenum shader_type;
    GLuint shader_source_length;
    int attached_order;
    char* shader_source;
} Express_Native_Program_Shader;
typedef struct Express_Native_Program {
    GHashTable *shader_map;
    GLuint programId;
    int shader_num;
    //ztodo:是否需要has_linked记录？
} Express_Native_Program;

typedef struct Express_Native_Buffer {
    GLuint bufferId;
    GLenum target;
    GLint size;
    GLenum usage;
    GLvoid* data;
} Express_Native_Buffer;

typedef struct Express_Native_buffer_Simple {
    GLuint bufferId;
    GLenum target;
    int data_upload_strategy;//1:通过glBufferData上传数据 2:通过glMapBufferRange上传数据
} Express_Native_buffer_Simple;


typedef struct Express_Native_Texture_Simple {
    GLint textureId;
    GLenum target;
} Express_Native_Texture_Simple;

typedef struct Express_Native_Framebuffer { //ztodo:没处理renderbuffer相关。此外现在只能绑一个东西，需要重新处理一下怎么定key(类似program那样，关于前两个字段独一无二）！！
    GLuint framebufferId;
    GLenum attachment_target[16]; //attach的texture
    GLenum renderbuffer_attachment[19];//后16个是GL_COLOR_ATTACHMENT0~15，前3个是GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT, GL_DEPTH_STENCIL_ATTACHMEN
    GLuint texture_id;
} Express_Native_Framebuffer;

typedef struct Express_Native_Texture {
    GLuint textureId;
    GLenum target;
    GLint width;
    GLint height;
    GLint depth;
    GLint internalFormat;

    GLubyte* pixels;

    GLuint binding2D;
    GLuint bindingCubeMap;

    GLenum minFilter;
    GLenum magFilter;
    GLenum wrapS;
    GLenum wrapT;

    GLenum texture_swizzle_r;
    GLenum texture_swizzle_g;
    GLenum texture_swizzle_b;
    GLenum texture_swizzle_a;


    // // 2D mipmap数据
    // GLenum format2D;
    // GLenum type2D;
    // GLsizei width2D;
    // GLsizei height2D;
    // GLboolean isCompressed2D;
    // GLsizei compressedSize2D;
    // GLubyte* bytes2D;

    // // CubeMap mipmap
    // GLenum formatCubeMap[MAX_CUBEMAP_MIPMAP_LEVELS];
    // GLenum typeCubeMap[MAX_CUBEMAP_MIPMAP_LEVELS];
    // GLsizei widthCubeMap[MAX_CUBEMAP_MIPMAP_LEVELS];
    // GLsizei heightCubeMap[MAX_CUBEMAP_MIPMAP_LEVELS];
    // GLboolean isCompressedCubeMap[MAX_CUBEMAP_MIPMAP_LEVELS];
    // GLsizei compressedSizeCubeMap[MAX_CUBEMAP_MIPMAP_LEVELS];
    // GLubyte* bytesCubeMap[MAX_CUBEMAP_MIPMAP_LEVELS];

    // GLint textureUnit; //纹理使用的单元（如 glActiveTexture 的单位）
} Express_Native_Texture;


void restore_opengl_context_textures(Opengl_Context *context);


// extern GList *native_shaders; 
// extern int native_shaders_num;
// extern int native_shaders_locker;
extern GHashTable *g_resource_list[NUM_RESOURCES];
// extern int g_resource_count[NUM_RESOURCES];
extern int g_resource_locker[NUM_RESOURCES];

extern int display_fbo_has_loaded;

extern GHashTable *loaded_hardware_buffers;
extern GHashTable *loaded_window_buffers;
extern GHashTable *registered_express_buffers;
extern GHashTable *registered_express_irqs;

extern VirtIODevice *startup_vdev;
extern VirtQueue *startup_out_data_queue;
extern VirtQueue *startup_in_data_queue;

extern int display_context_thread_id;

void clear_resource_tables(void);
void init_saving_snapshot(void);
void init_loading_snapshot(QEMUFile *f);



void save_native_resources(QEMUFile *f);
void load_native_resources(QEMUFile *f);
void save_native_shaders(QEMUFile *f);
void load_native_shaders(QEMUFile *f);

void save_native_programs(QEMUFile *f);
void load_native_programs(QEMUFile *f);

void save_native_programs_tmp(QEMUFile *f);
void load_native_programs_tmp(QEMUFile *f);
void load_native_textures_tmp(QEMUFile *f);
void load_native_shaders_tmp(QEMUFile *f);



void save_native_textures(QEMUFile *f);
void load_native_textures(QEMUFile *f);

void update_native_texture(Express_Native_Texture* texture_data);
bool compare_texture(Express_Native_Texture* native_texture);
bool compare_two_textures(const struct Express_Native_Texture* texture1, const struct Express_Native_Texture* texture2);

void save_single_texture(QEMUFile *f, GLint texture_id, GLenum texture_type);

int save_single_render_thread_context(QEMUFile *f, Render_Thread_Context *thread_context);
Render_Thread_Context* load_single_render_thread_context(QEMUFile *f);

int save_thread_context(QEMUFile *f, Thread_Context *context);
Render_Thread_Context* load_thread_context(QEMUFile *f);

void save_process_context(QEMUFile *f, Process_Context *process_context);
void load_process_context(QEMUFile *f, Process_Context *process_context);

void save_window_buffer(QEMUFile *f, Window_Buffer *buffer, GHashTable* gbuffer_map);
int load_window_buffer(QEMUFile *f, Window_Buffer *buffer, GHashTable* gbuffer_map);

void save_scatter_data(QEMUFile *f, Scatter_Data *scatter_data, int count);
Scatter_Data* load_scatter_data(QEMUFile *f, int *count);

void save_guest_mem(QEMUFile *f, Guest_Mem *guest_mem);
Guest_Mem* load_guest_mem(QEMUFile *f, int strategy);

void save_opengl_context(QEMUFile *f, Opengl_Context *context);
int load_opengl_context(QEMUFile *f, Opengl_Context *context);

void save_bound_buffer(QEMUFile *f, Bound_Buffer *buffer);
Bound_Buffer* load_bound_buffer(QEMUFile *f);

void save_buffer_status(QEMUFile *f, Buffer_Status *status);
Buffer_Status* load_buffer_status(QEMUFile *f);

void save_texture_binding_status(QEMUFile *f, Texture_Binding_Status *status);
Texture_Binding_Status* load_texture_binding_status(QEMUFile *f);

void save_resource_context(QEMUFile *f, Resource_Context *context);
Resource_Context* load_resource_context(QEMUFile *f);

void save_resource_map_status(QEMUFile *f, Resource_Map_Status *status);
Resource_Map_Status* load_resource_map_status(QEMUFile *f, int resource_type);

void save_egl_display(QEMUFile *f, Egl_Display *display);
int load_egl_display(QEMUFile *f, Egl_Display* display);

void save_egl_config(QEMUFile *f, eglConfig *config);
eglConfig* load_egl_config(QEMUFile *f);

void save_hardware_buffer(QEMUFile *f, Hardware_Buffer *buffer);
Hardware_Buffer* load_hardware_buffer(QEMUFile *f);

void save_thread_unique_ids(QEMUFile *f, GHashTable *thread_unique_ids);
int load_thread_unique_ids(QEMUFile *f, GHashTable *thread_unique_ids);

Teleport_Express_Call* load_teleport_express_call(QEMUFile *f);
void save_teleport_express_call(QEMUFile *f, Teleport_Express_Call *call);


void save_attrib_point(QEMUFile *f, Attrib_Point *point);
Attrib_Point* load_attrib_point(QEMUFile *f);

void save_guest_host_map(QEMUFile *f, Guest_Host_Map *map);
Guest_Host_Map* load_guest_host_map(QEMUFile *f);

GLuint restore_single_framebuffer(Express_Native_Framebuffer *framebuffer);
GLuint save_single_framebuffer(QEMUFile* f, Express_Native_Framebuffer *framebuffer);

GLint get_host_id_map(int type, GLint old_id);


#endif