#ifndef QEMU_EXPRESS_GPU_RENDER_H
#define QEMU_EXPRESS_GPU_RENDER_H
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#endif
#include "qemu/atomic.h"

#include "glad/glad.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#else
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif

#include "qemu/atomic.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#else
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif

#include "glad/glad.h"
#include "hw/express-gpu/GLFW/glfw3.h"
#include "hw/express-gpu/GLFW/glfw3native.h"

#include "hw/express-gpu/egl_window.h"

#include "hw/express-gpu/gl_helper.h"

#include "hw/express-gpu/egl_surface.h"

#include "hw/express-gpu/glv3_decl.h"

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

#define ATOMIC_LOCK(s)                                              \
     int atomic_cnt = 1;                                            \
     while (qatomic_cmpxchg(&(s), 0, 1) == 1 && atomic_cnt < 10000) \
          if(atomic_cnt % 10 == 0) LOGD("lock on %s %d fun:%s ", #s, atomic_cnt++, __FUNCTION__);
#define ATOMIC_UNLOCK(s) qatomic_cmpxchg(&(s), 1, 0)

#define ATOMIC_SET_USED(s) (qatomic_cmpxchg(&(s), 0, 1))
#define ATOMIC_SET_UNUSED(s) (qatomic_cmpxchg(&(s), 1, 0))


//是否启用opengl执行性能警告输出
#define ENABLE_OPENGL_PERFORMANCE_WARNING


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


extern GAsyncQueue *main_window_event_queue;

extern volatile int native_render_run;

extern volatile int device_interface_run;

extern Static_Context_Values *preload_static_context_value;

extern int host_opengl_version;

extern int DSA_enable;


extern QemuThread native_window_render_thread;

extern int force_show_native_render_window;


extern Hardware_Buffer *main_display_gbuffer;


void *native_window_thread(void *opaque);

int draw_wait_GSYNC(void *event, int wait_frame_num);


void remove_gbuffer_from_global_map(uint64_t gbuffer_id);

void add_gbuffer_to_global(Hardware_Buffer *global_gbuffer);

Hardware_Buffer *get_gbuffer_from_global_map(uint64_t gbuffer_id);

void opengl_paint_gbuffer(Hardware_Buffer *gbuffer);

void send_message_to_main_window(int message_code, void *data);

#endif