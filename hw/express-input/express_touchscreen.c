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
    int touch_x[10];
    int touch_y[10];
    int is_touched[10];
    int touch_cnt;
} __attribute__((packed, aligned(4))) Touchscreen_Data;

typedef struct Touchscreen_Context
{
    Touchscreen_Data data;
    Guest_Mem *guest_buffer;
    Teleport_Express_Call *irq_call;
    bool need_sync;
} Touchscreen_Context;

static Touchscreen_Context static_touchscreen_context;


//触摸屏的物理大小，可以通过命令行来设置
static Touchscreen_Prop static_prop = {
    .width = 1920,
    .height = 1080,
};

int *express_touchscreen_size = (int *)&static_prop;


// qemu这边的窗口大小
static int window_width = 0;
static int window_height = 0;

static bool now_finger_used[10];

static int now_finger_xpos = 0;
static int now_finger_ypos = 0;

static int now_scroll_ypos1 = 0;
static int now_scroll_ypos2 = 0;

static bool left_mouse_press = false;
static bool right_mouse_press = false;

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

static bool touchscreen_irq_enable = false;

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
    // display_width = width;
    // display_height = height;
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
            //占用一根指头
            if (now_replay_finger_num[index] == 0)
            {
                //前两根指头不能用，留给双指操作，因此最多8根指头同时操作
                int use_n = 2;
                while (qatomic_cmpxchg(&(now_finger_used[use_n]), false, true) && use_n < MAX_TOUCH_POINT)
                    use_n++;
                if (use_n >= MAX_TOUCH_POINT)
                {
                    return false;
                }
                now_replay_finger_num[index] = use_n;
            }
            now_replay_cnt[index] = 0;
            finger_is_replay[index] = true;

            // set_express_touchscreen_input(finger_xpos_record[index][0], finger_ypos_record[index][0], 1, now_replay_finger_num[index]);
            // if (finger_xpos_record[index][1] >= 0)
            // {
            //     //让now_cnt对应位置的xpos永远不为-1
            //     now_replay_cnt[index] = 1;
            // }
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
    //键盘按下后会先触发PRESS，然后过一会再会持续触发REPEAT，加入这个是为了保证在press和repeat间，replay不会间隔过久
    for (int i = 0; i < MAX_RECORD_SLOT; i++)
    {
        if (finger_is_record[i] && finger_is_replay[i])
        {
            int now_cnt = now_replay_cnt[i];
            set_express_touchscreen_input(finger_xpos_record[i][now_cnt], finger_ypos_record[i][now_cnt], 1, now_replay_finger_num[i]);
            if (now_cnt + 1 < MAX_RECORD_NUM && finger_xpos_record[i][now_cnt + 1] >= 0)
            {
                //让now_cnt对应位置的xpos永远不为-1
                now_replay_cnt[i]++;
            }
        }
    }
}

bool stop_mouse_replay(int index)
{
    if (index >= 0 && index < MAX_RECORD_SLOT && now_replay_finger_num[index] != 0 && finger_is_record[index] && finger_is_replay[index])
    {
        int now_cnt = now_replay_cnt[index];
        set_express_touchscreen_input(finger_xpos_record[index][now_cnt], finger_ypos_record[index][now_cnt], 0, now_replay_finger_num[index]);
        now_replay_cnt[index] = 0;
        finger_is_replay[index] = false;

        //释放指头
        qatomic_set(&(now_finger_used[now_replay_finger_num[index]]), false);
        now_replay_finger_num[index] = 0;
        return true;
    }
    return false;
}

void express_touchscreen_mouse_move_handle(GLFWwindow *window, double xpos, double ypos)
{
    if (is_scrolling)
    {
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos1, 0, 0);
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos2, 0, 1);
        is_scrolling = false;
        scroll_yoffset = 0;
    }

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

    // printf("now finger %d %d %d %d %d %d\n", now_finger_xpos, now_finger_ypos,real_display_width,real_display_height,window_width, window_height );

    if (left_mouse_press || right_mouse_press)
    {
        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, 1, 0);
        if (express_touchscreen_right_click_is_two_finger && right_mouse_press)
        {
            set_express_touchscreen_input(now_finger_xpos + 100, now_finger_ypos, 1, 1);
        }
    }
}

void express_touchscreen_mouse_click_handle(GLFWwindow *window, int button, int action, int mods)
{
    // printf("mouse click %d %d\n",button, action);
    if (is_scrolling)
    {
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos1, 0, 0);
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos2, 0, 1);
        is_scrolling = false;
        scroll_yoffset = 0;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        left_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, left_mouse_press, 0);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        right_mouse_press = (action != GLFW_RELEASE);
        set_express_touchscreen_input(now_finger_xpos, now_finger_ypos, right_mouse_press, 0);
        if (express_touchscreen_right_click_is_two_finger)
        {
            set_express_touchscreen_input(now_finger_xpos + 100, now_finger_ypos, right_mouse_press, 1);
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
        printf("scroll_yoffset %d\n", scroll_yoffset);

        now_scroll_ypos1 = now_finger_ypos + scroll_yoffset;
        now_scroll_ypos2 = now_finger_ypos - scroll_yoffset;

        if (scroll_yoffset < 0)
        {
            now_scroll_ypos1 += temp_finger_offset;
            now_scroll_ypos2 -= temp_finger_offset;
        }

        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos1, 1, 0);
        set_express_touchscreen_input(now_finger_xpos, now_scroll_ypos2, 1, 1);

        is_scrolling = true;
    }
}

void set_express_touchscreen_input(int x, int y, int is_touched, int index)
{
    if (index >= MAX_TOUCH_POINT)
    {
        return;
    }
    if (x < 0 || x >= static_prop.width || y < 0 || y >= static_prop.height)
    {
        return;
    }

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

void sync_express_touchscreen_input(void)
{

    //每次sync阶段才进行record或者replay的操作，这样频率才能对等，也不会因为press和repeat触发间隔大产生啥问题
    record_mouse_pos();
    keep_mouse_replaying();

    if (!static_touchscreen_context.need_sync || !touchscreen_irq_enable)
    {
        return;
    }
    if (static_touchscreen_context.irq_call == NULL)
    {
        printf("irq not ok!\n");
        return;
    }

    write_to_guest_mem(static_touchscreen_context.guest_buffer, &(static_touchscreen_context.data), 0, sizeof(Touchscreen_Data));

    static_touchscreen_context.need_sync = false;

    // printf("touchscreen irq send ok\n");
    send_express_device_irq(static_touchscreen_context.irq_call, 0, sizeof(Touchscreen_Data));
    static_touchscreen_context.irq_call = NULL;
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

static void touchscreen_irq_register(Teleport_Express_Call *call)
{
    // printf("touch register irq\n");

    if (static_touchscreen_context.irq_call != NULL)
    {
        send_express_device_irq(static_touchscreen_context.irq_call, 0, 0);
    }

    touchscreen_irq_enable = true;
    static_touchscreen_context.irq_call = call;
}

static void touchscreen_irq_release(void)
{
    if (static_touchscreen_context.irq_call != NULL)
    {
        send_express_device_irq(static_touchscreen_context.irq_call, 0, 0);

        printf("touchscreen_irq_release\n");
        touchscreen_irq_enable = false;
        static_touchscreen_context.irq_call = NULL;
    }
}

static Express_Device_Info express_touchscreen_info = {
    .enable_default = true,
    .name = "express-touchscreen",
    .option_name = "touchscreen",
    .driver_name = "express_touchscreen",
    .device_id = EXPRESS_TOUCHSCREEN_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE,

    .buffer_register = touchscreen_buffer_register,
    .irq_register = touchscreen_irq_register,
    .irq_release = touchscreen_irq_release,

    .static_prop = &(static_prop),
    .static_prop_size = sizeof(Touchscreen_Prop),

};

EXPRESS_DEVICE_INIT(express_touchscreen, &express_touchscreen_info)
