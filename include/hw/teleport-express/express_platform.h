#ifndef EXPRESS_PLATFORM_H
#define EXPRESS_PLATFORM_H

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

#define EXPRESS_CTRL_DEVICE_ID ((uint64_t)0)
#define EXPRESS_GPU_DEVICE_ID ((uint64_t)1)
#define EXPRESS_LOG_DEVICE_ID ((uint64_t)2)

#define EXPRESS_TOUCHSCREEN_DEVICE_ID ((uint64_t)3)
#define EXPRESS_KEYBOARD_DEVICE_ID ((uint64_t)4)
#define EXPRESS_BATTERY_DEVICE_ID ((uint64_t)5)
#define EXPRESS_ACCELEROMTETER_DEVICE_ID ((uint64_t)6)
#define EXPRESS_GYROSCOPE_DEVICE_ID ((uint64_t)7)
#define EXPRESS_GPS_DEVICE_ID ((uint64_t)8)
#define EXPRESS_MICROPHONE_DEVICE_ID ((uint64_t)9)
#define EXPRESS_DISPLAY_DEVICE_ID ((uint64_t)10)
#define EXPRESS_CAMERA_DEVICE_ID ((uint64_t)11)
#define EXPRESS_MODEM_DEVICE_ID ((uint64_t)12)
#define EXPRESS_CODEC_DEVICE_ID ((uint64_t)13)

#define EXPRESS_WIFI_DEVICE_ID ((u64)20)

#define EXPRESS_NET_DEVICE_ID ((u64)30)

#define EXPRESS_BRIDGE_DEVICE_ID ((uint64_t)40)

#define EXPRESS_SYNC_DEVICE_ID ((uint64_t)50)

#define EXPRESS_MEM_DEVICE_ID ((uint64_t)60)

//保留的fun_id，其他设备不可使用
#define EXPRESS_TERMINATE_FUN_ID (0)
#define EXPRESS_CLUSTER_FUN_ID (9999)
#define EXPRESS_REGISTER_BUFFER_FUN_ID (999999)
#define EXPRESS_IRQ_FUN_ID (1000000)
#define EXPRESS_GET_PROP_FUN_ID (1000001)
#define EXPRESS_RELEASE_IRQ_FUN_ID (1000002)


//存放call的缓冲区大小
#define CALL_BUF_SIZE 512

#define INPUT_DEVICE_TYPE 1
#define OUTPUT_DEVICE_TYPE 2


// device设备的id在高4字节，需要调用的函数id在低3字节，设备id决定到底哪个线程去处理，函数id决定怎么处理，中间一个字节的每个位决定函数处理是异步同步等信息
//设备id（4字节）|标志位（1字节）|函数id（3字节）
#define GET_DEVICE_ID(id) ((uint32_t)((id) >> 32))
#define GET_FUN_ID(id) ((uint32_t)((id)&0xffffff))
#define FUN_NEED_SYNC(id) (((id) >> 24) & 0x1)
#define FUN_HAS_HOST_SYNC(id) (((id) >> 24) & 0x2)

#define SYNC_FUN_ID(id) ((1L << 24) | (uint64_t)id)
#define HOST_SYNC_FUN_ID(id) ((1L << 25) | (uint64_t)id)

#define DEVICE_FUN_ID(device_id, id) (((uint64_t)device_id << 32) | id)


#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define swap(a, b, type) \
    {                    \
        type temp = a;   \
        a = b;           \
        b = temp;        \
    }

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#define THREAD_CONTROL_BEGIN \
dispatch_sync(dispatch_get_main_queue(), ^{ 
#define THREAD_CONTROL_END \
});
#else
#define THREAD_CONTROL_BEGIN
#define THREAD_CONTROL_END
#endif


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

typedef struct Teleport_Express_Queue_Elem Teleport_Express_Queue_Elem;
typedef struct VirtQueue VirtQueue;
typedef struct VirtIODevice VirtIODevice;
typedef struct Monitor Monitor;

typedef struct Teleport_Express_Call
{

    //调用id
    uint64_t id;

    uint64_t thread_id;

    uint64_t process_id; //guest传下来的：flag_buf->thread_id = (u64)current->pid; flag_buf->process_id = (u64)current->tgid;

    uint64_t unique_id;

    int64_t spend_time;

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

typedef bool (*EXPRESS_DECODE_FUN)(void *, uint64_t, const Call_Para *, int);


typedef struct Thread_Context
{
        //设备的类型id
    uint64_t device_id;

    //用于缓冲call的环形缓冲区
    Teleport_Express_Call *call_buf[CALL_BUF_SIZE + 2];

    //环形缓冲区的读写位置
    volatile int read_loc;
    volatile int write_loc;

//缓冲区用来通知 有数据/缓冲区有空位置 的event
// QemuEvent data_event;
#ifdef _WIN32
    HANDLE data_event;
#else
    void *data_event;
#endif

    //给特定设备用来标记当前thread是否初始化完成的标志
    int init;

    //当前线程是否已经运行起来了
    int thread_run;

    //对应到guest端调用起这个设备的线程的线程id
    uint64_t thread_id;

    uint64_t unique_id;

    uint64_t process_id;

    //标示当前线程
    void *this_thread;

    //这个线程连接到的teleport_express设备
    VirtIODevice *teleport_express_device;

    //特定设备自定义的context初始化函数
    void (*context_init)(struct Thread_Context *context);

    void (*context_destroy)(struct Thread_Context *context);

    //在数据到来后，特定设备自定义的处理call数据的函数，需要在这个函数中调用callback
    bool (*call_handler)(struct Thread_Context *context, uint64_t id, const Call_Para *all_para, int para_num);

} Thread_Context;

typedef struct Device_Context{
    bool irq_enabled;
    Teleport_Express_Call *irq_call;
    struct Express_Device_Info *device_info;
} Device_Context;

typedef struct Express_Device_Info
{
    //留作内部使用
    int device_index;
    bool enable;

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

    //设备的类型
    int device_type;

    //设备的初始化函数，一定为设备第一个被调用的函数
    void (*init)(void);

    //仅会在context对应线程上调用的init, destroy和call处理函数，仅output模式可用
    void (*context_init)(struct Thread_Context *context);
    void (*context_destroy)(struct Thread_Context *context);
    bool (*call_handler)(struct Thread_Context *context, uint64_t id, const Call_Para *all_para, int para_num);

    //设备定义的用于获取数据分发context的函数，负责处理从guest到host的数据，例如有一个统一的context或者对每一个线程维护一个context。不保证线程安全性。
    Thread_Context *(*get_context)(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

    // guest端设备文件被关闭时会调用的回调函数，返回值标示需要被销毁的context。若返回值不为NULL，则接下来会在context对应线程上调用context_destroy函数（若实现的话）。不保证线程安全性。
    // 注意：该接口是为了应对guest进程异常退出或资源泄露，导致内核关闭设备文件的情况。此时的thread_id可能和设备文件打开时不一致，但process_id和unique_id一致。
    Thread_Context *(*remove_context)(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

    // guest注册DMA内存的回调
    void (*buffer_register)(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id);
    
    // 获取设备用于容纳虚拟中断的context，负责host向guest发送通知
    Device_Context *(*get_device_context)(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);
    // 注册虚拟中断时的回调
    void (*irq_register)(Device_Context *context);
    // 虚拟中断释放时的回调
    void (*irq_release)(Device_Context *context);

    // QEMU monitor命令的处理函数
    void (*hmp_handler)(Monitor *mon, int argc, const char **argv);

    // 给外设提供的静态属性参数值，可以在内核内通过调用get_teleport_input_device_prop来获得
    void *static_prop;
    int static_prop_size;

} Express_Device_Info;

#define EXPRESS_DEVICE_INIT(device_name, info)                                         \
    static void __attribute__((constructor)) express_platform_init_##device_name(void) \
    {                                                                                  \
        init_express_device(info);                                                   \
    }

typedef void (*PlatformReadFromGuestMem)(Guest_Mem *guest, void *host, size_t start_loc, size_t length);
typedef void (*PlatformWriteToGuestMem)(Guest_Mem *guest, void *host, size_t start_loc, size_t length);
typedef void (*PlatformFreeCopiedGuestMem)(Guest_Mem *guest);

typedef struct {
    bool teleport_express_save_snapshot;

    bool express_gpu_gl_debug_enable;
    bool express_gpu_enable_windowed_mode;
    bool express_device_input_window_enable;

    bool express_gpu_keep_window_scale;

    int express_gpu_window_width;
    int express_gpu_window_height;

    int *express_touchscreen_size;
    bool express_touchscreen_scroll_is_zoom;
    bool express_touchscreen_right_click_is_two_finger;
    int express_touchscreen_scroll_ratio;

    bool express_keyboard_finger_replay;

    int express_display_pixel_width;
    int express_display_pixel_height;
    int express_display_refresh_rate;
    uint64_t express_display_count;
    char *express_display_options;
    int express_keyboard_count;

    bool express_display_headless_mode;
    bool express_gpu_open_shader_binary;

    PlatformReadFromGuestMem read_from_guest_mem;
    PlatformWriteToGuestMem write_to_guest_mem;
} ExpressPlatformOps;

extern ExpressPlatformOps g_ops;

void init_express_device(Express_Device_Info *info);
void init_express_platform(ExpressPlatformOps ops);

void deinit_express_device(void);
bool platform_device_should_stop(void);
Guest_Mem *duplicate_guest_mem(Guest_Mem *orig);
void free_duplicated_guest_mem(Guest_Mem *mem);

// todo: these functions are currently allowed, will be removed when refactor is complete
Thread_Context *thread_context_create(uint64_t thread_id, uint64_t device_id, uint64_t len, Express_Device_Info *info);
void *get_direct_ptr(Guest_Mem *guest_mem, int *flag);
void push_local_call_to_thread(Thread_Context *context, uint64_t id);
#endif