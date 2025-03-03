/**
 * @file express_gpu.c
 * @author gaodi (gaodi.sec@qq.com)
 * @brief 这个文件是用于定义express_gpu设备的文件，包含了direct用来调用的接口和涉及到的专用结构体
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020
 *
 */

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_device_common.h"

#include "hw/teleport-express/express_log.h"

#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/express_gpu.h"
#include "hw/express-gpu/express_gpu_render.h"
#include "hw/express-gpu/egl_draw.h"

#include "hw/express-gpu/glv3_trans.h"
#include "hw/express-gpu/egl_trans.h"
#include "hw/express-gpu/test_trans.h"
#include "hw/express-gpu/device_interface_window.h"

#include "hw/express-gpu/express_display.h"
#include "hw/express-gpu/express_gpu_snapshot.h"
#include "hw/express-gpu/glv3_resource.h"

#include "hw/express-gpu/vk_trans.h"


#include "qemu/atomic.h"

// 用于保存draw线程信息的hash表，方便分发到相应的线程
static GHashTable *render_thread_contexts = NULL;

static GHashTable *render_process_contexts = NULL;

bool express_gpu_gl_debug_enable = false;
bool express_gpu_independ_window_enable = false;
bool express_device_input_window_enable = false;
bool teleport_express_save_snapshot = true;

static void g_surface_map_destroy(gpointer data);

static void g_context_map_destroy(gpointer data);

static void gbuffer_map_destroy(gpointer data);

void init_render_thread_contexts_resources() {
    if (render_thread_contexts == NULL)
    {
        render_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

        render_process_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    if (g_resource_list[0] == NULL){ //ztodo:要放在这里吗？？？
        LOGD("in init_render_thread_contexts_resources");
        for (int i = 0; i < NUM_RESOURCES; i++) {
            ATOMIC_LOCK(g_resource_locker[i]);
            g_resource_list[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
            ATOMIC_UNLOCK(g_resource_locker[i]);
        }
    }
}

Process_Context* get_process_context_form_id(uint64_t process_id) {
    return g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
}

int save_render_process_contexts(QEMUFile *f)
{
    GHashTableIter iter;
    gpointer key, value;
    guint num_entries = g_hash_table_size(render_process_contexts);
    LOGD("in save_render_process_contexts with %d", num_entries);
    qemu_put_be32(f, num_entries);
    g_hash_table_iter_init(&iter, render_process_contexts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        
        Process_Context *process_context = (Process_Context *)value;
        qemu_put_be64(f, (uint64_t)key);
        LOGI("process id is %d", key);
        save_process_context(f, process_context);   
    }
    LOGD("in save_render_process_contexts with size %d", g_hash_table_size(render_process_contexts));
    return 0;
}

int load_render_process_contexts(QEMUFile *f) {
    // g_hash_table_remove_all(render_process_contexts);
    // GHashTable *process_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    guint num_process = qemu_get_be32(f);
    uint64_t process_id;
    
    LOGI("num process in load is %d", num_process);
    for (guint i = 0; i < num_process; i++) {
        process_id = qemu_get_be64(f);
        Process_Context *process_context = g_malloc0(sizeof(Process_Context));
        load_process_context(f, process_context);
        LOGI("process id in load is %d %d %d", process_id, g_hash_table_size(process_context->surface_map), process_context->thread_cnt);
        g_hash_table_insert(render_process_contexts, GUINT_TO_POINTER(process_id), process_context);

    }
    LOGD("in load_render_process_contexts with size %d", g_hash_table_size(render_process_contexts));
    // render_process_contexts = process_contexts;
    return 0;
}

void remove_all_render_thread_contexts(){
    // GHashTableIter iter;
    // gpointer key, value;

    // g_hash_table_iter_init(&iter, render_thread_contexts);
    // while (g_hash_table_iter_next(&iter, &key, &value)) {
    //     Render_Thread_Context *thread_context = (Render_Thread_Context *)value;
    //     ((Thread_Context*)thread_context)->context_destroy(thread_context);
        
    // }
    g_hash_table_remove_all(render_thread_contexts); //ztodo:很多资源还没销毁
}

int save_render_thread_contexts(QEMUFile *f)
{
    LOGD("in save render thread contexts!");
    GHashTableIter iter;
    gpointer key, value;
    guint num_entries = g_hash_table_size(render_thread_contexts);

    qemu_put_be32(f, num_entries);
    g_hash_table_iter_init(&iter, render_thread_contexts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Render_Thread_Context *thread_context = (Render_Thread_Context *)value;

        qemu_put_be64(f, (uint64_t)key);
        LOGI("thread id key is %lld", key);
        save_single_render_thread_context(f, thread_context);
    }

    return 0;
}

int load_render_thread_contexts(QEMUFile *f) {
    // GHashTable *thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    guint num_entries = qemu_get_be32(f);
    uint64_t thread_id;

    LOGD("in load render thread contexts with num entries %d!", num_entries);

    for (guint i = 0; i < num_entries; i++) {
        Render_Thread_Context *thread_context;
        thread_id = qemu_get_be64(f);
        LOGI("current loading thread id is %lld", thread_id);

        thread_context = load_single_render_thread_context(f);

        g_hash_table_insert(render_thread_contexts, GUINT_TO_POINTER(thread_id), thread_context);
        // g_free(thread_context);
    }

    // render_thread_contexts = thread_contexts;

    return 0;
}

void restore_opengl_vao_binding(Opengl_Context *context) {
    Bound_Buffer *bound_buffer = &(context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);

    GHashTable* new_vao_point_data = g_hash_table_new(g_direct_hash, g_direct_equal);
    // Attrib_Point *now_point = NULL;
    
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, bound_buffer->vao_point_data);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        uint64_t old_vao = (uint64_t)key;
        Attrib_Point *attrib_point = (Attrib_Point *)value;

        GLuint new_vao = 0;

        glGenVertexArrays(1, &new_vao);
        LOGI("vao point data %d %d %d %d", old_vao, new_vao, attrib_point->indices_buffer_len, attrib_point->indices_buffer_object);

        g_hash_table_insert(new_vao_point_data, GUINT_TO_POINTER(new_vao), (gpointer)attrib_point);

        if(old_vao == context->vao0) {
            context->vao0 = new_vao;
        }
        if(old_vao == status->guest_vao) {
            status->guest_vao = new_vao;
        }
    }

            // LOGI("current ebo should be %d %d", current_ebo, (&(opengl_context->bound_buffer_status.buffer_status))->guest_element_array_buffer);
        // LOGI("current vbo should be %d %d", current_vbo, (&(opengl_context->bound_buffer_status.buffer_status))->guest_array_buffer);

    bound_buffer->vao_point_data = new_vao_point_data;
    GLuint now_vao = status->guest_vao;
    if (status->guest_vao == 0)
    {
        now_vao = context->vao0;
    }
    // Attrib_Point *attrib_point = (Attrib_Point *)value;
    LOGI("current vao is %d", now_vao);
    glBindVertexArray(now_vao);
    Attrib_Point *now_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(now_vao));
    for(int i = 0; i < MAX_VERTEX_ATTRIBS_NUM; i++) {
        if(now_point->divisors[i] != 0) {
            glVertexAttribDivisor(i, now_point->divisors[i]);
            LOGI("restoring divisor %d %d", i, now_point->divisors[i]);
        }
    }
    // GLenum error = glGetError();
    // if (error == GL_NO_ERROR) {
    //     LOGI("VAO is valid.");
    // } else {
    //     LOGI("VAO is invalid.");
    // }

    // GLuint current_ebo = status->guest_element_array_buffer;
    // GLuint current_vbo = status->guest_array_buffer;

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_ebo);
    // glBindBuffer(GL_ARRAY_BUFFER, current_vbo);

    //ztodo:恢复Attrib_Point里面的数据

        //     if (status->guest_element_array_buffer != status->host_element_array_buffer)
        // {
        //     status->host_element_array_buffer = status->guest_element_array_buffer;
        //     new_buffer = status->host_element_array_buffer;
        //     need_sync = 1;
        // }
    

    // Attrib_Point *now_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(now_vao));

    // LOGD("in loading vao context %llx window %llx bind vao host %d", (uint64_t)context, (uint64_t)context->window, now_vao);

    // if (now_point == NULL)
    // {
    //     now_point = g_hash_table_lookup(bound_buffer->vao_point_data, GUINT_TO_POINTER(0));
    //     LOGE("error! vao %d cannot find with g_hash_table size %d", now_vao, g_hash_table_size(bound_buffer->vao_point_data));
    //     return;
    // }

    // bound_buffer->attrib_point = now_point;

    // status->guest_element_array_buffer = now_point->element_array_buffer;

    // status->guest_vao_ebo = now_point->element_array_buffer;

    // status->guest_vao = now_vao;




    // GLenum error = glGetError();
    // if (error == GL_NO_ERROR) {
    //     LOGI("VAO is valid.");
    // } else {
    //     LOGI("VAO is invalid.");
    // }
    // GLint current_vao;
    // glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);
    // if (current_vao == 0) {
    //     LOGE("No VAO bound! context %llx window %llx", (int64_t)context, (int64_t)context->window);
    // }

//     if (host_opengl_version < 45 || DSA_enable == 0) //ztodo：在别处添加这个判断
//     {
//         glBindVertexArray(now_vao);
//     }
}

void restore_buffers_binding(Opengl_Context *context) {
    Bound_Buffer *bound_buffer = &(context->bound_buffer_status);
    Buffer_Status *status = &(bound_buffer->buffer_status);


    if(status->guest_element_array_buffer != status->host_element_array_buffer) {
        status->host_element_array_buffer = status->guest_element_array_buffer;
    }

    if(status->guest_array_buffer != status->host_array_buffer) {
        status->host_array_buffer = status->guest_array_buffer;
    }

    GLuint current_ebo = status->guest_element_array_buffer;
    GLuint current_vbo = status->guest_array_buffer;
    LOGI("current vbo and ebo should be %d %d", current_vbo, current_ebo);

    
    glBindBuffer(GL_ARRAY_BUFFER, current_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_ebo);
    
    if(status->guest_copy_read_buffer != status->host_copy_read_buffer) {
        status->host_copy_read_buffer = status->guest_copy_read_buffer;
    }
    glBindBuffer(GL_COPY_READ_BUFFER, status->host_copy_read_buffer);

    if(status->guest_copy_write_buffer != status->host_copy_write_buffer) {
        status->host_copy_write_buffer = status->guest_copy_write_buffer;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, status->host_copy_write_buffer);

    if(status->guest_pixel_pack_buffer != status->host_pixel_pack_buffer) {
        status->host_pixel_pack_buffer = status->guest_pixel_pack_buffer;
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, status->host_pixel_pack_buffer);

    if(status->guest_pixel_unpack_buffer != status->host_pixel_unpack_buffer) {
        status->host_pixel_unpack_buffer = status->guest_pixel_unpack_buffer;
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, status->host_pixel_unpack_buffer);

    LOGI("in restore buffer binding bind pixel pack buffer %d unpack buffer %d", status->host_pixel_pack_buffer, status->host_pixel_unpack_buffer);

    if(status->guest_transform_feedback_buffer != status->host_transform_feedback_buffer) {
        status->host_transform_feedback_buffer = status->guest_transform_feedback_buffer;
    }
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, status->host_transform_feedback_buffer);

    if(status->guest_uniform_buffer != status->host_uniform_buffer) {
        status->host_uniform_buffer = status->guest_uniform_buffer;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, status->host_uniform_buffer);

    if(status->guest_atomic_counter_buffer != status->host_atomic_counter_buffer) {
        status->host_atomic_counter_buffer = status->guest_atomic_counter_buffer;
    }
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, status->host_atomic_counter_buffer);

    if(status->guest_dispatch_indirect_buffer != status->host_dispatch_indirect_buffer) {
        status->host_dispatch_indirect_buffer = status->guest_dispatch_indirect_buffer;
    }
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, status->host_dispatch_indirect_buffer);

    if(status->guest_draw_indirect_buffer != status->host_draw_indirect_buffer) {
        status->host_draw_indirect_buffer = status->guest_draw_indirect_buffer;
    }
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, status->host_draw_indirect_buffer);

    if(status->guest_shader_storage_buffer != status->host_shader_storage_buffer) {
        status->host_shader_storage_buffer = status->guest_shader_storage_buffer;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, status->host_shader_storage_buffer);

    if(status->guest_texture_buffer != status->host_texture_buffer) {
        status->host_texture_buffer = status->guest_texture_buffer;
    }
    glBindBuffer(GL_TEXTURE_BUFFER, status->host_texture_buffer);

}

void restore_framebuffer_binding(Opengl_Context *context) {

    // GLuint new_read_fbo0 = restore_single_framebuffer(f, context->read_fbo0);
    // GLuint new_draw_fbo0 = restore_single_framebuffer(context->draw_fbo0);

    // context->read_fbo0 = new_read_fbo0;
    // context->draw_fbo0 = new_draw_fbo0;

    GHashTableIter iter;
    gpointer key, value;
    GHashTable *resource_list = context->framebuffer_map;

    GHashTable* loaded_framebuffers = g_hash_table_new(g_direct_hash, g_direct_equal);

    LOGI("current framebuffer resource size is %d", g_hash_table_size(resource_list));

    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_Framebuffer *framebuffer = (Express_Native_Framebuffer *)value;
        GLuint framebuffer_id = framebuffer->framebufferId;
        LOGI("in loop of loading framebuffer ID: %d", framebuffer_id);
        if(g_hash_table_lookup(loaded_framebuffers, GUINT_TO_POINTER(framebuffer_id)) == NULL) {
            GLint new_framebuffer_id;
            // glDeleteFramebuffers(1, (GLuint*)&framebuffer_id);
            glGenFramebuffers(1, (GLuint*)&new_framebuffer_id);
            // change_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, framebuffer_id, new_framebuffer_id);

            LOGI("loading framebuffer ID: old %d new %d", framebuffer_id, new_framebuffer_id);

            GHashTableIter buffer_iter;
            gpointer buffer_key, buffer_value;
            g_hash_table_iter_init(&buffer_iter, loaded_hardware_buffers);
            while (g_hash_table_iter_next(&buffer_iter, &buffer_key, &buffer_value)) {
                Hardware_Buffer *buffer = (Hardware_Buffer *)buffer_value;
                if(buffer->data_fbo == framebuffer_id && buffer->data_fbo_changed == 0) {
                    LOGI("hardware buffer loading framebuffer ID: buffer %d %d %d", buffer->data_fbo, buffer->sampler_fbo, new_framebuffer_id);
                    buffer->data_fbo_changed = 1;
                    buffer->data_fbo = new_framebuffer_id;
                }
                if(buffer->sampler_fbo == framebuffer_id && buffer->sampler_fbo_changed == 0) {
                    buffer->sampler_fbo_changed = 1;
                    buffer->sampler_fbo = new_framebuffer_id;
                }
                
            }
            
            g_hash_table_iter_init(&buffer_iter, loaded_window_buffers);
            while (g_hash_table_iter_next(&buffer_iter, &buffer_key, &buffer_value)) {
                Window_Buffer *buffer = (Window_Buffer *)buffer_value;
                for(int i = 0; i < 3; i++) {
                    if(buffer->data_fbo[i] == framebuffer_id && buffer->date_fbo_changed[i] == 0) {
                        LOGI("window buffer loading framebuffer ID: buffer %d %d %d", buffer->data_fbo[i], buffer->sampler_fbo[i], new_framebuffer_id);
                        buffer->date_fbo_changed[i] = 1;
                        buffer->data_fbo[i] = new_framebuffer_id;
                        
                    }
                    if(buffer->sampler_fbo[i] == framebuffer_id && buffer->sampler_fbo_changed[i] == 0) {
                        buffer->sampler_fbo_changed[i] = 1;
                        buffer->sampler_fbo[i] = new_framebuffer_id;
                    }
                    
                }
            }

            g_hash_table_insert(loaded_framebuffers, GUINT_TO_POINTER(framebuffer_id), GUINT_TO_POINTER(new_framebuffer_id));
            
        }
        GLint new_framebuffer_id = (GLint)g_hash_table_lookup(loaded_framebuffers, GUINT_TO_POINTER(framebuffer_id));
        framebuffer->framebufferId = new_framebuffer_id;
        // if(framebuffer->attachment_target != 0) {
        //     restore_single_framebuffer(framebuffer);
        // }
        restore_single_framebuffer(framebuffer);
    }

    GHashTable *new_resource_list = g_hash_table_new(g_direct_hash, g_direct_equal);

    g_hash_table_iter_init(&iter, loaded_framebuffers);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLuint fb_id = (GLuint)key;
        GLuint new_id = (GLuint)value;
        LOGI("has loaded framebuffer ID: %d %d", (int)key, (int)value);
        Express_Native_Framebuffer *framebuffer = (Express_Native_Framebuffer *)g_hash_table_lookup(resource_list, GUINT_TO_POINTER(fb_id));
        // g_hash_table_remove(resource_list, GUINT_TO_POINTER(fb_id));
        g_hash_table_insert(new_resource_list, GUINT_TO_POINTER(new_id), framebuffer);
    }

    g_hash_table_remove_all(resource_list);

    g_hash_table_iter_init(&iter, new_resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        g_hash_table_insert(resource_list, key, value);
    }

    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_Framebuffer *framebuffer = (Express_Native_Framebuffer *)value;
        LOGI("framebuffer ID: %d key %d", framebuffer->framebufferId, (int)key);
    }



    Resource_Map_Status* framebuffer_resource = (&context->resource_status)->frame_buffer_resource; //ztodo:exclusive_resources又咋办呢
    for(int i = 0; i < framebuffer_resource->map_size; i++) {
        GLuint fb_id = framebuffer_resource->resource_id_map[i];
        GLuint new_id = (GLuint)g_hash_table_lookup(loaded_framebuffers, GUINT_TO_POINTER(fb_id));

        Express_Native_Framebuffer *framebuffer = (Express_Native_Framebuffer *)g_hash_table_lookup(resource_list, GUINT_TO_POINTER(fb_id));
        // g_hash_table_remove(resource_list, GUINT_TO_POINTER(fb_id));
        // g_hash_table_insert(resource_list, GUINT_TO_POINTER(new_id), framebuffer);

        LOGI("has old framebuffer of guest id %d id %d new %d", i, fb_id, new_id);
        framebuffer_resource->resource_id_map[i] = new_id;
    }
    
    GLint new_read_fbo0 = (GLint)g_hash_table_lookup(loaded_framebuffers, GUINT_TO_POINTER(context->read_fbo0));
    GLint new_draw_fbo0 = (GLint)g_hash_table_lookup(loaded_framebuffers, GUINT_TO_POINTER(context->draw_fbo0));
    // GLint new_read_fbo0 = (GLint)get_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, context->read_fbo0);
    // GLint new_draw_fbo0 = (GLint)get_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, context->draw_fbo0);

    LOGI("new read fbo0 is old %d %d new draw fbo0 is new %d %d",context->read_fbo0, context->draw_fbo0, new_read_fbo0, new_draw_fbo0);
    context->read_fbo0 = new_read_fbo0;
    context->draw_fbo0 = new_draw_fbo0;

    if(context->current_read_fbo != 0){ //ztodo:这几个的号需要映射一下
        glBindFramebuffer(GL_READ_FRAMEBUFFER, context->current_read_fbo);
    } else {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, context->read_fbo0);
    }
    LOGI("current read fbo is %d read fbo0 is %d", context->current_read_fbo, context->read_fbo0);


    if(context->current_write_fbo != 0){
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, context->current_write_fbo);
    } else {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, context->draw_fbo0);
    }

    GLint current_fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_fbo);
    LOGI("now current fbo is %d", current_fbo);

    GLuint glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("error! restore_framebuffer_binding glGetError %x", glerror);
    }

    g_free(loaded_framebuffers);


    LOGI("current write fbo is %d write fbo0 is %d", context->current_write_fbo, context->draw_fbo0);

}

void recover_snapshot_states_after_load(Render_Thread_Context* thread_context) { //ztodo:这些操作的顺序？
    
    Opengl_Context* opengl_context = thread_context->opengl_context;
    // Texture_Binding_Status *status = &(opengl_context->texture_binding_status);
    LOGD("in recover_snapshot_states_after_load for process %d opengl window %lld", ((Thread_Context*)thread_context)->thread_id, (uint64_t)opengl_context->window);
    Window_Buffer * real_surface_draw = thread_context->render_double_buffer_draw;
    Window_Buffer * real_surface_read = thread_context->render_double_buffer_read; 

    // LOGI("draw fbo is %d %d", opengl_context->draw_fbo0, real_surface_draw->gbuffer->data_fbo);

    egl_makeCurrent(opengl_context->window);
    // opengl_context_init(opengl_context);
    // render_surface_init(real_surface_draw);
    // if (real_surface_read != real_surface_draw)
    // {
    //     render_surface_init(real_surface_read);
    // }
    // connect_gbuffer_to_surface(real_surface_draw->gbuffer, real_surface_draw);


    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    glBlendFunc(thread_context->opengl_context->blendfunc_sfactor, thread_context->opengl_context->blendfunc_dfactor);
#ifdef __APPLE__
    glPointSize(10.0f);
    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
    glEnable(GL_FRAMEBUFFER_SRGB);

    glViewport(opengl_context->view_x, opengl_context->view_y, opengl_context->view_w, opengl_context->view_h);

    glScissor(opengl_context->gl_scissor_value[0], opengl_context->gl_scissor_value[1], opengl_context->gl_scissor_value[2], opengl_context->gl_scissor_value[3]);

    GLuint glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("error! recover_snapshot_states_after_load glGetError %x", glerror);
    }

    restore_opengl_context_textures(opengl_context);

    glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("error! recover_snapshot_states_after_load after bind textures glerror %x", glerror);
    }

    restore_opengl_vao_binding(opengl_context);

    glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("error! recover_snapshot_states_after_load after bind vao glerror %x", glerror);
    }

    restore_buffers_binding(opengl_context);

    glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("error! recover_snapshot_states_after_load after bind buffers glerror %x", glerror);
    }

    restore_framebuffer_binding(opengl_context);

    glUseProgram(opengl_context->current_program);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);





    // if(opengl_context->current_read_fbo != 0){
    //     glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_context->current_read_fbo);
    // } else {
    //     glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_context->read_fbo0);
    // }
    
    // if(opengl_context->current_write_fbo != 0){
    //     glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_context->current_write_fbo);
    // } else {
    //     glBindFramebuffer(GL_READ_FRAMEBUFFER, opengl_context->draw_fbo0);
    // }

    if(!opengl_context->is_current) {
        egl_makeCurrent(NULL);
    }

}


/**
 * @brief 根据不同类型调用决定调用哪个版本的opengl
 *
 * @param call
 */
static void decode_invoke(Thread_Context *context, Teleport_Express_Call *call)
{

    Render_Thread_Context *render_context = (Render_Thread_Context *)context;

    uint64_t fun_id = GET_FUN_ID(call->id);
    LOGD("enter gpu decode invoke id %llu", fun_id);


    if (fun_id == 10001)
    {
        recover_snapshot_states_after_load(render_context); //ztodo:释放call的资源
        // usleep(1000000);
        return;
    }
    else if (fun_id >= 200000)
    {
        test_decode_invoke(render_context, call);
    }
    else if (fun_id >= 1000 && fun_id < 2000)
    {
        vk_decode_invoke(render_context, call);
        // LOGD("get call vkCreateDevice!");
        // const VkInstanceCreateInfo* pCreateInfo;
        // const VkAllocationCallbacks* pAllocator;

        // Call_Para all_para[MAX_PARA_NUM];      
        // int para_num = get_para_from_call(call, all_para, MAX_PARA_NUM);
        // LOGI("get vk param number %d", para_num);

        // int need_free = 0;
        // char *_ptr;
        // _ptr = call_para_to_ptr(all_para[0], &need_free);
        // VkInstanceCreateInfo* local_pCreateInfo = _ptr;
        // LOGI("got vkCreateinfo with %lld %d %s %d %s",(long long)local_pCreateInfo->sType, local_pCreateInfo->enabledLayerCount, local_pCreateInfo->ppEnabledLayerNames, local_pCreateInfo->enabledExtensionCount, local_pCreateInfo->ppEnabledExtensionNames);

    }
    else if (fun_id > 10000)
    {
        // LOGI("get egl call with id %lld", fun_id);
        egl_decode_invoke(render_context, call);
    }
    else if (fun_id == EXPRESS_CLUSTER_FUN_ID)
    {
        cluster_decode_invoke(call, context, (EXPRESS_DECODE_FUN)decode_invoke);
    }
    else
    {
        gl3_decode_invoke(render_context, call);
    }
    if (express_gpu_gl_debug_enable && render_context->opengl_context != NULL && render_context->opengl_context->is_current)
    {
        GLenum error_code = glGetError();
        while (error_code != GL_NO_ERROR)
        {
            LOGE("#fun_id %llu context %llx gl error %x", fun_id, (uint64_t)render_context->opengl_context, error_code);
            error_code = glGetError();
        }
    }

    // GLuint glerror = glGetError();
    // if (glerror != GL_NO_ERROR) {
    //     LOGE("error! decode_invoke glGetError %x", glerror);
    // }
    return;
}

static Thread_Context *get_render_thread_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *info)
{
    if (render_thread_contexts == NULL)
    {
        render_thread_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

        render_process_contexts = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    if (g_resource_list[0] == NULL){ //ztodo:要放在这里吗？？？
        LOGI("init resource list");
        for (int i = 0; i < NUM_RESOURCES; i++) {
            ATOMIC_LOCK(g_resource_locker[i]);
            g_resource_list[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
            ATOMIC_UNLOCK(g_resource_locker[i]);
        }
    }

    Render_Thread_Context *thread_context = (Render_Thread_Context *)g_hash_table_lookup(render_thread_contexts, GUINT_TO_POINTER(thread_id));
    // 没有context就新建线程
    LOGD("getting new thread context with process id %lld unique id %lld thread id %lld device id %lld", process_id, unique_id, thread_id, device_id);
    if (thread_context == NULL)
    {
        // express_printf("create new thread\n");
        LOGI("create new thread context with thread id %lld device id %lld", thread_id, device_id);
        thread_context = (Render_Thread_Context *)thread_context_create(thread_id, device_id, sizeof(Render_Thread_Context), info);

        (thread_context->context).unique_id = unique_id;
        (thread_context->context).process_id = process_id;

        thread_context->thread_unique_ids = g_hash_table_new(g_direct_hash, g_direct_equal);

        // 处理好process_context与thread_context的关系
        // 新建进程上下文
        Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
        if (process == NULL)
        {
            LOGI("create new process context %lld", process_id);
            process = g_malloc(sizeof(Process_Context));
            process->context_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_context_map_destroy);
            // 注意，从surface_map删除的时候不一定需要删除surface，所以这里为空，但是从native_window中删除却需要
            process->surface_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_surface_map_destroy);

            process->gbuffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, gbuffer_map_destroy);
            process->thread_cnt = 0;

            g_hash_table_insert(render_process_contexts, GUINT_TO_POINTER(process_id), (gpointer)process);
        }
        qatomic_inc(&(process->thread_cnt));
        thread_context->process_context = process;
        g_hash_table_insert(render_thread_contexts, GUINT_TO_POINTER(thread_id), (gpointer)thread_context);
    }
    LOGD("got render thread context with device id %lld %lld thread id %lld %lld", thread_context->context.device_id, device_id, thread_context->context.thread_id, thread_id);
    // 这里是只要是新的unique_id就都给加上，所以需要额外排除那种不是专门针对gpu device的调用
    // 例如更新显存数据时，显示指定了一次gpu device调用，但是其实其本质是display device调用，
    // 因此那种情况下，需要显示的删除该unique_id
    g_hash_table_insert(thread_context->thread_unique_ids, GUINT_TO_POINTER(unique_id), (gpointer)1);
    return (Thread_Context *)thread_context;
}

static bool remove_render_thread_context(uint64_t type_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, struct Express_Device_Info *inf)
{
    Render_Thread_Context *render_context = (Render_Thread_Context *)g_hash_table_lookup(render_thread_contexts, GUINT_TO_POINTER(thread_id));
    LOGI("going to remove render thread context with thread id %lld process id %lld unique id %lld", thread_id, process_id, unique_id);
    g_hash_table_remove(render_context->thread_unique_ids, GUINT_TO_POINTER(unique_id));

    if (g_hash_table_size(render_context->thread_unique_ids) == 0)
    {
        g_hash_table_remove(render_thread_contexts, GUINT_TO_POINTER(thread_id));
        Process_Context *process = g_hash_table_lookup(render_process_contexts, GUINT_TO_POINTER(process_id));
        if (process != NULL)
        {
            if (process->thread_cnt == 1)
            {
                g_hash_table_remove(render_process_contexts, GUINT_TO_POINTER(process_id));
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

static void render_context_init(Thread_Context *context)
{

    express_printf("render context init!\n");
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
}

static void g_surface_map_destroy(gpointer data)
{
    Window_Buffer *real_surface = (Window_Buffer *)data;
    express_printf("try free surface %llx\n", (uint64_t)real_surface);
    if (real_surface->is_current)
    {
        real_surface->need_destroy = 1;
    }
    else
    {
        render_surface_destroy(real_surface);
    }
}

static void g_context_map_destroy(gpointer data)
{
    Opengl_Context *real_context = (Opengl_Context *)data;
    if (real_context->is_current)
    {
        // 假如当前的context正在被使用，则需要等到context没有被使用了才能删除
        express_printf("context %llx guest %llx is using\n", (uint64_t)real_context, (uint64_t)real_context->guest_context);
        real_context->need_destroy = 1;
    }
    else
    {
        express_printf("destroy context %llx\n", (uint64_t)real_context);
        opengl_context_destroy(real_context);
        g_free(real_context);
    }
}

static void gbuffer_map_destroy(gpointer data)
{
    Hardware_Buffer *gbuffer = (Hardware_Buffer *)data;

    LOGI("destroy map gbuffer %llx type %d ptr %llx width %d height %d format %x type %d", gbuffer->gbuffer_id, gbuffer->usage_type, (uint64_t)gbuffer, gbuffer->width, gbuffer->height, gbuffer->internal_format, gbuffer->usage_type);

    //@todo 没有context时，能不能delete sync？所以暂时让主线程去释放sync
    if (gbuffer->usage_type == GBUFFER_TYPE_TEXTURE)
    {
        if (gbuffer->data_sync != NULL)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, gbuffer->data_sync);
        }
        if (gbuffer->delete_sync != NULL)
        {
            send_message_to_main_window(MAIN_DESTROY_ONE_SYNC, gbuffer->delete_sync);
        }
        g_free(gbuffer);
    }
    else
    {
        // 其他类型的gbuffer真实释放由mem线程完成
        LOGE("non_texture gbuffer in process context id %" PRIx64 " ptr %p", gbuffer->gbuffer_id, gbuffer);
    }

    return;
}

static void render_context_destroy(Thread_Context *context)
{
    Render_Thread_Context *thread_context = (Render_Thread_Context *)context;
    Process_Context *process_context = thread_context->process_context;

    // 这个函数的出现表示文件close了，通道都关掉了
    // 目前通道关掉只有一种可能，就是进程退出了

    g_hash_table_destroy(thread_context->thread_unique_ids);

    // 保证都不是current状态，确保能够删除成功
    if (thread_context->opengl_context != NULL)
    {
        d_eglMakeCurrent(thread_context, NULL, NULL, NULL, NULL, 0, 0, 0, 0);
    }

    // process_context->thread_cnt -= 1;
    express_printf("process %llx destroy cnt %d\n", (uint64_t)process_context, process_context->thread_cnt);
    if (qatomic_dec_fetch(&(process_context->thread_cnt)) == 0)
    {
        // 由最后一个退出的线程清空资源
        express_printf("process %llx destroy everything\n", (uint64_t)process_context);
        g_hash_table_destroy(process_context->context_map);

        g_hash_table_destroy(process_context->surface_map);

        // image删除，这里主要是为了释放gbuffer映射
        g_hash_table_destroy(process_context->gbuffer_map);

        g_free(process_context);
    }
}

static Express_Device_Info express_gpu_info = {
    .enable_default = true,
    .name = "express-gpu",
    .option_name = "gl",
    .driver_name = "express_gpu",
    .device_id = EXPRESS_GPU_DEVICE_ID,
    .device_type = OUTPUT_DEVICE_TYPE,
    .context_init = render_context_init,
    .context_destroy = render_context_destroy,
    .call_handle = decode_invoke,
    .get_context = get_render_thread_context,
    .remove_context = remove_render_thread_context,
};

EXPRESS_DEVICE_INIT(express_gpu, &express_gpu_info)
