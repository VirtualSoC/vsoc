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

// 全键无冲设计
typedef struct Keyboard_Data
{
    bool key[MAX_KEY_CODE];
    bool key_is_refresh[MAX_KEY_CODE];
} __attribute__((packed, aligned(4))) Keyboard_Data;

typedef struct Keyboard_Context
{
    Device_Context device_context;
    Keyboard_Data data;
    Guest_Mem *guest_buffer;
    bool need_sync;
} Keyboard_Context;

static Keyboard_Context static_keyboard_context;

// static bool keyboard_irq_enable = false;

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
            static_keyboard_context.data.key[linux_code] = (action != GLFW_RELEASE);
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

    if (!static_keyboard_context.device_context.irq_enabled)
    {
        printf("express_keyboard irq is not ok\n");
        static_keyboard_context.need_sync = false;
        return;
    }

    // Teleport_Express_Call *origin_call = NULL;
    // if ((origin_call = qatomic_xchg(&static_keyboard_context.irq_call, NULL)) == NULL)
    // {
    //     printf("keyboard irq not ok!\n");
    //     return;
    // }

    // if (origin_call == (void *)1)
    // {
    //     printf("keyboard has been released!\n");
    //     return;
    // }

    // write_to_guest_mem(static_keyboard_context.guest_buffer, &(static_keyboard_context.data), 0, sizeof(Keyboard_Data));

    // static_keyboard_context.need_sync = false;
    // memset(static_keyboard_context.data.key_is_refresh, 0, sizeof(static_keyboard_context.data.key_is_refresh));

    // // printf("keyboard irq send ok\n");

    // send_express_device_irq(origin_call, 0, sizeof(Keyboard_Data));


    write_to_guest_mem(static_keyboard_context.guest_buffer, &(static_keyboard_context.data), 0, sizeof(Keyboard_Data));

    static_keyboard_context.need_sync = false;
    memset(static_keyboard_context.data.key_is_refresh, 0, sizeof(static_keyboard_context.data.key_is_refresh));

    set_express_device_irq((Device_Context *)&static_keyboard_context, 0, sizeof(Keyboard_Data));
    return;
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

// static void keyboard_irq_register(Teleport_Express_Call *call)
// {
//     express_printf("keyboard register irq\n");

//     Teleport_Express_Call *origin_call = NULL;
//     if ((origin_call = qatomic_xchg(&static_keyboard_context.irq_call, call)) != NULL)
//     {
//         if (origin_call == (void *)1)
//         {
//             // 此时已经release过了，所以此时需要直接发送call
//             // 但是可能此时继续产生send irq的中断请求，只是send出去的不会进行重置，所以这里进行二次交换，假如换到NULL，说明irq call被input函数发送出去了，就不用管了
//             if ((origin_call = qatomic_xchg(&static_keyboard_context.irq_call, NULL)) != NULL)
//             {
//                 // 这里origin_call不可能再次为1，因为已经release过一次了
//                 if (origin_call == (void *)1)
//                 {
//                     printf("error! keyboard register with half-released status get one release 1!\n");
//                     return;
//                 }
//                 send_express_device_irq(origin_call, 0, 0);
//                 printf("keyboard release bewteen send and reset\n");
//                 return;
//             }
//         }
//     }

//     keyboard_irq_enable = true;
// }

// static void keyboard_irq_release(Teleport_Express_Call *call)
// {
//     keyboard_irq_enable = false;
//     printf("keyboard_irq_release\n");

//     Teleport_Express_Call *origin_call = NULL;
//     if ((origin_call = qatomic_xchg(&static_keyboard_context.irq_call, 1)) != NULL)
//     {
//         if (origin_call != (void *)1)
//         {
//             send_express_device_irq(origin_call, 0, 0);

//             // 在irq_call被release函数获取时，不可能存在进一步的中断注入，因而也不可能出现中断的重置，所以可以放心设置为NULL
//             // 其他情况意味着在等待下一次中断重置过程中
//             qatomic_xchg(&static_keyboard_context.irq_call, NULL);
//             printf("keyboard_irq_release\n");
//         }
//         else
//         {
//             printf("error! keyboard release twice!\n");
//         }
//     }
// }

static Device_Context *get_keyboard_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    return (Device_Context *)&static_keyboard_context;
}

static Express_Device_Info express_keyboard_info = {
    .enable_default = true,
    .name = "express-keyboard",
    .option_name = "keyboard",
    .driver_name = "express_keyboard",
    .device_id = EXPRESS_KEYBOARD_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_keyboard_context,
    .buffer_register = keyboard_buffer_register,
    // .irq_register = keyboard_irq_register,
    // .irq_release = keyboard_irq_release,

};

EXPRESS_DEVICE_INIT(express_keyboard, &express_keyboard_info)
