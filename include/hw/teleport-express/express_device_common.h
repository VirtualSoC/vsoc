#ifndef EXPRESS_DEVICE_COMMON_H
#define EXPRESS_DEVICE_COMMON_H

#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_log.h"

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



#define EXPRESS_DEVICE_INIT(device_name, info)                                       \
    static void __attribute__((constructor)) express_thread_init_##device_name(void) \
    {                                                                                \
        express_device_init_common(info);                                            \
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
        //设备的类型id
    uint64_t device_id;

    //用于缓冲call的环形缓冲区
    Teleport_Express_Call *call_buf[CALL_BUF_SIZE + 2];

    //环形缓冲区的读写位置
    volatile int read_loc;
    volatile int write_loc;

    // int atomic_event_lock;

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

struct Express_Device_Info;

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

    //对应到Thread_Context中的两个设备自定义的函数——初始化函数和call处理函数，仅output模式可用
    void (*context_init)(struct Thread_Context *context);
    void (*context_destroy)(struct Thread_Context *context);
    void (*call_handle)(struct Thread_Context *context, Teleport_Express_Call *call);

    //设备定义的用于获取数据分发context的函数，负责处理从guest到host的数据，例如有一个统一的context或者对每一个线程维护一个context
    Thread_Context *(*get_context)(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);

    // guest端设备文件被关闭时会调用的回调函数，返回值是标示该context是否已经被销毁，即与该context相关的线程是否应该退出
    bool (*remove_context)(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);


    // guest注册DMA内存的回调
    void (*buffer_register)(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id);
    
    // 获取设备用于容纳虚拟中断的context，负责host向guest发送通知
    Device_Context *(*get_device_context)(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info);
    // 注册虚拟中断时的回调
    void (*irq_register)(Device_Context *context);
    // 虚拟中断释放时的回调
    void (*irq_release)(Device_Context *context);

    // 给外设提供的静态属性参数值，可以在内核内通过调用get_teleport_input_device_prop来获得
    void *static_prop;
    int static_prop_size;

} Express_Device_Info;


extern Device_Log_Setting_Info express_device_log_setting_info;

extern bool express_gpu_gl_debug_enable;
extern bool express_gpu_independ_window_enable;
extern bool express_device_input_window_enable;

extern bool express_gpu_keep_window_scale;

extern int express_gpu_window_width;
extern int express_gpu_window_height;

extern int *express_touchscreen_size;

extern bool express_touchscreen_scroll_is_zoom;
extern bool express_touchscreen_right_click_is_two_finger;
extern int express_touchscreen_scroll_ratio;

extern bool express_keyboard_finger_replay;

extern char *kernel_load_express_driver_names;
extern int kernel_load_express_driver_num;

extern int *express_display_pixel_width;
extern int *express_display_pixel_height;
extern int *express_display_phy_width;
extern int *express_display_phy_height;
extern int express_display_refresh_rate;

extern bool express_display_switch_open;

extern bool express_gpu_open_shader_binary;

extern char *express_ruim_file;

void express_device_init_common(Express_Device_Info *info);

Express_Device_Info *get_express_device_info(unsigned int device_id);

void cluster_decode_invoke(Teleport_Express_Call *call, void *context, EXPRESS_DECODE_FUN decode_fun);


#endif