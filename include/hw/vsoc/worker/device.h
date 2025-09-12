#pragma once

#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"

void call_device_init(void);

void get_context_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                                    uint32_t len);

// Forward declaration for DEVICE_CALL handler implemented in device.c
void device_call_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                             uint32_t len);

// Lookup helper for worker -> parent Device_Context handle mapping
uint64_t worker_get_parent_handle_for_dc(Device_Context *dc);
