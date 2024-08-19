#ifndef DCODEC_H
#define DCODEC_H

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"
#include "hw/teleport-express/teleport_express_distribute.h"

void dcodec_master_switch(struct Thread_Context *context, Teleport_Express_Call *call);

#endif