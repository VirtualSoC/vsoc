#include "express-gpu/egl_surface.h"

EGLBoolean d_eglSwapBuffers_special(Render_Thread_Context *context, EGLDisplay dpy, EGLSurface surface)
{
    printf("eglSwapBuffers inovke!\n");

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *egl_context = &(render_context->render_double_buffer);

    egl_swap_buffer(egl_context);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(1,0,0,1);
    // glClear(GL_COLOR_BUFFER_BIT);

    // glfwSwapBuffers(context->window);
    real_egl_swap_buffer(render_context);
    return GL_TRUE;
}

EGLBoolean d_eglMakeCurrent_special(Render_Thread_Context *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    printf("eglMakeCurrent invoke!\n");
    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *egl_context = &(render_context->render_double_buffer);
    render_windows_create(render_context);
    egl_context_make_current(egl_context);
    return GL_TRUE;
}

EGLBoolean d_eglTerminate_special(Render_Thread_Context *context, EGLDisplay dpy)
{
    // Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    // Double_Buffer *egl_context = &(render_context->render_double_buffer);

    printf("eglTerminate invoke!\n");

    render_context_destroy((Thread_Context *)context);
    return GL_TRUE;
}

void d_getEGLConfigParam_special(Render_Thread_Context *context, int *num_configs, int *num_config_attrs)
{
    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Egl_Display* display = &(context->egl_display);
    if (!display->is_init) {
        init_display(display);
    }
    *num_configs = g_hash_table_size(display->egl_config_set);
    *num_config_attrs = 15;
}