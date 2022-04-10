// #define STD_DEBUG_LOG
// #define TIMER_LOG
#include "express-gpu/glv3_context.h"


#include "glad/glad.h"
#include "express-gpu/egl_window.h"


//下面这三个函数都是销毁函数，不提供外部调用，只用来给g_hash_table_new_full用
static void g_buffer_map_destroy(gpointer data);

// static void g_vao_status_destroy(gpointer data);

static void g_vao_point_data_destroy(gpointer data);

GHashTable *program_is_external_map = NULL;

GHashTable *program_data_map = NULL;

GHashTable *to_external_texture_id_map = NULL;


static volatile GList *native_context_pool = NULL;
static int native_context_pool_size;
static int native_context_pool_locker = 0;



static void g_program_data_destroy(gpointer data)
{
    GLchar *program_data = (GLchar *)data;
    g_free(program_data);
}


void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{
    GLuint draw_fbo0 = ((Opengl_Context *)context)->draw_fbo0;
    GLuint read_fbo0 = ((Opengl_Context *)context)->read_fbo0;

    // glFlush();
    // glFinish();
    if (framebuffer == 0)
    {
        if (target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            // printf("conetxt %llx bind 0 framebuffer draw %u\n",context, draw_fbo0);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo0);
        }
        if (target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {
            // printf("conetxt %llx bind 0 framebuffer read %u\n",context, read_fbo0);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo0);
        }
    }
    else
    {
        glBindFramebuffer(target, framebuffer);
        // GLenum status = glCheckFramebufferStatus(target) ;
        // printf("conetxt %llx bind framebuffer %u status %x\n",context, framebuffer, status);
    }

    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, real_opengl_context->draw_fbo0);
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, real_opengl_context->read_fbo0);
}

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);
    
    switch (target)
    {
    case GL_ARRAY_BUFFER:
        status->array_buffer = buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        status->element_array_buffer = buffer;
        opengl_context->bound_buffer_status.attrib_point->element_array_buffer = buffer;
        break;
    case GL_COPY_READ_BUFFER:
        status->copy_read_buffer = buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        status->copy_write_buffer = buffer;
        break;
    case GL_PIXEL_PACK_BUFFER:
        status->pixel_pack_buffer = buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        status->pixel_unpack_buffer = buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->transform_feedback_buffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        status->uniform_buffer = buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->atomic_counter_buffer = buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        status->dispatch_indirect_buffer = buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        status->draw_indirect_buffer = buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->shader_storage_buffer = buffer;

    }

    // printf("context %llx glBindBuffer target %x buffer %d pre_buffer %d\n",context, target, buffer, pre_buffer);
    glBindBuffer(target, buffer);
}

int init_program_data(GLuint program)
{
    GLint link_status = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    int buf_len;
    GLchar *program_data;

    if (link_status == 0)
    {
        return 0;
    }
    else
    {
        //当前着色器定义的uniform常量和attrib变量的数目

        GLint uniform_num = 0;
        GLint attrib_num = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_num);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrib_num);

        //获得这些变量或者常量的名字的最大长度

        GLint max_uniform_name_len = 0;
        GLint max_attrib_name_len = 0;
        GLint max_uniform_block_name_len = 0;

        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_len);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_name_len);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_uniform_block_name_len);

        //获取transform_feedback_varyings和active_uniform_blocks，有另外两个函数的指针要根据这个确定大小
        GLint uniform_blocks_num = 0;
        GLint transform_feedback_varyings = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &uniform_blocks_num);
        glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_VARYINGS, &transform_feedback_varyings);

        int name_len = max_uniform_name_len > max_attrib_name_len ? max_uniform_name_len : max_attrib_name_len;
        name_len = name_len > max_uniform_block_name_len ? name_len : max_uniform_block_name_len;

        name_len += 1;

        GLchar *name_buf = g_malloc(name_len);

        buf_len = (name_len + 3 * 4) * (uniform_num + attrib_num + uniform_blocks_num) + 4 * 7 + name_len;
        program_data = g_malloc(buf_len);

        GLchar *temp_ptr = program_data;
        int *int_ptr = (int *)temp_ptr;

        *int_ptr = uniform_num;
        *(int_ptr + 1) = attrib_num;
        *(int_ptr + 2) = uniform_blocks_num;
        *(int_ptr + 3) = max_uniform_name_len;
        *(int_ptr + 4) = max_attrib_name_len;
        *(int_ptr + 5) = max_uniform_block_name_len;
        *(int_ptr + 6) = transform_feedback_varyings;

        temp_ptr += 7 * sizeof(int);

        GLint size;
        GLenum type;
        GLint location;
        int has_image = 0;
        //获得每一个uniform的相关信息
        for (int i = 0; i < uniform_num; i++)
        {
            glGetActiveUniform(program, i, name_len, NULL, &size, &type, name_buf);

            location = glGetUniformLocation(program, name_buf);

            if (strstr(name_buf, "has_EGL_image_external") != NULL)
            {
                if (program_is_external_map == NULL)
                {
                    program_is_external_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
                }
                g_hash_table_insert(program_is_external_map, program, (gpointer)1);
                has_image = 1;
                continue;
            }

            int_ptr = temp_ptr;

            *int_ptr = i - has_image;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = type;
            *(int_ptr + 3) = location;

            temp_ptr += 4 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;

            // printf("uniform |%d %d| |%s|\n", location, type, name_buf);
        }

        for (int i = 0; i < attrib_num; i++)
        {
            glGetActiveAttrib(program, i, name_len, NULL, &size, &type, name_buf);

            location = glGetAttribLocation(program, name_buf);

            int_ptr = temp_ptr;

            *int_ptr = i;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = type;
            *(int_ptr + 3) = location;

            temp_ptr += 4 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;

            // printf("attrib |%d %d| |%s|\n", location, type, name_buf);
        }

        int uniform_block_active_uniforms;
        for (int i = 0; i < uniform_blocks_num; i++)
        {
            int_ptr = temp_ptr;
            glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniform_block_active_uniforms);
            glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
            glGetActiveUniformBlockName(program, i, name_len, NULL, name_buf);

            *int_ptr = i;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = uniform_block_active_uniforms;

            temp_ptr += 3 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;
            // printf("uniform block |%d %d| |%s|\n", uniform_block_active_uniforms, size, name_buf);
        }

        if (has_image)
        {
            *((int *)program_data) = uniform_num - 1;
        }

        if (program_data_map == NULL)
        {
            program_data_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_program_data_destroy);
        }

        g_hash_table_insert(program_data_map, GUINT_TO_POINTER(program), program_data);

        if (buf_len > temp_ptr - program_data + 10)
        {
            buf_len = temp_ptr - program_data + 10;
        }
        g_free(name_buf);

        return buf_len;
    }
}

void d_glProgramBinary_special(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length, int *program_data_len)
{

    glProgramBinary(program, binaryFormat, binary, length);

    *program_data_len = init_program_data(program);
    return;
}

void d_glLinkProgram_special(void *context, GLuint program, int *program_data_len)
{

    glLinkProgram(program);

    *program_data_len = init_program_data(program);
    return;
}

void d_glGetProgramData(void *context, GLuint program, int buf_len, void *program_data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)program_data;

    if (program_data_map == NULL || program == 0)
    {

        printf("error! program_data_map %llx program %d", program_data_map, program);
        return;
    }

    GLchar *save_program_data = g_hash_table_lookup(program_data_map, GUINT_TO_POINTER(program));

    if (save_program_data == NULL)
    {
        printf("error! save_program_data NULL program %u", program);
        return;
    }

    guest_read(guest_mem, save_program_data, 0, buf_len);

    //读取完成后直接删除就行了
    g_hash_table_remove(program_data_map, GUINT_TO_POINTER(program));

    return;
}

void get_default_out(char *string, char *out)
{
    out[0] = 0;
    int out_loc = 0;
    char *out_name = NULL;
    char *out_type = NULL;
    while (*string != ';')
        string++;
    string--;
    while (isspace(*string))
        string--;
    while (!isspace(*string))
        string--;

    out_name = string + 1;
    for (int i = 0; out_name[i] != ';' && !isspace(out_name[i]) && out_name[i] != 0; i++)
    {
        out[out_loc] = out_name[i];
        out_loc++;
    }
    out[out_loc] = '=';
    out_loc++;

    while (isspace(*string))
        string--;

    while (!isspace(*string))
        string--;
    out_type = string + 1;

    if (out_type[3] == '4')
    {
        strcpy(out + out_loc, "vec4(0,0,0,0);return;}");
    }
    else if (out_type[3] == '3')
    {
        strcpy(out + out_loc, "vec3(0,0,0);return;}");
    }
    else
    {
        strcpy(out + out_loc, "0;return;}");
    }
}

void d_glShaderSource_special(void *context, GLuint shader, GLsizei count, GLint *length, const GLchar **string)
{
    static const char DEFAULT_VERSION[] = "#version 330\n";
    static const char SHADOW_SAMPLER_EXTENSION[] = "#extension GL_NV_shadow_samplers_cube : enable\n";
    static const char USE_EXTERNAL_UNIFORM[] = "if(has_EGL_image_external==1){";

    // printf("gl shader source %d before count%d:\n%s\n",shader, count, string[0]);

    int has_find_external = 0;
    int has_version = 0;
    int has_texturecube = 0;
    char *new_string1 = NULL;
    char *new_string2 = NULL;

    for (int i = 0; i < count; i++)
    {
        char *string_loc = strstr(string[i], "has_EGL_image_external");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_find_external = 1;
        }
        string_loc = strstr(string[i], "#version");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_version = 1;
        }
        string_loc = strstr(string[i], "textureCube");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_texturecube = 1;
        }
        string_loc = strstr(string[i], "gl_FragDepthEXT");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc = strstr(string[i], "gl_FragDepthEXT");
        }

        string_loc = strstr(string[i], "textureCubeLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[7] = '(';
            string_loc[8] = ' ';
            string_loc[9] = ' ';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc[16] = ' ';
            string_loc[17] = ' ';
            string_loc = strstr(string[i], "textureCubeLodEXT(");
        }

        string_loc = strstr(string[i], "texture1DLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[9] = '(';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc = strstr(string[i], "texture1DLodEXT(");
        }
        string_loc = strstr(string[i], "texture2DLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[9] = '(';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc = strstr(string[i], "texture2DLodEXT(");
        }
        string_loc = strstr(string[i], "texture2DProjLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[13] = '(';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc[16] = ' ';
            string_loc[17] = ' ';
            string_loc[18] = ' ';
            string_loc[19] = ' ';
            string_loc = strstr(string[i], "texture2DProjLodEXT(");
        }
        string_loc = strstr(string[i], "texture3DLodEXT(");
        while (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[9] = '(';
            string_loc[10] = ' ';
            string_loc[11] = ' ';
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
            string_loc[15] = ' ';
            string_loc = strstr(string[i], "texture3DLodEXT(");
        }
    }

    if (!has_version || has_texturecube)
    {
        new_string1 = g_malloc(length[0] + sizeof(DEFAULT_VERSION) + sizeof(SHADOW_SAMPLER_EXTENSION));
        int loc = 0;
        int origin_loc = 0;
        if(!has_version)
        {
            memcpy(new_string1, DEFAULT_VERSION, sizeof(DEFAULT_VERSION) - 1);
            loc += sizeof(DEFAULT_VERSION) - 1;
        }
        else
        {
            for(int i = 0;i < length[0]; i++)
            {
                new_string1[i]=string[0][i];
                loc+=1;
                if(new_string1[i]=='\n')
                {
                    break;
                }
            }
            origin_loc = loc;
        }
        if (has_texturecube)
        {
            memcpy(new_string1 + loc, SHADOW_SAMPLER_EXTENSION, sizeof(SHADOW_SAMPLER_EXTENSION) - 1);
            loc += sizeof(SHADOW_SAMPLER_EXTENSION) - 1;
        }
        memcpy(new_string1 + loc, string[0] + origin_loc, length[0] - origin_loc);
        loc += length[0] - origin_loc;
        new_string1[loc] = 0;
        length[0] = loc;
        string[0] = new_string1;
    }

    GLint shader_type;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);

    if (has_find_external == 1 && shader_type == GL_FRAGMENT_SHADER)
    {
        char *out_loc = NULL;
        for (int i = 0; i < count; i++)
        {
            out_loc = strstr(string[i], "out ");
            if (out_loc != NULL && (unsigned long long)(out_loc - string[i]) <= (unsigned long long)length[i])
            {
                if (isspace(*(out_loc - 1)))
                {
                    break;
                }
            }
        }
        if (out_loc == NULL)
        {
            out_loc = "out vec4 gl_FragColor;";
        }
        char default_out_string[200];

        get_default_out(out_loc, default_out_string);

        for (int i = 0; i < count; i++)
        {
            char *string_loc = strstr(string[i], "main(void)");
            if (string_loc == NULL)
            {
                string_loc = strstr(string[i], "main()");
            }

            if (string_loc != NULL && (unsigned long long)(string_loc - string[i]) <= (unsigned long long)length[i])
            {
                // printf("find main\n");
                while (string_loc[0] != '{' && (unsigned long long)(string_loc - string[i]) <= (unsigned long long)length[i])
                {
                    string_loc++;
                }
                string_loc++;
                if (string_loc - string[i] > length[i])
                {
                    break;
                }
                has_find_external = 1;
                new_string2 = g_malloc(length[i] + sizeof(USE_EXTERNAL_UNIFORM) - 1 + strlen(default_out_string));
                int offset = 0;
                memcpy(new_string2 + offset, string[i], string_loc - string[i]);
                offset += (string_loc - string[i]);
                memcpy(new_string2 + offset, USE_EXTERNAL_UNIFORM, sizeof(USE_EXTERNAL_UNIFORM) - 1);
                offset += sizeof(USE_EXTERNAL_UNIFORM) - 1;
                memcpy(new_string2 + offset, default_out_string, strlen(default_out_string));
                offset += strlen(default_out_string);
                memcpy(new_string2 + offset, string_loc, length[i] - (string_loc - string[i]));
                offset += length[i] - (string_loc - string[i]);

                length[i] = offset;
                string[i] = new_string2;
                // printf("shadersource:\n%s\n", string[i]);
            }
        }
    }

    glShaderSource(shader, count, string, length);
    // printf("gl shader source after count %d context %llx:\n%s\n", count, context, string[0]);

    if (new_string1 != NULL)
    {
        g_free(new_string1);
    }
    if (new_string2 != NULL)
    {
        g_free(new_string2);
    }
}

void d_glGetString_special(void *context, GLenum name, GLubyte *buffer)
{
    const GLubyte *static_string = glGetString(name);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetString string too long %x %s", name, static_string);
    }
    memcpy(buffer, static_string, len);
    //#1024
}

void d_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte *buffer)
{
    const GLubyte *static_string = glGetStringi(name, index);
    express_printf("getStringi index %u:%s\n", index, static_string);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetStringi string too long %x %u %s", name, index, static_string);
    }
    memcpy(buffer, static_string, len);
}

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    Opengl_Context *real_opengl_context = (Opengl_Context *)context;
    real_opengl_context->view_x = 0;
    real_opengl_context->view_y = 0;
    real_opengl_context->view_w = width;
    real_opengl_context->view_h = height;
    glViewport(x, y, width, height);
    return;
}

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image)
{
    //不会调用到host端来
    return;
}

void d_glUseProgram_special(void *context, GLuint program)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    int ret = 0;
    if (program_is_external_map != NULL)
    {
        ret = g_hash_table_lookup(program_is_external_map, GUINT_TO_POINTER(program));
    }

    if (ret == 1 && opengl_context->current_target == GL_TEXTURE_2D)
    {
        //当前需要使用external纹理

        for (int i = 0; i < preload_static_context_value->max_combined_texture_image_units; i++)
        {
            if (opengl_context->current_texture_external != 0)
            {
                GLuint texture = g_hash_table_lookup(to_external_texture_id_map, (gpointer)(opengl_context->current_texture_external));
                if (texture != 0)
                {
                    if(opengl_context->current_active_texture != 0)
                    {
                        glActiveTexture(GL_TEXTURE0);
                    }
                    glBindTexture(GL_TEXTURE_2D, texture);
                    opengl_context->current_target = GL_TEXTURE_EXTERNAL_OES;
                    
                    if(opengl_context->current_active_texture != 0)
                    {
                        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
                    }

                    // int now_active;
                    // glGetIntegerv(GL_ACTIVE_TEXTURE,&now_active);

                    // printf("context %llx program %u change to external texture %u i %d current %u real current %d\n", opengl_context, program, texture, i,opengl_context->current_texture_external,now_active-GL_TEXTURE0);
                    break;
                }
            }
        }
    }
    if (ret == 0 && opengl_context->current_target == GL_TEXTURE_EXTERNAL_OES)
    {
        // printf("context %llx change to normal texture %u\n", opengl_context, opengl_context->current_texture_2D[opengl_context->current_active_texture]);
        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[opengl_context->current_active_texture]);
        opengl_context->current_target = GL_TEXTURE_2D;
    }
    // printf("context %llx use program %u external active %d target %x\n", opengl_context, program,opengl_context->current_active_texture, opengl_context->current_target);

    glUseProgram(program);

    
}

void d_glBindEGLImage(void *context, GLenum target, GLeglImageOES image)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    uint64_t gbuffer_id = (uint64_t)image;
    Window_Buffer *real_surface = get_surface_from_gbuffer_id(gbuffer_id);
    EGL_Image *egl_image = get_image_from_gbuffer_id(gbuffer_id);
    // printf("#%llx glBindEGLImage %x image %llx real_surface %llx egl_image %llx now acquire %d\n", context, target, image, real_surface, egl_image, real_surface == NULL ? -1 : real_surface->now_acquired);
    // if(real_surface != NULL)
    // {
    //     printf("get gbuffer_id %llx surface %llx\n",gbuffer_id, real_surface);
    // }

    if (real_surface != NULL && egl_image != NULL)
    {
        printf("error! real_surface %llx and egl_image %llx are not NULL!",real_surface, egl_image);
    }

    if (real_surface == NULL && egl_image == NULL)
    {
        printf("error! real_surface and egl_image are all NULL! gbuffer_id %llx\n",gbuffer_id);
    }

    switch (target)
    {
    case GL_READ_ONLY:
    {
        if (real_surface != NULL)
        {
            acquire_texture_from_surface(real_surface);
            glBindTexture(GL_TEXTURE_2D, real_surface->fbo_texture[real_surface->now_acquired]);

            if (opengl_context->current_texture_external != 0)
            {
                if (to_external_texture_id_map == NULL)
                {
                    to_external_texture_id_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
                }
                g_hash_table_insert(to_external_texture_id_map, opengl_context->current_texture_external, GUINT_TO_POINTER(real_surface->fbo_texture[real_surface->now_acquired]));
            }
        }
        if (egl_image != NULL)
        {
            init_image_texture(egl_image);

            acquire_texture_from_image(egl_image);
            opengl_context->bind_image = egl_image;

            if (opengl_context->current_texture_external != 0)
            {
                if (to_external_texture_id_map == NULL)
                {
                    to_external_texture_id_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
                }
                g_hash_table_insert(to_external_texture_id_map, opengl_context->current_texture_external, GUINT_TO_POINTER(egl_image->fbo_texture));
            }
            // printf("eglimage bind texture %u\n",egl_image->fbo_texture);
            // if(!glIsTexture(egl_image->fbo_texture))
            // {
            //     printf("eglimage gbuffer_id %llx fbo_texture %u is delete!\n",egl_image->gbuffer_id, egl_image->fbo_texture);
            // }
            glBindTexture(GL_TEXTURE_2D, egl_image->fbo_texture);
        }
        break;
    }
    case GL_WRITE_ONLY:
    {
        if (real_surface != NULL)
        {
            glBindTexture(GL_TEXTURE_2D, real_surface->fbo_texture[real_surface->now_acquired]);
            printf("error! Surface is writen by image!");
        }

        if (egl_image != NULL && egl_image->target != EGL_GL_TEXTURE_2D)
        {
            if (opengl_context->draw_surface != NULL && opengl_context->draw_surface->I_am_composer == 0)
            {
                egl_image->need_reverse = 1;
            }

            egl_image->host_has_data = 1;
            init_image_fbo(egl_image, egl_image->need_reverse);

            glBindFramebuffer(GL_FRAMEBUFFER, egl_image->display_fbo);
        }
        break;
    }
    case GL_SYNC_FLUSH_COMMANDS_BIT:
    {
        if (real_surface != NULL)
        {
            release_texture_from_surface(real_surface);
        }
        if (egl_image != NULL)
        {
            release_texture_from_image(egl_image);
            opengl_context->bind_image = NULL;
        }
        break;
    }
    case GL_NONE:
    {
        if (egl_image != NULL)
        {
            opengl_context->bind_image = NULL;
            ATOMIC_SET_USED(egl_image->display_texture_is_use);
        }
    }
    default:
    {

        break;
    }
    }
    if(gbuffer_id == NULL)
    {
        printf("error\n");
    }
    release_surface(real_surface);
    return;
}

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{
    //当前google没实现，所以暂时先不管
}

void resource_context_init(Resource_Context *resources, Share_Resources *share_resources)
{
    if (share_resources != NULL)
    {
        resources->share_resources = share_resources;
        resources->share_resources->counter += 1;
    }
    else
    {
        resources->share_resources = g_malloc(sizeof(Share_Resources));
        memset(resources->share_resources, 0, sizeof(Share_Resources));
        resources->share_resources->counter = 1;
    }

    resources->exclusive_resources = g_malloc(sizeof(Exclusive_Resources));
    memset(resources->exclusive_resources, 0, sizeof(Exclusive_Resources));

    resources->texture_resource = &(resources->share_resources->texture_resource);
    resources->buffer_resource = &(resources->share_resources->buffer_resource);
    resources->render_buffer_resource = &(resources->share_resources->render_buffer_resource);
    resources->sampler_resource = &(resources->share_resources->sample_resource);

    resources->shader_resource = &(resources->share_resources->shader_resource);
    resources->program_resource = &(resources->share_resources->program_resource);

    resources->sync_resource = &(resources->share_resources->sync_resource);

    resources->frame_buffer_resource = &(resources->exclusive_resources->frame_buffer_resource);
    resources->program_pipeline_resource = &(resources->exclusive_resources->program_pipeline_resource);
    resources->transform_feedback_resource = &(resources->exclusive_resources->transform_feedback_resource);
    resources->vertex_array_resource = &(resources->exclusive_resources->vertex_array_resource);

    resources->query_resource = &(resources->exclusive_resources->query_resource);
}

#define DESTROY_RESOURCES(resource_name, resource_delete)                                              \
    if (resources->resource_name->resource_id_map != NULL)                                             \
    {                                                                                                  \
        for (int i = 1; i <= resources->resource_name->max_id; i++)                                    \
        {                                                                                              \
            if (resources->resource_name->resource_id_map[i] == 0)                                     \
                continue;                                                                              \
            if (now_delete_len < 1000)                                                                 \
            {                                                                                          \
                delete_buffers[now_delete_len] = (GLuint)resources->resource_name->resource_id_map[i]; \
                now_delete_len += 1;                                                                   \
            }                                                                                          \
            else                                                                                       \
            {                                                                                          \
                resource_delete(now_delete_len, delete_buffers);                                       \
                now_delete_len = 0;                                                                    \
            }                                                                                          \
        }                                                                                              \
        if (now_delete_len != 0)                                                                       \
        {                                                                                              \
            resource_delete(now_delete_len, delete_buffers);                                           \
            now_delete_len = 0;                                                                        \
        }                                                                                              \
        g_free(resources->resource_name->resource_id_map);                                             \
    }

void resource_context_destroy(Resource_Context *resources)
{
    GLuint delete_buffers[1000];
    GLuint now_delete_len = 0;

    resources->share_resources->counter -= 1;
    if (resources->share_resources->counter == 0)
    {
        DESTROY_RESOURCES(texture_resource, glDeleteTextures);
        DESTROY_RESOURCES(buffer_resource, glDeleteBuffers);
        DESTROY_RESOURCES(render_buffer_resource, glDeleteRenderbuffers);
        DESTROY_RESOURCES(sampler_resource, glDeleteSamplers);

        if (resources->shader_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->shader_resource->max_id; i++)
            {
                if (resources->shader_resource->resource_id_map[i] != 0)
                {
                    glDeleteShader((GLuint)resources->shader_resource->resource_id_map[i]);
                }
            }
            g_free(resources->shader_resource->resource_id_map);
        }

        if (resources->program_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->program_resource->max_id; i++)
            {
                if (resources->program_resource->resource_id_map[i] != 0)
                {
                    if (program_is_external_map != NULL)
                    {
                        g_hash_table_remove(program_is_external_map, GUINT_TO_POINTER((GLuint)resources->program_resource->resource_id_map[i]));
                    }
                    if (program_data_map != NULL)
                    {
                        g_hash_table_remove(program_data_map, GUINT_TO_POINTER((GLuint)resources->program_resource->resource_id_map[i]));
                    }
                    glDeleteProgram((GLuint)resources->program_resource->resource_id_map[i]);
                }
            }
            g_free(resources->program_resource->resource_id_map);
        }

        if (resources->sync_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->sync_resource->max_id; i++)
            {
                if (resources->sync_resource->resource_id_map[i] != 0)
                {
                    glDeleteSync((GLsync)resources->sync_resource->resource_id_map[i]);
                }
            }
            g_free(resources->sync_resource->resource_id_map);
        }

        g_free(resources->share_resources);
    }

    //下面这些资源不是共享资源，在windows删除后就会释放，但是目前不一定删除windows，所以需要注意，还是要删除
    DESTROY_RESOURCES(frame_buffer_resource, glDeleteFramebuffers);
    DESTROY_RESOURCES(program_pipeline_resource, glDeleteProgramPipelines);
    DESTROY_RESOURCES(transform_feedback_resource, glDeleteTransformFeedbacks);
    DESTROY_RESOURCES(vertex_array_resource, glDeleteVertexArrays);

    DESTROY_RESOURCES(query_resource, glDeleteQueries);

    // g_free(resources->frame_buffer_resource->resource_id_map);
    // g_free(resources->program_pipeline_resource->resource_id_map);
    // g_free(resources->transform_feedback_resource->resource_id_map);
    // g_free(resources->vertex_array_resource->resource_id_map);
    // g_free(resources->query_resource->resource_id_map);

    g_free(resources->exclusive_resources);
}

void *get_native_opengl_context()
{
    void *native_context = NULL;

    ATOMIC_LOCK(native_context_pool_locker);
    GList *first= g_list_first(native_context_pool);
    ATOMIC_UNLOCK(native_context_pool_locker);
    if(first == NULL)
    {
        // #ifdef USE_GLFW_AS_WGL
        send_message_to_main_window(MAIN_CREATE_CHILD_WINDOW, &native_context);
        // #else
        // 不能在子线程中创建context，不然会为空
        //     opengl_context->window = egl_createContext();
        // #endif

        //假如guest一创建context就立马销毁，发送到主线程的事件就会写入到释放后的内存上，所以这里进行等待，等待有context
            //等待window真正的建立起来
        int sleep_cnt = 0;
        while (native_context == NULL)
        {
            g_usleep(1000);
            sleep_cnt += 1;
            if (sleep_cnt >= 100 && sleep_cnt % 500 == 0)
            {
                printf("wait for window creating too long! ptr %llx\n", &native_context);
            }
        }
    }
    else
    {
        native_context = first->data;
        ATOMIC_LOCK(native_context_pool_locker);
        native_context_pool = g_list_remove(native_context_pool, native_context);
        native_context_pool_size--;
        ATOMIC_UNLOCK(native_context_pool_locker);
    }
    return native_context;
}



void release_native_opengl_context(void *native_context)
{
    //假如已经保存有闲置的超过10个context，则新释放的context直接销毁，否则保存下来
    if(native_context_pool_size < 10)
    {
        ATOMIC_LOCK(native_context_pool_locker);
        native_context_pool = g_list_append(native_context_pool, native_context);
        native_context_pool_size++;
        ATOMIC_UNLOCK(native_context_pool_locker);
    }
    else
    {

#ifdef USE_GLFW_AS_WGL
    glfwSetWindowShouldClose(native_context, 1);
    glfwDestroyWindow(native_context);
#else
    egl_destroyContext(native_context);
#endif
    }

}


Opengl_Context *opengl_context_create(Opengl_Context *share_context)
{
    Opengl_Context *opengl_context = g_malloc(sizeof(Opengl_Context));
    opengl_context->is_current = 0;
    opengl_context->need_destroy = 0;
    opengl_context->window = NULL;

    opengl_context->bind_image = NULL;

    opengl_context->current_texture_2D = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(opengl_context->current_texture_2D, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    opengl_context->current_texture_external = 0;

    opengl_context->current_target = GL_TEXTURE_2D;
    opengl_context->current_active_texture = 0;

    opengl_context->view_x = 0;
    opengl_context->view_y = 0;
    opengl_context->view_w = 0;
    opengl_context->view_h = 0;

    //要在opengl_context里创建window，因为opengl环境保存在window里
// #ifdef USE_GLFW_AS_WGL
    // send_message_to_main_window(MAIN_CREATE_CHILD_WINDOW, &(opengl_context->window));
    opengl_context->window = get_native_opengl_context();
// #else
// 不能在子线程中创建context，不然会为空
//     opengl_context->window = egl_createContext();
// #endif
    printf("send message create window opengl context %llx window_ptr %llx\n", opengl_context, &(opengl_context->window));

    Share_Resources *share_resources = NULL;
    if (share_context != NULL)
    {
        share_resources = share_context->resource_status.share_resources;
    }

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    // opengl_context->pixel_store_status.pack_alignment=4;
    // opengl_context->pixel_store_status.unpack_alignment=4;

    opengl_context->buffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_buffer_map_destroy);

    // bound_buffer->vao_status=g_hash_table_new_full(g_direct_hash, g_direct_equal,NULL,g_vao_status_destroy);
    bound_buffer->vao_point_data = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_vao_point_data_destroy);

    // bound_buffer->buffer_type=g_hash_table_new(g_direct_hash, g_direct_equal);

    // Buffer_Status *status=g_malloc(sizeof(Buffer_Status));
    // memset(status,0,sizeof(Buffer_Status));
    // g_hash_table_insert(bound_buffer->vao_status, GUINT_TO_POINTER(0), (gpointer)status);

    Attrib_Point *temp_point = g_malloc(sizeof(Attrib_Point));
    memset(temp_point, 0, sizeof(Attrib_Point));

    g_hash_table_insert(bound_buffer->vao_point_data, GUINT_TO_POINTER(0), (gpointer)temp_point);

    // bound_buffer->buffer_status=status;
    bound_buffer->attrib_point = temp_point;

    resource_context_init(&(opengl_context->resource_status), share_resources);

    bound_buffer->asyn_unpack_texture_buffer = 0;
    bound_buffer->asyn_pack_texture_buffer = 0;

    bound_buffer->has_init = 0;

    opengl_context->draw_fbo0 = 0;
    opengl_context->read_fbo0 = 0;

    return opengl_context;
}

void opengl_context_init(Opengl_Context *context)
{
    //初始化opengl_context的一些资源，因为这个时候已经makecurrent了
    Bound_Buffer *bound_buffer = &(context->bound_buffer_status);
    if (bound_buffer->has_init == 0)
    {
        //这个has_init也指opengl_context是否已经初始化
        bound_buffer->has_init = 1;
        glGenBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glGenBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

        glGenBuffers(1, &(bound_buffer->attrib_point->indices_buffer_object));
        glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, bound_buffer->attrib_point->buffer_object);

        //这两个选项在gles中是默认开启，这样能够在着色器中获取到一些内建变量，所以在gl中要手动开启
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        //原窗口大小是1*1，所以默认的viewport也是1*1，所以在初始化的时候要手动设置下viewport
        glViewport(context->view_x, context->view_y, context->view_w, context->view_h);
    }
}

/**
 * @brief 销毁opengl_context函数，主要为销毁资源，真正的opengl context可能会缓存
 * 因为销毁的时候可能没有makecurrent了，所以得先makecurrent以调用opengl函数
 * 
 * @param context 
 */
void opengl_context_destroy(Opengl_Context *context)
{
    express_printf("opengl context destroy %lx\n", context);
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);


#ifdef USE_GLFW_AS_WGL
    // printf("make current context %llx windows %llx\n",opengl_context,opengl_context->window);
    glfwMakeContextCurrent((GLFWwindow *)opengl_context->window);
#else
    egl_makeCurrent(opengl_context->window);
#endif


    //这三个remove后都有默认的销毁函数
    // g_hash_table_remove_all(opengl_context->buffer_map);
    g_hash_table_destroy(opengl_context->buffer_map);
    // g_hash_table_destroy(bound_buffer->vao_status);
    g_hash_table_destroy(bound_buffer->vao_point_data);

    if (bound_buffer->has_init == 1)
    {
        glDeleteBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glDeleteBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));
    }

    resource_context_destroy(&(opengl_context->resource_status));
#ifdef USE_GLFW_AS_WGL
#ifdef DEBUG_INDEPEND_WINDOW
    glfwHideWindow(opengl_context->window);
#endif
    glfwMakeContextCurrent(NULL);
#else
    egl_makeCurrent(NULL);
#endif
    release_native_opengl_context(opengl_context->window);
}

//下面这三个函数都是销毁函数，不提供外部调用，只用来给g_hash_table_new_full用
static void g_buffer_map_destroy(gpointer data)
{
    express_printf("buffer_map destroy\n");
    Guest_Host_Map *map_res = (Guest_Host_Map *)data;
    g_free(map_res);
}

// static void g_vao_status_destroy(gpointer data)
// {
//     express_printf("vao_status destroy\n");

//     Buffer_Status *vao_status = (Buffer_Status *)data;
//     g_free(vao_status);
// }

static void g_vao_point_data_destroy(gpointer data)
{
    Attrib_Point *vao_point = (Attrib_Point *)data;
    if (vao_point->indices_buffer_object != 0)
    {
        glDeleteBuffers(1, &(vao_point->indices_buffer_object));
        glDeleteBuffers(MAX_VERTEX_ATTRIBS_NUM, vao_point->buffer_object);
    }

    express_printf("vao_point destroy\n");

    g_free(vao_point);
}

