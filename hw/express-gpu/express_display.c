/**
 * @file express_display.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-04-23
 *
 * @copyright Copyright (c) 2023
 *
 */

// #define STD_DEBUG_LOG
#include "hw/express-gpu/express_display.h"

#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/egl_surface.h"
#include "hw/express-gpu/express_gpu.h"
#include "hw/express-gpu/express_gpu_main_window.h"
#include "hw/express-gpu/glv1.h"
#include "hw/express-gpu/sdl_control.h"

#include "hw/express-input/express_keyboard.h"
#include "hw/express-input/express_touchscreen.h"

#include "hw/express-mem/express_sync.h"

#include "qemu/atomic.h"
#include "sysemu/runstate.h"

static Thread_Context *static_display_context = NULL;

static void *native_display_context = NULL;

static Display_Status now_display_status;

static GLuint programID = 0;
static GLuint drawVAO = 0;

static GLint program_transform_loc = 0;
static GLuint now_transform_type = 0;

static Hardware_Buffer *display_write_gbuffer;
static Hardware_Buffer *display_read_gbuffer;

static QemuThread qemu_display_thread;

static bool window_need_refresh = false;

static int show_native_window = 0;

static bool window_is_shown = false;

// guest对应的虚拟显示器的大小
static int display_width = 0;
static int display_height = 0;

// QEMU的主窗口的长宽
static int window_width = 0;
static int window_height = 0;

// 显示的内容的实际位置和长宽
static int main_display_content_x = 0;
static int main_display_content_y = 0;

static int main_display_content_width = 0;
static int main_display_content_height = 0;

int express_gpu_window_width = 0;
int express_gpu_window_height = 0;

int sdl2_no_need = 0;

extern int main_window_run;

static Display_Info express_display_info = {
    .pixel_width = 1280,
    .pixel_height = 720,
    .phy_width = 1280,
    .phy_height = 720,
    .refresh_rate_bits = 0x0LL,
};

int *express_display_pixel_width = &(express_display_info.pixel_width);
int *express_display_pixel_height = &(express_display_info.pixel_height);
int *express_display_phy_width = &(express_display_info.phy_width);
int *express_display_phy_height = &(express_display_info.phy_height);
int express_display_refresh_rate;

int display_is_open = 1;

Hardware_Buffer *main_display_gbuffer;

bool express_display_switch_open = false;

bool express_gpu_keep_window_scale = false;

static void opengl_paint_composer_layers(GBuffer_Layers *layers);
static void display_present(void);
static void *virtual_display_thread(void *opaque);

void display_status_change(Display_Status status);

/**
 * @brief
 *
 * @param call
 */
static void display_decode_invoke(Thread_Context *context, Teleport_Express_Call *call)
{
    Call_Para all_para[10];
    size_t temp_len;
    char *temp;
    char *no_ptr_buf = NULL;
    int para_num = get_para_from_call(call, all_para, 10);

    switch (call->id)
    {
    case FUNID_Terminate:
    {
        // do nothing or hide window
        LOGI("display terminate");
    }
    break;
    case FUNID_Commit_Composer_Layer:
    {

        GBuffer_Layers *layers;
        size_t layers_size;

        if (unlikely(para_num < PARA_NUM_Commit_Composer_Layer))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(GBuffer_Layers)))
        {
            break;
        }

        layers = g_malloc0(temp_len);
        layers_size = temp_len;

        read_from_guest_mem(all_para[0].data, layers, 0, all_para[0].data_len);

        if (layers->layer_num * sizeof(GBuffer_Layer) + sizeof(GBuffer_Layers) != layers_size)
        {
            LOGE("error! Gbuffer_Layers' size is not equal to data size num %d calc size %lld layers_size %lld", layers->layer_num, layers->layer_num * sizeof(GBuffer_Layer) + sizeof(GBuffer_Layers), layers_size);
            g_free(layers);
            break;
        }

        opengl_paint_composer_layers(layers);
        g_free(layers);

        display_present();

        send_message_to_main_window(MAIN_PAINT, display_read_gbuffer);
    }
    break;
    case FUNID_Show_Window:
    {
        LOGI("show_native_window");
        show_native_window = 1;
    }
    break;
    case FUNID_Show_Window_FLIP_V:
    {
        LOGI("show_native_window-filp_v");
        show_native_window = 2;
    }
    break;
    case FUNID_Set_Sync_Flag:
    {
        uint64_t sync_id;

        if (unlikely(para_num < PARA_NUM_Set_Sync_Flag))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(uint64_t)))
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (unlikely(temp == NULL))
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                read_from_guest_mem(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        sync_id = *(uint64_t *)(temp);

        signal_express_sync((int)sync_id, true);
    }
    break;
    case FUNID_Wait_Sync:
    {
        uint64_t sync_id;

        if (unlikely(para_num < PARA_NUM_Wait_Sync))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(uint64_t)))
        {
            break;
        }

        int null_flag = 0;
        temp = get_direct_ptr(all_para[0].data, &null_flag);
        if (unlikely(temp == NULL))
        {
            if (temp_len != 0 && null_flag == 0)
            {
                temp = g_malloc(temp_len);
                no_ptr_buf = temp;
                read_from_guest_mem(all_para[0].data, temp, 0, all_para[0].data_len);
            }
            else
            {
                break;
            }
        }

        sync_id = *(uint64_t *)(temp);
        LOGD("going to wait for sync in display %d", (int)sync_id);
        wait_for_express_sync((int)sync_id, true);
    }
    break;
    case FUNID_Get_Display_Mods:
    {

        if (unlikely(para_num < PARA_NUM_Get_Display_Mods))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(Display_Info)))
        {
            break;
        }

        write_to_guest_mem(all_para[0].data, &express_display_info, 0, sizeof(Display_Info));
        // LOGI("FUNID_Get_Display_Mods");
    }
    break;
    case FUNID_Set_Display_Status:
    {

        Display_Status status;

        if (unlikely(para_num < PARA_NUM_Set_Display_Status))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(Display_Status)))
        {
            break;
        }

        read_from_guest_mem(all_para[0].data, &status, 0, sizeof(Display_Status));

        display_status_change(status);
        // LOGI("FUNID_Set_Display_Status");
    }
    break;
    case FUNID_Get_Display_Status:
    {

        if (unlikely(para_num < PARA_NUM_Get_Display_Status))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(Display_Status)))
        {
            break;
        }

        write_to_guest_mem(all_para[0].data, &now_display_status, 0, sizeof(Display_Status));
    }
    break;
    default:
    {
        LOGE("error! unknown display invoke id %llx para_num %d", call->id, para_num);
    }
    }

    if (no_ptr_buf != NULL)
    {
        g_free(no_ptr_buf);
    }

    call->callback(call, 1);

    return;
}

static Thread_Context *get_display_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{

    if (static_display_context == NULL)
    {
        static_display_context = thread_context_create(thread_id, device_id, sizeof(Thread_Context), info);

        if (express_display_refresh_rate > 0 && express_display_refresh_rate <= 64 * 15 /* 15 per bit, 64 bits */ && express_display_refresh_rate % 15 == 0) {
            express_display_info.refresh_rate_bits = 0x1ULL << ((express_display_refresh_rate - 15) / 15);
        }
        else {
            LOGW("invalid refresh rate setting %d, must be a multiple of 15, defaulting to 60.", express_display_refresh_rate);
            express_display_refresh_rate = 60;
        }
    }

    return static_display_context;
}

static void display_context_init(Thread_Context *context)
{
    express_printf("display context init!\n");
    // 这个render线程只能创建一次，且其他线程必须等待该线程运行成功
    start_main_window_thread();
    qemu_thread_create(&qemu_display_thread, "display_thread",
                       virtual_display_thread, (void *)0, QEMU_THREAD_DETACHED);

    // 新建一个context用于与纹理交互
    if (native_display_context == NULL)
    {
        send_message_to_main_window(MAIN_CREATE_CHILD_WINDOW, &native_display_context);

        int sleep_cnt = 0;
        while (native_display_context == NULL)
        {
            g_usleep(1000);
            sleep_cnt += 1;
            if (sleep_cnt >= 100 && sleep_cnt % 500 == 0)
            {
                LOGI("wait for native_display_context creating too long!");
            }
        }

        egl_makeCurrent(native_display_context);

        display_write_gbuffer = create_gbuffer(express_display_info.pixel_width, express_display_info.pixel_height,
                                               0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8, 0, 0, 0);
        display_read_gbuffer = create_gbuffer(express_display_info.pixel_width, express_display_info.pixel_height,
                                              0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8, 0, 0, 0);

        glGenFramebuffers(1, &display_write_gbuffer->data_fbo);
        glGenFramebuffers(1, &display_read_gbuffer->data_fbo);

        glBindFramebuffer(GL_FRAMEBUFFER, display_read_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_read_gbuffer->data_texture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_write_gbuffer->data_texture, 0);

        display_opengl_prepare(&programID, &drawVAO);
        glBindVertexArray(drawVAO);

        program_transform_loc = glGetUniformLocation(programID, "transform_loc");
        now_transform_type = 0;

        glEnable(GL_SCISSOR_TEST);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        // 开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
        glDisable(GL_BLEND);

        if (express_gpu_gl_debug_enable)
        {
            #ifdef _WIN32
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(gl_debug_output, NULL);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
            #endif
        }
    }
}

static void display_context_destroy(Thread_Context *context)
{
    if (native_display_context != NULL)
    {
        egl_makeCurrent(NULL);
        egl_destroyContext(native_display_context);
        native_display_context = NULL;
    }
}

static void opengl_paint_composer_layers(GBuffer_Layers *layers)
{
    int display_height = express_display_info.pixel_height;

    if (layers != NULL)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (!display_is_open && express_display_switch_open)
        {
            return;
        }

        for (int i = 0; i < layers->layer_num; i++)
        {
            GBuffer_Layer layer = layers->layer[i];

            express_printf("composer wait for sync %d\n", layer.write_sync_id);

            LOGD("going to wait for sync in display layer %d", layer.write_sync_id);

            wait_for_express_sync(layer.write_sync_id, true);

            Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(layer.gbuffer_id);
            if (gbuffer != NULL)
            {
                LOGD("draw layer gbuffer_id %llx  %d %d %d %d gbuffer_size %d %d blend_type %d transform_type %d",
                               layer.gbuffer_id, layer.x, layer.y, layer.width, layer.height, gbuffer->width, gbuffer->height, layer.blend_type, layer.transform_type);
                // layer的大小是显示的像素区域位置大小（与屏幕大小直接相关），
                // crop的大小是原始gbuffer裁剪后的像素位置大小（与屏幕大小无关，而与原始缓冲区大小有关），
                // 两者间可能存在缩放关系
                // 这里计算得到的是，在缩放正确的情况下，原始的整个gbuffer绘制到当前界面的位置
                int view_w = gbuffer->width * layer.width / layer.crop_width;
                int view_h = gbuffer->height * layer.height / layer.crop_height;
                int view_x = layer.x - layer.crop_x * layer.width / layer.crop_width;
                int view_y = 0;
                if (show_native_window == 2)
                {
                    // 安卓9的显示
                    view_y = layer.y - layer.crop_y * layer.height / layer.crop_height;
                    if (layer.transform_type == FLIP_V)
                    {
                        // guest在设置了上下翻转的情况下，layer的crop坐标也会是翻转后的图像区域坐标,
                        // 也就是，crop的xy实际是翻转后的图像的左上角（即实际的左下角）
                        // 需要手动把这个crop坐标上下翻转过来，获得真正图像左上角的xy坐标
                        view_y = layer.y - (gbuffer->height - layer.crop_height - layer.crop_y) * layer.height / layer.crop_height;
                    }
                }
                else if (show_native_window == 1)
                {
                    // 先进行缩放，计算原始gbuffer的左上角应该在哪（以窗口上面为y轴零点）
                    view_y = layer.y - layer.crop_y * layer.height / layer.crop_height;
                    // 然后计算gbuffer的左下角应该在哪（以窗口下面为y轴零点）
                    view_y = display_height - view_y - view_h;
                }

                LOGD("glviewport %d %d %d %d glScissor %d %d %d %d", view_x, view_y, view_w, view_h, layer.x, display_height - layer.y - layer.height, layer.width, layer.height);
                LOGD("layer %d %d %d %d crop %d %d %d %d", layer.x, layer.y, layer.width, layer.height, layer.crop_x, layer.crop_y, layer.crop_width, layer.crop_height);
                glViewport(view_x, view_y, view_w, view_h);

                // glScissor是当前视口的裁剪情况，整个裁剪是说这个区域外就不绘制了，但是空间还是占着
                // 而合成器的crop裁剪，是直接区域裁掉，所占的区域就没了
                // 简单的说，从效果上来看，合成器的裁剪是把原来的图片给剪了一下，变小了后再缩放贴到屏幕缓冲区的相应位置
                // 而glScissor，是原来的图片整个都贴到缓冲区的相应位置，但是屏幕缓冲区所指定的区域之外的地方用东西给盖住（其实是不绘制，而不是盖住）
                if (show_native_window == 2)
                {
                    glScissor(layer.x, layer.y, layer.width, layer.height);
                }
                else if (show_native_window == 1)
                {
                    glScissor(layer.x, display_height - layer.y - layer.height, layer.width, layer.height);
                }

                adjust_blend_type(layer.blend_type);

                // 合成器以翻转的形式合成，然后显示的时候再翻转一次，一是为了与系统内逻辑一致，
                // 否则浏览器自己合成视频播放图像时，会显示的倒着，二是为了更高效的复制GraphicBuffer的数据（不用倒着复制了）
                if (now_transform_type != layer.transform_type)
                {
                    now_transform_type = layer.transform_type;
                    if (now_transform_type != FLIP_V && now_transform_type != ROTATE_NONE)
                    {
                        LOGE("error! not support transform_type %d", now_transform_type);
                    }
                    glUniform1i(program_transform_loc, now_transform_type);
                }

                opengl_paint_gbuffer(gbuffer);

                express_printf("composer set sync %d\n", layer.read_sync_id);

                signal_express_sync(layer.read_sync_id, true);
            }
        }

        GLsync temp_sync = display_write_gbuffer->delete_sync;

        display_write_gbuffer->delete_sync = display_write_gbuffer->data_sync;
        display_write_gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        display_write_gbuffer->is_writing = 0;

        if (temp_sync != 0)
        {
            glDeleteSync(temp_sync);
        }

        glFlush();
    }
}

static void display_present(void)
{
    Hardware_Buffer *temp_gbuffer = display_read_gbuffer;

    display_read_gbuffer = display_write_gbuffer;

    display_write_gbuffer = temp_gbuffer;

    glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);

    // 保证主线程使用完成上一个gbuffer
    ATOMIC_LOCK(display_write_gbuffer->is_lock);

    main_display_gbuffer = display_read_gbuffer;

    // 这里直接unlock，不需要一直锁住，是因为只有这个画完了之后，才会赋值到main_display_gbuffer，所以不会被主线程访问到，也就不需要锁住
    // 无论主线程之后会不会继续读取，这里都要直接进行后续的绘制
    ATOMIC_UNLOCK(display_write_gbuffer->is_lock);

    window_need_refresh = true;

    static int now_screen_hz = 0;
    static uint64_t last_record_time = 0;
    uint64_t now_time = g_get_real_time();
    now_screen_hz++;

    if (now_time - last_record_time > 1000000)
    {
        float gen_frame_time_avg = 1.0f * (now_time - last_record_time) / now_screen_hz;
        LOGD("composer draw avg %.2f us %.2f FPS", gen_frame_time_avg, now_screen_hz * 1000000.0f / (now_time - last_record_time));
        last_record_time = now_time;
        now_screen_hz = 0;
    }
}

void display_status_change(Display_Status status)
{
    LOGI("display_status_change refresh_rate %d=>%d power_stats %d=>%d backlight %u=>%u",
           now_display_status.refresh_rate, status.refresh_rate, now_display_status.power_status, status.power_status,
           now_display_status.backlight, status.backlight);
    if (express_display_switch_open)
    {
        now_display_status = status;
        if (now_display_status.power_status == 3)
        {
            display_is_open = 0;
        }
        else
        {
            display_is_open = 1;
        }
    }
}

static void opengl_paint_composer_gbuffer(void)
{
    Hardware_Buffer *gbuffer = main_display_gbuffer;

    if (gbuffer == NULL)
    {
        return;
    }

    ATOMIC_LOCK(gbuffer->is_lock);

    if (display_width != gbuffer->width || display_height == gbuffer->height)
    {
        display_width = gbuffer->width;
        display_height = gbuffer->height;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(main_display_content_x, main_display_content_y, main_display_content_width, main_display_content_height);

    glWaitSync(gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);

    opengl_paint_gbuffer(gbuffer);

    ATOMIC_UNLOCK(gbuffer->is_lock);

    glFlush();
}

/**
 * @brief 界面上用于画出图像的函数，实际逻辑为取出gbuffer中的display_texture，然后画出来
 *
 * @param gbuffer
 */
void opengl_paint_gbuffer(Hardware_Buffer *gbuffer)
{
    if (gbuffer != NULL)
    {
        if (gbuffer->is_writing != 0)
        {
            LOGE("error! get writing gbuffer when opengl_paint");
        }

        LOGD("draw gbuffer_id %llx data sync %lld", gbuffer->gbuffer_id, (uint64_t)gbuffer->data_sync);

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

static void window_size_change_callback(GLFWwindow *window, int width, int height)
{
    window_need_refresh = true;

    // macos retina screen handling
    float xscale = 1, yscale = 1;
#ifdef __APPLE__
    glfwGetWindowContentScale(window, &xscale, &yscale);
#endif

    // 需要保证画面比例不变
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
            //printf("set window size %d %d  %d %d %d %d %d %dkeep scale\n", window_width, window_height,temp_window_width,temp_window_height,display_width,display_height,x,y);
            window_width = temp_window_width;
            window_height = temp_window_height;

            glViewport(0, 0, window_width, window_height);

            main_display_content_x = 0;
            main_display_content_y = 0;
            main_display_content_width = window_width;
            main_display_content_height = window_height;

            glfwSetWindowSize(window, window_width / xscale, window_height / yscale);
        }
        else
        {
            glViewport(x, y, temp_window_width, temp_window_height);

            main_display_content_x = x;
            main_display_content_y = y;
            main_display_content_width = temp_window_width;
            main_display_content_height = temp_window_height;
        }

        express_printf("set touchscreen size %d %d\n", window_width, window_height);
        set_touchscreen_window_size(window_width / xscale, window_height / yscale);
    }

    return;
}

static void close_window_callback(GLFWwindow *window)
{
    static gint64 last_click_time = 0;
    gint64 now_time = g_get_real_time();

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

/**
 * Virtual display thread entry point
 * One thread for each virtual display
 */
static void *virtual_display_thread(void *opaque) {
    GLuint programID = 0;
    GLuint drawVAO = 0;

    // 创建一个窗口，这个window也是context
    window_width = express_gpu_window_width;
    window_height = express_gpu_window_height;
    display_width = *express_display_pixel_width;
    display_height = *express_display_pixel_height;

    main_display_content_width = express_gpu_window_width;
    main_display_content_height = express_gpu_window_height;

    GLFWwindow* glfw_window = get_native_opengl_context(DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT);

    if (!glfw_window)
    {
        LOGE("error: cannot allocate native window for virtual display %x", glfwGetError(NULL));
        return NULL;
    }

    // window title
    glfwSetWindowTitle(glfw_window, "vSoC");

    // 键盘事件
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetKeyCallback(glfw_window, express_keyboard_handle_callback);

    // 鼠标事件
    glfwSetCursorPosCallback(glfw_window, express_touchscreen_mouse_move_handle);
    glfwSetMouseButtonCallback(glfw_window, express_touchscreen_mouse_click_handle);
    glfwSetScrollCallback(glfw_window, express_touchscreen_mouse_scroll_handle);

    // 开启触摸屏支持
    glfwSetInputMode(glfw_window, GLFW_TOUCH, GLFW_TRUE);
    glfwSetTouchCallback(glfw_window, express_touchscreen_touch_handle);

    // 捕获鼠标进出事件，在鼠标移动出窗口时，需要停用输入，即需要传递触摸屏release消息
    glfwSetCursorEnterCallback(glfw_window, express_touchscreen_entered_handle);

    // 设置窗口大小可以自由调整
    float xscale = 1, yscale = 1;
#ifdef __APPLE__
    // macos retina screen handling
    glfwGetWindowContentScale(glfw_window, &xscale, &yscale);
#endif
    glfwSetWindowSize(glfw_window, window_width / xscale, window_height / yscale);
    set_touchscreen_window_size(window_width / xscale, window_height / yscale);
    glfwSetFramebufferSizeCallback(glfw_window, window_size_change_callback);
    glfwSetWindowCloseCallback(glfw_window, close_window_callback);

    THREAD_CONTROL_END

    glfwMakeContextCurrent(glfw_window);

    glfwSwapInterval(0);

    prepare_draw_texi();

    display_opengl_prepare(&programID, &drawVAO);
    glBindVertexArray(drawVAO);

    program_transform_loc = glGetUniformLocation(programID, "transform_loc");
    now_transform_type = 0;
    glUniform1i(program_transform_loc, now_transform_type);

    LOGD("virtual display create");

    // 因为这个是最终窗口，因此不需要进行深度测试与模板测试，直接贴图，只要最后的图像数据就行
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    // 开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
    glDisable(GL_BLEND);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (express_gpu_gl_debug_enable)
    {
    #ifdef _WIN32
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(d_debug_message_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    #endif
    }

    int calc_screen_hz = 0;
    int now_screen_hz = 0;
    gint64 last_calc_time = 0;
    gint64 frame_start_time = g_get_real_time();
    gint64 remain_sleep_time = 0;
    gint64 frame_draw_time = 0;
    bool has_refresh = false;

    last_calc_time = frame_start_time;

    while (!glfwWindowShouldClose(glfw_window) && main_window_run == 2)
    {
        gint64 need_sleep_time = 0;
        gint64 now_time = 0;

        do
        {
            // 处理各种输入事件、opengl事件
            THREAD_CONTROL_BEGIN

            //处理各种输入事件、opengl事件
            glfwWaitEventsTimeout(0.001);

            THREAD_CONTROL_END

            sync_express_touchscreen_input((bool)display_is_open || !express_display_switch_open);
            sync_express_keyboard_input((bool)display_is_open || !express_display_switch_open);

            if (show_native_window != 0 && window_is_shown == false)
            {
                if (show_native_window == 2)
                {
                    // 合成器以翻转的形式合成，然后显示的时候再翻转一次，一是为了与安卓系统内逻辑一致，
                    // 否则浏览器自己合成视频播放图像时，会显示的倒着，二是为了更高效的复制GraphicBuffer的数据（不用倒着复制了）
                    // 鸿蒙就不翻转了，因为就没有翻转的功能
                    now_transform_type = FLIP_V;
                    glUniform1i(program_transform_loc, now_transform_type);
                }
                glfwShowWindow(glfw_window);
                glfwSwapBuffers(glfw_window);
                window_is_shown = true;
                sdl2_no_need = 1;
            }

            // 在窗口上绘制内容
            if (main_display_gbuffer != NULL && window_need_refresh)
            {
                if (!window_is_shown)
                {
                    window_is_shown = true;
                    glfwShowWindow(glfw_window);
                    sdl2_no_need = 1;
                }

                window_need_refresh = false;

                opengl_paint_composer_gbuffer();

                calc_screen_hz += 1;
                has_refresh = true;

                glfwSwapBuffers(glfw_window);
            }

            now_time = g_get_real_time();

            need_sleep_time = 1000000 / express_display_refresh_rate - (now_time - frame_start_time) + remain_sleep_time;

        } while (need_sleep_time > 2000);

        if (has_refresh)
        {
            frame_draw_time += now_time - frame_start_time;
            has_refresh = false;
        }

        frame_start_time = now_time;
        remain_sleep_time = need_sleep_time;

        // 丢帧丢了100ms了，就不管少休眠的时间了
        if (need_sleep_time < -100000)
        {
            need_sleep_time = 0;
        }

        if (now_time - last_calc_time > 1000000)
        {
            now_screen_hz = calc_screen_hz;
            calc_screen_hz = 0;
            float gen_frame_time_avg = 1.0f * frame_draw_time / now_screen_hz;
            if (now_screen_hz == 0)
            {
                express_printf("screen draw 0 frame this second\n");
            }
            else
            {
                LOGD("screen draw avg %.2f us %.2f FPS", gen_frame_time_avg, now_screen_hz * 1000000.0f / (now_time - last_calc_time));
            }

            char name[32];
            sprintf(name, "vSoC:%d FPS %.1f", 0, now_screen_hz * 1000000.0f / (now_time - last_calc_time));
            glfwSetWindowTitle(glfw_window, name);

            frame_draw_time = 0;
            last_calc_time = now_time;
        }
    }

    glfwMakeContextCurrent(NULL);

    THREAD_CONTROL_BEGIN

    glfwDestroyWindow(glfw_window);

    THREAD_CONTROL_END

    LOGD("virtual display close!");

    return NULL;
}

static Express_Device_Info express_gpu_info = {
    .enable_default = true,
    .name = "express-display",
    .option_name = "display",
    .device_id = EXPRESS_DISPLAY_DEVICE_ID,
    .device_type = OUTPUT_DEVICE_TYPE,
    .call_handle = display_decode_invoke,
    .context_init = display_context_init,
    .context_destroy = display_context_destroy,
    .get_context = get_display_thread_context,
};

EXPRESS_DEVICE_INIT(express_display, &express_gpu_info)
