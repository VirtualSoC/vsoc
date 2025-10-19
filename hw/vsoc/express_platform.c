// middleware layer between the teleport-express framework and vsoc virtual devices
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/express_device.h"
#include "hw/vsoc/teleport_express_call.h"
#include "hw/vsoc/container_utils.h"

#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <glib.h>
#include <sys/epoll.h>

#include "exec/cpu-common.h"
#include "exec/ramblock.h"
#include "sysemu/runstate.h"
#include "monitor/monitor.h"

// Shared memory structures now declared in header; define global pointer here.
ExpressPlatformOps g_ops;

// Multi-worker state
typedef struct VsocWorker {
    VsocIpcShared *shared;
    VsocIpcContext *ctx;
    pid_t pid;
    char *shm_name;
    int stdout_fd;
    int stderr_fd;
} VsocWorker;
static GPtrArray *g_workers = NULL; // array of VsocWorker*

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

static bool worker_started = false;
static bool should_stop = false;
static QemuThread parent_ipc_thread;
static bool parent_ipc_thread_started = false;

static GHashTable *g_orig_info = NULL;   // key: device_id (GINT_TO_POINTER), value: original Express_Device_Info*
static GHashTable *g_proxy_info = NULL;
// Unified forward map: key local pointer (Thread_Context* or Device_Context*) -> value worker_handle(uint64)
// Parent fast-path: in-flight async DEVICE_CALL completions keyed by IPC id
static GHashTable *g_inflight_async_calls = NULL; // key: (gpointer)(uintptr_t)ipc_id, value: Teleport_Express_Call*
static GHashTable *g_early_async_acks = NULL;     // key: (gpointer)(uintptr_t)ipc_id, value: (gpointer)(uintptr_t)(resp_byte)
// Thread-local storage to pass the generated async IPC id from proxy_call_handler to handle_thread_run
static GPrivate g_tls_async_ipc_id = G_PRIVATE_INIT(NULL);
static QemuMutex g_async_seq_lock;
static QemuMutex g_inflight_lock;
static uint32_t g_async_seq = 0x80000000u; // reserve high range for async fast-path ids

static QemuThread g_worker_log_thread;
static bool g_worker_log_thread_started = false;

static Guest_Mem *convert_guest_mem_to_gpa(Guest_Mem *mem);

// Helper to fetch the appropriate worker
static inline VsocWorker *get_worker(int wid) {
    if (!g_workers || g_workers->len <= wid) {
        LOGE("get_worker: invalid wid %d (max wid %d)", wid, g_workers ? (int)g_workers->len - 1 : -1);
        return NULL;
    }
    return (VsocWorker *)g_ptr_array_index(g_workers, wid);
}

// GLib child-watch callback to observe worker exits
static void worker_child_watch_cb(GPid pid, gint status, gpointer user_data)
{
    int wid = GPOINTER_TO_INT(user_data);
    if (WIFEXITED(status)) {
        LOGE("worker[%d] %d exited status %d", wid, (int)pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        LOGE("worker[%d] %d signaled %d", wid, (int)pid, WTERMSIG(status));
    } else {
        LOGE("worker[%d] %d exited (status=%d)", wid, (int)pid, status);
    }
    // Close the GPid handle as required when using DO_NOT_REAP_CHILD
    g_spawn_close_pid(pid);
    if (!should_stop && g_ops.force_shutdown) g_ops.force_shutdown(SHUTDOWN_CAUSE_HOST_ERROR);
}

// Reader threads to capture worker stdout/stderr and log via QEMU's logger
// Single-thread multiplexed worker log reader using epoll over all workers' stdout/stderr
typedef struct { char buf[8192]; size_t len; } VsocLineBuf;
typedef struct { VsocLineBuf lb; int wid; bool is_err; } VsocFdBuf;

static void register_worker_log_fd(int epfd, GHashTable *linebufs, int fd, int wid, bool is_err) {
    if (fd < 0) return;
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) (void)fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    struct epoll_event ev = {0};
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == 0) {
    VsocFdBuf *fb = (VsocFdBuf *)g_malloc0(sizeof(VsocFdBuf));
    fb->wid = wid;
    fb->is_err = is_err;
    g_hash_table_insert(linebufs, (gpointer)(intptr_t)fd, fb);
    } else {
        LOGD("epoll_ctl ADD failed for fd %d: %s", fd, strerror(errno));
    }
}

static void register_missing_worker_log_fds(int epfd, GHashTable *linebufs) {
    if (!g_workers) return;
    for (guint i = 0; i < g_workers->len; ++i) {
        VsocWorker *w = (VsocWorker *)g_ptr_array_index(g_workers, i);
        if (!w) continue;
        if (w->stdout_fd >= 0 && !g_hash_table_lookup(linebufs, (gpointer)(intptr_t)w->stdout_fd)) {
            register_worker_log_fd(epfd, linebufs, w->stdout_fd, (int)i, false);
        }
        if (w->stderr_fd >= 0 && !g_hash_table_lookup(linebufs, (gpointer)(intptr_t)w->stderr_fd)) {
            register_worker_log_fd(epfd, linebufs, w->stderr_fd, (int)i, true);
        }
    }
}

static void *worker_log_reader_mux(void *opaque) {
    (void)opaque;
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if (epfd < 0) {
        LOGE("epoll_create1 failed: %s", strerror(errno));
        return NULL;
    }

    // Simple per-fd line buffer
    GHashTable *linebufs = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);

    // Register all current worker fds
    register_missing_worker_log_fds(epfd, linebufs);

    struct epoll_event evs[32];
    char buf[4096];
    while (!should_stop) {
        register_missing_worker_log_fds(epfd, linebufs);

        int tracked_fds = linebufs ? g_hash_table_size(linebufs) : 0;
        if (tracked_fds == 0) {
            g_usleep(50000);
            continue;
        }

        int n = epoll_wait(epfd, evs, (int)(sizeof(evs)/sizeof(evs[0])), 500);
        if (n < 0) {
            if (errno == EINTR) continue;
            LOGE("epoll_wait failed: %s", strerror(errno));
            break;
        }
        if (n == 0) continue; // timeout
        for (int i = 0; i < n; ++i) {
            int fd = evs[i].data.fd;
            VsocFdBuf *fb = (VsocFdBuf *)g_hash_table_lookup(linebufs, (gpointer)(intptr_t)fd);
            if (!fb) continue;
            bool closed = (evs[i].events & (EPOLLHUP | EPOLLRDHUP)) != 0;
            // Read all available data
            for (;;) {
                ssize_t r = read(fd, buf, sizeof(buf));
                if (r > 0) {
                    for (ssize_t k = 0; k < r; ++k) {
                        if (fb->lb.len < sizeof(fb->lb.buf) - 1) fb->lb.buf[fb->lb.len++] = buf[k];
                        if (buf[k] == '\n') {
                            fb->lb.buf[fb->lb.len] = '\0';
                            printf("[w%d] %s", fb->wid, fb->lb.buf);
                            fb->lb.len = 0;
                        }
                    }
                } else if (r == 0) {
                    closed = true; // EOF
                    break;
                } else {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    closed = true;
                    break;
                }
            }
            if (closed) {
                if (fb->lb.len) { fb->lb.buf[fb->lb.len] = '\0'; printf("[w%d] %s", fb->wid, fb->lb.buf); fb->lb.len = 0; }
                (void)epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                g_hash_table_remove(linebufs, (gpointer)(intptr_t)fd);
            }
        }
    }

    // Cleanup
    GHashTableIter it; gpointer key, val;
    g_hash_table_iter_init(&it, linebufs);
    while (g_hash_table_iter_next(&it, &key, &val)) {
        int fd = (int)(intptr_t)key; VsocFdBuf *fb = (VsocFdBuf *)val;
        if (fb && fb->lb.len) { fb->lb.buf[fb->lb.len] = '\0'; printf("[w%d] %s", fb->wid, fb->lb.buf); }
        (void)epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }
    g_hash_table_destroy(linebufs);
    close(epfd);
    return NULL;
}

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
    m.gpa_base = (uint64_t)qemu_ram_get_offset(rb);
    m.size = (uint64_t)qemu_ram_get_used_length(rb);
    m.offset = 0; // memfd is per-RAMBlock; file offset is 0
    ensure_fd_inherited(m.fd);
    g_array_append_val(list->arr, m);
    LOGI("prepared RAM memfd for %s rb=%p fd=%d gpa_base=%#llx size=%#llx file_off=%#llx (per-block memfd)", qemu_ram_get_idstr(rb), (void *)rb, m.fd, (unsigned long long)m.gpa_base, (unsigned long long)m.size, (unsigned long long)m.offset);
    return 0;
}

// Helper: ensure all RAMBlock memfd FDs are inheritable (no CLOEXEC) before spawning workers.
static void ensure_ramblock_fds_inheritable(void) {
    RamRegionList list = { .arr = g_array_new(FALSE, TRUE, sizeof(RamRegionMeta)) };
    qemu_ram_foreach_block(collect_block_cb, &list);
    g_array_free(list.arr, TRUE);
}

static void send_ram_regions_to_workers(void) {
    RamRegionList list = { .arr = g_array_new(FALSE, TRUE, sizeof(RamRegionMeta)) };
    qemu_ram_foreach_block(collect_block_cb, &list);
    uint32_t count = (uint32_t)list.arr->len;
    size_t payload_sz = sizeof(count) + count * sizeof(RamRegionMeta);
    uint8_t *payload = g_malloc(payload_sz);
    memcpy(payload, &count, sizeof(count));
    memcpy(payload + sizeof(count), list.arr->data, count * sizeof(RamRegionMeta));
    // Send to all workers so each can map the memfd regions
    if (g_workers && g_workers->len > 0) {
        for (guint i = 0; i < g_workers->len; ++i) {
            VsocWorker *w = (VsocWorker *)g_ptr_array_index(g_workers, i);
            bool ok = vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_RAM_REGIONS, 0, payload, (uint32_t)payload_sz);
            if (!ok) LOGE("failed to send RAM_REGIONS to worker[%u]", i);
        }
    } else {
        LOGE("no workers available to send RAM_REGIONS metadata");
    }
    g_free(payload);
    g_array_free(list.arr, TRUE);
}

// Spawn a single worker at a given index and wait (up to a fixed timeout) for worker_ready.
// Returns 0 on success, negative on failure.
static int spawn_worker(int index) {
    const char *worker_path = getenv("VSOC_WORKER_PATH");
    if (!worker_path || !*worker_path) worker_path = "vsoc-worker";
    VsocWorker *w = g_new0(VsocWorker, 1);
    w->stdout_fd = -1;
    w->stderr_fd = -1;
    w->shm_name = g_strdup_printf("/vsoc_ipc_%d_%d", (int)getpid(), index);
    size_t shm_size = sizeof(VsocIpcShared);
    VsocIpcContext *ctx = vsoc_ipc_context_create(w->shm_name, shm_size, true);
    VsocIpcShared *shared = ctx ? vsoc_ipc_context_get_shared(ctx) : NULL;
    if (!ctx || !shared) {
        LOGE("spawn_worker: failed to create shared memory for worker %d", index);
        if (ctx) vsoc_ipc_context_destroy(ctx);
        g_free(w->shm_name); g_free(w); return -1;
    }
    w->shared = shared; w->ctx = ctx;
    g_ptr_array_add(g_workers, w);

    GError *error = NULL; GPid child_pid = -1; 
    int child_stdin=-1, child_stdout=-1, child_stderr=-1; 
    gboolean ok;
    gchar parent_pid_str[32]; 
    g_snprintf(parent_pid_str, sizeof(parent_pid_str), "%d", (int)getpid());
    gchar *argv_spawn[16];
    int argv_idx = 0;
    if (0) {
        argv_spawn[argv_idx++] = (gchar *)"gprofng";
        argv_spawn[argv_idx++] = (gchar *)"collect";
        argv_spawn[argv_idx++] = (gchar *)"app";
    }
    argv_spawn[argv_idx++] = (gchar *)worker_path;
    argv_spawn[argv_idx++] = w->shm_name;
    argv_spawn[argv_idx++] = parent_pid_str;
    argv_spawn[argv_idx] = NULL;
    ok = g_spawn_async_with_pipes(
        NULL, (gchar * const *)argv_spawn, NULL,
        G_SPAWN_SEARCH_PATH | G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_DO_NOT_REAP_CHILD,
        NULL, NULL,
        &child_pid,
        &child_stdin, &child_stdout, &child_stderr,
        &error);
    if (!ok) {
        if (error) { 
            LOGE("spawn_worker: launch failed worker[%d]: %s", index, error->message); 
            g_clear_error(&error); 
        }
        else { 
            LOGE("spawn_worker: launch failed worker[%d] (unknown error)", index); 
        }
        // Remove from array; last element should be this pointer.
        g_ptr_array_remove_index(g_workers, g_workers->len - 1);
        g_free(w->shm_name); g_free(w);
        return -2;
    }
    w->pid = (pid_t)child_pid;
    if (child_stdin >= 0) close(child_stdin);
    w->stdout_fd = child_stdout; w->stderr_fd = child_stderr;
    g_child_watch_add(child_pid, worker_child_watch_cb, GINT_TO_POINTER(index));
    LOGI("spawn_worker: spawned vsoc-worker[%d] pid %d shm %s", index, (int)w->pid, w->shm_name);

    // Compulsory wait for worker_ready (up to WAIT_MS_MAX ms)
    const int WAIT_MS_MAX = 10000; // fixed compulsory wait budget
    int waited = 0;
    while (waited < WAIT_MS_MAX) {
        if (w->shared && w->shared->worker_ready) break;
        g_usleep(1000); waited++;
    }
    if (!w->shared || !w->shared->worker_ready) {
        LOGE("spawn_worker: worker %d did not signal ready within %d ms", index, WAIT_MS_MAX);
    }

    // Initialize worker with platform ops and RAM regions.
    vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_PLATFORM_INIT, 0, &g_ops, sizeof(g_ops));

    return 0;
}

static void spawn_worker_processes(int count) {
    if (worker_started) return;
    if (count <= 0) count = 1;
    if (!g_workers) g_workers = g_ptr_array_new();
    for (int i = 0; i < count; ++i) {
        spawn_worker(i);
    }

    // After worker init, inform worker of RAM regions (FDs are already inherited)
    send_ram_regions_to_workers();
    worker_started = true;
}

// Handle SET_IRQ forwarded from worker: payload is struct Req; respond with int32 status
static void set_irq_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                               uint32_t len) {
    (void)type;
    (void)ctx;
    struct Req {
        uint64_t parent_handle; // parent Device_Context* value
        int32_t buf_index;
        int32_t len;
    } req;
    if (len != sizeof(req)) {
        LOGE("SET_IRQ: bad len %u expected %zu", len, sizeof(req));
        int32_t st = IRQ_NOT_READY; (void)vsoc_ipc_send_response(ctx, VSOC_IPC_TYPE_SET_IRQ, id, &st, sizeof(st));
        return;
    }
    memcpy(&req, data, sizeof(req));
    int32_t status = IRQ_NOT_READY;
    Device_Context *dc = (Device_Context *)(uintptr_t)req.parent_handle;
    if (dc && g_ops.set_express_device_irq) {
        status = g_ops.set_express_device_irq(dc, req.buf_index, req.len);
    } else {
        LOGD("SET_IRQ: dc %p no IRQ impl", dc);
    }
    (void)vsoc_ipc_send_response(ctx, VSOC_IPC_TYPE_SET_IRQ, id, &status, sizeof(status));
}

// Handle async DEVICE_CALL responses (no pending waiter)
static void device_call_ack_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                                        uint32_t len) {
    (void)type;
    (void)ctx;
    uint8_t resp = 0; if (len >= 1) resp = data[0];
    qemu_mutex_lock(&g_inflight_lock);
    Teleport_Express_Call *call = g_hash_table_lookup(g_inflight_async_calls, (gpointer)(uintptr_t)id);
    if (call) {
        g_hash_table_remove(g_inflight_async_calls, (gpointer)(uintptr_t)id);
        qemu_mutex_unlock(&g_inflight_lock);
        call->callback(call, resp ? true : false);
    } else {
        // Store early ACK to be matched when the call is enqueued by the device thread
        g_hash_table_insert(g_early_async_acks, (gpointer)(uintptr_t)id, (gpointer)(uintptr_t)(resp));
        qemu_mutex_unlock(&g_inflight_lock);
    }
}

// Worker forwarded shutdown notifications: invoke parent hooks
static void notify_shutdown_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                                        uint32_t len) {
    (void)type; (void)id; (void)data; (void)len; (void)ctx;
    (void)ctx;
    if (g_ops.notify_shutdown) g_ops.notify_shutdown();
}

static void force_shutdown_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                                       uint32_t len) {
    (void)type; (void)id;
    int reason = 0;
    if (len == sizeof(int32_t)) {
        int32_t r; memcpy(&r, data, sizeof(r)); reason = (int)r;
    }
    if (g_ops.force_shutdown) g_ops.force_shutdown(reason);
}

/**
 * sync device is shared, need to broadcast to all workers
 */
static void send_sync_buffer_to_wid(int wid, uint8_t *data, size_t len) {
    static uint8_t buf[VSOC_IPC_MAX_PAYLOAD];

    if (data != NULL && len > 0) {
        memset(buf, 0, sizeof(buf));
        size_t to_copy = len < sizeof(buf) ? len : sizeof(buf);
        memcpy(buf, data, to_copy);
    }

    VsocWorker *w = get_worker(wid);
    if (w) {
        bool ok = vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_BUFFER_REGISTER, 0, buf, (uint32_t)sizeof(buf));
        if (!ok) LOGE("send_sync_buffer_to_wid: send failed to worker[%d]", wid);
    }
}

static void proxy_buffer_register(Guest_Mem *data, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, uint64_t user_id, Express_Device_Info *info) {
    uint64_t device_id = (uint64_t)info->device_id;
    uint8_t buf[VSOC_IPC_MAX_PAYLOAD];
    uint8_t *p = buf; uint8_t *end = buf + sizeof(buf);
    // Header: [device_id(8)][thread_id(8)][process_id(8)][unique_id(8)][user_id(8)] then packed Guest_Mem
    if (p + sizeof(uint64_t)*5 > end) {
        LOGE("proxy_buffer_register: header overflow");
        // still fall through to local call below
    } else {
        memcpy(p, &device_id, sizeof(device_id)); p += sizeof(device_id);
        memcpy(p, &thread_id, sizeof(thread_id)); p += sizeof(thread_id);
        memcpy(p, &process_id, sizeof(process_id)); p += sizeof(process_id);
        memcpy(p, &unique_id, sizeof(unique_id)); p += sizeof(unique_id);
        memcpy(p, &user_id, sizeof(user_id)); p += sizeof(user_id);
        data = convert_guest_mem_to_gpa(data);
        size_t wrote = vsoc_ipc_guest_mem_pack(p, (size_t)(end - p), data);
        free_duplicated_guest_mem(data);
        if (wrote == 0) {
            LOGE("proxy_buffer_register: pack overflow");
        } else {
            p += wrote;
            uint32_t payload_len = (uint32_t)(p - buf);
            if (info->device_id == EXPRESS_SYNC_DEVICE_ID) {
                for (guint i = 0; i < g_workers->len; ++i) {
                    send_sync_buffer_to_wid((int)i, buf, payload_len);
                }
            } else {
                VsocWorker *w = get_worker(wid_from_ids(info->device_id, unique_id, user_id));
                bool ok = w && vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_BUFFER_REGISTER, 0, buf, payload_len);
                if (!ok) LOGE("proxy_buffer_register: send failed to worker[%d]", wid_from_ids(info->device_id, unique_id, user_id));
            }
        }
    }
}

static Device_Context *proxy_get_device_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, Express_Device_Info *info) {
    // Create a local Device_Context in QEMU so IRQ routing works locally
    Express_Device_Info *orig = g_orig_info ? g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(device_id)) : NULL;
    if (!orig || !orig->get_device_context) {
        LOGE("proxy_get_device_context: missing original get_device_context for device_id=%" PRIu64, device_id);
        return NULL;
    }
    Device_Context *dc = orig->get_device_context(device_id, thread_id, process_id, unique_id, orig);
    if (!dc) return NULL;
    // Point DC to proxy info so future irq_register/release calls route through our proxy wrappers
    Express_Device_Info *proxy = g_proxy_info ? g_hash_table_lookup(g_proxy_info, GINT_TO_POINTER(device_id)) : NULL;
    if (proxy) dc->device_info = proxy;

    // Notify worker to create/associate its Device_Context with this parent Device_Context pointer.
    struct Req { uint64_t parent_handle; uint64_t device_id, thread_id, process_id, unique_id; } req;
    req.parent_handle = (uint64_t)(uintptr_t)dc;
    req.device_id = device_id; req.thread_id = thread_id; req.process_id = process_id; req.unique_id = unique_id;
    {
        VsocWorker *w = get_worker(wid_from_device_context(dc));
        if (w) {
            bool ok = vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_GET_DEVICE_CONTEXT, 0, &req, sizeof(req));
            if (!ok) LOGE("proxy_get_device_context: send GET_DEVICE_CONTEXT to worker failed (wid=%d)", wid_from_device_context(dc));
        }
    }

    return dc;
}

static void proxy_irq_register(Device_Context *context) {
    if (!context) return;
    // Fire-and-forget: include device_id and parent_handle so worker resolves to its own dc
    struct { uint64_t device_id; uint64_t parent_handle; } payload;
    int device_id = context->device_info ? context->device_info->device_id : 0;
    payload.device_id = (uint64_t)device_id;
    payload.parent_handle = (uint64_t)(uintptr_t)context;
    {
        VsocWorker *w = get_worker(wid_from_device_context(context));
    (void)(w && vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_IRQ_REGISTER, 0, &payload, sizeof(payload)));
    }
    // Also run original parent-side irq_register to mirror state locally
    Express_Device_Info *orig = g_orig_info ? g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(device_id)) : NULL;
    if (orig && orig->irq_register) orig->irq_register(context);
}

static void proxy_irq_release(Device_Context *context) {
    if (!context) return;
    struct { uint64_t device_id; uint64_t parent_handle; } payload;
    int device_id = context->device_info ? context->device_info->device_id : 0;
    payload.device_id = (uint64_t)device_id;
    payload.parent_handle = (uint64_t)(uintptr_t)context;
    {
        VsocWorker *w = get_worker(wid_from_device_context(context));
    (void)(w && vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_IRQ_RELEASE, 0, &payload, sizeof(payload)));
    }
    Express_Device_Info *orig = g_orig_info ? g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(device_id)) : NULL;
    if (orig && orig->irq_release) orig->irq_release(context);
}

static bool proxy_call_handler(struct Thread_Context *context, uint64_t id, const Call_Para *all_para, int para_num) {
    Express_Device_Info *orig = g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(context->device_id));
    if (!orig || !orig->call_handler) {
        LOGE("proxy_call_handler: no original device info or call_handler for device %" PRIu64, context->device_id);
        goto fallback;
    }

    // Pass parent Thread_Context pointer; worker will resolve to its own context
    uint64_t parent_handle = (uint64_t)(uintptr_t)context;
    LOGD("proxy_call_handler: parent_handle=%" PRIx64 " id=%" PRIu64 " para_num=%d sync=%s", parent_handle, GET_FUN_ID(id), para_num, FUN_NEED_SYNC(id) ? "true" : "false");

    // Build DEVICE_CALL payload: [parent_handle(8)][id(8)][para_num(4)] + per-param packed Guest_Mem
    uint8_t buf[VSOC_IPC_MAX_PAYLOAD];
    uint8_t *p = buf; uint8_t *end = buf + sizeof(buf);
    if (p + sizeof(uint64_t)*2 + sizeof(int32_t) > end) {
        LOGE("proxy_call_handler: header overflow");
        goto fallback;
    }
    memcpy(p, &parent_handle, sizeof(parent_handle)); p += sizeof(parent_handle);
    memcpy(p, &id, sizeof(id)); p += sizeof(id);
    int32_t pn = para_num; memcpy(p, &pn, sizeof(pn)); p += sizeof(pn);

    for (int i = 0; i < para_num; ++i) {
        const Call_Para *cp = &all_para[i];
        Guest_Mem *data = convert_guest_mem_to_gpa(cp->data);
        size_t wrote = vsoc_ipc_guest_mem_pack(p, (size_t)(end - p), data);
        if (wrote == 0) {
            LOGE("proxy_call_handler: dev %d fun %d param (%d/%d) pack overflow, max %zu guestmem len %d gpa %d sg_count %d", GET_DEVICE_ID(id), GET_FUN_ID(id), i + 1, para_num, (size_t)(end - p), data->all_len, data->is_gpa ? true : false, data->num);
            goto fallback;
        }
        free_duplicated_guest_mem(data);
        p += wrote;
    }

    // Async fast-path: allocate a unique IPC id, stash it in TLS, and send non-blocking.
    uint32_t payload_len = (uint32_t)(p - buf);
    uint32_t ipc_id;
    qemu_mutex_lock(&g_async_seq_lock);
    ipc_id = g_async_seq++;
    if (g_async_seq == 0) g_async_seq = 0x80000000u; // wrap within reserved range
    qemu_mutex_unlock(&g_async_seq_lock);
    // Store the id in TLS for handle_thread_run to bind the call object.
    g_private_set(&g_tls_async_ipc_id, (gpointer)(uintptr_t)ipc_id);
    VsocWorker *w = get_worker(wid_from_thread_context(context));
    bool ok = w && vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_DEVICE_CALL, ipc_id, buf, payload_len);
    if (!ok) {
        g_private_set(&g_tls_async_ipc_id, NULL);
        LOGE("proxy_call_handler: async send to worker[%d] failed", wid_from_thread_context(context));
        goto fallback;
    }
    return true;

fallback:
    return false;
}

Thread_Context *proxy_get_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, uint64_t user_id, Express_Device_Info *info) {
    // Look up the original info to avoid recursion when calling into local implementation
    Express_Device_Info *orig = NULL;
    Express_Device_Info *proxy = NULL;
    if (g_orig_info) orig = g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(device_id));
    if (g_proxy_info) proxy = g_hash_table_lookup(g_proxy_info, GINT_TO_POINTER(device_id));

    Thread_Context *ctx = NULL;
    if (orig && orig->get_context) {
        // Prefer passing the proxy info so thread_context_create (or device code) sets call_handler to proxy impl
        ctx = orig->get_context(device_id, thread_id, process_id, unique_id, user_id, proxy ? proxy : orig);
    }
    if (ctx) {
        ctx->context_init = NULL;
        ctx->context_destroy = NULL;
    }

    // Notify worker to create/associate its context with this parent Thread_Context pointer.
    if (ctx) {
        struct Req { uint64_t parent_handle; uint64_t device_id, thread_id, process_id, unique_id, user_id; } req;
        req.parent_handle = (uint64_t)(uintptr_t)ctx;
        req.device_id = device_id; req.thread_id = thread_id; req.process_id = process_id; req.unique_id = unique_id; req.user_id = user_id;
        VsocWorker *w = get_worker(wid_from_thread_context(ctx));
        if (w) {
            bool ok = vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_GET_CONTEXT, 0, &req, sizeof(req));
            if (!ok) {
                LOGE("proxy_get_context: send GET_CONTEXT to worker failed (wid=%d)", wid_from_thread_context(ctx));
            }
        }
    }

    return ctx;
}

void register_express_device(const Express_Device_Info *info) {
    express_device_init_common(info);
}

void init_express_platform(const ExpressPlatformOps ops) {
    g_ops = ops;
    if (!ops.express_device_multi_process) return;

    if (!g_proxy_info) g_proxy_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!g_orig_info) g_orig_info = g_hash_table_new(g_direct_hash, g_direct_equal);

    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, get_all_express_device_info());
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        Express_Device_Info *info = (Express_Device_Info *)value;
        if (device_needs_proxy(info->device_id)) {
            // Create proxy wrapper with overridden hooks we want to intercept.
            Express_Device_Info *orig = g_malloc(sizeof(Express_Device_Info));
            memcpy(orig, info, sizeof(Express_Device_Info));
            info->proxy = true;
            info->get_context = proxy_get_context;
            info->call_handler = proxy_call_handler;
            info->get_device_context = proxy_get_device_context;
            info->buffer_register = proxy_buffer_register;
            info->irq_register = proxy_irq_register;
            info->irq_release = proxy_irq_release;

            g_hash_table_insert(g_proxy_info, GINT_TO_POINTER(info->device_id), info);
            g_hash_table_insert(g_orig_info, GINT_TO_POINTER(orig->device_id), orig);
        }
    }

    // Ensure inflight map exists and register a handler for DEVICE_CALL ACKs
    if (!g_inflight_async_calls) g_inflight_async_calls = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!g_early_async_acks) g_early_async_acks = g_hash_table_new(g_direct_hash, g_direct_equal);
    qemu_mutex_init(&g_async_seq_lock);
    qemu_mutex_init(&g_inflight_lock);

    vsoc_ipc_register_handler(VSOC_IPC_TYPE_SET_IRQ, set_irq_ipc_handler);
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_DEVICE_CALL, device_call_ack_ipc_handler);
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_NOTIFY_SHUTDOWN, notify_shutdown_ipc_handler);
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_FORCE_SHUTDOWN, force_shutdown_ipc_handler);

    ensure_ramblock_fds_inheritable();

    // Spawn as many workers as requested; default to 1
    int worker_count = (g_ops.express_display_count > 0) ? g_ops.express_display_count : 1;
    spawn_worker_processes(worker_count);

    // Start single log reader thread for all workers
    if (worker_started && !g_worker_log_thread_started) {
        qemu_thread_create(&g_worker_log_thread, "vsoc-worker-logs", worker_log_reader_mux, NULL, QEMU_THREAD_JOINABLE);
        g_worker_log_thread_started = true;
    }

    // Start dedicated IPC polling thread (parent only)
    if (!parent_ipc_thread_started) {
        void *parent_poll_thread(void *opaque) {
            (void)opaque;
            while (!should_stop) {
                // Poll all worker IPC rings
                if (g_workers && g_workers->len > 0) {
                    for (guint i = 0; i < g_workers->len; ++i) {
                        VsocWorker *w = (VsocWorker *)g_ptr_array_index(g_workers, i);
                        if (w->ctx) vsoc_ipc_poll(w->ctx);
                    }
                }
                g_usleep(1000); // 1ms poll interval
            }
            return NULL;
        }
        qemu_thread_create(&parent_ipc_thread, "vsoc-ipc-poll", parent_poll_thread, NULL, QEMU_THREAD_JOINABLE);
        parent_ipc_thread_started = true;
    }
}

void deinit_express_platform(void) {
    should_stop = true;
    if (g_worker_log_thread_started) {
        qemu_thread_join(&g_worker_log_thread);
        g_worker_log_thread_started = false;
    }
    if (parent_ipc_thread_started) {
        qemu_thread_join(&parent_ipc_thread);
        parent_ipc_thread_started = false;
    }
    // Consume residual messages
    express_gpu_shutdown_notify_callback();
    // Stop worker process
    if (worker_started) {
        if (g_workers && g_workers->len > 0) {
            for (guint i = 0; i < g_workers->len; ++i) {
                VsocWorker *w = (VsocWorker *)g_ptr_array_index(g_workers, i);
                if (w->pid > 0) {
                    // send force_shutdown to worker
                    int32_t reason = 0;
                    vsoc_ipc_send(w->ctx, VSOC_IPC_TYPE_FORCE_SHUTDOWN, 0, &reason, sizeof(reason));
                    int status = 0; (void)waitpid(w->pid, &status, 0);
                }
            }
        }
        worker_started = false;
    }
    // Unmap/destroy all shared regions and contexts, unlink shm names
    if (g_workers && g_workers->len > 0) {
        for (guint i = 0; i < g_workers->len; ++i) {
            VsocWorker *w = (VsocWorker *)g_ptr_array_index(g_workers, i);
            if (w->shared) munmap(w->shared, sizeof(VsocIpcShared));
            if (w->ctx) vsoc_ipc_context_destroy(w->ctx);
            if (w->shm_name && *w->shm_name) {
                if (shm_unlink(w->shm_name) != 0) { LOGE("shm_unlink %s failed: %s", w->shm_name, strerror(errno)); }
                else { LOGD("shm %s unlinked", w->shm_name); }
                g_free(w->shm_name);
            }
            g_free(w);
        }
        g_ptr_array_free(g_workers, TRUE);
        g_workers = NULL;
    }

    // No socket resources to close; FDs are owned by QEMU RAM blocks
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
        RAMBlock *block = qemu_ram_block_from_host(sd->iov_base, false, &block_offset);
        if (!block) {
            all_translatable = false;
            break;
        }
    }
    if (!all_translatable) { 
        mem->is_gpa = false; 
        return mem; 
    }
    for (int i = 0; i < mem->num; ++i) {
        Scatter_Data *sd = &mem->scatter_data[i];
        ram_addr_t block_off;
        RAMBlock *rb = qemu_ram_block_from_host(sd->iov_base, false, &block_off);
        // rb must be non-NULL here because all_translatable was true
        uint64_t gpa = (uint64_t)qemu_ram_get_offset(rb) + (uint64_t)block_off;
        sd->iov_base = (void *)(uintptr_t)gpa; // store GPA token
    }
    mem->is_gpa = true;
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
    if (!guest_mem || guest_mem->num != 1) {
        if (flag) *flag = 0;
        return NULL;
    }

    Scatter_Data *guest_data = guest_mem->scatter_data;
    if (flag) *flag = 1;
    // 这里也可能返回NULL，所以以flag来区分
    return guest_data->iov_base;
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

bool invoke_call_handler(Thread_Context *context, void *_call) {
    Teleport_Express_Call *call = (Teleport_Express_Call *)_call;
    bool success = false;

    if (call == NULL) {
        return success;
    }

    if (context->call_handler != NULL) {
        Call_Para all_para[MAX_PARA_NUM];
        get_para_from_call(call, all_para, MAX_PARA_NUM);
        if (!context->proxy && GET_FUN_ID(call->id) == EXPRESS_CLUSTER_FUN_ID) {
            success = cluster_decode_invoke(context, all_para, call->para_num);
        } else {
            success = context->call_handler(context, call->id, all_para, call->para_num);
        }
        // If proxied async, bind call to inflight table and defer callback to IPC poll handler
        if (context->proxy) {
            uintptr_t ipc_id = (uintptr_t)g_private_get(&g_tls_async_ipc_id);
            if (ipc_id == 0) {
                LOGE("async fast-path: missing IPC id for deferred call");
                // Fallback to immediate completion to avoid leak
                call->callback(call, success);
            } else {
                // Protect binding and race with early ACK
                qemu_mutex_lock(&g_inflight_lock);
                gpointer early = g_hash_table_lookup(g_early_async_acks, (gpointer)ipc_id);
                if (early) {
                    // ACK arrived before binding; consume and complete now
                    g_hash_table_remove(g_early_async_acks, (gpointer)ipc_id);
                    qemu_mutex_unlock(&g_inflight_lock);
                    g_private_set(&g_tls_async_ipc_id, NULL);
                    call->callback(call, ((uintptr_t)early) ? true : false);
                } else {
                    g_hash_table_insert(g_inflight_async_calls, (gpointer)ipc_id, call);
                    qemu_mutex_unlock(&g_inflight_lock);
                    g_private_set(&g_tls_async_ipc_id, NULL);
                    // Defer completion to ACK handler
                }
            }
        } else {
            call->callback(call, success);
        }
    }
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

    if (!context->proxy && context->context_init != NULL)
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

static void container_hmp_handler(Monitor *mon, int argc, const char **argv) {
    if (argc == 1 && strcmp(argv[0], "new") == 0) {
        // Dynamically add a new container: spawn worker and adjust display count.
        if (!g_ops.express_device_multi_process) {
            monitor_printf(mon, "error: multi-process not enabled\n");
            return;
        }

        int wid = (int)g_workers->len;
        assert(g_ops.express_display_count == wid);

        // increment display count so future workers know about it.
        g_ops.express_display_count += 1;
        
        if (spawn_worker(wid) != 0) {
            monitor_printf(mon, "error: failed to spawn new container\n");
            return;
        }

        send_ram_regions_to_workers();
        send_sync_buffer_to_wid(wid, NULL, 0);

        // notify guest side about new container
        Express_Device_Info *info = get_express_device_info(EXPRESS_TOUCHSCREEN_DEVICE_ID);
        Device_Context *ctx = info->get_device_context(info->device_id, 0, 0, 0, info);
        g_ops.set_express_device_irq((Device_Context *)ctx, wid, 0);

        monitor_printf(mon, "%d\n", wid);
    } else {
        monitor_printf(mon, "Usage: express container new\n");
    }

}

// stub info struct to register HMP handler for container
static Express_Device_Info express_container_info = {
    .enable_default = false,
    .name = "express-container",
    .option_name = "container",
    .device_id = EXPRESS_CONTAINER_DEVICE_ID,
    .device_type = 0,
    .hmp_handler = container_hmp_handler,
};

EXPRESS_DEVICE_INIT(express_container, &express_container_info)
