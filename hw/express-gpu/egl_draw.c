#define STD_DEBUG_LOG

#include "hw/express-gpu/egl_draw.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/egl_context.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/express_gpu_render.h"

static void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // 忽略一些不是错误的id
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;
    if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }

#ifdef ENABLE_OPENGL_PERFORMANCE_WARNING

#else
    if (type == GL_DEBUG_TYPE_PERFORMANCE)
    {
        return;
    }
#endif

    printf("\ndebug message(%u):%s\n", id, message);
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        printf("Source: API ");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Source: Window System ");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Source: Shader Compiler ");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Source: Third Party ");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Source: APPLICATION ");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("Type: Error ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Type: Deprecated Behaviour ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Type: Undefined Behaviour ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Type: Portability ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Type: Performance ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Type: Marker ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Type: Push Group ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Type: Pop Group ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Type: Other ");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("Severity: high");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Severity: medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Severity: low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Severity: notification");
        break;
    }
    printf("\n");
}

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id, int width, int height, int hal_format)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface_draw = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(draw));
    Window_Buffer *real_surface_read = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(read));

    Opengl_Context *real_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));

    express_printf("make current guest draw %llx read %llx context %llx\n", (uint64_t)draw, read, ctx);

    if (thread_context->render_double_buffer_draw == real_surface_draw && thread_context->render_double_buffer_read == real_surface_read)
    {
        return EGL_TRUE;
    }

    if (thread_context->render_double_buffer_draw != NULL && thread_context->render_double_buffer_draw != real_surface_draw)
    {
        thread_context->render_double_buffer_draw->is_current = 0;
        render_surface_uninit(thread_context->render_double_buffer_draw);
        Graphic_Buffer *old_draw_gbuffer = thread_context->render_double_buffer_draw->gbuffer;
        express_printf("makecurrent free draw surface %llx\n", (uint64_t)thread_context->render_double_buffer_draw);
        if (thread_context->render_double_buffer_draw->type == WINDOW_SURFACE && old_draw_gbuffer->gbuffer_id != gbuffer_id)
        {
            ATOMIC_LOCK(old_draw_gbuffer->is_lock);
            old_draw_gbuffer->remain_life_time = MAX_WINDOW_LIFE_TIME;
            if (old_draw_gbuffer->is_using == 0 && old_draw_gbuffer->is_dying == 0)
            {
                old_draw_gbuffer->is_dying = 1;
                send_message_to_main_window(MAIN_DESTROY_GBUFFER, old_draw_gbuffer);
            }
            ATOMIC_UNLOCK(old_draw_gbuffer->is_lock);
        }

        if (thread_context->render_double_buffer_draw->need_destroy)
        {
            render_surface_destroy(thread_context->render_double_buffer_draw);
            // g_hash_table_remove(process_context->surface_map, GUINT_TO_POINTER(thread_context->render_double_buffer_draw->guest_surface));
        }
    }

    if (thread_context->render_double_buffer_read != NULL && thread_context->render_double_buffer_read != thread_context->render_double_buffer_draw && thread_context->render_double_buffer_read != real_surface_read)
    {
        thread_context->render_double_buffer_read->is_current = 0;
        render_surface_uninit(thread_context->render_double_buffer_read);
        Graphic_Buffer *old_draw_gbuffer = thread_context->render_double_buffer_read->gbuffer;
        express_printf("makecurrent free read surface %llx\n", (uint64_t)thread_context->render_double_buffer_read);

        if (thread_context->render_double_buffer_read->type == WINDOW_SURFACE && old_draw_gbuffer->gbuffer_id != gbuffer_id)
        {
            ATOMIC_LOCK(old_draw_gbuffer->is_lock);
            old_draw_gbuffer->remain_life_time = MAX_WINDOW_LIFE_TIME;
            if (old_draw_gbuffer->is_using == 0 && old_draw_gbuffer->is_dying == 0)
            {
                old_draw_gbuffer->is_dying = 1;
                send_message_to_main_window(MAIN_DESTROY_GBUFFER, old_draw_gbuffer);
            }
            ATOMIC_UNLOCK(old_draw_gbuffer->is_lock);
        }
        if (thread_context->render_double_buffer_read->need_destroy)
        {
            render_surface_destroy(thread_context->render_double_buffer_read);
            // g_hash_table_remove(process_context->surface_map, GUINT_TO_POINTER(thread_context->render_double_buffer_draw->guest_surface));
        }
    }

    // context与opengl相关，当它销毁时窗口是要销毁的
    //原来current的context要destroy
    if (thread_context->opengl_context != NULL && thread_context->opengl_context != real_opengl_context)
    {
        // thread_context->opengl_context->draw_surface = NULL;
        express_printf("makecurrent context change %llx guest %llx %d window %llx\n", (uint64_t)thread_context->opengl_context, (uint64_t)thread_context->opengl_context->guest_context, thread_context->opengl_context->need_destroy, (uint64_t)thread_context->opengl_context->window);
        thread_context->opengl_context->is_current = 0;
        if (thread_context->opengl_context->need_destroy)
        {
            opengl_context_destroy(thread_context->opengl_context);
            g_free(thread_context->opengl_context);
        }
    }

    if (real_surface_read == NULL && real_surface_draw == NULL && real_opengl_context == NULL)
    {
        if (thread_context->opengl_context != NULL)
        {
            express_printf("thread %llx context %llx makecurrent window %llx null\n", thread_context, thread_context->opengl_context, thread_context->opengl_context->window);
        }
        express_printf("#%llx makecurrent null read %llx draw %llx\n", real_opengl_context, real_surface_read, real_surface_draw);
        if (thread_context->opengl_context != NULL && thread_context->opengl_context->independ_mode == 1)
        {
            glfwMakeContextCurrent(NULL);
        }
        else
        {
            egl_makeCurrent(NULL);
        }

        thread_context->opengl_context = NULL;
        thread_context->render_double_buffer_draw = NULL;
        thread_context->render_double_buffer_read = NULL;
        return EGL_TRUE;
    }

    if (real_opengl_context->independ_mode == 1)
    {
        glfwMakeContextCurrent((GLFWwindow *)real_opengl_context->window);
        if (real_surface_draw != NULL && real_surface_draw->type == WINDOW_SURFACE && real_surface_draw->width > 10 && real_surface_draw->height > 10)
        {
            glfwSetWindowSize(real_opengl_context->window, real_surface_draw->width, real_surface_draw->height);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
            glfwShowWindow((GLFWwindow *)real_opengl_context->window);
        }
        else
        {
            glfwHideWindow((GLFWwindow *)real_opengl_context->window);
        }
    }
    else
    {
        express_printf("thread %llx context %llx makecurrent window %llx\n", thread_context, real_opengl_context, real_opengl_context->window);
        egl_makeCurrent(real_opengl_context->window);
    }

    if(express_gpu_gl_debug_enable)
    {    
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }

    //然后设置当前的surface和context
    thread_context->render_double_buffer_read = real_surface_read;
    real_surface_read->is_current = 1;
    thread_context->render_double_buffer_draw = real_surface_draw;
    if (real_surface_draw != NULL)
    {
        real_surface_draw->is_current = 1;
        real_surface_draw->frame_start_time = 0;
        // real_surface_draw->last_frame_num = -1;
    }
    thread_context->opengl_context = real_opengl_context;
    real_opengl_context->is_current = 1;
    // real_opengl_context->draw_surface = real_surface_read;

    // printf("#%llx makecurrent draw surface %llx\n",real_opengl_context, real_surface_draw);
    //窗口大小设置一定要在init之前

    if (gbuffer_id != 0)
    {
        real_opengl_context->view_x = 0;
        real_opengl_context->view_y = 0;
        real_opengl_context->view_w = width;
        real_opengl_context->view_h = height;
    }
    else
    {
        real_opengl_context->view_x = 0;
        real_opengl_context->view_y = 0;
        real_opengl_context->view_w = real_surface_draw->width;
        real_opengl_context->view_h = real_surface_draw->height;
    }

    express_printf("context %llx gbuffer_id %llx makecurrent glviewport w %d h %d\n", real_opengl_context, gbuffer_id, real_opengl_context->view_w, real_opengl_context->view_h);
    glViewport(real_opengl_context->view_x, real_opengl_context->view_y, real_opengl_context->view_w, real_opengl_context->view_h);

    Graphic_Buffer *gbuffer = NULL;

    if (real_surface_draw != NULL)
    {
        if (real_surface_draw->type == WINDOW_SURFACE)
        {
            gbuffer = get_gbuffer_from_global_map(gbuffer_id);
            if (gbuffer == NULL)
            {
                express_printf("#context %llx makecurrent %llx with create window gbuffer %llx\n", real_opengl_context, real_surface_draw, gbuffer_id);
                gbuffer = create_gbuffer_from_hal(width, height, hal_format, real_surface_draw, gbuffer_id);

                express_printf("create gbuffer when makecurrent gbuffer %llx ptr %llx\n", gbuffer_id, gbuffer);

                add_gbuffer_to_global(gbuffer);
                set_global_gbuffer_type(gbuffer_id, GBUFFER_TYPE_WINDOW);
            }
            else
            {
                ATOMIC_LOCK(gbuffer->is_lock);
                gbuffer->remain_life_time = MAX_WINDOW_LIFE_TIME;
                if (gbuffer->is_using == 0 && gbuffer->is_dying == 1)
                {
                    gbuffer->is_dying = 0;
                    send_message_to_main_window(MAIN_CANCEL_GBUFFER, gbuffer);
                }
                ATOMIC_UNLOCK(gbuffer->is_lock);
            }
        }
        else
        {
            gbuffer = real_surface_draw->gbuffer;
            if (gbuffer == NULL)
            {
                express_printf("#context %llx makecurrent psurface %llx with create window gbuffer %llx ptr %llx\n", real_opengl_context, real_surface_draw, gbuffer_id, gbuffer);
                gbuffer = create_gbuffer_from_surface(real_surface_draw);

                gbuffer->gbuffer_id = 0;
                // psurface 不需要保留120帧
                gbuffer->remain_life_time = 0;
                // pbuffer 不需要添加到global_map中
            }
        }

        if (real_surface_draw->gbuffer != NULL)
        {
            real_surface_draw->gbuffer->is_writing = 0;
#ifdef _WIN32
            SetEvent(real_surface_draw->gbuffer->writing_ok_event);
#else
#endif
        }

        real_surface_draw->gbuffer = gbuffer;
    }

    opengl_context_init(real_opengl_context);
    render_surface_init(real_surface_draw);
    if (real_surface_read != real_surface_draw)
    {
        render_surface_init(real_surface_read);
    }

    connect_gbuffer_to_surface(gbuffer, real_surface_draw);

    //@todo 设置各种config、attrib

    GLint read_fbo = 0;
    GLint write_fbo = 0;

    if (real_surface_draw != NULL && real_surface_read != NULL)
    {
        read_fbo = real_surface_read->gbuffer->data_fbo;
        //设置framebuffer 0
        if (gbuffer->sampler_num > 1)
        {
            write_fbo = gbuffer->sampler_fbo;
        }
        else
        {
            write_fbo = gbuffer->data_fbo;
        }
    }

    real_opengl_context->read_fbo0 = read_fbo;
    real_opengl_context->draw_fbo0 = write_fbo;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);

    // printf("context %llx makecurrent fbo %d %d gbuffer %llx\n", real_opengl_context, real_opengl_context->draw_fbo0, real_opengl_context->read_fbo0, real_surface_draw->gbuffer->gbuffer_id);

    return EGL_TRUE;
}

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface, uint64_t gbuffer_id, int width, int height, int hal_format)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));

    // express_printf("swapbuffer %lx %lx\n", surface, real_surface);

    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    Opengl_Context *real_opengl_context = thread_context->opengl_context;
    // printf("context %llx swapbuffer\n",real_opengl_context);

    if (real_surface != thread_context->render_double_buffer_draw)
    {
        printf("error! real_surface != thread_context->render_double_buffer_draw %llx %llx\n", (uint64_t)real_surface, (uint64_t)thread_context->render_double_buffer_draw);
    }

    egl_surface_swap_buffer(context, real_surface, gbuffer_id, width, height, hal_format);

    if (real_surface->sampler_num > 1)
    {
        real_opengl_context->draw_fbo0 = real_surface->gbuffer->sampler_fbo;
    }
    else
    {
        real_opengl_context->draw_fbo0 = real_surface->gbuffer->data_fbo;
    }

    real_opengl_context->read_fbo0 = thread_context->render_double_buffer_read->gbuffer->data_fbo;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);

    // printf("context %llx swapbuffer fbo %d %d gbuffer %llx texture %d\n", real_opengl_context, real_opengl_context->draw_fbo0, real_opengl_context->read_fbo0, real_surface->gbuffer->gbuffer_id, real_surface->gbuffer->data_texture);

    // GLenum attachments[]={GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT,GL_DEPTH_STENCIL_ATTACHMENT};
    // glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 4, attachments);

    // // printf("context swapbuffer %llx draw_fbo0 %d\n",(uint64_t)real_opengl_context,real_opengl_context->draw_fbo0);

    return EGL_TRUE;
}

// static gboolean gbuffer_printf(gpointer key, gpointer data, gpointer user_data)
// {
//     Graphic_Buffer *gbuffer = (Graphic_Buffer *)data;
//     printf("%llx-%dx%d-%d ", gbuffer->gbuffer_id, gbuffer->width, gbuffer->height, gbuffer->usage_type);
//     return true;
// }

void d_eglQueueBuffer(void *context, uint64_t gbuffer_id, int is_composer)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    // Process_Context *process_context = thread_context->process_context;
    Opengl_Context *opengl_context = thread_context->opengl_context;

    // Window_Buffer *draw_surface = thread_context->render_double_buffer_draw;

    // glFlush();
    // glFinish();

    Graphic_Buffer *gbuffer = get_gbuffer_from_global_map((uint64_t)gbuffer_id);

    if (gbuffer_id == 0 || gbuffer == NULL)
    {
        //不可能不在自己进程下
        printf("error! context %llx queuebuffer id %llx not exist!\n", (uint64_t)opengl_context, (uint64_t)gbuffer_id);
        return;
    }

    //防止卡死，queue之后要主动解锁
    // egl_image->is_lock = 0;
    // egl_image->host_has_data = 1;
    // ATOMIC_UNLOCK(egl_image->display_texture_is_use);
    // ATOMIC_SET_UNUSED(egl_image->display_texture_is_use);
    express_printf("#%llx context queue buffer %llx\n", (uint64_t)opengl_context, gbuffer_id);

    if (gbuffer->sampler_num > 1)
    {
        // printf("use sample blit\n");

        if (opengl_context != NULL && opengl_context->enable_scissor == 1)
        {
            glDisable(GL_SCISSOR_TEST);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->sampler_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer->data_fbo);
        glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, 0, gbuffer->width, gbuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        // queuebuffer后肯定有自动绑定新的fbo，所以不用专门切换
        if (opengl_context != NULL && opengl_context->enable_scissor == 1)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, opengl_context->draw_fbo0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_context->read_fbo0);
    }

    if (preload_static_context_value->composer_pid != 0 && ((int)(gbuffer->gbuffer_id >> 32)) != preload_static_context_value->composer_pid)
    {
        //这种情况太罕见了，只在长按图标拖动时才会出现，这种情况下的gbuffer提供者也是自己，而不是surfaceflinger，它会把数据绘制到surface上，进而绘制到gbuffer上。
        //由于之后gbuffer被作为texture用时，会有个自动上下颠倒，因此直接在这里进行上下颠倒，这样保存到texture中的就已经是上下颠倒过了的
        //为什么surfaceflinger不需要上下颠倒？安卓9的surfaceflinger输出的图像是到正常的fbo里，不需要颠倒，而安卓10输出到gbuffer里，确实是上下颠倒的，但是我们的窗口在这种情况下进行颠倒输出的，因此不存在问题
        //为什么surfaceflinger把其他窗口gbuffer数据当成texture来用不需要颠倒？也需要颠倒，但是这个颠倒操作是surfaceflinger自己完成的，我们通过设置a_win->perform(a_win, NATIVE_WINDOW_SET_BUFFERS_TRANSFORM, HAL_TRANSFORM_FLIP_V)来实现
        printf("reverse gbuffer %llx\n", gbuffer->gbuffer_id);
        reverse_gbuffer(gbuffer);
    }

    if (gbuffer->delete_sync != 0)
    {
        glDeleteSync(gbuffer->delete_sync);
    }

    gbuffer->delete_sync = gbuffer->data_sync;
    gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    gbuffer->is_writing = 0;

    // glFinish();
    glFlush();
#ifdef _WIN32
    SetEvent(gbuffer->writing_ok_event);
#else
#endif

    if (opengl_context->independ_mode == 1)
    {
        if (opengl_context != NULL && opengl_context->enable_scissor == 1)
        {
            glDisable(GL_SCISSOR_TEST);
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->data_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, 0, gbuffer->width, gbuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        if (opengl_context != NULL && opengl_context->enable_scissor == 1)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        glfwSwapBuffers(opengl_context->window);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, opengl_context->draw_fbo0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_context->read_fbo0);
    }
    
    // if(express_gpu_independ_window_enable)
    // {
    //     glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->data_fbo);
    //     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //     glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, 0, gbuffer->width, gbuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //     glfwSwapBuffers(opengl_context->window);
    // }

    if (is_composer == 1)
    {
        //合成器生成的gbuffer没有createImage和destroyImage的过程，所以可能面临内存泄露的问题，因此合成器产生的gbuffer就直接延迟删除，假如要用到了，则延长寿命，但是始终不从链表上删除
        gbuffer->remain_life_time = MAX_COMPOSER_LIFE_TIME;
        if (gbuffer->is_dying == 0)
        {
            //保证destroy消息只发送一次，并且一直在链表上
            gbuffer->is_dying = 1;
            send_message_to_main_window(MAIN_DESTROY_GBUFFER, gbuffer);
        }
        // 合成器的生存时间要长5倍，相当于是10秒钟
        // set_display_gbuffer(gbuffer);

        send_message_to_main_window(MAIN_PAINT, gbuffer);
    }
    else
    {
        // is_using和is_dying要一起判断，一起设置，否则这个会产生竞争，导致消息遗漏或者消息过多，例如这里进了判断，刚设置完is_dying等于1, createImage就依据这个发送cancel destroy，然后这里发送destroy消息，就会导致gbuffer被意外删除
        ATOMIC_LOCK(gbuffer->is_lock);
        gbuffer->remain_life_time = MAX_WINDOW_LIFE_TIME;
        if (gbuffer->is_using == 0 && gbuffer->is_dying == 0)
        {
            gbuffer->is_dying = 1;
            send_message_to_main_window(MAIN_DESTROY_GBUFFER, gbuffer);
        }
        ATOMIC_UNLOCK(gbuffer->is_lock);
    }

    // int size =  (int)g_hash_table_size(process_context->gbuffer_map);
    // if(size > 0)
    // {
    //     printf("process %llx keep gbuffer %d (%d):\n", process_context,size, is_composer);
    //     g_hash_table_foreach(process_context->gbuffer_map, gbuffer_printf, NULL);
    //     printf("\n\n");
    // }
}

EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, int64_t invoke_time, uint64_t gbuffer_id, int width, int height, int hal_format, Guest_Mem *ret_invoke_time, Guest_Mem *swap_time)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));

    if (real_surface == NULL)
    {
        express_printf(RED("real surface is null!"));
        express_printf("surface %lx real %lx dpy %lx invoke_time %lld\n", surface, real_surface, dpy, invoke_time);
        return EGL_FALSE;
        // Guest_Mem *guest_mem_invoke = (Guest_Mem *)ret_invoke_time;
        // Guest_Mem *guest_mem_swap = (Guest_Mem *)swap_time;
        // int64_t a,b;
        // read_from_guest_mem(guest_mem_invoke, &a, 0, sizeof(EGLint));
        // read_from_guest_mem(guest_mem_swap, &b, 0, sizeof(EGLint));
        // express_printf("invoke time %lld swap_time %lld\n",a,b);
    }

    express_printf("#%llx swapbuffer real_surface %llx\n", thread_context->opengl_context, real_surface);

    // gint64 start_time = g_get_real_time();
    EGLBoolean ret = d_eglSwapBuffers_sync(context, dpy, surface, gbuffer_id, width, height, hal_format);
    // gint64 end_time = g_get_real_time();
    // gint64 now_swap_time = end_time - start_time;

    gint64 now_time = g_get_real_time();

    if (real_surface->swap_time_cnt < 5)
    {
        //前两帧刚开始很可能用来进行初始化，因此前两帧帧的时间不能保存，都假设只是2ms的时间，因为这个是一次传输的延迟，相当于是距离前一个同步的时间
        if (real_surface->swap_time_cnt <= 2 || real_surface->frame_start_time == 0)
        {
            real_surface->swap_time[real_surface->swap_loc] = 2000;
            real_surface->swap_time_all += 2000;
        }
        else
        {
            real_surface->swap_time[real_surface->swap_loc] = now_time - real_surface->frame_start_time;
            real_surface->swap_time_all += now_time - real_surface->frame_start_time;
        }

        real_surface->swap_loc = (real_surface->swap_loc + 1) % 5;
        real_surface->swap_time_cnt++;
    }
    else
    {
        if (real_surface->frame_start_time != 0)
        {
            real_surface->swap_time_all += 2000;
            real_surface->swap_time[real_surface->swap_loc] = 2000;
        }
        else
        {
            real_surface->swap_time_all += now_time - real_surface->frame_start_time;
            real_surface->swap_time[real_surface->swap_loc] = now_time - real_surface->frame_start_time;
        }
        real_surface->swap_time_all -= real_surface->swap_time[real_surface->swap_loc];
        real_surface->swap_loc = (real_surface->swap_loc + 1) % 5;
    }

    real_surface->frame_start_time = 0;

    // if(real_surface->swap_loc==0){
    //     express_printf("avg swap time %lld\n",real_surface->swap_time_all/real_surface->swap_time_cnt);
    // }

    gint64 now_avg_swap_time = real_surface->swap_time_all / real_surface->swap_time_cnt;

    // //保证这个swap_time不为0，方便guest判断是否有返回
    // if (real_surface->swap_time_cnt <= 10)
    // {
    //     now_avg_swap_time = -1;
    // }

    if (ret == EGL_TRUE)
    {

        // GLint now_flag_cnt = 0;
        Guest_Mem *guest_mem_invoke = (Guest_Mem *)ret_invoke_time;
        Guest_Mem *guest_mem_swap = (Guest_Mem *)swap_time;

        //加这个判断是为了防止guest端应用被强退，内存被释放之后，这里再进行内存的写入，导致潜在的系统崩溃
        Thread_Context *thread_context = (Thread_Context *)context;
        if (thread_context->init != 0)
        {
            // read_from_guest_mem(guest_mem, &now_flag_cnt, 0, sizeof(EGLint));

            // now_flag_cnt = (now_flag_cnt + 1) % 1024;

            // EGLint swap_time = (EGLint)(real_surface->frame_gen_time);
            // printf("#%llx write now_avg_swap_time %lld\n", ((Render_Thread_Context *)thread_context)->opengl_context, now_avg_swap_time);
            write_to_guest_mem(guest_mem_invoke, &invoke_time, 0, sizeof(int64_t));

            write_to_guest_mem(guest_mem_swap, &now_avg_swap_time, 0, sizeof(int64_t));
        }
    }

    //计算帧率
    if (now_time - real_surface->last_calc_time > 1000000 && real_surface->last_calc_time != 0)
    {
        double hz = real_surface->now_screen_hz * 1000000.0 / (now_time - real_surface->last_calc_time);
        printf("%llx surface draw %.2lfHz\n", (uint64_t)real_surface, hz);
        real_surface->now_screen_hz = 0;

        real_surface->last_calc_time = now_time;
        if (real_surface->I_am_composer)
        {
            if (hz > 132.0)
            {
                composer_refresh_HZ = 144;
            }
            else if (hz > 105.0)
            {
                composer_refresh_HZ = 120;
            }
            else if (hz > 82.5)
            {
                composer_refresh_HZ = 90;
            }
            else if (hz > 67.5)
            {
                composer_refresh_HZ = 75;
            }
            else
            {
                composer_refresh_HZ = 60;
            }
            if (composer_refresh_HZ != preload_static_context_value->composer_HZ)
            {
                preload_static_context_value->composer_HZ = composer_refresh_HZ;
            }
        }
    }
    else if (real_surface->last_calc_time == 0)
    {
        real_surface->last_calc_time = now_time;
        real_surface->now_screen_hz = 0;
    }

    real_surface->now_screen_hz += 1;

    return ret;
}

EGLBoolean d_eglSwapInterval(void *context, EGLDisplay dpy, EGLint interval)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Window_Buffer *real_surface = thread_context->render_double_buffer_draw;
    real_surface->swap_interval = interval;
    return EGL_TRUE;
}

EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return EGL_FALSE;
}

EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return EGL_FALSE;
}

void d_eglSetGraphicBufferID(void *context, EGLSurface draw, uint64_t gbuffer_id)
{
    // Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    // Process_Context *process_context = thread_context->process_context;
    // Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(draw));

    // if(real_surface == NULL)
    // {
    //     return;
    // }
    // set_gbuffer_id_surface(gbuffer_id, NULL, real_surface);
    // printf("%llx surface connect gbuffer_id %llx\n",real_surface,gbuffer_id);
}

// EGLClientBuffer d_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer *buffer);
