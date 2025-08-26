// middleware layer between the teleport-express framework and vsoc virtual devices
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/express_device.h"
#include "hw/vsoc/teleport_express_call.h"

#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <glib.h>

#include "exec/cpu-common.h"
#include "exec/ramblock.h"

// Shared memory structures now declared in header; define global pointer here.
ExpressPlatformOps g_ops;
static pid_t g_worker_pid = -1;
static char g_shm_name[64];

static bool worker_started = false;
static bool should_stop = false;
static QemuThread parent_ipc_thread;
static bool parent_ipc_thread_started = false;
// Gate to pause background polling while sync requests are in flight to avoid racing responses
static gint g_ipc_block_bg_poll = 0;

static GHashTable *g_proxy_info = NULL;
static GHashTable *g_worker_handle_by_ctx = NULL; // key: Thread_Context*, value: (gpointer)worker_handle
static GHashTable *g_orig_info = NULL;   // key: device_id (GINT_TO_POINTER), value: original Express_Device_Info*
// Reverse map for device IRQs: key worker_handle(uint64) -> value Device_Context*
static GHashTable *g_local_dc_by_worker_handle = NULL;

static Guest_Mem *convert_guest_mem_to_gpa(Guest_Mem *mem);

static void *map_shared_memory_parent(size_t size) {
    snprintf(g_shm_name, sizeof(g_shm_name), "/vsoc_ipc_%d", (int)getpid());
    int fd = shm_open(g_shm_name, O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        LOGE("shm_open parent failed: %s", strerror(errno));
        return NULL;
    }
    if (ftruncate(fd, (off_t)size) != 0) {
        LOGE("ftruncate failed: %s", strerror(errno));
        close(fd);
        return NULL;
    }
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == MAP_FAILED) {
        LOGE("mmap parent failed: %s", strerror(errno));
        return NULL;
    }
    return addr;
}

// Reader threads to capture worker stdout/stderr and log via QEMU's logger
typedef struct WorkerLogArg {
    int fd;
    bool is_err;
} WorkerLogArg;

static QemuThread g_worker_out_thread;
static QemuThread g_worker_err_thread;
static bool g_worker_out_thread_started = false;
static bool g_worker_err_thread_started = false;
static int g_worker_stdout_fd = -1;
static int g_worker_stderr_fd = -1;

static void *worker_log_reader(void *opaque) {
    WorkerLogArg *arg = (WorkerLogArg *)opaque;
    int fd = arg->fd;
    char buf[4096];
    char line[8192];
    size_t linelen = 0;
    for (;;) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n <= 0) break; // EOF or error
        for (ssize_t i = 0; i < n; ++i) {
            if (linelen < sizeof(line) - 1) {
                line[linelen++] = buf[i];
            }
            if (buf[i] == '\n') {
                line[linelen] = '\0';
                printf("[worker] %s", line);
                linelen = 0;
            }
        }
    }
    if (linelen) {
        line[linelen] = '\0';
        printf("[worker] %s", line);
    }
    close(fd);
    g_free(arg);
    return NULL;
}

static void spawn_worker_process(void) {
    if (worker_started) {
        return; // already spawned
    }
    size_t shm_size = sizeof(VsocGpuIpcShared);
    vsoc_ipc_shared = (VsocGpuIpcShared*)map_shared_memory_parent(shm_size);
    if (!vsoc_ipc_shared) {
        LOGE("failed to create shared memory; worker not spawned");
        return;
    }
    vsoc_ipc_shared->parent_ready = 1;
    vsoc_ipc_shared->worker_ready = 0;
    vsoc_ipc_shared->pw_head = vsoc_ipc_shared->pw_tail = 0;
    vsoc_ipc_shared->wp_head = vsoc_ipc_shared->wp_tail = 0;

    // Launch vsoc-worker directly and capture stdout/stderr via pipes
    GError *error = NULL;
    gboolean ok = FALSE;
    const char *worker_path = getenv("VSOC_WORKER_PATH");
    if (!worker_path || !*worker_path) worker_path = "vsoc-worker";
    LOGI("launching worker: %s %s", worker_path, g_shm_name);
    gchar *argv_spawn[] = { (gchar*)worker_path, (gchar*)g_shm_name, NULL };
    int child_stdin = -1, child_stdout = -1, child_stderr = -1;
    ok = g_spawn_async_with_pipes(
        NULL,
        argv_spawn,
        NULL,
        G_SPAWN_SEARCH_PATH | G_SPAWN_LEAVE_DESCRIPTORS_OPEN,
        NULL,
        NULL,
        &g_worker_pid,
        &child_stdin,
        &child_stdout,
        &child_stderr,
        &error);
    if (!ok) {
        if (error) { LOGE("failed to launch vsoc-worker: %s", error->message); g_clear_error(&error); }
        else { LOGE("failed to launch vsoc-worker (unknown error)"); }
        return;
    }
    // Close child's stdin (unused) and start reader threads for stdout/stderr
    if (child_stdin >= 0) close(child_stdin);
    g_worker_stdout_fd = child_stdout;
    g_worker_stderr_fd = child_stderr;
    WorkerLogArg *out_arg = g_new0(WorkerLogArg, 1); out_arg->fd = g_worker_stdout_fd; out_arg->is_err = false;
    WorkerLogArg *err_arg = g_new0(WorkerLogArg, 1); err_arg->fd = g_worker_stderr_fd; err_arg->is_err = true;
    qemu_thread_create(&g_worker_out_thread, "vsoc-worker-out", worker_log_reader, out_arg, QEMU_THREAD_JOINABLE);
    qemu_thread_create(&g_worker_err_thread, "vsoc-worker-err", worker_log_reader, err_arg, QEMU_THREAD_JOINABLE);
    g_worker_out_thread_started = g_worker_err_thread_started = true;

    worker_started = true;
    LOGI("spawned vsoc-worker pid %d shm %s", (int)g_worker_pid, g_shm_name);
}

// Build and send RAM region metadata using inherited FDs
typedef struct RamRegionMeta {
    int fd;
    uint32_t pad; // keep 8-byte alignment
    uint64_t gpa_base;
    uint64_t size;
    uint64_t offset;
} RamRegionMeta;

typedef struct RamRegionList {
    GArray *arr; // array of RamRegionMeta
} RamRegionList;

static void ensure_fd_inherited(int fd) {
    int flags = fcntl(fd, F_GETFD);
    if (flags >= 0 && (flags & FD_CLOEXEC)) {
        (void)fcntl(fd, F_SETFD, flags & ~FD_CLOEXEC);
    }
}

static int collect_block_cb(RAMBlock *rb, void *opaque) {
    RamRegionList *list = (RamRegionList *)opaque;
    if (!qemu_ram_is_shared(rb) || rb->fd < 0) return 0;
    RamRegionMeta m = {0};
    m.fd = rb->fd; m.pad = 0;
    m.gpa_base = 0; // (uint64_t)qemu_ram_get_offset(rb);
    m.size = (uint64_t)qemu_ram_get_used_length(rb);
    m.offset = 0;
    ensure_fd_inherited(m.fd);
    g_array_append_val(list->arr, m);
    LOGI("prepared RAM memfd for %s fd=%d gpa=%#llx size=%#llx off=%#llx rb_off=%#llx", qemu_ram_get_idstr(rb), m.fd, (unsigned long long)m.gpa_base, (unsigned long long)m.size, (unsigned long long)m.offset, (unsigned long long)rb->offset);
    return 0;
}

static void send_ram_regions_to_worker(void) {
    RamRegionList list = { .arr = g_array_new(FALSE, TRUE, sizeof(RamRegionMeta)) };
    qemu_ram_foreach_block(collect_block_cb, &list);
    uint32_t count = (uint32_t)list.arr->len;
    size_t payload_sz = sizeof(count) + count * sizeof(RamRegionMeta);
    uint8_t *payload = g_malloc(payload_sz);
    memcpy(payload, &count, sizeof(count));
    memcpy(payload + sizeof(count), list.arr->data, count * sizeof(RamRegionMeta));
    bool ok = vsoc_ipc_parent_send(VSOC_IPC_TYPE_RAM_REGIONS, 0, payload, (uint32_t)payload_sz, 0);
    if (!ok) LOGE("failed to send RAM_REGIONS metadata");
    g_free(payload);
    g_array_free(list.arr, TRUE);
}

// Handle SET_IRQ forwarded from worker: payload is struct Req; respond with int32 status
static void set_irq_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                               uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)flags; (void)from_worker;
    struct Req {
        uint64_t worker_handle; // worker Device_Context* value
        int32_t buf_index;
        int32_t len;
    } req;
    if (len != sizeof(req)) {
        LOGE("SET_IRQ: bad len %u expected %zu", len, sizeof(req));
        int32_t st = IRQ_NOT_READY; (void)vsoc_ipc_parent_send(VSOC_IPC_TYPE_SET_IRQ, id, &st, sizeof(st), VSOC_IPC_FLAG_RESPONSE);
        return;
    }
    memcpy(&req, data, sizeof(req));
    int32_t status = IRQ_NOT_READY;
    if (g_ops.set_express_device_irq && g_local_dc_by_worker_handle) {
        Device_Context *dc = g_hash_table_lookup(g_local_dc_by_worker_handle, (gpointer)(uintptr_t)req.worker_handle);
        if (!dc) {
            LOGE("SET_IRQ: unknown worker_handle=%" PRIx64, req.worker_handle);
        } else {
            status = g_ops.set_express_device_irq(dc, req.buf_index, req.len);
        }
    } else {
        LOGE("SET_IRQ: no IRQ impl or mapping table missing");
    }
    (void)vsoc_ipc_parent_send(VSOC_IPC_TYPE_SET_IRQ, id, &status, sizeof(status), VSOC_IPC_FLAG_RESPONSE);
}

void init_express_platform(const ExpressPlatformOps ops) {
    g_ops = ops;
    // Ensure memfd FDs won't be closed on exec
    {
        RamRegionList list = { .arr = g_array_new(FALSE, TRUE, sizeof(RamRegionMeta)) };
        qemu_ram_foreach_block(collect_block_cb, &list);
        // We don't send here; just ensure CLOEXEC cleared before spawn.
        g_array_free(list.arr, TRUE);
    }
    spawn_worker_process();
    // Wait for worker to attach shared memory (sets worker_ready)
    if (vsoc_ipc_shared) {
        int waited_ms = 0;
        while (!vsoc_ipc_shared->worker_ready && waited_ms < 3000) {
            g_usleep(1000); // 1ms
            waited_ms++;
        }
        if (!vsoc_ipc_shared->worker_ready) {
            LOGE("worker did not attach shared memory in time; proceeding anyway");
        } else {
            LOGI("worker_ready observed after %d ms", waited_ms);
        }
    }
    // Start dedicated IPC polling thread (parent only)
    if (!parent_ipc_thread_started) {
        void *parent_poll_thread(void *opaque) {
            (void)opaque;
            while (!should_stop) {
                if (g_atomic_int_get(&g_ipc_block_bg_poll) == 0) {
                    vsoc_ipc_poll_parent();
                }
                g_usleep(1000); // 1ms poll interval (tunable)
            }
            // final drain
            vsoc_ipc_poll_parent();
            return NULL;
        }
        qemu_thread_create(&parent_ipc_thread, "vsoc-ipc-poll", parent_poll_thread, NULL, QEMU_THREAD_JOINABLE);
        parent_ipc_thread_started = true;
    }

    // Register IRQ forwarding handler
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_SET_IRQ, set_irq_ipc_handler);
    vsoc_ipc_parent_send(VSOC_IPC_TYPE_PLATFORM_INIT, 0, &ops, sizeof(ops), 0);
    // After platform init, inform worker of RAM regions (FDs are already inherited)
    send_ram_regions_to_worker();
}

static void proxy_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, Express_Device_Info *info) {
    // 1) Send to worker with device_id for correct routing
    uint64_t device_id = (uint64_t)info->device_id;
    uint8_t buf[VSOC_IPC_MAX_PAYLOAD];
    uint8_t *p = buf; uint8_t *end = buf + sizeof(buf);
    // Header: [device_id(8)][thread_id(8)][process_id(8)][unique_id(8)] then packed Guest_Mem
    if (p + sizeof(uint64_t)*4 > end) {
        LOGE("proxy_buffer_register: header overflow");
        // still fall through to local call below
    } else {
        memcpy(p, &device_id, sizeof(device_id)); p += sizeof(device_id);
        memcpy(p, &thread_id, sizeof(thread_id)); p += sizeof(thread_id);
        memcpy(p, &process_id, sizeof(process_id)); p += sizeof(process_id);
        memcpy(p, &unique_id, sizeof(unique_id)); p += sizeof(unique_id);
    data = convert_guest_mem_to_gpa(data);
    size_t wrote = vsoc_ipc_guest_mem_pack(p, (size_t)(end - p), data);
    free_duplicated_guest_mem(data);
        if (wrote == 0) {
            LOGE("proxy_buffer_register: pack overflow");
        } else {
            p += wrote;
            uint32_t payload_len = (uint32_t)(p - buf);
            bool ok = vsoc_ipc_parent_send(VSOC_IPC_TYPE_BUFFER_REGISTER, 0, buf, payload_len, 0);
            if (!ok) LOGE("proxy_buffer_register: send failed");
        }
    }

    // // 2) Always call original buffer_register locally (sideloaded worker scenario)
    // Express_Device_Info *orig = g_orig_info ? g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(info->device_id)) : NULL;
    // if (orig && orig->buffer_register) {
    //     orig->buffer_register(data, thread_id, process_id, unique_id, orig);
    // }
}

static Device_Context *proxy_get_device_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, Express_Device_Info *info) {
    // 1) Ask worker to create its Device_Context and return a handle
    struct Req { uint64_t device_id, thread_id, process_id, unique_id; } req = { device_id, thread_id, process_id, unique_id };
    uint64_t worker_handle = 0; uint32_t resp_len = sizeof(worker_handle);
    g_atomic_int_inc(&g_ipc_block_bg_poll);
    int rc = vsoc_ipc_parent_request(VSOC_IPC_TYPE_GET_DEVICE_CONTEXT, &req, sizeof(req), &worker_handle, &resp_len, NULL, 3000);
    g_atomic_int_dec_and_test(&g_ipc_block_bg_poll);
    if (rc != 0 || resp_len != sizeof(worker_handle) || worker_handle == 0) {
        LOGE("proxy_get_device_context: worker request failed rc=%d len=%u handle=%" PRIx64, rc, resp_len, worker_handle);
        // Continue to create local context anyway; we need it to process IRQs
    }

    // 2) Create a local Device_Context in QEMU so IRQ routing works locally
    Express_Device_Info *orig = g_orig_info ? g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(device_id)) : NULL;
    if (!orig || !orig->get_device_context) {
        LOGE("proxy_get_device_context: missing original get_device_context for device_id=%" PRIu64, device_id);
        return NULL;
    }
    Device_Context *dc = orig->get_device_context(device_id, thread_id, process_id, unique_id, orig);
    if (!dc) return NULL;

    // 3) Map worker handle -> local Device_Context for future IRQ routing
    if (worker_handle && g_local_dc_by_worker_handle) {
        g_hash_table_insert(g_local_dc_by_worker_handle, (gpointer)(uintptr_t)worker_handle, dc);
    }

    return dc;
}
static bool proxy_call_handler(struct Thread_Context *context, uint64_t id, const Call_Para *all_para, int para_num) {
    Express_Device_Info *orig = g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(context->device_id));
    if (!orig || !orig->call_handler) {
        LOGE("proxy_call_handler: no original device info or call_handler for device %" PRIu64, context->device_id);
        goto fallback;
    }

    // If we don't have a worker handle for this context, fallback to local
    uint64_t worker_handle = (uint64_t)(uintptr_t)g_hash_table_lookup(g_worker_handle_by_ctx, context);
    if (worker_handle == 0) {
        LOGE("no worker handle for context %p", context);
        goto fallback;
    }

    // Build DEVICE_CALL payload: [handle(8)][id(8)][para_num(4)] + per-param packed Guest_Mem
    uint8_t buf[VSOC_IPC_MAX_PAYLOAD];
    uint8_t *p = buf; uint8_t *end = buf + sizeof(buf);
    if (p + sizeof(uint64_t)*2 + sizeof(int32_t) > end) {
        LOGE("proxy_call_handler: header overflow");
        goto fallback;
    }
    memcpy(p, &worker_handle, sizeof(worker_handle)); p += sizeof(worker_handle);
    memcpy(p, &id, sizeof(id)); p += sizeof(id);
    int32_t pn = para_num; memcpy(p, &pn, sizeof(pn)); p += sizeof(pn);

    for (int i = 0; i < para_num; ++i) {
        const Call_Para *cp = &all_para[i];
    Guest_Mem *data = convert_guest_mem_to_gpa(cp->data);
    size_t wrote = vsoc_ipc_guest_mem_pack(p, (size_t)(end - p), data);
    free_duplicated_guest_mem(data);
        if (wrote == 0) { LOGE("proxy_call_handler: param %d pack overflow", i); goto fallback; }
        p += wrote;
    }

    uint32_t payload_len = (uint32_t)(p - buf);
    if (FUN_NEED_SYNC(id)) {
        uint8_t resp = 0; uint32_t resp_len = sizeof(resp);
        g_atomic_int_inc(&g_ipc_block_bg_poll);
        int rc = vsoc_ipc_parent_request(VSOC_IPC_TYPE_DEVICE_CALL, buf, payload_len, &resp, &resp_len, NULL, 10000);
        g_atomic_int_dec_and_test(&g_ipc_block_bg_poll);
        if (rc != 0 || resp_len != sizeof(resp)) {
            LOGE("proxy_call_handler: request rc=%d resp_len=%u", rc, resp_len);
            goto fallback;
        }
    } else {
        bool ok = vsoc_ipc_parent_send(VSOC_IPC_TYPE_DEVICE_CALL, 0, buf, payload_len, 0);
        if (!ok) {
            LOGE("proxy_call_handler: async send failed");
            goto fallback;
        }
    }
    return true;

fallback:
    // return orig->call_handler(context, id, all_para, para_num);
    return false;
}

Thread_Context *proxy_get_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, Express_Device_Info *info) {
    // request structure
    struct Req { uint64_t device_id, thread_id, process_id, unique_id; } req = { device_id, thread_id, process_id, unique_id };
    uint64_t handle = 0; uint32_t resp_len = sizeof(handle);
    g_atomic_int_inc(&g_ipc_block_bg_poll);
    int rc = vsoc_ipc_parent_request(VSOC_IPC_TYPE_GET_CONTEXT, &req, sizeof(req), &handle, &resp_len, NULL, 3000);
    g_atomic_int_dec_and_test(&g_ipc_block_bg_poll);
    if (rc != 0 || resp_len != sizeof(handle) || handle == 0) {
        LOGE("proxy_get_context failed rc=%d len=%u handle=%" PRIx64, rc, resp_len, handle);
    }

    // Look up the original info to avoid recursion when calling into local implementation
    Express_Device_Info *orig = NULL;
    Express_Device_Info *proxy = NULL;
    if (g_orig_info) orig = g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(device_id));
    if (g_proxy_info) proxy = g_hash_table_lookup(g_proxy_info, GINT_TO_POINTER(device_id));

    Thread_Context *ctx = NULL;
    if (orig && orig->get_context) {
        // Prefer passing the proxy info so thread_context_create (or device code) sets call_handler to proxy impl
        ctx = orig->get_context(device_id, thread_id, process_id, unique_id, proxy ? proxy : orig);
    }
    if (ctx) {
        ctx->context_init = NULL;
        ctx->context_destroy = NULL;
        ctx->call_handler = proxy_call_handler;
    }
    if (ctx && handle) {
        // Remember worker handle for this local context
        g_hash_table_insert(g_worker_handle_by_ctx, ctx, (gpointer)(uintptr_t)handle);
    }
    return ctx;
}

void init_express_device(const Express_Device_Info *info) {
    if (!g_proxy_info) g_proxy_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!g_orig_info) g_orig_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!g_worker_handle_by_ctx) g_worker_handle_by_ctx = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!g_local_dc_by_worker_handle) g_local_dc_by_worker_handle = g_hash_table_new(g_direct_hash, g_direct_equal);

    // Map device_id -> original info for later lookup
    g_hash_table_insert(g_orig_info, GINT_TO_POINTER(info->device_id), info);

    if (info->device_id == EXPRESS_GPU_DEVICE_ID || info->device_id == EXPRESS_SYNC_DEVICE_ID || info->device_id == EXPRESS_DISPLAY_DEVICE_ID || info->device_id == EXPRESS_MEM_DEVICE_ID  || info->device_id == EXPRESS_TOUCHSCREEN_DEVICE_ID || info->device_id == EXPRESS_KEYBOARD_DEVICE_ID) {
        // Create proxy wrapper with overridden hooks we want to intercept.
        Express_Device_Info *proxy = g_malloc(sizeof(Express_Device_Info));
        memcpy(proxy, info, sizeof(Express_Device_Info));
        proxy->get_context = proxy_get_context;
        proxy->call_handler = proxy_call_handler;
        proxy->buffer_register = proxy_buffer_register;
        proxy->get_device_context = proxy_get_device_context;
    
        g_hash_table_insert(g_proxy_info, GINT_TO_POINTER(info->device_id), proxy);
        info = proxy;
    }

    express_device_init_common(info);
}

void deinit_express_platform(void) {
    should_stop = true;
    if (parent_ipc_thread_started) {
        qemu_thread_join(&parent_ipc_thread);
        parent_ipc_thread_started = false;
    }
    // Consume residual messages
    express_gpu_shutdown_notify_callback();
    // Stop worker process
    if (worker_started && g_worker_pid > 0) {
        LOGI("sending SIGTERM to worker pid %d", (int)g_worker_pid);
        kill(g_worker_pid, SIGTERM);
        int status = 0;
        pid_t r = waitpid(g_worker_pid, &status, 0);
        if (r < 0) {
            LOGE("waitpid failed for worker: %s", strerror(errno));
        } else {
            LOGI("worker exited status %d", status);
        }
        worker_started = false;
        g_worker_pid = -1;
    }
    if (vsoc_ipc_shared) {
        size_t shm_size = sizeof(VsocGpuIpcShared);
        munmap(vsoc_ipc_shared, shm_size);
        vsoc_ipc_shared = NULL;
        if (g_shm_name[0]) {
            if (shm_unlink(g_shm_name) != 0) {
                LOGE("shm_unlink %s failed: %s", g_shm_name, strerror(errno));
            } else {
                LOGI("shm %s unlinked", g_shm_name);
            }
        }
    }

    // No socket resources to close; FDs are owned by QEMU RAM blocks
    // Join log reader threads after worker exit to drain any pending output
    if (g_worker_out_thread_started) { qemu_thread_join(&g_worker_out_thread); g_worker_out_thread_started = false; }
    if (g_worker_err_thread_started) { qemu_thread_join(&g_worker_err_thread); g_worker_err_thread_started = false; }
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
    cpy->is_gpa = orig->is_gpa;
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
 * Converts a guest memory structure to use GPA instead of HVA (default) for its scatterlist data.
 * The converted Guest_Mem structure should not be used to read/write guest memory.
 * The caller is responsible for freeing the pointer (using free_duplicated_guest_mem()).
 */
static Guest_Mem *convert_guest_mem_to_gpa(Guest_Mem *mem) {
    if (!mem) return NULL;
    mem = duplicate_guest_mem(mem);
    bool all_translatable = true;
    for (int i = 0; i < mem->num; ++i) {
        Scatter_Data *sd = &mem->scatter_data[i];
        ram_addr_t block_offset;
        RAMBlock *block = qemu_ram_block_from_host(sd->data, false, &block_offset);
        if (!block) {
            all_translatable = false;
            break;
        }
    }
    if (!all_translatable) { mem->is_gpa = 0; return mem; }
    for (int i = 0; i < mem->num; ++i) {
        Scatter_Data *sd = &mem->scatter_data[i];
        ram_addr_t block_offset;
        (void)qemu_ram_block_from_host(sd->data, false, &block_offset);
        sd->data = (unsigned char *)(uintptr_t)block_offset;
    }
    mem->is_gpa = 1;
    return mem;
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
        Teleport_Express_Call *call = call_pop(context);

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
            LOGD("thread context %llx call end thread_id %lld process_id %lld", (uint64_t)context, call->thread_id, call->process_id);
            call->callback(call, 0);
            context->thread_run = 0;
            break;
        }

        //实际对每个call调用的操作
        if (context->call_handler != NULL)
        {
            if (GET_FUN_ID(call->id) == EXPRESS_CLUSTER_FUN_ID) {
                cluster_decode_invoke(call, context, context->call_handler);
            } else {
                Call_Para all_para[MAX_PARA_NUM];
                get_para_from_call(call, all_para, MAX_PARA_NUM);
                bool success = context->call_handler(context, call->id, all_para, call->para_num);
                call->callback(call, success);
            }
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
