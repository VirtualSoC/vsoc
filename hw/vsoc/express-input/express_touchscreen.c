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
#include "hw/teleport-express/express_log.h"
#include "hw/express-input/express_touchscreen.h"
#include "hw/teleport-express/express_platform.h"
#include "hw/teleport-express/teleport_express_register.h"
#include "hw/express-gpu/express_gpu_snapshot.h"

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

// 触摸屏的物理大小，可以通过命令行来设置
Touchscreen_Prop touchscreen_prop = {
    .count = 1,
    .width = 1920,
    .height = 1080,
};

int express_touchscreen_scroll_ratio = 10;
bool express_touchscreen_scroll_is_zoom = false;
bool express_touchscreen_right_click_is_two_finger = false;

GHashTable *g_touchscreen_contexts = NULL;

static inline int get_next_free_finger(Touchscreen_Context *context);
static inline void release_finger(Touchscreen_Context *context, int f);
static void reset_touchscreen_input(Touchscreen_Context *context);
static void set_express_touchscreen_input(Touchscreen_Context *context, int x, int y, int is_touched, int index);

static inline Touchscreen_Context *get_touchscreen_context(GLFWwindow *window)
{
    Display_Context *disp = (Display_Context *)glfwGetWindowUserPointer(window);
    Touchscreen_Context *context = (Touchscreen_Context *)g_hash_table_lookup(g_touchscreen_contexts, GUINT_TO_POINTER(disp->unique_id));
    if (context == NULL) {
        LOGE("error! no touchscreen context with id %" PRIu64 " found!", disp->unique_id);
    }
    return context;
}

/**
 * @brief Set the touchscreen size object
 *
 * @param width 触摸屏的物理配置宽度（也是虚拟机显示屏的分辨率宽度）
 * @param height 触摸屏的物理配置高度（也是虚拟机显示屏的分辨率高度）
 * @param max_width qemu显示的窗口的宽度
 * @param max_height qemu显示的窗口的高度
 */
void set_touchscreen_window_size(GLFWwindow *window, int max_width, int max_height, int rotation)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    context->window_width = max_width;
    context->window_height = max_height;

    bool prev_rotated = context->window_rotation == ROTATE_90 || context->window_rotation == ROTATE_270;
    bool current_rotated = rotation == ROTATE_90 || rotation == ROTATE_270;

    if (prev_rotated != current_rotated) {
        swap(context->touchscreen_width, context->touchscreen_height, int);
    }
    context->window_rotation = rotation;
}

void start_mouse_record(GLFWwindow *window, int index)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    if (context->current_record_index >= MAX_RECORD_SLOT)
    {
        return;
    }

    context->finger_is_record[context->current_record_index] = false;
    context->current_replay_finger_num[context->current_record_index] = -1;
    context->current_record_index = index;
    context->current_record_cnt = 0;
}

static void record_mouse_pos(Touchscreen_Context *context)
{
    if (context->current_record_index >= 0 && context->current_record_cnt < MAX_RECORD_NUM)
    {
        if (context->left_mouse_press)
        {
            context->finger_xpos_record[context->current_record_index][context->current_record_cnt] = context->current_finger_xpos;
            context->finger_ypos_record[context->current_record_index][context->current_record_cnt] = context->current_finger_ypos;
            context->current_record_cnt++;
            context->finger_is_record[context->current_record_index] = true;
        }
    }
}

void stop_mouse_record(GLFWwindow *window)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    if (context->current_record_index >= 0 && context->current_record_cnt < MAX_RECORD_NUM)
    {
        context->finger_xpos_record[context->current_record_index][context->current_record_cnt] = -1;
        context->finger_ypos_record[context->current_record_index][context->current_record_cnt] = -1;
        context->current_record_cnt++;
    }
    context->current_record_index = -1;
}

bool start_mouse_replay(GLFWwindow *window, int index)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    if (index >= 0 && index < MAX_RECORD_SLOT && context->finger_is_record[index])
    {
        if (context->finger_xpos_record[index][0] >= 0)
        {
            // 占用一根指头
            if (context->current_replay_finger_num[index] == -1)
            {
                // 前两根指头不能用，留给双指操作，因此最多8根指头同时操作
                context->current_replay_finger_num[index] = get_next_free_finger(context);
                if (context->current_replay_finger_num[index] == -1)
                {
                    return false;
                }
            }
            context->current_replay_cnt[index] = 0;
            context->finger_is_replay[index] = true;

            return true;
        }
        return false;
    }
    return false;
}

bool check_mouse_is_replaying(GLFWwindow *window, int index)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    return context->finger_is_replay[index];
}

static void keep_mouse_replaying(Touchscreen_Context *context)
{
    // 键盘按下后会先触发PRESS，然后过一会再会持续触发REPEAT，加入这个是为了保证在press和repeat间，replay不会间隔过久
    for (int i = 0; i < MAX_RECORD_SLOT; i++)
    {
        if (context->finger_is_record[i] && context->finger_is_replay[i])
        {
            int now_cnt = context->current_replay_cnt[i];
            set_express_touchscreen_input(context, context->finger_xpos_record[i][now_cnt], context->finger_ypos_record[i][now_cnt], 1, context->current_replay_finger_num[i]);
            if (now_cnt + 1 < MAX_RECORD_NUM && context->finger_xpos_record[i][now_cnt + 1] >= 0)
            {
                // 让now_cnt对应位置的xpos永远不为-1
                context->current_replay_cnt[i]++;
            }
        }
    }
}

bool stop_mouse_replay(GLFWwindow *window, int index)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    if (index >= 0 && index < MAX_RECORD_SLOT && context->finger_is_record[index] && context->finger_is_replay[index])
    {
        int now_cnt = context->current_replay_cnt[index];
        set_express_touchscreen_input(context, context->finger_xpos_record[index][now_cnt], context->finger_ypos_record[index][now_cnt], 0, context->current_replay_finger_num[index]);
        context->current_replay_cnt[index] = 0;
        context->finger_is_replay[index] = false;

        // 释放指头
        release_finger(context, context->current_replay_finger_num[index]);
        context->current_replay_finger_num[index] = -1;
        return true;
    }
    return false;
}

void express_touchscreen_mouse_move_handle(GLFWwindow *window, double xpos, double ypos)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    if (context->is_scrolling)
    {
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_scroll_ypos1, 0, context->scroll_finger1);
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_scroll_ypos2, 0, context->scroll_finger2);

        release_finger(context, context->scroll_finger1);
        release_finger(context, context->scroll_finger2);

        context->scroll_finger1 = -1;
        context->scroll_finger2 = -1;

        context->is_scrolling = false;
        context->scroll_yoffset = 0;
    }

    // printf("now mouse pos %lf %lf\n", xpos, ypos);

    int real_display_width = context->window_width;
    int real_display_height = context->window_height;
    if (express_gpu_keep_window_scale)
    {
        context->current_finger_xpos = (int)((double)xpos / context->window_width * context->touchscreen_width);
        context->current_finger_ypos = (int)((double)ypos / context->window_height * context->touchscreen_height);
    }
    else
    {
        if ((double)context->touchscreen_width / context->touchscreen_height > (double)context->window_width / context->window_height)
        {
            real_display_height = (double)context->touchscreen_height / context->touchscreen_width * context->window_width;
            ypos = min(max((ypos - (double)(context->window_height - real_display_height) / 2), 0), (double)real_display_height);
        }
        else
        {
            real_display_width = (double)context->touchscreen_width / context->touchscreen_height * context->window_height;
            xpos = min(max((xpos - (double)(context->window_width - real_display_width) / 2), 0), (double)real_display_width);
        }
    }

    context->current_finger_xpos = (int)(xpos / real_display_width * context->touchscreen_width);
    context->current_finger_ypos = (int)(ypos / real_display_height * context->touchscreen_height);

    // printf("now mouse %d %d %d %d %d %d\n", context->current_finger_xpos, context->current_finger_ypos, real_display_width, real_display_height, context->window_width, context->window_height);

    if (context->left_mouse_press)
    {
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_finger_ypos, 1, context->mouse_left_finger);
    }

    if (context->right_mouse_press)
    {
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_finger_ypos, 1, context->mouse_right_finger1);
        if (express_touchscreen_right_click_is_two_finger)
        {
            set_express_touchscreen_input(context, context->current_finger_xpos + 100, context->current_finger_ypos, 1, context->mouse_right_finger2);
        }
    }
}

void express_touchscreen_mouse_click_handle(GLFWwindow *window, int button, int action, int mods)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    LOGD("touchscreen id %d mouse click %d %d", context->id, button, action);
    if (context->is_scrolling)
    {
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_scroll_ypos1, 0, context->scroll_finger1);
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_scroll_ypos2, 0, context->scroll_finger2);

        release_finger(context, context->scroll_finger1);
        release_finger(context, context->scroll_finger2);

        context->scroll_finger1 = -1;
        context->scroll_finger2 = -1;

        context->is_scrolling = false;
        context->scroll_yoffset = 0;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            context->mouse_left_finger = get_next_free_finger(context);
            LOGD("press left mouse pos %d %d", context->current_finger_xpos, context->current_finger_ypos);
        }

        context->left_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_finger_ypos, context->left_mouse_press, context->mouse_left_finger);

        if (action == GLFW_RELEASE)
        {
            LOGD("release left mouse pos %d %d", context->current_finger_xpos, context->current_finger_ypos);
            release_finger(context, context->mouse_left_finger);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            context->mouse_right_finger1 = get_next_free_finger(context);
            if (express_touchscreen_right_click_is_two_finger)
            {
                context->mouse_right_finger2 = get_next_free_finger(context);
            }
        }

        context->right_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_finger_ypos, context->right_mouse_press, context->mouse_right_finger1);
        if (express_touchscreen_right_click_is_two_finger)
        {
            set_express_touchscreen_input(context, context->current_finger_xpos + 100, context->current_finger_ypos, context->right_mouse_press, context->mouse_right_finger2);
        }

        if (action == GLFW_RELEASE)
        {
            release_finger(context, context->mouse_right_finger1);
            if (express_touchscreen_right_click_is_two_finger)
            {
                release_finger(context, context->mouse_right_finger2);
            }
        }
    }

    return;
}

void express_touchscreen_mouse_scroll_handle(GLFWwindow *window, double xoffset, double yoffset)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    LOGD("touchscreen id %d scroll x %lf y %lf", context->id, xoffset, yoffset);
    if (express_touchscreen_scroll_is_zoom)
    {
        int temp_yoffset = ((int)yoffset) * express_touchscreen_scroll_ratio;
        int temp_finger_offset = min(context->current_finger_ypos, context->touchscreen_height - context->current_finger_ypos) - 100;

        // 加上offset不改变正负号才能加上去（同正同负）
        if (context->scroll_yoffset * (context->scroll_yoffset + temp_yoffset) > 0 || context->scroll_yoffset == 0)
        {
            // 偏移不能超过上下边距
            if (abs(context->scroll_yoffset + temp_yoffset) <= temp_finger_offset)
            {
                context->scroll_yoffset += temp_yoffset;
            }
        }
        express_printf("context->scroll_yoffset %d\n", context->scroll_yoffset);

        context->current_scroll_ypos1 = context->current_finger_ypos + context->scroll_yoffset;
        context->current_scroll_ypos2 = context->current_finger_ypos - context->scroll_yoffset;

        if (context->scroll_yoffset < 0)
        {
            context->current_scroll_ypos1 += temp_finger_offset;
            context->current_scroll_ypos2 -= temp_finger_offset;
        }

        if (context->scroll_finger1 == -1)
        {
            context->scroll_finger1 = get_next_free_finger(context);
            context->scroll_finger2 = get_next_free_finger(context);
        }

        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_scroll_ypos1, 1, context->scroll_finger1);
        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_scroll_ypos2, 1, context->scroll_finger2);

        context->is_scrolling = true;
    }
}

static gboolean ghash_auto_release_finger(gpointer key, gpointer value, gpointer user_data)
{
    Touchscreen_Context *context = (Touchscreen_Context *)user_data;
    GHashTable *save_map = context->touch_refresh_save_map;
    uint64_t life = (((uint64_t)value) >> 32);
    uint64_t finger = ((((uint64_t)value) << 32) >> 32);
    if (life == 0 || save_map == NULL)
    {
        printf("release key id %lld\n", (long long)(key));
        release_finger(context, finger);
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
    Touchscreen_Context *context = get_touchscreen_context(window);

    int real_display_width = context->window_width;
    int real_display_height = context->window_height;
    if (express_gpu_keep_window_scale)
    {
        context->current_finger_xpos = (int)((double)xpos / context->window_width * context->touchscreen_width);
        context->current_finger_ypos = (int)((double)ypos / context->window_height * context->touchscreen_height);
    }
    else
    {
        if ((double)context->touchscreen_width / context->touchscreen_height > (double)context->window_width / context->window_height)
        {
            real_display_height = (double)context->touchscreen_height / context->touchscreen_width * context->window_width;
            ypos = min(max((ypos - (double)(context->window_height - real_display_height) / 2), 0), (double)real_display_height);
        }
        else
        {
            real_display_width = (double)context->touchscreen_width / context->touchscreen_height * context->touchscreen_height;
            xpos = min(max((xpos - (double)(context->window_width - real_display_width) / 2), 0), (double)real_display_width);
        }
    }

    context->current_finger_xpos = (int)(xpos / real_display_width * context->touchscreen_width);
    context->current_finger_ypos = (int)(ypos / real_display_height * context->touchscreen_height);

    if (context->real_touch_id_map == NULL)
    {
        context->real_touch_id_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
        context->touch_refresh_save_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    }

    int64_t search_id = -1;
    int64_t search_finger = -1;
    if (!g_hash_table_lookup_extended(context->real_touch_id_map, GINT_TO_POINTER(touch_id), (gpointer *)&search_id, (gpointer *)&search_finger))
    {
        // 没有占用一个指头，但是却是释放，则直接返回
        if (action == GLFW_RELEASE)
        {
            return;
        }
        search_finger = (int64_t)get_next_free_finger(context);
    }
    if (search_finger != -1)
    {
        // 保持50的生命，大概是50ms
        search_finger &= 0xffffffffL;
        g_hash_table_insert(context->real_touch_id_map, GINT_TO_POINTER(touch_id), (gpointer)(((search_finger) | (50LL << 32))));

        set_express_touchscreen_input(context, context->current_finger_xpos, context->current_finger_ypos, action != GLFW_RELEASE, search_finger);
        if (action == GLFW_RELEASE)
        {
            release_finger(context, search_finger);
            g_hash_table_remove(context->real_touch_id_map, GINT_TO_POINTER(touch_id));
        }
    }
}

void express_touchscreen_entered_handle(GLFWwindow *window, int entered)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    // 触摸屏时，当所有的指头都出去后，才会触发这个回调
    // 触摸屏实在没办法解决指头出去后收不到release事件的情况，所以干脆搞成定时释放，要是没有触摸事件就直接释放指头
    // 鼠标是只要出去就回调
    if (entered == 0)
    {
        LOGD("touchscreen id %d mouse entered 0 press %d %d", context->id, context->left_mouse_press, context->right_mouse_press);
        // 只有仍在点击的情况下，才需要重置输入事件
        if (context->left_mouse_press || context->right_mouse_press)
        {
            reset_touchscreen_input(context);
        }
    }
}

/**
 * @brief 重置当前所有的触摸屏输入事件
 *
 */
static void reset_touchscreen_input(Touchscreen_Context *context)
{
    for (int i = 0; i < context->data.touch_cnt; i++)
    {
        if (context->data.is_touched[i])
        {
            context->data.is_touched[i] = 0;
            context->need_sync = true;
        }
        context->finger_used[i] = 0;
    }

    context->scroll_finger1 = -1;
    context->scroll_finger2 = -1;
    context->mouse_left_finger = -1;
    context->mouse_right_finger1 = -1;
    context->mouse_right_finger2 = -1;
    for (int i = 0; i < MAX_RECORD_SLOT; i++)
    {
        context->current_replay_finger_num[i] = -1;
    }

    context->left_mouse_press = false;
    context->right_mouse_press = false;

    if (context->real_touch_id_map != NULL)
    {
        g_hash_table_remove_all(context->real_touch_id_map);
    }
}

/**
 * @brief 获取下一个空闲的指头插槽
 *
 * @return int
 */
static int get_next_free_finger(Touchscreen_Context *context)
{
    for (int i = 0; i < MAX_TOUCH_POINT; i++)
    {
        if (context->finger_used[i] == 0)
        {
            context->finger_used[i] = 1;
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
static void release_finger(Touchscreen_Context *context, int f)
{
    if (f >= 0 && f < MAX_TOUCH_POINT)
    {
        if (context->data.is_touched[f])
        {
            context->data.is_touched[f] = 0;
            context->need_sync = true;
        }
        context->finger_used[f] = 0;
    }
}

static void set_express_touchscreen_input(Touchscreen_Context *context, int x, int y, int is_touched, int index)
{
    if (index >= MAX_TOUCH_POINT || index < 0)
    {
        return;
    }

    if (context->window_rotation == ROTATE_90) {
        x = context->touchscreen_width - x;
        swap(x, y, int);
    } else if (context->window_rotation == ROTATE_270) {
        swap(x, y, int);
    }
    x = max(min(x, touchscreen_prop.width), 1);
    y = max(min(y, touchscreen_prop.height), 1);

    LOGD("touchscreen id %d wh %d %d rotate %d touch xy %d %d transformed %d %d", context->id, context->touchscreen_width, context->touchscreen_height, context->window_rotation, context->current_finger_xpos, context->current_finger_ypos, x, y);

    if (context->data.touch_cnt <= index)
    {
        context->data.touch_cnt = index + 1;
    }

    // printf("input x %d y %d\n",x,y);
    context->data.touch_x[index] = x;
    context->data.touch_y[index] = y;

    if (is_touched || context->data.is_touched[index] != is_touched)
    {
        context->data.is_touched[index] = is_touched;
        context->need_sync = true;
    }
}

void sync_express_touchscreen_input(GLFWwindow *window, bool need_send)
{
    Touchscreen_Context *context = get_touchscreen_context(window);

    // 每次sync阶段才进行record或者replay的操作，这样频率才能对等，也不会因为press和repeat触发间隔大产生啥问题
    record_mouse_pos(context);
    keep_mouse_replaying(context);

    if (context->real_touch_id_map != NULL)
    {
        g_hash_table_foreach_remove(context->real_touch_id_map, ghash_auto_release_finger, context);
        g_hash_table_foreach(context->touch_refresh_save_map, ghash_refresh_finger_life, context->real_touch_id_map);
        g_hash_table_remove_all(context->touch_refresh_save_map);
    }

    if (!context->need_sync)
    {
        return;
    }

    if (!context->device_context.irq_enabled)
    {
        LOGD("touchscreen id %d express_touchscreen irq is not enabled", context->id);
        context->need_sync = false;
        return;
    }

    if (need_send)
    {
        LOGD("touchscreen id %d sending touchscreen data to guest", context->id);
        write_to_guest_mem(context->guest_buffer, &(context->data), 0, sizeof(Touchscreen_Data));
        set_express_device_irq((Device_Context *)context, 0, sizeof(Touchscreen_Data));
    }

    context->need_sync = false;
}

static void touchscreen_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id)
{
    Touchscreen_Context *context = (Touchscreen_Context *)g_hash_table_lookup(g_touchscreen_contexts, GUINT_TO_POINTER(unique_id));

    if (context->guest_buffer != NULL) {
        free_duplicated_guest_mem(context->guest_buffer);
    }

    LOGI("touch dev %" PRIu64 " register buffer", unique_id);
    context->guest_buffer = data;
}

static void remove_touchscreen_device_context(Device_Context *context) {
    g_free(context);
}

static Device_Context *get_touchscreen_device_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (g_touchscreen_contexts == NULL) {
        g_touchscreen_contexts = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)remove_touchscreen_device_context);
    }

    Touchscreen_Context *context = (Touchscreen_Context *)g_hash_table_lookup(g_touchscreen_contexts, GUINT_TO_POINTER(unique_id));

    if (context == NULL) {
        context = g_malloc0(sizeof(Touchscreen_Context));
        context->id = unique_id;
        context->touchscreen_width = touchscreen_prop.width;
        context->touchscreen_height = touchscreen_prop.height;

        context->mouse_left_finger = -1,
        context->mouse_right_finger1 = -1,
        context->mouse_right_finger2 = -1,
        context->current_record_index = -1,
        context->scroll_finger1 = -1,
        context->scroll_finger2 = -1,

        g_hash_table_insert(g_touchscreen_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }

    return (Device_Context *)context;
}

void save_touchscreen_context(QEMUFile* f) {
    // save total touchscreen count
    int context_count = (int)g_hash_table_size(g_touchscreen_contexts);
    qemu_put_be32(f, context_count);

    // iter through all touchscreen contexts and save them
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, g_touchscreen_contexts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Touchscreen_Context *context = (Touchscreen_Context *)value;
        qemu_put_buffer(f, (uint8_t *)context, sizeof(Touchscreen_Context));
        save_guest_mem(f, context->guest_buffer);
        if (context->device_context.irq_call == NULL) {
            qemu_put_be32(f, 0);
        } else {
            qemu_put_be32(f, 1);
            save_teleport_express_call(f, context->device_context.irq_call);
        }
    }

}

void load_touchscreen_context(QEMUFile *f){
    // clear all old touchscreen contexts
    if (g_touchscreen_contexts != NULL) {
        g_hash_table_remove_all(g_touchscreen_contexts);
    }

    // get total touchscreen count
    int context_count = qemu_get_be32(f);

    // load all touchscreen contexts
    for (int i = 0; i < context_count; i++) {
        Touchscreen_Context *context = g_malloc0(sizeof(Touchscreen_Context));
        qemu_get_buffer(f, (uint8_t *)context, sizeof(Touchscreen_Context));

        // these resources needs to be re-created
        context->guest_buffer = load_guest_mem(f, 0);

        int has_call = qemu_get_be32(f);
        if (has_call) {
            context->device_context.irq_call = load_teleport_express_call(f);
        }
        g_hash_table_insert(g_touchscreen_contexts, GUINT_TO_POINTER(context->id), (gpointer)context);
    }
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

    .static_prop = &touchscreen_prop,
    .static_prop_size = sizeof(Touchscreen_Prop),
};

EXPRESS_DEVICE_INIT(express_touchscreen, &express_touchscreen_info)
