#ifndef EM_RUIM_H
#define EM_RUIM_H

#define RUIM_NOT_FOUND -12344123

typedef struct Ruim Ruim;

// Ruim *ruim_create_empty(void);
Ruim *ruim_create_from_file(const char *filename);
// void ruim_save_to_file(Ruim *ruim);
int ruim_get_int(Ruim *ruim, const char *key, int defval);
const char *ruim_get_str(Ruim *ruim, const char *key, const char *defval);
void ruim_set_str(Ruim *ruim, const char *key, const char *value);

/* RUIM keys */

#define RUIM_OPER_NAME_INDEX                     "oper_name_index"
#define RUIM_OPER_INDEX                          "oper_index"
#define RUIM_SELECTION_MODE                      "selection_mode"
#define RUIM_OPER_COUNT                          "oper_count"
#define RUIM_MODEM_TECHNOLOGY                    "modem_technology"
#define RUIM_PREFERRED_MODE                      "preferred_mode"
#define RUIM_CDMA_SUBSCRIPTION_SOURCE            "cdma_subscription_source"
#define RUIM_CDMA_ROAMING_PREF                   "cdma_roaming_pref"
#define RUIM_IN_ECBM                             "in_ecbm"
#define RUIM_EMERGENCY_NUMBER_FMT                "emergency_number_%d"
#define RUIM_PRL_VERSION                         "prl_version"
#define RUIM_SREGISTER                           "sregister"
#define RUIM_SIGNAL_QUALITY                      "signal_quality"
#define RUIM_RSSI                                "rssi"
#define RUIM_AREA_CODE                           "area_code"
#define RUIM_CELL_ID                             "cell_id"
#define RUIM_OPERATOR_HOME_LONG_NAME             "operator_home_long_name"
#define RUIM_OPERATOR_HOME_SHORT_NAME            "operator_home_short_name"
#define RUIM_OPERATOR_HOME_MCCMNC                "operator_home_mccmnc"
#define RUIM_OPERATOR_ROAMING_LONG_NAME          "operator_roaming_long_name"
#define RUIM_OPERATOR_ROAMING_SHORT_NAME         "operator_roaming_short_name"
#define RUIM_OPERATOR_ROAMING_MCCMNC             "operator_roaming_mccmnc"
#define RUIM_VOICE_STATE                         "voice_state"
#define RUIM_DATA_STATE                          "data_state"
#define RUIM_DATA_NETWORK                        "data_network"
#define RUIM_VOICE_DOMAIN_PREFERENCE             "voice_domain_pref"

#define MAX_KEY_NAME 40

#endif