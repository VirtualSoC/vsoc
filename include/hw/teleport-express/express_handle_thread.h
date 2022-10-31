#ifndef QEMU_EXPRESS_HANDLE_THREAD_H
#define QEMU_EXPRESS_HANDLE_THREAD_H
#include "hw/teleport-express/teleport_express_distribute.h"




void *handle_thread_run(void *opaque);

void call_push(Thread_Context *context, Teleport_Express_Call *call);


#endif