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


#define WM_USER_PAINT WM_USER+10
#define WM_USER_CREATE WM_USER+11


extern HWND draw_native_window;

extern volatile int native_render_run;


typedef struct {

    GLFWwindow *window;

    GLuint fbo_draw;
    GLuint fbo_display;

    GLuint fbo_texture_draw;
    GLuint fbo_texture_display;
    
    int display_texture_is_use;
    GLsync dispaly_sync;

    int width;
    int height;
    
    int has_init;

} Double_Buffer;

#define TEXTURE_LOCK(use_texture) while(atomic_cmpxchg(&(use_texture), 0, 1) == 1);
#define TEXTURE_UNLOCK(use_texture) atomic_cmpxchg(&(use_texture), 1, 0)


void egl_swap_buffer(Double_Buffer *double_buffer);

void render_bind_frame_buffer(Double_Buffer *double_buffer);

GLuint get_display_texture(Double_Buffer *double_buffer);

void release_display_texture(Double_Buffer *double_buffer);


void *native_window_thread(void *opaque);
// void *opengl_ui_thread(void *opaque);

int egl_context_make_current(Double_Buffer *d_buffer);

int egl_context_destroy(Double_Buffer *d_buffer);

// void *my_gpu_render_thread(void *opaque);
// void push_to_render_buf(MYGPU_Opengl_Call *call);
// MYGPU_Opengl_Call *pop_from_render_buf(void);
// void opengl_invoke(MYGPU_Opengl_Call *call);

// MYGPU_Opengl_Call *pack_call_from_queue(VirtQueue *vq);



#endif