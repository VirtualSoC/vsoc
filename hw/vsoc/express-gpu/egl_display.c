// #define STD_DEBUG_LOG
#include "hw/express-gpu/egl_display.h"
#include "hw/teleport-express/express_log.h"

static const EGLint attr32_1[] = {
    EGL_RED_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    0
};

static const EGLint attr32_2[] = {
    EGL_RED_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    0
};

static const EGLint attr32_3[] = {
    EGL_RED_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    0
};

static const EGLint attr32_4[] = {
    EGL_RED_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    0
};

static const EGLint attr32_5[] = {
    EGL_RED_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_SAMPLES, 2,
    0
};

static const EGLint attr32_6[] = {
    EGL_RED_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_SAMPLES, 4,
    0
};

static const EGLint attr32_7[] = {
    EGL_RED_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    EGL_SAMPLES, 4,
    0
};

static const EGLint* const pixel_attrs[] = {
    attr32_1,
    attr32_2,
    attr32_3,
    attr32_4,
    attr32_5,
    attr32_6,
    attr32_7,
};

static int num_formats = sizeof(pixel_attrs) / sizeof(pixel_attrs[0]);

Egl_Display *default_egl_display;

static EGLint get_pixel_format_attrib(int index, EGLint attrib);
static void parse_pixel_format(Egl_Display *display, int index);

/**
 * @brief 初始化Egl_Display
 *
 * @param display_point 待初始化的Egl_Display
 */
void init_display(Egl_Display **display_point)
{
    default_egl_display = g_malloc0(sizeof(Egl_Display));

    init_configs(default_egl_display);

    default_egl_display->guest_ver_major = 1;
    default_egl_display->guest_ver_minor = 5;

    LOGD("egl display init, config size %d", g_hash_table_size(default_egl_display->egl_config_set));
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
    for (int idx = 1; idx <= num_formats; ++idx) {
        parse_pixel_format(display, idx);
    }

    add_window_independent_config(display, EGL_DEPTH_SIZE, depth_vals, NUM_DEPTH_VAL);
    add_window_independent_config(display, EGL_STENCIL_SIZE, stencil_vals, NUM_STENCILE_VAL);
    add_window_independent_config(display, EGL_SAMPLES, sample_vals, NUM_SAMPLE_VAL);
}

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

static void parse_pixel_format(Egl_Display *display, int index) {
    eglConfig *config = (eglConfig *)g_malloc0(sizeof(eglConfig));

    config->surface_type = EGL_WINDOW_BIT | EGL_PBUFFER_BIT;
    
    config->bind_to_tex_rgb = EGL_FALSE;  // 暂不支持
    config->bind_to_tex_rgba = EGL_FALSE; // 暂不支持

    config->native_visual_id = 0;
    config->native_visual_type = EGL_NONE;
    config->native_renderable = EGL_FALSE;
    config->caveat = EGL_NONE;

    config->min_swap_interval = MIN_SWAP_INTERVAL;
    config->max_swap_interval = MAX_SWAP_INTERVAL;

    config->renderable_type = RENDERABLE_SUPPORT;
    config->max_pbuffer_width = PBUFFER_MAX_WIDTH;
    config->max_pbuffer_height = PBUFFER_MAX_HEIGHT;
    config->max_pbuffer_size = PBUFFER_MAX_PIXELS;

    config->sample_buffers_num = config->samples_per_pixel > 0 ? 1 : 0;
    config->frame_buffer_level = 0;
    config->luminance_size = 0;
    config->buffer_size = 0;
    config->color_buffer_type = EGL_RGB_BUFFER;

    config->trans_red_val = 0;
    config->trans_green_val = 0;
    config->trans_blue_val = 0;
    config->transparent_type = EGL_NONE;

    config->alpha_size = get_pixel_format_attrib(index, EGL_ALPHA_SIZE);
    config->depth_size = get_pixel_format_attrib(index, EGL_DEPTH_SIZE);
    config->stencil_size = get_pixel_format_attrib(index, EGL_STENCIL_SIZE);
    config->red_size = get_pixel_format_attrib(index, EGL_RED_SIZE);
    config->green_size = get_pixel_format_attrib(index, EGL_GREEN_SIZE);
    config->blue_size = get_pixel_format_attrib(index, EGL_BLUE_SIZE);

    config->buffer_size = config->red_size + config->green_size + config->blue_size + config->alpha_size;
    config->conformant = ((config->buffer_size > 0) && (config->caveat != EGL_NON_CONFORMANT_CONFIG)) ? config->renderable_type : 0;
    config->pixel_format = 0;
    config->framebuffer_target_android = (config->buffer_size == 16 || config->buffer_size == 32) ? EGL_TRUE : EGL_FALSE;
    
    if (!add_config(display, config)) {
        g_free(config);
    }
}

static EGLint get_pixel_format_attrib(int index, EGLint attrib) {
    const EGLint *attrib_list = pixel_attrs[index - 1];
    while (*attrib_list)
    {
        if (*attrib_list == EGL_RED_SIZE || 
            *attrib_list == EGL_GREEN_SIZE || 
            *attrib_list == EGL_BLUE_SIZE ||
            *attrib_list == EGL_ALPHA_SIZE || 
            *attrib_list == EGL_DEPTH_SIZE || 
            *attrib_list == EGL_STENCIL_SIZE) {
            if (attrib == *attrib_list)
                return attrib_list[1];
            attrib_list += 2;
        } else {
            if (attrib == *attrib_list)
                return 1;
            attrib_list++;
        }
    }
    
    return 0;
}
