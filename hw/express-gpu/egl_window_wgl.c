#ifdef _WIN32
// #define STD_DEBUG_LOG

#include <glib.h>
#include <stdio.h>
#include <stdint.h>

#include "hw/express-gpu/egl_define.h"
#include "hw/express-gpu/egl_window.h"
#include "hw/teleport-express/express_log.h"

PFN_wglSwapInterval wglSwapInterval;
PFN_wglCreateContextAttribs wglCreateContextAttribs;

PFN_wglCreatePbuffer wglCreatePbuffer;
PFN_wglGetPbufferDC wglGetPbufferDC;
PFN_wglReleasePbufferDC wglReleasePbufferDC;
PFN_wglDestroyPbuffer wglDestroyPbuffer;

PFN_wglGetPixelFormatAttribiv wglGetPixelFormatAttribiv;
PFN_wglGetPixelFormatAttribfv wglGetPixelFormatAttribfv;
PFN_wglChoosePixelFormat wglChoosePixelFormat;

static HMODULE opengl_dll_moudle = NULL;

static HDC main_window_hdc;
static HGLRC main_window_context;

static GHashTable *context_pbuffer_map;
static GHashTable *context_dc_map;

static int static_pixel_format;

static int static_pbuffer_attribs[3];

extern bool express_gpu_gl_debug_enable;

WGLproc load_wgl_fun(const char *name);

WGLproc load_wgl_fun(const char *name)
{
    if (opengl_dll_moudle == NULL)
    {
        opengl_dll_moudle = LoadLibraryA("opengl32.dll");
        if (opengl_dll_moudle == NULL)
        {
            LOGE("error! no opengl dll!");
        }
    }

    WGLproc ret = (WGLproc)wglGetProcAddress(name);
    if (ret == NULL)
    {
        ret = (WGLproc)GetProcAddress(opengl_dll_moudle, name);
    }
    return ret;
}

#define LOAD_EGL_FUN(name)                        \
    name = (PFN_##name)load_wgl_fun(#name "ARB"); \
    if (name == NULL)                             \
        name = (PFN_##name)load_wgl_fun(#name);   \
    if (name == NULL)                             \
        name = (PFN_##name)load_wgl_fun(#name "EXT");

void egl_init(void *dpy, void *father_context)
{
    LOAD_EGL_FUN(wglSwapInterval);
    LOAD_EGL_FUN(wglCreateContextAttribs);
    LOAD_EGL_FUN(wglCreatePbuffer);
    LOAD_EGL_FUN(wglGetPbufferDC);
    LOAD_EGL_FUN(wglReleasePbufferDC);
    LOAD_EGL_FUN(wglDestroyPbuffer);
    LOAD_EGL_FUN(wglGetPixelFormatAttribiv);
    LOAD_EGL_FUN(wglGetPixelFormatAttribfv);
    LOAD_EGL_FUN(wglChoosePixelFormat);

    if (wglCreateContextAttribs == NULL)
    {
        LOGI("note! wglCreateContextAttribs is NULL! ");
    }

    context_dc_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    context_pbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    main_window_hdc = (HDC)dpy;
    main_window_context = (HGLRC)father_context;

    int iattribs[12];
    iattribs[0] = WGL_DRAW_TO_PBUFFER_ARB;
    iattribs[1] = 1;
    iattribs[2] = WGL_RED_BITS_ARB;
    iattribs[3] = 8;
    iattribs[4] = WGL_GREEN_BITS_ARB;
    iattribs[5] = 8;
    iattribs[6] = WGL_BLUE_BITS_ARB;
    iattribs[7] = 8;
    iattribs[8] = WGL_ALPHA_BITS_ARB;
    iattribs[9] = 8;
    iattribs[10] = 0;
    iattribs[11] = 0;

    float fattribs[2];
    fattribs[0] = 0;
    fattribs[1] = 0;

    int pFormats[1];
    unsigned int nFromat;
    nFromat = 0;
    if (!wglChoosePixelFormat(main_window_hdc, iattribs, fattribs, 1, pFormats, &nFromat) || nFromat == 0)
    {
        LOGE("error! cannot choose pixel format!");
        exit(-1);
    }

    static_pixel_format = pFormats[0];

    static_pbuffer_attribs[0] = WGL_PBUFFER_LARGEST_ARB;
    static_pbuffer_attribs[1] = 1;
    static_pbuffer_attribs[2] = 0;
}

void *egl_createContext(int context_flags)
{
    HPBUFFERARB pbuffer = wglCreatePbuffer(main_window_hdc, static_pixel_format, 1, 1, static_pbuffer_attribs);
    HDC pbuffer_dc = wglGetPbufferDC(pbuffer);
    HGLRC context = NULL;
    
    // 构造wgl的attrib_list
    // 注意，wgl的context flag和gl的不完全一致，所以要映射一下
    int attrib_list[10];
    int *ptr = attrib_list;

    int wgl_context_flags = 0;
    if ((context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) || express_gpu_gl_debug_enable)
    {
        wgl_context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
    }

    // robust access对guest来说是透明的，所以默认开启
    wgl_context_flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
    *ptr = WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;
    *(ptr + 1) = WGL_LOSE_CONTEXT_ON_RESET_ARB;
    ptr += 2;
    *ptr = WGL_CONTEXT_FLAGS_ARB;
    *(ptr + 1) = wgl_context_flags;
    ptr += 2;

    *ptr = 0; // end of attrib list

    if (wglCreateContextAttribs == NULL)
    {
        if (context_flags != 0) 
        {
            LOGW("warning! context flag %x not supported by wgl, ignoring.", context_flags);
        }
        context = wglCreateContext(pbuffer_dc);
        bool succ = wglShareLists(main_window_context, context);
        if (!succ) {
            LOGE("wglShareLists failed with error %x", (unsigned int)GetLastError());
        }
    }
    else
    {
        context = wglCreateContextAttribs(pbuffer_dc, main_window_context, attrib_list);
    }
    if (context != NULL)
    {
        g_hash_table_insert(context_dc_map, (gpointer)context, pbuffer_dc);
        g_hash_table_insert(context_pbuffer_map, (gpointer)context, pbuffer);
    }
    else
    {
        wglReleasePbufferDC(pbuffer, pbuffer_dc);
        wglDestroyPbuffer(pbuffer);
        LOGE("wgl create context failed with error %x", (unsigned int)GetLastError());
    }
    return context;
}

int egl_makeCurrent(void *context)
{
    if (context != NULL)
    {
        HDC pbuffer_dc = g_hash_table_lookup(context_dc_map, (gpointer)context);
        int ret = wglMakeCurrent(pbuffer_dc, (HGLRC)context);
        if (ret == 0)
        {
            int last_error = (int)GetLastError();
            LOGE("error! makecurrent window %llx failed error %d", (uint64_t)context, last_error);
            return -last_error;
        }
    }
    else
    {
        BOOL succ = wglMakeCurrent(NULL, NULL);
        if (succ != TRUE) return -1;
    }
    return EGL_TRUE;
}

void egl_destroyContext(void *context)
{

    if (context != NULL)
    {
        gint64 t = g_get_real_time();
        // LOGI("destroy ");
        LOGI("destroy window %llx", (uint64_t)context);
        HDC pbuffer_dc = g_hash_table_lookup(context_dc_map, (gpointer)context);
        HPBUFFERARB pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);

        wglDeleteContext((HGLRC)context);
        wglReleasePbufferDC(pbuffer, pbuffer_dc);
        wglDestroyPbuffer(pbuffer);

        g_hash_table_remove(context_dc_map, (gpointer)context);
        g_hash_table_remove(context_pbuffer_map, (gpointer)context);

        express_printf("destroy window ok %lld\n", g_get_real_time() - t);
    }
}
#endif