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

EGL_Image *create_real_image(void *context, uint64_t g_buffer_id, int format, int stride, int width, int height);
void connect_fbo_texture(Window_Buffer *d_buffer, int index, int new);

void egl_surface_swap_buffer(Window_Buffer *surface)
{

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
    // printf("unlock on read %llx swap %d ",surface,now_draw_buffer);
    // ATOMIC_UNLOCK(surface->display_texture_is_use[now_draw_buffer]);
    ATOMIC_SET_UNUSED(surface->display_texture_is_use[now_draw_buffer]);

#ifdef DEBUG_INDEPEND_WINDOW
    glBindFramebuffer(GL_READ_FRAMEBUFFER, surface->display_fbo[now_draw_buffer]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, surface->width, surface->height, 0, 0, surface->width, surface->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glfwSwapBuffers(surface->creater_window);
#endif

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
    //假如下一个是被锁定的状态的话，就给下下个缓冲区，被锁定一定是在被读取中，只会有一个，所以下下个肯定没有锁定
    //这样能够减少自旋空转时间，增加绘制效率
    if (surface->display_texture_is_use[next_draw_buffer] == 1)
    {
        next_draw_buffer = (next_draw_buffer + 1) % surface->buffer_num;
    }
    assert(surface->display_texture_is_use[next_draw_buffer] == 0);
    // printf("lock on read %llx swap %d ",surface,next_draw_buffer);

    // ATOMIC_LOCK(surface->display_texture_is_use[next_draw_buffer]);
    ATOMIC_SET_UNUSED(surface->display_texture_is_use[next_draw_buffer]);

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

void connect_fbo_texture(Window_Buffer *d_buffer, int index, int new)
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

    // express_printf("rgba %d %d %d %d ds %d %d MSAA %dX\n", red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits, sampler_num);

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
        // express_printf("choose rgba 2222 ");
    }
    else if (red_bits == 3 && green_bits == 3 && blue_bits == 2 && alpha_bits == 0)
    {
        //3320
        internal_format = GL_R3_G3_B2;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 3320 ");
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 0)
    {
        //4440
        internal_format = GL_RGB4;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 4440 ");
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 4)
    {
        //4444
        internal_format = GL_RGBA4;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 4444 ");
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 0)
    {
        //5550
        internal_format = GL_RGB5;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 5550 ");
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 1)
    {
        //5551
        internal_format = GL_RGB5_A1;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 5551 ");
    }
    else if (red_bits == 5 && green_bits == 6 && blue_bits == 5 && alpha_bits == 0)
    {
        //5650
        internal_format = GL_RGB565;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 5650 ");
    }
    else if (red_bits == 8 && green_bits == 0 && blue_bits == 0 && alpha_bits == 0)
    {
        //8000
        internal_format = GL_R8;
        format = GL_RED;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 8000 ");
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 0 && alpha_bits == 0)
    {
        //8800
        internal_format = GL_RG8;
        format = GL_RG;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 8800 ");
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 0)
    {
        //8880
        internal_format = GL_RGB8;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 8880 ");
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 8)
    {
        //8888
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        // express_printf("choose rgba 8888 ");
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 0)
    {
        //1010100
        internal_format = GL_RGB10;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
        // express_printf("choose rgba 1010100 ");
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 2)
    {
        //1010102
        internal_format = GL_RGB10_A2;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT_2_10_10_10_REV;
        // express_printf("choose rgba 1010102 ");
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 0)
    {
        //1212120
        internal_format = GL_RGB12;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
        // express_printf("choose rgba 1212120 ");
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 12)
    {
        //1212120
        internal_format = GL_RGBA12;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
        // express_printf("choose rgba 12121212 ");
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 0)
    {
        //1616160
        internal_format = GL_RGB16;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
        // express_printf("choose rgba 1616160 ");
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 16)
    {
        //1616160
        internal_format = GL_RGBA16;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
        // express_printf("choose rgba 16161616 ");
    }
    else
    {
        // express_printf("choose rgba default ");
    }

    printf("%llx surface choose red %d green %d blue %d alpha %d depth %d stencil %d\n", d_buffer, red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits);
    // internal_format = GL_RG8;
    // format = GL_RG;
    // type = GL_UNSIGNED_BYTE;

    if (depth_bits == 16)
    {
        depth_internal_format = GL_DEPTH_COMPONENT16;
        // express_printf("GL_DEPTH_COMPONENT16\n");
    }
    else if (depth_bits == 24)
    {
        depth_internal_format = GL_DEPTH_COMPONENT24;
        // express_printf("GL_DEPTH_COMPONENT24\n");
    }
    else if (depth_bits == 32)
    {
        depth_internal_format = GL_DEPTH_COMPONENT32F;
        // express_printf("GL_DEPTH_COMPONENT32F\n");
    }

    if (stencil_bits == 8)
    {
        stencil_internal_format = GL_STENCIL_INDEX8;
        // express_printf("GL_STENCIL_INDEX8\n");
        if (depth_internal_format == GL_DEPTH_COMPONENT24)
        {
            depth_internal_format = GL_DEPTH24_STENCIL8;
            // express_printf("GL_DEPTH24_STENCIL8\n");
        }
    }
    // depth_internal_format=0;
    // stencil_internal_format=0;

    glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture[index]);

    if (new == 1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, d_buffer->width, d_buffer->height, 0, format, type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

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
 * @brief 利用windows初始化surface
 * 
 * @param d_buffer 需要初始化的surface
 * @return int 返回1则创建成功，返回0则创建失败 
 */
int egl_surface_init(Window_Buffer *d_buffer, GLFWwindow *now_window, int need_draw)
{
    if (d_buffer == NULL || now_window == NULL)
    {
        return 0;
    }

    int buffer_num = d_buffer->buffer_num;

    if (d_buffer->creater_window == NULL)
    {
        //creater_window等于空意味着底下各种资源之前都没申请过，因此需要申请

        d_buffer->creater_window = now_window;

        d_buffer->swap_event = CreateEvent(NULL, FALSE, FALSE, NULL);

        if (d_buffer->config->sample_buffers_num != 0)
        {
            //窗口不需要开启多采样，只需要fbo开启就行
            // glfwWindowHint(GLFW_SAMPLES, d_buffer->config->samples_per_pixel);
            glEnable(GL_MULTISAMPLE);
        }
        else
        {
            glDisable(GL_MULTISAMPLE);
        }

        glGenFramebuffers(buffer_num, d_buffer->display_fbo);

        //默认情况下read_fbo直接是dispaly_fbo
        d_buffer->reader_window = now_window;
        memcpy(d_buffer->read_fbo, d_buffer->display_fbo, sizeof(d_buffer->read_fbo));

        glGenTextures(buffer_num, d_buffer->fbo_texture);
        glGenRenderbuffers(buffer_num, d_buffer->display_rbo_depth);
        glGenRenderbuffers(buffer_num, d_buffer->display_rbo_stencil);

        if (d_buffer->config->sample_buffers_num != 0)
        {
            glGenFramebuffers(buffer_num, d_buffer->sampler_fbo);
            glGenRenderbuffers(buffer_num, d_buffer->sampler_rbo);
        }

        for (int i = 0; i < buffer_num; i++)
        {
            connect_fbo_texture(d_buffer, i, 1);
        }

        //新创建的surface默认绑定到framebuffer 0上，而且其他绑定状态要取消
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        //底下相当于绑定到framebuffer 0上
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
    }
    else if (d_buffer->creater_window == now_window)
    {
        //和之前的一样，说明之前已经申请好了，这个时候只是makecurrent一下，资源都不用生成
        if (need_draw == 0)
        {
            //但是假如这个被用来读取，则必须要保证用来读取的fbo是当前context生成的
            if (d_buffer->reader_window != now_window)
            {
                //不等于的情况下，必须生成新的fbo，并且连接到texture上
                d_buffer->reader_window = now_window;
                glGenFramebuffers(buffer_num, d_buffer->read_fbo);

                for (int i = 0; i < buffer_num; i++)
                {
                    //读取只需要读取颜色缓冲区
                    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->read_fbo[i]);
                    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture[i], 0);
                }
            }
        }
        return 1;
    }
    else
    {
        //else的情况相当与有creater_window，但是这个create_window是其他的，不是当前这个
        //假如这个surface用来draw，则重新生成fbo，否则不重新生成fbo
        if (need_draw)
        {
            //原先的老的fbo咱们也没法操作它，不过要是原先的窗口释放了，它就自动释放了，因此暂时不管它，直接覆盖
            //而且这种时候surface肯定不会被画，所以重新生成不会产生冲突
            //不用担心creater_window内存泄露的问题，因为它是由opengl_context来释放的

            d_buffer->creater_window = now_window;

            glGenFramebuffers(buffer_num, d_buffer->display_fbo);
            if (d_buffer->config->sample_buffers_num != 0)
            {
                glGenFramebuffers(buffer_num, d_buffer->sampler_fbo);
            }

            for (int i = 0; i < buffer_num; i++)
            {
                connect_fbo_texture(d_buffer, i, 0);
            }
        }
        else
        {
            //为read的情况
            if (d_buffer->reader_window != now_window)
            {
                //不等于的情况下，必须生成新的fbo，并且连接到texture上
                d_buffer->reader_window = now_window;
                glGenFramebuffers(buffer_num, d_buffer->read_fbo);

                for (int i = 0; i < buffer_num; i++)
                {
                    //读取只需要读取颜色缓冲区
                    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->read_fbo[i]);
                    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture[i], 0);
                }
            }
            else
            {
                //现在的read_fbo就是当前窗口生成的，则不需要额外操作，read_fbo已经是绑定好了的
            }
        }
    }

    // //屏幕分离调试专用
    // #ifdef DEBUG_INDEPEND_WINDOW
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // #endif

    return 1;
}

Window_Buffer *render_surface_create(EGLConfig config, int width, int height, int type)
{
    //@todo 处理config、处理attrib_list

    //这里先根据attrb_list获取窗口的宽和高

    Window_Buffer *surface = g_malloc(sizeof(Window_Buffer));
    memset(surface, 0, sizeof(Window_Buffer));
    surface->type = type;
    surface->width = width;
    surface->height = height;
    surface->swap_interval = 1;
    surface->guest_native_window = NULL;
    surface->guest_gbuffer_id = 0;
    surface->now_acquired = -1;

    if (surface->type == WINDOW_SURFACE)
    {
        //windows_surface使用三重缓冲，这样当其中一个缓冲区被合成器锁定使用时，应用还能使用另外两个缓冲区进行交换绘制，不会卡住等待缓冲区释放
        surface->buffer_num = 3;
        surface->now_read = 0;
        surface->now_draw = 1;
    }
    else if (surface->type == P_SURFACE)
    {
        //pbuffer只有单缓冲区
        surface->buffer_num = 1;
        surface->now_draw = 0;
        surface->now_read = 0;
    }

    // int i = 0;
    // while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    // {
    //     switch (attrib_list[i])
    //     {
    //     case EGL_WIDTH:
    //         surface->width = attrib_list[i + 1];
    //         break;
    //     case EGL_HEIGHT:
    //         surface->height = attrib_list[i + 1];
    //         break;
    //     default:
    //         //todo 其他attrib属性的设置
    //         break;
    //     }
    //     i += 2;
    // }

    surface->config = config_to_hints(config, &surface->window_hints);

    return surface;
}

int render_surface_destroy(Window_Buffer *surface)
{

    if (surface == NULL)
    {
        return 0;
    }

    if (surface->is_current)
    {
        surface->need_destroy = 1;
    }
    else
    {

        //没有makecurrent的时候这些资源肯定没有被使用，但是这个时候也不能调用glDelete等函数，因为可能当前没有makecurrent，也就是没有opengl的环境
        //所以这里让主线程来清空数据
        //为什么不直接调用glfwDestroyWindow自动清空资源？因为部分共享资源不会被清空，需要手动清空
        // PostMessage(draw_native_window, WM_USER_SURFACE_DESTROY, 0, (LPARAM)surface);
        send_message_to_main_window(MAIN_DESTROY_SURFACE, surface);
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

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));

    express_printf("surface is composer %lx %lx\n", real_surface, surface);
    // static int has_pbuffer_composer = 0;
    // if (real_surface->type == P_SURFACE)
    // {
    //     has_pbuffer_composer = 1;
        // real_surface->I_am_composer = 1;
    // }

    // if (has_pbuffer_composer == 1)
    // {
    //     return;
    // }
    real_surface->I_am_composer = 1;
}

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    int i = 0;
    int width = 0;
    int height = 0;
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

    EGLSurface host_surface = (EGLSurface)render_surface_create(config, width, height, P_SURFACE);

    g_hash_table_insert(process_context->surface_map, GINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *host_surface = (Window_Buffer *)g_hash_table_lookup(process_context->native_window_surface_map, GINT_TO_POINTER(win));

    eglConfig *now_eglconfig = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GINT_TO_POINTER(config));

    int i = 0;
    int width = 0;
    int height = 0;
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

    // printf("host config %lx guest config %lx surface config %lx\n",now_eglconfig,config,host_surface==NULL?0:host_surface->config);

    //现实中发现，同一个win，也可能出现surface的长和宽不一样的情况，所以这里也进行比较
    if (host_surface == NULL || now_eglconfig != host_surface->config || width != host_surface->width || height != host_surface->height)
    {
        if (host_surface != NULL)
        {
            //长宽高只与ANativeWindow相关，但是其他配置可能会不一样，假如配置不一样，就需要重新生成
            //这里不需要从native_window_surface_map中remove，因为下面会insert更新相关的值

            //这个surface肯定没有被makecurrent，所以这里应该直接删除，但是也判断下防止意外发生
            if (host_surface->is_current == 1)
            {
                //这个调试时使用，防止有意外发生
                printf("errro! Same ANativeWindow create different surface and origin surface is current!!!");
            }

            //不需要手动destroy，因为native_window_surface_map带有默认销毁函数，所以在覆盖时会先调用销毁函数再覆盖
            // render_surface_destroy(host_surface);
        }
        if (host_surface != NULL && (now_eglconfig != host_surface->config || width != host_surface->width || height != host_surface->height))
        {
            express_printf("config change %lx host surface%lx width %d height %d => width %d height %d\n", now_eglconfig, host_surface->config, host_surface->width, host_surface->height, width, height);
            // assert(0);
        }
        host_surface = render_surface_create(config, width, height, WINDOW_SURFACE);
        printf("create surface %llx ( use win %llx )\n", host_surface, win);
        host_surface->guest_native_window = win;
        g_hash_table_insert(process_context->native_window_surface_map, GINT_TO_POINTER(win), (gpointer)host_surface);
    }
    else
    {
        //假如surface之前已经有了，而且配置一样，也就是这个surface是使用的先用的ANativeWindow，则不进行创建操作，直接返回这个surface就行
    }

    express_printf("surface create host %llx guest %llx width %d height %d guest width %d height %d\n", host_surface, guest_surface, host_surface->width, host_surface->height, width, height);
    g_hash_table_insert(process_context->surface_map, GINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GINT_TO_POINTER(surface));
    printf("destroy surface %llx\n", real_surface);
    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }
    if (real_surface->type == P_SURFACE)
    {
        //PBuffer就直接删除了，反正也没有连接屏幕缓冲区
        //会调用到pbuffer的删除函数g_p_surface_map_destroy
        g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
    }
    else
    {
        //有窗口连接的状态下，不删除surface，而是留下来，只把当前的映射取消，这样的话图像还能继续绘制到窗口上
        //会调用到pbuffer的删除函数g_p_surface_map_destroy
        g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
    }
    express_printf("destroy surface host %lx guest %lx\n", real_surface, surface);
    return EGL_TRUE;
}

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    return EGL_TRUE;
}

EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list, EGLImage guest_image)
{
    //创建image，要么是使用别的应用绘制使用的缓冲区，要么是新创建的缓冲区
    //前者之前肯定有surface连接，所以肯定找得到，后者不会找得到，必须得给手动建立一个

    //这里buffer和guest_image是一样的，都是gbuffer_id
    if (buffer != guest_image)
    {
        return -1;
    }

    if (attrib_list == NULL)
    {
        return -1;
    }

    int width = 0;
    int height = 0;
    int format = 0;
    int stride = 0;
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
        case EGL_TEXTURE_FORMAT:
            format = attrib_list[i + 1];
        case EGL_BUFFER_SIZE:
            //用buffer_size作为对齐选项
            stride = attrib_list[i + 1];
        default:
            //todo 其他attrib属性的设置
            break;
        }
        i += 2;
    }

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    uint64_t gbuffer_id = (uint64_t)buffer;
    Window_Buffer *surface = get_surface_from_gbuffer_id(gbuffer_id);
    if (surface != NULL)
    {
        printf("#%llx create image from surface %llx\n", thread_context == NULL ? NULL : thread_context->opengl_context, surface);
        return 1;
    }

    //image有可能会发生大小格式的改变
    EGL_Image *real_image = get_image_from_gbuffer_id(gbuffer_id);
    if (real_image != NULL && real_image->height == height && real_image->width == width && real_image->origin_format == format)
    {
        printf("#%llx create image from image %llx\n", thread_context == NULL ? NULL : thread_context->opengl_context, real_image);
        real_image->display_texture_is_use = 0;
        if (real_image->host_has_data == 1)
        {
            return 1;
        }
        else
        {
            //只有当host这边没有保存数据的时候才能返回0，这样会从guest端的GraphicBuffer里进行读取
            return 0;
        }
    }

    if (real_image != NULL)
    {
        printf("image change %d %d => %d %d\n", real_image->width, real_image->height, width, height);
    }

    //没有找到这个gbuffer_id说明这个gbuffer没有被用于创建surface，而且之前也没有出现过，很可能是来着于合成器surface
    //所以手动给它创建一个image

    real_image = create_real_image(context, gbuffer_id, format, stride, width, height);

    Process_Context *process_context = thread_context->process_context;
    express_printf("#%llx create image, gbuffer_id %llx, image %llx, width %d height %d texture %u time %lld\n", thread_context->opengl_context, gbuffer_id, guest_image, width, height, real_image->fbo_texture, g_get_real_time());

    g_hash_table_insert(process_context->gbuffer_image_map, GINT_TO_POINTER(gbuffer_id), (gpointer)real_image);

    set_image_gbuffer_id(NULL, real_image, gbuffer_id);
    return 0;
}

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{
    uint64_t gbuffer_id = (uint64_t)image;
    Window_Buffer *surface = get_surface_from_gbuffer_id(gbuffer_id);

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    EGL_Image *real_image = get_image_from_gbuffer_id(gbuffer_id);
    printf("%llx destroy image gbuffer_id %llx surface %llx image %llx\n", thread_context->opengl_context, image, surface, real_image);
    //这里不从map中移除，因为surface来自于ANativeWindow，只要应用没挂，它是仍然存在的，所以surface依然需要保持着映射
    if (surface != NULL && real_image == NULL)
    {
        // //后面也要确保real_image为null，是因为测试时gbuffer_id为1可能同时存在surface和image，当id为1的surface加入时，真正正在锁定的image可能无法释放
        // surface->ref_cnt -= 1;
        // if(surface->ref_cnt == 0){
        //     set_surface_gbuffer_id(NULL, gbuffer_id);
        //     // if(surface->need_destroy == 1){
        //     //     g_hash_table_remove(process_context->surface_map, GINT_TO_POINTER(surface));
        //     // }
        // }
        return EGL_TRUE;
    }

    //根据framework代码来看，每次queuebuffer后都会创建一次image，删除一次image，但是gbuffer都会存在，所以只有进程终止了之后才能删除它
    // printf("destroy image %lx\n",real_image);
    if (real_image != NULL)
    {
        if (real_image->is_lock)
        {
            release_texture_from_image(real_image);
        }
        // g_hash_table_remove(process_context->gbuffer_image_map, GINT_TO_POINTER(gbuffer_id));
        // 上面的remove函数的销毁函数会回收内存，调用下面两个函数，所以下面就注释了
        // destroy_real_image(real_image);
        // set_image_gbuffer_id(NULL, gbuffer_id);
        return EGL_TRUE;
    }
    return EGL_FALSE;
}

EGL_Image *create_real_image(void *context, uint64_t g_buffer_id, int format, int stride, int width, int height)
{
    // createimage的时候，是否有openglcontext状态？假如没有的话是否应该延迟到使用的时候？
    // 实际上systemui就会在没有context的情况下调用createimage
    EGL_Image *real_image = g_malloc(sizeof(EGL_Image));
    memset(real_image, 0, sizeof(EGL_Image));

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    int should_init = 0;
    if (thread_context->opengl_context != NULL)
    {
        should_init = 1;
    }
    else
    {
        printf("shoud not init\n");
    }

    GLuint pre_vbo;
    GLuint pre_texture;
    // GLuint pre_fbo;

    if (should_init == 1)
    {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLuint *)&pre_vbo);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
        // glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo);
    }

    real_image->fbo_sync = NULL;
    real_image->fbo_sync_need_delete = NULL;
    real_image->display_texture_is_use = 0;

    real_image->is_lock = 0;
    real_image->width = width;
    real_image->height = height;
    real_image->stride = stride;

    real_image->gbuffer_id = g_buffer_id;

    real_image->fbo_texture = 0;
    real_image->display_fbo = 0;

    real_image->origin_format = format;

    if (format == HAL_PIXEL_FORMAT_RGBA_8888 || format == HAL_PIXEL_FORMAT_RGBX_8888)
    {
        //根据鼠标显示来看，8888的情况下内存布局有反向
        real_image->internal_format = GL_RGBA8;
        real_image->format = GL_RGBA;
        real_image->pixel_type = GL_UNSIGNED_BYTE;
        // real_image->pixel_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        real_image->row_byte_len = width * 4;
    }
    else if (format == HAL_PIXEL_FORMAT_BGRA_8888)
    {
        printf("EGLImage with g_buffer_id %llx need format BGRA_8888!!!\n", (uint64_t)g_buffer_id);
        real_image->internal_format = GL_RGBA8;
        real_image->format = GL_BGRA;
        real_image->pixel_type = GL_UNSIGNED_INT_8_8_8_8;
        real_image->row_byte_len = width * 4;
    }
    else if (format == HAL_PIXEL_FORMAT_RGB_888)
    {
        real_image->internal_format = GL_RGB8;
        real_image->format = GL_RGB;
        real_image->pixel_type = GL_UNSIGNED_INT;
        real_image->row_byte_len = width * 3;
    }
    else if (format == HAL_PIXEL_FORMAT_RGB_565)
    {
        //根据视频播放来看，565的情况下内存没有反向
        real_image->internal_format = GL_RGB565;
        real_image->format = GL_RGB;
        real_image->pixel_type = GL_UNSIGNED_SHORT_5_6_5;
        real_image->row_byte_len = width * 2;
    }
    else
    {
        real_image->internal_format = GL_RGBA8;
        real_image->format = GL_RGBA;
        real_image->pixel_type = GL_UNSIGNED_INT;
        real_image->row_byte_len = width * 4;
        printf("error! unknown EGLImage format %d!!!\n", format);
    }

    if (should_init == 1)
    {
        glGenTextures(1, &(real_image->fbo_texture));
        //egl_image不需要深度缓冲和模板缓冲

        glBindTexture(GL_TEXTURE_2D, real_image->fbo_texture);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, real_image->internal_format, width, height, 0, real_image->format, real_image->pixel_type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // glGenFramebuffers(1, &(real_image->display_fbo));
        // glBindFramebuffer(GL_FRAMEBUFFER, real_image->display_fbo);
        // //附加颜色缓冲区
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, real_image->fbo_texture, 0);

        //需要还原原来绑定的texture和fbo
        glBindTexture(GL_TEXTURE_2D, pre_texture);
        glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
        // glBindFramebuffer(GL_FRAMEBUFFER, pre_fbo);
    }
    return real_image;
}

void destroy_real_image(EGL_Image *real_image)
{
    if(real_image->fbo_texture != 0)
    {
        glDeleteTextures(1, &(real_image->fbo_texture));
    }

    if(real_image->display_fbo != 0)
    {
        glDeleteFramebuffers(1, &(real_image->display_fbo));
        glDeleteFramebuffers(1, &(real_image->display_fbo_reverse));
        
        glDeleteTextures(1, &(real_image->fbo_texture_reverse));
    }

    if (real_image->fbo_sync != NULL)
    {
        glDeleteSync(real_image->fbo_sync);
    }
    if (real_image->fbo_sync_need_delete != NULL)
    {
        glDeleteSync(real_image->fbo_sync_need_delete);
    }
    g_free(real_image);
    return;
}