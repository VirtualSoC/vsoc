/**
 * @file express_gyro.c
 * @author Cheng en Huang (diandian86@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG

#include "hw/express-sensor/express_gyro.h"

typedef struct Express_Gyro_Data
{
    int x;
    int y;
    int z;
    int scale;
    int sample_hz;
    int temperature;
    int voltage;
    int enable;
} __attribute__((packed, aligned(4))) Express_Gyro_Data;

typedef struct Gyro_Context
{
    Device_Context device_context;
    Express_Gyro_Data data;
    Guest_Mem *guest_buffer;
    bool need_sync;
} Gyro_Context;

static Gyro_Context static_gyro_context = {
    .data = {
        .x = 0,
        .y = 0,
        .z = 0,
        .scale = 231450,        // 0.78125 gyroscope measurement range, ex +- 4G => 9.81/4096 = 0.023951
        .sample_hz = 200000000, // 1.0
        .temperature = 30000000,
        .voltage = 230000,
        .enable = 1}};

static bool gyro_data_init = false;

void express_gyro_status_changed(int status_type, int value)
{

    switch (status_type)
    {
    case EXPRESS_GYRO_X:
        static_gyro_context.data.x = value;
        break;
    case EXPRESS_GYRO_Y:
        static_gyro_context.data.y = value;
        break;
    case EXPRESS_GYRO_Z:
        static_gyro_context.data.z = value;
        break;
    case EXPRESS_GYRO_SCALE:
        static_gyro_context.data.scale = value;
        break;
    case EXPRESS_GYRO_SAMPLE_FREQUENCY:
        static_gyro_context.data.sample_hz = value;
        break;
    default:
        return;
    }

    static_gyro_context.need_sync = true;
}

void sync_express_gyro_status(void)
{
    if (!static_gyro_context.device_context.irq_enabled)
    {
        return;
    }

    if (!static_gyro_context.need_sync)
    {
        return;
    }

    write_to_guest_mem(static_gyro_context.guest_buffer, &(static_gyro_context.data), 0, sizeof(Express_Gyro_Data));

    static_gyro_context.need_sync = false;

    express_printf("gyro irq send ok\n");

    set_express_device_irq((Device_Context *)&static_gyro_context, 0, sizeof(Express_Gyro_Data));
}

static void gyro_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (static_gyro_context.guest_buffer != NULL)
    {
        free_copied_guest_mem(static_gyro_context.guest_buffer);
    }
    printf("gyro register buffer\n");
    static_gyro_context.guest_buffer = data;
}

static void gyro_irq_register(Device_Context *context)
{

    if (!gyro_data_init && static_gyro_context.guest_buffer != NULL)
    {
        gyro_data_init = true;
        static_gyro_context.need_sync = true;
        sync_express_gyro_status();
    }
}

static Device_Context *get_gyro_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    return (Device_Context *)&static_gyro_context;
}

static Express_Device_Info express_gyro_info = {
    .enable_default = true,
    .name = "express-gyro",
    .option_name = "gyro",
    .driver_name = "express_gyro",
    .device_id = EXPRESS_GYROSCOPE_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_gyro_context,
    .buffer_register = gyro_buffer_register,
    .irq_register = gyro_irq_register,

};

EXPRESS_DEVICE_INIT(express_gyro, &express_gyro_info)
