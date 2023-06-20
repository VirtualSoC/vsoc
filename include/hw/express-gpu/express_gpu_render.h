#ifndef QEMU_EXPRESS_GPU_RENDER_H
#define QEMU_EXPRESS_GPU_RENDER_H

#include "qemu/atomic.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include "glad/glad.h"
#include "hw/express-gpu/GLFW/glfw3.h"
#include "hw/express-gpu/GLFW/glfw3native.h"

#include "hw/express-gpu/egl_window.h"

#include "hw/express-gpu/gl_helper.h"

#include "hw/express-gpu/egl_surface.h"

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
#define MAIN_DESTROY_ONE_SYNC 7
#define MAIN_DESTROY_ONE_TEXTURE 8
#define MAIN_DESTROY_GBUFFER 9
#define MAIN_CANCEL_GBUFFER 10
#define MAIN_PAINT_LAYERS 11

#define GBUFFER_TYPE_WINDOW 1
#define GBUFFER_TYPE_TEXTURE 2


// #define GBUFFER_TYPE_NONE 0
// #define GBUFFER_TYPE_WINDOW 1
// #define GBUFFER_TYPE_BITMAP 2
// #define GBUFFER_TYPE_NATIVE 3
// #define GBUFFER_TYPE_BITMAP_NEED_DATA 4
// #define GBUFFER_TYPE_TEXTURE 5
// #define GBUFFER_TYPE_FBO 6
// #define GBUFFER_TYPE_FBO_NEED_DATA 7

#define ATOMIC_LOCK(s)                                              \
     int atomic_cnt = 1;                                            \
     while (qatomic_cmpxchg(&(s), 0, 1) == 1 && atomic_cnt < 10000) \
          if(atomic_cnt % 10 == 0) printf("lock on %s %d fun:%s ", #s, atomic_cnt++, __FUNCTION__);
#define ATOMIC_UNLOCK(s) qatomic_cmpxchg(&(s), 1, 0)

// #define ATOMIC_SET_USED(s) ATOMIC_LOCK(s)
// #define ATOMIC_SET_UNUSED(s) ATOMIC_UNLOCK(s)

#define ATOMIC_SET_USED(s) (qatomic_cmpxchg(&(s), 0, 1))
#define ATOMIC_SET_UNUSED(s) (qatomic_cmpxchg(&(s), 1, 0))

//是否启用opengl调试模式的宏定义
// 已经废弃，可以通过命令行设置
// #define ENABLE_OPENGL_DEBUG

//是否启用opengl执行性能警告输出
#define ENABLE_OPENGL_PERFORMANCE_WARNING

// 是否启用全局独立窗口进行调试的宏定义
// 已经废弃，可以通过命令行设置
// #define DEBUG_INDEPEND_WINDOW

//下面这个宏定义已经弃用
//是否使用glfw代替原生wgl创建pbuffer的过程，使用这个才能启用DEBUG_INDEPEND_WINDOW
// #define USE_GLFW_AS_WGL

//保证窗口有相同长宽比的宏
// #define ENSURE_SAME_WIDTH_HEIGHT_RATIO

//是否开启外置窗口固定帧率刷新模式的宏
// #define ENABLE_STATIC_WINDOW_REFRESH

//是否启用DSA模式
// DSA模式尚未测试确定可用，而且性能提升并不明显，暂时不支持
// #define ENABLE_DSA

#ifdef ENABLE_DSA
#define DSA_LIKELY(t) likely(t)
#else
#define DSA_LIKELY(t) unlikely(t)
#endif

typedef struct Main_window_Event
{
     int event_code;
     void *data;
} Main_window_Event;


typedef struct GBuffer_Layer{
     int x;
     int y;
     int z;
     int width;
     int height;
     int blend_type;
     int transform_type;
     int crop_x;
     int crop_y;
     int crop_width;
     int crop_height;
     int write_sync_id;
     int read_sync_id;
     //SetLayerVisibleRegion暂时先不支持
     uint64_t gbuffer_id;
} __attribute__((packed, aligned(4))) GBuffer_Layer;

typedef struct GBuffer_Layers{
     int layer_num;
     struct GBuffer_Layer layer[0];
} __attribute__((packed, aligned(4))) GBuffer_Layers;



// typedef struct Static_Context_Values1 Static_Context_Values1;

// extern HWND draw_native_window;

extern GAsyncQueue *main_window_event_queue;

extern volatile int native_render_run;

extern volatile int device_interface_run;

extern Static_Context_Values *preload_static_context_value;

// extern void *dummy_window_for_sync;

extern int host_opengl_version;

extern int DSA_enable;

extern int composer_refresh_HZ;

extern QemuThread native_window_render_thread;

extern int force_show_native_render_window;


extern Graphic_Buffer *main_display_gbuffer;


void *native_window_thread(void *opaque);
// void *opengl_ui_thread(void *opaque);

int draw_wait_GSYNC(void *event, int wait_frame_num);

// void render_windows_create(Window_Buffer *context);

// void set_compose_surface(Window_Buffer *origin_surface, Window_Buffer *new_surface);

// GLuint acquire_texture_from_surface(Window_Buffer *surface);

// void release_texture_from_surface(Window_Buffer *surface);

// GLuint acquire_texture_from_image(EGL_Image *image);

// void init_image_texture(EGL_Image *image);

// void init_image_fbo(EGL_Image *image, int need_reverse);

// void release_texture_from_image(EGL_Image *image);

// Window_Buffer *get_surface_from_gbuffer_id(uint64_t gbuffer_id);

// void release_surface(Window_Buffer *real_surface);

// void set_gbuffer_id_surface(uint64_t gbuffer_id, Window_Buffer *origin_surface, Window_Buffer *now_surface);

// EGL_Image *get_image_from_gbuffer_id(uint64_t gbuffer_id);

// void set_gbuffer_id_image(uint64_t gbuffer_id, EGL_Image *origin_image, EGL_Image *now_image);

void remove_gbuffer_from_global_map(uint64_t gbuffer_id);

void add_gbuffer_to_global(Graphic_Buffer *global_gbuffer);

Graphic_Buffer *get_gbuffer_from_global_map(uint64_t gbuffer_id);

void opengl_paint_gbuffer(Graphic_Buffer *gbuffer);

// int get_global_gbuffer_type(uint64_t gbuffer_id);

// void set_global_gbuffer_type(uint64_t gbuffer_id, int type);

void send_message_to_main_window(int message_code, void *data);

// void set_display_gbuffer(Graphic_Buffer *gbuffer);

// bool should_give_up_gpu();

#endif