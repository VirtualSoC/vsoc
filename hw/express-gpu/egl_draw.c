// #define STD_DEBUG_LOG

#include "hw/express-gpu/egl_draw.h"
#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/egl_context.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/express_gpu_render.h"

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id, int width, int height, int hal_format)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface_draw = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(draw));
    Window_Buffer *real_surface_read = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(read));

    Opengl_Context *real_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));

    express_printf("makecurrent guest draw %llx read %llx context %llx\n", (uint64_t)draw, (uint64_t)read, (uint64_t)ctx);
    express_printf("makecurrent host draw %llx read %llx context %llx\n", (uint64_t)real_surface_draw, (uint64_t)real_surface_read, (uint64_t)real_opengl_context);

    if (thread_context->opengl_context == real_opengl_context && thread_context->render_double_buffer_draw == real_surface_draw && thread_context->render_double_buffer_read == real_surface_read)
    {
        return EGL_TRUE;
    }

    if (thread_context->render_double_buffer_draw != NULL && thread_context->render_double_buffer_draw != real_surface_draw)
    {
        thread_context->render_double_buffer_draw->is_current = 0;
        render_surface_uninit(thread_context->render_double_buffer_draw);

        express_printf("makecurrent free draw surface %llx\n", (uint64_t)thread_context->render_double_buffer_draw);

        if (thread_context->render_double_buffer_draw->need_destroy)
        {
            render_surface_destroy(thread_context->render_double_buffer_draw);
        }
    }

    if (thread_context->render_double_buffer_read != NULL && thread_context->render_double_buffer_read != thread_context->render_double_buffer_draw && thread_context->render_double_buffer_read != real_surface_read)
    {
        thread_context->render_double_buffer_read->is_current = 0;
        render_surface_uninit(thread_context->render_double_buffer_read);

        express_printf("makecurrent free read surface %llx\n", (uint64_t)thread_context->render_double_buffer_read);

        if (thread_context->render_double_buffer_read->need_destroy)
        {
            render_surface_destroy(thread_context->render_double_buffer_read);
            // g_hash_table_remove(process_context->surface_map, GUINT_TO_POINTER(thread_context->render_double_buffer_draw->guest_surface));
        }
    }

    // context与opengl相关，当它销毁时窗口是要销毁的
    // 原来current的context要destroy
    if (thread_context->opengl_context != NULL && thread_context->opengl_context != real_opengl_context)
    {
        // thread_context->opengl_context->draw_surface = NULL;
        express_printf("makecurrent context change %llx guest %llx need_destroy %d window %llx\n", (uint64_t)thread_context->opengl_context, (uint64_t)thread_context->opengl_context->guest_context, thread_context->opengl_context->need_destroy, (uint64_t)thread_context->opengl_context->window);
#ifndef __APPLE__
        glDebugMessageCallback(NULL, NULL);
#endif
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
            express_printf("thread %llx context %llx window %llx makecurrent null\n", thread_context, thread_context->opengl_context, thread_context->opengl_context->window);
        }
        if (thread_context->opengl_context != NULL && thread_context->opengl_context->context_flags & DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT)
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

    if (real_opengl_context->context_flags & DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT)
    {
        glfwMakeContextCurrent((GLFWwindow *)real_opengl_context->window);
        if (real_surface_draw != NULL && real_surface_draw->type == WINDOW_SURFACE && real_surface_draw->width > 10 && real_surface_draw->height > 10)
        {
            THREAD_CONTROL_BEGIN

            LOGI("independent window width %d height %d width %d height %d", real_surface_draw->width, real_surface_draw->height, width, height);
            glfwSetWindowSize(real_opengl_context->window, width, height);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
            glfwShowWindow((GLFWwindow *)real_opengl_context->window);

            THREAD_CONTROL_END
        }
        else
        {
            THREAD_CONTROL_BEGIN

            glfwHideWindow((GLFWwindow *)real_opengl_context->window);

            THREAD_CONTROL_END
        }
    }
    else
    {
        express_printf("thread %llx context %llx makecurrent window %llx\n", thread_context, real_opengl_context, real_opengl_context->window);
        if (egl_makeCurrent(real_opengl_context->window) != EGL_TRUE) return EGL_FALSE;
    }

    if (express_gpu_gl_debug_enable || real_opengl_context->context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
    #ifdef _WIN32        
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(d_debug_message_callback, real_opengl_context);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    #endif
    }

    // 然后设置当前的surface和context
    thread_context->render_double_buffer_read = real_surface_read;
    if (real_surface_read != NULL)
    {
        real_surface_read->is_current = 1;
    }
    thread_context->render_double_buffer_draw = real_surface_draw;
    if (real_surface_draw != NULL)
    {
        real_surface_draw->is_current = 1;
        real_surface_draw->frame_start_time = 0;
    }
    thread_context->opengl_context = real_opengl_context;
    real_opengl_context->is_current = 1;
    // real_opengl_context->draw_surface = real_surface_read;

    // LOGI("#%llx makecurrent draw surface %llx",real_opengl_context, real_surface_draw);
    // 窗口大小设置一定要在init之前

    if (gbuffer_id != 0)
    {
        real_opengl_context->view_x = 0;
        real_opengl_context->view_y = 0;
        real_opengl_context->view_w = width;
        real_opengl_context->view_h = height;
    }
    else if (real_surface_draw != NULL)
    {
        real_opengl_context->view_x = 0;
        real_opengl_context->view_y = 0;
        real_opengl_context->view_w = real_surface_draw->width;
        real_opengl_context->view_h = real_surface_draw->height;
    }
    else
    {
        real_opengl_context->view_x = 0;
        real_opengl_context->view_y = 0;
        real_opengl_context->view_w = 0;
        real_opengl_context->view_h = 0;
    }

    express_printf("context %llx gbuffer_id %llx makecurrent glviewport w %d h %d\n", real_opengl_context, gbuffer_id, real_opengl_context->view_w, real_opengl_context->view_h);
    glViewport(real_opengl_context->view_x, real_opengl_context->view_y, real_opengl_context->view_w, real_opengl_context->view_h);

    Hardware_Buffer *gbuffer = NULL;

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
            }
            else
            {
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
        }

        real_surface_draw->gbuffer = gbuffer;
    }

    opengl_context_init(real_opengl_context);

    if (real_surface_draw == NULL && real_surface_read == NULL)
    {
        LOGI("host create surfaceless context %llx", (uint64_t)ctx);
        real_opengl_context->read_fbo0 = 0;
        real_opengl_context->draw_fbo0 = 0;
        return EGL_TRUE;
    }

    render_surface_init(real_surface_draw);
    if (real_surface_read != real_surface_draw)
    {
        render_surface_init(real_surface_read);
    }

    if (express_gpu_gl_debug_enable) {
        LOGI("real_surface_draw: type %x width %d height %d gbuffer %p gbuffer_id %llu", real_surface_draw->type, real_surface_draw->width, real_surface_draw->height, real_surface_draw->gbuffer, real_surface_draw->gbuffer_id);
    }

    connect_gbuffer_to_surface(gbuffer, real_surface_draw);

    //@todo 设置各种config、attrib

    GLint read_fbo = 0;
    GLint write_fbo = 0;

    if (real_surface_draw != NULL && real_surface_read != NULL)
    {
        read_fbo = real_surface_read->gbuffer->data_fbo;
        // 设置framebuffer 0
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

    // LOGI("context %llx makecurrent fbo %d %d gbuffer %llx", real_opengl_context, real_opengl_context->draw_fbo0, real_opengl_context->read_fbo0, real_surface_draw->gbuffer->gbuffer_id);

    return EGL_TRUE;
}

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface, uint64_t gbuffer_id, int width, int height, int hal_format)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));

    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    Opengl_Context *real_opengl_context = thread_context->opengl_context;
    
    if (real_opengl_context == NULL)
    {
        LOGE("eglswapbuffers on null opengl context! thread_context %p", thread_context);
        return EGL_FALSE;
    }
    // LOGI("context %llx swapbuffer",real_opengl_context);

    if (real_surface != thread_context->render_double_buffer_draw)
    {
        LOGE("error! real_surface != thread_context->render_double_buffer_draw %llx %llx", (uint64_t)real_surface, (uint64_t)thread_context->render_double_buffer_draw);
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

    // LOGI("context %llx swapbuffer fbo %d %d gbuffer %llx texture %d", real_opengl_context, real_opengl_context->draw_fbo0, real_opengl_context->read_fbo0, real_surface->gbuffer->gbuffer_id, real_surface->gbuffer->data_texture);

    return EGL_TRUE;
}

void d_eglQueueBuffer(void *context, uint64_t gbuffer_id, int is_composer)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    // Process_Context *process_context = thread_context->process_context;
    Opengl_Context *opengl_context = thread_context->opengl_context;

    Hardware_Buffer *gbuffer = get_gbuffer_from_global_map((uint64_t)gbuffer_id);

    if (gbuffer_id == 0 || gbuffer == NULL)
    {
        // 不可能不在自己进程下
        LOGE("error! context %llx queuebuffer id %llx not exist!", (uint64_t)opengl_context, (uint64_t)gbuffer_id);

        return;
    }

    // 这一帧被绘制到bufferqueue中
    LOGD("#%llx context queue buffer %" PRIx64 , (uint64_t)opengl_context, gbuffer_id);

    if (gbuffer->sampler_num > 1)
    {
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

    gbuffer->is_writing = 0;

    if (opengl_context->context_flags & DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT)
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
    }

    express_printf("#%llx swapbuffer real_surface %llx\n", thread_context->opengl_context, real_surface);

    EGLBoolean ret = d_eglSwapBuffers_sync(context, dpy, surface, gbuffer_id, width, height, hal_format);

    gint64 now_time = g_get_real_time();

    if (real_surface->swap_time_cnt < 5)
    {
        // 前两帧刚开始很可能用来进行初始化，因此前两帧帧的时间不能保存，都假设只是2ms的时间，因为这个是一次传输的延迟，相当于是距离前一个同步的时间
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
        real_surface->swap_time_all -= real_surface->swap_time[real_surface->swap_loc];
        if (real_surface->frame_start_time == 0)
        {
            real_surface->swap_time_all += 2000;
            real_surface->swap_time[real_surface->swap_loc] = 2000;
        }
        else
        {
            real_surface->swap_time_all += now_time - real_surface->frame_start_time;
            real_surface->swap_time[real_surface->swap_loc] = now_time - real_surface->frame_start_time;
        }
        real_surface->swap_loc = (real_surface->swap_loc + 1) % 5;
    }

    real_surface->frame_start_time = 0;

    gint64 now_avg_swap_time = real_surface->swap_time_all / real_surface->swap_time_cnt;

    if (ret == EGL_TRUE)
    {

        // GLint now_flag_cnt = 0;
        Guest_Mem *guest_mem_invoke = (Guest_Mem *)ret_invoke_time;
        Guest_Mem *guest_mem_swap = (Guest_Mem *)swap_time;

        // 加这个判断是为了防止guest端应用被强退，内存被释放之后，这里再进行内存的写入，导致潜在的系统崩溃
        Thread_Context *thread_context = (Thread_Context *)context;
        if (thread_context->init != 0)
        {
            write_to_guest_mem(guest_mem_invoke, &invoke_time, 0, sizeof(int64_t));

            write_to_guest_mem(guest_mem_swap, &now_avg_swap_time, 0, sizeof(int64_t));
        }
    }

    // 计算帧率
    if (now_time - real_surface->last_calc_time > 1000000 && real_surface->last_calc_time != 0)
    {
        double hz = real_surface->now_screen_hz * 1000000.0 / (now_time - real_surface->last_calc_time);
        LOGD("%llx surface draw %.2lfHz", (uint64_t)real_surface, hz);
        real_surface->now_screen_hz = 0;

        real_surface->last_calc_time = now_time;
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
    if (real_surface != NULL)
    {
        real_surface->swap_interval = interval;
        return EGL_TRUE;
    }
    return EGL_BAD_SURFACE;
}

EGLBoolean d_eglBindTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return EGL_FALSE;
}

EGLBoolean d_eglReleaseTexImage(void *context, EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return EGL_FALSE;
}
