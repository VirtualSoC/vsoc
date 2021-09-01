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
    glTexEnvxOES(target, pname, param);
}

void d_glTexParameterx_special(void *context, GLenum target, GLenum pname, GLint param)
{
    glTexParameterxOES(target, pname, param);
}

void d_glShadeModel_special(void *context, GLenum mode)
{
    glShadeModel(mode);
}

void d_glDrawTexiOES_special(void *context, GLint x, GLint y, GLint z, GLint width, GLint height)
{
    glUseProgram(draw_texi_program);

    Opengl_Context *opengl_context = (Opengl_Context *)context;

    float positions[] = {
        1.0f, 1.0f, 0.0f,   // top right
        1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f   // top left
    };

    float tex_coord[] = {
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bottom right
        0.0f, 0.0f, // bottom left
        0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glViewport(x, y, width, height);

    glBindBuffer(GL_BUFFER, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, positions);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, tex_coord);
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

    glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

    // glUseProgram(0);
}

void prepare_draw_texi()
{
    if (draw_texi_program == 0)
    {
        //数组取地址不是字符串指针的指针，所以这里不要用数组
        char *vShaderCode = "#version 300 es"
                            "layout(location = 0) in vec3 aPos;"
                            "layout(location = 1) in vec2 aTexCoord;"
                            "out vec2 TexCoord;"
                            "void main()"
                            "{"
                            "    gl_Position = vec4(aPos, 1.0);"
                            "    TexCoord = aTexCoord;"
                            "}";

        char *fShaderCode = "#version 300 es"
                            "precision mediump float;"
                            "out vec4 FragColor;"

                            "in vec2 TexCoord;"

                            "uniform sampler2D texture0;"
                            "uniform bool alpha_on;"

                            "void main()"
                            "{"
                            "vec4 tex_color = texture(texture0, TexCoord);"
                            "if(alpha_on){"
                            "    tex_color.a = 1.0 - tex_color.r;"
                            "}"
                            "FragColor = tex_color;"
                            "}";

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

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        draw_texi_program = program_id;
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