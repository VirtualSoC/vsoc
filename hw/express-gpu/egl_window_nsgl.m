#include "hw/express-gpu/egl_window.h"
#include <glib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>

#include "hw/express-gpu/egl_define.h"
#include "hw/express-gpu/egl_window.h"
#include "hw/teleport-express/express_log.h"
#include <Cocoa/Cocoa.h>
//#include <Appkit/NSOpenGL.h>


static void *egl_dll_moudle = NULL;

// static EGLDisplay main_window_display;
static NSOpenGLContext *main_window_context;

// static GHashTable *context_pbuffer_map;

static id static_pixelFormat;

void egl_init(void *dpy, void *father_context)
{   
    // context_pbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    main_window_context = (id)father_context;

    NSOpenGLPixelFormatAttribute attribs[]={
        NSOpenGLPFAAccelerated, NSOpenGLPFAClosestPolicy,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        0
    };


    static_pixelFormat =[[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];

    if(static_pixelFormat == nil)
    {
        printf("choose config error!!!");
    }
}

void *egl_createContext()
{
    id context = [[NSOpenGLContext alloc] initWithFormat:static_pixelFormat shareContext:main_window_context];
    
    printf("create context from NSGL\n");
    if (context != nil)
    {
        // g_hash_table_insert(context_pbuffer_map, (gpointer)context, pbuffer);
    }
    else
    {
        printf("error! create context null!\n");
    }
    return (void *)context;
}

int egl_makeCurrent(void *context)
{
    id contextNS = (id) context;
    if (contextNS != nil)
    {
        // EGLSurface pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);
        
        [contextNS makeCurrentContext];
        // eglMakeCurrent(main_window_display, pbuffer, pbuffer, context);
    }
    else
    {
        // eglMakeCurrent(main_window_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        [NSOpenGLContext clearCurrentContext];
    }
    return EGL_TRUE;
}

void egl_destroyContext(void *context)
{

    if (context != NULL)
    {
        // gint64 t = g_get_real_time();
        // printf("destroy ");
        // EGLContext pbuffer = g_hash_table_lookup(context_pbuffer_map, (gpointer)context);

        // eglDestroyContext(main_window_display, context);
        // eglDestroySurface(main_window_display, pbuffer);

        // g_hash_table_remove(context_pbuffer_map, (gpointer)context);
        id contextNS = (id) context;
        [contextNS release];


        // printf("destroy window %lld\n", g_get_real_time() - t);
    }
}
