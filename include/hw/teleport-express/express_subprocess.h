#pragma once

#include "hw/teleport-express/express_platform.h"

void express_device_init_common(Express_Device_Info *info);
void *handle_thread_run(void *opaque);
