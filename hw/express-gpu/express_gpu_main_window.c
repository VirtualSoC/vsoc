/**
 * @file express_gpu_main_window.c
 * @author gaodi (gaodi.sec@qq.com)
 * @author Jiaxing Qiu (jx.qiu@outlook.com)
 * @brief 'main_window' is an offscreen glfw window from which ALL graphics contexts are derived.
 * Resources are shared among the main context and the child graphics contexts. 

 * @copyright Copyright (c) 2020-2024 the authors
 *
 */
// #define STD_DEBUG_LOG
// #define TIMER_LOG
#include "qemu/osdep.h"
#include "qemu/atomic.h"
#include "sysemu/runstate.h"

#include "hw/teleport-express/express_log.h"
#include "hw/express-gpu/express_gpu_main_window.h"
#include "hw/teleport-express/teleport_express.h"

#include "hw/express-gpu/egl_context.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/gl_helper.h"
#include "hw/express-gpu/glv1.h"

#include "hw/express-mem/express_sync.h"

#include "hw/express-gpu/device_interface_window.h"

#include "hw/express-gpu/express_gpu_snapshot.h"


GAsyncQueue *main_window_event_queue = NULL;
int main_window_event_queue_lock = 0;

Static_Context_Values *preload_static_context_value = NULL;

#ifdef ENABLE_DSA
int DSA_enable = 1;
#else
int DSA_enable = 0;
#endif

bool express_gpu_open_shader_binary = true;

int main_window_run = 0;
int device_interface_run = 0;
int host_opengl_version = 0;

#define EVENT_QUEUE_LOCK                                    \
    while (qatomic_cmpxchg(&(event_queue_lock), 0, 1) == 1) \
        ;

#define EVENT_QUEUE_UNLOCK qatomic_cmpxchg(&(event_queue_lock), 1, 0);

static GHashTable *gbuffer_global_map = NULL;
static GHashTable *gbuffer_global_types = NULL;

static volatile int gbuffer_global_map_lock = 0;

static QemuThread qemu_main_window_thread;
static QemuThread qemu_device_interface_thread;

static GLFWwindow *main_window = NULL;

static const char GPU_VENDOR[] = "ARM";
#ifndef __APPLE__
static const char GPU_VERSION[] = "OpenGL ES 3.2 (";
#else
static const char GPU_VERSION[] = "OpenGL ES 3.0 (";
#endif
static const char GPU_RENDERER[] = "Mali-G77";

#ifndef __APPLE__
static const char GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.20";
#else
static const char GPU_SHADER_LANGUAGE_VERSION[] = "OpenGL ES GLSL ES 3.00";
#endif

static const int OPENGL_MAJOR_VERSION = 3;
#ifdef __APPLE__
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
        // /*13*/ "GL_KHR_texture_compression_astc_hdr",
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
};
static const int SPECIAL_EXTENSIONS_SIZE = 73;

extern Hardware_Buffer *main_display_gbuffer;

static void *sub_window_create(int context_flags);

static void glfw_error_callback(int error, const char *description)
{
    LOGE("glfw error 0x%x: %s", error, description);
    return;
}

static void shutdown_notify_callback(Notifier *notifier, void *data)
{
    LOGI("notify shutdown! %lld", g_get_real_time());

    ATOMIC_UNLOCK(main_window_event_queue_lock);
    teleport_express_should_stop = true;
    device_interface_run = 0;

    if (main_window_run == 2)
    {
        main_window_run = -1;
        int wait_cnt = 0;
        while (main_window_run == -1 && wait_cnt < 200)
        {
            g_usleep(5000);
            wait_cnt++;
        }
        if (main_window_run == -1)
        {
            LOGI("wait time too long!");
        }
    }
}

int save_gbuffer_global_map(QEMUFile *f)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);

    GHashTableIter iter;
    gpointer key, value;
    guint num_entries = g_hash_table_size(gbuffer_global_map);
    LOGD("in save_gbuffer_global_map with %d", num_entries);
    qemu_put_be32(f, num_entries);
    g_hash_table_iter_init(&iter, gbuffer_global_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Hardware_Buffer *global_gbuffer = (Hardware_Buffer *)value;
        // qemu_put_be64(f, (uint64_t)key);
        LOGI("gbuffer id is %llx %llx", key, global_gbuffer->gbuffer_id);
        save_hardware_buffer(f, global_gbuffer);   
    }

    ATOMIC_UNLOCK(gbuffer_global_map_lock);

    return 0;
}

int load_gbuffer_global_map(QEMUFile *f) {
    ATOMIC_LOCK(gbuffer_global_map_lock);

    GHashTable *gbuffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    guint num_entries = qemu_get_be32(f);
    uint64_t gbuffer_id;
    Hardware_Buffer *global_gbuffer = g_malloc0(sizeof(Hardware_Buffer));
    LOGD("num Hardware_Buffer in load is %d", num_entries);
    for (guint i = 0; i < num_entries; i++) {
        // gbuffer_id = qemu_get_be64(f);
        
        global_gbuffer = load_hardware_buffer(f);
        LOGD("gbuffer id in load is %lld %lld", gbuffer_id, global_gbuffer->gbuffer_id);
        g_hash_table_insert(gbuffer_map, GUINT_TO_POINTER(global_gbuffer->gbuffer_id), global_gbuffer);
    }
    gbuffer_global_map = gbuffer_map;

    ATOMIC_UNLOCK(gbuffer_global_map_lock);

    return 0;
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

                *window_ptr = (void *)sub_window_create((int)(intptr_t)*window_ptr);

                THREAD_CONTROL_END
            }

            break;

        case MAIN_DESTROY_GBUFFER:
        {
            Hardware_Buffer *gbuffer = (Hardware_Buffer *)child_event->data;
            destroy_gbuffer(gbuffer);
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
            if (status->gbuffer_id_map != NULL)
            {
                g_free(status->gbuffer_id_map);
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
            LOGD("child window message %d not handled", child_event->event_code);
            break;
        }
        g_free(child_event);
        int64_t end_time = g_get_real_time();
        if (end_time - start_time > 20000 && child_event != NULL)
        {
            LOGW("slow child event %d, spent %lld ms queue_size %d", child_event->event_code, (end_time - start_time) / 1000, g_async_queue_length(main_window_event_queue));
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

    preload_static_context_value->composer_HZ = express_display_refresh_rate;
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

    int num_extensions = preload_static_context_value->num_extensions;

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
    }

    if (has_dsa == 0)
    {
        DSA_enable = 0;
    }

    LOGI("host gl %d DSA_enable %d", host_opengl_version, DSA_enable);

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

/**
 * @brief 创建带window的opengl的context，这个创建过程是在主界面线程中进行的，通过消息机制来实现
 *
 * @param context_flags context的模式，例如单独窗口（非OpenGL原生）、debug context、robust context
 */
static void *sub_window_create(int context_flags)
{

    void *child_window = NULL;
    static int windows_cnt = 0;
    int cnt = windows_cnt++;

    if (context_flags & DGL_CONTEXT_FLAG_INDEPENDENT_MODE_BIT)
    {
        char name[32];
        sprintf(name, "child-window%d", cnt);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        if (express_gpu_gl_debug_enable)
        {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }
#endif
#ifdef _WIN32
        glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
#endif

        // 因为咱们是使用的fbo来绘制，因此窗口大小设为1就行了
        child_window = (void *)glfwCreateWindow(1, 1, name, NULL, main_window);

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

    LOGD("native window create success %p context flag %x\n", child_window, context_flags);

    return child_window;
}

/**
 * Main window thread entry point 
 * Unique per emulator instance
 */
void *main_window_thread(void *opaque)
{
    main_window_event_queue = g_async_queue_new();

#if defined(__linux__) && defined(GLFW_PLATFORM_WAYLAND)
    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
        glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_FALSE);
    }
    else {
        LOGE("glfw+wayland not supported, using x11. x11 does not play nice with egl, so expect errors to occur");
    }
#endif

    // 初始化glfw
    THREAD_CONTROL_BEGIN
    if (!glfwInit()){
    #ifdef __APPLE__ 
        exit(-1);
    #else
        return NULL;
    #endif
    }
    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#ifdef __linux__
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif
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
#endif
#ifdef _WIN32
    // macos does not support context flags
    // egl on linux reports 0x3009 (EGL_BAD_MATCH) when setting context robustness
    // so they are disabled for now
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
#endif

    main_window = glfwCreateWindow(1, 1, "Main Window", NULL, NULL);

    if (!main_window)
    {
        LOGF("fatal: cannot create main window %x", glfwGetError(NULL));

        glfwTerminate();
    #ifdef __APPLE__    
        exit(-1);
    #else
        return NULL;
    #endif

    }

    THREAD_CONTROL_END

    glfwMakeContextCurrent(main_window);

    glfwSwapInterval(0);

#ifdef __APPLE__
    void *dpy_dc = NULL;
    void *gl_context = (void *)glfwGetNSGLContext(main_window);
#endif
#ifdef __linux__
    void *dpy_dc = (void *)glfwGetEGLDisplay();
    void *gl_context = (void *)glfwGetEGLContext(main_window);
#endif
#ifdef _WIN32
    HDC dpy_dc = GetDC(glfwGetWin32Window(main_window));
    HGLRC gl_context = glfwGetWGLContext(main_window);
#endif

#ifndef USE_GLFW_AS_WGL
    egl_init(dpy_dc, gl_context);
#endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOGI("load glad error");
        return NULL;
    }

    Notifier shutdown_notifier;
    shutdown_notifier.notify = shutdown_notify_callback;
    qemu_register_shutdown_notifier(&shutdown_notifier);

    gbuffer_global_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    gbuffer_global_types = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);

    static_value_prepare();

    prepare_draw_texi();

    if (express_device_input_window_enable)
    {
        device_interface_run = 1;
        qemu_thread_create(&qemu_device_interface_thread, "interface_thread", interface_window_thread, (void *)&device_interface_run, QEMU_THREAD_DETACHED);
    }

    LOGI("main window successfully initialized");

    main_window_run = 2;

    if (express_gpu_gl_debug_enable)
    {
    #ifdef _WIN32
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(d_debug_message_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    #endif
    }

    while (!glfwWindowShouldClose(main_window) && main_window_run == 2)
    {
        THREAD_CONTROL_BEGIN

        //处理各种输入事件、opengl事件
        glfwWaitEventsTimeout(0.001);

        THREAD_CONTROL_END

        handle_child_window_event();
    }

    glfwMakeContextCurrent(NULL);

    THREAD_CONTROL_BEGIN

    glfwDestroyWindow(main_window);

    THREAD_CONTROL_END

    LOGI("main window closed");

    // 当他返回0时表示窗口被关掉了
    main_window_run = 0;
    return NULL;
}

/**
 * Starts and initializes the main window thread.
 * If the main window thread is already running, do nothing.
 */
void start_main_window_thread(void) {
    if (qatomic_cmpxchg(&main_window_run, 0, 1) == 0)
    {
        express_printf("create main window\n");
        // main_window线程只能创建一次，且其他线程必须等待该线程运行成功
        qemu_thread_create(&qemu_main_window_thread, "main_window_thread", main_window_thread, NULL, QEMU_THREAD_DETACHED);
        init_display(&default_egl_display);
    }

    if (main_window_run == 1)
    {
        do
        {
            g_usleep(5000);
        } while (main_window_run != 2);
    }
}

void add_gbuffer_to_global(Hardware_Buffer *global_gbuffer)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    g_hash_table_insert(gbuffer_global_map, (gpointer)(global_gbuffer->gbuffer_id), (gpointer)global_gbuffer);
    ATOMIC_UNLOCK(gbuffer_global_map_lock);
}

Hardware_Buffer *get_gbuffer_from_global_map(uint64_t gbuffer_id)
{
    ATOMIC_LOCK(gbuffer_global_map_lock);
    Hardware_Buffer *gbuffer = (Hardware_Buffer *)g_hash_table_lookup(gbuffer_global_map, (gpointer)(gbuffer_id));
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