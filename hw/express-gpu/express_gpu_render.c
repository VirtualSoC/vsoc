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

#include "express-gpu/egl_context.h"
#include "express-gpu/glv3_context.h"

#include <winsock2.h>
#include <windows.h>

#include "ui/console.h"

HWND draw_native_window;

#ifdef _WIN32
static HANDLE swap_event;
#else
static void *swap_event;
#endif

static unsigned int main_frame_num;

static int event_queue_lock;
static GQueue *event_queue;

#define EVENT_QUEUE_LOCK                                   \
    while (atomic_cmpxchg(&(event_queue_lock), 0, 1) == 1) \
        ;

#define EVENT_QUEUE_UNLOCK atomic_cmpxchg(&(event_queue_lock), 1, 0);

// static void *opengl_render_hwnd = NULL;

static GLFWwindow *glfw_window = NULL;

static GLint programID = 0;
static GLint drawVAO = 0;

static long window_width;
static long window_height;

static Double_Buffer *compose_surface;

volatile int native_render_run = 0;

static void opengl_paint(Double_Buffer *d_buffer);
static void egl_surface_create(Double_Buffer *d_buffer);

static void g_queue_event_notify(gpointer data, gpointer user_data);
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
        PostMessage(GetParent(hwnd), uMsg, wParam, lParam);
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
        temp_height = rcParent.bottom / 2;
        temp_width = rcParent.right / 2;
        express_printf("windows size %d %d\n", temp_height, temp_width);
        // if (rcParent.bottom * 4 > rcParent.right * 3){
        //     y = (rcParent.bottom - rcParent.right * 3.0 / 4.0) / 2;
        //     height = width * 3.0 / 4.0;
        // } else {
        //     x = (rcParent.right - rcParent.bottom * 4.0 / 3.0) / 2;
        //     width = height * 4.0 / 3.0;
        // }
        if (temp_height != window_height)
        {
            window_height = temp_height;

            // window_width = temp_width;
            window_width = temp_height;

            MoveWindow(hwnd, (int)(rcParent.right - window_height), (int)(temp_height * 0.5), window_width, window_height, FALSE);
            // MoveWindow(hwnd, 0, 0, window_width, window_height, FALSE);
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
        {

            Double_Buffer *d_buffer = (Double_Buffer *)lParam;
            if (d_buffer == NULL)
            {
                break;
            }
            express_printf("create window %lx\n",d_buffer);
            egl_surface_create(d_buffer);
        }

        break;
    case WM_USER_SURFACE_DESTROY:
    {
        Double_Buffer *d_buffer = (Double_Buffer *)lParam;
        if (d_buffer == NULL)
        {
            break;
        }
        express_printf("main windows destroy window %lx\n",d_buffer);
        glfwDestroyWindow(d_buffer->window);

        //destroywindows后，fbo会自动被删除，因为它不共享
        // glDeleteFramebuffers(surface->buffer_num, surface->display_fbo);

        glDeleteTextures(d_buffer->buffer_num, d_buffer->fbo_texture);
        glDeleteRenderbuffers(d_buffer->buffer_num, d_buffer->display_rbo);
        g_free(d_buffer);
    }
        break;
    case WM_USER_CONTEXT_DESTROY:
    {
        Opengl_Context *opengl_context = (Opengl_Context *)lParam;
        if (opengl_context == NULL)
        {
            break;
        }
        opengl_context_destroy(opengl_context);
        g_free(opengl_context);
    }
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

    //开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programObject);

    glClearColor(1, 1, 1, 1);

    return 1;
}

/**
 * @brief 界面上用于画出图像的函数，实际逻辑为取出d_buffer中的display_texture，然后画出来
 * 
 * @param d_buffer 
 */
static void opengl_paint(Double_Buffer *d_buffer)
{
    glClear(GL_COLOR_BUFFER_BIT);
    // glClearColor(1, 1, 1, 0);
    glViewport(0, 0, window_width, window_height);
    // glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(0, 0, 1, 0);
    // glDisable(GL_DEPTH_TEST);
    int now_read = d_buffer->now_read;

    TEXTURE_LOCK(d_buffer->display_texture_is_use[now_read]);
    // glWaitSync(double_buffer->dispaly_sync, 0, GL_TIMEOUT_IGNORED);
    // express_printf("main has error %x\n",glGetError());
    if (d_buffer->fbo_sync[now_read] != NULL)
    {
        //最多等待8ms
        glClientWaitSync(d_buffer->fbo_sync[now_read], GL_SYNC_FLUSH_COMMANDS_BIT, 8000000);
    }
    // glDeleteSync(double_buffer->dispaly_sync);

    GLuint texture = d_buffer->fbo_texture[now_read];

    // glBindVertexArray(drawVAO);

    glBindTexture(GL_TEXTURE_2D, texture);

    express_printf("main window paint texture %u\n", texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glFinish();

    TEXTURE_UNLOCK(d_buffer->display_texture_is_use[now_read]);
}

/**
 * @brief 创建opengl的context，这个创建过程是在主界面线程中进行的，通过消息机制来实现
 * 
 * @param d_buffer 需要创建context的双缓冲区，创建完成后会直接存入其中
 * @param width 界面的宽
 * @param height 界面的高
 */
static void egl_surface_create(Double_Buffer *d_buffer)
{

    GLFWwindow *child_window;
    static int cnt = 0;
    char name[100];
    sprintf(name, "opengl-child-window%d", cnt);
    cnt++;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
// glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

//屏幕分离调试专用
#ifdef DEBUG_INDEPEND_WINDOW
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    child_window = glfwCreateWindow(width, height, name, NULL, NULL);

#else
    child_window = glfwCreateWindow(d_buffer->width, d_buffer->height, name, NULL, glfw_window);
    if(child_window==NULL){
        char *s;
        int ret=glfwGetError(&s);
        express_printf("error code %d detail %s",ret,s);
    }
    
    assert(child_window!=NULL);
#endif
    d_buffer->window = child_window;
    
    express_printf("create windows surface %lx",d_buffer);
    //todo 根据配置设置窗口属性
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

    event_queue = g_queue_new();

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
    glBindVertexArray(drawVAO);

    express_printf("native windows create!\n");
    native_render_run = 2;

    glfwSwapInterval(1);
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

        // glfwWaitEvents();

        glfwPollEvents();

        main_frame_num = (main_frame_num + 1) % 65536;

        EVENT_QUEUE_LOCK;
        g_queue_foreach(event_queue, g_queue_event_notify, NULL);
        g_queue_clear(event_queue);
        EVENT_QUEUE_UNLOCK;
        if (compose_surface != NULL)
        {
            opengl_paint(compose_surface);
        }

        glfwSwapBuffers(glfw_window);
    }

    express_printf("native windows close!\n");

    //当他返回0时表示窗口被关掉了
    native_render_run = 0;
    // qemu_thread_join(&t);
    return NULL;
}

int draw_wait_GSYNC(HANDLE event, int wait_frame_num)
{
    EVENT_QUEUE_LOCK;
    g_queue_push_tail(event_queue, (gpointer)event);
    EVENT_QUEUE_UNLOCK;

#ifdef _WIN32
    //最多等待17ms，为一帧
    WaitForSingleObject(swap_event, 17);
#elif
#endif
    //这里wait_frame_num溢出的时候，wait很小，但是main很大
    //这种时候直接不进循环里去，因为初始情况也是wait_frame很小
    while (main_frame_num < wait_frame_num && wait_frame_num > 5)
    {
        //再次等待，直到跳过的frame_num对着了
        EVENT_QUEUE_LOCK;
        g_queue_push_tail(event_queue, (gpointer)event);
        EVENT_QUEUE_UNLOCK;
#ifdef _WIN32
        WaitForSingleObject(swap_event, 17);
#elif
#endif
    }
    return main_frame_num;
}

static void g_queue_event_notify(gpointer data, gpointer user_data)
{
    SetEvent((HANDLE)data);
    return;
}

void render_windows_create(Double_Buffer *context)
{

    // Render_Thread_Context *render_context = (Render_Thread_Context *)context;
    // Double_Buffer *buffer_context = (render_context->render_double_buffer);
    // Opengl_Context *opengl_context = (render_context->opengl_context);

    if (context != NULL)
    {
        //send是同步的，发送完消息需要等待消息处理完
        //调用egl_context_create
        SendMessage(draw_native_window, WM_USER_CREATE, 0, (LPARAM)context);
    }

    return;
}

void set_compose_surface(Double_Buffer *surface)
{
    compose_surface = surface;
}