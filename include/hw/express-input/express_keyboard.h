#ifndef EXPRESS_KEYBOARD_H
#define EXPRESS_KEYBOARD_H

#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

//这个范围内，所有的GLFW code里就只有GLFW_KEY_MENU传不过去（对应的KEY_MENU）
#define MAX_KEY_CODE 128

void express_keyboard_handle_callback(GLFWwindow *window, int key, int code, int action, int mods);

void sync_express_keyboard_input(void);

#endif