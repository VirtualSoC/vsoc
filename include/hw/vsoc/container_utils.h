#include "hw/vsoc/express_platform.h"

bool device_needs_proxy(int device_id);
int wid_from_uid(uint64_t uid);
int wid_from_ids(uint64_t device_id, uint64_t unique_id, uint64_t uid);
int wid_from_thread_context(Thread_Context *context);
int wid_from_thread_context_uid(Thread_Context *ctx, uint64_t uid);
int wid_from_device_context(Device_Context *context);
