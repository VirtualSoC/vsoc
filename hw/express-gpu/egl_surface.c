// #define STD_DEBUG_LOG
#include "express-gpu/egl_surface.h"






EGLBoolean d_eglSwapBuffers_special(Render_Thread_Context *context,EGLDisplay dpy, EGLSurface surface){
    express_printf("eglSwapBuffers inovke!\n");

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    Double_Buffer *egl_context = &(render_context->render_double_buffer);

    //渲染到texture专用
    egl_swap_buffer(egl_context);
    real_egl_swap_buffer(render_context);


    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(1,0,0,1);
    // glClear(GL_COLOR_BUFFER_BIT);

    // glfwSwapBuffers(context->window);
    // glUseProgram(dpy);
    // GLfloat vVertices[] = {0.0f, 0.5f, 0.0f,  -0.5f, -0.5f,
    //                            0.0f, 0.5f, -0.5f, 0.0f};
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    // glEnableVertexAttribArray(0);

    // glDrawArrays(GL_TRIANGLES, 0, 3);

    //屏幕分离调试专用
    // glfwSwapBuffers(render_context->render_double_buffer.window);

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

    //屏幕分离调试专用
    //ShowWindow(render_context->render_double_buffer.window, TRUE);
    return GL_TRUE;

}



EGLBoolean d_eglTerminate_special(Render_Thread_Context *context,EGLDisplay dpy){
    // Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    // Double_Buffer *egl_context = &(render_context->render_double_buffer);

    express_printf("eglTerminate invoke!\n");

    render_context_destroy((Thread_Context *)context);
    return GL_TRUE;

    
}