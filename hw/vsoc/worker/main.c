#include <stdio.h>
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"

int main(int argc, char **argv)
{
    LOGI("vsoc worker starting argc %d", argc);
    if (argc > 1) {
        // argv[1] is shared memory name from parent.
        setenv("VSOC_GPU_SHM", argv[1], 1);
    }
    setenv("VSOC_WORKER", "1", 1);

    ExpressPlatformOps ops; // zero-initialized by default if static; ensure runtime zero.
    memset(&ops, 0, sizeof(ops));
    init_express_platform(ops);

    LOGI("vsoc worker init complete; entering idle loop");

    // For now idle spin waiting for stop flag; replace with event-driven loop later.
    for (;;) {
        if (platform_should_stop()) break;
        g_usleep(5 * 1000); // 5ms poll interval
    }

    LOGI("vsoc worker exiting");

    deinit_express_platform();
    return 0;
}
