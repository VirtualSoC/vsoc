#ifndef EXPRESS_GYRO_H
#define EXPRESS_GYRO_H

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

void load_gyro_context(QEMUFile *f);
void save_gyro_context(QEMUFile *f);

void sync_express_gyro_status(void);

#endif