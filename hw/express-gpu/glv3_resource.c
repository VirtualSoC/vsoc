

#include "express-gpu/glv3_resource.h"

/**
 * @brief 创建一个host这端的id映射关系，映射关系为guest id到host id，方便查找真正的host id
 * 
 * @param status 
 * @param n 
 * @param guest_ids 
 * @param host_ids 
 * @return int 
 */
int create_host_map_ids(Resource_Map_Status *status, int n, const unsigned int *guest_ids, unsigned long long *host_ids)
{

    unsigned int max_id = 0;
    for (int i = 0; i < n; i++)
    {
        if (max_id < guest_ids[i])
        {
            max_id = guest_ids[i];
        }
    }

    //为了防止guest恶意申请id，导致qemu申请过大内存失败，进而在后面访问时crash
    if (max_id > (status->max_id + n) * 2 && max_id > 10000)
    {
        return 0;
    }

    //需要的内存空间过大时，进行重新申请
    if (status->map_size == 0 || max_id > status->map_size - 1)
    {
        int now_map_size = 2 * max_id + 1;
        unsigned long long *new_buf = g_malloc(now_map_size * sizeof(unsigned long long));
        if (status->resource_id_map != NULL)
        {

            memcpy(new_buf, status->resource_id_map, status->map_size * sizeof(unsigned long long));
            memset(new_buf + status->map_size, 0, (now_map_size - status->map_size) * sizeof(unsigned long long));
            g_free(status->resource_id_map);
        }
        else
        {
            memset(new_buf, 0, now_map_size * sizeof(unsigned long long));
        }
        status->resource_id_map = new_buf;
        status->map_size = now_map_size;
    }
    //这里才是构造映射关系，理论上这个map中的guest_id会是递增的，而不会跳跃
    for (int i = 0; i < n; i++)
    {
        if (guest_ids[i] == 0)
        {
            continue;
        }
        // printf("create texture id %d %d\n",(int)guest_ids[i],(int)host_ids[i]);
        status->resource_id_map[guest_ids[i]] = host_ids[i];
    }
    if (status->max_id < max_id)
    {
        status->max_id = max_id;
    }
}

/**
 * @brief 移除host这边的映射关系
 * 
 * @param status 
 * @param n 
 * @param guest_ids 
 */
void remove_host_map_ids(Resource_Map_Status *status, int n, const unsigned int *guest_ids)
{
    if (n <= 0)
    {
        return;
    }

    for (int i = 0; i < n; i++)
    {
        if (guest_ids[i] > status->max_id || status->max_id == 0 || guest_ids[i] == 0)
        {
            if (guest_ids[i] != 0)
            {
                printf("error remove! %u %u origin %llu\n", guest_ids[i], status->max_id, status->max_id != 0 ? status->resource_id_map[guest_ids[i]] : 0);
            }
            continue;
        }
        status->resource_id_map[guest_ids[i]] = 0;
    }
    while (status->max_id > 0 && status->resource_id_map[status->max_id] == 0)
    {
        status->max_id -= 1;
    }
    return;
}

void get_host_resource_ids(Resource_Map_Status *status, GLsizei n, const unsigned int *guest_ids, unsigned int *host_ids)
{
    for (int i = 0; i < n; i++)
    {
        if (guest_ids[i] > status->max_id || status->max_id == 0)
        {
            host_ids[i] = 0;
        }
        else
        {
            host_ids[i] = (unsigned int)status->resource_id_map[guest_ids[i]];
        }
    }
}

/**
 * @brief 用于decode调用后，第一时间根据guest id获取到host id
 * 
 * @param status 
 * @param id 
 * @return unsigned long long 
 */
unsigned long long get_host_resource_id(Resource_Map_Status *status, unsigned int id)
{
    if (id > status->max_id || status->max_id == 0)
    {
        return 0;
    }
    return status->resource_id_map[id];
}

int guest_has_resource_id(Resource_Map_Status *status, unsigned int id)
{
    if (id > status->max_id || status->max_id == 0 || status->resource_id_map[id] == 0)
    {
        return 0;
    }
    return 1;
}

unsigned long long get_host_texture_id(void *context, unsigned int id)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_buffer_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->buffer_resource;
    if (!guest_has_resource_id(map_status, id))
    {
        unsigned int host_id;
        glGenBuffers(1, &host_id);
        unsigned long long host_id_long = host_id;
        int ret = create_host_map_ids(map_status, 1, &id, &host_id_long);
        if (ret == 0)
        {
            return 0;
        }
        return host_id;
    }

    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_renderbuffer_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->render_buffer_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_sampler_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sampler_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_shader_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->shader_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_program_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_sync_id(void *context, unsigned int id)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sync_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_framebuffer_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->frame_buffer_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_pipeline_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_pipeline_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_feedback_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->transform_feedback_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_array_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->vertex_array_resource;
    return get_host_resource_id(map_status, id);
}

unsigned long long get_host_query_id(void *context, unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->query_resource;
    return get_host_resource_id(map_status, id);
}

void d_glGenBuffers(void *context, GLsizei n, const GLuint *buffers)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenBuffers(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->buffer_resource;

    create_host_map_ids(map_status, n, buffers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenRenderbuffers(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->render_buffer_resource;

    create_host_map_ids(map_status, n, renderbuffers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenTextures(void *context, GLsizei n, const GLuint *textures)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenTextures(n, host_buffers);
    // char temp[1000];
    // int loc=sprintf(temp,"#%llx gentexture %d ",context,(int)n);
    // for(int i = 0 ;i<n && loc<985;i++){
    //     loc+=sprintf(temp+loc,"host %u guest %u ",host_buffers[i],textures[i]);
    // }
    // printf("%s\n",temp);
    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;

    create_host_map_ids(map_status, n, textures, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    GLuint *host_buffers = g_malloc(count * sizeof(GLuint));
    glGenSamplers(count, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(count * sizeof(unsigned long long));
    for (int i = 0; i < count; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sampler_resource;

    create_host_map_ids(map_status, count, samplers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glCreateProgram(void *context, GLuint program)
{
    GLuint host_program = glCreateProgram();

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;

    unsigned long long host_program_long = (unsigned long long)host_program;

    create_host_map_ids(map_status, 1, &program, &host_program_long);
}

void d_glCreateShader(void *context, GLenum type, GLuint shader)
{
    GLuint host_shader = glCreateShader(type);

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->shader_resource;

    unsigned long long host_shader_long = (unsigned long long)host_shader;

    create_host_map_ids(map_status, 1, &shader, &host_shader_long);
}

void d_glFenceSync(void *context, GLenum condition, GLbitfield flags, GLsync sync)
{
    GLsync host_sync = glFenceSync(condition, flags);

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sync_resource;

    unsigned long long host_sync_long = (unsigned long long)host_sync;

    GLuint sync_int = (GLuint)sync;

    create_host_map_ids(map_status, 1, &sync_int, &host_sync_long);
}

void d_glCreateShaderProgramv_special(void *context, GLenum type, GLsizei count, const GLchar *const *strings, GLuint program, int *program_data_len)
{
    GLuint host_program = glCreateShaderProgramv(type, count, strings);

    if(host_program == 0)
    {
        *program_data_len = 0;
        return;
    }
    
    *program_data_len = init_program_data(host_program);


    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;

    unsigned long long host_program_long = (unsigned long long)host_program;

    create_host_map_ids(map_status, 1, &program, &host_program_long);
}

void d_glGenFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenFramebuffers(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->frame_buffer_resource;

    create_host_map_ids(map_status, n, framebuffers, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenProgramPipelines(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_pipeline_resource;

    create_host_map_ids(map_status, n, pipelines, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenTransformFeedbacks(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->transform_feedback_resource;

    create_host_map_ids(map_status, n, ids, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenVertexArrays(void *context, GLsizei n, const GLuint *arrays)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenVertexArrays(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->vertex_array_resource;

    create_host_map_ids(map_status, n, arrays, host_buffers_long);

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    // glGenVertexArrays(n, arrays);

    for (int i = 0; i < n; i++)
    {

        // Buffer_Status *status=g_malloc(sizeof(Buffer_Status));

        // memset(status,0,sizeof(Buffer_Status));
        // status->vertex_array_buffer=arrays[i];

        // g_hash_table_insert(bound_buffer->vao_status, GINT_TO_POINTER(arrays[i]), (gpointer)status);

        Attrib_Point *point_data = g_malloc(sizeof(Attrib_Point));
        memset(point_data, 0, sizeof(Attrib_Point));
        glGenBuffers(1, &(point_data->indices_buffer_object));
        glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, point_data->buffer_object);

        g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(host_buffers[i]), (gpointer)point_data);
    }

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glGenQueries(void *context, GLsizei n, const GLuint *ids)
{
    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    glGenQueries(n, host_buffers);

    unsigned long long *host_buffers_long = g_malloc(n * sizeof(unsigned long long));
    for (int i = 0; i < n; i++)
    {
        host_buffers_long[i] = (unsigned long long)host_buffers[i];
    }

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->query_resource;

    create_host_map_ids(map_status, n, ids, host_buffers_long);

    g_free(host_buffers);
    g_free(host_buffers_long);
}

void d_glDeleteBuffers(void *context, GLsizei n, const GLuint *buffers)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->buffer_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, buffers, host_buffers);
    glDeleteBuffers(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, buffers);
}

void d_glDeleteRenderbuffers(void *context, GLsizei n, const GLuint *renderbuffers)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->render_buffer_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, renderbuffers, host_buffers);
    glDeleteRenderbuffers(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, renderbuffers);
}

void d_glDeleteTextures(void *context, GLsizei n, const GLuint *textures)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->texture_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, textures, host_buffers);

    if(to_external_texture_id_map != NULL)
    {
        for(int i = 0;i<n;i++)
        {
            g_hash_table_remove(to_external_texture_id_map,GINT_TO_POINTER(host_buffers[i]));
        }
    }

    glDeleteTextures(n, host_buffers);
    g_free(host_buffers);

    // char temp[1000];
    // memset(temp,0,sizeof(temp));
    // int loc=sprintf(temp,"#%llx deletetexture %d ",context,(int)n);
    // for(int i = 0 ;i<n && loc<985;i++){
    //     loc+=sprintf(temp+loc,"host %u guest %u ",host_buffers[i],textures[i]);
    // }
    // printf("%s\n",temp);

    remove_host_map_ids(map_status, n, textures);
}

void d_glDeleteSamplers(void *context, GLsizei count, const GLuint *samplers)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sampler_resource;

    GLuint *host_buffers = g_malloc(count * sizeof(GLuint));
    get_host_resource_ids(map_status, count, samplers, host_buffers);
    glDeleteSamplers(count, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, count, samplers);
}

void d_glDeleteProgram(void *context, GLuint program)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_resource;

    GLuint host_program = (GLuint)get_host_resource_id(map_status, program);

    if(program_is_external_map != NULL)
    {
        g_hash_table_remove(program_is_external_map,GINT_TO_POINTER(host_program));
    }
    if(program_data_map != NULL)
    {
        g_hash_table_remove(program_data_map, GINT_TO_POINTER(host_program));
    }

    glDeleteProgram(host_program);

    remove_host_map_ids(map_status, 1, &program);
}

void d_glDeleteShader(void *context, GLuint shader)
{

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->shader_resource;

    GLuint host_shader = (GLuint)get_host_resource_id(map_status, shader);
    glDeleteShader(host_shader);

    remove_host_map_ids(map_status, 1, &shader);
}

void d_glDeleteSync(void *context, GLsync sync)
{

    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->sync_resource;

    unsigned int sync_int = (unsigned int)sync;

    GLsync host_sync = (GLsync)get_host_resource_id(map_status, sync_int);
    glDeleteSync(host_sync);

    remove_host_map_ids(map_status, 1, &sync_int);
}

void d_glDeleteFramebuffers(void *context, GLsizei n, const GLuint *framebuffers)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->frame_buffer_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, framebuffers, host_buffers);
    glDeleteFramebuffers(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, framebuffers);
}

void d_glDeleteProgramPipelines(void *context, GLsizei n, const GLuint *pipelines)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->program_pipeline_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, pipelines, host_buffers);
    glDeleteProgramPipelines(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, pipelines);
}

void d_glDeleteTransformFeedbacks(void *context, GLsizei n, const GLuint *ids)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->transform_feedback_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, ids, host_buffers);
    glDeleteTransformFeedbacks(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, ids);
}

void d_glDeleteVertexArrays(void *context, GLsizei n, const GLuint *arrays)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->vertex_array_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, arrays, host_buffers);
    glDeleteVertexArrays(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, arrays);

    Bound_Buffer *bound_buffer = &(((Opengl_Context *)context)->bound_buffer_status);

    for (int i = 0; i < n; i++)
    {
        GLuint vao_index = arrays[i];
        if (vao_index == 0)
        {
            continue;
        }
        //下面这个是为了当删除当前绑定的vao时，能够自动绑定vao为0
        Attrib_Point *vao_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(vao_index));

        // if(vao_status==bound_buffer->buffer_status){
        if (bound_buffer->attrib_point == vao_point)
        {
            // bound_buffer->buffer_status=g_hash_table_lookup(bound_buffer->vao_status, GINT_TO_POINTER(0));
            bound_buffer->attrib_point = g_hash_table_lookup(bound_buffer->vao_point_data, GINT_TO_POINTER(0));
        }
        g_hash_table_remove(bound_buffer->vao_point_data, GINT_TO_POINTER(vao_index));
    }
}

void d_glDeleteQueries(void *context, GLsizei n, const GLuint *ids)
{
    Resource_Context *resource_status = &(((Opengl_Context *)context)->resource_status);
    Resource_Map_Status *map_status = resource_status->query_resource;

    GLuint *host_buffers = g_malloc(n * sizeof(GLuint));
    get_host_resource_ids(map_status, n, ids, host_buffers);
    glDeleteQueries(n, host_buffers);
    g_free(host_buffers);

    remove_host_map_ids(map_status, n, ids);
}
