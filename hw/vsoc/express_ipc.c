#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include <errno.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#include "qemu/osdep.h"
#include "qemu/thread.h"

// change to LOGI to enable IPC call tracing (huge logs).
#define IPC_LOG LOGV

#define VSOC_IPC_MAX_PENDING 64

typedef struct PendingReq {
    uint32_t id;
    uint32_t type;
    bool     done;
    uint32_t resp_len;
    uint8_t *resp_buf;
    uint32_t resp_buf_cap;
    QemuCond cond;
    QemuMutex lock;
} PendingReq;

typedef struct VsocIpcContext {
    VsocIpcShared *shared;
    bool is_parent; // remember creator role to infer direction
    PendingReq pending[VSOC_IPC_MAX_PENDING];
    QemuMutex pending_table_lock;
    uint32_t seq;
    QemuMutex send_lock;             // bulk send lock (this side -> other side)
    QemuMutex resp_send_lock;        // response send lock (this side -> other side)
    // Ownership flags so response draining can proceed concurrently with bulk draining.
    gint bulk_consume_owner;
    gint resp_consume_owner;
} VsocIpcContext;

// Linux futex helpers for INTER-PROCESS synchronization.
// NOTE: *_PRIVATE variants may not wake across processes because the kernel
// hashes them using the caller's mm. Since these futex words live in a shared
// memory segment mapped into different processes, we must use FUTEX_WAIT/WAKE
// (no _PRIVATE flag) so the hash key is the file+offset, not (mm,vaddr).
static inline int futex_wake32(volatile uint32_t *addr, int n) {
    return syscall(SYS_futex, addr, FUTEX_WAKE, n, NULL, NULL, 0);
}

static inline int futex_wait32(volatile uint32_t *addr, uint32_t val, int timeout_ms) {
    // If timeout_ms <= 0 we block indefinitely (until woken or spuriously returned).
    if (timeout_ms <= 0) {
        return syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
    }
    struct timespec ts;
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000L;
    return syscall(SYS_futex, addr, FUTEX_WAIT, val, &ts, NULL, 0);
}

// Helper to initialize a context (call once per region)
static void vsoc_ipc_context_init(VsocIpcContext *ctx, VsocIpcShared *shared) {
    ctx->shared = shared;
    ctx->seq = 1;
    ctx->bulk_consume_owner = 0;
    ctx->resp_consume_owner = 0;
    qemu_mutex_init(&ctx->pending_table_lock);
    qemu_mutex_init(&ctx->send_lock);
    qemu_mutex_init(&ctx->resp_send_lock);
    for (int i=0;i<VSOC_IPC_MAX_PENDING;i++) {
        ctx->pending[i].id = 0;
        ctx->pending[i].done = false;
        qemu_cond_init(&ctx->pending[i].cond);
        qemu_mutex_init(&ctx->pending[i].lock);
    }
}

// Platform-independent shared memory mapping and context creation
VsocIpcContext *vsoc_ipc_context_create(const char *name, size_t size, bool parent) {
    if (!name || !*name || size < sizeof(VsocIpcShared)) return NULL;
    int fd = -1;
    if (parent) {
        fd = shm_open(name, O_CREAT | O_RDWR, 0600);
        if (fd < 0) { LOGE("shm_open(parent) %s failed: %s", name, strerror(errno)); return NULL; }
        if (ftruncate(fd, (off_t)size) != 0) {
            LOGE("ftruncate(%s) failed: %s", name, strerror(errno)); close(fd); shm_unlink(name); return NULL;
        }
    } else {
        fd = shm_open(name, O_RDWR, 0600);
        if (fd < 0) { LOGE("shm_open(attach) %s failed: %s", name, strerror(errno)); return NULL; }
    }
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (addr == MAP_FAILED) { LOGE("mmap %s failed: %s", name, strerror(errno)); if (parent) shm_unlink(name); return NULL; }

    VsocIpcShared *shared = (VsocIpcShared*)addr;
    if (parent) {
        memset(shared, 0, sizeof(*shared));
        shared->parent_ready = 1;
    } else {
        shared->worker_ready = 1;
    }
    VsocIpcContext *ctx = (VsocIpcContext *)g_malloc0(sizeof(VsocIpcContext));
    vsoc_ipc_context_init(ctx, shared);
    ctx->is_parent = parent ? true : false;
    return ctx;
}

void vsoc_ipc_context_destroy(VsocIpcContext *ctx) {
    if (!ctx) return;
    g_free(ctx);
}

struct VsocIpcShared *vsoc_ipc_context_get_shared(VsocIpcContext *ctx) {
    return ctx ? ctx->shared : NULL;
}

// No default-context helpers; callers must pass explicit context.

// Human-readable IPC type names for logging
static const char *vsoc_ipc_type_name(uint32_t type) {
    switch (type) {
    case VSOC_IPC_TYPE_PAD:            return "PAD";
    case VSOC_IPC_TYPE_TEST:           return "TEST";
    case VSOC_IPC_TYPE_DEVICE_CALL:    return "DEVICE_CALL";
    case VSOC_IPC_TYPE_GET_CONTEXT:    return "GET_CONTEXT";
    case VSOC_IPC_TYPE_PLATFORM_INIT:  return "PLATFORM_INIT";
    case VSOC_IPC_TYPE_BUFFER_REGISTER:return "BUFFER_REGISTER";
    case VSOC_IPC_TYPE_SET_IRQ:        return "SET_IRQ";
    case VSOC_IPC_TYPE_RAM_REGIONS:    return "RAM_REGIONS";
    case VSOC_IPC_TYPE_GET_DEVICE_CONTEXT: return "GET_DEVICE_CONTEXT";
    case VSOC_IPC_TYPE_IRQ_REGISTER:   return "IRQ_REGISTER";
    case VSOC_IPC_TYPE_IRQ_RELEASE:    return "IRQ_RELEASE";
    case VSOC_IPC_TYPE_NOTIFY_SHUTDOWN:return "NOTIFY_SHUTDOWN";
    case VSOC_IPC_TYPE_FORCE_SHUTDOWN: return "FORCE_SHUTDOWN";
    default:                           return "UNKNOWN";
    }
}

VsocIpcShared *vsoc_ipc_shared = NULL; // retained for external mapping code only


// ---------------- Global handler registration & dispatch ---------------------
typedef void (*VsocIpcHandler)(VsocIpcContext *ctx,
                               uint32_t type, uint32_t id, const uint8_t *data,
                               uint32_t len);

// Use a hash table so register() can update handlers in-place and lookups are O(1).
static GHashTable *g_handler_table = NULL; // key: GUINT_TO_POINTER(type), value: (gpointer)VsocIpcHandler
static QemuMutex g_handler_lock;
static bool g_handler_lock_inited = false;
static inline void ensure_handler_lock_inited(void) {
    if (!g_handler_lock_inited) {
        qemu_mutex_init(&g_handler_lock);
        // g_direct_hash/equal treat keys as direct pointers; we store uint32 via GUINT_TO_POINTER
        g_handler_table = g_hash_table_new(g_direct_hash, g_direct_equal);
        g_handler_lock_inited = true;
    }
}

void vsoc_ipc_register_handler(uint32_t type, VsocIpcHandler fn) {
    ensure_handler_lock_inited();
    qemu_mutex_lock(&g_handler_lock);
    // Insert will replace any existing entry for 'type'
    g_hash_table_insert(g_handler_table, GUINT_TO_POINTER(type), (gpointer)fn);
    qemu_mutex_unlock(&g_handler_lock);
}

static VsocIpcHandler vsoc_ipc_find_handler(uint32_t type) {
    ensure_handler_lock_inited();
    qemu_mutex_lock(&g_handler_lock);
    gpointer val = g_hash_table_lookup(g_handler_table, GUINT_TO_POINTER(type));
    qemu_mutex_unlock(&g_handler_lock);
    return (VsocIpcHandler)val;
}

// Variable-size ring helpers (contiguous write/read via PAD messages)
static inline uint64_t ring_free_bytes(uint64_t head, uint64_t tail, uint32_t cap) {
    return (uint64_t)cap - (head - tail);
}
static inline uint64_t ring_data_bytes(uint64_t head, uint64_t tail) {
    return head - tail;
}

static bool vsoc_ipc_send_slot(VsocIpcRing *ring, uint32_t cap, uint32_t type, uint32_t id, const void *data, uint32_t len) {
    if (!ring) return false;
    volatile uint64_t *headp = &ring->head;
    volatile uint64_t *tailp = &ring->tail;
    if (len > VSOC_IPC_MAX_PAYLOAD) { LOGE("IPC len %u > max %u", len, (unsigned)VSOC_IPC_MAX_PAYLOAD); return false; }
    VsocIpcMsgHdr hdr = { .type = type, .id = id, .len = len };
    uint32_t need = sizeof(hdr) + len;
    uint64_t head = qatomic_load_acquire(headp);
    uint64_t tail = qatomic_load_acquire(tailp);
    if (need > cap) { LOGE("IPC message too large need=%u cap=%u", need, cap); return false; }
    uint32_t off = (uint32_t)(head % cap);
    uint32_t room_to_end = cap - off;
    uint32_t pad = (room_to_end < need) ? room_to_end : 0;
    if (ring_free_bytes(head, tail, cap) < need + pad) { 
        LOGE("IPC ring full (%s)", vsoc_ipc_type_name(type)); 
        return false; 
    }
    if (pad) {
        if (room_to_end >= sizeof(VsocIpcMsgHdr)) {
            VsocIpcMsgHdr ph = { .type = VSOC_IPC_TYPE_PAD, .id = 0, .len = room_to_end - (uint32_t)sizeof(VsocIpcMsgHdr) };
            memcpy(ring->buf + off, &ph, sizeof(ph));
            head += room_to_end; off = 0;
        } else { head += room_to_end; off = 0; }
    }
    IPC_LOG("IPC: send %s id %u len %u head %llu->%llu tail %llu", vsoc_ipc_type_name(type), id, len,
            (unsigned long long)head, (unsigned long long)(head + need), (unsigned long long)tail);
    memcpy(ring->buf + off, &hdr, sizeof(hdr));
    if (len && data) memcpy(ring->buf + off + sizeof(hdr), data, len);
    head += need;
    qatomic_store_release(headp, head);
    return true;
}

static bool vsoc_ipc_peek_slot(VsocIpcRing *ring, uint32_t cap,
                               uint32_t *out_type, uint32_t *out_id, const uint8_t **out_ptr,
                               uint32_t *out_len, uint32_t *out_need) {
    volatile uint64_t *headp = &ring->head;
    volatile uint64_t *tailp = &ring->tail;
    uint64_t head = qatomic_load_acquire(headp);
    uint64_t tail = qatomic_load_acquire(tailp);
    if (head < tail) {
        LOGE("IPC: head < tail (head=%llu tail=%llu), clamping tail to head", (unsigned long long)head, (unsigned long long)tail);
        qatomic_store_release((volatile uint64_t *)tailp, head);
        return false;
    }
    if (head == tail) return false;
    for (;;) {
        uint32_t off = (uint32_t)(tail % cap);
        uint32_t room = cap - off;
        if (room < sizeof(VsocIpcMsgHdr)) {
            if (ring_data_bytes(head, tail) < room) return false;
            __sync_synchronize();
            tail += room; *tailp = tail; 
            if (head == tail) return false; 
            continue;
        }
        if (ring_data_bytes(head, tail) < sizeof(VsocIpcMsgHdr)) return false;
        VsocIpcMsgHdr hdr; memcpy(&hdr, ring->buf + off, sizeof(hdr));
        uint32_t need = (uint32_t)sizeof(VsocIpcMsgHdr) + hdr.len;
        if (hdr.type == VSOC_IPC_TYPE_PAD) {
            if (room < need) return false; 
            if (ring_data_bytes(head, tail) < need) return false;
            __sync_synchronize(); tail += need; *tailp = tail; 
            if (head == tail) return false; 
            continue;
        }
        if (room < need) {
            if (ring_data_bytes(head, tail) < room) return false;
            LOGE("IPC: corrupt message (type=%u, len=%u); dropping fragment to resync", hdr.type, hdr.len);
            __sync_synchronize(); tail += room; *tailp = tail; return false;
        }
        if (ring_data_bytes(head, tail) < need) return false;
        *out_type = hdr.type; *out_id = hdr.id; *out_len = hdr.len; *out_ptr = ring->buf + off + sizeof(VsocIpcMsgHdr); 
        if (out_need) *out_need = need; 
        return true;
    }
}

static void vsoc_ipc_consume(uint32_t cap, volatile uint64_t *tailp, uint32_t need) {
    (void)cap;
    (void)__sync_synchronize();
    *tailp += need;
}

// Generic internal send helper selecting ring based on ctx->is_parent and response flag.
static bool vsoc_ipc_send_dir(VsocIpcContext *ctx, bool response_ring,
                              uint32_t type, uint32_t id, const void *data, uint32_t len) {
    // Select ring struct pointer and associated lock/capacity
    VsocIpcRing *ring;
    QemuMutex *lock;
    uint32_t cap;
    if (response_ring) {
        if (ctx->is_parent) { ring = (VsocIpcRing *)&ctx->shared->pw_resp; } else { ring = (VsocIpcRing *)&ctx->shared->wp_resp; }
        lock = &ctx->resp_send_lock; cap = VSOC_IPC_RESP_BUF_SIZE;
    } else {
        if (ctx->is_parent) { ring = (VsocIpcRing *)&ctx->shared->pw; } else { ring = (VsocIpcRing *)&ctx->shared->wp; }
        lock = &ctx->send_lock; cap = VSOC_IPC_BUF_SIZE;
    }
    // Treat ring struct uniformly (layout: head, tail, doorbell, buf[])
    bool ok;
    qemu_mutex_lock(lock);
    ok = vsoc_ipc_send_slot(ring, cap, type, id, data, len);
    qemu_mutex_unlock(lock);
    if (ok) {
        volatile uint32_t *doorbell = &ring->doorbell;
        (void)qatomic_fetch_add(doorbell, 1);
        (void)futex_wake32(doorbell, 1);
    }
    return ok;
}

bool vsoc_ipc_send(VsocIpcContext *ctx, uint32_t type, uint32_t id, const void *data, uint32_t len) {
    return vsoc_ipc_send_dir(ctx, false, type, id, data, len);
}

bool vsoc_ipc_send_response(VsocIpcContext *ctx, uint32_t type, uint32_t id, const void *data, uint32_t len) {
    return vsoc_ipc_send_dir(ctx, true, type, id, data, len);
}

// Dispatch a single message from the incoming ring (response or bulk) for this side.
static bool vsoc_ipc_dispatch_one(VsocIpcContext *ctx, bool response_ring) {
    uint32_t type, id, len, need; const uint8_t *ptr = NULL; bool ok;
    VsocIpcRing *ring;
    uint32_t cap = response_ring ? VSOC_IPC_RESP_BUF_SIZE : VSOC_IPC_BUF_SIZE;
    if (response_ring) {
        ring = ctx->is_parent ? (VsocIpcRing *)&ctx->shared->wp_resp : (VsocIpcRing *)&ctx->shared->pw_resp;
    } else {
        ring = ctx->is_parent ? (VsocIpcRing *)&ctx->shared->wp : (VsocIpcRing *)&ctx->shared->pw;
    }
    ok = vsoc_ipc_peek_slot(ring, cap, &type, &id, &ptr, &len, &need);
    volatile uint64_t *headp = &ring->head;
    volatile uint64_t *tailp = &ring->tail;
    if (!ok) return false;

    IPC_LOG("IPC: dispatched(%s) to %s type %s id %u len %u head %llu tail %llu->%llu",
        response_ring ? "resp," : "bulk,",
        ctx->is_parent ? "parent" : "worker", vsoc_ipc_type_name(type), id, len,
            (unsigned long long)*headp, (unsigned long long)*tailp, (unsigned long long)(*tailp + need));
    if (response_ring) {
        bool matched_pending = false;
        for (int i = 0; i < VSOC_IPC_MAX_PENDING; i++) {
            PendingReq *pr = &ctx->pending[i];
            uint32_t pid = qatomic_load_acquire(&pr->id);
            if (pid == id && pr->type == type) {
                qemu_mutex_lock(&pr->lock);
                if (!pr->done) {
                    uint32_t copy = (pr->resp_buf && pr->resp_buf_cap < len) ? pr->resp_buf_cap : len;
                    if (pr->resp_buf && copy)
                        memcpy(pr->resp_buf, ptr, copy);
                    pr->resp_len = len;
                    pr->done = true;
                    qemu_cond_signal(&pr->cond);
                }
                qemu_mutex_unlock(&pr->lock);
                matched_pending = true;
                break;
            }
        }
        if (!matched_pending) { VsocIpcHandler h = vsoc_ipc_find_handler(type); if (h) h(ctx, type, id, ptr, len); }
        vsoc_ipc_consume(cap, tailp, need);
        return true;
    }
    VsocIpcHandler h = vsoc_ipc_find_handler(type);
    if (h) h(ctx, type, id, ptr, len); else LOGE("IPC: no handler for %s", vsoc_ipc_type_name(type));
    vsoc_ipc_consume(cap, tailp, need);
    return true;
}

// Poll only the response ring (used by request threads to avoid reentrancy on bulk handlers)
static void vsoc_ipc_poll_responses(VsocIpcContext *ctx) {
    if (!ctx || !ctx->shared) return;

    gint *owner = &ctx->resp_consume_owner;
    if (!g_atomic_int_compare_and_exchange(owner, 0, 1)) return;
    while (vsoc_ipc_dispatch_one(ctx, true)) {}
    g_atomic_int_set(owner, 0);
}

void vsoc_ipc_poll(VsocIpcContext *ctx) {
    if (!ctx || !ctx->shared) return;

    // Drain responses first
    vsoc_ipc_poll_responses(ctx);

    gint *bulk_owner = &ctx->bulk_consume_owner;
    if (!g_atomic_int_compare_and_exchange(bulk_owner, 0, 1)) return;
    int budget = CALL_BUF_SIZE / 8; if (budget < 1) budget = 1;
    while (budget-- > 0 && vsoc_ipc_dispatch_one(ctx, false)) {}
    g_atomic_int_set(bulk_owner, 0);
}

// Unified request implementation; direction is derived from ctx->is_parent.
int vsoc_ipc_request(VsocIpcContext *ctx,
                     uint32_t type,
                     const void *req, uint32_t req_len,
                     void *resp_buf, uint32_t *inout_resp_len,
                     uint32_t *inout_id, int timeout_ms) {
    if (!ctx || !ctx->shared) return -EIO;
    bool initiator_is_parent = ctx->is_parent;
    if (req_len > VSOC_IPC_MAX_PAYLOAD) return -EINVAL;
    if (inout_resp_len && *inout_resp_len > 0 && *inout_resp_len > VSOC_IPC_MAX_PAYLOAD) return -EINVAL;
    uint32_t id;
    qemu_mutex_lock(&ctx->pending_table_lock);
    if (inout_id && *inout_id) id = *inout_id; else {
        id = ctx->seq++;
        if (id == 0) id = ctx->seq++; // skip zero
        if (inout_id) *inout_id = id;
    }
    PendingReq *slot = NULL;
    for (int i=0;i<VSOC_IPC_MAX_PENDING;i++) if (ctx->pending[i].id == 0) { slot = &ctx->pending[i]; break; }
    if (!slot) { qemu_mutex_unlock(&ctx->pending_table_lock); return -ENOSPC; }
    slot->type = type; slot->done = false; slot->resp_buf = resp_buf; slot->resp_buf_cap = inout_resp_len ? *inout_resp_len : 0; slot->resp_len = 0;
    qemu_mutex_unlock(&ctx->pending_table_lock);
    qatomic_store_release(&slot->id, id);

    bool send_ok = vsoc_ipc_send_dir(ctx, false, type, id, req, req_len);
    if (!send_ok) { qemu_mutex_lock(&ctx->pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&ctx->pending_table_lock); return -EIO; }

    volatile uint32_t *resp_doorbell = initiator_is_parent ? &ctx->shared->wp_resp.doorbell : &ctx->shared->pw_resp.doorbell;
    int elapsed = 0; const int step = 1;
    for (;;) {
        vsoc_ipc_poll_responses(ctx);
        qemu_mutex_lock(&slot->lock);
        if (slot->done) { qemu_mutex_unlock(&slot->lock); break; }
        qemu_mutex_unlock(&slot->lock);
        if (timeout_ms > 0 && elapsed >= timeout_ms) {
            qemu_mutex_lock(&ctx->pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&ctx->pending_table_lock);
            return -ETIMEDOUT;
        }
        uint32_t v = qatomic_read(resp_doorbell);
        (void)futex_wait32(resp_doorbell, v, timeout_ms > 0 ? (timeout_ms - elapsed) : 0);
        elapsed += step;
    }
    if (inout_resp_len) *inout_resp_len = slot->resp_len;
    qemu_mutex_lock(&ctx->pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&ctx->pending_table_lock);
    return 0;
}

// ---------------- Guest_Mem <-> VsocGuestMemSeg helpers ---------------------
size_t vsoc_ipc_guest_mem_pack(uint8_t *dst, size_t cap, const struct Guest_Mem *gm) {
    if (!dst || cap < 8) return 0;
    uint8_t *p = dst; uint8_t *end = dst + cap;
    uint32_t num = 0, all_len = 0;
    if (gm) {
        const Guest_Mem *m = (const Guest_Mem *)(const void *)gm;
    if (m->num < 0) return 0;
    num = (uint32_t)m->num; all_len = (uint32_t)m->all_len;
        size_t need = 8 + (size_t)num * sizeof(VsocGuestMemSeg);
        if ((size_t)(end - p) < need) return 0;
        memcpy(p, &num, 4); p += 4;
        memcpy(p, &all_len, 4); p += 4;
        VsocGuestMemSeg *segs = (VsocGuestMemSeg *)(void *)p;
        p += (size_t)num * sizeof(VsocGuestMemSeg);
        // First fill table, then append inline bytes when needed
        if (m->is_gpa) {
            for (uint32_t s = 0; s < num; ++s) {
                const Scatter_Data *sd = &m->scatter_data[s];
                segs[s].len = (uint32_t)sd->iov_len;
                segs[s].addr = (uint64_t)(uintptr_t)sd->iov_base; // GPA token
                segs[s].flags = 0;
            }
            // no inline payload when is_gpa==1
        } else {
            for (uint32_t s = 0; s < num; ++s) {
                const Scatter_Data *sd = &m->scatter_data[s];
                segs[s].len = (uint32_t)sd->iov_len;
                segs[s].addr = 0; // ignored by worker
                segs[s].flags = VSOC_GM_SEG_FLAG_INLINE;
            }
            // Append inline bytes
            for (uint32_t s = 0; s < num; ++s) {
                const void *base = m->scatter_data[s].iov_base;
                uint32_t slen = segs[s].len;
                if ((size_t)(end - p) < slen) return 0;
                memcpy(p, base, slen);
                p += slen;
            }
        }
        return (size_t)(p - dst);
    }
    // Null gm packs as num=0, all_len=0
    memcpy(p, &num, 4); p += 4;
    memcpy(p, &all_len, 4); p += 4;
    return (size_t)(p - dst);
}

bool vsoc_ipc_guest_mem_unpack(const uint8_t *src, size_t len, struct Guest_Mem **out_gm, size_t *out_consumed) {
    if (!src || len < 8 || !out_gm) return false;
    const uint8_t *p = src; const uint8_t *end = src + len;
    uint32_t num = 0, all_len = 0;
    memcpy(&num, p, 4); p += 4; memcpy(&all_len, p, 4); p += 4;
    size_t seg_bytes = (size_t)num * sizeof(VsocGuestMemSeg);
    if (p + seg_bytes > end) return false;
    Guest_Mem *gm = (Guest_Mem *)g_malloc0(sizeof(Guest_Mem));
    gm->num = (int)num; gm->all_len = (int)all_len;
    if (num) gm->scatter_data = (Scatter_Data *)g_malloc0(sizeof(Scatter_Data) * (size_t)num);
    const VsocGuestMemSeg *segs = (const VsocGuestMemSeg *)(const void *)p;
    p += seg_bytes;
    // Determine whether this payload is GPA-based or inline based on first segment flag
    bool payload_is_gpa = true;
    for (uint32_t s = 0; s < num; ++s) if (segs[s].flags & VSOC_GM_SEG_FLAG_INLINE) { payload_is_gpa = false; break; }
    gm->is_gpa = payload_is_gpa ? true : false;
    for (uint32_t s = 0; s < num; ++s) {
        gm->scatter_data[s].iov_len = segs[s].len;
        if (!payload_is_gpa) {
            if (p + segs[s].len > end) { g_free(gm->scatter_data); g_free(gm); return false; }
            void *buf = g_malloc(segs[s].len);
            memcpy(buf, p, segs[s].len);
            p += segs[s].len;
            gm->scatter_data[s].iov_base = buf;
        } else {
            gm->scatter_data[s].iov_base = (void *)(uintptr_t)segs[s].addr; // GPA token
        }
    }
    *out_gm = (struct Guest_Mem *)(void *)gm;
    if (out_consumed) *out_consumed = (size_t)(p - src);
    return true;
}
