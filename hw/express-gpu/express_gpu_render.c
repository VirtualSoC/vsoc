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
#define STD_DEBUG_LOG
#include "qemu/osdep.h"
#include "qemu/atomic.h"

#include "express-gpu/express_gpu_render.h"

#include "direct-express/direct_express.h"
#include "direct-express/express_log.h"

#include <winsock2.h>
#include <windows.h>

#include "ui/console.h"

HWND draw_native_window;

// static void *opengl_render_hwnd = NULL;

static GLFWwindow *glfw_window = NULL;

static GLint programID = 0;
static GLint drawVAO = 0;

static long window_width;
static long window_height;

volatile int native_render_run = 0;

static void opengl_paint(Double_Buffer *d_buffer);
static void egl_context_create(Double_Buffer *d_buffer, int width, int height);

/**
 * @brief 子窗口的消息处理函数，会将鼠标点击等操作直接传递给底层的窗口，并且接受来自draw线程的界面重新绘制消息以及生成context消息，并进行一定的反应
 * 
 * @param hwnd 窗口的hwnd
 * @param uMsg 消息编号
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
static LRESULT CALLBACK subWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RECT rcParent;
    long temp_width, temp_height;
    switch (uMsg)
    {
    /******* Relay message to parent window *******/
    /* mouse moving */
    case WM_NCHITTEST:  /* 132 */
    case WM_SETCURSOR:  /* 32 */
    case WM_MOUSEFIRST: /* 512 */
    /* mouse clicking */
    case WM_MOUSEACTIVATE:              /* 33 */
    case WM_LBUTTONDOWN: /* 513 down */ //左键按下
    case WM_LBUTTONUP: /* 514 up */     //左键释放
    case WM_CAPTURECHANGED:
    case WM_APPCOMMAND:
    case WM_NCXBUTTONDBLCLK:
    case WM_NCXBUTTONDOWN:
    case WM_NCXBUTTONUP:
    case WM_LBUTTONDBLCLK: //左键双击
    case WM_MBUTTONDBLCLK: //中键双击
    case WM_MBUTTONDOWN:   //中键按下
    case WM_MBUTTONUP:     //中键释放
    case WM_RBUTTONDBLCLK: //右键双击
    case WM_RBUTTONDOWN:   //右键按下
    case WM_RBUTTONUP:     //右键释放
    case WM_XBUTTONDBLCLK: //X 键双击
    case WM_XBUTTONDOWN:   //X 键按下
    case WM_XBUTTONUP:     //X 键释放
    case WM_MOUSEWHEEL:    //滚滚轮
        //鼠标事件都要传输给父窗口
        SendMessage(GetParent(hwnd), uMsg, wParam, lParam);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SETFOCUS:
        //焦点也需要给父窗口
        SetFocus(GetParent(hwnd));
        break;

    /******* Creating child window *******/
    case WM_NCCREATE:         /* 129 */
    case WM_NCCALCSIZE:       /* 131 */
    case WM_CREATE:           /* 1 */
    case WM_SIZE:             /* 5 */
    case WM_MOVE:             /* 3 */
    case WM_SHOWWINDOW:       /* 24 */
    case WM_NCPAINT:          /* 133 */
    case WM_ERASEBKGND:       /* 20 */
    case WM_WINDOWPOSCHANGED: /* 71 */
    case WM_PAINT:            /* 15 */
        /******* Resizing window *******/
        /* WM_NCCALCSIZE: 131 */
        /* WM_WINDOWPOSCHANGED: 71 */
        /* WM_MOVE: 3 */
        /* WM_SIZE: 5 */

    case WM_WINDOWPOSCHANGING: /* 70 */
        /******* Resizing window *******/
        //所有重画的操作需要看看窗口大小需不需要重新调整
        GetClientRect(GetParent(hwnd), &rcParent);
        temp_height = rcParent.bottom / 4;
        temp_width = rcParent.right / 4;

        // if (rcParent.bottom * 4 > rcParent.right * 3){
        //     y = (rcParent.bottom - rcParent.right * 3.0 / 4.0) / 2;
        //     height = width * 3.0 / 4.0;
        // } else {
        //     x = (rcParent.right - rcParent.bottom * 4.0 / 3.0) / 2;
        //     width = height * 4.0 / 3.0;
        // }
        if (temp_height != window_height || temp_width != window_width)
        {
            window_height = temp_height;
            window_width = temp_width;
            MoveWindow(hwnd, 0, 0, window_width, window_height, FALSE);
        }

        break;
    case WM_USER_PAINT:
        //图层合成线程发来的需要渲染到界面的消息
        //express_printf("start render\n");
        // glClear(GL_COLOR_BUFFER_BIT);
        // glClearColor(wParam, 1, 0, 0);

        // glfwSwapBuffers(glfw_window);
        opengl_paint((Double_Buffer *)lParam);
        break;
    case WM_USER_CREATE:
        /**
         * @todo 修改窗口拉伸的逻辑，保证拉伸满足相应的比例关系
         * 
         */

        //context只能是由父线程创建，以进行资源共享
        // GetClientRect(GetParent(hwnd), &rcParent);
        // window_height = rcParent.bottom / 2;
        // window_width = rcParent.right / 2;

        egl_context_create((Double_Buffer *)lParam, window_width, window_height);

        break;

    default:
        //express_printf("child win msg: %d\n", uMsg);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/**
 * @brief 主窗口绘制时使用的着色器加载的代码
 * 
 * @param type 着色器类型
 * @param shaderSrc 着色器源码
 * @return GLuint 返回着色器编号，若为0则生成失败
 */
static GLuint LoadShader1(GLenum type, const char *shaderSrc)
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

        "out vec2 TexCoords;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);\n"
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

    GLuint vertexShader = LoadShader1(GL_VERTEX_SHADER, vShaderStr);
    GLuint fragmentShader = LoadShader1(GL_FRAGMENT_SHADER, fShaderStr);
    //express_printf("shader %d %d\n", vertexShader, fragmentShader);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programObject);

    return 1;
}

/**
 * @brief 界面上用于画出图像的函数，实际逻辑为取出d_buffer中的display_texture，然后画出来
 * 
 * @param d_buffer 
 */
static void opengl_paint(Double_Buffer *d_buffer)
{

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    // glClearColor(1, 1, 1, 0);
    glViewport(0, 0, d_buffer->width, d_buffer->height);
    // glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(0, 0, 1, 0);
    glDisable(GL_DEPTH_TEST);
    // express_printf("main has error %x\n",glGetError());

    //绘制这个texture时，要get后release，保证这个texture上的东西的确已经画出来了
    GLuint texture = get_display_texture(d_buffer);
    // express_printf("main has error %x\n",glGetError());

    glBindTexture(GL_TEXTURE_2D, texture);

    // express_printf("main has error %x\n",glGetError());

    express_printf("main window paint texture %u\n", texture);

    glBindVertexArray(drawVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // express_printf("main has error %x\n",glGetError());

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    release_display_texture(d_buffer);

    // express_printf("main has error %x\n",glGetError());
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // glClearColor(0, 0, 1, 1);
    // glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(glfw_window);
}

/**
 * @brief 创建opengl的context，这个创建过程是在主界面线程中进行的，通过消息机制来实现
 * 
 * @param d_buffer 需要创建context的双缓冲区，创建完成后会直接存入其中
 * @param width 界面的宽
 * @param height 界面的高
 */
static void egl_context_create(Double_Buffer *d_buffer, int width, int height)
{
    if (d_buffer->has_init)
    {
        return 0;
    }

    GLFWwindow *child_window;
    static int cnt = 0;
    char name[100];
    sprintf(name, "opengl-child-window%d", cnt);
    cnt++;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    child_window = glfwCreateWindow(width, height, name, NULL, glfw_window);
    d_buffer->window = child_window;
    d_buffer->width = width;
    d_buffer->height = height;

    //趁着这个时候有长宽，先把texture生成了，因为这个时候是在主线程里，所以有主线程的context，直接生成没事
    // glGenTextures(1, &d_buffer->fbo_texture_display);

    // glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture_display);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glGenTextures(1, &d_buffer->fbo_texture_draw);

    // glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture_draw);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glBindTexture(GL_TEXTURE_2D, 0);

    d_buffer->has_init = 1;
}

/**
 * @brief 创建双缓冲区，注意：这个操作只能在draw子线程中进行，并且在创建了context之后
 * 
 * @param d_buffer 需要创建的双缓冲区
 * @return int 返回1则创建成功，返回0则创建失败 
 */
int egl_context_make_current(Double_Buffer *d_buffer)
{
    if (d_buffer->window == NULL || !d_buffer->has_init)
    {
        return 0;
    }
    glfwMakeContextCurrent(d_buffer->window);

    glGenTextures(1, &d_buffer->fbo_texture_display);

    glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture_display);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d_buffer->width, d_buffer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &d_buffer->fbo_texture_draw);

    glBindTexture(GL_TEXTURE_2D, d_buffer->fbo_texture_draw);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d_buffer->width, d_buffer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    //framebuf只能在子线程中完成
    glGenFramebuffers(1, &d_buffer->fbo_display);
    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->fbo_display);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture_display, 0);

    glGenFramebuffers(1, &d_buffer->fbo_draw);
    glBindFramebuffer(GL_FRAMEBUFFER, d_buffer->fbo_draw);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_buffer->fbo_texture_draw, 0);

    //这句不能有，不然第一帧黑屏
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 1;
}

int egl_context_destroy(Double_Buffer *d_buffer)
{
    express_printf("egl context destroy\n");

    if (!d_buffer->has_init)
    {
        return 0;
    }
    // express_printf("delete fbo_display\n");

    glDeleteFramebuffers(1, &d_buffer->fbo_display);
    glDeleteFramebuffers(1, &d_buffer->fbo_draw);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteTextures(1, &d_buffer->fbo_texture_display);
    glDeleteTextures(1, &d_buffer->fbo_texture_draw);

    glDeleteSync(d_buffer->dispaly_sync);

    express_printf("windows destroy\n");

    glfwDestroyWindow(d_buffer->window);
    d_buffer->window = NULL;
    d_buffer->has_init = 0;
    return 1;
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
    HWND render_hwnd = (HWND)qemu_console_get_window_id(con);

    RECT rcParent;

    GetClientRect(render_hwnd, &rcParent);

    //初始化glfw
    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    express_printf("window width %ld, height %ld\n", rcParent.right, rcParent.bottom);

    //创建一个窗口，这个window也是context
    glfw_window = glfwCreateWindow(rcParent.right / 4, rcParent.bottom / 4, "opengl window", NULL, NULL);
    if (!glfw_window)
    {
        express_printf("create window error %x\n", glfwGetError(NULL));

        glfwTerminate();
        return NULL;
    }

    draw_native_window = glfwGetWin32Window(glfw_window);

    SetParent(draw_native_window, render_hwnd);
    SetWindowLong(draw_native_window, GWL_STYLE, WS_CHILD);
    SetWindowLongPtr(draw_native_window, GWLP_WNDPROC, (LONG_PTR)&subWindowProc);
    ShowWindow(draw_native_window, TRUE);

    glfwMakeContextCurrent(glfw_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        express_printf("load glad error\n");
        return NULL;
    }

    opengl_prepare(&programID, &drawVAO);

    express_printf("native windows create!\n");
    native_render_run = 2;

    // RECT rcParent;
    // long height, width;

    // GetClientRect(render_hwnd, &rcParent);
    // height = rcParent.bottom / 10;
    // width = rcParent.right / 10;

    // static const char className[] = "openglWin";

    // WNDCLASS wc = {};
    // if (!GetClassInfo(GetModuleHandle(NULL), className, &wc))
    // {
    //     wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // redraw if size changes
    //     wc.lpfnWndProc = &subWindowProc;               // points to window procedure
    //     wc.cbWndExtra = sizeof(void *);                // save extra window memory
    //     wc.lpszClassName = className;                  // name of window class
    //     wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //     RegisterClass(&wc);
    // }

    //express_printf("create child window\n");

    // opengl_render_hwnd = CreateWindowEx(
    //     WS_EX_NOPARENTNOTIFY, // do not bother our parent window
    //     className,
    //     "opengl",
    //     WS_CHILD,
    //     0, 0, width, height,
    //     render_hwnd,
    //     NULL,
    //     NULL,
    //     NULL);

    // ShowWindow(opengl_render_hwnd, TRUE);

    // SetBkMode()
    // QemuThread t;
    //此时创建用于解码的线程
    // qemu_thread_create(&t, "render", opengl_render_thread,
    //                    vdev, QEMU_THREAD_JOINABLE);

    // MSG msg;
    // while (GetMessage(&msg, NULL, 0, 0) > 0)
    // {
    //     //创建窗口的线程需要循环处理消息
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }

    // int a = 1;
    while (!glfwWindowShouldClose(glfw_window) && native_render_run == 2)
    {
        //express_printf("start render\n");
        // glClear(GL_COLOR_BUFFER_BIT);
        // glClearColor(1, 1, 0, 0);

        // glfwSwapBuffers(glfw_window);

        // PostMessage ( native_window, WM_USER_PAINT, a, 0 );
        // a=(a^1);

        glfwWaitEvents();
    }

    express_printf("native windows close!\n");

    //GetMessage为阻塞函数，当他返回0时表示窗口被关掉了
    native_render_run = 0;
    // qemu_thread_join(&t);
    return NULL;
}

/**
 * @brief 
 * 
 * @param double_buffer 
 */
void egl_swap_buffer(Double_Buffer *double_buffer)
{

    //这里也使用GPU等待是因为GPU那边画完了之后，这边才能在交换的新的东西上画
    TEXTURE_LOCK(double_buffer->display_texture_is_use);
    if (double_buffer->dispaly_sync != NULL)
    {
        glWaitSync(double_buffer->dispaly_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000000);
        glDeleteSync(double_buffer->dispaly_sync);
        double_buffer->dispaly_sync = NULL;
    }

    //交换FBO
    GLuint temp = double_buffer->fbo_draw;
    double_buffer->fbo_draw = double_buffer->fbo_display;
    double_buffer->fbo_display = temp;

    //交换Texture
    temp = double_buffer->fbo_texture_draw;
    double_buffer->fbo_texture_draw = double_buffer->fbo_texture_display;
    double_buffer->fbo_texture_display = temp;

    //交换了之后设定一个sync
    double_buffer->dispaly_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    TEXTURE_UNLOCK(double_buffer->display_texture_is_use);

    express_printf("swap framebuffer buffer\n");

    render_bind_frame_buffer(double_buffer);
    // express_printf("main has error %x\n",glGetError());

    // express_printf("main has error %x\n",glGetError());
}

/**
 * @brief 绑定当前应该是正在画的FrameBuffer，一般调用于绑定0号FrameBuffer中
 * 
 * @param double_buffer 双缓冲指针
 */
void render_bind_frame_buffer(Double_Buffer *double_buffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, double_buffer->fbo_draw);
}

/**
 * @brief 获得当前可以展示的texture，由于不一定画完了，因此可能需要等待。不过由于是gpu要使用这个texture，所以是gpu去等待前面的画完，然后才是去绑定
 * 并且会锁定当前的texture
 * @todo 确定gpu等待是可行的
 * 
 * @param double_buffer 
 * @return GLint 
 */
GLuint get_display_texture(Double_Buffer *double_buffer)
{
    TEXTURE_LOCK(double_buffer->display_texture_is_use);
    glWaitSync(double_buffer->dispaly_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000000);
    // express_printf("main has error %x\n",glGetError());

    glDeleteSync(double_buffer->dispaly_sync);
    // express_printf("main has error %x\n",glGetError());

    double_buffer->dispaly_sync = NULL;
    return double_buffer->fbo_texture_display;
}

/**
 * @brief 释放已经使用了的texture，这里会释放锁，同时建立sync栏杆，方便之后交换FBO时，等待这边画完
 * 
 * @param double_buffer 
 */
void release_display_texture(Double_Buffer *double_buffer)
{
    double_buffer->dispaly_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    TEXTURE_UNLOCK(double_buffer->display_texture_is_use);
}