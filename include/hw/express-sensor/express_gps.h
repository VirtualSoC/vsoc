#ifndef EXPRESS_GPS_H
#define EXPRESS_GPS_H

#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"

enum {
    EXPRESS_GPS_LATITUDE = 0,
    EXPRESS_GPS_LONGITUDE,
    EXPRESS_GPS_GROUND_SPEED,
    EXPRESS_GPS_ALTITUDE,
    EXPRESS_GPS_SPEED_DIRECTION,
    EXPRESS_GPS_NUM_ACTIVE_SV,
    EXPRESS_GPS_HDOP,
    EXPRESS_GPS_PDOP,
    EXPRESS_GPS_VDOP,
    EXPRESS_GPS_ACTIVE_SV_ID,
    EXPRESS_GPS_NUM_SV_INVIEW,
    EXPRESS_GPS_SV_INVIEW_ID,
    EXPRESS_GPS_SV_INVIEW_ELEVATION,
    EXPRESS_GPS_SV_INVIEW_AZIMUTH,
    EXPRESS_GPS_SV_INVIEW_SNR,
    EXPRESS_GPS_YEAR,
    EXPRESS_GPS_MONTH,
    EXPRESS_GPS_DAY,
    EXPRESS_GPS_HOUR,
    EXPRESS_GPS_MINUTE,
    EXPRESS_GPS_SECOND
};

void express_gps_status_changed(int status_type, int value);

void sync_express_gps_status(void);

#endif