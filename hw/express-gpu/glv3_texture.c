// #define STD_DEBUG_LOG

#include "hw/express-gpu/glv3_texture.h"
#include "hw/express-gpu/glv3_status.h"
#include "hw/express-gpu/glv3_resource.h"

#include "hw/express-gpu/express_gpu.h"

#include "hw/teleport-express/express_log.h"
#include "hw/express-gpu/express_gpu_snapshot.h"


void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc);

void prepare_unpack_texture(void *context, Guest_Mem *guest_mem, int start_loc, int end_loc)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);
    GLint asyn_texture = bound_buffer->asyn_unpack_texture_buffer;

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glNamedBufferData(asyn_texture, end_loc, NULL, GL_STREAM_DRAW);

        //然后把数据复制到内存里，之后交给dma传输   到底是invalidata还是unsync？
        GLubyte *map_pointer = glMapNamedBufferRange(asyn_texture, start_loc, end_loc - start_loc, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        read_from_guest_mem(guest_mem, map_pointer, start_loc, end_loc - start_loc);

        glUnmapNamedBuffer(asyn_texture);

        if (status->host_pixel_unpack_buffer != asyn_texture)
        {
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);
            status->host_pixel_unpack_buffer = asyn_texture;
        }
    }
    else
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, asyn_texture);
        status->host_pixel_unpack_buffer = asyn_texture; //能调到这说明本身也没绑定pbo，所以不担心覆盖

        //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
        glBufferData(GL_PIXEL_UNPACK_BUFFER, end_loc, NULL, GL_STREAM_DRAW);

        //然后把数据复制到内存里，之后交给dma传输   到底是invalidata还是unsync？
        GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        read_from_guest_mem(guest_mem, map_pointer, start_loc, end_loc - start_loc);


        // unsigned char* zero_buffer = (unsigned char*)malloc(end_loc - start_loc);
        // memset(zero_buffer, 0, end_loc - start_loc);
        // if (memcmp(map_pointer, (const void*)zero_buffer, end_loc - start_loc) == 0) {
        // // 说明 state->pixels 的内容全为 0
        //     LOGI("Memory is all zeros in updload");
        // }
        // free(zero_buffer);

        // LOGI("First pixels in RGBA format:");

        // for (int i = 0; i < min(30, (end_loc - start_loc)/4); i++) {
        //     LOGI("unpack texture %d: R=%d, G=%d, B=%d, A=%d\n",
        //         i, 
        //         map_pointer[i * 4 + 0], //R
        //         map_pointer[i * 4 + 1], //G
        //         map_pointer[i * 4 + 2], //B
        //         map_pointer[i * 4 + 3]  //A
        //     );
        // }

        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    LOGI("unpack texture pbo %lld %d start %d end %d", (uint64_t)opengl_context->window, asyn_texture, start_loc, end_loc);
}

void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{ //没有绑定GL_PIXEL_UNPACK_BUFFER(PBO) buffer
//数据首先写入到 GL_PIXEL_UNPACK_BUFFER，然后 OpenGL 会从该缓冲区将数据上传到纹理中
    LOGD("d_glTexImage2D_without_bound target %x level %d internalFormat %x w "
         "%d h %d format %x type %x pixels %x",
         target, level, internalformat, width, height, format, type, pixels);

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 context %p target %x", __FUNCTION__, context, target);
        return;
    }

    // need to perform a check to make sure its underlying texture storage
    // is of the same size as the specs of the image
    Texture_Binding_Status *tex_status = &(opengl_context->texture_binding_status);
    Hardware_Buffer *gbuffer = NULL;
    if (target == GL_TEXTURE_2D) {
        gbuffer = tex_status->current_2D_gbuffer;
    } else {
        gbuffer = tex_status->current_external_gbuffer;
    }
    if (gbuffer != NULL) {
        // glTexImage2D called on a gbuffer.
        // isolate previous storage and reallocate storage using specs from
        // Hardware_Buffer struct instead of using the supplied values

        GLuint prev_texture = 0;
        GLuint prev_unpack = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&prev_texture);
        glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, (GLint *)&prev_unpack);
        // glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, gbuffer->internal_format, gbuffer->width, gbuffer->height, 0, gbuffer->format, gbuffer->pixel_type, NULL);

        // glBindTexture(GL_TEXTURE_2D, prev_texture);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, prev_unpack);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        if (status->host_pixel_unpack_buffer != 0) //说明是guest那边解绑pbo的同步
        {
            status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, format, type, NULL);
        }
        else
        {
            glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);
        }
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    LOGI("going to upload data for texture target %d id %d width %d height %d len %d",target, bind_texture, width, height, guest_mem->all_len);
    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    //这时候是立即返回的，后续会进行dma传输(只分配空间)
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, format, type, NULL);
    }
    else
    {
        glTexImage2D(target, level, internalformat, width, height, border, format, type, NULL);
    }
}

void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);
    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, format, type, (void *)pixels);
    }
    else
    {
        glTexImage2D(target, level, internalformat, width, height, border, format, type, (void *)pixels);
    }
}

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
    Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

    if (guest_mem->all_len == 0)
    {
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
        LOGE("error! glTexSubImage2D get NULL data! target %x level %d xoffset %d yoffset %d width %d height %d format %x type %x buf_len %d",
               target, level, xoffset, yoffset, (int)width, (int)height, format, type, buf_len);
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, type, NULL);
            return;
        }
        else
        {
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, NULL);
            return;
        }
    }

    int start_loc = 0, end_loc = buf_len;
    LOGI("going upload data for subtexture target %d id %d width %d height %d",target, bind_texture, width, height);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        {
            texture_binding_status_sync(context, target);
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
        }
        else
        {
            glTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, type, 0);
        }
    }
    else
    {
        if (target == GL_TEXTURE_EXTERNAL_OES)
        {
            LOGI("going to tex subimage 2d for GL_TEXTURE_EXTERNAL_OES!");
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
            }
            glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
            // LOGI("%s external texture %u",__FUNCTION__,opengl_context->current_texture_external);
            glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, 0);
            glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
            }
        }
        else
        {
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, 0);
        }
    }
}

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
    }
    else
    {
        if (target == GL_TEXTURE_EXTERNAL_OES)
        {
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
            }
            glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
            // LOGI("%s external texture %u",__FUNCTION__,opengl_context->current_texture_external);
            glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
            glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
            }
        }
        else
        {
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, (void *)pixels);
        }
    }
}

void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    GLuint bind_texture = get_guest_binding_texture(context, target);

    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);

        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, format, type, NULL);
        }
        else
        {
            glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, NULL);
        }
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    express_printf("pixel start loc %d end loc %d\n", start_loc, end_loc);

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    //这时候是立即返回的，后续会进行dma传输
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, format, type, 0);
    }
    else
    {
        glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, 0);
    }
}

void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, format, type, (void *)pixels);
    }
    else
    {
        glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, (void *)pixels);
    }
}

void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint buf_len, const void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
        }
        else
        {
            glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
        }
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    prepare_unpack_texture(context, guest_mem, start_loc, end_loc);

    //这时候是立即返回的，后续会进行dma传输
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, 0);
    }
    else
    {
        glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, 0);
    }
}

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        if (bind_texture == 0)
        {
            LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
            return;
        }
        glTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (void *)pixels);
    }
    else
    {
        glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (void *)pixels);
    }
}

void d_glCompressedTexImage3D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{

    Guest_Mem *guest_mem = (Guest_Mem *)data;

    // LOGI("compress texture format %x",internalformat);
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        // pixels=NULL
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glCompressedTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, imageSize, NULL);
        }
        else
        {
            glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, NULL);
        }
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glCompressedTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, imageSize, 0);
    }
    else
    {
        glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, 0);
    }
}

void d_glCompressedTexImage3D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data)
{
    // LOGI("compress texture format %x",internalformat);
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glCompressedTextureImage3DEXT(bind_texture, target, level, internalformat, width, height, depth, border, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, (void *)data);
    }
}

void d_glCompressedTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glCompressedTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, NULL);
        }
        else
        {
            glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, NULL);
        }
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glCompressedTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, 0);
    }
    else
    {
        glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, 0);
    }
}

void d_glCompressedTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, GLintptr data)
{

    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        if (bind_texture == 0)
        {
            LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
            return;
        }
        glCompressedTextureSubImage3D(bind_texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, (void *)data);
    }
}

void d_glCompressedTexImage2D_without_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    // LOGI("compress texture format %x",internalformat);
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glCompressedTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, imageSize, NULL);
        }
        else
        {
            glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, NULL);
        }

        return;
    }
    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glCompressedTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, imageSize, 0);
    }
    else
    {
        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, 0);
    }
}

void d_glCompressedTexImage2D_with_bound(void *context, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLintptr data)
{
    // LOGI("compress texture format %x",internalformat);
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);

    GLuint bind_texture = get_guest_binding_texture(context, target);
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glCompressedTextureImage2DEXT(bind_texture, target, level, internalformat, width, height, border, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, (void *)data);
    }
}

void d_glCompressedTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)data;

    GLuint bind_texture = get_guest_binding_texture(context, target);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    if (bind_texture == 0)
    {
        LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
        return;
    }

    if (guest_mem->all_len == 0)
    {
        Buffer_Status *buffer_status = &(opengl_context->bound_buffer_status.buffer_status);
        if (buffer_status->host_pixel_unpack_buffer != 0)
        {
            buffer_status->host_pixel_unpack_buffer = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glCompressedTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, imageSize, NULL);
        }
        else
        {
            glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, NULL);
        }
        return;
    }

    prepare_unpack_texture(context, guest_mem, 0, imageSize);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        {
            texture_binding_status_sync(context, target);
            glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);
        }
        else
        {
            glCompressedTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, imageSize, 0);
        }
    }
    else
    {
        glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);
    }
}

void d_glCompressedTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, GLintptr data)
{
    buffer_binding_status_sync(context, GL_PIXEL_UNPACK_BUFFER);
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        if (bind_texture == 0)
        {
            LOGE("error! %s with texture 0 target %llx", __FUNCTION__, (uint64_t)context);
            return;
        }

        glCompressedTextureSubImage2D(bind_texture, level, xoffset, yoffset, width, height, format, imageSize, (void *)data);
    }
    else
    {
        glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, (void *)data);
    }
}

void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    if (unlikely(guest_mem->all_len == 0))
    {
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    Buffer_Status *status = &(bound_buffer->buffer_status);

    GLint asyn_texture = bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, asyn_texture);
    status->host_pixel_pack_buffer = asyn_texture;

    //因为曾经bind过texture，所以这里直接bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glNamedBufferData(asyn_texture, end_loc, NULL, GL_STREAM_READ);
    }
    else
    {
        glBufferData(GL_PIXEL_PACK_BUFFER, end_loc, NULL, GL_STREAM_READ);
    }
    glReadPixels(x, y, width, height, format, type, 0);

    //注意，此句会阻塞，直到pixels全部下载下来
    GLubyte *map_pointer = NULL;
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        map_pointer = glMapNamedBufferRange(asyn_texture, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);
    }
    else
    {
        map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);
    }

    write_to_guest_mem(guest_mem, map_pointer, 0, end_loc - start_loc);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glUnmapNamedBuffer(asyn_texture);
    }
    else
    {
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
}


void d_glReadnPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint buf_len, GLsizei bufSize, void *pixels)
{

    Guest_Mem *guest_mem = (Guest_Mem *)pixels;

    if (unlikely(guest_mem->all_len == 0))
    {
        return;
    }

    int start_loc = 0, end_loc = buf_len;

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    Buffer_Status *status = &(bound_buffer->buffer_status);

    GLint asyn_texture = bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, asyn_texture);
    status->host_pixel_pack_buffer = asyn_texture;

    //因为曾经bind过texture，所以这里直接bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glNamedBufferData(asyn_texture, end_loc, NULL, GL_STREAM_READ);
    }
    else
    {
        glBufferData(GL_PIXEL_PACK_BUFFER, end_loc, NULL, GL_STREAM_READ);
    }
    glReadnPixels(x, y, width, height, format, type, bufSize, 0);

    //注意，此句会阻塞，直到pixels全部下载下来
    GLubyte *map_pointer = NULL;
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        map_pointer = glMapNamedBufferRange(asyn_texture, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);
    }
    else
    {
        map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, start_loc, end_loc - start_loc, GL_MAP_READ_BIT);
    }

    write_to_guest_mem(guest_mem, map_pointer, 0, end_loc - start_loc);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        glUnmapNamedBuffer(asyn_texture);
    }
    else
    {
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
}

void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_PACK_BUFFER);

    glReadPixels(x, y, width, height, format, type, (void *)pixels);
}

void d_glReadnPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, GLintptr pixels)
{
    buffer_binding_status_sync(context, GL_PIXEL_PACK_BUFFER);

    glReadnPixels(x, y, width, height, format, type, bufSize, (void *)pixels);
}

void d_glReadBuffer_special(void *context, GLenum src)
{
    glReadBuffer(src);
}


void d_glFramebufferTexture2D_special(void *context, GLenum target, GLenum attachment, GLenum textarget, GLuint guest_texture, GLint level)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint host_texture = (GLuint)get_host_texture_id(opengl_context, guest_texture, 2);

    char is_init = set_host_texture_init(opengl_context, guest_texture);

    if (is_init == 2)
    {
        if (textarget == GL_TEXTURE_EXTERNAL_OES)
        {
            LOGI("going to bind framebuffer for GL_TEXTURE_EXTERNAL_OES! texture: %d", host_texture);

            textarget = GL_TEXTURE_2D;
        }
    }

    
    glFramebufferTexture2D(target, attachment, textarget, host_texture, level);

    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
    GHashTable *resource_list = g_resource_list[RESOURCE_TYPE_TEXTURE];
    if(g_hash_table_lookup(resource_list, GUINT_TO_POINTER(host_texture)) == NULL) {
        struct Express_Native_Texture_Simple* texture_resource = g_malloc0(sizeof(Express_Native_Texture_Simple));
        texture_resource->target = target;
        texture_resource->textureId = host_texture;        
        g_hash_table_insert(resource_list, GUINT_TO_POINTER(host_texture), texture_resource);            
    }
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
}

void d_glFramebufferTexture_special(void *context, GLenum target, GLenum attachment, GLuint guest_texture, GLint level)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint host_texture = (GLuint)get_host_texture_id(opengl_context, guest_texture, 3);

    char is_init = set_host_texture_init(opengl_context, guest_texture);

    glFramebufferTexture(target, attachment, host_texture, level);

    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
    GHashTable *resource_list = g_resource_list[RESOURCE_TYPE_TEXTURE];
    if(g_hash_table_lookup(resource_list, GUINT_TO_POINTER(host_texture)) == NULL) {
        struct Express_Native_Texture_Simple* texture_resource = g_malloc0(sizeof(Express_Native_Texture_Simple));
        texture_resource->target = target;
        texture_resource->textureId = host_texture;        
        g_hash_table_insert(resource_list, GUINT_TO_POINTER(host_texture), texture_resource);            
    }
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
}

void d_glCopyImageSubData(void *context, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth)
{
    Hardware_Buffer *gbuffer_src = NULL;
    Hardware_Buffer *gbuffer_dst = NULL;

    if (srcTarget == GL_RENDERBUFFER) {
        srcName = get_host_renderbuffer_id(context, srcName);
    } else {
        // 一定是texture
        srcName = get_host_texture_id(context, srcName, 4);
        gbuffer_src = get_texture_gbuffer_ptr(context, srcName);
    }
    if (dstTarget == GL_RENDERBUFFER) {
        dstName = get_host_renderbuffer_id(context, dstName);
    }
    else {
        dstName = get_host_texture_id(context, dstName, 5);
        gbuffer_dst = get_texture_gbuffer_ptr(context, srcName);
    }

    if (srcTarget == GL_TEXTURE_EXTERNAL_OES) {
        srcTarget = GL_TEXTURE_2D;
    }

    if (dstTarget == GL_TEXTURE_EXTERNAL_OES) {
        dstTarget = GL_TEXTURE_2D;
    }

    if (gbuffer_src) {
        if (gbuffer_src->data_sync != 0)
        {
            glWaitSync(gbuffer_src->data_sync, 0, GL_TIMEOUT_IGNORED);
            if (gbuffer_src->delete_sync != 0)
            {
                glDeleteSync(gbuffer_src->delete_sync);
            }
            gbuffer_src->delete_sync = gbuffer_src->data_sync;
            gbuffer_src->data_sync = NULL;
        }
    }
    if (gbuffer_dst) {
        if (gbuffer_dst->data_sync != 0)
        {
            glWaitSync(gbuffer_dst->data_sync, 0, GL_TIMEOUT_IGNORED);
            if (gbuffer_dst->delete_sync != 0)
            {
                glDeleteSync(gbuffer_dst->delete_sync);
            }
            gbuffer_dst->delete_sync = gbuffer_dst->data_sync;
            gbuffer_dst->data_sync = NULL;
        }
    }

    glCopyImageSubData(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName,
                       dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth,
                       srcHeight, srcDepth);

    if (gbuffer_src) {
        gbuffer_src->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    if (gbuffer_dst) {
        gbuffer_dst->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
}
