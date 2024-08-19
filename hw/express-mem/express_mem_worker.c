/**
 * express-mem worker thread
 * 
 * Copyright (c) 2023 Jiaxing Qiu <jx.qiu@outlook.com>
 * 
 * dma memory transfer is mostly delegated to mem worker threads
 * to avoid blocking on device threads
 */

// #define STD_DEBUG_LOG
#include "hw/express-mem/express_mem.h"
#include "hw/express-mem/express_sync.h"
#include "hw/express-gpu/glv3_context.h"

static __thread void *g_gl_context = NULL;
static __thread GLuint g_unpack_buffer = 0;
static __thread int g_unpack_buffer_size = 0;
static __thread GLsync g_unpack_buffer_sync = NULL;

static void init_worker_gl_context() {
    g_gl_context = get_native_opengl_context(0);
    egl_makeCurrent(g_gl_context);

    glGenBuffers(1, &g_unpack_buffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, g_unpack_buffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_unpack_buffer);
}

static void *begin_dma_to_gbuffer(int map_size) {
    map_size += 1000;
    if (g_gl_context == NULL) {
        init_worker_gl_context();
    }

    GLint sync_status = GL_SIGNALED;
    if (g_unpack_buffer_sync)
    {
        glGetSynciv(g_unpack_buffer_sync, GL_SYNC_STATUS, sizeof(GLint), NULL, &sync_status);
        glDeleteSync(g_unpack_buffer_sync);
        g_unpack_buffer_sync = NULL;
    }

    // 因为通过map上传的过程为异步的，所以这里假如fence未完成的话，需要重新bufferdata，以实现缓冲区孤立，避免同步（即避免需要同步等待gl用完这个缓冲区)
    if (sync_status != GL_SIGNALED || g_unpack_buffer_size < map_size)
    {
        glBufferData(GL_PIXEL_UNPACK_BUFFER, map_size, NULL, GL_STREAM_DRAW);
        g_unpack_buffer_size = map_size;
    }

    // mmap.
    return glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, map_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

static void end_dma_to_gbuffer(Hardware_Buffer *gbuffer) {
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    glBindTexture(GL_TEXTURE_2D, gbuffer->data_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, gbuffer->internal_format, gbuffer->width, gbuffer->height, 0, gbuffer->format, gbuffer->pixel_type, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    g_unpack_buffer_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();

#ifdef STD_DEBUG_LOG
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("gl error %x!", error);
    }
#endif
}

/**
 * internal worker function registered in the thread pool
*/
void express_mem_worker(gpointer data, gpointer user_data) {
    MemTransferTask *task = data;
    int ret = 0;

    if (task == NULL) {
        LOGE("task is null!");
        ret = -1;
        goto EXIT;
    }
    if ((task->src_dev == EXPRESS_MEM_TYPE_HOST_OPAQUE || task->dst_dev == EXPRESS_MEM_TYPE_HOST_OPAQUE 
      || task->src_dev == EXPRESS_MEM_TYPE_GUEST_OPAQUE || task->dst_dev == EXPRESS_MEM_TYPE_GUEST_OPAQUE)
        && !task->pre_cb) {
        // opaque memory cannot be transfered without custom code 
        LOGE("opaque memory type supplied, but no pre_cb is present");
        ret = -1;
        goto EXIT;
    }

    int64_t start_time = g_get_real_time();

    void *mapped_addr = NULL;
    if (task->dst_dev == EXPRESS_MEM_TYPE_TEXTURE) {
        mapped_addr = begin_dma_to_gbuffer(task->dst_len);
        if (!mapped_addr) {
            LOGE("failed to map gbuffer!");
            ret = -1;
            goto EXIT;
        }
    }
    else if (task->dst_dev == EXPRESS_MEM_TYPE_GUEST_MEM) {
        mapped_addr = g_malloc(task->dst_len);
        if (!mapped_addr) {
            LOGE("failed to map guest mem!");
            ret = -1;
            goto EXIT;
        }
    }

    if (task->pre_cb)
        task->pre_cb(task, mapped_addr);

    switch (task->dst_dev) {
        case EXPRESS_MEM_TYPE_TEXTURE: {
            end_dma_to_gbuffer((Hardware_Buffer *)task->dst_data);
            ((Hardware_Buffer *)task->dst_data)->pref_phy_dev = EXPRESS_MEM_TYPE_TEXTURE;
        } break;
        case EXPRESS_MEM_TYPE_GBUFFER_HOST_MEM: {
            end_dma_to_gbuffer((Hardware_Buffer *)task->dst_data);
            ((Hardware_Buffer *)task->dst_data)->pref_phy_dev = EXPRESS_MEM_TYPE_GBUFFER_HOST_MEM;
        } break;
        case EXPRESS_MEM_TYPE_GUEST_MEM: {
            write_to_guest_mem((Guest_Mem *)task->dst_data, mapped_addr, 0, task->dst_len);
            g_free(mapped_addr);
        }
        case EXPRESS_MEM_TYPE_GUEST_OPAQUE:
        case EXPRESS_MEM_TYPE_HOST_OPAQUE: {
            // nop
        } break;
        default: {
            LOGD("worker: dst_dev %s not supported!", memtype_to_str(task->dst_dev));
            ret = -1;
        }
    }
    if (task->sync_id > 0) {
        signal_express_sync(task->sync_id, task->src_dev == EXPRESS_MEM_TYPE_TEXTURE || task->dst_dev == EXPRESS_MEM_TYPE_TEXTURE);
    }

EXIT:
    if (task->post_cb) {
        task->post_cb(task, ret);
    }

    int64_t end_time = g_get_real_time();

    if (ret >= 0 && end_time - start_time > 0) {
        // make sure that the results are meaningful
        hg_update_bandwidth(task->dst_dev, task->src_dev, (double)task->dst_len / (end_time - start_time));
        // LOGI("update bandwidth (%s -> %s) end time %llx start_time %llx dst_len %d", memtype_to_str(task->src_dev), memtype_to_str(task->dst_dev), end_time, start_time, task->dst_len);
    }

    g_free(task);
}
