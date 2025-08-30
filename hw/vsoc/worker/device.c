#include "hw/vsoc/express_log.h"
#include "hw/vsoc/worker/device.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/express_event.h"
#include <glib.h>

GHashTable *g_devices = NULL;

// Worker: device-level context (for IRQs) creation via IPC
static void get_device_context_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                                    uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)flags; (void)from_worker;
    struct Req { uint64_t device_id, thread_id, process_id, unique_id; };
    if (len != sizeof(struct Req)) { LOGE("GET_DEVICE_CONTEXT: wrong len %u", len); uint64_t zero = 0; vsoc_ipc_worker_respond(VSOC_IPC_TYPE_GET_DEVICE_CONTEXT, id, &zero, sizeof(zero)); return; }
    const struct Req *req = (const struct Req *)data;
    Express_Device_Info *info = g_hash_table_lookup(g_devices, GINT_TO_POINTER(req->device_id));
    if (!info || !info->get_device_context) {
        LOGE("GET_DEVICE_CONTEXT: no device info or get_device_context not implemented for device_id=%" PRIu64, req->device_id);
        uint64_t zero = 0; vsoc_ipc_worker_respond(VSOC_IPC_TYPE_GET_DEVICE_CONTEXT, id, &zero, sizeof(zero)); return;
    }
    Device_Context *dc = info->get_device_context(req->device_id, req->thread_id, req->process_id, req->unique_id, info);
    uint64_t handle = (uint64_t)(uintptr_t)dc;
    vsoc_ipc_worker_respond(VSOC_IPC_TYPE_GET_DEVICE_CONTEXT, id, &handle, sizeof(handle));
}

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

// BUFFER_REGISTER handler: payload [device_id(8)][thread_id(8)][process_id(8)][unique_id(8)][num(4)][all_len(4)] + segs
void buffer_register_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                                 uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)flags; (void)from_worker;
    if (len < (int)(sizeof(uint64_t)*4 + sizeof(uint32_t)*2)) { LOGE("BUFFER_REGISTER: short header len=%u", len); return; }
    const uint8_t *p = data; const uint8_t *end = data + len;
    uint64_t device_id, thread_id, process_id, unique_id;
    memcpy(&device_id, p, 8); p += 8;
    memcpy(&thread_id, p, 8); p += 8;
    memcpy(&process_id, p, 8); p += 8;
    memcpy(&unique_id, p, 8); p += 8;
    size_t consumed = 0; Guest_Mem *gm = NULL;
    if (!vsoc_ipc_guest_mem_unpack(p, (size_t)(end - p), &gm, &consumed)) { LOGE("BUFFER_REGISTER: unpack failed"); return; }
    p += consumed;
    // Route to the specific device's buffer_register if present
    Express_Device_Info *info = g_devices ? g_hash_table_lookup(g_devices, GINT_TO_POINTER(device_id)) : NULL;
    if (!info || !info->buffer_register) {
        LOGE("BUFFER_REGISTER: no device handler for device_id=%" PRIu64, device_id);
        // Free allocated Guest_Mem and return
        if (gm->scatter_data) g_free(gm->scatter_data);
        g_free(gm);
        return;
    }
    info->buffer_register(gm, thread_id, process_id, unique_id, info);
    // buffer_register ownership rules: gm was allocated here; free after handler returns
    if (gm) {
        if (gm->scatter_data) g_free(gm->scatter_data);
        g_free(gm);
    }
    // No response is required for fire-and-forget; parent uses send, not request
}

typedef struct WorkerCall {
    uint64_t id;          // function id
    int para_num;         // number of parameters
    Call_Para *paras;     // owned array of parameters
    bool is_end;          // sentinel to end thread
    // IPC reply coordination (reply will be sent from device thread, not IPC handler)
    uint32_t ipc_slot_id; // IPC slot id to reply to
    bool need_reply;      // whether to reply to parent upon completion
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
        size_t consumed_i = 0; Guest_Mem *gm = NULL;
        if (!vsoc_ipc_guest_mem_unpack(p, (size_t)(end - p), &gm, &consumed_i)) { LOGE("DEVICE_CALL: param %d unpack failed", i); goto out; }
        p += consumed_i;
        paras[i].data = gm;
        paras[i].data_len = gm ? (size_t)gm->all_len : 0;
    }

    // Prepare a WorkerCall and push to the device thread
    Thread_Context *ctx = (Thread_Context *)(uintptr_t)handle;
    if (!ctx) { LOGE("DEVICE_CALL: null ctx handle"); goto out; }

    WorkerCall *wc = g_malloc0(sizeof(WorkerCall));
    wc->id = call_id;
    wc->para_num = para_num;
    wc->paras = paras; // ownership transferred to worker thread
    wc->is_end = false;
    wc->ipc_slot_id = id; // remember the slot id to reply later
    wc->need_reply = true; // always reply from worker thread when done

    // Queue to the device thread
    call_push(ctx, wc);

    // Return immediately; device thread will send response (for both sync and async)
    return;

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
    LOGD("express device init %s (id=%" PRIu64 ")", info->name, info->device_id);

    // Ensure handler is registered once
    static bool buffer_reg_handler_registered = false;
    if (!buffer_reg_handler_registered) {
        vsoc_ipc_register_handler(VSOC_IPC_TYPE_BUFFER_REGISTER, buffer_register_ipc_handler);
        buffer_reg_handler_registered = true;
    }

    // Ensure GET_DEVICE_CONTEXT handler registered once
    static bool get_dev_ctx_handler_registered = false;
    if (!get_dev_ctx_handler_registered) {
        vsoc_ipc_register_handler(VSOC_IPC_TYPE_GET_DEVICE_CONTEXT, get_device_context_ipc_handler);
        get_dev_ctx_handler_registered = true;
    }
}

bool invoke_call_handler(Thread_Context *context, void *_call) {
    WorkerCall *call = (WorkerCall *)_call;
    bool success = false;
    if (context->call_handler != NULL)
    {
        LOGD("proxy_call_handler: worker_handle=%" PRIx64 " id=%" PRIu64 " para_num=%d sync=%s", context, GET_FUN_ID(call->id), call->para_num, FUN_NEED_SYNC(call->id) ? "true" : "false");

        success = context->call_handler(context, call->id, call->paras, call->para_num);
    }
    // Free parameter memory ownership here
    if (call->paras) {
        for (int i = 0; i < call->para_num; ++i) {
            if (call->paras[i].data) {
                free_duplicated_guest_mem(call->paras[i].data);
            }
        }
        g_free(call->paras);
        call->paras = NULL;
    }
    // Send IPC reply to parent now that processing is complete
    if (call->need_reply) {
        uint8_t resp = success ? 1 : 0;
        (void)vsoc_ipc_worker_respond(VSOC_IPC_TYPE_DEVICE_CALL, call->ipc_slot_id, &resp, sizeof(resp));
    }
    g_free(call);
    return success;
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
            g_free(call);
            context->thread_run = 0;
            break;
        }
        invoke_call_handler(context, call);
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
