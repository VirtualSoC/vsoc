#ifndef EXPRESS_TOUCHSCREEN_H
#define EXPRESS_TOUCHSCREEN_H

#include "hw/teleport-express/express_log.h"

#include "hw/teleport-express/express_device_common.h"
#include "hw/teleport-express/teleport_express_register.h"

#include "hw/express-gpu/GLFW/glfw3.h"
#include "hw/express-gpu/GLFW/glfw3native.h"

#define MAX_TOUCH_POINT 10

#define MAX_RECORD_SLOT 100
#define MAX_RECORD_NUM 100

void set_touchscreen_window_size(int max_width, int max_height);

void start_mouse_record(int index);
void stop_mouse_record(void);

bool start_mouse_replay(int index);
bool check_mouse_is_replaying(int index);
bool stop_mouse_replay(int index);

void express_touchscreen_mouse_move_handle(GLFWwindow *window, double xpos, double ypos);
void express_touchscreen_mouse_click_handle(GLFWwindow *window, int button, int action, int mods);
void express_touchscreen_mouse_scroll_handle(GLFWwindow *window, double xoffset, double yoffset);
void express_touchscreen_touch_handle(GLFWwindow *window, int touch, int action, double xpos, double ypos);
void express_touchscreen_entered_handle(GLFWwindow *window, int entered);

void reset_touchscreen_input(void);

void set_express_touchscreen_input(int x, int y, int is_touched, int index);

void sync_express_touchscreen_input(bool need_send);

#endif
