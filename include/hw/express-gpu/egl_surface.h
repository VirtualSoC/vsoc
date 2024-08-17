#ifndef EGL_SURFACE_H
#define EGL_SURFACE_H

#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/express-gpu/egl_config.h"

#include "hw/express-gpu/express_display.h"

#include "glad/glad.h"
#include "hw/express-gpu/GLFW/glfw3.h"


#define P_SURFACE 1
#define WINDOW_SURFACE 2


typedef enum {
     EXPRESS_PIXEL_RGBA8888 = 0,
     EXPRESS_PIXEL_RGBX8888,
     EXPRESS_PIXEL_RGB888,
     EXPRESS_PIXEL_RGB565,
     EXPRESS_PIXEL_RGBA5551,
     EXPRESS_PIXEL_RGBX5551,
     EXPRESS_PIXEL_RGBA4444,
     EXPRESS_PIXEL_RGBX4444,
     EXPRESS_PIXEL_BGRA8888,
     EXPRESS_PIXEL_BGRX8888,
     EXPRESS_PIXEL_BGR565,
     EXPRESS_PIXEL_RGBA1010102,
     EXPRESS_PIXEL_R8,
	EXPRESS_PIXEL_YUV420888, 
     // todo: add yuv

} EXPRESS_Pixel_Format;

#define MAX_WINDOW_LIFE_TIME (60*5)
#define MAX_BITMAP_LIFE_TIME (60*15)
#define MAX_COMPOSER_LIFE_TIME (MAX_WINDOW_LIFE_TIME*10)


typedef enum ExpressMemType {
    // uninitialized
    EXPRESS_MEM_TYPE_UNKNOWN = 0x00,

    // host memory mask
    EXPRESS_MEM_TYPE_HOST_MASK = 0x0f,

    // host opaque structure type
    EXPRESS_MEM_TYPE_HOST_OPAQUE = 0x01,

    // gbuffer texture, should be casted to Hardware_Buffer*
    EXPRESS_MEM_TYPE_TEXTURE = 0x02,

    // host CPU memory type, raw memory addr
    EXPRESS_MEM_TYPE_HOST_MEM = 0x03,

    // gbuffer host memory type, should be casted to Hardware_Buffer*
    EXPRESS_MEM_TYPE_GBUFFER_HOST_MEM = 0x04,

    // cuda memory type, deviceptr
    EXPRESS_MEM_TYPE_CUDA = 0x05,

    // guest memory mask
    EXPRESS_MEM_TYPE_GUEST_MASK = 0xf0,

    // guest opaque structure type
    EXPRESS_MEM_TYPE_GUEST_OPAQUE = 0x10,

    // guest memory type, should be casted to Guest_Mem*
    EXPRESS_MEM_TYPE_GUEST_MEM = 0x20,
} ExpressMemType;

typedef struct Hardware_Buffer{

     int is_writing;

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

     int pixel_size;
     int usage;
     int size;

     Guest_Mem *guest_data; // storage for EXPRESS_MEM_TYPE_GUEST_MEM
     void *host_data; // storage for EXPRESS_MEM_TYPE_GBUFFER_HOST_MEM

     ExpressMemType last_phy_dev;
     int last_phy_usage;
     ExpressMemType pref_phy_dev;

     int last_virt_dev;
     int last_virt_usage;
     int last_virt_time;

} Hardware_Buffer;


typedef struct Window_Buffer
{
     int type;

     GLFWHints window_hints;
     
     eglConfig *config;

     EGLSurface guest_surface;

     Hardware_Buffer *gbuffer;
     uint64_t gbuffer_id;

     //表示窗口的宽和高，只有Pbuffer适用
     int width;
     int height;

     int is_current;
     int need_destroy;

     int swap_interval;

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
} Window_Buffer;


Hardware_Buffer *create_gbuffer_with_context(int width, int height, int hal_format, void *thread_context, EGLContext ctx, uint64_t gbuffer_id);

Hardware_Buffer *create_gbuffer_from_hal(int width, int height, int hal_format, Window_Buffer *surface, uint64_t gbuffer_id);

Hardware_Buffer *create_gbuffer(int width, int height, int sampler_num, 
     int format,
     int pixel_type,
     int internal_format,
     int depth_internal_format,
     int stencil_internal_format,
     uint64_t gbuffer_id);

Hardware_Buffer *create_gbuffer_from_surface(Window_Buffer *surface);

void connect_gbuffer_to_surface(Hardware_Buffer *gbuffer, Window_Buffer *surface);

void reverse_gbuffer(Hardware_Buffer *gbuffer);

void egl_surface_swap_buffer(void *render_context, Window_Buffer *surface,uint64_t gbuffer_id, int width, int height, int hal_format);

int render_surface_destroy(Window_Buffer *surface);

void destroy_gbuffer(Hardware_Buffer *gbuffer);

void render_surface_init(Window_Buffer *surface);

void render_surface_uninit(Window_Buffer *surface);

void d_eglIamComposer(void *context, EGLSurface surface, unsigned int pid);

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface);

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface);

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface);

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute,EGLint value);


EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target,
                                  EGLClientBuffer buffer, const EGLint *attrib_list,EGLImage guest_image);

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image);


int egl_surface_init(Window_Buffer *d_buffer, void *now_window, int need_draw);

#endif