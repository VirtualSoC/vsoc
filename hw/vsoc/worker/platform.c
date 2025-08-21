#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/gpu/express_gpu_main_window.h"
#include "hw/vsoc/express_event.h"

#include "qemu/osdep.h"
#include "qemu/thread.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// Worker-specific attachment logic separated from parent implementation.
// Parent provides spawning separately; worker just maps existing shared memory.

ExpressPlatformOps g_ops;
VsocGpuIpcShared *vsoc_ipc_shared; // ensure we have a definition if not already.
bool should_stop = false;

void qemu_system_killed(int signal, pid_t pid);
void qemu_system_killed(int signal, pid_t pid) {
    LOGI("subprocess qemu killed: signal=%d pid=%d", signal, pid);
}

static void attach_shared_memory(void) {
    if (vsoc_ipc_shared) return; // already attached
    const char *name = getenv("VSOC_GPU_SHM");
    if (!name) {
        LOGE("worker: VSOC_GPU_SHM not set (cannot attach shm)");
        return;
    }
    int fd = shm_open(name, O_RDWR, 0600);
    if (fd < 0) {
        LOGE("shm_open worker failed: %s", strerror(errno));
        return;
    }
    size_t shm_size = sizeof(VsocGpuIpcShared);
    void *addr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == MAP_FAILED) {
        LOGE("mmap worker failed: %s", strerror(errno));
        return;
    }
    vsoc_ipc_shared = (VsocGpuIpcShared*)addr;
    vsoc_ipc_shared->worker_ready = 1;
    LOGI("worker attached shared memory %s", name);
}

void monitor_log(Monitor *mon, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LOGW("warning! monitoring not implemented for device: %s", fmt);
    va_end(args);
}

void init_express_platform(ExpressPlatformOps ops) {
    g_ops = ops;
    attach_shared_memory();
}

void deinit_express_platform(void) {
    should_stop = true;
    // Consume residual messages
    express_gpu_shutdown_notify_callback();

    // Worker side: unmap only (parent unlinks)
    if (vsoc_ipc_shared) {
        size_t shm_size = sizeof(VsocGpuIpcShared);
        munmap(vsoc_ipc_shared, shm_size);
        vsoc_ipc_shared = NULL;
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

void *handle_thread_run(void *opaque);

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
