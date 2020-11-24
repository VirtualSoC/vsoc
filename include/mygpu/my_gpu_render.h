#ifndef QEMU_MY_GPU_RENDER_H
#define QEMU_MY_GPU_RENDER_H
#include "mygpu/my_gpu.h"



//用位来表示类型
#define DIRECT_PARA 1
#define COPY_PARA 2
#define RET_PARA 4

/**
 * @brief 释放MYGPU_Queue_Elem链表中的MYGPU_Queue_Elem，包括额外的申请空间
 * 
 */
#define MYGPU_QUEUE_ELEMS_FREE(header_ptr)             \
    for (MYGPU_Queue_Elem *a = (header_ptr); a != NULL;) \
    {                                                  \
        MYGPU_Queue_Elem *b = a;                       \
        a = a->next;                                   \
        if (((b->type) & COPY_PARA) !=0 && b->para != NULL)   \
        {                                              \
            g_free(b->para);                           \
        }                                              \
        g_free(b);                                     \
    }


/**
 * @brief 将MYGPU_Queue_Elem链表中的所有elem元素push回vring中，方便guest空间回收
 * 
 */
#define VIRTIO_ELEM_PUSH_ALL(vq, elem, header_ptr, num, next) \
    for (elem *a = (elem *)(header_ptr); a != NULL; a = a->next)        \
    {                                                         \
        virtqueue_push(vq, (VirtQueueElement *)a, num);                           \
    }

/**
 * @brief 自定义的Queue_Elem结构体，用来接收guest端传输过来的数据元信息
 * 
 */
typedef struct MYGPU_Queue_Elem
{
    VirtQueueElement elem;
    
    //该数据的对外指针
    void *para;

    //数据的长度
    size_t len;

    //数据的类型
    int type;

    struct MYGPU_Queue_Elem *next;
} MYGPU_Queue_Elem;


typedef struct MYGPU_Opengl_Call
{

    //调用id
    int opengl_id;

    int thread_id;

    //参数数目
    size_t para_num;

    MYGPU_Queue_Elem *elem_header;
    MYGPU_Queue_Elem *elem_tail;

    VirtQueue *vq;
    VirtIODevice *vdev;
    
    //渲染线程处理完之后的回调函数，必须要进行的是内存释放的工作
    void (*callback)(struct MYGPU_Opengl_Call *call);

    struct MYGPU_Opengl_Call *next;

} MYGPU_Opengl_Call;


typedef struct
{
    //用于guest端唤醒进程的flag
    int flag;

    //调用的id
    int id;

    //总共的参数数目
    int para_num;

    int thread_id;

    //调用的普通返回值
    unsigned long ret;

    //注意：这里没有剩下的几个参数是因为这几个参数qemu不需要，是给驱动在之后用的

} MYGPU_Flag_Buf;



void *opengl_ui_thread(void *opaque);


// void *my_gpu_render_thread(void *opaque);
// void push_to_render_buf(MYGPU_Opengl_Call *call);
// MYGPU_Opengl_Call *pop_from_render_buf(void);
// void opengl_invoke(MYGPU_Opengl_Call *call);

// MYGPU_Opengl_Call *pack_call_from_queue(VirtQueue *vq);



#endif