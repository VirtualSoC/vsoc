#ifndef EXPRESS_KEYBOARD_H
#define EXPRESS_KEYBOARD_H

#include "hw/vsoc/express_log.h"
#include "hw/vsoc/input/express_touchscreen.h"

// 这个范围内，所有glfw所转化的按键都能传过去，且linux系统基本的按键都能传过去
// 对于安卓而言，就是差个KEY_APPSELECT，也就是差个最近任务键
#define MAX_KEY_CODE 249

#define KEY_ESC			1

#define KEY_LEFTALT		56

#define KEY_PAGEUP		104
#define KEY_PAGEDOWN		109

#define KEY_VOLUMEDOWN		114
#define KEY_VOLUMEUP		115

#define KEY_POWER		116	/* SC System Power Down */

#define KEY_LEFTMETA		125
#define KEY_RIGHTMETA		126

#define KEY_MENU		139	/* Menu (show menu) */
#define KEY_BACK		158	/* AC Back */
#define KEY_HOMEPAGE		172	/* AC Home */

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

extern int express_keyboard_count;

void express_keyboard_handle_callback(GLFWwindow *window, int key, int code, int action, int mods);
void load_keyboard_context(QEMUFile *f);
void save_keyboard_context(QEMUFile *f);

void sync_express_keyboard_input(GLFWwindow *window, bool need_send);

#endif