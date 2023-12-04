#ifndef EXPRESS_BRIDGE_H
#define EXPRESS_BRIDGE_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"
#include "hw/teleport-express/teleport_express_distribute.h"



typedef struct Birdge_Connection_Context{
    Device_Context device_context;
    Guest_Mem *guest_data;

    int socket_fd;

    bool read_thread_should_running;
    QemuThread read_thread;

} Birdge_Connection_Context;

typedef struct
{
    Thread_Context thread_context;

    int status_id;

    uint64_t unique_id;

    uint64_t thread_id;

    Birdge_Connection_Context connection_context;

} Bridge_Thread_Context;


#endif