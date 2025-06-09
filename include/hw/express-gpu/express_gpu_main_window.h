#ifndef EXPRESS_GPU_MAIN_WINDOW_H
#define EXPRESS_GPU_MAIN_WINDOW_H

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_EGL
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif

#include "qemu/atomic.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "hw/express-gpu/egl_window.h"
#include "hw/express-gpu/gl_helper.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/glv3_decl.h"

#define MAIN_PAINT 1
#define MAIN_CREATE_CHILD_WINDOW 2
#define MAIN_DESTROY_CHILD_WINDOW 3
#define MAIN_DESTROY_ALL_EGLSYNC 5
#define MAIN_DESTROY_ONE_SYNC 6
#define MAIN_DESTROY_GBUFFER 7

#define ATOMIC_LOCK(s)                                              \
     int atomic_cnt = 1;                                            \
     while (qatomic_cmpxchg(&(s), 0, 1) == 1 && atomic_cnt < 10000) \
          if(atomic_cnt % 10 == 0) LOGD("lock on %s %d fun:%s ", #s, atomic_cnt++, __FUNCTION__);
#define ATOMIC_UNLOCK(s) qatomic_cmpxchg(&(s), 1, 0)


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

typedef struct Destroy_Child_Window_Event_Data
{
     void *window; //指向子窗口的指针
     int context_flags; //子窗口的上下文标志
} Destroy_Child_Window_Event_Data;

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


extern Static_Context_Values *preload_static_context_value;

extern int host_opengl_version;

extern int DSA_enable;

void start_main_window_thread(void);

int save_gbuffer_global_map(QEMUFile *f);
int load_gbuffer_global_map(QEMUFile *f);

void remove_gbuffer_from_global_map(uint64_t gbuffer_id);

void add_gbuffer_to_global(Hardware_Buffer *global_gbuffer);

Hardware_Buffer *get_gbuffer_from_global_map(uint64_t gbuffer_id);

void send_message_to_main_window(int message_code, void *data);

#endif