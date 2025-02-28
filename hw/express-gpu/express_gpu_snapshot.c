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
#include "hw/express-gpu/egl_trans.h"
#include "hw/express-gpu/express_gpu_snapshot.h"
#include "hw/express-gpu/glv3_context.h"
#include "hw/express-gpu/glv3_program.h"
#include "hw/teleport-express/express_handle_thread.h"

#include "hw/teleport-express/teleport_express_register.h"
#include "hw/teleport-express/teleport_express_distribute.h"

#include "migration/qemu-file.h"

#include "exec/cpu-common.h"




//ztodo: 这些资源也得实现保存和加载
// GList *native_shaders = NULL;
// int native_shaders_num = 0;
// int native_shaders_locker = 0;
GHashTable *g_resource_list[NUM_RESOURCES];// = { NULL };
GHashTable *loaded_hardware_buffers;
GHashTable *loaded_window_buffers;
// int g_resource_count[NUM_RESOURCES] = { 0 };
int g_resource_locker[NUM_RESOURCES] = { 0 }; 

VirtIODevice *startup_vdev;
VirtQueue *startup_out_data_queue;
VirtQueue *startup_in_data_queue;

//hashtable肯定不是效率最高的，先这样吧.直接用数组肯定是最快的
static GHashTable *g_resource_ids_map[NUM_RESOURCES] = { NULL };


static __thread void *g_gl_context = NULL;
static bool is_init = false;

void init_saving_snapshot() {
    if (is_init) {
        return;
    }
    is_init = true;
    g_gl_context = get_native_opengl_context(0);
    egl_makeCurrent(g_gl_context);
    for (int i = 0; i < NUM_RESOURCES; i++) {
        g_resource_ids_map[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
        // g_resource_list[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
}

void init_loading_snapshot(QEMUFile *f) {
    LOGI("init_loading_snapshot");
    is_init = true;

    if (g_resource_list[0] == NULL){
        LOGD("in init_render_thread_contexts_resources");
        for (int i = 0; i < NUM_RESOURCES; i++) {
            ATOMIC_LOCK(g_resource_locker[i]);
            g_resource_list[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
            ATOMIC_UNLOCK(g_resource_locker[i]);
        }
    }

    init_render_thread_contexts_resources();

    Teleport_Express *g = TELEPORT_EXPRESS(startup_vdev);

    realize_input_device(startup_vdev);

    if (g->distribute_thread_run == 0) //第一次调用到，新建分发线程
    {
        LOGI("start handle thread");
        // guest_null_ptr_init(startup_out_data_queue);
        g->distribute_thread_run = 1;
        qemu_thread_create(&g->distribute_thread, "teleport-express-distribute", call_distribute_thread,
                           startup_vdev, QEMU_THREAD_JOINABLE);
    }
    if(g->input_thread_run == 0){
        qemu_thread_create(&g->input_thread, "teleport-express-input", input_sync_thread,
                           startup_vdev, QEMU_THREAD_JOINABLE);
        LOGI("start input thread");
        g->input_thread_run = 1;
    }
    if (qatomic_cmpxchg(&(g->register_input_vq_locker), 0, 1) == 0)
    {
        LOGI("register input buffer call");
        register_input_buffer_call(startup_vdev, startup_in_data_queue);
        qatomic_set(&(g->register_input_vq_locker), 0);
    }

    start_main_window_thread();

    g_gl_context = get_native_opengl_context(0);
    egl_makeCurrent(g_gl_context);
    for (int i = 0; i < NUM_RESOURCES; i++) {
        g_resource_ids_map[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
        // g_resource_list[i] = g_hash_table_new(g_direct_hash, g_direct_equal);
    }

    set_input_event_startup();

    LOGI("init_loading_snapshot end");
}

void clear_resource_tables() {
    for (int i = 0; i < NUM_RESOURCES; i++) {
        if (g_resource_ids_map[i] != NULL) {
            g_hash_table_remove_all(g_resource_ids_map[i]);
        }
    }
}

void change_host_id_map(int type, GLint old_id, GLint new_id){ //ztodo:记得每次load snapshot结束之后清空哈希表！
    g_hash_table_insert(g_resource_ids_map[type], GUINT_TO_POINTER(old_id), GUINT_TO_POINTER(new_id));
    return;
}

GLint get_host_id_map(int type, GLint old_id) {
    if (old_id == 0) {
        return 0;
    }
    GLint new_id = (GLint)g_hash_table_lookup(g_resource_ids_map[type], GUINT_TO_POINTER(old_id));
    if(!new_id) {
        LOGE("get_host_id_map failed, type %d old_id: %d", type, old_id);
    }
    return new_id;
}

// void save_native_vertex_arrays(QEMUFile *f) {
//     ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_VERTEX_ARRAY]);
//     GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_VERTEX_ARRAY];
//     qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many vertex arrays
//     LOGI("saving vertex array num %d", g_hash_table_size(resource_list));

//     GHashTableIter iter;
//     gpointer key, value;
//     g_hash_table_iter_init(&iter, resource_list);
//     while (g_hash_table_iter_next(&iter, &key, &value)) {
//         GLint vertex_array = (GLint)value;
//         LOGI("saving vertex array ID: %d", vertex_array);
//         qemu_put_be64(f, vertex_array);
//     }

//     ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_VERTEX_ARRAY]);
// }

// void load_native_vertex_arrays(QEMUFile *f) {
//     int vertex_array_num = qemu_get_be32(f);
//     for(int i = 0; i < vertex_array_num ; i++) {
//         uint64_t vertex_array_id = qemu_get_be64(f);

//         // glDeleteVertexArrays(1, (GLuint*)&vertex_array_id);
//         GLint new_vertex_array_id;
//         glGenVertexArrays(1, (GLuint*)&new_vertex_array_id);
//         LOGI("loading vertex array ID: old %d new %d",vertex_array_id, new_vertex_array_id);
//         change_host_id_map(RESOURCE_TYPE_VERTEX_ARRAY, vertex_array_id, new_vertex_array_id);
//     }

// }


void save_native_buffer(QEMUFile *f, GLint buffer_id, GLenum buffer_target) {
    LOGI("saving buffer ID: %d, Type: %d", buffer_id, buffer_target);
    qemu_put_be32(f, buffer_id);
    GLint size;
    glBindBuffer(buffer_target, buffer_id);
    glGetBufferParameteriv(buffer_target, GL_BUFFER_SIZE, &size);
    qemu_put_be32(f, size);
    if (size > 0) {
        void* data = g_malloc0(size);
        if (data) {
            glGetBufferSubData(buffer_target, 0, size, data);
            LOGI("saving buffer of content length %d", size);
            qemu_put_buffer(f, data, size);
            g_free(data);
        }
    }
    glBindBuffer(buffer_target, 0);
}

void load_native_buffer(QEMUFile *f, GLint buffer_id, GLenum buffer_target, int strategy) {
    GLint size = qemu_get_be32(f);
    GLint new_buffer_id = 0;
    glGenBuffers(1, (GLuint*)&new_buffer_id);
    change_host_id_map(RESOURCE_TYPE_BUFFER, buffer_id, new_buffer_id);

    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_BUFFER];
    Express_Native_buffer_Simple* newBuffer = g_malloc0(sizeof(Express_Native_buffer_Simple));
    newBuffer->bufferId = new_buffer_id;  
    newBuffer->target = buffer_target;     
    newBuffer->data_upload_strategy = strategy;
    g_hash_table_insert(resource_list, GUINT_TO_POINTER(new_buffer_id), newBuffer);            

    glBindBuffer(buffer_target, new_buffer_id);
    if (size > 0) {
        void* data = g_malloc0(size);
        if (data) {
            qemu_get_buffer(f, data, size);

                if(strategy == 2) {
                    glBufferData(buffer_target, size, NULL, GL_STATIC_DRAW);

                    GLubyte *map_pointer = glMapBufferRange(buffer_target, 0, size, 0xa);
                    memcpy(map_pointer, data, size);
                    glUnmapBuffer(buffer_target);   
                }
                else {
                    glBufferData(buffer_target, size, data, GL_STATIC_DRAW); //ztodo:测试mapbuffer是否更快,以及usage参数应该GL_STATIC_DRAW效率不够高

                }
            glGetBufferSubData(buffer_target, 0, size, data);

            g_free(data);
        }
    } else {
        void* data = malloc(size);
        if (data) {
            qemu_get_buffer(f, data, size);
            glBufferData(buffer_target, size, data, GL_STATIC_DRAW);
            LOGI("loading buffer context of size 0");
            free(data);
        }
    }
}

void save_native_buffers(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_BUFFER]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_BUFFER];
    qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many buffers
    LOGI("saving buffer num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_buffer_Simple* buffer = (Express_Native_buffer_Simple *)value;
        LOGI("saving buffer ID: %d, Type: %d", buffer->bufferId, buffer->target);
        qemu_put_be64(f, buffer->bufferId);
        qemu_put_be32(f, buffer->target);
        qemu_put_be32(f, buffer->data_upload_strategy);
        GLint size;
        glBindBuffer(buffer->target, buffer->bufferId);
        GLint glerror = glGetError();
        if (glerror != GL_NO_ERROR) {
            LOGE("error! glBindBuffer failed! gl error %x ", glerror);
        }
        glGetBufferParameteriv(buffer->target, GL_BUFFER_SIZE, &size);
        qemu_put_be32(f, size);
        if (size > 0) {
            void* data = g_malloc0(size);
            if (data) {
                memset(data, 0, size);
                glGetBufferSubData(buffer->target, 0, size, data);
                LOGI("saving buffer of content length %d", size);

                GLenum error = glGetError();
                if (error != GL_NO_ERROR) {
                    LOGI("error when reading buffer data");
                }

                qemu_put_buffer(f, data, size);
                g_free(data);
            }
        }
        glBindBuffer(buffer->target, 0);
    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_BUFFER]);
}

void load_native_buffers_tmp(QEMUFile *f) {
    int buffer_num = qemu_get_be32(f);
    for(int i = 0; i < buffer_num ; i++) {
        uint64_t buffer_id = qemu_get_be64(f);

        change_host_id_map(RESOURCE_TYPE_BUFFER, buffer_id, buffer_id);

        GLint target = qemu_get_be32(f);
        int strategy = qemu_get_be32(f);

        GLint size = qemu_get_be32(f);

        glBindBuffer(target, buffer_id);
        LOGI("loading buffer of id %d size %d type %x", buffer_id, size, target);

        if (size > 0) {
            void* data = g_malloc0(size);
            if (data) {
                                
                // glGetBufferSubData(target, 0, size, data);
                // if(size > 40000)
                //     LOGI("before loading buffer context first 4 bytes %x %x %x %x", ((char*)data)[9140], ((char*)data)[141], ((char*)data)[42], ((char*)data)[49151]);

                // memset(data, 0, size);
                
                // // glBufferSubData(target, 0, size, data);
                // if(size > 40000) {
                //     GLubyte *map_pointer = glMapBufferRange(target, 0, size, 0xa);
                //     memcpy(map_pointer, data, size);
                //     glUnmapBuffer(target);                    
                // }


                qemu_get_buffer(f, data, size);

                // if(target == GL_ARRAY_BUFFER) {
                //     glBufferSubData(target, 0, size, data);
                // }
                // memset(data, 0, size);
                // glGetBufferSubData(target, 0, size, data);
                // if(size > 40000)
                //     LOGI("loading buffer context first 4 bytes %x %x %x %x", ((char*)data)[1], ((char*)data)[12], ((char*)data)[22], ((char*)data)[16]);
                // GLuint glerror = glGetError();
                // if (glerror != GL_NO_ERROR) {
                //     LOGI("error when loading buffer!");
                // }
                g_free(data);
            }
        } else {
            void* data = malloc(size);
            if (data) {
                qemu_get_buffer(f, data, size);
                glBufferData(target, size, data, GL_STATIC_DRAW);
                LOGI("loading buffer context of size 0");
                free(data);
            }
        }


        GLint newsize;
        glGetBufferParameteriv(target, GL_BUFFER_SIZE, &newsize);
        LOGI("current buffer size is %d", newsize);


    }
}

void load_native_buffers(QEMUFile *f) {
    int buffer_num = qemu_get_be32(f);
    for(int i = 0; i < buffer_num ; i++) {
        uint64_t buffer_id = qemu_get_be64(f);

        GLint target = qemu_get_be32(f);
        int strategy = qemu_get_be32(f);
        GLint size = qemu_get_be32(f);

        

        // if(size == 49152) {
        //     LOGI("loading buffer ID: %d, Type: %x, Size: %d", buffer_id, target, size);

        //     change_host_id_map(RESOURCE_TYPE_BUFFER, buffer_id, buffer_id);
        //     if (size > 0) {
        //         void* data = malloc(size);
        //         if (data) {
        //             qemu_get_buffer(f, data, size);
        //             free(data);
        //         }
        //     }
        //     continue;
        // }
            
        // glDeleteBuffers(1, (GLuint*)&buffer_id);
        GLint new_buffer_id = 0;
        // while(new_buffer_id != buffer_id)
        glGenBuffers(1, (GLuint*)&new_buffer_id);



        change_host_id_map(RESOURCE_TYPE_BUFFER, buffer_id, new_buffer_id);



        LOGI("loading buffer ID: old %d new %d, Type: %x, Size: %d strategy %d", buffer_id, new_buffer_id, target, size, strategy);
        glBindBuffer(target, new_buffer_id);
        if (size > 0) {
            void* data = g_malloc0(size);
            if (data) {
                qemu_get_buffer(f, data, size);

                 if(strategy == 2) {
                    // glBufferData(target, size, data, GL_STATIC_DRAW);

                    glBufferData(target, size, NULL, GL_STATIC_DRAW);

                    GLubyte *map_pointer = glMapBufferRange(target, 0, size, 0xa);
                    memcpy(map_pointer, data, size);
                    glUnmapBuffer(target);   
                 }
                 else {
                    glBufferData(target, size, data, GL_STATIC_DRAW); //ztodo:测试mapbuffer是否更快,以及usage参数应该GL_STATIC_DRAW效率不够高

                 }
                // LOGI("real data loading buffer context first 4 bytes %x %x %x %x", ((char*)data)[1], ((char*)data)[12], ((char*)data)[22], ((char*)data)[16]);
                     
                // memset(data, 0, size);
                glGetBufferSubData(target, 0, size, data); //这句必须要有，或许是为了同步数据。

                g_free(data);
            }
        } 
        // else {
        //     void* data = malloc(size);
        //     if (data) {
        //         qemu_get_buffer(f, data, size);
        //         glBufferData(target, size, data, GL_STATIC_DRAW);
        //         LOGI("loading buffer context of size 0");
        //         free(data);
        //     }
        // }
    }
}

void save_single_framebuffer(QEMUFile* f, Express_Native_Framebuffer *framebuffer){
    qemu_put_be64(f, framebuffer->framebufferId);
    // qemu_put_be32(f, framebuffer->attachment_target);
    for(int i = 0; i < 16; i++) {
        qemu_put_be32(f, framebuffer->attachment_target[i]);
        LOGI("saving framebuffer attachment %d %d", i, framebuffer->attachment_target[i]);
    }
    for(int i = 0; i < 19; i++) {
        qemu_put_be32(f, framebuffer->renderbuffer_attachment[i]);
        LOGI("saving framebuffer renderbuffer attachment %d %d", i, framebuffer->renderbuffer_attachment[i]);
    }
    qemu_put_be64(f, framebuffer->texture_id);


    // glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

    // GLenum err = glGetError();
    // if (err != GL_NO_ERROR) {
    //     LOGE("glBindFramebuffer error: %d", err);
    //     // 处理错误
    // }

    // for (int i = 0; i < 16; i++) {
    //     GLint attachment_object;
    //     glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attachment_object);
    //     LOGI("saving framebuffer attachment %d %d", i, attachment_object);
    //     if (attachment_object != 0) {
    //         qemu_put_be32(f, attachment_object);
    //     } else {
    //         qemu_put_be32(f, attachment_object);
    //         break;
    //     }
    // }

    // //深度附件
    // GLint depth_attachment;
    // glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depth_attachment);
    // qemu_put_be32(f, depth_attachment);

    // //模板附件
    // GLint stencil_attachment;
    // glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &stencil_attachment);
    // qemu_put_be32(f, stencil_attachment);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // LOGI("saving framebuffer all info %d %d %d", framebufferId, depth_attachment, stencil_attachment);
}

void save_native_framebuffers(QEMUFile *f, GHashTable* resource_list) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);
    // GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];
    qemu_put_be32(f, g_hash_table_size(resource_list));
    LOGI("saving framebuffer num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_Framebuffer *framebuffer = (Express_Native_Framebuffer *)value;
        // GLuint framebufferId = (GLuint )key;
        LOGI("saving framebuffer ID: %d", framebuffer->framebufferId);
        save_single_framebuffer(f, framebuffer);
    }


    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);
}

void restore_single_framebuffer(Express_Native_Framebuffer *framebuffer) {
    GLuint glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("error! before restore_single_framebuffer glGetError %x", glerror);
    }

    GLuint framebuffer_id = framebuffer->framebufferId;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    for(int i = 0; i < 16; i++) {
        if(framebuffer->attachment_target[i] != 0) {
            GLuint attachment_type = GL_COLOR_ATTACHMENT0 + i;
            GLuint texture_id = framebuffer->attachment_target[i];
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D, texture_id, 0);
            LOGI("loading framebuffer attachment %d old %d new %d", i, texture_id, attachment_type);
        }    
    }
    if(framebuffer->renderbuffer_attachment[0] != 0) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->renderbuffer_attachment[0]);
        LOGI("loading framebuffer attachment depth old %d new %d", framebuffer->renderbuffer_attachment[0], GL_DEPTH_ATTACHMENT);
    }
    if(framebuffer->renderbuffer_attachment[1] != 0) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->renderbuffer_attachment[1]);
        LOGI("loading framebuffer attachment GL_STENCIL_ATTACHMENT old %d new %d", framebuffer->renderbuffer_attachment[0], GL_DEPTH_ATTACHMENT);

    }
    if(framebuffer->renderbuffer_attachment[2] != 0) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->renderbuffer_attachment[2]);
    }
    for(int i = 3; i < 19; i++) {
        if(framebuffer->renderbuffer_attachment[i] != 0) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, framebuffer->renderbuffer_attachment[i]);
        }
    }

    // GLuint framebuffer_id = old_framebuffer_id;

    // glDeleteFramebuffers(1, (GLuint*)&framebuffer_id);
    // GLint new_framebuffer_id;
    // glGenFramebuffers(1, (GLuint*)&new_framebuffer_id);


    // LOGI("loading framebuffer ID: old %d new %d", framebuffer_id, new_framebuffer_id);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D, texture_id, 0);

    // for (int j = 0; j < 16; j++) {
    //     GLuint attachment_object = qemu_get_be32(f);
    //     if (attachment_object != 0) {
    //         GLuint new_attachment_object = get_host_id_map(RESOURCE_TYPE_TEXTURE, attachment_object);
    //         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + j, GL_TEXTURE_2D, new_attachment_object, 0);
    //         LOGI("loading framebuffer attachment %d old %d new %d", j, attachment_object, new_attachment_object);
    //     } else {
    //         break;
    //     }
    // }

    // //深度附件
    // GLuint depth_attachment = qemu_get_be32(f);
    // if (depth_attachment != 0) {
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment, 0);
    // }

    // //模板附件
    // GLuint stencil_attachment = qemu_get_be32(f);
    // if (stencil_attachment != 0) {
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_attachment, 0);
    // }

    glerror = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (glerror != GL_FRAMEBUFFER_COMPLETE) {
        LOGE("error! framebuffer not complete! status %x gl error %x", glerror, glGetError());
        // GLint currentTexture;
        // glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture); 

        // GLint width, height, internalFormat, format, type;

        // GLboolean isValid = glIsTexture(texture_id);
        // if (isValid) {
        //     LOGI("Texture is valid");
        // } else {
        //     LOGI("Texture is invalid");
        // }


        // // 绑定指定纹理 ID 进行检查
        // glBindTexture(GL_TEXTURE_2D, texture_id);

        // GLenum bindError = glGetError();
        // if (bindError != GL_NO_ERROR) {
        //     glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture); 

        //     LOGE("Error occurred during texture bind operation: %x current %d new %d", bindError, currentTexture, texture_id);
        // }        

        // // 获取纹理尺寸
        // glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        // glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        // // 获取纹理的内部格式
        // glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

        // // 输出纹理的属性
        // LOGE("Texture ID: %d Width: %d, Height: %d, Internal Format: %d", texture_id, width, height, internalFormat, format, type);

        // // 恢复之前的纹理绑定状态
        // glBindTexture(GL_TEXTURE_2D, currentTexture);

        // 可选：检查纹理绑定操作是否有错误

    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // LOGI("loading framebuffer all info %d %d %d", new_framebuffer_id, depth_attachment, stencil_attachment);

    // return new_framebuffer_id;

}

void load_native_framebuffers(QEMUFile *f, GHashTable* resource_list) {

    //ztodo:恢复surface->data_fbo等等fbo

    // GHashTable* loaded_framebuffers = g_hash_table_new(g_direct_hash, g_direct_equal);

    // GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_FRAMEBUFFER];

    int framebuffer_num = qemu_get_be32(f);
    for (int i = 0; i < framebuffer_num; i++) {
        // uint64_t framebuffer_id = qemu_get_be64(f);
        // GLuint type = qemu_get_be32(f);
        // uint64_t texture_id = qemu_get_be64(f);
        Express_Native_Framebuffer *new_framebuffer = g_malloc0(sizeof(Express_Native_Framebuffer));

        new_framebuffer->framebufferId = qemu_get_be64(f);
        for(int i = 0; i < 16; i++) {
            new_framebuffer->attachment_target[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
            LOGI("loading framebuffer attachment %d %d", i, new_framebuffer->attachment_target[i]);
        }
        for(int i = 0; i < 19; i++) {
            new_framebuffer->renderbuffer_attachment[i] = get_host_id_map(RESOURCE_TYPE_RENDERBUFFER, qemu_get_be32(f));
            LOGI("loading framebuffer renderbuffer attachment %d %d", i, new_framebuffer->renderbuffer_attachment[i]);
        }
        // new_framebuffer->attachment_target = type;
        uint64_t texture_id = qemu_get_be64(f);
        new_framebuffer->texture_id = get_host_id_map(RESOURCE_TYPE_TEXTURE, texture_id);
        LOGI("loading framebuffer ID: %d, Texture ID: %d %d", new_framebuffer->framebufferId, texture_id, new_framebuffer->texture_id);
        g_hash_table_insert(resource_list, GUINT_TO_POINTER(new_framebuffer->framebufferId), new_framebuffer);
        // texture_id = get_host_id_map(RESOURCE_TYPE_TEXTURE, texture_id);
        // if(g_hash_table_lookup(loaded_framebuffers, GUINT_TO_POINTER(framebuffer_id)) == NULL) {
        //     GLint new_framebuffer_id;
        //     glDeleteFramebuffers(1, (GLuint*)&framebuffer_id);
        //     glGenFramebuffers(1, (GLuint*)&new_framebuffer_id);
        //     change_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, framebuffer_id, new_framebuffer_id);

            
        //     g_hash_table_insert(loaded_framebuffers, GUINT_TO_POINTER(framebuffer_id), GUINT_TO_POINTER(new_framebuffer_id));
            
        // }
        // GLint new_framebuffer_id = (GLint)g_hash_table_lookup(loaded_framebuffers, GUINT_TO_POINTER(framebuffer_id));
        // LOGI("loading framebuffer ID: old %d new %d", framebuffer_id, new_framebuffer_id);
        // if(type != 0) {
        //     restore_single_framebuffer(f, new_framebuffer_id, type, texture_id);    
        // }

        // GLint new_framebuffer_id = restore_single_framebuffer(f, framebuffer_id);
        // change_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, framebuffer_id, new_framebuffer_id);
    }

    // g_free(loaded_framebuffers);
}

void save_single_sampler(QEMUFile *f, GLint sampler_id) {
    LOGI("saving sampler ID: %d", sampler_id);
    qemu_put_be32(f, sampler_id);
    GLint param;
    GLint border_color[4];
    GLfloat paramf;
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_WRAP_S, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_WRAP_T, &param);
    qemu_put_be32(f, param);

    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_WRAP_R, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_MIN_FILTER, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_MAG_FILTER, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_COMPARE_MODE, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_COMPARE_FUNC, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameterfv(sampler_id, GL_TEXTURE_MAX_ANISOTROPY_EXT, &paramf);
    qemu_put_be32(f, *(GLuint *)&paramf);
    
    glGetSamplerParameterfv(sampler_id, GL_TEXTURE_MIN_LOD, &paramf);
    qemu_put_be32(f, *(GLuint *)&paramf);
    
    glGetSamplerParameterfv(sampler_id, GL_TEXTURE_MAX_LOD, &paramf);
    qemu_put_be32(f, *(GLuint *)&paramf);
    
    glGetSamplerParameterfv(sampler_id, GL_TEXTURE_LOD_BIAS, &paramf);
    qemu_put_be32(f, *(GLuint *)&paramf);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_BORDER_COLOR, border_color);
    qemu_put_be32(f, border_color[0]);
    qemu_put_be32(f, border_color[1]);
    qemu_put_be32(f, border_color[2]);
    qemu_put_be32(f, border_color[3]);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_SWIZZLE_R, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_SWIZZLE_G, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_SWIZZLE_B, &param);
    qemu_put_be32(f, param);
    
    glGetSamplerParameteriv(sampler_id, GL_TEXTURE_SWIZZLE_A, &param);
    qemu_put_be32(f, param);
}

void save_native_samplers(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_SAMPLER]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_SAMPLER];
    qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many samplers
    LOGI("saving sampler num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLint sampler_id = (GLint)value;
        save_single_sampler(f, sampler_id);
    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_SAMPLER]);
}

void save_single_renderbuffer(QEMUFile *f, GLint rb_id) {
    LOGI("saving renderbuffer ID: %d", rb_id);
    qemu_put_be32(f, rb_id);
    GLint width, height, format, samples;
    glBindRenderbuffer(GL_RENDERBUFFER, rb_id);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    qemu_put_be32(f, width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    qemu_put_be32(f, height);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format);
    qemu_put_be32(f, format);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
    qemu_put_be32(f, samples);
}

void save_native_renderbuffers(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_RENDERBUFFER]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_RENDERBUFFER];
    qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many samplers
    LOGI("saving renderbuffer num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLint rb_id = (GLint)value;
        save_single_renderbuffer(f, rb_id);
    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_RENDERBUFFER]);
}

void save_single_program_pipeline(QEMUFile *f, GLint pp_id) {
    LOGI("saving program_pipeline ID: %d", pp_id);
    qemu_put_be32(f, pp_id);

    GLint vertProgBound;
    glGetProgramPipelineiv(pp_id, GL_VERTEX_SHADER, &vertProgBound);
    qemu_put_be32(f, vertProgBound);
    GLint fragProgBound;
    glGetProgramPipelineiv(pp_id, GL_FRAGMENT_SHADER, &fragProgBound);
    qemu_put_be32(f, fragProgBound);
    GLint geomProgBound;
    glGetProgramPipelineiv(pp_id, GL_GEOMETRY_SHADER, &geomProgBound);
    qemu_put_be32(f, geomProgBound);
    GLint tessCtrlProgBound;
    glGetProgramPipelineiv(pp_id, GL_TESS_CONTROL_SHADER, &tessCtrlProgBound);
    qemu_put_be32(f, tessCtrlProgBound);
    GLint tessEvalProgBound;
    glGetProgramPipelineiv(pp_id, GL_TESS_EVALUATION_SHADER, &tessEvalProgBound);
    qemu_put_be32(f, tessEvalProgBound);
    GLint compProgBound;
    glGetProgramPipelineiv(pp_id, GL_COMPUTE_SHADER, &compProgBound);
    qemu_put_be32(f, compProgBound);

    int activeStagesMask = 0;
    
    glBindProgramPipeline(pp_id);
    GLint activeProg;
    glGetProgramPipelineiv(pp_id, GL_ACTIVE_PROGRAM, &activeProg);
    qemu_put_be32(f, activeProg);
}

void load_single_program_pipeline(QEMUFile *f, GLint old_pp_id) {
    GLint pp_id;
    glGenProgramPipelines(1, (GLuint*)&pp_id);
    change_host_id_map(RESOURCE_TYPE_PROGRAM_PIPELINE, old_pp_id, pp_id);

    GLint vertProgBound = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    glUseProgramStages(pp_id, GL_VERTEX_SHADER_BIT, vertProgBound);
    GLint fragProgBound = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    glUseProgramStages(pp_id, GL_FRAGMENT_SHADER_BIT, fragProgBound);
    GLint geomProgBound = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    glUseProgramStages(pp_id, GL_GEOMETRY_SHADER_BIT, geomProgBound);
    GLint tessCtrlProgBound = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    glUseProgramStages(pp_id, GL_TESS_CONTROL_SHADER_BIT, tessCtrlProgBound);
    GLint tessEvalProgBound = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    glUseProgramStages(pp_id, GL_TESS_EVALUATION_SHADER_BIT, tessEvalProgBound);
    GLint compProgBound = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    glUseProgramStages(pp_id, GL_COMPUTE_SHADER_BIT, compProgBound);

    glBindProgramPipeline(pp_id);

    GLint activeProg = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    glActiveShaderProgram(pp_id, activeProg);

    LOGI("loading program_pipeline ID: old %d new %d", old_pp_id, pp_id);
}

void save_native_program_pipelines(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_PROGRAM_PIPELINE]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_PROGRAM_PIPELINE];
    qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many samplers
    LOGI("saving program_pipeline num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLint pp_id = (GLint)value;
        save_single_program_pipeline(f, pp_id);
    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_PROGRAM_PIPELINE]);
}

void load_native_program_pipelines(QEMUFile *f) {
    int pp_num = qemu_get_be32(f);
    for(int i = 0; i < pp_num ; i++) {
        GLint old_pp_id = qemu_get_be32(f);
        load_single_program_pipeline(f, old_pp_id);
    }
}

void load_single_sampler(QEMUFile *f, GLint old_sampler_id) {


    GLint param;
    GLint border_color[4];
    GLuint sampler_id;
    glGenSamplers(1, &sampler_id);

    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_SAMPLER];
    g_hash_table_insert(resource_list, GUINT_TO_POINTER(sampler_id), GUINT_TO_POINTER(sampler_id));

    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_R, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_COMPARE_MODE, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_COMPARE_FUNC, param);
    
    param = qemu_get_be32(f);
    glSamplerParameterf(sampler_id, GL_TEXTURE_MAX_ANISOTROPY_EXT, *(GLfloat *)&param);
    
    param = qemu_get_be32(f);
    glSamplerParameterf(sampler_id, GL_TEXTURE_MIN_LOD, *(GLfloat *)&param);
    
    param = qemu_get_be32(f);
    glSamplerParameterf(sampler_id, GL_TEXTURE_MAX_LOD, *(GLfloat *)&param);
    
    param = qemu_get_be32(f);
    glSamplerParameterf(sampler_id, GL_TEXTURE_LOD_BIAS, *(GLfloat *)&param);
    
    border_color[0] = qemu_get_be32(f);
    border_color[1] = qemu_get_be32(f);
    border_color[2] = qemu_get_be32(f);
    border_color[3] = qemu_get_be32(f);
    glSamplerParameteriv(sampler_id, GL_TEXTURE_BORDER_COLOR, border_color);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_SWIZZLE_R, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_SWIZZLE_G, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_SWIZZLE_B, param);
    
    param = qemu_get_be32(f);
    glSamplerParameteri(sampler_id, GL_TEXTURE_SWIZZLE_A, param);


    change_host_id_map(RESOURCE_TYPE_SAMPLER, old_sampler_id, sampler_id);
    LOGI("loading sampler ID: old %d new %d", old_sampler_id, sampler_id);
}

void load_native_samplers(QEMUFile *f) {
    int sampler_num = qemu_get_be32(f);
    for(int i = 0; i < sampler_num ; i++) {
        GLint old_sampler_id = qemu_get_be32(f);
        load_single_sampler(f, old_sampler_id);
    }
}

void load_single_renderbuffer(QEMUFile *f, GLint old_rb_id) {
    GLint rb_id;
    glGenRenderbuffers(1, (GLuint*)&rb_id);

    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_RENDERBUFFER];
    g_hash_table_insert(resource_list, GUINT_TO_POINTER(rb_id), GUINT_TO_POINTER(rb_id));

    GLint width, height, format, samples;
    width = qemu_get_be32(f);
    height = qemu_get_be32(f);
    format = qemu_get_be32(f);
    samples = qemu_get_be32(f);
    glBindRenderbuffer(GL_RENDERBUFFER, rb_id);
    if (samples > 0) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    }

    change_host_id_map(RESOURCE_TYPE_RENDERBUFFER, old_rb_id, rb_id);
    LOGI("loading renderbuffer ID: old %d new %d width %d height %d format %x samples %d", old_rb_id, rb_id, width, height, format, samples);
}

void load_native_renderbuffers(QEMUFile *f) {
    int rb_num = qemu_get_be32(f);
    for(int i = 0; i < rb_num ; i++) {
        GLint old_rb_id = qemu_get_be32(f);
        load_single_renderbuffer(f, old_rb_id);
    }
}

void save_native_resources(QEMUFile *f){
    save_native_shaders(f);
    save_native_programs(f);
    save_native_textures(f);

    save_native_buffers(f);

    save_native_samplers(f);

    save_native_renderbuffers(f);

    save_native_program_pipelines(f);
}

void load_native_resources(QEMUFile *f){
    load_native_shaders(f);
    load_native_programs(f);
    load_native_textures(f);

    load_native_buffers(f);
    load_native_samplers(f);
    load_native_renderbuffers(f);
    load_native_program_pipelines(f);
}

void save_virtqueue_element(QEMUFile *f, VirtQueueElement *elem) {
    qemu_put_be32(f, elem->index);
    qemu_put_be32(f, elem->len);
    qemu_put_be32(f, elem->ndescs);
    qemu_put_be32(f, elem->out_num);
    qemu_put_be32(f, elem->in_num);
    qemu_put_be64(f, (uint64_t)elem->in_addr);
    qemu_put_be64(f, (uint64_t)elem->out_addr);

    if(elem->in_num != 0){
        uint64_t in_address = ((uint64_t)elem->in_sg->iov_base);
        void* in_real_guest_mem = (void *)qemu_ram_addr_from_host((void*)in_address);
        qemu_put_be64(f, (uint64_t)in_real_guest_mem);
        qemu_put_be32(f, elem->in_sg->iov_len);        
    } else {
        qemu_put_be64(f, (uint64_t)elem->in_sg->iov_base);
        qemu_put_be32(f, elem->in_sg->iov_len);  
    }

    if(elem->out_num != 0){
        uint64_t out_address = ((uint64_t)elem->out_sg->iov_base);
        void* out_real_guest_mem = (void *)qemu_ram_addr_from_host((void*)out_address);
        qemu_put_be64(f, (uint64_t)out_real_guest_mem);
        qemu_put_be32(f, elem->out_sg->iov_len);        
    } else {
        qemu_put_be64(f, (uint64_t)elem->out_sg->iov_base);
        qemu_put_be32(f, elem->out_sg->iov_len);  
    }

    // uint64_t out_address = ((uint64_t)elem->out_sg->iov_base);
    // void* out_real_guest_mem = (void *)qemu_ram_addr_from_host((void*)out_address);
    // qemu_put_be64(f, (uint64_t)out_real_guest_mem);
    // qemu_put_be32(f, elem->out_sg->iov_len);

    // LOGI("in address is %llu out address is %llu in num %d out num %d", (uint64_t)in_address, (uint64_t)out_address, elem->in_num, elem->out_num);

    // LOGI("saving virtqueue element of in_address %llu out_address %llu in addr %llu out addr %llu", (uint64_t)in_real_guest_mem, (uint64_t)out_real_guest_mem, (uint64_t)elem->in_addr, (uint64_t)elem->out_addr);

}

void load_virtqueue_element(QEMUFile *f, VirtQueueElement *elem) {
    elem->index = qemu_get_be32(f);
    elem->len = qemu_get_be32(f);
    elem->ndescs = qemu_get_be32(f);
    elem->out_num = qemu_get_be32(f);
    elem->in_num = qemu_get_be32(f);
    elem->in_addr = (void *)qemu_get_be64(f);
    elem->out_addr = (void *)qemu_get_be64(f);

    elem->in_sg = g_malloc0(sizeof(struct iovec));
    elem->out_sg = g_malloc0(sizeof(struct iovec));

    // elem->in_sg->iov_base = (void *)qemu_get_be64(f);
    if(elem->in_num != 0) {
        uint64_t in_address = qemu_get_be64(f);
        elem->in_sg->iov_len = qemu_get_be32(f);
        hwaddr len = elem->in_sg->iov_len;
        hwaddr xlat;
        MemoryRegion *mr = address_space_translate(&address_space_memory,
            (hwaddr)in_address,
            &xlat, &len, false,
            MEMTXATTRS_UNSPECIFIED);
        void *in_hva = NULL;
        if (mr) {
            in_hva = qemu_map_ram_ptr(mr->ram_block, xlat);
        }
        elem->in_sg->iov_base = in_hva;        
    } else {
        elem->in_sg->iov_base = (void *)qemu_get_be64(f);
        elem->in_sg->iov_len = qemu_get_be32(f);
    }

    // elem->out_sg->iov_base = (void *)qemu_get_be64(f);

    if(elem->out_num != 0) {
        uint64_t out_address = qemu_get_be64(f);
        elem->out_sg->iov_len = qemu_get_be32(f);
        hwaddr len = elem->out_sg->iov_len;
        hwaddr xlat;
        MemoryRegion *mr = address_space_translate(&address_space_memory,
            (hwaddr)out_address,
            &xlat, &len, false,
            MEMTXATTRS_UNSPECIFIED);
        void *out_hva = NULL;
        if (mr) {
            out_hva = qemu_map_ram_ptr(mr->ram_block, xlat);
        }
        elem->out_sg->iov_base = out_hva;        
    } else {
        elem->out_sg->iov_base = (void *)qemu_get_be64(f);
        elem->out_sg->iov_len = qemu_get_be32(f);
    }


    // LOGI("finish loading virtqueue element of in_address %p out_address %p", in_hva, out_hva);
}

void save_teleport_express_queue_elem(QEMUFile *f, Teleport_Express_Queue_Elem *elem) {
    LOGI("in save_teleport_express_queue_elem");
    save_virtqueue_element(f, &(elem->elem));
    qemu_put_be64(f, (int64_t)elem->para);
    if(elem->para != NULL) {
        save_guest_mem(f, (Guest_Mem *)elem->para);
    }
    // save_guest_mem(f, (Guest_Mem *)elem->para);
    qemu_put_be64(f, elem->len);

}

void load_teleport_express_queue_elem(QEMUFile *f, Teleport_Express_Queue_Elem *elem) {
    LOGI("in load_teleport_express_queue_elem");
    VirtQueueElement *vq_elem = g_malloc0(sizeof(VirtQueueElement));
    load_virtqueue_element(f, vq_elem);
    elem->elem = *vq_elem;
    elem->para = (void *)qemu_get_be64(f);
    if(elem->para != NULL) {
        elem->para = load_guest_mem(f, 1);
    }
    // elem->para = vq_elem;
    elem->len = qemu_get_be64(f);
    elem->next = NULL;
}



void save_teleport_express_call(QEMUFile *f, Teleport_Express_Call *call) {
    qemu_put_be64(f, call->id);
    qemu_put_be64(f, call->thread_id);
    qemu_put_be64(f, call->process_id);
    qemu_put_be64(f, call->unique_id);
    qemu_put_be64(f, call->spend_time);
    qemu_put_be64(f, call->para_num);

    save_teleport_express_queue_elem(f, call->elem_header);
    save_teleport_express_queue_elem(f, call->elem_tail);

    if(call->vq == startup_in_data_queue) {
        qemu_put_be32(f, 1);
        LOGI("call type is in data queue")
    } else if (call->vq == startup_out_data_queue) {
        qemu_put_be32(f, 2);
    } else {
        qemu_put_be32(f, 0);
        LOGE("error! call->vq is not startup_in_data_queue or startup_out_data_queue");
    }

    qemu_put_be32(f, call->is_end);

}

Teleport_Express_Call* load_teleport_express_call(QEMUFile *f) {
    // Teleport_Express_Call *call = g_malloc0(sizeof(Teleport_Express_Call));
    Teleport_Express_Call *call = alloc_one_call();

    call->id = qemu_get_be64(f);
    call->thread_id = qemu_get_be64(f);
    call->process_id = qemu_get_be64(f);
    call->unique_id = qemu_get_be64(f);
    call->spend_time = qemu_get_be64(f);
    call->para_num = qemu_get_be64(f);

    call->elem_header = g_malloc0(sizeof(Teleport_Express_Queue_Elem));
    load_teleport_express_queue_elem(f, call->elem_header);

    call->elem_tail = g_malloc0(sizeof(Teleport_Express_Queue_Elem));
    load_teleport_express_queue_elem(f, call->elem_tail);


    call->vdev = startup_vdev;

    int call_type = qemu_get_be32(f);
    if(call_type == 1) {
        call->vq = startup_in_data_queue;
        call->callback = get_input_call_release_ptr();
        LOGI("call type is in data queue");
    } else if (call_type == 2) {
        call->vq = startup_out_data_queue;
        call->callback = get_push_free_callback_ptr();
        LOGI("call type is out data queue");
    } else {
        LOGE("error! call_type is not 1 or 2");
    }

    call->is_end = qemu_get_be32(f);

    return call;
}

void save_native_shaders(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_SHADER]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_SHADER];
    qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many shaders
    LOGI("saving shader num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_Shader* shader = (Express_Native_Shader *)value;
        LOGI("saving shader ID: %d, Type: %d, Delete Status: %d", shader->id, shader->type, shader->deleteStatus);
        qemu_put_be64(f, shader->id);
        qemu_put_be32(f, shader->type);
        qemu_put_byte(f, shader->deleteStatus);
        GLint compile_status;
        glGetShaderiv(shader->id, GL_COMPILE_STATUS, &compile_status);

        qemu_put_byte(f, compile_status); //ztodo:byte可以吗
        GLint sourceLength;
        glGetShaderiv(shader->id, GL_SHADER_SOURCE_LENGTH, &sourceLength);

        qemu_put_be32(f, sourceLength);

        char* shader_source;
        if (sourceLength > 0) {
            shader_source = (char*)g_malloc0(sourceLength);
            if (shader_source) {
                glGetShaderSource(shader->id, sourceLength, NULL, shader_source);
                LOGD("saving shader of content length %d %d %s", sourceLength, sizeof(shader_source), shader_source);
                qemu_put_buffer(f, shader_source, sourceLength);
            }
            g_free(shader_source); 
        } //ztodo:如果是0，load之前判断一下
    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_SHADER]);
}


void load_native_programs_tmp(QEMUFile *f) {
    int program_num = qemu_get_be32(f);
    for(int i = 0; i < program_num ; i++) {
        uint64_t program_id = qemu_get_be32(f);

        GLint new_program_id = program_id;
        change_host_id_map(RESOURCE_TYPE_PROGRAM, program_id, new_program_id); //重新映射guest-host的id
    }
}

void load_program_uniform_and_attrib_info(QEMUFile* f, GLuint program) {
    // int name_len = qemu_get_be32(f);
    // GLint uniform_num = qemu_get_be32(f);
    // LOGI("get uniform num %d attrib num %d", uniform_num, name_len);
    
    // for(int i = 0; i < uniform_num; i++) {
    //     int bufsize = qemu_get_be32(f);
    //     GLchar *name_buf = g_malloc(bufsize + 1);
    //     memset(name_buf, 0, bufsize + 1);

    //     qemu_get_buffer(f, name_buf, bufsize);
    //     GLint location = qemu_get_be32(f);
    //     // glBindUniformLocation(program, location, name_buf);

    //     LOGI("in load program set uniform index %d name %s", location, name_buf);

    // }

    GLint attrib_num = qemu_get_be32(f);
    for(int i = 0; i < attrib_num; i++) {
        int bufsize = qemu_get_be32(f);
        GLchar *name_buf = g_malloc(bufsize + 1);
        memset(name_buf, 0, bufsize + 1);
        qemu_get_buffer(f, name_buf, bufsize);
        GLint location = qemu_get_be32(f);
        glBindAttribLocation(program, location, name_buf);

        LOGI("in load program set attrib index %d name %s", location, name_buf);

    }

        // glBindAttribLocation(new_program_id, 0, "position");
        // glBindAttribLocation(new_program_id, 1, "texCoords");
}


void load_native_programs(QEMUFile *f){  //ztodo:应该先重新创建program、更新id！

    // GHashTable* programs = g_hash_table_new(g_direct_hash, g_direct_equal);

    int program_num = qemu_get_be32(f);
    for(int i = 0; i < program_num ; i++) {
        GLuint program_id = qemu_get_be32(f);
        GLint new_program_id = 0;
        // while(new_program_id != program_id){
            new_program_id = glCreateProgram();
        //     LOGI("create program of old %d new %d", program_id, new_program_id);

        // }
        // if(program_id == 82){
        // if(program_id == 82 || program_id == 57|| program_id == 100|| program_id == 118|| program_id == 121 || program_id == 124) {
        //     change_host_id_map(RESOURCE_TYPE_PROGRAM, program_id, program_id);
        // }
        // else    
        change_host_id_map(RESOURCE_TYPE_PROGRAM, program_id, new_program_id); //重新映射guest-host的id       
        
        Express_Native_Program *current_program = g_malloc0(sizeof(Express_Native_Program));
        current_program = g_malloc0(sizeof(Express_Native_Program));
        current_program->shader_map = g_hash_table_new(g_direct_hash, g_direct_equal);
        g_hash_table_insert(g_resource_list[RESOURCE_TYPE_PROGRAM], GUINT_TO_POINTER(new_program_id), GUINT_TO_POINTER(current_program));
        GHashTable *shader_map = current_program->shader_map;

        int shader_num = qemu_get_be32(f);
        LOGI("program %d has shader num %d", new_program_id, shader_num);
        current_program->shader_num = shader_num;

        int shader_attached_order[10] = {0};

        for (int j = 0; j < shader_num; j++) {
            GLint shader_type = qemu_get_be32(f);
            GLint shader_source_length = qemu_get_be32(f);
            GLint attached_order = qemu_get_be32(f);
            char* shader_source = g_malloc0(shader_source_length);
            if (shader_source) {
                qemu_get_buffer(f, shader_source, shader_source_length);
                GLuint shader_id = glCreateShader(shader_type);
                glShaderSource(shader_id, 1, (const char **)&shader_source, NULL);
                // glCompileShader(shader_id);
                shader_attached_order[attached_order] = shader_id;
                // glAttachShader(new_program_id, shader_id);
                LOGI("loading program of old %d %d shader %d %d %s",program_id, new_program_id, shader_id, shader_type, shader_source);
                // glDeleteShader(shader_id);
                // g_free(shader_source);

                Express_Native_Program_Shader *current_shader = g_malloc0(sizeof(Express_Native_Program_Shader));
                current_shader->shader_id = shader_id;
                current_shader->attached_order = j;
                current_shader->shader_type = shader_type;
                current_shader->shader_source_length = shader_source_length;
                current_shader->shader_source = shader_source;
                g_hash_table_insert(shader_map, GUINT_TO_POINTER(shader_id), current_shader);
            }
        }
        
        for(int j = 0; j < shader_num; j++) {
            glCompileShader(shader_attached_order[j]);
            glAttachShader(new_program_id, shader_attached_order[j]);
            LOGI("link program of %d shader %d", new_program_id, shader_attached_order[j]);
            GLenum glerror = glGetError();
            if (glerror != GL_NO_ERROR) {
                LOGE("error! in loading program glAttachShader failed! gl error %x ", glerror);
            }
            glDeleteShader(shader_attached_order[j]);

        }

        // glBindAttribLocation(new_program_id, 0, "position");
        // glBindAttribLocation(new_program_id, 1, "texCoords");

        load_program_uniform_and_attrib_info(f, new_program_id);

        glLinkProgram(new_program_id);
        glUseProgram(new_program_id);

        GLint uniform_num = qemu_get_be32(f);
        GLint max_uniform_name_len = 0;
        glGetProgramiv(new_program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_len);
        int name_len = max_uniform_name_len;
        // GLchar *name_buf = g_malloc(name_len);
        GLint size;
        GLenum type;
        GLint location;
        // glGetProgramiv(new_program_id, GL_ACTIVE_UNIFORMS, &uniform_num);
        for (int i = 0; i < uniform_num; i++) {
            // glGetActiveUniform(new_program_id, i, name_len, NULL, &size, &type, name_buf);
            int bufsize = qemu_get_be32(f);
            GLchar *name_buf = g_malloc(bufsize + 1);
            memset(name_buf, 0, bufsize + 1);

            qemu_get_buffer(f, name_buf, bufsize);
            GLint old_loc = qemu_get_be32(f);
            GLint old_type = qemu_get_be32(f);
            location = glGetUniformLocation(new_program_id, name_buf);
            if(old_loc != location) {
                LOGE("error! in loading program uniform %d %d", old_loc, location);
            }

            // GLuint textureUnit;
            // glGetUniformiv(program_id, location, &textureUnit);
            LOGI("going to recover uniform value %d %d %d %d %d %s", new_program_id, location, old_loc, old_type, bufsize, name_buf);
            switch (old_type) {
                case GL_FLOAT:
                    {
                        GLfloat value;
                        uint32_t raw_value = qemu_get_be32(f);
                        value = *(GLfloat*)&raw_value;
                    }
                    break;

                case GL_INT:
                    {
                        GLint value = (GLint)qemu_get_be64(f);
                        glUniform1i(location, value);
                        LOGI("before loading get program uniform %d name %s index %d size %d value %d", program_id, name_buf, location, strlen(name_buf), value);
                    }
                    break;

                case GL_BOOL:
                    {
                        GLint value = (GLint)qemu_get_be64(f);  // GL_BOOL 是 GLint 类型
                        glUniform1i(location, value);
                    }
                    break;

                case GL_FLOAT_VEC2:
                    {
                        GLfloat value[2];
                        for (int i = 0; i < 2; ++i) {
                            uint32_t raw_value = qemu_get_be32(f);
                            value[i] = *(GLfloat*)&raw_value;
                        }
                        glUniform2fv(location, 1, value);
                    }
                    break;

                case GL_FLOAT_VEC3:
                    {
                        GLfloat value[3];
                        for (int i = 0; i < 3; ++i) {
                            uint32_t raw_value = qemu_get_be32(f);
                            value[i] = *(GLfloat*)&raw_value;
                        }
                        glUniform3fv(location, 1, value);
                    }
                    break;

                case GL_FLOAT_VEC4:
                    {
                        GLfloat value[4];
                        for (int i = 0; i < 4; ++i) {
                            // value[i] = (GLfloat)qemu_get_be64(f);
                            uint32_t raw_value = qemu_get_be32(f);
                            value[i] = *(GLfloat*)&raw_value;
                        }
                        LOGI("GL_FLOAT_VEC4 loading get program uniform %d name %s index %d size %d value %f %f %f %f", program_id, name_buf, location, strlen(name_buf), value[0], value[1], value[2], value[3]);
                        glUniform4fv(location, 1, value);
                    }
                    break;

                case GL_INT_VEC2:
                    {
                        GLint value[2];
                        for (int i = 0; i < 2; ++i) {
                            value[i] = (GLint)qemu_get_be64(f);
                        }
                        glUniform2iv(location, 1, value);
                    }
                    break;

                case GL_INT_VEC3:
                    {
                        GLint value[3];
                        for (int i = 0; i < 3; ++i) {
                            value[i] = (GLint)qemu_get_be64(f);
                        }
                        glUniform3iv(location, 1, value);
                    }
                    break;

                case GL_INT_VEC4:
                    {
                        GLint value[4];
                        for (int i = 0; i < 4; ++i) {
                            value[i] = (GLint)qemu_get_be64(f);
                        }
                        glUniform4iv(location, 1, value);
                    }
                    break;

                case GL_FLOAT_MAT2:
                    {
                        GLfloat value[4];  // 2x2 matrix (4 elements)
                        for (int i = 0; i < 4; ++i) {
                            uint32_t raw_value = qemu_get_be32(f);
                            value[i] = *(GLfloat*)&raw_value;
                        }
                        glUniformMatrix2fv(location, 1, GL_FALSE, value);
                    }
                    break;

                case GL_FLOAT_MAT3:
                    {
                        GLfloat value[9];  // 3x3 matrix (9 elements)
                        for (int i = 0; i < 9; ++i) {
                            uint32_t raw_value = qemu_get_be32(f);
                            value[i] = *(GLfloat*)&raw_value;
                        }
                        glUniformMatrix3fv(location, 1, GL_FALSE, value);
                    }
                    break;

                case GL_FLOAT_MAT4:
                    {
                        GLfloat value[16];  // 4x4 matrix (16 elements)
                        for (int i = 0; i < 16; ++i) {
                            uint32_t raw_value = qemu_get_be32(f);
                            value[i] = *(GLfloat*)&raw_value;
                        }
                        glUniformMatrix4fv(location, 1, GL_FALSE, value);
                    }
                    break;

                case GL_SAMPLER_1D:
                case GL_SAMPLER_2D:
                case GL_SAMPLER_3D:
                case GL_SAMPLER_CUBE:
                case GL_SAMPLER_1D_SHADOW:
                case GL_SAMPLER_2D_SHADOW:
                    {
                        GLint value = (GLint)qemu_get_be64(f);
                        glUniform1i(location, value);
                        LOGI("samplers before loading get program uniform %d name %s index %d size %d value %d", program_id, name_buf, location, strlen(name_buf), value);
                    }
                    break;

                default:
                    LOGI("uniform type %x not supported", old_type);
                    break;
            }


            LOGI("loading get program uniform %d name %s index %d size %d type %x", program_id, name_buf, location, strlen(name_buf), old_type);

        }


        GLenum error = glGetError();
        if(error != GL_NO_ERROR) {
            LOGE("error! glLinkProgram failed! gl error %x ", error);
        }
        LOGI("in load native programs loading program of %d %d", program_id, new_program_id);
    }


    if (program_is_external_map == NULL)
    {
        program_is_external_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
    }
    g_hash_table_remove_all(program_is_external_map);
    int external_program_num = qemu_get_be32(f);
    for (int i = 0; i < external_program_num; i++) {
        GLuint external_program = qemu_get_be32(f);
        GLuint new_program_id = get_host_id_map(RESOURCE_TYPE_PROGRAM, external_program);
        g_hash_table_insert(program_is_external_map, GUINT_TO_POINTER(new_program_id), GUINT_TO_POINTER(1));
        LOGI("loading external program of id %d old %d", new_program_id, external_program);
    }

}

void save_native_programs_tmp(QEMUFile *f) {
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_PROGRAM];
    GHashTableIter iter;
    gpointer key, value;
    
    // GHashTable* programs = g_hash_table_new(g_direct_hash, g_direct_equal);
    g_hash_table_iter_init(&iter, resource_list);
    qemu_put_be32(f, g_hash_table_size(resource_list));
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLuint program = (GLuint)program;
        qemu_put_be32(f, program);

    }

}

void save_program_uniform_and_attrib_info(QEMUFile* f, GLuint program) {
    GLint uniform_num = 0;
    GLint attrib_num = 0;
    GLint uniform_blocks_num = 0;


    GLint max_uniform_name_len = 0;
    GLint max_attrib_name_len = 0;
    GLint max_uniform_block_name_len = 0;

    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_len);
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_name_len);
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_uniform_block_name_len);

    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_num);
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrib_num);

    int name_len = max_uniform_name_len > max_attrib_name_len ? max_uniform_name_len : max_attrib_name_len;
    name_len = name_len > max_uniform_block_name_len ? name_len : max_uniform_block_name_len;
    name_len += 1;

    // qemu_put_be32(f, name_len);

    GLchar *name_buf = g_malloc(name_len);
    GLint size;
    GLenum type;
    GLint location;



    qemu_put_be32(f, attrib_num);
    for (int i = 0; i < attrib_num; i++){
        glGetActiveAttrib(program, i, name_len, NULL, &size, &type, name_buf);
        location = glGetAttribLocation(program, name_buf);
        LOGI("saving get program attrib %d name %s index %d size %d", program, name_buf, location, strlen(name_buf));

        qemu_put_be32(f, strlen(name_buf));
        qemu_put_buffer(f, name_buf, strlen(name_buf));
        qemu_put_be32(f, location);
        
    }

    qemu_put_be32(f, uniform_num);
    for (int i = 0; i < uniform_num; i++) {
        glGetActiveUniform(program, i, name_len, NULL, &size, &type, name_buf);
        location = glGetUniformLocation(program, name_buf);
        LOGI("saving get program uniform %d name %s index %d size %d", program, name_buf, location, strlen(name_buf));
        
        qemu_put_be32(f, strlen(name_buf));
        qemu_put_buffer(f, name_buf, strlen(name_buf));
        qemu_put_be32(f, location);
        qemu_put_be32(f, type);

        switch (type) {
            case GL_FLOAT:
                {
                    GLfloat value;
                    glGetUniformfv(program, location, &value);
                    qemu_put_be32(f, *(uint32_t*)&value);
                }
                break;

            case GL_INT:
                {
                    GLint value;
                    glGetUniformiv(program, location, &value);
                    qemu_put_be64(f, value);

                }
                break;

            case GL_BOOL:
                {
                    GLint value;
                    glGetUniformiv(program, location, &value);
                    qemu_put_be64(f, value);

                }
                break;

            case GL_FLOAT_VEC2:
                {
                    GLfloat value[2];
                    glGetUniformfv(program, location, value);
                    for(int i = 0; i < 2; i++) {
                        qemu_put_be32(f, *(uint32_t*)&value[i]);
                    }
                }
                break;

            case GL_FLOAT_VEC3:
                {
                    GLfloat value[3];
                    glGetUniformfv(program, location, value);
                    for(int i = 0; i < 3; i++) {
                        qemu_put_be32(f, *(uint32_t*)&value[i]);
                    }
                }
                break;

            case GL_FLOAT_VEC4:
                {
                    GLfloat value[4];
                    glGetUniformfv(program, location, value);
                    for(int i = 0; i < 4; i++) {
                        qemu_put_be32(f, *(uint32_t*)&value[i]);
                        LOGI("saving and get program uniform %d name %s index %d size %d value %f", program, name_buf, location, strlen(name_buf), value[i]);
                    }
                }
                break;

            case GL_INT_VEC2:
                {
                    GLint value[2];
                    glGetUniformiv(program, location, value);
                    for(int i = 0; i < 2; i++) {
                        qemu_put_be64(f, value[i]);
                    }
                }
                break;

            case GL_INT_VEC3:
                {
                    GLint value[3];
                    glGetUniformiv(program, location, value);
                    for(int i = 0; i < 3; i++) {
                        qemu_put_be64(f, value[i]);
                    }
                }
                break;

            case GL_INT_VEC4:
                {
                    GLint value[4];
                    glGetUniformiv(program, location, value);
                    for(int i = 0; i < 4; i++) {
                        qemu_put_be64(f, value[i]);
                    }
                }
                break;

            case GL_FLOAT_MAT2:
                {
                    GLfloat value[4];  // 2x2 matrix
                    glGetUniformfv(program, location, value);
                    for(int i = 0; i < 4; i++) {
                        qemu_put_be32(f, *(uint32_t*)&value[i]);
                    }
                }
                break;

            case GL_FLOAT_MAT3:
                {
                    GLfloat value[9];  // 3x3 matrix
                    glGetUniformfv(program, location, value);
                    for(int i = 0; i < 9; i++) {
                        qemu_put_be32(f, *(uint32_t*)&value[i]);
                    }
                }
                break;

            case GL_FLOAT_MAT4:
                {
                    GLfloat value[16];  // 4x4 matrix
                    glGetUniformfv(program, location, value);
                    for(int i = 0; i < 16; i++) {
                        qemu_put_be32(f, *(uint32_t*)&value[i]);
                    }
                }
                break;

            case GL_SAMPLER_1D:
            case GL_SAMPLER_2D:
            case GL_SAMPLER_3D:
            case GL_SAMPLER_CUBE:
            case GL_SAMPLER_1D_SHADOW:
            case GL_SAMPLER_2D_SHADOW:
                {
                    GLint value;
                    glGetUniformiv(program, location, &value);
                    qemu_put_be64(f, value);  // Sampler types are typically handled as integers
                }
                break;

            default:
                break;
        }

    }    

//ztodo:uniform block的保存加载
    // int uniform_block_active_uniforms;
    // for (int i = 0; i < uniform_blocks_num; i++)
    // {
    //     int_ptr = (int *)temp_ptr;
    //     glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniform_block_active_uniforms);
    //     glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
    //     glGetActiveUniformBlockName(program, i, name_len, NULL, name_buf);
    // }

}

void save_native_programs(QEMUFile *f) {
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_PROGRAM]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_PROGRAM];
    // qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many shaders
    // LOGI("saving program num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    
    // GHashTable* programs = g_hash_table_new(g_direct_hash, g_direct_equal);
    qemu_put_be32(f, g_hash_table_size(resource_list));
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLuint program_id = (GLuint)key;
        qemu_put_be32(f, program_id);

        Express_Native_Program* program = (Express_Native_Program *)value;
        GHashTableIter shader_iter;
        gpointer shader_key, shader_value;
        GHashTable* shaders = program->shader_map;

        qemu_put_be32(f, g_hash_table_size(shaders));

        g_hash_table_iter_init(&shader_iter, shaders);
        while (g_hash_table_iter_next(&shader_iter, &shader_key, &shader_value)) {
            GLuint shader_id = (GLuint)shader_key;
            Express_Native_Program_Shader *shader = (Express_Native_Program_Shader *)shader_value;
            qemu_put_be32(f, shader->shader_type);
            qemu_put_be32(f, shader->shader_source_length);
            qemu_put_be32(f, shader->attached_order);
            qemu_put_buffer(f, shader->shader_source, shader->shader_source_length);
            //ztodo:free这些资源
        }

        //存储program每个attrib和uniform的index
        save_program_uniform_and_attrib_info(f, program_id);

    //     uint64_t linked_program = (uint64_t)key;
    //     uint64_t program_id = (linked_program >> 32) & ((1 << 32) - 1); //取高32位
    //     // LOGI("saving program ID:%lld %d, shader: %d",linked_program, (linked_program >> 32) & ((1 << 32) - 1), linked_program & ((1 << 32) - 1));
    //     // qemu_put_be64(f, linked_program);

    //     GLint linkStatus = 0;
    //     glGetProgramiv(program_id, GL_LINK_STATUS, &linkStatus);

    //     // g_hash_table_insert(programs, GUINT_TO_POINTER(program_id), GUINT_TO_POINTER(linkStatus));

    }

    // g_hash_table_iter_init(&iter, programs); //先单独存program，因为要重建
    // qemu_put_be32(f, g_hash_table_size(programs));
    // while (g_hash_table_iter_next(&iter, &key, &value)) {
    //     uint64_t program_id = (uint64_t)key;
    //     GLint status = (GLint)value;
    //     qemu_put_be64(f, program_id);
    //     qemu_put_byte(f, status);
    //     LOGD("saving program of id %lld status %d", program_id, status);
    // }

    // qemu_put_be32(f, g_hash_table_size(resource_list)); //first save how many shaders
    // LOGD("saving program num %d", g_hash_table_size(resource_list));
    // g_hash_table_iter_init(&iter, resource_list);
    // while (g_hash_table_iter_next(&iter, &key, &value)) {
    //     uint64_t linked_program = (uint64_t)key;
    //     qemu_put_be64(f, linked_program);

    // }

    qemu_put_be32(f, g_hash_table_size(program_is_external_map));
    g_hash_table_iter_init(&iter, program_is_external_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GLuint external_program = (GLuint)key;
        qemu_put_be32(f, external_program);
    }

    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_PROGRAM]);
}

void update_native_shader(GLint shader_id, GLenum shader_type, GLboolean deleted_status, GLboolean compile_status, GLint source_length, const char* shader_source) {

    // glDeleteShader(shader_id);


    GLint new_shader_id = glCreateShader(shader_type);
    LOGD("create new shader of new id %d old id %d %s", new_shader_id, shader_id, shader_source);
    glShaderSource(new_shader_id, 1, &shader_source, NULL);
    LOGD("going to compile shader!");
    if(compile_status) {
        
        glCompileShader(new_shader_id);

        GLint compileStatus;
        glGetShaderiv(new_shader_id, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(new_shader_id, GL_INFO_LOG_LENGTH, &logLength);
            char* log = (char*)malloc(logLength);
            glGetShaderInfoLog(new_shader_id, logLength, &logLength, log);
            LOGE("Shader compile failed in saving snapshot: %s %s", log, shader_source);
            free(log);
        }        
    }
    LOGD("change when load shader from %d to %d", shader_id, new_shader_id);
    change_host_id_map(RESOURCE_TYPE_SHADER, shader_id, new_shader_id); //ztodo: 重新映射guest-host的id

    Express_Native_Shader* newShader = g_malloc0(sizeof(Express_Native_Shader));
    newShader->id = new_shader_id;
    newShader->type = shader_type;
    newShader->deleteStatus = GL_FALSE;
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_SHADER]);
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_SHADER];
    g_hash_table_insert(resource_list, GUINT_TO_POINTER(new_shader_id), newShader);
    LOGI("created shader! host shader id %d type %d all %d", new_shader_id, shader_type, g_hash_table_size(resource_list));
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_SHADER]);
}

void load_native_shaders_tmp(QEMUFile *f) {
    // 没有办法在保留原来id的情况下改属性的值，这样的话就得新建然后重新映射host id
    int shader_num = qemu_get_be32(f);   
    LOGD("in load native shaders! num %d", shader_num);

    for (int i = 0; i < shader_num; i++) {
        GLint shader_id = qemu_get_be64(f);
        GLenum shader_type = qemu_get_be32(f);
        GLboolean deleted_status = qemu_get_byte(f);
        GLboolean compile_status = qemu_get_byte(f);
        GLint source_length = qemu_get_be32(f);
        const char* shader_source = (char*)malloc(source_length);
        qemu_get_buffer(f, shader_source, source_length);
        LOGI("loading shader of id %d type %d content %s", shader_id, shader_type, shader_source);
        // bool isSame = compare_shader(shader_id, shader_type, deleted_status, compile_status, source_length, shader_source);
        // if(isSame) { //如果没变就不操作了 update:不再复用资源，全部重建
        //     continue;
        // }
        // update_native_shader(shader_id, shader_type, deleted_status, compile_status, source_length, shader_source);
        change_host_id_map(RESOURCE_TYPE_SHADER, shader_id, shader_id);
    }
}

void load_native_shaders(QEMUFile *f) {
    // 没有办法在保留原来id的情况下改属性的值，这样的话就得新建然后重新映射host id
    int shader_num = qemu_get_be32(f);   
    LOGI("in load native shaders! num %d", shader_num);

    for (int i = 0; i < shader_num; i++) {
        GLint shader_id = qemu_get_be64(f);
        GLenum shader_type = qemu_get_be32(f);
        GLboolean deleted_status = qemu_get_byte(f);
        GLboolean compile_status = qemu_get_byte(f);
        GLint source_length = qemu_get_be32(f);
        const char* shader_source = (char*)malloc(source_length);
        qemu_get_buffer(f, shader_source, source_length);
        LOGI("loading shader of id %d type %d content %s", shader_id, shader_type, shader_source);
        update_native_shader(shader_id, shader_type, deleted_status, compile_status, source_length, shader_source);
    }
}

void saveTextureAsPPM(const char *baseFilename, int textureId, int width, int height, unsigned char *pixels) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_%d.ppm", baseFilename, textureId);

    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", width, height);

    for (int i = 0; i < width * height; ++i) {
        fwrite(&pixels[i * 4], 1, 3, file); //只写入 RGB 三个字节 跳过 A 通道
    }


    fclose(file);
    LOGI("Saved texture to %s\n", filename);

    LOGI("First pixels in RGBA format:");
    for (int i = 0; i < width * height; i++) {
        LOGI("Pixel %d: R=%d, G=%d, B=%d, A=%d\n",
               i, 
               pixels[i * 4 + 0], //R
               pixels[i * 4 + 1], //G
               pixels[i * 4 + 2], //B
               pixels[i * 4 + 3]  //A
        );
    }
}

GLuint get_pixel_size(GLenum internalFormat) {
    switch (internalFormat) {
        case GL_ALPHA:
            return 1;  // 1 channel (Alpha), 1 byte per channel (8 bits/channel)
        case GL_RGBA8:
        case GL_SRGB8_ALPHA8_EXT:
        case GL_RGBA32F:
        case GL_RGBA8I:
        case GL_RGBA8UI:
        case GL_RGBA16F:
        case GL_SRGB_ALPHA:
            return 4;  // 4 channels (RGBA), 1 byte per channel (8 bits/channel)
        case GL_RGB8:
        case GL_RGB32F:
        case GL_RGB16F:
            return 3;  // 3 channels (RGB), 1 byte per channel (8 bits/channel)
        case GL_R8:
        case GL_R8I:
        case GL_R8UI:
        case GL_R16F:
        case GL_R16I:
        case GL_R16UI:
        case GL_R32F:
        case GL_R32I:
        case GL_R32UI:
            return 1;  // 1 channel (Red), 1 byte per channel (8 bits/channel)
        case GL_RG8:
        case GL_RG32F:
        case GL_RG16F:
        case GL_RG8I:
        case GL_RG8UI:
        case GL_RG16I:
        case GL_RG16UI:
            return 2;  // 2 channels (Red, Green), 1 byte per channel (8 bits/channel)
        case GL_RGB5_A1:
            return 4;  // 4 channels (RGB + Alpha), 5 bits for RGB, 1 bit for Alpha
        case GL_RGBA16:
            return 8;  // 4 channels (RGBA), 2 bytes per channel (16 bits/channel)
        case GL_RGBA16I:
        case GL_RGBA16UI:
            return 8;  // 4 channels (RGBA), 2 bytes per channel (16 bits/channel)
        case GL_RGB16:
            return 6;  // 3 channels (RGB), 2 bytes per channel (16 bits/channel)
        case GL_RGB16I:
        case GL_RGB16UI:
            return 6;  // 3 channels (RGB), 2 bytes per channel (16 bits/channel)
        case GL_RGBA32I:
        case GL_RGBA32UI:
            return 16;  // 4 channels (RGBA), 4 bytes per channel (32 bits/channel)
        case GL_RGB32I:
        case GL_RGB32UI:
            return 12;  // 3 channels (RGB), 4 bytes per channel (32 bits/channel)
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return 1;  // 1 channel (Depth), 1 byte per channel (8 bits/channel), or 4 bytes for float types
        case GL_DEPTH_STENCIL:
            return 2;  // 1 channel (Depth) + 1 channel (Stencil), 1 byte per channel (8 bits/channel)
        case GL_COMPRESSED_RGB:
        case GL_COMPRESSED_RGBA:
            return 1;  // Compressed formats; the size per pixel is format-dependent
        case GL_COMPRESSED_SRGB:
        case GL_COMPRESSED_SRGB_ALPHA:
            return 1;  // Compressed formats; the size per pixel is format-dependent
        default:
            return 4;  // Unrecognized format
    }
}


GLenum get_format_for_internal_format(GLint internal_format) {
    GLenum format; //ztodo:不确定是否齐全
    switch (internal_format) {
        case GL_ALPHA:
            format = GL_ALPHA;
            break;
        case GL_RGBA8:
            format = GL_RGBA;
            break;
        case GL_SRGB8_ALPHA8_EXT:
            format = GL_RGBA;
            break;
        case GL_R8:
            format = GL_RED;
            break;
        case GL_RG8:
            format = GL_RG;
            break;
        case GL_RGB8:
            format = GL_RGB;
            break;
        case GL_RGBA32F:
            format = GL_RGBA;
            break;
        case GL_RGB32F:
            format = GL_RGB;
            break;
        case GL_R16F:
            format = GL_RED;
            break;
        case GL_RG16F:
            format = GL_RG;
            break;
        case GL_R8I:
            format = GL_RED;
            break;
        case GL_RG8I:
            format = GL_RG;
            break;
        case GL_RGBA8I:
            format = GL_RGBA;
            break;
        case GL_R16I:
            format = GL_RED;
            break;
        case GL_RG16I:
            format = GL_RG;
            break;
        case GL_RGB16F:
            format = GL_RGB;
            break;
        case GL_RGBA16F:
            format = GL_RGBA;
            break;
        case GL_SRGB:
            format = GL_RGB;
            break;
        case GL_SRGB_ALPHA:
            format = GL_RGBA;
            break;
        case GL_DEPTH_COMPONENT:
            format = GL_DEPTH_COMPONENT;
            break;
        case GL_DEPTH_STENCIL:
            format = GL_DEPTH_STENCIL;
            break;
        default:
            format = GL_RGBA;
            break;
    }
    return format;
}

void save_single_texture(QEMUFile *f, GLint texture_id, GLenum texture_type) {
    Express_Native_Texture* state = g_malloc0(sizeof(Express_Native_Texture));

    state->textureId = texture_id;
    state->target = texture_type;
    

    GLenum target = state->target;
    if(target == GL_TEXTURE_EXTERNAL_OES) {
        target = GL_TEXTURE_2D;
    }


    glBindTexture(target, state->textureId);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);


    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &state->width);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &state->height);
    if (target == GL_TEXTURE_3D) {
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, &state->depth);
    } else {
        state->depth = 0;
    }
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &state->internalFormat);

    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&state->binding2D);

    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, (GLint *)&state->bindingCubeMap);

    glGetTexParameteriv(target, GL_TEXTURE_MIN_FILTER, &state->minFilter);
    glGetTexParameteriv(target, GL_TEXTURE_MAG_FILTER, &state->magFilter);
    glGetTexParameteriv(target, GL_TEXTURE_WRAP_S, &state->wrapS);
    glGetTexParameteriv(target, GL_TEXTURE_WRAP_T, &state->wrapT);

    glGetTexParameteriv(target, GL_TEXTURE_SWIZZLE_R, &state->texture_swizzle_r);
    glGetTexParameteriv(target, GL_TEXTURE_SWIZZLE_G, &state->texture_swizzle_g);
    glGetTexParameteriv(target, GL_TEXTURE_SWIZZLE_B, &state->texture_swizzle_b);
    glGetTexParameteriv(target, GL_TEXTURE_SWIZZLE_A, &state->texture_swizzle_a);

    qemu_put_be32(f, state->width);
    qemu_put_be32(f, state->height);
    qemu_put_be32(f, state->depth);
    qemu_put_be32(f, state->internalFormat);

    qemu_put_be32(f, state->minFilter);
    qemu_put_be32(f, state->magFilter);
    qemu_put_be32(f, state->wrapS);
    qemu_put_be32(f, state->wrapT);

    qemu_put_be32(f, state->texture_swizzle_r);
    qemu_put_be32(f, state->texture_swizzle_g);
    qemu_put_be32(f, state->texture_swizzle_b);
    qemu_put_be32(f, state->texture_swizzle_a);

    qemu_put_be32(f, state->binding2D);
    qemu_put_be32(f, state->bindingCubeMap);
    GLint pixel_size = get_pixel_size(state->internalFormat);
    LOGI("get pixel size %d", pixel_size);
    GLint size = state->width * state->height * pixel_size;
    state->pixels = (GLubyte *)malloc(state->width * state->height * pixel_size * 4); //ztodo:记得free
    memset(state->pixels, 0, size);

    //ztodo:使用pbo加速!!!


    GLenum format = get_format_for_internal_format(state->internalFormat);

    glGetTexImage(target, 0, format, GL_UNSIGNED_BYTE, state->pixels); //ztodo:第二个、倒数第二个参数
    LOGI("in saving texture of id %d target %d height %d width %d depth %d format %x pixels %d", texture_id, state->target, state->height, state->width, state->depth, state->internalFormat, size);
    qemu_put_buffer(f, state->pixels, size);    

    free(state->pixels);

    // unsigned char* zero_buffer = (unsigned char*)malloc(size);
    // memset(zero_buffer, 0, size);
    // if (memcmp(state->pixels, (const void*)zero_buffer, size) == 0) {
    // // 说明 state->pixels 的内容全为 0
    //     LOGI("Memory is all zeros");
    // } else {
    //     // 说明 state->pixels 的内容不是全为 0
    //     LOGI("Memory is not all zeros");
    //     // for (int i = 0; i < min(30, size); i++) {
    //     //     LOGD("unpack texture %d: R=%d, G=%d, B=%d, A=%d\n",
    //     //         i, 
    //     //         state->pixels[i * 4 + 0], //R
    //     //         state->pixels[i * 4 + 1], //G
    //     //         state->pixels[i * 4 + 2], //B
    //     //         state->pixels[i * 4 + 3]  //A
    //     //     );
    //     // }
    // }
    // free(zero_buffer);




//zodo:pbo加速相关代码
    // GLuint downloadPboId;
    // glGenBuffers(1, &downloadPboId);
    // glBindBuffer(GL_PIXEL_PACK_BUFFER, downloadPboId);
    // glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, NULL, GL_STREAM_READ);
    // glGetTexImage(state->target, 0, state->internalFormat, GL_UNSIGNED_BYTE, 0);
    // GLubyte* downloadPtr = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, dataSize, GL_MAP_READ_BIT);
    // if(downloadPtr) {
    //     memcpy(state->pixels, downloadPtr, dataSize);
    //     glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    // }
    // glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    // glDeleteBuffers(1, &downloadPboId);



}

void save_native_textures(QEMUFile *f){
    ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
    
    GHashTable* resource_list = g_resource_list[RESOURCE_TYPE_TEXTURE];

    qemu_put_be32(f, g_hash_table_size(resource_list));
    LOGI("saving texture num %d", g_hash_table_size(resource_list));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, resource_list);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Express_Native_Texture_Simple* texture = (Express_Native_Texture_Simple *)value;
        qemu_put_be64(f, texture->textureId);
        qemu_put_be32(f, texture->target);
        save_single_texture(f, texture->textureId, texture->target);        
        g_free(texture);
        
    }
    ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_TEXTURE]);
}


void update_native_texture(Express_Native_Texture* texture_data){
    GLuint glerror = 0;
    GLint width = 0, height = 0, currentTexture = 0;
    GLenum binding_target = texture_data->target;
    if(binding_target == GL_TEXTURE_EXTERNAL_OES) {
        binding_target = GL_TEXTURE_2D;
    } 
    // if(texture_data->textureId == 1 || texture_data->textureId == 3)
    // glDeleteTextures(1, (GLuint*)&texture_data->textureId);
    GLuint new_texture_id;
    // while(new_texture_id != texture_data->textureId) {
    //     glGenTextures(1, &new_texture_id);
    // }
    glGenTextures(1, &new_texture_id);

    GHashTable *resource_list = g_resource_list[RESOURCE_TYPE_TEXTURE];
    if(g_hash_table_lookup(resource_list, GUINT_TO_POINTER(new_texture_id)) == NULL) {
        struct Express_Native_Texture_Simple* texture_resource = g_malloc0(sizeof(Express_Native_Texture_Simple));
        texture_resource->target = binding_target;
        texture_resource->textureId = new_texture_id;        
        LOGD("in bindtexture save texture host id %d target %d", texture_resource->textureId, texture_resource->target);
        g_hash_table_insert(resource_list, GUINT_TO_POINTER(new_texture_id), texture_resource);            
    }

    glBindTexture(binding_target, new_texture_id);

    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture); 
    LOGI("before native resource textures info current texture %d target %d", currentTexture, texture_data->target);

    glTexParameteri(texture_data->target, GL_TEXTURE_MIN_FILTER, texture_data->minFilter);
    glTexParameteri(texture_data->target, GL_TEXTURE_MAG_FILTER, texture_data->magFilter);
    glTexParameteri(texture_data->target, GL_TEXTURE_WRAP_S, texture_data->wrapS);
    glTexParameteri(texture_data->target, GL_TEXTURE_WRAP_T, texture_data->wrapT);

    glTexParameteri(texture_data->target, GL_TEXTURE_SWIZZLE_R, texture_data->texture_swizzle_r);
    glTexParameteri(texture_data->target, GL_TEXTURE_SWIZZLE_G, texture_data->texture_swizzle_g);
    glTexParameteri(texture_data->target, GL_TEXTURE_SWIZZLE_B, texture_data->texture_swizzle_b);
    glTexParameteri(texture_data->target, GL_TEXTURE_SWIZZLE_A, texture_data->texture_swizzle_a);
    
    GLenum format = get_format_for_internal_format(texture_data->internalFormat); //ztodo:这么转换吗？


    if (texture_data->target == GL_TEXTURE_3D || texture_data->target == GL_TEXTURE_CUBE_MAP) {
        glTexParameteri(texture_data->target, GL_TEXTURE_WRAP_R, texture_data->wrapS);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    if (texture_data->target == GL_TEXTURE_2D || texture_data->target == GL_TEXTURE_EXTERNAL_OES) {
        glTexImage2D(GL_TEXTURE_2D, 0, texture_data->internalFormat, texture_data->width, texture_data->height, 0, format, GL_UNSIGNED_BYTE, texture_data->pixels);
        glerror = glGetError();
        if (glerror != GL_NO_ERROR) {
            LOGE("error! loading texture glTexImage2D failed! %x", glerror);
        } else {
            LOGI("success! loading texture glTexImage2D success! with id %d format %d internal %d %d %d", new_texture_id, format, texture_data->internalFormat, texture_data->target, texture_data->pixels);
        }
    } else if (texture_data->target == GL_TEXTURE_3D) {
        glTexImage3D(texture_data->target, 0, texture_data->internalFormat, texture_data->width, texture_data->height, texture_data->depth, 0, format, GL_UNSIGNED_BYTE, texture_data->pixels);
    } else if (texture_data->target == GL_TEXTURE_CUBE_MAP) {
        for (GLuint i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texture_data->internalFormat, texture_data->width, texture_data->height, 0, format, GL_UNSIGNED_BYTE, texture_data->pixels);
        }
    }

    // if (texture_data->target == GL_TEXTURE_2D) {
        // glGenerateMipmap(GL_TEXTURE_2D);
    // }

    // glBindTexture(texture_data->target, 0); //ztodo:应该不用
    LOGI("loaded native texture new id %d old id %d width %d height %d", new_texture_id, texture_data->textureId, texture_data->width, texture_data->height);
    LOGI("loaded texture all info: minFilter %d magFilter %d wrapS %d wrapT %d", texture_data->minFilter, texture_data->magFilter, texture_data->wrapS, texture_data->wrapT);
    if(texture_data->width == 1024){ //ztodo:这个可以删了吧？？？
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
        // change_host_id_map(RESOURCE_TYPE_TEXTURE, texture_data->textureId, texture_data->textureId);
        // memset(texture_data->pixels, 0, texture_data->width * texture_data->height * 4);
    }
    // else
    change_host_id_map(RESOURCE_TYPE_TEXTURE, texture_data->textureId, new_texture_id);
}

void load_native_textures_tmp(QEMUFile *f){
    int texture_num = qemu_get_be32(f);   
    LOGI("in load native textures! num %d", texture_num);

    for (int i = 0; i < texture_num; i++) {
        Express_Native_Texture* native_texture = g_malloc0(sizeof(struct Express_Native_Texture));
        native_texture->textureId = qemu_get_be64(f);
        native_texture->target = qemu_get_be32(f);
        native_texture->width = qemu_get_be32(f);
        native_texture->height = qemu_get_be32(f);
        native_texture->depth = qemu_get_be32(f);
        native_texture->internalFormat = qemu_get_be32(f);

        native_texture->minFilter = qemu_get_be32(f);
        native_texture->magFilter = qemu_get_be32(f);
        native_texture->wrapS = qemu_get_be32(f);
        native_texture->wrapT = qemu_get_be32(f);
        native_texture->binding2D = qemu_get_be32(f);
        native_texture->bindingCubeMap = qemu_get_be32(f);
                native_texture->wrapS = qemu_get_be32(f);
        native_texture->wrapT = qemu_get_be32(f);
        native_texture->binding2D = qemu_get_be32(f);
        native_texture->bindingCubeMap = qemu_get_be32(f);

        GLint pixel_size = get_pixel_size(native_texture->internalFormat);

        GLint source_length = native_texture->width * native_texture->height * pixel_size;

        const char* shader_source = (char*)malloc(native_texture->width * native_texture->height * 4);
        qemu_get_buffer(f, shader_source, source_length);
        LOGI("loading texture of id %d type %d", native_texture->textureId, native_texture->target);
        native_texture->pixels = (GLubyte*)shader_source;
        // bool isSame = compare_texture(native_texture);
        // if(isSame) { //如果没变就不操作了
        //     continue;
        // }

        GLint new_texture_id = native_texture->textureId;

//         if(native_texture->width == 1024) {
//             LOGI("change texture id %d", native_texture->textureId);
//             // memset(native_texture->pixels, 0, native_texture->width * native_texture->height * 4);
//             glBindTexture(native_texture->target, native_texture->textureId);
//     GLenum format = get_format_for_internal_format(native_texture->internalFormat); //ztodo:这么转换吗？
// // glTexSubImage2D(native_texture->target, 0, 0, 0, native_texture->width, native_texture->height, format, GL_UNSIGNED_BYTE, native_texture->pixels);
// // 再试试下面这种写法！
//             glTexImage2D(native_texture->target, 0, native_texture->internalFormat, native_texture->width, native_texture->height, 0, format, GL_UNSIGNED_BYTE, native_texture->pixels);
//             glBindTexture(native_texture->target, 0);
//         }
        change_host_id_map(RESOURCE_TYPE_TEXTURE, native_texture->textureId, new_texture_id);
        free(shader_source);
        g_free(native_texture);
    }
}


void load_native_textures(QEMUFile *f){
    int texture_num = qemu_get_be32(f);   
    LOGI("in load native textures! num %d", texture_num);

    for (int i = 0; i < texture_num; i++) {
        Express_Native_Texture* native_texture = g_malloc0(sizeof(struct Express_Native_Texture));
        native_texture->textureId = qemu_get_be64(f);
        native_texture->target = qemu_get_be32(f);
        native_texture->width = qemu_get_be32(f);
        native_texture->height = qemu_get_be32(f);
        native_texture->depth = qemu_get_be32(f);
        native_texture->internalFormat = qemu_get_be32(f);

        native_texture->minFilter = qemu_get_be32(f);
        native_texture->magFilter = qemu_get_be32(f);
        native_texture->wrapS = qemu_get_be32(f);
        native_texture->wrapT = qemu_get_be32(f);

        native_texture->texture_swizzle_r = qemu_get_be32(f);
        native_texture->texture_swizzle_g = qemu_get_be32(f);
        native_texture->texture_swizzle_b = qemu_get_be32(f);
        native_texture->texture_swizzle_a = qemu_get_be32(f);
        
        native_texture->binding2D = qemu_get_be32(f);
        native_texture->bindingCubeMap = qemu_get_be32(f);

        GLint pixel_size = get_pixel_size(native_texture->internalFormat);
        GLint source_length = native_texture->width * native_texture->height * pixel_size;

        const char* shader_source = (char*)malloc(native_texture->width * native_texture->height * 4);
        qemu_get_buffer(f, shader_source, source_length);
        LOGI("loading texture of id %d type %d", native_texture->textureId, native_texture->target);
        native_texture->pixels = (GLubyte*)shader_source;
        // bool isSame = compare_texture(native_texture);
        // if(isSame) { //如果没变就不操作了
        //     continue;
        // }
        update_native_texture(native_texture);
        free(shader_source);
        g_free(native_texture);
    }
}

int save_single_render_thread_context(QEMUFile *f, Render_Thread_Context *thread_context) {
    LOGI("in save single render thread context!");

    if (save_thread_context(f, &thread_context->context) < 0) {
        return -1;
    }

    if (thread_context->process_context) {
        Process_Context *process = get_process_context_form_id((thread_context->context).process_id);
        if(process != NULL) {
            qemu_put_byte(f, 1);//说明load的时候直接在map里查就行
            LOGI("get process %lld from map %d", (thread_context->context).process_id, g_hash_table_size(process->surface_map));
        } else {
            LOGE("error! can't find process from global map.");
        }

    }

    save_thread_unique_ids(f, thread_context->thread_unique_ids);  

    if (thread_context->render_double_buffer_read) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER((thread_context->render_double_buffer_read)->guest_surface));
        if(window_buffer != NULL) {
            // LOGI("save render_double_buffer_read is not null!");
            LOGI("save render_double_buffer_read is not null! %lld %lld", (uint64_t)(thread_context->render_double_buffer_read)->guest_surface, GUINT_TO_POINTER((thread_context->render_double_buffer_read)->guest_surface));
            qemu_put_be64(f, (uint64_t)(thread_context->render_double_buffer_read)->guest_surface);
        } else {
            qemu_put_be64(f, 0);
            LOGE("error! can't find render_double_buffer_read from global map!");
        }
    } else {
        qemu_put_be64(f, 0);
        LOGI("render_double_buffer_read is null!");
    }
    if (thread_context->render_double_buffer_draw) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER((thread_context->render_double_buffer_draw)->guest_surface));
        if(window_buffer != NULL) {
            LOGI("save render_double_buffer_draw is not null! %lld", (uint64_t)(thread_context->render_double_buffer_draw)->guest_surface);
            qemu_put_be64(f, (uint64_t)(thread_context->render_double_buffer_draw)->guest_surface);
        } else {
            qemu_put_be64(f, 0);
            LOGE("error! can't find render_double_buffer_draw from global map!");
        }
    } else {
        qemu_put_be64(f, 0);
        LOGI("render_double_buffer_draw is null!");
    }

    if (thread_context->opengl_context) {
        Opengl_Context* opengl_context = (Opengl_Context *)g_hash_table_lookup((thread_context->process_context)->context_map, GUINT_TO_POINTER((thread_context->opengl_context)->guest_context));
        if(opengl_context != NULL) {
            qemu_put_be64(f, (uint64_t)(thread_context->opengl_context)->guest_context);
        } else {
            LOGE("error! can't from opengl context from map!");
            qemu_put_be64(f, 0);
        }
        
    } else {
        qemu_put_be64(f, 0);
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

static void local_free_callback(Teleport_Express_Call *call, int notify) {
    g_free(call);
}

Render_Thread_Context* load_single_render_thread_context(QEMUFile *f) {
    LOGI("in load single render thread context!");


    Render_Thread_Context* thread_context = load_thread_context(f);

    // if (load_thread_context(f, &thread_context->context, unique_id) < 0) {
    //     return -1;
    // }

    // thread_context->process_context = g_malloc0(sizeof(Process_Context));
    // load_process_context(f, thread_context->process_context);
    int has_process = (int)qemu_get_byte(f);
    if(has_process) {
        thread_context->process_context = get_process_context_form_id((thread_context->context).process_id);
        LOGI("has process and get %lld %d %d", (uint64_t)(thread_context->context).process_id, thread_context->process_context->thread_cnt, g_hash_table_size(thread_context->process_context->surface_map));
    } else {
        LOGE("error! can't find process!");
    }

    load_thread_unique_ids(f, thread_context->thread_unique_ids);

    uint64_t has_read_window = qemu_get_be64(f);
    if (has_read_window) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER(has_read_window));
        if(window_buffer != NULL) {
            LOGI("successfully load render buffer read of %lld", has_read_window);
            thread_context->render_double_buffer_read = window_buffer;
        } else {
            LOGE("error! can't get render buffer read when loading %lld", has_read_window);
        }
        // thread_context->render_double_buffer_read = g_malloc0(sizeof(Window_Buffer));
        // load_window_buffer(f, thread_context->render_double_buffer_read, (thread_context->process_context)->gbuffer_map);
    } else {
        thread_context->render_double_buffer_read = NULL;
    }
    uint64_t has_write_window = qemu_get_be64(f);
    if (has_write_window) {
        Window_Buffer* window_buffer = (Window_Buffer *)g_hash_table_lookup((thread_context->process_context)->surface_map, GUINT_TO_POINTER(has_write_window));
        if(window_buffer != NULL) {
            thread_context->render_double_buffer_draw = window_buffer;
            LOGI("successfully load render buffer write of %lld %lld", has_write_window, GUINT_TO_POINTER(has_write_window));

        } else {
            LOGE("error! can't get render buffer write when loading %lld %d", has_write_window, g_hash_table_size((thread_context->process_context)->surface_map));

        }
        // thread_context->render_double_buffer_draw = g_malloc0(sizeof(Window_Buffer));
        // load_window_buffer(f, thread_context->render_double_buffer_draw, (thread_context->process_context)->gbuffer_map);
    } else {
        thread_context->render_double_buffer_draw = NULL;
    }

    uint64_t has_opengl_context = qemu_get_be64(f);
    if (has_opengl_context) {
        Opengl_Context* opengl_context = (Opengl_Context *)g_hash_table_lookup((thread_context->process_context)->context_map, GUINT_TO_POINTER(has_opengl_context));
        if(opengl_context != NULL) {
            thread_context->opengl_context = opengl_context;
        } else {
            LOGE("error! opengl context null when loading");
        }
        // thread_context->opengl_context = g_malloc0(sizeof(Opengl_Context));
        // if (load_opengl_context(f, thread_context->opengl_context) < 0) {
        //     LOGE("failed to load opengl context of thread context");
        //     return NULL;
        // }        
    } else {
        LOGI("has opengl context is null");
        thread_context->opengl_context = NULL;
    }

    int has_egl_display = qemu_get_be32(f);
    if(has_egl_display) {
        load_egl_display(f, thread_context->egl_display);
    }

    Teleport_Express_Call* call = g_malloc0(sizeof(Teleport_Express_Call));
    call->id = FUNID_snapshotLoad;
    call->thread_id = (thread_context->context).thread_id;
    call->process_id = (thread_context->context).process_id;
    call->unique_id = (thread_context->context).unique_id;
    call->callback = local_free_callback;

    if(has_opengl_context) {
        push_to_thread(call);
    }

    return thread_context;
}


int save_thread_context(QEMUFile *f, Thread_Context *context) {
    qemu_put_be64(f, context->device_id);
    // qemu_put_be32(f, context->read_loc);
    // qemu_put_be32(f, context->write_loc);
    // qemu_put_be32(f, context->init);
    // qemu_put_be32(f, context->thread_run);
    qemu_put_be64(f, context->thread_id);
    qemu_put_be64(f, context->unique_id);
    qemu_put_be64(f, context->process_id);

    LOGI("in save thread context with device id %lld thread id %lld process_id %lld unique_id %lld", context->device_id, context->thread_id, context->process_id, context->unique_id);

//可能需要保存事件的触发状态，恢复时重新创建handle?
// #ifdef _WIN32
//     DWORD event_state = WaitForSingleObject(context->data_event, 0);
//     qemu_put_be32(f, event_state == WAIT_OBJECT_0 ? 1 : 0);
// #endif

    return 0;
}

Render_Thread_Context* load_thread_context(QEMUFile *f) {
    uint64_t device_id = qemu_get_be64(f);
    // uint32_t read_loc = qemu_get_be32(f);
    // uint32_t write_loc = qemu_get_be32(f);
    // uint32_t init = qemu_get_be32(f);
    // uint32_t thread_run = qemu_get_be32(f);
    uint64_t thread_id = qemu_get_be64(f);
    uint64_t unique_id = qemu_get_be64(f);
    uint64_t process_id = qemu_get_be64(f);

    Express_Device_Info *device_info = get_express_device_info(EXPRESS_GPU_DEVICE_ID);

    Render_Thread_Context *context = (Render_Thread_Context *)device_info->get_context(device_id, thread_id, process_id, unique_id, device_info);

    LOGI("in load_thread_context with device id %lld thread id %lld process %lld unique %lld", device_id, thread_id, process_id, unique_id);

    return context;
}



void save_process_context(QEMUFile *f, Process_Context *process_context) {
    LOGI("in save_process_context!");
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
        // LOGI("saving window buffer %lld %lld", (uint64_t)window_buffer->gbuffer->gbuffer_id, guest_surface_id);
        qemu_put_be64(f, guest_surface_id);
        save_window_buffer(f, window_buffer, process_context->gbuffer_map);
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
        LOGI("in load process context with id %lld %lld", guest_surface_id, GUINT_TO_POINTER(guest_surface_id));
        Window_Buffer *window_buffer = g_malloc0(sizeof(Window_Buffer));
        load_window_buffer(f, window_buffer, process_context->gbuffer_map);
        g_hash_table_insert(process_context->surface_map, GUINT_TO_POINTER(guest_surface_id), window_buffer);
    }
    LOGI("process has surface map size %d", g_hash_table_size(process_context->surface_map));

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

void save_window_buffer(QEMUFile *f, Window_Buffer *buffer, GHashTable* gbuffer_map) {
    // if(type == 0) { //保存非map中的window buffer,就先看看map里有没有

    // }
    // LOGI("in save_window_buffer! %d", sizeof(buffer->window_hints.hints));
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
    //ztodo:gbuffer可能为null?
    if(buffer->gbuffer == NULL) {
        qemu_put_be32(f, 0);
        return;
    }
    qemu_put_be32(f, 1);
    uint64_t gbuffer_id = (buffer->gbuffer)->gbuffer_id;
    int target = 0;
    if(gbuffer_id == 0){
        //处理来自create_gbuffer_from_surface的gbuffer
        LOGI("save gbuffer from surface!");
        qemu_put_be32(f, target);
        save_hardware_buffer(f, buffer->gbuffer); 
    } else {
        target = 1; //来自global map
        Hardware_Buffer *gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        if(gbuffer == NULL) {
            target = 2; // 来自gbuffer_map
            gbuffer = (Hardware_Buffer *)g_hash_table_lookup(gbuffer_map, GUINT_TO_POINTER(gbuffer_id));
        } else {
            LOGI("save gbuffer from global map of id %lld", gbuffer_id);
        } 

        if(gbuffer == NULL) {
            LOGE("error! can't find gbuffer of %lld", gbuffer_id);
        } else {
            qemu_put_be32(f, target); //保存gbuffer来自哪的gbuffer
            qemu_put_be64(f, gbuffer_id);
        }
    }

    // save_hardware_buffer(f, buffer->gbuffer); //要是gbuffer id是global map的，那就可以查到。否则gbuffer_id是0，是create_from_surface的
}

int load_window_buffer(QEMUFile *f, Window_Buffer *buffer, GHashTable* gbuffer_map) {
    
    // Window_Buffer *buffer = g_malloc0(sizeof(Window_Buffer));
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
        buffer->date_fbo_changed[i] = 0;
        buffer->sampler_fbo_changed[i] = 0;
        GLint old_texture = qemu_get_be32(f);
        buffer->connect_texture[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, old_texture); 
        //ztodo:它们的id没有更新！！！！
        LOGI("load window buffer fbo %d %d %d %d %d", i, buffer->data_fbo[i], buffer->sampler_fbo[i], buffer->connect_texture[i], old_texture);
    }

    // buffer->gbuffer = load_hardware_buffer(f);
    int has_gbuffer = qemu_get_be32(f);
    if(has_gbuffer == 0) {
        buffer->gbuffer = NULL;
        return 0;
    }

    int target = qemu_get_be32(f);
    if(target == 0) {
        buffer->gbuffer = load_hardware_buffer(f);
    }
    else if(target == 1) {
        uint64_t gbuffer_id = qemu_get_be64(f);
        buffer->gbuffer = get_gbuffer_from_global_map(gbuffer_id);
        LOGI("load gbuffer from global map of id %lld", gbuffer_id);
    } else {
        uint64_t gbuffer_id = qemu_get_be64(f);
        buffer->gbuffer = (Hardware_Buffer *)g_hash_table_lookup(gbuffer_map, GUINT_TO_POINTER(gbuffer_id));
    }

    g_hash_table_insert(loaded_window_buffers, GUINT_TO_POINTER(buffer), buffer);

    return 0;
}


//开始巨大的save opengl context！

void save_scatter_data(QEMUFile *f, Scatter_Data *scatter_data, int count) {
    qemu_put_be32(f, count);
    // qemu_put_be32(f, scatter_data->len);

    // // int size = sizeof(Scatter_Data) * count;
    // int size = scatter_data->len * count;

    // qemu_put_buffer(f, (const uint8_t *)scatter_data, size);

    for (int i = 0; i < count; i++) {
        qemu_put_be32(f, scatter_data[i].len);
        // qemu_put_buffer(f, scatter_data[i].data, scatter_data[i].len);
        uint64_t address = 0;
        address = ((uint64_t)scatter_data[i].data);// & 0xFFFFFFFFF;
        void* real_guest_mem = (void *)qemu_ram_addr_from_host((void*)address);

        qemu_put_be64(f, (uint64_t)real_guest_mem);
        // LOGI("scatter data size is %d data %llx %llx", sizeof(scatter_data[i].data), address, (uint64_t)real_guest_mem);

        // LOGI("saving scatter data %d %lld", (int)scatter_data[i].data, (uint64_t)scatter_data[i].data);
        // qemu_put_buffer(f, scatter_data[i].data, scatter_data[i].len);
    }
}

// void save_scatter_data_sync(QEMUFile *f, Scatter_Data *scatter_data, int count) {
//     qemu_put_be32(f, count);
//     // qemu_put_be32(f, scatter_data->len);

//     int size = sizeof(Scatter_Data) * count;

//     qemu_put_buffer(f, (const uint8_t *)scatter_data, size);
// }

Scatter_Data* load_scatter_data(QEMUFile *f, int *count) {
    *count = qemu_get_be32(f);
    // int len = qemu_get_be32(f);
    int size = sizeof(Scatter_Data) * (*count);
    // int size = len * (*count);

    // LOGI("scatter data size is %d", sizeof(data));

    Scatter_Data *scatter_data = g_malloc0(size);
    memset(scatter_data, 0, size);
    // qemu_get_buffer(f, (const uint8_t *)scatter_data, size);
    for (int i = 0; i < *count; i++) {
        scatter_data[i].len = qemu_get_be32(f);
        hwaddr len = scatter_data[i].len;
        // LOGI("loading scatter data len %d", len);

        uint64_t address = qemu_get_be64(f); //存的是gpa
        LOGI("loading scatter data address %llu", address);
        // uint64_t hva = (uint64_t)cpu_physical_memory_map((hwaddr)address, &len, false);
        // uint64_t hva1 = (uint64_t)cpu_physical_memory_map((hwaddr)address, &len, true);

        hwaddr xlat;

        // LOGI("loading scatter data address %llx", address);

        MemoryRegion *mr = address_space_translate(&address_space_memory,
            (hwaddr)address,
            &xlat, &len, false,
            MEMTXATTRS_UNSPECIFIED);

        // void *hva = cpu_physical_memory_map((hwaddr)real_guest_mem, &len, false);
        void *hva = NULL;
        if (mr) {
            hva = qemu_map_ram_ptr(mr->ram_block, xlat);
            // printf("GPA 0x%lx corresponds to HVA %p\n", gpa, hva);
        } 

        // qemu_get_buffer(f, (void*)&address, sizeof(address));
        scatter_data[i].data = (unsigned char *)hva;
    
        // LOGI("loading scatter data %d %lld", (int)scatter_data[i].data, (uint64_t)scatter_data[i].data);

        // if(scatter_data[i].len == 0) {
        //     scatter_data[i].data = NULL;
        //     continue;
        // }
        // void* tmpptr = g_malloc0(scatter_data[i].len);
        // qemu_get_buffer(f, tmpptr, scatter_data[i].len);
        // memcpy(scatter_data[i].data, tmpptr, scatter_data[i].len);
        LOGI("loading scatter data len %d %d %llu %llu", len, *count, (unsigned long long)hva, (unsigned long long)address);
    }
    

    return scatter_data;
}

void save_guest_mem(QEMUFile *f, Guest_Mem *guest_mem) {
    qemu_put_be32(f, guest_mem->num);
    qemu_put_be32(f, guest_mem->all_len);
    save_scatter_data(f, guest_mem->scatter_data, guest_mem->num);
}

Guest_Mem* load_guest_mem(QEMUFile *f, int strategy) {
    Guest_Mem *guest_mem = NULL;
    if(strategy == 0) {
        guest_mem = g_malloc(sizeof(Guest_Mem));
        LOGI("in 1 get guest mem pointer %lld size %d", (uint64_t)guest_mem, sizeof(Guest_Mem));
    } else {
        guest_mem = alloc_one_guest_mem();
    }
    // Guest_Mem *guest_mem = alloc_one_guest_mem();//g_malloc0(sizeof(Guest_Mem));
    
    LOGI("get guest mem pointer %lld size %d", (uint64_t)guest_mem, sizeof(Guest_Mem));

    guest_mem->num = qemu_get_be32(f);
    guest_mem->all_len = qemu_get_be32(f);

    LOGI("going to load scatter data %d %d", guest_mem->num, guest_mem->all_len);
    guest_mem->scatter_data = load_scatter_data(f, &guest_mem->num);
    LOGI("successfully load scatter data %d %d %d data %llx", guest_mem->num, guest_mem->all_len, guest_mem->scatter_data[0].len, guest_mem->scatter_data[0].data);
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

    LOGI("save hardware buffer texture id %llx %d %d", buffer->gbuffer_id, buffer->data_texture, buffer->data_fbo);
}

Hardware_Buffer* load_hardware_buffer(QEMUFile *f) {
    Hardware_Buffer *buffer = g_malloc0(sizeof(Hardware_Buffer));

    buffer->is_writing = qemu_get_be32(f);
    buffer->is_lock = qemu_get_be32(f);
    buffer->sampler_num = qemu_get_be32(f);
    buffer->data_texture = qemu_get_be32(f);
    GLuint new_texture = get_host_id_map(RESOURCE_TYPE_TEXTURE, buffer->data_texture);  
    LOGI("load hardware buffer texture id %d %d", buffer->data_texture, new_texture);
    buffer->data_texture = new_texture;

    buffer->reverse_rbo = qemu_get_be32(f);
    buffer->sampler_rbo = qemu_get_be32(f);
    buffer->rbo_depth = qemu_get_be32(f);
    buffer->rbo_stencil = qemu_get_be32(f);

    buffer->data_fbo = qemu_get_be32(f);
    buffer->sampler_fbo = qemu_get_be32(f);

    buffer->data_fbo_changed = 0;
    buffer->sampler_fbo_changed = 0;
    
    buffer->has_connected_fbo = qemu_get_be32(f);
    buffer->has_connected_fbo = 0;

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

    glGenRenderbuffers(1, &(buffer->rbo_depth));
    glGenRenderbuffers(1, &(buffer->rbo_stencil));

    if (buffer->sampler_num > 1)
    {
        glGenRenderbuffers(1, &(buffer->sampler_rbo));
    }


    if (buffer->depth_internal_format != 0)
    {
        // 这个相当于给与一个深度缓冲区，让这个fbo可以有颜色缓冲区，有深度缓冲区，模板缓冲区
        LOGI("in load gbuffer gen depth of id %d", buffer->rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo_depth);
        if (buffer->sampler_num > 1)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, buffer->sampler_num, buffer->depth_internal_format, buffer->width, buffer->height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, buffer->depth_internal_format, buffer->width, buffer->height);
        }
        GLenum glerror = glGetError();
        if (glerror != GL_NO_ERROR)
        {
            LOGE("error! gen depth buffer failed %d", glerror);
        }
    }

    // 之所以当深度24模板8时要合并，是因为这样效率更高
    if (buffer->stencil_internal_format != 0 && buffer->depth_internal_format != GL_DEPTH24_STENCIL8)
    {
        LOGI("in load gbuffer gen stencil of id %d", buffer->rbo_stencil);
        glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo_stencil);
        if (buffer->sampler_num > 1)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, buffer->sampler_num, buffer->stencil_internal_format, buffer->width, buffer->height);
        }
        else
        {
            glRenderbufferStorage(GL_RENDERBUFFER, buffer->stencil_internal_format, buffer->width, buffer->height);
        }
        GLenum glerror = glGetError();
        if (glerror != GL_NO_ERROR)
        {
            LOGE("error! gen stencil buffer failed %d", glerror);
        }
    }

    //todo 不知道这个是啥！该咋存！
    buffer->data_sync = (GLsync)qemu_get_be64(f);
    buffer->delete_sync = (GLsync)qemu_get_be64(f);

    int has_guest_data = qemu_get_be32(f);
    if (has_guest_data) {
        buffer->guest_data = load_guest_mem(f, 0);
    } else {
        buffer->guest_data = NULL;
    }

    buffer->host_data = (void *)qemu_get_be64(f);

    g_hash_table_insert(loaded_hardware_buffers, GUINT_TO_POINTER(buffer), buffer);

    return buffer;
}


void save_attrib_point(QEMUFile *f, Attrib_Point *point) {
    for (int i = 0; i < MAX_VERTEX_ATTRIBS_NUM; i++) {
        // qemu_put_be32(f, point->buffer_object[i]);
        save_native_buffer(f, point->buffer_object[i], GL_ARRAY_BUFFER);
        qemu_put_be32(f, point->buffer_loc[i]);
        qemu_put_be32(f, point->remain_buffer_len[i]);
        qemu_put_be32(f, point->buffer_len[i]);
    }
    // qemu_put_be32(f, point->indices_buffer_object);
    save_native_buffer(f, point->indices_buffer_object, GL_ELEMENT_ARRAY_BUFFER);

    qemu_put_be32(f, point->indices_buffer_len);
    qemu_put_be32(f, point->remain_indices_buffer_len);
    qemu_put_be32(f, point->element_array_buffer);
}

Attrib_Point* load_attrib_point(QEMUFile *f) {
    Attrib_Point *point = g_malloc0(sizeof(Attrib_Point));
    for (int i = 0; i < MAX_VERTEX_ATTRIBS_NUM; i++) {
        // point->buffer_object[i] = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
        point->buffer_object[i] = qemu_get_be32(f);
        load_native_buffer(f, point->buffer_object[i], GL_ARRAY_BUFFER, 2);
        point->buffer_object[i] = get_host_id_map(RESOURCE_TYPE_BUFFER, point->buffer_object[i]);

        point->buffer_loc[i] = qemu_get_be32(f);
        point->remain_buffer_len[i] = qemu_get_be32(f);
        point->buffer_len[i] = qemu_get_be32(f);
        LOGI("load attrib point vbo %d %d %d %d", point->buffer_object[i], point->buffer_loc[i], point->remain_buffer_len[i], point->buffer_len[i]);
    }
    // point->indices_buffer_object = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    point->indices_buffer_object = qemu_get_be32(f);
    load_native_buffer(f, point->indices_buffer_object, GL_ELEMENT_ARRAY_BUFFER, 2);
    point->indices_buffer_object = get_host_id_map(RESOURCE_TYPE_BUFFER, point->indices_buffer_object);

    point->indices_buffer_len = qemu_get_be32(f);
    point->remain_indices_buffer_len = qemu_get_be32(f);
    // point->element_array_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    point->element_array_buffer = qemu_get_be32(f);

    LOGI("load attrib point ebo %d %d %d %d", point->indices_buffer_object, point->indices_buffer_len, point->remain_indices_buffer_len, point->element_array_buffer);
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
    LOGI("guest and host vao value %d %d", status->guest_vao, status->host_vao);
}

Buffer_Status* load_buffer_status(QEMUFile *f) {
    Buffer_Status *status = g_malloc0(sizeof(Buffer_Status));

    status->guest_array_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_array_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_element_array_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_element_array_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    LOGI("loading buffer status vbo %d %d ebo %d %d", status->guest_array_buffer, status->host_array_buffer, status->guest_element_array_buffer, status->host_element_array_buffer);

    status->guest_copy_read_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_copy_read_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_copy_write_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_copy_write_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_pixel_pack_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_pixel_pack_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_pixel_unpack_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_pixel_unpack_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_transform_feedback_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_transform_feedback_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_uniform_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_uniform_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_atomic_counter_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_atomic_counter_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_dispatch_indirect_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_dispatch_indirect_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_draw_indirect_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_draw_indirect_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_shader_storage_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_shader_storage_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_texture_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_texture_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_vertex_array_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_vertex_array_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_vao_ebo = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));
    status->host_vao_ebo = get_host_id_map(RESOURCE_TYPE_BUFFER, qemu_get_be32(f));

    status->guest_vao = qemu_get_be32(f);
    status->host_vao = qemu_get_be32(f);

    return status;
}

void save_bound_buffer(QEMUFile *f, Bound_Buffer *buffer) {
    save_attrib_point(f, buffer->attrib_point);
    save_buffer_status(f, &buffer->buffer_status);
    // qemu_put_be32(f, buffer->asyn_unpack_texture_buffer);
    // qemu_put_be32(f, buffer->asyn_pack_texture_buffer);

    save_native_buffer(f, buffer->asyn_unpack_texture_buffer, GL_PIXEL_UNPACK_BUFFER);
    save_native_buffer(f, buffer->asyn_pack_texture_buffer, GL_PIXEL_PACK_BUFFER);

    qemu_put_be32(f, buffer->has_init);

    guint num_entries = g_hash_table_size(buffer->vao_point_data);
    qemu_put_be32(f, num_entries);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, buffer->vao_point_data);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        
        GLuint vao_id = GPOINTER_TO_UINT(key);
        Attrib_Point *point_data = (Attrib_Point *)value;

        qemu_put_be32(f, vao_id);
        LOGI("save vao id %lld", vao_id);
        save_attrib_point(f, point_data);
    }
}

Bound_Buffer* load_bound_buffer(QEMUFile *f) {
    Bound_Buffer *buffer = g_malloc0(sizeof(Bound_Buffer));
    buffer->attrib_point = load_attrib_point(f);
    buffer->buffer_status = *load_buffer_status(f);

    GLuint old_asyn_unpack_texture_buffer = qemu_get_be32(f);
    load_native_buffer(f, old_asyn_unpack_texture_buffer, GL_PIXEL_UNPACK_BUFFER, 2);
    buffer->asyn_unpack_texture_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, old_asyn_unpack_texture_buffer);
    GLuint old_asyn_pack_texture_buffer = qemu_get_be32(f);
    load_native_buffer(f, old_asyn_pack_texture_buffer, GL_PIXEL_PACK_BUFFER, 2);
    buffer->asyn_pack_texture_buffer = get_host_id_map(RESOURCE_TYPE_BUFFER, old_asyn_pack_texture_buffer);
    
    LOGI("old and new pack and unpack texture buffer %d %d %d %d", old_asyn_unpack_texture_buffer, buffer->asyn_unpack_texture_buffer, old_asyn_pack_texture_buffer, buffer->asyn_pack_texture_buffer);

    buffer->has_init = qemu_get_be32(f);

    guint num_entries = qemu_get_be32(f);
    buffer->vao_point_data = g_hash_table_new(g_direct_hash, g_direct_equal);

    for (guint i = 0; i < num_entries; i++) {
        GLuint vao_id = qemu_get_be32(f);
        // GLuint new_id = get_host_id_map(RESOURCE_TYPE_VERTEX_ARRAY, vao_id);

        LOGI("load vao id %lld", vao_id);

        Attrib_Point *point_data = load_attrib_point(f);

        g_hash_table_insert(buffer->vao_point_data, GUINT_TO_POINTER(vao_id), point_data);
    }

    return buffer;
}

void save_resource_map_status(QEMUFile *f, Resource_Map_Status *status) {
    LOGI("save resource map status with map_size %d max size %d", status->map_size, status->gbuffer_map_max_size);
    qemu_put_be32(f, status->max_id);
    qemu_put_be32(f, status->map_size);
    
    for (unsigned int i = 0; i < status->map_size; i++) {
        LOGD("saving %d %d %d", i, status->resource_id_map[i], status->resource_is_init[i]);
        qemu_put_be64(f, status->resource_id_map[i]);
        qemu_put_byte(f, status->resource_is_init[i]);
    }

    qemu_put_be32(f, status->gbuffer_map_max_size);

    for (unsigned int i = 0; i < status->gbuffer_map_max_size; i++) {
        if (status->gbuffer_id_map[i] != 0 && get_gbuffer_from_global_map(status->gbuffer_id_map[i]) != NULL) {
            qemu_put_byte(f, 1); //有有效数据
            LOGI("save resource map status with gbuffer id %lld id %d", status->gbuffer_id_map[i], i);
            save_hardware_buffer(f, get_gbuffer_from_global_map(status->gbuffer_id_map[i]));
        } else {
            LOGD("buffer of %d is null!", i);
            qemu_put_byte(f, 0);
        }
    }
}

Resource_Map_Status* load_resource_map_status(QEMUFile *f, int resource_type) {
    Resource_Map_Status *status = g_malloc0(sizeof(Resource_Map_Status));
    status->max_id = qemu_get_be32(f);
    status->map_size = qemu_get_be32(f);
    status->resource_id_map = g_malloc0(sizeof(long long) * status->map_size);
    status->resource_is_init = g_malloc0(sizeof(char) * status->map_size);
    for (unsigned int i = 0; i < status->map_size; i++) {
        status->resource_id_map[i] = qemu_get_be64(f);
        status->resource_is_init[i] = qemu_get_byte(f);
        long long new_id = (long long)g_hash_table_lookup(g_resource_ids_map[resource_type], GUINT_TO_POINTER(status->resource_id_map[i])); 
        if(new_id != NULL) {
            LOGD("in load resource of type %d change id from %lld to %lld", resource_type, status->resource_id_map[i], new_id);
            status->resource_id_map[i] = new_id;//ztodo：这里的语法？
        }

        // if(status->resource_id_map[i] != 0) 

        LOGD("loading %d %d %d %d", resource_type, status->resource_id_map[i], status->resource_is_init[i], g_hash_table_size(g_resource_ids_map[resource_type]));
    }

    status->gbuffer_map_max_size = qemu_get_be32(f);
    status->gbuffer_id_map = g_malloc0(sizeof(uint64_t) * status->gbuffer_map_max_size);

    for (unsigned int i = 0; i < status->gbuffer_map_max_size; i++) {
        if (qemu_get_byte(f)) {
            status->gbuffer_id_map[i] = load_hardware_buffer(f)->gbuffer_id;
            LOGD("load resource map status with gbuffer id %lld id %d", status->gbuffer_id_map[i], i);

        } else {
            status->gbuffer_id_map[i] = 0;
        }
    }
    LOGD("load resource map status with map_size %d max size %d", status->map_size, status->gbuffer_map_max_size);
    return status;
}

void save_resource_context(QEMUFile *f, Resource_Context *context) {

    save_resource_map_status(f, context->texture_resource);
    LOGI("finish saving texture resource!");
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

    qemu_put_be32(f, context->share_resources->counter);
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

    context->texture_resource = load_resource_map_status(f, RESOURCE_TYPE_TEXTURE);
    LOGI("finish loading texture resource!");

    context->buffer_resource = load_resource_map_status(f, RESOURCE_TYPE_BUFFER);
    context->render_buffer_resource = load_resource_map_status(f, RESOURCE_TYPE_RENDERBUFFER);
    context->sampler_resource = load_resource_map_status(f, RESOURCE_TYPE_SAMPLER);

    context->shader_resource = load_resource_map_status(f, RESOURCE_TYPE_SHADER);
    context->program_resource = load_resource_map_status(f, RESOURCE_TYPE_PROGRAM);

    context->sync_resource = load_resource_map_status(f, RESOURCE_TYPE_SYNC);

    context->frame_buffer_resource = load_resource_map_status(f, RESOURCE_TYPE_FRAMEBUFFER);
    context->program_pipeline_resource = load_resource_map_status(f, RESOURCE_TYPE_PROGRAM_PIPELINE);
    context->transform_feedback_resource = load_resource_map_status(f, RESOURCE_TYPE_TRANSFORM_FEEDBACK);
    context->vertex_array_resource = load_resource_map_status(f, RESOURCE_TYPE_VERTEX_ARRAY);

    context->query_resource = load_resource_map_status(f, RESOURCE_TYPE_QUERY);

    context->share_resources = g_malloc0(sizeof(Share_Resources));
    context->share_resources->counter = qemu_get_be32(f);
    context->share_resources->texture_resource = *load_resource_map_status(f, RESOURCE_TYPE_TEXTURE);
    context->share_resources->buffer_resource = *load_resource_map_status(f, RESOURCE_TYPE_BUFFER);
    context->share_resources->render_buffer_resource = *load_resource_map_status(f, RESOURCE_TYPE_RENDERBUFFER);
    context->share_resources->sample_resource = *load_resource_map_status(f, RESOURCE_TYPE_SAMPLER);
    context->share_resources->program_resource = *load_resource_map_status(f, RESOURCE_TYPE_PROGRAM);
    context->share_resources->shader_resource = *load_resource_map_status(f, RESOURCE_TYPE_SHADER);
    context->share_resources->sync_resource = *load_resource_map_status(f, RESOURCE_TYPE_SYNC);

    context->exclusive_resources = g_malloc0(sizeof(Exclusive_Resources));
    context->exclusive_resources->frame_buffer_resource = *load_resource_map_status(f, RESOURCE_TYPE_FRAMEBUFFER);
    context->exclusive_resources->program_pipeline_resource = *load_resource_map_status(f, RESOURCE_TYPE_PROGRAM_PIPELINE);
    context->exclusive_resources->transform_feedback_resource = *load_resource_map_status(f, RESOURCE_TYPE_TRANSFORM_FEEDBACK);
    context->exclusive_resources->vertex_array_resource = *load_resource_map_status(f, RESOURCE_TYPE_VERTEX_ARRAY);
    context->exclusive_resources->query_resource = *load_resource_map_status(f, RESOURCE_TYPE_QUERY);

    return context;
}

void save_texture_binding_status(QEMUFile *f, Texture_Binding_Status *status) {
    qemu_put_be32(f, status->guest_current_active_texture);
    qemu_put_be32(f, status->host_current_active_texture);
    qemu_put_be32(f, status->now_max_texture_unit);
    LOGI("in save texture binding status with max unit %d %d %d", status->now_max_texture_unit, sizeof(status->guest_current_texture_2D)/sizeof(int), status->texture_unit_num);

    int tot_num = sizeof(status->guest_current_texture_2D)/sizeof(int);
    // qemu_put_be32(f, tot_num);
    for (GLuint i = 0; i <= status->now_max_texture_unit; i++) {
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

    LOGI("value of all status are %d %d %d %d %d %d %llx %d %d %d %d %d %d %d", status->texture_unit_num, status->guest_current_active_texture, status->host_current_active_texture, status->now_max_texture_unit, status->current_texture_external, status->current_2D_gbuffer, status->guest_current_texture_2D[0], status->host_current_texture_2D[0], status->guest_current_texture_cube_map[0], status->host_current_texture_cube_map[0], status->guest_current_texture_3D[0], status->host_current_texture_3D[0], status->guest_current_texture_2D_array[0], status->host_current_texture_2D_array[0]);

    qemu_put_be32(f, status->texture_unit_num);
    
    qemu_put_be32(f, status->current_texture_external);

    qemu_put_be64(f, (uint64_t)status->current_2D_gbuffer);

    if(status->current_2D_gbuffer != NULL) {
        save_hardware_buffer(f, status->current_2D_gbuffer);
    }
}

Texture_Binding_Status* load_texture_binding_status(QEMUFile *f) { //ztodo:这些都得改
    Texture_Binding_Status *status = g_malloc0(sizeof(Texture_Binding_Status));

    status->guest_current_active_texture = qemu_get_be32(f);
    status->host_current_active_texture = qemu_get_be32(f);
    status->now_max_texture_unit = qemu_get_be32(f);

    int tot_num = status->now_max_texture_unit + 1;

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
        status->guest_current_texture_2D[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_2D[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->guest_current_texture_cube_map[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_cube_map[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->guest_current_texture_3D[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_3D[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->guest_current_texture_2D_array[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_2D_array[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->guest_current_texture_2D_multisample[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_2D_multisample[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->guest_current_texture_2D_multisample_array[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_2D_multisample_array[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->guest_current_texture_cube_map_array[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_cube_map_array[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->guest_current_texture_buffer[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
        status->host_current_texture_buffer[i] = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));
    }

    LOGI("loaded values of all status are %d %d %d %d %d %d %d %d %d %d %d %d %d %d", status->texture_unit_num, status->guest_current_active_texture, status->host_current_active_texture, status->now_max_texture_unit, status->current_texture_external, status->current_2D_gbuffer, status->guest_current_texture_2D[status->guest_current_active_texture], status->host_current_texture_2D[status->guest_current_active_texture], status->guest_current_texture_cube_map[status->guest_current_active_texture], status->host_current_texture_cube_map[status->guest_current_active_texture], status->guest_current_texture_3D[0], status->host_current_texture_3D[0], status->guest_current_texture_2D_array[0], status->host_current_texture_2D_array[0]);


    status->texture_unit_num = qemu_get_be32(f);
    // status->now_max_texture_unit = qemu_get_be32(f);
    status->current_texture_external = get_host_id_map(RESOURCE_TYPE_TEXTURE, qemu_get_be32(f));

    status->current_2D_gbuffer = (Hardware_Buffer *)(uint64_t)qemu_get_be64(f);
    if(status->current_2D_gbuffer != 0) {
        status->current_2D_gbuffer = load_hardware_buffer(f);
    }

    return status;
}


void save_opengl_context(QEMUFile *f, Opengl_Context *context) {

    LOGI("in save opengl context of %llx %llx", context, context->window);

    //不确定是否需要深拷贝，先浅拷贝了 update:需要额外处理，先注释了
    // qemu_put_be64(f, context->window);
    // LOGI("the native context saving now is %llx", (int64_t)context->window);
    // save_native_context(f, context->window);

    save_bound_buffer(f, &context->bound_buffer_status);
    save_resource_context(f, &context->resource_status);
    save_texture_binding_status(f, &context->texture_binding_status);    

    //不确定是否需要深拷贝，先浅拷贝了
    qemu_put_be64(f, (uint64_t)context->share_context);




    // ATOMIC_LOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);

    qemu_put_be32(f, context->draw_fbo0);
    // save_single_framebuffer(f, context->draw_fbo0);
    qemu_put_be32(f, context->read_fbo0);
    // save_single_framebuffer(f, context->read_fbo0);
    // LOGI("in save opengl context with fbo0 %d %d", context->draw_fbo0, context->read_fbo0);
    // ATOMIC_UNLOCK(g_resource_locker[RESOURCE_TYPE_FRAMEBUFFER]);

    save_native_framebuffers(f, context->framebuffer_map);

    qemu_put_be32(f, context->current_read_fbo);
    qemu_put_be32(f, context->current_write_fbo);
    qemu_put_be32(f, context->current_program);


    qemu_put_be32(f, context->vao0);
    LOGI("in save opengl context with fbo0 %d %d %d", context->draw_fbo0, context->read_fbo0, context->vao0);

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

    qemu_put_be32(f, context->blendfunc_dfactor);
    qemu_put_be32(f, context->blendfunc_sfactor);

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

void restore_opengl_context_textures(Opengl_Context *context) {
    Texture_Binding_Status *status = &(context->texture_binding_status);
    // egl_makeCurrent(context->window);


    texture_binding_status_sync(context, GL_TEXTURE_2D);
    texture_binding_status_sync(context, GL_TEXTURE_2D_MULTISAMPLE);
    texture_binding_status_sync(context, GL_TEXTURE_2D_MULTISAMPLE_ARRAY);
    texture_binding_status_sync(context, GL_TEXTURE_3D);
    texture_binding_status_sync(context, GL_TEXTURE_2D_ARRAY);
    texture_binding_status_sync(context, GL_TEXTURE_CUBE_MAP);
    texture_binding_status_sync(context, GL_TEXTURE_CUBE_MAP_ARRAY);
    texture_binding_status_sync(context, GL_TEXTURE_BUFFER);
    texture_binding_status_sync(context, GL_TEXTURE_EXTERNAL_OES);

    GLuint current_active_texture = status->guest_current_active_texture;
    glActiveTexture(current_active_texture + GL_TEXTURE0);

    GLuint glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("glActiveTexture error %d", glerror);
    }

    GLint textureId = 0;
    // 尝试不bind那个texture
    glBindTexture(GL_TEXTURE_2D, status->guest_current_texture_2D[current_active_texture]);   
    GLint width = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    // if(width == 1024){
    //     LOGI("texture is for words!");
    //     glBindTexture(GL_TEXTURE_2D, 0);
    //     glDeleteTextures(1, &status->guest_current_texture_2D[current_active_texture]);
    // }
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureId);
    LOGI("The currently bound GL_TEXTURE_2D ID is: %d should bind is %d", textureId, status->guest_current_texture_2D[current_active_texture]);

    glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("glBindTexture GL_TEXTURE_2D error %x", glerror);
    }

    textureId = 0;    
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, status->guest_current_texture_2D_multisample[current_active_texture]);
    glGetIntegerv(GL_TEXTURE_2D_MULTISAMPLE, &textureId);
    LOGI("The currently bound GL_TEXTURE_2D_MULTISAMPLE ID is:%d %d", status->guest_current_texture_2D_multisample[current_active_texture], textureId);

    glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("glBindTexture GL_TEXTURE_2D_MULTISAMPLE error %x", glerror);
    }

    textureId = 0;
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, status->guest_current_texture_2D_multisample_array[current_active_texture]);    
    glGetIntegerv(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, &textureId);
    LOGI("The currently bound GL_TEXTURE_2D_MULTISAMPLE_ARRAY ID is: %d %d", textureId, status->guest_current_texture_2D_multisample_array[current_active_texture]);
    glerror = glGetError();
    if (glerror != GL_NO_ERROR) {
        LOGE("glBindTexture GL_TEXTURE_2D_MULTISAMPLE_ARRAY error %x", glerror);
    }
    

    glBindTexture(GL_TEXTURE_3D, status->guest_current_texture_3D[current_active_texture]);
    glGetIntegerv(GL_TEXTURE_3D, &textureId);
    LOGI("The currently bound GL_TEXTURE_3D ID is: %d", textureId);

    // glBindTexture(GL_TEXTURE_2D_ARRAY, status->guest_current_texture_2D_array[current_active_texture]);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, status->guest_current_texture_cube_map[current_active_texture]);
    // glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, status->guest_current_texture_cube_map_array[current_active_texture]);
    // glBindTexture(GL_TEXTURE_BUFFER, status->guest_current_texture_buffer[current_active_texture]);
    LOGI("restoring textures all values %d %d %d %d %d %d %d %d %d %d %d %d %d %d", status->texture_unit_num, status->guest_current_active_texture, status->host_current_active_texture, status->now_max_texture_unit, status->current_texture_external, status->current_2D_gbuffer, status->guest_current_texture_2D[current_active_texture], status->host_current_texture_2D[current_active_texture], status->guest_current_texture_cube_map[current_active_texture], status->host_current_texture_cube_map[current_active_texture], status->guest_current_texture_3D[current_active_texture], status->host_current_texture_3D[current_active_texture], status->guest_current_texture_2D_array[current_active_texture], status->host_current_texture_2D_array[current_active_texture]);
}

int load_opengl_context(QEMUFile *f, Opengl_Context *context) {

    // Opengl_Context *context = g_malloc0(sizeof(Opengl_Context));
    
    // context->window = (void*)qemu_get_be64(f);

    LOGI("in load opengl context of %llx %llx", context, context->window);
    // load_native_context(f, context->window);

    context->bound_buffer_status = *load_bound_buffer(f);
    context->resource_status = *load_resource_context(f);
    context->texture_binding_status = *load_texture_binding_status(f);


    context->share_context = (void*)qemu_get_be64(f);


    context->draw_fbo0 = qemu_get_be32(f);
    // GLuint new_draw_fbo = get_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, context->draw_fbo0);
    // context->draw_fbo0 = new_draw_fbo;
    // LOGI("in load opengl context with draw_fbo0 %d %d", context->draw_fbo0, new_draw_fbo);

    context->read_fbo0 = qemu_get_be32(f);
    // GLuint new_read_fbo = get_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, context->read_fbo0);
    // context->read_fbo0 = new_read_fbo;

    context->framebuffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    load_native_framebuffers(f, context->framebuffer_map);

    context->current_read_fbo = qemu_get_be32(f); //get_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, qemu_get_be32(f));
    context->current_write_fbo = qemu_get_be32(f); //get_host_id_map(RESOURCE_TYPE_FRAMEBUFFER, qemu_get_be32(f));
    context->current_program = get_host_id_map(RESOURCE_TYPE_PROGRAM, qemu_get_be32(f));
    LOGI("in load opengl context with fbo0 %d %d %d", context->current_read_fbo, context->current_write_fbo, context->current_program);

    

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

    context->window = get_native_opengl_context(context->context_flags);

    
    // restore_opengl_context_textures(context);

    context->draw_texi_vao = qemu_get_be32(f);
    context->draw_texi_vbo = qemu_get_be32(f);
    context->draw_texi_ebo = qemu_get_be32(f);

    context->blendfunc_dfactor = qemu_get_be32(f);
    context->blendfunc_sfactor = qemu_get_be32(f);

    guint num_entries = qemu_get_be32(f);
    context->buffer_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    // g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_buffer_map_destroy); ztodo:改成这个

    LOGI("in load opengl context with num_entries %d", num_entries);
    for (guint i = 0; i < num_entries; i++) {
        guint64 buffer_key = qemu_get_be64(f);

        GLint target = (buffer_key >> 32) & ((1ll << 32) - 1);
        GLint buffer_id = buffer_key & ((1ll << 32) - 1);

        GLint new_buffer_id = get_host_id_map(RESOURCE_TYPE_BUFFER, buffer_id);

        LOGI("loading opengl buffer map of target %x old id %d new id %d", target, buffer_id, new_buffer_id);

        Guest_Host_Map *map = load_guest_host_map(f); //ztodo:还得重新map。。。。。

        g_hash_table_insert(context->buffer_map, (gpointer)((((guint64)target) << 32) + new_buffer_id), map);
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

int load_thread_unique_ids(QEMUFile *f, GHashTable *thread_unique_ids) {
    guint unique_id_count = qemu_get_be32(f);

    for (guint i = 0; i < unique_id_count; i++) {
        uint64_t unique_id = qemu_get_be64(f);
        g_hash_table_insert(thread_unique_ids, GUINT_TO_POINTER(unique_id), GUINT_TO_POINTER(1));
    }

    return 0;
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

int load_egl_display(QEMUFile *f, Egl_Display *display) {
    display->egl_config_set = g_hash_table_new(g_direct_hash, g_direct_equal);

    guint num_configs = qemu_get_be32(f);

    for (guint i = 0; i < num_configs; i++) {
        eglConfig *config = load_egl_config(f);
        g_hash_table_insert(display->egl_config_set, GUINT_TO_POINTER(config->config_id), config);
    }

    display->guest_ver_major = qemu_get_be32(f);
    display->guest_ver_minor = qemu_get_be32(f);
    display->is_init = qemu_get_be32(f);

    return 0;
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



// bool compare_two_textures(const struct Express_Native_Texture* texture1, const struct Express_Native_Texture* texture2) {
//     if (texture1 == NULL || texture2 == NULL) {
//         return false;
//     }

//     if (texture1->textureId != texture2->textureId) return false;
//     if (texture1->target != texture2->target) return false;
//     if (texture1->width != texture2->width) return false;
//     if (texture1->height != texture2->height) return false;
//     if (texture1->depth != texture2->depth) return false;
//     if (texture1->internalFormat != texture2->internalFormat) return false;

//     if (texture1->pixels && texture2->pixels) {
//         size_t size1 = texture1->width * texture1->height * 4;
//         size_t size2 = texture2->width * texture2->height * 4;
//         if (size1 != size2) {
//             return false;
//         }
//         if (memcmp(texture1->pixels, texture2->pixels, size1) != 0) {
//             return false;
//         }
//     } else if (texture1->pixels != texture2->pixels) {
//         return false;
//     }
//     return true;
// }


// bool compare_texture(Express_Native_Texture* native_texture) {
//     Express_Native_Texture* current_texture = g_malloc0(sizeof(struct Express_Native_Texture));
//     current_texture->textureId = native_texture->textureId;
//     current_texture->target = native_texture->target; //ztodo:type会change吗 

//     glBindTexture(current_texture->target, current_texture->textureId);



//     glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_WIDTH, &current_texture->width);
//     glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_HEIGHT, &current_texture->height);
//     if (current_texture->target == GL_TEXTURE_3D) {
//         glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_DEPTH, &current_texture->depth);
//     } else {
//         current_texture->depth = 0;
//     }
//     glGetTexLevelParameteriv(current_texture->target, 0, GL_TEXTURE_INTERNAL_FORMAT, &current_texture->internalFormat);

//     // glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&current_texture->binding2D);

//     // glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, (GLint *)&current_texture->bindingCubeMap);

//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_MIN_FILTER, &current_texture->minFilter);
//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_MAG_FILTER, &current_texture->magFilter);
//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_WRAP_S, &current_texture->wrapS);
//     // glGetTexParameteriv(current_texture->target, GL_TEXTURE_WRAP_T, &current_texture->wrapT); //ztodo:这些还没存
    
//     GLint size = current_texture->width * current_texture->height * 4;
//     current_texture->pixels = (GLubyte *)malloc(size); //ztodo:记得free
//     glGetTexImage(current_texture->target, 0, current_texture->internalFormat, GL_UNSIGNED_BYTE, current_texture->pixels); //ztodo:第二个、倒数第二个参数
//     return compare_two_textures(native_texture, current_texture);

// }