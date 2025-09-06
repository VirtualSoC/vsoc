#pragma once

#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_ipc.h"

void get_context_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                                    uint32_t len, uint32_t flags);

// Forward declaration for DEVICE_CALL handler implemented in device.c
void device_call_ipc_handler(VsocIpcContext *ctx, uint32_t type, uint32_t id, const uint8_t *data,
                             uint32_t len, uint32_t flags);
