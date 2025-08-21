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
#include "hw/teleport-express/express_platform.h"
#include "hw/express-input/express_keyboard.h"
#include "hw/express-gpu/express_gpu_snapshot.h"

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
    int id;
    Keyboard_Data data;
    Guest_Mem *guest_buffer;
    bool need_sync;
} Keyboard_Context;

int express_keyboard_count = 1;
bool express_keyboard_finger_replay = false;
GHashTable *g_keyboard_contexts = NULL;

static inline Keyboard_Context *get_keyboard_context(GLFWwindow *window)
{
    Display_Context *disp = (Display_Context *)glfwGetWindowUserPointer(window);
    Keyboard_Context *context = (Keyboard_Context *)g_hash_table_lookup(g_keyboard_contexts, GUINT_TO_POINTER(disp->unique_id));
    if (context == NULL) {
        LOGE("error! no keyboard context with id %" PRIu64 " found!", disp->unique_id);
    }
    return context;
}

#ifdef ENABLE_SNAPSHOT

void save_keyboard_context(QEMUFile* f){
    // save total keyboard count
    int context_count = (int)g_hash_table_size(g_keyboard_contexts);
    qemu_put_be32(f, context_count);

    // iter through all keyboard contexts and save them
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, g_keyboard_contexts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Keyboard_Context *context = (Keyboard_Context *)value;
        qemu_put_buffer(f, (uint8_t *)context, sizeof(Keyboard_Context));

        save_guest_mem(f, context->guest_buffer);

        if(context->device_context.irq_call == NULL){
            qemu_put_be32(f, 0);
        } else {
            qemu_put_be32(f, 1);
            save_teleport_express_call(f, context->device_context.irq_call);
        }
    }
}

void load_keyboard_context(QEMUFile *f){
    // clear all old keyboard contexts
    if (g_keyboard_contexts != NULL) {
        g_hash_table_remove_all(g_keyboard_contexts);
    }

    // get total keyboard count
    int context_count = qemu_get_be32(f);

    // load all keyboard contexts
    for (int i = 0; i < context_count; i++) {
        Keyboard_Context *context = g_malloc0(sizeof(Keyboard_Context));
        qemu_get_buffer(f, (uint8_t *)context, sizeof(Keyboard_Context));

        // these resources needs to be re-created
        context->guest_buffer = load_guest_mem(f, 0);

        int has_call = qemu_get_be32(f);
        if (has_call) {
            context->device_context.irq_call = load_teleport_express_call(f);
        }
        g_hash_table_insert(g_keyboard_contexts, GUINT_TO_POINTER(context->id), (gpointer)context);
    }
}

#endif

void express_keyboard_handle_callback(GLFWwindow *window, int key, int code, int action, int mods)
{
    Keyboard_Context *context = get_keyboard_context(window);

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
            start_mouse_record(window, linux_code);
        }
        else if (action == GLFW_RELEASE)
        {
            stop_mouse_record(window);
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
                    g_ops.express_gpu_keep_window_scale = save_scale;
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
                    save_scale = g_ops.express_gpu_keep_window_scale;
                    glfwGetWindowPos(window, &windowed_x, &windowed_y);
                    glfwGetWindowSize(window, &windowed_width, &windowed_height);
                    g_ops.express_gpu_keep_window_scale = false;
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
        context->data.key[linux_code] = (action != GLFW_RELEASE);
        context->data.key_is_refresh[linux_code] = true;
        context->need_sync = true;
    }
    else
    {
        if ((action == GLFW_PRESS && !start_mouse_replay(window, linux_code)) ||
            (action == GLFW_REPEAT && !check_mouse_is_replaying(window, linux_code)) ||
            (action == GLFW_RELEASE && !stop_mouse_replay(window, linux_code)))
        {
            context->data.key[linux_code] = (action != GLFW_RELEASE);
            context->data.key_is_refresh[linux_code] = true;
            context->need_sync = true;
        }
    }
}

void sync_express_keyboard_input(GLFWwindow *window, bool need_send)
{
    Keyboard_Context *context = get_keyboard_context(window);

    if (!context->need_sync)
    {
        return;
    }

    if (!context->device_context.irq_enabled)
    {
        printf("express_keyboard irq is not ok\n");
        context->need_sync = false;
        return;
    }

    if (need_send)
    {
        g_ops.write_to_guest_mem(context->guest_buffer, &(context->data), 0, sizeof(Keyboard_Data));
        g_ops.set_express_device_irq((Device_Context *)context, 0, sizeof(Keyboard_Data));
    }

    context->need_sync = false;
    memset(context->data.key_is_refresh, 0, sizeof(context->data.key_is_refresh));

    return;
}

static void keyboard_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    Keyboard_Context *context = (Keyboard_Context *)g_hash_table_lookup(g_keyboard_contexts, GUINT_TO_POINTER(unique_id));

    if (context->guest_buffer != NULL)
    {
        free_duplicated_guest_mem(context->guest_buffer);
    }
    LOGI("keyboard %d register buffer", unique_id);
    context->guest_buffer = data;
}

static void remove_keyboard_device_context(Device_Context *context) {
    g_free(context);
}

static Device_Context *get_keyboard_device_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (g_keyboard_contexts == NULL) {
        g_keyboard_contexts = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)remove_keyboard_device_context);
    }

    Keyboard_Context *context = (Keyboard_Context *)g_hash_table_lookup(g_keyboard_contexts, GUINT_TO_POINTER(unique_id));

    if (context == NULL) {
        context = g_malloc0(sizeof(Keyboard_Context));
        context->id = unique_id;
        g_hash_table_insert(g_keyboard_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }

    return (Device_Context *)context;
}

static Express_Device_Info express_keyboard_info = {
    .enable_default = true,
    .name = "express-keyboard",
    .option_name = "keyboard",
    .driver_name = "express_keyboard",
    .device_id = EXPRESS_KEYBOARD_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_keyboard_device_context,
    .buffer_register = keyboard_buffer_register,

    .static_prop = &express_keyboard_count,
    .static_prop_size = sizeof(express_keyboard_count),
};

EXPRESS_DEVICE_INIT(express_keyboard, &express_keyboard_info)
