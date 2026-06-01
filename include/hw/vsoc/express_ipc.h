/* IPC interface for VSOC shared memory messaging between parent (QEMU) and worker. */
#ifndef EXPRESS_IPC_H
#define EXPRESS_IPC_H

#include <stdint.h>
#include <stdbool.h>

// Message type namespace (initial minimal set). Additional types should use
// values >= 100 to avoid colliding with reserved early values.
enum {
    VSOC_IPC_TYPE_PAD = 0,           // internal padding message; not dispatched
    VSOC_IPC_TYPE_TEST = 1,
    VSOC_IPC_TYPE_DEVICE_CALL = 2,
    VSOC_IPC_TYPE_GET_CONTEXT = 100,
    VSOC_IPC_TYPE_PLATFORM_INIT = 101,
    VSOC_IPC_TYPE_BUFFER_REGISTER = 102,
    VSOC_IPC_TYPE_SET_IRQ = 103,
    VSOC_IPC_TYPE_RAM_REGIONS = 104,
    VSOC_IPC_TYPE_GET_DEVICE_CONTEXT = 105,
    VSOC_IPC_TYPE_IRQ_REGISTER = 106,
    VSOC_IPC_TYPE_IRQ_RELEASE = 107,
    VSOC_IPC_TYPE_NOTIFY_SHUTDOWN = 108,
    VSOC_IPC_TYPE_FORCE_SHUTDOWN = 109,
    VSOC_IPC_TYPE_HMP_COMMAND = 110,
    VSOC_IPC_TYPE_FRAME_PACER_STATS = 111,    // worker -> parent: double ratio
    VSOC_IPC_TYPE_FRAME_PACER_K_UPDATE = 112, // parent -> worker: double K
};


// ---------------- Low-level shared IPC data structures (needed by worker & parent) ---------
// Variable-size ring buffers per direction. Each message is:
//   struct { uint32_t type, id, len; uint8_t payload[len]; }
// Head/tail are monotonically increasing byte counters; effective offset is modulo buffer size.
// Single producer and single consumer per ring.
#define VSOC_IPC_BUF_SIZE       (1024u * 1024)  // bulk request/notification ring per direction
#define VSOC_IPC_RESP_BUF_SIZE  (64u * 1024)          // dedicated small response ring per direction
#define VSOC_IPC_MAX_PAYLOAD    (64u * 1024)          // Upper bound for a single message payload

typedef struct VsocIpcMsgHdr {
    uint32_t type;
    uint32_t id;
    uint32_t len;
} VsocIpcMsgHdr;

// Generic ring declaration macro so we can have identical ring structs with different buffer sizes.
#define VSOC_IPC_RING_DECL(name, SZ) struct { \
    volatile uint64_t head; \
    volatile uint64_t tail; \
    volatile uint32_t doorbell; \
    uint8_t           buf[SZ]; \
} name

// Unified typed view of any ring. All concrete rings in VsocIpcShared start
// with this exact prefix (head, tail, doorbell, then an in-place byte buffer).
// We pass capacities separately, so the flexible array member is only used for
// pointer arithmetic / readability and does not define size.
typedef struct VsocIpcRing {
    volatile uint64_t head;
    volatile uint64_t tail;
    volatile uint32_t doorbell;
    uint8_t           buf[]; // flexible payload region (actual size depends on ring instance)
} VsocIpcRing;

typedef struct VsocIpcShared {
    volatile uint32_t parent_ready;
    volatile uint32_t worker_ready;
    // Bulk rings (requests / notifications)
    VSOC_IPC_RING_DECL(pw, VSOC_IPC_BUF_SIZE);      // parent -> worker
    VSOC_IPC_RING_DECL(wp, VSOC_IPC_BUF_SIZE);      // worker -> parent
    // Response-only rings
    VSOC_IPC_RING_DECL(pw_resp, VSOC_IPC_RESP_BUF_SIZE); // parent -> worker responses
    VSOC_IPC_RING_DECL(wp_resp, VSOC_IPC_RESP_BUF_SIZE); // worker -> parent responses
} VsocIpcShared;

// Global shared memory pointer (allocated & owned by parent, attached by worker).
extern VsocIpcShared *vsoc_ipc_shared;

// Forward declare opaque context for use in handler signature
struct VsocIpcContext;

// Handler receives a direct pointer into the ring for the payload. The memory
// is only valid during the handler call; do not retain the pointer beyond the
// call. If the handler needs to keep the data, it must copy it.
// The first argument is the IPC context this message belongs to.
typedef void (*VsocIpcHandler)(struct VsocIpcContext *ctx,
                               uint32_t type, uint32_t id, const uint8_t *data,
                               uint32_t len);

// Opaque IPC context for one IPC region (one worker link)
typedef struct VsocIpcContext VsocIpcContext;

// Create/destroy IPC context and map shared memory by name.
// When create=true (parent), creates/truncates the shm object to 'size' and initializes flags.
// When parent=false (worker), opens existing shm and marks worker_ready.
VsocIpcContext *vsoc_ipc_context_create(const char *name, size_t size, bool parent);
void vsoc_ipc_context_destroy(VsocIpcContext *ctx);

// Accessor for the shared memory backing of a context (useful on parent to check flags)
struct VsocIpcShared *vsoc_ipc_context_get_shared(VsocIpcContext *ctx);

// Register (or replace) a handler for a message type. Safe to call in both
// parent and worker after init_express_platform.
void vsoc_ipc_register_handler(uint32_t type, VsocIpcHandler fn);

// Unified non-blocking send helpers.
// Return false if ring full or invalid length.
bool vsoc_ipc_send(VsocIpcContext *ctx, uint32_t type, uint32_t id, const void *data, uint32_t len);

// Response send helper (places message on dedicated response ring in the
// direction opposite the originating request).
bool vsoc_ipc_send_response(VsocIpcContext *ctx, uint32_t type, uint32_t id, const void *data, uint32_t len);

// Blocking request helper. Generates a sequence id internally if *inout_id is 0.
// On success returns 0 and fills resp buffer/len. Timeout in ms; <=0 infinite.
// Returns -ETIMEDOUT, -ENOSPC, -EINVAL, or -EIO.
int vsoc_ipc_request(VsocIpcContext *ctx,
                     uint32_t type,
                     const void *req, uint32_t req_len,
                     void *resp_buf, uint32_t *inout_resp_len,
                     uint32_t *inout_id,
                     int timeout_ms);

// Poll to drain & dispatch pending incoming messages (both response and bulk).
void vsoc_ipc_poll(VsocIpcContext *ctx);

typedef struct VsocGuestMemSeg {
    uint64_t addr;      // parent-process VA of segment start (token in worker)
    uint32_t len;       // length of this segment
} VsocGuestMemSeg;

// Guest_Mem-level flags serialized in the wire header. Bit0 indicates that all
// segments are GPA-backed (is_gpa=1). If unset, payload bytes for all segments
// follow the segment table (inline data case).
#define VSOC_GM_FLAG_IS_GPA   0x1u

// Forward declaration to avoid heavy includes here
struct Guest_Mem;

// Pack a Guest_Mem into wire format: [num(uint32)][all_len(uint32)][gm_flags(uint32)] + VsocGuestMemSeg[num] + [inline bytes]
// Returns number of bytes written on success; 0 on error (insufficient space or invalid input).
size_t vsoc_ipc_guest_mem_pack(uint8_t *dst, size_t cap, const struct Guest_Mem *gm);

// Unpack from wire format at src: [num(uint32)][all_len(uint32)][gm_flags(uint32)] + VsocGuestMemSeg[num] + [inline bytes]
// Allocates a Guest_Mem and fills it; sets *out_consumed to total bytes consumed.
// Returns true on success; caller owns the returned Guest_Mem and must free its scatter_data and the struct.
bool vsoc_ipc_guest_mem_unpack(const uint8_t *src, size_t len, struct Guest_Mem **out_gm, size_t *out_consumed);

#endif // EXPRESS_IPC_H
