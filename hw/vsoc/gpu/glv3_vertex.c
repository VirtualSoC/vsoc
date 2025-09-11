// #define STD_DEBUG_LOG
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/gpu/glv3_vertex.h"
#include "hw/vsoc/gpu/glv3_status.h"

GLint set_vertex_attrib_data(void *context, GLuint index, GLuint offset, GLuint length, const void *pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLuint max_len = offset + length;

    unsigned char *map_pointer = NULL;

    GLint padding = -min(point_data->buffer_len[index] - point_data->remain_buffer_len[index] - (GLint)offset, 0);

    if (!DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0)) {
        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index]);
    }

    LOGD("set_vertex_attrib_data vbo index %d offset %d length %d padding %d",point_data->buffer_object[index], index, offset, length, padding);

    if (max_len > point_data->buffer_len[index])
    {
        //当前的缓冲区大小不足，直接将原来的缓冲区加到当前最大大小的BUFFER_MULTIPLY_FACTOR倍，类似于vector的翻倍机制

        int alloc_size = max_len * BUFFER_MULTIPLY_FACTOR;
        if (alloc_size < 1024)
        {
            alloc_size = 1024;
        }

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0)) {
            glNamedBufferData(point_data->buffer_object[index], alloc_size, NULL, GL_STREAM_DRAW);
        } else {
            glBufferData(GL_ARRAY_BUFFER, alloc_size, NULL, GL_DYNAMIC_DRAW);
        }
        point_data->buffer_len[index] = alloc_size;

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0)) {
            map_pointer = glMapNamedBufferRange(point_data->buffer_object[index], offset, length,
                GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

        } else {
            map_pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset, length,
                GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }

        g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer, 0, length);
        LOGD("set_vertex_attrib_data vbo index %d offset %d length %d pointer %d",point_data->buffer_object[index], offset, length, (int)map_pointer);

        point_data->buffer_loc[index] = 0;
        point_data->remain_buffer_len[index] = alloc_size - max_len;
    }
    else if (padding + length > point_data->remain_buffer_len[index])
    {
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0)) {
            map_pointer = glMapNamedBufferRange(point_data->buffer_object[index], 0, point_data->buffer_len[index],
                GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        } else {
            map_pointer = glMapBufferRange(GL_ARRAY_BUFFER, 0, point_data->buffer_len[index],
                GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        }

        // TODO 测试是否需要从0开始映射
        g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer + offset, 0, length);

        LOGD("set_vertex_attrib_data vbo index %d offset %d length %d pointer %d",point_data->buffer_object[index], offset, length, (int)map_pointer);

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0)) {
            glFlushMappedNamedBufferRange(point_data->buffer_object[index], offset, length);
        } else {
            glFlushMappedBufferRange(GL_ARRAY_BUFFER, offset, length);
        }

        point_data->buffer_loc[index] = 0;
        point_data->remain_buffer_len[index] = point_data->buffer_len[index] - max_len;
    }
    else
    {
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0)) {
            map_pointer = glMapNamedBufferRange(point_data->buffer_object[index],
                point_data->buffer_len[index] - point_data->remain_buffer_len[index], length + padding,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
        } else {
            map_pointer = glMapBufferRange(GL_ARRAY_BUFFER,
                point_data->buffer_len[index] - point_data->remain_buffer_len[index], length + padding,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
        }

        g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer + padding, 0, length);

        LOGD("set_vertex_attrib_data vbo index %d offset %d length %d pointer %d %d length %d", point_data->buffer_object[index], offset, length, (int)map_pointer, padding, point_data->buffer_len[index] - point_data->remain_buffer_len[index]);

        // sometimes different offsets are used on the same host vbo, causing accesses of negative vbo indices and therefore undefined behaviour 
        // therefore some padding is added to avoid negative buffer_loc
        // fixes flickering icons in OpenHarmony 4.0 on Intel graphics cards.
        point_data->buffer_loc[index] = point_data->buffer_len[index] - point_data->remain_buffer_len[index] + padding - offset;
        point_data->remain_buffer_len[index] -= length + padding;
    }

    express_printf("attrib point loc %d %d index %d offset %d len %d\n", point_data->buffer_loc[index], point_data->buffer_loc[index] + length, index, offset, length);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0)) {
        glUnmapNamedBuffer(point_data->buffer_object[index]);
    } else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    return point_data->buffer_loc[index];
}

void d_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);
    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        Attrib_Point *point_data = bound_buffer->attrib_point;

        express_printf("d_glVertexAttribPointer_without_bound vao %d %d obj %d index %u size %d type %x normalized %d stride %d offset %u length %d\n", status->guest_vao, status->host_vao,
                       point_data->buffer_object[index], index, size, type, normalized, stride, offset, length);

        glVertexArrayVertexAttribOffsetEXT(status->guest_vao, point_data->buffer_object[index], index, size, type, normalized, stride, (GLintptr)loc);
    }
    else
    {
        GLint vbo = status->host_array_buffer;

        LOGD("d_glVertexAttribPointer_without_bound index %u size %d type %x normalized %d stride %d offset %u length %d origin vbo %d", index, size, type, normalized, stride, offset, length, vbo);

        glVertexAttribPointer(index, size, type, normalized, stride, (void *)(uint64_t)loc);

        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }

    return;
}

void d_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);
    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        express_printf("d_glVertexAttribIPointer_without_bound index %u size %d type %x stride %d offset %u length %d\n", index, size, type, stride, offset, length);

        Attrib_Point *point_data = bound_buffer->attrib_point;

        glVertexArrayVertexAttribIOffsetEXT(status->guest_vao, point_data->buffer_object[index], index, size, type, stride, (GLintptr)loc);
    }
    else
    {
        glVertexAttribIPointer(index, size, type, stride, (const void *)(uint64_t)loc);

        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }

    return;
}

void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Attrib_Point *point_data = bound_buffer->attrib_point;
    Buffer_Status *status = &(bound_buffer->buffer_status);

    if (offset + point_data->buffer_loc[index_father] < 0)
    {
        LOGE("d_glVertexAttribPointer_offset negative loc %lld! offset %lld index_father %u buffer_loc %d", offset + point_data->buffer_loc[index_father], offset, index_father, point_data->buffer_loc[index_father]);
    }
    else 
    {
        LOGD("d_glVertexAttribPointer_offset loc %lld offset %lld index_father %u buffer_loc %d", offset + point_data->buffer_loc[index_father], offset, index_father, point_data->buffer_loc[index_father]);
    }

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        express_printf("pointer offset vao %d %d obj %d  index %u size %d type %x stride %d offset %u real offset %d\n",
                       status->guest_vao, status->host_vao, point_data->buffer_object[index], index, size, type, stride, offset, offset + point_data->buffer_loc[index_father]);

        glVertexArrayVertexAttribOffsetEXT(status->guest_vao, point_data->buffer_object[index_father], index, size, type, normalized, stride, (GLintptr)(offset + point_data->buffer_loc[index_father]));
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

        glVertexAttribPointer(index, size, type, normalized, stride, (const void *)(offset + point_data->buffer_loc[index_father]));
        LOGD("d_glVertexAttribPointer_offset vbo %d index %u size %d type %x normalized %d stride %d offset %u real offset %d", point_data->buffer_object[index_father], index, size, type, normalized, stride, offset, offset + point_data->buffer_loc[index_father]);
        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }

    return;
}

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset)
{

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Attrib_Point *point_data = bound_buffer->attrib_point;

    Buffer_Status *status = &(bound_buffer->buffer_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        glVertexArrayVertexAttribIOffsetEXT(status->guest_vao, point_data->buffer_object[index_father], index, size, type, stride, (GLintptr)(offset + point_data->buffer_loc[index_father]));
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

        express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);

        glVertexAttribIPointer(index, size, type, stride, (const void *)(offset + point_data->buffer_loc[index_father]));

        glBindBuffer(GL_ARRAY_BUFFER, status->host_array_buffer);
    }

    return;
}

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);

        glVertexArrayVertexAttribOffsetEXT(status->guest_vao, status->guest_array_buffer, index, size, type, normalized, stride, (GLintptr)pointer);
    }
    else
    {
#ifdef STD_DEBUG_LOG
        GLint ebo = 0;
        GLint vbo = 0;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
        express_printf("%llx d_glVertexAttribPointer_with_bound index %u size %d type %x normalized %d stride %d pointer %llx ebo %d vbo %d\n", (uint64_t)context, index, size, type, normalized, stride, pointer, ebo, vbo);
#endif
        GLint ebo = 0;
        GLint vbo = 0;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);

        glVertexAttribPointer(index, size, type, normalized, stride, (void *)pointer);
        LOGD("%llx d_glVertexAttribPointer_with_bound index %d size %d type %x normalized %d stride %d pointer %llx vbo %d ebo %d", (uint64_t)context, index, size, type, normalized, stride, pointer, vbo, ebo);
    }
    return;
}

void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);

        glVertexArrayVertexAttribIOffsetEXT(status->guest_vao, status->guest_array_buffer, index, size, type, stride, (GLintptr)pointer);
    }
    else
    {
        GLint ebo = 0;
        GLint vbo = 0;
#ifdef STD_DEBUG_LOG
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
        express_printf("%llx d_glVertexAttribIPointer_with_bound index %u size %d type %x stride %d pointer %llx ebo %d vbo %d\n", (uint64_t)context, index, size, type, stride, pointer, ebo, vbo);

        glVertexAttribIPointer(index, size, type, stride, (void *)pointer);
    }

    return;
}

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{
    //反正array draw的时候还会传divisor，所以这里就不用保存了
    express_printf("%llx glVertexAttribDivisor %u %u\n", (uint64_t)context, index, divisor);
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);
        glVertexArrayVertexAttribDivisorEXT(status->guest_vao, index, divisor);
    }
    else
    {
        glVertexAttribDivisor(index, divisor);
    }
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Attrib_Point *point_data = bound_buffer->attrib_point;
    point_data->divisors[index] = divisor;
    GLint currentVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    LOGD("d_glVertexAttribDivisor_origin index %d divisor %d guest vao %d vao %d", index, divisor, ((Opengl_Context *)context)->bound_buffer_status.buffer_status.guest_vao, currentVAO);
}

void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{
    //这个enable和disable不需要设置本地状态，因为guest在发送顶点数据的时候会告知是否enable
    express_printf("%llx glDisableVertexAttribArray %u\n", (uint64_t)context, index);
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);
        glDisableVertexArrayAttribEXT(status->guest_vao, index);
    }
    else
    {
        glDisableVertexAttribArray(index);
    }
}
void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{
    //这个enable和disable不需要设置本地状态，因为guest在发送顶点数据的时候会告知是否enable
    LOGD("%llx glEnableVertexAttribArray %u", (uint64_t)context, index);
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);
        glEnableVertexArrayAttribEXT(status->guest_vao, index);
    }
    else
    {
        glEnableVertexAttribArray(index);
    }
    LOGD("d_glEnableVertexAttribArray_origin index %d", index);
}

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

#ifdef STD_DEBUG_LOG
    GLint cu_vao = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &cu_vao);
    express_printf("%llx glDrawArrays mode %x first %d count %d vao %d\n", (uint64_t)context, mode, first, count, cu_vao);
#endif
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            // status->host_current_texture_unit[0] = status->current_texture_external;
            express_printf("use external texture %d\n", status->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawArrays(mode, first, count);
        express_printf("glDrawArrays end\n");
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);

            LOGD("gldrawarrays use external texture %d", status->current_texture_external);

            GLuint glerror = glGetError();
            if (glerror != GL_NO_ERROR) {
                LOGE("error! glDrawArrays glBindTexture glGetError %x texture %d", glerror, status->current_texture_external);
            }
            
            GLint width, height;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            LOGD("use external texture %d %d %d", status->current_texture_external, width, height);
        }

        GLint programID = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &programID);
        GLint texCoordsLocation = glGetAttribLocation(programID, "texCoords");
        GLint positionLocation = glGetAttribLocation(programID, "position");
        GLint enabled;

    // 是否启用
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        LOGD("glDrawArrays programID %d texCoordsLocation %d positionLocation %d %d", programID, texCoordsLocation, positionLocation, enabled);

        GLuint glerror = glGetError();
        if (glerror != GL_NO_ERROR) {
            LOGD("error! before glDrawArrays glGetError %x", glerror);
        }

        glDrawArrays(mode, first, count);

        GLuint rtextureId1 = 0;
        GLuint rfboID = 0;
        GLuint wtextureId1 = 0;
        GLuint wfboID = 0;

        GLuint curtex = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &curtex);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &rfboID);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &wfboID);

        glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&rtextureId1);
        glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&wtextureId1);

        LOGD("current glDrawArrays %d %d %d fbo binded texture %d %d current texture %d fbo %d %d", mode, first, count, rtextureId1, wtextureId1, curtex, rfboID, wfboID);

        glerror = glGetError();
        if (glerror != GL_NO_ERROR) {
            LOGD("error! after glDrawArrays glGetError %x fbo %d %d", glerror, rfboID, wfboID);
        }
        
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawArraysInstanced(mode, first, count, instancecount);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        glDrawArraysInstanced(mode, first, count, instancecount);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElements(mode, count, type, (void *)indices);
    }
    else
    {
#ifdef STD_DEBUG_LOG
        GLint ebo = 0;
        GLint vbo = 0;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
        express_printf("drawElements %x %d %x %lx vbo %d ebo %d\n", mode, (int)count, type, indices, vbo, ebo);
#endif

        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            LOGD("use external texture %d", status->current_texture_external);
        }
        if (g_ops.express_gpu_gl_debug_enable)
        {
            GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                GLint fbo;
                glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fbo);
                LOGE("error! context %llx draw framebuffer %u not complete! status %x error %x",(uint64_t)context, fbo, status, glGetError());

                GLint color0, color1, depth, stencil, depth_stencil;
                glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &color0);
                glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &color1);
                glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &depth);
                glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &stencil);
                glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &depth_stencil);
                LOGI("    attachment type: color0 %x color1 %x depth %x stencil %x depth_stencil %x", color0, color1, depth, stencil, depth_stencil);
            }
        }

        glDrawElements(mode, count, type, (void *)indices);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

GLint set_indices_data(void *context, void *pointer, GLint length)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLint buffer_loc = 0;
    unsigned char *map_pointer = NULL;

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        if (length > point_data->indices_buffer_len)
        {
            //当前的缓冲区大小不足，直接将原来的缓冲区加到当前最大大小的两倍，类似于vector的翻倍机制

            int alloc_size = length * BUFFER_MULTIPLY_FACTOR;
            if (alloc_size < 1024)
            {
                alloc_size = 1024;
            }

            // todo stream_draw需要验证
            glNamedBufferData(point_data->indices_buffer_object, alloc_size, NULL, GL_STREAM_DRAW);
            point_data->indices_buffer_len = alloc_size;
            map_pointer = glMapNamedBufferRange(point_data->indices_buffer_object, 0, length,
                                                GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

            g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer, 0, length);
            glFlushMappedNamedBufferRange(point_data->indices_buffer_object, 0, length);

            point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
            buffer_loc = 0;
        }
        else if (length > point_data->remain_indices_buffer_len)
        {
            map_pointer = glMapNamedBufferRange(point_data->indices_buffer_object, 0, point_data->indices_buffer_len,
                                                GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

            // TODO 测试是否需要从0开始映射
            g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer, 0, length);

            glFlushMappedNamedBufferRange(point_data->indices_buffer_object, 0, length);

            point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
            buffer_loc = 0;
        }
        else
        {
            map_pointer = glMapNamedBufferRange(point_data->indices_buffer_object,
                                                point_data->indices_buffer_len - point_data->remain_indices_buffer_len, length,
                                                GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

            g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer, 0, length);

            glFlushMappedNamedBufferRange(point_data->indices_buffer_object, 0, length);

            buffer_loc = point_data->indices_buffer_len - point_data->remain_indices_buffer_len;
            point_data->remain_indices_buffer_len -= length;
        }

        express_printf("indices loc %d %d\n", buffer_loc, buffer_loc + length);

        glUnmapNamedBuffer(point_data->indices_buffer_object);
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
        if (length > point_data->indices_buffer_len)
        {
            //当前的缓冲区大小不足，直接将原来的缓冲区加到当前最大大小的两倍，类似于vector的翻倍机制

            int alloc_size = length * BUFFER_MULTIPLY_FACTOR;
            if (alloc_size < 1024)
            {
                alloc_size = 1024;
            }

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, alloc_size, NULL, GL_DYNAMIC_DRAW);
            point_data->indices_buffer_len = alloc_size;
            map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length,
                                           GL_MAP_WRITE_BIT);

            g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer, 0, length);

            point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
            buffer_loc = 0;
        }
        else if (length > point_data->remain_indices_buffer_len)
        {
            map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, point_data->indices_buffer_len,
                                           GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

            // TODO 测试是否需要从0开始映射
            g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer, 0, length);

            point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
            buffer_loc = 0;
        }
        else
        {
            map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,
                                           point_data->indices_buffer_len - point_data->remain_indices_buffer_len, length,
                                           GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

            g_ops.read_from_guest_mem((Guest_Mem *)pointer, map_pointer, 0, length);


            buffer_loc = point_data->indices_buffer_len - point_data->remain_indices_buffer_len;
            point_data->remain_indices_buffer_len -= length;
        }

        express_printf("indices loc %d %d\n", buffer_loc, buffer_loc + length);

        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }

    return buffer_loc;
}

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{ //没有绑定ebo的情况。这个时候indices是真实的数组，而不是偏移

    int len = count * gl_sizeof(type);

    express_printf("drawElements without %x %d %x %lx len %d\n", mode, (int)count, type, indices, len);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, (void *)indices, len); //非dsa模式，会在这个函数里绑定ebo，所以这个buffer_loc是offset

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElements(mode, count, type, (const void *)(uint64_t)buffer_loc);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }

        // GLuint currentVAO = 0;
        // glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&currentVAO);
        //     GLuint currentVBO = 0, currentEBO = 0;
        // glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&currentVBO);
        // glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&currentEBO);
        // LOGI("Current VAO ID: %d %d %d", currentVAO, currentVBO, currentEBO);

        glDrawElements(mode, count, type, (const void *)(uint64_t)buffer_loc);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    int len = count * gl_sizeof(type);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, (void *)indices, len);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsInstanced(mode, count, type, (const void *)(uint64_t)buffer_loc, instancecount);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }

        glDrawElementsInstanced(mode, count, type, (const void *)(uint64_t)buffer_loc, instancecount);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsInstanced(mode, count, type, (void *)indices, instancecount);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        glDrawElementsInstanced(mode, count, type, (void *)indices, instancecount);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{
    //这里的start和end不会对此时的操作有影响，因为只要在那个范围内了，该传输过去还是得传输过去，只是最后会不会用的问题

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        // buffer_binding_status_sync(context, GL_ELEMENT_ARRAY_BUFFER);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawRangeElements(mode, start, end, count, type, (void *)indices);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            LOGD("gldrawRangeElements use external texture %d", status->current_texture_external);
        }


        GLint current_vao;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);
        if (current_vao == 0) {
            LOGE("No VAO bound! context %llx window %llx", (int64_t)context, (int64_t)opengl_context->window);
        }

        GLint current_vbo;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current_vbo);
        if (current_vbo == 0) {
            LOGD("No VBO bound!");
        }

        GLint current_ebo;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current_ebo);
        if (current_ebo == 0) {
            LOGE("No EBO bound!");
            // GLuint ebo = get_guest_binding_buffer(context, GL_ELEMENT_ARRAY_BUFFER);
            
        }
        // LOGI("current ebo should be %d %d", current_ebo, (&(opengl_context->bound_buffer_status.buffer_status))->guest_element_array_buffer);
        // LOGI("current vbo should be %d %d", current_vbo, (&(opengl_context->bound_buffer_status.buffer_status))->guest_array_buffer);
        GLint currentFBO = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
        GLint textureID = 0;
        glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &textureID);

        LOGD("in drawRangeElements %d start %d end %d count %d type %d currentFBO %d %d %d %d %d %d", indices, start, end, count, type, currentFBO, textureID, current_ebo, current_vbo, opengl_context->current_read_fbo, opengl_context->current_write_fbo);

        // GLint size;
        // void* data;
        // glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        // if (size > 0) {
        //     // void* data = g_malloc0(size);
        //     // if (data) {
        //     //     memset(data, 0, size);
        //     //     glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
        //     //     LOGI("saving elementbuffer %d context first 4 bytes %x %x %x %x", current_ebo, ((char*)data)[1], ((char*)data)[12], ((char*)data)[22], ((char*)data)[16]);
        //     // }
        //     data = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, size, GL_MAP_READ_BIT);
        //     if (data) {
        //         // 如果映射成功，直接访问数据
        //         LOGI("saving elementbuffer %d context first 4 bytes %x %x %x %x", current_ebo, ((char*)data)[1], ((char*)data)[12], ((char*)data)[22], ((char*)data)[16]);
                
        //         // 完成后取消映射
        //         glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        //     }
        // }

        
        glDrawRangeElements(mode, start, end, count, type, (void *)indices);
        // float r = (float)rand() / RAND_MAX;
        // float g = (float)rand() / RAND_MAX;
        // float b = (float)rand() / RAND_MAX;

        // glClearColor(r, g, b, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, (void *)indices, len);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawRangeElements(mode, start, end, count, type, (const void *)(uint64_t)buffer_loc);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        glDrawRangeElements(mode, start, end, count, type, (const void *)(uint64_t)buffer_loc);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void d_glVertexBindingDivisor_special(void *context, GLuint bindingindex, GLuint divisor)
{
    glVertexBindingDivisor(bindingindex, divisor);
}

void d_glDrawArraysIndirect_with_bound(void *context, GLenum mode, GLintptr indirect)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);

        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawArraysIndirect(mode, (void *)indirect);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawArraysIndirect(mode, (void *)indirect);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawArraysIndirect_without_bound(void *context, GLenum mode, const void *indirect)
{
    //由于indirect指向的数据只是一个结构体，里面的数据量很少，所以是直接接在前面两个参数后面的，不用单独来一个para
    //由于数据量很少，所以就不用专门再搞个buffer来存储了
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        vao_binding_status_sync(context);
        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawArraysIndirect(mode, indirect);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawArraysIndirect(mode, indirect);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawElementsIndirect_with_bound(void *context, GLenum mode, GLenum type, GLintptr indirect)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);

        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsIndirect(mode, type, (void *)indirect);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawElementsIndirect(mode, type, (void *)indirect);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawElementsIndirect_without_bound(void *context, GLenum mode, GLenum type, const void *indirect)
{
    //由于indirect指向的数据只是一个结构体，里面的数据量很少，所以是直接接在前面两个参数后面的，不用单独来一个para
    //由于数据量很少，所以就不用专门再搞个buffer来存储了
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);

        buffer_binding_status_sync(context, GL_DRAW_INDIRECT_BUFFER);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsIndirect(mode, type, indirect);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
        }
        glDrawElementsIndirect(mode, type, indirect);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawElementsBaseVertex_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLint basevertex)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsBaseVertex(mode, count, type, (void *)indices, basevertex);
    }
    else
    {
        GLuint ebo = 0;
        GLuint vbo = 0;
#ifdef STD_DEBUG_LOG
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
#endif
        express_printf("drawElements %x %d %x %lx vbo %u ebo %u\n", mode, (int)count, type, indices, vbo, ebo);

        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }

        glDrawElementsBaseVertex(mode, count, type, (void *)indices, basevertex);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawElementsBaseVertex_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex)
{

    int len = count * gl_sizeof(type);

    express_printf("drawElements without %x %d %x %lx len %d\n", mode, (int)count, type, indices, len);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, (void *)indices, len);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsBaseVertex(mode, count, type, (const void *)(uint64_t)buffer_loc, basevertex);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }

        glDrawElementsBaseVertex(mode, count, type, (const void *)(uint64_t)buffer_loc, basevertex);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void d_glDrawRangeElementsBaseVertex_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices, GLint basevertex)
{
    //这里的start和end不会对此时的操作有影响，因为只要在那个范围内了，该传输过去还是得传输过去，只是最后会不会用的问题

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawRangeElementsBaseVertex(mode, start, end, count, type, (void *)indices, basevertex);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        glDrawRangeElementsBaseVertex(mode, start, end, count, type, (void *)indices, basevertex);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawRangeElementsBaseVertex_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex)
{

    int len = count * gl_sizeof(type);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, (void *)indices, len);
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawRangeElementsBaseVertex(mode, start, end, count, type, (const void *)(uint64_t)buffer_loc, basevertex);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        glDrawRangeElementsBaseVertex(mode, start, end, count, type, (const void *)(uint64_t)buffer_loc, basevertex);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void d_glDrawElementsInstancedBaseVertex_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount, GLint basevertex)
{

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        vao_binding_status_sync(context);
        // buffer_binding_status_sync(context, GL_ELEMENT_ARRAY_BUFFER);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;

            // status->host_current_texture_unit[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsInstancedBaseVertex(mode, count, type, (void *)indices, instancecount, basevertex);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        glDrawElementsInstancedBaseVertex(mode, count, type, (void *)indices, instancecount, basevertex);
        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }
    }
}

void d_glDrawElementsInstancedBaseVertex_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex)
{

    int len = count * gl_sizeof(type);

    vao_binding_status_sync(context);

    GLint buffer_loc = set_indices_data(context, (void *)indices, len);

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Texture_Binding_Status *status = &(opengl_context->texture_binding_status);

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTextureUnit(0, status->current_texture_external);
            status->host_current_texture_2D[0] = status->current_texture_external;
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }
        else
        {
            texture_unit_status_sync(context, -1);
        }

        glDrawElementsInstancedBaseVertex(mode, count, type, (const void *)(uint64_t)buffer_loc, instancecount, basevertex);
    }
    else
    {
        if (opengl_context->is_using_external_program == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, status->current_texture_external);
            // LOGI("use external texture %d", opengl_context->current_texture_external);
        }

        glDrawElementsInstancedBaseVertex(mode, count, type, (const void *)(uint64_t)buffer_loc, instancecount, basevertex);

        if (opengl_context->is_using_external_program == 1)
        {
            glBindTexture(GL_TEXTURE_2D, status->host_current_texture_2D[0]);
            glActiveTexture(status->host_current_active_texture + GL_TEXTURE0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

