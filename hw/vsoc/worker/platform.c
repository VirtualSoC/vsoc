#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/worker/device.h"
#include "hw/vsoc/worker/guestmem.h"

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
VsocIpcContext *g_ipc_ctx = NULL;

void qemu_system_killed(int signal, pid_t pid);
void qemu_system_killed(int signal, pid_t pid) {
    LOGI("qemu_system_killed not implemented: signal=%d pid=%d", signal, pid);
}

void monitor_log(Monitor *mon, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LOGW("warning! monitoring not implemented for device: %s", fmt);
    va_end(args);
}

// Forward display shutdown events to parent via IPC. Parent will invoke its own g_ops.* hooks.
static void worker_notify_shutdown_impl(void) {
    (void)vsoc_ipc_send(g_ipc_ctx, VSOC_IPC_TYPE_NOTIFY_SHUTDOWN, 0, NULL, 0);
}

static void worker_force_shutdown_impl(int reason) {
    int32_t r = (int32_t)reason;

    (void)vsoc_ipc_send(g_ipc_ctx, VSOC_IPC_TYPE_FORCE_SHUTDOWN, 0, &r, sizeof(r));
    should_stop = true;
}

static void worker_read_from_guest_mem(Guest_Mem *guest, void *host, size_t start_loc, size_t length) {
    if (!guest || !host || length == 0) return;
    if (start_loc >= guest->all_len) return;
    size_t remaining = length;
    size_t copied = 0;
    size_t off = start_loc;
    bool is_inline = !guest->is_gpa;
    for (uint32_t i = 0; i < guest->num && remaining; ++i) {
    uint64_t seg_len = (uint64_t)guest->scatter_data[i].iov_len;
        if (off >= seg_len) { off -= seg_len; continue; }
    void *seg_ptr = guest->scatter_data[i].iov_base;
        uint64_t gpa = is_inline ? 0 : (uint64_t)(uintptr_t)seg_ptr;
        size_t seg_avail = (size_t)(seg_len - off);
        size_t chunk = seg_avail < remaining ? seg_avail : remaining;
        if (is_inline) {
            memcpy((uint8_t*)host + copied, (uint8_t*)seg_ptr + off, chunk);
        } else {
            if (!guestmem_read(gpa + off, (uint8_t*)host + copied, chunk)) {
                LOGE("guestmem_read failed at seg %u gpa=%#llx off=%zu len=%zu", i, (unsigned long long)gpa, off, chunk);
                return;
            }
        }
        copied += chunk;
        remaining -= chunk;
        off = 0; // after first segment consumption, subsequent segments start at 0
    }
}

static void worker_write_to_guest_mem(Guest_Mem *guest, void *host, size_t start_loc, size_t length) {
    if (!guest || !host || length == 0) return;
    if (start_loc >= guest->all_len) return;
    size_t remaining = length;
    size_t written = 0;
    size_t off = start_loc;
    bool is_inline = !guest->is_gpa;
    if (is_inline) {
        LOGW("attempt to write into read-only inline buffer; dropping write len=%zu", length);
        return;
    }
    for (uint32_t i = 0; i < guest->num && remaining; ++i) {
        uint64_t seg_len = (uint64_t)guest->scatter_data[i].iov_len;
        if (off >= seg_len) { off -= seg_len; continue; }
        void *seg_ptr = guest->scatter_data[i].iov_base;
        uint64_t gpa = (uint64_t)(uintptr_t)seg_ptr;
        size_t seg_avail = (size_t)(seg_len - off);
        size_t chunk = seg_avail < remaining ? seg_avail : remaining;
        if (!guestmem_write(gpa + off, (uint8_t*)host + written, chunk)) {
            LOGE("guestmem_write failed at seg %u gpa=%#llx off=%zu len=%zu", i, (unsigned long long)gpa, off, chunk);
            return;
        }
        written += chunk;
        remaining -= chunk;
        off = 0;
    }
}

// Forward set_express_device_irq to parent via IPC and return status
static int worker_set_express_device_irq(Device_Context *device_context, int buf_index, int len) {
    if (!device_context) return IRQ_NOT_READY;
    struct __attribute__((packed)) Req {
        uint64_t parent_handle; // parent-side Device_Context* value
        int32_t buf_index;
        int32_t len;
    } req;
    uint64_t ph = worker_get_parent_handle_for_dc(device_context);
    req.parent_handle = ph;
    req.buf_index = buf_index;
    req.len = len;

    int32_t resp = IRQ_SET_OK; uint32_t resp_len = sizeof(resp);
    int rc = vsoc_ipc_request(g_ipc_ctx, VSOC_IPC_TYPE_SET_IRQ, &req, sizeof(req), &resp, &resp_len, NULL, 3000);
    if (rc != 0 || resp_len != sizeof(resp)) {
        LOGE("worker_set_express_device_irq: request failed rc=%d resp_len=%u", rc, resp_len);
        return IRQ_NOT_READY;
    }
    return resp;
}

void init_express_platform(const ExpressPlatformOps ops) {
    g_ops = ops;
    g_ops.read_from_guest_mem = worker_read_from_guest_mem;
    g_ops.write_to_guest_mem = worker_write_to_guest_mem;
    // Forward IRQ requests back to QEMU via IPC
    g_ops.set_express_device_irq = worker_set_express_device_irq;
    g_ops.notify_shutdown = worker_notify_shutdown_impl;
    g_ops.force_shutdown = worker_force_shutdown_impl;

    vsoc_ipc_register_handler(VSOC_IPC_TYPE_GET_CONTEXT, get_context_ipc_handler);
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_DEVICE_CALL, device_call_ipc_handler);

    call_device_init();
}

void deinit_express_platform(void) {
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
        if (mem->scatter_data) {
            g_free(mem->scatter_data);
        }
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
    if (!guest_mem || guest_mem->num <= 0) {
        if (flag) *flag = 0;
        return NULL;
    }
    if (likely(guest_mem->num == 1) && guest_mem->is_gpa) {
    Scatter_Data *guest_data = guest_mem->scatter_data;
    uint64_t gpa = (uint64_t)(uintptr_t)guest_data->iov_base;
    size_t len = guest_data->iov_len;
        // If guest provided a real NULL pointer, keep the old semantics: flag=1, return NULL
        if (gpa == 0) { if (flag) *flag = 1; return NULL; }
        size_t contig = 0;
        void *ptr = guestmem_ptr(gpa, len, &contig);
        if (ptr && contig >= len) { if (flag) *flag = 1; return ptr; }
        // Fall back to copy path if not fully contiguous
    }
    if (flag) *flag = 0;
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
