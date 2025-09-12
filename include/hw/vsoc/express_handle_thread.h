#ifndef QEMU_EXPRESS_HANDLE_THREAD_H
#define QEMU_EXPRESS_HANDLE_THREAD_H

#include "hw/vsoc/express_platform.h"
#include "qemu/osdep.h"
#include "qemu/thread.h"

void call_push(Thread_Context *context, void *call);

void *call_pop(Thread_Context *context);

bool cluster_decode_invoke(Thread_Context *context, const Call_Para *all_para, int para_num);

#endif