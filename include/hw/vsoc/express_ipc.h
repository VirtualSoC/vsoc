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
    VSOC_IPC_TYPE_PLATFORM_INIT = 101,
    VSOC_IPC_TYPE_BUFFER_REGISTER = 102,
    VSOC_IPC_TYPE_SET_IRQ = 103,
    VSOC_IPC_TYPE_RAM_REGIONS = 104,
    VSOC_IPC_TYPE_GET_DEVICE_CONTEXT = 105,
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
#define VSOC_IPC_RING_SIZE    128
#define VSOC_IPC_MAX_PAYLOAD  16384

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

typedef struct VsocGuestMemSeg {
    uint64_t addr;      // parent-process VA of segment start (token in worker)
    uint32_t len;       // length of this segment
    uint32_t flags;
} VsocGuestMemSeg;

// When packing/unpacking Guest_Mem, we support read-only inline literal segments for
// cases where the parent HVA doesn't belong to guest RAM. We mark such segments by
// setting VsocGuestMemSeg.flags bit0. On unpack, we allocate and copy the bytes and
// tag the Guest_Mem scatter_data[i].iov_base pointer with the top bit to signal "inline".
#define VSOC_GM_SEG_FLAG_INLINE   0x1u

// Forward declaration to avoid heavy includes here
struct Guest_Mem;

// Pack a Guest_Mem into wire format: [num(uint32)][all_len(uint32)] + VsocGuestMemSeg[num]
// Returns number of bytes written on success; 0 on error (insufficient space or invalid input).
size_t vsoc_ipc_guest_mem_pack(uint8_t *dst, size_t cap, const struct Guest_Mem *gm);

// Unpack from wire format at src: [num(uint32)][all_len(uint32)] + VsocGuestMemSeg[num]
// Allocates a Guest_Mem and fills it; sets *out_consumed to total bytes consumed.
// Returns true on success; caller owns the returned Guest_Mem and must free its scatter_data and the struct.
bool vsoc_ipc_guest_mem_unpack(const uint8_t *src, size_t len, struct Guest_Mem **out_gm, size_t *out_consumed);

#endif // EXPRESS_IPC_H
