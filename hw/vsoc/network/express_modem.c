/**
 * @file em_core.c
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-26
 *
 * @copyright Copyright (c) 2022
 *
 */


// #define STD_DEBUG_LOG
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/network/express_modem.h"
#include "hw/vsoc/network/em_core.h"
#include "hw/vsoc/network/em_config.h"

#include "qemu/sockets.h"
#include "qemu/thread.h"

Express_Modem *modems = NULL;
GMutex modems_mutex;

static QemuThread em_thread_id[NR_MODEM];

void sync_express_modem_status(void)
{
    // The modem device is independent from kernel so there's no need to sync.
    return;
}

void *express_modem_get_status_field(int slot, int status)
{
    Express_Modem *modem = &modems[slot];
    switch (status) {
        case EXPRESS_MODEM_SIGNAL_QUALITY:
            return &modem->quality;
        case EXPRESS_MODEM_OPERATOR_HOME:
            return &modem->operators[DEFCONF_OPERATOR_HOME_INDEX];
        case EXPRESS_MODEM_OPERATOR_ROAMING:
            return &modem->operators[DEFCONF_OPERATOR_ROAMING_INDEX];
        case EXPRESS_MODEM_AREA_CODE:
            return &modem->area_code;
        case EXPRESS_MODEM_CELL_ID:
            return &modem->cell_id;
        case EXPRESS_MODEM_VOICE_STATE:
            return &modem->voice_state;
        case EXPRESS_MODEM_DATA_STATE:
            return &modem->data_state;
        case EXPRESS_MODEM_DATA_NETWORK:
            return &modem->data_network_requested;
        case EXPRESS_MODEM_FROM_NUMBER:
            return &modem->input_from_number;
        case EXPRESS_MODEM_INPUT_SMS_STR:
            return &modem->input_sms_str;
        default:
            return NULL;
    }
}

void express_modem_status_changed(int slot, int status)
{
    Express_Modem *modem = &modems[slot];
    switch (status) {
        case EXPRESS_MODEM_RSSI:
            /* The extent to which RSSI can represent the signal strength 
               is closely related to the modem itself, so the calculation 
               method here is just for reference. */ 
            modem->rssi = (modem->rssi - (-113)) / 2;
            break;
        case EXPRESS_MODEM_RECEIVE_SMS:
            express_modem_receive_sms(modem, modem->input_from_number, modem->input_sms_str);
            break;
        case EXPRESS_MODEM_DATA_NETWORK:
            em_set_data_network_type(modem, dataNetworkTypeFromInt(modem->data_network_requested));
            break;
        case EXPRESS_MODEM_AREA_CODE:
        case EXPRESS_MODEM_CELL_ID:
            em_set_data_registration(modem, modem->data_state);
            em_set_voice_registration(modem, modem->voice_state);
            break;
        case EXPRESS_MODEM_DATA_STATE:
            em_set_data_registration(modem, modem->data_state);
            break;
        case EXPRESS_MODEM_VOICE_STATE:
            em_set_voice_registration(modem, modem->voice_state);
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
        }
    }

    Express_Modem *modem = &modems[slot];
    modem->serial = fd;
    modem->slot = slot;
    em_init(modem);

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

    LOGW("RIL connection closed, restarting...");
}

static void *em_thread(void *opaque)
{
    unsigned long long slot = (unsigned long long)opaque;
    while (true) {
        em_loop(slot, RIL_MODEM_PORT + slot * 2);
    }
    return NULL;
}

void express_modem_init(void) {
    bool need_init = false;
    g_mutex_lock(&modems_mutex);
    if (modems == NULL) {
        modems = g_malloc0(NR_MODEM * sizeof(Express_Modem));
        need_init = true;
    }
    g_mutex_unlock(&modems_mutex);

    if (need_init) {
        for (int i = 0; i < NR_MODEM; i++) {
            LOGI("express modem init");
            qemu_thread_create(&em_thread_id[i], "modem", 
                    em_thread, (void*)i, QEMU_THREAD_DETACHED);
        }
    }
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

EXPRESS_DEVICE_INIT(express_modem, &express_modem_info)
