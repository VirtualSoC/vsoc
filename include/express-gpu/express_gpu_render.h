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



#define WM_USER_PAINT WM_USER + 10
#define WM_USER_CREATE WM_USER + 11
#define WM_USER_SURFACE_DESTROY WM_USER + 12
#define WM_USER_CONTEXT_DESTROY WM_USER + 13




// 是否启用独立窗口进行调试的宏定义
// #define DEBUG_INDEPEND_WINDOW

extern HWND draw_native_window;

extern volatile int native_render_run;


void *native_window_thread(void *opaque);
// void *opengl_ui_thread(void *opaque);

int draw_wait_GSYNC(HANDLE event, int wait_frame_num);

void render_windows_create(Double_Buffer *context);

void set_compose_surface(Double_Buffer *surface);


#endif