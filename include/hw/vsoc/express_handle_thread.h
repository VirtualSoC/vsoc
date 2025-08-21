#ifndef QEMU_EXPRESS_HANDLE_THREAD_H
#define QEMU_EXPRESS_HANDLE_THREAD_H

#include "qemu/osdep.h"
#include "qemu/thread.h"
#include "hw/vsoc/teleport_express_distribute.h"

void call_push(Thread_Context *context, Teleport_Express_Call *call);

Teleport_Express_Call *call_pop(Thread_Context *context);

void *handle_thread_run(void *opaque);

Thread_Context *thread_context_create(uint64_t thread_id, uint64_t device_id, uint64_t len, Express_Device_Info *info);

#endif