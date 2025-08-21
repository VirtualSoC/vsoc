#ifndef QEMU_TELEPORT_EXPRESS_REGISTER_H
#define QEMU_TELEPORT_EXPRESS_REGISTER_H
#include "hw/vsoc/teleport_express.h"
#include "hw/vsoc/express_device.h"

#include "hw/vsoc/teleport_express_call.h"


void register_input_buffer_call(VirtIODevice *vdev, VirtQueue *vq);

// void send_express_device_irq(Teleport_Express_Call *irq_call, int buf_index, int len);

int set_express_device_irq(Device_Context *device_context, int buf_index, int len);

void *input_sync_thread(void *opaque);

void express_input_device_sync(void);

void realize_input_device(VirtIODevice *vdev);

void (*get_input_call_release_ptr(void))(Teleport_Express_Call *, int);

void set_input_event_startup(void);

#endif