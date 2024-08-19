#ifndef QEMU_TELEPORT_EXPRESS_CALL_H
#define QEMU_TELEPORT_EXPRESS_CALL_H
#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_device_common.h"


//用位来表示类型
#define DIRECT_PARA 1
#define COPY_PARA 2
#define RET_PARA 4

//最多的参数数目
#define MAX_PARA_NUM 32


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





typedef struct Teleport_Express_Flag_Buf
{
    //用于guest端唤醒进程的flag
    volatile uint64_t flag;

    volatile int64_t mem_spend_time;

    volatile uint64_t ret_data;

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




Teleport_Express_Call *alloc_one_call(void);
void release_one_call(Teleport_Express_Call *call, bool notify);
Guest_Mem *alloc_one_guest_mem(void);
void release_one_guest_mem(Guest_Mem *mem);

/**
 * Get a pointer to the guest memory region para is pointing to.
 * If need_free is non-zero, the caller is responsible for freeing the memory (using g_free).
*/
void *call_para_to_ptr(Call_Para para, int *need_free);
void *get_direct_ptr(Guest_Mem *guest_mem, int *flag);
void read_from_guest_mem(Guest_Mem *guest, void *host, size_t start_loc, size_t length);
void write_to_guest_mem(Guest_Mem *guest, void *host, size_t start_loc, size_t length);
void host_guest_buffer_exchange(Scatter_Data *guest_data, unsigned char *host_data, size_t start_loc, size_t length, int is_guest_to_host);


int fill_teleport_express_queue_elem(Teleport_Express_Queue_Elem *elem, unsigned long long *id, unsigned long long *thread_id, unsigned long long *process_id, unsigned long long *unique_id, unsigned long long *num);
Teleport_Express_Call *pack_call_from_queue(VirtQueue *vq, int index);



int get_para_from_call(Teleport_Express_Call *call, Call_Para *call_para, unsigned long para_num);


Guest_Mem *copy_guest_mem_from_call(Teleport_Express_Call *call, int index);

void free_copied_guest_mem(Guest_Mem *mem);


void guest_null_ptr_init(VirtQueue *vq);

void common_call_callback(Teleport_Express_Call *call);

bool call_is_interrupt(Teleport_Express_Call *call);

#endif