#ifndef EXPRESS_DEVICE_COMMON_H
#define EXPRESS_DEVICE_COMMON_H

#include "hw/teleport-express/teleport_express.h"

#define EXPRESS_CTRL_DEVICE_ID ((uint64_t)0)
#define EXPRESS_GPU_DEVICE_ID ((uint64_t)1)
#define EXPRESS_LOG_DEVICE_ID ((uint64_t)2)



//保留的fun_id，其他设备不可使用
#define EXPRESS_TERMINATE_FUN_ID (0)
#define EXPRESS_CLUSTER_FUN_ID (9999)
#define EXPRESS_REGISTER_FUN_ID (99999999)


//存放call的缓冲区大小
#define CALL_BUF_SIZE 512



#define EXPRESS_DEVICE_NUM 1

#define EXPRESS_DEVICE_NAMES \
"express_cdev "


//device设备的id在高4字节，需要调用的函数id在低3字节，设备id决定到底哪个线程去处理，函数id决定怎么处理，中间一个字节的每个位决定函数处理是异步同步等信息
//设备id（4字节）|标志位（1字节）|函数id（3字节）
#define GET_DEVICE_ID(id)  ((id) >> 32)
#define GET_FUN_ID(id)     ((id)&0xffffff)
#define FUN_NEED_SYNC(id)   (((id)>>24)&0x1)
#define FUN_HAS_HOST_SYNC(id) (((id) >> 24) & 0x2)


#define EXPRESS_DEVICE_INIT(device_name, info)                                       \
    static void __attribute__((constructor)) express_thread_init_##device_name(void) \
    {                                                                                \
        express_device_init_common(info);                                            \
    }



/**
 * @brief 自定义的Queue_Elem结构体，用来接收guest端传输过来的数据元信息
 * 
 */
typedef struct Teleport_Express_Queue_Elem
{
    VirtQueueElement elem;

    //该数据的对外指针
    void *para;

    //数据的长度
    size_t len;

    struct Teleport_Express_Queue_Elem *next;
} Teleport_Express_Queue_Elem;



typedef struct Teleport_Express_Call
{

    //调用id
    uint64_t id;

    uint64_t thread_id;

    uint64_t process_id;

    uint64_t unique_id;

    gint64 spend_time;

    //参数数目
    uint64_t para_num;

    Teleport_Express_Queue_Elem *elem_header;
    Teleport_Express_Queue_Elem *elem_tail;

    VirtQueue *vq;
    VirtIODevice *vdev;

    //渲染线程处理完之后的回调函数，必须要进行的是内存释放的工作
    void (*callback)(struct Teleport_Express_Call *call, int notify);

    struct Teleport_Express_Call *next;

    int is_end;

} Teleport_Express_Call;

typedef void (*EXPRESS_DECODE_FUN)(void *, Teleport_Express_Call *);

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
    Teleport_Express_Call *call_buf[CALL_BUF_SIZE + 2];

    //环形缓冲区的读写位置
    volatile int read_loc;
    volatile int write_loc;

    int atomic_event_lock;

//缓冲区用来通知 有数据/缓冲区有空位置 的event
// QemuEvent data_event;
#ifdef _WIN32
    HANDLE data_event;
#else

#endif

    //标示当前线程
    QemuThread this_thread;

    //这个线程连接到的teleport_express设备
    VirtIODevice *teleport_express_device;

    //特定设备自定义的context初始化函数
    void (*context_init)(struct Thread_Context *context);

    void (*context_destroy)(struct Thread_Context *context);

    //在数据到来后，特定设备自定义的处理call数据的函数，需要在这个函数中调用callback
    void (*call_handle)(struct Thread_Context *context, Teleport_Express_Call *call);

} Thread_Context;


typedef struct Express_Device_Info
{
    //留作内部使用
    int device_index;

    //该设备是否默认启用
    bool enable_default;

    //设备的名字
    const char *name;

    //该设备是否开启对应的选项名字，若为空则表示该设备默认开启
    const char *option_name;

    //该设备所用到的内核设备名字，内核会使用该名字加载驱动
    const char *driver_name;

    //设备的类型id
    int device_id;

    //对应到Thread_Context中的两个设备自定义的函数——初始化函数和call处理函数
    void (*context_init)(struct Thread_Context *context);
    void (*context_destroy)(struct Thread_Context *context);
    void (*call_handle)(struct Thread_Context *context, Teleport_Express_Call *call);

    //设备定义的用于获取context的函数，例如有一个统一的context或者对每一个线程维护一个context
    Thread_Context *(*get_context)(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

    void (*remove_context)(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

} Express_Device_Info;


extern bool express_gpu_gl_debug_enable;
extern bool express_gpu_independ_window_enable;

extern char *kernel_load_express_driver_names;
extern int kernel_load_express_driver_num;



void express_device_init_common(Express_Device_Info *info);

Express_Device_Info *get_express_device_info(unsigned int device_id);

void cluster_decode_invoke(Teleport_Express_Call *call, void *context, EXPRESS_DECODE_FUN decode_fun);


//FUN_ID为0是保留字段

// #define FUNID_eglSwapBuffers_special ((EXPRESS_GPU_DEVICE_ID<<32u)+(((uint64_t)0x8+0x4)<<24u)+100000)


#endif