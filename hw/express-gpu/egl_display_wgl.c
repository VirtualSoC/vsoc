#include "express-gpu/egl_display.h"
#include "direct-express/express_log.h"
#include <wingdi.h>

/**
 * @brief 初始化EGL configuration并将所有可用configuration保存到全局的hash表中
 * 
 * @param display 待初始化的Egl_Display
 */
void init_configs(Egl_Display *display)
{
    if (display->egl_config_set == NULL)
    {
        display->egl_config_set = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    // 驱动要求必须先ChoosePixelFormat，所以初始化一个dummy window
    HDC dummy_ctx = create_dummy_window();
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                       // 版本号
        PFD_DRAW_TO_WINDOW |     // 支持window
            PFD_SUPPORT_OPENGL | // 支持OpenGL
            PFD_DOUBLEBUFFER,    // 双缓冲
        PFD_TYPE_RGBA,           // RGBA类型
        32,                      // 32-bit颜色深度
        0, 0, 0, 0, 0, 0,        // 忽略color bits
        0,                       // 没有alpha缓冲
        0,                       // 忽略shift bit
        0,                       // 无accumulation buffer
        0, 0, 0, 0,              // 忽略accum bits
        24,                      // 24-bit z-buffer
        0,                       // 无stencil buffer
        0,                       // 无auxiliary buffer
        PFD_MAIN_PLANE,          // 主layer
        0,                       // 保留
        0, 0, 0                  // 忽略layer masks
    };
    ChoosePixelFormat(dummy_ctx, &pfd);

    int num_formats = DescribePixelFormat(dummy_ctx, 1, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    if (num_formats == 0)
    {
        express_printf("No pixel format found!");
    }

    for (int idx = 1; idx <= num_formats; idx++)
    {
        DescribePixelFormat(dummy_ctx, idx, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        parse_pixel_format(display, dummy_ctx, &pfd, idx);
    }
}

/**
 * @brief 初始化WGL Extension
 * 
 * @param display 待初始化的Egl_Display
 */
void init_wgl_extension(Egl_Display *display)
{
    if (display->wgl_ext)
        return;

    display->wgl_ext->instance = LoadLibraryA("opengl32.dll");
    if (!display->wgl_ext->instance)
    {
        express_printf("Cannot initialize opengl32.dll");
    }

    display->wgl_ext->GetPixelFormatAttribivARB = (EXWGLGETPIXELFORMATATTRIBIVARBPROC)
        wglGetProcAddress("wglGetPixelFormatAttribivARB");
}

/**
 * @brief 将PixelFormat解析为eglConfig
 * 
 * @param display 待初始化的Egl_Display
 * @param pfd PixelFormatDescriptor
 * @param id 第几个PixelFormat
 */
void parse_pixel_format(Egl_Display *display, HDC dummy_ctx, PIXELFORMATDESCRIPTOR *pfd, int id)
{
    eglConfig *config = (eglConfig *)malloc(sizeof(eglConfig));
    memset(config, 0, sizeof(config));

    if (display->wgl_ext == NULL)
    {
        init_wgl_extension(display);
    }

    if (!display->wgl_ext->GetPixelFormatAttribivARB)
    {
        express_printf("No available wglGetPixelFormatAttribivARB");
        return;
    }

    int window = 0, window_attrib = WGL_DRAW_TO_WINDOW_ARB;
    RETURN_IF_FALSE(display->wgl_ext->GetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &window_attrib, &window));

    int pbuffer = 0, pbuffer_attrib = WGL_DRAW_TO_PBUFFER_ARB;
    RETURN_IF_FALSE(display->wgl_ext->GetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &pbuffer_attrib, &pbuffer));

    config->surface_type = 0;
    if (window)
        config->surface_type |= EGL_WINDOW_BIT;
    if (pbuffer)
        config->surface_type |= EGL_PBUFFER_BIT;

    config->native_visual_id = 0;
    config->native_visual_type = EGL_NONE;
    config->caveat = EGL_FALSE;
    config->native_renderable = EGL_FALSE;
    config->renderable_type = RENDERABLE_SUPPORT;
    config->max_pbuffer_width = PBUFFER_MAX_WIDTH;
    config->max_pbuffer_height = PBUFFER_MAX_HEIGHT;
    config->max_pbuffer_size = PBUFFER_MAX_PIXELS;
    config->samples_per_pixel = 0;
    config->frame_buffer_level = 0;

    int transparent, transparent_attrib = WGL_TRANSPARENT_ARB;
    RETURN_IF_FALSE(display->wgl_ext->GetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_attrib, &transparent));
    if (transparent)
    {
        config->transparent_type = EGL_TRANSPARENT_RGB;
        int transparent_red_attrib = WGL_TRANSPARENT_RED_VALUE_ARB;
        RETURN_IF_FALSE(display->wgl_ext->GetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_red_attrib, &config->trans_red_val));

        int transparent_green_attrib = WGL_TRANSPARENT_GREEN_VALUE_ARB;
        RETURN_IF_FALSE(display->wgl_ext->GetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_green_attrib, &config->trans_green_val));

        int transparent_blue_attrib = WGL_TRANSPARENT_BLUE_VALUE_ARB;
        RETURN_IF_FALSE(display->wgl_ext->GetPixelFormatAttribivARB(dummy_ctx, id, 0, 1, &transparent_blue_attrib, &config->trans_blue_val));
    }
    else
    {
        config->transparent_type = EGL_NONE;
    }

    config->red_size = pfd->cRedBits;
    config->green_size = pfd->cGreenBits;
    config->blue_size = pfd->cBlueBits;
    config->alpha_size = pfd->cAlphaBits;
    config->depth_size = pfd->cDepthBits;
    config->stencil_size = pfd->cStencilBits;

    config->pixel_format = pfd;
    config->config_id = id;

    g_hash_table_insert(display->egl_config_set, GINT_TO_POINTER(id), (gpointer)config);
}

/**
 * @brief 创建一个dummy window用于configuration初始化
 * 
 * @return HDC 创建的dummy window
 */
HDC create_dummy_window()
{
    WNDCLASSEX wcx;
    wcx.cbSize = sizeof(wcx);                       // size of structure
    wcx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // redraw if size changes
    wcx.lpfnWndProc = dummy_window_proc;            // points to window procedure
    wcx.cbClsExtra = 0;                             // no extra class memory
    wcx.cbWndExtra = sizeof(void *);                // save extra window memory, to store VasWindow instance
    wcx.hInstance = NULL;                           // handle to instance
    wcx.hIcon = NULL;                               // predefined app. icon
    wcx.hCursor = NULL;
    wcx.hbrBackground = NULL;       // no background brush
    wcx.lpszMenuName = NULL;        // name of menu resource
    wcx.lpszClassName = "DummyWin"; // name of window class
    wcx.hIconSm = (HICON)NULL;      // small class icon

    RegisterClassEx(&wcx);

    HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                               "DummyWin",
                               "Dummy",
                               WS_POPUP,
                               0,
                               0,
                               1,
                               1,
                               NULL,
                               NULL,
                               0, 0);
    return GetDC(hwnd);
}

/**
 * @brief 用于创建dummy window的dummy procedure
 */
LRESULT CALLBACK dummy_window_proc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
    return DefWindowProc(hwnd, u_msg, w_param, l_param);
}