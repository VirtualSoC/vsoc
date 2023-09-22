/**
 * @file express_modem.h
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _modem_driver_h
#define _modem_driver_h

// #include "hw/express-network/em_core.h"
#include "hw/express-network/em_input.h"
#include "hw/express-network/em_sms.h"

// #include "hw/express-network/modem/call.h"
// #include "hw/express-network/modem/network.h"
// #include "hw/express-network/modem/mt.h"
// #include "hw/express-network/modem/sim.h"
// #include "hw/express-network/modem/sms.h"

#include <stdint.h>
#include <stdbool.h>

#define NR_MODEM 1

typedef struct Express_Modem Express_Modem;

/* Used in driver input window */
extern struct Express_Modem *modems;

enum {
    EXPRESS_MODEM_RSSI = 0,
    EXPRESS_MODEM_SIGNAL_QUALITY,
    EXPRESS_MODEM_OPERATOR_HOME,
    EXPRESS_MODEM_OPERATOR_ROAMING,
    EXPRESS_MODEM_AREA_CODE,
    EXPRESS_MODEM_CELL_ID,
    EXPRESS_MODEM_VOICE_STATE,
    EXPRESS_MODEM_DATA_STATE,
    EXPRESS_MODEM_DATA_NETWORK,
    EXPRESS_MODEM_FROM_NUMBER,
    EXPRESS_MODEM_INPUT_SMS_STR,
    EXPRESS_MODEM_RECEIVE_SMS
};

/* Operator status, see +COPS commands */
typedef enum {
    A_STATUS_UNKNOWN = 0,
    A_STATUS_AVAILABLE,
    A_STATUS_CURRENT,
    A_STATUS_DENIED
} AOperatorStatus;

typedef struct {
    AOperatorStatus  status;
    char             name[3][16];
} AOperatorRec, *AOperator;

void *express_modem_get_status_field(int slot, int status);
void express_modem_status_changed(int slot, int status);
void sync_express_modem_status(void);

#endif /* _modem_driver_h */
