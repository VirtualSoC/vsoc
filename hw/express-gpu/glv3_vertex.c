// #define STD_DEBUG_LOG

#include "express-gpu/glv3_vertex.h"

// /**
//  * @brief 利用之前发送来的顶点数据，将这些顶点数据复制到GPU显存中，假如当前是即时顶点模式，则是draw之前的必备操作
//  *
//  * @param context 实际是Opengl_Context
//  * @param data host端的内存
//  * @param len 内存数据的长度
//  */
// void set_attrib_point_index(void *context,void *data,int len){

//     express_printf("set_attrib_point_index %d\n",len);

//     Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
//     Buffer_Status *status = bound_buffer->buffer_status;

//     Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
//     if (point_data == NULL)
//     {
//         point_data = g_malloc(sizeof(Attrib_Point));
//         memset(point_data, 0, sizeof(Attrib_Point));
//         glGenBuffers(1,&(point_data->indices_buffer_object));
//         glGenBuffers(1,&(point_data->buffer_object));

//         g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer), (gpointer)point_data);
//     }

//     if(len>point_data->indices_buffer_len){
//         // glDeleteBuffers(1,&(point_data->indices_buffer_object));
//         // glGenBuffers(1,&(point_data->indices_buffer_object));
//         //todo stream_draw需要测试验证
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER,len*2,NULL,GL_STREAM_DRAW);
//         point_data->indices_buffer_len=len*2;
//     }
//     else
//     {
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_len, NULL, GL_STREAM_DRAW);

//     }
//     //确定了缓冲区大小后，映射取得其指针，尽可能只修改一小部分
//     //GL_MAP_INVALIDATE_RANGE_BIT 用于缓冲区孤立，防止隐式同步
//     GLubyte *map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, len,
//             GL_MAP_WRITE_BIT |GL_MAP_INVALIDATE_RANGE_BIT);

//     // host_guest_buffer_exchange(data,map_pointer,0,len,1);
//     memcpy(data,map_pointer,len);
//     glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

// }

// void set_attrib_point(void *context,GLsizei instancecount){
//     express_printf("set_attrib_point %d\n",instancecount);

//     Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
//     Buffer_Status *status = bound_buffer->buffer_status;
//     // if (status->array_buffer == 0)
//     // {
//         //绑定为0的情况，这个时候需要把数据复制到临时的array_buffer中
//         Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
//         if (point_data == NULL)
//         {
//             return;
//         }
//         flush_array_buffer(point_data,instancecount);
//     // }
// }

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

// void d_glGenVertexArrays_origin(void *context, GLsizei n, GLuint *arrays)
// {
//     Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

//     // glGenVertexArrays(n, arrays);

//     for (int i = 0; i < n; i++)
//     {

//         // Buffer_Status *status=g_malloc(sizeof(Buffer_Status));

//         // memset(status,0,sizeof(Buffer_Status));
//         // status->vertex_array_buffer=arrays[i];

//         // g_hash_table_insert(bound_buffer->vao_status, GINT_TO_POINTER(arrays[i]), (gpointer)status);

//         Attrib_Point *point_data = g_malloc(sizeof(Attrib_Point));
//         memset(point_data, 0, sizeof(Attrib_Point));
//         glGenBuffers(1, &(point_data->indices_buffer_object));
//         glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, point_data->buffer_object);

//         g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(arrays[i]), (gpointer)point_data);
//     }

//     return;
// }

// void d_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint *arrays)
// {
//     Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

//     for (int i = 0; i < n; i++)
//     {
//         GLuint vao_index = arrays[i];
//         if(vao_index==0){
//             continue;
//         }
//         // Buffer_Status *vao_status = g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(vao_index));
//         Attrib_Point *vao_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(vao_index));

//         // if(vao_status==bound_buffer->buffer_status){
//         if (bound_buffer->attrib_point == vao_point)
//         {
//             // bound_buffer->buffer_status=g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(0));
//             bound_buffer->attrib_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(0));
//         }

//         // // GLuint buffer_index[2];
//         // // buffer_index[0]=vao_point->indices_buffer_object;
//         // // buffer_index[1]=vao_point->buffer_object;

//         // // glDeleteBuffers(2,buffer_index);
//         // // g_free(vao_point);
//         // // g_free(vao_status);
//         // g_hash_table_remove(bound_buffer->vao_status, GINT_TO_POINTER(vao_index));
//         g_hash_table_remove(bound_buffer->vao_point_data, GINT_TO_POINTER(vao_index));
//     }

//     glDeleteVertexArrays(n, arrays);
// }

void d_glBindVertexArray_special(void *context, GLuint array)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    // Buffer_Status *vao_status = g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(array));

    // bound_buffer->buffer_status=vao_status;

    Attrib_Point *temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(array));
    if (temp_point == NULL)
    {
        temp_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(0));
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

    // set_attrib_point(context,1);
    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    // }
    // //然后处理indices为数组的情况
    // int len=count*gl_sizeof(type);
    // // Guest_Mem *guest_mem=(Guest_Mem *)indices;
    // // Scatter_Data *s_data=guest_mem->scatter_data;

    // // if (guest_mem->all_len == 0)
    // // {
    // //     //pixels=NULL
    // //     glDrawElements(mode,count,type,NULL);
    // //     return;
    // // }

    // // if(len==s_data[0].len){
    // //     glDrawElements(mode,count,type,s_data[0].data);
    // // }else{
    // //     set_attrib_point_index(context,s_data,len);
    // //     glDrawElements(mode,count,type,0);
    // //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // // }
    // express_printf("indices:");
    // for(int i=0;i<len/sizeof(unsigned short);i++){
    //     express_printf("%hu ",((unsigned short *)indices)[i]);
    // }
    // express_printf("\n");

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

    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;

    // set_attrib_point(context,instancecount);
    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    // }
    // //然后处理indices为数组的情况
    // int len=count*gl_sizeof(type);

    // // Guest_Mem *guest_mem=(Guest_Mem *)indices;
    // // Scatter_Data *s_data=guest_mem->scatter_data;

    // // if (guest_mem->all_len == 0)
    // // {
    // //     //pixels=NULL
    // //     glDrawElementsInstanced(mode,count,type,NULL,instancecount);
    // //     return;
    // // }

    // // if(len==s_data[0].len){
    // //     glDrawElementsInstanced(mode,count,type,s_data[0].data,instancecount);
    // // }else{
    // //     set_attrib_point_index(context,s_data,len);
    // //     glDrawElementsInstanced(mode,count,type,0,instancecount);
    // //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // // }

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

    // Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    // Buffer_Status *status = bound_buffer->buffer_status;

    // set_attrib_point(context,1);
    // if (status->array_buffer != 0)
    // {
    //     glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    // }
    // //然后处理indices为数组的情况
    // int len=count*gl_sizeof(type);
    // // Guest_Mem *guest_mem=(Guest_Mem *)indices;
    // // Scatter_Data *s_data=guest_mem->scatter_data;

    // // if (guest_mem->all_len == 0)
    // // {
    // //     //pixels=NULL
    // //     glDrawRangeElements(mode, start, end, count, type, NULL);
    // //     return;
    // // }

    // // if(len==s_data[0].len){
    // //     glDrawRangeElements(mode, start, end, count, type, s_data[0].data);
    // // }else{
    // //     set_attrib_point_index(context,s_data,len);
    // //     glDrawRangeElements(mode, start, end, count, type, 0);
    // //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // // }

    // // @todo 这里根据数据的长度来决定到底是采用普通模式还是缓冲区模式，需要测试哪个长度更节约时间
    // if(len<40000){
    //     glDrawRangeElements(mode, start, end, count, type, indices);
    // }else{
    //     set_attrib_point_index(context,indices,len);

    //     glDrawRangeElements(mode, start, end, count, type, 0);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // }
}