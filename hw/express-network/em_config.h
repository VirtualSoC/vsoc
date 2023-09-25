/* EM CONFIG FILE
 * This file defines default values of the ruim config file.
 */

/* the Android GSM stack checks that the operator's name has changed
 * when roaming is on. If not, it will not update the Roaming status icon
 *
 * this means that we need to emulate two distinct operators:
 * - the first one for the 'home' registration state, must also correspond
 *   to the emulated user's IMEI
 *
 * - the second one for the 'roaming' registration state, must have a
 *   different name and MCC/MNC
 */

#ifndef STRINGIFY
#define  _STRINGIFY(x)  #x
#define  STRINGIFY(x)  _STRINGIFY(x)
#endif

#define  DEFCONF_OPERATOR_HOME_INDEX 0
#define  DEFCONF_OPERATOR_HOME_MCC   460
#define  DEFCONF_OPERATOR_HOME_MNC   01
#define  DEFCONF_OPERATOR_HOME_NAME  "Unicom"
#define  DEFCONF_OPERATOR_HOME_MCCMNC  STRINGIFY(DEFCONF_OPERATOR_HOME_MCC) \
                               STRINGIFY(DEFCONF_OPERATOR_HOME_MNC)

#define  DEFCONF_OPERATOR_ROAMING_INDEX 1
#define  DEFCONF_OPERATOR_ROAMING_MCC   310
#define  DEFCONF_OPERATOR_ROAMING_MNC   295
#define  DEFCONF_OPERATOR_ROAMING_NAME  "midrivers"
#define  DEFCONF_OPERATOR_ROAMING_MCCMNC  STRINGIFY(DEFCONF_OPERATOR_ROAMING_MCC) \
                                  STRINGIFY(DEFCONF_OPERATOR_ROAMING_MNC)

#define DEFCONF_OPERATOR_HOME_LONG_NAME      DEFCONF_OPERATOR_HOME_NAME
#define DEFCONF_OPERATOR_HOME_SHORT_NAME     DEFCONF_OPERATOR_HOME_NAME
#define DEFCONF_OPERATOR_ROAMING_LONG_NAME   DEFCONF_OPERATOR_ROAMING_NAME
#define DEFCONF_OPERATOR_ROAMING_SHORT_NAME  DEFCONF_OPERATOR_ROAMING_NAME

#define DEFCONF_AREA_CODE 3
#define DEFCONF_CELL_ID   91

#define DEFCONF_MODEM_TECHNOLOGY "gsm"
#define DEFCONF_PREFERRED_MASK   0x0f

#define DEFCONF_SIGNAL_QUALITY  MODERATE

#define DEFCONF_CDMA_SUBSCRIPTION_SOURCE    A_SUBSCRIPTION_RUIM
#define DEFCONF_CDMA_ROAMING_PREF           A_ROAMING_PREF_ANY

#define DEFCONF_VOICE_STATE     A_REGISTRATION_HOME
#define DEFCONF_DATA_STATE      A_REGISTRATION_HOME
#define DEFCONF_DATA_NETWORK    A_DATA_NETWORK_LTE

#define DEFCONF_VOICE_DOMAIN_PREF   3
