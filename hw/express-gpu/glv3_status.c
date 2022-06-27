#define STD_DEBUG_LOG
// #define TIMER_LOG
#include "express-gpu/glv3_status.h"
// #include "express-gpu/glv3_resource.h"

#include "glad/glad.h"
#include "express-gpu/egl_window.h"
#include "express-gpu/offscreen_render_thread.h"




void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{
    GLuint draw_fbo0 = ((Opengl_Context *)context)->draw_fbo0;
    GLuint read_fbo0 = ((Opengl_Context *)context)->read_fbo0;

    // glFlush();
    // glFinish();
    if (framebuffer == 0)
    {
        if (target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            // printf("conetxt %llx bind 0 framebuffer draw %u\n",context, draw_fbo0);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo0);
        }
        if (target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            // printf("conetxt %llx bind 0 framebuffer read %u\n",context, read_fbo0);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo0);
        }
    }
    else
    {
        glBindFramebuffer(target, framebuffer);
        // GLenum status = glCheckFramebufferStatus(target) ;
        // printf("conetxt %llx bind framebuffer %u status %x\n",context, framebuffer, status);
    }

    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);
}

void d_glBindBuffer_special(void *context, GLenum target, GLuint buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);
    
    switch (target)
    {
    case GL_ARRAY_BUFFER:
        status->guest_array_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_array_buffer = buffer;
        }
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        status->guest_element_array_buffer = buffer;
        opengl_context->bound_buffer_status.attrib_point->element_array_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_element_array_buffer = buffer;
        }
        break;
    case GL_COPY_READ_BUFFER:
        status->guest_copy_read_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_copy_read_buffer = buffer;
        }
        break;
    case GL_COPY_WRITE_BUFFER:
        status->guest_copy_write_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_copy_write_buffer = buffer;
        }
        break;
    case GL_PIXEL_PACK_BUFFER:
        status->guest_pixel_pack_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_pixel_pack_buffer = buffer;
        }
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        status->guest_pixel_unpack_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_pixel_unpack_buffer = buffer;
        }
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->guest_transform_feedback_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_transform_feedback_buffer = buffer;
        }
        break;
    case GL_UNIFORM_BUFFER:
        status->guest_uniform_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_uniform_buffer = buffer;
        }
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->guest_atomic_counter_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_atomic_counter_buffer = buffer;
        }
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        status->guest_dispatch_indirect_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_dispatch_indirect_buffer = buffer;
        }
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        status->guest_draw_indirect_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_draw_indirect_buffer = buffer;
        }
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->guest_shader_storage_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_shader_storage_buffer = buffer;
        }
        break;
    case GL_TEXTURE_BUFFER:
        status->guest_texture_buffer = buffer;
        if(host_opengl_version < 45)
        {
            status->host_texture_buffer = buffer;
        }
        break;
    default:
        printf("error target %x buffer %d glBindBuffer\n", target, buffer);
        break;
    }

    express_printf("context %llx glBindBuffer target %x buffer %d\n",context, target, buffer);

    if(host_opengl_version < 45)
    {
        glBindBuffer(target, buffer);
    }

}


void buffer_binding_status_sync(void *context, GLenum target)
{
    int need_sync = 0;
    GLuint new_buffer = 0;

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        if(status->guest_array_buffer != status->host_array_buffer)
        {
            status->host_array_buffer = status->guest_array_buffer;
            new_buffer = status->host_array_buffer;
            need_sync = 1;
        }
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        if(status->guest_element_array_buffer != status->host_element_array_buffer)
        {
            status->host_element_array_buffer = status->guest_element_array_buffer;
            new_buffer = status->host_element_array_buffer;
            need_sync = 1;
        }
        break;
    case GL_COPY_READ_BUFFER:
        if(status->guest_copy_read_buffer != status->host_copy_read_buffer)
        {
            status->host_copy_read_buffer = status->guest_copy_read_buffer;
            new_buffer = status->host_copy_read_buffer;
            need_sync = 1;
        }
        break;
    case GL_COPY_WRITE_BUFFER:
        if(status->guest_copy_write_buffer != status->host_copy_write_buffer)
        {
            status->host_copy_write_buffer = status->guest_copy_write_buffer;
            new_buffer = status->host_copy_write_buffer;
            need_sync = 1;
        }
        break;
    case GL_PIXEL_PACK_BUFFER:
        if(status->guest_pixel_pack_buffer != status->host_pixel_pack_buffer)
        {
            status->host_pixel_pack_buffer = status->guest_pixel_pack_buffer;
            new_buffer = status->host_array_buffer;
            need_sync = 1;
        }
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        if(status->guest_pixel_unpack_buffer != status->host_pixel_unpack_buffer)
        {
            status->host_pixel_unpack_buffer = status->guest_pixel_unpack_buffer;
            new_buffer = status->host_pixel_unpack_buffer;
            need_sync = 1;
        }
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        if(status->guest_transform_feedback_buffer != status->host_transform_feedback_buffer)
        {
            status->host_transform_feedback_buffer = status->guest_transform_feedback_buffer;
            new_buffer = status->host_transform_feedback_buffer;
            need_sync = 1;
        }
        break;
    case GL_UNIFORM_BUFFER:
        if(status->guest_uniform_buffer != status->host_uniform_buffer)
        {
            status->host_uniform_buffer = status->guest_uniform_buffer;
            new_buffer = status->host_uniform_buffer;
            need_sync = 1;
        }
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        if(status->guest_atomic_counter_buffer != status->host_atomic_counter_buffer)
        {
            status->host_atomic_counter_buffer = status->guest_atomic_counter_buffer;
            new_buffer = status->host_atomic_counter_buffer;
            need_sync = 1;
        }
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        if(status->guest_dispatch_indirect_buffer != status->host_dispatch_indirect_buffer)
        {
            status->host_dispatch_indirect_buffer = status->guest_dispatch_indirect_buffer;
            new_buffer = status->host_dispatch_indirect_buffer;
            need_sync = 1;
        }
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        if(status->guest_draw_indirect_buffer != status->host_draw_indirect_buffer)
        {
            status->host_draw_indirect_buffer = status->guest_draw_indirect_buffer;
            new_buffer = status->host_draw_indirect_buffer;
            need_sync = 1;
        }
        break;
    case GL_SHADER_STORAGE_BUFFER:
        if(status->guest_shader_storage_buffer != status->host_shader_storage_buffer)
        {
            status->host_shader_storage_buffer = status->guest_shader_storage_buffer;
            new_buffer = status->host_shader_storage_buffer;
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_BUFFER:
        if(status->guest_texture_buffer != status->host_texture_buffer)
        {
            status->host_texture_buffer = status->guest_texture_buffer;
            new_buffer = status->host_texture_buffer;
            need_sync = 1;
        }
        break;
    }
    if(need_sync != 0)
    {
        glBindBuffer(target, new_buffer);
    }

}


GLuint get_guest_binding_buffer(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);
    
    switch (target)
    {
    case GL_ARRAY_BUFFER:
        return status->guest_array_buffer;
    case GL_ELEMENT_ARRAY_BUFFER:
        return status->guest_element_array_buffer;
    case GL_COPY_READ_BUFFER:
        return status->guest_copy_read_buffer;
    case GL_COPY_WRITE_BUFFER:
        return status->guest_copy_write_buffer;
    case GL_PIXEL_PACK_BUFFER:
        return status->guest_pixel_pack_buffer;
    case GL_PIXEL_UNPACK_BUFFER:
        return status->guest_pixel_unpack_buffer;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return status->guest_transform_feedback_buffer;
    case GL_UNIFORM_BUFFER:
        return status->guest_uniform_buffer;
    case GL_ATOMIC_COUNTER_BUFFER:
        return status->guest_atomic_counter_buffer;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return status->guest_dispatch_indirect_buffer;
    case GL_DRAW_INDIRECT_BUFFER:
        return status->guest_draw_indirect_buffer;
    case GL_SHADER_STORAGE_BUFFER:
        return status->guest_shader_storage_buffer;
    case GL_TEXTURE_BUFFER:
        return status->guest_texture_buffer;
    default:
        printf("error target %x get_guest_binding_buffer\n", target);
        break;
    }
    return 0;
}



void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    Opengl_Context *real_opengl_context = (Opengl_Context *)context;

    // if(width < real_opengl_context->view_w && height < real_opengl_context->view_h)
    // {
    //     real_opengl_context->view_x = x;
    //     real_opengl_context->view_y = real_opengl_context->view_h - height;
    //     real_opengl_context->view_w = width;
    //     real_opengl_context->view_h = height;
    //     glViewport(x, real_opengl_context->view_h - height, width, height);
    //     printf("context %llx glViewport change y %d w %d h %d\n", context, real_opengl_context->view_y, width, height);

    //     return;
    // }

    // real_opengl_context->view_x = x;
    // real_opengl_context->view_y = y;
    // real_opengl_context->view_w = width;
    // real_opengl_context->view_h = height;

    express_printf("context %llx glViewport w %d h %d\n", context, width, height);
    glViewport(x, y, width, height);
    return;
}

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image)
{
    //不会调用到host端来
    return;
}






void d_glBindEGLImage(void *t_context, GLenum target, uint64_t image, GLuint texture, GLuint share_texture, EGLContext guest_share_ctx)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)t_context;

    Process_Context *process_context = thread_context->process_context;   
    Opengl_Context *opengl_context = (Opengl_Context *)thread_context->opengl_context;
    uint64_t gbuffer_id = (uint64_t)image;

    GLuint host_share_texture;

    //调用这个函数的时候，前面肯定有glbindtexture，所以opengl_context肯定存在
    if(opengl_context == NULL)
    {
        printf("error! opengl_context null when bindEGLImage image id %llx\n",gbuffer_id);
        return;
    }


    if(gbuffer_id == 0)
    {
        //说明要连接的是普通的texture类型

        Opengl_Context *share_opengl_context =
         (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(guest_share_ctx));
        
        if(share_opengl_context == NULL)
        {
            printf("error! glBindEGLImage with null share_context when gbuffer_id is 0 guest context %llx share_texture %d\n", guest_share_ctx, share_texture);
            return;
        }


        host_share_texture = get_host_texture_id(share_opengl_context, share_texture);
    }
    else
    {
        //连接的为gbuffer
        Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));

        if(gbuffer == NULL)
        {
            //不是本进程创建的gbuffer，则到全局去找，这个一般只出现在合成器上
            gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        }

        if(gbuffer == NULL)
        {
            //不可能没找到
            printf("error! cannot find gbuffer(id %llx)\n",gbuffer_id);
            return;
        }
        // printf("glBindEGLImage gbuffer_id %llx when write %d sync %d\n", gbuffer_id, gbuffer->is_writing, gbuffer->data_sync);
        host_share_texture = gbuffer->data_texture;
        if(gbuffer->is_dying)
        {
            gbuffer->remain_life_time = MAX_LIFE_TIME;
        }

        if (gbuffer->data_sync != 0)
        {
            // glClientWaitSync(gbuffer->data_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
            glWaitSync(gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);
            // if(gbuffer->delete_sync != 0)
            // {
            //     glDeleteSync(gbuffer->delete_sync);
            // }
            // gbuffer->delete_sync = gbuffer->data_sync;
            // gbuffer->data_sync = NULL;
        }

    }

    //原来的texture直接删除掉，假设原来的texture不会再被正常使用——不确定@todo
    int origin_texture = (int)set_share_texture(opengl_context, texture, host_share_texture);
    if(origin_texture > 0)
    {
        glDeleteTextures(1, &origin_texture);
    }


    d_glBindTexture_special(opengl_context, target, host_share_texture);

    return;
}


// void d_glFramebufferEGLImage(void *context, GLenum target, GLenum attachment, GLenum textarget, GLeglImageOES image, GLint level)
// {
// //     uint64_t gbuffer_id = (uint64_t)image;
// //     EGL_Image *egl_image = get_image_from_gbuffer_id(gbuffer_id);
// //     if(egl_image->fbo_texture == 0)
// //     {
// //         init_image_texture(egl_image);
// //     }
// //     //image肯定存在，因为还有前面一系列的创建和连接的过程
// //     glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, egl_image->fbo_texture, level);    

// }

// void d_glBindSharedGLImage(void *context, GLenum target, GLuint texture, void *share_ctx)
// {
//     GLuint share_texture = (GLuint)get_host_texture_id((Opengl_Context *)share_ctx, (unsigned int)texture);

//     int sleep_cnt = 0;
//     while(share_texture == 0 && sleep_cnt < 5)
//     {
//         //多线程操作，可能被share的texture还没创建起来，最多等它5ms
//         g_usleep(1000);
//         sleep_cnt++;
//         share_texture = (GLuint)get_host_texture_id((Opengl_Context *)share_ctx, (unsigned int)texture);
//     }
//     glBindTexture(target, share_texture);
// }


// void d_glFramebufferSharedTexture2D(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, void *share_context)
// {
//     // GLuint share_texture = (GLuint)get_host_texture_id((Opengl_Context *)share_context, (unsigned int)texture);

//     // int sleep_cnt = 0;
//     // while(share_texture == 0 && sleep_cnt < 5)
//     // {
//     //     //多线程操作，可能被share的texture还没创建起来，最多等它5ms
//     //     g_usleep(1000);
//     //     sleep_cnt++;
//     //     share_texture = (GLuint)get_host_texture_id((Opengl_Context *)share_context, (unsigned int)texture);
//     // }
//     // glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, share_texture, level);
// }


void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{
    //当前google没实现，所以暂时先不管
}





void d_glBindTexture_special(void *context, GLenum target, GLuint texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    // printf("context %llx target %x texture %u current %d   ", opengl_context,target, texture,opengl_context->current_active_texture);
    // if (target == GL_TEXTURE_EXTERNAL_OES)
    // {
    //     target = GL_TEXTURE_2D;
    //     opengl_context->current_texture_external = texture;
    // }


    switch(target)
    {
    case GL_TEXTURE_2D:
        status->guest_current_texture_2D[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_2D[status->guest_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        status->guest_current_texture_2D_multisample[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_2D_multisample[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        status->guest_current_texture_2D_multisample_array[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_2D_multisample_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_3D:
        status->guest_current_texture_3D[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_3D[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_2D_ARRAY:
        status->guest_current_texture_2D_array[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_2D_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_CUBE_MAP:
        status->guest_current_texture_cube_map[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_cube_map[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        status->guest_current_texture_cube_map_array[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_cube_map_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_BUFFER:
        status->guest_current_texture_buffer[status->guest_current_active_texture] = texture;
        if(host_opengl_version < 45)
        {
            status->host_current_texture_buffer[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        status->current_texture_external = texture;
        break;
    
    }

    if(target != GL_TEXTURE_EXTERNAL_OES)
    {
        if(host_opengl_version < 45)
        {
            glBindTexture(target, texture);
        }
        else
        {
            if(status->current_texture_unit[status->guest_current_active_texture] != texture)
            {
                //相当于ActiveTexture后BindTexture的效果，把纹理绑定到着色器的采样点上
                glBindTextureUnit(status->guest_current_active_texture, texture);
                status->current_texture_unit[status->guest_current_active_texture] = texture;
            }
        }
    }


    // if (target == GL_TEXTURE_2D)
    // {
    //     opengl_context->current_texture_2D[opengl_context->current_active_texture] = texture;
    // }
    // else if(target == GL_TEXTURE_EXTERNAL_OES)
    // {
    //     //注意要直接return，不能改变状态
    //     opengl_context->current_texture_external = texture;
    //     return;
    // }
    
    // glBindTexture(target, texture);
}

void texture_binding_status_sync(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint current_active_texture = status->guest_current_active_texture;


    if(current_active_texture != status->host_current_active_texture)
    {
        status->host_current_active_texture = current_active_texture;
        glActiveTexture(current_active_texture + GL_TEXTURE0);
    }
    
    int need_sync = 0;
    GLuint new_texture = 0;
    
    switch(target)
    {
    case GL_TEXTURE_2D:
        if(status->guest_current_texture_2D[current_active_texture] != status->host_current_texture_2D[current_active_texture])
        {
            status->host_current_texture_2D[current_active_texture] = status->guest_current_texture_2D[current_active_texture];
            new_texture = status->guest_current_texture_2D[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        if(status->guest_current_texture_2D_multisample[current_active_texture] != status->host_current_texture_2D_multisample[current_active_texture])
        {
            status->host_current_texture_2D_multisample[current_active_texture] = status->guest_current_texture_2D_multisample[current_active_texture];
            new_texture = status->guest_current_texture_2D_multisample[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        if(status->guest_current_texture_2D_multisample_array[current_active_texture] != status->host_current_texture_2D_multisample_array[current_active_texture])
        {
            status->host_current_texture_2D_multisample_array[current_active_texture] = status->guest_current_texture_2D_multisample_array[current_active_texture];
            new_texture = status->guest_current_texture_2D_multisample_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_3D:
        if(status->guest_current_texture_3D[current_active_texture] != status->host_current_texture_3D[current_active_texture])
        {
            status->host_current_texture_3D[current_active_texture] = status->guest_current_texture_3D[current_active_texture];
            new_texture = status->guest_current_texture_3D[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_ARRAY:
        if(status->guest_current_texture_2D_array[current_active_texture] != status->host_current_texture_2D_array[current_active_texture])
        {
            status->host_current_texture_2D_array[current_active_texture] = status->guest_current_texture_2D_array[current_active_texture];
            new_texture = status->guest_current_texture_2D_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_CUBE_MAP:
        if(status->guest_current_texture_cube_map[current_active_texture] != status->host_current_texture_cube_map[current_active_texture])
        {
            status->host_current_texture_cube_map[current_active_texture] = status->guest_current_texture_cube_map[current_active_texture];
            new_texture = status->guest_current_texture_cube_map[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        if(status->guest_current_texture_cube_map_array[current_active_texture] != status->host_current_texture_cube_map_array[current_active_texture])
        {
            status->host_current_texture_cube_map_array[current_active_texture] = status->guest_current_texture_cube_map_array[current_active_texture];
            new_texture = status->guest_current_texture_cube_map_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_BUFFER:
        if(status->guest_current_texture_buffer[current_active_texture] != status->host_current_texture_buffer[current_active_texture])
        {
            status->host_current_texture_buffer[current_active_texture] = status->guest_current_texture_buffer[current_active_texture];
            new_texture = status->guest_current_texture_buffer[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        break;
    
    }

    if(need_sync != 0)
    {
        glBindTexture(target, new_texture);
    }

}



void d_glActiveTexture_special(void *context, GLenum texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    if (texture - GL_TEXTURE0 >= 0 && texture - GL_TEXTURE0 < preload_static_context_value->max_combined_texture_image_units)
    {

        Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
        status->guest_current_active_texture = texture - GL_TEXTURE0;

        if(host_opengl_version < 45)
        {
            status->host_current_active_texture = texture - GL_TEXTURE0;
            glActiveTexture(texture);
        }

    }
    // printf("active texuture %d\n",opengl_context->current_active_texture);

    // glActiveTexture(texture);
}


void active_texture_sync(void *context)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if(status->guest_current_active_texture != status->host_current_active_texture)
    {
        status->host_current_active_texture = status->guest_current_active_texture;
        glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
    }

}



void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{

    // Pixel_Store_Status *pixel_store = &(((Opengl_Context *)context)->pixel_store_status);
    // switch (pname)
    // {
    // case GL_UNPACK_ALIGNMENT:
    //     pixel_store->unpack_alignment = param;
    //     break;
    // case GL_PACK_ALIGNMENT:
    //     pixel_store->pack_alignment = param;
    //     break;
    // case GL_UNPACK_ROW_LENGTH:
    //     pixel_store->unpack_row_length = param;
    //     break;
    // case GL_UNPACK_IMAGE_HEIGHT:
    //     pixel_store->unpack_image_height = param;
    //     break;
    // case GL_UNPACK_SKIP_PIXELS:
    //     pixel_store->unpack_skip_pixels = param;
    //     break;
    // case GL_UNPACK_SKIP_ROWS:
    //     pixel_store->unpack_skip_rows = param;
    //     break;
    // case GL_UNPACK_SKIP_IMAGES:
    //     pixel_store->unpack_skip_images = param;
    //     break;
    // case GL_PACK_ROW_LENGTH:
    //     pixel_store->pack_row_length = param;
    //     break;
    // case GL_PACK_SKIP_PIXELS:
    //     pixel_store->pack_skip_pixels = param;
    //     break;
    // case GL_PACK_SKIP_ROWS:
    //     pixel_store->pack_skip_rows = param;
    //     break;
    // default:
    //     return;
    // }
    glPixelStorei(pname, param);
    //express_printf("glPixelStorei %x %d\n",pname,param);

    return;
}