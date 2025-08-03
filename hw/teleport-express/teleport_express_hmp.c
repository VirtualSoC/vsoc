/**
 * @file teleport_express_hmp.c
 * @author Jiaxing Qiu
 * @brief HMP commands for Teleport Express
 */

#include "hw/teleport-express/express_device_common.h"

#include "qemu/osdep.h"
#include "monitor/hmp.h"
#include "monitor/monitor.h"
#include "qapi/qmp/qdict.h"

#include <string.h>
#include <stdio.h>

#define MAX_ARGS 8 // Max 8 arguments

void hmp_vsoc(Monitor *mon, const QDict *qdict)
{
    const char *args_str = qdict_get_try_str(qdict, "args");

    if (!args_str) {
        monitor_printf(mon, "vSoC command requires subcommands. Try 'help vsoc'.\n");
        return;
    }

    char *args_copy = g_strdup(args_str);
    const char *argv[MAX_ARGS];
    int argc = 0;
    char *saveptr = NULL;
    char *token;

    for (token = strtok_r(args_copy, " ", &saveptr);
         token != NULL && argc < MAX_ARGS;
         token = strtok_r(NULL, " ", &saveptr)) {
        argv[argc++] = token;
    }

    if (argc == 0) {
        monitor_printf(mon, "No vSoC device specified.\n");
        g_free(args_copy);
        return;
    }

    const char *dev_name = argv[0];
    Express_Device_Info *info = get_express_device_info_by_name(dev_name);

    if (!info) {
        monitor_printf(mon, "vSoC device '%s' not found.\n", dev_name);
        g_free(args_copy);
        return;
    }

    if (info->hmp_handler) {
        info->hmp_handler(mon, argc - 1, argc > 1 ? &argv[1] : NULL);
    } else {
        monitor_printf(mon, "vSoC device '%s' does not support HMP commands.\n", dev_name);
    }

    g_free(args_copy);
}