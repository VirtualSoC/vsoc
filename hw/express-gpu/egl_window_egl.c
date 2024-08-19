#include "hw/express-gpu/egl_window.h"
#include <glib.h>
#include <dlfcn.h>
#include <stdio.h>

PFN_eglMakeCurrent eglMakeCurrent;
PFN_eglCreateContext eglCreateContext;
PFN_eglCreatePbufferSurface eglCreatePbufferSurface;
PFN_eglDestroySurface eglDestroySurface;
PFN_eglDestroyContext eglDestroyContext; 
PFN_eglChooseConfig eglChooseConfig;


static void *egl_dll_moudle = NULL;

static EGLDisplay main_window_display;
static EGLContext main_window_context;

static GHashTable *context_pbuffer_map;

static EGLint static_config;


static int static_context_attribs[]={
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_CONTEXT_MINOR_VERSION, 3,
    EGL_NONE
};

static int static_pbuffer_attribs[]={
    EGL_WIDTH, 1,
    EGL_HEIGHT,1,
    EGL_NONE
};

EGLproc load_egl_fun(const char *name);

EGLproc load_egl_fun(const char *name)
{
    if (egl_dll_moudle == NULL)
    {
        egl_dll_moudle = dlopen("libEGL.dylib", RTLD_LAZY | RTLD_LOCAL);
        if (egl_dll_moudle == NULL)
        {
            printf("error! no opengl dll!\n");
        }
    }

    EGLproc ret = dlsym(egl_dll_moudle, name);
    if(ret == NULL)
    {
        printf("load egl fun %s failed!\n",name);
    }
    return ret;
}

#define LOAD_EGL_FUN(name)                            \
    name = (PFN_##name)load_egl_fun(#name "ARB");     \
    if (name == NULL)                                 \
        name = (PFN_##name)load_egl_fun(#name);       \
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

    
    context_pbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    main_window_display = (EGLDisplay)dpy;
    main_window_context = (EGLContext)father_context;

    EGLint attrib_list={
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLint num_configs=0;
    if(!eglChooseConfig(main_window_display, attrib_list,&static_config,1, &num_configs))
    {
        printf("choose config error!!!");
    }
}

void *egl_createContext()
{
    EGLContext context = eglCreateContext(main_window_display, static_config, main_window_context, static_context_attribs);
    EGLSurface pbuffer = eglCreatePbufferSurface(main_window_display, static_config, static_pbuffer_attribs);

    if (context != EGL_NO_CONTEXT && pbuffer != EGL_NO_SURFACE)
    {
        g_hash_table_insert(context_pbuffer_map, (gpointer)context, pbuffer);
    }
    else
    {
        eglDestroyContext(main_window_display, context);
        eglDestroySurface(main_window_display, pbuffer);
        printf("error! create context null!\n");
    }
    return context;
}

void egl_makeCurrent(void *context)
{
    if (context != NULL)
    {
        EGLSurface pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);
        eglMakeCurrent(main_window_display, pbuffer, pbuffer, context);
    }
    else
    {
        eglMakeCurrent(main_window_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
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