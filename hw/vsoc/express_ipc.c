#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"
#include <errno.h>

#include "qemu/osdep.h"
#include "qemu/thread.h"

#define IPC_LOG LOGD

#define VSOC_IPC_MAX_PENDING 64

// Human-readable IPC type names for logging
static const char *vsoc_ipc_type_name(uint32_t type) {
    switch (type) {
    case VSOC_IPC_TYPE_DEVICE_CALL:    return "DEVICE_CALL";
    case VSOC_IPC_TYPE_GET_CONTEXT:    return "GET_CONTEXT";
    case VSOC_IPC_TYPE_PLATFORM_INIT:  return "PLATFORM_INIT";
    case VSOC_IPC_TYPE_BUFFER_REGISTER:return "BUFFER_REGISTER";
    case VSOC_IPC_TYPE_SET_IRQ:        return "SET_IRQ";
    case VSOC_IPC_TYPE_RAM_REGIONS:    return "RAM_REGIONS";
    case VSOC_IPC_TYPE_GET_DEVICE_CONTEXT: return "GET_DEVICE_CONTEXT";
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

static inline bool ring_has_space(uint32_t head, uint32_t tail) {
    return (head - tail) < VSOC_IPC_RING_SIZE; // unsigned wrap logic
}
static inline bool ring_has_data(uint32_t head, uint32_t tail) {
    return head != tail;
}

static bool vsoc_ipc_send_slot(VsocIpcSlot *ring, volatile uint32_t *headp, volatile uint32_t *tailp,
                               uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags) {
    if (!vsoc_ipc_shared) return false;
    if (len > VSOC_IPC_MAX_PAYLOAD) {
        LOGE("IPC len %u > max %u", len, (unsigned)VSOC_IPC_MAX_PAYLOAD);
        return false;
    }
    uint32_t head = *headp;
    uint32_t tail = *tailp; // snapshot
    if (!ring_has_space(head, tail)) {
        LOGE("IPC ring full (%s)", vsoc_ipc_type_name(type));
        return false;
    }

    IPC_LOG("IPC: send %s slot %u len %u flags %u", vsoc_ipc_type_name(type), id, len, flags);

    uint32_t idx = head & (VSOC_IPC_RING_SIZE - 1);
    VsocIpcSlot *slot = &ring[idx];
    slot->len = len;
    if (len && data) memcpy((void*)slot->payload, data, len);
    slot->id = id;
    slot->flags = flags;
    // publish last
    __sync_synchronize();
    slot->type = type;
    __sync_synchronize();
    *headp = head + 1;
    return true;
}

static bool vsoc_ipc_recv_slot(VsocIpcSlot *ring, volatile uint32_t *headp, volatile uint32_t *tailp,
                               uint32_t *out_type, uint32_t *out_id, uint8_t *buf, uint32_t *inout_len, uint32_t *out_flags) {
    uint32_t head = *headp;
    uint32_t tail = *tailp;
    if (!ring_has_data(head, tail)) return false;
    uint32_t idx = tail & (VSOC_IPC_RING_SIZE - 1);
    VsocIpcSlot *slot = &ring[idx];
    uint32_t len = slot->len;
    uint32_t copy = (buf && *inout_len < len) ? *inout_len : len;
    if (buf && copy) memcpy(buf, (const void*)slot->payload, copy);
    *inout_len = len;
    *out_type = slot->type;
    *out_id = slot->id;
    *out_flags = slot->flags;
    __sync_synchronize();
    *tailp = tail + 1;
    return true;
}

bool vsoc_ipc_parent_send(uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags) {
    vsoc_ipc_pending_init_once();
    bool ok;
    qemu_mutex_lock(&g_parent_send_lock);
    ok = vsoc_ipc_send_slot(vsoc_ipc_shared->parent_to_worker, &vsoc_ipc_shared->pw_head, &vsoc_ipc_shared->pw_tail,
                              type, id, data, len, flags);
    qemu_mutex_unlock(&g_parent_send_lock);
    return ok;
}

bool vsoc_ipc_worker_send(uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags) {
    vsoc_ipc_pending_init_once();
    bool ok;
    qemu_mutex_lock(&g_worker_send_lock);
    ok = vsoc_ipc_send_slot(vsoc_ipc_shared->worker_to_parent, &vsoc_ipc_shared->wp_head, &vsoc_ipc_shared->wp_tail,
                              type, id, data, len, flags);
    qemu_mutex_unlock(&g_worker_send_lock);
    return ok;
}

static void vsoc_ipc_dispatch_one(bool from_worker_ring) {
    uint32_t type, id, flags; uint8_t tmp[VSOC_IPC_MAX_PAYLOAD]; uint32_t len = sizeof(tmp);
    bool ok;
    if (from_worker_ring) {
        ok = vsoc_ipc_recv_slot(vsoc_ipc_shared->worker_to_parent, &vsoc_ipc_shared->wp_head, &vsoc_ipc_shared->wp_tail,
                                 &type, &id, tmp, &len, &flags);
    } else {
        ok = vsoc_ipc_recv_slot(vsoc_ipc_shared->parent_to_worker, &vsoc_ipc_shared->pw_head, &vsoc_ipc_shared->pw_tail,
                                 &type, &id, tmp, &len, &flags);
    }
    if (!ok) return;

    IPC_LOG("IPC: dispatched %s slot %u len %u", vsoc_ipc_type_name(type), id, len);

    // If this is a response, complete a pending request (applies to both parent and worker).
    if (flags & VSOC_IPC_FLAG_RESPONSE) {
        for (int i=0;i<VSOC_IPC_MAX_PENDING;i++) {
            PendingReq *pr = &g_pending[i];
            if (pr->id == id && pr->type == type) {
                qemu_mutex_lock(&pr->lock);
                if (!pr->done) {
                    uint32_t copy = (pr->resp_buf && pr->resp_buf_cap < len) ? pr->resp_buf_cap : len;
                    if (pr->resp_buf && copy)
                        memcpy(pr->resp_buf, tmp, copy);
                    pr->resp_len = len;
                    pr->done = true;
                    qemu_cond_signal(&pr->cond);
                }
                qemu_mutex_unlock(&pr->lock);
                break;
            }
        }
        // Responses are for completing pending requests only; do not dispatch to handlers.
        return;
    }
    VsocIpcHandler h = vsoc_ipc_find_handler(type);
    if (h) {
        h(type, id, tmp, len, flags, from_worker_ring);
    } else {
        LOGE("IPC: no handler for %s", vsoc_ipc_type_name(type));
    }
}

void vsoc_ipc_poll_parent(void) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return;
    while (vsoc_ipc_shared->wp_head != vsoc_ipc_shared->wp_tail) vsoc_ipc_dispatch_one(true);
}

void vsoc_ipc_poll_worker(void) {
    vsoc_ipc_pending_init_once();
    if (!vsoc_ipc_shared) return;
    while (vsoc_ipc_shared->pw_head != vsoc_ipc_shared->pw_tail) vsoc_ipc_dispatch_one(false);
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
    slot->id = id; slot->type = type; slot->done = false; slot->resp_buf = resp_buf; slot->resp_buf_cap = inout_resp_len ? *inout_resp_len : 0; slot->resp_len = 0;
    qemu_mutex_unlock(&g_pending_table_lock);

    if (!vsoc_ipc_parent_send(type, id, req, req_len, 0)) {
        qemu_mutex_lock(&g_pending_table_lock); slot->id = 0; qemu_mutex_unlock(&g_pending_table_lock); return -EIO; }

    int elapsed = 0; const int step = 1; // ms
    for (;;) {
        // pump incoming responses
        vsoc_ipc_poll_parent();
        qemu_mutex_lock(&slot->lock);
        if (slot->done) { qemu_mutex_unlock(&slot->lock); break; }
        qemu_mutex_unlock(&slot->lock);
        if (timeout_ms > 0 && elapsed >= timeout_ms) {
            qemu_mutex_lock(&g_pending_table_lock); slot->id = 0; qemu_mutex_unlock(&g_pending_table_lock);
            return -ETIMEDOUT;
        }
        // sleep small slice
        g_usleep(1000 * step);
        elapsed += step;
    }
    // copy out result length
    if (inout_resp_len) *inout_resp_len = slot->resp_len;
    qemu_mutex_lock(&g_pending_table_lock); slot->id = 0; qemu_mutex_unlock(&g_pending_table_lock);
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
    slot->id = id; slot->type = type; slot->done = false; slot->resp_buf = resp_buf; slot->resp_buf_cap = inout_resp_len ? *inout_resp_len : 0; slot->resp_len = 0;
    qemu_mutex_unlock(&g_pending_table_lock);

    if (!vsoc_ipc_worker_send(type, id, req, req_len, 0)) {
        qemu_mutex_lock(&g_pending_table_lock); slot->id = 0; qemu_mutex_unlock(&g_pending_table_lock); return -EIO; }

    int elapsed = 0; const int step = 1; // ms
    for (;;) {
        // pump incoming responses (from parent)
        vsoc_ipc_poll_worker();
        qemu_mutex_lock(&slot->lock);
        if (slot->done) { qemu_mutex_unlock(&slot->lock); break; }
        qemu_mutex_unlock(&slot->lock);
        if (timeout_ms > 0 && elapsed >= timeout_ms) {
            qemu_mutex_lock(&g_pending_table_lock); slot->id = 0; qemu_mutex_unlock(&g_pending_table_lock);
            return -ETIMEDOUT;
        }
        g_usleep(1000 * step);
        elapsed += step;
    }
    if (inout_resp_len) *inout_resp_len = slot->resp_len;
    qemu_mutex_lock(&g_pending_table_lock); slot->id = 0; qemu_mutex_unlock(&g_pending_table_lock);
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
                segs[s].len = (uint32_t)sd->len;
                segs[s].addr = (uint64_t)(uintptr_t)sd->data; // GPA token
                segs[s].flags = 0;
            }
            // no inline payload when is_gpa==1
        } else {
            for (uint32_t s = 0; s < num; ++s) {
                const Scatter_Data *sd = &m->scatter_data[s];
                segs[s].len = (uint32_t)sd->len;
                segs[s].addr = 0; // ignored by worker
                segs[s].flags = VSOC_GM_SEG_FLAG_INLINE;
            }
            // Append inline bytes
            for (uint32_t s = 0; s < num; ++s) {
                const void *base = m->scatter_data[s].data;
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
        gm->scatter_data[s].len = segs[s].len;
        if (!payload_is_gpa) {
            if (p + segs[s].len > end) { g_free(gm->scatter_data); g_free(gm); return false; }
            void *buf = g_malloc(segs[s].len);
            memcpy(buf, p, segs[s].len);
            p += segs[s].len;
            gm->scatter_data[s].data = (unsigned char *)buf;
        } else {
            gm->scatter_data[s].data = (unsigned char *)(uintptr_t)segs[s].addr; // GPA token
        }
    }
    *out_gm = (struct Guest_Mem *)(void *)gm;
    if (out_consumed) *out_consumed = (size_t)(p - src);
    return true;
}
