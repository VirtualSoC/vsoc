#pragma once

#include "hw/vsoc/express_platform.h"

void get_context_ipc_handler(uint32_t type, uint32_t id, const uint8_t *data,
                                    uint32_t len, uint32_t flags, bool from_worker);

void express_device_init_common(Express_Device_Info *info);