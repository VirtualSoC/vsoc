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

#include "express-gpu/glv1.h"

static GLuint draw_texi_vao = 0;
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
    // glTexEnvxOES(target, pname, param);
}

void d_glTexParameterx_special(void *context, GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

void d_glShadeModel_special(void *context, GLenum mode)
{
    // glShadeModel(mode);
}

void d_glDrawTexiOES_special(void *context, GLint x, GLint y, GLint z, GLint width, GLint height, GLfloat left_x, GLfloat right_x, GLfloat bottom_y, GLfloat top_y)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    GLuint pre_vbo;
    GLuint pre_ebo;

    float fz = z >= 1 ? 1.0f : z;
    fz = z <= 0 ? 0.0f : z;
    fz = fz * 2.0f - 1.0f;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&pre_vbo);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *)&pre_ebo);

    glUseProgram(draw_texi_program);


    GLint now_texture_target;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &now_texture_target);

    GLuint now_bind_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &now_bind_texture);

    glUniform1i(draw_texi_texture_id_loc, now_texture_target - GL_TEXTURE0);

    float positions[] = {
        1.0f, 1.0f, fz,   // top right
        1.0f, -1.0f, fz,  // bottom right
        -1.0f, -1.0f, fz, // bottom left
        -1.0f, 1.0f, fz   // top left
    };

    float tex_coord[] = {
        right_x, top_y,    // top right
        right_x, bottom_y, // bottom right
        left_x, bottom_y,  // bottom left
        left_x, top_y,     // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glViewport(x, y, width, height);

    glBindBuffer(GL_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, positions);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, tex_coord);
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

    glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pre_ebo);
}

void prepare_draw_texi()
{
    if (draw_texi_program == 0)
    {
        //数组取地址不是字符串指针的指针，所以这里不要用数组
        char *vShaderCode = "#version 300 es\n"
                            "layout(location = 0) in vec3 aPos;\n"
                            "layout(location = 1) in vec2 aTexCoord;\n"
                            "out vec2 TexCoord;\n"
                            "void main()\n"
                            "{\n"
                            "    gl_Position = vec4(aPos, 1.0);\n"
                            "    TexCoord = aTexCoord;\n"
                            "}\n";

        char *fShaderCode = "#version 300 es\n"
                            "precision mediump float;\n"
                            "out vec4 FragColor;\n"

                            "in vec2 TexCoord;\n"

                            "uniform sampler2D texture_id;\n"
                            "uniform bool alpha_on;\n"

                            "void main()\n"
                            "{\n"
                            "   vec4 tex_color = texture(texture_id, TexCoord);\n"
                            "   if(alpha_on){\n"
                            "       tex_color.a = 1.0 - tex_color.r;\n"
                            "   }\n"
                            "   FragColor = tex_color;\n"
                            "}\n";

        GLuint vertex, fragment, geometry;
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
        printf("linked %d program %u", linked, program_id);
        if (!linked)
        {
            GLint infoLen = 0;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
            printf("GL_INFO_LOG_LENGTH %d", infoLen);
            if (infoLen > 1)
            {
                char *infoLog = (char *)malloc(sizeof(char) * infoLen);
                glGetProgramInfoLog(program_id, infoLen, NULL, infoLog);
                printf("Error linking program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
    }

    // if (draw_texi_vao == 0)
    // {
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