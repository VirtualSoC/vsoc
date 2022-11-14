/**
 * @file express_keyboard.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG

#include "hw/express-input/express_keyboard.h"
#include "hw/express-input/express_touchscreen.h"

#include "ui/input.h"

//全键无冲设计
typedef struct Keyboard_Data
{
    bool key[MAX_KEY_CODE];
    bool key_is_refresh[MAX_KEY_CODE];
} __attribute__((packed, aligned(4))) Keyboard_Data;

typedef struct Keyboard_Context
{
    Keyboard_Data data;
    Guest_Mem *guest_buffer;
    Teleport_Express_Call *irq_call;
    bool need_sync;
} Keyboard_Context;

static Keyboard_Context static_keyboard_context;

bool express_keyboard_finger_replay = false;

void express_keyboard_handle_callback(GLFWwindow *window, int key, int code, int action, int mods)
{

    if (code > qemu_input_map_glfw_to_qcode_len)
    {
        return;
    }
    int qcode = qemu_input_map_glfw_to_qcode[key];

    if (qcode > qemu_input_map_qcode_to_linux_len)
    {
        return;
    }

    int linux_code = qemu_input_map_qcode_to_linux[qcode];

    if (linux_code >= MAX_KEY_CODE)
    {
        return;
    }

    // 56是KEY_LEFTALT，不能用于记录（KEY_RIGHTALT是100）
    // record和replay、input只能三选一
    if (express_keyboard_finger_replay && (mods & GLFW_MOD_ALT) != 0 && linux_code < MAX_RECORD_SLOT && linux_code != 56)
    {
        if (action == GLFW_PRESS)
        {
            start_mouse_record(linux_code);
        }
        else if (action == GLFW_RELEASE)
        {
            stop_mouse_record();
        }
    }
    else
    {
        if ((action == GLFW_PRESS && !start_mouse_replay(linux_code)) ||
            (action == GLFW_REPEAT && !check_mouse_is_replaying(linux_code)) ||
            (action == GLFW_RELEASE && !stop_mouse_replay(linux_code)))
        {
            static_keyboard_context.data.key[linux_code] = true;
            static_keyboard_context.data.key_is_refresh[linux_code] = true;
            static_keyboard_context.need_sync = true;
        }
    }
}

void sync_express_keyboard_input(void)
{

    if (!static_keyboard_context.need_sync)
    {
        return;
    }
    if (static_keyboard_context.irq_call == NULL)
    {
        printf("irq not ok!\n");
        return;
    }

    write_to_guest_mem(static_keyboard_context.guest_buffer, &(static_keyboard_context.data), 0, sizeof(Keyboard_Data));

    static_keyboard_context.need_sync = false;
    memset(static_keyboard_context.data.key_is_refresh, 0, sizeof(static_keyboard_context.data.key_is_refresh));

    // printf("irq send ok\n");
    send_express_device_irq(static_keyboard_context.irq_call, 0, sizeof(Keyboard_Data));
    static_keyboard_context.irq_call = NULL;
}

static void keyboard_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (static_keyboard_context.guest_buffer != NULL)
    {
        free_copied_guest_mem(static_keyboard_context.guest_buffer);
    }
    printf("keyboard register buffer\n");
    static_keyboard_context.guest_buffer = data;
}

static void keyboard_irq_register(Teleport_Express_Call *call)
{
    // printf("touch register irq\n");

    static_keyboard_context.irq_call = call;
}

static Express_Device_Info express_keyboard_info = {
    .enable_default = true,
    .name = "express-keyboard",
    .option_name = "keyboard",
    .driver_name = "express_keyboard",
    .device_id = EXPRESS_KEYBOARD_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .buffer_register = keyboard_buffer_register,
    .irq_register = keyboard_irq_register,

};

EXPRESS_DEVICE_INIT(express_keyboard, &express_keyboard_info)
