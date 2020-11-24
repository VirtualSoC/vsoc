/**
 * @file my_gpu_render.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 包括opengl绘制的窗口及其线程，opengl取指线程，使用轮询实现取指
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "mygpu/my_gpu_render.h"

#include <EGL/eglplatform.h>

#include <winsock2.h>
#include <windows.h>

#include "ui/console.h"

#include "mygpu/egl_trans.h"
#include "mygpu/glv1_trans.h"
#include "mygpu/glv32_trans.h"



//这是VirtQueueElement里面的实际东西
// typedef struct VirtQueueElement
// {
//     //elem的翻译
//     unsigned int index; // header
//     unsigned int len;  //packe模式用的
//     unsigned int ndescs; //消耗的desc数目
//     unsigned int out_num; //out数组长度
//     unsigned int in_num;  //in数组长度
//     hwaddr *in_addr;  //desc中放的实际值（物理地址）
//     hwaddr *out_addr; //
//     struct iovec *in_sg;
//     struct iovec *out_sg; //实际地址和长度
// } VirtQueueElement;

static void *opengl_render_hwnd = NULL;
static int render_run = 0;

/**
 * @brief 打印debug消息，需要标号为0，并且只有一个指针参数
 * 
 * @param call 
 */
static void virtio_printf(MYGPU_Opengl_Call *call)
{
    if (call->para_num == 1)
    {
        printf("call message: %s\n", (char *)call->elem_tail->para);
    }
    return;
}



/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 * 
 * @param call 
 */
static void decode_invoke(MYGPU_Opengl_Call *call)
{

    if (call->opengl_id == 0)
    {
        //opengl_id为0表示debug输出消息
        virtio_printf(call);
    }
    else if (call->opengl_id < 1000)
    {
        egl_decode_invoke(call);
    }
    else if(call->opengl_id<2000){
        //opengl1.0不兼容任何东西，所以单独列出来
        gl1_decode_invoke(call);
    }
    else{
        //由于现阶段3.2版本的opengl能兼容2.0,3.0,3.1，所以暂时先这样，出了事情再说
        gl32_decode_invoke(call);
    }
    call->callback(call);
    return;
}

/**
 * @brief 根据iov获得一个连续内存块，内存块内数据为iov数组的复制结果，这个函数一般用于有多个iov时
 * 
 * @param sg iov存放的位置
 * @param num iov的数目
 * @param all_cnt 需要回写的变量，表示到底复制了多少数据
 * @return void* 返回内存块的指针
 */
static void *alloc_buf_from_iov(struct iovec *sg, unsigned int num, size_t *all_cnt)
{
    int buf_len = 0;
    for (int i = 0; i < num; i++)
    {
        buf_len += sg[i].iov_len;
    }
    /**
     * @todo 使用内存映射来实现buf
     * 
     */

    void *buf = g_malloc(buf_len);
    size_t ret = iov_to_buf(sg, num, 0, buf, buf_len);
    if (ret != buf_len)
    {
        printf("error iov_to_buf\n");
        g_free(buf);
        return NULL;
    }
    *all_cnt = ret;
    return buf;
}

/**
 * @brief 检查调用的fun_id与参数数目的对应关系是否满足，防止瞎传
 * 
 * @param id 调用的函数id
 * @param num 参数的数目
 * @return int 返回检查结果是否正确，1表示正确，0表示错误
 */
static int check_fun_id_para_num(int id, int num)
{
    printf("mygpu get invoke %d %d\n", id, num);
    return 1;
    //todo
}

/**
 * @brief 将MYGPU_Queue_Elem内的数据填充完毕，也就是初始化MYGPU_Queue_Elem中除了
 * VirtQueueElement的其他部分，例如para指针，type类型等
 * 
 * @param elem 需要填充的elem数据
 * @param id 需要回传的函数调用id（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @param num 需要回传的参数数目（假如有的话），不需要则设为NULL（只有第一个elem需要）
 * @return int 返回填充是否完成，1表示完成，0表示失败
 */
static int fill_mygpu_queue_elem(MYGPU_Queue_Elem *elem, int *id, int *thread_id, size_t *num)
{
    VirtQueueElement *v_elem = &elem->elem;
    if ((v_elem->out_num != 0 && v_elem->in_num != 0) || (v_elem->out_num == 0 && v_elem->in_num == 0))
    {
        return 0;
    }
    elem->para = NULL;
    elem->next = NULL;
    elem->type = 0;
    //sg数目多余1个就要单独复制出来
    if (v_elem->out_num > 1)
    {
        elem->para = alloc_buf_from_iov(v_elem->out_sg, v_elem->out_num, &elem->len);
        elem->type |= COPY_PARA;
    }
    else if (v_elem->out_num == 1)
    {
        //只有一个sg的情况下直接传递指针
        elem->para = v_elem->out_sg->iov_base;
        elem->len = v_elem->out_sg->iov_len;
        elem->type |= DIRECT_PARA;
    }

    if (v_elem->in_num > 1)
    {
        elem->para = alloc_buf_from_iov(v_elem->in_sg, v_elem->in_num, &elem->len);
        elem->type |= COPY_PARA;
    }
    else if (v_elem->in_num == 1)
    {
        elem->para = v_elem->in_sg->iov_base;
        elem->len = v_elem->in_sg->iov_len;
        elem->type |= DIRECT_PARA;
    }
    if (elem->para == NULL)
    {
        return 0;
    }
    if (id != NULL && num != NULL && thread_id != NULL)
    {
        //在设置了id和num指针的情况下才传出数据
        //这种情况还要先检查是不是in_buf
        if (v_elem->in_num != 1 || v_elem->out_num != 0)
        {
            return 0;
        }
        MYGPU_Flag_Buf *flag_buf = (MYGPU_Flag_Buf *)elem->para;
        *id = flag_buf->id;
        *num = flag_buf->para_num;
        *thread_id = flag_buf->thread_id;
        if (!check_fun_id_para_num(*id, *num))
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief 在渲染线程使用完数据后的回调函数，主要用于内存的会写和释放，以及反馈给guest
 * 
 * @param call 
 */
static void push_free_callback(MYGPU_Opengl_Call *call)
{

    printf("enter callback\n");
    /**
     * @todo 更换复制方式
     * 
     */
    MYGPU_Queue_Elem *temp;
    for (temp = call->elem_header; temp != NULL; temp = temp->next)
    {
        //RET_PARA由调用者修改
        if (((temp->type) & RET_PARA) != 0)
        {
            //该参数需要将数据写回去
            printf("need copy back\n");
            int loc = 0;
            for (int i = 0; i < temp->elem.in_num; i++)
            {
                memcpy(temp->elem.in_sg->iov_base, temp->para + loc, temp->elem.in_sg->iov_len);
                loc += temp->elem.in_sg->iov_len;
            }
        }
    }

    //告知guest数据都消耗完了，调用完成，让guest唤醒休眠的线程
    VirtQueue *vq = call->vq;
    VIRTIO_ELEM_PUSH_ALL(vq, MYGPU_Queue_Elem, call->elem_header, 1, next);
    MYGPU_QUEUE_ELEMS_FREE(call->elem_header);
    virtio_notify(VIRTIO_DEVICE(call->vdev), vq);
    printf("notify\n");
}

/**
 * @brief 从queue中打包出一个opengl调用
 * 
 * @param vq 
 * @return MYGPU_Opengl_Call* 返回为NULL表示queue中没有数据，或者有数据但是数据不对
 */
static MYGPU_Opengl_Call *pack_call_from_queue(VirtQueue *vq)
{

    MYGPU_Queue_Elem *elem;

    MYGPU_Opengl_Call *opengl_call;

    size_t para_num;
    int opengl_id;
    int thread_id;

    elem = virtqueue_pop(vq, sizeof(MYGPU_Queue_Elem));
    while (elem)
    {

        // //debug test
        // {
        //     fill_mygpu_queue_elem(elem, &opengl_id, &para_num);
        //     // printf("fill ok %d\n",elem==&elem->elem);
        //     // virtqueue_push(vq, elem, 1);
        //     // for (MYGPU_Queue_Elem *a = (MYGPU_Queue_Elem *)(elem); a != NULL; a = a->next)
        //     // {
        //     //     virtqueue_push(vq, a, 1);
        //     // }
        //     VIRTIO_ELEM_PUSH_ALL(vq, MYGPU_Queue_Elem, elem, 1, next);
        //     printf("push\n");
        //     MYGPU_QUEUE_ELEMS_FREE(elem);
        //     // virtio_notify(VIRTIO_DEVICE(vdev), vq);
        //     printf("pop next\n");
        //     // return;
        //     elem = virtqueue_pop(vq, sizeof(MYGPU_Queue_Elem));
        //     fill_mygpu_queue_elem(elem, NULL, NULL);
        //     printf("mygpu get user data %s\n", elem->para);
        //     VIRTIO_ELEM_PUSH_ALL(vq, MYGPU_Queue_Elem, elem, 1, next);
        //     MYGPU_QUEUE_ELEMS_FREE(elem);
        //     virtio_notify(VIRTIO_DEVICE(vdev), vq);
        //     printf("notify\n");
        //     elem = virtqueue_pop(vq, sizeof(MYGPU_Queue_Elem));
        //     continue;
        // }

        if (unlikely(fill_mygpu_queue_elem(elem, &opengl_id, &thread_id, &para_num) == 0))
        {
            //第一个elem检查出错，说明不是一个调用，因此将这个elem释放掉，然后继续获取下一个
            VIRTIO_ELEM_PUSH_ALL(vq, MYGPU_Queue_Elem, elem, 1, next);
            MYGPU_QUEUE_ELEMS_FREE(elem);

            return NULL;
        }

        opengl_call = g_malloc(sizeof(MYGPU_Opengl_Call));
        opengl_call->elem_header = elem;
        opengl_call->elem_tail = elem;
        opengl_call->vq = vq;

        opengl_call->para_num = para_num;
        opengl_call->opengl_id = opengl_id;
        opengl_call->thread_id = thread_id;

        opengl_call->next = NULL;

        //会有para_num个传入参数，这些elem本应该都是out类型
        for (int i = 0; i < para_num; i++)
        {
            elem = virtqueue_pop(vq, sizeof(MYGPU_Queue_Elem));
            if (unlikely(elem == NULL || elem->elem.in_num != 0 || elem->elem.out_num == 0 || fill_mygpu_queue_elem(elem, NULL, NULL, NULL) == 0))
            {
                //要么是数据复制有问题，要么是这个elem是个in的类型，破坏了调用结构
                //因此将已经保存的数据抛弃，将这个elem作为第一个elem重新尝试fill，所以是break后continue
                VIRTIO_ELEM_PUSH_ALL(vq, MYGPU_Queue_Elem, opengl_call->elem_header, 1, next);
                MYGPU_QUEUE_ELEMS_FREE(opengl_call->elem_header);
                g_free(opengl_call);
                opengl_call = NULL;
                break;
            }
            opengl_call->elem_tail->next = elem;
            opengl_call->elem_tail = elem;
        }
        //不让用goto就得break后continue
        if (opengl_call == NULL)
        {
            continue;
        }

        //返回call，elem的空间释放由call里面的回调函数实现，call结构体由渲染线程释放
        return opengl_call;
    }

    return NULL;
}

/**
 * @brief 临时的debug调用函数，后续不需要
 * 
 * @param call 
 */
static void opengl_invoke(MYGPU_Opengl_Call *call)
{
    printf("opengl invoke\n");
    int cnt1 = 0;
    for (MYGPU_Queue_Elem *i = call->elem_header; i != NULL; i = i->next)
    {
        cnt1++;
        if (cnt1 == 1)
        {
            MYGPU_Flag_Buf *flag_buf = (MYGPU_Flag_Buf *)i->para;
            flag_buf->ret = 2;

            printf("call para %d call fun_id %d thread id %d\n", flag_buf->para_num, flag_buf->id, call->thread_id);

            continue;
        }
        char *temp = (char *)i->para;
        if (call->opengl_id == 0 && cnt1 == 2)
        {
            printf("call message printf: %s\n", temp);
        }
        else if (call->opengl_id == 1 || call->opengl_id == 2 || call->opengl_id == 3)
        {
            if (cnt1 == 2)
            {
                int *t = (int *)temp;
                printf("call message int %d %d %d\n", t[0], t[1], t[2]);
            }
        }
    }
    if (call->opengl_id == 4)
    {
        char *t = call->elem_tail->para;
        for (int i = 0; i < call->elem_tail->len - 1; i++)
        {
            t[i] = 'c';
        }
    }

    call->callback(call);
    g_free(call);
}

/**
 * @brief 子窗口的消息处理函数
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
    long height, width, x, y;
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
        height = rcParent.bottom / 3;
        width = rcParent.right / 3;

        y = x = 0;
        // if (rcParent.bottom * 4 > rcParent.right * 3){
        //     y = (rcParent.bottom - rcParent.right * 3.0 / 4.0) / 2;
        //     height = width * 3.0 / 4.0;
        // } else {
        //     x = (rcParent.right - rcParent.bottom * 4.0 / 3.0) / 2;
        //     width = height * 4.0 / 3.0;
        // }
        MoveWindow(hwnd, x, y, width, height, FALSE);
        break;

    default:
        printf("child win msg: %d\n", uMsg);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/**
 * @brief 真正用于取出vring上传过来的数据，然后调用相关解码的线程
 * 
 * @param opaque 这个传入的是VirtIODevice
 * @return void* 
 */
static void *opengl_render_thread(void *opaque)
{
    VirtIODevice *vdev = opaque;
    MYGPU *g = MY_GPU(vdev);
    VirtQueue *vq = g->data_queue;

    /**
     * @todo 自适应轮询间隔时间
     * 
     */
    // MYGPU_Queue_Elem *elem;

    MYGPU_Opengl_Call *opengl_call = NULL;

    // size_t para_num;
    // int opengl_id;

    unsigned long usleep_time = 1000;
    while (render_run)
    {
        //从queue中打包调用，假如打包失败的话，失败的部分也还是会还给guest
        if ((opengl_call = pack_call_from_queue(vq)) != NULL)
        {
            printf("virtio has data\n");
            opengl_call->vdev = vdev;
            opengl_call->callback = push_free_callback;
            //实际调用
            // decode_invoke(opengl_call);
            //临时调用
            opengl_invoke(opengl_call);
        }
        else
        {
            g_usleep(usleep_time);
        }
    }

    return NULL;
}

/**
 * @brief 覆盖在原来窗口上面用于绘制的窗口的线程主函数，主要包括了窗口的建立和设置
 * 
 * @param opaque 需要传入VirtIODevice
 * @return void* 
 */
void *opengl_ui_thread(void *opaque)
{
    VirtIODevice *vdev = opaque;

    //通过这个方式获取hwnd要求必须使用SDL接口创建界面
    QemuConsole *con;
    while ((con = qemu_console_lookup_by_index(0)) == NULL)
    {
        //理论上启动这个线程时，主窗口的hwnd肯定是有了，所以不会进到这个等待循环内
        g_usleep(1000000);
        printf("con is NULL\n");
    }
    HWND render_hwnd = (HWND)qemu_console_get_window_id(con);

    RECT rcParent;
    long height, width;

    //在透明窗口解决前不全屏幕覆盖
    GetClientRect(render_hwnd, &rcParent);
    height = rcParent.bottom / 3;
    width = rcParent.right / 3;

    static const char className[] = "openglWin";

    WNDCLASS wc = {};
    if (!GetClassInfo(GetModuleHandle(NULL), className, &wc))
    {
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // redraw if size changes
        wc.lpfnWndProc = &subWindowProc;               // points to window procedure
        wc.cbWndExtra = sizeof(void *);                // save extra window memory
        wc.lpszClassName = className;                  // name of window class
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClass(&wc);
    }

    printf("create child window\n");

    opengl_render_hwnd = CreateWindowEx(
        WS_EX_NOPARENTNOTIFY, // do not bother our parent window
        className,
        "opengl",
        WS_CHILD,
        0, 0, width, height,
        render_hwnd,
        NULL,
        NULL,
        NULL);

    ShowWindow(opengl_render_hwnd, TRUE);
    render_run = 1;
    // SetBkMode()
    QemuThread t;
    //此时创建用于解码的线程
    qemu_thread_create(&t, "render", opengl_render_thread,
                       vdev, QEMU_THREAD_JOINABLE);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        //创建窗口的线程需要循环处理消息
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    //GetMessage为阻塞函数，当他返回0时表示窗口被关掉了
    render_run = 0;
    qemu_thread_join(&t);
    return NULL;
}
