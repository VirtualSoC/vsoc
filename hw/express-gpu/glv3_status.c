// #define STD_DEBUG_LOG
// #define TIMER_LOG

#include <regex.h>

#include "hw/express-gpu/glv3_status.h"
#include "hw/express-gpu/glv3_resource.h"

#include "glad/glad.h"
#include "hw/express-gpu/egl_window.h"
#include "hw/express-gpu/express_gpu.h"
#include "hw/teleport-express/express_event.h"
#include "hw/express-mem/express_mem.h"

#ifdef __APPLE__
#define max(a, b)                       \
  (((a) > (b)) ? (a) : (b))
#define min(a, b)                       \
  (((a) < (b)) ? (a) : (b))
#endif
void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{
    GLuint draw_fbo0 = ((Opengl_Context *)context)->draw_fbo0;
    GLuint read_fbo0 = ((Opengl_Context *)context)->read_fbo0;

    if (framebuffer == 0)
    {
        if (target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            // LOGI("conetxt %llx bind 0 framebuffer draw %u",(uint64_t)context, draw_fbo0);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo0);
        }
        if (target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            // LOGI("conetxt %llx bind 0 framebuffer read %u",(uint64_t)context, read_fbo0);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo0);
        }
    }
    else
    {
        glBindFramebuffer(target, framebuffer);
    }
}

void d_glBindBuffer_special(void *context, GLenum target, GLuint guest_buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint buffer = (GLuint)get_host_buffer_id(opengl_context, (unsigned int)guest_buffer);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        status->guest_array_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_array_buffer = buffer;
        }
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        // if(guest_buffer == 0 && status->guest_vao == opengl_context->vao0)
        // {

        // }
        status->guest_element_array_buffer = buffer;
        // opengl_context->bound_buffer_status.attrib_point->element_array_buffer = buffer;
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            if (buffer == 0)
            {
                Attrib_Point *point_data = opengl_context->bound_buffer_status.attrib_point;
                if (status->guest_vao_ebo != point_data->indices_buffer_object)
                {
                    status->guest_vao_ebo = point_data->indices_buffer_object;
                    glVertexArrayElementBuffer(status->guest_vao, point_data->indices_buffer_object);
                }
            }
            else
            {
                if (status->guest_vao_ebo != buffer)
                {
                    status->guest_vao_ebo = buffer;
                    glVertexArrayElementBuffer(status->guest_vao, status->guest_vao_ebo);
                }
            }
        }

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_element_array_buffer = buffer;
        }
        break;
    case GL_COPY_READ_BUFFER:
        status->guest_copy_read_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_copy_read_buffer = buffer;
        }
        break;
    case GL_COPY_WRITE_BUFFER:
        status->guest_copy_write_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_copy_write_buffer = buffer;
        }
        break;
    case GL_PIXEL_PACK_BUFFER:
        status->guest_pixel_pack_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_pixel_pack_buffer = buffer;
        }
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        status->guest_pixel_unpack_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_pixel_unpack_buffer = buffer;
        }
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->guest_transform_feedback_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_transform_feedback_buffer = buffer;
        }
        break;
    case GL_UNIFORM_BUFFER:
        status->guest_uniform_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_uniform_buffer = buffer;
        }
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->guest_atomic_counter_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_atomic_counter_buffer = buffer;
        }
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        status->guest_dispatch_indirect_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_dispatch_indirect_buffer = buffer;
        }
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        status->guest_draw_indirect_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_draw_indirect_buffer = buffer;
        }
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->guest_shader_storage_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_shader_storage_buffer = buffer;
        }
        break;
    case GL_TEXTURE_BUFFER:
        status->guest_texture_buffer = buffer;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_texture_buffer = buffer;
        }
        break;
    default:
        LOGE("error target %x buffer %d glBindBuffer", target, buffer);
        break;
    }

    express_printf("context %llx glBindBuffer target %x buffer %d guest %d\n", (uint64_t)context, target, buffer, guest_buffer);

    // if((host_opengl_version < 45 || DSA_enable == 0) || is_init == 0)
    if (host_opengl_version < 45 || DSA_enable == 0)
    // if(target != GL_ELEMENT_ARRAY_BUFFER)
    {
        glBindBuffer(target, buffer);
    }
}

void d_glBindBufferRange_special(void *context, GLenum target, GLuint index, GLuint guest_buffer, GLintptr offset, GLsizeiptr size)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint buffer = (GLuint)get_host_buffer_id(opengl_context, (unsigned int)guest_buffer);

    switch (target)
    {
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->guest_transform_feedback_buffer = buffer;
        status->host_transform_feedback_buffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        status->guest_uniform_buffer = buffer;
        status->host_uniform_buffer = buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->guest_atomic_counter_buffer = buffer;
        status->host_atomic_counter_buffer = buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->guest_shader_storage_buffer = buffer;
        status->host_shader_storage_buffer = buffer;
        break;
    default:
        LOGE("error target %x buffer %d glBindBufferRange", target, buffer);
        break;
    }

    express_printf("context %llx glBindBufferRange target %x buffer %d\n", (uint64_t)context, target, buffer);

    glBindBufferRange(target, index, buffer, offset, size);
}

void d_glBindBufferBase_special(void *context, GLenum target, GLuint index, GLuint guest_buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint buffer = (GLuint)get_host_buffer_id(opengl_context, (unsigned int)guest_buffer);

    switch (target)
    {
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->guest_transform_feedback_buffer = buffer;
        status->host_transform_feedback_buffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        status->guest_uniform_buffer = buffer;
        status->host_uniform_buffer = buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->guest_atomic_counter_buffer = buffer;
        status->host_atomic_counter_buffer = buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->guest_shader_storage_buffer = buffer;
        status->host_shader_storage_buffer = buffer;
        break;
    default:
        LOGE("error target %x buffer %d glBindBufferBase", target, buffer);
        break;
    }

    express_printf("context %llx glBindBufferBase target %x buffer %d\n", (uint64_t)context, target, buffer);
    glBindBufferBase(target, index, buffer);
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
        if (status->guest_array_buffer != status->host_array_buffer)
        {
            status->host_array_buffer = status->guest_array_buffer;
            new_buffer = status->host_array_buffer;
            need_sync = 1;
        }
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        if (status->guest_element_array_buffer != status->host_element_array_buffer)
        {
            status->host_element_array_buffer = status->guest_element_array_buffer;
            new_buffer = status->host_element_array_buffer;
            need_sync = 1;
        }
        LOGI("@todo! buffer sync element array buffer (glDrawElements)!");
        break;
    case GL_COPY_READ_BUFFER:
        if (status->guest_copy_read_buffer != status->host_copy_read_buffer)
        {
            status->host_copy_read_buffer = status->guest_copy_read_buffer;
            new_buffer = status->host_copy_read_buffer;
            need_sync = 1;
        }
        break;
    case GL_COPY_WRITE_BUFFER:
        if (status->guest_copy_write_buffer != status->host_copy_write_buffer)
        {
            status->host_copy_write_buffer = status->guest_copy_write_buffer;
            new_buffer = status->host_copy_write_buffer;
            need_sync = 1;
        }
        break;
    case GL_PIXEL_PACK_BUFFER:
        if (status->guest_pixel_pack_buffer != status->host_pixel_pack_buffer)
        {
            status->host_pixel_pack_buffer = status->guest_pixel_pack_buffer;
            new_buffer = status->host_pixel_pack_buffer;
            need_sync = 1;
        }
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        if (status->guest_pixel_unpack_buffer != status->host_pixel_unpack_buffer)
        {
            status->host_pixel_unpack_buffer = status->guest_pixel_unpack_buffer;
            new_buffer = status->host_pixel_unpack_buffer;
            need_sync = 1;
        }
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        if (status->guest_transform_feedback_buffer != status->host_transform_feedback_buffer)
        {
            status->host_transform_feedback_buffer = status->guest_transform_feedback_buffer;
            new_buffer = status->host_transform_feedback_buffer;
            need_sync = 1;
        }
        break;
    case GL_UNIFORM_BUFFER:
        if (status->guest_uniform_buffer != status->host_uniform_buffer)
        {
            status->host_uniform_buffer = status->guest_uniform_buffer;
            new_buffer = status->host_uniform_buffer;
            need_sync = 1;
        }
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        if (status->guest_atomic_counter_buffer != status->host_atomic_counter_buffer)
        {
            status->host_atomic_counter_buffer = status->guest_atomic_counter_buffer;
            new_buffer = status->host_atomic_counter_buffer;
            need_sync = 1;
        }
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        if (status->guest_dispatch_indirect_buffer != status->host_dispatch_indirect_buffer)
        {
            status->host_dispatch_indirect_buffer = status->guest_dispatch_indirect_buffer;
            new_buffer = status->host_dispatch_indirect_buffer;
            need_sync = 1;
        }
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        if (status->guest_draw_indirect_buffer != status->host_draw_indirect_buffer)
        {
            status->host_draw_indirect_buffer = status->guest_draw_indirect_buffer;
            new_buffer = status->host_draw_indirect_buffer;
            need_sync = 1;
        }
        break;
    case GL_SHADER_STORAGE_BUFFER:
        if (status->guest_shader_storage_buffer != status->host_shader_storage_buffer)
        {
            status->host_shader_storage_buffer = status->guest_shader_storage_buffer;
            new_buffer = status->host_shader_storage_buffer;
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_BUFFER:
        if (status->guest_texture_buffer != status->host_texture_buffer)
        {
            status->host_texture_buffer = status->guest_texture_buffer;
            new_buffer = status->host_texture_buffer;
            need_sync = 1;
        }
        break;
    }
    if (need_sync != 0)
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
        LOGE("error target %x get_guest_binding_buffer", target);
        break;
    }
    return 0;
}

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    Opengl_Context *real_opengl_context = (Opengl_Context *)context;

    real_opengl_context->view_x = x;
    real_opengl_context->view_y = y;
    real_opengl_context->view_w = width;
    real_opengl_context->view_h = height;

    express_printf("context %llx glViewport w %d h %d\n", (uint64_t)context, width, height);
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
    Hardware_Buffer *gbuffer = NULL;

    GLuint host_share_texture;

    //调用这个函数的时候，前面肯定有glbindtexture，所以opengl_context肯定存在
    if (opengl_context == NULL)
    {
        LOGE("error! opengl_context null when bindEGLImage image id %llx", gbuffer_id);
        return;
    }

    gbuffer = (Hardware_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id));
    if (gbuffer == NULL) 
    {
        gbuffer = get_gbuffer_from_global_map(gbuffer_id);
    }
    if (gbuffer == NULL)
    {
        LOGE("error! glBindEGLImage with NULL gbuffer when finding in process %p gbuffer_id %llx target %x", process_context, gbuffer_id, target);
        return;
    }

    LOGD("glBindEGLImage gbuffer %llx ptr %llx type %d is_texture2d (%d)",gbuffer->gbuffer_id, gbuffer, gbuffer->usage_type, target == GL_TEXTURE_2D);

    if (gbuffer->usage_type != GBUFFER_TYPE_TEXTURE)
    {
        set_texture_gbuffer_ptr(opengl_context, texture, gbuffer);
        LOGD("glBindEGLImage gbuffer_id %llx is_writing %d sync %d", gbuffer_id, gbuffer->is_writing, gbuffer->data_sync);
        Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
        if (target == GL_TEXTURE_2D)
        {
            status->current_2D_gbuffer = gbuffer;
        }
        else
        {
            status->current_external_gbuffer = gbuffer;
        }
        if (gbuffer->pref_phy_dev != EXPRESS_MEM_TYPE_UNKNOWN && gbuffer->pref_phy_dev != EXPRESS_MEM_TYPE_TEXTURE) {
            // oops, prefetch failure, sync gpu mem
            if (gbuffer->pref_phy_dev == EXPRESS_MEM_TYPE_GBUFFER_HOST_MEM) {
                LOGD("gbuffer %" PRIx64 " sync data: %s -> %s", gbuffer->gbuffer_id, memtype_to_str(gbuffer->pref_phy_dev), memtype_to_str(EXPRESS_MEM_TYPE_TEXTURE));
                GLuint prev_texture = 0;
                GLuint prev_unpack = 0;
                glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&prev_texture);
                glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, (GLint *)&prev_unpack);
                glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                glTexImage2D(GL_TEXTURE_2D, 0, gbuffer->internal_format, gbuffer->width, gbuffer->height, 0, gbuffer->format, gbuffer->pixel_type, gbuffer->host_data);

                glBindTexture(GL_TEXTURE_2D, prev_texture);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, prev_unpack);
            }
            else {
                LOGW("prefetch failed, but don't know how to recover data. expect graphics glitches");
            }
        }
        update_gbuffer_phy_usage(gbuffer, EXPRESS_MEM_TYPE_TEXTURE, false);
    }

    host_share_texture = gbuffer->data_texture;

    //原来的texture直接删除掉，假设原来的texture不会再被正常使用——不确定@todo
    unsigned int origin_texture = (int)set_share_texture(opengl_context, texture, host_share_texture);
    if (origin_texture > 0)
    {
        glDeleteTextures(1, &origin_texture);
    }

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
    switch (target)
    {
    case GL_TEXTURE_2D:
        status->guest_current_texture_2D[status->guest_current_active_texture] = host_share_texture;
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_current_texture_2D[status->guest_current_active_texture] = host_share_texture;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        status->current_texture_external = host_share_texture;
        break;
    }

    if (target != GL_TEXTURE_EXTERNAL_OES)
    {
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            glBindTexture(target, host_share_texture);
        }
        // else
        // {
        //     status->guest_current_texture_unit[status->guest_current_active_texture] = host_share_texture;
        // }
    }

    return;
}

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{
    //当前google没实现，所以暂时先不管
}

void d_glBindTexture_special(void *context, GLenum target, GLuint guest_texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint texture = (GLuint)get_host_texture_id(opengl_context, (unsigned int)guest_texture);

    char is_init = set_host_texture_init(opengl_context, guest_texture);

    express_printf("context %llx target %x texture %u guest %d current %d\n", (uint64_t)opengl_context, target, texture, guest_texture, status->guest_current_active_texture);

    if (is_init == 0)
    {
        if (status->guest_current_active_texture != status->host_current_active_texture)
        {
            status->host_current_active_texture = status->guest_current_active_texture;
            glActiveTexture(status->guest_current_active_texture + GL_TEXTURE0);
        }
    }

    switch (target)
    {
    case GL_TEXTURE_2D:
        status->guest_current_texture_2D[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D[status->host_current_active_texture] = texture;
        }
        if (is_init == 2)
        {
            status->current_2D_gbuffer = get_texture_gbuffer_ptr(context, guest_texture);
        }
        else
        {
            status->current_2D_gbuffer = NULL;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        status->guest_current_texture_2D_multisample[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D_multisample[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        status->guest_current_texture_2D_multisample_array[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D_multisample_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_3D:
        status->guest_current_texture_3D[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_3D[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_2D_ARRAY:
        status->guest_current_texture_2D_array[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_2D_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_CUBE_MAP:
        status->guest_current_texture_cube_map[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_cube_map[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        status->guest_current_texture_cube_map_array[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_cube_map_array[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_BUFFER:
        status->guest_current_texture_buffer[status->guest_current_active_texture] = texture;
        if (host_opengl_version < 45 || DSA_enable == 0 || is_init == 0)
        {
            status->host_current_texture_buffer[status->host_current_active_texture] = texture;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        status->current_texture_external = texture;
        if (is_init == 2)
        {
            status->current_external_gbuffer = get_texture_gbuffer_ptr(context, guest_texture);
        }
        else
        {
            status->current_external_gbuffer = NULL;
        }
        break;
    default:
        LOGE("error! glBindBuffer error target %x", target);
        break;
    }

    if (target != GL_TEXTURE_EXTERNAL_OES)
    {
        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            // LOGI("context %llx bind texuture %d active %d", (uint64_t)context, texture, a-GL_TEXTURE0);
            glBindTexture(target, texture);
        }
        else
        {
            if (is_init == 0)
            {
                glBindTexture(target, texture);
            }
        }
    }
}

void texture_binding_status_sync(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint current_active_texture = status->guest_current_active_texture;

    if (current_active_texture != status->host_current_active_texture)
    {
        LOGI("active texture sync host %d guest %d", status->host_current_active_texture, current_active_texture);
        status->host_current_active_texture = current_active_texture;
        glActiveTexture(current_active_texture + GL_TEXTURE0);
    }

    int need_sync = 0;
    GLuint new_texture = 0;

    // GLuint temp=0;
    switch (target)
    {
    case GL_TEXTURE_2D:
        if (status->guest_current_texture_2D[current_active_texture] != status->host_current_texture_2D[current_active_texture])
        {
            status->host_current_texture_2D[current_active_texture] = status->guest_current_texture_2D[current_active_texture];
            new_texture = status->guest_current_texture_2D[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        if (status->guest_current_texture_2D_multisample[current_active_texture] != status->host_current_texture_2D_multisample[current_active_texture])
        {
            status->host_current_texture_2D_multisample[current_active_texture] = status->guest_current_texture_2D_multisample[current_active_texture];
            new_texture = status->guest_current_texture_2D_multisample[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        if (status->guest_current_texture_2D_multisample_array[current_active_texture] != status->host_current_texture_2D_multisample_array[current_active_texture])
        {
            status->host_current_texture_2D_multisample_array[current_active_texture] = status->guest_current_texture_2D_multisample_array[current_active_texture];
            new_texture = status->guest_current_texture_2D_multisample_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_3D:
        if (status->guest_current_texture_3D[current_active_texture] != status->host_current_texture_3D[current_active_texture])
        {
            status->host_current_texture_3D[current_active_texture] = status->guest_current_texture_3D[current_active_texture];
            new_texture = status->guest_current_texture_3D[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_2D_ARRAY:
        if (status->guest_current_texture_2D_array[current_active_texture] != status->host_current_texture_2D_array[current_active_texture])
        {
            status->host_current_texture_2D_array[current_active_texture] = status->guest_current_texture_2D_array[current_active_texture];
            new_texture = status->guest_current_texture_2D_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_CUBE_MAP:
        if (status->guest_current_texture_cube_map[current_active_texture] != status->host_current_texture_cube_map[current_active_texture])
        {
            target = GL_TEXTURE_CUBE_MAP;
            status->host_current_texture_cube_map[current_active_texture] = status->guest_current_texture_cube_map[current_active_texture];
            new_texture = status->guest_current_texture_cube_map[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        if (status->guest_current_texture_cube_map_array[current_active_texture] != status->host_current_texture_cube_map_array[current_active_texture])
        {
            status->host_current_texture_cube_map_array[current_active_texture] = status->guest_current_texture_cube_map_array[current_active_texture];
            new_texture = status->guest_current_texture_cube_map_array[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_BUFFER:
        if (status->guest_current_texture_buffer[current_active_texture] != status->host_current_texture_buffer[current_active_texture])
        {
            status->host_current_texture_buffer[current_active_texture] = status->guest_current_texture_buffer[current_active_texture];
            new_texture = status->guest_current_texture_buffer[current_active_texture];
            need_sync = 1;
        }
        break;
    case GL_TEXTURE_EXTERNAL_OES:
        break;
    default:
        LOGE("error texture target %x need sync", target);
        break;
    }

    if (need_sync != 0)
    {
        glBindTexture(target, new_texture);
    }
}

#define CHANGE_TEXTURE_UNIT_WITH_TYPE(status, type, index)                               \
    {                                                                                    \
        if (status->guest_current_texture_##type != status->host_current_texture_##type) \
        {                                                                                \
            status->host_current_texture_##type = status->guest_current_texture_##type;  \
            glBindTextureUnit(index, status->guest_current_texture_##type);              \
        }                                                                                \
    }

void texture_unit_status_sync(void *context, GLint index)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (index < 0)
    {
        for (int i = 0; i <= status->now_max_texture_unit; i++)
        {
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map[i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 3D [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_array [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample_array [i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map_array[i], i);
            CHANGE_TEXTURE_UNIT_WITH_TYPE(status, buffer[i], i);
        }
    }
    else
    {
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map[index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 3D [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_array [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, 2D_multisample_array [index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, cube_map_array[index], index);
        CHANGE_TEXTURE_UNIT_WITH_TYPE(status, buffer[index], index);
    }
}

GLuint get_guest_binding_texture(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    GLuint current_active_texture = status->guest_current_active_texture;

    switch (target)
    {
    case GL_TEXTURE_2D:
        return status->guest_current_texture_2D[current_active_texture];
    case GL_TEXTURE_2D_MULTISAMPLE:
        return status->guest_current_texture_2D_multisample[current_active_texture];
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return status->guest_current_texture_2D_multisample_array[current_active_texture];
    case GL_TEXTURE_3D:
        return status->guest_current_texture_3D[current_active_texture];
    case GL_TEXTURE_2D_ARRAY:
        return status->guest_current_texture_2D_array[current_active_texture];
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        return status->guest_current_texture_cube_map[current_active_texture];
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        return status->guest_current_texture_cube_map_array[current_active_texture];
    case GL_TEXTURE_BUFFER:
        return status->guest_current_texture_buffer[current_active_texture];
    case GL_TEXTURE_EXTERNAL_OES:
        return status->current_texture_external;
        break;
    default:
        break;
    }
    LOGE("error! get_guest_binding_texture target %x", target);
    return 0;
}

void d_glActiveTexture_special(void *context, GLenum texture)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    if ((int)texture - GL_TEXTURE0 >= 0 && texture - GL_TEXTURE0 < preload_static_context_value->max_combined_texture_image_units)
    {

        Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
        status->guest_current_active_texture = texture - GL_TEXTURE0;

        if (host_opengl_version < 45 || DSA_enable == 0)
        {
            status->host_current_active_texture = texture - GL_TEXTURE0;
            glActiveTexture(texture);
        }

        if (texture - GL_TEXTURE0 > status->now_max_texture_unit)
        {
            status->now_max_texture_unit = texture - GL_TEXTURE0;
        }
        // GLuint tex = 0;
        // GLuint ac = 0;
        // glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex);
        // glGetIntegerv(GL_ACTIVE_TEXTURE, &ac);
        // LOGI("context %llx active texture %d tex %u active %d", (uint64_t)context, status->host_current_active_texture, tex, ac-GL_TEXTURE0);
    }
}

void active_texture_sync(void *context)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (status->guest_current_active_texture != status->host_current_active_texture)
    {
        status->host_current_active_texture = status->guest_current_active_texture;
        glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
    }
}

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{
    glPixelStorei(pname, param);
    return;
}

void d_glBindVertexArray_special(void *context, GLuint array)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint pre_vao = status->guest_vao;

    GLuint now_vao = (GLuint)get_host_array_id(context, (unsigned int)array);
    if (array == 0 && now_vao == 0)
    {
        now_vao = opengl_context->vao0;
    }

    Attrib_Point *now_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(now_vao));
    Attrib_Point *pre_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(pre_vao));

    express_printf("context %llx bind vao host %d guest %d pre %d\n", (uint64_t)context, now_vao, array, pre_vao);

    if (now_point == NULL)
    {
        now_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(0));
        LOGE("error! vao %d cannot find", now_vao);
        return;
    }

    if (pre_point->element_array_buffer != status->guest_vao_ebo)
    {
        pre_point->element_array_buffer = status->guest_vao_ebo;
    }

    bound_buffer->attrib_point = now_point;

    status->guest_element_array_buffer = now_point->element_array_buffer;

    status->guest_vao_ebo = now_point->element_array_buffer;

    status->guest_vao = now_vao;

    if (host_opengl_version < 45 || DSA_enable == 0)
    {
        glBindVertexArray(now_vao);
    }
}

GLuint get_guest_binding_vao(void *context)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    return (GLuint)status->guest_vao;
}

void vao_binding_status_sync(void *context)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    Buffer_Status *status = &(bound_buffer->buffer_status);

    if (status->host_vao != status->guest_vao)
    {
        express_printf("#%llx vao_binding_status_sync bind vao %d pre %d ebo %d\n", (uint64_t)opengl_context, status->guest_vao, status->host_vao, status->guest_vao_ebo);
        glBindVertexArray(status->guest_vao);

        status->host_vao = status->guest_vao;
        status->host_vao_ebo = status->guest_vao_ebo;

        status->guest_element_array_buffer = status->guest_vao_ebo;
        status->host_element_array_buffer = status->guest_vao_ebo;
    }
}

/**
 * DebugMessage分发函数。
 * 为了能同时把debugMessage用于host端调试和guest端需求。
 * @param userParam host端的回调函数
*/
void d_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    Opengl_Context *context = (Opengl_Context *)userParam;
    // 1. 在host端打印debug信息。如果context为NULL则是主窗口，否则是应用窗口
    // 忽略一些不是错误的id
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131154 || id == 131186) {}
    else if (context != NULL && (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION)) {}

#ifdef ENABLE_OPENGL_PERFORMANCE_WARNING

#else
    else if (type == GL_DEBUG_TYPE_PERFORMANCE) {}
#endif
    else
    {
        if (context != NULL) 
        {
            LOGI("\ndebug message (id: %u, context %p): %s", id, context, message);
        }
        else
        {
            LOGI("main debug message (id: %u): %s", id, message);
        }
        switch (source)
        {
        case GL_DEBUG_SOURCE_API:
            LOGI("Source: API ");
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            LOGI("Source: Window System ");
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            LOGI("Source: Shader Compiler ");
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            LOGI("Source: Third Party ");
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            LOGI("Source: APPLICATION ");
            break;
        case GL_DEBUG_SOURCE_OTHER:
            break;
        }

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:
            LOGI("Type: Error ");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            LOGI("Type: Deprecated Behaviour ");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            LOGI("Type: Undefined Behaviour ");
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            LOGI("Type: Portability ");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            LOGI("Type: Performance ");
            break;
        case GL_DEBUG_TYPE_MARKER:
            LOGI("Type: Marker ");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            LOGI("Type: Push Group ");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            LOGI("Type: Pop Group ");
            break;
        case GL_DEBUG_TYPE_OTHER:
            LOGI("Type: Other ");
            break;
        }

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            LOGI("Severity: high");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            LOGI("Severity: medium");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            LOGI("Severity: low");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            LOGI("Severity: notification");
            break;
        }
        LOGI("");
    }

    // 2. 向guest端写入debug message
    if (!context || !(context->context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) || !(context->debug_message_buffer) || !(context->is_current))
    {
        if (context && context->context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            LOGE("error! debug context is not ready for messages: buffer %p is_current %d", context->debug_message_buffer, context->is_current);
        }
        return;
    }

    // 查询guest端读取情况
    Guest_Mem *guest_mem = (Guest_Mem *)(context->debug_message_buffer);
    unsigned char *desc_local = NULL;
    void *no_ptr_buf = NULL;
    int desc_len = sizeof(RingBufferDesc);
    int null_flag = 0;
    desc_local = get_direct_ptr(guest_mem, &null_flag);
    if (unlikely(desc_local == NULL))
    {
        if (desc_len != 0 && null_flag == 0)
        {
            desc_local = g_malloc(desc_len);
            no_ptr_buf = desc_local;
            read_from_guest_mem(guest_mem, desc_local, 0, desc_len);
        }
    }
    RingBufferDesc *desc = (RingBufferDesc *)desc_local;
    const size_t block_size = desc->block_size;
    // express_printf("guest debug ring buffer desc %p block_count %zu block_size %zu read_idx %d write_idx %d\n", desc, desc->block_count, desc->block_size, desc->read_idx, desc->write_idx);
 
    // TODO: 尽力而为做一下id翻译
    // TODO: 区分来自guest和host端的message。guest端的message不用做id翻译

    // 构建message
    // 这里是从guest端block_size计算出的max_debug_message_length，也可以直接从host端拿
    length = min(length, block_size - sizeof(DebugMessageDesc) - 1); 
    char *data = (char *)g_malloc(block_size);
    DebugMessageDesc *local_data = (DebugMessageDesc *)data;
    local_data->length = length;
    local_data->source = source;
    local_data->type = type;
    local_data->id = id;
    local_data->severity = severity;
    memcpy(data + sizeof(DebugMessageDesc), message, length);
    data[sizeof(DebugMessageDesc) + length] = 0; // null-terminated

    // 写回guest内存。
    // 因为host端开了GL_DEBUG_OUTPUT_SYNCHRONOUS，所以这个函数同一时间只可能被一个线程调用，而且是在gl函数执行完成之前调用的，不会存在并发写入的情况
    LOGI("host write debug message context %p write_idx %d read_idx %d message length %d", context, desc->write_idx, desc->read_idx, length);
    write_to_guest_mem(guest_mem, data, sizeof(RingBufferDesc) + (desc->write_idx % desc->block_count) * block_size, block_size);
    desc->write_idx += 1;
    write_to_guest_mem(guest_mem, &(desc->write_idx), (uint64_t)&(desc->write_idx) - (uint64_t)desc, sizeof(desc->write_idx));

    g_free(data);
    if (no_ptr_buf != NULL) {
        g_free(no_ptr_buf);
    }
}
