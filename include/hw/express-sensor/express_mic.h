#ifndef EXPRESS_MICROPHONE_H
#define EXPRESS_MICROPHONE_H

#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"

void express_mic_status_changed(const void *buf, int size);

void sync_express_mic_status(void);

int start_capture_from_file(char *path);

int start_capture(void);

void stop_capture(void);
#endif