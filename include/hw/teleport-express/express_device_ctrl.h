#ifndef QEMU_EXPRESS_DEVICE_CTRL_H
#define QEMU_EXPRESS_DEVICE_CTRL_H

#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/express_device_common.h"




#define FUNID_getExpressDeviceNum ((EXPRESS_CTRL_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 1)
#define FUNID_getExpressDeviceNames ((EXPRESS_CTRL_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 2)
#define FUNID_getExpressDeviceLogSettingInfo ((EXPRESS_CTRL_DEVICE_ID << 32u) + (((unsigned long long)0x1) << 24u) + 3)


void express_device_ctrl_invoke(Teleport_Express_Call *call);


#endif