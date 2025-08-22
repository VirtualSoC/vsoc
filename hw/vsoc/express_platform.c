// middleware layer between the teleport-express framework and vsoc virtual devices
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/express_event.h"
#include "hw/vsoc/express_handle_thread.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/express_device.h"
#include "hw/vsoc/teleport_express_call.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <glib.h>

// Shared memory structures now declared in header; define global pointer here.
ExpressPlatformOps g_ops;
static pid_t g_worker_pid = -1;
static char g_shm_name[64];

static bool worker_started = false;
static bool should_stop = false;
static QemuThread parent_ipc_thread;
static bool parent_ipc_thread_started = false;

static GHashTable *g_proxy_info = NULL;
static GHashTable *g_worker_handle_by_ctx = NULL; // key: Thread_Context*, value: (gpointer)worker_handle
static GHashTable *g_orig_info = NULL;   // key: device_id (GINT_TO_POINTER), value: original Express_Device_Info*

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

    // Build a command to open a terminal window and run the worker inside it.
    // Try common terminal emulators in order.
    GError *error = NULL;
    gboolean ok = FALSE;

    // Ensure log directory exists for tee output
    g_mkdir_with_parents("log", 0755);

    // 1) gnome-terminal via bash -lc
    if (!ok && g_find_program_in_path("gnome-terminal")) {
        gchar cmdline[256];
        // If 'tee' exists, pipe output to log/worker.log and terminal; else run normally.
        snprintf(cmdline, sizeof(cmdline),
                 "(command -v tee >/dev/null 2>&1 && (vsoc-worker %s |& tee -a log/worker.log)) || vsoc-worker %s; echo 'worker exited'; exec bash",
                 g_shm_name, g_shm_name);
        gchar *argv_gterm[] = {
            (gchar*)"gnome-terminal", (gchar*)"--", (gchar*)"bash", (gchar*)"-lc",
            cmdline, NULL
        };
        ok = g_spawn_async(NULL, argv_gterm, NULL, G_SPAWN_SEARCH_PATH,
                           NULL, NULL, &g_worker_pid, &error);
        if (!ok && error) { LOGW("gnome-terminal launch failed: %s", error->message); g_clear_error(&error); }
    }

    // 2) xterm: widely available and simple
    if (!ok && g_find_program_in_path("xterm")) {
        gchar cmdline[256];
        snprintf(cmdline, sizeof(cmdline),
                 "(command -v tee >/dev/null 2>&1 && (vsoc-worker %s |& tee -a log/worker.log)) || vsoc-worker %s; echo 'worker exited'; exec bash",
                 g_shm_name, g_shm_name);
        gchar *argv_xterm[] = {
            (gchar*)"xterm", (gchar*)"-T", (gchar*)"VSOC Worker",
            (gchar*)"-e", (gchar*)"bash", (gchar*)"-lc", cmdline, NULL
        };
        ok = g_spawn_async(NULL, argv_xterm, NULL, G_SPAWN_SEARCH_PATH,
                           NULL, NULL, &g_worker_pid, &error);
        if (!ok && error) { LOGW("xterm launch failed: %s", error->message); g_clear_error(&error); }
    }

    if (!ok) {
        LOGE("failed to launch worker in a terminal; ensure xterm/gnome-terminal is installed");
        return;
    }

    worker_started = true;
    LOGI("spawned vsoc-worker (+terminal) pid %d shm %s", (int)g_worker_pid, g_shm_name);
}

void init_express_platform(const ExpressPlatformOps ops) {
    g_ops = ops;
    spawn_worker_process();
    // Start dedicated IPC polling thread (parent only)
    if (!parent_ipc_thread_started) {
        void *parent_poll_thread(void *opaque) {
            (void)opaque;
            while (!should_stop) {
                vsoc_ipc_poll_parent();
                g_usleep(1000); // 1ms poll interval (tunable)
            }
            // final drain
            vsoc_ipc_poll_parent();
            return NULL;
        }
        qemu_thread_create(&parent_ipc_thread, "vsoc-ipc-poll", parent_poll_thread, NULL, QEMU_THREAD_JOINABLE);
        parent_ipc_thread_started = true;
    }

    vsoc_ipc_parent_send(VSOC_IPC_TYPE_PLATFORM_INIT, 0, &ops, sizeof(ops), 0);
}

Thread_Context *proxy_get_context(uint64_t device_id, uint64_t thread_id, uint64_t process_id, uint64_t unique_id, Express_Device_Info *info) {
    // request structure
    struct Req { uint64_t device_id, thread_id, process_id, unique_id; } req = { device_id, thread_id, process_id, unique_id };
    uint64_t handle = 0; uint32_t resp_len = sizeof(handle);
    int rc = vsoc_ipc_parent_request(VSOC_IPC_TYPE_GET_CONTEXT, &req, sizeof(req), &handle, &resp_len, NULL, 3000);
    if (rc != 0 || resp_len != sizeof(handle) || handle == 0) {
        LOGE("proxy_get_context failed rc=%d len=%u handle=%" PRIx64, rc, resp_len, handle);
    }

    // Look up the original info (not the proxy) to avoid recursion when calling into local implementation
    Express_Device_Info *orig = NULL;
    if (g_orig_info) orig = g_hash_table_lookup(g_orig_info, GINT_TO_POINTER(device_id));
    Thread_Context *ctx = orig->get_context(device_id, thread_id, process_id, unique_id, orig);
    if (ctx && handle) {
        // Remember worker handle for this local context
        g_hash_table_insert(g_worker_handle_by_ctx, ctx, (gpointer)(uintptr_t)handle);
    }
    return ctx;
}

bool proxy_call_handler(struct Thread_Context *context, uint64_t id, const Call_Para *all_para, int para_num) {
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

    // Build DEVICE_CALL payload: [handle(8)][id(8)][para_num(4)] + per-param [num(4)][all_len(4)] + segments
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
        uint32_t num = 0, all_len = 0;
        if (cp->data) { num = (uint32_t)cp->data->num; all_len = (uint32_t)cp->data->all_len; }
        if (p + sizeof(uint32_t)*2 > end) { LOGE("proxy_call_handler: param %d header overflow", i); goto fallback; }
        memcpy(p, &num, sizeof(num)); p += sizeof(num);
        memcpy(p, &all_len, sizeof(all_len)); p += sizeof(all_len);
        size_t need = (size_t)num * sizeof(VsocGuestMemSeg);
        if (p + need > end) { LOGE("proxy_call_handler: param %d segs overflow (num=%u)", i, num); goto fallback; }
        if (num) {
            VsocGuestMemSeg *segs = (VsocGuestMemSeg*)p;
            for (uint32_t s = 0; s < num; ++s) {
                segs[s].addr = (uint64_t)(uintptr_t)cp->data->scatter_data[s].data;
                segs[s].len = (uint32_t)cp->data->scatter_data[s].len;
                segs[s]._pad = 0;
            }
            p += need;
        }
    }

    uint32_t payload_len = (uint32_t)(p - buf);
    if (FUN_NEED_SYNC(id)) {
        uint8_t resp = 0; uint32_t resp_len = sizeof(resp);
        int rc = vsoc_ipc_parent_request(VSOC_IPC_TYPE_DEVICE_CALL, buf, payload_len, &resp, &resp_len, NULL, 10000);
        if (rc != 0 || resp_len != sizeof(resp)) {
            LOGE("proxy_call_handler: request rc=%d resp_len=%u", rc, resp_len);
        }
    } else {
        bool ok = vsoc_ipc_parent_send(VSOC_IPC_TYPE_DEVICE_CALL, 0, buf, payload_len, 0);
        if (!ok) {
            LOGE("proxy_call_handler: async send failed");
        }
    }

fallback:
    return orig->call_handler(context, id, all_para, para_num);
}

void init_express_device(const Express_Device_Info *info) {
    if (!g_proxy_info) g_proxy_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!g_orig_info) g_orig_info = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!g_worker_handle_by_ctx) g_worker_handle_by_ctx = g_hash_table_new(g_direct_hash, g_direct_equal);

    // Create proxy wrapper with overridden hooks we want to intercept.
    Express_Device_Info *proxy = g_malloc(sizeof(Express_Device_Info));
    memcpy(proxy, info, sizeof(Express_Device_Info));
    proxy->get_context = proxy_get_context;
    proxy->call_handler = proxy_call_handler;

    // Map device_id -> original info for later lookup
    g_hash_table_insert(g_orig_info, GINT_TO_POINTER(info->device_id), info);
    g_hash_table_insert(g_proxy_info, GINT_TO_POINTER(info->device_id), proxy);

    express_device_init_common(proxy);
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
