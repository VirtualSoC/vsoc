#include "hw/vsoc/express_log.h"
#include "hw/vsoc/worker/device.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/express_event.h"
#include <glib.h>

GHashTable *g_devices = NULL;

// Worker context lookup handler for GET_CONTEXT
void get_context_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                                    uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)flags; (void)from_worker;
    if (len != sizeof(uint64_t)*4) {
        LOGE("GET_CONTEXT wrong len %u", len); return;
    }
    struct Req { uint64_t device_id, thread_id, process_id, unique_id; };
    const struct Req *req = (const struct Req*)data;
    Express_Device_Info *info = g_hash_table_lookup(g_devices, GINT_TO_POINTER(req->device_id));
    if (!info || !info->get_context) {
        LOGE("GET_CONTEXT: no device info or get_context not implemented for device_id=%" PRIu64, req->device_id);
        uint64_t zero = 0; 
        vsoc_ipc_worker_respond(VSOC_IPC_TYPE_GET_CONTEXT, id, &zero, sizeof(zero));
        return;
    }
    Thread_Context *ctx = info->get_context(req->device_id, req->thread_id, req->process_id, req->unique_id, info);
    uint64_t handle = (uint64_t)(uintptr_t)ctx;
    vsoc_ipc_worker_respond(VSOC_IPC_TYPE_GET_CONTEXT, id, &handle, sizeof(handle));
}

// Worker handler for DEVICE_CALL: payload layout
// [handle(uint64_t)][id(uint64_t)][para_num(int32_t)]
// For each param i: [num(uint32_t)][all_len(uint32_t)][VsocGuestMemSeg segments[num]]
typedef struct WorkerIpcCallCtx {
    GMutex m;
    GCond c;
    bool is_sync;
    bool done;
    bool result;
} WorkerIpcCallCtx;

typedef struct WorkerCall {
    uint64_t id;          // function id
    int para_num;         // number of parameters
    Call_Para *paras;     // owned array of parameters
    bool is_end;          // sentinel to end thread
    WorkerIpcCallCtx *w;  // optional sync context
} WorkerCall;

void device_call_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                             uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)flags; (void)from_worker;
    const uint8_t *p = data; const uint8_t *end = data + len;
    if (p + sizeof(uint64_t)*2 + sizeof(int32_t) > end) { LOGE("DEVICE_CALL: bad header len=%u", len); return; }
    uint64_t handle; memcpy(&handle, p, sizeof(handle)); p += sizeof(handle);
    uint64_t call_id; memcpy(&call_id, p, sizeof(call_id)); p += sizeof(call_id);
    int32_t para_num; memcpy(&para_num, p, sizeof(para_num)); p += sizeof(para_num);
    if (para_num < 0 || para_num > 1024) { LOGE("DEVICE_CALL: invalid para_num=%d", para_num); return; }

    // Reconstruct Call_Para array
    Call_Para *paras = NULL;
    if (para_num > 0) {
        paras = g_malloc0(sizeof(Call_Para) * (size_t)para_num);
    }
    for (int i = 0; i < para_num; ++i) {
        if (p + sizeof(uint32_t)*2 > end) { LOGE("DEVICE_CALL: param %d header truncated", i); goto out; }
        uint32_t num, all_len32; memcpy(&num, p, sizeof(num)); p += sizeof(num); memcpy(&all_len32, p, sizeof(all_len32)); p += sizeof(all_len32);
        size_t seg_bytes = (size_t)num * sizeof(VsocGuestMemSeg);
        if (p + seg_bytes > end) { LOGE("DEVICE_CALL: param %d segs truncated (num=%u)", i, num); goto out; }
        Guest_Mem *gm = g_malloc0(sizeof(Guest_Mem));
        gm->num = (int)num;
        gm->all_len = (int)all_len32;
        gm->scatter_data = g_malloc0(sizeof(Scatter_Data) * (size_t)num);
        const VsocGuestMemSeg *segs = (const VsocGuestMemSeg *)(const void *)p;
        for (uint32_t s = 0; s < num; ++s) {
            gm->scatter_data[s].data = (unsigned char *)(uintptr_t)segs[s].addr; // parent VA; do not deref in worker
            gm->scatter_data[s].len = segs[s].len;
        }
        p += seg_bytes;
        paras[i].data = gm;
        paras[i].data_len = gm->all_len;
    }

    // Prepare a WorkerCall and push to the device thread
    Thread_Context *ctx = (Thread_Context *)(uintptr_t)handle;
    if (!ctx) { LOGE("DEVICE_CALL: null ctx handle"); goto out; }

    WorkerIpcCallCtx *w = g_malloc0(sizeof(WorkerIpcCallCtx));
    g_mutex_init(&w->m); g_cond_init(&w->c);
    w->is_sync = FUN_NEED_SYNC(call_id);
    w->done = false; w->result = false;

    WorkerCall *wc = g_malloc0(sizeof(WorkerCall));
    wc->id = call_id;
    wc->para_num = para_num;
    wc->paras = paras; // ownership transferred to worker thread
    wc->is_end = false;
    wc->w = w;

    // Queue to the device thread
    call_push(ctx, wc);

    // For sync calls, wait for completion and respond
    if (w->is_sync) {
        g_mutex_lock(&w->m);
        while (!w->done) {
            g_cond_wait(&w->c, &w->m);
        }
        g_mutex_unlock(&w->m);
        uint8_t resp = w->result ? 1 : 0;
        (void)vsoc_ipc_worker_respond(VSOC_IPC_TYPE_DEVICE_CALL, id, &resp, sizeof(resp));
        g_cond_clear(&w->c);
        g_mutex_clear(&w->m);
        g_free(w);
        return;
    } else {
        // Async: return without responding; worker thread will free context
        return;
    }

out:
    if (paras) {
        for (int i = 0; i < para_num; ++i) {
            if (paras[i].data) {
                if (paras[i].data->scatter_data) g_free(paras[i].data->scatter_data);
                g_free(paras[i].data);
            }
        }
        g_free(paras);
    }
}

void init_express_device(const Express_Device_Info *info)
{
    if (!g_devices) {
        g_devices = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
    g_hash_table_insert(g_devices, GINT_TO_POINTER(info->device_id), info);
    LOGI("express device init %s (id=%" PRIu64 ")", info->name, info->device_id);
}

/**
 * @brief 处理线程运行函数，分发线程会分发call到这个线程，然后调用call_handler进行处理
 *
 * @param opaque
 * @return void*
 */
void *handle_thread_run(void *opaque) //初始化后运行的新qemu thread
{

    Thread_Context *context = (Thread_Context *)opaque;

    if (context->context_init != NULL)
    {
        context->context_init(context);
    }
    context->thread_run = 2;
    context->init = 1;
    while (context->thread_run)
    {
        WorkerCall *call = (WorkerCall *)call_pop(context);

        if (platform_should_stop())
        {
            break;
        }

        if (call == NULL)
        {
            continue;
        }

        if (call->is_end)
        {
            LOGD("thread context %llx call end", (uint64_t)context);
            // free the call and stop
            if (call->paras) {
                for (int i = 0; i < call->para_num; ++i) {
                    if (call->paras[i].data) {
                        if (call->paras[i].data->scatter_data) g_free(call->paras[i].data->scatter_data);
                        g_free(call->paras[i].data);
                    }
                }
                g_free(call->paras);
            }
            if (call->w && !call->w->is_sync) {
                // async sentinel should not happen, but free if present
                g_cond_clear(&call->w->c);
                g_mutex_clear(&call->w->m);
                g_free(call->w);
            }
            g_free(call);
            context->thread_run = 0;
            break;
        }

        //实际对每个call调用的操作
        if (context->call_handler != NULL)
        {
            bool success = context->call_handler(context, call->id, call->paras, call->para_num);
            // Free parameter memory ownership here
            if (call->paras) {
                for (int i = 0; i < call->para_num; ++i) {
                    if (call->paras[i].data) {
                        if (call->paras[i].data->scatter_data) g_free(call->paras[i].data->scatter_data);
                        g_free(call->paras[i].data);
                    }
                }
                g_free(call->paras);
                call->paras = NULL;
            }
            // Notify sync waiter if any
            if (call->w && call->w->is_sync) {
                g_mutex_lock(&call->w->m);
                call->w->result = success ? true : false;
                call->w->done = true;
                g_cond_signal(&call->w->c);
                g_mutex_unlock(&call->w->m);
            } else if (call->w) {
                // Async: free the wait context here
                g_cond_clear(&call->w->c);
                g_mutex_clear(&call->w->m);
                g_free(call->w);
            }
            g_free(call);
        }
    }

    delete_event(context->data_event);

    if (context->context_destroy != NULL)
    {
        context->context_destroy(context);
    }

    LOGD("handle thread exit %llu", context->thread_id);
    g_free(context);
    return NULL;
}
