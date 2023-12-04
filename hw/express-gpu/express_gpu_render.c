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
#define TIMER_LOG
#include "qemu/osdep.h"
#include "qemu/atomic.h"

#include "hw/express-gpu/express_gpu_render.h"

#include "hw/teleport-express/teleport_express.h"
#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/egl_context.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/glv1.h"
#include "hw/express-gpu/gl_helper.h"

#include "hw/express-gpu/express_sync.h"

#include "ui/console.h"
#include "ui/input.h"
#include "sysemu/runstate.h"

#include "hw/express-gpu/sdl_control.h"

#include "hw/express-gpu/device_interface_window.h"

#include "hw/express-input/express_touchscreen.h"
#include "hw/express-input/express_keyboard.h"

GAsyncQueue *main_window_event_queue = NULL;
volatile int main_window_event_queue_lock = 0;

Static_Context_Values *preload_static_context_value = NULL;

int sdl2_no_need = 0;

int host_opengl_version = 0;

#ifdef ENABLE_DSA
int DSA_enable = 1;
#else
int DSA_enable = 0;
#endif
int VSYNC_enable = 0;

int express_gpu_window_FPS = 60;

bool express_gpu_keep_window_scale = false;

bool express_gpu_open_shader_binary = true;

QemuThread native_window_render_thread;

// static unsigned int main_frame_num = 0;

// static int event_queue_lock;
// static GQueue *sync_event_queue;

static GHashTable *gbuffer_global_map = NULL;
static GHashTable *gbuffer_global_types = NULL;

static volatile int gbuffer_global_map_lock = 0;

static int calc_screen_hz = 0;

static int now_screen_hz = 0;

static gint64 last_calc_time = 0;

static QemuThread device_interface_thread;

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

// void *dummy_window_for_sync = NULL;

static GLuint programID = 0;
static GLuint drawVAO = 0;

static GLint program_transform_loc = 0;
static GLuint now_transform_type = 0;

static bool window_is_shown = false;

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

int force_show_native_render_window = 0;

// guest对应的虚拟显示器的大小
static int display_width = 0;
static int display_height = 0;

static bool window_need_refresh = false;

Graphic_Buffer *main_display_gbuffer;

volatile int native_render_run = 0;
volatile int device_interface_run = 0;

static QemuConsole *input_receive_con = NULL;

static const char GPU_VENDOR[] = "ARM";
#ifdef _WIN32
static const char GPU_VERSION[] = "OpenGL ES 3.2 (";
#else
static const char GPU_VERSION[] = "OpenGL ES 3.0 (";
#endif
static const char GPU_RENDERER[] = "Mali-G77";

#ifdef _WIN32
static const char GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.20";
#else
static const char GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.0";
#endif


// google device info
//  static const GLubyte GPU_VENDOR[] = "Google (";
//  static const GLubyte GPU_VERSION[] = "OpenGL ES 3.0 (";
//  static const GLubyte GPU_RENDERER[] = "Android Emulator OpenGL ES Translator (";
//  static const GLubyte GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.00";

static const int OPENGL_MAJOR_VERSION = 3;
#ifdef _WIN32
static const int OPENGL_MINOR_VERSION = 2;
#else
static const int OPENGL_MINOR_VERSION = 0;
#endif

static const char *SPECIAL_EXTENSIONS[] = {
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
        
        /* --- Android Extension Pack, as required by GLES 3.2 --- */
        "GL_ANDROID_extension_pack_es31a",

        "GL_KHR_blend_equation_advanced",
        "GL_EXT_shader_io_blocks",
        
        "GL_EXT_texture_sRGB_decode",
        "GL_KHR_debug",

        "GL_OES_sample_shading", // -> GL_ARB_sample_shading
        "GL_OES_sample_variables", // -> GL_ARB_sample_shading
        "GL_OES_texture_stencil8", // -> GL_ARB_texture_stencil8
        "GL_EXT_geometry_shader", // -> GL_ARB_geometry_shader4
        "GL_EXT_gpu_shader5", // -> GL_ARB_gpu_shader5
        "GL_OES_shader_multisample_interpolation", // -> GL_ARB_gpu_shader5
        "GL_EXT_tessellation_shader", // -> GL_ARB_tessellation_shader
        "GL_EXT_texture_border_clamp", // -> GL_ARB_texture_border_clamp
        "GL_EXT_texture_cube_map_array", // -> GL_ARB_texture_cube_map_array
        "GL_OES_shader_image_atomic", // -> GL_ARB_shader_image_load_store
        "GL_EXT_draw_buffers_indexed", // -> GL_EXT_draw_buffers2 + GL_ARB_draw_buffers_blend
        "GL_OES_texture_storage_multisample_2d_array", 

        "GL_EXT_primitive_bounding_box",
        "GL_OES_primitive_bounding_box",

        /* --- GLES 3.2 额外要求的扩展 --- */
        "GL_KHR_robustness",
        "GL_KHR_robust_buffer_access_behavior",

        // mumu模拟器12的额外扩展
        "GL_OES_copy_image",
        "GL_EXT_draw_elements_base_vertex",
        "GL_OES_geometry_shader",
        "GL_OES_shader_io_blocks",
        "GL_OES_texture_border_clamp", // -> GL_ARB_texture_border_clamp
        "GL_OES_texture_buffer",
        "GL_OES_texture_cube_map_array", // -> GL_ARB_texture_cube_map_array
        "GL_OES_surfaceless_context", // -> EGL_KHR_surfaceless_context

        // "GL_EXT_EGL_image_external_wrap_modes",
        // "GL_EXT_debug_marker",
        // "GL_EXT_discard_framebuffer",
        // "GL_EXT_draw_buffers",
        // "GL_EXT_float_blend",
        // "GL_EXT_frag_depth",
        // "GL_EXT_instanced_arrays",
        // "GL_EXT_multi_draw_indirect",
        // /*15*/ "GL_EXT_multisampled_render_to_texture",  // 这个暂时不能有，因为它需要支持相关函数 这个具体涉及到glFramebufferTexture2DMultisampleEXT函数，这个函数是tile-based GPU使用TBDR渲染特有的，用来节省带宽，IMR模式的PC上没有，所以没法启用。glFramebufferTexture2DMultisampleEXT这个函数被PUBG在抗锯齿时用到了
        // "GL_EXT_occlusion_query_boolean",
        // "GL_EXT_read_format_bgra",
        // "GL_EXT_sRGB",
        // "GL_EXT_shader_texture_bptc",
        // "GL_EXT_shader_texture_dxt1",
        // "GL_EXT_shader_texture_rgtc",
        // "GL_EXT_shader_texture_s3tc",
        // "GL_EXT_shader_texture_s3tc_srgb",
        // "GL_EXT_texture_filter_anisotropic",
        // /*40*/ "GL_EXT_texture_format_BGRA8888", // pc很可能是不支持的
        // "GL_EXT_texture_rg",
        // "GL_EXT_texture_storage",
        // "GL_OES_compressed_EAC_R11_signed_texture",
        // "GL_OES_compressed_EAC_R11_unsigned_texture",
        // "GL_OES_compressed_EAC_RG11_signed_texture",
        // "GL_OES_compressed_EAC_RG11_unsigned_texture",
        // "GL_OES_compressed_ETC2_RGB8_texture",
        // "GL_OES_compressed_ETC2_RGBA8_texture",
        // "GL_OES_compressed_ETC2_punchthroughA_RGBA8_texture",
        // "GL_OES_compressed_ETC2_punchthroughA_sRGB8_alpha_texture",
        // "GL_OES_compressed_ETC2_sRGB8_alpha8_texture",
        // "GL_OES_compressed_ETC2_sRGB8_texture",
        // "GL_OES_depth_texture_cube_map",
        // "GL_OES_texture_3D",
};
static const int SPECIAL_EXTENSIONS_SIZE = 74;

// 支持这些扩展需要添加一些函数，所以暂时先不支持——因为有些扩展会被全平台的skia识别而使用，但是这些函数实际为空所以会发生错误
//  static const GLubyte *NOT_SUPPORT_EXTENSIONS[] =
//      {
//          //gl
//          /* 1*/ "GL_NV_texture_barrier",          // and gles
//          /* 2*/ "GL_KHR_blend_equation_advanced", // and gles
//          /* 3*/ "GL_NV_blend_equation_advanced",  // and gles
//          /* 4*/ "GL_ARB_clear_texture",
//          /* 5*/ "GL_ARB_draw_indirect",
//          /* 6*/ "GL_ARB_timer_query",
//          /* 7*/ "GL_EXT_timer_query",
//          /* 8*/ "GL_ARB_multi_draw_indirect",
//          /* 9*/ "GL_NV_path_rendering",            // and gles
//          /*10*/ "GL_NV_framebuffer_mixed_samples", // and gles
//          /*11*/ "GL_EXT_debug_marker",             //and gles
//          /*12*/ "GL_ARB_invalidate_subdata",
//          /*13*/ "GL_KHR_debug",             // and gles
//          /*14*/ "GL_EXT_window_rectangles", // and gles

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

static void *native_window_create(int context_flags);

// static void g_queue_event_notify(gpointer data, gpointer user_data);

static Notifier shutdown_notifier;

static Dying_List *dying_gbuffer;

static gint64 last_click_time = 0;

void window_size_change_callback(GLFWwindow *window, int width, int height);

static void close_window_callback(GLFWwindow *window)
{
    gint64 now_time = g_get_real_time();

    // LOGI("shutdown time %lld",now_time);
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
    LOGI("notify shutdown! %lld", g_get_real_time());

    ATOMIC_UNLOCK(main_window_event_queue_lock);
    // ATOMIC_UNLOCK(compose_surface_lock);
    // set_compose_surface(NULL, NULL);
    main_display_gbuffer = NULL;
    teleport_express_should_stop = true;
    device_interface_run = 0;

    // glfwTerminate();
    if (native_render_run == 2)
    {
        native_render_run = -1;
        int wait_cnt = 0;
        while (native_render_run == -1 && wait_cnt < 200)
        {
            // LOGI("wait thread close ");
            g_usleep(5000);
            wait_cnt++;
        }
        if (native_render_run == -1)
        {
            LOGI("wait time too long!");
        }

        // LOGI("wait thread close done %d",native_render_run);
    }
}

void window_size_change_callback(GLFWwindow *window, int width, int height)
{
    //printf("width and height %d %d\n",width,height);
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
            //express_printf("set window size %d %d keep scale\n", window_width, window_height);

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

    if (main_display_gbuffer == gbuffer)
    {
        gbuffer->remain_life_time = MAX_COMPOSER_LIFE_TIME;
        // main_display_gbuffer = NULL;
        return 0;
    }

    if (gbuffer->gbuffer_id != 0)
    {
        // psurface的gbuffer_id为0
        remove_gbuffer_from_global_map(gbuffer->gbuffer_id);
    }

    destroy_gbuffer(gbuffer);
    LOGI("gbuffer %llx is dead", gbuffer->gbuffer_id);

    return 1;
}

static void handle_child_window_event(void)
{
    ATOMIC_LOCK(main_window_event_queue_lock);
    Main_window_Event *child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
    ATOMIC_UNLOCK(main_window_event_queue_lock);

    int paint_event_cnt = 0;
    while (child_event != NULL)
    {
        int64_t start_time = 0;

        if (paint_event_cnt >= 2)
        {
            LOGE("error! too many event %d paint_num %d", child_event->event_code, paint_event_cnt);
        }

        start_time = g_get_real_time();
        switch (child_event->event_code)
        {
        case MAIN_PAINT:
        {
            // Graphic_Buffer *gbuffer = (Graphic_Buffer *)child_event->data;
            // main_display_gbuffer = gbuffer;
            window_need_refresh = true;
        }
        break;
        case MAIN_CREATE_CHILD_WINDOW:

            // context只能是由父线程创建，以进行资源共享
            {
            #ifdef __APPLE__
                __block void **window_ptr = (Window_Buffer *)child_event->data;
            #else
                void **window_ptr = (void **)child_event->data;
            #endif

                if (window_ptr == NULL)
                {
                    LOGW("warning: create child window empty window_ptr");
                    break;
                }
                // LOGI("start create window ptr %llx", window_ptr);

                THREAD_CONTROL_BEGIN

                *window_ptr = (void *)native_window_create((int)(intptr_t)*window_ptr);

                THREAD_CONTROL_END
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
                // LOGI("real destroy gbuffer %llx ptr %llx", gbuffer->gbuffer_id, gbuffer);
                dying_gbuffer = dying_list_append(dying_gbuffer, gbuffer);
            }
        }
        break;
        case MAIN_CANCEL_GBUFFER:
        {
            Graphic_Buffer *gbuffer = (Graphic_Buffer *)child_event->data;
            if (gbuffer != NULL)
            {
                // LOGI("real cancel gbuffer delete %llx ptr %llx", gbuffer->gbuffer_id, gbuffer);
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
        int64_t end_time = g_get_real_time();
        if (end_time - start_time > 20000 && child_event != NULL)
        {
            LOGW("warning! slow child_event %d time spend %lld now_time %lld queue_size %d", child_event->event_code, (end_time - start_time) / 1000, end_time / 1000, g_async_queue_length(main_window_event_queue));
        }

        ATOMIC_LOCK(main_window_event_queue_lock);
        child_event = (Main_window_Event *)g_async_queue_try_pop(main_window_event_queue);
        ATOMIC_UNLOCK(main_window_event_queue_lock);
    }
    return;
}

static void static_value_prepare(void)
{

    preload_static_context_value = g_malloc0(sizeof(Static_Context_Values) + 512 * 100 + 400);

    preload_static_context_value->composer_HZ = 60;
    preload_static_context_value->composer_pid = 0;

    // initialize static status
    preload_static_context_value->major_version = OPENGL_MAJOR_VERSION;
    preload_static_context_value->minor_version = OPENGL_MINOR_VERSION;

    prepare_integer_value(preload_static_context_value);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        LOGE("error when creating static vaules %x", error);
    }
    // 下面三个值之所以要限定范围，是因为guest端有个固定大小的数组，这个最大值是数组的最大大小
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
    //     LOGI("support compress texture%d %x ",i, preload_static_context_value->compressed_texture_formats[i]);
    // }
    // LOGI("binary formats num %d eg %d shader formats num %d eg %d",preload_static_context_value->num_program_binary_formats,
    //     preload_static_context_value->program_binary_formats[0],
    //     preload_static_context_value->num_shader_binary_formats,
    //     preload_static_context_value->shader_binary_formats[0]);

    //@todo 增加换硬件后暂时移除binary的功能
    if (!express_gpu_open_shader_binary)
    {
        preload_static_context_value->num_program_binary_formats = 0;
        preload_static_context_value->num_shader_binary_formats = 0;
    }

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
    LOGI("gl vendor:%s", (char *)gl_string);

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
    LOGI("gl version:%s", string_loc + (unsigned long)(preload_static_context_value->version));

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
    LOGI("gl renderer:%s", (char *)gl_string);

    preload_static_context_value->shading_language_version = (unsigned long long)(temp_loc - string_loc);
    memcpy(temp_loc, GPU_SHADER_LANGUAGE_VERSION, sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1);
    temp_loc += sizeof(GPU_SHADER_LANGUAGE_VERSION) - 1;
    *temp_loc = 0;
    temp_loc++;
    LOGI("gl shading_language_version:%s", string_loc + (unsigned long)(preload_static_context_value->shading_language_version));

    char *extensions_start = temp_loc;

    // int no_need_extensions_cnt = 0;
    int num_extensions = preload_static_context_value->num_extensions;

    // num_extensions = 0;
    // 目前暂时只设定固定的扩展支持

    int start_loc = 0;
    int has_dsa = 0;
    for (int i = start_loc; i < start_loc + num_extensions; i++)
    {

        gl_string = (const char *)glGetStringi(GL_EXTENSIONS, i);

        if (express_gpu_gl_debug_enable)
        {
            LOGI("host extension %d %s", i, gl_string);
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
        // LOGI("%d %s", i, temp_loc - strlen(gl_string));
        // temp_loc++;
    }

    if (has_dsa == 0)
    {
        DSA_enable = 0;
    }

    LOGI("host gl %d DSA_enable %d", host_opengl_version, DSA_enable);

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
        LOGI("extensions len %d num %d: %s|", extensions_len, num_extensions, string_loc + (unsigned long)(preload_static_context_value->extensions_gles2));
    }
    assert(temp_loc < ((char *)preload_static_context_value) + sizeof(Static_Context_Values) + 512 * 100 + 400);
}

static void opengl_paint_composer_gbuffer(void)
{
    if (main_display_gbuffer == NULL)
    {
        return;
    }

    ATOMIC_LOCK(main_display_gbuffer->is_lock);

    Graphic_Buffer *gbuffer = main_display_gbuffer;

    if (display_width != gbuffer->width || display_height == gbuffer->height)
    {
        display_width = gbuffer->width;
        display_height = gbuffer->height;
        // real_window_width = window_width;
        // real_window_height = window_height;
        // glViewport(0, 0, window_width, window_height);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(main_display_content_x, main_display_content_y, main_display_content_width, main_display_content_height);

    glWaitSync(gbuffer->data_sync, 0, GL_TIMEOUT_IGNORED);

    opengl_paint_gbuffer(gbuffer);

    ATOMIC_UNLOCK(gbuffer->is_lock);

    // int64_t now_time = g_get_real_time();
    // static int64_t last_display_time = 0;
    // LOGI("paint %llx time %lld gap %lld", (int64_t)main_display_gbuffer, now_time/1000, (now_time - last_display_time)/1000);
    // last_display_time = now_time;

    glFlush();
}

/**
 * @brief 界面上用于画出图像的函数，实际逻辑为取出gbuffer中的display_texture，然后画出来
 *
 * @param gbuffer
 */
void opengl_paint_gbuffer(Graphic_Buffer *gbuffer)
{
    if (gbuffer != NULL)
    {
        // LOGI("opengl_paint gbuffer %llx texture %d", gbuffer->gbuffer_id, gbuffer->data_texture);
        gbuffer->remain_life_time = MAX_COMPOSER_LIFE_TIME;
        // LOGI("paint texture %d",gbuffer->data_texture);

        if (gbuffer->is_writing != 0)
        {
            LOGE("error! get writing gbuffer when opengl_paint");
        }

        express_printf("draw gbuffer_id %llx data sync %lld\n", gbuffer->gbuffer_id, (uint64_t)gbuffer->data_sync);

        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

/**
 * @brief 创建带window的opengl的context，这个创建过程是在主界面线程中进行的，通过消息机制来实现
 *
 * @param context_flags context的模式，例如单独窗口（非OpenGL原生）、debug context、robust context
 */
static void *native_window_create(int context_flags)
{

    void *child_window = NULL;
    static int windows_cnt = 0;
    int cnt = windows_cnt++;

    if (context_flags & DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT)
    {
        char name[100];
        sprintf(name, "opengl-child-window%d", cnt);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        // glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        if (express_gpu_gl_debug_enable)
        {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }
        glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
#endif

        // 因为咱们是使用的fbo来绘制，因此窗口大小设为1就行了
        child_window = (void *)glfwCreateWindow(1, 1, name, NULL, glfw_window);

        if (child_window == NULL)
        {
            const char *s = NULL;
            int ret = glfwGetError(&s);
            LOGE("error code %d detail %s", ret, s);
        }
    }
    else
    {
        child_window = egl_createContext(context_flags);
    }

    assert(child_window != NULL);

    express_printf("native window create success %p context flag %x\n", child_window, context_flags);
    
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

    // 通过这个方式获取hwnd要求必须使用SDL接口创建界面
    QemuConsole *con = NULL;
    // while ((con = qemu_console_lookup_by_index(0)) == NULL)
    // {
    //     //理论上启动这个线程时，主窗口的hwnd肯定是有了，所以不会进到这个等待循环内
    //     g_usleep(10000);
    //     express_printf("con is NULL\n");
    // }

    input_receive_con = con;

    // sync_event_queue = g_queue_new();

    main_window_event_queue = g_async_queue_new();

    // HWND render_hwnd = (HWND)qemu_console_get_window_id(con);
    // RECT rcParent;

    // GetClientRect(render_hwnd, &rcParent);

    // 初始化glfw
    THREAD_CONTROL_BEGIN
    if (!glfwInit()){
    #ifdef __APPLE__ 
        exit(-1);
    #else
        return NULL;
    #endif
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    if (express_gpu_gl_debug_enable)
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
#endif

    // 创建一个窗口，这个window也是context
    window_width = express_gpu_window_width;
    window_height = express_gpu_window_height;
    display_width = *express_display_pixel_width;
    display_height = *express_display_pixel_height;

    main_display_content_width = express_gpu_window_width;
    main_display_content_height = express_gpu_window_height;

    glfw_window = glfwCreateWindow(window_width, window_height, "Trinity", NULL, NULL);

    if (!glfw_window)
    {
        express_printf("create window error %x\n", glfwGetError(NULL));

        glfwTerminate();
    #ifdef __APPLE__    
        exit(-1);
    #else
        return NULL;
    #endif

    }

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

    if (express_device_input_window_enable)
    {
        device_interface_run = 1;
        qemu_thread_create(&device_interface_thread, "interface_thread", interface_window_thread, (void *)&device_interface_run, QEMU_THREAD_DETACHED);
    }

#ifdef __APPLE__
    void *dpy_dc = NULL;
    void *gl_context = (void *)glfwGetNSGLContext(glfw_window);
#endif
#ifdef __linux__
    void *dpy_dc = (void *)glfwGetEGLDisplay();
    void *gl_context = (void *)glfwGetEGLContext(glfw_window);
#endif
#ifdef _WIN32
    HDC dpy_dc = GetDC(glfwGetWin32Window(glfw_window));
    HGLRC gl_context = glfwGetWGLContext(glfw_window);
#endif

#ifndef USE_GLFW_AS_WGL
    egl_init(dpy_dc, gl_context);
#endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOGI("load glad error");
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

    program_transform_loc = glGetUniformLocation(programID, "transform_loc");
    now_transform_type = 0;
    glUniform1i(program_transform_loc, now_transform_type);

    LOGI("native windows create!\n");

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
            // 处理各种输入事件、opengl事件
            THREAD_CONTROL_BEGIN

            //处理各种输入事件、opengl事件
            glfwWaitEventsTimeout(0.001);

            THREAD_CONTROL_END

            sync_express_touchscreen_input((bool)display_is_open || !express_display_switch_open);
            sync_express_keyboard_input((bool)display_is_open || !express_display_switch_open);
            // express_input_device_sync();

            handle_child_window_event();
            // LOGI("start run native thread %d %d", force_show_native_render_window, window_is_shown);

            if (force_show_native_render_window != 0 && window_is_shown == false)
            {
                if (force_show_native_render_window == 2)
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
                // opengl_paint_composer_layers();

                calc_screen_hz += 1;
                has_refresh = true;

                glfwSwapBuffers(glfw_window);

                // 把foreach放到下面，是因为主线程的消息中可能有取消gbuffer销毁流程的消息
                // 放到绘制函数里，是为了避免过快销毁gbuffer（绘制函数外是最高1000hz的频率
                dying_list_foreach(dying_gbuffer, try_destroy_gbuffer);
            }
            else
            {
                if ((main_display_gbuffer == NULL) && window_is_shown == true && force_show_native_render_window == 0)
                {
                    window_is_shown = false;
                    LOGI("hide window");
                    glfwHideWindow(glfw_window);

                    sdl2_no_need = 0;
                }
            }

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

        // 丢帧丢了100ms了，就不管少休眠的时间了
        if (need_sleep_time < -100000)
        {
            need_sleep_time = 0;
        }
        // LOGI("remain_sleep_time %lld", remain_sleep_time);

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

            frame_draw_time = 0;
            last_calc_time = now_time;
        }
    }

    // qemu_system_shutdown_request(SHUTDOWN_CAUSE_HOST_UI);
    glfwMakeContextCurrent(NULL);

    THREAD_CONTROL_BEGIN

    glfwDestroyWindow(glfw_window);

    THREAD_CONTROL_END

    LOGI("native windows close!");

    // 当他返回0时表示窗口被关掉了
    native_render_run = 0;
    // qemu_thread_join(&t);
    return NULL;
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
    // if (gbuffer != NULL)
    // {
    //     gbuffer->remain_life_time = (gbuffer->usage_type == GBUFFER_TYPE_BITMAP ? MAX_BITMAP_LIFE_TIME : MAX_WINDOW_LIFE_TIME);
    // }
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
    if (message_code == MAIN_PAINT || message_code == MAIN_PAINT_LAYERS || message_code == MAIN_CREATE_CHILD_WINDOW)
    {
        glfwPostEmptyEvent();
    }
}