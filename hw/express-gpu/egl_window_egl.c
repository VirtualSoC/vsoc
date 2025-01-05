#include "hw/teleport-express/express_log.h"
#include "hw/express-gpu/egl_window.h"
#include <glib.h>
#include <dlfcn.h>
#include <stdio.h>

// EGL function pointer typedefs
typedef void (*EGLproc)(void);

typedef int EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;
typedef void *EGLConfig;
typedef void *EGLContext;
typedef void *EGLDisplay;
typedef void *EGLSurface;

#define EGL_NONE 0x3038 /* Attrib list terminator */

#define EGL_CONTEXT_MAJOR_VERSION 0x3098
#define EGL_CONTEXT_MINOR_VERSION 0x30FB
#define EGL_WIDTH 0x3057
#define EGL_HEIGHT 0x3056

#define EGL_ALPHA_SIZE 0x3021
#define EGL_BLUE_SIZE 0x3022
#define EGL_GREEN_SIZE 0x3023
#define EGL_RED_SIZE 0x3024
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026

#if defined(__cplusplus)
#define EGL_CAST(type, value) (static_cast<type>(value))
#else
#define EGL_CAST(type, value) ((type)(value))
#endif

#define EGL_NO_CONTEXT EGL_CAST(EGLContext, 0)
#define EGL_NO_DISPLAY EGL_CAST(EGLDisplay, 0)
#define EGL_NO_SURFACE EGL_CAST(EGLSurface, 0)

typedef EGLBoolean (*PFN_eglGetConfigAttrib)(EGLDisplay, EGLConfig, EGLint, EGLint *);
typedef EGLBoolean (*PFN_eglGetConfigs)(EGLDisplay, EGLConfig *, EGLint, EGLint *);
typedef EGLint (*PFN_eglGetError)(void);
typedef EGLBoolean (*PFN_eglInitialize)(EGLDisplay, EGLint *, EGLint *);
typedef EGLBoolean (*PFN_eglTerminate)(EGLDisplay);
typedef EGLBoolean (*PFN_eglBindAPI)(EGLenum);

typedef EGLContext (*PFN_eglCreateContext)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);

typedef EGLBoolean (*PFN_eglChooseConfig)(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
typedef EGLSurface (*PFN_eglCreatePbufferSurface)(EGLDisplay, EGLConfig, const EGLint *);
typedef EGLBoolean (*PFN_eglDestroySurface)(EGLDisplay, EGLSurface);
typedef EGLBoolean (*PFN_eglDestroyContext)(EGLDisplay, EGLContext);
typedef EGLBoolean (*PFN_eglMakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);
typedef EGLBoolean (*PFN_eglSwapInterval)(EGLDisplay, EGLint);

PFN_eglMakeCurrent eglMakeCurrent;
PFN_eglCreateContext eglCreateContext;
PFN_eglCreatePbufferSurface eglCreatePbufferSurface;
PFN_eglDestroySurface eglDestroySurface;
PFN_eglDestroyContext eglDestroyContext; 
PFN_eglChooseConfig eglChooseConfig;
PFN_eglGetError eglGetError;

static EGLDisplay main_window_display;
static EGLContext main_window_context;

static GHashTable *context_pbuffer_map;

static EGLConfig static_config;

static int static_context_attribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 4,
#ifdef __APPLE__
    EGL_CONTEXT_MINOR_VERSION, 1,
#else
    EGL_CONTEXT_MINOR_VERSION, 6,
#endif
    EGL_NONE
};

static int static_pbuffer_attribs[] = {
    EGL_WIDTH, 1,
    EGL_HEIGHT, 1,
    EGL_NONE
};

static EGLproc load_egl_fun(const char *name)
{
    static void *egl_module = NULL;
    if (egl_module == NULL)
    {
        egl_module = dlopen("libEGL.so", RTLD_LAZY);
        if (egl_module == NULL)
        {
            LOGE("error! libEGL not found");
        }
    }

    EGLproc ret = dlsym(egl_module, name);
    if (ret == NULL)
    {
        LOGD("load egl fun %s failed", name);
    }
    return ret;
}

#define LOAD_EGL_FUN(name)                            \
    name = (PFN_##name)load_egl_fun(#name);           \
    if (name == NULL)                                 \
        name = (PFN_##name)load_egl_fun(#name "ARB"); \
    if (name == NULL)                                 \
        name = (PFN_##name)load_egl_fun(#name "KHR"); \
    if (name == NULL)                                 \
        name = (PFN_##name)load_egl_fun(#name "EXT"); \
    if (name == NULL)                                 \
        printf("cannot find %s", #name);

void egl_init(void *dpy, void *father_context)
{
    LOAD_EGL_FUN(eglMakeCurrent);
    LOAD_EGL_FUN(eglCreateContext);
    LOAD_EGL_FUN(eglCreatePbufferSurface);
    LOAD_EGL_FUN(eglDestroySurface);
    LOAD_EGL_FUN(eglDestroyContext);
    LOAD_EGL_FUN(eglChooseConfig);
    LOAD_EGL_FUN(eglGetError);

    context_pbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    main_window_display = (EGLDisplay)dpy;
    main_window_context = (EGLContext)father_context;

    EGLint attrib_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLint num_configs = 0;
    if (!eglChooseConfig(main_window_display, attrib_list, &static_config, 1, &num_configs) || num_configs == 0)
    {
        LOGE("choose config error %x display %p context %p", eglGetError(), main_window_display, main_window_context);
    }
}

void *egl_createContext(int context_flags)
{
    EGLContext context = eglCreateContext(main_window_display, static_config, main_window_context, static_context_attribs);
    if (context == EGL_NO_CONTEXT)
    {
        LOGE("error! cannot create context error %x", eglGetError());
    }

    EGLSurface pbuffer = eglCreatePbufferSurface(main_window_display, static_config, static_pbuffer_attribs);

    if (context != EGL_NO_CONTEXT && pbuffer != EGL_NO_SURFACE)
    {
        g_hash_table_insert(context_pbuffer_map, (gpointer)context, pbuffer);
    }
    else
    {
        eglDestroyContext(main_window_display, context);
        eglDestroySurface(main_window_display, pbuffer);
        LOGE("error! cannot create context error %x", eglGetError());
    }
    return context;
}

int egl_makeCurrent(void *context)
{
    if (context != NULL)
    {
        EGLSurface pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);
        return eglMakeCurrent(main_window_display, pbuffer, pbuffer, context);
    }
    else
    {
        return eglMakeCurrent(main_window_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
}

void egl_destroyContext(void *context)
{
    if (context != NULL)
    {
        EGLContext pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);

        eglDestroyContext(main_window_display, context);
        eglDestroySurface(main_window_display, pbuffer);

        g_hash_table_remove(context_pbuffer_map, (gpointer)context);
    }
}