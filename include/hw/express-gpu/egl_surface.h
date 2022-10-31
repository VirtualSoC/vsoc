#ifndef EGL_SURFACE_H
#define EGL_SURFACE_H

#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/express-gpu/egl_config.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>



#define P_SURFACE 1
#define WINDOW_SURFACE 2


#define HAL_PIXEL_FORMAT_RGBA_8888 1
#define HAL_PIXEL_FORMAT_RGBX_8888 2
#define HAL_PIXEL_FORMAT_RGB_888 3
#define HAL_PIXEL_FORMAT_RGB_565 4
#define HAL_PIXEL_FORMAT_BGRA_8888 5

#define MAX_WINDOW_LIFE_TIME (60*5)
#define MAX_BITMAP_LIFE_TIME (60*15)
#define MAX_COMPOSER_LIFE_TIME (MAX_WINDOW_LIFE_TIME*10)



typedef struct Graphic_Buffer{

     int is_writing;
     
     //假如合成器在使用这个gbufffer时发现正在写入，则新建并监听这个事件，以让对方来唤醒自己
#ifdef _WIN32
    HANDLE writing_ok_event;
#else

#endif
     int is_lock;
     int sampler_num;
     

     GLuint data_texture;
     GLuint reverse_rbo;
     GLuint sampler_rbo;
     GLuint rbo_depth;
     GLuint rbo_stencil;

     //这两个fbo不是gbuffer自己产生的
     GLuint data_fbo;
     GLuint sampler_fbo;


     //这个用来指示surface连接上gbuffer，防止出现三个gbuffer都连接到surface了，然后一个gbuffer长时间没用释放了，而surface的connect_texture还有值，导致后续绘制时新生成的gbuffer无法连接到surface
     int has_connected_fbo;

     GLsync data_sync;
     GLsync delete_sync;
     uint64_t gbuffer_id;

     int remain_life_time;
     int is_dying;
     int is_using;
     int need_reverse;


     int format;
     int pixel_type;
     int internal_format;
     int row_byte_len;
     int depth_internal_format;
     int stencil_internal_format;


     int hal_format;

     int stride;
     int width;
     int height;

     int usage_type;

} Graphic_Buffer;


typedef struct Window_Buffer
{
     int type;

     GLFWHints window_hints;
     
     eglConfig *config;

     EGLSurface guest_surface;

     Graphic_Buffer *gbuffer;
     uint64_t gbuffer_id;

     // //可以交换的事件
     // void *swap_event;
     

     //表示窗口的宽和高，只有Pbuffer适用
     int width;
     int height;


     int is_current;
     int need_destroy;

     // int last_frame_num;
     int swap_interval;

     int I_am_composer;


     int64_t swap_time[20];
     int64_t swap_time_all;
     int swap_loc;
     int swap_time_cnt;

     int64_t frame_start_time;


     gint64 last_calc_time;
     int now_screen_hz;

     int sampler_num;
     int format;
     int pixel_type;
     int internal_format;
     // int row_byte_len;
     int depth_internal_format;
     int stencil_internal_format;
     
     GLuint now_fbo_loc;
     GLuint data_fbo[3];
     GLuint sampler_fbo[3];
     GLuint connect_texture[3];

     // int64_t temp_time;

} Window_Buffer;


Graphic_Buffer *create_gbuffer_with_context(int width, int height, int hal_format, void *thread_context, EGLContext ctx, uint64_t gbuffer_id);

Graphic_Buffer *create_gbuffer_from_hal(int width, int height, int hal_format, Window_Buffer *surface, uint64_t gbuffer_id);

Graphic_Buffer *create_gbuffer(int width, int height, int sampler_num, 
     int format,
     int pixel_type,
     int internal_format,
     int depth_internal_format,
     int stencil_internal_format,
     uint64_t gbuffer_id);

Graphic_Buffer *create_gbuffer_from_surface(Window_Buffer *surface);

void connect_gbuffer_to_surface(Graphic_Buffer *gbuffer, Window_Buffer *surface);

void reverse_gbuffer(Graphic_Buffer *gbuffer);

void egl_surface_swap_buffer(void *render_context, Window_Buffer *surface,uint64_t gbuffer_id, int width, int height, int hal_format);

int render_surface_destroy(Window_Buffer *surface);

void destroy_gbuffer(Graphic_Buffer *gbuffer);

void render_surface_init(Window_Buffer *surface);

void render_surface_uninit(Window_Buffer *surface);


// void destroy_real_image(EGL_Image *real_image);

void d_eglIamComposer(void *context, EGLSurface surface, unsigned int pid);

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


EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target,
                                  EGLClientBuffer buffer, const EGLint *attrib_list,EGLImage guest_image);

// void d_eglRemainImage(void *context, EGLImage image);

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);


int egl_surface_init(Window_Buffer *d_buffer, void *now_window, int need_draw);

// void init_image_texture(EGL_Image *image);

#endif