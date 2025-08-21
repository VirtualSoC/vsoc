// middleware layer between the teleport-express framework and vsoc virtual devices
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_device.h"
#include "hw/vsoc/express_event.h"

#include "hw/vsoc/gpu/express_gpu_main_window.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

// Shared memory structures now declared in header; define global pointer here.
ExpressPlatformOps g_ops;
VsocGpuIpcShared *vsoc_ipc_shared = NULL;
static pid_t g_worker_pid = -1;
static char g_shm_name[64];

static bool worker_started = false;
static bool should_stop = false;

void *handle_thread_run(void *opaque);

// Test handler: echoes back with incremented id
static void vsoc_test_handler(uint32_t type, uint32_t id, const uint8_t *data, uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)flags;
    char buf[VSOC_IPC_MAX_PAYLOAD + 1];
    uint32_t copy = len < VSOC_IPC_MAX_PAYLOAD ? len : VSOC_IPC_MAX_PAYLOAD;
    memcpy(buf, data, copy); buf[copy] = '\0';
    LOGI("IPC %s TEST id=%u '%s'", from_worker ? "from-worker" : "from-parent", id, buf);
    const char *ack = from_worker ? "WORKER-ACK" : "PARENT-ACK";
    uint32_t nid = id + 1;
    if (from_worker) vsoc_ipc_worker_send(VSOC_IPC_TYPE_TEST, nid, ack, strlen(ack), 0);
    else vsoc_ipc_parent_send(VSOC_IPC_TYPE_TEST, nid, ack, strlen(ack), 0);
}

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

static void *worker_log_reader(void *opaque) {
    int fd = GPOINTER_TO_INT(opaque);
    char buf[512];
    GString *line = g_string_new(NULL);
    for (;;) {
        ssize_t r = read(fd, buf, sizeof(buf));
        if (r <= 0) break;
        for (ssize_t i = 0; i < r; ++i) {
            char c = buf[i];
            if (c == '\n') {
                if (line->len) {
                    printf("[worker] %s\n", line->str);
                    g_string_truncate(line, 0);
                } else {
                    printf("[worker]\n");
                }
            } else if (c != '\r') {
                g_string_append_c(line, c);
            }
        }
    }
    if (line->len) {
        printf("[worker] %s\n", line->str);
    }
    g_string_free(line, TRUE);
    close(fd);
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

    int out_pipe[2];
    if (pipe(out_pipe) != 0) {
        LOGE("pipe failed: %s", strerror(errno));
        out_pipe[0] = out_pipe[1] = -1;
    }
    int err_pipe[2];
    if (pipe(err_pipe) != 0) {
        LOGE("pipe stderr failed: %s", strerror(errno));
        err_pipe[0] = err_pipe[1] = -1;
    }

    gchar *argv[] = { (gchar*)"vsoc-worker", (gchar*)g_shm_name, NULL };
    GError *error = NULL;
    gboolean ok = g_spawn_async_with_fds(NULL, argv, NULL, G_SPAWN_SEARCH_PATH,
                                         NULL, NULL, &g_worker_pid,
                                         -1,
                                         out_pipe[1],
                                         err_pipe[1],
                                         &error);
    if (out_pipe[1] >= 0) close(out_pipe[1]);
    if (err_pipe[1] >= 0) close(err_pipe[1]);
    if (!ok) {
        LOGE("failed to spawn vsoc-worker: %s", error ? error->message : "unknown");
        if (error) g_error_free(error);
        if (out_pipe[0] >= 0) close(out_pipe[0]);
        if (err_pipe[0] >= 0) close(err_pipe[0]);
        return;
    }
    // Start reader threads
    if (out_pipe[0] >= 0) {
        QemuThread t; // detached ephemeral thread
        qemu_thread_create(&t, "worker_stdout", worker_log_reader, GINT_TO_POINTER(out_pipe[0]), QEMU_THREAD_DETACHED);
    }
    if (err_pipe[0] >= 0) {
        QemuThread t2;
        qemu_thread_create(&t2, "worker_stderr", worker_log_reader, GINT_TO_POINTER(err_pipe[0]), QEMU_THREAD_DETACHED);
    }
    worker_started = true;
    LOGI("spawned vsoc-worker pid %d shm %s", (int)g_worker_pid, g_shm_name);
    // Initial test message
    vsoc_ipc_parent_send(VSOC_IPC_TYPE_TEST, 1, "PING", 4, 0);
}

void init_express_platform(ExpressPlatformOps ops) {
    g_ops = ops;
    spawn_worker_process();
}

void init_express_device(Express_Device_Info *info) {
    express_device_init_common(info);
}

void deinit_express_platform(void) {
    should_stop = true;
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
