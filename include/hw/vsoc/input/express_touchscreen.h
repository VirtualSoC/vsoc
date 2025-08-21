#ifndef EXPRESS_TOUCHSCREEN_H
#define EXPRESS_TOUCHSCREEN_H

#include "hw/vsoc/gpu/express_display.h"

#define MAX_TOUCH_POINT 10

#define MAX_RECORD_SLOT 100
#define MAX_RECORD_NUM 100

typedef struct Touchscreen_Prop
{
    int count;
    int width;
    int height;
} __attribute__((packed, aligned(4))) Touchscreen_Prop;

extern Touchscreen_Prop touchscreen_prop;

void set_touchscreen_window_size(GLFWwindow *window, int max_width, int max_height, int rotation);

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

void load_touchscreen_context(QEMUFile *f);
void save_touchscreen_context(QEMUFile* f);

#endif
