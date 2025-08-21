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
// #define TIMER_LOG
#include "hw/vsoc/express_log.h"

#include "hw/vsoc/gpu/express_display.h"
#include "hw/vsoc/gpu/egl_surface.h"
#include "hw/vsoc/gpu/express_gpu.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/gpu/sdl_control.h"

#include "hw/vsoc/input/express_keyboard.h"
#include "hw/vsoc/input/express_touchscreen.h"

#include "hw/vsoc/mem/express_sync.h"

#include "sysemu/runstate.h"
#include "monitor/monitor.h"
#include <math.h>

int express_gpu_window_width;
int express_gpu_window_height;
bool express_display_headless_mode;
bool express_gpu_keep_window_scale;

static GHashTable *g_display_contexts = NULL;
static GMutex g_display_contexts_mutex;

int sdl2_no_need = 0;

static void display_context_init(Display_Context *disp);
static void display_context_destroy(Thread_Context *context);
static void window_size_change_callback(GLFWwindow *window, int width, int height);
static void close_window_callback(GLFWwindow *window);
static void opengl_paint_gbuffer(Hardware_Buffer *gbuffer);
static void handle_display_rotation(Display_Context *disp, GBuffer_Layers *layers);
static void opengl_paint_composer_layers(Display_Context *disp, GBuffer_Layers *layers);
static void display_present(Display_Context *disp);
static void display_status_change(Display_Context *disp, Display_Status status);

static bool display_call_handler(Thread_Context *context, uint64_t id, const Call_Para *para, int para_num)
{
    Display_Context *disp = (Display_Context *)context; // context already corresponds to this display
    bool ok = true;

    LOGD("display call id %llx", id);

    switch (id)
    {
    case FUNID_Get_Display_Count:
    {
        if (para[0].data_len < sizeof(uint64_t)) {
            LOGE("error! incorrect FUNID_Get_Display_Count arguments");
            ok = false; break;
        }
        uint64_t display_count = get_display_count();
        g_ops.write_to_guest_mem(para[0].data, &display_count, 0, sizeof(uint64_t));
    } break;
    case FUNID_Commit_Composer_Layer:
    {
        if (unlikely(para_num < PARA_NUM_Commit_Composer_Layer)) { ok = false; break; }
        size_t layers_size = para[0].data_len;
        if (unlikely(layers_size < sizeof(GBuffer_Layers))) { ok = false; break; }

        GBuffer_Layers *layers = g_malloc0(layers_size);
        g_ops.read_from_guest_mem(para[0].data, layers, 0, layers_size);
        if (layers->layer_num * sizeof(GBuffer_Layer) + sizeof(GBuffer_Layers) != layers_size) {
            LOGE("error! Gbuffer_Layers size mismatch num %d calc %lld actual %lld", layers->layer_num, (long long)(layers->layer_num * sizeof(GBuffer_Layer) + sizeof(GBuffer_Layers)), (long long)layers_size);
            g_free(layers); ok = false; break;
        }
        TIMER_START_ON_THREAD(compose_layer);
        if (!express_display_headless_mode) {
            handle_display_rotation(disp, layers);
        }
        opengl_paint_composer_layers(disp, layers);
        g_free(layers);
        display_present(disp);
        TIMER_END(compose_layer);
        TIMER_PRINT_MOVING(compose_layer, 100);
    } break;
    case FUNID_Show_Window:
    {
        LOGD("disp %s: Show_Window", disp->info.name);
        disp->flip_type = ROTATE_NONE;
        glUniform1i(disp->transform_uniform, disp->flip_type);
    } break;
    case FUNID_Show_Window_FLIP_V:
    {
        LOGD("disp %s: Show_Window_FLIP_V", disp->info.name);
        disp->flip_type = FLIP_V;
        glUniform1i(disp->transform_uniform, disp->flip_type);
    } break;
    case FUNID_Set_Sync_Flag:
    {
        if (unlikely(para_num < PARA_NUM_Set_Sync_Flag)) { ok = false; break; }
        if (para[0].data_len < sizeof(uint64_t)) { ok = false; break; }
        uint64_t sync_id = 0;
        g_ops.read_from_guest_mem(para[0].data, &sync_id, 0, sizeof(uint64_t));
        signal_express_sync((int)sync_id, false);
    } break;
    case FUNID_Wait_Sync:
    {
        if (unlikely(para_num < PARA_NUM_Wait_Sync)) { ok = false; break; }
        if (para[0].data_len < sizeof(uint64_t)) { ok = false; break; }
        uint64_t sync_id = 0;
        g_ops.read_from_guest_mem(para[0].data, &sync_id, 0, sizeof(uint64_t));
        wait_for_express_sync((int)sync_id, true);
    } break;
    case FUNID_Get_Display_Mods:
    {
        if (unlikely(para_num < PARA_NUM_Get_Display_Mods)) { ok = false; break; }
        if (para[0].data_len < sizeof(Display_Info)) { ok = false; break; }
        display_context_init(disp);
        g_ops.write_to_guest_mem(para[0].data, &disp->info, 0, sizeof(Display_Info));
    } break;
    case FUNID_Set_Display_Status:
    {
        if (unlikely(para_num < PARA_NUM_Set_Display_Status)) { ok = false; break; }
        if (para[0].data_len < sizeof(Display_Status)) { ok = false; break; }
        Display_Status status;
        g_ops.read_from_guest_mem(para[0].data, &status, 0, sizeof(Display_Status));
        display_status_change(disp, status);
    } break;
    case FUNID_Get_Display_Status:
    {
        if (unlikely(para_num < PARA_NUM_Get_Display_Status)) { ok = false; break; }
        if (para[0].data_len < sizeof(Display_Status)) { ok = false; break; }
        g_ops.write_to_guest_mem(para[0].data, &disp->status, 0, sizeof(Display_Status));
    } break;
    case FUNID_Snapshot_Load:
    {
        display_context_init(disp);
    } break;
    default:
    {
        LOGE("error! unknown display call id %llx para_num %d", id, para_num);
        ok = false;
    } break;
    }

    LOGD("finish one display call id %llx success %d", id, ok);
    return ok;
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
        g_mutex_lock(&g_display_contexts_mutex);
        g_hash_table_insert(g_display_contexts, GUINT_TO_POINTER(unique_id), (gpointer)context);
        g_mutex_unlock(&g_display_contexts_mutex);
    }
    return context;
}

static void display_context_init(Display_Context *disp)
{
    start_main_window_thread();

    int width, height, refresh_rate;
    get_display_info((int)disp->unique_id, &width, &height, &refresh_rate);

    sprintf(disp->info.name, "%" PRIu64, disp->unique_id);
    disp->info.pixel_width = width;
    disp->info.pixel_height = height;

    if (refresh_rate > 0 && refresh_rate <= 64 * 15 /* 15 per bit, 64 bits */ && refresh_rate % 15 == 0) {
        disp->info.refresh_rate_bits = 0x1ULL << ((refresh_rate - 15) / 15);
    }
    else {
        LOGW("invalid refresh rate setting %d, must be a multiple of 15, defaulting to 60", refresh_rate);
        refresh_rate = 60;
        disp->info.refresh_rate_bits = 0x1ULL << ((refresh_rate - 15) / 15);
    }

    disp->window_width = express_gpu_window_width;
    disp->window_height = express_gpu_window_height;

    disp->content_w = express_gpu_window_width;
    disp->content_h = express_gpu_window_height;

    // 新建一个context用于与纹理交互
    if (disp->window == NULL)
    {
        char name[64];
        sprintf(name, "vSoC:%s", disp->info.name);

        if (express_display_headless_mode) {
            disp->window = get_native_opengl_context(0);
            egl_makeCurrent(disp->window);
        } else {
            // 创建一个窗口，这个window也是context
            disp->window = get_native_opengl_context(DGL_CONTEXT_FLAG_WINDOWED_MODE_BIT);
            glfwSetWindowUserPointer(disp->window, disp);

            if (!disp->window)
            {
                LOGE("error: cannot allocate native window for virtual display %x", glfwGetError(NULL));
                return;
            }

            // window title
            glfwSetWindowTitle(disp->window, name);

            // 键盘事件
            glfwSetInputMode(disp->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetKeyCallback(disp->window, express_keyboard_handle_callback);

            // 鼠标事件
            glfwSetCursorPosCallback(disp->window, express_touchscreen_mouse_move_handle);
            glfwSetMouseButtonCallback(disp->window, express_touchscreen_mouse_click_handle);
            glfwSetScrollCallback(disp->window, express_touchscreen_mouse_scroll_handle);

#ifdef GLFW_TOUCH
            // 开启触摸屏支持
            glfwSetInputMode(disp->window, GLFW_TOUCH, GLFW_TRUE);
            glfwSetTouchCallback(disp->window, express_touchscreen_touch_handle);
#else
#warning "Touchscreen not supported! Please use GLFW from https://github.com/torkeldanielsson/glfw/tree/touch."
#endif

            // 捕获鼠标进出事件，在鼠标移动出窗口时，需要停用输入，即需要传递触摸屏release消息
            glfwSetCursorEnterCallback(disp->window, express_touchscreen_entered_handle);

            // 设置窗口大小可以自由调整
            float xscale = 1, yscale = 1;
#ifdef __APPLE__
            // macos retina screen handling
            glfwGetWindowContentScale(disp->window, &xscale, &yscale);
#endif
            glfwSetWindowSize(disp->window, disp->window_width / xscale, disp->window_height / yscale);
            set_touchscreen_window_size(disp->window, disp->window_width / xscale, disp->window_height / yscale, disp->transform_type);
            glfwSetFramebufferSizeCallback(disp->window, window_size_change_callback);
            glfwSetWindowCloseCallback(disp->window, close_window_callback);

            THREAD_CONTROL_END

            glfwMakeContextCurrent(disp->window);

            glfwSwapInterval(0);

            glfwShowWindow(disp->window);
        }
        sdl2_no_need = 1;

        if (g_ops.express_gpu_gl_debug_enable)
        {
#ifndef __APPLE__
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(d_debug_message_callback, NULL);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
        }

        display_opengl_prepare(&disp->programID, &disp->drawVAO);
        glBindVertexArray(disp->drawVAO);

        glGenFramebuffers(1, &disp->blitFBO);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        disp->transform_uniform = glGetUniformLocation(disp->programID, "transform_loc");

        // 因为这个是最终窗口，因此不需要进行深度测试与模板测试，直接贴图，只要最后的图像数据就行
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        // 开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
        glDisable(GL_BLEND);

        glDisable(GL_MULTISAMPLE);

        LOGI("display %s create %dx%d@%dhz", name, disp->info.pixel_width, disp->info.pixel_height, refresh_rate);
    }
    disp->is_open = true;
}

static void display_context_destroy(Thread_Context *context)
{
    Display_Context *disp = (Display_Context *)context;
    g_mutex_lock(&g_display_contexts_mutex);
    g_hash_table_remove(g_display_contexts, GUINT_TO_POINTER(disp->unique_id));
    g_mutex_unlock(&g_display_contexts_mutex);

    glDeleteFramebuffers(1, &disp->blitFBO);
    glDeleteVertexArrays(1, &disp->drawVAO);
    glDeleteProgram(disp->programID);

    if (disp->window != NULL) {
        if (express_display_headless_mode) {
            egl_makeCurrent(NULL);
            release_native_opengl_context(disp->window, 0);
        } else {
            glfwMakeContextCurrent(NULL);
            glfwHideWindow(disp->window);
            release_native_opengl_context(disp->window, DGL_CONTEXT_FLAG_WINDOWED_MODE_BIT);
        }
        disp->window = NULL;
    }
    LOGI("display %s terminate", disp->info.name);
}

static void handle_display_rotation(Display_Context *disp, GBuffer_Layers *layers) {
    int target_transform = ROTATE_NONE;
    for (int i = 0; i < layers->layer_num; i++) {
        GBuffer_Layer *layer = &layers->layer[i];
        int layer_transform = ROTATE_NONE;
        switch (layer->transform_type) {
            case ROTATE_90:
            case FLIP_H_ROT:
            case FLIP_V_ROT:
                layer_transform = ROTATE_90;
                break;
            case ROTATE_180:
                layer_transform = ROTATE_180;
                break;
            case ROTATE_270:
                layer_transform = ROTATE_270;
                break;
            default:
                break;
        }

        // todo: handle conflicting rotation
        // report conflicting rotation
        // if (target_transform != ROTATE_NONE && layer_transform != ROTATE_NONE && target_transform != layer_transform) {
        //     LOGW("conflicting layer rotation %d vs %d", target_transform, layer_transform);
        //     target_transform = ROTATE_NONE;
        //     break;
        // }

        // if any layer is rotated, treat the display as rotated
        if (layer_transform != ROTATE_NONE) {
            target_transform = layer_transform;
        }
    }

    if (disp->transform_type != target_transform) {
        LOGI("display %s rotation change %d -> %d", disp->info.name, disp->transform_type, target_transform);

        bool prev_rotated = disp->transform_type == ROTATE_90 || disp->transform_type == ROTATE_270;
        bool current_rotated = target_transform == ROTATE_90 || target_transform == ROTATE_270;

        disp->transform_type = target_transform;

        if (prev_rotated == current_rotated) {
            glfwSetWindowSize(disp->window, disp->window_width, disp->window_height);
        }
        else {
            glfwSetWindowSize(disp->window, disp->window_height, disp->window_width);
        }
    }
}

static void opengl_paint_composer_layers(Display_Context *disp, GBuffer_Layers *layers)
{
    if (layers == NULL || layers->layer_num <= 0)
    {
        return;
    }

    if (!disp->is_open)
    {
        LOGE("display %s is not open, cannot paint layers", disp->info.name);

        for (int i = 0; i < layers->layer_num; i++)
        {
            signal_express_sync(layers->layer[i].read_sync_id, false);
        }
        return;
    }

    for (int i = 0; i < layers->layer_num; i++)
    {
        GBuffer_Layer layer = layers->layer[i];

        LOGD("composer wait for write sync gbuffer %" PRIx64 " sync %d", layer.gbuffer_id, layer.write_sync_id);

        wait_for_express_sync(layer.write_sync_id, true);

        Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(layer.gbuffer_id);
        if (gbuffer != NULL)
        {
            if (disp->transform_type == ROTATE_90 || disp->transform_type == ROTATE_270) {
                // layer xywh 是旋转前的，但layer crop是旋转后的，所以要调整一下
                swap(layer.x, layer.y, int);
                swap(layer.width, layer.height, int);
            }

            // layer的大小是显示的像素区域位置大小（与屏幕大小直接相关），
            // crop的大小是原始gbuffer裁剪后的像素位置大小（与屏幕大小无关，而与原始缓冲区大小有关），
            // 两者间可能存在缩放关系
            // 这里计算得到的是，在缩放正确的情况下，原始的整个gbuffer绘制到当前界面的位置
            int view_w = gbuffer->width * layer.width / layer.crop_width;
            int view_h = gbuffer->height * layer.height / layer.crop_height;
            int view_x = layer.x - layer.crop_x * layer.width / layer.crop_width;
            int view_y = layer.y - layer.crop_y * layer.height / layer.crop_height; // 按缩放计算原始gbuffer左上角位置（按窗口上方为坐标零点）

            if (disp->transform_type == FLIP_V && layer.transform_type == FLIP_V) {
                // 安卓9下FLIP_V处理
                // guest设置了上下翻转时，crop坐标也是翻转后的，
                // 这里需要将crop的y反转，恢复成原始图像左上角的坐标
                view_y = layer.y - (gbuffer->height - layer.crop_height - layer.crop_y) * layer.height / layer.crop_height;
            }
            else if (disp->transform_type == ROTATE_90 || disp->transform_type == ROTATE_270) {
                // 转换成以窗口下方为零点的位置
                view_y = disp->info.pixel_width - view_y - view_h;
            }
            else {
                view_y = disp->info.pixel_height - view_y - view_h;
            }

            LOGD("non-scaled view %d %d %d %d", view_x, view_y, view_w, view_h);

            float xscale, yscale;
            if (disp->transform_type == ROTATE_90 || disp->transform_type == ROTATE_270) {
                xscale = (float)disp->content_w / disp->info.pixel_height;
                yscale = (float)disp->content_h / disp->info.pixel_width;
            }
            else {
                xscale = (float)disp->content_w / disp->info.pixel_width;
                yscale = (float)disp->content_h / disp->info.pixel_height;
            }
            view_x = round(view_x * xscale);
            view_y = round(view_y * yscale);
            view_w = round(view_w * xscale);
            view_h = round(view_h * yscale);

            LOGD("draw layer %d xywh %d %d %d %d crop %d %d %d %d gbuffer id %llx texture %d size %d %d blend_type %d transform_type %d",
                            i, layer.x, layer.y, layer.width, layer.height, layer.crop_x, layer.crop_y, layer.crop_width, layer.crop_height, layer.gbuffer_id, gbuffer->data_texture, gbuffer->width, gbuffer->height, layer.blend_type, layer.transform_type);

            LOGD("content xywh %d %d %d %d glviewport %d %d %d %d dispT %d layerT %d", disp->content_x, disp->content_y, disp->content_w, disp->content_h, view_x, view_y, view_w, view_h, disp->transform_type, layer.transform_type);

            if (layer.blend_type == BLEND_DST) {
                // Keeps only the destination (old image), new image is ignored
            } else if (layer.blend_type == BLEND_NONE || layer.blend_type == BLEND_SRC) {
                float sx = (float)disp->content_w / disp->info.pixel_width;
                float sy = (float)disp->content_h / disp->info.pixel_height;

                int dstX0 = disp->content_x + (int)lround(layer.x * sx);
                int invY  = disp->info.pixel_height - (layer.y + layer.height);
                int dstY0 = disp->content_y + (int)lround(invY * sy);
                int dstX1 = dstX0 + (int)lround(layer.width * sx);
                int dstY1 = dstY0 + (int)lround(layer.height * sy);

                int srcX0 = layer.crop_x;
                int srcX1 = layer.crop_x + layer.crop_width;
                int srcY0 = gbuffer->height - (layer.crop_y + layer.crop_height);
                int srcY1 = gbuffer->height - layer.crop_y;

                // quick path if no blending takes place
                glBindFramebuffer(GL_READ_FRAMEBUFFER, disp->blitFBO);
                glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->data_texture, 0);

                glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_NEAREST);

                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            } else {
                glViewport(view_x, view_y, view_w, view_h);

                // glScissor是当前视口的裁剪情况，整个裁剪是说这个区域外就不绘制了，但是空间还是占着
                // 而合成器的crop裁剪，是直接区域裁掉，所占的区域就没了
                // 简单的说，从效果上来看，合成器的裁剪是把原来的图片给剪了一下，变小了后再缩放贴到屏幕缓冲区的相应位置
                // 而glScissor，是原来的图片整个都贴到缓冲区的相应位置，但是屏幕缓冲区所指定的区域之外的地方用东西给盖住（其实是不绘制，而不是盖住）
                if (disp->transform_type == FLIP_V)
                {
                    glScissor(layer.x, layer.y, layer.width, layer.height);
                }
                else if (disp->transform_type == ROTATE_NONE)
                {
                    glScissor(layer.x, disp->info.pixel_height - layer.y - layer.height, layer.width, layer.height);
                }
                else if (disp->transform_type == ROTATE_90 || disp->transform_type == ROTATE_270)
                {
                    glScissor(layer.x, disp->info.pixel_width - layer.y - layer.height, layer.width, layer.height);
                }
    
                adjust_blend_type(layer.blend_type);
                opengl_paint_gbuffer(gbuffer);
            }
        }
        else {
            LOGW("display %s: cannot find layer gbuffer %llx", disp->info.name, layer.gbuffer_id);
        }
        LOGD("composer set sync %d", layer.read_sync_id);
        signal_express_sync(layer.read_sync_id, true);
    }
}

static void display_present(Display_Context *disp)
{
    if (!express_display_headless_mode) {
        glfwSwapBuffers(disp->window);
    }

    uint64_t now_time = g_get_monotonic_time();
    char name[64];
    disp->fps_counter++;

    if (now_time - disp->last_fps_timestamp > 1000 * 1000)
    {
        float gen_frame_time_avg = 1.0f * (now_time - disp->last_fps_timestamp) / disp->fps_counter / 1000.0f;
        float fps = disp->fps_counter * 1000000.0f / (now_time - disp->last_fps_timestamp);
        disp->last_fps = fps;
        LOGD("display %s: composer draw avg %.2f ms %.2f FPS", disp->info.name, gen_frame_time_avg, fps);
        sprintf(name, "vSoC:%s FPS %.1f", disp->info.name, fps);
        if (!express_display_headless_mode) {
            glfwSetWindowTitle(disp->window, name);
        }

        disp->last_fps_timestamp = now_time;
        disp->fps_counter = 0;
    }
}

static void display_status_change(Display_Context *disp, Display_Status status)
{
    LOGI("display_status_change refresh_rate %d=>%d power_stats %d=>%d backlight %u=>%u",
           disp->status.refresh_rate, status.refresh_rate, disp->status.power_status, status.power_status,
           disp->status.backlight, status.backlight);

    disp->status = status;
    // if (disp->status.power_status == 3)
    // {
    //     disp->is_open = false;
    // }
    // else
    // {
    //     disp->is_open = true;
    // }
}

/**
 * @brief 界面上用于画出图像的函数，实际逻辑为取出gbuffer中的display_texture，然后画出来
 *
 * @param gbuffer
 */
static void opengl_paint_gbuffer(Hardware_Buffer *gbuffer)
{
    if (gbuffer->is_writing != 0)
    {
        LOGE("error! get writing gbuffer when opengl_paint");
    }

    LOGD("draw gbuffer_id %llx data sync %lld", gbuffer->gbuffer_id, (uint64_t)gbuffer->data_sync);

    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void window_size_change_callback(GLFWwindow *window, int width, int height)
{
    Display_Context *disp = (Display_Context *)glfwGetWindowUserPointer(window);

    if (disp->window_width == width && disp->window_height == height)
        return;

    // macos retina screen handling
    float xscale = 1, yscale = 1;
#ifdef __APPLE__
    glfwGetWindowContentScale(window, &xscale, &yscale);
#endif

    disp->window_width = width;
    disp->window_height = height;

    int temp_window_width = disp->window_width;
    int temp_window_height = disp->window_height;
    int x = 0;
    int y = 0;

    // 需要保证画面比例不变
    double target_ratio = (double)disp->info.pixel_width / disp->info.pixel_height;
    double current_ratio = (double)disp->window_width / disp->window_height;
    if (disp->transform_type == ROTATE_90 || disp->transform_type == ROTATE_270)
    {
        target_ratio = (double)disp->info.pixel_height / disp->info.pixel_width;
    }

    if (target_ratio > current_ratio + 0.001)
    {
        temp_window_height = (int)(disp->window_width / target_ratio);
        y = (disp->window_height - temp_window_height) / 2;
    }
    else if (target_ratio < current_ratio - 0.001)
    {
        temp_window_width = (int)(target_ratio * disp->window_height);
        x = (disp->window_width - temp_window_width) / 2;
    }

    if (express_gpu_keep_window_scale)
    {
        //printf("set window size %d %d  %d %d %d %d %d %dkeep scale\n", window_width, window_height,temp_window_width,temp_window_height,disp->info.pixel_width,disp->info.pixel_height,x,y);
        disp->window_width = temp_window_width;
        disp->window_height = temp_window_height;

        disp->content_x = 0;
        disp->content_y = 0;
        disp->content_w = disp->window_width;
        disp->content_h = disp->window_height;

        glfwSetWindowSize(window, disp->window_width / xscale, disp->window_height / yscale);
        glViewport(0, 0, disp->window_width, disp->window_height);
    }
    else
    {
        glViewport(x, y, temp_window_width, temp_window_height);

        disp->content_x = x;
        disp->content_y = y;
        disp->content_w = temp_window_width;
        disp->content_h = temp_window_height;
    }

    LOGD("set window size (%d %d) -> (%d %d) ratio %.2f -> %.2f info wh %d %d", width, height, disp->window_width, disp->window_height, current_ratio, target_ratio, disp->info.pixel_width, disp->info.pixel_height);
    set_touchscreen_window_size(disp->window, disp->window_width / xscale, disp->window_height / yscale, disp->transform_type);

    return;
}

static void close_window_callback(GLFWwindow *window)
{
    // todo: handle display hotplug event
    static gint64 last_click_time = 0;
    gint64 now_time = g_get_monotonic_time();

    glfwSetWindowShouldClose(window, GLFW_FALSE);
    if (now_time - last_click_time < 500000)
    {
        g_ops.notify_shutdown(SHUTDOWN_CAUSE_HOST_UI);
    }
    else
    {
        g_ops.force_shutdown();
    }
    last_click_time = now_time;
}

#ifdef ENABLE_SNAPSHOT

void save_display_context(QEMUFile *f) {
    LOGI("in save_display_context");

    if (g_display_contexts == NULL) {
        qemu_put_be32(f, 0);
        return;
    }

    // get and save total displays
    int display_count = (int)g_hash_table_size(g_display_contexts);
    qemu_put_be32(f, display_count);

    // iter through all display contexts and save them
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, g_display_contexts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Display_Context *disp = (Display_Context *)value;
        qemu_put_buffer(f, (uint8_t *)disp, sizeof(Display_Context));
    }
}

void load_display_context(QEMUFile *f) {
    LOGI("in load_display_context");

    // copy old display contexts from g_display_contexts to a new g_array
    GArray *old_displays = g_array_new(FALSE, FALSE, sizeof(Display_Context *));
    if (g_display_contexts && g_hash_table_size(g_display_contexts) > 0) {
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, g_display_contexts);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            Display_Context *disp = (Display_Context *)value;
            g_array_append_val(old_displays, disp);
            LOGI("terminating old display key %lld id %d name %s", key, disp->unique_id, disp->info.name);
        }
    }

    // iter through the array and terminate all threads
    for (int i = 0; i < old_displays->len; i++) {
        Display_Context *disp = g_array_index(old_displays, Display_Context *, i);

        push_local_call_to_thread(&disp->thread_context, FUNID_Terminate);
    }

    // wait for all threads to terminate
    while (g_display_contexts && g_hash_table_size(g_display_contexts) > 0) {
        LOGI("waiting for %d old displays to terminate...", g_hash_table_size(g_display_contexts));
        g_usleep(10000);
    }

    g_array_free(old_displays, TRUE);

    // get total displays
    int display_count = qemu_get_be32(f);

    // load all display contexts
    Express_Device_Info *display_device_info = get_express_device_info(EXPRESS_DISPLAY_DEVICE_ID);

    for (int i = 0; i < display_count; i++) {
        Display_Context *old_disp = g_malloc0(sizeof(Display_Context));
        qemu_get_buffer(f, (uint8_t *)old_disp, sizeof(Display_Context));
        LOGI("recovering display id %d name vSoC:%s (%dx%d) window %p", old_disp->unique_id, old_disp->info.name, old_disp->info.pixel_width, old_disp->info.pixel_height, old_disp->window);

        Display_Context *new_disp = (Display_Context *)display_device_info->get_context(EXPRESS_DISPLAY_DEVICE_ID, old_disp->thread_context.thread_id, old_disp->thread_context.process_id, old_disp->unique_id, display_device_info);

        // copy all the members of the Display_Context struct except the thread_context
        int offset = sizeof(Thread_Context);
        memcpy((uint8_t *)new_disp + offset, (uint8_t *)old_disp + offset, sizeof(Display_Context) - offset);

        // these resources needs to be re-created
        new_disp->window = NULL;
        new_disp->programID = 0;
        new_disp->drawVAO = 0;
        new_disp->last_fps_timestamp = 0;
        new_disp->fps_counter = 0;

        if (old_disp->window != NULL) {
            push_local_call_to_thread(&new_disp->thread_context, FUNID_Snapshot_Load);
        }
        g_free(old_disp);
    }
}

#endif

static void init_display_options(void) {
    int count = get_display_count();

    touchscreen_prop.count = count;
    get_display_info(0, &touchscreen_prop.width, &touchscreen_prop.height, NULL);

    express_keyboard_count = count;
}

static void display_hmp_handler(Monitor *mon, int argc, const char **argv) {
    if (argc < 1) {
        MONITOR_LOG(mon, "Usage: display <command> [args]\n");
        MONITOR_LOG(mon, "Available commands:\n");
        MONITOR_LOG(mon, "  count - Get the number of displays\n");
        MONITOR_LOG(mon, "  fps - Get the FPS of all displays\n");
        return;
    }

    if (strcmp(argv[0], "count") == 0) {
        int count = g_hash_table_size(g_display_contexts);
        MONITOR_LOG(mon, "%d\n", count);
    }
    else if (strcmp(argv[0], "fps") == 0) {
        if (g_display_contexts != NULL) {
            GHashTableIter iter;
            gpointer key, value;
            g_hash_table_iter_init(&iter, g_display_contexts);
            while (g_hash_table_iter_next(&iter, &key, &value)) {
                Display_Context *disp = (Display_Context *)value;
                MONITOR_LOG(mon, "%.2f ", disp->last_fps);
            }
        }
        MONITOR_LOG(mon, "\n");
    }
    else {
        MONITOR_LOG(mon, "Unknown display command: %s\n", argv[0]);
    }
}

void handle_display_event(void) {
    if (express_display_headless_mode || !g_display_contexts) {
        return;
    }

    // throttle input updates to screen refresh
    static uint64_t last_event_time = 0;
    uint64_t current_time = g_get_monotonic_time();
    if (current_time - last_event_time < 1000 * 1000 / g_ops.express_display_refresh_rate) {
        return;
    }
    last_event_time = current_time;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, g_display_contexts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Display_Context *disp = (Display_Context *)value;
        if (disp->is_open) {
            sync_express_touchscreen_input(disp->window, true);
            sync_express_keyboard_input(disp->window, true);
        }
        if (current_time - disp->last_fps_timestamp > 1000 * 1000) {
            disp->last_fps = disp->fps_counter;
            disp->fps_counter = 0;
            disp->last_fps_timestamp = current_time;
        }
    }
}

static Express_Device_Info express_display_info = {
    .enable_default = true,
    .name = "express-display",
    .option_name = "display",
    .device_id = EXPRESS_DISPLAY_DEVICE_ID,
    .device_type = OUTPUT_DEVICE_TYPE,
    .init = init_display_options,
    .call_handler = display_call_handler,
    .get_context = get_display_context,
    .context_destroy = display_context_destroy,
    .hmp_handler = display_hmp_handler,
};

EXPRESS_DEVICE_INIT(express_display, &express_display_info)
