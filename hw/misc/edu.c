/*
 * QEMU educational PCI device
 *
 * Copyright (c) 2012-2015 Jiri Slaby
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "hw/pci/pci.h"
#include "hw/hw.h"
#include "hw/pci/msi.h"
#include "qemu/timer.h"
#include "qemu/main-loop.h" /* iothread mutex */
#include "qemu/module.h"
#include "qapi/visitor.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "EGL/eglplatform.h"
#include "GLES2/gl2.h"
#include "ui/console.h"
#include "qemu/typedefs.h"
#include "windows.h"
#include "winuser.h"

#define TYPE_PCI_EDU_DEVICE "edu"
#define EDU(obj) OBJECT_CHECK(EduState, obj, TYPE_PCI_EDU_DEVICE)

#define FACT_IRQ 0x00000001
#define DMA_IRQ 0x00000100

#define DMA_START 0x40000
#define DMA_SIZE 4096

typedef struct
{
    PCIDevice pdev;
    MemoryRegion mmio;

    QemuThread thread;
    QemuMutex thr_mutex;
    QemuCond thr_cond;
    bool stopping;

    uint32_t addr4;
    uint32_t fact;
#define EDU_STATUS_COMPUTING 0x01
#define EDU_STATUS_IRQFACT 0x80
    uint32_t status;

    uint32_t irq_status;

#define EDU_DMA_RUN 0x1
#define EDU_DMA_DIR(cmd) (((cmd)&0x2) >> 1)
#define EDU_DMA_FROM_PCI 0
#define EDU_DMA_TO_PCI 1
#define EDU_DMA_IRQ 0x4
    struct dma_state
    {
        dma_addr_t src;
        dma_addr_t dst;
        dma_addr_t cnt;
        dma_addr_t cmd;
    } dma;
    QEMUTimer dma_timer;
    char dma_buf[DMA_SIZE];
    uint64_t dma_mask;
} EduState;

/* qemu-angle OpenGL */
//GLuint LoadShader ( GLenum type, const char *shaderSrc );
GLuint LoadShader1(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
    {
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
        return 0;
    }

    return shader;
}

static bool edu_msi_enabled(EduState *edu)
{
    return msi_enabled(&edu->pdev);
}

static void edu_raise_irq(EduState *edu, uint32_t val)
{
    edu->irq_status |= val;
    if (edu->irq_status)
    {
        if (edu_msi_enabled(edu))
        {
            msi_notify(&edu->pdev, 0);
        }
        else
        {
            pci_set_irq(&edu->pdev, 1);
        }
    }
}

static void edu_lower_irq(EduState *edu, uint32_t val)
{
    edu->irq_status &= ~val;

    if (!edu->irq_status && !edu_msi_enabled(edu))
    {
        pci_set_irq(&edu->pdev, 0);
    }
}

static bool within(uint64_t addr, uint64_t start, uint64_t end)
{
    return start <= addr && addr < end;
}

static void edu_check_range(uint64_t addr, uint64_t size1, uint64_t start,
                            uint64_t size2)
{
    uint64_t end1 = addr + size1;
    uint64_t end2 = start + size2;

    if (within(addr, start, end2) &&
        end1 > addr && within(end1, start, end2))
    {
        return;
    }

    hw_error("EDU: DMA range 0x%016" PRIx64 "-0x%016" PRIx64
             " out of bounds (0x%016" PRIx64 "-0x%016" PRIx64 ")!",
             addr, end1 - 1, start, end2 - 1);
}

static dma_addr_t edu_clamp_addr(const EduState *edu, dma_addr_t addr)
{
    dma_addr_t res = addr & edu->dma_mask;

    if (addr != res)
    {
        printf("EDU: clamping DMA %#.16" PRIx64 " to %#.16" PRIx64 "!\n", addr, res);
    }

    return res;
}

static void edu_dma_timer(void *opaque)
{
    EduState *edu = opaque;
    bool raise_irq = false;

    if (!(edu->dma.cmd & EDU_DMA_RUN))
    {
        return;
    }

    if (EDU_DMA_DIR(edu->dma.cmd) == EDU_DMA_FROM_PCI)
    {
        uint64_t dst = edu->dma.dst;
        edu_check_range(dst, edu->dma.cnt, DMA_START, DMA_SIZE);
        dst -= DMA_START;
        pci_dma_read(&edu->pdev, edu_clamp_addr(edu, edu->dma.src),
                     edu->dma_buf + dst, edu->dma.cnt);
    }
    else
    {
        uint64_t src = edu->dma.src;
        edu_check_range(src, edu->dma.cnt, DMA_START, DMA_SIZE);
        src -= DMA_START;
        pci_dma_write(&edu->pdev, edu_clamp_addr(edu, edu->dma.dst),
                      edu->dma_buf + src, edu->dma.cnt);
    }

    edu->dma.cmd &= ~EDU_DMA_RUN;
    if (edu->dma.cmd & EDU_DMA_IRQ)
    {
        raise_irq = true;
    }

    if (raise_irq)
    {
        edu_raise_irq(edu, DMA_IRQ);
    }
}

static void dma_rw(EduState *edu, bool write, dma_addr_t *val, dma_addr_t *dma,
                   bool timer)
{
    if (write && (edu->dma.cmd & EDU_DMA_RUN))
    {
        return;
    }

    if (write)
    {
        *dma = *val;
    }
    else
    {
        *val = *dma;
    }

    if (timer)
    {
        timer_mod(&edu->dma_timer, qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 100);
    }
}

static uint64_t edu_mmio_read(void *opaque, hwaddr addr, unsigned size)
{
    EduState *edu = opaque;
    uint64_t val = ~0ULL;

    if (addr < 0x80 && size != 4)
    {
        return val;
    }

    if (addr >= 0x80 && size != 4 && size != 8)
    {
        return val;
    }

    switch (addr)
    {
    case 0x00:
        val = 0x010000edu;
        break;
    case 0x04:
        val = edu->addr4;
        break;
    case 0x08:
        qemu_mutex_lock(&edu->thr_mutex);
        val = edu->fact;
        qemu_mutex_unlock(&edu->thr_mutex);
        break;
    case 0x20:
        val = atomic_read(&edu->status);
        break;
    case 0x24:
        val = edu->irq_status;
        break;
    case 0x80:
        dma_rw(edu, false, &val, &edu->dma.src, false);
        break;
    case 0x88:
        dma_rw(edu, false, &val, &edu->dma.dst, false);
        break;
    case 0x90:
        dma_rw(edu, false, &val, &edu->dma.cnt, false);
        break;
    case 0x98:
        dma_rw(edu, false, &val, &edu->dma.cmd, false);
        break;
    }

    return val;
}

static void edu_mmio_write(void *opaque, hwaddr addr, uint64_t val,
                           unsigned size)
{
    EduState *edu = opaque;

    if (addr < 0x80 && size != 4)
    {
        return;
    }

    if (addr >= 0x80 && size != 4 && size != 8)
    {
        return;
    }

    switch (addr)
    {
    case 0x04:
        edu->addr4 = ~val;
        break;
    case 0x08:
        if (atomic_read(&edu->status) & EDU_STATUS_COMPUTING)
        {
            break;
        }
        /* EDU_STATUS_COMPUTING cannot go 0->1 concurrently, because it is only
         * set in this function and it is under the iothread mutex.
         */
        qemu_mutex_lock(&edu->thr_mutex);
        edu->fact = val;
        atomic_or(&edu->status, EDU_STATUS_COMPUTING);
        qemu_cond_signal(&edu->thr_cond);
        qemu_mutex_unlock(&edu->thr_mutex);
        break;
    case 0x20:
        if (val & EDU_STATUS_IRQFACT)
        {
            atomic_or(&edu->status, EDU_STATUS_IRQFACT);
        }
        else
        {
            atomic_and(&edu->status, ~EDU_STATUS_IRQFACT);
        }
        break;
    case 0x60:
        edu_raise_irq(edu, val);
        break;
    case 0x64:
        edu_lower_irq(edu, val);
        break;
    case 0x80:
        dma_rw(edu, true, &val, &edu->dma.src, false);
        break;
    case 0x88:
        dma_rw(edu, true, &val, &edu->dma.dst, false);
        break;
    case 0x90:
        dma_rw(edu, true, &val, &edu->dma.cnt, false);
        break;
    case 0x98:
        if (!(val & EDU_DMA_RUN))
        {
            break;
        }
        dma_rw(edu, true, &val, &edu->dma.cmd, true);
        break;
    }
}

static const MemoryRegionOps edu_mmio_ops = {
    .read = edu_mmio_read,
    .write = edu_mmio_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 8,
    },
    .impl = {
        .min_access_size = 4,
        .max_access_size = 8,
    },

};

// struct SubWindowUserData {
//     SubWindowRepaintCallback repaint_callback;
//     void* repaint_callback_param;
// };
EGLDisplay eglDisplay;
EGLContext eglContext;
EGLSurface eglSurface;
static LRESULT CALLBACK subWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // printf("message thread id %d\n",GetCurrentThreadId());
    // ShowCursor(TRUE);
    printf("currentmessage %d\n",uMsg);
    if (uMsg == WM_DESTROY)
    {
        PostQuitMessage(0);
        // auto user_data =
        //     (SubWindowUserData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        // if (user_data && user_data->repaint_callback) {
        //     user_data->repaint_callback(user_data->repaint_callback_param);
        // }
    }
    else if (uMsg == WM_PAINT)
    {
        // eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
        // SetBkMode(GetDC(hwnd),TRANSPARENT);
        
        // eglSwapBuffers(eglDisplay, eglSurface);
    }else if(uMsg == WM_SETFOCUS){
        SetFocus(GetParent(hwnd));
        // printf("set focus\n");
        // return 0;
    }else if(uMsg == WM_MOUSEMOVE){
        // SetFocus(hwnd);
        printf("mouse move\n");
        // return 0;
    }
    else if(uMsg==WM_SIZE){
        printf("---------size change\n");
    }
    
    // else if (uMsg == WM_NCDESTROY) {
    //     SubWindowUserData* user_data =
    //         (SubWindowUserData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    //     delete user_data;
    // }
    PostMessage(GetParent(hwnd), uMsg, wParam, lParam);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static void *message_thread(void *opaque)
{

  EGLNativeDisplayType eglNativeDisplay = 0;

    EGLConfig config;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 vPosition;  \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "}                                        \n";

    char fShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
        "}                                            \n";

    int loc = 0;
    printf("thread create\n");

    Sleep(5000);
    printf("thread create\n");


     // Create EGL display connection
    eglDisplay = eglGetDisplay(eglNativeDisplay);

    if (eglDisplay == EGL_NO_DISPLAY)
        printf("shit eglDisplay %ld\n", GetLastError());

    // Initialize EGL for this display, returns EGL version
    if (!eglInitialize(eglDisplay, &majorVersion, &minorVersion))
        printf("shit eglinitialize %ld\n", GetLastError());

    {
        EGLint numConfigs = 0;
        EGLint attribList[] =
            {
                EGL_RED_SIZE, 5,
                EGL_GREEN_SIZE, 6,
                EGL_BLUE_SIZE, 5,
                EGL_ALPHA_SIZE, 0,
                EGL_DEPTH_SIZE, 24,
                EGL_STENCIL_SIZE, 0,
                EGL_SAMPLE_BUFFERS, 0,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_NONE};

        if (!eglChooseConfig(eglDisplay, attribList, &config, 1, &numConfigs))
            printf("shit eglChooseConfig %ld\n", GetLastError());

        if (numConfigs < 1)
            printf("shit numConfigs %ld\n", GetLastError());
    }


    eglSurface = eglCreateWindowSurface(eglDisplay, config, opaque, NULL);
    if (eglSurface == EGL_NO_SURFACE)
        printf("shit eglCreateWindowSurface %ld\n", GetLastError());


    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    // gl
    programObject = glCreateProgram();
    if (programObject == 0)
        printf("shit glCreateProgram %ld\n", GetLastError());

    vertexShader = LoadShader1(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader1(GL_FRAGMENT_SHADER, fShaderStr);

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
        printf("shit glGetProgramiv %ld\n", GetLastError());

    printf("drawing.");
    // int loc = 0;
    MSG msg;
    glUseProgram(programObject);
    printf("current swapbuf message thread id %d\n",GetCurrentThreadId());
    while (1)
    {
        // eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

        GLfloat vVertices[] = {
            0.0f, 1.0f, 0.0f,
            -0.5f, -1.0f, 0.0f,
            1.0f, -0.5f, 0.0f};
        glViewport(loc, 0, 640, 480);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the program object

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        eglSwapBuffers(eglDisplay, eglSurface);
        // SendMessage(opaque, WM_PAINT, 0, 0);
        loc += 1;
        loc %= 200;
        Sleep(30);
        // printf(".");
    }
}

/*
 * We purposely use a thread, so that users are forced to wait for the status
 * register.
 */
static void *edu_fact_thread(void *opaque)
{
    EGLNativeDisplayType eglNativeDisplay = 0;

    EGLConfig config;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 vPosition;  \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "}                                        \n";

    char fShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
        "}                                            \n";

    // while (render_hwnd == NULL)
    // {
    //     Sleep(300);
    //     printf(".");
    // }
    Sleep(15000);

    QemuConsole *con = qemu_console_lookup_by_index(0);
    void *render_hwnd = qemu_console_get_window_id(con);

    static const char className[] = "subWin";

    WNDCLASS wc = {};
    if (!GetClassInfo(GetModuleHandle(NULL), className, &wc))
    {
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // redraw if size changes
        wc.lpfnWndProc = &subWindowProc;               // points to window procedure
        wc.cbWndExtra = sizeof(void *);                // save extra window memory
        wc.lpszClassName = className;                  // name of window class
        wc.hCursor = LoadCursor(NULL,IDC_ARROW);
        RegisterClass(&wc);
    }

    EGLNativeWindowType render_hwnd_egl = CreateWindowEx(
        WS_EX_NOPARENTNOTIFY, // do not bother our parent window
        className,
        "sub",
        WS_POPUP|WS_EX_LAYERED,
        0, 0, 400, 400,
        render_hwnd,
        NULL,
        NULL,
        NULL);

    // WS_DISABLED

    QemuThread t;
    qemu_thread_create(&t, "edu2", message_thread,
                       render_hwnd_egl, QEMU_THREAD_JOINABLE);

    printf("\nget render_hwnd l %lx\n", render_hwnd);
    ShowWindow(render_hwnd_egl, TRUE);
    
    SetLayeredWindowAttributes(render_hwnd_egl,RGB(255,255,255), 0, LWA_ALPHA | LWA_COLORKEY);
    
    
    // EnableWindow(render_hwnd_egl, false);
    // ShowCursor(TRUE);
    // ShowCursor(TRUE);
    // ShowCursor(TRUE);

    

    // eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    // eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    // // gl
    // programObject = glCreateProgram();
    // if (programObject == 0)
    //     printf("shit glCreateProgram %ld\n", GetLastError());

    // vertexShader = LoadShader1(GL_VERTEX_SHADER, vShaderStr);
    // fragmentShader = LoadShader1(GL_FRAGMENT_SHADER, fShaderStr);

    // glAttachShader(programObject, vertexShader);
    // glAttachShader(programObject, fragmentShader);

    // glLinkProgram(programObject);

    // glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    // if (!linked)
    //     printf("shit glGetProgramiv %ld\n", GetLastError());

    // printf("drawing.");
    // int loc = 0;
    MSG msg;
    // glUseProgram(programObject);

    printf("current trans thread id %d\n",GetCurrentThreadId());
    // while (1)
    // {
        //  printf("enter message\n");
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            // printf("GetMessage %d\n", msg.message);
            // if (msg.message == WM_QUIT)
            // {
            //     break;
            // }
            // else
            // {

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            // }
        }
        // Sleep(10);
    // }
    return NULL;
}

static void pci_edu_realize(PCIDevice *pdev, Error **errp)
{
    EduState *edu = EDU(pdev);
    uint8_t *pci_conf = pdev->config;

    pci_config_set_interrupt_pin(pci_conf, 1);

    if (msi_init(pdev, 0, 1, true, false, errp))
    {
        return;
    }

    timer_init_ms(&edu->dma_timer, QEMU_CLOCK_VIRTUAL, edu_dma_timer, edu);

    qemu_mutex_init(&edu->thr_mutex);
    qemu_cond_init(&edu->thr_cond);
    qemu_thread_create(&edu->thread, "edu", edu_fact_thread,
                       edu, QEMU_THREAD_JOINABLE);

    memory_region_init_io(&edu->mmio, OBJECT(edu), &edu_mmio_ops, edu,
                          "edu-mmio", 1 * MiB);
    //Base Address Registers
    pci_register_bar(pdev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &edu->mmio);
}

static void pci_edu_uninit(PCIDevice *pdev)
{
    EduState *edu = EDU(pdev);

    qemu_mutex_lock(&edu->thr_mutex);
    edu->stopping = true;
    qemu_mutex_unlock(&edu->thr_mutex);
    qemu_cond_signal(&edu->thr_cond);
    qemu_thread_join(&edu->thread);

    qemu_cond_destroy(&edu->thr_cond);
    qemu_mutex_destroy(&edu->thr_mutex);

    timer_del(&edu->dma_timer);
    msi_uninit(pdev);
}

static void edu_instance_init(Object *obj)
{
    EduState *edu = EDU(obj);

    edu->dma_mask = (1UL << 28) - 1;
    object_property_add_uint64_ptr(obj, "dma_mask",
                                   &edu->dma_mask, OBJ_PROP_FLAG_READWRITE);
}

static void edu_class_init(ObjectClass *class, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(class);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(class);

    k->realize = pci_edu_realize;
    k->exit = pci_edu_uninit;
    k->vendor_id = PCI_VENDOR_ID_QEMU;
    k->device_id = 0x11e8;
    k->revision = 0x10;
    k->class_id = PCI_CLASS_OTHERS;
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static void pci_edu_register_types(void)
{
    static InterfaceInfo interfaces[] = {
        {INTERFACE_CONVENTIONAL_PCI_DEVICE},
        {},
    };
    static const TypeInfo edu_info = {
        .name = TYPE_PCI_EDU_DEVICE,
        .parent = TYPE_PCI_DEVICE,
        .instance_size = sizeof(EduState),
        .instance_init = edu_instance_init,
        .class_init = edu_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&edu_info);
}
type_init(pci_edu_register_types)
