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
#include "hw/teleport-express/express_device_common.h"

#include "hw/express-gpu/express_display.h"
#include "hw/express-gpu/express_gpu_snapshot.h"

#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/express_gpu.h"
#include "hw/express-gpu/express_gpu_render.h"

#include "hw/express-gpu/egl_surface.h"

#include "hw/express-gpu/express_gpu_snapshot.h"

#include "hw/express-mem/express_sync.h"

#include "qemu/atomic.h"

static Thread_Context *static_display_context = NULL;

static void *native_display_context = NULL;

static Display_Status now_display_status;

static GLuint programID = 0;
static GLuint drawVAO = 0;

static GLint program_transform_loc = 0;
static GLuint now_transform_type = 0;

static Hardware_Buffer *display_write_gbuffer;
static Hardware_Buffer *display_read_gbuffer;

#ifdef __APPLE__
int display_write_gbuffer_texture;
int display_read_gbuffer_texture;
int old_display_read_gbuffer;
int old_display_write_gbuffer;
#endif

static int display_has_inited = 0;


void save_display_texture_ids(QEMUFile *f) {
    qemu_put_be32(f, display_write_gbuffer->data_texture);
    qemu_put_be32(f, display_read_gbuffer->data_texture);
    LOGI("save display texture %d %d", display_write_gbuffer->data_texture, display_read_gbuffer->data_texture);
}

void update_display_gbuffer_texture(QEMUFile *f) {
    GLint write_texture = qemu_get_be32(f);
    GLint read_texture = qemu_get_be32(f);
    // LOGI("before update date texture %d %d", display_write_gbuffer->data_texture, display_read_gbuffer->data_texture);

#ifdef __APPLE__
    // macOS-specific implementation
    display_write_gbuffer_texture = get_host_id_map(RESOURCE_TYPE_TEXTURE, write_texture);
    display_read_gbuffer_texture = get_host_id_map(RESOURCE_TYPE_TEXTURE, read_texture);
#else
    // Windows-specific implementation
    display_write_gbuffer->data_texture = get_host_id_map(RESOURCE_TYPE_TEXTURE, write_texture);
    display_read_gbuffer->data_texture = get_host_id_map(RESOURCE_TYPE_TEXTURE, read_texture);
#endif

    // LOGI("restore display read and write %d %d %d %d", write_texture, read_texture, display_write_gbuffer->data_texture, display_read_gbuffer->data_texture);
}

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

    LOGD("display decode invoke id %llx", call->id);

    switch (call->id)
    {
    case FUNID_Terminate:
    {
        // do nothing or hide window
        LOGI("display terminate");
    }
    break;
    case FUNID_Snapshot_Load:
    {
        LOGI("display snapshot load");
        glBindFramebuffer(GL_FRAMEBUFFER, display_read_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_read_gbuffer->data_texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_write_gbuffer->data_texture, 0);
        main_display_gbuffer = display_read_gbuffer;
        send_message_to_main_window(MAIN_PAINT, display_read_gbuffer);
    }
    break;
    case FUNID_Commit_Composer_Layer:
    {
        LOGD("display commit composer layer");
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
        LOGD("display commit composer layer going to opengl_paint_composer_layers %d", layers->layer_num);
        opengl_paint_composer_layers(layers); //对每一层进行渲染
        g_free(layers);

        display_present(); //将当前帧缓冲区设置为可显示状态
        LOGD("going to present");
        send_message_to_main_window(MAIN_PAINT, display_read_gbuffer); //通知主线程更新显示内容
    }
    break;
    case FUNID_Show_Window:
    {
        LOGI("force_show_native_render_window");
        force_show_native_render_window = 1;
    }
    break;
    case FUNID_Show_Window_FLIP_V:
    {
        LOGI("force_show_native_render_window-filp_v");
        force_show_native_render_window = 2;
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

    LOGD("finish one display call of id %llx", call->id);
    call->callback(call, 1);

    return;
}


static Thread_Context *get_display_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    LOGD("get display thread context %llx %llx", thread_id, device_id);
    if (static_display_context == NULL)
    {
        static_display_context = thread_context_create(thread_id, device_id, sizeof(Thread_Context), info);
        display_context_thread_id = thread_id;

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

void init_display_context_vmload(){
    
    if(display_has_inited != 0){
        return;
    }
    LOGD("in init display context vmload!");
    display_has_inited = 1;
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
        LOGI("display context init! %llx %llx", (uint64_t)display_write_gbuffer, (uint64_t)display_read_gbuffer);
        LOGI("display fbo id %d %d %d %d", display_write_gbuffer->data_fbo, display_read_gbuffer->data_fbo, display_read_gbuffer->data_texture, display_write_gbuffer->data_texture);

        glBindFramebuffer(GL_FRAMEBUFFER, display_read_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_read_gbuffer->data_texture, 0);


        glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_write_gbuffer->data_texture, 0);

        //ztodo:这个要重新创建，在重启的情况下
        main_window_opengl_prepare(&programID, &drawVAO);
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

static void display_context_init(Thread_Context *context)
{

    
    // if(display_has_inited != 0){
    //     return;
    // }
    // LOGI("display context init!");
    // display_has_inited = 1;
    // 这个render线程只能创建一次，且其他线程必须等待该线程运行成功
    if (qatomic_cmpxchg(&native_render_run, 0, 1) == 0)
    {
        LOGI("create native window");
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

    #ifdef __APPLE__
        old_display_read_gbuffer = display_read_gbuffer->data_texture;
        old_display_write_gbuffer = display_write_gbuffer->data_texture;
    #endif

        LOGI("display context init! %llx %llx", (uint64_t)display_write_gbuffer, (uint64_t)display_read_gbuffer);
        LOGI("display fbo id %d %d %d %d", display_write_gbuffer->data_fbo, display_read_gbuffer->data_fbo, display_read_gbuffer->data_texture, display_write_gbuffer->data_texture);

        glBindFramebuffer(GL_FRAMEBUFFER, display_read_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_read_gbuffer->data_texture, 0);


        glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_write_gbuffer->data_texture, 0);

        //ztodo:这个要重新创建，在重启的情况下



        main_window_opengl_prepare(&programID, &drawVAO);
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
#ifdef __APPLE__
    if(!display_fbo_has_loaded) {
        GLint currentFBO = 0;
        GLint currentTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

        // display_write_gbuffer = create_gbuffer(express_display_info.pixel_width, express_display_info.pixel_height,
        //     0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8, 0, 0, 0);
        // display_read_gbuffer = create_gbuffer(express_display_info.pixel_width, express_display_info.pixel_height,
        //    0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8, 0, 0, 0);
        // GLenum glerror = glGetError();
        // if (glerror != GL_NO_ERROR)
        // {
        //     LOGE("error1 when glGenFramebuffers display fbo %x", glerror);
        // }

        // glGenFramebuffers(1, &display_write_gbuffer->data_fbo);
        // glGenFramebuffers(1, &display_read_gbuffer->data_fbo);
        display_read_gbuffer->data_texture = display_read_gbuffer_texture;
        display_write_gbuffer->data_texture = display_write_gbuffer_texture;

        glBindFramebuffer(GL_FRAMEBUFFER, display_read_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_read_gbuffer->data_texture, 0);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // glerror = glGetError();
        // if (glerror != GL_NO_ERROR)
        // {
        //     LOGE("error1 when reconnecting display fbo %x", glerror);
        // }

        // glBindTexture(GL_TEXTURE_2D, display_write_gbuffer->data_texture);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, express_display_info.pixel_width, express_display_info.pixel_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, display_write_gbuffer->data_texture, 0);

        // glBindTexture(GL_TEXTURE_2D, currentTexture);
        // glerror = glGetError();
        // if (glerror != GL_NO_ERROR)
        // {
        //     LOGE("error when reconnecting display fbo %x", glerror);
        // }
        display_fbo_has_loaded = 1;
        // LOGI("reset display_fbo_has_loaded after load of read fbo %d write fbo %d texture %d %d", display_read_gbuffer->data_fbo, display_write_gbuffer->data_fbo, display_read_gbuffer->data_texture, display_write_gbuffer->data_texture);
    }
#endif    

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
                LOGD("draw layer %d gbuffer_id %llx texture %d %d %d %d %d gbuffer_size %d %d blend_type %d transform_type %d",
                               i, layer.gbuffer_id, gbuffer->data_texture, layer.x, layer.y, layer.width, layer.height, gbuffer->width, gbuffer->height, layer.blend_type, layer.transform_type);
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
                // LOGI("transform_type %d layer transform_type %d", disp->transform_type, layer.transform_type)


                LOGD("glviewport %d %d %d %d glScissor %d %d %d %d", view_x, view_y, view_w, view_h, layer.x, display_height - layer.y - layer.height, layer.width, layer.height);
                LOGD("layer %d %d %d %d crop %d %d %d %d", layer.x, layer.y, layer.width, layer.height, layer.crop_x, layer.crop_y, layer.crop_width, layer.crop_height);
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
                        LOGE("error! not support transform_type %d", now_transform_type);
                    }
                    glUniform1i(program_transform_loc, now_transform_type);
                }

                if(gbuffer->backend_type == HARDWARE_BUFFER_BACKEND_VULKAN) {
                    glUniform1i(program_transform_loc, FLIP_V);
                    LOGI("set vulkan transform uniform %d", FLIP_V);
                }
                opengl_paint_gbuffer(gbuffer);
                if(gbuffer->backend_type == HARDWARE_BUFFER_BACKEND_VULKAN) {
                    glUniform1i(program_transform_loc, layer.transform_type);
                }


                LOGD("composer set sync %d", layer.read_sync_id);

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

    display_write_gbuffer = temp_gbuffer; //交换read和write buffer

    glBindFramebuffer(GL_FRAMEBUFFER, display_write_gbuffer->data_fbo);

    // 保证主线程使用完成上一个gbuffer
    ATOMIC_LOCK(display_write_gbuffer->is_lock);

    main_display_gbuffer = display_read_gbuffer;

    // 这里直接unlock，不需要一直锁住，是因为只有这个画完了之后，才会赋值到main_display_gbuffer，所以不会被主线程访问到，也就不需要锁住
    // 无论主线程之后会不会继续读取，这里都要直接进行后续的绘制
    ATOMIC_UNLOCK(display_write_gbuffer->is_lock);

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
