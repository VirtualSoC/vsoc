#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"


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
        LOGE("IPC handler table full (type %u)", type);
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
        LOGE("IPC ring full (type %u)", type);
        return false;
    }
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
    return vsoc_ipc_send_slot(vsoc_ipc_shared->parent_to_worker, &vsoc_ipc_shared->pw_head, &vsoc_ipc_shared->pw_tail,
                              type, id, data, len, flags);
}
bool vsoc_ipc_worker_send(uint32_t type, uint32_t id, const void *data, uint32_t len, uint32_t flags) {
    return vsoc_ipc_send_slot(vsoc_ipc_shared->worker_to_parent, &vsoc_ipc_shared->wp_head, &vsoc_ipc_shared->wp_tail,
                              type, id, data, len, flags);
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
    VsocIpcHandler h = vsoc_ipc_find_handler(type);
    if (h) h(type, id, tmp, len, flags, from_worker_ring);
    else LOGE("IPC: no handler for type %u", type);
}

void vsoc_ipc_poll_parent(void) {
    if (!vsoc_ipc_shared) return;
    while (vsoc_ipc_shared->wp_head != vsoc_ipc_shared->wp_tail) vsoc_ipc_dispatch_one(true);
}
void vsoc_ipc_poll_worker(void) {
    if (!vsoc_ipc_shared) return;
    while (vsoc_ipc_shared->pw_head != vsoc_ipc_shared->pw_tail) vsoc_ipc_dispatch_one(false);
}
