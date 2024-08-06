/**
 * @file express_touchscreen.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-11-1
 *
 * @copyright Copyright (c) 2022
 *
 */

// #define STD_DEBUG_LOG

#include "hw/express-input/express_touchscreen.h"

int express_touchscreen_scroll_ratio = 10;
bool express_touchscreen_scroll_is_zoom = false;
bool express_touchscreen_right_click_is_two_finger = false;

#ifndef _WIN32
#define max(a, b)                       \
  (((a) > (b)) ? (a) : (b))
#define min(a, b)                       \
  (((a) < (b)) ? (a) : (b))
#endif


typedef struct Touchscreen_Prop
{
    int width;
    int height;
} __attribute__((packed, aligned(4))) Touchscreen_Prop;

typedef struct Touchscreen_Data
{
    int touch_x[MAX_TOUCH_POINT];
    int touch_y[MAX_TOUCH_POINT];
    int is_touched[MAX_TOUCH_POINT];
    int touch_cnt;
} __attribute__((packed, aligned(4))) Touchscreen_Data;

typedef struct Touchscreen_Context
{
    Device_Context device_context;
    Touchscreen_Data data;
    int finger_used[MAX_TOUCH_POINT];
    Guest_Mem *guest_buffer;
    bool need_sync;
} Touchscreen_Context;

static Touchscreen_Context static_touchscreen_context;

// 触摸屏的物理大小，可以通过命令行来设置
static Touchscreen_Prop static_prop = {
    .width = 1920,
    .height = 1080,
};

int *express_touchscreen_size = (int *)&static_prop;

// qemu这边的窗口大小
static int window_width = 0;
static int window_height = 0;

static int now_finger_xpos = 0;
static int now_finger_ypos = 0;

static int now_scroll_ypos1 = 0;
static int now_scroll_ypos2 = 0;

static bool left_mouse_press = false;
static bool right_mouse_press = false;
static int mouse_left_finger = -1;
static int mouse_right_finger1 = -1;
static int mouse_right_finger2 = -1;

static int finger_xpos_record[MAX_RECORD_SLOT][MAX_RECORD_NUM];
static int finger_ypos_record[MAX_RECORD_SLOT][MAX_RECORD_NUM];

static bool finger_is_record[MAX_RECORD_SLOT];
static bool finger_is_replay[MAX_RECORD_SLOT];

static int now_record_index = -1;
static int now_record_cnt = 0;

static int now_replay_cnt[MAX_RECORD_SLOT];
static int now_replay_finger_num[MAX_RECORD_SLOT];

static int scroll_yoffset = 0;
static bool is_scrolling = false;
static int scroll_finger1 = -1;
static int scroll_finger2 = -1;

static GHashTable *real_touch_id_map = NULL;
static GHashTable *touch_refresh_save_map = NULL;

inline int get_next_avali_finger(void);

inline void release_finger(int f);

/**
 * @brief Set the touchscreen size object
 *
 * @param width 触摸屏的物理配置宽度（也是虚拟机显示屏的分辨率宽度）
 * @param height 触摸屏的物理配置高度（也是虚拟机显示屏的分辨率高度）
 * @param max_width qemu显示的窗口的宽度
 * @param max_height qemu显示的窗口的高度
 */
void set_touchscreen_window_size(int max_width, int max_height)
{
    window_width = max_width;
    window_height = max_height;
}

void start_mouse_record(int index)
{
    if (now_record_index >= MAX_RECORD_SLOT)
    {
        return;
    }

    finger_is_record[now_record_index] = false;
    now_replay_finger_num[now_record_index] = -1;
    now_record_index = index;
    now_record_cnt = 0;
}

static void record_mouse_pos(void)
{
    if (now_record_index >= 0 && now_record_cnt < MAX_RECORD_NUM)
    {
        if (left_mouse_press)
        {
            finger_xpos_record[now_record_index][now_record_cnt] = now_finger_xpos;
            finger_ypos_record[now_record_index][now_record_cnt] = now_finger_ypos;
            now_record_cnt++;
            finger_is_record[now_record_index] = true;
        }
    }
}

void stop_mouse_record(void)
{
    if (now_record_index >= 0 && now_record_cnt < MAX_RECORD_NUM)
    {
        finger_xpos_record[now_record_index][now_record_cnt] = -1;
        finger_ypos_record[now_record_index][now_record_cnt] = -1;
        now_record_cnt++;
    }
    now_record_index = -1;
}

bool start_mouse_replay(int index)
{
    if (index >= 0 && index < MAX_RECORD_SLOT && finger_is_record[index])
    {
        if (finger_xpos_record[index][0] >= 0)
        {
            // 占用一根指头
            if (now_replay_finger_num[index] == -1)
            {
                // 前两根指头不能用，留给双指操作，因此最多8根指头同时操作
                now_replay_finger_num[index] = get_next_avali_finger();
                if (now_replay_finger_num[index] == -1)
                {
                    return false;
                }
            }
            now_replay_cnt[index] = 0;
            finger_is_replay[index] = true;

            return true;
        }
        return false;
    }
    return false;
}

bool check_mouse_is_replaying(int index)
{
    return finger_is_replay[index];
}

static void keep_mouse_replaying(void)
{
    // 键盘按下后会先触发PRESS，然后过一会再会持续触发REPEAT，加入这个是为了保证在press和repeat间，replay不会间隔过久
    for (int i = 0; i < MAX_RECORD_SLOT; i++)
    {
        if (finger_is_record[i] && finger_is_replay[i])
        {
            int now_cnt = now_replay_cnt[i];
            set_express_touchscreen_input(finger_xpos_record[i][now_cnt], finger_ypos_record[i][now_cnt], 1, now_replay_finger_num[i]);
            if (now_cnt + 1 < MAX_RECORD_NUM && finger_xpos_record[i][now_cnt + 1] >= 0)
            {
                // 让now_cnt对应位置的xpos永远不为-1
                now_replay_cnt[i]++;
            }
        }
    }
}

bool stop_mouse_replay(int index)
{
    if (index >= 0 && index < MAX_RECORD_SLOT && finger_is_record[index] && finger_is_replay[index])
    {
        int now_cnt = now_replay_cnt[index];
        set_express_touchscreen_input(finger_xpos_record[index][now_cnt], finger_ypos_record[index][now_cnt], 0, now_replay_finger_num[index]);
        now_replay_cnt[index] = 0;
        finger_is_replay[index] = false;

        // 释放指头
        release_finger(now_replay_finger_num[index]);
        now_replay_finger_num[index] = -1;
        return true;
    }
    return false;
}

void express_touchscreen_mouse_move_handle(GLFWwindow *window, double xpos, double ypos)
{
    if (is_scrolling)
    {
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos1, 0, scroll_finger1);
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos2, 0, scroll_finger2);

        release_finger(scroll_finger1);
        release_finger(scroll_finger2);

        scroll_finger1 = -1;
        scroll_finger2 = -1;

        is_scrolling = false;
        scroll_yoffset = 0;
    }

    // printf("now mouse pos %lf %lf\n", xpos, ypos);

    int real_display_width = window_width;
    int real_display_height = window_height;
    if (express_gpu_keep_window_scale)
    {
        now_finger_xpos = (int)((double)xpos / window_width * static_prop.width);
        now_finger_ypos = (int)((double)ypos / window_height * static_prop.height);
    }
    else
    {
        if ((double)static_prop.width / static_prop.height > (double)window_width / window_height)
        {
            real_display_height = (double)static_prop.height / static_prop.width * window_width;
            ypos = min(max((ypos - (double)(window_height - real_display_height) / 2), 0), (double)real_display_height);
        }
        else
        {
            real_display_width = (double)static_prop.width / static_prop.height * window_height;
            xpos = min(max((xpos - (double)(window_width - real_display_width) / 2), 0), (double)real_display_width);
        }
    }

    now_finger_xpos = (int)(xpos / real_display_width * static_prop.width);
    now_finger_ypos = (int)(ypos / real_display_height * static_prop.height);

    // printf("now mouse %d %d %d %d %d %d\n", now_finger_xpos, now_finger_ypos, real_display_width, real_display_height, window_width, window_height);

    if (left_mouse_press)
    {
        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, 1, mouse_left_finger);
    }

    if (right_mouse_press)
    {
        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, 1, mouse_right_finger1);
        if (express_touchscreen_right_click_is_two_finger)
        {
            set_express_touchscreen_input(now_finger_xpos + 100, now_finger_ypos, 1, mouse_right_finger2);
        }
    }
}

void express_touchscreen_mouse_click_handle(GLFWwindow *window, int button, int action, int mods)
{
    // printf("mouse click %d %d\n",button, action);
    if (is_scrolling)
    {
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos1, 0, scroll_finger1);
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos2, 0, scroll_finger2);

        release_finger(scroll_finger1);
        release_finger(scroll_finger2);

        scroll_finger1 = -1;
        scroll_finger2 = -1;

        is_scrolling = false;
        scroll_yoffset = 0;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mouse_left_finger = get_next_avali_finger();
            // printf("press left mouse pos %d %d\n", now_finger_xpos, now_finger_ypos);
        }

        left_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, left_mouse_press, mouse_left_finger);

        if (action == GLFW_RELEASE)
        {
            // printf("release left mouse pos %d %d\n", now_finger_xpos, now_finger_ypos);
            release_finger(mouse_left_finger);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            mouse_right_finger1 = get_next_avali_finger();
            if (express_touchscreen_right_click_is_two_finger)
            {
                mouse_right_finger2 = get_next_avali_finger();
            }
        }

        right_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, right_mouse_press, mouse_right_finger1);
        if (express_touchscreen_right_click_is_two_finger)
        {
            set_express_touchscreen_input(now_finger_xpos + 100, now_finger_ypos, right_mouse_press, mouse_right_finger2);
        }

        if (action == GLFW_RELEASE)
        {
            release_finger(mouse_right_finger1);
            if (express_touchscreen_right_click_is_two_finger)
            {
                release_finger(mouse_right_finger2);
            }
        }
    }

    return;
}

void express_touchscreen_mouse_scroll_handle(GLFWwindow *window, double xoffset, double yoffset)
{
    // printf("scroll x %lf y %lf\n", xoffset, yoffset);
    if (express_touchscreen_scroll_is_zoom)
    {
        int temp_yoffset = ((int)yoffset) * express_touchscreen_scroll_ratio;
        int temp_finger_offset = min(now_finger_ypos, static_prop.height - now_finger_ypos) - 100;

        // 加上offset不改变正负号才能加上去（同正同负）
        if (scroll_yoffset * (scroll_yoffset + temp_yoffset) > 0 || scroll_yoffset == 0)
        {
            // 偏移不能超过上下边距
            if (abs(scroll_yoffset + temp_yoffset) <= temp_finger_offset)
            {
                scroll_yoffset += temp_yoffset;
            }
        }
        express_printf("scroll_yoffset %d\n", scroll_yoffset);

        now_scroll_ypos1 = now_finger_ypos + scroll_yoffset;
        now_scroll_ypos2 = now_finger_ypos - scroll_yoffset;

        if (scroll_yoffset < 0)
        {
            now_scroll_ypos1 += temp_finger_offset;
            now_scroll_ypos2 -= temp_finger_offset;
        }

        if (scroll_finger1 == -1)
        {
            scroll_finger1 = get_next_avali_finger();
            scroll_finger2 = get_next_avali_finger();
        }

        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos1, 1, scroll_finger1);
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos2, 1, scroll_finger2);

        is_scrolling = true;
    }
}

static gboolean ghash_auto_release_finger(gpointer key, gpointer value, gpointer user_data)
{
    GHashTable *save_map = (GHashTable *)user_data;
    uint64_t life = (((uint64_t)value) >> 32);
    uint64_t finger = ((((uint64_t)value) << 32) >> 32);
    if (life == 0 || save_map == NULL)
    {
        printf("release key id %lld\n", (long long)(key));
        release_finger(finger);
        return TRUE;
    }
    else
    {
        life--;
        g_hash_table_insert(save_map, key, (gpointer)((life << 32) | (finger)));
        return FALSE;
    }
}

static void ghash_refresh_finger_life(gpointer key, gpointer value, gpointer user_data)
{
    GHashTable *origin_map = (GHashTable *)user_data;
    if (origin_map != NULL)
    {
        g_hash_table_insert(origin_map, key, value);
    }
}

void express_touchscreen_touch_handle(GLFWwindow *window, int touch_id, int action, double xpos, double ypos)
{
    int real_display_width = window_width;
    int real_display_height = window_height;
    if (express_gpu_keep_window_scale)
    {
        now_finger_xpos = (int)((double)xpos / window_width * static_prop.width);
        now_finger_ypos = (int)((double)ypos / window_height * static_prop.height);
    }
    else
    {
        if ((double)static_prop.width / static_prop.height > (double)window_width / window_height)
        {
            real_display_height = (double)static_prop.height / static_prop.width * window_width;
            ypos = min(max((ypos - (double)(window_height - real_display_height) / 2), 0), (double)real_display_height);
        }
        else
        {
            real_display_width = (double)static_prop.width / static_prop.height * static_prop.height;
            xpos = min(max((xpos - (double)(window_width - real_display_width) / 2), 0), (double)real_display_width);
        }
    }

    now_finger_xpos = (int)(xpos / real_display_width * static_prop.width);
    now_finger_ypos = (int)(ypos / real_display_height * static_prop.height);

    if (real_touch_id_map == NULL)
    {
        real_touch_id_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
        touch_refresh_save_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    }

    int64_t search_id = -1;
    int64_t search_finger = -1;
    if (!g_hash_table_lookup_extended(real_touch_id_map, GINT_TO_POINTER(touch_id), (gpointer *)&search_id, (gpointer *)&search_finger))
    {
        // 没有占用一个指头，但是却是释放，则直接返回
        if (action == GLFW_RELEASE)
        {
            return;
        }
        search_finger = (int64_t)get_next_avali_finger();
    }
    if (search_finger != -1)
    {
        // 保持50的生命，大概是50ms
        search_finger &= 0xffffffffL;
        g_hash_table_insert(real_touch_id_map, GINT_TO_POINTER(touch_id), (gpointer)(((search_finger) | (50LL << 32))));

        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, action != GLFW_RELEASE, search_finger);
        if (action == GLFW_RELEASE)
        {
            release_finger(search_finger);
            g_hash_table_remove(real_touch_id_map, GINT_TO_POINTER(touch_id));
        }
    }
}

void express_touchscreen_entered_handle(GLFWwindow *window, int entered)
{
    // 触摸屏时，当所有的指头都出去后，才会触发这个回调
    // 触摸屏实在没办法解决指头出去后收不到release事件的情况，所以干脆搞成定时释放，要是没有触摸事件就直接释放指头
    // 鼠标是只要出去就回调
    if (entered == 0)
    {
        // printf("mouse entered 0 press %d %d\n", left_mouse_press, right_mouse_press);
        // 只有仍在点击的情况下，才需要重置输入事件
        if (left_mouse_press || right_mouse_press)
        {
            reset_touchscreen_input();
        }
    }
}

/**
 * @brief 重置当前所有的触摸屏输入事件
 *
 */
void reset_touchscreen_input(void)
{
    for (int i = 0; i < static_touchscreen_context.data.touch_cnt; i++)
    {
        if (static_touchscreen_context.data.is_touched[i])
        {
            static_touchscreen_context.data.is_touched[i] = 0;
            static_touchscreen_context.need_sync = true;
        }
        static_touchscreen_context.finger_used[i] = 0;
    }

    scroll_finger1 = -1;
    scroll_finger2 = -1;
    mouse_left_finger = -1;
    mouse_right_finger1 = -1;
    mouse_right_finger2 = -1;
    for (int i = 0; i < MAX_RECORD_SLOT; i++)
    {
        now_replay_finger_num[i] = -1;
    }

    left_mouse_press = false;
    right_mouse_press = false;

    if (real_touch_id_map != NULL)
    {
        g_hash_table_remove_all(real_touch_id_map);
    }
}

/**
 * @brief 获取下一个空闲的指头插槽
 *
 * @return int
 */
int get_next_avali_finger(void)
{
    for (int i = 0; i < MAX_TOUCH_POINT; i++)
    {
        if (static_touchscreen_context.finger_used[i] == 0)
        {
            static_touchscreen_context.finger_used[i] = 1;
            return i;
        }
    }
    return -1;
}

/**
 * @brief 释放指头f
 *
 * @param f 表示指头的index
 */
void release_finger(int f)
{
    if (f >= 0 && f < MAX_TOUCH_POINT)
    {
        if (static_touchscreen_context.data.is_touched[f])
        {
            static_touchscreen_context.data.is_touched[f] = 0;
            static_touchscreen_context.need_sync = true;
        }
        static_touchscreen_context.finger_used[f] = 0;
    }
}

void set_express_touchscreen_input(int x, int y, int is_touched, int index)
{
    if (index >= MAX_TOUCH_POINT || index < 0)
    {
        return;
    }

    x = max(min(x, static_prop.width), 1);
    y = max(min(y, static_prop.height), 1);

    if (static_touchscreen_context.data.touch_cnt <= index)
    {
        static_touchscreen_context.data.touch_cnt = index + 1;
    }

    // printf("input x %d y %d\n",x,y);
    static_touchscreen_context.data.touch_x[index] = x;
    static_touchscreen_context.data.touch_y[index] = y;

    if (is_touched || static_touchscreen_context.data.is_touched[index] != is_touched)
    {
        static_touchscreen_context.data.is_touched[index] = is_touched;
        static_touchscreen_context.need_sync = true;
    }
}

void sync_express_touchscreen_input(bool need_send)
{

    // 每次sync阶段才进行record或者replay的操作，这样频率才能对等，也不会因为press和repeat触发间隔大产生啥问题
    record_mouse_pos();
    keep_mouse_replaying();

    if (real_touch_id_map != NULL)
    {
        g_hash_table_foreach_remove(real_touch_id_map, ghash_auto_release_finger, touch_refresh_save_map);
        g_hash_table_foreach(touch_refresh_save_map, ghash_refresh_finger_life, real_touch_id_map);
        g_hash_table_remove_all(touch_refresh_save_map);
    }

    if (!static_touchscreen_context.need_sync)
    {
        return;
    }

    if (!static_touchscreen_context.device_context.irq_enabled)
    {
        printf("express_touchscreen irq is not ok\n");
        static_touchscreen_context.need_sync = false;
        return;
    }

    if (need_send)
    {
        write_to_guest_mem(static_touchscreen_context.guest_buffer, &(static_touchscreen_context.data), 0, sizeof(Touchscreen_Data));
        set_express_device_irq((Device_Context *)&static_touchscreen_context, 0, sizeof(Touchscreen_Data));
    }

    static_touchscreen_context.need_sync = false;
}

static void touchscreen_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (static_touchscreen_context.guest_buffer != NULL)
    {
        free_copied_guest_mem(static_touchscreen_context.guest_buffer);
    }
    printf("touch register buffer\n");
    static_touchscreen_context.guest_buffer = data;
}

static Device_Context *get_touchscreen_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    return (Device_Context *)&static_touchscreen_context;
}

static Express_Device_Info express_touchscreen_info = {
    .enable_default = true,
    .name = "express-touchscreen",
    .option_name = "touchscreen",
    .driver_name = "express_touchscreen",
    .device_id = EXPRESS_TOUCHSCREEN_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_touchscreen_context,
    .buffer_register = touchscreen_buffer_register,

    .static_prop = &(static_prop),
    .static_prop_size = sizeof(Touchscreen_Prop),

};

EXPRESS_DEVICE_INIT(express_touchscreen, &express_touchscreen_info)
