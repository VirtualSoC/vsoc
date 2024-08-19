#include <glib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>

#include "hw/express-gpu/egl_define.h"
#include "hw/express-gpu/egl_window.h"
#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/egl_define.h"
#include "hw/express-gpu/egl_window.h"
#include "hw/teleport-express/express_log.h"
#include <Cocoa/Cocoa.h>

static void *egl_dll_moudle = NULL;

static NSOpenGLContext *main_window_context;

static id static_pixelFormat;

void egl_init(void *dpy, void *father_context)
{   
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

    express_printf("create context from NSGL2!\n");

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
        [contextNS makeCurrentContext];
    }
    else
    {
        [NSOpenGLContext clearCurrentContext];
    }
    return EGL_TRUE;
}

void egl_destroyContext(void *context)
{

    if (context != NULL)
    {
        id contextNS = (id) context;
        [contextNS release];
    }
}
