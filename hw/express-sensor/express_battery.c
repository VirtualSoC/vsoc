/**
 * @file express_battery.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-11-23
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG

#include "hw/express-sensor/express_battery.h"

typedef struct Express_Battery_Data
{
    int status_changed;
    int online;
    int status;
    int health;
    int present;
    int capacity; // capacity in percents
    int voltage;
    int temperature; // temperature of the power supply.
    int technology;
    int cycle_count;
    int current_now;
    int charge_full;
    int charge_counter;
} __attribute__((packed, aligned(4))) Express_Battery_Data;

typedef struct Battery_Context
{
    Express_Battery_Data data;
    Guest_Mem *guest_buffer;
    Teleport_Express_Call *irq_call;
    bool need_sync;
} Battery_Context;

static Battery_Context static_battery_context = {
    .data = {
        .status_changed = BATTERY_STATUS_CHANGED,
        .online = 0,
        .status = POWER_SUPPLY_STATUS_DISCHARGING,
        .health = POWER_SUPPLY_HEALTH_GOOD,
        .present = 1,
        .capacity = 100,    // 100% charged
        .voltage = 4000000, // 4v
        .temperature = 250, // 25 Celsyus
        .technology = POWER_SUPPLY_TECHNOLOGY_LION,
        .cycle_count = 10,
        .current_now = 900000,  // 900mA
        .charge_full = 5000000, // 5000 mAH
        .charge_counter = 10000,
    }};

static bool battery_irq_enable = false;
static bool battery_data_init = false;

void express_ac_plug_status_changed(bool is_pluged)
{
    static_battery_context.data.online = is_pluged;

    static_battery_context.data.status = POWER_SUPPLY_STATUS_CHARGING;
    static_battery_context.data.status_changed |= AC_STATUS_CHANGED;

    static_battery_context.need_sync = true;

    if (static_battery_context.data.capacity == 100)
    {
        static_battery_context.data.status = POWER_SUPPLY_STATUS_FULL;
    }
}

void express_battery_status_changed(int status_type, int value)
{

    switch (status_type)
    {
    case POWER_SUPPLY_PROP_STATUS:
        static_battery_context.data.status = value;
        break;
    case POWER_SUPPLY_PROP_HEALTH:
        static_battery_context.data.health = value;
        break;
    case POWER_SUPPLY_PROP_PRESENT:
        static_battery_context.data.present = value;
        break;
    case POWER_SUPPLY_PROP_TECHNOLOGY:
        static_battery_context.data.technology = value;
        break;
    case POWER_SUPPLY_PROP_CAPACITY:
        static_battery_context.data.capacity = value;
        static_battery_context.data.status = POWER_SUPPLY_STATUS_DISCHARGING;
        break;
    case POWER_SUPPLY_PROP_VOLTAGE_NOW:
        static_battery_context.data.voltage = value;
        break;
    case POWER_SUPPLY_PROP_TEMP:
        static_battery_context.data.temperature = value;
        break;
    case POWER_SUPPLY_PROP_CYCLE_COUNT:
        static_battery_context.data.cycle_count = value;
        break;
    case POWER_SUPPLY_PROP_CURRENT_NOW:
        static_battery_context.data.current_now = value;
        break;
    case POWER_SUPPLY_PROP_CHARGE_FULL:
        static_battery_context.data.charge_full = value;
        break;
    case POWER_SUPPLY_PROP_CHARGE_COUNTER:
        static_battery_context.data.charge_counter = value;
        break;
    default:
        return;
    }

    static_battery_context.data.status_changed |= BATTERY_STATUS_CHANGED;
    static_battery_context.need_sync = true;
}

void sync_express_battery_status(void)
{

    if (!static_battery_context.need_sync || !battery_irq_enable)
    {
        return;
    }
    if (static_battery_context.irq_call == NULL)
    {
        printf("irq not ok!\n");
        return;
    }

    write_to_guest_mem(static_battery_context.guest_buffer, &(static_battery_context.data), 0, sizeof(Express_Battery_Data));

    static_battery_context.need_sync = false;
    static_battery_context.data.status_changed = 0;

    printf("battery irq send ok\n");

    send_express_device_irq(static_battery_context.irq_call, 0, sizeof(Express_Battery_Data));
    static_battery_context.irq_call = NULL;
}

static void battery_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (static_battery_context.guest_buffer != NULL)
    {
        free_copied_guest_mem(static_battery_context.guest_buffer);
    }
    printf("battery register buffer\n");
    static_battery_context.guest_buffer = data;
}

static void battery_irq_register(Teleport_Express_Call *call)
{
    printf("register irq\n");
    if (static_battery_context.irq_call != NULL)
    {
        send_express_device_irq(static_battery_context.irq_call, 0, 0);
    }

    battery_irq_enable = true;
    static_battery_context.irq_call = call;

    if (!battery_data_init && static_battery_context.guest_buffer != NULL)
    {
        battery_data_init = true;
        static_battery_context.need_sync = true;
        sync_express_battery_status();
    }
}

static void battery_irq_release(void)
{
    if (static_battery_context.irq_call != NULL)
    {
        send_express_device_irq(static_battery_context.irq_call, 0, 0);

        printf("battery_irq_release\n");
        battery_irq_enable = false;
        static_battery_context.irq_call = NULL;
    }
}

static Express_Device_Info express_battery_info = {
    .enable_default = true,
    .name = "express-battery",
    .option_name = "battery",
    .driver_name = "express_battery",
    .device_id = EXPRESS_BATTERY_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .buffer_register = battery_buffer_register,
    .irq_register = battery_irq_register,
    .irq_release = battery_irq_release,

};

EXPRESS_DEVICE_INIT(express_battery, &express_battery_info)
