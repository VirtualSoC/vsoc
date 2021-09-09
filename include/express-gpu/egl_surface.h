#ifndef EGL_SURFACE_H
#define EGL_SURFACE_H

#include "direct-express/direct_express_distribute.h"
#include "express-gpu/egl_define.h"
#include "express-gpu/egl_config.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>



#define P_SURFACE 1
#define WINDOW_SURFACE 2


typedef struct EGL_Image{
     int display_texture_is_use;
     GLuint display_fbo;
     GLuint fbo_texture;
     GLsync fbo_sync;
     GLsync fbo_sync_need_delete;
} EGL_Image;


typedef struct Double_Buffer
{

     //原始的native窗口
     GLFWwindow *window;
     GLFWHints window_hints;

     eglConfig *config;

     uint64_t guest_native_window;

     uint64_t guest_gbuffer_id;

     // struct Double_Buffer *now_acquired_surface;
     // struct Double_Buffer *last_acquired_surface;

     //可以交换的事件
     HANDLE swap_event;

     //这里用数组循环是为了方便进行设置单缓冲、双缓冲，甚至是三重缓冲
     GLuint display_fbo[5];

     //用于MSAA抗锯齿的fbo
     GLuint sampler_fbo[5];
     GLuint now_draw;
     GLuint now_read;
     GLuint now_acquired;
     // GLuint draw_num;
     // GLuint read_num;

     GLuint buffer_num;
 
     // GLuint fbo_draw;
     // GLuint fbo_display;

     GLuint fbo_texture[5];
     GLuint display_rbo_depth[5];
     GLuint display_rbo_stencil[5];

     //用于MSAA抗锯齿的rbo
     //由于图像之后可能要被surfaceflinger使用，因此这里只能使用bilt来复制出来数据
     GLuint sampler_rbo[5];
     

     GLsync fbo_sync[5];


     GLsync delete_sync[5];
     int delete_loc;

     //表示这个纹理当前是不是被用来绘制，是的话这个时候这个纹理不能被用来绘制，只能读取
     int display_texture_is_use[5];




     //表示窗口的宽和高
     int width;
     int height;

     int type;

     int is_current;
     int need_destroy;

     int last_frame_num;
     int swap_interval;

     int I_am_composer;

     // //它绑定的opengl指针
     // void *opengl_context;
     
     // int calc_hz;
     // int draw_hz;

     // int frame_gen_time;
     // gint64 last_gen_time;

     // gint64 last_swap_time;

     int64_t swap_time[20];
     int64_t swap_time_all;
     int swap_loc;
     int swap_time_cnt;

     int ref_count;

     int has_connect_opengl;

} Double_Buffer;

#define ATOMIC_LOCK(s)                      \
     while (atomic_cmpxchg(&(s), 0, 1) == 1) \
          ;
#define ATOMIC_UNLOCK(s) atomic_cmpxchg(&(s), 1, 0)

void egl_surface_swap_buffer(Double_Buffer *surface);

int render_surface_destroy(Double_Buffer *surface);


void d_eglIamComposer(void *context, EGLSurface surface);

// EGLBoolean d_eglSwapBuffers_special(Render_Thread_Context *context, EGLDisplay dpy, EGLSurface surface);
// EGLBoolean d_eglMakeCurrent_special(Render_Thread_Context *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
// EGLBoolean d_eglTerminate_special(Render_Thread_Context *context, EGLDisplay dpy);
// int d_getEGLConfigParam_special(Render_Thread_Context *context, int *num_configs);
// EGLint d_getEGLConfigs_special(Render_Thread_Context *context, EGLint num_attrs, EGLint *attr_list);
// EGLBoolean d_getEGLVersion_special(Render_Thread_Context *context, EGLint *ver_major, EGLint *ver_minor);

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface);

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface);

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute,EGLint value);


void d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target,
                                  EGLClientBuffer buffer, const EGLAttrib *attrib_list,EGLImage guest_image);

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);

#endif