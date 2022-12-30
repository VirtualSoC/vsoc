#ifndef EXPRESS_ACCEL_H
#define EXPRESS_ACCEL_H

#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"

enum {
    EXPRESS_ACCEL_X = 0,
    EXPRESS_ACCEL_Y,
    EXPRESS_ACCEL_Z,
    EXPRESS_ACCEL_SCALE,
    EXPRESS_ACCEL_SAMPLE_FREQUENCY,
    EXPRESS_ACCEL_TEMPERATURE,
    EXPRESS_ACCEL_VOLTAGE
};

void express_accel_status_changed(int status_type, int value);

void sync_express_accel_status(void);

#endif