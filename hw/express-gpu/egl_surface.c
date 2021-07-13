// #define STD_DEBUG_LOG
#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_display.h"

EGLBoolean d_eglSwapBuffers_special(Render_Thread_Context *context, EGLDisplay dpy, EGLSurface surface)
{
    printf("eglSwapBuffers inovke!\n");

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *egl_context = &(render_context->render_double_buffer);
    Opengl_Context *opengl_context=&(render_context->opengl_context);

    #ifdef DEBUG_INDEPEND_WINDOW
        //屏幕分离调试专用
        glfwSwapBuffers(render_context->render_double_buffer.window);
    #else
        //渲染到texture专用
        egl_swap_buffer(egl_context);
        real_egl_swap_buffer(render_context);
        opengl_context->fbo0 = egl_context->fbo_draw;

    #endif



    return GL_TRUE;
}

EGLBoolean d_eglMakeCurrent_special(Render_Thread_Context *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    printf("eglMakeCurrent invoke!\n");
    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *egl_context = &(render_context->render_double_buffer);
    Opengl_Context *opengl_context = &(render_context->opengl_context);
    egl_context->width = dpy;
    egl_context->height = draw;

    render_windows_create(render_context);
    egl_context_make_current(egl_context);
    opengl_context_create(opengl_context);
    
    opengl_context->fbo0 = egl_context->fbo_draw;
    
    #ifdef DEBUG_INDEPEND_WINDOW
        //屏幕分离调试专用
        ShowWindow(render_context->render_double_buffer.window, TRUE); 
    #endif

#ifdef DEBUG_INDEPEND_WINDOW
    //屏幕分离调试专用
    ShowWindow(render_context->render_double_buffer.window, TRUE);
#endif

    return GL_TRUE;
}

EGLBoolean d_eglTerminate_special(Render_Thread_Context *context, EGLDisplay dpy)
{
    // Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    // Double_Buffer *egl_context = &(render_context->render_double_buffer);

    express_printf("eglTerminate invoke!\n");

    render_context_destroy((Thread_Context *)context);
    return GL_TRUE;
}

int d_getEGLConfigParam_special(Render_Thread_Context *context, int *num_configs)
{
    VALIDATE_DISPLAY;
    *num_configs = g_hash_table_size(display->egl_config_set);
    return sizeof(config_attrs) / sizeof(config_attrs[0]);
}

EGLint d_getEGLConfigs_special(Render_Thread_Context *context, EGLint num_attrs, EGLint *attr_list)
{
    VALIDATE_DISPLAY;
    GList *config_list = g_hash_table_get_values(display->egl_config_set);

    int index = 0;
    memcpy(attr_list, config_attrs, NUM_ATTRS * sizeof(unsigned int));
    index++;

    for (GList *it = config_list; it; it = it->next)
    {
        eglConfig *config = (eglConfig *)it->data;
        memcpy(attr_list + index * NUM_ATTRS, config, NUM_ATTRS * sizeof(unsigned int));
        index++;
    }

    return index - 1;
}

EGLBoolean d_getEGLVersion_special(Render_Thread_Context *context, EGLint* ver_major, EGLint* ver_minor) {
    VALIDATE_DISPLAY;

    if (!display->is_init) return EGL_FALSE;

    *ver_major = display->guest_ver_major;
    *ver_minor = display->guest_ver_minor;

    return EGL_TRUE;
}