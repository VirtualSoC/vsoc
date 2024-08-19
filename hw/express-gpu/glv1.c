/**
 * @file glv1.c
 * @author gaodi
 * @brief glv1的固定管线函数
 * @version 0.1
 * @date 2021-08-31
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "hw/express-gpu/glv1.h"
#include "hw/express-gpu/glv3_status.h"

// static GLuint draw_texi_vao = 0;
static GLuint draw_texi_program = 0;
static GLint draw_texi_texture_id_loc = 0;

void d_glTexEnvf_special(void *context, GLenum target, GLenum pname, GLfloat param)
{
    glTexEnvf(target, pname, param);
}

void d_glTexEnvi_special(void *context, GLenum target, GLenum pname, GLint param)
{
    glTexEnvi(target, pname, param);
}

void d_glTexEnvx_special(void *context, GLenum target, GLenum pname, GLfixed param)
{
    express_printf("null glTexEnvxOES %llx %llx %llx pname %x param %x\n", (unsigned long long)glTexEnvxOES, (unsigned long long)glTexEnvi, (unsigned long long)glTexEnvf, pname, param);
    glTexEnvi(target, pname, param);
}

void d_glTexParameterx_special(void *context, GLenum target, GLenum pname, GLint param)
{

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        GLuint bind_texture = get_guest_binding_texture(context, target);
        glTextureParameteri(bind_texture, pname, param);
    }
    else
    {
        Opengl_Context *opengl_context = (Opengl_Context *)context;
        if (target == GL_TEXTURE_EXTERNAL_OES)
        {

            Texture_Binding_Status *texture_status = &(opengl_context->texture_binding_status);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
            }
            glBindTexture(GL_TEXTURE_2D, texture_status->current_texture_external);
            glTexParameterx(GL_TEXTURE_2D, pname, param);
            glBindTexture(GL_TEXTURE_2D, texture_status->host_current_texture_2D[0]);
            if (texture_status->host_current_active_texture != 0)
            {
                glActiveTexture(texture_status->host_current_active_texture + GL_TEXTURE0);
            }
        }
        else
        {
            glTexParameteri(target, pname, param);
        }
    }
}

void d_glShadeModel_special(void *context, GLenum mode)
{
    // glShadeModel(mode);
}

void d_glDrawTexiOES_special(void *context, GLint x, GLint y, GLint z, GLint width, GLint height, GLfloat left_x, GLfloat right_x, GLfloat bottom_y, GLfloat top_y)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint pre_vbo;
    GLuint pre_vao;

    float fz = z >= 1 ? 1.0f : z;
    fz = z <= 0 ? 0.0f : z;
    fz = fz * 2.0f - 1.0f;

    float positions_tex_coord[] = {
        1.0f, 1.0f, fz,    // top right
        1.0f, -1.0f, fz,   // bottom right
        -1.0f, -1.0f, fz,  // bottom left
        -1.0f, 1.0f, fz,   // top left
        right_x, top_y,    // top right
        right_x, bottom_y, // bottom right
        left_x, bottom_y,  // bottom left
        left_x, top_y,     // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    if (DSA_LIKELY(host_opengl_version >= 45 && DSA_enable != 0))
    {
        if (opengl_context->draw_texi_vao == 0)
        {
            glCreateVertexArrays(1, &(opengl_context->draw_texi_vao));

            glCreateBuffers(1, &(opengl_context->draw_texi_vbo));
            glCreateBuffers(1, &(opengl_context->draw_texi_ebo));
            glNamedBufferData(opengl_context->draw_texi_vbo, 20 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

            glNamedBufferData(opengl_context->draw_texi_ebo, 6 * sizeof(int), indices, GL_STATIC_DRAW);

            glEnableVertexArrayAttribEXT(opengl_context->draw_texi_vao, 0);
            glVertexArrayVertexAttribOffsetEXT(opengl_context->draw_texi_vao, opengl_context->draw_texi_vbo, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexArrayVertexAttribOffsetEXT(opengl_context->draw_texi_vao, opengl_context->draw_texi_vbo, 1, 2, GL_FLOAT, GL_FALSE, 0, 12 * sizeof(float));
            glEnableVertexArrayAttribEXT(opengl_context->draw_texi_vao, 1);
            // glVertexArrayAttribBinding(opengl_context->draw_texi_vao, 0, 0);
            // glVertexArrayAttribBinding(opengl_context->draw_texi_vao, 1, 1);

            glVertexArrayElementBuffer(opengl_context->draw_texi_vao, opengl_context->draw_texi_ebo);
        }

        express_printf("glv1 draw texture %d x %d y %d z %d width %d height %d left_x %f right_x %f bottom_y %f top_y %f\n", opengl_context->texture_binding_status.guest_current_texture_2D[opengl_context->texture_binding_status.guest_current_active_texture], x, y, z, width, height, left_x, right_x, bottom_y, top_y);

        Opengl_Context *opengl_context = (Opengl_Context *)context;
        Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);
        Buffer_Status *status = &(bound_buffer->buffer_status);

        glUseProgram(draw_texi_program);
        glUniform1i(draw_texi_texture_id_loc, 0);
        glViewport(x, y, width, height);

        if (status->host_vao != opengl_context->draw_texi_vao)
        {
            glBindVertexArray(opengl_context->draw_texi_vao);
            // LOGI("glv1 bind vao %d",opengl_context->draw_texi_vao);

            status->host_vao = opengl_context->draw_texi_vao;
            status->host_vao_ebo = opengl_context->draw_texi_ebo;

            status->host_element_array_buffer = opengl_context->draw_texi_ebo;
        }

        glNamedBufferSubData(opengl_context->draw_texi_vbo, (GLintptr)0, (GLsizeiptr)20 * sizeof(float), positions_tex_coord);

        texture_unit_status_sync(context, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

        glUseProgram(0);
    }
    else
    {

        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&pre_vbo);

        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint *)&pre_vao);

        glUseProgram(draw_texi_program);

        if (opengl_context->draw_texi_vao == 0)
        {
            glGenVertexArrays(1, &(opengl_context->draw_texi_vao));
            glBindVertexArray(opengl_context->draw_texi_vao);
            glGenBuffers(1, &(opengl_context->draw_texi_vbo));
            glGenBuffers(1, &(opengl_context->draw_texi_ebo));
            glBindBuffer(GL_ARRAY_BUFFER, opengl_context->draw_texi_vbo);
            glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_context->draw_texi_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        // LOGI("glv1 draw texture %d x %d y %d z %d width %d height %d left_x %f right_x %f bottom_y %f top_y %f",opengl_context->current_texture_2D[opengl_context->current_active_texture], x, y, z, width, height, left_x, right_x, bottom_y, top_y);

        GLint now_texture_target;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &now_texture_target);

        // GLuint now_bind_texture;
        // glGetIntegerv(GL_TEXTURE_BINDING_2D, &now_bind_texture);

        glUniform1i(draw_texi_texture_id_loc, now_texture_target - GL_TEXTURE0);
        glViewport(x, y, width, height);

        glBindVertexArray(opengl_context->draw_texi_vao);

        glBindBuffer(GL_ARRAY_BUFFER, opengl_context->draw_texi_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)0, (GLsizeiptr)20 * sizeof(float), positions_tex_coord);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

        glUseProgram(0);
        glBindVertexArray(pre_vao);
        glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    }
}

void prepare_draw_texi(void)
{
    if (draw_texi_program == 0)
    {
        //数组取地址不是字符串指针的指针，所以这里不要用数组
#ifdef _WIN32
        const char *vShaderCode = "#version 300 es\n"
#else
        const char *vShaderCode = "#version 330\n"
#endif
                                  "layout(location = 0) in vec3 a_pos;\n"
                                  "layout(location = 1) in vec2 atex_coord;\n"
                                  "out vec2 tex_coord;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_Position = vec4(a_pos, 1.0);\n"
                                  "    tex_coord = atex_coord;\n"
                                  "}\n";

#ifdef _WIN32
        const char *fShaderCode = "#version 300 es\n"
#else
        const char *fShaderCode = "#version 330\n"
#endif
                                  "precision mediump float;\n"
                                  "out vec4 frag_color;\n"
                                  "in vec2 tex_coord;\n"
                                  "uniform sampler2D texture_id;\n"
                                  "void main()\n"
                                  "{\n"
                                  "   frag_color = texture(texture_id, tex_coord);\n"
                                  "}\n";

        GLuint vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);

        // shader Program
        GLuint program_id = glCreateProgram();
        glAttachShader(program_id, vertex);
        glAttachShader(program_id, fragment);

        glLinkProgram(program_id);

        draw_texi_texture_id_loc = glGetUniformLocation(program_id, "texture_id");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        draw_texi_program = program_id;

        GLint linked;
        glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
        express_printf("linked %d program %u\n", linked, program_id);
        if (!linked)
        {
            GLint infoLen = 0;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
            LOGI("GL_INFO_LOG_LENGTH %d", infoLen);
            if (infoLen > 1)
            {
                char *infoLog = (char *)malloc(sizeof(char) * infoLen);
                glGetProgramInfoLog(program_id, infoLen, NULL, infoLog);
                LOGE("error linking program:\n%s", infoLog);
                free(infoLog);
            }
        }
    }

    // if (draw_texi_vao == 0)
    // {
    //     vao不是线程间共享的，所以要么每个线程单独一个vao，要么就不用vao
    //     GLuint vbo;
    //     GLuint ebo;
    //     float vbo_data[] = {
    //         1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    //         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    //         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    //         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    //     glGenBuffers(1, &vbo);

    //     glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //     glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

    //     GLuint vao;
    //     glGenVertexArrays(1, &vao);

    //     glBindVertexArray(vao);

    //     glBindBuffer(GL_ARRAY_BUFFER, vbo_data);

    //     glEnableVertexAttribArray(0);
    //     glEnableVertexAttribArray(1);

    //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const void *)0);

    //     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const void *)(3 * sizeof(GLfloat)));
    //     glBindVertexArray(0);
    //     draw_texi_vao = vao;
    // }
}