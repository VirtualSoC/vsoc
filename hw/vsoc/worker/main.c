#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include "hw/vsoc/worker/guestmem.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/prctl.h>

static void attach_shared_memory(const char *name) {
    if (vsoc_ipc_shared) return; // already attached
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
    LOGD("worker attached shared memory %s", name);
}

// Handler for RAM region metadata from parent. FDs are inherited and referenced by number.
static void ram_regions_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                                   uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)id; (void)flags; (void)from_worker;
    if (len < 4) {
        LOGE("RAM_REGIONS: payload too small (%u)", len);
        return;
    }
    const uint8_t *p = data; const uint8_t *end = data + len;
    uint32_t count = 0; memcpy(&count, p, sizeof(count)); p += 4;
    const size_t entry_sz = 4 /*fd*/ + 4 /*pad*/ + 8 /*gpa*/ + 8 /*size*/ + 8 /*offset*/;
    if ((size_t)(end - p) != count * entry_sz) {
        LOGE("RAM_REGIONS: size mismatch count=%u payload=%zu expected=%zu", count, (size_t)(end - p), (size_t)count * entry_sz);
        return;
    }
    // Build a temporary array and atomically replace the mapping table.
    VsocGuestMemRegionInfo *infos = NULL;
    if (count) infos = (VsocGuestMemRegionInfo*)calloc(count, sizeof(*infos));
    uint32_t kept = 0;
    for (uint32_t i = 0; i < count; ++i) {
        int32_t fd = -1; uint32_t pad = 0; (void)pad;
        uint64_t gpa = 0, size = 0, off = 0;
        memcpy(&fd, p, 4); p += 4; memcpy(&pad, p, 4); p += 4;
        memcpy(&gpa, p, 8); p += 8; memcpy(&size, p, 8); p += 8; memcpy(&off, p, 8); p += 8;
        if (fd < 0) {
            LOGW("RAM_REGIONS: skip invalid fd=%d", fd);
            continue;
        }
        if (fcntl(fd, F_GETFD) == -1) {
            LOGE("RAM_REGIONS: inherited fd %d not valid in worker: %s", fd, strerror(errno));
            continue;
        }
        long page = sysconf(_SC_PAGESIZE);
        if (page > 0 && (off % (uint64_t)page) != 0) {
            LOGE("guestmem region fd=%d has non-page-aligned file_offset=%#llx (page=%ld)", fd, (unsigned long long)off, page);
        }
        infos[kept++] = (VsocGuestMemRegionInfo){ .fd = fd, .gpa_base = gpa, .size = size, .file_offset = off };
    }
    int rc = guestmem_replace_all(infos, kept);
    if (rc != 0) {
        LOGE("guestmem_replace_all failed rc=%d (kept=%u)", rc, kept);
    } else {
        LOGD("RAM_REGIONS: mapped %u regions", kept);
    }
    free(infos);
}

void platform_init_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                              uint32_t len, uint32_t flags, bool from_worker) {
    (void)type; (void)id; (void)flags; (void)from_worker;
    if (len != sizeof(ExpressPlatformOps)) {
        LOGE("PLATFORM_INIT wrong len %u expected %zu", len, sizeof(ExpressPlatformOps));
        return;
    }
    ExpressPlatformOps *ops = (ExpressPlatformOps *)data;
    // The ops struct arrived over IPC; any function pointers inside are invalid in this process.
    // Always replace with worker-safe implementations.
    ops->read_from_guest_mem = NULL;
    ops->write_to_guest_mem = NULL;
    ops->set_express_device_irq = NULL;
    ops->notify_shutdown = NULL;
    ops->force_shutdown = NULL;

    init_express_platform(*ops);
}

int main(int argc, char **argv)
{
    register_signal_handlers();
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    LOGD("vsoc worker starting argc %d", argc);
    // Ensure worker terminates when parent dies (Linux). Use SIGKILL so it cannot be ignored
    // and so we don't rely on any in-process handlers during catastrophic parent exits.
    if (prctl(PR_SET_PDEATHSIG, SIGKILL) != 0) {
        LOGW("PR_SET_PDEATHSIG failed; worker may outlive parent");
    }
    // Parent may have already died between fork/exec and prctl
    if (getppid() == 1) {
        LOGW("parent already exited; quitting worker");
        return 0;
    }
    if (argc < 2) {
        LOGE("usage: %s <shm_name>", argv[0]);
        return 1;
    }

    setenv("VSOC_WORKER", "1", 1);

    // Initialize guest memory subsystem (mutex, etc.).
    guestmem_init();

    attach_shared_memory(argv[1]);
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_PLATFORM_INIT, platform_init_ipc_handler);
    vsoc_ipc_register_handler(VSOC_IPC_TYPE_RAM_REGIONS, ram_regions_ipc_handler);

    LOGI("vsoc worker init: entering event loop");

    // For now idle spin waiting for stop flag; replace with event-driven loop later.
    while (!platform_should_stop()) {
        // Fallback: if parent unexpectedly exits, PPID becomes 1
        if (getppid() == 1) {
            LOGW("detected parent death via PPID; stopping worker");
            break;
        }
        vsoc_ipc_poll_worker_bg();
        // g_usleep(1 * 1000); // 1ms poll interval
    }

    LOGI("vsoc worker exiting");

    deinit_express_platform();
    guestmem_clear_all();
    return 0;
}
