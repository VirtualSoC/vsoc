#include "express-gpu/glv3_mem.h"

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{

    

    //todo 测试到底是直接bufferdata快，还是用map后复制快，这里两个都实现下(但是subdata实现的还不完全)，根据数据大小决定采用哪种方式
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        glBufferData(target, size, NULL, usage);
        return;
    }

    if (size == s_data[0].len)
    {
        //size等于第一个scatter的len，说明大小较小，可以直接data过去
        glBufferData(target, size, s_data[0].data, usage);
    }
    else
    {
        //先分配足够大的空间，然后用映射内存的方式来进行写入
        glBufferData(target, size, NULL, usage);
        GLubyte *map_pointer = glMapBufferRange(target, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);
        glUnmapBuffer(target);
    }
}
void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{
    

    //todo 测试到底是直接bufferdata快，还是用map后复制快，这里两个都实现下(但是subdata实现的还不完全)，根据数据大小决定采用哪种方式
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;

    if (guest_mem->all_len == 0)
    {
        glBufferSubData(target, offset, size, NULL);
        return;
    }

    if (size == s_data[0].len)
    {
        //size等于第一个scatter的len，说明大小较小，可以直接data过去
        glBufferSubData(target, offset, size, s_data[0].data);
    }
    else
    {
        //注意，此处可能会引起隐式同步
        GLubyte *map_pointer = glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT);
        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);
        glUnmapBuffer(target);
    }
}

void d_glDeleteBuffers_origin(void *context, GLsizei n, const GLuint *buffers)
{
    glDeleteBuffers(n, buffers);
}

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{

    char *int_data=g_malloc(sizeof(GLenum)+sizeof(GLintptr)+sizeof(GLsizeiptr)+sizeof(GLbitfield));
    size_t int_data_loc=0;
    memcpy(int_data+int_data_loc,&target,sizeof(GLenum));
    int_data_loc+=sizeof(GLenum);
    memcpy(int_data+int_data_loc,&offset,sizeof(GLintptr));
    int_data_loc+=sizeof(GLintptr);
    memcpy(int_data+int_data_loc,&length,sizeof(GLsizeiptr));
    int_data_loc+=sizeof(GLsizeiptr);
    memcpy(int_data+int_data_loc,&access,sizeof(GLbitfield));
    int_data_loc+=sizeof(GLbitfield);

    d_glMapBufferRange_write(context, (const void *)int_data, mem_buf);

    g_free(int_data);
    //写入的情况需要把map里的数据读取到缓冲区里
    if (access & GL_MAP_READ_BIT)
    {
        GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
        Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, GINT_TO_POINTER(target));
        host_guest_buffer_exchange(map_res->guest_data, map_res->host_data, 0, length, 0);
    }
}

void d_glMapBufferRange_write(void *context, const void *int_data, const void *mem_buf)
{

    //注意，写入类型的map调用在guest端是异步的，但是这个函数本身运行时是同步的，而且同一个渲染线程执行起来肯定是串行的
    //所以不会出现这个map被阻塞住了，还没获得host端指针，然后guest端调用flush向这个指针写入数据的情况

    GLenum target;
    GLintptr offset;
    GLsizeiptr length;
    GLbitfield access;

    Guest_Mem *guest_mem_int=(Guest_Mem *)int_data;

    size_t start_loc=0;
    guest_write(guest_mem_int,&target,start_loc,sizeof(GLenum));
    start_loc+=sizeof(GLenum);
    guest_write(guest_mem_int,&offset,start_loc,sizeof(GLintptr));
    start_loc+=sizeof(GLintptr);
    guest_write(guest_mem_int,&length,start_loc,sizeof(GLsizeiptr));
    start_loc+=sizeof(GLsizeiptr);
    guest_write(guest_mem_int,&access,start_loc,sizeof(GLbitfield));


    
    Guest_Mem *guest_mem = (Guest_Mem *)mem_buf;
    Scatter_Data *s_data = guest_mem->scatter_data;

    GLubyte *map_pointer = glMapBufferRange(target, offset, length, access);

    //然后保存下这个map结果
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, GINT_TO_POINTER(target));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
    }
    else
    {
        //todo 可能之前map过，然后切换绑定对象了，这个时候会发生什么需要测试
        //todo 假如维持绑定的过程中出现了对象被删除或者调用了glBufferData时，会自动取消映射，这个需要特殊处理，现阶段先假定都是正常unmap的
    }

    map_res->access = access;
    map_res->host_data = map_pointer;
    map_res->map_len = length;
    map_res->target = target;

    if (map_res->guest_data != NULL)
    {
        g_free(map_res->guest_data);
        // map_res->guest_data=NULL;
    }
    map_res->guest_data = g_malloc(guest_mem->num * sizeof(Scatter_Data));
    memcpy(map_res->guest_data, s_data, guest_mem->num * sizeof(Scatter_Data));
}

GLboolean d_glUnmapBuffer_special(void *context, GLenum target)
{
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, GINT_TO_POINTER(target));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
        return GL_FALSE;
    }
    if (map_res->access & GL_MAP_WRITE_BIT)
    {

        if ((map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT) == 0)
        {
            //flush模式的unmap直接unmap就行了
            //其他情况下的write的unmap，需要先进行数据交换
            host_guest_buffer_exchange(map_res->guest_data, map_res->host_data, 0, map_res->map_len, 1);
        }
    }

    GLboolean ret = glUnmapBuffer(target);
    map_res->host_data=NULL;
    if (map_res->guest_data != NULL)
    {
        g_free(map_res->guest_data);
        memset(map_res, 0, sizeof(Guest_Host_Map));
    }
    //注意：unmap后不删除hash表中保存的map_res是因为这个还会被复用，因为其键是target
    return ret;
}

void d_glFlushMappedBufferRange_origin(void *context, GLenum target, GLintptr offset, GLsizeiptr length)
{
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, GINT_TO_POINTER(target));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
        return;
    }
    if (map_res->access & GL_MAP_WRITE_BIT)
    {

        if ((map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT))
        {
            //flush模式的unmap直接unmap就行了
            //其他情况下的write的unmap，需要先进行数据交换
            host_guest_buffer_exchange(map_res->guest_data, map_res->host_data + offset, offset, length, 1);
            glFlushMappedBufferRange(target, offset, length);
        }
    }
}
