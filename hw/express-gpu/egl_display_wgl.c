#ifdef _WIN32
// #define STD_DEBUG_LOG
#include "hw/express-gpu/egl_display_wgl.h"
#include "hw/teleport-express/express_log.h"
#include <wingdi.h>

Egl_Display_WGL default_wgl_display;

// 加载WGL函数
#define LOAD_WGL_FUNCS(return_type, func_name, param)                                                                          \
    wgl_display->wgl_ext->func_name = (return_type(WINAPI *) param)GetProcAddress(wgl_display->wgl_ext->instance, #func_name); \
    if (!(wgl_display->wgl_ext->func_name))                                                                                    \
    {                                                                                                                          \
        LOGE("Fail to load %s", #func_name);                                                                                 \
    }

// 加载WGL Extension函数
#define LOAD_WGL_EXT_FUNCS(return_type, func_name, param)                                                               \
    wgl_display->wgl_ext->func_name = (return_type(WINAPI *) param)wgl_display->wgl_ext->wglGetProcAddress(#func_name); \
    if (!(wgl_display->wgl_ext->func_name))                                                                             \
    {                                                                                                                   \
        LOGE("Fail to load %s", #func_name);                                                                          \
    }

#endif