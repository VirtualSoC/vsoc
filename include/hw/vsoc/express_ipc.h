/* IPC interface for VSOC shared memory messaging between parent (QEMU) and worker. */
#ifndef EXPRESS_IPC_H
#define EXPRESS_IPC_H

#include <stdint.h>
#include <stdbool.h>

// Message type namespace (initial minimal set). Additional types should use
// values >= 100 to avoid colliding with reserved early values.
enum {
    VSOC_IPC_TYPE_TEST = 1,
    VSOC_IPC_TYPE_DEVICE_CALL = 2,
    VSOC_IPC_TYPE_GET_CONTEXT = 100,
    // Guest memory proxy operations
    VSOC_IPC_TYPE_GMEM_READ = 101,
    VSOC_IPC_TYPE_GMEM_WRITE = 102,
    VSOC_IPC_TYPE_PLATFORM_INIT = 103,
};

// IPC message flags
#define VSOC_IPC_FLAG_RESPONSE 0x1

typedef void (*VsocIpcHandler)(uint32_t type, uint32_t id, const uint8_t *data,
                               uint32_t len, uint32_t flags, bool from_worker);

// Register (or replace) a handler for a message type. Safe to call in both
// parent and worker after init_express_platform.
void vsoc_ipc_register_handler(uint32_t type, VsocIpcHandler fn);

// Non-blocking send helpers. Return false if ring full or invalid length.
bool vsoc_ipc_parent_send(uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags);
bool vsoc_ipc_worker_send(uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags);

// Worker convenience for replying (adds RESPONSE flag automatically).
bool vsoc_ipc_worker_respond(uint32_t type, uint32_t id, const void *data, uint32_t len);

// Blocking parent-side request helper. Generates a sequence id internally if *inout_id is 0.
// On success returns 0 and fills resp buffer/len. Timeout in ms; <=0 means infinite (discouraged).
// Returns -ETIMEDOUT, -ENOSPC (no pending slots), -EINVAL (size), or -EIO (send failure / not initialized).
int vsoc_ipc_parent_request(uint32_t type,
                            const void *req, uint32_t req_len,
                            void *resp_buf, uint32_t *inout_resp_len,
                            uint32_t *inout_id,
                            int timeout_ms);

// Blocking worker-side request helper (mirror of parent_request). Used by
// worker to ask parent to perform privileged operations like guest memory IO.
int vsoc_ipc_worker_request(uint32_t type,
                            const void *req, uint32_t req_len,
                            void *resp_buf, uint32_t *inout_resp_len,
                            uint32_t *inout_id,
                            int timeout_ms);

// Poll to drain & dispatch pending messages. Parent should call regularly
// from main loop or a timer; worker calls inside its run loop.
void vsoc_ipc_poll_parent(void);
void vsoc_ipc_poll_worker(void);

// ---------------- Low-level shared IPC data structures (needed by worker & parent) ---------
#define VSOC_IPC_RING_SIZE    64
#define VSOC_IPC_MAX_PAYLOAD  1024

typedef struct VsocIpcSlot {
    volatile uint32_t type;
    volatile uint32_t id;
    volatile uint32_t len;
    volatile uint32_t flags;
    unsigned char payload[VSOC_IPC_MAX_PAYLOAD];
} VsocIpcSlot;

typedef struct VsocGpuIpcShared {
    volatile uint32_t parent_ready;
    volatile uint32_t worker_ready;
    volatile uint32_t pw_head;
    volatile uint32_t pw_tail;
    volatile uint32_t wp_head;
    volatile uint32_t wp_tail;
    VsocIpcSlot parent_to_worker[VSOC_IPC_RING_SIZE];
    VsocIpcSlot worker_to_parent[VSOC_IPC_RING_SIZE];
} VsocGpuIpcShared;

// Global shared memory pointer (allocated & owned by parent, attached by worker).
extern VsocGpuIpcShared *vsoc_ipc_shared;

// ---------------- Guest memory proxy payloads ----------------
// Header sent for GMEM_READ/WRITE requests followed by an array of segments.
// For READ: payload is only the header + segments; response carries 'length' bytes.
// For WRITE: payload is header + segments + trailing data buffer of 'length' bytes.
typedef struct VsocGuestMemRWReq {
    uint32_t num;       // number of segments in the scatter list
    uint32_t all_len;   // total logical length of the Guest_Mem object
    uint32_t offset;    // offset into the logical Guest_Mem to start IO
    uint32_t length;    // number of bytes to read/write
} VsocGuestMemRWReq;

typedef struct VsocGuestMemSeg {
    uint64_t addr;      // parent-process VA of segment start (token in worker)
    uint32_t len;       // length of this segment
    uint32_t _pad;      // reserved/pad for 16-byte alignment
} VsocGuestMemSeg;

#endif // EXPRESS_IPC_H
