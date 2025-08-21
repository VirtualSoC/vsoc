#pragma once

#include "hw/vsoc/express_platform.h"

void express_device_init_common(Express_Device_Info *info);
void *handle_thread_run(void *opaque);
