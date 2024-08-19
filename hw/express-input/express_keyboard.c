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
    if (express_keyboard_finger_replay && (mods & GLFW_MOD_ALT) != 0 && linux_code < MAX_RECORD_SLOT && linux_code != KEY_LEFTALT)
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
    else if ((mods & GLFW_MOD_CONTROL) != 0 && linux_code == KEY_LEFTMETA)
    {
        // linux_code等于125是左边的windows键（或者是command键）
        // 按住ctrl加windows键能在不同屏幕之间全屏切换
        if (action == GLFW_PRESS)
        {
            static int windowed_x, windowed_y, windowed_width, windowed_height, monitor_index = 0, monitor_num = 0, save_scale;
            GLFWmonitor **monitors = glfwGetMonitors(&monitor_num);
            if (monitor_index >= monitor_num)
            {
                if (glfwGetWindowMonitor(window))
                {
                    express_gpu_keep_window_scale = save_scale;
                    glfwSetWindowMonitor(window, NULL,
                                         windowed_x, windowed_y,
                                         windowed_width, windowed_height, 0);
                }
                monitor_index = 0;
            }
            else if (monitor_num > 0)
            {
                GLFWmonitor *monitor = monitors[monitor_index];
                const GLFWvidmode *mode = glfwGetVideoMode(monitor);
                if (glfwGetWindowMonitor(window) == NULL)
                {
                    // 保存这个是否scale，防止全屏时，计算触控区域出现异常
                    save_scale = express_gpu_keep_window_scale;
                    glfwGetWindowPos(window, &windowed_x, &windowed_y);
                    glfwGetWindowSize(window, &windowed_width, &windowed_height);
                    express_gpu_keep_window_scale = false;
                    ;
                }
                glfwSetWindowMonitor(window, monitor,
                                     0, 0, mode->width, mode->height,
                                     mode->refreshRate);
                monitor_index++;
            }
        }
    }
    else if ((mods & GLFW_MOD_SHIFT) != 0 && (linux_code == KEY_ESC || linux_code == KEY_PAGEUP || linux_code == KEY_PAGEDOWN))
    {
        if (linux_code == KEY_ESC)
        {
            // shift加esc等于KEY_POWER按键
            linux_code = KEY_POWER;
        }
        else if (linux_code == KEY_PAGEUP)
        {
            // shift加pageup等于KEY_VOLUMEUP按键
            linux_code = KEY_VOLUMEUP;
        }
        else if (linux_code == KEY_PAGEDOWN)
        {
            // shift加pagedown等于KEY_VOLUMEDOWN按键
            linux_code = KEY_VOLUMEDOWN;
        }
        static_keyboard_context.data.key[linux_code] = (action != GLFW_RELEASE);
        static_keyboard_context.data.key_is_refresh[linux_code] = true;
        static_keyboard_context.need_sync = true;
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

void sync_express_keyboard_input(bool need_send)
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

    if (need_send)
    {
        write_to_guest_mem(static_keyboard_context.guest_buffer, &(static_keyboard_context.data), 0, sizeof(Keyboard_Data));
        set_express_device_irq((Device_Context *)&static_keyboard_context, 0, sizeof(Keyboard_Data));
    }

    static_keyboard_context.need_sync = false;
    memset(static_keyboard_context.data.key_is_refresh, 0, sizeof(static_keyboard_context.data.key_is_refresh));

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

};

EXPRESS_DEVICE_INIT(express_keyboard, &express_keyboard_info)
