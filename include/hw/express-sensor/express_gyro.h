#ifndef EXPRESS_GYRO_H
#define EXPRESS_GYRO_H

#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"

enum {
    EXPRESS_GYRO_X = 0,
    EXPRESS_GYRO_Y,
    EXPRESS_GYRO_Z,
    EXPRESS_GYRO_SCALE,
    EXPRESS_GYRO_SAMPLE_FREQUENCY,
    EXPRESS_GYRO_TEMPERATURE,
    EXPRESS_GYRO_VOLTAGE
};

void express_gyro_status_changed(int status_type, int value);

void sync_express_gyro_status(void);

#endif