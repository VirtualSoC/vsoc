// #include "hw/express-gpu/express_gpu_snapshot.h"
// #include "migration/qemu-file.h"
// #include <stdint.h>
// #include "migration/qemu-file-types.h"
// #include "migration/qemu-file.h"
// #include "exec/cpu-common.h"
#include "hw/teleport-express/teleport_express.h"

#include "hw/teleport-express/teleport_express_call.h"
#include "hw/teleport-express/teleport_express_distribute.h"
#include "hw/teleport-express/teleport_express_register.h"

#include "hw/teleport-express/express_log.h"

#include "hw/virtio/virtio.h"

#include "hw/express-gpu/express_gpu.h"
#include "hw/express-gpu/express_gpu_snapshot.h"
#include "migration/qemu-file.h"



int save_single_render_thread_context(QEMUFile *f, Render_Thread_Context *thread_context) {
    LOGI("in save single render thread context!");
    if (save_thread_context(f, &thread_context->context) < 0) {
        return -1;
    }

    if (thread_context->process_context) {
        save_process_context(f, thread_context->process_context);
    }

    save_thread_unique_ids(f, thread_context->thread_unique_ids);

    if (thread_context->render_double_buffer_read) {
        qemu_put_be32(f, 1);
        save_window_buffer(f, thread_context->render_double_buffer_read);
    } else {
        qemu_put_be32(f, 0);
        LOGI("render_double_buffer_read is null!");
    }

    if (thread_context->render_double_buffer_draw) {
        qemu_put_be32(f, 1);
        save_window_buffer(f, thread_context->render_double_buffer_draw);
    } else {
        qemu_put_be32(f, 0);
        LOGI("render_double_buffer_draw is null!");
    }

    if (thread_context->opengl_context) {
        qemu_put_be32(f, 1);
        save_opengl_context(f, thread_context->opengl_context);
    } else {
        qemu_put_be32(f, 0);
        LOGI("opengl context is null!");
    }

    if (thread_context->egl_display) {
        qemu_put_be32(f, 1);
        save_egl_display(f, thread_context->egl_display);
    } else {
        qemu_put_be32(f, 0);
    }

    return 0;
}

int load_single_render_thread_context(QEMUFile *f, Render_Thread_Context *thread_context) {
    LOGI("in load single render thread context!");

    if (load_thread_context(f, &thread_context->context) < 0) {
        return -1;
    }

    thread_context->process_context = g_malloc0(sizeof(Process_Context));
    load_process_context(f, thread_context->process_context);

    thread_context->thread_unique_ids = load_thread_unique_ids(f);

    int has_read_window = qemu_get_be32(f);
    if (has_read_window) {
        thread_context->render_double_buffer_read = load_window_buffer(f);
    }
    int has_write_window = qemu_get_be32(f);
    if (has_write_window) {
        thread_context->render_double_buffer_draw = load_window_buffer(f);
    }

    int has_opengl_context = qemu_get_be32(f);
    if (has_opengl_context) {
        Opengl_Context *context;
        if(thread_context->opengl_context == NULL) {
            context = g_malloc0(sizeof(Opengl_Context));
        } else {
            context = thread_context->opengl_context;
        }

        if (load_opengl_context(f, context) < 0) {
            return -1;
        }        
    }

    int has_egl_display = qemu_get_be32(f);
    if(has_egl_display) {
        thread_context->egl_display = load_egl_display(f);
    }
    
    return 0;
}


int save_thread_context(QEMUFile *f, Thread_Context *context) {
    qemu_put_be64(f, context->device_id);
    qemu_put_be32(f, context->read_loc);
    qemu_put_be32(f, context->write_loc);
    qemu_put_be32(f, context->init);
    qemu_put_be32(f, context->thread_run);
    qemu_put_be64(f, context->thread_id);

//可能需要保存事件的触发状态，恢复时重新创建handle?
// #ifdef _WIN32
//     DWORD event_state = WaitForSingleObject(context->data_event, 0);
//     qemu_put_be32(f, event_state == WAIT_OBJECT_0 ? 1 : 0);
// #endif

    return 0;
}

int load_thread_context(QEMUFile *f, Thread_Context *context) {
    context->device_id = qemu_get_be64(f);
    context->read_loc = qemu_get_be32(f);
    context->write_loc = qemu_get_be32(f);
    context->init = qemu_get_be32(f);
    context->thread_run = qemu_get_be32(f);
    context->thread_id = qemu_get_be64(f);
    LOGI("in load_thread_context with device id %lld thread id %lld", context->device_id, context->thread_id);

// #ifdef _WIN32 //重新创建handle
//     if (context->write_loc == context->read_loc) {
//         context->data_event = CreateEvent(NULL, FALSE, FALSE, NULL);
//     } else {
//         context->data_event = CreateEvent(NULL, FALSE, TRUE, NULL);
//     }
    
// 我想开了。。要不就直接不管了
// #endif

//todo: this_thread和teleport_express_device要如何处理？先不管了
    return 0;
}



void save_process_context(QEMUFile *f, Process_Context *process_context) {
    LOGI("in save_process_context");
    GHashTableIter iter;
    gpointer key, value;
    guint gbuffer_count = g_hash_table_size(process_context->gbuffer_map);
    LOGI("in save_process_context with gbuffer count %d", gbuffer_count);
    qemu_put_be32(f, gbuffer_count);
    g_hash_table_iter_init(&iter, process_context->gbuffer_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t gbuffer_id = (uint64_t)key;
        Hardware_Buffer *gbuffer = (Hardware_Buffer *)value;
        
        qemu_put_be64(f, gbuffer_id); // guest那边的id肯定是不会变的
        LOGI("save process context gbuffer id %lld", gbuffer_id);
        save_hardware_buffer(f, gbuffer); // host这边或许需要怎么重新处理一下
    }
    guint surface_count = g_hash_table_size(process_context->surface_map);
    LOGI("in save_process_context with surface_count count %d", surface_count);

    qemu_put_be32(f, surface_count);
    g_hash_table_iter_init(&iter, process_context->surface_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t guest_surface_id = (uint64_t)key;
        Window_Buffer *window_buffer = (Window_Buffer *)value;
        LOGI("saving window buffer %lld", (uint64_t)window_buffer);
        qemu_put_be64(f, guest_surface_id);
        save_window_buffer(f, window_buffer);
    }
    guint context_count = g_hash_table_size(process_context->context_map);
    LOGI("in save_process_context with context_count count %d", context_count);

    qemu_put_be32(f, context_count);
    g_hash_table_iter_init(&iter, process_context->context_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t context_id = (uint64_t)key;
        Opengl_Context *opengl_context = (Opengl_Context *)value;
        
        qemu_put_be64(f, context_id);
        save_opengl_context(f, opengl_context);
    }
    LOGI("in save_process_context with thread count %d", process_context->thread_cnt);
    qemu_put_be32(f, process_context->thread_cnt);
}

void load_process_context(QEMUFile *f, Process_Context *process_context) {
    process_context->gbuffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    process_context->surface_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    process_context->context_map = g_hash_table_new(g_direct_hash, g_direct_equal);

    guint gbuffer_count = qemu_get_be32(f);
    LOGI("in load_process_context with gbuffer count %d", gbuffer_count);

    for (guint i = 0; i < gbuffer_count; i++) {
        uint64_t gbuffer_id = qemu_get_be64(f);
        LOGI("load process context gbuffer id %lld", gbuffer_id);
        Hardware_Buffer *gbuffer = load_hardware_buffer(f);
        g_hash_table_insert(process_context->gbuffer_map, GUINT_TO_POINTER(gbuffer_id), gbuffer);
    }

    guint surface_count = qemu_get_be32(f);
    LOGI("in load_process_context with surface_count count %d", surface_count);

    for (guint i = 0; i < surface_count; i++) {
        uint64_t guest_surface_id = qemu_get_be64(f);
        Window_Buffer *window_buffer = load_window_buffer(f);
        g_hash_table_insert(process_context->surface_map, GUINT_TO_POINTER(guest_surface_id), window_buffer);
    }

    guint context_count = qemu_get_be32(f);
    LOGI("in load_process_context with context_count count %d", context_count);

    for (guint i = 0; i < context_count; i++) {
        uint64_t context_id = qemu_get_be64(f);
        Opengl_Context *opengl_context = g_malloc0(sizeof(Opengl_Context));
        load_opengl_context(f, opengl_context);
        g_hash_table_insert(process_context->context_map, GUINT_TO_POINTER(context_id), opengl_context);
    }

    process_context->thread_cnt = qemu_get_be32(f);
    LOGI("in load_process_context with thread count %d", process_context->thread_cnt);

}

void save_window_buffer(QEMUFile *f, Window_Buffer *buffer) {
    LOGI("in save_window_buffer! %d", sizeof(buffer->window_hints.hints));
    qemu_put_be32(f, buffer->type);
    qemu_put_buffer(f, (uint8_t *)buffer->window_hints.hints, sizeof(buffer->window_hints.hints));

    save_egl_config(f, buffer->config);

//不确定这个guest_surface是这么存的啊，先这样吧 
    qemu_put_be64(f, (uint64_t)buffer->guest_surface);

    qemu_put_be64(f, buffer->gbuffer_id);
    qemu_put_be32(f, buffer->width);
    qemu_put_be32(f, buffer->height);
    qemu_put_be32(f, buffer->is_current);
    qemu_put_be32(f, buffer->need_destroy);
    qemu_put_be32(f, buffer->swap_interval);
    LOGI("in save_window_buffer! %lld %lld %d %d %d %d %d", buffer->guest_surface, buffer->gbuffer_id, buffer->width, buffer->height, buffer->is_current, buffer->need_destroy, buffer->swap_interval);

    for (int i = 0; i < 20; i++) {
        qemu_put_be64(f, buffer->swap_time[i]);
    }
    qemu_put_be64(f, buffer->swap_time_all);
    qemu_put_be32(f, buffer->swap_loc);
    qemu_put_be32(f, buffer->swap_time_cnt);
    qemu_put_be64(f, buffer->frame_start_time);
    qemu_put_be64(f, buffer->last_calc_time);
    qemu_put_be32(f, buffer->now_screen_hz);
    qemu_put_be32(f, buffer->sampler_num);
    qemu_put_be32(f, buffer->format);
    qemu_put_be32(f, buffer->pixel_type);
    qemu_put_be32(f, buffer->internal_format);
    qemu_put_be32(f, buffer->depth_internal_format);
    qemu_put_be32(f, buffer->stencil_internal_format);
    
    qemu_put_be32(f, buffer->now_fbo_loc);
    for (int i = 0; i < 3; i++) {
        qemu_put_be32(f, buffer->data_fbo[i]);
        qemu_put_be32(f, buffer->sampler_fbo[i]);
        qemu_put_be32(f, buffer->connect_texture[i]);
    }

    save_hardware_buffer(f, buffer->gbuffer);
}

Window_Buffer* load_window_buffer(QEMUFile *f) {
    
    Window_Buffer *buffer = g_malloc0(sizeof(Window_Buffer));
    LOGI("in load_window_buffer! %d", sizeof(buffer->window_hints.hints));

    buffer->type = qemu_get_be32(f);
    qemu_get_buffer(f, (uint8_t *)buffer->window_hints.hints, sizeof(buffer->window_hints.hints));

    buffer->config = load_egl_config(f);

    buffer->guest_surface = (EGLSurface)qemu_get_be64(f);

    buffer->gbuffer_id = qemu_get_be64(f);
    buffer->width = qemu_get_be32(f);
    buffer->height = qemu_get_be32(f);
    buffer->is_current = qemu_get_be32(f);
    buffer->need_destroy = qemu_get_be32(f);
    buffer->swap_interval = qemu_get_be32(f);
    LOGI("in load_window_buffer! %lld %lld %d %d %d %d %d", buffer->guest_surface, buffer->gbuffer_id, buffer->width, buffer->height, buffer->is_current, buffer->need_destroy, buffer->swap_interval);
    for (int i = 0; i < 20; i++) {
        buffer->swap_time[i] = qemu_get_be64(f);
    }
    buffer->swap_time_all = qemu_get_be64(f);
    buffer->swap_loc = qemu_get_be32(f);
    buffer->swap_time_cnt = qemu_get_be32(f);
    buffer->frame_start_time = qemu_get_be64(f);
    buffer->last_calc_time = qemu_get_be64(f);
    buffer->now_screen_hz = qemu_get_be32(f);
    buffer->sampler_num = qemu_get_be32(f);
    buffer->format = qemu_get_be32(f);
    buffer->pixel_type = qemu_get_be32(f);
    buffer->internal_format = qemu_get_be32(f);
    buffer->depth_internal_format = qemu_get_be32(f);
    buffer->stencil_internal_format = qemu_get_be32(f);

    buffer->now_fbo_loc = qemu_get_be32(f);
    for (int i = 0; i < 3; i++) {
        buffer->data_fbo[i] = qemu_get_be32(f);
        buffer->sampler_fbo[i] = qemu_get_be32(f);
        buffer->connect_texture[i] = qemu_get_be32(f);
    }

    buffer->gbuffer = load_hardware_buffer(f);
    return buffer;
}


//开始巨大的save opengl context！

void save_scatter_data(QEMUFile *f, Scatter_Data *scatter_data, int count) {
    qemu_put_be32(f, count);

    for (int i = 0; i < count; i++) {
        qemu_put_be32(f, scatter_data[i].len);
        qemu_put_buffer(f, scatter_data[i].data, scatter_data[i].len);
    }
}

Scatter_Data* load_scatter_data(QEMUFile *f, int *count) {
    *count = qemu_get_be32(f);

    Scatter_Data *scatter_data = g_malloc0(sizeof(Scatter_Data) * (*count));
    for (int i = 0; i < *count; i++) {
        scatter_data[i].len = qemu_get_be32(f);
        scatter_data[i].data = g_malloc0(scatter_data[i].len);
        qemu_get_buffer(f, scatter_data[i].data, scatter_data[i].len);
    }

    return scatter_data;
}

void save_guest_mem(QEMUFile *f, Guest_Mem *guest_mem) {
    qemu_put_be32(f, guest_mem->num);
    qemu_put_be32(f, guest_mem->all_len);
    save_scatter_data(f, guest_mem->scatter_data, guest_mem->num);
}

Guest_Mem* load_guest_mem(QEMUFile *f) {
    Guest_Mem *guest_mem = g_malloc0(sizeof(Guest_Mem));
    guest_mem->num = qemu_get_be32(f);
    guest_mem->all_len = qemu_get_be32(f);
    guest_mem->scatter_data = load_scatter_data(f, &guest_mem->num);

    return guest_mem;
}

void save_hardware_buffer(QEMUFile *f, Hardware_Buffer *buffer) {
    qemu_put_be32(f, buffer->is_writing);
    qemu_put_be32(f, buffer->is_lock);
    qemu_put_be32(f, buffer->sampler_num);
    qemu_put_be32(f, buffer->data_texture);
    qemu_put_be32(f, buffer->reverse_rbo);
    qemu_put_be32(f, buffer->sampler_rbo);
    qemu_put_be32(f, buffer->rbo_depth);
    qemu_put_be32(f, buffer->rbo_stencil);
    qemu_put_be32(f, buffer->data_fbo);
    qemu_put_be32(f, buffer->sampler_fbo);
    qemu_put_be32(f, buffer->has_connected_fbo);
    qemu_put_be64(f, buffer->gbuffer_id);
    qemu_put_be32(f, buffer->is_using);
    qemu_put_be32(f, buffer->need_reverse);
    qemu_put_be32(f, buffer->format);
    qemu_put_be32(f, buffer->pixel_type);
    qemu_put_be32(f, buffer->internal_format);
    qemu_put_be32(f, buffer->row_byte_len);
    qemu_put_be32(f, buffer->depth_internal_format);
    qemu_put_be32(f, buffer->stencil_internal_format);
    qemu_put_be32(f, buffer->hal_format);
    qemu_put_be32(f, buffer->stride);
    qemu_put_be32(f, buffer->width);
    qemu_put_be32(f, buffer->height);
    qemu_put_be32(f, buffer->usage_type);
    qemu_put_be32(f, buffer->pixel_size);
    qemu_put_be32(f, buffer->usage);
    qemu_put_be32(f, buffer->size);
    qemu_put_be32(f, buffer->last_phy_dev);
    qemu_put_be32(f, buffer->last_phy_usage);
    qemu_put_be32(f, buffer->pref_phy_dev);
    qemu_put_be32(f, buffer->last_virt_dev);
    qemu_put_be32(f, buffer->last_virt_usage);
    qemu_put_be32(f, buffer->last_virt_time);

    // todo 咋存？
    qemu_put_be64(f, (uint64_t)buffer->data_sync);
    qemu_put_be64(f, (uint64_t)buffer->delete_sync);

    if (buffer->guest_data) {
        qemu_put_be32(f, 1);
        save_guest_mem(f, buffer->guest_data);
    } else {
        qemu_put_be32(f, 0);
    }

    qemu_put_be64(f, (uint64_t)buffer->host_data);
}

Hardware_Buffer* load_hardware_buffer(QEMUFile *f) {
    Hardware_Buffer *buffer = g_malloc0(sizeof(Hardware_Buffer));

    buffer->is_writing = qemu_get_be32(f);
    buffer->is_lock = qemu_get_be32(f);
    buffer->sampler_num = qemu_get_be32(f);
    buffer->data_texture = qemu_get_be32(f);
    buffer->reverse_rbo = qemu_get_be32(f);
    buffer->sampler_rbo = qemu_get_be32(f);
    buffer->rbo_depth = qemu_get_be32(f);
    buffer->rbo_stencil = qemu_get_be32(f);
    buffer->data_fbo = qemu_get_be32(f);
    buffer->sampler_fbo = qemu_get_be32(f);
    buffer->has_connected_fbo = qemu_get_be32(f);
    buffer->gbuffer_id = qemu_get_be64(f);
    buffer->is_using = qemu_get_be32(f);
    buffer->need_reverse = qemu_get_be32(f);
    buffer->format = qemu_get_be32(f);
    buffer->pixel_type = qemu_get_be32(f);
    buffer->internal_format = qemu_get_be32(f);
    buffer->row_byte_len = qemu_get_be32(f);
    buffer->depth_internal_format = qemu_get_be32(f);
    buffer->stencil_internal_format = qemu_get_be32(f);
    buffer->hal_format = qemu_get_be32(f);
    buffer->stride = qemu_get_be32(f);
    buffer->width = qemu_get_be32(f);
    buffer->height = qemu_get_be32(f);
    buffer->usage_type = qemu_get_be32(f);
    buffer->pixel_size = qemu_get_be32(f);
    buffer->usage = qemu_get_be32(f);
    buffer->size = qemu_get_be32(f);
    buffer->last_phy_dev = qemu_get_be32(f);
    buffer->last_phy_usage = qemu_get_be32(f);
    buffer->pref_phy_dev = qemu_get_be32(f);
    buffer->last_virt_dev = qemu_get_be32(f);
    buffer->last_virt_usage = qemu_get_be32(f);
    buffer->last_virt_time = qemu_get_be32(f);

    //todo 不知道这个是啥！该咋存！
    buffer->data_sync = (GLsync)qemu_get_be64(f);
    buffer->delete_sync = (GLsync)qemu_get_be64(f);

    int has_guest_data = qemu_get_be32(f);
    if (has_guest_data) {
        buffer->guest_data = load_guest_mem(f);
    } else {
        buffer->guest_data = NULL;
    }

    buffer->host_data = (void *)qemu_get_be64(f);

    return buffer;
}


void save_attrib_point(QEMUFile *f, Attrib_Point *point) {
    for (int i = 0; i < MAX_VERTEX_ATTRIBS_NUM; i++) {
        qemu_put_be32(f, point->buffer_object[i]);
        qemu_put_be32(f, point->buffer_loc[i]);
        qemu_put_be32(f, point->remain_buffer_len[i]);
        qemu_put_be32(f, point->buffer_len[i]);
    }
    qemu_put_be32(f, point->indices_buffer_object);
    qemu_put_be32(f, point->indices_buffer_len);
    qemu_put_be32(f, point->remain_indices_buffer_len);
    qemu_put_be32(f, point->element_array_buffer);
}

Attrib_Point* load_attrib_point(QEMUFile *f) {
    Attrib_Point *point = g_malloc0(sizeof(Attrib_Point));
    for (int i = 0; i < MAX_VERTEX_ATTRIBS_NUM; i++) {
        point->buffer_object[i] = qemu_get_be32(f);
        point->buffer_loc[i] = qemu_get_be32(f);
        point->remain_buffer_len[i] = qemu_get_be32(f);
        point->buffer_len[i] = qemu_get_be32(f);
    }
    point->indices_buffer_object = qemu_get_be32(f);
    point->indices_buffer_len = qemu_get_be32(f);
    point->remain_indices_buffer_len = qemu_get_be32(f);
    point->element_array_buffer = qemu_get_be32(f);
    return point;
}

void save_buffer_status(QEMUFile *f, Buffer_Status *status) {
    qemu_put_be32(f, status->guest_array_buffer);
    qemu_put_be32(f, status->host_array_buffer);

    qemu_put_be32(f, status->guest_element_array_buffer);
    qemu_put_be32(f, status->host_element_array_buffer);

    qemu_put_be32(f, status->guest_copy_read_buffer);
    qemu_put_be32(f, status->host_copy_read_buffer);

    qemu_put_be32(f, status->guest_copy_write_buffer);
    qemu_put_be32(f, status->host_copy_write_buffer);

    qemu_put_be32(f, status->guest_pixel_pack_buffer);
    qemu_put_be32(f, status->host_pixel_pack_buffer);

    qemu_put_be32(f, status->guest_pixel_unpack_buffer);
    qemu_put_be32(f, status->host_pixel_unpack_buffer);

    qemu_put_be32(f, status->guest_transform_feedback_buffer);
    qemu_put_be32(f, status->host_transform_feedback_buffer);

    qemu_put_be32(f, status->guest_uniform_buffer);
    qemu_put_be32(f, status->host_uniform_buffer);

    qemu_put_be32(f, status->guest_atomic_counter_buffer);
    qemu_put_be32(f, status->host_atomic_counter_buffer);

    qemu_put_be32(f, status->guest_dispatch_indirect_buffer);
    qemu_put_be32(f, status->host_dispatch_indirect_buffer);

    qemu_put_be32(f, status->guest_draw_indirect_buffer);
    qemu_put_be32(f, status->host_draw_indirect_buffer);

    qemu_put_be32(f, status->guest_shader_storage_buffer);
    qemu_put_be32(f, status->host_shader_storage_buffer);

    qemu_put_be32(f, status->guest_texture_buffer);
    qemu_put_be32(f, status->host_texture_buffer);

    qemu_put_be32(f, status->guest_vertex_array_buffer);
    qemu_put_be32(f, status->host_vertex_array_buffer);

    qemu_put_be32(f, status->guest_vao_ebo);
    qemu_put_be32(f, status->host_vao_ebo);

    qemu_put_be32(f, status->guest_vao);
    qemu_put_be32(f, status->host_vao);
}

Buffer_Status* load_buffer_status(QEMUFile *f) {
    Buffer_Status *status = g_malloc0(sizeof(Buffer_Status));

    status->guest_array_buffer = qemu_get_be32(f);
    status->host_array_buffer = qemu_get_be32(f);

    status->guest_element_array_buffer = qemu_get_be32(f);
    status->host_element_array_buffer = qemu_get_be32(f);

    status->guest_copy_read_buffer = qemu_get_be32(f);
    status->host_copy_read_buffer = qemu_get_be32(f);

    status->guest_copy_write_buffer = qemu_get_be32(f);
    status->host_copy_write_buffer = qemu_get_be32(f);

    status->guest_pixel_pack_buffer = qemu_get_be32(f);
    status->host_pixel_pack_buffer = qemu_get_be32(f);

    status->guest_pixel_unpack_buffer = qemu_get_be32(f);
    status->host_pixel_unpack_buffer = qemu_get_be32(f);

    status->guest_transform_feedback_buffer = qemu_get_be32(f);
    status->host_transform_feedback_buffer = qemu_get_be32(f);

    status->guest_uniform_buffer = qemu_get_be32(f);
    status->host_uniform_buffer = qemu_get_be32(f);

    status->guest_atomic_counter_buffer = qemu_get_be32(f);
    status->host_atomic_counter_buffer = qemu_get_be32(f);

    status->guest_dispatch_indirect_buffer = qemu_get_be32(f);
    status->host_dispatch_indirect_buffer = qemu_get_be32(f);

    status->guest_draw_indirect_buffer = qemu_get_be32(f);
    status->host_draw_indirect_buffer = qemu_get_be32(f);

    status->guest_shader_storage_buffer = qemu_get_be32(f);
    status->host_shader_storage_buffer = qemu_get_be32(f);

    status->guest_texture_buffer = qemu_get_be32(f);
    status->host_texture_buffer = qemu_get_be32(f);

    status->guest_vertex_array_buffer = qemu_get_be32(f);
    status->host_vertex_array_buffer = qemu_get_be32(f);

    status->guest_vao_ebo = qemu_get_be32(f);
    status->host_vao_ebo = qemu_get_be32(f);

    status->guest_vao = qemu_get_be32(f);
    status->host_vao = qemu_get_be32(f);

    return status;
}

void save_bound_buffer(QEMUFile *f, Bound_Buffer *buffer) {
    save_attrib_point(f, buffer->attrib_point);
    save_buffer_status(f, &buffer->buffer_status);
    qemu_put_be32(f, buffer->asyn_unpack_texture_buffer);
    qemu_put_be32(f, buffer->asyn_pack_texture_buffer);
    qemu_put_be32(f, buffer->has_init);

    guint num_entries = g_hash_table_size(buffer->vao_point_data);
    qemu_put_be32(f, num_entries);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, buffer->vao_point_data);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLuint vao_id = GPOINTER_TO_UINT(key);
        Attrib_Point *point_data = (Attrib_Point *)value;

        qemu_put_be64(f, vao_id);

        save_attrib_point(f, point_data);
    }
}

Bound_Buffer* load_bound_buffer(QEMUFile *f) {
    Bound_Buffer *buffer = g_malloc0(sizeof(Bound_Buffer));
    buffer->attrib_point = load_attrib_point(f);
    buffer->buffer_status = *load_buffer_status(f);
    buffer->asyn_unpack_texture_buffer = qemu_get_be32(f);
    buffer->asyn_pack_texture_buffer = qemu_get_be32(f);
    buffer->has_init = qemu_get_be32(f);

    guint num_entries = qemu_get_be32(f);
    buffer->vao_point_data = g_hash_table_new(g_direct_hash, g_direct_equal);

    for (guint i = 0; i < num_entries; i++) {
        guint64 vao_id = qemu_get_be64(f);

        Attrib_Point *point_data = load_attrib_point(f);

        g_hash_table_insert(buffer->vao_point_data, GUINT_TO_POINTER(vao_id), point_data);
    }

    return buffer;
}

void save_resource_map_status(QEMUFile *f, Resource_Map_Status *status) {
    qemu_put_be32(f, status->max_id);
    qemu_put_be32(f, status->map_size);
    for (unsigned int i = 0; i < status->map_size; i++) {
        qemu_put_be64(f, status->resource_id_map[i]);
        qemu_put_byte(f, status->resource_is_init[i]);
    }
}

Resource_Map_Status* load_resource_map_status(QEMUFile *f) {
    Resource_Map_Status *status = g_malloc0(sizeof(Resource_Map_Status));
    status->max_id = qemu_get_be32(f);
    status->map_size = qemu_get_be32(f);
    status->resource_id_map = g_malloc0(sizeof(long long) * status->map_size);
    status->resource_is_init = g_malloc0(sizeof(char) * status->map_size);
    for (unsigned int i = 0; i < status->map_size; i++) {
        status->resource_id_map[i] = qemu_get_be64(f);
        status->resource_is_init[i] = qemu_get_byte(f);
    }
    return status;
}

void save_resource_context(QEMUFile *f, Resource_Context *context) {

    save_resource_map_status(f, context->texture_resource);
    save_resource_map_status(f, context->buffer_resource);
    save_resource_map_status(f, context->render_buffer_resource);
    save_resource_map_status(f, context->sampler_resource);

    save_resource_map_status(f, context->shader_resource);
    save_resource_map_status(f, context->program_resource);

    save_resource_map_status(f, context->sync_resource);

    save_resource_map_status(f, context->frame_buffer_resource);
    save_resource_map_status(f, context->program_pipeline_resource);
    save_resource_map_status(f, context->transform_feedback_resource);
    save_resource_map_status(f, context->vertex_array_resource);

    save_resource_map_status(f, context->query_resource);

    save_resource_map_status(f, &context->share_resources->texture_resource);
    save_resource_map_status(f, &context->share_resources->buffer_resource);
    save_resource_map_status(f, &context->share_resources->render_buffer_resource);
    save_resource_map_status(f, &context->share_resources->sample_resource);
    save_resource_map_status(f, &context->share_resources->program_resource);
    save_resource_map_status(f, &context->share_resources->shader_resource);
    save_resource_map_status(f, &context->share_resources->sync_resource);

    save_resource_map_status(f, &context->exclusive_resources->frame_buffer_resource);
    save_resource_map_status(f, &context->exclusive_resources->program_pipeline_resource);
    save_resource_map_status(f, &context->exclusive_resources->transform_feedback_resource);
    save_resource_map_status(f, &context->exclusive_resources->vertex_array_resource);
    save_resource_map_status(f, &context->exclusive_resources->query_resource);
}

Resource_Context* load_resource_context(QEMUFile *f) {
    Resource_Context *context = g_malloc0(sizeof(Resource_Context));

    context->texture_resource = load_resource_map_status(f);
    context->buffer_resource = load_resource_map_status(f);
    context->render_buffer_resource = load_resource_map_status(f);
    context->sampler_resource = load_resource_map_status(f);

    context->shader_resource = load_resource_map_status(f);
    context->program_resource = load_resource_map_status(f);

    context->sync_resource = load_resource_map_status(f);

    context->frame_buffer_resource = load_resource_map_status(f);
    context->program_pipeline_resource = load_resource_map_status(f);
    context->transform_feedback_resource = load_resource_map_status(f);
    context->vertex_array_resource = load_resource_map_status(f);

    context->query_resource = load_resource_map_status(f);

    context->share_resources = g_malloc0(sizeof(Share_Resources));
    context->share_resources->texture_resource = *load_resource_map_status(f);
    context->share_resources->buffer_resource = *load_resource_map_status(f);
    context->share_resources->render_buffer_resource = *load_resource_map_status(f);
    context->share_resources->sample_resource = *load_resource_map_status(f);
    context->share_resources->program_resource = *load_resource_map_status(f);
    context->share_resources->shader_resource = *load_resource_map_status(f);
    context->share_resources->sync_resource = *load_resource_map_status(f);

    context->exclusive_resources = g_malloc0(sizeof(Exclusive_Resources));
    context->exclusive_resources->frame_buffer_resource = *load_resource_map_status(f);
    context->exclusive_resources->program_pipeline_resource = *load_resource_map_status(f);
    context->exclusive_resources->transform_feedback_resource = *load_resource_map_status(f);
    context->exclusive_resources->vertex_array_resource = *load_resource_map_status(f);
    context->exclusive_resources->query_resource = *load_resource_map_status(f);

    return context;
}

void save_texture_binding_status(QEMUFile *f, Texture_Binding_Status *status) {
    qemu_put_be32(f, status->guest_current_active_texture);
    qemu_put_be32(f, status->host_current_active_texture);
    qemu_put_be32(f, status->now_max_texture_unit);
    LOGI("in save texture binding status with max unit %d %d %d", status->now_max_texture_unit, sizeof(status->guest_current_texture_2D)/sizeof(int), status->texture_unit_num);

    int tot_num = sizeof(status->guest_current_texture_2D)/sizeof(int);
    qemu_put_be32(f, tot_num);
    for (GLuint i = 0; i < tot_num; i++) {
        qemu_put_be32(f, status->guest_current_texture_2D[i]);
        qemu_put_be32(f, status->host_current_texture_2D[i]);
        qemu_put_be32(f, status->guest_current_texture_cube_map[i]);
        qemu_put_be32(f, status->host_current_texture_cube_map[i]);
        qemu_put_be32(f, status->guest_current_texture_3D[i]);
        qemu_put_be32(f, status->host_current_texture_3D[i]);
        qemu_put_be32(f, status->guest_current_texture_2D_array[i]);
        qemu_put_be32(f, status->host_current_texture_2D_array[i]);
        qemu_put_be32(f, status->guest_current_texture_2D_multisample[i]);
        qemu_put_be32(f, status->host_current_texture_2D_multisample[i]);
        qemu_put_be32(f, status->guest_current_texture_2D_multisample_array[i]);
        qemu_put_be32(f, status->host_current_texture_2D_multisample_array[i]);
        qemu_put_be32(f, status->guest_current_texture_cube_map_array[i]);
        qemu_put_be32(f, status->host_current_texture_cube_map_array[i]);
        qemu_put_be32(f, status->guest_current_texture_buffer[i]);
        qemu_put_be32(f, status->host_current_texture_buffer[i]);
    }

    qemu_put_be32(f, status->texture_unit_num);
    
    qemu_put_be32(f, status->current_texture_external);

    qemu_put_be64(f, (uint64_t)status->current_2D_gbuffer);
    qemu_put_be64(f, (uint64_t)status->current_external_gbuffer);
}

Texture_Binding_Status* load_texture_binding_status(QEMUFile *f) {
    Texture_Binding_Status *status = g_malloc0(sizeof(Texture_Binding_Status));

    status->guest_current_active_texture = qemu_get_be32(f);
    status->host_current_active_texture = qemu_get_be32(f);
    status->now_max_texture_unit = qemu_get_be32(f);

    int tot_num = qemu_get_be32(f);
    LOGI("in load texture binding status with max unit %d %d", tot_num, status->now_max_texture_unit);

    status->guest_current_texture_2D = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_cube_map = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_cube_map = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_3D = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_3D = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_2D_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_2D_multisample = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D_multisample = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_2D_multisample_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_2D_multisample_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_cube_map_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_cube_map_array = g_malloc0(sizeof(GLuint) * tot_num);
    status->guest_current_texture_buffer = g_malloc0(sizeof(GLuint) * tot_num);
    status->host_current_texture_buffer = g_malloc0(sizeof(GLuint) * tot_num);

    for (GLuint i = 0; i < tot_num; i++) {
        status->guest_current_texture_2D[i] = qemu_get_be32(f);
        status->host_current_texture_2D[i] = qemu_get_be32(f);
        status->guest_current_texture_cube_map[i] = qemu_get_be32(f);
        status->host_current_texture_cube_map[i] = qemu_get_be32(f);
        status->guest_current_texture_3D[i] = qemu_get_be32(f);
        status->host_current_texture_3D[i] = qemu_get_be32(f);
        status->guest_current_texture_2D_array[i] = qemu_get_be32(f);
        status->host_current_texture_2D_array[i] = qemu_get_be32(f);
        status->guest_current_texture_2D_multisample[i] = qemu_get_be32(f);
        status->host_current_texture_2D_multisample[i] = qemu_get_be32(f);
        status->guest_current_texture_2D_multisample_array[i] = qemu_get_be32(f);
        status->host_current_texture_2D_multisample_array[i] = qemu_get_be32(f);
        status->guest_current_texture_cube_map_array[i] = qemu_get_be32(f);
        status->host_current_texture_cube_map_array[i] = qemu_get_be32(f);
        status->guest_current_texture_buffer[i] = qemu_get_be32(f);
        status->host_current_texture_buffer[i] = qemu_get_be32(f);
    }

    status->texture_unit_num = qemu_get_be32(f);
    // status->now_max_texture_unit = qemu_get_be32(f);
    status->current_texture_external = qemu_get_be32(f);

    status->current_2D_gbuffer = (Hardware_Buffer *)(uint64_t)qemu_get_be64(f);
    status->current_external_gbuffer = (Hardware_Buffer *)(uint64_t)qemu_get_be64(f);

    return status;
}

void save_opengl_context(QEMUFile *f, Opengl_Context *context) {

    //不确定是否需要深拷贝，先浅拷贝了 update:需要额外处理，先注释了
    // qemu_put_be64(f, context->window);

    save_bound_buffer(f, &context->bound_buffer_status);
    save_resource_context(f, &context->resource_status);
    save_texture_binding_status(f, &context->texture_binding_status);    

    //不确定是否需要深拷贝，先浅拷贝了
    qemu_put_be64(f, (uint64_t)context->share_context);





    qemu_put_be32(f, context->draw_fbo0);
    qemu_put_be32(f, context->read_fbo0);
    qemu_put_be32(f, context->vao0);

    qemu_put_be32(f, context->view_x);
    qemu_put_be32(f, context->view_y);
    qemu_put_be32(f, context->view_w);
    qemu_put_be32(f, context->view_h);

    qemu_put_be32(f, context->is_current);
    qemu_put_be32(f, context->need_destroy);

    qemu_put_be64(f, (uint64_t)context->guest_context);
    
    // save_texture_binding_status(f, &context->texture_binding_status);

    qemu_put_be32(f, context->is_using_external_program);
    qemu_put_be32(f, context->enable_scissor);
    qemu_put_be32(f, context->context_flags);

    qemu_put_be32(f, context->draw_texi_vao);
    qemu_put_be32(f, context->draw_texi_vbo);
    qemu_put_be32(f, context->draw_texi_ebo);

    guint num_entries = g_hash_table_size(context->buffer_map);
    LOGI("in save opengl context with num_entries %d", num_entries);

    qemu_put_be32(f, num_entries);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, context->buffer_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        guint64 buffer_key = (guint64)key;
        Guest_Host_Map *map = (Guest_Host_Map *)value;

        qemu_put_be64(f, buffer_key);
        save_guest_host_map(f, map);
    }
}

int load_opengl_context(QEMUFile *f, Opengl_Context *context) {

    // Opengl_Context *context = g_malloc0(sizeof(Opengl_Context));

    // context->window = qemu_get_be64(f);

    context->bound_buffer_status = *load_bound_buffer(f);
    context->resource_status = *load_resource_context(f);
    context->texture_binding_status = *load_texture_binding_status(f);

    context->share_context = (void *)qemu_get_be64(f);


    context->draw_fbo0 = qemu_get_be32(f);
    context->read_fbo0 = qemu_get_be32(f);
    context->vao0 = qemu_get_be32(f);

    context->view_x = qemu_get_be32(f);
    context->view_y = qemu_get_be32(f);
    context->view_w = qemu_get_be32(f);
    context->view_h = qemu_get_be32(f);

    context->is_current = qemu_get_be32(f);
    context->need_destroy = qemu_get_be32(f);

    context->guest_context = (EGLContext)qemu_get_be64(f);

    // context->texture_binding_status = *load_texture_binding_status(f);

    context->is_using_external_program = qemu_get_be32(f);
    context->enable_scissor = qemu_get_be32(f);
    context->context_flags = qemu_get_be32(f);

    context->draw_texi_vao = qemu_get_be32(f);
    context->draw_texi_vbo = qemu_get_be32(f);
    context->draw_texi_ebo = qemu_get_be32(f);

    guint num_entries = qemu_get_be32(f);
    context->buffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);

    LOGI("in load opengl context with num_entries %d", num_entries);
    for (guint i = 0; i < num_entries; i++) {
        guint64 buffer_key = qemu_get_be64(f);
        Guest_Host_Map *map = load_guest_host_map(f);

        g_hash_table_insert(context->buffer_map, GUINT_TO_POINTER(buffer_key), map);
    }


    return 0;
}

void save_guest_host_map(QEMUFile *f, Guest_Host_Map *map) {
    qemu_put_be64(f, (uint64_t)map->host_data);
    qemu_put_be64(f, map->map_len);
    qemu_put_be32(f, map->target);
    qemu_put_be32(f, map->access);
}

Guest_Host_Map* load_guest_host_map(QEMUFile *f) {
    Guest_Host_Map *map = g_malloc0(sizeof(Guest_Host_Map));
    map->host_data = (GLubyte *)qemu_get_be64(f);
    map->map_len = qemu_get_be64(f);
    map->target = qemu_get_be32(f);
    map->access = qemu_get_be32(f);
    return map;
}

void save_thread_unique_ids(QEMUFile *f, GHashTable *thread_unique_ids) {
    GHashTableIter iter;
    gpointer key;
    
    guint unique_id_count = g_hash_table_size(thread_unique_ids);
    qemu_put_be32(f, unique_id_count);

    g_hash_table_iter_init(&iter, thread_unique_ids);
    while (g_hash_table_iter_next(&iter, &key, NULL)) {
        uint64_t unique_id = (uint64_t)key;
        qemu_put_be64(f, unique_id);
    }
}

GHashTable* load_thread_unique_ids(QEMUFile *f) {
    GHashTable *thread_unique_ids = g_hash_table_new(g_direct_hash, g_direct_equal);
    
    guint unique_id_count = qemu_get_be32(f);

    for (guint i = 0; i < unique_id_count; i++) {
        uint64_t unique_id = qemu_get_be64(f);
        g_hash_table_insert(thread_unique_ids, GUINT_TO_POINTER(unique_id), GUINT_TO_POINTER(1));
    }

    return thread_unique_ids;
}


void save_egl_display(QEMUFile *f, Egl_Display *display) {
    guint num_configs = g_hash_table_size(display->egl_config_set);
    qemu_put_be32(f, num_configs);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, display->egl_config_set);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        eglConfig *config = (eglConfig *)value;
        save_egl_config(f, config);
    }

    qemu_put_be32(f, display->guest_ver_major);
    qemu_put_be32(f, display->guest_ver_minor);
    qemu_put_be32(f, display->is_init);
}

Egl_Display* load_egl_display(QEMUFile *f) {
    Egl_Display *display = g_malloc0(sizeof(Egl_Display));
    display->egl_config_set = g_hash_table_new(g_direct_hash, g_direct_equal);

    guint num_configs = qemu_get_be32(f);

    for (guint i = 0; i < num_configs; i++) {
        eglConfig *config = load_egl_config(f);
        g_hash_table_insert(display->egl_config_set, GUINT_TO_POINTER(config->config_id), config);
    }

    display->guest_ver_major = qemu_get_be32(f);
    display->guest_ver_minor = qemu_get_be32(f);
    display->is_init = qemu_get_be32(f);

    return display;
}

void save_egl_config(QEMUFile *f, eglConfig *config) {
    qemu_put_be32(f, config->red_size);
    qemu_put_be32(f, config->green_size);
    qemu_put_be32(f, config->blue_size);
    qemu_put_be32(f, config->alpha_size);
    qemu_put_be32(f, config->bind_to_tex_rgb);
    qemu_put_be32(f, config->bind_to_tex_rgba);
    qemu_put_be32(f, config->caveat);
    qemu_put_be32(f, config->config_id);
    qemu_put_be32(f, config->frame_buffer_level);
    qemu_put_be32(f, config->depth_size);
    qemu_put_be32(f, config->max_pbuffer_width);
    qemu_put_be32(f, config->max_pbuffer_height);
    qemu_put_be32(f, config->max_pbuffer_size);
    qemu_put_be32(f, config->max_swap_interval);
    qemu_put_be32(f, config->min_swap_interval);
    qemu_put_be32(f, config->native_renderable);
    qemu_put_be32(f, config->renderable_type);
    qemu_put_be32(f, config->native_visual_id);
    qemu_put_be32(f, config->native_visual_type);
    qemu_put_be32(f, config->sample_buffers_num);
    qemu_put_be32(f, config->samples_per_pixel);
    qemu_put_be32(f, config->stencil_size);
    qemu_put_be32(f, config->luminance_size);
    qemu_put_be32(f, config->buffer_size);
    qemu_put_be32(f, config->surface_type);
    qemu_put_be32(f, config->transparent_type);
    qemu_put_be32(f, config->trans_red_val);
    qemu_put_be32(f, config->trans_green_val);
    qemu_put_be32(f, config->trans_blue_val);
    qemu_put_be32(f, config->conformant);
    qemu_put_be32(f, config->color_buffer_type);
    qemu_put_be32(f, config->alpha_mask_size);
    qemu_put_be32(f, config->recordable_android);
    qemu_put_be32(f, config->framebuffer_target_android);
}

eglConfig* load_egl_config(QEMUFile *f) {
    eglConfig *config = g_malloc0(sizeof(eglConfig));
    config->red_size = qemu_get_be32(f);
    config->green_size = qemu_get_be32(f);
    config->blue_size = qemu_get_be32(f);
    config->alpha_size = qemu_get_be32(f);
    config->bind_to_tex_rgb = qemu_get_be32(f);
    config->bind_to_tex_rgba = qemu_get_be32(f);
    config->caveat = qemu_get_be32(f);
    config->config_id = qemu_get_be32(f);
    config->frame_buffer_level = qemu_get_be32(f);
    config->depth_size = qemu_get_be32(f);
    config->max_pbuffer_width = qemu_get_be32(f);
    config->max_pbuffer_height = qemu_get_be32(f);
    config->max_pbuffer_size = qemu_get_be32(f);
    config->max_swap_interval = qemu_get_be32(f);
    config->min_swap_interval = qemu_get_be32(f);
    config->native_renderable = qemu_get_be32(f);
    config->renderable_type = qemu_get_be32(f);
    config->native_visual_id = qemu_get_be32(f);
    config->native_visual_type = qemu_get_be32(f);
    config->sample_buffers_num = qemu_get_be32(f);
    config->samples_per_pixel = qemu_get_be32(f);
    config->stencil_size = qemu_get_be32(f);
    config->luminance_size = qemu_get_be32(f);
    config->buffer_size = qemu_get_be32(f);
    config->surface_type = qemu_get_be32(f);
    config->transparent_type = qemu_get_be32(f);
    config->trans_red_val = qemu_get_be32(f);
    config->trans_green_val = qemu_get_be32(f);
    config->trans_blue_val = qemu_get_be32(f);
    config->conformant = qemu_get_be32(f);
    config->color_buffer_type = qemu_get_be32(f);
    config->alpha_mask_size = qemu_get_be32(f);
    config->recordable_android = qemu_get_be32(f);
    config->framebuffer_target_android = qemu_get_be32(f);

    return config;
}
