#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/worker/device.h"

#include "qemu/osdep.h"
#include "qemu/thread.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Worker-specific attachment logic separated from parent implementation.
// Parent provides spawning separately; worker just maps existing shared memory.

ExpressPlatformOps g_ops;
bool should_stop = false;

// Forward declaration for DEVICE_CALL handler implemented in device.c
void device_call_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                             uint32_t len, uint32_t flags, bool from_worker);

void qemu_system_killed(int signal, pid_t pid);
void qemu_system_killed(int signal, pid_t pid) {
    LOGI("subprocess qemu killed: signal=%d pid=%d", signal, pid);
}

void monitor_log(Monitor *mon, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LOGW("warning! monitoring not implemented for device: %s", fmt);
    va_end(args);
}

static void worker_ipc_read_from_guest_mem(Guest_Mem *guest, void *host, size_t start_loc, size_t length) {
    if (!guest || length == 0) return;
    uint32_t num = (uint32_t)guest->num;
    size_t head = sizeof(VsocGuestMemRWReq) + sizeof(VsocGuestMemSeg) * num;
    uint8_t reqbuf[VSOC_IPC_MAX_PAYLOAD];
    if (head > sizeof(reqbuf)) { LOGE("GMEM READ: too many segments"); return; }
    VsocGuestMemRWReq *hdr = (VsocGuestMemRWReq *)reqbuf;
    hdr->num = num;
    hdr->all_len = (uint32_t)guest->all_len;
    VsocGuestMemSeg *segs = (VsocGuestMemSeg *)(reqbuf + sizeof(VsocGuestMemRWReq));
    for (uint32_t i = 0; i < num; ++i) {
        segs[i].addr = (uint64_t)(uintptr_t)guest->scatter_data[i].data;
        segs[i].len = (uint32_t)guest->scatter_data[i].len;
    }
    size_t remaining = length;
    size_t dst_off = 0;
    while (remaining) {
        uint32_t chunk = (uint32_t)min(remaining, (size_t)VSOC_IPC_MAX_PAYLOAD);
        hdr->offset = start_loc;
        hdr->length = chunk;
        uint32_t resp_len = chunk;
        int rc = vsoc_ipc_worker_request(VSOC_IPC_TYPE_GMEM_READ, reqbuf, (uint32_t)head, (uint8_t*)host + dst_off, &resp_len, NULL, 3000);
        if (rc != 0) { LOGE("GMEM READ IPC rc=%d", rc); break; }
        dst_off += resp_len;
        start_loc += resp_len;
        remaining -= resp_len;
        if (resp_len == 0) break;
    }
}

static void worker_ipc_write_to_guest_mem(Guest_Mem *guest, void *host, size_t start_loc, size_t length) {
    if (!guest || length == 0) return;
    uint32_t num = (uint32_t)guest->num;
    size_t head = sizeof(VsocGuestMemRWReq) + sizeof(VsocGuestMemSeg) * num;
    uint8_t reqbuf[VSOC_IPC_MAX_PAYLOAD];
    VsocGuestMemRWReq *hdr = (VsocGuestMemRWReq *)reqbuf;
    hdr->num = num;
    hdr->all_len = (uint32_t)guest->all_len;
    VsocGuestMemSeg *segs = (VsocGuestMemSeg *)(reqbuf + sizeof(VsocGuestMemRWReq));
    if (head > sizeof(reqbuf)) { LOGE("GMEM WRITE: too many segments"); return; }
    for (uint32_t i = 0; i < num; ++i) { segs[i].addr = (uint64_t)(uintptr_t)guest->scatter_data[i].data; segs[i].len = (uint32_t)guest->scatter_data[i].len; }
    size_t remaining = length;
    size_t src_off = 0;
    while (remaining) {
        uint32_t chunk = (uint32_t)min(remaining, (size_t)(VSOC_IPC_MAX_PAYLOAD - head));
        if (head + chunk > sizeof(reqbuf)) { LOGE("GMEM WRITE: payload overflow"); break; }
        hdr->offset = start_loc;
        hdr->length = chunk;
        memcpy(reqbuf + head, (uint8_t*)host + src_off, chunk);
        uint32_t resp_len = 0;
        int rc = vsoc_ipc_worker_request(VSOC_IPC_TYPE_GMEM_WRITE, reqbuf, (uint32_t)(head + chunk), NULL, &resp_len, NULL, 3000);
        if (rc != 0) { LOGE("GMEM WRITE IPC rc=%d", rc); break; }
        src_off += chunk;
        start_loc += chunk;
        remaining -= chunk;
    }
}

void init_express_platform(const ExpressPlatformOps ops) {
    g_ops = ops;
    // The ops struct arrived over IPC; any function pointers inside are invalid in this process.
    // Always replace with worker-safe implementations.
    g_ops.read_from_guest_mem = worker_ipc_read_from_guest_mem;
    g_ops.write_to_guest_mem = worker_ipc_write_to_guest_mem;
    g_ops.set_express_device_irq = NULL;
    g_ops.notify_shutdown = NULL;
    g_ops.force_shutdown = NULL;

    vsoc_ipc_register_handler(VSOC_IPC_TYPE_GET_CONTEXT, get_context_ipc_handler);
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_DEVICE_CALL, device_call_ipc_handler);
}

void deinit_express_platform(void) {
    should_stop = true;
    // Consume residual messages
    express_gpu_shutdown_notify_callback();
}

bool platform_should_stop(void) {
    return should_stop;
}

// Duplicate a Guest_Mem (deep copy scatter list) for persistence beyond handler scope
Guest_Mem *duplicate_guest_mem(Guest_Mem *orig) {
    if (!orig) return NULL;
    Guest_Mem *cpy = g_malloc(sizeof(Guest_Mem));
    cpy->num = orig->num;
    cpy->all_len = orig->all_len;
    cpy->scatter_data = g_malloc(sizeof(Scatter_Data) * cpy->num);
    memcpy(cpy->scatter_data, orig->scatter_data, sizeof(Scatter_Data) * cpy->num);
    return cpy;
}

void free_duplicated_guest_mem(Guest_Mem *mem) {
    if (mem) {
        g_free(mem->scatter_data);
        g_free(mem);
    }
}

/**
 * @brief 获取直接的guest端指针，flag表示是否获取到了，返回guest端的指针，可能为NULL，因为当初传入的指针可能真的为NULL
 *
 * @param guest_mem
 * @param flag
 * @return void*
 */
void *get_direct_ptr(Guest_Mem *guest_mem, int *flag)
{
    if (likely(guest_mem->num == 1))
    {
        Scatter_Data *guest_data = guest_mem->scatter_data;
        *flag = 1;
        //这里也可能返回NULL，所以以flag来区分
        return guest_data->data;
    }
    *flag = 0;
    return NULL;
}

void *call_para_to_ptr(Call_Para para, int *need_free) {
    size_t ptr_len = 0;
    unsigned char *ptr = NULL;

    ptr_len = para.data_len;

    int null_flag = 0;
    ptr = get_direct_ptr(para.data, &null_flag);
    if (unlikely(ptr == NULL)) {
        if (ptr_len != 0 && null_flag == 0) {
            ptr = g_malloc(ptr_len);
            *need_free = 1;
            g_ops.read_from_guest_mem(para.data, ptr, 0, para.data_len);
        }
    }

    return ptr;
}

/**
 * @brief 创建一个thread_context，并根据这个context新建一个线程
 *
 * @param context 需要初始化的线程context
 */
Thread_Context *thread_context_create(uint64_t thread_id, uint64_t device_id, uint64_t len, Express_Device_Info *info)
{
    Thread_Context *context = g_malloc0(len);
    context->device_id = device_id;
    context->thread_id = thread_id;

    context->read_loc = 0;
    context->write_loc = 0;
    // context->atomic_event_lock = 0;
    context->init = 0;
    context->thread_run = 1;

    context->context_init = info->context_init;
    context->context_destroy = info->context_destroy;
    context->call_handler = info->call_handler;

//线程缓冲区事件初始化
    context->data_event = create_event(0, 0);

    char thread_name[32];
    snprintf(thread_name, sizeof(thread_name), "%s_handle_thread", info->name);

    qemu_thread_create(&context->this_thread, thread_name, handle_thread_run, context, QEMU_THREAD_JOINABLE);

    return context;
}
