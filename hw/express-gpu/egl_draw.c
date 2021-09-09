#define STD_DEBUG_LOG

#include "express-gpu/egl_draw.h"
#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"

EGLBoolean d_eglMakeCurrent(void *context, EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx, uint64_t gbuffer_id)
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
        // if (thread_context->render_double_buffer_draw->I_am_composer)
        // {
        //     set_compose_surface(NULL);
        // }
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

    // if (real_surface_draw->has_connect_opengl == 0 && real_opengl_context->draw_surface != NULL)
    // {
    //     Double_Buffer *temp_surface = g_malloc(sizeof(Double_Buffer));
    //     memcpy(temp_surface, real_surface_draw, sizeof(Double_Buffer));
    //     PostMessage(draw_native_window, WM_USER_SURFACE_DESTROY, 0, (LPARAM)(temp_surface));


    //     memcpy(real_surface_draw, real_opengl_context->draw_surface, sizeof(Double_Buffer));
    //     g_free(real_opengl_context->draw_surface);
    //     real_opengl_context->draw_surface = real_surface_draw;
    // }else{

    // }


    //然后设置当前的surface和context
    thread_context->render_double_buffer_read = real_surface_read;
    real_surface_read->is_current = 1;
    thread_context->render_double_buffer_draw = real_surface_draw;
    real_surface_draw->is_current = 1;
    real_surface_draw->last_frame_num = -1;
    thread_context->opengl_context = real_opengl_context;
    real_opengl_context->is_current = 1;

    real_opengl_context->view_x = 0;
    real_opengl_context->view_y = 0;
    real_opengl_context->view_w = real_surface_draw->width;
    real_opengl_context->view_h = real_surface_draw->height;

    //设置gbuffer_id，用于找到它
    set_surface_gbuffer_id(real_surface_draw, gbuffer_id);

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

    if (real_surface_draw->config->sample_buffers_num != 0)
    {
        real_opengl_context->draw_fbo0 = real_surface_draw->sampler_fbo[real_surface_draw->now_draw];
    }
    else
    {
        real_opengl_context->draw_fbo0 = real_surface_draw->display_fbo[real_surface_draw->now_draw];
    }

    real_opengl_context->read_fbo0 = real_surface_read->display_fbo[real_surface_read->now_read];

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);

#ifdef DEBUG_INDEPEND_WINDOW
    //屏幕分离调试专用
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ShowWindow(real_surface_draw->window, TRUE);
#else
    if (real_surface_draw->I_am_composer)
    {
        set_compose_surface(real_surface_draw);
    }
#endif
    //makecurrent的时候要释放所有的锁，防止死锁，这个时候肯定没有swapbuffer，所以直接清空就行了
    memset(real_surface_draw->display_texture_is_use, 0, sizeof(real_surface_draw->display_texture_is_use));

    ATOMIC_LOCK(real_surface_draw->display_texture_is_use[real_surface_read->now_draw]);

    return EGL_TRUE;
}

EGLBoolean d_eglSwapBuffers_sync(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    // assert(surface > 1000);
    Double_Buffer *real_surface = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    // express_printf("swapbuffer %lx %lx\n", surface, real_surface);

    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    Opengl_Context *real_opengl_context = thread_context->opengl_context;

    egl_surface_swap_buffer(real_surface);
    if (real_surface->config->sample_buffers_num != 0)
    {
        real_opengl_context->draw_fbo0 = real_surface->sampler_fbo[real_surface->now_draw];
    }
    else
    {
        real_opengl_context->draw_fbo0 = real_surface->display_fbo[real_surface->now_draw];
    }
    real_opengl_context->read_fbo0 = real_surface->display_fbo[real_surface->now_read];
    return EGL_TRUE;
}

void d_eglQueueBuffer(void *context, EGLImage gbuffer_id)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Double_Buffer *real_surface = thread_context->render_double_buffer_draw;
    real_surface->guest_gbuffer_id = (uint64_t)gbuffer_id;

    //queuebuffer似乎不需要垂直同步

    //垂直同步
    //刚开始要初始化
    // if (real_surface->last_frame_num == -1)
    // {
    //     real_surface->last_frame_num = draw_wait_GSYNC(real_surface->swap_event, -1);
    // }
    // else
    // {
    //     int next_frame_num = (real_surface->last_frame_num + real_surface->swap_interval) % 65536;
    //     real_surface->last_frame_num = draw_wait_GSYNC(real_surface->swap_event, next_frame_num);
    // }
}

EGLBoolean d_eglSwapBuffers(void *context, EGLDisplay dpy, EGLSurface surface, int64_t invoke_time, int64_t *ret_invoke_time, int64_t *swap_time)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;
    Double_Buffer *real_surface = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    if (real_surface == NULL)
    {
        express_printf(RED("real surface is null!"));
        express_printf("surface %lx real %lx dpy %lx invoke_time %lld\n", surface, real_surface, dpy, invoke_time);
        return EGL_FALSE;
        // Guest_Mem *guest_mem_invoke = (Guest_Mem *)ret_invoke_time;
        // Guest_Mem *guest_mem_swap = (Guest_Mem *)swap_time;
        // int64_t a,b;
        // guest_write(guest_mem_invoke, &a, 0, sizeof(EGLint));
        // guest_write(guest_mem_swap, &b, 0, sizeof(EGLint));
        // express_printf("invoke time %lld swap_time %lld\n",a,b);
    }

    gint64 start_time = g_get_real_time();
    EGLBoolean ret = d_eglSwapBuffers_sync(context, dpy, surface);
    gint64 end_time = g_get_real_time();
    gint64 now_swap_time = end_time - start_time;

    if (real_surface->swap_time_cnt < 20)
    {
        real_surface->swap_time[real_surface->swap_loc] = now_swap_time;
        real_surface->swap_loc = (real_surface->swap_loc + 1) % 20;
        real_surface->swap_time_all += now_swap_time;
        real_surface->swap_time_cnt++;
    }
    else
    {
        real_surface->swap_time_all -= real_surface->swap_time[real_surface->swap_loc];
        real_surface->swap_time_all += now_swap_time;
        real_surface->swap_time[real_surface->swap_loc] = now_swap_time;
        real_surface->swap_loc = (real_surface->swap_loc + 1) % 20;
    }
    // if(real_surface->swap_loc==0){
    //     express_printf("avg swap time %lld\n",real_surface->swap_time_all/real_surface->swap_time_cnt);
    // }

    gint64 now_avg_swap_time = real_surface->swap_time_all / real_surface->swap_time_cnt;

    //保证这个swap_time不为0，方便guest判断是否有返回
    if (real_surface->swap_time_cnt <= 10)
    {
        now_avg_swap_time = -1;
    }

    if (ret == EGL_TRUE)
    {

        // GLint now_flag_cnt = 0;
        Guest_Mem *guest_mem_invoke = (Guest_Mem *)ret_invoke_time;
        Guest_Mem *guest_mem_swap = (Guest_Mem *)swap_time;

        //加这个判断是为了防止guest端应用被强退，内存被释放之后，这里再进行内存的写入，导致潜在的系统崩溃
        Thread_Context *thread_context = (Thread_Context *)context;
        if (thread_context->init != 0)
        {
            // guest_write(guest_mem, &now_flag_cnt, 0, sizeof(EGLint));

            // now_flag_cnt = (now_flag_cnt + 1) % 1024;

            // EGLint swap_time = (EGLint)(real_surface->frame_gen_time);

            guest_read(guest_mem_invoke, &invoke_time, 0, sizeof(int64_t));

            guest_read(guest_mem_swap, &now_avg_swap_time, 0, sizeof(int64_t));
        }
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

//EGLClientBuffer d_eglGetNativeClientBufferANDROID(void *context, AHardwareBuffer *buffer);
