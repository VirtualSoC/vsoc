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



#define ATOMIC_LOCK(s)                      \
     while (atomic_cmpxchg(&(s), 0, 1) == 1) \
          printf("lock on %s ",#s);
#define ATOMIC_UNLOCK(s) atomic_cmpxchg(&(s), 1, 0)


// #define ATOMIC_SET_USED(s) ATOMIC_LOCK(s)
// #define ATOMIC_SET_UNUSED(s) ATOMIC_UNLOCK(s)

#define ATOMIC_SET_USED(s) (atomic_cmpxchg(&(s), 0, 1))
#define ATOMIC_SET_UNUSED(s) (atomic_cmpxchg(&(s), 1, 0))


//是否启用opengl调试模式的宏定义
// #define ENABLE_OPENGL_DEBUG


// 是否启用独立窗口进行调试的宏定义
// #define DEBUG_INDEPEND_WINDOW


typedef struct Main_window_Event{
     int event_code;
     void *data;
} Main_window_Event;


// extern HWND draw_native_window;

extern GAsyncQueue *main_window_event_queue;

extern volatile int native_render_run;


void *native_window_thread(void *opaque);
// void *opengl_ui_thread(void *opaque);

int draw_wait_GSYNC(void *event, int wait_frame_num);

// void render_windows_create(Window_Buffer *context);

void set_compose_surface(Window_Buffer *origin_surface,Window_Buffer *new_surface);

GLuint acquire_texture_from_surface(Window_Buffer *surface);

void release_texture_from_surface(Window_Buffer *surface);

GLuint acquire_texture_from_image(EGL_Image *image);

void init_image_texture(EGL_Image *image);

void init_image_fbo(EGL_Image *image, int need_reverse);

void release_texture_from_image(EGL_Image *image);


Window_Buffer *get_surface_from_gbuffer_id(uint64_t gbuffer_id);

void set_surface_gbuffer_id(Window_Buffer *surface, uint64_t gbuffer_id);

EGL_Image *get_image_from_gbuffer_id(uint64_t gbuffer_id);

void set_image_gbuffer_id(EGL_Image *origin_image,EGL_Image *now_image, uint64_t gbuffer_id);

void send_message_to_main_window(int message_code, void *data);

// bool should_give_up_gpu();

#endif