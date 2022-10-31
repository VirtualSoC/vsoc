#ifndef QEMU_TELEPORT_EXPRESS_DISTRIBUTE_H
#define QEMU_TELEPORT_EXPRESS_DISTRIBUTE_H
#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_device_common.h"


//用位来表示类型
#define DIRECT_PARA 1
#define COPY_PARA 2
#define RET_PARA 4

//最多的参数数目
#define MAX_PARA_NUM 60


//配置vm退出时是否进行取数据的工作，若是，则通知轮询线程外，会主动取数据，直到轮询线程结束休眠开始干活，若否，则只是通知轮询线程
#define DISTRIBUTE_WHEN_VM_EXIT


/**
 * @brief 释放Teleport_Express_Queue链表中的Teleport_Express_Queue_Elem包括额外的申请空间
 * 
 */
#define TELEPORT_EXPRESS_QUEUE_ELEMS_FREE(header_ptr)               \
    for (Teleport_Express_Queue_Elem *a = (header_ptr); a != NULL;) \
    {                                                             \
        Teleport_Express_Queue_Elem *b = a;                         \
        a = a->next;                                              \
        if (b->para != NULL)                                      \
        {                                                         \
            release_one_guest_mem(b->para);                       \
        }                                                         \
        g_free(b);                                                \
    }

/**
 * @brief 将Teleport_Express_Queue_Elem链表中的所有elem元素push回vring中，方便guest空间回收
 * 
 */
#define VIRTIO_ELEM_PUSH_ALL(vq, elem, header_ptr, num, next)    \
    for (elem *a = (elem *)(header_ptr); a != NULL; a = a->next) \
    {                                                            \
        virtqueue_push(vq, (VirtQueueElement *)a, num);          \
    }


//device设备的id在高4字节，需要调用的函数id在低3字节，设备id决定到底哪个线程去处理，函数id决定怎么处理，中间一个字节的每个位决定函数处理是异步同步等信息
//设备id（4字节）|标志位（1字节）|函数id（3字节）
// #define GET_DEVICE_ID(id) ((id) >> 32)
// #define GET_FUN_ID(id) ((id)&0xffffff)
// #define FUN_NEED_SYNC(id) (((id) >> 24) & 0x1)
// #define FUN_HAS_HOST_SYNC(id) (((id) >> 24) & 0x2)



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

typedef struct Teleport_Express_Flag_Buf
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

} Teleport_Express_Flag_Buf;





extern bool teleport_express_should_stop;

extern int atomic_distribute_thread_running;


void *call_distribute_thread(void *opaque);

void guest_null_ptr_init(VirtQueue *vq);

void virtqueue_data_distribute_and_recycle(VirtQueue *vq, int *pop_flag, int *recycle_flag);

Thread_Context *thread_context_create(uint64_t thread_id, uint64_t type_id, uint64_t len, Express_Device_Info *info);


int get_para_from_call(Teleport_Express_Call *call, Call_Para *call_para, unsigned long para_num);

void get_process_mess(Teleport_Express_Call *call, int *fun_id, int *process_id, int *thread_id, int *num_free);

// void set_call_return_val(Teleport_Express_Call *call, unsigned char* ret, size_t len);


void wake_up_distribute(void);

void *get_direct_ptr(Guest_Mem *guest_mem, int *flag);

void guest_write(Guest_Mem *guest, void *host, size_t start_loc, size_t length);

void guest_read(Guest_Mem *guest, void *host, size_t start_loc, size_t length);

void host_guest_buffer_exchange(Scatter_Data *guest_data, unsigned char *host_data, size_t start_loc, size_t length, int is_guest_to_host);


#endif