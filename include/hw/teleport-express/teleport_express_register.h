#ifndef QEMU_TELEPORT_EXPRESS_REGISTER_H
#define QEMU_TELEPORT_EXPRESS_REGISTER_H
#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_device_common.h"

#include "hw/teleport-express/teleport_express_call.h"





void register_input_buffer_call(VirtIODevice *vdev, VirtQueue *vq);

void send_express_device_irq(Teleport_Express_Call *irq_call, int buf_index, int len);

void express_input_device_sync(void);

#endif