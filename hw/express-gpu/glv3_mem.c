#define STD_DEBUG_LOG

#include "express-gpu/glv3_mem.h"

GLuint get_buffer_binding_id(void *context, GLenum target);

GLuint get_buffer_binding_id(void *context, GLenum target)
{
    // GLuint buffer_id;

    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

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
        status->atomic_counter_buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return status->dispatch_indirect_buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        return status->draw_indirect_buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        return status->shader_storage_buffer;
    default:
        return 0;
    }
    return 0;

    // switch(target)
    // {
    //     case GL_ARRAY_BUFFER:
    //         glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_ATOMIC_COUNTER_BUFFER:
    //         glGetIntegerv(GL_ATOMIC_COUNTER_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_COPY_READ_BUFFER:
    //         glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_COPY_WRITE_BUFFER:
    //         glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_DISPATCH_INDIRECT_BUFFER:
    //         glGetIntegerv(GL_DISPATCH_INDIRECT_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_DRAW_INDIRECT_BUFFER:
    //         glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_ELEMENT_ARRAY_BUFFER:
    //         glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_PIXEL_PACK_BUFFER:
    //         glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_PIXEL_UNPACK_BUFFER:
    //         glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_QUERY_BUFFER:
    //         glGetIntegerv(GL_QUERY_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_SHADER_STORAGE_BUFFER:
    //         glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_TEXTURE_BUFFER:
    //         glGetIntegerv(GL_TEXTURE_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_TRANSFORM_FEEDBACK_BUFFER:
    //         glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     case GL_UNIFORM_BUFFER:
    //         glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, (GLint *)&buffer_id);
    //         break;
    //     default:
    //         printf("error! get_buffer_binding_id target %x not found! ",(int)target);
    //         break;
    // }
    // return buffer_id;
}

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

        // express_printf("glBufferData %d:",size);
        // float *temp=(float *)s_data[0].data;
        // for(int i=0;i<size/4;i++){
        //     express_printf("%f ",temp[i]);
        // }
        // express_printf("\n");
    }
    else
    {
        //先分配足够大的空间，然后用映射内存的方式来进行写入
        glBufferData(target, size, NULL, usage);
        GLubyte *map_pointer = glMapBufferRange(target, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);

        // express_printf("glBufferData %d:",size);
        // float *temp=g_malloc(size);
        // host_guest_buffer_exchange(s_data, temp, 0, size, 1);
        // for(int i=0;i<size/4;i++){
        //     express_printf("%f ",temp[i]);
        // }
        // express_printf("\n");
        // glBufferData(target, size,temp, usage);

        // g_free(temp);

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

    d_glMapBufferRange_write(context, target, offset, length, access);

    //写入的情况需要把map里的数据读取到缓冲区里
    if (access & GL_MAP_READ_BIT)
    {
        GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
        Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
        guest_read((Guest_Mem *)mem_buf, (void *)map_res->host_data, 0, length);
        //host_guest_buffer_exchange(map_res->guest_data, map_res->host_data, 0, length, 0);
    }
}

void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    // if(target == GL_ARRAY_BUFFER)
    // {
    //     GLuint vbo;
    //     glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
    //     printf("map buffer vbo %u\n",vbo);
    // }
    // if(target == GL_ELEMENT_ARRAY_BUFFER)
    // {
    //     GLuint ebo;
    //     glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
    //     printf("map buffer ebo %u\n",ebo);
    // }

    // printf("mapbufferrange target %x offset %d length %d access %x\n",(int)target,(int)offset,(int)length,(int)access);
    GLubyte *map_pointer = glMapBufferRange(target, offset, length, access);

    //然后保存下这个map结果
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, GINT_TO_POINTER(target));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)), (gpointer)map_res);
    }
    else
    {
        //@todo 可能之前map过，然后切换绑定对象了，这个时候会发生什么需要测试
        //@todo 假如维持绑定的过程中出现了对象被删除或者调用了glBufferData时，会自动取消映射，这个需要特殊处理，现阶段先假定都是正常unmap的
    }

    map_res->access = access;
    map_res->host_data = map_pointer;
    map_res->map_len = length;
    map_res->target = target;

    //注意，写入类型的map调用在guest端是异步的，但是这个函数本身运行时是同步的，而且同一个渲染线程执行起来肯定是串行的
    //所以不会出现这个map被阻塞住了，还没获得host端指针，然后guest端调用flush向这个指针写入数据的情况

    // GLenum target;
    // GLintptr offset;
    // GLsizeiptr length;
    // GLbitfield access;

    // Guest_Mem *guest_mem_int=(Guest_Mem *)int_data;

    // size_t start_loc=0;
    // guest_write(guest_mem_int,&target,start_loc,sizeof(GLenum));
    // start_loc+=sizeof(GLenum);
    // guest_write(guest_mem_int,&offset,start_loc,sizeof(GLintptr));
    // start_loc+=sizeof(GLintptr);
    // guest_write(guest_mem_int,&length,start_loc,sizeof(GLsizeiptr));
    // start_loc+=sizeof(GLsizeiptr);
    // guest_write(guest_mem_int,&access,start_loc,sizeof(GLbitfield));

    // Guest_Mem *guest_mem = (Guest_Mem *)mem_buf;
    // Scatter_Data *s_data = guest_mem->scatter_data;

    // GLubyte *map_pointer = glMapBufferRange(target, offset, length, access);

    // //然后保存下这个map结果
    // GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    // Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, GINT_TO_POINTER(target));
    // if (map_res == NULL)
    // {
    //     map_res = g_malloc(sizeof(Guest_Host_Map));
    //     memset(map_res, 0, sizeof(Guest_Host_Map));
    //     g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
    // }
    // else
    // {
    //     //todo 可能之前map过，然后切换绑定对象了，这个时候会发生什么需要测试
    //     //todo 假如维持绑定的过程中出现了对象被删除或者调用了glBufferData时，会自动取消映射，这个需要特殊处理，现阶段先假定都是正常unmap的
    // }

    // map_res->access = access;
    // map_res->host_data = map_pointer;
    // map_res->map_len = length;
    // map_res->target = target;

    // if (map_res->guest_data != NULL)
    // {
    //     g_free(map_res->guest_data);
    //     // map_res->guest_data=NULL;
    // }
    // map_res->guest_data = g_malloc(guest_mem->num * sizeof(Scatter_Data));
    // memcpy(map_res->guest_data, s_data, guest_mem->num * sizeof(Scatter_Data));
}

GLboolean d_glUnmapBuffer_special(void *context, GLenum target)
{
    // if(target == GL_ARRAY_BUFFER)
    // {
    //     GLuint vbo;
    //     glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
    //     printf("unmap buffer vbo %u\n",vbo);
    // }
    // if(target == GL_ELEMENT_ARRAY_BUFFER)
    // {
    //     GLuint ebo;
    //     glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
    //     printf("unmap buffer ebo %u\n",ebo);
    // }
    // printf("unmap buffer %x\n",(int)target);
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {
        // map_res = g_malloc(sizeof(Guest_Host_Map));
        // memset(map_res, 0, sizeof(Guest_Host_Map));
        // g_hash_table_insert(buffer_map, GINT_TO_POINTER(target), (gpointer)map_res);
        return GL_FALSE;
    }

    //这里不需要更新映射的这个缓冲区
    GLboolean ret = glUnmapBuffer(target);
    g_hash_table_remove(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
    // memset(map_res, 0, sizeof(Guest_Host_Map));
    //注意：unmap后不删除hash表中保存的map_res是因为这个还会被复用，因为其键是target
    return ret;
}

void d_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data)
{
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_buffer_binding_id(context, target)), (gpointer)map_res);

        return;
    }
    if (map_res->host_data == NULL)
    {
        return;
    }
    if (map_res->access & GL_MAP_WRITE_BIT)
    {
        guest_write((Guest_Mem *)data, map_res->host_data + offset, 0, length);
        if ((map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT))
        {
            glFlushMappedBufferRange(target, offset, length);
        }
    }
}
