#include "egl_display.h"

#ifdef _WIN32

#include <windows.h>
#include "wglext.h"

#define LOAD_WGLPROC_CHECK(PROC_NAME) \
    wgl_display->wgl_ext->PROC_NAME = GetProcAddress(wgl_display->wgl_ext->instance, #PROC_NAME); \
    if (!(wgl_display->wgl_ext->PROC_NAME)) { express_printf("Fail to load %s\n", #PROC_NAME); }

typedef PROC (WINAPI * EXWGL_GetProcAddress_PROC)(LPCSTR);
typedef HGLRC (WINAPI * EXWGL_CreateContext_PROC)(HDC);
typedef BOOL (WINAPI * EXWGL_DeleteContext_PROC)(HGLRC);
typedef HDC (WINAPI * EXWGL_GetCurrentDC_PROC)(void);
typedef HGLRC (WINAPI * EXWGL_GetCurrentContext_PROC)(void);
typedef BOOL (WINAPI * EXWGL_MakeCurrent_PROC)(HDC,HGLRC);
typedef BOOL (WINAPI * EXWGL_ShareLists_PROC)(HGLRC,HGLRC);
typedef BOOL (WINAPI * EXWGL_GetPixelFormatAttribivARB_PROC)(HDC,int,int,UINT,const int*,int*);

typedef struct {
    HINSTANCE instance;
    EXWGL_GetProcAddress_PROC wglGetProcAddress;
    EXWGL_CreateContext_PROC wglCreateContext;
    EXWGL_DeleteContext_PROC wglDeleteContext;
    EXWGL_GetCurrentDC_PROC wglGetCurrentDC;
    EXWGL_GetCurrentContext_PROC wglGetCurrentContext;
    EXWGL_MakeCurrent_PROC wglMakeCurrent;
    EXWGL_ShareLists_PROC wglShareLists;

    // 下面函数只能通过wglGetProcAddress获得
    EXWGL_GetPixelFormatAttribivARB_PROC GetPixelFormatAttribivARB;
} WGL_Extension;

void init_wgl_extension(Egl_Display* display);
void parse_pixel_format(Egl_Display* display, HDC dummy_ctx, PIXELFORMATDESCRIPTOR* pfd, int id);
HWND create_dummy_window();
LRESULT CALLBACK dummy_window_proc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

typedef struct {
    Egl_Display display;
    WGL_Extension* wgl_ext;
} Egl_Display_WGL;

#endif