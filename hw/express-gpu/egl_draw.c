#define STD_DEBUG_LOG

#include "express-gpu/egl_draw.h"
#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Double_Buffer *real_surface_draw = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(draw));
    Double_Buffer *real_surface_read = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(read));

    Opengl_Context *real_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GINT_TO_POINTER(ctx));

    //原来current的surface可能要destroy

    if (thread_context->render_double_buffer_draw != NULL &&
        thread_context->render_double_buffer_draw != real_surface_draw)
    {
        if (thread_context->render_double_buffer_draw->I_am_composer)
        {
            set_compose_surface(NULL);
        }

        if (thread_context->render_double_buffer_draw->need_destroy)
        {
            PostMessage(draw_native_window, WM_USER_SURFACE_DESTROY, 0, (LPARAM)(thread_context->render_double_buffer_draw));
        }
        else
        {
            thread_context->render_double_buffer_draw->is_current = 0;
        }
    }

    //有可能draw和read可能一样，就不能删除两次
    if (thread_context->render_double_buffer_draw != thread_context->render_double_buffer_read &&
        thread_context->render_double_buffer_read != NULL &&
        thread_context->render_double_buffer_read != real_surface_read)
    {
        if (thread_context->render_double_buffer_read->need_destroy)
        {
            PostMessage(draw_native_window, WM_USER_SURFACE_DESTROY, 0, (LPARAM)(thread_context->render_double_buffer_read));
        }
        else
        {
            thread_context->render_double_buffer_read->is_current = 0;
        }
    }

    //原来current的context要destroy
    if (thread_context->opengl_context != NULL && thread_context->opengl_context != real_opengl_context)
    {
        if (thread_context->opengl_context->need_destroy)
        {
            PostMessage(draw_native_window, WM_USER_CONTEXT_DESTROY, 0, (LPARAM)(thread_context->opengl_context));
        }
        else
        {
            thread_context->opengl_context->is_current = 0;
        }
    }

    if (real_surface_read == NULL || real_surface_draw == NULL || real_opengl_context == NULL)
    {
        glfwMakeContextCurrent(NULL);
        return EGL_TRUE;
    }

    //然后设置当前的surface和context
    thread_context->render_double_buffer_read = real_surface_read;
    real_surface_read->is_current = 1;
    thread_context->render_double_buffer_draw = real_surface_draw;
    real_surface_draw->is_current = 1;
    thread_context->opengl_context = real_opengl_context;
    real_opengl_context->is_current = 1;

    //@todo 设置各种config、attrib

    glfwMakeContextCurrent(real_surface_draw->window);

    //初始化opengl_context的一些资源，因为这个时候已经makecurrent了
    Bound_Buffer *bound_buffer = &(real_opengl_context->bound_buffer_status);
    if (bound_buffer->has_init == 0)
    {
        bound_buffer->has_init = 1;
        glGenBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glGenBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

        glGenBuffers(1, &(bound_buffer->attrib_point->indices_buffer_object));
        glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, bound_buffer->attrib_point->buffer_object);

        //这两个选项在gles中是默认开启，这样能够在着色器中获取到一些内建变量，所以在gl中要手动开启
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);
    }

    real_opengl_context->draw_fbo0 = real_surface_draw->display_fbo[real_surface_draw->now_draw];
    real_opengl_context->read_fbo0 = real_surface_read->display_fbo[real_surface_read->now_read];

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);

    if (real_surface_draw->I_am_composer)
    {
        set_compose_surface(real_surface_draw);
    }

    TEXTURE_LOCK(real_surface_draw->display_texture_is_use[real_surface_read->now_draw]);

#ifdef DEBUG_INDEPEND_WINDOW
    //屏幕分离调试专用
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    ShowWindow(render_context->render_double_buffer->window, TRUE);
#endif

    return EGL_TRUE;
}

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    assert(surface > 1000);
    Double_Buffer *real_surface = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    // express_printf("swapbuffer %lx %lx\n", surface, real_surface);

    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    Opengl_Context *real_opengl_context = thread_context->opengl_context;

    egl_surface_swap_buffer(real_surface);

    real_opengl_context->draw_fbo0 = real_surface->display_fbo[real_surface->now_draw];
    real_opengl_context->read_fbo0 = real_surface->display_fbo[real_surface->now_read];
    return EGL_TRUE;
}

EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, EGLint *ret_flag)
{
    EGLBoolean ret = d_eglSwapBuffers_sync(context, dpy, surface);
    if (ret == EGL_TRUE)
    {
        GLint now_flag_cnt = 0;
        Guest_Mem *guest_mem = (Guest_Mem *)ret_flag;

        guest_write(guest_mem, &now_flag_cnt, 0, sizeof(EGLint));

        now_flag_cnt = (now_flag_cnt + 1) % 1024;

        guest_read(guest_mem, &now_flag_cnt, 0, sizeof(EGLint));
    }
    return ret;
}

EGLBoolean d_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Double_Buffer *real_surface = thread_context->render_double_buffer_draw;
    real_surface->swap_interval = interval;
}

EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
}

EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
}

void d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list, EGLImage guest_image)
{
}

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{
}

//EGLClientBuffer d_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer *buffer);
