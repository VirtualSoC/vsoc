/**
 * @file express_gps.c
 * @author Cheng en Huang (diandian86@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_device.h"
#include "hw/vsoc/sensor/express_gps.h"
#include "hw/vsoc/gpu/express_gpu_snapshot.h"

typedef struct
{
    uint8_t new_data : 1;
    uint8_t gps2dfix : 1;
    uint8_t gps3dfix : 1;
    uint8_t wp_reached : 1;
    uint8_t numsats : 4;
} Status_Register;

typedef struct
{
    int lat; // degree*10 000 000
    int lon; // degree*10 000 000
} GPS_Coordinates;

// sometimes it's more efficient to read a block of registers, such as these ones which are all together consecutively
typedef struct
{
    uint8_t prn;       // ID from 0~99
    uint8_t elevation; // in degrees (maximun 90)
    uint8_t snr;       // SNR, 00-99 dB (null when not tracking)
    uint16_t azimuth;  // degrees from true north, 000 to 359
} GPS_Satellite;

// sometimes it's more efficient to read a block of registers, such as these ones which are all together consecutively
typedef struct
{
    uint8_t active_sv_prn[12]; // current using PRN ID of satallies
    uint8_t num_sv_inview;
    uint8_t pdop;
    uint8_t hdop;
    uint8_t vdop;
    uint8_t month;
    uint8_t day;
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
    uint16_t ground_speed; // m/s*100
    uint16_t altitude;     // meters
    uint16_t speed_dir;    // speed directions in degree
    uint16_t year;
    GPS_Satellite sv_inview[16];
} GPS_Detail;

typedef struct Express_GPS_Data
{
    Status_Register status;
    GPS_Coordinates location;
    GPS_Detail detail;
} __attribute__((packed, aligned(4))) Express_GPS_Data;

typedef struct GPS_Context
{
    Device_Context device_context;
    Express_GPS_Data data;
    Guest_Mem *guest_buffer;
    bool need_sync;
} GPS_Context;

static GPS_Context static_gps_context = {
    .data = {
        // St Petersburg, Florida
        .status = {1, 1, 1, 0, 9},
        .location = {116326759, 40003304}, // lat, lon
        .detail = {
            {22, 11, 27, 01, 03, 02, 10, 21, 19, 0, 0, 0}, // SV IDs
            15,                                            // nums of satellites in view
            17,
            10,
            13, // pdop, hdop, vdop
            12,
            30,
            12,
            0,
            0,
            0,    // ground speed
            1050, // altitude
            0,    // ground course
            2022, // gps year
            {     // satellites in view
             {26, 25, 53, 138},
             {16, 25, 67, 91},
             {01, 51, 77, 238},
             {02, 45, 41, 85},
             {03, 38, 01, 312},
             {30, 68, 37, 187},
             {11, 22, 44, 49},
             {9, 67, 71, 76},
             {10, 14, 12, 177},
             {19, 86, 37, 235},
             {21, 84, 95, 343},
             {22, 77, 66, 40},
             {8, 50, 60, 177},
             {6, 81, 46, 336},
             {27, 63, 83, 209},
             {0, 0, 0, 0}}}}};

static bool gps_data_init = false;

#ifdef ENABLE_SNAPSHOT

void save_gps_data(QEMUFile *f, Express_GPS_Data *data)
{
    qemu_put_be32(f, data->status.new_data);
    qemu_put_be32(f, data->status.gps2dfix);
    qemu_put_be32(f, data->status.gps3dfix);
    qemu_put_be32(f, data->status.wp_reached);
    qemu_put_be32(f, data->status.numsats);
    qemu_put_be32(f, data->location.lat);
    qemu_put_be32(f, data->location.lon);
    qemu_put_be32(f, data->detail.num_sv_inview);
    qemu_put_be32(f, data->detail.pdop);
    qemu_put_be32(f, data->detail.hdop);
    qemu_put_be32(f, data->detail.vdop);
    qemu_put_be32(f, data->detail.month);
    qemu_put_be32(f, data->detail.day);
    qemu_put_be32(f, data->detail.hr);
    qemu_put_be32(f, data->detail.min);
    qemu_put_be32(f, data->detail.sec);
    qemu_put_be32(f, data->detail.ground_speed);
    qemu_put_be32(f, data->detail.altitude);
    qemu_put_be32(f, data->detail.speed_dir);
    qemu_put_be32(f, data->detail.year);

    for (int i = 0; i < 16; i++)
    {
        qemu_put_be32(f, data->detail.sv_inview[i].prn);
        qemu_put_be32(f, data->detail.sv_inview[i].elevation);
        qemu_put_be32(f, data->detail.sv_inview[i].snr);
        qemu_put_be32(f, data->detail.sv_inview[i].azimuth);
    }

    for (int i = 0; i < 12; i++)
    {
        qemu_put_be32(f, data->detail.active_sv_prn[i]);
    }
}

void load_gps_data(QEMUFile *f, Express_GPS_Data *data)
{
    data->status.new_data = qemu_get_be32(f);
    data->status.gps2dfix = qemu_get_be32(f);
    data->status.gps3dfix = qemu_get_be32(f);
    data->status.wp_reached = qemu_get_be32(f);
    data->status.numsats = qemu_get_be32(f);
    data->location.lat = qemu_get_be32(f);
    data->location.lon = qemu_get_be32(f);
    data->detail.num_sv_inview = qemu_get_be32(f);
    data->detail.pdop = qemu_get_be32(f);
    data->detail.hdop = qemu_get_be32(f);
    data->detail.vdop = qemu_get_be32(f);
    data->detail.month = qemu_get_be32(f);
    data->detail.day = qemu_get_be32(f);
    data->detail.hr = qemu_get_be32(f);
    data->detail.min = qemu_get_be32(f);
    data->detail.sec = qemu_get_be32(f);
    data->detail.ground_speed = qemu_get_be32(f);
    data->detail.altitude = qemu_get_be32(f);
    data->detail.speed_dir = qemu_get_be32(f);
    data->detail.year = qemu_get_be32(f);

    for (int i = 0; i < 16; i++)
    {
        data->detail.sv_inview[i].prn = qemu_get_be32(f);
        data->detail.sv_inview[i].elevation = qemu_get_be32(f);
        data->detail.sv_inview[i].snr = qemu_get_be32(f);
        data->detail.sv_inview[i].azimuth = qemu_get_be32(f);
    }

    for (int i = 0; i < 12; i++)
    {
        data->detail.active_sv_prn[i] = qemu_get_be32(f);
    }
}

void save_gps_context(QEMUFile* f){
    save_gps_data(f, &(static_gps_context.data));
    save_guest_mem(f, static_gps_context.guest_buffer);
    qemu_put_be32(f, static_gps_context.need_sync);
    qemu_put_be32(f, static_gps_context.device_context.irq_enabled);

    if(static_gps_context.device_context.irq_call == NULL){
        qemu_put_be32(f, 0);
    } else {
        qemu_put_be32(f, 1);
        save_teleport_express_call(f, static_gps_context.device_context.irq_call);
    }    
}

void load_gps_context(QEMUFile *f){
    load_gps_data(f, &(static_gps_context.data));
    static_gps_context.guest_buffer = load_guest_mem(f, 0);
    static_gps_context.need_sync = qemu_get_be32(f);
    static_gps_context.device_context.irq_enabled = qemu_get_be32(f);

    Express_Device_Info *device_info = get_express_device_info(EXPRESS_GPS_DEVICE_ID);
    static_gps_context.device_context.device_info = device_info;

    int has_call = qemu_get_be32(f);
    if(has_call) {
        static_gps_context.device_context.irq_call = load_teleport_express_call(f);
    }
}

#endif

void express_gps_status_changed(int status_type, int value)
{

    switch (status_type)
    {
    case EXPRESS_GPS_LATITUDE:
        static_gps_context.data.location.lat = value;
        break;
    case EXPRESS_GPS_LONGITUDE:
        static_gps_context.data.location.lon = value;
        break;
    case EXPRESS_GPS_GROUND_SPEED:
        static_gps_context.data.detail.ground_speed = value;
        break;
    case EXPRESS_GPS_ALTITUDE:
        static_gps_context.data.detail.altitude = value;
        break;
    case EXPRESS_GPS_SPEED_DIRECTION:
        static_gps_context.data.detail.speed_dir = value;
        break;
    case EXPRESS_GPS_YEAR:
        static_gps_context.data.detail.year = value;
        break;
    case EXPRESS_GPS_MONTH:
        static_gps_context.data.detail.month = value;
        break;
    case EXPRESS_GPS_DAY:
        static_gps_context.data.detail.day = value;
        break;
    case EXPRESS_GPS_HOUR:
        static_gps_context.data.detail.hr = value;
        break;
    case EXPRESS_GPS_MINUTE:
        static_gps_context.data.detail.min = value;
        break;
    case EXPRESS_GPS_SECOND:
        static_gps_context.data.detail.sec = value;
        break;
    case EXPRESS_GPS_HDOP:
        static_gps_context.data.detail.hdop = value;
        break;
    case EXPRESS_GPS_PDOP:
        static_gps_context.data.detail.pdop = value;
        break;
    case EXPRESS_GPS_VDOP:
        static_gps_context.data.detail.vdop = value;
        break;
    case EXPRESS_GPS_NUM_ACTIVE_SV:
        static_gps_context.data.status.numsats = value;
        break;
    case EXPRESS_GPS_ACTIVE_SV_ID:
        static_gps_context.data.detail.active_sv_prn[value % 100] = value / 100;
        break;
    case EXPRESS_GPS_NUM_SV_INVIEW:
        static_gps_context.data.detail.num_sv_inview = value;
    default:
        return;
    }

    static_gps_context.need_sync = true;
}

void sync_express_gps_status(void)
{
    if (!static_gps_context.device_context.irq_enabled)
    {
        return;
    }

    if (!static_gps_context.need_sync)
    {
        return;
    }

    g_ops.write_to_guest_mem(static_gps_context.guest_buffer, &(static_gps_context.data), 0, sizeof(Express_GPS_Data));

    static_gps_context.need_sync = false;

    express_printf("gps irq send ok\n");

    g_ops.set_express_device_irq((Device_Context *)&static_gps_context, 0, sizeof(Express_GPS_Data));
}

static void gps_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, uint64_t user_id, Express_Device_Info *info)
{
    if (static_gps_context.guest_buffer != NULL)
    {
        free_duplicated_guest_mem(static_gps_context.guest_buffer);
    }
    printf("gps register buffer\n");
    static_gps_context.guest_buffer = data;
}

static void gps_irq_register(Device_Context *context)
{

    if (!gps_data_init && static_gps_context.guest_buffer != NULL)
    {
        gps_data_init = true;
        static_gps_context.need_sync = true;
        sync_express_gps_status();
    }
}

static Device_Context *get_gps_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    return (Device_Context *)&static_gps_context;
}

static Express_Device_Info express_gps_info = {
    .enable_default = true,
    .name = "express-gps",
    .option_name = "gps",
    .driver_name = "express_gps",
    .device_id = EXPRESS_GPS_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_gps_context,
    .buffer_register = gps_buffer_register,
    .irq_register = gps_irq_register,

};

EXPRESS_DEVICE_INIT(express_gps, &express_gps_info)
