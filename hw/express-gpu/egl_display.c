// #define STD_DEBUG_LOG
#include "hw/express-gpu/egl_display.h"
#include "hw/teleport-express/express_log.h"

Egl_Display *default_egl_display;

#ifdef __WIN32
/**
 * @brief 初始化Egl_Display
 *
 * @param display_point 待初始化的Egl_Display
 */
void init_display(Egl_Display **display_point)
{
    default_egl_display = g_malloc0(sizeof(Egl_Display));

    LOGD("init display");
    init_configs(default_egl_display);

    default_egl_display->guest_ver_major = 1;
    default_egl_display->guest_ver_minor = 5;

    default_egl_display->is_init = true;
}

/**
 * @brief 初始化EGL configuration并将所有可用configuration保存到全局的hash表中
 *
 * @param display 待初始化的Egl_Display
 */
void init_configs(Egl_Display *display)
{
    if (display->egl_config_set == NULL)
    {
        display->egl_config_set = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    // 添加一些与窗口无关的配置
    add_simple_config(display);
    add_window_independent_config(display, EGL_DEPTH_SIZE, depth_vals, NUM_DEPTH_VAL);
    add_window_independent_config(display, EGL_STENCIL_SIZE, stencil_vals, NUM_STENCILE_VAL);
    add_window_independent_config(display, EGL_SAMPLES, sample_vals, NUM_SAMPLE_VAL);
}
#endif

EGLBoolean add_config(Egl_Display *display, eglConfig *config)
{
    if (!is_config_in_table(config, display->egl_config_set))
    {
        config->config_id = g_hash_table_size(display->egl_config_set) + 1;
        g_hash_table_insert(display->egl_config_set, GUINT_TO_POINTER(config->config_id), (gpointer)config);
        return EGL_TRUE;
    }
    else
    {
        return EGL_FALSE;
    }
}

/**
 * @brief 添加与窗口无关的配置信息，可以指定某个配置属性，添加一系列值
 *
 * @param display 需要添加配置的display
 * @param attr_enum 需要添加的配置属性的Enum，比如EGL_RED_SIZE
 * @param vals 需要添加的属性值数组
 * @param val_size 属性值数组的长度
 */
void add_window_independent_config(Egl_Display *display, EGLint attr_enum, const EGLint *vals, EGLint val_size)
{
    int config_set_size = g_hash_table_size(display->egl_config_set);
    for (int i = 0; i < config_set_size; i++)
    {
        eglConfig *config = (eglConfig *)g_hash_table_lookup(default_egl_display->egl_config_set, GUINT_TO_POINTER(i + 1));
        for (int j = 0; j < val_size; j++)
        {
            eglConfig *new_config = (eglConfig *)g_malloc(sizeof(eglConfig));
            memcpy(new_config, config, sizeof(eglConfig));
            set_val_by_enum(new_config, vals[j], attr_enum);
            if (!add_config(display, new_config))
            {
                g_free(new_config);
            }
        }
    }
}

void add_simple_config(Egl_Display *display)
{
    EGLint red_sizes[] = {5, 8, 8};
    EGLint green_sizes[] = {6, 8, 8};
    EGLint blue_sizes[] = {5, 8, 8};
    EGLint alpha_sizes[] = {0, 0, 8};
    int length = sizeof(red_sizes) / sizeof(EGLint);

    for (int i = 0; i < length; i++)
    {
        eglConfig *config = (eglConfig *)g_malloc(sizeof(eglConfig));
        memset(config, 0xff, sizeof(eglConfig));

        config->surface_type = EGL_DONT_CARE;

        config->bind_to_tex_rgb = EGL_FALSE;  // 暂不支持
        config->bind_to_tex_rgba = EGL_FALSE; // 暂不支持
        config->transparent_type = EGL_NONE;
        config->color_buffer_type = EGL_RGB_BUFFER;
        config->surface_type = (EGL_WINDOW_BIT | EGL_PBUFFER_BIT);

        config->native_renderable = EGL_FALSE;
        config->recordable_android = EGL_FALSE;
        config->pixel_format = NULL;
        config->frame_buffer_level = 0;
        config->luminance_size = 0;
        config->depth_size = 0;
        config->stencil_size = 0;
        config->luminance_size = 0;
        config->max_pbuffer_width = PBUFFER_MAX_WIDTH;
        config->max_pbuffer_height = PBUFFER_MAX_HEIGHT;
        config->max_pbuffer_size = PBUFFER_MAX_PIXELS;
        config->native_visual_id = 0;

        config->native_visual_type = EGL_NONE;
        config->caveat = EGL_NONE;

        config->min_swap_interval = MIN_SWAP_INTERVAL;
        config->max_swap_interval = MAX_SWAP_INTERVAL;
        config->renderable_type = RENDERABLE_SUPPORT;

        config->samples_per_pixel = 0;
        config->sample_buffers_num = 0 > 0 ? 1 : 0;

        set_val_by_enum(config, red_sizes[i], EGL_RED_SIZE);
        set_val_by_enum(config, green_sizes[i], EGL_GREEN_SIZE);
        set_val_by_enum(config, blue_sizes[i], EGL_BLUE_SIZE);
        set_val_by_enum(config, alpha_sizes[i], EGL_ALPHA_SIZE);
        set_val_by_enum(config, 0, EGL_LUMINANCE_SIZE);

        if (!add_config(display, config))
        {
            g_free(config);
        }
    }
}