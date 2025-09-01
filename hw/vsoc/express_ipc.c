#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include <errno.h>

#include "qemu/osdep.h"
#include "qemu/thread.h"

// change to LOGI to enable IPC call tracing (huge logs).
#define IPC_LOG LOGV

#define VSOC_IPC_MAX_PENDING 64

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

VsocGpuIpcShared *vsoc_ipc_shared = NULL;
static PendingReq g_pending[VSOC_IPC_MAX_PENDING];
static QemuMutex g_pending_table_lock; // protects allocation of slots & sequence
static uint32_t g_seq = 1; // sequence generator (skip 0)
static bool g_pending_inited = false;
// Serialize concurrent writers to each ring to avoid head races
static QemuMutex g_parent_send_lock;
static QemuMutex g_worker_send_lock;
// Ensure single-consumer semantics per ring; allow reentrant polling from same thread
static gint g_parent_consume_owner = 0;
static gint g_worker_consume_owner = 0;
static __thread int tls_parent_poll_depth = 0;
static __thread int tls_worker_poll_depth = 0;

static void vsoc_ipc_pending_init_once(void) {
    if (g_pending_inited) return;
    qemu_mutex_init(&g_pending_table_lock);
    qemu_mutex_init(&g_parent_send_lock);
    qemu_mutex_init(&g_worker_send_lock);
    for (int i=0;i<VSOC_IPC_MAX_PENDING;i++) {
        g_pending[i].id = 0;
        g_pending[i].done = false;
        qemu_cond_init(&g_pending[i].cond);
        qemu_mutex_init(&g_pending[i].lock);
    }
    g_pending_inited = true;
}


// ---------------- Handler registration & dispatch ----------------------------
typedef void (*VsocIpcHandler)(uint32_t type, uint32_t id, const uint8_t *data,
                               uint32_t len, uint32_t flags, bool from_worker);

typedef struct HandlerEntry { uint32_t type; VsocIpcHandler fn; } HandlerEntry;
static HandlerEntry g_handlers[16];
static int g_handler_count = 0;

void vsoc_ipc_register_handler(uint32_t type, VsocIpcHandler fn) {
    for (int i = 0; i < g_handler_count; ++i) {
        if (g_handlers[i].type == type) { g_handlers[i].fn = fn; return; }
    }
    if (g_handler_count < (int)(sizeof(g_handlers)/sizeof(g_handlers[0]))) {
        g_handlers[g_handler_count].type = type;
        g_handlers[g_handler_count].fn = fn;
        g_handler_count++;
    } else {
    LOGE("IPC handler table full (%s)", vsoc_ipc_type_name(type));
    }
}

static VsocIpcHandler vsoc_ipc_find_handler(uint32_t type) {
    for (int i = 0; i < g_handler_count; ++i)
        if (g_handlers[i].type == type) return g_handlers[i].fn;
    return NULL;
}

// Variable-size ring helpers (contiguous write/read via PAD messages)
static inline uint64_t ring_free_bytes(uint64_t head, uint64_t tail, uint32_t cap) {
    return (uint64_t)cap - (head - tail);
}
static inline uint64_t ring_data_bytes(uint64_t head, uint64_t tail) {
    return head - tail;
}

static bool vsoc_ipc_send_slot(uint8_t *ring, uint32_t cap, volatile uint64_t *headp, volatile uint64_t *tailp,
                               uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags) {
    if (!vsoc_ipc_shared) return false;
    if (len > VSOC_IPC_MAX_PAYLOAD) {
        LOGE("IPC len %u > max %u", len, (unsigned)VSOC_IPC_MAX_PAYLOAD);
        return false;
    }
    VsocIpcMsgHdr hdr = { .type = type, .id = id, .len = len, .flags = flags };
    uint32_t need = sizeof(hdr) + len;
    uint64_t head = qatomic_load_acquire(headp);
    uint64_t tail = qatomic_load_acquire(tailp); // snapshot
    if (need > cap) { LOGE("IPC message too large need=%u cap=%u", need, cap); return false; }
    // Ensure space for padding if wrap would split the message. We never write a PAD header;
    // instead we just advance head to the start, and the receiver skips to start when needed.
    uint32_t off = (uint32_t)(head % cap);
    uint32_t room_to_end = cap - off;
    uint32_t pad = (room_to_end < need) ? room_to_end : 0;
    if (ring_free_bytes(head, tail, cap) < need + pad) { LOGE("IPC ring full (%s)", vsoc_ipc_type_name(type)); return false; }

    if (pad) {
        if (room_to_end >= sizeof(VsocIpcMsgHdr)) {
            // Explicit PAD header so receiver can skip even when header fits but whole message wouldn't
            VsocIpcMsgHdr ph = { .type = VSOC_IPC_TYPE_PAD, .id = 0, .len = room_to_end - (uint32_t)sizeof(VsocIpcMsgHdr), .flags = 0 };
            memcpy(ring + off, &ph, sizeof(ph));
            // Optionally clear the remaining pad payload for debugging
            // if (ph.len) memset(ring + off + sizeof(ph), 0, ph.len);
            head += room_to_end;
            off = 0;
        } else {
            // Implicit pad (can't even fit a header), just advance to start
            head += room_to_end;
            off = 0;
        }
    }

    IPC_LOG("IPC: send %s id %u len %u flags %u head %llu->%llu tail %llu", vsoc_ipc_type_name(type), id, len, flags, (unsigned long long)head, (unsigned long long)(head + need), (unsigned long long)tail);

    // Now we have contiguous room for the full message at [off..off+need)
    memcpy(ring + off, &hdr, sizeof(hdr));
    if (len && data) memcpy(ring + off + sizeof(hdr), data, len);
    head += need;
    qatomic_store_release(headp, head);
    return true;
}

static bool vsoc_ipc_peek_slot(uint8_t *ring, uint32_t cap, volatile uint64_t *headp, volatile uint64_t *tailp,
                               uint32_t *out_type, uint32_t *out_id, const uint8_t **out_ptr, uint32_t *out_len, uint32_t *out_flags,
                               uint32_t *out_need) {
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
            // Implicit pad: only skip if producer has actually advanced head by 'room'
            if (ring_data_bytes(head, tail) < room) return false; // not enough produced bytes yet
            __sync_synchronize();
            tail += room;
            *tailp = tail;
            if (head == tail) return false; // nothing more yet
            continue;
        }
        if (ring_data_bytes(head, tail) < sizeof(VsocIpcMsgHdr)) return false; // header not complete yet
        VsocIpcMsgHdr hdr;
        memcpy(&hdr, ring + off, sizeof(hdr));
        uint32_t need = (uint32_t)sizeof(VsocIpcMsgHdr) + hdr.len;
        if (hdr.type == VSOC_IPC_TYPE_PAD) {
            if (room < need) return false; // corrupt or not produced fully yet
            if (ring_data_bytes(head, tail) < need) return false; // not complete yet
            // Explicit pad: advance tail past the PAD header+payload and look for the next real message
            __sync_synchronize();
            tail += need;
            *tailp = tail;
            if (head == tail) return false; // nothing more yet
            continue;
        }
        if (room < need) {
            // Writer should have inserted PAD; if not yet fully produced, wait.
            if (ring_data_bytes(head, tail) < room) return false;
            LOGE("IPC: corrupt message (type=%u, len=%u); dropping fragment to resync", hdr.type, hdr.len);
            __sync_synchronize();
            tail += room; // move to start safely (producer has advanced past end)
            *tailp = tail;
            return false;
        }
        if (ring_data_bytes(head, tail) < need) return false; // not complete yet
        *out_type = hdr.type;
        *out_id = hdr.id;
        *out_len = hdr.len;
        *out_flags = hdr.flags;
        *out_ptr = ring + off + sizeof(VsocIpcMsgHdr);
        if (out_need) *out_need = need;
        return true;
    }
}

static void vsoc_ipc_consume(uint32_t cap, volatile uint64_t *tailp, uint32_t need) {
    (void)cap;
    (void)__sync_synchronize();
    *tailp += need;
}

bool vsoc_ipc_parent_send(uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags) {
    vsoc_ipc_pending_init_once();
    bool ok;
    qemu_mutex_lock(&g_parent_send_lock);
    ok = vsoc_ipc_send_slot(vsoc_ipc_shared->parent_to_worker, VSOC_IPC_BUF_SIZE, &vsoc_ipc_shared->pw_head, &vsoc_ipc_shared->pw_tail,
                              type, id, data, len, flags);
    qemu_mutex_unlock(&g_parent_send_lock);
    return ok;
}

bool vsoc_ipc_worker_send(uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags) {
    vsoc_ipc_pending_init_once();
    bool ok;
    qemu_mutex_lock(&g_worker_send_lock);
    ok = vsoc_ipc_send_slot(vsoc_ipc_shared->worker_to_parent, VSOC_IPC_BUF_SIZE, &vsoc_ipc_shared->wp_head, &vsoc_ipc_shared->wp_tail,
                              type, id, data, len, flags);
    qemu_mutex_unlock(&g_worker_send_lock);
    return ok;
}

static bool vsoc_ipc_dispatch_one(bool from_worker_ring) {
    uint32_t type, id, flags, len, need; const uint8_t *ptr = NULL; bool ok;
    volatile uint64_t *headp = from_worker_ring ? &vsoc_ipc_shared->wp_head : &vsoc_ipc_shared->pw_head;
    volatile uint64_t *tailp = from_worker_ring ? &vsoc_ipc_shared->wp_tail : &vsoc_ipc_shared->pw_tail;
    uint8_t *ring = from_worker_ring ? vsoc_ipc_shared->worker_to_parent : vsoc_ipc_shared->parent_to_worker;
    ok = vsoc_ipc_peek_slot(ring, VSOC_IPC_BUF_SIZE, headp, tailp, &type, &id, &ptr, &len, &flags, &need);
    if (!ok) return false;

    IPC_LOG("IPC: dispatched to %s type %s id %u len %u flags %u head %llu tail %llu->%llu", from_worker_ring ? "parent" : "worker", vsoc_ipc_type_name(type), id, len, flags, (unsigned long long)*headp, (unsigned long long)*tailp, (unsigned long long)(*tailp + need));

    // If this is a response, first try to complete a pending request (applies to both parent and worker).
    if (flags & VSOC_IPC_FLAG_RESPONSE) {
        bool matched_pending = false;
        for (int i = 0; i < VSOC_IPC_MAX_PENDING; i++) {
            PendingReq *pr = &g_pending[i];
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
        // If no pending waiter, fall-through to dispatch to a registered handler for responses.
        if (!matched_pending) {
            VsocIpcHandler h = vsoc_ipc_find_handler(type);
            if (h) {
                h(type, id, ptr, len, flags, from_worker_ring);
            } else {
                LOGE("IPC: response %s id %u had no pending waiter and no handler; dropping", vsoc_ipc_type_name(type), id);
            }
        }
        // Now it is safe to release the bytes
        vsoc_ipc_consume(VSOC_IPC_BUF_SIZE, tailp, need);
        return true;
    }
    VsocIpcHandler h = vsoc_ipc_find_handler(type);
    if (h) {
        h(type, id, ptr, len, flags, from_worker_ring);
    } else {
        LOGE("IPC: no handler for %s", vsoc_ipc_type_name(type));
    }
    vsoc_ipc_consume(VSOC_IPC_BUF_SIZE, tailp, need);
    return true;
}

void vsoc_ipc_poll_parent(void) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return;
    bool acquired = false;
    tls_parent_poll_depth++;
    if (tls_parent_poll_depth == 1) {
        // First entry on this thread: try to become the sole consumer
        if (g_atomic_int_compare_and_exchange(&g_parent_consume_owner, 0, 1)) acquired = true;
        else { tls_parent_poll_depth--; return; }
    } else {
        // Re-entrant call from same thread: proceed without acquiring
        acquired = true;
    }
    while (vsoc_ipc_dispatch_one(true)) { /* keep draining until empty or partial */ }
    if (--tls_parent_poll_depth == 0 && acquired) {
        g_atomic_int_set(&g_parent_consume_owner, 0);
    }
}

void vsoc_ipc_poll_worker(void) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return;
    bool acquired = false;
    tls_worker_poll_depth++;
    if (tls_worker_poll_depth == 1) {
        if (g_atomic_int_compare_and_exchange(&g_worker_consume_owner, 0, 1)) acquired = true;
        else { tls_worker_poll_depth--; return; }
    } else {
        acquired = true;
    }
    while (vsoc_ipc_dispatch_one(false)) { /* keep draining until empty or partial */ }
    if (--tls_worker_poll_depth == 0 && acquired) {
        g_atomic_int_set(&g_worker_consume_owner, 0);
    }
}

void vsoc_ipc_poll_parent_bg(void) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return;
    vsoc_ipc_poll_parent();
}

void vsoc_ipc_poll_worker_bg(void) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return;
    vsoc_ipc_poll_worker();
}

bool vsoc_ipc_worker_respond(uint32_t type, uint32_t id, const void *data, uint32_t len) {
    return vsoc_ipc_worker_send(type, id, data, len, VSOC_IPC_FLAG_RESPONSE);
}

int vsoc_ipc_parent_request(uint32_t type,
                            const void *req, uint32_t req_len,
                            void *resp_buf, uint32_t *inout_resp_len,
                            uint32_t *inout_id,
                            int timeout_ms) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return -EIO;
    if (req_len > VSOC_IPC_MAX_PAYLOAD) return -EINVAL;
    if (inout_resp_len && *inout_resp_len > 0 && *inout_resp_len > VSOC_IPC_MAX_PAYLOAD) return -EINVAL;
    uint32_t id;
    qemu_mutex_lock(&g_pending_table_lock);
    if (inout_id && *inout_id) id = *inout_id; else {
        id = g_seq++;
        if (id == 0) id = g_seq++; // skip zero
        if (inout_id) *inout_id = id;
    }
    // find slot
    PendingReq *slot = NULL;
    for (int i=0;i<VSOC_IPC_MAX_PENDING;i++) if (g_pending[i].id == 0) { slot = &g_pending[i]; break; }
    if (!slot) { qemu_mutex_unlock(&g_pending_table_lock); return -ENOSPC; }
    slot->type = type; slot->done = false; slot->resp_buf = resp_buf; slot->resp_buf_cap = inout_resp_len ? *inout_resp_len : 0; slot->resp_len = 0;
    qemu_mutex_unlock(&g_pending_table_lock);
    qatomic_store_release(&slot->id, id);

    if (!vsoc_ipc_parent_send(type, id, req, req_len, 0)) {
    qemu_mutex_lock(&g_pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&g_pending_table_lock); return -EIO; }

    int elapsed = 0; const int step = 1; // ms
    for (;;) {
        // pump incoming responses
        vsoc_ipc_poll_parent();
        qemu_mutex_lock(&slot->lock);
        if (slot->done) { qemu_mutex_unlock(&slot->lock); break; }
        qemu_mutex_unlock(&slot->lock);
        if (timeout_ms > 0 && elapsed >= timeout_ms) {
            qemu_mutex_lock(&g_pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&g_pending_table_lock);
            return -ETIMEDOUT;
        }
        // sleep small slice
        g_usleep(1000 * step);
        elapsed += step;
    }
    // copy out result length
    if (inout_resp_len) *inout_resp_len = slot->resp_len;
    qemu_mutex_lock(&g_pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&g_pending_table_lock);
    return 0;
}

int vsoc_ipc_worker_request(uint32_t type,
                            const void *req, uint32_t req_len,
                            void *resp_buf, uint32_t *inout_resp_len,
                            uint32_t *inout_id,
                            int timeout_ms) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return -EIO;
    if (req_len > VSOC_IPC_MAX_PAYLOAD) return -EINVAL;
    if (inout_resp_len && *inout_resp_len > 0 && *inout_resp_len > VSOC_IPC_MAX_PAYLOAD) return -EINVAL;
    uint32_t id;
    qemu_mutex_lock(&g_pending_table_lock);
    if (inout_id && *inout_id) id = *inout_id; else {
        id = g_seq++;
        if (id == 0) id = g_seq++;
        if (inout_id) *inout_id = id;
    }
    PendingReq *slot = NULL;
    for (int i=0;i<VSOC_IPC_MAX_PENDING;i++) if (g_pending[i].id == 0) { slot = &g_pending[i]; break; }
    if (!slot) { qemu_mutex_unlock(&g_pending_table_lock); return -ENOSPC; }
    slot->type = type; slot->done = false; slot->resp_buf = resp_buf; slot->resp_buf_cap = inout_resp_len ? *inout_resp_len : 0; slot->resp_len = 0;
    qemu_mutex_unlock(&g_pending_table_lock);
    qatomic_store_release(&slot->id, id);

    if (!vsoc_ipc_worker_send(type, id, req, req_len, 0)) {
    qemu_mutex_lock(&g_pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&g_pending_table_lock); return -EIO; }

    int elapsed = 0; const int step = 1; // ms
    for (;;) {
        // pump incoming responses (from parent)
        vsoc_ipc_poll_worker();
        qemu_mutex_lock(&slot->lock);
        if (slot->done) { qemu_mutex_unlock(&slot->lock); break; }
        qemu_mutex_unlock(&slot->lock);
        if (timeout_ms > 0 && elapsed >= timeout_ms) {
            qemu_mutex_lock(&g_pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&g_pending_table_lock);
            return -ETIMEDOUT;
        }
        g_usleep(1000 * step);
        elapsed += step;
    }
    if (inout_resp_len) *inout_resp_len = slot->resp_len;
    qemu_mutex_lock(&g_pending_table_lock); qatomic_store_release(&slot->id, 0); qemu_mutex_unlock(&g_pending_table_lock);
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
    gm->is_gpa = payload_is_gpa ? 1 : 0;
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
