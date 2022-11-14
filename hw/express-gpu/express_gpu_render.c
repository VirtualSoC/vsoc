/**
 * @file express_gpu_render.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 使用host的GPU进行渲染，并且构建双缓冲，包含了双缓冲的相关函数
 * @version 0.1
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020
 *
 */
// #define STD_DEBUG_LOG
// #define TIMER_LOG
#include "qemu/osdep.h"
#include "qemu/atomic.h"

#include "hw/express-gpu/express_gpu_render.h"

#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/egl_context.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/glv1.h"
#include "hw/express-gpu/gl_helper.h"

#include "ui/console.h"
#include "ui/input.h"
#include "sysemu/runstate.h"

#include "hw/express-gpu/sdl_control.h"

#include "hw/express-input/express_touchscreen.h"
#include "hw/express-input/express_keyboard.h"

GAsyncQueue *main_window_event_queue = NULL;
int main_window_event_queue_lock = 0;

Static_Context_Values *preload_static_context_value = NULL;

int sdl2_no_need = 0;

int host_opengl_version = 0;

#ifdef ENABLE_DSA
int DSA_enable = 1;
#else
int DSA_enable = 0;
#endif
int VSYNC_enable = 0;

int composer_refresh_HZ = 60;

int express_gpu_window_FPS = 60;

bool express_gpu_keep_window_scale = false;

// static unsigned int main_frame_num = 0;

// static int event_queue_lock;
// static GQueue *sync_event_queue;

static GHashTable *gbuffer_global_map = NULL;
static GHashTable *gbuffer_global_types = NULL;

static volatile int gbuffer_global_map_lock = 0;

static volatile int gbuffer_global_types_lock = 0;

static int calc_screen_hz = 0;

static int now_screen_hz = 0;

static gint64 last_calc_time = 0;

// #ifdef ENABLE_STATIC_WINDOW_REFRESH
// static gint64 frame_start_time = 0;
// static gint64 remain_sleep_time = 0;
// #else
// static int has_painted = 0;
// static int size_has_change = 0;
// #endif

// static gint64 gen_frame_time_avg_1s = 0;

#define EVENT_QUEUE_LOCK                                    \
    while (qatomic_cmpxchg(&(event_queue_lock), 0, 1) == 1) \
        ;

#define EVENT_QUEUE_UNLOCK qatomic_cmpxchg(&(event_queue_lock), 1, 0);

static GLFWwindow *glfw_window = NULL;

void *dummy_window_for_sync = NULL;

static GLuint programID = 0;
static GLuint drawVAO = 0;

static GLint reverse_loc = 0;
static GLuint is_reverse = 0;

static bool window_is_shown = false;

static int window_width = 0;
static int window_height = 0;

int express_gpu_window_width = 0;
int express_gpu_window_height = 0;

static int display_width = 0;
static int display_height = 0;

static bool window_need_refresh = false;
;

// static long real_window_width = 0;
// static long real_window_height = 0;

// static double mouse_pos_record[100][100];
// static int mouse_pos_record_num[100];
// static int key_repeat_cnt[100];
// static int mouse_click_record[100];
// static int key_is_repeat[100];
// static int now_press_key;
// static double now_mouse_xpos;
// static double now_mouse_ypos;

// static bool is_replaying;
// static int replaying_key;

// static bool is_click;

static Graphic_Buffer *display_gbuffer;

volatile int native_render_run = 0;

static QemuConsole *input_receive_con = NULL;

static const char GPU_VENDOR[] = "ARM";
static const char GPU_VERSION[] = "OpenGL ES 3.1 (";
static const char GPU_RENDERER[] = "Mali-G77";
static const char GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.10";

// google devide info
//  static const GLubyte GPU_VENDOR[] = "Google (";
//  static const GLubyte GPU_VERSION[] = "OpenGL ES 3.0 (";
//  static const GLubyte GPU_RENDERER[] = "Android Emulator OpenGL ES Translator (";
//  static const GLubyte GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.00";

static const int OPENGL_MAJOR_VERSION = 3;
static const int OPENGL_MINOR_VERSION = 1;

static const char *SPECIAL_EXTENSIONS[] =
    {
        /*1*/ "GL_OES_EGL_image",
        /*2*/ "GL_OES_EGL_image_external",
        /*3*/ "GL_OES_EGL_sync",
        /*4*/ "GL_OES_depth24",
        /*5*/ "GL_OES_depth32",
        /*6*/ "GL_OES_texture_float",
        /*15*/ "GL_OES_texture_float_linear",
        /*7*/ "GL_OES_texture_half_float",
        /*8*/ "GL_OES_texture_half_float_linear",
        /*9*/ "GL_OES_compressed_ETC1_RGB8_texture",
        /*10*/ "GL_OES_depth_texture",
        /*11*/ "GL_OES_EGL_image_external_essl3",
        /*12*/ "GL_KHR_texture_compression_astc_ldr",
        /*13*/ "GL_KHR_texture_compression_astc_hdr",
        /*14*/ "GL_OES_vertex_array_object",
        // /*14*/ "GL_EXT_shader_framebuffer_fetch",   //这个暂时看情况支持，webview用它来混合，会着色器中使用变量gl_LastFragData
        // /*15*/ "GL_EXT_multisampled_render_to_texture",  //这个暂时不能有，因为它需要支持相关函数 这个具体涉及到glFramebufferTexture2DMultisampleEXT函数，这个函数是tile-based GPU使用TBDR渲染特有的，用来节省带宽，IMR模式的PC上没有，所以没法启用。glFramebufferTexture2DMultisampleEXT这个函数被PUBG在抗锯齿时用到了
        /*16*/ "GL_EXT_color_buffer_float",
        /*17*/ "GL_EXT_color_buffer_half_float",
        /*18*/ "GL_OES_element_index_uint",
        /*19*/ "GL_OES_texture_float_linear",
        /*20*/ "GL_OES_compressed_paletted_texture",
        /*21*/ "GL_OES_packed_depth_stencil",
        /*22*/ "GL_OES_texture_npot",
        /*23*/ "GL_OES_rgb8_rgba8",
        /*24*/ "GL_OES_framebuffer_object",
        /*25*/ "GL_ARB_texture_non_power_of_two",
        /*26*/ "GL_OES_blend_func_separate",
        /*27*/ "GL_OES_blend_equation_separate",
        /*28*/ "GL_OES_blend_subtract",
        /*29*/ "GL_OES_byte_coordinates",
        /*30*/ "GL_OES_point_size_array",
        /*31*/ "GL_OES_point_sprite",
        /*32*/ "GL_OES_single_precision",
        /*33*/ "GL_OES_stencil_wrap",
        /*34*/ "GL_OES_texture_env_crossbar",
        /*35*/ "GL_OES_texture_mirrored_repeat",
        /*36*/ "GL_OES_texture_cube_map",
        /*37*/ "GL_OES_draw_texture",
        /*38*/ "GL_OES_fbo_render_mipmap",
        /*39*/ "GL_OES_stencil8",
        /*41*/ "GL_EXT_blend_minmax",
        /*42*/ "GL_OES_standard_derivatives",
        /*43*/ "GL_EXT_robustness",
        /*44*/ "GL_EXT_copy_image",
        /*45*/ "GL_EXT_texture_buffer",
        /*46*/ "GL_OES_vertex_half_float",

        // /*40*/ "GL_EXT_texture_format_BGRA8888", pc很可能是不支持的
};
static const int SPECIAL_EXTENSIONS_SIZE = 46 - 1;

//支持这些扩展需要添加一些函数，所以暂时先不支持——因为有些扩展会被全平台的skia识别而使用，但是这些函数实际为空所以会发生错误
// static const GLubyte *NOT_SUPPORT_EXTENSIONS[] =
//     {
//         //gl
//         /* 1*/ "GL_NV_texture_barrier",          // and gles
//         /* 2*/ "GL_KHR_blend_equation_advanced", // and gles
//         /* 3*/ "GL_NV_blend_equation_advanced",  // and gles
//         /* 4*/ "GL_ARB_clear_texture",
//         /* 5*/ "GL_ARB_draw_indirect",
//         /* 6*/ "GL_ARB_timer_query",
//         /* 7*/ "GL_EXT_timer_query",
//         /* 8*/ "GL_ARB_multi_draw_indirect",
//         /* 9*/ "GL_NV_path_rendering",            // and gles
//         /*10*/ "GL_NV_framebuffer_mixed_samples", // and gles
//         /*11*/ "GL_EXT_debug_marker",             //and gles
//         /*12*/ "GL_ARB_invalidate_subdata",
//         /*13*/ "GL_KHR_debug",             // and gles
//         /*14*/ "GL_EXT_window_rectangles", // and gles

//         //gles
//         /*15*/ "GL_EXT_blend_func_extended",
//         /*16*/ "GL_EXT_clear_texture",
//         /*17*/ "GL_EXT_multi_draw_indirect",
//         /*18*/ "GL_OES_texture_buffer",
//         /*19*/ "GL_EXT_texture_buffer",
//         /*20*/ "GL_CHROMIUM_map_sub",
//         /*21*/ "GL_CHROMIUM_path_rendering",
//         /*22*/ "GL_CHROMIUM_framebuffer_mixed_samples",
//         /*23*/ "GL_CHROMIUM_bind_uniform_location"};
// static const int NOT_SUPPORT_EXTENSION_SIZE = 23;

static void opengl_paint(Graphic_Buffer *gbuffer);
static void *native_window_create(int independ_mode);

// static void g_queue_event_notify(gpointer data, gpointer user_data);

static Notifier shutdown_notifier;

static Dying_List *dying_gbuffer;

static gint64 last_click_time = 0;

void remove_gbuffer_from_global_map(uint64_t gbuffer_id);
void window_size_change_callback(GLFWwindow *window, int width, int height);

static void close_window_callback(GLFWwindow *window)
{
    gint64 now_time = g_get_real_time();

    // printf("shutdown time %lld\n",now_time);
    glfwSetWindowShouldClose(window, GLFW_FALSE);
    if (now_time - last_click_time < 500000)
    {
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_HOST_UI);
    }
    else
    {
        qemu_system_powerdown_request();
    }
    last_click_time = now_time;
}

static void shutdown_notify_callback(Notifier *notifier, void *data)
{
    printf("notify shutdown! %lld\n", g_get_real_time());

    ATOMIC_UNLOCK(main_window_event_queue_lock);
    // ATOMIC_UNLOCK(compose_surface_lock);
    // set_compose_surface(NULL, NULL);
    display_gbuffer = NULL;
    teleport_express_should_stop = true;

    // glfwTerminate();
    if (native_render_run == 2)
    {
        native_render_run = -1;
        int wait_cnt = 0;
        while (native_render_run == -1 && wait_cnt < 200)
        {
            // printf("wait thread close \n");
            g_usleep(5000);
            wait_cnt++;
        }
        if (native_render_run == -1)
        {
            printf("wait time too long!\n");
        }

        // printf("wait thread close done %d\n",native_render_run);
    }
}

// static void keyboard_handle_callback(GLFWwindow *window, int key, int code, int action, int mods)
// {
//     int qcode;
//     bool down = false;

//     if (code > qemu_input_map_glfw_to_qcode_len)
//     {
//         return;
//     }
//     qcode = qemu_input_map_glfw_to_qcode[key];

//     if (action == GLFW_RELEASE)
//     {
//         down = false;
//     }
//     else
//     {
//         down = true;
//     }

//     // if ((mods & GLFW_MOD_ALT) != 0 && (action == GLFW_PRESS || action == GLFW_REPEAT) && key < 100)
//     // {
//     //     if (key_is_repeat[key] == 0)
//     //     {
//     //         // printf("press key %d\n",key);
//     //         now_press_key = key;
//     //         key_is_repeat[key] = 1;
//     //         mouse_click_record[key] = 0;
//     //         mouse_pos_record_num[key] = 0;
//     //         key_repeat_cnt[key] = 0;
//     //     }
//     // }

//     // if (action == GLFW_RELEASE && key < 100)
//     // {
//     //     key_is_repeat[key] = 0;
//     //     now_press_key = 0;
//     // }

//     // if (mouse_click_record[key] == 1 && key_is_repeat[key] == 0)
//     // {
//     //     // printf("replay %d\n",key);

//     //     if (action == GLFW_PRESS)
//     //     {
//     //         // qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)(mouse_pos_record[key][0] / real_window_width * window_width), 0, window_width);
//     //         // qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)(mouse_pos_record[key][1] / real_window_height * window_height), 0, window_height);
//     //         // qemu_input_queue_btn(input_receive_con, INPUT_BUTTON_LEFT, true);
//     //         // key_repeat_cnt[key] = 1;
//     //         // printf("replay click pos1 %lf %lf\n",mouse_pos_record[key][0],mouse_pos_record[key][1]);
//     //         if (is_click)
//     //         {
//     //             qemu_input_queue_btn(input_receive_con, INPUT_BUTTON_LEFT, false);
//     //             is_click = false;
//     //         }
//     //         if (is_replaying)
//     //         {
//     //             return;
//     //         }
//     //         is_replaying = true;
//     //         replaying_key = key;
//     //         key_repeat_cnt[key] = 0;
//     //     }
//     //     else if (action == GLFW_REPEAT)
//     //     {
//     //         // printf("replay hold\n");
//     //     }
//     //     else
//     //     {
//     //         is_replaying = false;
//     //         // printf("replay release\n");
//     //         qemu_input_queue_btn(input_receive_con, INPUT_BUTTON_LEFT, false);
//     //     }
//     // }
//     // else
//     // {
//     qemu_input_event_send_key_qcode(input_receive_con, (QKeyCode)qcode, down);
//     // }

//     // qemu_input_event_sync();

//     // printf("key:%d, code:%d, action:%d, mods:%d,scancode %d,qcode %d\n", key, code, action, mods, glfwGetKeyScancode(key),qcode);
// }

// static bool is_left_click = 0;

// static void mouse_move_handle_callback(GLFWwindow *window, double xpos, double ypos)
// {
//     now_mouse_xpos = xpos;
//     now_mouse_ypos = ypos;

//     // printf("now mouse %lf %lf\n", xpos,ypos);

//     if (is_replaying)
//     {
//         return;
//     }

// #ifdef ENSURE_SAME_WIDTH_HEIGHT_RATIO
//     set_express_touchscreen_input((int)(xpos / real_window_width * window_width), (int)(ypos / real_window_height * window_height), is_left_click, 0);

//     // qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)(xpos / real_window_width * window_width), 0, window_width);
//     // qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)(ypos / real_window_height * window_height), 0, window_height);
// #else
//     if (real_window_height > window_height)
//     {
//         ypos -= (real_window_height - window_height) / 2;
//     }

//     if (real_window_width > window_width)
//     {
//         xpos -= (real_window_width - window_width) / 2;
//     }
//     if ((int)ypos > window_height || (int)xpos > window_width || (int)ypos < 0 || (int)xpos < 0)
//     {
//         return;
//     }

//     qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)xpos, 0, window_width);
//     qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)ypos, 0, window_height);
// // qemu_input_event_sync();
// #endif
// }

// static void mouse_click_handle_callback(GLFWwindow *window, int button, int action, int mods)
// {
//     InputButton btn;
//     // printf("mouse click %d %d\n",button, action);
//     if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//     {
//         if (now_press_key != 0)
//         {
//             // printf("record pos1 %lf %lf\n",now_mouse_xpos,now_mouse_ypos);
//             mouse_click_record[now_press_key] = 1;
//         }
//     }

//     if (button == GLFW_MOUSE_BUTTON_LEFT)
//     {
//         btn = INPUT_BUTTON_LEFT;
//         if (action != GLFW_RELEASE)
//         {
//             is_left_click = 1;
//         }
//         else
//         {
//             is_left_click = 0;
//         }
//         set_express_touchscreen_input((int)(now_mouse_xpos / real_window_width * window_width), (int)(now_mouse_ypos / real_window_height * window_height), is_left_click, 0);
//     }
//     else if (button == GLFW_MOUSE_BUTTON_RIGHT)
//     {
//         btn = INPUT_BUTTON_RIGHT;
//     }
//     else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
//     {
//         btn = INPUT_BUTTON_MIDDLE;
//     }
//     else
//     {
//         return;
//     }

//     bool press = true;
//     if (action == GLFW_RELEASE)
//     {
//         press = false;
//     }

//     if (is_click == press || is_replaying)
//     {
//         return;
//     }
//     is_click = press;

//     if(false)
//     qemu_input_queue_btn(input_receive_con, btn, press);
//     // qemu_input_event_sync();
// }

// static void mouse_scroll_handle_callback(GLFWwindow *window, double xoffset, double yoffset)
// {
//     InputButton btn;
//     if (yoffset > 0)
//     {
//         btn = INPUT_BUTTON_WHEEL_UP;
//     }
//     else if (yoffset < 0)
//     {
//         btn = INPUT_BUTTON_WHEEL_DOWN;
//     }
//     else
//     {
//         return;
//     }

//     qemu_input_queue_btn(input_receive_con, btn, true);
//     qemu_input_event_sync();
//     qemu_input_queue_btn(input_receive_con, btn, false);
//     qemu_input_event_sync();
// }

void window_size_change_callback(GLFWwindow *window, int width, int height)
{
    window_need_refresh = true;

    //需要保证画面比例不变
    if (window_width != width || window_height != height)
    {

        window_width = width;
        window_height = height;

        int temp_window_width = window_width;
        int temp_window_height = window_height;
        int x = 0;
        int y = 0;

        if ((double)display_width / display_height > (double)window_width / window_height)
        {
            temp_window_height = (int)((double)display_height / display_width * window_width);
            y = (window_height - temp_window_height) / 2;
        }
        else
        {
            temp_window_width = (int)((double)display_width / display_height * window_height);
            x = (window_width - temp_window_width) / 2;
        }

        if (express_gpu_keep_window_scale)
        {
            window_width = temp_window_width;
            window_height = temp_window_height;

            glViewport(0, 0, window_width, window_height);

            glfwSetWindowSize(window, window_width, window_height);
        }
        else
        {
            glViewport(x, y, temp_window_width, temp_window_height);
        }

        set_touchscreen_size(display_width, display_height, window_width, window_height);

        // #ifdef ENSURE_SAME_WIDTH_HEIGHT_RATIO
        //         if (calc_width < width && calc_height > height)
        //         {
        //             real_window_width = calc_width;
        //             real_window_height = height;
        //         }
        //         else if (calc_width > width && calc_height < height)
        //         {
        //             real_window_width = width;
        //             real_window_height = calc_height;
        //         }
        //         else
        //         {
        //             //其他情况认为是精度计算问题，直接用新的值
        //             real_window_width = width;
        //             real_window_height = height;
        //         }
        //         glViewport(0, 0, real_window_width, real_window_height);

        //         glfwSetWindowSize(window, real_window_width, real_window_height);

        // #else
        //         int x = 0, y = 0;

        //         if (calc_width < width && calc_height > height)
        //         {
        //             window_width = calc_width;
        //             window_height = height;
        //             x = (width - calc_width) / 2;
        //         }
        //         else if (calc_width > width && calc_height < height)
        //         {
        //             window_width = width;
        //             window_height = calc_height;
        //             y = (height - calc_height) / 2;
        //         }
        //         else
        //         {
        //             //其他情况认为是精度计算问题，直接用新的值
        //             window_width = width;
        //             window_height = height;
        //         }
        //         real_window_width = width;
        //         real_window_height = height;
        //         glViewport(x, y, window_width, window_height);
        // #endif
    }

    // #ifndef ENABLE_STATIC_WINDOW_REFRESH
    //     size_has_change = 1;
    // #endif

    return;
}

static int try_destroy_gbuffer(void *data)
{
    Graphic_Buffer *gbuffer = (Graphic_Buffer *)data;

    if (gbuffer == NULL)
    {
        return 1;
    }

    if (gbuffer->is_dying == 0)
    {
        return 1;
    }

    if (gbuffer->remain_life_time > 0)
    {
        gbuffer->remain_life_time--;
        return 0;
    }

    if (display_gbuffer == gbuffer)
    {
        gbuffer->remain_life_time = MAX_COMPOSER_LIFE_TIME;
        // display_gbuffer = NULL;
        return 0;
    }

    if (gbuffer->gbuffer_id != 0)
    {
        // psurface的gbuffer_id为0
        remove_gbuffer_from_global_map(gbuffer->gbuffer_id);
    }

    if (gbuffer->usage_type == GBUFFER_TYPE_BITMAP)
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_BITMAP_NEED_DATA);
    }
    else if (gbuffer->usage_type == GBUFFER_TYPE_FBO)
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_FBO_NEED_DATA);
    }
    else
    {
        set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_NONE);
    }
    destroy_gbuffer(gbuffer);
    // printf("gbuffer %llx is dead\n",gbuffer->gbuffer_id);

    return 1;
}

static void handle_child_window_event(void)
{
    ATOMIC_LOCK(main_window_event_queue_lock);
    Main_window_Event *child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
    ATOMIC_UNLOCK(main_window_event_queue_lock);

    while (child_event != NULL)
    {
        switch (child_event->event_code)
        {
        case MAIN_PAINT:
        {
            // #ifdef ENABLE_STATIC_WINDOW_REFRESH
            // #else
            Graphic_Buffer *gbuffer = (Graphic_Buffer *)child_event->data;
            display_gbuffer = gbuffer;
            window_need_refresh = true;

            // opengl_paint(gbuffer);
            // has_painted = 1;
            // glfwSwapBuffers(glfw_window);
            // #endif
        }
        break;
        case MAIN_CREATE_CHILD_WINDOW:

            // context只能是由父线程创建，以进行资源共享
            {
                void **window_ptr = (void **)child_event->data;
                if (window_ptr == NULL)
                {
                    break;
                }
                // printf("create window\n");
                // printf("start create window ptr %llx\n", window_ptr);
                int independ_mode = 0;
                if (*window_ptr != NULL)
                {
                    independ_mode = 1;
                }

                *window_ptr = (void *)native_window_create(independ_mode);
                // printf("create window time %lld window %llx\n", g_get_real_time() - t, *window_ptr);
            }

            break;

        case MAIN_DESTROY_GBUFFER:
        {
            Graphic_Buffer *gbuffer = (Graphic_Buffer *)child_event->data;
            if (gbuffer->gbuffer_id == 0)
            {
                destroy_gbuffer(gbuffer);
            }
            else
            {
                // printf("real destroy gbuffer %llx ptr %llx\n", gbuffer->gbuffer_id, gbuffer);
                dying_gbuffer = dying_list_append(dying_gbuffer, gbuffer);
            }
        }
        break;
        case MAIN_CANCEL_GBUFFER:
        {
            Graphic_Buffer *gbuffer = (Graphic_Buffer *)child_event->data;
            if (gbuffer != NULL)
            {
                // printf("real cancel gbuffer delete %llx ptr %llx\n", gbuffer->gbuffer_id, gbuffer);
                dying_gbuffer = dying_list_remove(dying_gbuffer, gbuffer);
            }
        }
        break;

        case MAIN_DESTROY_ALL_EGLSYNC:
        {
            Resource_Map_Status *status = (Resource_Map_Status *)child_event->data;
            if (status == NULL || status->max_id == 0)
            {
                break;
            }
            for (int i = 1; i <= status->max_id; i++)
            {
                if (status->resource_id_map[i] != 0)
                {
                    glDeleteSync((GLsync)status->resource_id_map[i]);
                }
            }
            if (status->resource_id_map != NULL)
            {
                g_free(status->resource_id_map);
            }
            if (status->resource_is_init != NULL)
            {
                g_free(status->resource_is_init);
            }
            if (status->gbuffer_ptr_map != NULL)
            {
                g_free(status->gbuffer_ptr_map);
            }
            g_free(status);
        }
        break;
        case MAIN_DESTROY_ONE_SYNC:
        {
            GLsync sync = (GLsync)child_event->data;
            if (sync == NULL)
            {
                break;
            }

            glDeleteSync(sync);
        }
        break;
        default:
            // express_printf("child win msg: %d\n", uMsg);
            break;
        }
        g_free(child_event);

        ATOMIC_LOCK(main_window_event_queue_lock);
        child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
        ATOMIC_UNLOCK(main_window_event_queue_lock);
    }

    return;
}

static void static_value_prepare(void)
{

    preload_static_context_value = g_malloc0(sizeof(Static_Context_Values) + 512 * 100 + 400);

    preload_static_context_value->composer_HZ = composer_refresh_HZ;
    preload_static_context_value->composer_pid = 0;

    // initialize static status
    preload_static_context_value->major_version = OPENGL_MAJOR_VERSION;
    preload_static_context_value->minor_version = OPENGL_MINOR_VERSION;

    prepare_integer_value(preload_static_context_value);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        printf("error when creating static vaules %x\n", error);
    }
    //下面三个值之所以要限定范围，是因为guest端有个固定大小的数组，这个最大值是数组的最大大小
    if (preload_static_context_value->max_vertex_attribs > 32)
    {
        preload_static_context_value->max_vertex_attribs = 32;
    }

    if (preload_static_context_value->max_image_units > 16)
    {
        preload_static_context_value->max_image_units = 16;
    }

    if (preload_static_context_value->max_vertex_attrib_bindings > 32)
    {
        preload_static_context_value->max_vertex_attrib_bindings = 32;
    }

    preload_static_context_value->num_compressed_texture_formats += 1;
    if (preload_static_context_value->num_compressed_texture_formats > 128)
    {
        preload_static_context_value->compressed_texture_formats[127] = GL_ETC1_RGB8_OES;
        preload_static_context_value->num_compressed_texture_formats = 128;
    }
    else
    {
        preload_static_context_value->compressed_texture_formats[preload_static_context_value->num_compressed_texture_formats - 1] = GL_ETC1_RGB8_OES;
    }

    // for(int i = 0; i<preload_static_context_value->num_compressed_texture_formats && i<128;i++)
    // {
    //     printf("support compress texture%d %x \n",i, preload_static_context_value->compressed_texture_formats[i]);
    // }
    // printf("binary formats num %d eg %d shader formats num %d eg %d\n",preload_static_context_value->num_program_binary_formats,
    //     preload_static_context_value->program_binary_formats[0],
    //     preload_static_context_value->num_shader_binary_formats,
    //     preload_static_context_value->shader_binary_formats[0]);

    //@todo 增加换硬件后暂时移除binary的功能
    // preload_static_context_value->num_program_binary_formats =0;
    // preload_static_context_value->num_shader_binary_formats = 0;

    if (preload_static_context_value->num_program_binary_formats > 8)
    {
        preload_static_context_value->num_program_binary_formats = 8;
    }
    if (preload_static_context_value->num_shader_binary_formats > 8)
    {
        preload_static_context_value->num_shader_binary_formats = 8;
    }
    if (preload_static_context_value->num_compressed_texture_formats > 128)
    {
        preload_static_context_value->num_compressed_texture_formats = 128;
    }

    char *string_loc = ((char *)preload_static_context_value) + sizeof(Static_Context_Values);

    char *temp_loc = string_loc;

    const char *gl_string;
    gl_string = (const char *)glGetString(GL_VENDOR);
    preload_static_context_value->vendor = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_VENDOR, sizeof(GPU_VENDOR) - 1);
    temp_loc += sizeof(GPU_VENDOR) - 1;
    // memcpy(temp_loc, gl_string, strlen(gl_string));
    // temp_loc += strlen(gl_string);
    // *temp_loc = ')';
    // temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("\ngl vendor:%s\n", (char *)gl_string);

    gl_string = (const char *)glGetString(GL_VERSION);

    if (gl_string != NULL && gl_string[0] == '4')
    {
        int major_version = gl_string[0] - '0';
        int minor_version = gl_string[2] - '0';
        if (major_version <= 4 && major_version >= 1 && minor_version >= 1 && minor_version <= 9)
        {
            host_opengl_version = major_version * 10 + minor_version;
        }
        // host_opengl_version = 0;
    }

    preload_static_context_value->version = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_VERSION, sizeof(GPU_VERSION) - 1);
    temp_loc += sizeof(GPU_VERSION) - 1;
    memcpy(temp_loc, gl_string, strlen(gl_string));
    temp_loc += strlen(gl_string);
    *temp_loc = ')';
    temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("gl version:%s\n", string_loc + (unsigned long)(preload_static_context_value->version));

    gl_string = (const char *)glGetString(GL_RENDERER);
    preload_static_context_value->renderer = (unsigned long long)(temp_loc - string_loc);

    memcpy(temp_loc, GPU_RENDERER, sizeof(GPU_RENDERER) - 1);
    temp_loc += sizeof(GPU_RENDERER) - 1;
    // memcpy(temp_loc, gl_string, strlen(gl_string));
    // temp_loc += strlen(gl_string);
    // *temp_loc = ')';
    // temp_loc++;
    *temp_loc = 0;
    temp_loc++;
    printf("gl renderer:%s\n", (char *)gl_string);

    preload_static_context_value->shading_language_version = (unsigned long long)(temp_loc - string_loc);
    memcpy(temp_loc, GPU_SHADER_LANGUAGE_VERSION, sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1);
    temp_loc += sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1;
    *temp_loc = 0;
    temp_loc++;
    printf("gl shading_language_version:%s\n", string_loc + (unsigned long)(preload_static_context_value->shading_language_version));

    char *extensions_start = temp_loc;

    // int no_need_extensions_cnt = 0;
    int num_extensions = preload_static_context_value->num_extensions;

    // num_extensions = 0;
    //目前暂时只设定固定的扩展支持

    int start_loc = 0;
    int has_dsa = 0;
    for (int i = start_loc; i < start_loc + num_extensions; i++)
    {

        gl_string = (const char *)glGetStringi(GL_EXTENSIONS, i);

        if (express_gpu_gl_debug_enable)
        {
            printf("host extension %d %s\n", i, gl_string);
        }

        if (strstr(gl_string, "GL_EXT_direct_state_access") != NULL)
        {
            has_dsa = 1;
        }

        // if(i >= 512 - SPECIAL_EXTENSIONS_SIZE + no_need_extensions_cnt)
        // {
        //     continue;
        // }

        // int no_need_flag = 0;
        // for (int j = 0; j < NOT_SUPPORT_EXTENSION_SIZE; j++)
        // {
        //     if (strstr(gl_string, NOT_SUPPORT_EXTENSIONS[j]) != NULL)
        //     {
        //         no_need_flag = 1;
        //         break;
        //     }
        // }
        // if (no_need_flag == 1)
        // {
        //     no_need_extensions_cnt += 1;
        //     continue;
        // }

        // preload_static_context_value->extensions[i - start_loc - no_need_extensions_cnt] = (unsigned long long)(temp_loc - string_loc);

        // memcpy(temp_loc, gl_string, strlen(gl_string));
        // temp_loc += strlen(gl_string);
        // *temp_loc = 0;
        // printf("%d %s\n", i, temp_loc - strlen(gl_string));
        // temp_loc++;
    }

    if (has_dsa == 0)
    {
        DSA_enable = 0;
    }

    printf("host gl %d DSA_enable %d\n", host_opengl_version, DSA_enable);

    // num_extensions -= no_need_extensions_cnt;

    for (int i = 0; i < SPECIAL_EXTENSIONS_SIZE; i++)
    {
        preload_static_context_value->extensions[i] = temp_loc - string_loc;

        memcpy(temp_loc, SPECIAL_EXTENSIONS[i], strlen(SPECIAL_EXTENSIONS[i]));
        temp_loc += strlen(SPECIAL_EXTENSIONS[i]);
        *temp_loc = 0;
        temp_loc++;
    }

    num_extensions = SPECIAL_EXTENSIONS_SIZE;

    preload_static_context_value->num_extensions = num_extensions;

    int extensions_len = temp_loc - extensions_start;

    memcpy(temp_loc, extensions_start, extensions_len);
    for (int i = 0; i < extensions_len; i++)
    {
        if (*temp_loc == 0)
        {
            *temp_loc = ' ';
        }
        temp_loc++;
    }
    temp_loc--;
    if (*temp_loc == ' ')
    {
        *temp_loc = 0;
    }

    preload_static_context_value->extensions_gles2 = (unsigned long long)(extensions_start - string_loc + extensions_len);
    if (express_gpu_gl_debug_enable)
    {
        printf("extensions len %d num %d: %s|\n", extensions_len, num_extensions, string_loc + (unsigned long)(preload_static_context_value->extensions_gles2));
    }
    assert(temp_loc < ((char *)preload_static_context_value) + sizeof(Static_Context_Values) + 512 * 100 + 400);
}

/**
 * @brief 界面上用于画出图像的函数，实际逻辑为取出d_buffer中的display_texture，然后画出来
 *
 * @param d_buffer
 */
static void opengl_paint(Graphic_Buffer *gbuffer)
{

    if (gbuffer != NULL)
    {
        // printf("opengl_paint gbuffer %llx texture %d\n", gbuffer->gbuffer_id, gbuffer->data_texture);
        gbuffer->remain_life_time = MAX_COMPOSER_LIFE_TIME;

        if (display_width != gbuffer->width || display_height == gbuffer->height)
        {
            display_width = gbuffer->width;
            display_height = gbuffer->height;
            // real_window_width = window_width;
            // real_window_height = window_height;
            // glViewport(0, 0, window_width, window_height);
        }
        // if (is_reverse == 0)
        // {
        //     is_reverse = 1;
        //     glUniform1i(reverse_loc, 1);
        // }
        // printf("paint texture %d\n",gbuffer->data_texture);

        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (gbuffer->data_sync != 0)
        {
            // glClientWaitSync(gbuffer->data_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
            glWaitSync(gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);
            if (gbuffer->delete_sync != 0)
            {
                glDeleteSync(gbuffer->delete_sync);
            }
            gbuffer->delete_sync = gbuffer->data_sync;
            gbuffer->data_sync = NULL;
        }

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
}

static void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // 忽略一些不是错误的id
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    printf("main debug message(%u):%s\n", id, message);
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        printf("Source: API ");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        printf("Source: Window System ");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        printf("Source: Shader Compiler ");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        printf("Source: Third Party ");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        printf("Source: APPLICATION ");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("Type: Error ");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("Type: Deprecated Behaviour ");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("Type: Undefined Behaviour ");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("Type: Portability ");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("Type: Performance ");
        break;
    case GL_DEBUG_TYPE_MARKER:
        printf("Type: Marker ");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        printf("Type: Push Group ");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        printf("Type: Pop Group ");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("Type: Other ");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        printf("Severity: high");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("Severity: medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("Severity: low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("Severity: notification");
        break;
    }
    printf("\n");
}

/**
 * @brief 创建带window的opengl的context，这个创建过程是在主界面线程中进行的，通过消息机制来实现
 *
 * @param independ_mode 是否需要单独窗口模式
 */
static void *native_window_create(int independ_mode)
{

    void *child_window = NULL;
    static int windows_cnt = 0;
    int cnt = windows_cnt++;

    if (independ_mode == 1)
    {
        char name[100];
        sprintf(name, "opengl-child-window%d", cnt);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        // glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

        // glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        //因为咱们是使用的fbo来绘制，因此窗口大小设为1就行了
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        child_window = (void *)glfwCreateWindow(1, 1, name, NULL, glfw_window);

        if (child_window == NULL)
        {
            const char *s = NULL;
            int ret = glfwGetError(&s);
            express_printf("error code %d detail %s", ret, s);
        }
    }
    else
    {
        child_window = egl_createContext();
    }

    assert(child_window != NULL);

    // express_printf("create windows surface %lx\n", d_buffer);
    // todo 根据配置设置窗口属性
    return child_window;
}

/**
 * @brief 覆盖在原来窗口上面用于绘制的窗口的线程主函数，主要包括了窗口的建立和设置
 *
 * @param opaque 需要传入VirtIODevice
 * @return void*
 */
void *native_window_thread(void *opaque)
{
    // VirtIODevice *vdev = opaque;
    // Teleport_Express *e = TELEPORT_EXPRESS(vdev);

    //通过这个方式获取hwnd要求必须使用SDL接口创建界面
    QemuConsole *con;
    while ((con = qemu_console_lookup_by_index(0)) == NULL)
    {
        //理论上启动这个线程时，主窗口的hwnd肯定是有了，所以不会进到这个等待循环内
        g_usleep(10000);
        express_printf("con is NULL\n");
    }

    input_receive_con = con;

    // sync_event_queue = g_queue_new();

    main_window_event_queue = g_async_queue_new();

    // HWND render_hwnd = (HWND)qemu_console_get_window_id(con);
    // RECT rcParent;

    // GetClientRect(render_hwnd, &rcParent);

    //初始化glfw
    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    if (express_gpu_gl_debug_enable)
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }

    //创建一个窗口，这个window也是context
    window_width = express_gpu_window_width;
    window_height = express_gpu_window_height;

    glfw_window = glfwCreateWindow(window_width, window_height, "Trinity", NULL, NULL);

    if (!glfw_window)
    {
        express_printf("create window error %x\n", glfwGetError(NULL));

        glfwTerminate();
        return NULL;
    }

    //键盘事件
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetKeyCallback(glfw_window, express_keyboard_handle_callback);

    //鼠标事件
    glfwSetCursorPosCallback(glfw_window, express_touchscreen_mouse_move_handle);
    glfwSetMouseButtonCallback(glfw_window, express_touchscreen_mouse_click_handle);
    glfwSetScrollCallback(glfw_window, express_touchscreen_mouse_scroll_handle);

    //设置窗口大小可以自由调整
    glfwSetFramebufferSizeCallback(glfw_window, window_size_change_callback);

    glfwSetWindowCloseCallback(glfw_window, close_window_callback);

    glfwSwapInterval(0);

    glfwMakeContextCurrent(glfw_window);

    HDC dpy_dc = GetDC(glfwGetWin32Window(glfw_window));
    HGLRC gl_context = glfwGetWGLContext(glfw_window);
    egl_init(dpy_dc, gl_context);

    dummy_window_for_sync = egl_createContext();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        express_printf("load glad error\n");
        return NULL;
    }

    shutdown_notifier.notify = shutdown_notify_callback;
    qemu_register_shutdown_notifier(&shutdown_notifier);

    gbuffer_global_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    gbuffer_global_types = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    prepare_draw_texi();
    static_value_prepare();

    native_render_run = 2;

    main_window_opengl_prepare(&programID, &drawVAO);
    glBindVertexArray(drawVAO);

    reverse_loc = glGetUniformLocation(programID, "need_reverse");
    is_reverse = 0;

    express_printf("native windows create!\n");

    // if (VSYNC_enable == 0)
    // {
    //     glfwSwapInterval(0);
    // }
    // else
    // {
    //     glfwSwapInterval(1);
    // }

    // int a = 1;
    // glViewport(0, 0, window_width, window_height);
    //因为这个是最终窗口，因此不需要进行深度测试与模板测试，直接贴图，只要最后的图像数据就行
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    //开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
    glDisable(GL_BLEND);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (express_gpu_gl_debug_enable)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }

    gint64 frame_start_time = g_get_real_time();
    gint64 remain_sleep_time = 0;
    gint64 frame_draw_time = 0;
    bool has_refresh = false;

    last_calc_time = frame_start_time;

    while (!glfwWindowShouldClose(glfw_window) && native_render_run == 2)
    {
        gint64 need_sleep_time = 0;
        gint64 now_time = 0;

        do
        {
            //处理各种输入事件、opengl事件
            glfwWaitEventsTimeout(0.001);

            sync_express_touchscreen_input();
            sync_express_keyboard_input();
            express_input_device_sync();

            handle_child_window_event();

            // 在窗口上绘制内容
            if (display_gbuffer != NULL && window_need_refresh)
            {
                if (!window_is_shown)
                {
                    window_is_shown = true;
                    glfwShowWindow(glfw_window);

                    sdl2_no_need = 1;
                }

                window_need_refresh = false;

                opengl_paint(display_gbuffer);
                calc_screen_hz += 1;
                has_refresh = true;

                set_touchscreen_size(display_width, display_height, window_width, window_height);

                glfwSwapBuffers(glfw_window);
            }
            else
            {
                if (display_gbuffer == NULL && window_is_shown == true)
                {
                    window_is_shown = false;
                    printf("hide window\n");
                    glfwHideWindow(glfw_window);

                    sdl2_no_need = 0;
                }
            }

            // 把foreach放到下面，是因为主线程的消息中可能有取消gbuffer销毁流程的消息
            dying_list_foreach(dying_gbuffer, try_destroy_gbuffer);

            now_time = g_get_real_time();

            need_sleep_time = 1000000 / express_gpu_window_FPS - (now_time - frame_start_time) + remain_sleep_time;

        } while (need_sleep_time > 2000);

        if (has_refresh)
        {
            frame_draw_time += now_time - frame_start_time;
            has_refresh = false;
        }

        frame_start_time = now_time;
        remain_sleep_time = need_sleep_time;

        //丢帧丢了100ms了，就不管少休眠的时间了
        if (need_sleep_time < -100000)
        {
            need_sleep_time = 0;
        }
        // printf("remain_sleep_time %lld\n", remain_sleep_time);

        if (now_time - last_calc_time > 1000000)
        {
            now_screen_hz = calc_screen_hz;
            calc_screen_hz = 0;
            float gen_frame_time_avg = 1.0f * frame_draw_time / now_screen_hz;
            if (now_screen_hz == 0)
            {
                printf("screen draw 0 frame this second\n");
            }
            else
            {
                printf("screen draw avg %.2f us %.2f FPS\n", gen_frame_time_avg, now_screen_hz * 1000000.0f / (now_time - last_calc_time));
            }

            frame_draw_time = 0;
            last_calc_time = now_time;
        }

        // glfwWaitEvents();
        // #ifdef ENABLE_STATIC_WINDOW_REFRESH
        //         frame_start_time = g_get_real_time();
        // #else
        //         size_has_change = 0;
        // #endif

        //         // main_frame_num = (main_frame_num + 1) % 65536;

        //         // TIMER_START(queue)
        //         // EVENT_QUEUE_LOCK;
        //         // // if (compose_surface != NULL)
        //         // //     SetEvent((HANDLE)compose_surface->swap_event);
        //         // g_queue_foreach(sync_event_queue, g_queue_event_notify, NULL);
        //         // g_queue_clear(sync_event_queue);
        //         // EVENT_QUEUE_UNLOCK;
        //         // TIMER_END(queue)
        //         // TIMER_OUTPUT(queue, 100)
        //         glClear(GL_COLOR_BUFFER_BIT);

        // #ifdef ENABLE_STATIC_WINDOW_REFRESH
        //         glfwPollEvents();
        // #else
        //         glfwWaitEventsTimeout(0.005);
        // #endif

        //         handle_child_window_event();

        //         // if (is_replaying && replaying_key != 0)
        //         // {
        //         //     if (key_repeat_cnt[replaying_key] < mouse_pos_record_num[replaying_key])
        //         //     {
        //         //         set_express_touchscreen_input((int)(mouse_pos_record[replaying_key][key_repeat_cnt[replaying_key] * 2] / real_window_width * window_width),
        //         //                                       (int)(mouse_pos_record[replaying_key][key_repeat_cnt[replaying_key] * 2 + 1] / real_window_height * window_height), 1, 1);

        //         //         qemu_input_queue_abs(input_receive_con, INPUT_AXIS_X, (int)(mouse_pos_record[replaying_key][key_repeat_cnt[replaying_key] * 2] / real_window_width * window_width), 0, window_width);
        //         //         qemu_input_queue_abs(input_receive_con, INPUT_AXIS_Y, (int)(mouse_pos_record[replaying_key][key_repeat_cnt[replaying_key] * 2 + 1] / real_window_height * window_height), 0, window_height);
        //         //         if (key_repeat_cnt[replaying_key] == 0)
        //         //         {
        //         //             qemu_input_queue_btn(input_receive_con, INPUT_BUTTON_LEFT, true);
        //         //         }
        //         //         key_repeat_cnt[replaying_key]++;
        //         //     }
        //         // }

        //         // if (now_press_key != 0 && mouse_click_record[now_press_key] == 1 && mouse_pos_record_num[now_press_key] < 50)
        //         // {
        //         //     mouse_pos_record[now_press_key][mouse_pos_record_num[now_press_key] * 2] = now_mouse_xpos;
        //         //     mouse_pos_record[now_press_key][mouse_pos_record_num[now_press_key] * 2 + 1] = now_mouse_ypos;
        //         //     mouse_pos_record_num[now_press_key]++;
        //         // }

        //         sync_express_touchscreen_input();

        //         // qemu_input_event_sync();

        //         if (display_gbuffer != NULL)
        //         {
        //             if (sdl2_no_need == 0 && window_width != 0 && window_height != 0)
        //             {
        //                 sdl2_no_need = 1;
        //                 glfwSetWindowSize(glfw_window, window_width * 3 / 4, window_height * 3 / 4);
        //                 glfwShowWindow(glfw_window);
        //             }

        // #ifdef ENABLE_STATIC_WINDOW_REFRESH
        //             opengl_paint(display_gbuffer);
        //             glfwSwapBuffers(glfw_window);
        // #endif
        //         }
        //         else
        //         {
        //             if (sdl2_no_need == 1)
        //             {
        //                 sdl2_no_need = 0;
        //                 window_height = 0;
        //                 window_width = 0;
        //                 real_window_width = window_width;
        //                 real_window_height = window_height;
        //                 glfwHideWindow(glfw_window);
        //             }
        //         }

        // #ifdef ENABLE_STATIC_WINDOW_REFRESH
        // #else
        //         if (!has_painted)
        //         {
        //             if (size_has_change == 1)
        //             {
        //                 opengl_paint(display_gbuffer);
        //                 glfwSwapBuffers(glfw_window);
        //             }
        //             continue;
        //         }
        //         has_painted = 0;
        // #endif

        //         // 把foreach放到下面，是因为主线程的消息中可能有取消gbuffer销毁流程的消息
        //         dying_list_foreach(dying_gbuffer, try_destroy_gbuffer);

        //         gint64 now_time = g_get_real_time();

        //         //注意：帧生成时间波动挺大的

        //         //计算真实窗口帧率
        //         if (now_time - last_calc_time > 1000000 && last_calc_time != 0)
        //         {
        //             calc_screen_hz += 1;
        //             now_screen_hz = calc_screen_hz;
        //             calc_screen_hz = 0;
        //             gen_frame_time_avg_1s = 1000000 / now_screen_hz;
        //             express_printf("screen draw avg %lldus %dHz\n", gen_frame_time_avg_1s, now_screen_hz);

        //             last_calc_time = now_time;
        //         }
        //         else if (last_calc_time == 0)
        //         {
        //             last_calc_time = now_time;
        //             calc_screen_hz = 0;
        //         }
        //         else
        //         {
        //             calc_screen_hz += 1;
        //         }
        // #ifdef ENABLE_STATIC_WINDOW_REFRESH
        //         gint64 spend_time = now_time - frame_start_time;
        //         if (VSYNC_enable == 0)
        //         {
        //             long need_sleep = 1000000 / composer_refresh_HZ - spend_time + remain_sleep_time;

        //             if (need_sleep <= 0)
        //             {
        //                 need_sleep = 0;
        //             }

        //             gint64 sleep_start_time = now_time;
        //             g_usleep(need_sleep);
        //             gint64 sleep_end_time = g_get_real_time();
        //             remain_sleep_time = need_sleep - (sleep_end_time - sleep_start_time);
        //         }
        // #endif
    }

    // qemu_system_shutdown_request(SHUTDOWN_CAUSE_HOST_UI);
    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(glfw_window);

    printf("native windows close!\n");

    //当他返回0时表示窗口被关掉了
    native_render_run = 0;
    // qemu_thread_join(&t);
    return NULL;
}

// /**
//  * @brief swapbuffer时的垂直同步
//  *
//  * @param event
//  * @param interval
//  * @param now_hz
//  * @return int
//  */
// int draw_wait_GSYNC(void *event, int wait_frame_num)
// {

//     //帧率太小的情况，赶不及窗口帧率，直接返回当前窗口frame_num

//     // @todo
//     // 有些游戏有卡顿，怀疑是Gsync的问题，因此这里直接返回main_frame_num试试
//     return main_frame_num;

//     if (wait_frame_num == -1)
//     {
//         return main_frame_num;
//     }

//     if (wait_frame_num - main_frame_num > 60000)
//     {

//         return main_frame_num;
//     }
//     else if (main_frame_num - wait_frame_num > 60000)
//     {
//         while (wait_frame_num != main_frame_num)
//         {
//             EVENT_QUEUE_LOCK;
//             g_queue_push_tail(sync_event_queue, (gpointer)event);
//             EVENT_QUEUE_UNLOCK;
// #ifdef _WIN32
//             DWORD ret = WaitForSingleObject((HANDLE)event, 100);
//             if (ret == WAIT_TIMEOUT)
//             {
//                 express_printf("gsync wait timeout\n");
//             }
// #endif
//         }
//         return main_frame_num;
//     }
//     else if (wait_frame_num <= main_frame_num)
//     {
//         return main_frame_num;
//     }
//     else if (wait_frame_num > main_frame_num)
//     {
//         while (wait_frame_num != main_frame_num)
//         {
//             EVENT_QUEUE_LOCK;
//             g_queue_push_tail(sync_event_queue, (gpointer)event);
//             EVENT_QUEUE_UNLOCK;
// #ifdef _WIN32
//             DWORD ret = WaitForSingleObject(event, 100);
//             if (ret == WAIT_TIMEOUT)
//             {
//                 express_printf("gsync wait timeout\n");
//             }
// #endif
//         }
//         return main_frame_num;
//     }

// }

// static void g_queue_event_notify(gpointer data, gpointer user_data)
// {
// #ifdef _WIN32
//     SetEvent((HANDLE)data);
// #endif
//     return;
// }

void set_display_gbuffer(Graphic_Buffer *gbuffer)
{
    // display_gbuffer = gbuffer;
}

int get_global_gbuffer_type(uint64_t gbuffer_id)
{
    ATOMIC_LOCK(gbuffer_global_types_lock);
    int type = (int)(uint64_t)g_hash_table_lookup(gbuffer_global_types, (gpointer)(gbuffer_id));
    ATOMIC_UNLOCK(gbuffer_global_types_lock);
    return type;
}

void set_global_gbuffer_type(uint64_t gbuffer_id, int type)
{
    ATOMIC_LOCK(gbuffer_global_types_lock);
    if (type == GBUFFER_TYPE_NONE)
    {
        g_hash_table_remove(gbuffer_global_types, (gpointer)(gbuffer_id));
    }
    else
    {
        g_hash_table_insert(gbuffer_global_types, (gpointer)(gbuffer_id), GINT_TO_POINTER(type));
    }
    ATOMIC_UNLOCK(gbuffer_global_types_lock);
    return;
}

void add_gbuffer_to_global(Graphic_Buffer *global_gbuffer)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    g_hash_table_insert(gbuffer_global_map, (gpointer)(global_gbuffer->gbuffer_id), (gpointer)global_gbuffer);
    ATOMIC_UNLOCK(gbuffer_global_map_lock);
}

Graphic_Buffer *get_gbuffer_from_global_map(uint64_t gbuffer_id)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    Graphic_Buffer *gbuffer = (Graphic_Buffer *)g_hash_table_lookup(gbuffer_global_map, (gpointer)(gbuffer_id));
    if (gbuffer != NULL)
    {
        gbuffer->remain_life_time = (gbuffer->usage_type == GBUFFER_TYPE_BITMAP ? MAX_BITMAP_LIFE_TIME : MAX_WINDOW_LIFE_TIME);
    }
    ATOMIC_UNLOCK(gbuffer_global_map_lock);

    return gbuffer;
}

void remove_gbuffer_from_global_map(uint64_t gbuffer_id)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    g_hash_table_remove(gbuffer_global_map, (gpointer)(gbuffer_id));
    ATOMIC_UNLOCK(gbuffer_global_map_lock);
}

void send_message_to_main_window(int message_code, void *data)
{
    Main_window_Event *event = g_malloc(sizeof(Main_window_Event));
    event->event_code = message_code;
    event->data = data;
    ATOMIC_LOCK(main_window_event_queue_lock);
    g_async_queue_push(main_window_event_queue, (gpointer)event);
    ATOMIC_UNLOCK(main_window_event_queue_lock);
    if (message_code == MAIN_PAINT || message_code == MAIN_CREATE_CHILD_WINDOW)
    {
        glfwPostEmptyEvent();
    }
}