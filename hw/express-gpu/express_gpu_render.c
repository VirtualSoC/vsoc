/**
 * @file express_gpu_render.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 使用host的GPU进行渲染，并且构建双缓冲，包含了双缓冲的相关函数
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
// #define STD_DEBUG_LOG
// #define TIMER_LOG
#include "qemu/osdep.h"
#include "qemu/atomic.h"

#include "express-gpu/express_gpu_render.h"

#include "direct-express/direct_express.h"
#include "direct-express/express_log.h"

#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"
#include "express-gpu/glv1.h"

#include "ui/console.h"
#include "ui/input.h"
#include "sysemu/runstate.h"

#include "express-gpu/sdl_control.h"

// HWND draw_native_window;

GAsyncQueue *main_window_event_queue = NULL;
int main_window_event_queue_lock = 0;

Static_Context_Values *preload_static_context_value = NULL;

int sdl2_no_need = 0;

static unsigned int main_frame_num = 0;

static int event_queue_lock;
static GQueue *sync_event_queue;

static GHashTable *gbuffer_id_surface_map = NULL;
static GHashTable *gbuffer_id_image_map = NULL;
static int gbuffer_id_surface_map_lock = 0;
static int gbuffer_id_image_map_lock = 0;


static int calc_screen_hz = 0;

static int now_screen_hz = 0;

static gint64 last_calc_time = 0;
static gint64 frame_start_time = 0;
static gint64 remain_sleep_time = 0;

// static gint64 stand_frame_time = 0;
// static volatile gint64 last_gen_frame_time = 0;
// static gint64 gen_frame_time_all = 0;
// static gen_frame_cnt = 0;
// static gint64 now_gen_frame_time = 0;

static gint64 gen_frame_time_avg_1s = 0;

// static int force_gsync = 0;

#define EVENT_QUEUE_LOCK                                   \
    while (atomic_cmpxchg(&(event_queue_lock), 0, 1) == 1) \
        ;

#define EVENT_QUEUE_UNLOCK atomic_cmpxchg(&(event_queue_lock), 1, 0);

// static void *opengl_render_hwnd = NULL;

static GLFWwindow *glfw_window = NULL;

void *dummy_window_for_sync = NULL;

static GLuint programID = 0;
static GLuint drawVAO = 0;

static GLint reverse_loc = 0;
static GLuint is_reverse = 0;

static long window_width = 0;
static long window_height = 0;

static long real_window_width = 0;
static long real_window_height = 0;

static Window_Buffer *compose_surface;
static int compose_surface_lock = 0;

volatile int native_render_run = 0;

static QemuConsole *input_receive_con = NULL;

static const GLubyte GPU_VENDOR[] = "Express_GPU (";
static const GLubyte GPU_VERSION[] = "OpenGL ES 3.1 (";
static const GLubyte GPU_RENDERER[] = "OpenGL ES Translator (";
static const GLubyte GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.10";

//google devide info
// static const GLubyte GPU_VENDOR[] = "Google (";
// static const GLubyte GPU_VERSION[] = "OpenGL ES 3.0 (";
// static const GLubyte GPU_RENDERER[] = "Android Emulator OpenGL ES Translator (";
// static const GLubyte GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.00";

static const int OPENGL_MAJOR_VERSION = 3;
static const int OPENGL_MINOR_VERSION = 1;

static const GLubyte *SPECIAL_EXTENSIONS[] =
    {
        /*1*/ "GL_OES_EGL_image",
        /*2*/ "GL_OES_EGL_image_external",
        /*3*/ "GL_OES_EGL_sync",
        /*4*/ "GL_OES_depth24",
        /*5*/ "GL_OES_depth32",
        /*6*/ "GL_OES_texture_float",
        /*25*/ "GL_OES_texture_float_linear",
        /*7*/ "GL_OES_texture_half_float",
        /*8*/ "GL_OES_texture_half_float_linear",
        /*9*/ "GL_OES_compressed_ETC1_RGB8_texture",
        /*10*/ "GL_OES_depth_texture",
        /*11*/ "GL_OES_EGL_image_external_essl3",
        /*12*/ "GL_KHR_texture_compression_astc_ldr",
        /*13*/ "GL_KHR_texture_compression_astc_hdr",
        /*14*/ "GL_OES_vertex_array_object",
        // /*14*/ "GL_EXT_shader_framebuffer_fetch",   //这个暂时看情况支持，webview用它来混合，会着色器中使用变量gl_LastFragData
        // /*15*/ "GL_EXT_multisampled_render_to_texture",  //这个暂时不能有，因为它需要支持相关函数
        /*16*/ "GL_EXT_color_buffer_float",
        /*17*/ "GL_EXT_color_buffer_half_float",
        /*18*/ "GL_OES_element_index_uint",
        /*19*/ "GL_OES_texture_float_linear",
        /*20*/ "GL_OES_compressed_paletted_texture",
        /*21*/ "GL_OES_packed_depth_stencil",
        /*22*/ "GL_OES_texture_npot",
        /*23*/ "GL_OES_rgb8_rgba8",
        /*24*/ "GL_OES_framebuffer_object",
};
static const int SPECIAL_EXTENSIONS_SIZE = 24;

//支持这些扩展需要添加一些函数，所以暂时先不支持——因为有些扩展会被全平台的skia识别而使用，但是这些函数实际为空所以会发生错误
static const GLubyte *NOT_SUPPORT_EXTENSIONS[] =
    {
        //gl
        /* 1*/ "GL_NV_texture_barrier",          // and gles
        /* 2*/ "GL_KHR_blend_equation_advanced", // and gles
        /* 3*/ "GL_NV_blend_equation_advanced",  // and gles
        /* 4*/ "GL_ARB_clear_texture",
        /* 5*/ "GL_ARB_draw_indirect",
        /* 6*/ "GL_ARB_timer_query",
        /* 7*/ "GL_EXT_timer_query",
        /* 8*/ "GL_ARB_multi_draw_indirect",
        /* 9*/ "GL_NV_path_rendering",            // and gles
        /*10*/ "GL_NV_framebuffer_mixed_samples", // and gles
        /*11*/ "GL_EXT_debug_marker",             //and gles
        /*12*/ "GL_ARB_invalidate_subdata",
        /*13*/ "GL_KHR_debug",             // and gles
        /*14*/ "GL_EXT_window_rectangles", // and gles

        //gles
        /*15*/ "GL_EXT_blend_func_extended",
        /*16*/ "GL_EXT_clear_texture",
        /*17*/ "GL_EXT_multi_draw_indirect",
        /*18*/ "GL_OES_texture_buffer",
        /*19*/ "GL_EXT_texture_buffer",
        /*20*/ "GL_CHROMIUM_map_sub",
        /*21*/ "GL_CHROMIUM_path_rendering",
        /*22*/ "GL_CHROMIUM_framebuffer_mixed_samples",
        /*23*/ "GL_CHROMIUM_bind_uniform_location"};
static const int NOT_SUPPORT_EXTENSION_SIZE = 23;

static void opengl_paint(Window_Buffer *d_buffer);
static void *native_window_create();

static void g_queue_event_notify(gpointer data, gpointer user_data);

Notifier shutdown_notifier;

static gint64 last_click_time = 0;
static void close_window_callback(GLFWwindow *window)
{
    gint64 now_time = g_get_real_time();

    // printf("shutdown time %lld\n",now_time);
    glfwSetWindowShouldClose(window, GLFW_FALSE);
    if (now_time - last_click_time < 500000)
    {
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_HOST_UI);
    }
    else
    {
        qemu_system_powerdown_request();
    }
    last_click_time = now_time;
}

static void shutdown_notify_callback(Notifier *notifier, void *data)
{
    printf("notify shutdown! %lld\n", g_get_real_time());

    ATOMIC_UNLOCK(main_window_event_queue_lock);
    ATOMIC_UNLOCK(compose_surface_lock);
    set_compose_surface(NULL, NULL);
    direct_express_should_stop = true;

    // glfwTerminate();
    if (native_render_run == 2)
    {
        native_render_run = -1;
        int wait_cnt = 0;
        while (native_render_run == -1 && wait_cnt < 200)
        {
            // printf("wait thread close \n");
            g_usleep(5000);
            wait_cnt++;
        }
        if (native_render_run == -1)
        {
            printf("wait time too long!\n");
        }

        // printf("wait thread close done %d\n",native_render_run);
    }
}

static void keyboard_handle_callback(GLFWwindow *window, int key, int code, int action, int mods)
{
    int qcode;
    bool down = false;

    if (code > qemu_input_map_glfw_to_qcode_len)
    {
        return;
    }
    qcode = qemu_input_map_glfw_to_qcode[key];

    if (action == GLFW_RELEASE)
    {
        down = false;
    }
    else
    {
        down = true;
    }

    qemu_input_event_send_key_qcode(input_receive_con, (QKeyCode)qcode, down);
    // qemu_input_event_sync();

    // printf("key:%d, code:%d, action:%d, mods:%d,scancode %d,qcode %d\n", key, code, action, mods, glfwGetKeyScancode(key),qcode);
}

static void mouse_move_handle_callback(GLFWwindow *window, double xpos, double ypos)
{
#ifdef ENSURE_SAME_WIDTH_HEIGHT_RATIO
    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)(xpos / real_window_width * window_width), 0, window_width);
    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)(ypos / real_window_height * window_height), 0, window_height);
#else
    if (real_window_height > window_height)
    {
        ypos -= (real_window_height - window_height) / 2;
    }

    if (real_window_width > window_width)
    {
        xpos -= (real_window_width - window_width) / 2;
    }
    if ((int)ypos > window_height || (int)xpos > window_width || (int)ypos < 0 || (int)xpos < 0)
    {
        return;
    }

    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)xpos, 0, window_width);
    qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)ypos, 0, window_height);
// qemu_input_event_sync();
#endif
}

static void mouse_click_handle_callback(GLFWwindow *window, int button, int action, int mods)
{
    InputButton btn;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        btn = INPUT_BUTTON_LEFT;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        btn = INPUT_BUTTON_RIGHT;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        btn = INPUT_BUTTON_MIDDLE;
    }
    else
    {
        return;
    }

    bool press = true;
    if (action == GLFW_RELEASE)
    {
        press = false;
    }
    qemu_input_queue_btn(input_receive_con, btn, press);
    // qemu_input_event_sync();
}

static void mouse_scroll_handle_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    InputButton btn;
    if (yoffset > 0)
    {
        btn = INPUT_BUTTON_WHEEL_UP;
    }
    else if (yoffset < 0)
    {
        btn = INPUT_BUTTON_WHEEL_DOWN;
    }
    else
    {
        return;
    }

    qemu_input_queue_btn(input_receive_con, btn, true);
    qemu_input_event_sync();
    qemu_input_queue_btn(input_receive_con, btn, false);
    qemu_input_event_sync();
}

void window_size_change_callback(GLFWwindow *window, int width, int height)
{
    //需要保证画面比例不变
    int x = 0, y = 0;
    if (real_window_width != width || real_window_height != height)
    {
        int calc_width = height * window_width / window_height;
        int calc_height = width * window_height / window_width;

#ifdef ENSURE_SAME_WIDTH_HEIGHT_RATIO
        if (calc_width < width && calc_height > height)
        {
            real_window_width = calc_width;
            real_window_height = height;
            x = (width - calc_width) / 2;
        }
        else if (calc_width > width && calc_height < height)
        {
            real_window_width = width;
            real_window_height = calc_height;
            y = (height - calc_height) / 2;
        }
        else
        {
            //其他情况认为是精度计算问题，直接用新的值
            real_window_width = width;
            real_window_height = height;
        }
        glViewport(0, 0, real_window_width, real_window_height);

        glfwSetWindowSize(window, real_window_width, real_window_height);

        return;

#else

        if (calc_width < width && calc_height > height)
        {
            window_width = calc_width;
            window_height = height;
            x = (width - calc_width) / 2;
        }
        else if (calc_width > width && calc_height < height)
        {
            window_width = width;
            window_height = calc_height;
            y = (height - calc_height) / 2;
        }
        else
        {
            //其他情况认为是精度计算问题，直接用新的值
            window_width = width;
            window_height = height;
        }
        real_window_width = width;
        real_window_height = height;

        glViewport(x, y, window_width, window_height);

#endif
    }
}

static void handle_child_window_event()
{
    ATOMIC_LOCK(main_window_event_queue_lock);
    Main_window_Event *child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
    ATOMIC_UNLOCK(main_window_event_queue_lock);

    while (child_event != NULL)
    {
        switch (child_event->event_code)
        {
        case MAIN_PAINT:
            break;
        case MAIN_CREATE_CHILD_WINDOW:

            //context只能是由父线程创建，以进行资源共享
            {
                void **window_ptr = (Window_Buffer *)child_event->data;
                if (window_ptr == NULL)
                {
                    break;
                }
                // printf("create window\n");
                // gint64 t = g_get_real_time();
                // printf("start create window %lld\n", t);
                *window_ptr = (void *)native_window_create();
                // printf("create window %lld\n", g_get_real_time() - t);
            }

            break;
        case MAIN_DESTROY_SURFACE:
        {
            //这个destroy调用来自于客户端进程关闭后的销毁函数
            Window_Buffer *surface = (Window_Buffer *)child_event->data;
            if (surface == NULL)
            {
                break;
            }
            if (surface->I_am_composer)
            {
                set_compose_surface(surface, NULL);
            }
            // if (surface->guest_gbuffer_id != 0)
            // {
            //     set_gbuffer_id_surface(NULL, surface->guest_gbuffer_id);
            // }

            // if (surface->type == WINDOW_SURFACE)
            // {
            //     //surface删除的时候，只有当surface是window类型，而且当前gbuffer_id确实是当前的surface的时候才能删除连接
            //     for(int i = 0;i<surface->guest_gbuffer_num;i++)
            //     {
            //         set_gbuffer_id_surface(surface->guest_gbuffer_id[i], surface, NULL);
            //     }
            // }
            // printf("real destroy surface %llx\n", surface);

            //删除surface只是试图删除它拥有的缓冲区，而不需要删除window
            glDeleteTextures(surface->buffer_num, surface->fbo_texture);
            glDeleteRenderbuffers(surface->buffer_num, surface->display_rbo_depth);
            glDeleteRenderbuffers(surface->buffer_num, surface->display_rbo_stencil);
            if (surface->config->sample_buffers_num != 0)
            {
                glDeleteRenderbuffers(surface->buffer_num, surface->sampler_rbo);
            }
            for (int i = 0; i < 5; i++)
            {
                if (surface->delete_sync[i] != 0)
                {
                    glDeleteSync(surface->delete_sync[i]);
                }
            }
            g_free(surface);
        }
        break;
        case MAIN_DESTROY_CONTEXT:
        {
            Opengl_Context *opengl_context = (Opengl_Context *)child_event->data;
            if (opengl_context == NULL)
            {
                break;
            }

            //删除context意味着要删除窗口，不过这个时候窗口连接的surface假如仍然存在的话，surface对应的texture的空间一定存在
            if (opengl_context->window != NULL)
            {
                // gint64 t = g_get_real_time();
                // printf("start destroy window %lld\n", t);
#ifdef USE_GLFW_AS_WGL
                glfwSetWindowShouldClose(opengl_context->window, 1);
                glfwDestroyWindow((GLFWwindow *)opengl_context->window);
#else
                // egl_destroyContext(opengl_context->window);
#endif
                // printf("end destroy window %lld\n", g_get_real_time() - t);
            }

            opengl_context_destroy(opengl_context);
            g_free(opengl_context);
        }
        break;
        case MAIN_DESTROY_IMAGE:
        {
            EGL_Image *real_image = (EGL_Image *)child_event->data;
            if (real_image == NULL)
            {
                break;
            }
            set_gbuffer_id_image(real_image->gbuffer_id, real_image, NULL);
            express_printf("real destroy image %lx\n", real_image);

            destroy_real_image(real_image);
        }
        break;
        case MAIN_DESTROY_ALL_EGLSYNC:
        {
            Resource_Map_Status *status = (Resource_Map_Status *)child_event->data;
            if (status == NULL || status->max_id == 0)
            {
                break;
            }
            for (int i = 1; i <= status->max_id; i++)
            {
                if (status->resource_id_map[i] != 0)
                {
                    glDeleteSync((GLsync)status->resource_id_map[i]);
                }
            }
            if (status->resource_id_map != NULL)
            {
                g_free(status->resource_id_map);
            }
            g_free(status);
        }
        break;
        case MAIN_DESTROY_ONE_SYNC:
        {
            GLsync sync = (GLsync)child_event->data;
            if (sync == NULL)
            {
                break;
            }

            glDeleteSync(sync);
        }
        break;
        case MAIN_DESTROY_ONE_TEXTURE:
        {
            GLuint texture = (GLsync)child_event->data;
            if (texture == 0)
            {
                break;
            }

            glDeleteTextures(1, &texture);
        }
        break;
        default:
            //express_printf("child win msg: %d\n", uMsg);
            break;
        }
        g_free(child_event);

        ATOMIC_LOCK(main_window_event_queue_lock);
        child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
        ATOMIC_UNLOCK(main_window_event_queue_lock);
    }

    return;
}

/**
 * @brief 主窗口绘制时使用的着色器加载的代码
 * 
 * @param type 着色器类型
 * @param shaderSrc 着色器源码
 * @return GLuint 返回着色器编号，若为0则生成失败
 */
static GLuint load_shader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
    {
        //    express_printf("Shader==0\n");
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char *infoLog = malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        //    express_printf("compiled==0\n");
        return 0;
    }

    return shader;
}

/**
 * @brief 当前界面使用OpenGL渲染的前置操作，例如加载着色器，生成顶点等
 * 
 * @param program 返回值，填入生成的着色器ID
 * @param VAO 返回值，填入生成的顶点数组ID 
 * @return int 返回1表示准备成功，为0则说明准备失败
 */
static int opengl_prepare(GLint *program, GLint *VAO)
{
    char vShaderStr[] =
        "#version 300 es\n"
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 texCoords;\n"
        "uniform int need_reverse;\n"
        "out vec2 TexCoords;\n"
        "void main()\n"
        "{\n"
        "    if(need_reverse == 0)\n"
        "    {\n"
        "       gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "       gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);\n"
        "    }\n"
        "    TexCoords = texCoords;\n"
        "}\n";

    char fShaderStr[] =
        "#version 300 es\n"
        "precision mediump float;                     \n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D screenTexture;\n"
        "void main(){\n"
        "color = texture(screenTexture, TexCoords);\n"
        "}\n";

    GLuint programObject = glCreateProgram();
    if (programObject == 0)
    {
        //express_printf("shit glCreateProgram2 %ld\n", GetLastError());
        return 0;
    }

    GLuint vertexShader = load_shader(GL_VERTEX_SHADER, vShaderStr);
    GLuint fragmentShader = load_shader(GL_FRAGMENT_SHADER, fShaderStr);
    //express_printf("shader %d %d\n", vertexShader, fragmentShader);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    reverse_loc = glGetUniformLocation(programObject, "need_reverse");
    is_reverse = 0;

    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        //express_printf("shit glGetProgramiv2 %ld\n", GetLastError());
        return 0;
    }

    GLfloat quadVertices[] = {// Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                              // Positions   // TexCoords
                              -1.0f, 1.0f, 0.0f, 1.0f,
                              -1.0f, -1.0f, 0.0f, 0.0f,
                              1.0f, -1.0f, 1.0f, 0.0f,

                              -1.0f, 1.0f, 0.0f, 1.0f,
                              1.0f, -1.0f, 1.0f, 0.0f,
                              1.0f, 1.0f, 1.0f, 1.0f};

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    *program = programObject;
    *VAO = quadVAO;

    //开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programObject);

    glClearColor(0, 1, 0, 1);

    return 1;
}

static void static_value_prepare()
{

    preload_static_context_value = g_malloc(sizeof(Static_Context_Values) + 512 * 100 + 400);
    memset(preload_static_context_value, 0, sizeof(Static_Context_Values) + 512 * 100 + 400);

    // initialize static status
    preload_static_context_value->major_version = OPENGL_MAJOR_VERSION;
    preload_static_context_value->minor_version = OPENGL_MINOR_VERSION;

    preload_static_context_value->implementation_color_read_type = 5121;
    preload_static_context_value->implementation_color_read_format = 6408;
    preload_static_context_value->max_array_texture_layers = 2048;
    preload_static_context_value->max_color_attachments = 8;
    preload_static_context_value->max_combined_uniform_blocks = 84;
    preload_static_context_value->max_draw_buffers = 16;
    preload_static_context_value->max_fragment_input_components = 128;
    preload_static_context_value->max_fragment_uniform_blocks = 14;
    preload_static_context_value->max_program_texel_offset = 7;
    preload_static_context_value->max_transform_feedback_interleaved_components = 128;
    preload_static_context_value->max_transform_feedback_separate_attribs = 4;
    preload_static_context_value->max_transform_feedback_separate_components = 4;
    preload_static_context_value->max_uniform_buffer_bindings = 84;
    preload_static_context_value->max_varying_components = 124;
    preload_static_context_value->max_varying_vectors = 31;
    preload_static_context_value->max_vertex_output_components = 128;
    preload_static_context_value->max_vertex_uniform_blocks = 14;
    preload_static_context_value->min_program_texel_offset = -8;
    preload_static_context_value->max_uniform_block_size = 65536;
    preload_static_context_value->aliased_point_size_range[0] = 1.0f;
    preload_static_context_value->aliased_point_size_range[1] = 2047.0f;

    //这个地方就算溢出了也不怕，后面还有那么多位置撑着
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, &(preload_static_context_value->compressed_texture_formats));
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, &(preload_static_context_value->program_binary_formats));
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, &(preload_static_context_value->shader_binary_formats));
    glGetIntegerv(GL_SUBPIXEL_BITS, &(preload_static_context_value->subpixel_bits));
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &(preload_static_context_value->max_3d_texture_size));
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &(preload_static_context_value->max_combined_texture_image_units));
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &(preload_static_context_value->max_cube_map_texture_size));
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &(preload_static_context_value->max_elements_vertices));
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &(preload_static_context_value->max_elements_indices));
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &(preload_static_context_value->max_fragment_uniform_components));
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &(preload_static_context_value->max_renderbuffer_size));
    glGetIntegerv(GL_MAX_SAMPLES, &(preload_static_context_value->max_samples));
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &(preload_static_context_value->max_texture_size));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &(preload_static_context_value->max_vertex_attribs));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &(preload_static_context_value->max_vertex_uniform_components));


    glGetIntegerv(GL_MAX_IMAGE_UNITS, &(preload_static_context_value->max_image_units));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &(preload_static_context_value->max_vertex_attrib_bindings));
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &(preload_static_context_value->max_computer_uniform_blocks));
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &(preload_static_context_value->max_computer_texture_image_units));
    glGetIntegerv(GL_MAX_COMPUTE_IMAGE_UNIFORMS, &(preload_static_context_value->max_computer_image_uniforms));
    glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &(preload_static_context_value->max_computer_sharde_memory_size));
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &(preload_static_context_value->max_computer_uniform_components));
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_computer_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &(preload_static_context_value->max_computer_atomic_counters));
    glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_compute_uniform_components));
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &(preload_static_context_value->max_computer_work_group_invocations));
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &(preload_static_context_value->max_computer_work_group_count[0]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &(preload_static_context_value->max_computer_work_group_count[1]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &(preload_static_context_value->max_computer_work_group_count[2]));
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &(preload_static_context_value->max_computer_work_group_size[0]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &(preload_static_context_value->max_computer_work_group_size[1]));
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &(preload_static_context_value->max_computer_work_group_size[2]));


    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &(preload_static_context_value->max_uniform_locations));
    glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &(preload_static_context_value->max_framebuffer_width));
    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &(preload_static_context_value->max_framebuffer_height));
    glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &(preload_static_context_value->max_framebuffer_samples));
    glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_vertex_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_fragment_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, &(preload_static_context_value->max_combined_atomic_counter_buffers));
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &(preload_static_context_value->max_fragment_atomic_counters));
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &(preload_static_context_value->max_combined_atomic_counters));
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE, &(preload_static_context_value->max_atomic_counter_buffer_size));
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &(preload_static_context_value->max_atomic_counter_buffer_bindings));
    glGetIntegerv(GL_MAX_VERTEX_IMAGE_UNIFORMS, &(preload_static_context_value->max_vertex_image_uniforms));
    glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, &(preload_static_context_value->max_fragment_image_uniforms));
    glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNIFORMS, &(preload_static_context_value->max_combined_image_uniforms));
    glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_vertex_shader_storage_blocks));
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_fragment_shader_storage_blocks));
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_compute_shader_storage_blocks));
    glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &(preload_static_context_value->max_combined_shader_storage_blocks));
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &(preload_static_context_value->max_shader_storage_buffer_bindings));
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &(preload_static_context_value->max_shader_storage_block_size));
    glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &(preload_static_context_value->max_combined_shader_output_resources));
    glGetIntegerv(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET, &(preload_static_context_value->min_program_texture_gather_offset));
    glGetIntegerv(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET, &(preload_static_context_value->max_program_texture_gather_offset));
    glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &(preload_static_context_value->max_sample_mask_words));
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &(preload_static_context_value->max_color_texture_samples));
    glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &(preload_static_context_value->max_depth_texture_samples));
    glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &(preload_static_context_value->max_integer_samples));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &(preload_static_context_value->max_vertex_attrib_relative_offset));
    // glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &(preload_static_context_value->max_vertex_attrib_bindings));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &(preload_static_context_value->max_vertex_attrib_stride));



    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &(preload_static_context_value->max_vertex_texture_image_units));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &(preload_static_context_value->max_vertex_uniform_vectors));
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &(preload_static_context_value->max_viewport_dims));
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &(preload_static_context_value->num_shader_binary_formats));
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &(preload_static_context_value->num_compressed_texture_formats));
    // glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &(preload_static_context_value->uniform_buffer_offset_alignment));
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &(preload_static_context_value->max_array_texture_layers));
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &(preload_static_context_value->max_color_attachments));
    glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &(preload_static_context_value->max_combined_uniform_blocks));
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &(preload_static_context_value->max_draw_buffers));
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &(preload_static_context_value->max_fragment_input_components));
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &(preload_static_context_value->max_fragment_uniform_blocks));
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &(preload_static_context_value->max_fragment_uniform_vectors));
    glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &(preload_static_context_value->max_program_texel_offset));
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &(preload_static_context_value->max_transform_feedback_separate_attribs));
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, &(preload_static_context_value->max_transform_feedback_separate_components));
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &(preload_static_context_value->max_transform_feedback_interleaved_components));
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &(preload_static_context_value->max_uniform_buffer_bindings));
    // glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &(preload_static_context_value->max_atomic_counter_buffer_bindings));
    // glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &(preload_static_context_value->max_shader_storage_buffer_bindings));
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &(preload_static_context_value->max_varying_vectors));
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &(preload_static_context_value->max_varying_components));
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &(preload_static_context_value->max_vertex_output_components));
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &(preload_static_context_value->max_vertex_uniform_blocks));
    glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &(preload_static_context_value->min_program_texel_offset));
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &(preload_static_context_value->num_program_binary_formats));
    // glGetIntegerv(GL_SAMPLES, &(preload_static_context_value->samples));
    // glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &(preload_static_context_value->shader_storage_buffer_offset_alignment));
    glGetIntegerv(GL_SUBPIXEL_BITS, &(preload_static_context_value->subpixel_bits));

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, preload_static_context_value->aliased_line_width_range);
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, preload_static_context_value->aliased_point_size_range);
    glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &(preload_static_context_value->max_texture_log_bias));

    glGetInteger64v(GL_MAX_ELEMENT_INDEX, &(preload_static_context_value->max_element_index));
    glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &(preload_static_context_value->max_server_wait_timeout));
    glGetInteger64v(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_vertex_uniform_components));
    glGetInteger64v(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &(preload_static_context_value->max_combined_fragment_uniform_components));
    glGetInteger64v(GL_MAX_UNIFORM_BLOCK_SIZE, &(preload_static_context_value->max_uniform_block_size));


    GLenum error =glGetError();
    if(error!=GL_NO_ERROR)
    {
        printf("error when creating static vaules %x\n",error);
    }
    //下面三个值之所以要限定范围，是因为guest端有个固定大小的数组，这个最大值是数组的最大大小
    if (preload_static_context_value->max_vertex_attribs > 32)
    {
        preload_static_context_value->max_vertex_attribs = 32;
    }

    if(preload_static_context_value->max_image_units > 16)
    {
        preload_static_context_value->max_image_units = 16;
    }

    if(preload_static_context_value->max_vertex_attrib_bindings > 32)
    {
        preload_static_context_value->max_vertex_attrib_bindings = 32;
    }

    preload_static_context_value->num_compressed_texture_formats += 1;
    if (preload_static_context_value->num_compressed_texture_formats > 128)
    {
        preload_static_context_value->compressed_texture_formats[127] = GL_ETC1_RGB8_OES;
        preload_static_context_value->num_compressed_texture_formats = 128;
    }
    else
    {
        preload_static_context_value->compressed_texture_formats[preload_static_context_value->num_compressed_texture_formats - 1] = GL_ETC1_RGB8_OES;
    }

    if (preload_static_context_value->num_program_binary_formats > 8)
    {
        preload_static_context_value->num_program_binary_formats = 8;
    }
    if (preload_static_context_value->num_shader_binary_formats > 8)
    {
        preload_static_context_value->num_shader_binary_formats = 8;
    }
    if (preload_static_context_value->num_compressed_texture_formats > 128)
    {
        preload_static_context_value->num_compressed_texture_formats = 128;
    }

    char *string_loc = ((char *)preload_static_context_value) + sizeof(Static_Context_Values);

    glGetIntegerv(GL_NUM_EXTENSIONS, &(preload_static_context_value->num_extensions));

    char *temp_loc = string_loc;

    const GLubyte *gl_string;
    gl_string = glGetString(GL_VENDOR);
    preload_static_context_value->vendor = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_VENDOR, sizeof(GPU_VENDOR) - 1);
    temp_loc += sizeof(GPU_VENDOR) - 1;
    memcpy(temp_loc, gl_string, strlen(gl_string));
    temp_loc += strlen(gl_string);
    *temp_loc = ')';
    temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("\ngl vendor:%s\n", string_loc + (unsigned long)(preload_static_context_value->vendor));

    gl_string = glGetString(GL_VERSION);
    preload_static_context_value->version = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_VERSION, sizeof(GPU_VERSION) - 1);
    temp_loc += sizeof(GPU_VERSION) - 1;
    memcpy(temp_loc, gl_string, strlen(gl_string));
    temp_loc += strlen(gl_string);
    *temp_loc = ')';
    temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("gl version:%s\n", string_loc + (unsigned long)(preload_static_context_value->version));

    gl_string = glGetString(GL_RENDERER);
    preload_static_context_value->renderer = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_RENDERER, sizeof(GPU_RENDERER) - 1);
    temp_loc += sizeof(GPU_RENDERER) - 1;
    memcpy(temp_loc, gl_string, strlen(gl_string));
    temp_loc += strlen(gl_string);
    *temp_loc = ')';
    temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("gl renderer:%s\n", string_loc + (unsigned long)(preload_static_context_value->renderer));

    preload_static_context_value->shading_language_version = (unsigned long long)(temp_loc - string_loc);
    memcpy(temp_loc, GPU_SHADER_LANGUAGE_VERSION, sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1);
    temp_loc += sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1;
    *temp_loc = 0;
    temp_loc++;
    printf("gl shading_language_version:%s\n", string_loc + (unsigned long)(preload_static_context_value->shading_language_version));

    char *extensions_start = temp_loc;

    int no_need_extensions_cnt = 0;
    int num_extensions = preload_static_context_value->num_extensions;

    num_extensions = 0;

    int start_loc = 0;
    for (int i = start_loc; i < start_loc + num_extensions && i < 512 - SPECIAL_EXTENSIONS_SIZE + no_need_extensions_cnt; i++)
    {

        gl_string = glGetStringi(GL_EXTENSIONS, i);

        int no_need_flag = 0;
        for (int j = 0; j < NOT_SUPPORT_EXTENSION_SIZE; j++)
        {
            if (strstr(gl_string, NOT_SUPPORT_EXTENSIONS[j]) != NULL)
            {
                no_need_flag = 1;
                break;
            }
        }
        if (no_need_flag == 1)
        {
            no_need_extensions_cnt += 1;
            continue;
        }

        preload_static_context_value->extensions[i - start_loc - no_need_extensions_cnt] = (unsigned long long)(temp_loc - string_loc);

        memcpy(temp_loc, gl_string, strlen(gl_string));
        temp_loc += strlen(gl_string);
        *temp_loc = 0;
        printf("%d %s\n", i, temp_loc - strlen(gl_string));
        temp_loc++;
    }

    num_extensions -= no_need_extensions_cnt;

    for (int i = 0; i < SPECIAL_EXTENSIONS_SIZE; i++)
    {
        preload_static_context_value->extensions[num_extensions + i] = temp_loc - string_loc;

        memcpy(temp_loc, SPECIAL_EXTENSIONS[i], strlen(SPECIAL_EXTENSIONS[i]));
        temp_loc += strlen(SPECIAL_EXTENSIONS[i]);
        *temp_loc = 0;
        temp_loc++;
    }

    num_extensions += SPECIAL_EXTENSIONS_SIZE;

    preload_static_context_value->num_extensions = num_extensions;

    int extensions_len = temp_loc - extensions_start;

    memcpy(temp_loc, extensions_start, extensions_len);
    for (int i = 0; i < extensions_len; i++)
    {
        if (*temp_loc == 0)
        {
            *temp_loc = ' ';
        }
        temp_loc++;
    }
    temp_loc--;
    if (*temp_loc == ' ')
    {
        *temp_loc = 0;
    }

    preload_static_context_value->extensions_gles2 = (unsigned long long)(extensions_start - string_loc + extensions_len);
    printf("extensions len %d num %d: %s|\n", extensions_len, num_extensions, string_loc + (unsigned long)(preload_static_context_value->extensions_gles2));
    assert(temp_loc < ((char *)preload_static_context_value) + sizeof(Static_Context_Values) + 512 * 100 + 400);
}

/**
 * @brief 界面上用于画出图像的函数，实际逻辑为取出d_buffer中的display_texture，然后画出来
 * 
 * @param d_buffer 
 */
static void opengl_paint(Window_Buffer *d_buffer)
{
    // glClear(GL_COLOR_BUFFER_BIT);
    // glClearColor(1, 1, 1, 0);
    // glViewport(0, 0, window_width, window_height);
    // glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(0, 0, 1, 0);
    // glDisable(GL_DEPTH_TEST);

    if (d_buffer->type == WINDOW_SURFACE)
    {
        if (is_reverse == 1)
        {
            is_reverse = 0;
            glUniform1i(reverse_loc, 0);
        }
        if (window_width == 0 || window_height == 0)
        {
            window_width = d_buffer->width;
            window_height = d_buffer->height;
            real_window_width = window_width;
            real_window_height = window_height;
            glViewport(0, 0, window_width, window_height);
        }

        GLuint texture = acquire_texture_from_surface(d_buffer);

        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        release_texture_from_surface(d_buffer);
    }
    else if (d_buffer->type == P_SURFACE)
    {
        if (d_buffer->display_guest_gbuffer_id == 0)
        {
            return;
        }
        if (is_reverse == 0)
        {
            is_reverse = 1;
            glUniform1i(reverse_loc, 1);
        }

        //注意，下面这种情况是为了照顾surfaceflinger的合成逻辑
        EGL_Image *real_image = get_image_from_gbuffer_id(d_buffer->display_guest_gbuffer_id);
        // printf("main acquire image %lx to read\n",real_image);

        if (window_width == 0 || window_height == 0)
        {
            window_width = real_image->width;
            window_height = real_image->height;
            real_window_width = window_width;
            real_window_height = window_height;
            glViewport(0, 0, window_width, window_height);
        }

        GLuint texture = acquire_texture_from_image(real_image);

        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        release_texture_from_image(real_image);
        // printf("main release image %lx to read\n",real_image);
    }
}

#ifdef ENABLE_OPENGL_DEBUG
static void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // 忽略一些不是错误的id
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    printf("main debug message(%u):%s\n", id, message);
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        printf("Source: API ");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Source: Window System ");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Source: Shader Compiler ");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Source: Third Party ");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Source: APPLICATION ");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("Type: Error ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Type: Deprecated Behaviour ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Type: Undefined Behaviour ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Type: Portability ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Type: Performance ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Type: Marker ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Type: Push Group ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Type: Pop Group ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Type: Other ");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("Severity: high");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Severity: medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Severity: low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Severity: notification");
        break;
    }
    printf("\n");
}
#endif

/**
 * @brief 创建opengl的context，这个创建过程是在主界面线程中进行的，通过消息机制来实现
 * 
 * @param d_buffer 需要创建context的双缓冲区，创建完成后会直接存入其中
 * @param width 界面的宽
 * @param height 界面的高
 */
static void *native_window_create()
{

    void *child_window = NULL;

#ifdef USE_GLFW_AS_WGL
    static int cnt = 0;
    char name[100];
    sprintf(name, "opengl-child-window%d", cnt);
    cnt++;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    // @todo 验证把下面windowhit给注释掉了（会影响窗口）会不会影响到fbo
    // int idx = 0;
    // while (d_buffer->window_hints.hints[idx] != (int64_t)GLFW_DONT_CARE && idx < HINTS_LEN)
    // {
    //     int64_t hint_enum = d_buffer->window_hints.hints[idx];
    //     int64_t hint_val = d_buffer->window_hints.hints[idx + 1];
    //     glfwWindowHint(hint_enum, hint_val);
    //     idx += 2;
    // }

    // //屏幕分离调试专用
#ifdef DEBUG_INDEPEND_WINDOW
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    child_window = (void *)glfwCreateWindow(1, 1, name, NULL, glfw_window);
#else
//因为咱们是使用的fbo来绘制，因此窗口大小设为1就行了
#ifdef ENABLE_OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    child_window = (void *)glfwCreateWindow(1, 1, name, NULL, glfw_window);

    if (child_window == NULL)
    {
        char *s;
        int ret = glfwGetError(&s);
        express_printf("error code %d detail %s", ret, s);
    }

#endif

#else
    child_window = egl_createContext();
#endif

    //假如某个缓冲区同时被读取和写入，也就是同时以texture读取，以及用其他opengl函数画时，整个opengl环境就会炸
    assert(child_window != NULL);

    // express_printf("create windows surface %lx\n", d_buffer);
    //todo 根据配置设置窗口属性
    return child_window;
}

/**
 * @brief 覆盖在原来窗口上面用于绘制的窗口的线程主函数，主要包括了窗口的建立和设置
 * 
 * @param opaque 需要传入VirtIODevice
 * @return void* 
 */
void *native_window_thread(void *opaque)
{
    VirtIODevice *vdev = opaque;
    Direct_Express *e = DIRECT_EXPRESS(vdev);

    //通过这个方式获取hwnd要求必须使用SDL接口创建界面
    QemuConsole *con;
    while ((con = qemu_console_lookup_by_index(0)) == NULL)
    {
        //理论上启动这个线程时，主窗口的hwnd肯定是有了，所以不会进到这个等待循环内
        g_usleep(10000);
        express_printf("con is NULL\n");
    }

    input_receive_con = con;

    sync_event_queue = g_queue_new();

    main_window_event_queue = g_async_queue_new();

    // HWND render_hwnd = (HWND)qemu_console_get_window_id(con);
    // RECT rcParent;

    // GetClientRect(render_hwnd, &rcParent);

    //初始化glfw
    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

#ifdef ENABLE_OPENGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    //创建一个窗口，这个window也是context
    //这个窗口的大小不用在意，因为之后会重新设置窗口大小
    glfw_window = glfwCreateWindow(1024, 768, "Z模拟器", NULL, NULL);
    if (!glfw_window)
    {
        express_printf("create window error %x\n", glfwGetError(NULL));

        glfwTerminate();
        return NULL;
    }

    //键盘事件
    glfwSetKeyCallback(glfw_window, keyboard_handle_callback);
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //鼠标事件
    glfwSetCursorPosCallback(glfw_window, mouse_move_handle_callback);
    glfwSetMouseButtonCallback(glfw_window, mouse_click_handle_callback);
    glfwSetScrollCallback(glfw_window, mouse_scroll_handle_callback);

    //设置窗口大小可以自由调整
    glfwSetFramebufferSizeCallback(glfw_window, window_size_change_callback);

    glfwSetWindowCloseCallback(glfw_window, close_window_callback);

    shutdown_notifier.notify = shutdown_notify_callback;
    qemu_register_shutdown_notifier(&shutdown_notifier);

    // draw_native_window = glfwGetWin32Window(glfw_window);

    // SetParent(draw_native_window, render_hwnd);
    // SetWindowLong(draw_native_window, GWL_STYLE, WS_CHILD);
    // SetWindowLongPtr(draw_native_window, GWLP_WNDPROC, (LONG_PTR)&sub_window_proc);
    // ShowWindow(draw_native_window, TRUE);

    glfwMakeContextCurrent(glfw_window);

    HDC dpy_dc = GetDC(glfwGetWin32Window(glfw_window));
    HGLRC gl_context = glfwGetWGLContext(glfw_window);
    egl_init(dpy_dc, gl_context);

#ifdef USE_GLFW_AS_WGL
    dummy_window_for_sync = glfwCreateWindow(1, 1, "sync", NULL, glfw_window);
#else
    dummy_window_for_sync = egl_createContext();
#endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        express_printf("load glad error\n");
        return NULL;
    }

    gbuffer_id_surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    gbuffer_id_image_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    prepare_draw_texi();
    static_value_prepare();

    native_render_run = 2;

    opengl_prepare(&programID, &drawVAO);
    glBindVertexArray(drawVAO);

    express_printf("native windows create!\n");

#ifdef SPECIAL_SCREEN_SYNC_HZ
    glfwSwapInterval(0);
#else
    glfwSwapInterval(1);
#endif

    // int a = 1;
    // glViewport(0, 0, window_width, window_height);
    //因为这个是最终窗口，因此不需要进行深度测试与模板测试，直接贴图，只要最后的图像数据就行
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    //开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
    glDisable(GL_BLEND);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef ENABLE_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
    while (!glfwWindowShouldClose(glfw_window) && native_render_run == 2)
    {
        // glfwWaitEvents();
        frame_start_time = g_get_real_time();

        main_frame_num = (main_frame_num + 1) % 65536;

        // TIMER_START(queue)
        EVENT_QUEUE_LOCK;
        // if (compose_surface != NULL)
        //     SetEvent((HANDLE)compose_surface->swap_event);
        g_queue_foreach(sync_event_queue, g_queue_event_notify, NULL);
        g_queue_clear(sync_event_queue);
        EVENT_QUEUE_UNLOCK;
        // TIMER_END(queue)
        // TIMER_OUTPUT(queue, 100)
        glClear(GL_COLOR_BUFFER_BIT);

        handle_child_window_event();
        glfwPollEvents();
        qemu_input_event_sync();

        ATOMIC_LOCK(compose_surface_lock);
        if (compose_surface != NULL)
        {
            opengl_paint(compose_surface);

            if (sdl2_no_need == 0 && window_width != 0 && window_height != 0)
            {
                sdl2_no_need = 1;
                glfwSetWindowSize(glfw_window, window_width, window_height);
                glfwShowWindow(glfw_window);
            }

            // TIMER_START(paint)
            // TIMER_END(paint)

            // TIMER_START(event)
            ATOMIC_UNLOCK(compose_surface_lock);

            // TIMER_END(event)
            // TIMER_OUTPUT(event, 100)

            // TIMER_START(swap)
            glfwSwapBuffers(glfw_window);

            // TIMER_END(swap)

            // TIMER_OUTPUT(paint, 100)
            // TIMER_OUTPUT(swap, 100)
        }
        else
        {
            if (sdl2_no_need == 1)
            {
                sdl2_no_need = 0;
                window_height = 0;
                window_width = 0;
                real_window_width = window_width;
                real_window_height = window_height;
                glfwHideWindow(glfw_window);
            }

            // TIMER_START(event)
            ATOMIC_UNLOCK(compose_surface_lock);

            // TIMER_END(event)
            // TIMER_OUTPUT(event, 100)
            glfwSwapBuffers(glfw_window);
        }

        gint64 now_time = g_get_real_time();

        //注意：帧生成时间波动挺大的

        //计算真实窗口帧率
        if (now_time - last_calc_time > 1000000 && last_calc_time != 0)
        {
            calc_screen_hz += 1;
            now_screen_hz = calc_screen_hz;
            calc_screen_hz = 0;
            gen_frame_time_avg_1s = 1000000 / now_screen_hz;
            express_printf("screen draw avg %lldus %dHz\n", gen_frame_time_avg_1s, now_screen_hz);

            last_calc_time = now_time;
        }
        else if (last_calc_time == 0)
        {
            last_calc_time = now_time;
            calc_screen_hz = 0;
        }
        else
        {
            calc_screen_hz += 1;
        }

#ifdef SPECIAL_SCREEN_SYNC_HZ

        gint64 spend_time = now_time - frame_start_time;
        long need_sleep = 1000000 / SPECIAL_SCREEN_SYNC_HZ - spend_time + remain_sleep_time;

        if (need_sleep <= 0)
        {
            need_sleep = 0;
        }

        gint64 sleep_start_time = g_get_real_time();
        g_usleep(need_sleep);
        gint64 sleep_end_time = g_get_real_time();
        remain_sleep_time = need_sleep - (sleep_end_time - sleep_start_time);
#endif
    }

    // qemu_system_shutdown_request(SHUTDOWN_CAUSE_HOST_UI);
    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(glfw_window);

    printf("native windows close!\n");

    //当他返回0时表示窗口被关掉了
    native_render_run = 0;
    // qemu_thread_join(&t);
    return NULL;
}

/**
 * @brief swapbuffer时的垂直同步
 * 
 * @param event 
 * @param interval 
 * @param now_hz 
 * @return int
 */
int draw_wait_GSYNC(void *event, int wait_frame_num)
{

    //帧率太小的情况，赶不及窗口帧率，直接返回当前窗口frame_num

    if (wait_frame_num == -1)
    {
        return main_frame_num;
    }

    if (wait_frame_num - main_frame_num > 60000)
    {

        return main_frame_num;
    }
    else if (main_frame_num - wait_frame_num > 60000)
    {
        while (wait_frame_num != main_frame_num)
        {
            EVENT_QUEUE_LOCK;
            g_queue_push_tail(sync_event_queue, (gpointer)event);
            EVENT_QUEUE_UNLOCK;
#ifdef _WIN32
            DWORD ret = WaitForSingleObject((HANDLE)event, 100);
            if (ret == WAIT_TIMEOUT)
            {
                express_printf("gsync wait timeout\n");
            }
#endif
        }
        return main_frame_num;
    }
    else if (wait_frame_num <= main_frame_num)
    {
        return main_frame_num;
    }
    else if (wait_frame_num > main_frame_num)
    {
        while (wait_frame_num != main_frame_num)
        {
            EVENT_QUEUE_LOCK;
            g_queue_push_tail(sync_event_queue, (gpointer)event);
            EVENT_QUEUE_UNLOCK;
#ifdef _WIN32
            DWORD ret = WaitForSingleObject(event, 100);
            if (ret == WAIT_TIMEOUT)
            {
                express_printf("gsync wait timeout\n");
            }
#endif
        }
        return main_frame_num;
    }

    //     if (wait_frame_num <= main_frame_num || wait_frame_num - main_frame_num > 60000)
    //     {
    //         //帧率太小了，赶不及窗口帧率，直接返回当前窗口frame_num
    //         return main_frame_num;
    //     }
    //     else
    //     {
    //         while (wait_frame_num > main_frame_num || main_frame_num - wait_frame_num > 60000)
    //         {
    //             EVENT_QUEUE_LOCK;
    //             g_queue_push_tail(event_queue, (gpointer)event);
    //             EVENT_QUEUE_UNLOCK;
    // #ifdef _WIN32
    //             DWORD ret = WaitForSingleObject(event, 20);
    // #elif
    // #endif
    //             if (ret == WAIT_TIMEOUT)
    //             {
    //                 express_printf("gsync wait timeout\n");
    //             }
    //         }
    //     }
    //     if (gen_time == 0 || interval == 0)
    //     {
    //         //假如这个时候帧率还没计算出来，则等待着最高帧率计算，或者程序设定不进行垂直同步
    //         return;
    //     }
    //     else if (gen_time * now_screen_hz > 10000 * interval)
    //     {
    //         // gen_time>1000000/(now_screen_hz/interval)
    //         //如果当前帧数达不到设定的垂直同步帧数，也就是帧生成时间大于该帧数的帧生成时间，则不进行垂直同步
    //         return;
    //     }
    //     else
    //     {
    //         //否则，说明帧生成时间过短，需要等待信号
    //         //等待的信号数量等于（需要等待的时间除以真正的帧生成时间 的向上取整）
    //         int wait_cnt = interval - (gen_time * now_screen_hz) / 1000000;

    //         while (wait_cnt != 0)
    //         {
    //             EVENT_QUEUE_LOCK;
    //             g_queue_push_tail(event_queue, (gpointer)event);
    //             EVENT_QUEUE_UNLOCK;
    // #ifdef _WIN32
    //             DWORD ret = WaitForSingleObject(event, 20);
    // #elif
    // #endif

    //             if (ret == WAIT_TIMEOUT)
    //             {
    //                 express_printf("gsync wait timeout\n");
    //             }
    //             wait_cnt--;
    //         }
    //     }
    // return;
}

// bool should_give_up_gpu()
// {
//     if(stand_frame_time == 0){
//         //正在计算标准的帧生成时间，此时需要放弃gpu，优先保证主窗口
//         return true;
//     }
//     if (last_gen_frame_time * 10 > stand_frame_time * 11 ){
//         //上一次帧生成时间过大，超过标准的110%，则需要放弃gpu，优先保证主窗口
//         express_printf("give up gpu %lld %lld\n",last_gen_frame_time,stand_frame_time);
//         return true;
//     }
//     express_printf("hold gpu %lld %lld\n",last_gen_frame_time,stand_frame_time);
//     //默认情况都不需要放弃GPU
//     return false;
// }

static void g_queue_event_notify(gpointer data, gpointer user_data)
{
#ifdef _WIN32
    SetEvent((HANDLE)data);
#endif
    return;
}

// void render_windows_create(Window_Buffer *context)
// {

//     // Render_Thread_Context *render_context = (Render_Thread_Context *)context;
//     // Window_Buffer *buffer_context = (render_context->render_double_buffer);
//     // Opengl_Context *opengl_context = (render_context->opengl_context);

//     if (context != NULL)
//     {
//         //send是同步的，发送完消息需要等待消息处理完
//         //调用egl_context_create
//         SendMessage(draw_native_window, WM_USER_WINDOW_CREATE, 0, (LPARAM)context);
//     }

//     return;
// }

void set_compose_surface(Window_Buffer *old_surface, Window_Buffer *new_surface)
{
    if (old_surface != NULL)
    {
        if (compose_surface != old_surface)
        {
            return;
        }
    }
    if (compose_surface == new_surface)
    {
        return;
    }
    ATOMIC_LOCK(compose_surface_lock);
    compose_surface = new_surface;
    ATOMIC_UNLOCK(compose_surface_lock);
    express_printf("change compose surface %lx\n", compose_surface);
}

GLuint acquire_texture_from_surface(Window_Buffer *surface)
{

    int now_read = surface->now_read;
    // TIMER_START(texture_loc)

    surface->temp_time = g_get_real_time();

    //PBuffer不允许获取texture
    if (surface->type == P_SURFACE)
    {
        return 0;
    }
    //printf("lock on read %llx texture %d ",surface,now_read);
    // ATOMIC_LOCK(surface->display_texture_is_use[now_read]);
    if (surface->now_acquired != -1)
    {
        ATOMIC_SET_UNUSED(surface->display_texture_is_use[surface->now_acquired]);
    }

    ATOMIC_SET_USED(surface->display_texture_is_use[now_read]);

    // TIMER_END(texture_loc)

    // TIMER_OUTPUT(texture_loc, 100)
    // TIMER_START(sync)
    glFlush();
    if (surface->fbo_sync[now_read] != NULL)
    {
        //最多等待80ms
        // glClientWaitSync(surface->fbo_sync[now_read], GL_SYNC_FLUSH_COMMANDS_BIT, 80000000);
        glWaitSync(surface->fbo_sync[now_read], 0, GL_TIMEOUT_IGNORED);
    }

    // TIMER_END(sync)

    // TIMER_OUTPUT(sync, 100)

    GLuint texture = surface->fbo_texture[now_read];

    surface->now_acquired = now_read;

    return texture;
}

void release_texture_from_surface(Window_Buffer *surface)
{

    int now_read = surface->now_acquired;

    //PBuffer不允许获取texture
    if (surface->type == P_SURFACE)
    {
        return;
    }
    GLsync wait_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();

    //延迟删除glsync，以防止waitsync后立马删除这个sync引起的屏幕闪烁问题（不确定是不是这个原因引起）
    if (surface->fbo_sync[now_read] != NULL)
    {
        if (surface->delete_sync[surface->delete_loc] != 0)
        {
            glDeleteSync(surface->delete_sync[surface->delete_loc]);
        }
        surface->delete_sync[surface->delete_loc] = surface->fbo_sync[now_read];
        surface->delete_loc = (surface->delete_loc + 1) % 5;
    }
    surface->fbo_sync[now_read] = wait_sync;
    // TIMER_END(finish)

    // TIMER_OUTPUT(finish,100)

    // uint64_t spend_time = g_get_real_time() - surface->temp_time;
    // static int cal_cnt = 0;
    // static long all_spend_time = 0;
    // if(cal_cnt<100){
    //     all_spend_time += spend_time;
    //     cal_cnt += 1;
    // }else{
    //     printf("lock avg time %lluus(%llu/%d)\n",spend_time/cal_cnt,spend_time,cal_cnt);
    //     all_spend_time = 0;
    //     cal_cnt = 0;
    // }
    // printf("unlock on read %llx texture %d ",surface,now_read);
    // ATOMIC_UNLOCK(surface->display_texture_is_use[now_read]);
    ATOMIC_SET_UNUSED(surface->display_texture_is_use[now_read]);
}

GLuint acquire_texture_from_image(EGL_Image *image)
{

    if (image->is_lock == 1)
    {
        return 0;
    }
    // ATOMIC_LOCK(image->display_texture_is_use);
    ATOMIC_SET_USED(image->display_texture_is_use);

    glFlush();
    image->is_lock = 1;
    if (image->fbo_sync != NULL)
    {
        //最多等待8ms
        // glClientWaitSync(image->fbo_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 80000000);
        glWaitSync(image->fbo_sync, 0, GL_TIMEOUT_IGNORED);
        if (image->fbo_sync_need_delete != NULL)
        {
            glDeleteSync(image->fbo_sync_need_delete);
        }
        image->fbo_sync_need_delete = image->fbo_sync;
    }
    image->fbo_sync = NULL;

    GLuint texture = image->fbo_texture;

    return texture;
}

void init_image_texture(EGL_Image *image)
{
    if (image->fbo_texture == 0 && image->target != EGL_GL_TEXTURE_2D)
    {
        //image需要初始化，这个时候肯定有context了
        GLuint pre_vbo;
        // GLuint pre_texture;
        // GLuint pre_fbo;

        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLuint *)&pre_vbo);
        // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
        // glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo);

        glGenTextures(1, &(image->fbo_texture));
        // glGenFramebuffers(1, &(image->display_fbo));
        //egl_image不需要深度缓冲和模板缓冲

        glBindTexture(GL_TEXTURE_2D, image->fbo_texture);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, image->internal_format, image->width, image->height, 0, image->format, image->pixel_type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // glBindFramebuffer(GL_FRAMEBUFFER, image->display_fbo);
        // //附加颜色缓冲区
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->fbo_texture, 0);

        // 其中的texture其实可以不还原，因为紧接着就会读取
        // glBindTexture(GL_TEXTURE_2D, pre_texture);
        glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
        // glBindFramebuffer(GL_FRAMEBUFFER, pre_fbo);
        // printf("image %llx need init texture %u\n", image->gbuffer_id, image->fbo_texture);
    }
}

void init_image_fbo(EGL_Image *image, int need_reverse)
{
    if (image->display_fbo == 0)
    {
        glGenFramebuffers(1, &(image->display_fbo));
        glBindFramebuffer(GL_FRAMEBUFFER, image->display_fbo);
        //附加颜色缓冲区
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->fbo_texture, 0);

        if (need_reverse)
        {
            image->fbo_texture_reverse = image->fbo_texture;
            image->display_fbo_reverse = image->display_fbo;
            image->fbo_texture = 0;
            init_image_texture(image);

            glGenFramebuffers(1, &(image->display_fbo));
            glBindFramebuffer(GL_FRAMEBUFFER, image->display_fbo);
            //附加颜色缓冲区
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->fbo_texture, 0);
        }
    }
}

void release_texture_from_image(EGL_Image *image)
{

    if (image->is_lock == 0)
    {
        return;
    }

    if (image->need_reverse == 1)
    {
        image->need_reverse = 0;

        // printf("reverse eglimage gbuffer_id %llx\n", image->gbuffer_id);

        glBlitNamedFramebuffer(image->display_fbo, image->display_fbo_reverse, 0, 0, image->width, image->height, 0, image->height, image->width, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        GLuint temp_id;

        temp_id = image->display_fbo;
        image->display_fbo = image->display_fbo_reverse;
        image->display_fbo_reverse = temp_id;

        temp_id = image->fbo_texture;
        image->fbo_texture = image->fbo_texture_reverse;
        image->fbo_texture_reverse = temp_id;

        glBindTexture(GL_TEXTURE_2D, image->fbo_texture);
        glBindFramebuffer(GL_FRAMEBUFFER, image->display_fbo);
    }

    GLsync wait_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
    image->is_lock = 0;
    //延迟删除glsync，以防止waitsync后立马删除这个sync引起的屏幕闪烁问题（不确定是不是这个原因引起）
    if (image->fbo_sync != NULL)
    {
        if (image->fbo_sync_need_delete != NULL)
        {
            glDeleteSync(image->fbo_sync_need_delete);
        }
        image->fbo_sync_need_delete = image->fbo_sync;
    }
    image->fbo_sync = wait_sync;

    // ATOMIC_UNLOCK(image->display_texture_is_use);
    ATOMIC_SET_UNUSED(image->display_texture_is_use);
}

Window_Buffer *get_surface_from_gbuffer_id(uint64_t gbuffer_id)
{
    if (gbuffer_id_surface_map == NULL)
    {
        return NULL;
    }
    ATOMIC_LOCK(gbuffer_id_surface_map_lock);
    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(gbuffer_id_surface_map, (gpointer)(gbuffer_id));
    ATOMIC_UNLOCK(gbuffer_id_surface_map_lock);
    return real_surface;
}

void set_gbuffer_id_surface(uint64_t gbuffer_id, Window_Buffer *origin_surface, Window_Buffer *now_surface)
{
    if (gbuffer_id_surface_map == NULL)
    {
        return;
    }

    ATOMIC_LOCK(gbuffer_id_surface_map_lock);
    if (now_surface == NULL)
    {
        Window_Buffer *real_surface = (EGL_Image *)g_hash_table_lookup(gbuffer_id_surface_map, (gpointer)(gbuffer_id));
        if (real_surface == origin_surface)
        {
            g_hash_table_remove(gbuffer_id_surface_map, (gpointer)(gbuffer_id));
        }
    }
    else
    {
        g_hash_table_insert(gbuffer_id_surface_map, (gpointer)(gbuffer_id), (gpointer)now_surface);
    }
    ATOMIC_UNLOCK(gbuffer_id_surface_map_lock);
    
    return;
}

EGL_Image *get_image_from_gbuffer_id(uint64_t gbuffer_id)
{
    if (gbuffer_id_image_map == NULL)
    {
        return NULL;
    }
    ATOMIC_LOCK(gbuffer_id_image_map_lock);
    EGL_Image *real_image = (EGL_Image *)g_hash_table_lookup(gbuffer_id_image_map, (gpointer)(gbuffer_id));
    ATOMIC_UNLOCK(gbuffer_id_image_map_lock);
    return real_image;
}

void set_gbuffer_id_image(uint64_t gbuffer_id, EGL_Image *origin_image, EGL_Image *now_image)
{
    if (gbuffer_id_image_map == NULL)
    {
        return;
    }
    ATOMIC_LOCK(gbuffer_id_image_map_lock);
    if (now_image == NULL)
    {
        EGL_Image *real_image = (EGL_Image *)g_hash_table_lookup(gbuffer_id_image_map, (gpointer)(gbuffer_id));
        if (real_image == origin_image)
        {
            g_hash_table_remove(gbuffer_id_image_map, (gpointer)(gbuffer_id));
        }
    }
    else
    {
        g_hash_table_insert(gbuffer_id_image_map, (gpointer)(gbuffer_id), (gpointer)now_image);
    }
    ATOMIC_UNLOCK(gbuffer_id_image_map_lock);
    return;
}

void send_message_to_main_window(int message_code, void *data)
{
    Main_window_Event *event = g_malloc(sizeof(Main_window_Event));
    event->event_code = message_code;
    event->data = data;
    ATOMIC_LOCK(main_window_event_queue_lock);
    g_async_queue_push(main_window_event_queue, (gpointer)event);
    ATOMIC_UNLOCK(main_window_event_queue_lock);
}