#ifndef QEMU_TELEPORT_EXPRESS_REGISTER_H
#define QEMU_TELEPORT_EXPRESS_REGISTER_H
#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_device_common.h"

#include "hw/teleport-express/teleport_express_call.h"


#define IRQ_SET_OK 0
#define IRQ_NOT_ENABLE -1
#define IRQ_NOT_READY -2
#define IRQ_RELEASED -3



void register_input_buffer_call(VirtIODevice *vdev, VirtQueue *vq);

// void send_express_device_irq(Teleport_Express_Call *irq_call, int buf_index, int len);

int set_express_device_irq(Device_Context *device_context, int buf_index, int len);

void *input_sync_thread(void *opaque);

void express_input_device_sync(void);

#endif