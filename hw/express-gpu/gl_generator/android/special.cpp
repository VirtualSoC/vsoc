#include "special.h"
#include "all_gl.h"


/**
 * @brief 获取当前绑定的buffer
 * 
 * @param context 当前opengl自定义的上下文
 * @param target buffer类型
 * @return GLint buffer编号，编号为0表示没有绑定
 */
GLint get_bound_buffer(void *context, GLenum target)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    switch (target)
    {
    case GL_ARRAY_BUFFER:
        return status->array_buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        return status->element_array_buffer;
        break;
    case GL_COPY_READ_BUFFER:
        return status->copy_read_buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        return status->copy_write_buffer;
        break;
    case GL_PIXEL_PACK_BUFFER:
        return status->pixel_pack_buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        return status->pixel_unpack_buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return status->transform_feedback_buffer;
        break;
    case GL_UNIFORM_BUFFER:
        return status->uniform_buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        return status->atomic_counter_buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return status->dispatch_indirect_buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        return status->draw_indirect_buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        return status->shader_storage_buffer;
        break;
    }
}

// void set_point_attrib_data(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, const void *data)
// {
//     //只有当当前状态下没有绑定vbo时，才需要复制保存下这个数据
//     if (get_bound_buffer(context, GL_ARRAY_BUFFER) != 0)
//     {
//         return;
//     }
//     Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);

//     Attrib_Point *point_data = bound_buffer->vao_point_data[bound_buffer->vertex_array_buffer];
//     if (point_data == NULL)
//     {
//         point_data = new Attrib_Point;
//         bound_buffer->vao_point_data[bound_buffer->vertex_array_buffer] = point_data;
//         memset(point_data, 0, sizeof(Attrib_Point));
//     }
//     point_data->location[index] = 1;
//     point_data->size[index] = size;
//     point_data->type[index] = type;
//     point_data->stride[index] = stride;
//     point_data->data[index] = data;
// }

/**
 * @brief 获得当前顶点索引数组中的最大下标和最小下标，方便更新数据
 * 
 * @param context 
 * @param data 
 * @param min_index 
 * @param max_index 
 */
void get_max_min_point_index_direct(void *context, const void *data, GLsizei count, GLenum type, GLuint *min_index, GLuint *max_index)
{
    //这里因为indices是客户端的内存空间的数组，我们不知道这个数组是不是变化了，所以必须重新遍历一遍来寻找最大最小值
    GLuint min_temp = 0xffffffff;
    GLuint max_temp = 0;
    for (int i = 0; i < count; i++)
    {
        GLuint t;
        switch (type)
        {
        case GL_UNSIGNED_BYTE:
            t = (GLuint) * ((GLubyte *)data + i);
            break;
        case GL_UNSIGNED_SHORT:
            t = (GLuint) * ((GLushort *)data + i);
            break;
        case GL_UNSIGNED_INT:
            t = (GLuint) * ((GLuint *)data + i);
            break;
        }
        if (t < min_temp)
        {
            min_temp = t;
        }
        if (t > max_temp)
        {
            max_temp = t;
        }
    }
    *min_index = min_temp;
    *max_index = max_temp;
    return;
}

/**
 * @brief 在有indices时，假如没有绑定vbo则发送顶点属性数据。这个发送是先找到索引的最大值最小值（从索引数组或者之前缓存下来的数据），然后发送相关的数据，host根据最大最小的偏移寻找相应的数据
 * 
 * @param context 
 * @param count 
 * @param type 
 * @param indices 
 * @param instancecount 
 */
void send_point_attrib_data_indices(void *context, GLsizei count, GLenum type, const void *indices, GLuint instancecount)
{
    if (get_bound_buffer(context, GL_ARRAY_BUFFER) == 0)
    {
        //只有没有绑定vbo的时候才会有这些传输数据的操作
        GLuint min_index;
        GLuint max_index;
        //首先要看有没有绑定ebo，根据ebo或者indices确定范围
        if (get_bound_buffer(context, GL_ELEMENT_ARRAY_BUFFER) == 0)
        {
            //没有绑定ebo
            //没有绑定ebo时每次都是传入一个数组

            get_max_min_point_index_direct(context, indices, count, type, &min_index, &max_index);
        }
        else
        {
            //有绑定ebo

            //根据ebo中的数据获取到最大最小值，这里使用rmq st算法，当有修改的时候，直接重新生成一次就行了
            get_max_min_point_index_rmq(context, (uint64_t)indices, count, type, &min_index, &max_index);
        }
        send_point_attrib_data(context, min_index, max_index, instancecount);
    }
}

void get_max_min_point_index_rmq(void *context, uint64_t offset, GLsizei count, GLenum type, GLuint *min_index, GLuint *max_index)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Element_Array_Buffer *ebo_data = bound_buffer->ebo_buffer[bound_buffer->buffer_status->element_array_buffer];
    if (ebo_data->has_update)
    {
        //假如有更新的话，就要重新生成st表，否则就直接读取
        GLuint real_size = ebo_data->size / gl_sizeof(type);
        //这里是需要更新公共的那个缓存幂次的数组
        if (real_size > bound_buffer->ebo_mm_num || bound_buffer->ebo_mm == NULL)
        {
            GLuint t_size = real_size;
            //数组加大，使用加倍的方式使得不至于每次增加都需要加大，else里的是防止加的过大了
            if (t_size < bound_buffer->ebo_mm_num * 2)
            {
                t_size = bound_buffer->ebo_mm_num * 2;
            }
            else
            {
                t_size += bound_buffer->ebo_mm_num;
            }
            GLuint *temp = new GLuint[t_size + 1];
            for (int i = 1; i < t_size + 1; i++)
            {
                if (i < bound_buffer->ebo_mm_num + 1)
                {
                    temp[i] = bound_buffer->ebo_mm[i];
                }
                else
                {
                    temp[i] = ((i & (i - 1)) == 0) ? temp[i - 1] + 1 : temp[i - 1];
                }
            }
            bound_buffer->ebo_mm_num = t_size;
            delete[] bound_buffer->ebo_mm;
            bound_buffer->ebo_mm = temp;
        }
        int max_log_n = bound_buffer->ebo_mm[real_size];
        if (ebo_data->dp_num < real_size)
        {
            //假如之前的数组本身就比较大，那就不用重新申请内存了，直接在当前的上面处理
            //否则就是需要重新申请一片内存
            if (ebo_data->dp_max != NULL)
            {
                for (int i = 1; i <= ebo_data->dp_num; i++)
                {
                    delete[] ebo_data->dp_max[i];
                    delete[] ebo_data->dp_min[i];
                }
                delete[] ebo_data->dp_max;
                delete[] ebo_data->dp_min;
            }

            //为了充分利用cpu的cache，这里的dp数组进行了调整，使得数组的行更长
            ebo_data->dp_max = new GLuint *[max_log_n + 1];
            ebo_data->dp_min = new GLuint *[max_log_n + 1];

            for (int i = 0; i <= max_log_n; i++)
            {
                ebo_data->dp_max[i] = new GLuint[real_size + 5];
                ebo_data->dp_min[i] = new GLuint[real_size + 5];
            }
        }
        for (int i = 1; i <= real_size; i++)
        {
            GLuint t;
            switch (ebo_data->type)
            {
            case GL_UNSIGNED_BYTE:
                t = (GLuint) * ((GLubyte *)ebo_data->data + i);
                break;
            case GL_UNSIGNED_SHORT:
                t = (GLuint) * ((GLushort *)ebo_data->data + i);
                break;
            case GL_UNSIGNED_INT:
                t = (GLuint) * ((GLuint *)ebo_data->data + i);
                break;
            }
            ebo_data->dp_max[0][i] = t;
            ebo_data->dp_min[0][i] = t;
        }
        for (int j = 1; j < max_log_n; j++)
        {
            for (int i = 1; i + (i << j) - 1 <= real_size; i++)
            {
                ebo_data->dp_max[j][i] = std::max(ebo_data->dp_max[j - 1][i], ebo_data->dp_max[j - 1][i + (1 << (j - 1))]);
                ebo_data->dp_min[j][i] = std::min(ebo_data->dp_max[j - 1][i], ebo_data->dp_max[j - 1][i + (1 << (j - 1))]);
            }
        }
    }

    //st算法能够实现o(1)查询最大最小值
    int k = bound_buffer->ebo_mm[count + 1];
    *max_index = std::max(ebo_data->dp_max[k][offset + 1], ebo_data->dp_max[k][(offset + count + 1) - (1 << k) + 1]);
    *min_index = std::min(ebo_data->dp_min[k][offset + 1], ebo_data->dp_min[k][(offset + count + 1) - (1 << k) + 1]);
    return;
}

void send_point_attrib_data(void *context, GLuint min_index, GLuint max_index, GLuint instancecount)
{
    //由于实际的内存数据可能会发生变化，因此每次draw都需要发送一次这个顶点属性数据，这里只更新部分

    //由于会有结构数组和数组结构的区别，因此这里进行了聚类，寻找结构数组，减少复制

    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);

    Attrib_Point *point_data = bound_buffer->vao_point_data[bound_buffer->buffer_status->vertex_array_buffer];

    if (point_data->has_update)
    {
        //只有更新过顶点属性数组指针，才需要重新进行一次聚类
        point_data->has_update = 0;
        for (int i = 0; i < 32; i++)
        {
            if (point_data->location[i] == 0)
            {
                continue;
            }
            GLint real_index = point_data->data_real_index[i];
            GLint temp_index = real_index;
            //找到范围内最小的那个指针对应的index
            while (temp_index != point_data->data_real_index[temp_index])
            {
                temp_index = point_data->data_real_index[temp_index];
            }
            point_data->data_real_index[i] = temp_index;
            temp_index = real_index;
            real_index = point_data->data_real_index[i];
            //更新这个路径上的其他数组real_index
            while (temp_index != point_data->data_real_index[temp_index])
            {
                temp_index = point_data->data_real_index[temp_index];
                point_data->data_real_index[temp_index] = real_index;
            }
        }
    }

    //然后发送固定数组（因为数组可能显得更大了，可能需要重新映射一遍）

    int send_num = 0;
    int send_index[32];
    memset(send_index, 0, sizeof(send_index));
    for (int i = 0; i < 32; i++)
    {
        if (point_data->location[i] == 0)
        {
            continue;
        }
        GLuint index_father = point_data->data_real_index[i];

        //这里的逻辑是，先把数据发送和映射过去，然后再继续发送使用的偏移位置
        if (send_index[index_father] == 0)
        {
            //假如这个数组没有发送过的话就发送
            send_index[index_father] = 1;
            int real_stride = point_data->stride[index_father];
            if (real_stride == 0)
            {
                //stride为0时，说明是紧密存放，实际的stride就是一个顶点属性的大小
                real_stride = point_data->size[index_father] * gl_sizeof(point_data->type[index_father]);
                if (point_data->type[index_father] == GL_INT_2_10_10_10_REV || point_data->type[index_father] == GL_UNSIGNED_INT_2_10_10_10_REV)
                {
                    real_stride /= 4;
                }
            }

            int max_len = real_stride * (max_index + 1);
            //这里divisor两个属性都要看，选择其中最大的那个
            int divisor = point_data->divisor[index_father];
            if (divisor)
            {
                //这里加divisor-1是为了向上取整
                max_len = std::max(max_len, real_stride * (int)((instancecount + divisor - 1) / divisor));
            }

            divisor = point_data->divisor[i];
            if (divisor)
            {
                //这里加divisor-1是为了向上取整
                max_len = std::max(max_len, real_stride * (int)((instancecount + divisor - 1) / divisor));
            }
            if (max_len > point_data->max_len[index_father])
            {
                //只有当这个数据没有被映射过（max_len[index_father]为0），或者之前映射过，但是目前的范围更大（那个大于号）时，才需要重新映射

                //假如之前有映射过，那就要put_page
                if (point_data->get_page[index_father])
                {
                    //todo data_index_length可以只用映射一次就可以一直保留着
                    put_page_host(context,point_data->data_index_length[index_father],8);
                    put_page_host(context, point_data->data[index_father], point_data->max_len[index_father]);
                }

                //注意：这里是映射过去全部数据，后续再根据需要使用其中的少部分数据，
                
                memcpy(point_data->data_index_length[index_father],&index_father,sizeof(GLuint));
                memcpy(point_data->data_index_length[index_father]+sizeof(GLuint),&max_len,sizeof(GLuint));
                
                if (point_data->invoke_type[index_father] == GL_FLOAT)
                {
                    d_glVertexAttribPointer_data(context, max_len,point_data->data_index_length[index_father], point_data->data[index_father]);
                }
                else
                {
                    d_glVertexAttribIPointer_data(context, max_len, point_data->data_index_length[index_father], point_data->data[index_father]);
                }
                point_data->get_page[index_father] = 1;
            }
        }

        //发送偏移即真实使用到的范围，这两个差异只是有没有normalized
        if (point_data->invoke_type[index_father] == GL_INT)
        {
            //顶点属性都没启用,但是还是要传输，因为这个顶点属性可能某个结构数组中的一部分
            //if(point_data->enabled[i])
            d_glVertexAttribPointer_offset(context, i, point_data->size[i], point_data->type[i], point_data->normalized[i],
                                           point_data->stride[i], min_index, max_index, index_father, point_data->divisor[i], point_data->enabled[i], (uint64_t)point_data->data[i] - (uint64_t)point_data->data[index_father]);
        }
        else
        {
            // if(point_data->enabled[i])
            d_glVertexAttribIPointer_offset(context, i, point_data->size[i], point_data->type[i],
                                            point_data->stride[i], min_index, max_index, index_father, point_data->divisor[i], point_data->enabled[i], (uint64_t)point_data->data[i] - (uint64_t)point_data->data[index_father]);
        }
    }
}



uint64_t gl_get_program_uniform_size(void *context,GLuint program,GLint location){
    if(((Direct_GPU_Context *)context)->all_program_info.find(program)!=((Direct_GPU_Context *)context)->all_program_info.end()){

        Program_Info *info=((Direct_GPU_Context *)context)->all_program_info[program];
        if(info->uniform_loction_type.find(location)!=info->uniform_loction_type.end()){
            GLenum type=info->uniform_loction_type[location];
            return gl_sizeof(type);
        }
    }
    //默认返回4，因为调用这个函数是用来确定的uniform大小，方便确定指针指向空间长度的，但是这个指针就是int或者float类型的
    return 4;
    
}




void d_glGenVertexArrays(void *context, GLsizei n, GLuint *arrays)
{
    d_glGenVertexArrays_origin(context, n, arrays);
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *t_buffer;
    t_buffer = new Buffer_Status;
    memset(t_buffer, 0, sizeof(Buffer_Status));
    for (int i = 0; i < n; i++)
    {
        t_buffer->vertex_array_buffer = arrays[i];

        bound_buffer->vao_status[arrays[i]] = t_buffer;

        Attrib_Point *point_data = new Attrib_Point;
        memset(point_data, 0, sizeof(Attrib_Point));
        bound_buffer->vao_point_data[arrays[i]] = point_data;
    }
}

void d_glBindVertexArray(void *context, GLuint array)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);

    //这里直接是指针赋值，调整了下指针的指向
    bound_buffer->buffer_status = bound_buffer->vao_status[array];

    d_glBindVertexArray_origin(context, array);
}

/**
 * @brief 删除vao
 * 
 * @param context 
 * @param id 
 */
void d_glDeleteVertexArrays(void *context, GLsizei n, const GLuint *arrays)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    for (int i = 0; i < n; i++)
    {
        if (arrays[i] == 0)
        {
            //todo 这里出错，不允许这样的操作
            continue;
        }
        if (bound_buffer->buffer_status->vertex_array_buffer == arrays[i])
        {
            //正好删除的vao就是当前绑定的vao，就直接切换到默认的
            bound_buffer->buffer_status = bound_buffer->vao_status[0];
        }

        if (bound_buffer->vao_status[arrays[i]] != NULL)
        {
            delete bound_buffer->vao_status[arrays[i]];
        }
        bound_buffer->vao_status.erase(arrays[i]);
        if (bound_buffer->vao_point_data[arrays[i]] != NULL)
        {
            //注意，这里的顶点属性数据指针是客户端设的，它自己会删除，我们这里不需要操作
            delete bound_buffer->vao_point_data[arrays[i]];
        }
        bound_buffer->vao_point_data.erase(arrays[i]);
    }

    d_glDeleteVertexArrays_origin(context, n, arrays);
}

void d_glGenVertexArraysOES(void *context, GLsizei n, GLuint *arrays)
{
    d_glGenVertexArrays(context, n, arrays);
}

void d_glBindVertexArrayOES(void *context, GLuint array)
{
    d_glBindVertexArray(context, array);
}

void d_glDeleteVertexArraysOES(void *context, GLsizei n, const GLuint *arrays)
{
    d_glDeleteVertexArrays(context, n, arrays);
}

/**
 * @brief 绑定特定buffer，需要放到绑定的opengl函数中一起调用
 * 
 * @param context 上下文
 * @param target 指定的buffer类型
 * @param id buffer id
 */
void d_glBindBuffer(void *context, GLenum target, GLint id)
{

    d_glBindBuffer_origin(context, target, id);
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    //由于这里status是指针，所以对这个status的更新也会直接反映到vao相应的status上去
    if(id!=0){
        bound_buffer->buffer_type[id]=target;
    }
    switch (target)
    {
    case GL_ARRAY_BUFFER:
        status->array_buffer = id;
        //bound_buffer->vao_vbo[bound_buffer->vertex_array_buffer] = id;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        status->element_array_buffer = id;
        //bound_buffer->vao_ebo[bound_buffer->vertex_array_buffer] = id;
        break;
    case GL_COPY_READ_BUFFER:
        status->copy_read_buffer = id;
        break;
    case GL_COPY_WRITE_BUFFER:
        status->copy_write_buffer = id;
        break;
    case GL_PIXEL_PACK_BUFFER:
        status->pixel_pack_buffer = id;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        status->pixel_unpack_buffer = id;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->transform_feedback_buffer = id;
        break;
    case GL_UNIFORM_BUFFER:
        status->uniform_buffer = id;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->atomic_counter_buffer = id;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        status->dispatch_indirect_buffer = id;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        status->draw_indirect_buffer = id;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->shader_storage_buffer = id;
    }
}

void d_glPixelStorei(void *context, GLenum pname, GLint param)
{
    d_glPixelStorei_origin(context, pname, param);
    Pixel_Store_Status *pixel_store = &(((Direct_GPU_Context *)context)->pixel_store_status);
    switch (pname)
    {
    case GL_UNPACK_ALIGNMENT:
        pixel_store->unpack_alignment = param;
        break;
    case GL_PACK_ALIGNMENT:
        pixel_store->pack_alignment = param;
        break;
    case GL_UNPACK_ROW_LENGTH:
        pixel_store->unpack_row_length = param;
        break;
    case GL_UNPACK_IMAGE_HEIGHT:
        pixel_store->unpack_image_height = param;
        break;
    case GL_UNPACK_SKIP_PIXELS:
        pixel_store->unpack_skip_pixels = param;
        break;
    case GL_UNPACK_SKIP_ROWS:
        pixel_store->unpack_skip_rows = param;
        break;
    case GL_UNPACK_SKIP_IMAGES:
        pixel_store->unpack_skip_images = param;
        break;
    case GL_PACK_ROW_LENGTH:
        pixel_store->pack_row_length = param;
        break;
    case GL_PACK_SKIP_PIXELS:
        pixel_store->pack_skip_pixels = param;
        break;
    case GL_PACK_SKIP_ROWS:
        pixel_store->pack_skip_rows = param;
        break;
    default:
        return;
    }
    return;
}

/**
 * @brief glGetActiveUniformBlockiv函数专用，用于确定params到底对应了多大的空间
 * 假如pname是GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES，则params对应是个数组，且数组大小是uniformBlockIndex对应的GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS，也就是这个uniform_block的uniform的个数
 * 
 * @param context 
 * @param program 
 * @param uniformBlockIndex 
 * @param pname 
 * @return int 
 */
int gl_get_uniform_block_para_size(void *context, GLuint program, GLuint uniformBlockIndex, GLenum pname)
{
    if (pname == GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES)
    {
        if (((Direct_GPU_Context *)context)->all_program_info.find(program) != ((Direct_GPU_Context *)context)->all_program_info.end())
        {
            Program_Info *target_program = ((Direct_GPU_Context *)context)->all_program_info[program];
            if (target_program->uniform_block_index_num.find(uniformBlockIndex) != target_program->uniform_block_index_num.end())
            {
                GLint uniform_num = target_program->uniform_block_index_num[uniformBlockIndex];
                return uniform_num;
            }
            else
            {
                GLint uniform_num;
                d_glGetActiveUniformBlockiv(context, program, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniform_num);
                target_program->uniform_block_index_num[uniformBlockIndex] = uniform_num;
                return uniform_num;
            }
        }
    }
    return gl_pname_size(pname);
}

/**
 * @brief 获取特定program中location位置的uniform变量占的空间的大小，假如没有这个location或者这个program没有link成功，都返回0
 * 
 * @param context 
 * @param program 
 * @param location 
 * @return size_t 
 */
size_t get_program_uniform_size(void *context, GLuint program, GLint location)
{
    if (((Direct_GPU_Context *)context)->all_program_info.find(program) != ((Direct_GPU_Context *)context)->all_program_info.end())
    {
        Program_Info *target_program = ((Direct_GPU_Context *)context)->all_program_info[program];
        if (target_program->uniform_loction_type.find(location) != target_program->uniform_loction_type.end())
        {
            GLenum type = target_program->uniform_loction_type[location];
            return gl_sizeof(type);
        }
    }
    return 0;
}

uint64_t gl_pixel_data_size(void *context, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack)
{
    return gl_pixel_data_3d_size(context, width, height, 1, format, type, pack);
}

uint64_t gl_pixel_data_3d_size(void *context, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack)
{
    Pixel_Store_Status *status = (Pixel_Store_Status *)context;
    int align = 4;
    int row_length = 0;
    int skip_rows = 0;
    int skip_pixels = 0;
    int skip_images = 0;
    int image_height = 0;

    GLsizei real_width;
    GLsizei real_height;

    //获得此时的状态值
    if (pack)
    {
        align = status->pack_alignment;
        row_length = status->pack_row_length;
        skip_rows = status->pack_skip_rows;
        skip_pixels = status->pack_skip_pixels;
    }
    else
    {
        align = status->unpack_alignment;
        row_length = status->unpack_row_length;
        skip_rows = status->unpack_skip_rows;
        skip_pixels = status->unpack_skip_pixels;

        skip_images = status->unpack_skip_images;
        image_height = status->unpack_image_height;
    }

    //获得一张图片的真实像素宽度和高度
    real_width = row_length == 0 ? width : row_length;
    real_height = image_height == 0 ? height : image_height;

    //每个像素点所占的空间
    GLsizei pixel_size = pixel_size_calc(format, type);
    //获得图片每一行的字节数
    GLsizei width_size = pixel_size * real_width;
    //每一行的数据进行对其
    width_size = (width_size + align - 1) & (~(align - 1));

    //数据图像开始读取的地方，等于（跳过的图片数目*图片的高度+跳过的行数）*每行所占的字节数+跳过的像素数*像素大小
    int start_loc = (skip_images * real_height + skip_rows) * width_size + skip_pixels * pixel_size;

    //数据图像结束读取的地方，等于 开始读取的地方+要读取的图片的高度*图片每行所占的字节数*图片的张数，depth表示深度也就表示要读取多少张图片
    int end_loc = start_loc + real_height * width_size * depth;

    return end_loc;
}

/**
 * @brief 将数据发送到host端，假如这边是异步的，异步需要传输的数据大小小于MAX_ASYN_BUF_SIZE，那就缓存下来，下次发送同步命令或者缓冲区满的时候，再flush直接发送过去
 * 假如是同步的，那就直接发送过去
 * 
 * @param context 
 * @param send_buf 
 * @param send_buf_len 
 */
void send_to_host(void *context, const unsigned char *send_buf, size_t send_buf_len)
{
    Direct_GPU_Context *real_context = (Direct_GPU_Context *)context;

    read(real_context->direct_express,(void *) send_buf, send_buf_len);



    // if (asyn == 0 || real_context->asyn_buf_loc + send_buf_len >= MAX_ASYN_BUF_SIZE)
    // {
    //     //缓冲区满就flush or 是同步发送就flush
    //     write(real_context->direct_express, real_context->asyn_buf, real_context->asyn_buf_loc);
    //     real_context->asyn_buf_loc = 0;
    //     //flush时delete相关数组，并且清空
    //     for (int i = 0; i < real_context->need_free_bufs.size(); i++)
    //     {
    //         char *t = real_context->need_free_bufs[i];
    //         if (t != NULL)
    //         {
    //             delete[] t;
    //         }
    //     }
    //     real_context->need_free_bufs.clear();
    // }

    // if (asyn == 1 && real_context->asyn_buf_loc + send_buf_len < MAX_ASYN_BUF_SIZE)
    // {
    //     //同时为异步且缓冲区大小够的话就保存到缓冲区里
    //     memcpy(real_context->asyn_buf + real_context->asyn_buf_loc, send_buf, send_buf_len);
    //     real_context->asyn_buf_loc += send_buf_len;
    //     if (save_buf != NULL)
    //     {
    //         real_context->need_free_bufs.push_back(save_buf);
    //     }
    // }
    // else
    // {
    //     //否则直接write
    //     write(real_context->direct_express, send_buf, send_buf_len);
    //     if (save_buf != NULL)
    //     {
    //         delete[] save_buf;
    //     }
    // }
}

/**
 * @brief 告知驱动需要将这部分page的页面引用释放，实际是通过调用read来告知驱动的
 * 
 * @param context 
 * @param buf 
 * @param buf_len 
 */
void put_page_host(void *context,const void *buf, size_t buf_len)
{
    Direct_GPU_Context *real_context = (Direct_GPU_Context *)context;
    //put_page通过直接read来实现
    read(real_context->direct_express, (void *)buf, buf_len);
}

/**
 * @brief 这一堆的image2D或者image3D的函数最后一个pixels，是根据是否绑定率特定buffer来决定是指针还是偏移
 * 
 * @param context 
 * @param target 
 * @param level 
 * @param internalformat 
 * @param width 
 * @param height 
 * @param border 
 * @param format 
 * @param type 
 * @param pixels 
 */
void d_glTexImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glTexImage2D_without_bound(context, target, level, internalformat, width, height, border, format, type, pixels);
    }
    else
    {
        d_glTexImage2D_with_bound(context, target, level, internalformat, width, height, border, format, type, (GLintptr)pixels);
    }
}

void d_glTexSubImage2D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glTexSubImage2D_without_bound(context, target, level, internalformat, width, height, border, format, type, pixels);
    }
    else
    {
        d_glTexSubImage2D_with_bound(context, target, level, internalformat, width, height, border, format, type, (GLintptr)pixels);
    }
}

void d_glTexImage3D(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glTexImage3D_without_bound(context, target, level, internalformat, width, height, depth, border, format, type, pixels);
    }
    else
    {
        d_glTexImage3D_with_bound(context, target, level, internalformat, width, height, depth, border, format, type, (GLintptr)pixels);
    }
}

void d_glTexSubImage3D(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glTexSubImage3D_without_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    }
    else
    {
        d_glTexSubImage3D_with_bound(context, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, (GLintptr)pixels);
    }
}


void d_glCompressedTexImage3D(void *context,GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data){

    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glCompressedTexImage3D_without_bound(context,target,level, internalformat, width, height, depth, border, imageSize, data);
    }
    else
    {
        d_glCompressedTexImage3D_with_bound(context,target,level, internalformat, width, height, depth, border, imageSize, (GLintptr)data);
    }

}
void d_glCompressedTexSubImage3D(void *context,GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data){
    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glCompressedTexSubImage3D_without_bound(context,target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    }
    else
    {
        d_glCompressedTexSubImage3D_with_bound(context,target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize,(GLintptr)data);
    }
}


void d_glCompressedTexImage2D(void *context,GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data){
    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glCompressedTexImage2D_without_bound(context,target, level, internalformat, width, height, border, imageSize, data);
    }
    else
    {
        d_glCompressedTexImage2D_with_bound(context,target, level, internalformat, width, height, border, imageSize, (GLintptr)data);
    }
}
void d_glCompressedTexSubImage2D(void *context,GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data){
    if (get_bound_buffer(context, GL_PIXEL_UNPACK_BUFFER) == 0)
    {
        d_glCompressedTexSubImage2D_without_bound(context,target, level, xoffset, yoffset, width, height, format, imageSize, data);
    }
    else
    {
        d_glCompressedTexSubImage2D_with_bound(context,target, level, xoffset, yoffset, width, height, format, imageSize, (GLintptr)data);
    }
}



void d_glReadPixels(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
{
    if (get_bound_buffer(context, GL_PIXEL_PACK_BUFFER) == 0)
    {
        d_glReadPixels_without_bound(context, x, y, width, height, format, type, pixels);
    }
    else
    {
        d_glReadPixels_with_bound(context, x, y, width, height, format, type, (GLintptr)pixels);
    }
}

void glReadnPixelsEXT(void *context,GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data)
{
    //todo set error if bufSize< pixeldatasize
    d_glReadPixels(context, x, y, width, height, format, type, data);
}


/**
 * @brief 链接着色器时需要将着色器涉及到的一些状态信息都保存下来，比如着色器里定义的常量名字、类型等，因此这个函数调用的时间可能会比较长一些，因为要往返调用多次
 * 
 * @param context 
 * @param program 着色器编号
 */
void d_glLinkProgram(void *context, GLuint program)
{

    //先链接
    d_glLinkProgram_origin(context, program);

    GLint link_status = 0;
    d_glGetProgramiv(context, program, GL_LINK_STATUS, &link_status);

    //链接不上的没有后面那些事情
    if (!link_status)
    {
        return;
    }

    Program_Info *new_program = new Program_Info;

    //当前着色器定义的uniform常量和attrib变量的数目
    GLint uniform_num = 0;
    GLint attrib_num = 0;
    d_glGetProgramiv(context, program, GL_ACTIVE_UNIFORMS, &uniform_num);
    d_glGetProgramiv(context, program, GL_ACTIVE_ATTRIBUTES, &attrib_num);

    new_program->uniform_num = uniform_num;
    new_program->attrib_num = attrib_num;

    //获得这些变量或者常量的名字的最大长度
    GLint max_uniform_name_len = 0;
    GLint max_attrib_name_len = 0;
    d_glGetProgramiv(context, program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_len);
    d_glGetProgramiv(context, program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_name_len);
    int buf_len = max_uniform_name_len > max_attrib_name_len ? max_uniform_name_len : max_attrib_name_len;
    buf_len += 1;

    GLchar *name_buf = new GLchar[buf_len];
    GLint size;
    GLenum type;
    GLint location;

    //获得每一个uniform的相关信息
    for (GLint i = 0; i < uniform_num; i++)
    {
        d_glGetActiveUniform(context, program, i, buf_len, NULL, &size, &type, name_buf);
        location = d_glGetUniformLocation(context, program, name_buf);
        new_program->uniform_loction_type[location] = type;
        new_program->uniform_name_location[name_buf] = location;
    }

    for (GLint i = 0; i < attrib_num; i++)
    {
        d_glGetActiveAttrib(context, program, i, buf_len, NULL, &size, &type, name_buf);
        location = d_glGetAttribLocation(context, program, name_buf);
        new_program->attrib_loction_type[location] = type;
        new_program->attrib_name_location[name_buf] = location;
    }

    //获取transform_feedback_varyings和active_uniform_blocks，有另外两个函数的指针要根据这个确定大小
    GLint active_uniform_blocks;
    d_glGetProgramiv(context, program, GL_ACTIVE_UNIFORM_BLOCKS, &active_uniform_blocks);
    new_program->active_uniform_block_count = active_uniform_blocks;

    GLint transform_feedback_varyings;
    d_glGetProgramiv(context, program, GL_TRANSFORM_FEEDBACK_VARYINGS, &transform_feedback_varyings);
    new_program->transform_feedback_varyings = transform_feedback_varyings;

    ((Direct_GPU_Context *)context)->all_program_info[program] = new_program;
    delete[] name_buf;
}

void d_glDeleteProgram(void *context, GLuint program)
{
    //删除program时需要删除guest端保存的中间状态
    if (((Direct_GPU_Context *)context)->all_program_info.find(program) != ((Direct_GPU_Context *)context)->all_program_info.end())
    {
        Program_Info *delete_program = ((Direct_GPU_Context *)context)->all_program_info[program];
        ((Direct_GPU_Context *)context)->all_program_info.erase(program);
        delete delete_program;
    }
    d_glDeleteProgram_origin(context, program);
}

void d_glDrawArrays(void *context, GLenum mode, GLint first, GLsizei count)
{
    if (get_bound_buffer(context, GL_ARRAY_BUFFER) == 0)
    {
        //没有绑定vbo，这个时候要复制顶点数组里的数据到对面
        send_point_attrib_data(context, first, first + count, 1);
    }
    d_glDrawArrays_origin(context, mode, first, count);
}

void d_glDrawArraysInstanced(void *context, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
    if (get_bound_buffer(context, GL_ARRAY_BUFFER) == 0)
    {
        //没有绑定vbo，这个时候要复制顶点数组里的数据到对面
        send_point_attrib_data(context, first, first + count, instancecount);
    }
    d_glDrawArraysInstanced_origin(context, mode, first, count, instancecount);
}

void d_glDrawElements(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices)
{

    send_point_attrib_data_indices(context, count, type, indices, 1);
    //这里的有没有绑定的函数区别，只是用于决定后面的indices到底是偏移还是数组
    if (get_bound_buffer(context, GL_ELEMENT_ARRAY_BUFFER) == 0)
    {
        //没有ebo绑定的，这个indices是真实的数组
        d_glDrawElements_without_bound(context, mode, count, type, indices);
    }
    else
    {
        d_glDrawElements_with_bound(context, mode, count, type, (GLsizeiptr)indices);
    }
}

void d_glDrawRangeElements(void *context, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
    //这个range是start和end，是说indices数组里的索引值，这个值要在start和end之间，假如不在这之间会产生错误，但是实际实现时可能不会检查这个错误，所以google模拟器这个调用和没有range的一模一样

    send_point_attrib_data_indices(context, count, type, indices, 1);
    if (get_bound_buffer(context, GL_ELEMENT_ARRAY_BUFFER) == 0)
    {
        d_glDrawRangeElements_without_bound(context, mode, start, end, count, type, indices);
    }
    else
    {
        d_glDrawRangeElements_with_bound(context, mode, count, start, end, type, (GLsizeiptr)indices);
    }
}

void d_glDrawElementsInstanced(void *context, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{

    send_point_attrib_data_indices(context, count, type, indices, instancecount);
    if (get_bound_buffer(context, GL_ELEMENT_ARRAY_BUFFER) == 0)
    {
        d_glDrawElementsInstanced_without_bound(context, mode, count, type, indices, instancecount);
    }
    else
    {
        d_glDrawElementsInstanced_with_bound(context, mode, count, type, (GLsizeiptr)indices, instancecount);
    }
}

void init_vertex_attrib_offset(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLenum invoke_type, const void *pointer)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    if (index > 32)
    {
        //error
        //todo 这里的32应该设置为当前gpu最大支持的顶点属性数
    }

    //只有当当前状态下有绑定vbo时，才需要保存下这个数据
    if (get_bound_buffer(context, GL_ARRAY_BUFFER) == 0)
    {
        return;
    }

    Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];
    if (point_data == NULL)
    {
        point_data = new Attrib_Point;
        bound_buffer->vao_point_data[status->vertex_array_buffer] = point_data;
        memset(point_data, 0, sizeof(Attrib_Point));
        point_data->max_len[index] = 0;
    }
    else
    {
        //原来就有顶点在，原来是顶点数组，现在变成缓冲区数组，那么原来的就要put_page了
        if (point_data->is_offset[index] == 0)
        {

            if (point_data->get_page[index])
            {
                put_page_host(context,point_data->data_index_length[index],8);
                put_page_host(context, point_data->data[index], point_data->max_len[index]);
                point_data->get_page[index] = 0;
            }

            point_data->max_len[index] = 0;
        }
    }

    point_data->location[index] = 1;
    point_data->size[index] = size;
    point_data->type[index] = type;
    point_data->normalized[index] = normalized;
    if(pointer==point_data->data[index]){
        point_data->data_has_change[index]=GL_FALSE;
    }else{
        point_data->data_has_change[index]=GL_TRUE;
        point_data->data[index] = pointer;

    }
    
    point_data->data_real_index[index] = index;
    point_data->invoke_type[index] = invoke_type;
    point_data->is_offset[index] = 1;
    point_data->stride[index] = stride;
    if (stride == 0)
    {
        point_data->stride[index] = point_data->size[index] * gl_sizeof(point_data->type[index]);
        if (point_data->type[index] == GL_INT_2_10_10_10_REV || point_data->type[index] == GL_UNSIGNED_INT_2_10_10_10_REV)
        {
            point_data->stride[index] /= 4;
        }

        point_data->has_update = 1;
    }
}

void init_vertex_attrib_pointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLenum invoke_type, const void *pointer)
{
    //首先需要看有没有绑定vbo，假如vbo为0，那么这pointer指针需要保存下来，实际传输过去不需要映射数据，只有最终draw时才复制数据
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    if (index > 32)
    {
        //error
        //todo 这里的32应该设置为当前gpu最大支持的顶点属性数
    }

    //只有当当前状态下没有绑定vbo时，才需要保存下这个数据
    if (get_bound_buffer(context, GL_ARRAY_BUFFER) != 0)
    {
        return;
    }

    Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];
    if (point_data == NULL)
    {
        point_data = new Attrib_Point;
        bound_buffer->vao_point_data[status->vertex_array_buffer] = point_data;
        memset(point_data, 0, sizeof(Attrib_Point));
        point_data->max_len[index] = 0;
    }
    else
    {
        //原来就有顶点在，只有当数组位置不一致时(而且是母数组时)，数组长度才要清零，这个时候假如get_page过就要put_page
        if (point_data->is_offset[index] == 0 && pointer != point_data->data[index] && point_data->data_real_index[index] == index)
        {

            if (point_data->get_page[index])
            {
                put_page_host(context,point_data->data_index_length[index],8);
                put_page_host(context, point_data->data[index], point_data->max_len[index]);
                point_data->get_page[index] = 0;
            }

            point_data->max_len[index] = 0;
        }
    }

    point_data->location[index] = 1;
    point_data->size[index] = size;
    point_data->type[index] = type;
    point_data->normalized[index] = normalized;
    point_data->data[index] = pointer;
    point_data->data_real_index[index] = index;
    point_data->invoke_type[index] = invoke_type;
    point_data->is_offset[index] = 0;
    point_data->has_update = 1;
    point_data->stride[index] = stride;
    if (stride == 0)
    {
        point_data->stride[index] = point_data->size[index] * gl_sizeof(point_data->type[index]);
        if (point_data->type[index] == GL_INT_2_10_10_10_REV || point_data->type[index] == GL_UNSIGNED_INT_2_10_10_10_REV)
        {
            point_data->stride[index] /= 4;
        }
    }

    //每次init这个point数据时，都进行一次聚类的一些操作，方便之后传输数据减少复制

    for (int i = 0; i < 32; i++)
    {
        if (point_data->location[i] == 0 || i == index || point_data->is_offset[i]==1)
        {
            continue;
        }
        point_data->data_dis[i][index] = llabs((uint64_t)((char *)point_data->data[i] - (char *)pointer));
        point_data->data_dis[index][i] = point_data->data_dis[i][index];
        //聚类的过程中，两个是一个类要求其距离在一个stride之内，且其stride都一样
        if (point_data->data_dis[i][index] < stride && point_data->stride[i] == stride)
        {
            //假如有人不按常理出牌，结构数组中不同的属性不在一个index上，或者是用结构数组，但是stride居然还不一样，那这就不是我们能控制的了
            if (point_data->data[i] > pointer)
            {
                point_data->data_real_index[i] = index;
            }
            else
            {
                if (point_data->data_real_index[i] == i)
                {
                    //假如这个index不相等的话，就说明还有比当前索引i更小一点的，所以这个时候其实不用更新，防止更新过度，反而更新到大一些的去了
                    point_data->data_real_index[index] = i;
                }
            }
        }
    }
}

void d_glVertexAttribPointer(void *context, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
{

    if (get_bound_buffer(context, GL_ARRAY_BUFFER) == 0)
    {
        // 没有绑定时把这个指针传输过去没用，因为数据会发生变化，所以先保存下来具体位置
        init_vertex_attrib_pointer(context, index, size, type, normalized, stride, GL_FLOAT, pointer);
    }
    else
    {
        init_vertex_attrib_offset(context, index, size, type, normalized, stride, GL_FLOAT, pointer);
        //这时pointer不是指针
        d_glVertexAttribPointer_with_bound(context, index, size, type, normalized, stride, (GLintptr)pointer);
    }
}

void d_glVertexAttribIPointer(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer)
{
    //error处理，这个模式下不能是浮点数类型
    if (type == GL_HALF_FLOAT || type == GL_FLOAT || type == GL_FIXED || type == GL_INT_2_10_10_10_REV || type == GL_UNSIGNED_INT_2_10_10_10_REV)
    {
        //todo error处理
        return;
    }

    if (get_bound_buffer(context, GL_ARRAY_BUFFER) == 0)
    {
        // 没有绑定时把这个指针传输过去没用，因为数据会发生变化，所以先保存下来
        init_vertex_attrib_pointer(context, index, size, type, GL_FALSE, stride, GL_INT, pointer);
    }
    else
    {
        init_vertex_attrib_offset(context, index, size, type, GL_FALSE, stride, GL_INT, pointer);
        //这时pointer不是指针
        d_glVertexAttribIPointer_with_bound(context, index, size, type, stride, (GLintptr)pointer);
    }
}

void d_glGetVertexAttribPointerv(void *context, GLuint index, GLenum pname, void **pointer)
{
    if (pname != GL_VERTEX_ATTRIB_ARRAY_POINTER)
    {
        return;
    }
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];
    *pointer = (void *)point_data->data[index];

    // if(get_bound_buffer(context, GL_ARRAY_BUFFER) == 0){
    //     Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    //     Buffer_Status *status = bound_buffer->buffer_status;
    //     Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];
    //     *pointer=(void *)point_data->data[index];
    // }else{
    //     d_glGetVertexAttribPointerv_with_bound(context,index,pname,(GLintptr *)pointer);
    // }
}

int get_vertex_attrib(void *context, GLuint index, GLenum pname, void *param)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];
    switch (pname)
    {
    //下面这几种情况那个指针都只有一个元素，不是真实的数组
    case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
        *((GLint *)param) = (GLint)point_data->enabled[index];
        break;
    case GL_VERTEX_ATTRIB_ARRAY_INTEGER:
        if (point_data->invoke_type[index] == GL_INT)
        {
            *((GLint *)param) = 1;
        }
        else
        {
            *((GLint *)param) = 0;
        }
        break;
    case GL_VERTEX_ATTRIB_ARRAY_SIZE:
        *((GLint *)param) = (GLint)point_data->size[index];
        break;
    case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
        *((GLint *)param) = (GLint)point_data->stride[index];
        break;
    case GL_VERTEX_ATTRIB_ARRAY_TYPE:
        *((GLenum *)param) = (GLenum)point_data->type[index];
        break;
    case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
        *((GLint *)param) = (GLint)point_data->normalized[index];
        break;
    case GL_VERTEX_ATTRIB_ARRAY_DIVISOR:
        *((GLuint *)param) = (GLuint)point_data->divisor[index];
        break;

    //这种情况下返回的是数组，即常量顶点属性，因此要交给host端去返回（todo:是否在客户端缓存下来会更快？）
    case GL_CURRENT_VERTEX_ATTRIB:
        return 0;
        break;

    //下面这两个都是gles3.2的，所以暂时先放着 todo
    case GL_VERTEX_ATTRIB_BINDING:
        break;
    case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
        break;
    default:
        //这里理论上不可能
        break;
    }
    return 1;
}

void d_glGetVertexAttribIiv(void *context, GLuint index, GLenum pname, GLint *params)
{
    if (!get_vertex_attrib(context, index, pname, params))
    {
        d_glGetVertexAttribIiv_origin(context, index, pname, params);
    }
}
void d_glGetVertexAttribIuiv(void *context, GLuint index, GLenum pname, GLuint *params)
{
    if (!get_vertex_attrib(context, index, pname, params))
    {
        d_glGetVertexAttribIuiv_origin(context, index, pname, params);
    }
}

void d_glGetVertexAttribfv(void *context, GLuint index, GLenum pname, GLfloat *params)
{
    if (!get_vertex_attrib(context, index, pname, params))
    {
        d_glGetVertexAttribfv_origin(context, index, pname, params);
    }
}
void d_glGetVertexAttribiv(void *context, GLuint index, GLenum pname, GLint *params)
{
    if (!get_vertex_attrib(context, index, pname, params))
    {
        d_glGetVertexAttribiv_origin(context, index, pname, params);
    }
}

void d_glDisableVertexAttribArray(void *context, GLuint index)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];
    point_data->enabled[index] = 0;
    d_glDisableVertexAttribArray_origin(context, index);
}
void d_glEnableVertexAttribArray(void *context, GLuint index)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];
    point_data->enabled[index] = 1;
    d_glEnableVertexAttribArray_origin(context, index);
}

void d_glVertexAttribDivisor(void *context, GLuint index, GLuint divisor)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    Attrib_Point *point_data = bound_buffer->vao_point_data[status->vertex_array_buffer];

    // //注意，divisor只有在有绑定的时候才直接发送host
    // if (get_bound_buffer(context, GL_ARRAY_BUFFER) == 0)
    // {

    //     if (index > 32)
    //     {
    //         //error
    //         //todo 这里的32应该设置为当前gpu最大支持的顶点属性数
    //     }
    //     point_data->divisor[index] = divisor;
    // }
    // else
    // {
        point_data->divisor[index] = divisor;
        d_glVertexAttribDivisor_origin(context, index, divisor);
    // }
}

/**
 * @brief 获取内存映射后的内存指针     guest端单独实现
 * 
 * @param context 
 * @param target 
 * @param pname 
 * @param params 
 */
void d_glGetBufferPointerv(void *context, GLenum target, GLenum pname, void **params)
{
    if (pname != GL_BUFFER_MAP_POINTER)
    {
        //todo set error
        return;
    }
    Buffer_Mapped *buffer_ptr = &(((Direct_GPU_Context *)context)->buffer_ptr);
    void *ret = NULL;
    switch (target)
    {
    case GL_ARRAY_BUFFER:
        ret = buffer_ptr->array_buf_ptr.map_data;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        ret = buffer_ptr->atomic_counter_buf_ptr.map_data;
        break;
    case GL_COPY_READ_BUFFER:
        ret = buffer_ptr->copy_read_buf_ptr.map_data;
        break;
    case GL_COPY_WRITE_BUFFER:
        ret = buffer_ptr->copy_write_buf_ptr.map_data;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        ret = buffer_ptr->dispatch_indirect_buf_ptr.map_data;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        ret = buffer_ptr->draw_indirect_buf_ptr.map_data;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        ret = buffer_ptr->elem_array_buf_ptr.map_data;
        break;
    case GL_PIXEL_PACK_BUFFER:
        ret = buffer_ptr->pixel_pack_buf_ptr.map_data;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        ret = buffer_ptr->pixel_unpack_buf_ptr.map_data;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        ret = buffer_ptr->shader_storage_buf_ptr.map_data;
        break;
    case GL_TEXTURE_BUFFER:
        ret = buffer_ptr->texture_buf_ptr.map_data;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        ret = buffer_ptr->trans_feedback_buf_ptr.map_data;
        break;
    case GL_UNIFORM_BUFFER:
        ret = buffer_ptr->uniform_buf_ptr.map_data;
        break;
    }
    if (params == NULL)
    {
        return;
    }
    *params = ret;
    return;
}

void d_glBufferData(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    if (target == GL_ELEMENT_ARRAY_BUFFER)
    {
        Element_Array_Buffer *ebo_data = bound_buffer->ebo_buffer[status->element_array_buffer];
        if (ebo_data == NULL)
        {
            ebo_data = new Element_Array_Buffer;
            ebo_data->size = size;
            ebo_data->type = 0;
            //这里给ebo对应的那个索引数组保存一个备份，因为人家可能bufferdata后就释放了这个空间
            char *data_save = new char[size];
            if (data != NULL)
            {
                memcpy(data_save, data, size);
            }
            //注意，这里只能保存下数据，但是不能确定数据到底是啥，draw的时候才能知道是啥类型，所以这个时候还没法用
            ebo_data->data = data_save;
        }
        else
        {
            //错误处理，bufferdata一定是对一个刚申请的ebo
        }
    }
    d_glBufferData_custom(context, target, size, data, usage);
}

void d_glBufferSubData(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{

    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    if (target == GL_ELEMENT_ARRAY_BUFFER)
    {
        Element_Array_Buffer *ebo_data = bound_buffer->ebo_buffer[status->element_array_buffer];
        if (ebo_data == NULL || ebo_data->data == NULL)
        {

            //正常流程这里不可能到，需要特殊处理
        }
        else
        {
            //复制到ebo的那个数组中，同时更新update，表示之后需要重新计算st表
            memcpy((char *)ebo_data->data + offset, data, size);
            ebo_data->has_update = GL_TRUE;
        }
    }
    d_glBufferSubData(context, target, offset, size, data);
}

void d_glDeleteBuffers(void *context, GLsizei n, const GLuint *buffers)
{

    Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);

    for (int i = 0; i < n; i++)
    {
        GLuint id = buffers[i];
        if(id==0){
            continue;
        }
        GLenum type = bound_buffer->buffer_type[id];
        switch (type)
        {
        case GL_ELEMENT_ARRAY_BUFFER:
            //ebo是我们单独复制出来的数据，因为假如前面没有绑定vbo，设置顶点属性时直接设的数组指针，那么单独保留ebo里的数据就能方便找到顶点属性数组的大小
            //也因为为了方便确定大小，这个buffer里还保存了max_index
        {

            Element_Array_Buffer *ebo_data = bound_buffer->ebo_buffer[id];
            if (ebo_data != NULL)
            {
                delete[] ebo_data->data;

                if (ebo_data->dp_max != NULL)
                {
                    for (int i = 1; i <= ebo_data->dp_num; i++)
                    {
                        delete[] ebo_data->dp_max[i];
                        delete[] ebo_data->dp_min[i];
                    }
                    delete[] ebo_data->dp_max;
                    delete[] ebo_data->dp_min;
                }
                delete[] ebo_data;
            }
            //就算id不在map里也只是erase后返回0
            bound_buffer->ebo_buffer.erase(id);
            break;
        }
        case GL_ARRAY_BUFFER:
            //vbo
            break;
        }
        bound_buffer->buffer_type.erase(id);
    }

    d_glDeleteBuffers_origin(context, n, buffers);
}

Map_Result *get_map_result(void *context, GLenum target)
{
    Buffer_Mapped *buffer_ptr = &(((Direct_GPU_Context *)context)->buffer_ptr);
    Map_Result *map_res = NULL;
    switch (target)
    {
    case GL_ARRAY_BUFFER:
        map_res = &(buffer_ptr->array_buf_ptr);
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        map_res = &(buffer_ptr->atomic_counter_buf_ptr);
        break;
    case GL_COPY_READ_BUFFER:
        map_res = &(buffer_ptr->copy_read_buf_ptr);
        break;
    case GL_COPY_WRITE_BUFFER:
        map_res = &(buffer_ptr->copy_write_buf_ptr);
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        map_res = &(buffer_ptr->dispatch_indirect_buf_ptr);
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        map_res = &(buffer_ptr->draw_indirect_buf_ptr);
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        map_res = &(buffer_ptr->elem_array_buf_ptr);
        break;
    case GL_PIXEL_PACK_BUFFER:
        map_res = &(buffer_ptr->pixel_pack_buf_ptr);
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        map_res = &(buffer_ptr->pixel_unpack_buf_ptr);
        break;
    case GL_SHADER_STORAGE_BUFFER:
        map_res = &(buffer_ptr->shader_storage_buf_ptr);
        break;
    case GL_TEXTURE_BUFFER:
        map_res = &(buffer_ptr->texture_buf_ptr);
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        map_res = &(buffer_ptr->trans_feedback_buf_ptr);
        break;
    case GL_UNIFORM_BUFFER:
        map_res = &(buffer_ptr->uniform_buf_ptr);
        break;
    }

    return map_res;
}

void *d_glMapBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    char *map_data;
    d_glGetBufferPointerv(context, target, GL_BUFFER_MAP_POINTER, (void **)&map_data);
    if (map_data == NULL)
    {
        //为null说明没有映射过
        map_data = new char[length];
    }

    Map_Result *map_res = get_map_result(context, target);

    if (map_res == NULL)
    {
        //需要设置error
        return NULL;
    }

    map_res->access = access;
    map_res->length = length;
    map_res->map_data = map_data;
    map_res->offset = offset;
    map_res->target = target;

    if (access & GL_MAP_READ_BIT)
    {
        //理论上只有read模式需要把host的数据读取回来，但是不知道为啥goole写的有write标志而且没有两个invalidate标志也把数据读取回来
        d_glMapBufferRange_read(context, target, offset, length, access, map_data);
    }
    else
    {
        size_t int_data_loc=0;
        memcpy(map_res->map_data_info+int_data_loc,&target,sizeof(GLenum));
        int_data_loc+=sizeof(GLenum);
        memcpy(map_res->map_data_info+int_data_loc,&offset,sizeof(GLintptr));
        int_data_loc+=sizeof(GLintptr);
        memcpy(map_res->map_data_info+int_data_loc,&length,sizeof(GLsizeiptr));
        int_data_loc+=sizeof(GLsizeiptr);
        memcpy(map_res->map_data_info+int_data_loc,&access,sizeof(GLbitfield));
        int_data_loc+=sizeof(GLbitfield);

        //这里先让host那边保存下来这个指针，之后unmap或者flush的时候直接取相应的位置的数据就行了
        d_glMapBufferRange_write(context, length, (const void *)map_res->map_data_info, (const void *)map_data);
    }

    return map_data;
}

GLboolean d_glUnmapBuffer(void *context, GLenum target)
{

    Map_Result *map_res = get_map_result(context, target);
    if (map_res->map_data == NULL)
    {
        //为null应该要报错
        return GL_FALSE;
    }

    GLboolean ret = GL_TRUE;

    //注意：前面map时先判断read，这里先判断write，是因为可能同时有read和write标志，有write标志时，相关内存页是一定要释放的，而前面先read是因为，当有read时，最后一个指针参数是一定要作为输出参数来处理的
    //只有同时是wirte并且没有设置flush标志时，unmap时才需要刷新全部范围
    d_glUnmapBuffer_special(context, target);

    //unmap是同步的（放在2.6里），所以这里肯定是host端该复制的复制完了，然后这里才会put_page的
    // if (map_res->access & GL_MAP_WRITE_BIT)
    if (!(map_res->access & GL_MAP_READ_BIT))
    {
        put_page_host(context, map_res->map_data_info, sizeof(GLenum)+sizeof(GLintptr)+sizeof(GLsizeiptr)+sizeof(GLbitfield));
        put_page_host(context, map_res->map_data, map_res->length);
    }

    // if ((map_res->access & GL_MAP_WRITE_BIT)&&(map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT)==0)
    // {
    //     //其他情况下，也就是write的情况，host需要复制过去后，再unmap,但是这是host操作的事情，所以不需要处理
    //     //因为在map期间对缓冲区不允许操作，假如真的操作了是未定义操作，不同平台上产生不同的效果，所以暂时不考虑map期间的操作这个事（数据写入与真实的数据写入时机不一致）
    //     //注意，在host端只有write模式才会写回去，假如有GL_MAP_FLUSH_EXPLICIT_BIT，就不需要写回去
    //     ret = d_glUnmapBuffer_write(context, target);
    // }
    // else
    // {
    //     //read的情况下，host那边只用unmap就行了,所以其实不需要映射内存过去，所以把长度设置为0就行了
    //     ret = d_glUnmapBuffer_read(context, target);
    // }
    //unmap后要删除map_res的数据，但是在此之前，假如是索引数组的话，需要保存下来
    if (target == GL_ELEMENT_ARRAY_BUFFER && (map_res->access & GL_MAP_WRITE_BIT))
    {
        Bound_Buffer *bound_buffer = &(((Direct_GPU_Context *)context)->bound_buffer_status);
        Buffer_Status *status = bound_buffer->buffer_status;
        Element_Array_Buffer *ebo_data = bound_buffer->ebo_buffer[status->element_array_buffer];
        if (ebo_data == NULL || ebo_data->data == NULL)
        {
            //出问题了，这里肯定不可能是NULL，因为在map之前肯定分配过gpu内存的，所以肯定不是空的了
        }
        else
        {
            memcpy((char *)ebo_data->data + map_res->offset, map_res->map_data, map_res->length);
            ebo_data->has_update = GL_TRUE;
        }
    }

    delete[] map_res->map_data;
    memset(map_res, 0, sizeof(Map_Result));

    //只有一种很罕见的情况会返回false，所以这里甚至可以直接返回True
    //todo: 验证直接返回True会不会有啥问题
    return ret;
}

void d_glFlushMappedBufferRange(void *context, GLenum target, GLintptr offset, GLsizeiptr length)
{

    Map_Result *map_res = get_map_result(context, target);
    if (map_res->map_data == NULL || (map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT) == 0)
    {
        //为null应该要报错
        return;
    }

    //能够调用到这个函数只可能是write模式，需要把数据写过去，但是host只需要读取host部分数据就行了
    d_glFlushMappedBufferRange_origin(context, target, offset, length);
    return;
}

/**
 * @brief 初始化context，注意这个context是线程唯一的
 * 
 */
void * context_init(void)
{


    Direct_GPU_Context *real_context = new Direct_GPU_Context;


    //初始化Bound_Buffer
    Bound_Buffer *bound_buffer = &(real_context->bound_buffer_status);

    //默认4000大小，不够时运行时自动增加
    bound_buffer->ebo_mm_num = 4000;

    GLuint *temp = new GLuint[4000 + 1];
    for (int i = 1; i < 4000 + 1; i++)
    {
        temp[i] = ((i & (i - 1)) == 0) ? temp[i - 1] + 1 : temp[i - 1];
    }
    bound_buffer->ebo_mm = temp;

    Buffer_Status *origin_status=new Buffer_Status;
    memset(origin_status,0,sizeof(Buffer_Status));
    bound_buffer->vao_status[0]=origin_status;
    bound_buffer->buffer_status=origin_status;

    Attrib_Point *origin_attrib_point=new Attrib_Point;
    memset(origin_attrib_point,0,sizeof(Attrib_Point));
    bound_buffer->vao_point_data[0]=origin_attrib_point;



    //初始化pixel_store_status
    Pixel_Store_Status *pixel_store=&(real_context->pixel_store_status);
    memset(pixel_store,0,sizeof(Pixel_Store_Status));
    pixel_store->unpack_alignment=4;
    pixel_store->pack_alignment=4;


    Buffer_Mapped *map_result=&(real_context->buffer_ptr);
    memset(map_result,0,sizeof(Buffer_Mapped));



    //这里打开设备
    int direct_express = open("/dev/direct_express", O_RDWR);
    if (direct_express <0)
    {
        delete real_context;
        return NULL;
    }
    real_context->direct_express = direct_express;
    //暂时先用这个大小的缓冲区
    // real_context->need_free_bufs.reserve(100);
    // memset(real_context->asyn_buf, 0, sizeof(char) * MAX_ASYN_BUF_SIZE);
    // real_context->asyn_buf_loc = 0;




    return real_context;
}

const GLubyte *d_glGetStringi(GLenum name, GLuint index)
{
    return "test";
}  

const GLubyte *d_glGetString(GLenum name)
{
    return "test";
}