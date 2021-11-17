#ifndef QEMU_EXPRESS_GPU_RENDER_H
#define QEMU_EXPRESS_GPU_RENDER_H

#include "qemu/atomic.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#include <GLFW/glfw3native.h>

#include "express-gpu/egl_surface.h"

#define SPECIAL_SCREEN_SYNC_HZ 60

#define WM_USER_PAINT WM_USER + 10
#define WM_USER_WINDOW_CREATE WM_USER + 11
#define WM_USER_SURFACE_DESTROY WM_USER + 12
#define WM_USER_CONTEXT_DESTROY WM_USER + 13
#define WM_USER_IMAGE_DESTROY WM_USER + 14

#define MAIN_PAINT 1
#define MAIN_CREATE_CHILD_WINDOW 2
#define MAIN_DESTROY_SURFACE 3
#define MAIN_DESTROY_CONTEXT 4
#define MAIN_DESTROY_IMAGE 5
#define MAIN_DESTROY_ALL_EGLSYNC 6
#define MAIN_DESTROY_ONE_EGLSYNC 7


#define ATOMIC_LOCK(s)                       \
     while (atomic_cmpxchg(&(s), 0, 1) == 1) \
          printf("lock on %s ", #s);
#define ATOMIC_UNLOCK(s) atomic_cmpxchg(&(s), 1, 0)

// #define ATOMIC_SET_USED(s) ATOMIC_LOCK(s)
// #define ATOMIC_SET_UNUSED(s) ATOMIC_UNLOCK(s)

#define ATOMIC_SET_USED(s) (atomic_cmpxchg(&(s), 0, 1))
#define ATOMIC_SET_UNUSED(s) (atomic_cmpxchg(&(s), 1, 0))

//是否启用opengl调试模式的宏定义
// #define ENABLE_OPENGL_DEBUG

// 是否启用独立窗口进行调试的宏定义
// #define DEBUG_INDEPEND_WINDOW

typedef struct Main_window_Event
{
     int event_code;
     void *data;
} Main_window_Event;

//注意顺序，保证不影响结构体对齐
typedef struct Static_Context_Values
{

     GLint num_extensions;
     //____________ FIXED VALUE ____________
     GLint major_version;
     GLint minor_version;
     GLint implementation_color_read_format;
     GLint implementation_color_read_type;
     GLint max_array_texture_layers;
     GLint max_color_attachments;
     GLint max_combined_uniform_blocks;
     GLint max_draw_buffers;
     GLint max_fragment_input_components;
     GLint max_fragment_uniform_blocks;
     GLint max_program_texel_offset;
     GLint max_transform_feedback_interleaved_components;
     GLint max_transform_feedback_separate_attribs;
     GLint max_transform_feedback_separate_components;
     GLint max_uniform_buffer_bindings;
     GLint max_varying_components;
     GLint max_varying_vectors;
     GLint max_vertex_output_components;
     GLint max_vertex_uniform_blocks;
     GLint min_program_texel_offset;
     GLint num_program_binary_formats;
     //     GLint samples;
     //     GLint sample_buffers;
     //____________ QUERY HOST ___________
     GLint subpixel_bits;
     GLint num_compressed_texture_formats;
     GLint compressed_texture_formats[128];
     GLint max_3d_texture_size;
     GLint max_texture_size;
     GLint max_combined_texture_image_units;
     GLint max_cube_map_texture_size;
     GLint max_elements_vertices;
     GLint max_elements_indices;
     GLint max_fragment_uniform_components;
     GLint max_fragment_uniform_vectors;
     GLint max_renderbuffer_size;
     GLint max_vertex_attribs;
     GLint max_vertex_texture_image_units;
     GLint max_vertex_uniform_components;
     GLint max_vertex_uniform_vectors;
     GLint max_viewport_dims[2];
     GLint max_samples;
     GLint max_atomic_counter_buffer_bindings;
     GLint max_shader_storage_buffer_bindings;
     GLint num_shader_binary_formats;
     GLint program_binary_formats[8];
     GLint shader_binary_formats[8];
     //     GLint uniform_buffer_offset_alignment;
     //     GLint shader_storage_buffer_offset_alignment;
     GLfloat aliased_line_width_range[2];
     GLfloat aliased_point_size_range[2];
     GLfloat max_texture_log_bias;
     GLint64 max_element_index;
     GLint64 max_server_wait_timeout;
     GLint64 max_combined_fragment_uniform_components;
     GLint64 max_combined_vertex_uniform_components;
     GLint64 max_uniform_block_size;
     //下面实际要作为指针使用，保证与32位应用的兼容性，所以要这样弄
     GLuint64 vendor;
     GLuint64 version;
     GLuint64 renderer;
     GLuint64 shading_language_version;
     GLuint64 extensions_gles2;
     GLuint64 extensions[512];
     //大概需要512*100+400左右的空间存放字符串，这么大的空间应该是够了

} Static_Context_Values;

// extern HWND draw_native_window;

extern GAsyncQueue *main_window_event_queue;

extern volatile int native_render_run;

extern Static_Context_Values *preload_static_context_value;

extern GLFWwindow *glfw_dummy_window_for_sync;

void *native_window_thread(void *opaque);
// void *opengl_ui_thread(void *opaque);

int draw_wait_GSYNC(void *event, int wait_frame_num);

// void render_windows_create(Window_Buffer *context);

void set_compose_surface(Window_Buffer *origin_surface, Window_Buffer *new_surface);

GLuint acquire_texture_from_surface(Window_Buffer *surface);

void release_texture_from_surface(Window_Buffer *surface);

GLuint acquire_texture_from_image(EGL_Image *image);

void init_image_texture(EGL_Image *image);

void init_image_fbo(EGL_Image *image, int need_reverse);

void release_texture_from_image(EGL_Image *image);

Window_Buffer *get_surface_from_gbuffer_id(uint64_t gbuffer_id);

void set_surface_gbuffer_id(Window_Buffer *surface, uint64_t gbuffer_id);

EGL_Image *get_image_from_gbuffer_id(uint64_t gbuffer_id);

void set_image_gbuffer_id(EGL_Image *origin_image, EGL_Image *now_image, uint64_t gbuffer_id);

void send_message_to_main_window(int message_code, void *data);

// bool should_give_up_gpu();

#endif