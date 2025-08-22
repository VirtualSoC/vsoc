#ifndef EXPRESS_BRIDGE_H
#define EXPRESS_BRIDGE_H

#include "hw/vsoc/express_platform.h"
#include "qemu/osdep.h"
#include "qemu/thread.h"

typedef struct Bridge_Connection_Context{
    Device_Context device_context;
    Guest_Mem *guest_data;

    int socket_fd;

    bool read_thread_should_running;
    QemuThread read_thread;

} Bridge_Connection_Context;

typedef struct
{
    Thread_Context thread_context;

    int status_id;

    uint64_t unique_id;

    uint64_t thread_id;

    Bridge_Connection_Context connection_context;

} Bridge_Thread_Context;


#endif