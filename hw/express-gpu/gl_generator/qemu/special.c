#include "GLES3/gl3.h"
#include "GLES2/gl2ext.h"
#include "GLES3/gl3platform.h"

#include "express-gpu/glv3_trans.h"

// glMapBufferRange GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf
// GLboolean glUnmapBuffer GLenum target, GLsizeiptr length,const void *mem_buf
// glFlushMappedBufferRange GLenum target, GLintptr offset, GLsizeiptr length,const void *membuf
//拒绝映射两次的行为由guest来保证
// gpu_mem_map(render_context, target, offset, length, access, mem_buf);
// GLboolean ret = gpu_mem_unmap(render_context, target, mem_buf);
// gpu_mem_flush(render_context,target, offset, length, mem_buf);

//host不实现，交给guest实现
// glGetBufferPointerv GLenum target, GLenum pname, void **params
//glGetBufferPointervOES GLenum target, GLenum pname, void **params
// glExtGetBufferPointervQCOM GLenum target, void **params

//const GLubyte* glGetStringi GLenum name, GLuint index
//const GLubyte* glGetString GLenum name

// glGetVertexAttribPointerv GLuint index, GLenum pname, void **pointer
//todo 调查清楚这个pointer到底是用来干啥的  ---可能用来存放指针，可能用来存放偏移

//google模拟器未实现
//glMapBufferRangeEXT GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access
//将buffer对象的全部或部分数据存储映射到客户端地址空间   google模拟器未实现
//glFlushMappedBufferRangeEXT GLenum target, GLintptr offset, GLsizeiptr length
//映射范围修改  google模拟器未实现

#include "express-gpu/offscreen_render_thread.h"

typedef struct Opengl_Context
{
    Pixel_Store_Status pixel_store_status;
    Bound_Buffer bound_buffer_status;

    GHashTable *buffer_map;

} Opengl_Context;

typedef struct Pixel_Store_Status
{
    //表示数据对齐的标准
    int unpack_alignment;
    int pack_alignment;

    //表示对于每个图片，每一行有多少个像素点，为0的时候像素点的个数就是传入的width
    int unpack_row_length;
    int pack_row_length;

    //表示对于这个图片的读取要跳过多少行像素
    int unpack_skip_rows;
    int pack_skip_rows;

    //表示对于这个图片的这一行的读取要跳过多少个像素
    int unpack_skip_pixels;
    int pack_skip_pixels;

    //表示对于一堆图片的读取要跳过多少张图片
    int unpack_skip_images;

    //表示每张图片的高度
    int unpack_image_height;
} Pixel_Store_Status;

typedef struct Buffer_Status
{
    GLint array_buffer;         //vbo
    GLint element_array_buffer; //ebo
    GLint copy_read_buffer;
    GLint copy_write_buffer;
    GLint pixel_pack_buffer;
    GLint pixel_unpack_buffer;
    GLint transform_feedback_buffer;
    GLint uniform_buffer;
    GLint atomic_counter_buffer;
    GLint dispatch_indirect_buffer;
    GLint draw_indirect_buffer;
    GLint shader_storage_buffer;
    GLint vertex_array_buffer; //vao

} Buffer_Status;

typedef struct Bound_Buffer
{

    //这个buffer_status指针是指向实际vao_status里的值
    Buffer_Status *buffer_status;

    // std::map<GLint, GLenum> buffer_type;
    GHashTable *buffer_type;

    //std::map<GLint, Buffer_Status *> vao_status;
    GHashTable *vao_status;

    //std::map<GLint, Attrib_Point *> vao_point_data;
    GHashTable *vao_point_data;

    GLint asyn_unpack_texture_buffer;
    GLint asyn_pack_texture_buffer;


    //std::map<GLint, Element_Array_Buffer *> ebo_buffer;

} Bound_Buffer;

typedef struct Attrib_Point
{
    //真实的存储下来的数据
    Scatter_Data *data[32];
    GLint data_len[32];

    //father指示其的数据在哪，然后offset指示了在数组中的
    GLint father[32];
    GLintptr offset[32];

    //顶点的常规属性
    GLint size[32];
    GLenum type[32];
    GLsizei stride[32];
    GLboolean normalized[32];
    GLuint divisor[32];
    GLenum invoke_type[32];
    GLuint min_index;
    GLuint max_index;

    GLint buffer_object;
    GLint buffer_len;

    GLint indices_buffer_object;
    GLint indices_buffer_len;

    //表示顶点属性是否启用顶点数组
    GLboolean enabled[32];

} Attrib_Point;


typedef struct Guest_Host_Map{
    GLubyte *host_data;
    Scatter_Data *guest_data;
    size_t map_len;
    GLenum target;
    GLbitfield access;

}Guest_Host_Map;


typedef struct Guest_Mem{
    Scatter_Data *scatter_data;
    int num;
}Guest_Mem;

typedef struct Scatter_Data
{
    size_t len;
    GLubyte *data;
} Scatter_Data;


void guest_write(Guest_Mem *guest,void *host,size_t start_loc, size_t length){

    Scatter_Data *guest_data=guest->scatter_data;
    host_guest_buffer_exchange(guest_data, host, start_loc, length, 1);

}

void guest_read(Guest_Mem *guest,void *host,size_t start_loc, size_t length){

    Scatter_Data *guest_data=guest->scatter_data;
    host_guest_buffer_exchange(guest_data, host, start_loc, length, 0);

}


void context_init(void *context){
    Opengl_Context *opengl_context=(Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    

    opengl_context->buffer_map=g_hash_table_new(g_direct_hash, g_direct_equal);

    bound_buffer->vao_status=g_hash_table_new(g_direct_hash, g_direct_equal);
    bound_buffer->vao_point_data=g_hash_table_new(g_direct_hash, g_direct_equal);

    bound_buffer->buffer_type=g_hash_table_new(g_direct_hash, g_direct_equal);
    
    Buffer_Status *status=g_malloc(sizeof(Buffer_Status));
    memset(status,0,sizeof(Buffer_Status));
    g_hash_table_insert(bound_buffer->vao_status, GINT_TO_POINTER(0), (gpointer)status);

    Attrib_Point *temp_point=g_malloc(sizeof(Attrib_Point));
    memset(temp_point,0,sizeof(Attrib_Point));
    GLint temp_buffer[4];
    // glGenBuffers(4,temp_buffer);
    temp_point->buffer_object=temp_buffer[0];
    temp_point->indices_buffer_object=temp_buffer[1];

    g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(0), (gpointer)temp_point);

    bound_buffer->buffer_status=status;

    bound_buffer->asyn_pack_texture_buffer=temp_buffer[2];
    bound_buffer->asyn_unpack_texture_buffer=temp_buffer[3];

}

/**
 * @brief 
 * 
 * @param guest_data 
 * @param host_data 
 * @param start_loc 
 * @param length 
 * @param is_guest_to_host 
 */
void host_guest_buffer_exchange(Scatter_Data *guest_data, unsigned char *host_data, size_t start_loc, size_t length, int is_guest_to_host)
{

    int walk_loc = 0;
    int remain_len = length;
    int guest_loc = start_loc;
    int host_loc = 0;
    int cpy_len = 0;
    int guest_index = 0;
    while (remain_len > 0)
    {
        if (guest_data[guest_index].len > guest_loc)
        {
            //一直找到start_loc所在的那个区块
            if (remain_len < guest_data[guest_index].len - guest_loc)
            {
                if (is_guest_to_host)
                {
                    memcpy(host_data + host_loc, guest_data[guest_index].data + guest_loc, remain_len);
                }
                else
                {
                    memcpy(guest_data[guest_index].data + guest_loc, host_data + host_loc, remain_len);
                }
                break;
            }
            else
            {
                if (is_guest_to_host)
                {
                    memcpy(host_data + host_loc, guest_data[guest_index].data + guest_loc, guest_data[guest_index].len - guest_loc);
                }
                else
                {
                    memcpy(guest_data[guest_index].data + guest_loc, host_data + host_loc, guest_data[guest_index].len - guest_loc);
                }
                host_loc += guest_data[guest_index].len - guest_loc;
                remain_len -= guest_data[guest_index].len - guest_loc;
            }
            //只要复制了一次之后guest_loc都为0，因为这个时候后面的都是从下一段内存的刚开始的位置开始（因为内存连续）
            guest_loc = 0;
        }
        else
        {
            guest_loc -= guest_data[guest_index].len;
        }
        guest_index++;
    }
}

void flush_array_buffer(Attrib_Point *point_data, GLint instancecount)
{
    //找到当前应该申请的显存空间的最大值
    int max_len = 0;
    //这个father_loc用来描述每个真正的数组，对应到缓冲区的哪个地方
    int father_loc[32];
    memset(father_loc, 0, sizeof(father_loc));
    for (int i = 0; i < 32; i++)
    {
        if (point_data->data[i] != NULL)
        {
            father_loc[i] = max_len;
            max_len += point_data->data_len[i];
        }
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

    int father_enable[32];
    int father_max_loc[32];
    int father_min_loc[32];
    memset(father_enable, 0, sizeof(father_enable));
    memset(father_max_loc, 0, sizeof(father_max_loc));
    memset(father_min_loc, 0x3f3f3f3f, sizeof(father_min_loc));

    //通过这个循环找到实际那几个数组内，需要复制到缓冲区的偏移的两端
    for (int i = 0; i < 32; i++)
    {
        father_enable[point_data->father[i]] += point_data->enabled[i];

        if (point_data->enabled[i] == 0)
        {
            continue;
        }

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

    //然后把数组根据偏移复制进去
    for (int i = 0; i < 32; i++)
    {
        if (point_data->data[i] != NULL)
        {
            //复制完后就通知一下更新一下这小部分
            host_guest_buffer_exchange(point_data->data[i], map_pointer + father_loc[i], father_min_loc[i], father_max_loc[i] - father_min_loc[i], 1);
            glFlushMappedBufferRange(GL_ARRAY_BUFFER, father_loc[i] + father_min_loc[i], father_max_loc[i] - father_min_loc[i]);
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    //此时数据都更新到位了，该设置顶点属性了
    for (int i = 0; i < 32; i++)
    {
        if (point_data->enabled[i])
        {
            if (point_data->invoke_type[i] == GL_INT)
            {
                glVertexAttribIPointer(i, point_data->size[i], point_data->type[i], point_data->stride[i],
                                       point_data->offset[i] + father_loc[point_data->father[i]]);
            }
            else
            {
                glVertexAttribPointer(i, point_data->size[i], point_data->type[i], point_data->normalized[i],
                                      point_data->stride[i], point_data->offset[i] + father_loc[point_data->father[i]]);
            }
        }
    }

    //全部操作完成后，这时候数据写入到缓冲区中，然后顶点属性的偏移也设置了，这个时候就可以draw了
    return;
}

void set_attrib_point(void *context,GLsizei instancecount){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    if (status->array_buffer == 0)
    {
        //绑定为0的情况，这个时候需要把数据复制到临时的array_buffer中
        Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, status->vertex_array_buffer);
        if (point_data == NULL)
        {
            return;
        }
        flush_array_buffer(point_data,instancecount);
    }
}

void prepare_unpack_texture(void *context,Scatter_Data *s_data,int start_loc,int end_loc){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    GLint asyn_texture=bound_buffer->asyn_unpack_texture_buffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,asyn_texture);
    //因为曾经bind过texture，所以这里bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_UNPACK_BUFFER, end_loc,NULL,GL_STREAM_DRAW);

    //然后把数据复制到内存里，之后交给dma传输   到底是invalidata还是unsync？
    GLubyte *map_pointer=glMapBufferRange(GL_PIXEL_UNPACK_BUFFER,start_loc,end_loc-start_loc,GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT);
    host_guest_buffer_exchange(s_data,map_pointer,start_loc,end_loc-start_loc,1);
}


void gl_pixel_data_loc(void *store_status, GLsizei width, GLsizei height, GLenum format, GLenum type, int pack, int *start_loc,int *end_loc)
{
    gl_pixel_data_3d_loc(store_status, width, height, 1, format, type, pack,start_loc,end_loc);
}

void gl_pixel_data_3d_loc(void *store_status, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pack, int *start_loc,int *end_loc)
{
    Pixel_Store_Status *status = (Pixel_Store_Status *)store_status;
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
    *start_loc = (skip_images * real_height + skip_rows) * width_size + skip_pixels * pixel_size;

    //数据图像结束读取的地方，等于 开始读取的地方+要读取的图片的高度*图片每行所占的字节数*图片的张数，depth表示深度也就表示要读取多少张图片
    *end_loc = *start_loc + real_height * width_size * depth;

    return *end_loc;
}




void set_attrib_point_index(void *context,Scatter_Data *data,int len){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, status->vertex_array_buffer);
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



void d_glVertexAttribIPointer_with_bound(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr pointer)
{
    glVertexAttribIPointer(index, size, type, stride, pointer);
}
void d_glVertexAttribIPointer_data(void *context, GLuint index, GLuint size, const void *pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, status->vertex_array_buffer);
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
    point_data->data_len[index] = size;

    return;
}
void d_glVertexAttribIPointer_offset(void *context, GLuint index, GLint size, GLenum type, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, status->vertex_array_buffer);
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
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void d_glVertexAttribPointer_data(void *context, GLuint index, GLuint size, const void *pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;

    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, status->vertex_array_buffer);
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

    point_data->data_len[index] = size;

    return;
}
void d_glVertexAttribPointer_offset(void *context, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint min_index, GLuint max_index, GLuint index_father, GLuint divisor, GLboolean enabled, GLintptr pointer)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    Attrib_Point *point_data = g_hash_table_lookup(bound_buffer->vao_point_data, status->vertex_array_buffer);
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
    point_data->invoke_type[index] = GL_INT;
    point_data->min_index = min_index;
    point_data->max_index = max_index;
}

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer)
{

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
    GLint id = buffer;
    //由于这里status是指针，所以对这个status的更新也会直接反映到vao相应的status上去
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

    glBindBuffer(target, buffer);
}

void d_glDeleteProgram_origin(void *context, GLuint program)
{
    glDeleteProgram(program);
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

void d_glLinkProgram_origin(void *context, GLuint program)
{
    glLinkProgram(program);
}

void d_glPixelStorei_origin(void *context, GLenum pname, GLint param)
{

    Pixel_Store_Status *pixel_store = &(((Opengl_Context *)context)->pixel_store_status);
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

    glPixelStorei(pname, param);
}

void d_glBindVertexArray_origin(void *context, GLuint array)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    Buffer_Status *vao_status = g_hash_table_lookup(bound_buffer->vao_status, array);

    bound_buffer->buffer_status=vao_status;
    

    glBindVertexArray(array);
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

void d_glGetVertexAttribIiv_origin(GLuint index, GLenum pname, GLint *params)
{
    glGetVertexAttribIiv(index, pname, params);
}

void d_glGetVertexAttribIuiv_origin(GLuint index, GLenum pname, GLuint *params)
{
    glGetVertexAttribIuiv(index, pname, params);
}
void d_glGetVertexAttribfv_origin(GLuint index, GLenum pname, GLfloat *params)
{
    glGetVertexAttribfv(index, pname, params);
}
void d_glGetVertexAttribiv_origin(GLuint index, GLenum pname, GLint *params)
{
    glGetVertexAttribiv(index, pname, params);
}

void d_glDisableVertexAttribArray_origin(void *context, GLuint index)
{
    glDisableVertexAttribArray(index);
}
void d_glEnableVertexAttribArray_origin(void *context, GLuint index)
{
    glEnableVertexAttribArray(index);
}

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
    
    if(data==NULL){
        glBufferData(target, size, NULL, usage);
        return;
    }

    //todo 测试到底是直接bufferdata快，还是用map后复制快，这里两个都实现下(但是subdata实现的还不完全)，根据数据大小决定采用哪种方式
    Guest_Mem *guest_mem=(Guest_Mem *)data;
    Scatter_Data *s_data=guest_mem->scatter_data;
    if(size==s_data[0].len){
        //size等于第一个scatter的len，说明大小较小，可以直接data过去
        glBufferData(target, size, s_data[0].data, usage);

    }else{
        //先分配足够大的空间，然后用映射内存的方式来进行写入
        glBufferData(target, size, NULL, usage);
        GLubyte *map_pointer=glMapBufferRange(target,0,size,GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT);
        host_guest_buffer_exchange(s_data,map_pointer,0,size,1);
        glUnmapBuffer(target);
    }
}
void glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{
    if(data==NULL){
        glBufferSubData(target, offset, size, NULL);
        return;
    }

    //todo 测试到底是直接bufferdata快，还是用map后复制快，这里两个都实现下(但是subdata实现的还不完全)，根据数据大小决定采用哪种方式
    Guest_Mem *guest_mem=(Guest_Mem *)data;
    Scatter_Data *s_data=guest_mem->scatter_data;

    if(size==s_data[0].len){
        //size等于第一个scatter的len，说明大小较小，可以直接data过去
        glBufferData(target,offset, size, s_data[0].data);

    }else{
        //注意，此处可能会引起隐式同步
        GLubyte *map_pointer=glMapBufferRange(target,offset,size,GL_MAP_WRITE_BIT);
        host_guest_buffer_exchange(s_data,map_pointer,0,size,1);
        glUnmapBuffer(target);
    }
}

void d_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint *buffers)
{
    glDeleteBuffers(n, buffers);
}

void d_glDrawElements_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices)
{
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,1);


    glDrawElements(mode, count,type,indices);
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

void d_glDrawElementsInstanced_with_bound(void *context, GLenum mode, GLsizei count, GLenum type, GLsizeiptr indices, GLsizei instancecount){
    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);
    Buffer_Status *status = bound_buffer->buffer_status;
   
    set_attrib_point(context,instancecount);


    glDrawElementsInstanced(mode, count,type,indices,instancecount);
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


    glDrawRangeElements(mode, start, end, count, type, indices);
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

    if(len==s_data[0].len){
        glDrawRangeElements(mode, start, end, count, type, s_data[0].data);
    }else{
        set_attrib_point_index(context,s_data,len);
        glDrawRangeElements(mode, start, end, count, type, s_data[0].data);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    
    if (status->array_buffer == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

}




void d_glTexImage2D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
    if(pixels==NULL){
        glTexImage2D(target,level,internalformat,width,height,border,format,type,NULL);
    }
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem=(Guest_Mem *)pixels;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc=0,end_loc=0;
    gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    prepare_unpack_texture(context,s_data,start_loc,end_loc);

    //这时候是立即返回的，后续会进行dma传输
    glTexImage2D( target, level, internalformat, width, height, border, format, type,  0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);

}

void d_glTexImage2D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLintptr pixels)
{
    glTexImage2D(target,level,internalformat,width,height,border,format,type,pixels);
}

void d_glTexSubImage2D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
    if(pixels==NULL){
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format,  type,  NULL);
    }
    Guest_Mem *guest_mem=(Guest_Mem *)pixels;
    Scatter_Data *s_data=guest_mem->scatter_data;


    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc=0,end_loc=0;
    gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    prepare_unpack_texture(context,s_data,start_loc,end_loc);

    //这时候是立即返回的，后续会进行dma传输
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format,  type,  0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}

void d_glTexSubImage2D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format,  type, pixels);
}

void d_glTexImage3D_without_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
    if(pixels==NULL){
        glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, NULL);

    }
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem=(Guest_Mem *)pixels;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc=0,end_loc=0;
    gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    prepare_unpack_texture(context,s_data,start_loc,end_loc);

    //这时候是立即返回的，后续会进行dma传输
    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, 0);


    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);



}

void d_glTexImage3D_with_bound(void *context, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLintptr pixels){
    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
}
void d_glTexSubImage3D_without_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels){

    if(pixels==NULL){
        glTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, NULL);
    }
    //没有绑定时，正好可以使用异步纹理传输
    Guest_Mem *guest_mem=(Guest_Mem *)pixels;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc=0,end_loc=0;
    gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    prepare_unpack_texture(context,s_data,start_loc,end_loc);

    //这时候是立即返回的，后续会进行dma传输
    glTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, 0);


    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);


}

void d_glTexSubImage3D_with_bound(void *context, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLintptr pixels){
    glTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}



void d_glCompressedTexImage3D_without_bound(void *context,GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data){
    
    Guest_Mem *guest_mem=(Guest_Mem *)data;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context,s_data,0,imageSize);
    glCompressedTexImage3D(target,level, internalformat, width, height, depth, border, imageSize, data);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}

void d_glCompressedTexImage3D_with_bound(void *context,GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLintptr data){

    glCompressedTexImage3D(target,level, internalformat, width, height, depth, border, imageSize, data);
}

void d_glCompressedTexSubImage3D_without_bound(void *context,GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data){
    Guest_Mem *guest_mem=(Guest_Mem *)data;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context,s_data,0,imageSize);
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}

void d_glCompressedTexSubImage3D_with_bound(void *context,GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data){
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}



void d_glCompressedTexImage2D_without_bound(void *context,GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data){
    Guest_Mem *guest_mem=(Guest_Mem *)data;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context,s_data,0,imageSize);
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}

void d_glCompressedTexImage2D_with_bound(void *context,GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data){
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

void d_glCompressedTexSubImage2D_without_bound(void *context,GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data){
    Guest_Mem *guest_mem=(Guest_Mem *)data;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);
    prepare_unpack_texture(context,s_data,0,imageSize);
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
}


void d_glCompressedTexSubImage2D_with_bound(void *context,GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data){
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}




void d_glReadPixels_without_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels){
    //由于没有绑定的情况还是需要读取完数据后再进行复制，所以这里实际上无法做到异步
    Guest_Mem *guest_mem=(Guest_Mem *)pixels;
    Scatter_Data *s_data=guest_mem->scatter_data;

    Pixel_Store_Status *status=&(((Opengl_Context *)context)->pixel_store_status);

    int start_loc=0,end_loc=0;
    gl_pixel_data_loc(status,width,height,format,type,0,&start_loc,&end_loc);

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    GLint asyn_texture=bound_buffer->asyn_pack_texture_buffer;
    glBindBuffer(GL_PIXEL_PACK_BUFFER,asyn_texture);
    //因为曾经bind过texture，所以这里直接bind相应的buffer，这里重新bufferdata是为了孤立缓冲区
    glBufferData(GL_PIXEL_PACK_BUFFER, end_loc,NULL,GL_STREAM_DRAW);
    glReadPixels(x,y,width,height,format,type,0);
    
    //注意，此句会阻塞，直到pixels全部下载下来
    GLubyte *map_pointer=glMapBufferRange(GL_PIXEL_UNPACK_BUFFER,start_loc,end_loc-start_loc,GL_MAP_READ_BIT);
    host_guest_buffer_exchange(s_data,map_pointer,start_loc,end_loc-start_loc,0);

    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
}

void d_glReadPixels_with_bound(void *context, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLintptr pixels)
{
    glReadPixels(x, y, width, height, format, type, pixels);
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
        Buffer_Status *vao_status = g_hash_table_lookup(bound_buffer->vao_status, vao_index);
        Attrib_Point *vao_point = g_hash_table_lookup(bound_buffer->vao_point_data, vao_index);

        if(vao_status==bound_buffer->buffer_status){
            bound_buffer->buffer_status=g_hash_table_lookup(bound_buffer->vao_status, 0);
        }

        GLint buffer_index[2];
        buffer_index[0]=vao_point->indices_buffer_object;
        buffer_index[1]=vao_point->buffer_object;

        glDeleteBuffers(2,buffer_index);
        g_free(vao_point);
        g_free(vao_status);
        g_hash_table_remove(bound_buffer->vao_status, vao_index);
        g_hash_table_remove(bound_buffer->vao_point_data, vao_index);

    }


    glDeleteVertexArrays(n, arrays);
}



void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{


    d_glMapBufferRange_write(context, target, offset, length, access, mem_buf);

    //写入的情况需要把map里的数据读取到缓冲区里
    if (access & GL_MAP_READ_BIT)
    {
        GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
        Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map,target);
        host_guest_buffer_exchange(map_res->guest_data,map_res->host_data,0,length,0);
    }

}

void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, const void *mem_buf){

    //注意，写入类型的map调用在guest端是异步的，但是这个函数运行时是同步的，而且同一个渲染线程执行起来肯定是串行的
    //所以不会出现这个map被阻塞住了，还没获得host端指针，然后guest端调用flush向这个指针写入数据的情况

    //把数据写入到guest中
    Guest_Mem *guest_mem=(Guest_Mem *)mem_buf;
    Scatter_Data *s_data=guest_mem->scatter_data;

    GLubyte *map_pointer=glMapBufferRange(target,offset,length,access);

    //然后保存下这个map结果
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map,target);
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
    }else{
        //todo 可能之前map过，然后切换绑定对象了，这个时候会发生什么需要测试
        //todo 假如维持绑定的过程中出现了对象被删除或者调用了glBufferData时，会自动取消映射，这个需要特殊处理，现阶段先假定都是正常unmap的
    }

    map_res->access=access;
    map_res->host_data=map_pointer;
    map_res->map_len=length;
    map_res->target=target;


    if(map_res->guest_data!=NULL){
        g_free(map_res->guest_data);
        // map_res->guest_data=NULL;
    }
    map_res->guest_data =g_malloc(guest_mem->num*sizeof(Scatter_Data));
    memcpy(map_res->guest_data,s_data,guest_mem->num*sizeof(Scatter_Data));

}

GLboolean d_glUnmapBuffer_special(void *context, GLenum target)
{
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map,target);
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
        return GL_FALSE;
    }
    if (map_res->access & GL_MAP_WRITE_BIT){

        if((map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT)==0){
            //flush模式的unmap直接unmap就行了
            //其他情况下的write的unmap，需要先进行数据交换
            host_guest_buffer_exchange(map_res->guest_data,map_res->host_data,0,map_res->map_len,1);
            
        }
    }

    GLboolean ret=glUnmapBuffer(target);
    if(map_res->guest_data!=NULL){
        g_free(map_res->guest_data);
        memset(map_res, 0, sizeof(Guest_Host_Map));
    }
    //注意：unmap后不删除hash表中保存的map_res是因为这个还会被复用，因为其键是target
    return ret;
}


void d_glFlushMappedBufferRange_origin(void *context, GLenum target, GLintptr offset, GLsizeiptr length){
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map,target);
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
        return;
    }
    if (map_res->access & GL_MAP_WRITE_BIT){

        if((map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT)){
            //flush模式的unmap直接unmap就行了
            //其他情况下的write的unmap，需要先进行数据交换
            host_guest_buffer_exchange(map_res->guest_data,map_res->host_data+offset,offset,length,1);
            glFlushMappedBufferRange(target,offset,length);
        }
    }

}




// glTestIntAsyn GLint a, GLuint b, GLfloat c, GLdouble d

// glPrintfAsyn GLint a, GLuint size, GLdouble c, const GLchar *out_string#strlen(out_string)+1

// GLint glTestInt1 GLint a, GLuint b
// GLuint glTestInt2 GLint a, GLuint b

// GLint64 glTestInt3 GLint64 a, GLuint64 b
// GLuint64 glTestInt4 GLint64 a, GLuint64 b

// GLfloat glTestInt5 GLint a, GLuint b
// GLdouble glTestInt6 GLint a, GLuint b

// glTestPointer1 GLint a, const GLint *b#sizeof(GLint)*10

// glTestPointer2 GLint a, const GLint *b#sizeof(GLint)*10, GLint *c#sizeof(GLint)*10

// GLint glTestPointer3 GLint a, const GLint *b#sizeof(GLint)*10, GLint *c#sizeof(GLint)*10

// GLint glTestPointer4 GLint a, const GLint *b#sizeof(GLint)*1000, GLint *c#sizeof(GLint)*1000


// glTestString GLint a, GLint count, const GLchar *const*strings#count|strlen(strings[i])+1, GLint buf_len, GLchar *char_buf#buf_len

// glPrintf GLchar *out_string#strlen(out_string)+1


void glTestIntAsyn(GLint a, GLuint b, GLfloat c, GLdouble d){
    printf("glTestInt asyn %d,%u,%f,%lf\n",a,b,c,d);
    fflush(stdout);
}

void glPrintfAsyn(GLint a, GLuint size, GLdouble c, const GLchar *out_string){
    printf("glTestInt asyn string %d,%u,%lf,%s\n",a,size,c,out_string);
}

GLint glTestInt1(GLint a, GLuint b){
    printf("glTestInt1 %d,%u\n",a,b);
    fflush(stdout);
    return 576634565;
}
GLuint glTestInt2(GLint a, GLuint b){
    printf("glTestInt2 %d,%u\n",a,b);
    fflush(stdout);
    return 4000001200u;
}

GLint64 glTestInt3(GLint64 a, GLuint64 b){
printf("glTestInt3 %lld,%llu\n",a,b);
fflush(stdout);
    return 453489431344456;

}
GLuint64 glTestInt4(GLint64 a, GLuint64 b){
    printf("glTestInt4 %lld,%llu\n",a,b);
    fflush(stdout);
    return 436004354364364345;

}

GLfloat glTestInt5(GLint a, GLuint b){
     printf("glTestInt5 %d,%u\n",a,b);
     fflush(stdout);
    return 3.1415926;
}
GLdouble glTestInt6(GLint a, GLuint b){
     printf("glTestInt6 %d,%u\n",a,b);
     fflush(stdout);
    return 3.1415926535;
}

void glTestPointer1(GLint a, const GLint *b){
    printf("glTestPointer1 %d ",a);
    for(int i=0;i<10;i++){
        printf("%d ",b[i]);
    }
    printf("\n");
    fflush(stdout);
    return;
}

void glTestPointer2(GLint a, const GLint *b, GLint *c){
    printf("glTestPointer2 %d %d\n",a,*b);
    for(int i=0;i<10;i++){
        c[i]=b[i];
    }
    fflush(stdout);
    return;
}

GLint glTestPointer3(GLint a, const GLint *b, GLint *c){
    printf("glTestPointer3 %d\n",a);
    for(int i=b[0];i<a;i++){
        c[i]=b[i];
    }
    fflush(stdout);
    return 12456687;
}

GLint glTestPointer4(GLint a, const GLint *b, GLint *c){
     printf("glTestPointer4 %d,%d\n",a,*b);
    for(int i=0;i<1000;i++){
        c[i]=b[i];
    }
    fflush(stdout);
    return 12456687;
}


void glTestString(GLint a, GLint count, const GLchar *const*strings, GLint buf_len, GLchar *char_buf){
    printf("glTestString %d %d %d\nString:\n",a,count,buf_len);
    for(int i=0;i<count;i++){
        printf("%s\n",strings[i]);
    }
    char *t="printf ok!";
    memcpy(char_buf,t,strlen(t));
    fflush(stdout);
}

void glPrintf(GLint buf_len, GLchar *out_string){
    char *t="temp test abcd";
    memcpy(out_string,t,strlen(t));
    printf("glPrintf %d\n",buf_len);
    fflush(stdout);
}

// glInOutTest GLint a, GLint b, const GLchar *e#strlen(e), GLint *c#sizeof(GLint), GLdouble *d#sizeof(GLdouble), GLsizei buf_len, GLchar *f#buf_len

// glSaveLongTime GLuint a, GLdouble b, const void *pointer#a

void glInOutTest(GLint a, GLint b, const GLchar *e, GLint *c, GLdouble *d, GLsizei buf_len, GLchar *f){
    printf("glInOutTest %d,%d,%s   buf_len%llu\n",a,b,e,buf_len);
    *c=78646313;
    *d=3.141592653543;
    
    char *t="glInOutTest printf ok! test ok!";
    memcpy(f,t,strlen(t));
    fflush(stdout);
}

void *pointer_test=NULL;

void glSaveLongTime(GLuint a, GLdouble b, const void *pointer){

    if(b>2.0){
        if(pointer_test==NULL){
            pointer_test=pointer;
        }else{
            g_free(pointer_test);

        }
        pointer_test=pointer;
    }

    char temp[100];
    guest_write(pointer_test,temp,0,100);
    printf("glSaveLongTime %s\n",temp);

}