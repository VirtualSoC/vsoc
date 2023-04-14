/**
 * @file express_accel.c
 * @author Cheng en Huang (diandian86@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG

#include "hw/express-sensor/express_accel.h"

typedef struct Express_Accel_Data
{
    int x;
    int y;
    int z;
    int scale;
    int sample_hz;
    int temperature;
    int voltage;
    int enable;
} __attribute__((packed, aligned(4))) Express_Accel_Data;

typedef struct Accel_Context
{
    Device_Context device_context;
    Express_Accel_Data data;
    Guest_Mem *guest_buffer;
    bool need_sync;
} Accel_Context;

static Accel_Context static_accel_context = {
    .data = {
        .x = 0,
        .y = 0,
        .z = 0,
        .scale = 781250,        // 0.78125 accelerometer measurement range, ex +- 4G => 9.81/4096 = 0.023951
        .sample_hz = 800000000, // 1.0
        .temperature = 30000000,
        .voltage = 230000,
        .enable = 1}};

void express_accel_status_changed(int status_type, int value)
{

    switch (status_type)
    {
    case EXPRESS_ACCEL_X:
        static_accel_context.data.x = value;
        break;
    case EXPRESS_ACCEL_Y:
        static_accel_context.data.y = value;
        break;
    case EXPRESS_ACCEL_Z:
        static_accel_context.data.z = value;
        break;
    case EXPRESS_ACCEL_SCALE:
        static_accel_context.data.scale = value;
        break;
    case EXPRESS_ACCEL_SAMPLE_FREQUENCY:
        static_accel_context.data.sample_hz = value;
        break;
    default:
        return;
    }

    static_accel_context.need_sync = true;
}

void sync_express_accel_status(void)
{
    if (!static_accel_context.device_context.irq_enabled)
    {
        return;
    }

    if (!static_accel_context.need_sync)
    {
        return;
    }

    write_to_guest_mem(static_accel_context.guest_buffer, &(static_accel_context.data), 0, sizeof(Express_Accel_Data));

    static_accel_context.need_sync = false;

    set_express_device_irq((Device_Context *)&static_accel_context, 0, sizeof(Express_Accel_Data));
}

static void accel_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (static_accel_context.guest_buffer != NULL)
    {
        free_copied_guest_mem(static_accel_context.guest_buffer);
    }
    printf("accel register buffer\n");
    static_accel_context.guest_buffer = data;
}

static Device_Context *get_accel_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    return (Device_Context *)&static_accel_context;
}

static Express_Device_Info express_accel_info = {
    .enable_default = true,
    .name = "express-accel",
    .option_name = "accel",
    .driver_name = "express_accel",
    .device_id = EXPRESS_ACCELEROMTETER_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_accel_context,
    .buffer_register = accel_buffer_register,

};

EXPRESS_DEVICE_INIT(express_accel, &express_accel_info)
