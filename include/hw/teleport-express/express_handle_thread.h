#ifndef QEMU_EXPRESS_HANDLE_THREAD_H
#define QEMU_EXPRESS_HANDLE_THREAD_H
#include "qemu/osdep.h"
#include "qemu/thread.h"
#include "hw/teleport-express/teleport_express_distribute.h"

void call_push(Thread_Context *context, Teleport_Express_Call *call);

Teleport_Express_Call *call_pop(Thread_Context *context);


void *handle_thread_run(void *opaque);


#endif