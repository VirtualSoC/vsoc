#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>

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

void platform_init_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                              uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)id; (void)flags; (void)from_worker;
    if (len != sizeof(ExpressPlatformOps)) {
        LOGE("PLATFORM_INIT wrong len %u expected %zu", len, sizeof(ExpressPlatformOps));
        return;
    }
    ExpressPlatformOps *ops = (ExpressPlatformOps *)data;
    // todo: implement the worker-side handlers
    ops->read_from_guest_mem = NULL;
    ops->write_to_guest_mem = NULL;
    ops->set_express_device_irq = NULL;
    ops->notify_shutdown = NULL;
    ops->force_shutdown = NULL;

    init_express_platform(*ops);
}

int main(int argc, char **argv)
{
    // Ensure logs flush quickly when stdout is a pipe (e.g., via tee)
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    LOGI("vsoc worker starting argc %d", argc);
    // Ensure worker terminates when parent dies (Linux)
    if (prctl(PR_SET_PDEATHSIG, SIGTERM) != 0) {
        LOGW("PR_SET_PDEATHSIG failed; worker may outlive parent");
    }
    // Parent may have already died between fork/exec and prctl
    if (getppid() == 1) {
        LOGW("parent already exited; quitting worker");
        return 0;
    }
    if (argc > 1) {
        // argv[1] is shared memory name from parent.
        setenv("VSOC_GPU_SHM", argv[1], 1);
    }
    setenv("VSOC_WORKER", "1", 1);

    attach_shared_memory();
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_PLATFORM_INIT, platform_init_ipc_handler);

    LOGI("vsoc worker init: entering event loop");

    // For now idle spin waiting for stop flag; replace with event-driven loop later.
    while (!platform_should_stop()) {
        // Fallback: if parent unexpectedly exits, PPID becomes 1
        if (getppid() == 1) {
            LOGW("detected parent death via PPID; stopping worker");
            break;
        }
        vsoc_ipc_poll_worker();
        g_usleep(1 * 1000); // 1ms poll interval
    }

    LOGI("vsoc worker exiting");

    deinit_express_platform();
    return 0;
}
