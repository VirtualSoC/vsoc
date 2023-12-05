#ifndef EXPRESS_SYNC_H
#define EXPRESS_SYNC_H

#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"


#define MAX_SYNC_NUM 512


void signal_express_sync(int sync_id, bool need_gpu_sync);
void wait_for_express_sync(int sync_id, bool need_gpu_sync);


#endif