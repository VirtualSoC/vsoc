/**
 * @file express_modem.c
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "hw/express-network/express_modem.h"
#include "hw/express-network/em_core.h"
#include "hw/express-network/em_input.h"

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

#include "hw/express-network/express_bridge.h"

#include "qemu/sockets.h"
#include "qemu/thread.h"

#include "hw/express-network/modem/general.h"
#include "hw/express-network/modem/umts.h"

#define RIL_MODEM_PORT 28256

#ifndef STRINGIFY
#define  _STRINGIFY(x)  #x
#define  STRINGIFY(x)  _STRINGIFY(x)
#endif

void sync_express_modem_status(void)
{
    // The modem device is independent from kernel so there's no need to sync.
    return;
}

void express_modem_status_changed(Express_Modem *modem, int status)
{
    switch (status)
    {
        case EXPRESS_MODEM_RSSI:
            /* The extent to which RSSI can represent the signal strength 
               is closely related to the modem itself, so the calculation 
               method here is just for reference. */ 
            modem->rssi = (modem->rssi - (-113)) / 2;
            break;
        case EXPRESS_MODEM_RECEIVE_SMS:
            express_modem_receive_sms(modem, modem->input_from_number, modem->input_sms_str);
            break;
        default:
            break;
    }
}

static void
em_loop(int slot, int port)
{
    int fd;
    char addr_str[32];
    Error *err;

    sprintf(addr_str, "127.0.0.1:%d", port);
    while (true) {
        err = NULL;
        fd = inet_connect(addr_str, &err);
        if (fd >= 0) {
            LOGI("Connected to RIL on localhost:%d, fd=%d", port, fd);
            break;
        } else {
            LOGW("Retry connecting to RIL");
        }
    }

    Express_Modem *modem = &modems[slot];
    em_init(modem, slot, fd);

    while (true) {
        char c;
        int ret = recv(modem->serial, &c, 1, 0);
        if (ret <= 0) {
            LOGE("Error return value %d: %s", errno, strerror(errno));
            break;
        } else {
            // LOGD("Receive 1 byte from RIL");
        }

        if (modem->in_sms) {
            if (c != 26)
                goto AppendChar;

            modem->in_buff[ modem->in_pos ] = c;
            modem->in_pos++;
            modem->in_sms = 0;
            c = '\n';
        }

        if (c == '\n' || c == '\r') {
            const char*  answer;

            if (modem->in_pos == 0)  /* skip empty lines */
                continue;

            modem->in_buff[ modem->in_pos ] = 0;
            // print_command(modem->in_buff, modem->in_pos);
            LOGD("Command received: %s", modem->in_buff);
            modem->in_pos                = 0;

            LOGD( "%s: << %s\n", __FUNCTION__, modem->in_buff );
            answer = em_send(modem, modem->in_buff);
            if (answer != NULL) {
                LOGD( "%s: >> %s\n", __FUNCTION__, answer );
                int len = strlen(answer);
                if (len == 2 && answer[0] == '>' && answer[1] == ' ')
                    modem->in_sms = 1;

                send(modem->serial, (const uint8_t*)answer, len, 0);
                send(modem->serial, (const uint8_t*)"\r", 1, 0);

            } else
                LOGD( "%s: -- NO ANSWER\n", __FUNCTION__ );

            continue;
        }
    AppendChar:
        modem->in_buff[ modem->in_pos++ ] = c;
        if (modem->in_pos == sizeof(modem->in_buff)) {
            /* input is too long !! */
            modem->in_pos = 0;
        }
    }

    LOGE("RIL connection closed, restarting...");
}

static QemuThread em_thread_id[NR_MODEM];
static void *em_thread(void *opaque)
{
    unsigned long long slot = (unsigned long long)opaque;
    while (true) {
        em_loop(slot, RIL_MODEM_PORT + slot * 2);
    }
    return NULL;
}

static Express_Device_Info express_modem_info = {
    .enable_default = true,
    .name = "express-modem",
    .option_name = "modem",
    .device_id = EXPRESS_MODEM_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE | OUTPUT_DEVICE_TYPE,

    .static_prop = NULL,
    .static_prop_size = 0,
};

static void __attribute__((constructor))
express_thread_init_express_modem(void) {
    express_device_init_common(&express_modem_info);
    for (unsigned long long i = 0; i < NR_MODEM; ++i) {
        qemu_thread_create(&em_thread_id[i], "modem", 
                        em_thread, (void*)i, QEMU_THREAD_DETACHED);
    }
}
