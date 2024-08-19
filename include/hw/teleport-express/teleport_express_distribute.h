#ifndef QEMU_TELEPORT_EXPRESS_DISTRIBUTE_H
#define QEMU_TELEPORT_EXPRESS_DISTRIBUTE_H
#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_call.h"





//配置vm退出时是否进行取数据的工作，若是，则通知轮询线程外，会主动取数据，直到轮询线程结束休眠开始干活，若否，则只是通知轮询线程
#define DISTRIBUTE_WHEN_VM_EXIT






extern int atomic_distribute_thread_running;


void *call_distribute_thread(void *opaque);


void virtqueue_data_distribute_and_recycle(VirtQueue *vq, int *pop_flag, int *recycle_flag, int *need_irq);

Thread_Context *thread_context_create(uint64_t thread_id, uint64_t type_id, uint64_t len, Express_Device_Info *info);



// void get_process_mess(Teleport_Express_Call *call, int *fun_id, int *process_id, int *thread_id, int *num_free);

// void set_call_return_val(Teleport_Express_Call *call, unsigned char* ret, size_t len);


void wake_up_distribute(void);



#endif