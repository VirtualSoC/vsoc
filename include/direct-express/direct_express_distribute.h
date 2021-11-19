#ifndef QEMU_DIRECT_EXPRESS_DISTRIBUTE_H
#define QEMU_DIRECT_EXPRESS_DISTRIBUTE_H
#include "direct-express/direct_express.h"

//存放call的缓冲区大小
#define CALL_BUF_SIZE 512

//用位来表示类型
#define DIRECT_PARA 1
#define COPY_PARA 2
#define RET_PARA 4

//最多的参数数目
#define MAX_PARA_NUM 64

#define TERMINATE_FUN_ID 0

/**
 * @brief 释放Direct_Express_Queue链表中的Direct_Express_Queue_Elem包括额外的申请空间
 * 
 */
#define DIRECT_EXPRESS_QUEUE_ELEMS_FREE(header_ptr)               \
    for (Direct_Express_Queue_Elem *a = (header_ptr); a != NULL;) \
    {                                                             \
        Direct_Express_Queue_Elem *b = a;                         \
        a = a->next;                                              \
        if (b->para != NULL)                                      \
        {                                                         \
            release_one_guest_mem(b->para);                       \
        }                                                         \
        g_free(b);                                                \
    }

/**
 * @brief 将Direct_Express_Queue_Elem链表中的所有elem元素push回vring中，方便guest空间回收
 * 
 */
#define VIRTIO_ELEM_PUSH_ALL(vq, elem, header_ptr, num, next)    \
    for (elem *a = (elem *)(header_ptr); a != NULL; a = a->next) \
    {                                                            \
        virtqueue_push(vq, (VirtQueueElement *)a, num);          \
    }

#define EXPRESS_DEVICE_INIT(device_name, info)                                       \
    static void __attribute__((constructor)) express_thread_init_##device_name(void) \
    {                                                                                \
        express_device_init_common(info);                                            \
    }

//device设备的id在高4字节，需要调用的函数id在低3字节，设备id决定到底哪个线程去处理，函数id决定怎么处理，中间一个字节的每个位决定函数处理是异步同步等信息
//设备id（4字节）|标志位（1字节）|函数id（3字节）
// #define GET_DEVICE_ID(id) ((id) >> 32)
// #define GET_FUN_ID(id) ((id)&0xffffff)
// #define FUN_NEED_SYNC(id) (((id) >> 24) & 0x1)
// #define FUN_HAS_HOST_SYNC(id) (((id) >> 24) & 0x2)

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
    uint64_t id;

    uint64_t thread_id;

    uint64_t process_id;

    uint64_t unique_id;

    gint64 spend_time;

    //参数数目
    uint64_t para_num;

    Direct_Express_Queue_Elem *elem_header;
    Direct_Express_Queue_Elem *elem_tail;

    VirtQueue *vq;
    VirtIODevice *vdev;

    //渲染线程处理完之后的回调函数，必须要进行的是内存释放的工作
    void (*callback)(struct Direct_Express_Call *call, int notify);

    struct Direct_Express_Call *next;

    int is_end;

} Direct_Express_Call;

//scatter与下面这个iovec等价
//struct iovec {
//     void *iov_base;
//     size_t iov_len;
// };
typedef struct Scatter_Data
{
    unsigned char *data;
    size_t len;
} Scatter_Data;

typedef struct Guest_Mem
{
    Scatter_Data *scatter_data;
    int num;
    int all_len;
} Guest_Mem;

typedef struct Call_Para
{
    // int is_direct;
    Guest_Mem *data;
    size_t data_len;
} Call_Para;

typedef struct Direct_Express_Flag_Buf
{
    //用于guest端唤醒进程的flag
    uint64_t flag;

    int64_t mem_spend_time;

    //调用的id，注意这是64位，所以肯定是8个字节的
    uint64_t id;

    //总共的参数数目
    uint64_t para_num;

    uint64_t thread_id;

    uint64_t process_id;

    uint64_t unique_id;

    // uint64_t  num_free;

    // //调用的普通返回值
    // volatile uint64_t ret;

    //注意：这里没有剩下的几个参数是因为这几个参数qemu不需要，是给驱动在之后用的

} Direct_Express_Flag_Buf;

typedef struct Thread_Context
{

    //给特定设备用来标记当前thread是否初始化完成的标志
    int init;

    //当前线程是否已经运行起来了
    int thread_run;

    //对应到guest端调用起这个设备的线程的线程id
    uint64_t thread_id;

    //设备的类型id
    uint64_t type_id;

    //用于缓冲call的环形缓冲区
    Direct_Express_Call *call_buf[CALL_BUF_SIZE + 2];

    //环形缓冲区的读写位置
    int read_loc;
    int write_loc;

    int atomic_event_lock;

//缓冲区用来通知 有数据/缓冲区有空位置 的event
// QemuEvent data_event;
#ifdef _WIN32
    HANDLE data_event;
#else

#endif

    //标示当前线程
    QemuThread this_thread;

    //这个线程连接到的direct_express设备
    VirtIODevice *direct_express_device;

    //特定设备自定义的context初始化函数
    void (*context_init)(struct Thread_Context *context);

    void (*context_destroy)(struct Thread_Context *context);

    //在数据到来后，特定设备自定义的处理call数据的函数，需要在这个函数中调用callback
    void (*call_handle)(struct Thread_Context *context, Direct_Express_Call *call);

} Thread_Context;

typedef struct Express_Device_Info
{

    //设备的名字
    const char *name;

    //设备的类型id
    int type_id;

    //对应到Thread_Context中的两个设备自定义的函数——初始化函数和call处理函数
    void (*context_init)(struct Thread_Context *context);
    void (*context_destroy)(struct Thread_Context *context);
    void (*call_handle)(struct Thread_Context *context, Direct_Express_Call *call);

    //设备定义的用于获取context的函数，例如有一个统一的context或者对每一个线程维护一个context
    Thread_Context *(*get_context)(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

    void (*remove_context)(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

} Express_Device_Info;

extern bool direct_express_should_stop;

void *call_distribute_thread(void *opaque);

Thread_Context *thread_context_create(uint64_t thread_id, uint64_t type_id, uint64_t len, Express_Device_Info *info);

void mark_call_return(Direct_Express_Call *call, int loc);

int get_para_from_call(Direct_Express_Call *call, Call_Para *call_para, unsigned long para_num);

void get_process_mess(Direct_Express_Call *call, int *fun_id, int *process_id, int *thread_id, int *num_free);

// void set_call_return_val(Direct_Express_Call *call, unsigned char* ret, size_t len);

void express_device_init_common(Express_Device_Info *info);

void wake_up_distribute(void);

void *get_direct_ptr(Guest_Mem *guest_mem, int *flag);

void guest_write(Guest_Mem *guest, void *host, size_t start_loc, size_t length);

void guest_read(Guest_Mem *guest, void *host, size_t start_loc, size_t length);

void host_guest_buffer_exchange(Scatter_Data *guest_data, unsigned char *host_data, size_t start_loc, size_t length, int is_guest_to_host);


#endif