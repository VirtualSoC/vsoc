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

    // qemu这边的窗口大小
    int window_width;
    int window_height;

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

static Touchscreen_Context context = {
    .mouse_left_finger = -1,
    .mouse_right_finger1 = -1,
    .mouse_right_finger2 = -1,
    .current_record_index = -1,
    .scroll_finger1 = -1,
    .scroll_finger2 = -1,
};

// 触摸屏的物理大小，可以通过命令行来设置
static Touchscreen_Prop static_prop = {
    .width = 1920,
    .height = 1080,
};

int *express_touchscreen_size = (int *)&static_prop;

inline int get_next_free_finger(void);

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
    context.window_width = max_width;
    context.window_height = max_height;
}

void start_mouse_record(int index)
{
    if (context.current_record_index >= MAX_RECORD_SLOT)
    {
        return;
    }

    context.finger_is_record[context.current_record_index] = false;
    context.current_replay_finger_num[context.current_record_index] = -1;
    context.current_record_index = index;
    context.current_record_cnt = 0;
}

static void record_mouse_pos(void)
{
    if (context.current_record_index >= 0 && context.current_record_cnt < MAX_RECORD_NUM)
    {
        if (context.left_mouse_press)
        {
            context.finger_xpos_record[context.current_record_index][context.current_record_cnt] = context.current_finger_xpos;
            context.finger_ypos_record[context.current_record_index][context.current_record_cnt] = context.current_finger_ypos;
            context.current_record_cnt++;
            context.finger_is_record[context.current_record_index] = true;
        }
    }
}

void stop_mouse_record(void)
{
    if (context.current_record_index >= 0 && context.current_record_cnt < MAX_RECORD_NUM)
    {
        context.finger_xpos_record[context.current_record_index][context.current_record_cnt] = -1;
        context.finger_ypos_record[context.current_record_index][context.current_record_cnt] = -1;
        context.current_record_cnt++;
    }
    context.current_record_index = -1;
}

bool start_mouse_replay(int index)
{
    if (index >= 0 && index < MAX_RECORD_SLOT && context.finger_is_record[index])
    {
        if (context.finger_xpos_record[index][0] >= 0)
        {
            // 占用一根指头
            if (context.current_replay_finger_num[index] == -1)
            {
                // 前两根指头不能用，留给双指操作，因此最多8根指头同时操作
                context.current_replay_finger_num[index] = get_next_free_finger();
                if (context.current_replay_finger_num[index] == -1)
                {
                    return false;
                }
            }
            context.current_replay_cnt[index] = 0;
            context.finger_is_replay[index] = true;

            return true;
        }
        return false;
    }
    return false;
}

bool check_mouse_is_replaying(int index)
{
    return context.finger_is_replay[index];
}

static void keep_mouse_replaying(void)
{
    // 键盘按下后会先触发PRESS，然后过一会再会持续触发REPEAT，加入这个是为了保证在press和repeat间，replay不会间隔过久
    for (int i = 0; i < MAX_RECORD_SLOT; i++)
    {
        if (context.finger_is_record[i] && context.finger_is_replay[i])
        {
            int now_cnt = context.current_replay_cnt[i];
            set_express_touchscreen_input(context.finger_xpos_record[i][now_cnt], context.finger_ypos_record[i][now_cnt], 1, context.current_replay_finger_num[i]);
            if (now_cnt + 1 < MAX_RECORD_NUM && context.finger_xpos_record[i][now_cnt + 1] >= 0)
            {
                // 让now_cnt对应位置的xpos永远不为-1
                context.current_replay_cnt[i]++;
            }
        }
    }
}

bool stop_mouse_replay(int index)
{
    if (index >= 0 && index < MAX_RECORD_SLOT && context.finger_is_record[index] && context.finger_is_replay[index])
    {
        int now_cnt = context.current_replay_cnt[index];
        set_express_touchscreen_input(context.finger_xpos_record[index][now_cnt], context.finger_ypos_record[index][now_cnt], 0, context.current_replay_finger_num[index]);
        context.current_replay_cnt[index] = 0;
        context.finger_is_replay[index] = false;

        // 释放指头
        release_finger(context.current_replay_finger_num[index]);
        context.current_replay_finger_num[index] = -1;
        return true;
    }
    return false;
}

void express_touchscreen_mouse_move_handle(GLFWwindow *window, double xpos, double ypos)
{
    if (context.is_scrolling)
    {
        set_express_touchscreen_input(context.current_finger_xpos, context.current_scroll_ypos1, 0, context.scroll_finger1);
        set_express_touchscreen_input(context.current_finger_xpos, context.current_scroll_ypos2, 0, context.scroll_finger2);

        release_finger(context.scroll_finger1);
        release_finger(context.scroll_finger2);

        context.scroll_finger1 = -1;
        context.scroll_finger2 = -1;

        context.is_scrolling = false;
        context.scroll_yoffset = 0;
    }

    // printf("now mouse pos %lf %lf\n", xpos, ypos);

    int real_display_width = context.window_width;
    int real_display_height = context.window_height;
    if (express_gpu_keep_window_scale)
    {
        context.current_finger_xpos = (int)((double)xpos / context.window_width * static_prop.width);
        context.current_finger_ypos = (int)((double)ypos / context.window_height * static_prop.height);
    }
    else
    {
        if ((double)static_prop.width / static_prop.height > (double)context.window_width / context.window_height)
        {
            real_display_height = (double)static_prop.height / static_prop.width * context.window_width;
            ypos = min(max((ypos - (double)(context.window_height - real_display_height) / 2), 0), (double)real_display_height);
        }
        else
        {
            real_display_width = (double)static_prop.width / static_prop.height * context.window_height;
            xpos = min(max((xpos - (double)(context.window_width - real_display_width) / 2), 0), (double)real_display_width);
        }
    }

    context.current_finger_xpos = (int)(xpos / real_display_width * static_prop.width);
    context.current_finger_ypos = (int)(ypos / real_display_height * static_prop.height);

    // printf("now mouse %d %d %d %d %d %d\n", context.current_finger_xpos, context.current_finger_ypos, real_display_width, real_display_height, context.window_width, context.window_height);

    if (context.left_mouse_press)
    {
        set_express_touchscreen_input(context.current_finger_xpos, context.current_finger_ypos, 1, context.mouse_left_finger);
    }

    if (context.right_mouse_press)
    {
        set_express_touchscreen_input(context.current_finger_xpos, context.current_finger_ypos, 1, context.mouse_right_finger1);
        if (express_touchscreen_right_click_is_two_finger)
        {
            set_express_touchscreen_input(context.current_finger_xpos + 100, context.current_finger_ypos, 1, context.mouse_right_finger2);
        }
    }
}

void express_touchscreen_mouse_click_handle(GLFWwindow *window, int button, int action, int mods)
{
    // printf("mouse click %d %d\n",button, action);
    if (context.is_scrolling)
    {
        set_express_touchscreen_input(context.current_finger_xpos, context.current_scroll_ypos1, 0, context.scroll_finger1);
        set_express_touchscreen_input(context.current_finger_xpos, context.current_scroll_ypos2, 0, context.scroll_finger2);

        release_finger(context.scroll_finger1);
        release_finger(context.scroll_finger2);

        context.scroll_finger1 = -1;
        context.scroll_finger2 = -1;

        context.is_scrolling = false;
        context.scroll_yoffset = 0;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            context.mouse_left_finger = get_next_free_finger();
            // printf("press left mouse pos %d %d\n", context.current_finger_xpos, context.current_finger_ypos);
        }

        context.left_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(context.current_finger_xpos, context.current_finger_ypos, context.left_mouse_press, context.mouse_left_finger);

        if (action == GLFW_RELEASE)
        {
            // printf("release left mouse pos %d %d\n", context.current_finger_xpos, context.current_finger_ypos);
            release_finger(context.mouse_left_finger);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            context.mouse_right_finger1 = get_next_free_finger();
            if (express_touchscreen_right_click_is_two_finger)
            {
                context.mouse_right_finger2 = get_next_free_finger();
            }
        }

        context.right_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(context.current_finger_xpos, context.current_finger_ypos, context.right_mouse_press, context.mouse_right_finger1);
        if (express_touchscreen_right_click_is_two_finger)
        {
            set_express_touchscreen_input(context.current_finger_xpos + 100, context.current_finger_ypos, context.right_mouse_press, context.mouse_right_finger2);
        }

        if (action == GLFW_RELEASE)
        {
            release_finger(context.mouse_right_finger1);
            if (express_touchscreen_right_click_is_two_finger)
            {
                release_finger(context.mouse_right_finger2);
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
        int temp_finger_offset = min(context.current_finger_ypos, static_prop.height - context.current_finger_ypos) - 100;

        // 加上offset不改变正负号才能加上去（同正同负）
        if (context.scroll_yoffset * (context.scroll_yoffset + temp_yoffset) > 0 || context.scroll_yoffset == 0)
        {
            // 偏移不能超过上下边距
            if (abs(context.scroll_yoffset + temp_yoffset) <= temp_finger_offset)
            {
                context.scroll_yoffset += temp_yoffset;
            }
        }
        express_printf("context.scroll_yoffset %d\n", context.scroll_yoffset);

        context.current_scroll_ypos1 = context.current_finger_ypos + context.scroll_yoffset;
        context.current_scroll_ypos2 = context.current_finger_ypos - context.scroll_yoffset;

        if (context.scroll_yoffset < 0)
        {
            context.current_scroll_ypos1 += temp_finger_offset;
            context.current_scroll_ypos2 -= temp_finger_offset;
        }

        if (context.scroll_finger1 == -1)
        {
            context.scroll_finger1 = get_next_free_finger();
            context.scroll_finger2 = get_next_free_finger();
        }

        set_express_touchscreen_input(context.current_finger_xpos, context.current_scroll_ypos1, 1, context.scroll_finger1);
        set_express_touchscreen_input(context.current_finger_xpos, context.current_scroll_ypos2, 1, context.scroll_finger2);

        context.is_scrolling = true;
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
    int real_display_width = context.window_width;
    int real_display_height = context.window_height;
    if (express_gpu_keep_window_scale)
    {
        context.current_finger_xpos = (int)((double)xpos / context.window_width * static_prop.width);
        context.current_finger_ypos = (int)((double)ypos / context.window_height * static_prop.height);
    }
    else
    {
        if ((double)static_prop.width / static_prop.height > (double)context.window_width / context.window_height)
        {
            real_display_height = (double)static_prop.height / static_prop.width * context.window_width;
            ypos = min(max((ypos - (double)(context.window_height - real_display_height) / 2), 0), (double)real_display_height);
        }
        else
        {
            real_display_width = (double)static_prop.width / static_prop.height * static_prop.height;
            xpos = min(max((xpos - (double)(context.window_width - real_display_width) / 2), 0), (double)real_display_width);
        }
    }

    context.current_finger_xpos = (int)(xpos / real_display_width * static_prop.width);
    context.current_finger_ypos = (int)(ypos / real_display_height * static_prop.height);

    if (context.real_touch_id_map == NULL)
    {
        context.real_touch_id_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
        context.touch_refresh_save_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    }

    int64_t search_id = -1;
    int64_t search_finger = -1;
    if (!g_hash_table_lookup_extended(context.real_touch_id_map, GINT_TO_POINTER(touch_id), (gpointer *)&search_id, (gpointer *)&search_finger))
    {
        // 没有占用一个指头，但是却是释放，则直接返回
        if (action == GLFW_RELEASE)
        {
            return;
        }
        search_finger = (int64_t)get_next_free_finger();
    }
    if (search_finger != -1)
    {
        // 保持50的生命，大概是50ms
        search_finger &= 0xffffffffL;
        g_hash_table_insert(context.real_touch_id_map, GINT_TO_POINTER(touch_id), (gpointer)(((search_finger) | (50LL << 32))));

        set_express_touchscreen_input(context.current_finger_xpos, context.current_finger_ypos, action != GLFW_RELEASE, search_finger);
        if (action == GLFW_RELEASE)
        {
            release_finger(search_finger);
            g_hash_table_remove(context.real_touch_id_map, GINT_TO_POINTER(touch_id));
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
        // printf("mouse entered 0 press %d %d\n", context.left_mouse_press, context.right_mouse_press);
        // 只有仍在点击的情况下，才需要重置输入事件
        if (context.left_mouse_press || context.right_mouse_press)
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
    for (int i = 0; i < context.data.touch_cnt; i++)
    {
        if (context.data.is_touched[i])
        {
            context.data.is_touched[i] = 0;
            context.need_sync = true;
        }
        context.finger_used[i] = 0;
    }

    context.scroll_finger1 = -1;
    context.scroll_finger2 = -1;
    context.mouse_left_finger = -1;
    context.mouse_right_finger1 = -1;
    context.mouse_right_finger2 = -1;
    for (int i = 0; i < MAX_RECORD_SLOT; i++)
    {
        context.current_replay_finger_num[i] = -1;
    }

    context.left_mouse_press = false;
    context.right_mouse_press = false;

    if (context.real_touch_id_map != NULL)
    {
        g_hash_table_remove_all(context.real_touch_id_map);
    }
}

/**
 * @brief 获取下一个空闲的指头插槽
 *
 * @return int
 */
int get_next_free_finger(void)
{
    for (int i = 0; i < MAX_TOUCH_POINT; i++)
    {
        if (context.finger_used[i] == 0)
        {
            context.finger_used[i] = 1;
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
        if (context.data.is_touched[f])
        {
            context.data.is_touched[f] = 0;
            context.need_sync = true;
        }
        context.finger_used[f] = 0;
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

    if (context.data.touch_cnt <= index)
    {
        context.data.touch_cnt = index + 1;
    }

    // printf("input x %d y %d\n",x,y);
    context.data.touch_x[index] = x;
    context.data.touch_y[index] = y;

    if (is_touched || context.data.is_touched[index] != is_touched)
    {
        context.data.is_touched[index] = is_touched;
        context.need_sync = true;
    }
}

void sync_express_touchscreen_input(bool need_send)
{

    // 每次sync阶段才进行record或者replay的操作，这样频率才能对等，也不会因为press和repeat触发间隔大产生啥问题
    record_mouse_pos();
    keep_mouse_replaying();

    if (context.real_touch_id_map != NULL)
    {
        g_hash_table_foreach_remove(context.real_touch_id_map, ghash_auto_release_finger, context.touch_refresh_save_map);
        g_hash_table_foreach(context.touch_refresh_save_map, ghash_refresh_finger_life, context.real_touch_id_map);
        g_hash_table_remove_all(context.touch_refresh_save_map);
    }

    if (!context.need_sync)
    {
        return;
    }

    if (!context.device_context.irq_enabled)
    {
        printf("express_touchscreen irq is not ok\n");
        context.need_sync = false;
        return;
    }

    if (need_send)
    {
        write_to_guest_mem(context.guest_buffer, &(context.data), 0, sizeof(Touchscreen_Data));
        set_express_device_irq((Device_Context *)&context, 0, sizeof(Touchscreen_Data));
    }

    context.need_sync = false;
}

static void touchscreen_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    if (context.guest_buffer != NULL)
    {
        free_copied_guest_mem(context.guest_buffer);
    }
    printf("touch register buffer\n");
    context.guest_buffer = data;
}

static Device_Context *get_touchscreen_device_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    return (Device_Context *)&context;
}

static Express_Device_Info express_touchscreen_info = {
    .enable_default = true,
    .name = "express-touchscreen",
    .option_name = "touchscreen",
    .driver_name = "express_touchscreen",
    .device_id = EXPRESS_TOUCHSCREEN_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .get_device_context = get_touchscreen_device_context,
    .buffer_register = touchscreen_buffer_register,

    .static_prop = &(static_prop),
    .static_prop_size = sizeof(Touchscreen_Prop),

};

EXPRESS_DEVICE_INIT(express_touchscreen, &express_touchscreen_info)
