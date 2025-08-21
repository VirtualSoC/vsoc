#include <stdio.h>

#include "hw/teleport-express/express_platform.h"

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("vsoc stub starting\n");
    /* Initialize shared tables to verify we can call into the library. */
    ExpressPlatformOps ops;
    init_express_platform(ops);
    printf("vsoc init complete\n");
    return 0;
}
