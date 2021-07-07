// #define STD_DEBUG_LOG
#include "express-gpu/egl_surface.h"






EGLBoolean d_eglSwapBuffers_special(Render_Thread_Context *context,EGLDisplay dpy, EGLSurface surface){
    express_printf("eglSwapBuffers inovke!\n");

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

EGLBoolean d_eglMakeCurrent_special(Render_Thread_Context *context,EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx){
    express_printf("eglMakeCurrent invoke!\n");
    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *egl_context = &(render_context->render_double_buffer);
    Opengl_Context *opengl_context=&(render_context->opengl_context);
    egl_context->width=dpy;
    egl_context->height=draw;

    render_windows_create(render_context);
    egl_context_make_current(egl_context);
    opengl_context_create(opengl_context);
    
    opengl_context->fbo0 = egl_context->fbo_draw;
    
    #ifdef DEBUG_INDEPEND_WINDOW
        //屏幕分离调试专用
        ShowWindow(render_context->render_double_buffer.window, TRUE); 
    #endif

    return GL_TRUE;

}



EGLBoolean d_eglTerminate_special(Render_Thread_Context *context,EGLDisplay dpy){
    // Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    // Double_Buffer *egl_context = &(render_context->render_double_buffer);

    express_printf("eglTerminate invoke!\n");

    render_context_destroy((Thread_Context *)context);
    return GL_TRUE;

    
}