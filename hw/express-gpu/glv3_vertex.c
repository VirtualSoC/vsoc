// #define STD_DEBUG_LOG

#include "express-gpu/glv3_vertex.h"



GLint set_vertex_attrib_data(void *context, GLuint index, GLuint offset, GLuint length, const void *pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLuint max_len = offset + length;

    unsigned char *map_pointer = NULL;
    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index]);

    if (max_len > point_data->buffer_len[index])
    {
        //当前的缓冲区大小不足，直接将原来的缓冲区加到当前最大大小的两倍，类似于vector的翻倍机制
        // glDeleteBuffers(1, &(point_data->buffer_object));
        // glGenBuffers(1, &(point_data->buffer_object));

        //todo stream_draw需要验证
        glBufferData(GL_ARRAY_BUFFER, max_len * BUFFER_MULTIPLY_FACTOR, NULL, GL_STREAM_DRAW);
        point_data->buffer_len[index] = max_len * BUFFER_MULTIPLY_FACTOR;
        map_pointer = glMapBufferRange(GL_ARRAY_BUFFER, offset, length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);
        glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);

        point_data->buffer_loc[index] = 0;
        point_data->remain_buffer_len[index] = max_len * 2 - max_len;
    }
    else if (length > point_data->remain_buffer_len[index])
    {
        map_pointer = glMapBufferRange(GL_ARRAY_BUFFER, 0, point_data->buffer_len[index],
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        //TODO 测试是否需要从0开始映射
        guest_write((Guest_Mem *)pointer, map_pointer + offset, 0, length);

        glFlushMappedBufferRange(GL_ARRAY_BUFFER, offset, length);

        point_data->buffer_loc[index] = 0;
        point_data->remain_buffer_len[index] = point_data->buffer_len[index] - max_len;

        // glBufferData(GL_ARRAY_BUFFER, point_data->buffer_len[index], NULL, GL_STREAM_DRAW);
        // map_pointer=glMapBufferRange(GL_ARRAY_BUFFER, offset, length,
        //     GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

        // guest_write((Guest_Mem *)pointer,map_pointer,0,length);
        // glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);

        // point_data->buffer_loc[index]=0;
        // point_data->remain_buffer_len[index]=max_len*2-max_len;
    }
    else
    {
        map_pointer = glMapBufferRange(GL_ARRAY_BUFFER,
                                       point_data->buffer_len[index] - point_data->remain_buffer_len[index], length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

        glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);

        point_data->buffer_loc[index] = point_data->buffer_len[index] - point_data->remain_buffer_len[index] - offset;
        point_data->remain_buffer_len[index] -= length;
    }

    express_printf("attrib point loc %d %d\n", point_data->buffer_loc[index], point_data->buffer_loc[index] + length);

    glUnmapBuffer(GL_ARRAY_BUFFER);

    return point_data->buffer_loc[index];
}

void d_glVertexAttribPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    glVertexAttribPointer(index, size, type, normalized, stride, loc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribIPointer_without_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint offset, GLuint length, const void *pointer)
{
    GLint loc = set_vertex_attrib_data(context, index, offset, length, pointer);

    glVertexAttribIPointer(index, size, type, stride, loc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint index_father, GLintptr offset)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->attrib_point;

    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

    express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);
    glVertexAttribPointer(index, size, type, normalized, stride, offset + point_data->buffer_loc[index_father]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint index_father, GLintptr offset)
{

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->attrib_point;

    glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object[index_father]);

    express_printf("pointer offset %lld\n", offset + point_data->buffer_loc[index_father]);

    glVertexAttribIPointer(index, size, type, stride, offset + point_data->buffer_loc[index_father]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{

    glVertexAttribPointer(index, size, type, normalized, stride, (void *)pointer);
    return;
}

void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{

    glVertexAttribIPointer(index, size, type, stride, (void *)pointer);
    return;
}



void d_glBindVertexArray_special(void *context, GLuint array)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    // Buffer_Status *vao_status = g_hash_table_lookup(bound_buffer->vao_status, GUINT_TO_POINTER(array));

    // bound_buffer->buffer_status=vao_status;

    Attrib_Point *temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(array));
    if (temp_point == NULL)
    {
        temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(0));
    }

    bound_buffer->attrib_point = temp_point;

    bound_buffer->buffer_status.element_array_buffer = temp_point->element_array_buffer;
    
    glBindVertexArray(array);
}

void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{
    //反正array draw的时候还会传divisor，所以这里就不用保存了
    express_printf("glVertexAttribDivisor %u %u\n", index, divisor);
    glVertexAttribDivisor(index, divisor);
}

//这个实际调用不到
// void d_glGetVertexAttribPointerv_with_bound(void *context, GLuint index, GLenum pname, GLintptr *pointer)
// {
//     aaaaa
// }

void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{
    //这个enable和disable不需要设置本地状态，因为guest在发送顶点数据的时候会告知是否enable
    glDisableVertexAttribArray(index);
}
void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{
    //这个enable和disable不需要设置本地状态，因为guest在发送顶点数据的时候会告知是否enable

    glEnableVertexAttribArray(index);
}

void d_glDrawArrays_origin(void *context, GLenum mode, GLint first, GLsizei count)
{
    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;

    // set_attrib_point(context,1);

    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    //     // express_printf("glBindBuffer %x\n",glGetError());

    // }

    // @todo 啥都不设置直接draw会导致segment fault，需要额外处理

    glDrawArrays(mode, first, count);
    // express_printf("glDrawArrays %x\n",glGetError());

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set_attrib_point(context,1);
    // glDrawArrays(mode, first, count);
    // express_printf("glDrawArrays %x\n",glGetError());

    // if (status->array_buffer == 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    //     express_printf("glBindBuffer %x\n",glGetError());

    // }
}

void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;

    // set_attrib_point(context,instancecount);
    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    // }

    glDrawArraysInstanced(mode, first, count, instancecount);
}

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{
    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;

    // set_attrib_point(context,1);
    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    // }
    // GLuint ebo;
    // GLuint vbo;
    // glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
    // glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
    // printf("drawElements %x %d %x %lx vbo %u ebo %u\n",mode,(int)count,type,indices, vbo, ebo);
    
    glDrawElements(mode, count, type, (void *)indices);
}

GLint set_indices_data(void *context, void *pointer, GLint length)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->attrib_point;

    GLint buffer_loc = 0;
    unsigned char *map_pointer = NULL;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
    if (length > point_data->indices_buffer_len)
    {
        //当前的缓冲区大小不足，直接将原来的缓冲区加到当前最大大小的两倍，类似于vector的翻倍机制
        // glDeleteBuffers(1, &(point_data->buffer_object));
        // glGenBuffers(1, &(point_data->buffer_object));

        //todo stream_draw需要验证
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * BUFFER_MULTIPLY_FACTOR, NULL, GL_STREAM_DRAW);
        point_data->indices_buffer_len = length * BUFFER_MULTIPLY_FACTOR;
        map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);
        glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length);

        point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
        buffer_loc = 0;
    }
    else if (length > point_data->remain_indices_buffer_len)
    {
        map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, point_data->indices_buffer_len,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        //TODO 测试是否需要从0开始映射
        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

        glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length);

        point_data->remain_indices_buffer_len = point_data->indices_buffer_len - length;
        buffer_loc = 0;
    }
    else
    {
        map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,
                                       point_data->indices_buffer_len - point_data->remain_indices_buffer_len, length,
                                       GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

        guest_write((Guest_Mem *)pointer, map_pointer, 0, length);

        glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, length);

        buffer_loc = point_data->indices_buffer_len - point_data->remain_indices_buffer_len;
        point_data->remain_indices_buffer_len -= length;

        // glBufferData(GL_ARRAY_BUFFER, point_data->indices_buffer_len, NULL, GL_STREAM_DRAW);
        // map_pointer=glMapBufferRange(GL_ARRAY_BUFFER, 0, length,
        //     GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

        // guest_write((Guest_Mem *)pointer,map_pointer,0,length);
        // glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, length);

        // point_data->remain_indices_buffer_len=point_data->indices_buffer_len-length;
        // buffer_loc=0;
    }

    express_printf("indices loc %d %d\n", buffer_loc, buffer_loc + length);

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    return buffer_loc;
}

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    // printf("drawElements without %x %d %x %lx len %d\n",mode,(int)count,type,indices,len);

    GLint buffer_loc = set_indices_data(context, indices, len);
    glDrawElements(mode, count, type, buffer_loc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    // // @todo 这里根据数据的长度来决定到底是采用普通模式还是缓冲区模式，需要测试哪个长度更节约时间
    // if(len<40000){
    //     glDrawElements(mode,count,type,indices);
    // }else{
    //     set_attrib_point_index(context,indices,len);

    //     glDrawElements(mode,count,type,0);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // }
}

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    int len = count * gl_sizeof(type);

    GLint buffer_loc = set_indices_data(context, indices, len);
    glDrawElementsInstanced(mode, count, type, buffer_loc, instancecount);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    // // @todo 这里根据数据的长度来决定到底是采用普通模式还是缓冲区模式，需要测试哪个长度更节约时间
    // if(len<40000){
    //     glDrawElementsInstanced(mode,count,type,indices,instancecount);
    // }else{
    //     set_attrib_point_index(context,indices,len);
    //     glDrawElementsInstanced(mode,count,type,0,instancecount);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // }
}

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount)
{
    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;

    // set_attrib_point(context,instancecount);
    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    // }

    glDrawElementsInstanced(mode, count, type, (void *)indices, instancecount);
}

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{
    //这里的start和end不会对此时的操作有影响，因为只要在那个范围内了，该传输过去还是得传输过去，只是最后会不会用的问题

    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;

    // set_attrib_point(context,1);
    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    // }

    glDrawRangeElements(mode, start, end, count, type, (void *)indices);
}

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{

    int len = count * gl_sizeof(type);

    GLint buffer_loc = set_indices_data(context, indices, len);

    glDrawRangeElements(mode, start, end, count, type, buffer_loc);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // // @todo 这里根据数据的长度来决定到底是采用普通模式还是缓冲区模式，需要测试哪个长度更节约时间
    // if(len<40000){
    //     glDrawRangeElements(mode, start, end, count, type, indices);
    // }else{
    //     set_attrib_point_index(context,indices,len);

    //     glDrawRangeElements(mode, start, end, count, type, 0);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // }
}

void d_glVertexBindingDivisor_special(void *context, GLuint bindingindex, GLuint divisor)
{
    glVertexBindingDivisor(bindingindex, divisor);
}


void d_glDrawArraysIndirect_with_bound(void *context, GLenum mode, GLintptr indirect)
{
    glDrawArraysIndirect(mode, (void *)indirect);
}

void d_glDrawArraysIndirect_without_bound(void *context, GLenum mode, const void *indirect)
{
    //由于indirect指向的数据只是一个结构体，里面的数据量很少，所以是直接接在前面两个参数后面的，不用单独来一个para
    //由于数据量很少，所以就不用专门再搞个buffer来存储了
    glDrawArraysIndirect(mode, indirect);
}

void d_glDrawElementsIndirect_with_bound(void *context, GLenum mode, GLenum type, GLintptr indirect)
{
    glDrawElementsIndirect(mode, type, (void *)indirect);
}


void d_glDrawElementsIndirect_without_bound(void *context, GLenum mode, GLenum type, const void *indirect)
{
    //由于indirect指向的数据只是一个结构体，里面的数据量很少，所以是直接接在前面两个参数后面的，不用单独来一个para
    //由于数据量很少，所以就不用专门再搞个buffer来存储了
    glDrawElementsIndirect(mode, type, indirect);
}