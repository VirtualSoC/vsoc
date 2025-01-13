#ifndef EXPRESS_TOUCHSCREEN_H
#define EXPRESS_TOUCHSCREEN_H

#include "hw/express-gpu/express_display.h"

#define MAX_TOUCH_POINT 10

#define MAX_RECORD_SLOT 100
#define MAX_RECORD_NUM 100

void set_touchscreen_window_size(GLFWwindow *window, int max_width, int max_height);

void start_mouse_record(GLFWwindow *window, int index);
void stop_mouse_record(GLFWwindow *window);

bool start_mouse_replay(GLFWwindow *window, int index);
bool check_mouse_is_replaying(GLFWwindow *window, int index);
bool stop_mouse_replay(GLFWwindow *window, int index);

void express_touchscreen_mouse_move_handle(GLFWwindow *window, double xpos, double ypos);
void express_touchscreen_mouse_click_handle(GLFWwindow *window, int button, int action, int mods);
void express_touchscreen_mouse_scroll_handle(GLFWwindow *window, double xoffset, double yoffset);
void express_touchscreen_touch_handle(GLFWwindow *window, int touch, int action, double xpos, double ypos);
void express_touchscreen_entered_handle(GLFWwindow *window, int entered);

void sync_express_touchscreen_input(GLFWwindow *window, bool need_send);

#endif
