#ifndef QEMU_TELEPORT_EXPRESS_DISTRIBUTE_H
#define QEMU_TELEPORT_EXPRESS_DISTRIBUTE_H
#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_device.h"
#include "hw/teleport-express/teleport_express_call.h"

//配置vm退出时是否进行取数据的工作，若是，则通知轮询线程外，会主动取数据，直到轮询线程结束休眠开始干活，若否，则只是通知轮询线程
#define DISTRIBUTE_WHEN_VM_EXIT

extern int atomic_distribute_thread_running;

void *call_distribute_thread(void *opaque);

void virtqueue_data_distribute_and_recycle(VirtQueue *vq, int *pop_flag, int *recycle_flag, int *need_irq);

void wake_up_distribute(void);

void (*get_push_free_callback_ptr(void))(Teleport_Express_Call *, int);

void push_to_thread(Teleport_Express_Call *call);
void push_local_call_to_thread(Thread_Context *context, uint64_t id);

#endif