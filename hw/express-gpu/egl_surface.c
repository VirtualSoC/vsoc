/**
 * @file egl_surface.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief surface决定到底绘制是个啥样的逻辑，swap时是个啥样的逻辑
 * @version 0.1
 * @date 2021-08-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#define STD_DEBUG_LOG
#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_display.h"

#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"

void egl_surface_swap_buffer(Double_Buffer *surface)
{
    //这句很重要，没了这个画不出来，这个是保证之前的绘制操作都针对原来的draw进行的
    GLsync wait_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
    //这句话让之前的画面都渲染出来
    // glFinish();

    int now_draw_buffer = surface->now_draw;
    //解除对当前绘制的缓冲区的锁定，这个时候这个缓冲区能够被使用
    TEXTURE_UNLOCK(surface->display_texture_is_use[now_draw_buffer]);

    if (surface->fbo_sync[now_draw_buffer] != NULL)
    {
        glDeleteSync(surface->fbo_sync[now_draw_buffer]);
        surface->fbo_sync[now_draw_buffer] = wait_sync;
    }
    surface->now_read = surface->now_draw;

    if (surface->swap_interval > 0)
    {
        int next_frame_num = (surface->last_frame_num + surface->swap_interval) % 65536;
        surface->last_frame_num = draw_wait_GSYNC(surface->swap_event, next_frame_num);
    }

    //尝试锁定下一个将要绘制的缓冲区
    int next_draw_buffer = (surface->now_draw + 1) % surface->buffer_num;
    TEXTURE_LOCK(surface->display_texture_is_use[next_draw_buffer]);
    surface->now_draw = next_draw_buffer;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->display_fbo[surface->now_draw]);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, surface->display_fbo[surface->now_read]);
}

/**
 * @brief 利用windows初始化surface，注意：这个操作只能在draw子线程中进行，并且在创建了context之后
 * 
 * @param d_buffer 需要初始化的surface
 * @return int 返回1则创建成功，返回0则创建失败 
 */
int egl_surface_init(Double_Buffer *d_buffer)
{
    if (d_buffer == NULL || d_buffer->window == NULL)
    {
        return 0;
    }
    //必须要先makecurrent，不然下面的资源没法申请
    //这个也不能放到主窗口线程中去，因为fbo是不共享的，只能子窗口自己生成
    glfwMakeContextCurrent(d_buffer->window);
    d_buffer->swap_event = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (d_buffer->type == WINDOW_SURFACE)
    {
        d_buffer->buffer_num = 2;
        d_buffer->now_read = 0;
        d_buffer->now_draw = 1;
    }
    else if (d_buffer->type == P_SURFACE)
    {
        //pbuffer只有单缓冲区
        d_buffer->buffer_num = 1;
        d_buffer->now_draw = 0;
        d_buffer->now_read = 0;
    }
    int buffer_num = d_buffer->buffer_num;

    glGenTextures(buffer_num, d_buffer->fbo_texture);
    glGenFramebuffers(buffer_num, d_buffer->display_fbo);
    glGenRenderbuffers(buffer_num, d_buffer->display_rbo);

    for (int i = 0; i < buffer_num; i++)
    {
        //@todo 验证这样的默认设置是否足够显示(例如颜色空间是否足够，深度空间是否足够)
        glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture[i]);

        //因为这个是最终画面，所以不需要透明，RGB就行，不需要RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, d_buffer->width, d_buffer->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //这个相当于给与一个深度缓冲区，让这个fbo可以有颜色缓冲区，有深度缓冲区
        glBindRenderbuffer(GL_RENDERBUFFER, d_buffer->display_rbo[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, d_buffer->width, d_buffer->height); // Use a single renderbuffer object for both a depth AND stencil buffer.

        glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->display_fbo[i]);
        //附加颜色缓冲区
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture[i], 0);
        //附加深度缓冲区
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d_buffer->display_rbo[i]); // Now actually attach it
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //这里将读写的framebuffer分离，是为了readpixel时，能够从后缓冲区读取数据
    //（对于我们的程序，后缓冲区就是fbo_dispaly，而对于绑定fbo不为0时时会选择从read fbo读取，所以要这样把display-fbo设置为read）
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, d_buffer->display_fbo[d_buffer->now_draw]);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, d_buffer->display_fbo[d_buffer->now_read]);

//屏幕分离调试专用
#ifdef DEBUG_INDEPEND_WINDOW
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

    //最后detach当前的context，以防止这个surface被其他线程用了
    glfwMakeContextCurrent(NULL);

    return 1;
}

Double_Buffer *render_surface_create(EGLConfig config, const EGLint *attrib_list, int type)
{
    //@todo 处理config、处理attrib_list

    //这里先根据attrb_list获取窗口的宽和高

    Double_Buffer *surface = g_malloc(sizeof(Double_Buffer));
    memset(surface, 0, sizeof(Double_Buffer));
    surface->type = type;
    surface->width = 0;
    surface->height = 0;
    surface->swap_interval = 1;

    int i = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            surface->width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            surface->height = attrib_list[i + 1];
            break;
        default:
            //todo 其他attrib属性的设置
            break;
        }
        i += 2;
    }

    //创建真实的窗口
    render_windows_create(surface);
    assert(surface->window!=NULL);

    egl_surface_init(surface);

    return surface;
}

int render_surface_destroy(Double_Buffer *surface)
{

    if (surface == NULL)
    {
        return 0;
    }
    // express_printf("delete fbo_display\n");

    express_printf("windows destroy\n");

    if (surface->I_am_composer)
    {
        set_compose_surface(NULL);
    }

    if (surface->is_current)
    {
        surface->need_destroy = 1;
    }
    else
    {

        //没有makecurrent的时候这些资源肯定没有被使用，但是这个时候也不能调用glDelete等函数，因为真的context已经不在了
        //所以这里让主线程来清空数据
        //为什么不直接调用glfwDestroyWindow自动清空资源？因为部分共享资源不会被清空，需要手动清空
        PostMessage(draw_native_window, WM_USER_SURFACE_DESTROY, 0, (LPARAM)surface);
        // glDeleteFramebuffers(surface->buffer_num, surface->display_fbo);
        // glDeleteTextures(surface->buffer_num, surface->fbo_texture);
        // glDeleteRenderbuffers(surface->buffer_num, surface->display_rbo);

        // glfwDestroyWindow(surface->window);
        // g_free(surface);
    }
    // glfwMakeContextCurrent(NULL);

    return 1;
}

void d_eglIamComposer(void *context, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Double_Buffer *real_surface = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));
    
    express_printf("surface is composer %lx %lx\n",real_surface,surface);

    real_surface->I_am_composer = 1;
}

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    EGLSurface host_surface = (EGLSurface)render_surface_create(config, attrib_list, P_SURFACE);

    g_hash_table_insert(process_context->surface_map, GINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    EGLSurface host_surface =  (EGLSurface)render_surface_create(config, attrib_list, WINDOW_SURFACE);

    express_printf("surface create %lx %lx\n",host_surface,guest_surface);
    g_hash_table_insert(process_context->surface_map, GINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    // Double_Buffer *real_surface = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));
    // if (real_surface == NULL)
    // {
    //     return EGL_FALSE;
    // }
    //g_map设定了destroy函数
    // render_surface_destroy(real_surface);
    express_printf("destroy surface %lx\n",surface);
    g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
    return EGL_TRUE;
}

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    return EGL_TRUE;
}
