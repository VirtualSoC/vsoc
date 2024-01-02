// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"
#include "hw/teleport-express/express_device_common.h"

#include "hw/express-mem/express_mem.h"
#include "hw/express-gpu/express_sync.h"
#include "hw/express-gpu/express_gpu_render.h"
#include "hw/express-gpu/glv3_context.h"

#include "qemu/atomic.h"

static Thread_Context *g_context = NULL;
static GThreadPool *g_pool = NULL;

static void *g_gl_context = NULL;
static GLuint unpack_buffer;
static int unpack_buffer_size = 0;
static GLsync unpack_buffer_sync = NULL;


const char *memtype_to_str(ExpressMemType loc) {
    switch (loc) {
        case EXPRESS_MEM_TYPE_UNKNOWN:
            return "unknown";
        case EXPRESS_MEM_TYPE_GUEST_MEM:
            return "guest_mem";
        case EXPRESS_MEM_TYPE_GUEST_OPAQUE:
            return "guest_opaque";
        case EXPRESS_MEM_TYPE_GBUFFER:
            return "gbuffer";
        case EXPRESS_MEM_TYPE_HOST_MEM:
            return "host_mem";
        case EXPRESS_MEM_TYPE_HOST_OPAQUE:
            return "host_opaque";
        default:
            return "error";
    }
}

/**
 * update the gbuffer location
 * used at the beginning of every gbuffer access
 */
void update_gbuffer_location(Graphic_Buffer *gbuffer, ExpressMemType loc, int pid, int write) {
    if (gbuffer == NULL) {
        LOGE("update_gbuffer_location got null gbuffer!");
        return;
    }
    if (gbuffer->locations == NULL) {
        LOGE("gbuffer %" PRIx64 " location tracking not enabled!", gbuffer->gbuffer_id);
        return;
    }

    if (write) {
        gbuffer->location = loc;
        gbuffer->pid = pid;
        LOGD("gbuffer %" PRIx64 " pid %d write %s", gbuffer->gbuffer_id, gbuffer->pid, memtype_to_str(gbuffer->location));
        return;
    } // else read

    /**
     * identifier: 64-bit, format as follows.
     * 0 -------- 31 32 -------- 63 
     *     tid          location
     */
    uint64_t id = gbuffer->location | ((uint64_t)gbuffer->pid << 32);
    ExpressMemType pred_loc = (ExpressMemType)g_hash_table_lookup(gbuffer->locations, GUINT_TO_POINTER(id));
    if (loc == pred_loc) {
        LOGD("gbuffer %" PRIx64 " pid %d prefetch %s -> %s succ!", gbuffer->gbuffer_id, gbuffer->pid, memtype_to_str(gbuffer->location), memtype_to_str(pred_loc));
    }
    else {
        LOGD("gbuffer %" PRIx64 " pid %d prefetch %s -> %s (actual %s) fail!", gbuffer->gbuffer_id, gbuffer->pid, memtype_to_str(gbuffer->location), memtype_to_str(pred_loc), memtype_to_str(loc));
        g_hash_table_insert(gbuffer->locations, GUINT_TO_POINTER(id), GUINT_TO_POINTER(loc));
    }
}

/**
 * used at the end of every gbuffer access
*/
ExpressMemType predict_gbuffer_location(Graphic_Buffer *gbuffer) {
    if (gbuffer == NULL) {
        LOGE("predict_gbuffer_location got null gbuffer!");
        return EXPRESS_MEM_TYPE_UNKNOWN;
    }
    if (gbuffer->locations == NULL) {
        LOGE("gbuffer %" PRIx64 " location tracking not enabled!", gbuffer->gbuffer_id);
        return EXPRESS_MEM_TYPE_UNKNOWN;
    }
    uint64_t id = gbuffer->location | ((uint64_t)gbuffer->pid << 32);
    ExpressMemType pred_loc = (ExpressMemType)g_hash_table_lookup(gbuffer->locations, GUINT_TO_POINTER(id));
    LOGD("gbuffer %" PRIx64 " pid %d predict prefetch %s -> %s?", gbuffer->gbuffer_id, gbuffer->pid, memtype_to_str(gbuffer->location), memtype_to_str(pred_loc));
    return pred_loc;
}

static void mem_master_switch(Thread_Context *context, Teleport_Express_Call *call)
{
    char *ptr = NULL;
    size_t ptr_len = 0;
    int need_free = 0;
    Call_Para all_para[MAX_PARA_NUM];
    int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);

    switch (call->id) {

    case FUNID_Terminate_Gbuffer:
    {
        Gralloc_Gbuffer_Info info;

        if (unlikely(para_num < PARA_NUM_Terminate_Gbuffer))
        {
            break;
        }

        ptr_len = all_para[0].data_len;
        if (unlikely(ptr_len < sizeof(Gralloc_Gbuffer_Info)))
        {
            break;
        }

        ptr = call_para_to_ptr(all_para[0], &need_free);
        info = *(Gralloc_Gbuffer_Info *)(ptr);

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
                destroy_gbuffer(gbuffer);
            }
            LOGI("terminate gbuffer id %llx", info.gbuffer_id);
        }
    }
    break;
    case FUNID_Gbuffer_Host_To_Guest:
    {
        Gralloc_Gbuffer_Info info;

        if (unlikely(para_num < PARA_NUM_Gbuffer_Host_To_Guest))
        {
            break;
        }

        ptr_len = all_para[0].data_len;
        if (unlikely(ptr_len < sizeof(Gralloc_Gbuffer_Info)))
        {
            break;
        }

        ptr = call_para_to_ptr(all_para[0], &need_free);
        info = *(Gralloc_Gbuffer_Info *)(ptr);

        gbuffer_data_host_to_guest(info);
    }
    break;
    case FUNID_Gbuffer_Guest_To_Host:
    {
        Gralloc_Gbuffer_Info info;
        uint64_t sync_id;

        if (unlikely(para_num < PARA_NUM_Gbuffer_Guest_To_Host))
        {
            break;
        }

        ptr_len = all_para[0].data_len;
        if (unlikely(ptr_len < sizeof(Gralloc_Gbuffer_Info)))
        {
            break;
        }

        ptr = call_para_to_ptr(all_para[0], &need_free);
        info = *(Gralloc_Gbuffer_Info *)(ptr);

        if (need_free) {
            g_free(ptr);
        }

        ptr = call_para_to_ptr(all_para[1], &need_free);
        sync_id = *(uint64_t *)(ptr);

        gbuffer_data_guest_to_host(info, (int)(uint32_t)sync_id);
    }
    break;
    case FUNID_Alloc_Gbuffer:
    {
        Gralloc_Gbuffer_Info info;

        if (unlikely(para_num < PARA_NUM_Alloc_Gbuffer))
        {
            break;
        }

        ptr_len = all_para[0].data_len;
        if (unlikely(ptr_len < sizeof(Gralloc_Gbuffer_Info)))
        {
            break;
        }

        ptr = call_para_to_ptr(all_para[0], &need_free);
        info = *(Gralloc_Gbuffer_Info *)(ptr);

        Guest_Mem *gbuffer_data = copy_guest_mem_from_call(call, 2);

        alloc_gbuffer_with_gralloc(info, gbuffer_data);
    }
    break;
    case FUNID_Mem_Signal_Sync:
    {
        uint64_t sync_id;

        if (unlikely(para_num < PARA_NUM_Mem_Signal_Sync))
        {
            break;
        }

        ptr_len = all_para[0].data_len;
        if (unlikely(ptr_len < sizeof(uint64_t)))
        {
            break;
        }

        ptr = call_para_to_ptr(all_para[0], &need_free);
        sync_id = *(uint64_t *)(ptr);

        signal_express_sync((int)sync_id, true);
    }
    break;
    case FUNID_Mem_Wait_Sync:
    {
        uint64_t sync_id;

        if (unlikely(para_num < PARA_NUM_Mem_Wait_Sync))
        {
            break;
        }

        ptr_len = all_para[0].data_len;
        if (unlikely(ptr_len < sizeof(uint64_t)))
        {
            break;
        }

        ptr = call_para_to_ptr(all_para[0], &need_free);
        sync_id = *(uint64_t *)(ptr);

        wait_for_express_sync((int)sync_id, true);
    }
    break;
    case FUNID_Update_Gbuffer_Location:
    {
        uint64_t gbuffer_id;
        int pid;
        int write;

        if (unlikely(para_num < PARA_NUM_Update_Gbuffer_Location))
        {
            break;
        }

        ptr_len = all_para[0].data_len;
        if (unlikely(ptr_len < 3 * sizeof(int64_t)))
        {
            break;
        }

        ptr = call_para_to_ptr(all_para[0], &need_free);
        gbuffer_id = *(uint64_t *)(ptr);
        pid = *(int *)(ptr + 8);
        write = *(int *)(ptr + 16);

        Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        if (gbuffer) {
            update_gbuffer_location(gbuffer, write ? EXPRESS_MEM_TYPE_GBUFFER : EXPRESS_MEM_TYPE_GUEST_MEM, pid, write);
        }
    }
    break;
    default:
    {
        LOGE("error! function id %d not recognized!", GET_FUN_ID(call->id));
    }

    }

    if (need_free) {
        g_free(ptr);
    }

    call->callback(call, 1);
    return;
}

static Thread_Context *get_mem_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (g_context == NULL)
    {
        g_context = thread_context_create(thread_id, device_id, sizeof(Thread_Context), info);
        g_pool = g_thread_pool_new(
            express_mem_worker, /* worker function */
            NULL,  /* pool-specific user data */
            4,     /* max threads */
            false, /* exclusive */
            NULL   /* errors */
        );
    }
    return g_context;
}

static void mem_context_init(Thread_Context *context)
{
    // 新建一个context用于与纹理交互
    if (g_gl_context == NULL)
    {
        g_gl_context = get_native_opengl_context(0);
        egl_makeCurrent(g_gl_context);

        glGenBuffers(1, &unpack_buffer);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, unpack_buffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, unpack_buffer);
    }
}

static void mem_context_destroy(Thread_Context *context)
{
    if (g_gl_context != NULL)
    {
        glDeleteBuffers(1, &unpack_buffer);

        egl_makeCurrent(NULL);
        egl_destroyContext(g_gl_context);
        g_gl_context = NULL;
    }
}

void alloc_gbuffer_with_gralloc(Gralloc_Gbuffer_Info info, Guest_Mem *mem_data)
{
    if (info.width == 0 || info.height == 0 || info.size == 0)
    {
        return;
    }

    Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(info.gbuffer_id);
    LOGI("alloc_gbuffer_with_gralloc id %" PRIx64 " width %d height %d size %d", info.gbuffer_id, info.width, info.height, info.size);

    if (gbuffer == NULL)
    {
        gbuffer = create_gbuffer_from_gralloc_info(info, info.gbuffer_id);
        gbuffer->guest_data = mem_data;
        add_gbuffer_to_global(gbuffer);
        LOGI("alloc gbuffer id %" PRIx64 " size %d mem_len %d width %d height %d stride %d pixel_size %d", gbuffer->gbuffer_id, gbuffer->size, mem_data->all_len, gbuffer->width, gbuffer->height, gbuffer->stride, gbuffer->pixel_size);
    }
    else
    {
        // a host copy exists. probably allocated by eglCreateImage.
        // check if the info is compatible, and update the info
        if (info.width != gbuffer->width || info.height != gbuffer->height)
        {
            LOGE("alloc_gbuffer_with_gralloc gbuffer info not matching: id %" PRIx64 " width %d height %d stride %d pixel_size %d origin %d %d %d %d", gbuffer->gbuffer_id, info.width, info.height, info.stride, info.pixel_size, gbuffer->width, gbuffer->height, gbuffer->stride, gbuffer->pixel_size);
            return;
        }

        // update gbuffer info
        if (gbuffer->stride == 0) {
            gbuffer->stride = info.stride;
        }

        if (gbuffer->pixel_size == 0) {
            gbuffer->pixel_size = info.pixel_size;
        }

        if (gbuffer->guest_data == NULL) {
            gbuffer->guest_data = mem_data;
        }
        else {
            LOGW("alloc_gbuffer_with_gralloc gbuffer %" PRIx64 "already has guest mem, discarding previous copy", info.gbuffer_id);
            free_copied_guest_mem(gbuffer->guest_data);
            gbuffer->guest_data = mem_data;
        }
    }
}

Graphic_Buffer *create_gbuffer_from_gralloc_info(Gralloc_Gbuffer_Info info, uint64_t gbuffer_id)
{

    int sampler_num = 0;
    int format = GL_RGBA;
    int pixel_type = GL_UNSIGNED_BYTE;
    int internal_format = GL_RGBA8;
    int depth_internal_format = 0;
    int stencil_internal_format = 0;
    int width = info.width;
    int height = info.height;

    if (info.format == EXPRESS_PIXEL_RGBA8888 || info.format == EXPRESS_PIXEL_RGBX8888)
    {
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_BYTE;
        // row_byte_len = width * 4;
    }
    else if (info.format == EXPRESS_PIXEL_RGB888)
    {
        internal_format = GL_RGB8;
        format = GL_RGB;
        pixel_type = GL_UNSIGNED_BYTE;
        // row_byte_len = width * 3;
    }
    else if (info.format == EXPRESS_PIXEL_RGB565)
    {
        internal_format = GL_RGB565;
        format = GL_RGB;
        pixel_type = GL_UNSIGNED_SHORT_5_6_5;
        // row_byte_len = width * 2;
    }
    else if (info.format == EXPRESS_PIXEL_RGBA5551 || info.format == EXPRESS_PIXEL_RGBX5551)
    {
        internal_format = GL_RGB5_A1;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_SHORT_5_5_5_1;
        // GL_UNSIGNED_SHORT_5_5_5_1
        // row_byte_len = width * 2;
    }
    else if (info.format == EXPRESS_PIXEL_RGBA4444 || info.format == EXPRESS_PIXEL_RGBX4444)
    {
        internal_format = GL_RGBA4;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_SHORT_4_4_4_4;
        // row_byte_len = width * 2;
    }
    else if (info.format == EXPRESS_PIXEL_BGRA8888 || info.format == EXPRESS_PIXEL_BGRX8888)
    {
        // LOGI("EGLImage with g_buffer_id %llx need format BGRA_8888!!!", (uint64_t)g_buffer_id);
        internal_format = GL_RGBA8;
        format = GL_BGRA;
        pixel_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        // row_byte_len = width * 4;
    }
    else if (info.format == EXPRESS_PIXEL_BGR565)
    {
        internal_format = GL_RGB565;
        format = GL_BGR;
        pixel_type = GL_UNSIGNED_SHORT_5_6_5_REV;
        // row_byte_len = width * 2;
    }
    else if (info.format == EXPRESS_PIXEL_RGBA1010102)
    {
        internal_format = GL_RGB10_A2;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_INT_2_10_10_10_REV;
        // row_byte_len = width * 2;
    }
    else if (info.format == EXPRESS_PIXEL_R8)
    {
        internal_format = GL_R8;
        format = GL_RED;
        pixel_type = GL_UNSIGNED_BYTE;
        // row_byte_len = width * 2;
    }
    else
    {
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        pixel_type = GL_UNSIGNED_INT;
        // row_byte_len = width * 4;
        LOGE("error! unknown gralloc format %d!!!", info.format);
    }

    Graphic_Buffer *gbuffer = create_gbuffer(width, height, sampler_num,
                          format,
                          pixel_type,
                          internal_format,
                          depth_internal_format,
                          stencil_internal_format,
                          gbuffer_id);

    gbuffer->usage = info.usage;
    gbuffer->pixel_size = info.pixel_size;
    gbuffer->size = info.size;
    gbuffer->stride = info.stride;
    return gbuffer;
}

static void guest_to_host_dma_task(MemTransferTask *task, void *mapped_addr) {
    Graphic_Buffer *gbuffer = task->dst_data;
    Guest_Mem *mem_data = task->src_data;

    int real_width = gbuffer->width;
    if (real_width % (gbuffer->stride) != 0)
    {
        real_width = (real_width / gbuffer->stride + 1) * gbuffer->stride;
    }

    int row_byte_len = gbuffer->pixel_size * gbuffer->width;

    int all_pixel_size = row_byte_len * gbuffer->height;

    LOGI("gbuffer_data_guest_to_host id %llx width %d height %d internal_format %x format %x row_byte_len %d buf_len %d",
           gbuffer->gbuffer_id, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->format, row_byte_len, mem_data->all_len);

    // GraphicBuffer里的图片是正的，放到纹理里要倒个个
    // -- 不用倒个了，因为合成的时候，普通窗口都进行了倒个，然后显示的时候，又进行了倒个
    if (gbuffer->stride != row_byte_len)
    {
        for (int i = 0; i < gbuffer->height; i++)
        {
            // read_from_guest_mem(mem_data, mapped_addr + (gbuffer->height - i - 1) * row_byte_len, i * gbuffer->stride, row_byte_len);
            read_from_guest_mem(mem_data, mapped_addr + i * row_byte_len, i * gbuffer->stride, row_byte_len);
        }
    }
    else
    {
        read_from_guest_mem(mem_data, mapped_addr, 0, all_pixel_size);
        // for (int i = 0; i < gbuffer->height; i++)
        // {
        //     read_from_guest_mem(mem_data, mapped_addr + (gbuffer->height - i - 1) * row_byte_len, i * row_byte_len, row_byte_len);
        // }
    }
}

void gbuffer_data_guest_to_host(Gralloc_Gbuffer_Info info, int sync_id)
{
    Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(info.gbuffer_id);

    if (gbuffer == NULL)
    {
        LOGE("error! gbuffer_data_guest_to_host get null gbuffer: id %" PRIx64 "", info.gbuffer_id);
        return;
    }

    if (info.width != gbuffer->width || info.height != gbuffer->height || info.stride != gbuffer->stride || info.pixel_size != gbuffer->pixel_size)
    {
        LOGE("gbuffer_data_guest_to_host gbuffer info not matching: id %" PRIx64 " width %d height %d stride %d pixel_size %d, local %d %d %d %d", info.gbuffer_id, info.width, info.height, info.stride, info.pixel_size, gbuffer->width, gbuffer->height, gbuffer->stride, gbuffer->pixel_size);
        return;
    }

    if (gbuffer->guest_data == NULL) {
        LOGE("error! gbuffer_data_guest_to_host with null guest_data!");
        return;
    }

    mem_transfer_async(EXPRESS_MEM_TYPE_GBUFFER, EXPRESS_MEM_TYPE_GUEST_OPAQUE, gbuffer, gbuffer->guest_data, gbuffer->size, gbuffer->guest_data->all_len, sync_id, guest_to_host_dma_task, NULL, NULL);
}

void gbuffer_data_host_to_guest(Gralloc_Gbuffer_Info info)
{
    Graphic_Buffer *gbuffer = get_gbuffer_from_global_map(info.gbuffer_id);

    if (gbuffer == NULL)
    {
        LOGE("error! gbuffer_data_host_to_guest get null gbuffer");
        return;
    }

    if (info.width != gbuffer->width || info.height != gbuffer->height || info.stride != gbuffer->stride || info.pixel_size != gbuffer->pixel_size)
    {
        LOGE("error! guest download gbuffer data size error width height stride pixel_size %d %d %d %d origin %d %d %d %d",
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

    // LOGI("GraphicBuffer data width %d height %d row_byte_len %d guest_row_byte_len %d", egl_image->width, egl_image->height, row_byte_len, guest_row_byte_len);

    if (all_pixel_size > mem_data->all_len)
    {
        LOGE("error! gbuffer_data_host_to_guest len error! row %d height %d get len %d", row_byte_len, info.height, mem_data->all_len);
        return;
    }

    if (unpack_buffer_size < all_pixel_size)
    {
        unpack_buffer_size = all_pixel_size;
        glBufferData(GL_PIXEL_PACK_BUFFER, unpack_buffer_size, NULL, GL_STREAM_DRAW);
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
            glBufferData(GL_PIXEL_PACK_BUFFER, unpack_buffer_size, NULL, GL_STREAM_DRAW);
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
        LOGE("error %x when d_glReadGraphicBuffer width %d height %d internal_format %x format %x row_byte_len %d buf_len %d",
               error, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->format, row_byte_len, mem_data->all_len);
    }

    LOGI("gbuffer_data_host_to_guest id %llx width %d height %d internal_format %x format %x row_byte_len %d buf_len %d",
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

/**
 * initiate shared memory transfer using express-mem workers.
 * for the arguments, see struct MemTransferTask.
*/
void mem_transfer_async(ExpressMemType dst_loc, ExpressMemType src_loc, void *dst_data, void *src_data, int dst_len, int src_len, int sync_id, PreprocessCbType pre_cb, PostprocessCbType post_cb, void *private_data) {
    // task will be freed in express_mem_worker()
    MemTransferTask *task = g_malloc0(sizeof(MemTransferTask));
    task->dst_loc = dst_loc;
    task->src_loc = src_loc;
    task->dst_data = dst_data;
    task->src_data = src_data;
    task->dst_len = dst_len;
    task->src_len = src_len;
    task->sync_id = sync_id;
    task->pre_cb = pre_cb;
    task->post_cb = post_cb;
    task->private_data = private_data;

    LOGI("transfer_async received task: %s (size %d) -> %s (size %d) sync %d; pending tasks: %u", memtype_to_str(src_loc), src_len, memtype_to_str(dst_loc), dst_len, sync_id, g_thread_pool_unprocessed(g_pool));

    g_thread_pool_push(g_pool, (gpointer)task, NULL);
}

static Express_Device_Info express_mem_info = {
    .enable_default = true,
    .name = "express-mem",
    .option_name = "mem",
    .driver_name = "express_mem",
    .device_id = EXPRESS_MEM_DEVICE_ID,
    .device_type = OUTPUT_DEVICE_TYPE,
    .call_handle = mem_master_switch,
    .context_init = mem_context_init,
    .context_destroy = mem_context_destroy,
    .get_context = get_mem_thread_context,
};

EXPRESS_DEVICE_INIT(express_mem, &express_mem_info)
