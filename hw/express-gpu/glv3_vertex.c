#define STD_DEBUG_LOG

#include "express-gpu/glv3_vertex.h"



// /**
//  * @brief 利用之前发送来的顶点数据，将这些顶点数据复制到GPU显存中，假如当前是即时顶点模式，则是draw之前的必备操作
//  * 
//  * @param context 实际是Opengl_Context
//  * @param data guest端的内存数据，类型为Scatter_Data
//  * @param len guest端的内存数据的长度
//  */
// void set_attrib_point_index(void *context,Scatter_Data *data,int len){
    
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
    
//     host_guest_buffer_exchange(data,map_pointer,0,len,1);
//     glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

// }


void safe_release_point_data_data(Attrib_Point *point_data,int index){
    //这里这个判断是为了防止有多个顶点共用一个数组，但是程序后续把其中一个顶点设置到了buffer上，另一个顶点仍然在数组里
    //因此要在这里循环找到共用的顶点，把数据交给它
    GLint replace_father=index;
    if(point_data->in_buffer[index]==GL_FALSE && point_data->enabled[index]==GL_TRUE 
        && point_data->data[index]!=NULL && point_data->father[index]==index){
        for(int i=0;i<32;i++){
            if(point_data->enabled[i]==GL_FALSE || point_data->father[i]!=index || i==index
            || point_data->in_buffer[index]==GL_TRUE){
                continue;
            }
            if(replace_father==index){
                replace_father = i;
                point_data->data[i]=point_data->data[index];
                point_data->data_len[i]=point_data->data_len[index];
                point_data->father[i]=i;
            }else{
                point_data->father[i]=replace_father;
            }

        }
    }

    //如果没有这种共用顶点的话，就要释放这个data数据
    if(replace_father==index && point_data->data[index]!=NULL){
        g_free(point_data->data[index]);
    }
    point_data->data[index]=NULL;
    point_data->data_len[index]=0;
    return;

}


/**
 * @brief 利用之前发送来的顶点数据，将这些顶点数据复制到GPU显存中，假如当前是即时顶点模式，则是draw之前的必备操作
 * 
 * @param context 实际是Opengl_Context
 * @param data host端的内存
 * @param len 内存数据的长度
 */
void set_attrib_point_index(void *context,void *data,int len){
    
    express_printf("set_attrib_point_index %d\n",len);

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
    
    // host_guest_buffer_exchange(data,map_pointer,0,len,1);
    memcpy(data,map_pointer,len);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

}





void set_attrib_point(void *context,GLsizei instancecount){
    express_printf("set_attrib_point %d\n",instancecount);

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    // if (status->array_buffer == 0)
    // {
        //绑定为0的情况，这个时候需要把数据复制到临时的array_buffer中
        Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
        if (point_data == NULL)
        {
            return;
        }
        flush_array_buffer(point_data,instancecount);
    // }
}


void flush_array_buffer(Attrib_Point *point_data, GLint instancecount)
{
    express_printf("flush_array_buffer %d\n",glGetError());
    //找到当前应该申请的显存空间的最大值
    int max_len = 0;
    int father_enable[32];
    int father_max_loc[32];
    int father_min_loc[32];
    memset(father_enable, 0, sizeof(father_enable));
    memset(father_max_loc, 0, sizeof(father_max_loc));
    memset(father_min_loc, 0x3f3f3f3f, sizeof(father_min_loc));


    //这个father_loc用来描述每个真正的数组，对应到缓冲区的哪个地方
    int father_loc[32];

    memset(father_loc, 0, sizeof(father_loc));

    for (int i = 0; i < 32; i++)
    {
        if (point_data->enabled[i] == 0 || point_data->in_buffer[i] ==GL_TRUE)
        {
            continue;
        }
        father_enable[point_data->father[i]] += point_data->enabled[i];
        
        //通过这个循环找到实际那几个数组内，需要复制到缓冲区的偏移的两端

        //这里的stride肯定不为0，所以就算是紧密堆积也不存在问题
        int max_loc = point_data->stride[i] * (point_data->max_index + 1);
        int min_loc = point_data->stride[i] * point_data->min_index;
        //这里divisor两个属性都要看，选择其中最大的那个
        int divisor = point_data->divisor[i];
        if (divisor)
        {
            //这里加divisor-1是为了向上取整
            max_loc = max(max_loc, point_data->stride[i] * (int)((instancecount + divisor - 1) / divisor));
        }

        divisor = point_data->divisor[point_data->father[i]];
        if (divisor)
        {
            //这里加divisor-1是为了向上取整
            max_loc = max(max_loc, point_data->stride[point_data->father[i]] * (int)((instancecount + divisor - 1) / divisor));
        }

        father_max_loc[point_data->father[i]] = max(max_loc, father_max_loc[point_data->father[i]]);
        father_min_loc[point_data->father[i]] = min(min_loc, father_min_loc[point_data->father[i]]);


    }




    for (int i = 0; i < 32; i++)
    {
        if (point_data->data[i] != NULL && point_data->in_buffer[i] ==GL_FALSE && father_enable[i]!=0)
        {
            father_loc[i] = max_len;
            max_len += point_data->data_len[i];
        }
    }

    //max_len为0说明没有顶点数据是通过数组传送进来的
    if(max_len==0){
        return;
    }



    if (max_len > point_data->buffer_len)
    {
        //当前的缓冲区大小不足，直接将原来的缓冲区加到当前最大大小的两倍，类似于vector的翻倍机制
        // glDeleteBuffers(1, &(point_data->buffer_object));
        // glGenBuffers(1, &(point_data->buffer_object));
        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object);

        //todo stream_draw需要验证
        glBufferData(GL_ARRAY_BUFFER, max_len * 2, NULL, GL_STREAM_DRAW);
        point_data->buffer_len=max_len*2;
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, point_data->buffer_object);
        glBufferData(GL_ARRAY_BUFFER, point_data->buffer_len, NULL, GL_STREAM_DRAW);

    }
    //确定了缓冲区大小后，映射取得其指针，尽可能只修改一小部分
    //GL_MAP_INVALIDATE_RANGE_BIT 用于缓冲区孤立，防止隐式同步
    GLubyte *map_pointer = glMapBufferRange(GL_ARRAY_BUFFER, 0, max_len,
            GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT|GL_MAP_INVALIDATE_RANGE_BIT);






    float *temp;
    //然后把数组根据偏移复制进去
    for (int i = 0; i < 32; i++)
    {
         if (father_enable[i] == 0 || point_data->in_buffer[i] ==GL_TRUE)
        {
            continue;
        }

        if (point_data->data[i] != NULL)
        {
            //复制完后就通知一下更新一下这小部分
            int length=father_max_loc[i] - father_min_loc[i];
            temp=g_malloc(length);
            express_printf("father loc %d %d\n",father_min_loc[i], father_max_loc[i]);

            host_guest_buffer_exchange(point_data->data[i], temp, father_min_loc[i], father_max_loc[i] - father_min_loc[i], 1);
            express_printf("%d %d len data:",i,length);
            for(int i=0;i<length/4;i++){
                express_printf("%f ",temp[i]);
            }
            express_printf("\n");
            g_free(temp);
            
            host_guest_buffer_exchange(point_data->data[i], map_pointer + father_loc[i], father_min_loc[i], father_max_loc[i] - father_min_loc[i], 1);
            glFlushMappedBufferRange(GL_ARRAY_BUFFER, father_loc[i] + father_min_loc[i], father_max_loc[i] - father_min_loc[i]);
        }else{
            //@todo 这里要报错，因为不应该出现这种情况
        }
    }

    //数据写进去后，只有umap后，buffer才可以使用
    glUnmapBuffer(GL_ARRAY_BUFFER);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    express_printf("glBindBuffer %x\n",glGetError());


    //此时数据都更新到位了，该设置顶点属性了
    for (int i = 0; i < 32; i++)
    {
        if (point_data->enabled[i] ==GL_TRUE && point_data->in_buffer[i] == GL_FALSE)
        {
            if (point_data->invoke_type[i] == GL_INT)
            {
                glVertexAttribIPointer(i, point_data->size[i], point_data->type[i], point_data->stride[i],
                                       (void *)(point_data->offset[i] + father_loc[point_data->father[i]]));
            }
            else
            {
                glVertexAttribPointer(i, point_data->size[i], point_data->type[i], point_data->normalized[i],
                                      point_data->stride[i], (void *)(point_data->offset[i] + father_loc[point_data->father[i]]));
                // glVertexAttribPointer(i, point_data->size[i], point_data->type[i], point_data->normalized[i],
                //                       point_data->stride[i], (void *)temp);
                express_printf("glVertexAttribPointer %x | %d %d %u %u %d\n",glGetError(),i, point_data->size[i], point_data->type[i], point_data->normalized[i],
                                      point_data->stride[i]);

            }
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //全部操作完成后，这时候数据写入到缓冲区中，然后顶点属性的偏移也设置了，这个时候就可以draw了
    return;
}



void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{



    //假如with_bound之前有调用过一次without_bound的话，需要清空之前保存下来的顶点信息
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
    if (point_data != NULL)
    {

        point_data->father[index] =  index;
        point_data->offset[index] = pointer;
        point_data->size[index] = size;
        point_data->type[index] = type;
        point_data->stride[index] = stride;
        // point_data->normalized[index]=normalized;
        point_data->invoke_type[index] = GL_INT;   
        point_data->enabled[index]=GL_TRUE;
        
        //上面几个设置都不重要因为不会用到，只有这个会用到，判断这个顶点维度是否在buffer里
        point_data->in_buffer[index]=GL_TRUE;

        safe_release_point_data_data(point_data,index);

        point_data->data[index]=NULL;
        point_data->data_len[index]=NULL;



    }


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
    if(status->vertex_array_buffer==0 && point_data->buffer_object==0){
        glGenBuffers(1,&(point_data->indices_buffer_object));
        glGenBuffers(1,&(point_data->buffer_object));
    }

    Guest_Mem *guest_mem=(Guest_Mem *)pointer;

    Scatter_Data *s_data=guest_mem->scatter_data;

    //注意，这个地方需要将guest端内存长时保存，所以这里额外分配一个空间用于存储这个scatter_data
    if(point_data->data[index]!=NULL){
        safe_release_point_data_data(point_data,index);

        point_data->data[index]=NULL;
        point_data->data_len[index]=NULL;

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
    point_data->enabled[index]=GL_TRUE;
    point_data->in_buffer[index]=GL_FALSE;

    if(index_father!=index && point_data->data[index]!=NULL){
        safe_release_point_data_data(point_data,index);

        point_data->data[index]=NULL;
        point_data->data_len[index]=0;

    }

}

void d_glVertexAttribPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr pointer)
{

    //假如with_bound之前有调用过一次without_bound的话，需要清空之前保存下来的顶点信息
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(status->vertex_array_buffer));
    if (point_data != NULL)
    {


        point_data->father[index] =  index;
        point_data->offset[index] = pointer;
        point_data->size[index] = size;
        point_data->type[index] = type;
        point_data->stride[index] = stride;
        // point_data->normalized[index]=normalized;
        point_data->invoke_type[index] = GL_FLOAT;   
        point_data->enabled[index]=GL_TRUE;
        
        //上面几个设置都不重要因为不会用到，只有这个会用到，判断这个顶点维度是否在buffer里
        point_data->in_buffer[index]=GL_TRUE;

        safe_release_point_data_data(point_data,index);
        
        
        point_data->data[index]=NULL;
        point_data->data_len[index]=0;


    }

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

    express_printf(YELLOW("d_glVertexAttribPointer_data index %u length %u\n"), index, length);
    

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
    if(status->vertex_array_buffer==0 && point_data->buffer_object==0){
        glGenBuffers(1,&(point_data->indices_buffer_object));
        glGenBuffers(1,&(point_data->buffer_object));
    }

    
    Guest_Mem *guest_mem=(Guest_Mem *)pointer;

    Scatter_Data *s_data=guest_mem->scatter_data;


    // #ifdef STD_DEBUG_LOG
    // float *temp=g_malloc(length*4);
    // guest_write(guest_mem,temp,0,length);
    // express_printf(YELLOW("point_data "));

    // for(int i=0;i<length;i++)
    //     express_printf(YELLOW("%08x"),temp[i]);
    // express_printf("\n");
    // #endif

    if(point_data->data[index]!=NULL){
        safe_release_point_data_data(point_data,index);
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
    point_data->enabled[index]=GL_TRUE;
    
    if(index_father!=index && point_data->data[index]!=NULL){
        safe_release_point_data_data(point_data,index);

        point_data->data[index]=NULL;
        point_data->data_len[index]=NULL;

    }


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
        // Attrib_Point *vao_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(vao_index));

        if(vao_status==bound_buffer->buffer_status){
            bound_buffer->buffer_status=g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(0));
        }

        // GLuint buffer_index[2];
        // buffer_index[0]=vao_point->indices_buffer_object;
        // buffer_index[1]=vao_point->buffer_object;

        // glDeleteBuffers(2,buffer_index);
        // g_free(vao_point);
        // g_free(vao_status);
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
    //反正array draw的时候还会传divisor，所以这里就不用保存了
    express_printf("glVertexAttribDivisor %u %u\n",index,divisor);
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


    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
        // express_printf("glBindBuffer %x\n",glGetError());
        
    }

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
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    
    set_attrib_point(context,instancecount);
    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }

    glDrawArraysInstanced(mode, first, count, instancecount);
    
}



void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,1);
    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }

    glDrawElements(mode, count,type,(void *)indices);
    
}

void d_glDrawElements_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    set_attrib_point(context,1);
    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }
    //然后处理indices为数组的情况
    int len=count*gl_sizeof(type);
    // Guest_Mem *guest_mem=(Guest_Mem *)indices;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    // if (guest_mem->all_len == 0)
    // {
    //     //pixels=NULL
    //     glDrawElements(mode,count,type,NULL);
    //     return;
    // }



    // if(len==s_data[0].len){
    //     glDrawElements(mode,count,type,s_data[0].data);
    // }else{
    //     set_attrib_point_index(context,s_data,len);
    //     glDrawElements(mode,count,type,0);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // }
    express_printf("indices:");
    for(int i=0;i<len/sizeof(unsigned short);i++){
        express_printf("%hu ",((unsigned short *)indices)[i]);
    }
    express_printf("\n");


    // @todo 这里根据数据的长度来决定到底是采用普通模式还是缓冲区模式，需要测试哪个长度更节约时间
    if(len<40000){
        glDrawElements(mode,count,type,indices);
    }else{
        set_attrib_point_index(context,indices,len);

        glDrawElements(mode,count,type,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    



}

void d_glDrawElementsInstanced_without_bound(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    set_attrib_point(context,instancecount);
    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }
    //然后处理indices为数组的情况
    int len=count*gl_sizeof(type);

    // Guest_Mem *guest_mem=(Guest_Mem *)indices;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    // if (guest_mem->all_len == 0)
    // {
    //     //pixels=NULL
    //     glDrawElementsInstanced(mode,count,type,NULL,instancecount);
    //     return;
    // }

    // if(len==s_data[0].len){
    //     glDrawElementsInstanced(mode,count,type,s_data[0].data,instancecount);
    // }else{
    //     set_attrib_point_index(context,s_data,len);
    //     glDrawElementsInstanced(mode,count,type,0,instancecount);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // }
    
    // @todo 这里根据数据的长度来决定到底是采用普通模式还是缓冲区模式，需要测试哪个长度更节约时间
    if(len<40000){
        glDrawElementsInstanced(mode,count,type,indices,instancecount);
    }else{
        set_attrib_point_index(context,indices,len);
        glDrawElementsInstanced(mode,count,type,0,instancecount);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }

}

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,instancecount);
    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }

    glDrawElementsInstanced(mode, count,type,(void *)indices,instancecount);
 
}

void d_glDrawRangeElements_with_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr indices)
{
    //这里的start和end不会对此时的操作有影响，因为只要在那个范围内了，该传输过去还是得传输过去，只是最后会不会用的问题

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,1);
    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }

    glDrawRangeElements(mode, start, end, count, type, (void *)indices);


}

void d_glDrawRangeElements_without_bound(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    set_attrib_point(context,1);
    if (status->array_buffer != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,status->array_buffer);
    }
    //然后处理indices为数组的情况
    int len=count*gl_sizeof(type);
    // Guest_Mem *guest_mem=(Guest_Mem *)indices;
    // Scatter_Data *s_data=guest_mem->scatter_data;

    // if (guest_mem->all_len == 0)
    // {
    //     //pixels=NULL
    //     glDrawRangeElements(mode, start, end, count, type, NULL);
    //     return;
    // }



    // if(len==s_data[0].len){
    //     glDrawRangeElements(mode, start, end, count, type, s_data[0].data);
    // }else{
    //     set_attrib_point_index(context,s_data,len);
    //     glDrawRangeElements(mode, start, end, count, type, 0);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    // }

    // @todo 这里根据数据的长度来决定到底是采用普通模式还是缓冲区模式，需要测试哪个长度更节约时间
    if(len<40000){
        glDrawRangeElements(mode, start, end, count, type, indices);
    }else{
        set_attrib_point_index(context,indices,len);
        
        glDrawRangeElements(mode, start, end, count, type, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    


}