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

#define STD_DEBUG_LOG
#include "hw/teleport-express/express_device_common.h"

#include "hw/express-gpu/express_display.h"

#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/express_gpu.h"
#include "hw/express-gpu/express_gpu_render.h"

#include "hw/express-gpu/egl_surface.h"

#include "hw/express-gpu/express_sync.h"

#include "qemu/atomic.h"

static Thread_Context *static_display_context = NULL;

static void *native_display_context = NULL;

static GLuint un_pack_buffer;
static int un_pack_buffer_size = 0;

static GLsync unpack_buffer_sync = NULL;

static Display_Status now_display_status;

static GLuint programID = 0;
static GLuint drawVAO = 0;

static GLint program_transform_loc = 0;
static GLuint now_transform_type = 0;

static Graphic_Buffer *display_write_gbuffer;
static Graphic_Buffer *display_read_gbuffer;

Display_Info express_display_info = {
    .pixel_width = 1280,
    .pixel_height = 720,
    .phy_width = 1280,
    .phy_height = 720,
    .refresh_rate_bits = 0x4LL,
};

int *express_display_pixel_width = &(express_display_info.pixel_width);
int *express_display_pixel_height = &(express_display_info.pixel_height);
int *express_display_phy_width = &(express_display_info.phy_width);
int *express_display_phy_height = &(express_display_info.phy_height);

int display_is_open = 1;

bool express_display_switch_open = false;

static void opengl_paint_composer_layers(GBuffer_Layers *layers);
static void display_present(void);

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
        printf("display terminate\n");
    }
    break;
    case FUNID_Terminate_Gbuffer:
    {
        Gralloc_Gbuffer_Info info;

        if (unlikely(para_num < PARA_NUM_Terminate_Gbuffer))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(Gralloc_Gbuffer_Info)))
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

        info = *(Gralloc_Gbuffer_Info *)(temp);

        Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(info.gbuffer_id);
        if (gbuffer != NULL)
        {
            if (gbuffer->is_dying == 1)
            {
                gbuffer->remain_life_time = 3;
            }
            else
            {
                remove_gbuffer_from_global_map(info.gbuffer_id);
                // set_global_gbuffer_type(gbuffer->gbuffer_id, GBUFFER_TYPE_NONE);
                destroy_gbuffer(gbuffer);
            }
            printf("terminate gbuffer id %llx\n", info.gbuffer_id);
        }
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
            printf(RED("error! Gbuffer_Layers' size is not equal to data size num %d calc size %lld layers_size %lld\n"), layers->layer_num, layers->layer_num * sizeof(GBuffer_Layer) + sizeof(GBuffer_Layers), layers_size);
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
        printf("force_show_native_render_window\n");
        force_show_native_render_window = 1;
    }
    break;
    case FUNID_Show_Window_FLIP_V:
    {
        printf("force_show_native_render_window-filp_v\n");
        force_show_native_render_window = 2;
    }
    break;
    case FUNID_Gbuffer_Download:
    {
        Gralloc_Gbuffer_Info info;

        if (unlikely(para_num < PARA_NUM_Gbuffer_Download))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(Gralloc_Gbuffer_Info)))
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

        info = *(Gralloc_Gbuffer_Info *)(temp);

        guest_download_gbuffer_data(info);
    }
    break;
    case FUNID_Gbuffer_Upload:
    {
        Gralloc_Gbuffer_Info info;

        if (unlikely(para_num < PARA_NUM_Gbuffer_Upload))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(Gralloc_Gbuffer_Info)))
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

        info = *(Gralloc_Gbuffer_Info *)(temp);

        guest_upload_gbuffer_data(info);
    }
    break;
    case FUNID_Alloc_Gbuffer:
    {
        Gralloc_Gbuffer_Info info;

        if (unlikely(para_num < PARA_NUM_Alloc_Gbuffer))
        {
            break;
        }

        temp_len = all_para[0].data_len;
        if (unlikely(temp_len < sizeof(Gralloc_Gbuffer_Info)))
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

        info = *(Gralloc_Gbuffer_Info *)(temp);

        Guest_Mem *gbuffer_data = copy_guest_mem_from_call(call, 2);

        alloc_gbuffer_with_gralloc(info, gbuffer_data);
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

        set_express_sync_id((int)sync_id, true);
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
        // printf("FUNID_Get_Display_Mods\n");
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
        // printf("FUNID_Set_Display_Status\n");
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
        printf("error! unknown display invoke id %llx para_num %d\n", call->id, para_num);
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
    }

    return static_display_context;
}

static void display_context_init(Thread_Context *context)
{

    express_printf("display context init!\n");
    // 这个render线程只能创建一次，且其他线程必须等待该线程运行成功
    if (qatomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        express_printf("create native window\n");
        qemu_thread_create(&native_window_render_thread, "handle_thread", native_window_thread, context->teleport_express_device, QEMU_THREAD_DETACHED);
        init_display(&default_egl_display);
    }

    if (native_render_run == 1)
    {
        do
        {
            g_usleep(5000);
        } while (native_render_run != 2);
    }

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
                printf("wait for native_display_context creating too long!\n");
            }
        }

        egl_makeCurrent(native_display_context);

        glGenBuffers(1, &un_pack_buffer);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, un_pack_buffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, un_pack_buffer);

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

        main_window_opengl_prepare(&programID, &drawVAO);
        glBindVertexArray(drawVAO);

        program_transform_loc = glGetUniformLocation(programID, "transform_loc");
        now_transform_type = 0;

        glEnable(GL_SCISSOR_TEST);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        // 开启透明度混合后，默认不开透明度的线程的绘制结果对应的texture的透明度默认为0，叠加上去后会导致透明，看不到东西
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
    }
}

static void display_context_destroy(Thread_Context *context)
{
    if (native_display_context != NULL)
    {
        glDeleteBuffers(1, &un_pack_buffer);

        egl_makeCurrent(NULL);
        egl_destroyContext(native_display_context);
        native_display_context = NULL;
    }
}

static void opengl_paint_composer_layers(GBuffer_Layers *layers)
{
    int display_height = express_display_info.pixel_height;
    // int display_width = express_display_info.pixel_width;

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

            wait_for_express_sync(layer.write_sync_id, true);

            Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(layer.gbuffer_id);
            if (gbuffer != NULL)
            {
                express_printf("draw layer gbuffer_id %llx  %d %d %d %d gbuffer_size %d %d blend_type %d transform_type %d\n",
                       layer.gbuffer_id, layer.x, layer.y, layer.width, layer.height, gbuffer->width, gbuffer->height, layer.blend_type, layer.transform_type);
                // layer的大小是显示的像素区域位置大小（与屏幕大小直接相关），
                // crop的大小是原始gbuffer裁剪后的像素位置大小（与屏幕大小无关，而与原始缓冲区大小有关），
                // 两者间可能存在缩放关系
                // 这里计算得到的是，在缩放正确的情况下，原始的整个gbuffer绘制到当前界面的位置
                int view_w = gbuffer->width * layer.width / layer.crop_width;
                int view_h = gbuffer->height * layer.height / layer.crop_height;
                int view_x = layer.x - layer.crop_x * layer.width / layer.crop_width;
                int view_y = 0;
                if (force_show_native_render_window == 2)
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
                else if (force_show_native_render_window == 1)
                {
                    // 先进行缩放，计算原始gbuffer的左上角应该在哪（以窗口上面为y轴零点）
                    view_y = layer.y - layer.crop_y * layer.height / layer.crop_height;
                    // 然后计算gbuffer的左下角应该在哪（以窗口下面为y轴零点）
                    view_y = display_height - view_y - view_h;
                }

                express_printf("glviewport %d %d %d %d glScissor %d %d %d %d\n", view_x, view_y, view_w, view_h, layer.x, display_height - layer.y - layer.height, layer.width, layer.height);
                express_printf("layer %d %d %d %d crop %d %d %d %d\n", layer.x, layer.y, layer.width, layer.height, layer.crop_x, layer.crop_y, layer.crop_width, layer.crop_height);
                glViewport(view_x, view_y, view_w, view_h);

                // glScissor是当前视口的裁剪情况，整个裁剪是说这个区域外就不绘制了，但是空间还是占着
                // 而合成器的crop裁剪，是直接区域裁掉，所占的区域就没了
                // 简单的说，从效果上来看，合成器的裁剪是把原来的图片给剪了一下，变小了后再缩放贴到屏幕缓冲区的相应位置
                // 而glScissor，是原来的图片整个都贴到缓冲区的相应位置，但是屏幕缓冲区所指定的区域之外的地方用东西给盖住（其实是不绘制，而不是盖住）
                if (force_show_native_render_window == 2)
                {
                    glScissor(layer.x, layer.y, layer.width, layer.height);
                }
                else if (force_show_native_render_window == 1)
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
                        printf("error! not support transform_type %d\n", now_transform_type);
                    }
                    glUniform1i(program_transform_loc, now_transform_type);
                }

                opengl_paint_gbuffer(gbuffer);

                express_printf("composer set sync %d\n", layer.read_sync_id);

                set_express_sync_id(layer.read_sync_id, true);
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
    Graphic_Buffer *temp_gbuffer = display_read_gbuffer;

    display_read_gbuffer = display_write_gbuffer;

    display_write_gbuffer = temp_gbuffer;

    glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);

    // 保证主线程使用完成上一个gbuffer
    ATOMIC_LOCK(display_write_gbuffer->is_lock);

    main_display_gbuffer = display_read_gbuffer;

    // 这里直接unlock，不需要一直锁住，是因为只有这个画完了之后，才会赋值到main_display_gbuffer，所以不会被主线程访问到，也就不需要锁住
    // 无论主线程之后会不会继续读取，这里都要直接进行后续的绘制
    ATOMIC_UNLOCK(display_write_gbuffer->is_lock);

    // int64_t now_time = g_get_real_time();
    // static int64_t last_display_time = 0;
    // printf("display_present %llx time %lld gap %lld\n", (int64_t)main_display_gbuffer, now_time/1000, (now_time - last_display_time)/1000);
    // last_display_time = now_time;

    static int now_screen_hz = 0;
    static uint64_t last_record_time = 0;
    uint64_t now_time = g_get_real_time();
    now_screen_hz++;

    if (now_time - last_record_time > 1000000)
    {
        float gen_frame_time_avg = 1.0f * (now_time - last_record_time) / now_screen_hz;
        printf("composer draw avg %.2f us %.2f FPS\n", gen_frame_time_avg, now_screen_hz * 1000000.0f / (now_time - last_record_time));
        last_record_time = now_time;
        now_screen_hz = 0;
    }
}

void display_status_change(Display_Status status)
{
    printf("display_status_change refresh_rate %d=>%d power_stats %d=>%d backlight %u=>%u\n",
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

void alloc_gbuffer_with_gralloc(Gralloc_Gbuffer_Info info, Guest_Mem *mem_data)
{
    Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(info.gbuffer_id);

    if (info.width == 0 || info.height == 0 || info.size == 0)
    {
        return;
    }

    if (gbuffer == NULL)
    {
        gbuffer = create_gbuffer_from_gralloc_info(info, info.gbuffer_id);
        add_gbuffer_to_global(gbuffer);
        // set_global_gbuffer_type(info.gbuffer_id, GBUFFER_TYPE_NATIVE);
        gbuffer->usage = info.usage;
        gbuffer->pixel_size = info.pixel_size;
        gbuffer->size = info.size;
        gbuffer->stride = info.stride;
        gbuffer->guest_data = mem_data;
        express_printf("alloc gbuffer size %d mem len %d\n", gbuffer->size, mem_data->all_len);
    }
    else
    {
        printf("error! alloc_gbuffer_with_gralloc get no-null gbuffer origin %d %d new %d %d\n", gbuffer->width, gbuffer->height, info.width, info.height);
        free_copied_guest_mem(mem_data);
    }

    if (info.width != gbuffer->width || info.height != gbuffer->height || info.stride != gbuffer->stride || info.pixel_size != gbuffer->pixel_size)
    {
        printf("error! alloc_gbuffer_with_gralloc gbuffer data size error width height stride pixel_size %d %d %d %d origin %d %d %d %d", info.width, info.height, info.stride, info.pixel_size, gbuffer->width, gbuffer->height, gbuffer->stride, gbuffer->pixel_size);
        return;
    }
}

void guest_upload_gbuffer_data(Gralloc_Gbuffer_Info info)
{
    Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(info.gbuffer_id);

    if (gbuffer == NULL)
    {
        printf("error! guest_upload_gbuffer_data get null gbuffer\n");
        return;
    }

    if (info.width != gbuffer->width || info.height != gbuffer->height || info.stride != gbuffer->stride || info.pixel_size != gbuffer->pixel_size)
    {
        printf("error! guest_upload_gbuffer_data gbuffer data size error width height stride pixel_size %d %d %d %d origin %d %d %d %d", info.width, info.height, info.stride, info.pixel_size, gbuffer->width, gbuffer->height, gbuffer->stride, gbuffer->pixel_size);
        return;
    }

    Guest_Mem *mem_data = gbuffer->guest_data;

    int real_width = info.width;
    if (real_width % (info.stride) != 0)
    {
        real_width = (real_width / info.stride + 1) * info.stride;
    }

    int row_byte_len = info.pixel_size * info.width;

    int all_pixel_size = row_byte_len * info.height;

    // printf("GraphicBuffer data width %d height %d row_byte_len %d guest_row_byte_len %d\n", egl_image->width, egl_image->height, row_byte_len, guest_row_byte_len);

    if (all_pixel_size > mem_data->all_len)
    {
        printf("error! guest_upload_gbuffer_data len error! row %d height %d get len %d\n", row_byte_len, info.height, mem_data->all_len);
        return;
    }

    // 因为通过map上传的过程为异步的，所以这里假如fence未完成的话，需要重新bufferdata，以实现缓冲区孤立，避免同步（即避免需要同步等待gl用完这个缓冲区)
    if (un_pack_buffer_size < all_pixel_size)
    {
        un_pack_buffer_size = all_pixel_size;
        glBufferData(GL_PIXEL_UNPACK_BUFFER, un_pack_buffer_size, NULL, GL_STREAM_DRAW);
        printf("glBufferData new gbuffer size %d\n", un_pack_buffer_size);
    }
    else
    {
        GLint sync_status = GL_SIGNALED;
        GLsizei sync_status_len;
        if (unpack_buffer_sync != NULL)
        {
            glGetSynciv(unpack_buffer_sync, GL_SYNC_STATUS, sizeof(GLint), &sync_status_len, &sync_status);
        }

        if (sync_status == GL_UNSIGNALED)
        {
            glBufferData(GL_PIXEL_UNPACK_BUFFER, un_pack_buffer_size, NULL, GL_STREAM_DRAW);
            // GLenum ret = glClientWaitSync(unpack_buffer_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
            // glGetSynciv(unpack_buffer_sync, GL_SYNC_STATUS, sizeof(GLint), &sync_status_len, &sync_status);
            express_printf("glBufferData no sync new gbuffer size %d\n", un_pack_buffer_size);
        }
    }

    if (unpack_buffer_sync != NULL)
    {
        glDeleteSync(unpack_buffer_sync);
        unpack_buffer_sync = NULL;
    }

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, all_pixel_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    printf("guest_upload_gbuffer_data id %llx width %d height %d internal_format %x format %x row_byte_len %d buf_len %d\n",
           gbuffer->gbuffer_id, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->format, row_byte_len, mem_data->all_len);

    // GraphicBuffer里的图片是正的，放到纹理里要倒个个
    // -- 不用倒个了，因为合成的时候，普通窗口都进行了倒个，然后显示的时候，又进行了倒个
    if (info.stride != row_byte_len)
    {
        for (int i = 0; i < info.height; i++)
        {
            // read_from_guest_mem(mem_data, map_pointer + (info.height - i - 1) * row_byte_len, i * info.stride, row_byte_len);
            read_from_guest_mem(mem_data, map_pointer + i * row_byte_len, i * info.stride, row_byte_len);
        }
    }
    else
    {
        read_from_guest_mem(mem_data, map_pointer, 0, all_pixel_size);
        // for (int i = 0; i < info.height; i++)
        // {
        //     read_from_guest_mem(mem_data, map_pointer + (info.height - i - 1) * row_byte_len, i * row_byte_len, row_byte_len);
        // }
    }

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

    // 这时候是立即返回的，后续会进行dma传输
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gbuffer->width, gbuffer->height, gbuffer->format, gbuffer->pixel_type, NULL);

    unpack_buffer_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    // if (gbuffer->data_sync != NULL)
    // {
    //     if (gbuffer->delete_sync != NULL)
    //     {
    //         glDeleteSync(gbuffer->delete_sync);
    //     }

    //     gbuffer->delete_sync = gbuffer->data_sync;
    //     gbuffer->data_sync = NULL;
    // }

    // gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    glFlush();
}

void guest_download_gbuffer_data(Gralloc_Gbuffer_Info info)
{
    Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(info.gbuffer_id);

    if (gbuffer == NULL)
    {
        printf("error! guest_download_gbuffer_data get null gbuffer\n");
        return;
    }

    if (info.width != gbuffer->width || info.height != gbuffer->height || info.stride != gbuffer->stride || info.pixel_size != gbuffer->pixel_size)
    {
        printf("error! guest download gbuffer data size error width height stride pixel_size %d %d %d %d origin %d %d %d %d",
               info.width, info.height, info.stride, info.pixel_size, gbuffer->width, gbuffer->height, gbuffer->stride, gbuffer->pixel_size);
        return;
    }

    Guest_Mem *mem_data = gbuffer->guest_data;

    int real_width = info.width;
    if (real_width % (info.stride) != 0)
    {
        real_width = (real_width / info.stride + 1) * info.stride;
    }

    int row_byte_len = info.pixel_size * info.width;

    int all_pixel_size = row_byte_len * info.height;

    // printf("GraphicBuffer data width %d height %d row_byte_len %d guest_row_byte_len %d\n", egl_image->width, egl_image->height, row_byte_len, guest_row_byte_len);

    if (all_pixel_size > mem_data->all_len)
    {
        printf("error! guest_download_gbuffer_data len error! row %d height %d get len %d\n", row_byte_len, info.height, mem_data->all_len);
        return;
    }

    if (un_pack_buffer_size < all_pixel_size)
    {
        un_pack_buffer_size = all_pixel_size;
        glBufferData(GL_PIXEL_PACK_BUFFER, un_pack_buffer_size, NULL, GL_STREAM_DRAW);
    }
    else
    {
        GLint sync_status = GL_SIGNALED;
        GLsizei sync_status_len;
        if (unpack_buffer_sync != NULL)
        {
            glGetSynciv(unpack_buffer_sync, GL_SYNC_STATUS, sizeof(GLint), &sync_status_len, &sync_status);
        }

        if (sync_status == GL_UNSIGNALED)
        {
            glBufferData(GL_PIXEL_PACK_BUFFER, un_pack_buffer_size, NULL, GL_STREAM_DRAW);
        }
    }

    if (unpack_buffer_sync != NULL)
    {
        glDeleteSync(unpack_buffer_sync);
        unpack_buffer_sync = NULL;
    }

    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

    glGetTexImage(GL_TEXTURE_2D, 0, gbuffer->format, gbuffer->pixel_type, 0);

    GLint error = glGetError();
    if (error != 0)
    {
        printf("error %x when d_glReadGraphicBuffer width %d height %d internal_format %x format %x row_byte_len %d buf_len %d\n",
               error, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->format, row_byte_len, mem_data->all_len);
    }

    printf("guest_download_gbuffer_data id %llx width %d height %d internal_format %x format %x row_byte_len %d buf_len %d\n",
           gbuffer->gbuffer_id, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->format, row_byte_len, mem_data->all_len);

    GLubyte *map_pointer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, all_pixel_size, GL_MAP_READ_BIT);

    if (info.stride != row_byte_len)
    {
        for (int i = 0; i < info.height; i++)
        {
            // read_from_guest_mem(guest_mem, map_pointer + (height - i - 1) * row_byte_len, i * guest_row_byte_len, row_byte_len);
            write_to_guest_mem(mem_data, map_pointer + i * row_byte_len, i * info.stride, row_byte_len);
        }
    }
    else
    {
        write_to_guest_mem(mem_data, map_pointer, 0, all_pixel_size);
    }

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
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
