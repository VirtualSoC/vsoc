#ifndef EGL_DISPLAY_H
#define EGL_DISPLAY_H

#include "egl_config.h"
#include "wglext.h"
#include "direct-express/express_log.h"
#include <windows.h>

#define RENDERABLE_SUPPORT 0 | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT;
#define PBUFFER_MAX_WIDTH  32767
#define PBUFFER_MAX_HEIGHT 32767
#define PBUFFER_MAX_PIXELS (PBUFFER_MAX_WIDTH * PBUFFER_MAX_HEIGHT)

#define RETURN_IF_FALSE(a) \
        do { if (!(a)) {express_printf("%s fails to pass FALSE check at line %d", __FUNCTION__, __LINE__); return;} } while (0)

typedef BOOL (WINAPI * EXWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC,int,int,UINT,const int*,int*);

typedef struct {
    HINSTANCE instance;
    EXWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;
} WGL_Extension;

typedef struct {
    GHashTable *egl_config_set;
    WGL_Extension* wgl_ext;
} Egl_Display;

void init_configs(Egl_Display* display);
void init_wgl_extension(Egl_Display* display);

void parse_pixel_format(Egl_Display* display, HDC dummy_ctx, PIXELFORMATDESCRIPTOR* pfd, int id);

HDC create_dummy_window();
LRESULT CALLBACK dummy_window_proc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

#endif