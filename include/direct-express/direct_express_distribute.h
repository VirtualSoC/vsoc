#ifndef QEMU_DIRECT_EXPRESS_DISTRIBUTE_H
#define QEMU_DIRECT_EXPRESS_DISTRIBUTE_H
#include "direct-express/direct_express.h"


//存放call的缓冲区大小
#define CALL_BUF_SIZE 512

//用位来表示类型
#define DIRECT_PARA 1
#define COPY_PARA 2
#define RET_PARA 4

/**
 * @brief 释放Direct_Express_Queue链表中的Direct_Express_Queue_Elem包括额外的申请空间
 * 
 */
#define DIRECT_EXPRESS_QUEUE_ELEMS_FREE(header_ptr)             \
    for (Direct_Express_Queue_Elem *a = (header_ptr); a != NULL;) \
    {                                                  \
        Direct_Express_Queue_Elem *b = a;                       \
        a = a->next;                                   \
        if (((b->type) & COPY_PARA) !=0 && b->para != NULL)   \
        {                                              \
            g_free(b->para);                           \
        }                                              \
        g_free(b);                                     \
    }


/**
 * @brief 将Direct_Express_Queue_Elem链表中的所有elem元素push回vring中，方便guest空间回收
 * 
 */
#define VIRTIO_ELEM_PUSH_ALL(vq, elem, header_ptr, num, next) \
    for (elem *a = (elem *)(header_ptr); a != NULL; a = a->next)        \
    {                                                         \
        virtqueue_push(vq, (VirtQueueElement *)a, num);                           \
    }


#define EXPRESS_DEVICE_INIT(device_name,info)                                         \
static void __attribute__((constructor)) express_thread_init_ ## device_name(void)    \
{                                                                           \
    express_device_init_common(info);                                   \
}



//device设备的id在高2字节，需要调用的函数id在低2字节，设备id决定到底哪个线程去处理，函数id决定怎么处理
#define GET_DEVICE_ID(id) ((id)>>16)
#define GET_FUN_ID(id) ((id)&0xffff)


/**
 * @brief 自定义的Queue_Elem结构体，用来接收guest端传输过来的数据元信息
 * 
 */
typedef struct Direct_Express_Queue_Elem
{
    VirtQueueElement elem;
    
    //该数据的对外指针
    void *para;

    //数据的长度
    size_t len;

    //数据的类型
    int type;

    struct Direct_Express_Queue_Elem *next;
} Direct_Express_Queue_Elem;


typedef struct Direct_Express_Call
{

    //调用id
    int fun_id;

    int thread_id;

    // long long get_time;

    //参数数目
    size_t para_num;

    Direct_Express_Queue_Elem *elem_header;
    Direct_Express_Queue_Elem *elem_tail;

    VirtQueue *vq;
    VirtIODevice *vdev;
    
    //渲染线程处理完之后的回调函数，必须要进行的是内存释放的工作
    void (*callback)(struct Direct_Express_Call *call,int notify);

    struct Direct_Express_Call * next;

} Direct_Express_Call;


typedef struct
{
    //用于guest端唤醒进程的flag
    volatile int flag;

    //调用的id
    int id;

    //总共的参数数目
    int para_num;

    int thread_id;

    int process_id;

    int num_free;

    //调用的普通返回值
    volatile unsigned long ret;

    //注意：这里没有剩下的几个参数是因为这几个参数qemu不需要，是给驱动在之后用的

} Direct_Express_Flag_Buf;


typedef struct Thread_Context
{

    //给特定设备用来标记当前thread是否初始化完成的标志
    int init;

    //当前线程是否已经运行起来了
    int thread_run;

    //对应到guest端调用起这个设备的线程的线程id
    int thread_id;

    //设备的类型id
    int type_id;

    //用于缓冲call的环形缓冲区
    Direct_Express_Call *call_buf[CALL_BUF_SIZE];

    //环形缓冲区的读写位置
    int read_loc;
    int write_loc;

    //缓冲区用来通知 有数据/缓冲区有空位置 的event
    QemuEvent data_event;

    //标示当前线程
    QemuThread this_thread;

    //这个线程连接到的direct_express设备
    VirtIODevice *direct_express_device;

    //特定设备自定义的context初始化函数
    void (*context_init)(struct Thread_Context *context);

    //在数据到来后，特定设备自定义的处理call数据的函数，需要在这个函数中调用callback
    void (*call_handle)(struct Thread_Context *context, Direct_Express_Call *call);

} Thread_Context;


typedef struct Express_Device_Info{
    
    //设备的名字
    char *name;
    

    //设备的类型id
    int type_id;


    //对应到Thread_Context中的两个设备自定义的函数——初始化函数和call处理函数
    void (*context_init)(struct Thread_Context *context);
    void (*call_handle)(struct Thread_Context *context, Direct_Express_Call *call);


    //设备定义的用于获取context的函数，例如有一个统一的context或者对每一个线程维护一个context
    Thread_Context *(*get_context)(int type_id,int thread_id,struct Express_Device_Info *info);

} Express_Device_Info;



void *call_distribute_thread(void *opaque);


Thread_Context *thread_context_create(int thread_id,int type_id,unsigned int len,Express_Device_Info *info);


void express_device_init_common(Express_Device_Info *info);


void wake_up_distribute();

#endif