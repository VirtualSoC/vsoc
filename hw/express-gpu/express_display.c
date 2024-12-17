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

int express_display_pixel_width;
int express_display_pixel_height;
int express_gpu_window_width;
int express_gpu_window_height;
int express_display_refresh_rate;
bool express_display_switch_open = false;
bool express_gpu_keep_window_scale = false;

static GHashTable *g_display_contexts = NULL;

static int atomic_id_counter;

int sdl2_no_need = 0;

extern int main_window_run;

static void opengl_paint_composer_layers(Display_Context *disp, GBuffer_Layers *layers);
static void display_present(Display_Context *disp);
void display_status_change(Display_Context *disp, Display_Status status);

static void *virtual_display_thread(void *opaque);

static void display_decode_invoke(Thread_Context *context, Teleport_Express_Call *call)
{
    Call_Para all_para[10];
    size_t temp_len;
    char *temp;
    char *no_ptr_buf = NULL;
    int para_num = get_para_from_call(call, all_para, 10);
    Display_Context *disp = (Display_Context *)g_hash_table_lookup(g_display_contexts, GUINT_TO_POINTER(call->unique_id));

    switch (call->id)
    {
    case FUNID_Terminate:
    {
        // do nothing or hide window
        LOGI("display uid %" PRIx64 " terminate");
    }
    break;
    case FUNID_Get_Display_Count:
    {
        int need_free = 0;
        char *_ptr;
        _ptr = call_para_to_ptr(all_para[0], &need_free);

        uint64_t display_count = 1;
        write_to_guest_mem(all_para[0].data, &display_count, 0, sizeof(uint64_t));
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

        opengl_paint_composer_layers(disp, layers);
        g_free(layers);

        display_present(disp);
    }
    break;
    case FUNID_Show_Window:
    {
        LOGI("disp %s: show_native_window", disp->info.name);
        disp->show_native_window = 1;

        char name[32];
        sprintf(name, "display_%s", disp->info.name);
        qemu_thread_create(&disp->qemu_thread, name,
                    virtual_display_thread, (void *)disp,
                    QEMU_THREAD_DETACHED);
    }
    break;
    case FUNID_Show_Window_FLIP_V:
    {
        LOGI("disp %s: show_native_window_filp_v", disp->info.name);
        disp->show_native_window = 2;

        char name[32];
        sprintf(name, "display_%s", disp->info.name);
        qemu_thread_create(&disp->qemu_thread, name,
                    virtual_display_thread, (void *)disp,
                    QEMU_THREAD_DETACHED);

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

        write_to_guest_mem(all_para[0].data, &disp->info, 0, sizeof(Display_Info));
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

        display_status_change(disp, status);
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

        write_to_guest_mem(all_para[0].data, &disp->status, 0, sizeof(Display_Status));
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

static Thread_Context *get_display_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (g_display_contexts == NULL) // init
    {
        g_display_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    Thread_Context *context = (Thread_Context *)g_hash_table_lookup(g_display_contexts, GUINT_TO_POINTER(unique_id));

    // 没有context就新建线程
    if (context == NULL)
    {
        context = thread_context_create(thread_id, device_id, sizeof(Display_Context), info);

        Display_Context *disp = (Display_Context *)context;
        disp->unique_id = unique_id;

        LOGD("display uid %" PRId64 " create context", unique_id);
        g_hash_table_insert(g_display_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
    }
    return context;
}

static void display_context_init(Thread_Context *context)
{
    Display_Context *disp = (Display_Context *)context;
    start_main_window_thread();

    sprintf(disp->info.name, "%d", qatomic_fetch_inc(&atomic_id_counter));
    disp->info.pixel_width = express_display_pixel_width;
    disp->info.pixel_height = express_display_pixel_height;

    if (express_display_refresh_rate > 0 && express_display_refresh_rate <= 64 * 15 /* 15 per bit, 64 bits */ && express_display_refresh_rate % 15 == 0) {
        disp->info.refresh_rate_bits = 0x1ULL << ((express_display_refresh_rate - 15) / 15);
    }
    else {
        LOGW("invalid refresh rate setting %d, must be a multiple of 15, defaulting to 60.", express_display_refresh_rate);
        express_display_refresh_rate = 60;
        disp->info.refresh_rate_bits = 0x1ULL << ((express_display_refresh_rate - 15) / 15);
    }

    disp->window_width = express_gpu_window_width;
    disp->window_height = express_gpu_window_height;

    disp->content_w = express_gpu_window_width;
    disp->content_h = express_gpu_window_height;

    // 新建一个context用于与纹理交互
    if (disp->window == NULL)
    {
        disp->window = get_native_opengl_context(0);

        egl_makeCurrent(disp->window);

        disp->write_gbuffer = create_gbuffer(disp->info.pixel_width, disp->info.pixel_height,
                                               0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8, 0, 0, 0);
        disp->read_gbuffer = create_gbuffer(disp->info.pixel_width, disp->info.pixel_height,
                                              0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8, 0, 0, 0);

        glGenFramebuffers(1, &disp->write_gbuffer->data_fbo);
        glGenFramebuffers(1, &disp->read_gbuffer->data_fbo);

        glBindFramebuffer(GL_FRAMEBUFFER, disp->read_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, disp->read_gbuffer->data_texture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, disp->write_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, disp->write_gbuffer->data_texture, 0);

        display_opengl_prepare(&disp->programID, &disp->drawVAO);
        glBindVertexArray(disp->drawVAO);

        disp->transform_uniform = glGetUniformLocation(disp->programID, "transform_loc");
        disp->transform_type = 0;

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
    Display_Context *disp = (Display_Context *)context;
    if (disp->window != NULL) {
        egl_makeCurrent(NULL);
        egl_destroyContext(disp->window);
        disp->window = NULL;
    }
    if (disp->write_gbuffer != NULL) {
        destroy_gbuffer(disp->write_gbuffer);
    }
    if (disp->read_gbuffer != NULL) {
        destroy_gbuffer(disp->read_gbuffer);
    }
}

static void opengl_paint_composer_layers(Display_Context *disp, GBuffer_Layers *layers)
{
    if (layers != NULL)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (!disp->is_open && express_display_switch_open)
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
                if (disp->show_native_window == 2)
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
                else if (disp->show_native_window == 1)
                {
                    // 先进行缩放，计算原始gbuffer的左上角应该在哪（以窗口上面为y轴零点）
                    view_y = layer.y - layer.crop_y * layer.height / layer.crop_height;
                    // 然后计算gbuffer的左下角应该在哪（以窗口下面为y轴零点）
                    view_y = disp->info.pixel_height - view_y - view_h;
                }

                LOGD("glviewport %d %d %d %d glScissor %d %d %d %d", view_x, view_y, view_w, view_h, layer.x, disp->info.pixel_height - layer.y - layer.height, layer.width, layer.height);
                LOGD("layer %d %d %d %d crop %d %d %d %d", layer.x, layer.y, layer.width, layer.height, layer.crop_x, layer.crop_y, layer.crop_width, layer.crop_height);
                glViewport(view_x, view_y, view_w, view_h);

                // glScissor是当前视口的裁剪情况，整个裁剪是说这个区域外就不绘制了，但是空间还是占着
                // 而合成器的crop裁剪，是直接区域裁掉，所占的区域就没了
                // 简单的说，从效果上来看，合成器的裁剪是把原来的图片给剪了一下，变小了后再缩放贴到屏幕缓冲区的相应位置
                // 而glScissor，是原来的图片整个都贴到缓冲区的相应位置，但是屏幕缓冲区所指定的区域之外的地方用东西给盖住（其实是不绘制，而不是盖住）
                if (disp->show_native_window == 2)
                {
                    glScissor(layer.x, layer.y, layer.width, layer.height);
                }
                else if (disp->show_native_window == 1)
                {
                    glScissor(layer.x, disp->info.pixel_height - layer.y - layer.height, layer.width, layer.height);
                }

                adjust_blend_type(layer.blend_type);

                // 合成器以翻转的形式合成，然后显示的时候再翻转一次，一是为了与系统内逻辑一致，
                // 否则浏览器自己合成视频播放图像时，会显示的倒着，二是为了更高效的复制GraphicBuffer的数据（不用倒着复制了）
                if (disp->transform_type != layer.transform_type)
                {
                    disp->transform_type = layer.transform_type;
                    if (disp->transform_type != FLIP_V && disp->transform_type != ROTATE_NONE)
                    {
                        LOGE("error! not support transform_type %d", disp->transform_type);
                    }
                    glUniform1i(disp->transform_uniform, disp->transform_type);
                }

                opengl_paint_gbuffer(gbuffer);

                express_printf("composer set sync %d\n", layer.read_sync_id);

                signal_express_sync(layer.read_sync_id, true);
            }
        }

        GLsync temp_sync = disp->write_gbuffer->delete_sync;

        disp->write_gbuffer->delete_sync = disp->write_gbuffer->data_sync;
        disp->write_gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        disp->write_gbuffer->is_writing = 0;

        if (temp_sync != 0)
        {
            glDeleteSync(temp_sync);
        }

        glFlush();
    }
}

static void display_present(Display_Context *disp)
{
    Hardware_Buffer *temp_gbuffer = disp->read_gbuffer;
    disp->read_gbuffer = disp->write_gbuffer;
    disp->write_gbuffer = temp_gbuffer;

    glBindFramebuffer(GL_FRAMEBUFFER, disp->write_gbuffer->data_fbo);

    // 保证主线程使用完成上一个gbuffer
    ATOMIC_LOCK(disp->write_gbuffer->is_lock);

    disp->display_gbuffer = disp->read_gbuffer;

    // 这里直接unlock，不需要一直锁住，是因为只有这个画完了之后，才会赋值到display_gbuffer，所以不会被主线程访问到，也就不需要锁住
    // 无论主线程之后会不会继续读取，这里都要直接进行后续的绘制
    ATOMIC_UNLOCK(disp->write_gbuffer->is_lock);

    disp->window_need_refresh = true;

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

void display_status_change(Display_Context *disp, Display_Status status)
{
    LOGI("display_status_change refresh_rate %d=>%d power_stats %d=>%d backlight %u=>%u",
           disp->status.refresh_rate, status.refresh_rate, disp->status.power_status, status.power_status,
           disp->status.backlight, status.backlight);
    if (express_display_switch_open)
    {
        disp->status = status;
        if (disp->status.power_status == 3)
        {
            disp->is_open = 0;
        }
        else
        {
            disp->is_open = 1;
        }
    }
}

static void opengl_paint_composer_gbuffer(Display_Context *disp)
{
    Hardware_Buffer *gbuffer = disp->display_gbuffer;

    if (gbuffer == NULL)
    {
        return;
    }

    ATOMIC_LOCK(gbuffer->is_lock);

    if (disp->info.pixel_width != gbuffer->width || disp->info.pixel_height == gbuffer->height)
    {
        disp->info.pixel_width = gbuffer->width;
        disp->info.pixel_height = gbuffer->height;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(disp->content_x, disp->content_y, disp->content_w, disp->content_h);

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
    Display_Context *disp = (Display_Context *)glfwGetWindowUserPointer(window);

    // macos retina screen handling
    float xscale = 1, yscale = 1;
#ifdef __APPLE__
    glfwGetWindowContentScale(window, &xscale, &yscale);
#endif

    // 需要保证画面比例不变
    if (disp->window_width != width || disp->window_height != height)
    {

        disp->window_width = width;
        disp->window_height = height;

        int temp_window_width = disp->window_width;
        int temp_window_height = disp->window_height;
        int x = 0;
        int y = 0;

        if ((double)disp->info.pixel_width / disp->info.pixel_height > (double)disp->window_width / disp->window_height)
        {
            temp_window_height = (int)((double)disp->info.pixel_height / disp->info.pixel_width * disp->window_width);
            y = (disp->window_height - temp_window_height) / 2;
        }
        else
        {
            temp_window_width = (int)((double)disp->info.pixel_width / disp->info.pixel_height * disp->window_height);
            x = (disp->window_width - temp_window_width) / 2;
        }

        if (express_gpu_keep_window_scale)
        {
            //printf("set window size %d %d  %d %d %d %d %d %dkeep scale\n", window_width, window_height,temp_window_width,temp_window_height,disp->info.pixel_width,disp->info.pixel_height,x,y);
            disp->window_width = temp_window_width;
            disp->window_height = temp_window_height;

            glViewport(0, 0, disp->window_width, disp->window_height);

            disp->content_x = 0;
            disp->content_y = 0;
            disp->content_w = disp->window_width;
            disp->content_h = disp->window_height;

            glfwSetWindowSize(window, disp->window_width / xscale, disp->window_height / yscale);
        }
        else
        {
            glViewport(x, y, temp_window_width, temp_window_height);

            disp->content_x = x;
            disp->content_y = y;
            disp->content_w = temp_window_width;
            disp->content_h = temp_window_height;
        }

        express_printf("set touchscreen size %d %d\n", disp->window_width, disp->window_height);
        set_touchscreen_window_size(disp->window_width / xscale, disp->window_height / yscale);
    }

    disp->window_need_refresh = true;

    return;
}

static void close_window_callback(GLFWwindow *window)
{
    // todo: handle display hotplug event
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
    Display_Context *disp = (Display_Context *)opaque;
    GLuint programID = 0;
    GLuint drawVAO = 0;
    char name[32];

    // 创建一个窗口，这个window也是context
    GLFWwindow* glfw_window = get_native_opengl_context(DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT);
    glfwSetWindowUserPointer(glfw_window, disp);

    if (!glfw_window)
    {
        LOGE("error: cannot allocate native window for virtual display %x", glfwGetError(NULL));
        return NULL;
    }

    // window title
    sprintf(name, "vSoC:%s", disp->info.name);
    glfwSetWindowTitle(glfw_window, name);

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
    glfwSetWindowSize(glfw_window, disp->window_width / xscale, disp->window_height / yscale);
    set_touchscreen_window_size(disp->window_width / xscale, disp->window_height / yscale);
    glfwSetFramebufferSizeCallback(glfw_window, window_size_change_callback);
    glfwSetWindowCloseCallback(glfw_window, close_window_callback);

    THREAD_CONTROL_END

    glfwMakeContextCurrent(glfw_window);

    glfwSwapInterval(0);

    prepare_draw_texi();

    display_opengl_prepare(&programID, &drawVAO);
    glBindVertexArray(drawVAO);

    disp->transform_uniform = glGetUniformLocation(disp->programID, "transform_loc");
    glUniform1i(disp->transform_uniform, disp->transform_type);

    LOGD("virtual display %s create", name);

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
            // todo: verify if events are already processed in the main window thread
            // no need to poll in subwindows
            // glfwWaitEventsTimeout(0.001);

            sync_express_touchscreen_input((bool)disp->is_open || !express_display_switch_open);
            sync_express_keyboard_input((bool)disp->is_open || !express_display_switch_open);

            if (disp->show_native_window != 0 && disp->window_is_shown == false)
            {
                if (disp->show_native_window == 2)
                {
                    // 合成器以翻转的形式合成，然后显示的时候再翻转一次，一是为了与安卓系统内逻辑一致，
                    // 否则浏览器自己合成视频播放图像时，会显示的倒着，二是为了更高效的复制GraphicBuffer的数据（不用倒着复制了）
                    // 鸿蒙就不翻转了，因为就没有翻转的功能
                    disp->transform_type = FLIP_V;
                    glUniform1i(disp->transform_uniform, disp->transform_type);
                }
                glfwShowWindow(glfw_window);
                glfwSwapBuffers(glfw_window);
                disp->window_is_shown = true;
                sdl2_no_need = 1;
            }

            // 在窗口上绘制内容
            if (disp->display_gbuffer != NULL && disp->window_need_refresh)
            {
                if (!disp->window_is_shown)
                {
                    disp->window_is_shown = true;
                    glfwShowWindow(glfw_window);
                    sdl2_no_need = 1;
                }

                disp->window_need_refresh = false;

                opengl_paint_composer_gbuffer(disp);

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

            LOGD("screen draw avg %.2f us %.2f FPS", gen_frame_time_avg, now_screen_hz * 1000000.0f / (now_time - last_calc_time));

            sprintf(name, "vSoC:%s FPS %.1f", disp->info.name, now_screen_hz * 1000000.0f / (now_time - last_calc_time));
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
    .get_context = get_display_context,
};

EXPRESS_DEVICE_INIT(express_display, &express_gpu_info)
