/**
 * @file egl_surface.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief surface决定到底绘制是个啥样的逻辑，swap时是个啥样的逻辑
 * @version 0.1
 * @date 2021-08-01
 *
 * @copyright Copyright (c) 2021
 *
 */

// #define STD_DEBUG_LOG
// #define TIMER_LOG
#include "hw/vsoc/gpu/egl_surface.h"
#include "hw/vsoc/gpu/egl_display.h"
#include "hw/vsoc/gpu/egl_draw.h"

#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/gpu/express_gpu.h"
#include "hw/vsoc/gpu/glv3_resource.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/gpu/express_gpu_snapshot.h"


Window_Buffer *render_surface_create(EGLConfig eglconfig, int width, int height, int surface_type);

void egl_surface_swap_buffer(void *render_context, Window_Buffer *surface, uint64_t gbuffer_id, int width, int height, int hal_format)
{
 
    Render_Thread_Context *thread_context = (Render_Thread_Context *)render_context;
    Opengl_Context *opengl_context = (Opengl_Context *)(thread_context->opengl_context);

    Hardware_Buffer *now_draw_gbuffer = surface->gbuffer;

    LOGD("surface %llx swapbuffer gbuffer_id %llx sync %d", (uint64_t)surface, now_draw_gbuffer->gbuffer_id, now_draw_gbuffer->data_sync);

    Hardware_Buffer *next_draw_gbuffer = NULL;

    if (surface->type == WINDOW_SURFACE)
    {
        next_draw_gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        if (next_draw_gbuffer == NULL)
        {
            LOGD("create gbuffer_id %" PRIx64 " when surface %p swapbuffer context %p width %d height %d", gbuffer_id, surface, opengl_context, width, height);
            next_draw_gbuffer = create_gbuffer_from_hal(width, height, hal_format, surface, gbuffer_id);

            LOGD("create gbuffer when swapbuffer gbuffer %" PRIx64 " ptr %p", gbuffer_id, next_draw_gbuffer);

            add_gbuffer_to_global(next_draw_gbuffer);
        }
        else
        {
        }
    }
    else
    {
        next_draw_gbuffer = now_draw_gbuffer;
    }
    LOGD("going to swapbuffer next gbuffer %llx %d", next_draw_gbuffer->gbuffer_id, next_draw_gbuffer->data_texture);
    connect_gbuffer_to_surface(next_draw_gbuffer, surface, opengl_context->framebuffer_map);

    GLuint glerror = glGetError();
    if (glerror != GL_NO_ERROR)
    {
        LOGE("error! swapbuffer connect_gbuffer_to_surface glGetError %x %llx %d", glerror, next_draw_gbuffer->gbuffer_id, next_draw_gbuffer->data_texture);
    }

    surface->gbuffer = next_draw_gbuffer;

    if (surface->gbuffer->sampler_num > 1)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->gbuffer->sampler_fbo);
    }
    else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, surface->gbuffer->data_fbo);
    }
    LOGD("in swapbuffer bind gbuffer %x fbo %d", surface->gbuffer->gbuffer_id, surface->gbuffer->data_fbo);
    glerror = glGetError();
    if (glerror != GL_NO_ERROR)
    {
        LOGE("error! swapbuffer glBindFramebuffer GL_DRAW_FRAMEBUFFER glGetError %x %d", glerror, surface->gbuffer->data_fbo);
    }

    return;
}

Window_Buffer *render_surface_create(EGLConfig eglconfig, int width, int height, int surface_type)
{
    //@todo 处理config、处理attrib_list

    // 这里先根据attrb_list获取窗口的宽和高
    Window_Buffer *surface = g_malloc0(sizeof(Window_Buffer));
    surface->type = surface_type;
    surface->width = width;
    surface->height = height;
    surface->swap_interval = 1;

    eglConfig *config = config_to_hints(eglconfig, &surface->window_hints);
    surface->config = config;

    EGLint internal_format = GL_RGB;
    EGLenum format = GL_RGB;
    EGLenum type = GL_UNSIGNED_BYTE;

    EGLenum depth_internal_format = 0;
    EGLenum stencil_internal_format = 0;

    EGLint red_bits = config->red_size;
    EGLint green_bits = config->green_size;
    EGLint blue_bits = config->blue_size;
    EGLint alpha_bits = config->alpha_size;
    EGLint stencil_bits = config->stencil_size;
    EGLint depth_bits = config->depth_size;
    if (config->sample_buffers_num != 0)
    {
        if (config->sample_buffers_num == -1)
        {
            config->sample_buffers_num = 0;
        }
    }
    EGLint sampler_num = config->samples_per_pixel;

    // express_printf("rgba %d %d %d %d ds %d %d MSAA %dX\n", red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits, sampler_num);

    // 2222
    // 3320
    // 4440
    // 4444
    // 5550
    // 5551
    // 5650
    // 8000
    // 8800
    // 8880
    // 8888
    // 1010100
    // 1010102
    // 1212120
    // 12121212
    // 1616160
    // 16161616
    if (red_bits == 2 && green_bits == 2 && blue_bits == 2 && alpha_bits == 2)
    {
        // 2222
        internal_format = GL_RGBA2;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 3 && green_bits == 3 && blue_bits == 2 && alpha_bits == 0)
    {
        // 3320
        internal_format = GL_R3_G3_B2;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 0)
    {
        // 4440
        internal_format = GL_RGB4;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 4 && green_bits == 4 && blue_bits == 4 && alpha_bits == 4)
    {
        // 4444
        internal_format = GL_RGBA4;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 0)
    {
        // 5550
        internal_format = GL_RGB5;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 5 && green_bits == 5 && blue_bits == 5 && alpha_bits == 1)
    {
        // 5551
        internal_format = GL_RGB5_A1;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 5 && green_bits == 6 && blue_bits == 5 && alpha_bits == 0)
    {
        // 5650
        internal_format = GL_RGB565;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 0 && blue_bits == 0 && alpha_bits == 0)
    {
        // 8000
        internal_format = GL_R8;
        format = GL_RED;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 0 && alpha_bits == 0)
    {
        // 8800
        internal_format = GL_RG8;
        format = GL_RG;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 0)
    {
        // 8880
        internal_format = GL_RGB8;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 8 && green_bits == 8 && blue_bits == 8 && alpha_bits == 8)
    {
        // 8888
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 0)
    {
        // 1010100
        internal_format = GL_RGB10;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 10 && green_bits == 10 && blue_bits == 10 && alpha_bits == 2)
    {
        // 1010102
        internal_format = GL_RGB10_A2;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT_2_10_10_10_REV;
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 0)
    {
        // 1212120
        internal_format = GL_RGB12;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 12 && green_bits == 12 && blue_bits == 12 && alpha_bits == 12)
    {
        // 1212120
        internal_format = GL_RGBA12;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 0)
    {
        // 1616160
        internal_format = GL_RGB16;
        format = GL_RGB;
        type = GL_UNSIGNED_INT;
    }
    else if (red_bits == 16 && green_bits == 16 && blue_bits == 16 && alpha_bits == 16)
    {
        // 1616160
        internal_format = GL_RGBA16;
        format = GL_RGBA;
        type = GL_UNSIGNED_INT;
    }
    else
    {
        // express_printf("choose rgba default ");
    }

    // LOGI("%llx surface choose red %d green %d blue %d alpha %d depth %d stencil %d width %d height %d", surface, red_bits, green_bits, blue_bits, alpha_bits, depth_bits, stencil_bits, surface->width, surface->height);

    if (internal_format == GL_RGB565)
    {
        internal_format = GL_RGB8;
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
    }

    if (depth_bits == 16)
    {
        depth_internal_format = GL_DEPTH_COMPONENT16;
    }
    else if (depth_bits == 24)
    {
        depth_internal_format = GL_DEPTH_COMPONENT24;
    }
    else if (depth_bits == 32)
    {
        depth_internal_format = GL_DEPTH_COMPONENT32F;
    }

    if (stencil_bits == 8)
    {
        stencil_internal_format = GL_STENCIL_INDEX8;
        if (depth_internal_format == GL_DEPTH_COMPONENT24 || depth_internal_format == GL_DEPTH_COMPONENT16 || depth_internal_format == GL_DEPTH_COMPONENT32F)
        {
            depth_internal_format = GL_DEPTH24_STENCIL8;
        }
    }

    surface->sampler_num = sampler_num;
    surface->format = format;
    surface->internal_format = internal_format;
    surface->pixel_type = type;
    surface->depth_internal_format = depth_internal_format;
    surface->stencil_internal_format = stencil_internal_format;

    LOGD("create surface %llx\n", (uint64_t)surface);

    return surface;
}

void render_surface_init(Window_Buffer *surface, GHashTable* resource_list)
{
    int num = 1;
    if (surface->type == WINDOW_SURFACE)
    {
        num = 3;
    }
    if (surface->data_fbo[0] != 0)
    {
        LOGD("fbo already init %d", surface->data_fbo[0]);
        return;
    }
    glGenFramebuffers(num, surface->data_fbo);
    LOGD("create fbo in init surface of num %d %d %d %d", num, surface->data_fbo[0], surface->data_fbo[1], surface->data_fbo[2]);

    // resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];
    surface->framebuffer_map = resource_list;
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);
    for (int i = 0; i < num; i++)
    {
       
        // GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];
        // if(g_hash_table_lookup(resource_list, GUINT_TO_POINTER(surface->data_fbo[i])) == NULL) {
        //     Express_Native_Framebuffer* newFramebuffer = g_malloc0(sizeof(Express_Native_Framebuffer));
        //     newFramebuffer->framebufferId = surface->data_fbo[i];
        //     g_hash_table_insert(resource_list, GUINT_TO_POINTER(surface->data_fbo[i]), newFramebuffer);
        // }
        // GHashTable* resource_list = opengl_context->framebuffer_map;
        // if(surface->data_fbo[i] == 1)continue;
        if(g_hash_table_lookup(resource_list, GUINT_TO_POINTER(surface->data_fbo[i])) == NULL) {
            Express_Native_Framebuffer* newFramebuffer = g_malloc0(sizeof(Express_Native_Framebuffer));
            newFramebuffer->framebufferId = surface->data_fbo[i];
            LOGD("in render_surface_init save framebuffer of id %d", surface->data_fbo[i]);
            g_hash_table_insert(resource_list, GUINT_TO_POINTER(surface->data_fbo[i]), newFramebuffer);

            // if(surface->data_fbo[i] == 1){
            //     newFramebuffer = g_malloc0(sizeof(Express_Native_Framebuffer));
            //     newFramebuffer->framebufferId = 2;
            //     newFramebuffer->attachment_target = GL_COLOR_ATTACHMENT0;
            //     newFramebuffer->texture_id = 3;
            //     g_hash_table_insert(resource_list, GUINT_TO_POINTER(2), newFramebuffer);
            // }
        }
        
    }
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);


    if (surface->sampler_num > 1)
    {
        glGenFramebuffers(num, surface->sampler_fbo);
        LOGD("in surface of fbo %d sampler %d", surface->data_fbo[0], surface->sampler_fbo[0]);
    }
}

void render_surface_uninit(Window_Buffer *surface, GHashTable* resource_list)
{
    int num = 1;
    if (surface->type == WINDOW_SURFACE)
    {
        num = 3;
    }
    if (surface->data_fbo[0] == 0)
    {
        return;
    }
    glDeleteFramebuffers(num, surface->data_fbo);

    LOGD("delete fbo in uninit surface %d %d %d", surface->data_fbo[0], surface->data_fbo[1], surface->data_fbo[2]);

    // resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];
    if(!resource_list) {
        resource_list = surface->framebuffer_map;
        LOGD("get old resource list of %d", resource_list);
    }
    for (int i = 0; i < num; i++)
    {
        ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);
        // GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];
        // GHashTable* resource_list = opengl_context->framebuffer_map;
        LOGD("in render surface uninit of map size %d", g_hash_table_size(resource_list));
        Express_Native_Framebuffer* current_buffer = g_hash_table_lookup(resource_list, GUINT_TO_POINTER(surface->data_fbo[i]));
        
        if(current_buffer != NULL) {
            LOGD("in render surface uninit delete %d", current_buffer->framebufferId);
            g_hash_table_remove(resource_list, GUINT_TO_POINTER(surface->data_fbo[i]));

        } else {
            LOGE("error! can't find framebuffer of %d", surface->data_fbo[i]);
        }
        ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);
    }        


    surface->data_fbo[0] = 0;
    surface->data_fbo[1] = 0;
    surface->data_fbo[2] = 0;
    surface->connect_texture[0] = 0;
    surface->connect_texture[1] = 0;
    surface->connect_texture[2] = 0;

    if (surface->sampler_num > 1)
    {
        glDeleteFramebuffers(num, surface->sampler_fbo);
        surface->sampler_fbo[0] = 0;
        surface->sampler_fbo[1] = 0;
        surface->sampler_fbo[2] = 0;
    }
}

int render_surface_destroy(Window_Buffer *surface)
{
    if (surface->type == P_SURFACE && surface->gbuffer != NULL)
    {
        send_message_to_main_window(MAIN_DESTROY_GBUFFER, surface->gbuffer);
    }
    express_printf("free surface %llx\n", (uint64_t)surface);
    g_free(surface);

    return 1;
}

void d_eglIamComposer(void *context, EGLSurface surface, unsigned int pid)
{
    preload_static_context_value->composer_pid = pid;
}

void d_eglCreatePbufferSurface(void *context, EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    int i = 0;
    int width = 0;
    int height = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            height = attrib_list[i + 1];
            break;
        default:
            // todo 其他attrib属性的设置
            break;
        }
        i += 2;
    }

    // guest端如果最后没用PbufferSurface渲染的话，width和height可以为0
    // 但如果host端这里为0的话，到时候对应gbuffer的data_texture宽高就会是0，无法绑定到FBO上去了
    // 所以这里设置成了1
    if (width == 0) {
        width = 1;
    }

    if (height == 0) {
        height = 1;
    }

    Window_Buffer *host_surface = render_surface_create(config, width, height, P_SURFACE);
    host_surface->guest_surface = guest_surface;

    LOGD("pbuffer surface create host %p guest %p width %d height %d guest width %d height %d", host_surface, guest_surface, host_surface->width, host_surface->height, width, height);

    g_hash_table_insert(process_context->surface_map, GUINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

void d_eglCreateWindowSurface(void *context, EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list, EGLSurface guest_surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    int i = 0;
    int width = 0;
    int height = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            height = attrib_list[i + 1];
            break;
        default:
            // todo 其他attrib属性的设置
            LOGI("window_surface attrib_list %x %x", attrib_list[i], attrib_list[i + 1]);
            break;
        }
        i += 2;
    }

    // windowSurface其实不需要宽和高

    Window_Buffer *host_surface = render_surface_create(config, width, height, WINDOW_SURFACE);
    host_surface->guest_surface = guest_surface;
    LOGD("surface create host %p guest %p width %d height %d guest width %d height %d", host_surface, guest_surface, host_surface->width, host_surface->height, width, height);
    g_hash_table_insert(process_context->surface_map, GUINT_TO_POINTER(guest_surface), (gpointer)host_surface);
}

EGLBoolean d_eglDestroySurface(void *context, EGLDisplay dpy, EGLSurface surface)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    Window_Buffer *real_surface = (Window_Buffer *)g_hash_table_lookup(process_context->surface_map, GUINT_TO_POINTER(surface));
    if (real_surface == NULL)
    {
        return EGL_FALSE;
    }

    // 会调用到 surface_map 的删除函数 g_surface_map_destroy
    g_hash_table_remove(process_context->surface_map, GUINT_TO_POINTER(surface));

    express_printf("destroy surface host %llx guest %llx\n", (uint64_t)real_surface, (uint64_t)surface);
    return EGL_TRUE;
}

Hardware_Buffer *create_gbuffer_with_context(int width, int height, int hal_format, void *t_context, EGLContext ctx, uint64_t gbuffer_id)
{
    static void *temp_window;
    static int temp_window_lock = 0;

    Render_Thread_Context *thread_context = (Render_Thread_Context *)t_context;

    if (thread_context->opengl_context == NULL) {
        LOGD("create gbuffer: no current opengl context, using temporary window");
        if (temp_window == NULL) {
            temp_window = get_native_opengl_context(0);
        }
        ATOMIC_LOCK(temp_window_lock);
        egl_makeCurrent(temp_window);
    }

    Hardware_Buffer *gbuffer = create_gbuffer_from_hal(width, height, hal_format, NULL, gbuffer_id);

    if (thread_context->opengl_context == NULL)
    {
        egl_makeCurrent(NULL);
        ATOMIC_UNLOCK(temp_window_lock);
    }

    return gbuffer;
}

Hardware_Buffer *create_gbuffer_from_hal(int width, int height, int hal_format, Window_Buffer *surface, uint64_t gbuffer_id)
{

    int sampler_num = 0;
    int format = GL_RGBA;
    int pixel_type = GL_UNSIGNED_INT;
    int internal_format = GL_RGBA8;
    int depth_internal_format = 0;
    int stencil_internal_format = 0;
    if (surface != NULL)
    {
        sampler_num = surface->sampler_num;
        format = surface->format;
        pixel_type = surface->pixel_type;
        internal_format = surface->internal_format;
        depth_internal_format = surface->depth_internal_format;
        stencil_internal_format = surface->stencil_internal_format;
    }

    if (hal_format == EXPRESS_PIXEL_RGBA8888 || hal_format == EXPRESS_PIXEL_RGBX8888)
    {
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_BYTE;
    }
    else if (hal_format == EXPRESS_PIXEL_RGB888 || hal_format == EXPRESS_PIXEL_YUV420888)
    {
        internal_format = GL_RGB8;
        format = GL_RGB;
        pixel_type = GL_UNSIGNED_BYTE;
    }
    else if (hal_format == EXPRESS_PIXEL_RGB565)
    {
        internal_format = GL_RGB565;
        format = GL_RGB;
        pixel_type = GL_UNSIGNED_SHORT_5_6_5;
    }
    else if (hal_format == EXPRESS_PIXEL_RGBA5551 || hal_format == EXPRESS_PIXEL_RGBX5551)
    {
        internal_format = GL_RGB5_A1;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_SHORT_5_5_5_1;
    }
    else if (hal_format == EXPRESS_PIXEL_RGBA4444 || hal_format == EXPRESS_PIXEL_RGBX4444)
    {
        internal_format = GL_RGBA4;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_SHORT_4_4_4_4;
    }
    else if (hal_format == EXPRESS_PIXEL_BGRA8888 || hal_format == EXPRESS_PIXEL_BGRX8888)
    {
        // LOGI("EGLImage with g_buffer_id %llx need format BGRA_8888!!!", (uint64_t)g_buffer_id);
        internal_format = GL_RGBA8;
        format = GL_BGRA;
        pixel_type = GL_UNSIGNED_INT_8_8_8_8_REV;
    }
    else if (hal_format == EXPRESS_PIXEL_BGR565)
    {
        internal_format = GL_RGB565;
        format = GL_BGR;
        pixel_type = GL_UNSIGNED_SHORT_5_6_5_REV;
    }
    else if (hal_format == EXPRESS_PIXEL_RGBA1010102)
    {
        internal_format = GL_RGB10_A2;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_INT_2_10_10_10_REV;
    }
    else if (hal_format == EXPRESS_PIXEL_R8)
    {
        internal_format = GL_R8;
        format = GL_RED;
        pixel_type = GL_UNSIGNED_BYTE;
    }
    else
    {
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_INT;
        LOGE("error! unknown gralloc format %d!!!", hal_format);
    }

    return create_gbuffer(width, height, sampler_num,
                          format,
                          pixel_type,
                          internal_format,
                          depth_internal_format,
                          stencil_internal_format,
                          gbuffer_id);
}

Hardware_Buffer *create_gbuffer_from_surface(Window_Buffer *surface)
{
    return create_gbuffer(surface->width, surface->height, surface->sampler_num,
                          surface->format,
                          surface->pixel_type,
                          surface->internal_format,
                          surface->depth_internal_format,
                          surface->stencil_internal_format,
                          0);
}

Hardware_Buffer *create_gbuffer(int width, int height, int sampler_num,
                               int format,
                               int pixel_type,
                               int internal_format,
                               int depth_internal_format,
                               int stencil_internal_format,
                               uint64_t gbuffer_id)
{
    Hardware_Buffer *gbuffer = g_malloc0(sizeof(Hardware_Buffer));

    gbuffer->usage_type = GBUFFER_TYPE_WINDOW;
    gbuffer->gbuffer_id = gbuffer_id;

    GLuint pre_vbo = 0;
    GLuint pre_texture = 0;
    GLuint pre_fbo_draw = 0;
    GLuint pre_fbo_read = 0;
    GLuint pre_rbo = 0;
    GLuint pre_unpack_buffer = 0;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&pre_vbo);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&pre_texture);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint *)&pre_rbo);

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_draw);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_read); //保存当前绑定的，便于最后恢复

    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, (GLint *)&pre_unpack_buffer);

    glGenTextures(1, &(gbuffer->data_texture)); //创建一个fbo，然后它的颜色缓冲用的是texture，深度和模板缓冲用的是fbo

    glGenRenderbuffers(1, &(gbuffer->rbo_depth));
    glGenRenderbuffers(1, &(gbuffer->rbo_stencil));

    if (sampler_num > 1)
    {
        glGenRenderbuffers(1, &(gbuffer->sampler_rbo));
    }

    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);

    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
    GHashTable *resource_list = g_resource_list[RESOURCE_TYPE_TEXTURE];
    if(g_hash_table_lookup(resource_list, GUINT_TO_POINTER(gbuffer->data_texture)) == NULL) {
        struct Express_Native_Texture_Simple* texture_resource = g_malloc0(sizeof(Express_Native_Texture_Simple));
        texture_resource->target = GL_TEXTURE_2D;
        texture_resource->textureId = gbuffer->data_texture; 
        LOGD("in create_gbuffer save texture host id %d target %d", texture_resource->textureId, texture_resource->target);
        g_hash_table_insert(resource_list, GUINT_TO_POINTER(gbuffer->data_texture), texture_resource);            
    }
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (g_ops.express_gpu_gl_debug_enable)
    {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            LOGE("error when creating gbuffer1 init error %x", error);
        }
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, pixel_type, NULL);

    if (g_ops.express_device_input_window_enable)
    {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            LOGE("error when creating gbuffer1 %x width %d height %d format %x pixel_type %x ", error, width, height, format, pixel_type);
        }
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    static int max_sampler_num = -1;
    if (max_sampler_num == -1)
    {
        glGetInternalformativ(GL_RENDERBUFFER, GL_RGB, GL_SAMPLES, 1, &max_sampler_num);
    }

    if (max_sampler_num < sampler_num)
    {
        express_printf("over large sampler num %d max %d\n", sampler_num, max_sampler_num);
        sampler_num = max_sampler_num;
    }

    if (sampler_num > 1)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->sampler_rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, internal_format, width, height);
    }

    if (depth_internal_format != 0)
    {
        // 这个相当于给与一个深度缓冲区，让这个fbo可以有颜色缓冲区，有深度缓冲区，模板缓冲区
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->rbo_depth);
        if (sampler_num > 1)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, depth_internal_format, width, height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, depth_internal_format, width, height);
        }
    }

    // 之所以当深度24模板8时要合并，是因为这样效率更高
    if (stencil_internal_format != 0 && depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->rbo_stencil);
        if (sampler_num > 1)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampler_num, stencil_internal_format, width, height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, stencil_internal_format, width, height);
        }
    }

    if (g_ops.express_gpu_gl_debug_enable)
    {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            LOGE("error when creating gbuffer2 %x", error);
        }
    }

    glBindTexture(GL_TEXTURE_2D, pre_texture);
    glBindBuffer(GL_ARRAY_BUFFER, pre_vbo);
    glBindRenderbuffer(GL_RENDERBUFFER, pre_rbo);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pre_unpack_buffer);

    gbuffer->format = format;
    gbuffer->pixel_type = pixel_type;
    gbuffer->internal_format = internal_format;
    // gbuffer->row_byte_len = row_byte_len;
    gbuffer->depth_internal_format = depth_internal_format;
    gbuffer->stencil_internal_format = stencil_internal_format;

    gbuffer->width = width;
    gbuffer->height = height;
    gbuffer->sampler_num = sampler_num;

    gbuffer->pixel_size = pixel_size_calc(format, pixel_type);
    gbuffer->stride = width * gbuffer->pixel_size;
    gbuffer->size = width * height * gbuffer->pixel_size;

    LOGD("create gbuffer id %" PRIx64 " texture %d w %d h %d format 0x%x", gbuffer->gbuffer_id, gbuffer->data_texture, gbuffer->width, gbuffer->height, gbuffer->format);

    return gbuffer;
}

void reverse_gbuffer(Hardware_Buffer *gbuffer)
{
    if (gbuffer->reverse_rbo == 0)
    {
        GLuint pre_rbo = 0;

        glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint *)&pre_rbo);

        glGenRenderbuffers(1, &(gbuffer->reverse_rbo));
        glBindRenderbuffer(GL_RENDERBUFFER, gbuffer->reverse_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, gbuffer->internal_format, gbuffer->width, gbuffer->height);

        glBindRenderbuffer(GL_RENDERBUFFER, pre_rbo);
    }

    GLuint pre_fbo_draw = 0;
    GLuint pre_fbo_read = 0;

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_draw);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint *)&pre_fbo_read);

    // 因为这种情况出现的太少了，因为普通应用根本没有权限自己建个bufferQueue，因此这里就直接创建一个临时的fbo，用完就丢弃了
    GLuint temp_fbo;
    glGenFramebuffers(1, &temp_fbo);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, temp_fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gbuffer->reverse_rbo);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->data_fbo);

    glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, gbuffer->height, gbuffer->width, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer->data_fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, temp_fbo);

    glBlitFramebuffer(0, 0, gbuffer->width, gbuffer->height, 0, 0, gbuffer->width, gbuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glDeleteFramebuffers(1, &temp_fbo);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, pre_fbo_read);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pre_fbo_draw);
}

void connect_gbuffer_to_surface(Hardware_Buffer *gbuffer, Window_Buffer *surface, GHashTable* resource_list)
{
    LOGD("connect gbuffer %llx texture %d to surface %llx fbo %d", gbuffer->gbuffer_id, gbuffer->data_texture, (uint64_t)surface, surface->data_fbo[surface->now_fbo_loc]);

    gbuffer->data_fbo = surface->data_fbo[surface->now_fbo_loc];
    gbuffer->sampler_fbo = surface->sampler_fbo[surface->now_fbo_loc];
    surface->gbuffer = gbuffer;

    LOGD("gbuffer data texture %d surface texture %d surface data fbo %d", gbuffer->data_texture, surface->connect_texture[surface->now_fbo_loc], surface->data_fbo[surface->now_fbo_loc]);

    if (gbuffer->has_connected_fbo == 1 && gbuffer->data_texture == surface->connect_texture[surface->now_fbo_loc])
    {
        if (surface->type == WINDOW_SURFACE)
        {
            surface->now_fbo_loc = (surface->now_fbo_loc + 1) % 3;
        }

        GLuint current_fbo = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *)&current_fbo);
        LOGD("gbuffer %llx already connected to surface %llx current fbo %d", gbuffer->gbuffer_id, (uint64_t)surface, current_fbo);
        return;
    }
    LOGD("connect surface %llx fbo %d to gbuffer %llx", surface, surface->data_fbo[surface->now_fbo_loc], gbuffer->gbuffer_id);

    if (surface->sampler_num > 1)
    {
        // 窗口不需要开启多采样，只需要fbo开启就行
        glEnable(GL_MULTISAMPLE);
    }
    else
    {
        glDisable(GL_MULTISAMPLE);
    }

    glGetError();
    glBindFramebuffer(GL_FRAMEBUFFER, surface->data_fbo[surface->now_fbo_loc]);
    GLint error = glGetError();
    if (error != GL_NO_ERROR)
    {
        LOGE("error! binding surface framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->data_fbo[surface->now_fbo_loc], gbuffer->data_texture);
    }

    // 附加颜色缓冲区
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->data_texture, 0);

    error = glGetError();
    if(error!=GL_NO_ERROR) {
        LOGE("error! surface framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->data_fbo[surface->now_fbo_loc], gbuffer->data_texture);
    }

    // resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);
    // GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];
    // GHashTable* resource_list = opengl_context->framebuffer_map;

    Express_Native_Framebuffer* oldFramebuffer = g_hash_table_lookup(resource_list, GUINT_TO_POINTER(surface->data_fbo[surface->now_fbo_loc]));
    if(oldFramebuffer != NULL) {
        // Express_Native_Framebuffer* newFramebuffer = g_malloc0(sizeof(Express_Native_Framebuffer));
        oldFramebuffer->framebufferId = surface->data_fbo[surface->now_fbo_loc];
        // oldFramebuffer->texture_id = gbuffer->data_texture;
        // oldFramebuffer->attachment_target = GL_COLOR_ATTACHMENT0;
        oldFramebuffer->attachment_target[0] = gbuffer->data_texture;
        // g_hash_table_insert(resource_list, GUINT_TO_POINTER(surface->data_fbo[surface->now_fbo_loc]), oldFramebuffer);
        LOGD("update framebuffer info of id %d texture %d type %d", oldFramebuffer->framebufferId, oldFramebuffer->texture_id, oldFramebuffer->attachment_target);
    } else {
        LOGE("error!! can't find framebuffer of %d", surface->data_fbo[surface->now_fbo_loc]);
    }
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);

    surface->connect_texture[surface->now_fbo_loc] = gbuffer->data_texture;
    gbuffer->has_connected_fbo = 1;

    if (surface->sampler_num > 1 && gbuffer->sampler_num > 1)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, surface->sampler_fbo[surface->now_fbo_loc]);
        // 附加颜色缓冲区
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gbuffer->sampler_rbo);
        // LOGI("connect sampler fbo %d to gbuffer %llx", surface->sampler_fbo[surface->now_fbo_loc], gbuffer->gbuffer_id);
        error = glGetError();
        if(error!=GL_NO_ERROR) {
            LOGE("error! sampler framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->sampler_fbo[surface->now_fbo_loc], gbuffer->data_texture);
        }
    }

    // 附加深度缓冲区
    if (surface->depth_internal_format == GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer->rbo_depth);
        // LOGI("connect depth fbo %d to gbuffer %llx", gbuffer->rbo_depth, gbuffer->gbuffer_id);
        error = glGetError();
        if(error!=GL_NO_ERROR) {
            LOGE("error! depth framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->sampler_fbo[surface->now_fbo_loc], gbuffer->data_texture);
        }
    }
    else if (surface->depth_internal_format != 0)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gbuffer->rbo_depth);
        // LOGI("connect depth fbo %d to gbuffer %llx", gbuffer->rbo_depth, gbuffer->gbuffer_id);
        error = glGetError();
        if(error!=GL_NO_ERROR) {
            LOGE("error! depth framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->sampler_fbo[surface->now_fbo_loc], gbuffer->data_texture);
        }
    }

    // 附加模板缓冲区
    if (surface->stencil_internal_format != 0 && surface->depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer->rbo_stencil);
        LOGD("connect stencil current fbo %d fbo %d to gbuffer %llx",surface->data_fbo[surface->now_fbo_loc], gbuffer->rbo_stencil, gbuffer->gbuffer_id);
        error = glGetError();
        if(error!=GL_NO_ERROR) {
            LOGE("error! stencil framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->sampler_fbo[surface->now_fbo_loc], gbuffer->data_texture);
        }
    }
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    error = glGetError();
    if(error!=GL_NO_ERROR) {
        LOGE("error! framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->sampler_fbo[surface->now_fbo_loc], gbuffer->data_texture);
    }

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGE("error! surface framebuffer not complete! status %x gl error %x ", status, glGetError());
        LOGI("format %x pixel_type %x internal_format %x depth_internal_format %x stencil_internal_format %x", surface->format, surface->pixel_type, surface->internal_format, surface->depth_internal_format, surface->stencil_internal_format);

        LOGI("data texture %d: ", gbuffer->data_texture);
        glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
        int dims[2] = {0, 0};
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, dims);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, dims + 1);
        LOGI("width %d height %d ", dims[0], dims[1]);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, dims);
        LOGI("internal_format %x", dims[0]);
        
        LOGI("");
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (surface->type == WINDOW_SURFACE)
    {
        surface->now_fbo_loc = (surface->now_fbo_loc + 1) % 3;
    }

    error = glGetError();
    if(error!=GL_NO_ERROR) {
        LOGE("error! framebuffer not complete! gl error %x framebuffer %d texture %d", error, surface->sampler_fbo[surface->now_fbo_loc], gbuffer->data_texture);
    }

    return;
}

GLuint gbuffer_make_data_fbo(Hardware_Buffer *gbuffer) {
    if (gbuffer->data_fbo != 0) {
        LOGD("gbuffer %llx already has data fbo %d", gbuffer->gbuffer_id, gbuffer->data_fbo);
        return gbuffer->data_fbo;
    }

    // save previous FBO
    GLuint prev_fbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint *)&prev_fbo);

    glGenFramebuffers(1, &gbuffer->data_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer->data_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer->data_texture, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prev_fbo);

    return gbuffer->data_fbo; 
}

void destroy_gbuffer(Hardware_Buffer *gbuffer)
{
    LOGD("terminate gbuffer id %" PRIx64 " w %d h %d", gbuffer->gbuffer_id, gbuffer->width, gbuffer->height);

    if (gbuffer->data_texture != 0)
    {
        glDeleteTextures(1, &(gbuffer->data_texture));
        // LOGD("in delete gbuffers delete texture %d", gbuffer->data_texture);
        ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
        GHashTable *resource_list = g_resource_list[RESOURCE_TYPE_TEXTURE];
        if(g_hash_table_lookup(resource_list, GUINT_TO_POINTER(gbuffer->data_texture)) != NULL) {
            g_hash_table_remove(resource_list, GUINT_TO_POINTER(gbuffer->data_texture));
        }
        ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
    }
    // gbuffer->data_fbo是surface产生的，与context强相关，不能在这里销毁

    if (gbuffer->sampler_rbo != 0)
    {
        glDeleteRenderbuffers(1, &(gbuffer->sampler_rbo));
    }
    if (gbuffer->rbo_depth != 0)
    {
        glDeleteRenderbuffers(1, &(gbuffer->rbo_depth));
    }
    if (gbuffer->rbo_stencil != 0)
    {
        glDeleteRenderbuffers(1, &(gbuffer->rbo_stencil));
    }

    if (gbuffer->data_sync != 0)
    {
        glDeleteSync(gbuffer->data_sync);
    }

    if (gbuffer->delete_sync != 0)
    {
        glDeleteSync(gbuffer->delete_sync);
    }

    if (gbuffer->guest_data != NULL)
    {
        free_duplicated_guest_mem(gbuffer->guest_data);
    }

    if (gbuffer->host_data != NULL) {
        g_free(gbuffer->host_data);
    }

    g_free(gbuffer);
}

EGLBoolean d_eglSurfaceAttrib(void *context, EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    return EGL_TRUE;
}

EGLint d_eglCreateImage(void *context, EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer depressed_buffer, const EGLint *attrib_list, EGLImage guest_image)
{
    // 创建image，要么是使用别的应用绘制使用的缓冲区，要么是新创建的缓冲区
    // 前者之前肯定有surface连接，所以肯定找得到，后者不会找得到，必须得给手动建立一个

    // 这里buffer和guest_image不一样，因为buffer可能是32位应用传过来的，所以会被截断，导致和gbuffer_id不一样，所以这里直接重命名为depressed_buffer

    if (attrib_list == NULL)
    {
        return -1;
    }

    int width = 1;
    int height = 1;
    int hal_format = 0;
    int i = 0;
    while (attrib_list != NULL && attrib_list[i] != EGL_NONE)
    {
        switch (attrib_list[i])
        {
        case EGL_WIDTH:
            width = attrib_list[i + 1];
            break;
        case EGL_HEIGHT:
            height = attrib_list[i + 1];
            break;
        case EGL_TEXTURE_FORMAT:
            hal_format = attrib_list[i + 1];
            break;
        default:
            // todo 其他attrib属性的设置
            break;
        }
        i += 2;
    }

    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    Process_Context *process_context = thread_context->process_context;

    uint64_t gbuffer_id = (uint64_t)guest_image;

    Hardware_Buffer *gbuffer = NULL;

    if (target == EGL_NATIVE_BUFFER_ANDROID || target == EGL_NATIVE_BUFFER_OHOS) //估计就是GL_TEXTURE_EXTERNAL_OES，创建gbuffer_type_window
    {
        LOGD("in eglcreateImage and target is android!");

        gbuffer = get_gbuffer_from_global_map(gbuffer_id);

        if (gbuffer == NULL) //大概出现十余次(普通应用)
        {
            LOGI("(%s) create image with gbuffer id %" PRIx64 " width %d height %d format %d process_context %" PRIx64, process_context->guest_process_name, gbuffer_id, width, height, hal_format, (uint64_t)process_context);
            gbuffer = create_gbuffer_with_context(width, height, hal_format, thread_context, ctx, gbuffer_id);

            if (gbuffer != NULL) {
                add_gbuffer_to_global(gbuffer);
            }
        }
    }
    else //桌面和简单应用使用运行不到这里来
    {
        LOGD("in eglcreateImage and target is %d", target);
        Opengl_Context *share_opengl_context = (Opengl_Context *)g_hash_table_lookup(process_context->context_map, GUINT_TO_POINTER(ctx));

        if (share_opengl_context == NULL)
        {
            LOGE("error! glCreateImage with null share_context");
            return 0;
        }

        // gbuffer_id直接截取后面4个字节就是share的texture
        GLuint host_share_texture = get_host_texture_id(share_opengl_context, (GLuint)gbuffer_id, 0);

        // 之所以这里没有判断是否存在gbuffer，是因为作为texture的情况下，gbuffer一定不存在。即同一个进程下同一个id的EGLImage不会创建两次
        gbuffer = g_malloc0(sizeof(Hardware_Buffer));
        gbuffer->usage_type = GBUFFER_TYPE_TEXTURE;
        gbuffer->data_texture = host_share_texture;
        gbuffer->gbuffer_id = gbuffer_id;

        // share texure模式下，eglCreateImage时，当前应该是有opengl上下文的，只是创建的这个image之后交给其他线程使用
        if (thread_context->opengl_context != share_opengl_context)
        {
            //假如现在opengl不对
            LOGI("eglCreateImage share texture get different opengl_context %llx share %llx is_current %d", (uint64_t)thread_context->opengl_context, (uint64_t)share_opengl_context, share_opengl_context->is_current);
            make_opengl_current(share_opengl_context, true);
        }
        gbuffer->data_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

        if (thread_context->opengl_context != share_opengl_context) {
            make_opengl_current(share_opengl_context, false);
            if (thread_context->opengl_context != NULL && thread_context->opengl_context->is_current) {
                make_opengl_current(thread_context->opengl_context, true);
            }
        }

        // texture类型的gbuffer不需要放到global表中，因为这个image只能在自己进程内共享
        g_hash_table_insert(process_context->gbuffer_map, (gpointer)(gbuffer_id), (gpointer)gbuffer);
    }
    express_printf("eglcreateImage gbuffer %llx target %x ptr %llx\n", gbuffer_id, target, gbuffer);

    return 1;
}

EGLBoolean d_eglDestroyImage(void *context, EGLDisplay dpy, EGLImage image)
{
    uint64_t gbuffer_id = (uint64_t)image;

    // LOGI("send destroy gbuffer %llx message",gbuffer->gbuffer_id);
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;

    Process_Context *process_context = thread_context->process_context;

    Hardware_Buffer *gbuffer = (Hardware_Buffer *)g_hash_table_lookup(process_context->gbuffer_map, (gpointer)(gbuffer_id));

    LOGD("destroyImage gbuffer %llx ptr %llx", gbuffer_id, gbuffer);

    if (gbuffer)
    {
        g_hash_table_remove(process_context->gbuffer_map, (gpointer)(gbuffer_id));
    }

    return EGL_TRUE;
}
