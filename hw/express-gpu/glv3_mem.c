// #define STD_DEBUG_LOG

#include "hw/express-gpu/glv3_mem.h"

GLuint get_guest_buffer_binding_id(void *context, GLenum target);

GLuint get_guest_buffer_binding_id(void *context, GLenum target)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        return status->guest_array_buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        return status->guest_element_array_buffer;
        break;
    case GL_COPY_READ_BUFFER:
        return status->guest_copy_read_buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        return status->guest_copy_write_buffer;
        break;
    case GL_PIXEL_PACK_BUFFER:
        return status->guest_pixel_pack_buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        return status->guest_pixel_unpack_buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return status->guest_transform_feedback_buffer;
        break;
    case GL_UNIFORM_BUFFER:
        return status->guest_uniform_buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        return status->guest_atomic_counter_buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return status->guest_dispatch_indirect_buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        return status->guest_draw_indirect_buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        return status->guest_shader_storage_buffer;
    default:
        LOGI("get_guest_buffer_binding_id error target %x", target);
        return 0;
    }
    return 0;
}

void d_glBufferData_custom(void *context, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{

    // todo 测试到底是直接bufferdata快，还是用map后复制快，这里两个都实现下(但是subdata实现的还不完全)，根据数据大小决定采用哪种方式
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;
    GLuint bind_buffer = get_guest_binding_buffer(context, target);

    express_printf("%llx %s target %x bind_buffer %d size %lld usage %x real size %d\n", (uint64_t)context, __FUNCTION__, target, bind_buffer, size, usage, guest_mem->all_len);

    if (size == 0)
    {
        return;
    }

    if (guest_mem->all_len == 0)
    {
        express_printf("glBufferData null\n");
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glNamedBufferData(bind_buffer, size, NULL, usage);
        }
        else
        {
            glBufferData(target, size, NULL, usage);
        }
        return;
    }

    if (size == s_data[0].len)
    {
        // size等于第一个scatter的len，说明大小较小，可以直接data过去
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glNamedBufferData(bind_buffer, size, s_data[0].data, usage);
        }
        else
        {
            glBufferData(target, size, s_data[0].data, usage);
        }
    }
    else
    {
        //先分配足够大的空间，然后用映射内存的方式来进行写入
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glNamedBufferData(bind_buffer, size, NULL, usage);
        }
        else
        {
            glBufferData(target, size, NULL, usage);
        }

        GLubyte *map_pointer = NULL;
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            map_pointer = glMapNamedBufferRange(bind_buffer, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }
        else
        {
            map_pointer = glMapBufferRange(target, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }
        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);

        express_printf("glBufferData indirect %d\n", size);

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glUnmapNamedBuffer(bind_buffer);
        }
        else
        {
            glUnmapBuffer(target);
        }
    }
}
void d_glBufferSubData_custom(void *context, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{

    // todo 测试到底是直接bufferdata快，还是用map后复制快，这里两个都实现下(但是subdata实现的还不完全)，根据数据大小决定采用哪种方式
    Guest_Mem *guest_mem = (Guest_Mem *)data;
    Scatter_Data *s_data = guest_mem->scatter_data;
    GLuint bind_buffer = get_guest_binding_buffer(context, target);
    if (bind_buffer == 0)
    {
        LOGI("d_glBufferSubData_custom target %x", target);
    }

    if (guest_mem->all_len == 0)
    {
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glNamedBufferSubData(bind_buffer, offset, size, NULL);
        }
        else
        {
            glBufferSubData(target, offset, size, NULL);
        }
        return;
    }

    if (size == s_data[0].len)
    {
        // size等于第一个scatter的len，说明大小较小，可以直接data过去
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glNamedBufferSubData(bind_buffer, offset, size, s_data[0].data);
        }
        else
        {
            glBufferSubData(target, offset, size, s_data[0].data);
        }
    }
    else
    {
        // char *temp=g_malloc(size);
        // host_guest_buffer_exchange(s_data, temp, 0, size, 1);
        // glBufferSubData(target, offset, size, temp);

        // g_free(temp);

        // return;

        //注意，此处可能会引起隐式同步
        GLubyte *map_pointer = NULL;
        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            map_pointer = glMapNamedBufferRange(bind_buffer, offset, size, GL_MAP_WRITE_BIT);
        }
        else
        {
            map_pointer = glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT);
        }

        host_guest_buffer_exchange(s_data, map_pointer, 0, size, 1);

        if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
        {
            glUnmapNamedBuffer(bind_buffer);
        }
        else
        {
            glUnmapBuffer(target);
        }
    }

    // GLint error = glGetError();
    // if(error!=0)
    // {
    //     GLint all_size = 0;

    //     glGetNamedBufferParameteriv(bind_buffer, GL_BUFFER_SIZE, &all_size);
    //     LOGI("glBufferSubData context %llx error %x target %x buffer %d offset %d size %d all size %d", (uint64_t)context, error, target, bind_buffer, (int)offset, (int)size, (int)all_size);
    // }
}

void d_glMapBufferRange_read(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access, void *mem_buf)
{

    d_glMapBufferRange_write(context, target, offset, length, access);

    //写入的情况需要把map里的数据读取到缓冲区里
    if (access & GL_MAP_READ_BIT)
    {
        GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
        Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));
        write_to_guest_mem((Guest_Mem *)mem_buf, (void *)map_res->host_data, 0, length);
        // host_guest_buffer_exchange(map_res->guest_data, map_res->host_data, 0, length, 0);
    }
}

void d_glMapBufferRange_write(void *context, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{

    // GLint size = 0;

    // glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    // express_printf("mapbufferrange target %x offset %d length %d end %d buffer id %d buffer size %d access %x\n",(int)target,(int)offset,(int)length,(int)offset+(int)length,buffer,size,(int)access);

    GLuint bind_buffer = get_guest_binding_buffer(context, target);

    GLubyte *map_pointer = NULL;

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        map_pointer = glMapNamedBufferRange(bind_buffer, offset, length, access);
    }
    else
    {
        map_pointer = glMapBufferRange(target, offset, length, access);
    }

    //然后保存下这个map结果
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)), (gpointer)map_res);
    }
    else
    {
        //@todo 可能之前map过，然后切换绑定对象了，这个时候会发生什么需要测试
        //@todo 假如维持绑定的过程中出现了对象被删除或者调用了glBufferData时，会自动取消映射，这个需要特殊处理，现阶段先假定都是正常unmap的
        LOGE("error! map_res is not NULL!");
    }

    map_res->access = access;
    map_res->host_data = map_pointer;
    map_res->map_len = length;
    map_res->target = target;

    //注意，写入类型的map调用在guest端是异步的，但是这个函数本身运行时是同步的，而且同一个渲染线程执行起来肯定是串行的
    //所以不会出现这个map被阻塞住了，还没获得host端指针，然后guest端调用flush向这个指针写入数据的情况
}

GLboolean d_glUnmapBuffer_special(void *context, GLenum target)
{

    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));
    if (map_res == NULL)
    {
        LOGE("error! unmap get NULL map_res!");
        return GL_FALSE;
    }

    express_printf("unmap target %x\n", target);

    //这里不需要更新映射的这个缓冲区
    GLboolean ret = GL_TRUE;
    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        GLuint bind_buffer = get_guest_binding_buffer(context, target);
        ret = glUnmapNamedBuffer(bind_buffer);
    }
    else
    {
        ret = glUnmapBuffer(target);
    }

    g_hash_table_remove(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));

    return ret;
}

void d_glFlushMappedBufferRange_special(void *context, GLenum target, GLintptr offset, GLsizeiptr length, const void *data)
{
    GHashTable *buffer_map = ((Opengl_Context *)context)->buffer_map;
    Guest_Host_Map *map_res = g_hash_table_lookup(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)));

    if (map_res == NULL)
    {
        map_res = g_malloc(sizeof(Guest_Host_Map));
        memset(map_res, 0, sizeof(Guest_Host_Map));
        g_hash_table_insert(buffer_map, (gpointer)((((guint64)target) << 32) + get_guest_buffer_binding_id(context, target)), (gpointer)map_res);
        LOGE("error! flush data get NULL map_res!");
        return;
    }
    if (map_res->host_data == NULL)
    {
        LOGE("error! host data get NULL!");
        return;
    }
    if (map_res->access & GL_MAP_WRITE_BIT)
    {
        read_from_guest_mem((Guest_Mem *)data, map_res->host_data + offset, 0, length);

        uint32_t crc = 0;
        // for(int i=0;i<length;i++)
        // {
        //     crc = updateCRC32((map_res->host_data + offset)[i],crc);
        // }

        express_printf("flush mapbufferrange target %x offset %d length %d access %x crc %x\n", (int)target, (int)offset, (int)length, (int)map_res->access, crc);
        if ((map_res->access & GL_MAP_FLUSH_EXPLICIT_BIT))
        {
            if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
            {
                GLuint bind_buffer = get_guest_binding_buffer(context, target);
                glFlushMappedNamedBufferRange(bind_buffer, offset, length);
            }
            else
            {
                glFlushMappedBufferRange(target, offset, length);
            }
        }
    }
}
