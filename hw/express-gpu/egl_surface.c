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
// #define TIMER_LOG
#include "express-gpu/egl_surface.h"
#include "express-gpu/egl_display.h"

#include "express-gpu/express_gpu_render.h"
#include "express-gpu/offscreen_render_thread.h"



EGL_Image *create_real_image(void *context, int width, int height);
void destroy_real_image(EGL_Image *real_image);


void egl_surface_swap_buffer(Double_Buffer *surface)
{

#ifdef DEBUG_INDEPEND_WINDOW
    glfwSwapBuffers(surface->window);
    return;
#endif
    // GLenum ret=glGetError();
    // if(ret!=GL_NO_ERROR){
    //     express_printf("swap before get gl error %x\n",ret);
    // }
    if (surface->config->sample_buffers_num != 0)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, surface->sampler_fbo[surface->now_draw]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->display_fbo[surface->now_draw]);
        glBlitFramebuffer(0, 0, surface->width, surface->height, 0, 0, surface->width, surface->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    //这句很重要，没了这个画不出来，这个是保证之前的绘制操作都针对原来的draw进行的
    GLsync wait_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
    //这句话让之前的画面都渲染出来
    // glFinish();
    // printf("child_windows gpu finish %lld\n",wait_sync);

    int now_draw_buffer = surface->now_draw;

    if (surface->fbo_sync[now_draw_buffer] != NULL)
    {
        // glDeleteSync(surface->fbo_sync[now_draw_buffer]);
        glDeleteSync(surface->fbo_sync[now_draw_buffer]);
    }
    surface->fbo_sync[now_draw_buffer] = wait_sync;

    //解除对当前绘制的缓冲区的锁定，这个时候这个缓冲区能够被使用
    ATOMIC_UNLOCK(surface->display_texture_is_use[now_draw_buffer]);

    surface->now_read = surface->now_draw;
    // surface->read_num=surface->draw_num;

    //垂直同步
    //刚开始要初始化
    if (surface->last_frame_num == -1)
    {
        surface->last_frame_num = draw_wait_GSYNC(surface->swap_event, -1);
    }
    else
    {
        int next_frame_num = (surface->last_frame_num + surface->swap_interval) % 65536;
        surface->last_frame_num = draw_wait_GSYNC(surface->swap_event, next_frame_num);
    }

    //尝试锁定下一个将要绘制的缓冲区
    int next_draw_buffer = (surface->now_draw + 1) % surface->buffer_num;
    ATOMIC_LOCK(surface->display_texture_is_use[next_draw_buffer]);
    surface->now_draw = next_draw_buffer;
    // surface->draw_num+=1;

    // printf("child_windows cpu wait %lld\n",surface->fbo_sync[next_draw_buffer]);

    TIMER_START(sync)
    if (surface->fbo_sync[next_draw_buffer] != 0)
    {
        glClientWaitSync(surface->fbo_sync[next_draw_buffer], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
    }
    TIMER_END(sync)
    TIMER_OUTPUT(sync, 100)
    // if(surface->fbo_used_type[next_draw_buffer]==SELF_USE){
    //     //根本不需要等待，因为它是被自己使用，所以不用担心延迟问题
    //     glWaitSync(surface->fbo_sync[next_draw_buffer],0,GL_TIMEOUT_IGNORED);
    //     // GLenum ret=glClientWaitSync(surface->fbo_sync[next_draw_buffer], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
    //     express_printf("direct draw  %u\n",surface->draw_num);
    // }else if(surface->fbo_used_type[next_draw_buffer]==OTHER_USE){
    // if(ret==GL_TIMEOUT_EXPIRED){
    //     express_printf("client wait timeout\n");
    // }
    // express_printf("need cpu wait %x %u\n",ret,surface->draw_num);GL_CONDITION_SATISFIED;
    // surface->fbo_used_type[next_draw_buffer]=SELF_USE;
    // }

    // //需要放弃gpu的时候，进行cpu等待，否则进行gpu等待
    // if(should_give_up_gpu()){
    //     //最多等待1s，这句是让host端窗口帧率优先得到保证的关键
    //     glClientWaitSync(surface->fbo_sync[next_draw_buffer], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
    // }else{
    //     glWaitSync(surface->fbo_sync[next_draw_buffer],0,GL_TIMEOUT_IGNORED);
    // }

    // glDeleteSync(surface->fbo_sync[next_draw_buffer]);
    // surface->fbo_sync[next_draw_buffer] = NULL;

    // express_printf("client paint fbo %u\n", surface->display_fbo[surface->now_draw]);
    if (surface->config->sample_buffers_num != 0)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->sampler_fbo[surface->now_draw]);
    }
    else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->display_fbo[surface->now_draw]);
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, surface->display_fbo[surface->now_read]);

    // gint64 now_time = g_get_real_time();
    // if(surface->last_gen_time!=0){
    //     surface->frame_gen_time=(int)(now_time - surface->last_gen_time);
    // }
    // surface->last_gen_time=now_time;

    // if (now_time - surface->last_swap_time > 1000000 && surface->last_swap_time != 0)
    // {
    //     surface->calc_hz += 1;
    //     surface->draw_hz = surface->calc_hz;
    //     express_printf("surface draw %dHz\n", surface->draw_hz);
    //     surface->calc_hz = 0;
    //     surface->last_swap_time = now_time;
    // }
    // else if (surface->last_swap_time == 0)
    // {
    //     surface->last_swap_time = now_time;
    //     surface->calc_hz = 0;
    // }
    // else
    // {
    //     surface->calc_hz += 1;
    // }
}

void create_fbo_texture(Double_Buffer *d_buffer, int index)
{

    EGLint internal_format = GL_RGB;
    EGLenum format = GL_RGB;
    EGLenum type = GL_UNSIGNED_BYTE;

    EGLenum depth_internal_format = 0;
    EGLenum stencil_internal_format = 0;

    EGLint red_bits = d_buffer->config->red_size;
    EGLint green_bits = d_buffer->config->green_size;
    EGLint blue_bits = d_buffer->config->blue_size;
    EGLint alpha_bits = d_buffer->config->alpha_size;
    EGLint stencil_bits = d_buffer->config->stencil_size;
    EGLint depth_bits = d_buffer->config->depth_size;
    EGLint need_sampler = d_buffer->config->sample_buffers_num;
    EGLint sampler_num = d_buffer->config->samples_per_pixel;

    express_printf("rgba %d %d %d %d ds %d %d MSAA %dX\n", red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits, sampler_num);

    // 2222
    // 3320
    // 4440
    // 4444
    // 5550
    // 5551
    // 5650
    // 8000
    // 8800
    // 8880
    // 8888
    // 1010100
    // 1010102
    // 1212120
    // 12121212
    // 1616160
    // 16161616
    if (red_bits == 2 && green_bits == 2 && blue_bits == 2 && alpha_bits == 2)
    {
        //2222
        internal_format = GL_RGBA2;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 2222 ");
    }
    else if (red_bits == 3 && green_bits == 3 && blue_bits == 2 && alpha_bits == 0)
    {
        //3320
        internal_format = GL_R3_G3_B2;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 3320 ");
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 0)
    {
        //4440
        internal_format = GL_RGB4;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 4440 ");
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 4)
    {
        //4444
        internal_format = GL_RGBA4;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 4444 ");
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 0)
    {
        //5550
        internal_format = GL_RGB5;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 5550 ");
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 1)
    {
        //5551
        internal_format = GL_RGB5_A1;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 5551 ");
    }
    else if (red_bits == 5 && green_bits == 6 && blue_bits == 5 && alpha_bits == 0)
    {
        //5650
        internal_format = GL_RGB565;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 5650 ");
    }
    else if (red_bits == 8 && green_bits == 0 && blue_bits == 0 && alpha_bits == 0)
    {
        //8000
        internal_format = GL_R8;
        format = GL_RED;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 8000 ");
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 0 && alpha_bits == 0)
    {
        //8800
        internal_format = GL_RG8;
        format = GL_RG;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 8800 ");
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 0)
    {
        //8880
        internal_format = GL_RGB8;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 8880 ");
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 8)
    {
        //8888
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        express_printf("choose rgba 8888 ");
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 0)
    {
        //1010100
        internal_format = GL_RGB10;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
        express_printf("choose rgba 1010100 ");
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 2)
    {
        //1010102
        internal_format = GL_RGB10_A2;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT_2_10_10_10_REV;
        express_printf("choose rgba 1010102 ");
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 0)
    {
        //1212120
        internal_format = GL_RGB12;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
        express_printf("choose rgba 1212120 ");
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 12)
    {
        //1212120
        internal_format = GL_RGBA12;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
        express_printf("choose rgba 12121212 ");
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 0)
    {
        //1616160
        internal_format = GL_RGB16;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
        express_printf("choose rgba 1616160 ");
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 16)
    {
        //1616160
        internal_format = GL_RGBA16;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
        express_printf("choose rgba 16161616 ");
    }
    else
    {
        express_printf("choose rgba default ");
    }

    // internal_format = GL_RG8;
    // format = GL_RG;
    // type = GL_UNSIGNED_BYTE;

    if (depth_bits == 16)
    {
        depth_internal_format = GL_DEPTH_COMPONENT16;
        express_printf("GL_DEPTH_COMPONENT16\n");
    }
    else if (depth_bits == 24)
    {
        depth_internal_format = GL_DEPTH_COMPONENT24;
        express_printf("GL_DEPTH_COMPONENT24\n");
    }
    else if (depth_bits == 32)
    {
        depth_internal_format = GL_DEPTH_COMPONENT32F;
        express_printf("GL_DEPTH_COMPONENT32F\n");
    }

    if (stencil_bits == 8)
    {
        stencil_internal_format = GL_STENCIL_INDEX8;
        express_printf("GL_STENCIL_INDEX8\n");
        if (depth_internal_format == GL_DEPTH_COMPONENT24)
        {
            depth_internal_format = GL_DEPTH24_STENCIL8;
            express_printf("GL_DEPTH24_STENCIL8\n");
        }
    }
    // depth_internal_format=0;
    // stencil_internal_format=0;

    glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture[index]);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, d_buffer->width, d_buffer->height, 0, format, type, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    static int max_sampler_num = -1;
    if (max_sampler_num == -1)
    {
        glGetInternalformativ(GL_RENDERBUFFER, GL_RGB, GL_SAMPLES, 1, &max_sampler_num);
    }
    if (max_sampler_num < sampler_num)
    {
        express_printf("over large sampler num %d max %d\n", sampler_num, max_sampler_num);
        sampler_num = max_sampler_num;
        d_buffer->config->samples_per_pixel = sampler_num;
        if (sampler_num == 0)
        {
            need_sampler = 0;
            d_buffer->config->sample_buffers_num = 0;
        }
    }

    if (need_sampler)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, d_buffer->sampler_rbo[index]);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, internal_format, d_buffer->width, d_buffer->height);
    }

    if (depth_internal_format != 0)
    {
        //这个相当于给与一个深度缓冲区，让这个fbo可以有颜色缓冲区，有深度缓冲区，模板缓冲区
        glBindRenderbuffer(GL_RENDERBUFFER, d_buffer->display_rbo_depth[index]);
        if (need_sampler)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, depth_internal_format, d_buffer->width, d_buffer->height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, depth_internal_format, d_buffer->width, d_buffer->height);
        }
    }

    //之所以当深度24模板8时要合并，是因为这样效率更高
    if (stencil_internal_format != 0 && depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, d_buffer->display_rbo_stencil[index]);
        if (need_sampler)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, stencil_internal_format, d_buffer->width, d_buffer->height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, stencil_internal_format, d_buffer->width, d_buffer->height);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->display_fbo[index]);
    //附加颜色缓冲区
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture[index], 0);

    //附加深度缓冲区
    if (need_sampler)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->sampler_fbo[index]);
        //附加颜色缓冲区
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, d_buffer->sampler_rbo[index]);
    }

    if (depth_internal_format == GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d_buffer->display_rbo_depth[index]);
    }
    else if (depth_internal_format != 0)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d_buffer->display_rbo_depth[index]);
    }
    //附加模板缓冲区
    if (stencil_internal_format != 0 && depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d_buffer->display_rbo_stencil[index]);
    }
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

    // d_buffer->config->sample_buffers_num = 1;
    // d_buffer->config->samples_per_pixel = 4;

    if (d_buffer->config->sample_buffers_num != 0)
    {
        //窗口不需要开启多采样，只需要fbo开启就行
        // glfwWindowHint(GLFW_SAMPLES, d_buffer->config->samples_per_pixel);
        glEnable(GL_MULTISAMPLE);
    }

    if (d_buffer->type == WINDOW_SURFACE)
    {
        //windows_surface是否应该使用三重缓冲?
        d_buffer->buffer_num = 3;
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
    glGenRenderbuffers(buffer_num, d_buffer->display_rbo_depth);
    glGenRenderbuffers(buffer_num, d_buffer->display_rbo_stencil);

    if (d_buffer->config->sample_buffers_num != 0)
    {
        glGenFramebuffers(buffer_num, d_buffer->sampler_fbo);
        glGenRenderbuffers(buffer_num, d_buffer->sampler_rbo);
    }

    for (int i = 0; i < buffer_num; i++)
    {
        create_fbo_texture(d_buffer, i);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //这里将读写的framebuffer分离，是为了readpixel时，能够从后缓冲区读取数据
    //（对于我们的程序，后缓冲区就是fbo_dispaly，而对于绑定fbo不为0时时会选择从read fbo读取，所以要这样把display-fbo设置为read）

    if (d_buffer->config->sample_buffers_num != 0)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, d_buffer->sampler_fbo[d_buffer->now_draw]);
    }
    else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, d_buffer->display_fbo[d_buffer->now_draw]);
    }
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
    surface->guest_native_window = NULL;

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

    surface->config = config_to_hints(config, &surface->window_hints);

    //创建真实的窗口
    render_windows_create(surface);
    // assert(surface->window != NULL);

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

    // if (surface->I_am_composer)
    // {
    //     set_compose_surface(NULL);
    // }

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

    express_printf("surface is composer %lx %lx\n", real_surface, surface);

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

    Double_Buffer *host_surface = (Double_Buffer *)g_hash_table_lookup(process_context->native_window_surface_map, GINT_TO_POINTER(win));
    if (host_surface == NULL)
    {
        host_surface = render_surface_create(config, attrib_list, WINDOW_SURFACE);
        host_surface->guest_native_window = win;
        g_hash_table_insert(process_context->native_window_surface_map, GINT_TO_POINTER(win), (gpointer)host_surface);
    }else{
        //新创建的时候，需要继承了原先的window
    }

    // express_printf("surface create %lx %lx\n", host_surface, guest_surface);
    g_hash_table_insert(process_context->surface_map, GINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Double_Buffer *real_surface = (Double_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));
    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    if (real_surface->guest_native_window == NULL)
    {
        render_surface_destroy(real_surface);
        g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
    }
    else
    {
        //有窗口连接的状态下，不删除surface，而是留下来，只把当前的映射取消，这样的话图像还能继续绘制到窗口上
        g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
    }
    express_printf("destroy surface %lx\n", surface);
    return EGL_TRUE;
}

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    return EGL_TRUE;
}

void d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list, EGLImage guest_image)
{
    //这里buffer和guest_image是一样的，都是gbuffer_id
    if (buffer != guest_image)
    {
        return;
    }

    if (attrib_list == NULL)
    {
        return;
    }

    uint64_t gbuffer_id = (uint64_t)buffer;
    Double_Buffer *surface = get_surface_from_gbuffer_id(gbuffer_id);
    if (surface != NULL)
    {
        return;
    }

    EGL_Image *real_image = get_image_from_gbuffer_id(gbuffer_id);
    if (real_image != NULL)
    {
        return;
    }

    //没有找到这个gbuffer_id说明这个gbuffer没有被用于创建surface，很可能是来着于合成器surface
    //所以手动给它创建一个image
    int width = 0;
    int height = 0;
    int i = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            height = attrib_list[i + 1];
            break;
        default:
            //todo 其他attrib属性的设置
            break;
        }
        i += 2;
    }

    real_image = create_real_image(context, width, height);

    set_image_gbuffer_id(real_image, gbuffer_id);
    return;
}

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{
    uint64_t gbuffer_id = (uint64_t)image;
    Double_Buffer *surface = get_surface_from_gbuffer_id(gbuffer_id);
    //这里都只是简单从map中移除，因为surface来自于ANativeWindow，它是仍然存在的，所以surface依然需要存在
    if (surface != NULL)
    {
        set_surface_gbuffer_id(NULL, gbuffer_id);
        return EGL_TRUE;
    }

    EGL_Image *real_image = get_image_from_gbuffer_id(gbuffer_id);
    //但是假如是这个image被销毁了，因为这个image来着于ANativeWindowBuffer，它销毁意味着buffer可能没了，所以也删除掉
    if (real_image != NULL)
    {
        destroy_real_image(real_image);
        set_image_gbuffer_id(NULL, gbuffer_id);
        return EGL_TRUE;
    }
    return EGL_FALSE;
}

EGL_Image *create_real_image(void *context, int width, int height)
{
    EGL_Image *real_image = g_malloc(sizeof(EGL_Image));

    GLuint pre_vbo;
    GLuint pre_texture;
    GLuint pre_fbo;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLuint *)&pre_vbo);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo);

    real_image->fbo_sync = NULL;
    real_image->fbo_sync_need_delete = NULL;
    real_image->display_texture_is_use = 0;

    glGenTextures(1, &(real_image->fbo_texture));
    glGenFramebuffers(1, &(real_image->display_fbo));
    //egl_image不需要深度缓冲和模板缓冲

    glBindTexture(GL_TEXTURE_2D, real_image->fbo_texture);
    glBindTexture(GL_ARRAY_BUFFER, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, real_image->display_fbo);
    //附加颜色缓冲区
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, real_image->fbo_texture, 0);

    //t需要还原原来绑定的texture和fbo
    glBindTexture(GL_TEXTURE_2D, pre_texture);
    glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    glBindFramebuffer(GL_FRAMEBUFFER, pre_fbo);

    return real_image;
}

void destroy_real_image(EGL_Image *real_image)
{
    glDeleteTextures(1, &(real_image->fbo_texture));
    glDeleteFramebuffers(1, &(real_image->display_fbo));
    if (real_image->fbo_sync != NULL)
    {
        glDeleteSync(real_image->fbo_sync);
    }
    return;
}