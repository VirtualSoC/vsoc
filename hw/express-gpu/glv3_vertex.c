#include "express-gpu/glv3_vertex.h"



/**
 * @brief 利用之前发送来的顶点数据，将这些顶点数据复制到GPU显存中，假如当前是即时顶点模式，则是draw之前的必备操作
 * 
 * @param context 实际是Opengl_Context
 * @param data guest端的内存数据，类型为Scatter_Data
 * @param len guest端的内存数据的长度
 */
void set_attrib_point_index(void *context,Scatter_Data *data,int len){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
    if (point_data == NULL)
    {
        point_data = g_malloc(sizeof(Attrib_Point));
        memset(point_data, 0, sizeof(Attrib_Point));
        glGenBuffers(1,&(point_data->indices_buffer_object));
        glGenBuffers(1,&(point_data->buffer_object));
        
        g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer), (gpointer)point_data);
    }

    if(len>point_data->indices_buffer_len){
        // glDeleteBuffers(1,&(point_data->indices_buffer_object));
        // glGenBuffers(1,&(point_data->indices_buffer_object));
        //todo stream_draw需要测试验证
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,len*2,NULL,GL_STREAM_DRAW);
        point_data->indices_buffer_len=len*2;
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, point_data->indices_buffer_len, NULL, GL_STREAM_DRAW);

    }
    //确定了缓冲区大小后，映射取得其指针，尽可能只修改一小部分
    //GL_MAP_INVALIDATE_RANGE_BIT 用于缓冲区孤立，防止隐式同步
    GLubyte *map_pointer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, len,
            GL_MAP_WRITE_BIT |GL_MAP_INVALIDATE_RANGE_BIT);
    
    host_guest_buffer_exchange(data,map_pointer,0,len,1);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

}


void set_attrib_point(void *context,GLsizei instancecount){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    if (status->array_buffer == 0)
    {
        //绑定为0的情况，这个时候需要把数据复制到临时的array_buffer中
        Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
        if (point_data == NULL)
        {
            return;
        }
        flush_array_buffer(point_data,instancecount);
    }
}




void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{
    glVertexAttribIPointer(index, size, type, stride, (void *)pointer);
}

/**
 * @brief 设置顶点数据，用于之后draw时使用。这里对guest端的顶点数据指针进行了数据维持，实现了长时映射
 * 
 * @param context Opengl_Context类型的context
 * @param index 顶点编号
 * @param size 顶点数据的大小
 * @param pointer 顶点数据的指针，实际为Guest_Mem类型
 */
void d_glVertexAttribIPointer_data(void *context, const void *int_data, const void *pointer)
{
    GLuint index;
    GLuint length;
    Guest_Mem *guest_mem_int=(Guest_Mem *)int_data;

    guest_write(guest_mem_int,&index,0,sizeof(GLuint));
    guest_write(guest_mem_int,&length,sizeof(GLuint),sizeof(GLuint));


    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
    if (point_data == NULL)
    {
        point_data = g_malloc(sizeof(Attrib_Point));
        memset(point_data, 0, sizeof(Attrib_Point));
        glGenBuffers(1,&(point_data->indices_buffer_object));
        glGenBuffers(1,&(point_data->buffer_object));
        g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer), (gpointer)point_data);
    }

    Guest_Mem *guest_mem=(Guest_Mem *)pointer;

    Scatter_Data *s_data=guest_mem->scatter_data;

    //注意，这个地方需要将guest端内存长时保存，所以这里额外分配一个空间用于存储这个scatter_data
    if(point_data->data[index]!=NULL){
        g_free(point_data->data[index]);
        point_data->data[index]=NULL;
    }
    point_data->data[index] =g_malloc(guest_mem->num*sizeof(Scatter_Data));
    memcpy(point_data->data[index],s_data,guest_mem->num*sizeof(Scatter_Data));

    // point_data->data[index] = (Scatter_Data *)pointer;
    point_data->data_len[index] = length;

    return;
}

/**
 * @brief 设置顶点数据偏移，其偏移为其father里的数据的偏移。不同顶点数据可能指向同一段内存的不同位置，即使用了结构数组模式，这种情况直接设置偏移。
 * 
 * @param context Opengl_Context类型的context
 * @param index 顶点编号
 * @param size 顶点数据的大小
 * @param type 顶点数据的类型
 * @param stride 顶点数据读取下个顶点的偏移量
 * @param min_index 顶点的下标的最小值
 * @param max_index 顶点的下标的最大值
 * @param index_father 这个顶点数据的父顶点，其数据与父顶点在同一个结构数组内
 * @param divisor 实例化绘制时的divisor
 * @param enabled 这个顶点属性是否使能
 * @param pointer 数据偏移
 */
void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
    if (point_data == NULL)
    {
        //不可能出现这种情况
        return;
    }
    point_data->father[index] = index_father;
    point_data->offset[index] = pointer;
    point_data->size[index] = size;
    point_data->type[index] = type;
    point_data->stride[index] = stride;
    // point_data->normalized[index]=normalized;
    point_data->divisor[index] = divisor;
    point_data->invoke_type[index] = GL_INT;
    point_data->min_index = min_index;
    point_data->max_index = max_index;
}

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{
    glVertexAttribPointer(index, size, type, normalized, stride, (void *)pointer);
}

/**
 * @brief 设置顶点数据，用于之后draw时使用。这里对guest端的顶点数据指针进行了数据维持，实现了长时映射
 * 
 * @param context Opengl_Context类型的context
 * @param index 顶点编号
 * @param size 顶点数据的大小
 * @param pointer 顶点数据的指针，实际为Guest_Mem类型
 */
void d_glVertexAttribPointer_data(void *context, const void *int_data,  const void *pointer)
{

    GLuint index;
    GLuint length;
    Guest_Mem *guest_mem_int=(Guest_Mem *)int_data;


    guest_write(guest_mem_int,&index,0,sizeof(GLuint));
    guest_write(guest_mem_int,&length,sizeof(GLuint),sizeof(GLuint));

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
    if (point_data == NULL)
    {
        point_data = g_malloc(sizeof(Attrib_Point));
        memset(point_data, 0, sizeof(Attrib_Point));
        glGenBuffers(1,&(point_data->indices_buffer_object));
        glGenBuffers(1,&(point_data->buffer_object));
        g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer), (gpointer)point_data);
    }
    
    Guest_Mem *guest_mem=(Guest_Mem *)pointer;

    Scatter_Data *s_data=guest_mem->scatter_data;

    if(point_data->data[index]!=NULL){
        g_free(point_data->data[index]);
        point_data->data[index]=NULL;
    }
    point_data->data[index] =g_malloc(guest_mem->num*sizeof(Scatter_Data));
    memcpy(point_data->data[index],s_data,guest_mem->num*sizeof(Scatter_Data));

    point_data->data_len[index] = length;

    return;
}



/**
 * @brief 设置顶点数据偏移，其偏移为其father里的数据的偏移。不同顶点数据可能指向同一段内存的不同位置，即使用了结构数组模式，这种情况直接设置偏移。
 * 
 * @param context Opengl_Context类型的context
 * @param index 顶点编号
 * @param size 顶点数据的大小
 * @param type 顶点数据的类型
 * @param normalized 是否归一化
 * @param stride 顶点数据读取下个顶点的偏移量
 * @param min_index 顶点的下标的最小值
 * @param max_index 顶点的下标的最大值
 * @param index_father 这个顶点数据的父顶点，其数据与父顶点在同一个结构数组内
 * @param divisor 实例化绘制时的divisor
 * @param enabled 这个顶点属性是否使能
 * @param pointer 数据偏移
 */
void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
    if (point_data == NULL)
    {
        //不可能出现这种情况
        return;
    }
    point_data->father[index] = index_father;
    point_data->offset[index] = pointer;
    point_data->size[index] = size;
    point_data->type[index] = type;
    point_data->stride[index] = stride;
    point_data->normalized[index] = normalized;
    point_data->divisor[index] = divisor;
    point_data->invoke_type[index] = GL_FLOAT;
    point_data->min_index = min_index;
    point_data->max_index = max_index;
}

void d_glGenVertexArrays_origin(void *context, GLsizei n, GLuint *arrays)
{

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Buffer_Status *status=g_malloc(sizeof(Buffer_Status));

    memset(status,0,sizeof(Buffer_Status));
    g_hash_table_insert(bound_buffer->vao_status, GINT_TO_POINTER(0), (gpointer)status);


    Attrib_Point *point_data = g_malloc(sizeof(Attrib_Point));
    memset(point_data, 0, sizeof(Attrib_Point));
    glGenBuffers(1,&(point_data->indices_buffer_object));
    glGenBuffers(1,&(point_data->buffer_object));
    
    g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer), (gpointer)point_data);


    glGenVertexArrays(n, arrays);
}


void d_glDeleteVertexArrays_origin(void *context, GLsizei n, const GLuint *arrays)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    
    for(int i=0;i<n;i++){
        GLuint vao_index=arrays[i];
        Buffer_Status *vao_status = g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(vao_index));
        Attrib_Point *vao_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(vao_index));

        if(vao_status==bound_buffer->buffer_status){
            bound_buffer->buffer_status=g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(0));
        }

        GLuint buffer_index[2];
        buffer_index[0]=vao_point->indices_buffer_object;
        buffer_index[1]=vao_point->buffer_object;

        glDeleteBuffers(2,buffer_index);
        g_free(vao_point);
        g_free(vao_status);
        g_hash_table_remove(bound_buffer->vao_status, GINT_TO_POINTER(vao_index));
        g_hash_table_remove(bound_buffer->vao_point_data, GINT_TO_POINTER(vao_index));

    }


    glDeleteVertexArrays(n, arrays);
}


void d_glBindVertexArray_origin(void *context, GLuint array)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Buffer_Status *vao_status = g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(array));

    bound_buffer->buffer_status=vao_status;
    

    glBindVertexArray(array);
}


void d_glVertexAttribDivisor_origin(void *context, GLuint index, GLuint divisor)
{
    //反正0 array draw的时候还会传divisor，所以这里就不用保存了
    glVertexAttribDivisor(index, divisor);
}

//这个实际调用不到
// void d_glGetVertexAttribPointerv_with_bound(void *context, GLuint index, GLenum pname, GLintptr *pointer)
// {
//     aaaaa
// }

void d_glGetVertexAttribIiv_origin(void *context, GLuint index, GLenum pname, GLint *params)
{
    glGetVertexAttribIiv(index, pname, params);
}

void d_glGetVertexAttribIuiv_origin(void *context, GLuint index, GLenum pname, GLuint *params)
{
    glGetVertexAttribIuiv(index, pname, params);
}
void d_glGetVertexAttribfv_origin(void *context, GLuint index, GLenum pname, GLfloat *params)
{
    glGetVertexAttribfv(index, pname, params);
}
void d_glGetVertexAttribiv_origin(void *context, GLuint index, GLenum pname, GLint *params)
{
    glGetVertexAttribiv(index, pname, params);
}

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
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    set_attrib_point(context,1);
    glDrawArrays(mode, first, count);
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }
}



void d_glDrawArraysInstanced_origin(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    
    set_attrib_point(context,instancecount);
    
    glDrawArraysInstanced(mode, first, count, instancecount);
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }
}



void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,1);


    glDrawElements(mode, count,type,(void *)indices);
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    set_attrib_point(context,1);

    //然后处理indices为数组的情况
    int len=count*gl_sizeof(type);
    Guest_Mem *guest_mem=(Guest_Mem *)indices;
    Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glDrawElements(mode,count,type,NULL);
        return;
    }



    if(len==s_data[0].len){
        glDrawElements(mode,count,type,s_data[0].data);
    }else{
        set_attrib_point_index(context,s_data,len);
        glDrawElements(mode,count,type,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }


}

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    set_attrib_point(context,instancecount);
    
    //然后处理indices为数组的情况
    int len=count*gl_sizeof(type);

    Guest_Mem *guest_mem=(Guest_Mem *)indices;
    Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glDrawElementsInstanced(mode,count,type,NULL,instancecount);
        return;
    }

    if(len==s_data[0].len){
        glDrawElementsInstanced(mode,count,type,s_data[0].data,instancecount);
    }else{
        set_attrib_point_index(context,s_data,len);
        glDrawElementsInstanced(mode,count,type,0,instancecount);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

}

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,instancecount);


    glDrawElementsInstanced(mode, count,type,(void *)indices,instancecount);
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{
    //这里的start和end不会对此时的操作有影响，因为只要在那个范围内了，该传输过去还是得传输过去，只是最后会不会用的问题

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,1);


    glDrawRangeElements(mode, start, end, count, type, (void *)indices);
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

}

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    set_attrib_point(context,1);

    //然后处理indices为数组的情况
    int len=count*gl_sizeof(type);
    Guest_Mem *guest_mem=(Guest_Mem *)indices;
    Scatter_Data *s_data=guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        //pixels=NULL
        glDrawRangeElements(mode, start, end, count, type, NULL);
        return;
    }



    if(len==s_data[0].len){
        glDrawRangeElements(mode, start, end, count, type, s_data[0].data);
    }else{
        set_attrib_point_index(context,s_data,len);
        glDrawRangeElements(mode, start, end, count, type, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

}