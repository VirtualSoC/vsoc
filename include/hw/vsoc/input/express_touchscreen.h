#ifndef EXPRESS_TOUCHSCREEN_H
#define EXPRESS_TOUCHSCREEN_H

#include "hw/vsoc/display/express_display.h"

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

typedef struct Touchscreen_Data {
	int touch_x[10];
	int touch_y[10];
	int is_touched[10];
	int touch_cnt;
} __attribute__((packed, aligned(4))) Touchscreen_Data;

typedef struct Touchscreen_Context
{
    Device_Context device_context;
    int id;
    Touchscreen_Data data;

    int finger_used[MAX_TOUCH_POINT];
    Guest_Mem *guest_buffer;
    bool need_sync;

    // 触摸屏的物理大小
    int touchscreen_width;
    int touchscreen_height;

    // qemu的窗口大小
    int window_width;
    int window_height;
    int window_rotation;

    int current_finger_xpos;
    int current_finger_ypos;

    int current_scroll_ypos1;
    int current_scroll_ypos2;

    bool left_mouse_press;
    bool right_mouse_press;
    int mouse_left_finger;
    int mouse_right_finger1;
    int mouse_right_finger2;

    int finger_xpos_record[MAX_RECORD_SLOT][MAX_RECORD_NUM];
    int finger_ypos_record[MAX_RECORD_SLOT][MAX_RECORD_NUM];

    bool finger_is_record[MAX_RECORD_SLOT];
    bool finger_is_replay[MAX_RECORD_SLOT];

    int current_record_index;
    int current_record_cnt;

    int current_replay_cnt[MAX_RECORD_SLOT];
    int current_replay_finger_num[MAX_RECORD_SLOT];

    int scroll_yoffset;
    bool is_scrolling;
    int scroll_finger1;
    int scroll_finger2;

    GHashTable *real_touch_id_map;
    GHashTable *touch_refresh_save_map;
} Touchscreen_Context;

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
