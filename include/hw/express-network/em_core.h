/**
 * @file em_core.h
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef EM_CORE_H
#define EM_CORE_H

#include "hw/express-network/em_input.h"
#include "hw/express-network/em_sim_card.h"
#include "hw/express-network/em_remote_call.h"
#include "hw/express-network/em_sms.h"
#include "hw/express-network/em_utils.h"
#include "hw/express-network/em_ruim.h"

#include <stdbool.h>

// #include "sysdeps.h"

#define MODEM_DEV_STATE_SAVE_VERSION 2

#ifdef __cplusplus
extern "C" {
#endif

typedef void (ModemCallback)(void* user_data, int numActiveCalls);

/** MODEM OBJECT
 **/
typedef struct Express_Modem Express_Modem;

/* a function used by the modem to send unsolicited messages to the channel controller */
typedef void (*AModemUnsolFunc)( void*  opaque, const char*  message );

extern Express_Modem *em_create( int slot, AModemUnsolFunc  unsol_func, void*  unsol_opaque );
extern void        em_set_legacy( Express_Modem *modem );
extern void        em_destroy( Express_Modem *modem );
extern void        em_set_sim_present( Express_Modem *modem, int is_present );
extern void        em_set_notification_callback(Express_Modem *modem,
                                                    ModemCallback* callback_func,
                                                    void* user_data);

/* send a command to the modem */
extern const char*  em_send( Express_Modem *modem, const char*  cmd );

/** RADIO STATE
 **/
typedef enum {
    A_RADIO_STATE_OFF = 0,          /* Radio explictly powered off (eg CFUN=0) */
    A_RADIO_STATE_ON,               /* Radio on */
} ARadioState;

extern ARadioState  em_get_radio_state( Express_Modem *modem );
extern void         em_set_radio_state( Express_Modem *modem, ARadioState  state );

/** SIM CARD STATUS
 **/
extern ASimCard    em_get_sim( Express_Modem *modem );

/** VOICE AND DATA NETWORK REGISTRATION
 **/

/* 'stat' for +CREG/+CGREG commands */
typedef enum {
    A_REGISTRATION_UNREGISTERED = 0,
    A_REGISTRATION_HOME = 1,
    A_REGISTRATION_SEARCHING,
    A_REGISTRATION_DENIED,
    A_REGISTRATION_UNKNOWN,
    A_REGISTRATION_ROAMING
} ARegistrationState;

// The values in ADataNetworkType can be sent to the System in an
// 'AT+CREG' message to indicate 'networkType'. These values match enum
// RIL_RadioTechnology in the System's hardware/ril/include/telephony/ril.h
typedef enum {
    A_DATA_NETWORK_UNKNOWN =  0,   // RADIO_TECH_UNKNOWN
    A_DATA_NETWORK_GPRS,           // RADIO_TECH_GPRS
    A_DATA_NETWORK_EDGE,           // RADIO_TECH_EDGE
    A_DATA_NETWORK_UMTS,           // RADIO_TECH_UMTS
    A_DATA_NETWORK_LTE     = 14,   // RADIO_TECH_LTE
    A_DATA_NETWORK_NR      = 20,   // RADIO_TECH_NR
} ADataNetworkType;
// TODO: Merge the usage of these two structs and rename ADataNetworkType
typedef enum {
    A_TECH_GSM = 0,
    A_TECH_LTE,
    A_TECH_UNKNOWN // This must always be the last value in the enum
} AModemTech;

typedef enum {
    A_SUBSCRIPTION_NVRAM = 0,
    A_SUBSCRIPTION_RUIM,
    A_SUBSCRIPTION_UNKNOWN // This must always be the last value in the enum
} ACdmaSubscriptionSource;

typedef enum {
    A_ROAMING_PREF_HOME = 0,
    A_ROAMING_PREF_AFFILIATED,
    A_ROAMING_PREF_ANY,
    A_ROAMING_PREF_UNKNOWN // This must always be the last value in the enum
} ACdmaRoamingPref;

extern ARegistrationState  em_get_voice_registration( Express_Modem *modem );
extern void                em_set_voice_registration( Express_Modem *modem, ARegistrationState    state );

extern ARegistrationState  em_get_data_registration( Express_Modem *modem );
extern void                em_set_data_registration( Express_Modem *modem, ARegistrationState    state );
extern void                em_set_meter_state( Express_Modem *modem, int meteron );
extern void                em_set_data_network_type( Express_Modem *modem, ADataNetworkType   type );

extern ADataNetworkType    android_parse_network_type( const char*  speed );
extern AModemTech          android_parse_modem_tech( const char*  tech );
extern void                em_set_cdma_subscription_source( Express_Modem *modem, ACdmaSubscriptionSource ssource );
extern void                em_set_cdma_prl_version( Express_Modem *modem, int prlVersion);


/** OPERATOR NAMES
 **/
typedef enum {
    A_NAME_LONG = 0,
    A_NAME_SHORT,
    A_NAME_NUMERIC,
    A_NAME_MAX  /* don't remove */
} ANameIndex;

/* retrieve operator name into user-provided buffer. returns number of writes written, including terminating zero */
extern int   em_get_operator_name ( Express_Modem *modem, ANameIndex  index, char*  buffer, int  buffer_size );

/* reset one operator name from a user-provided buffer, set buffer_size to -1 for zero-terminated strings */
extern void  em_set_operator_name( Express_Modem *modem, ANameIndex  index, const char*  buffer, int  buffer_size );

/** CALL STATES
 **/

typedef enum {
    A_CALL_OUTBOUND = 0,
    A_CALL_INBOUND  = 1,
} ACallDir;

typedef enum {
    A_CALL_ACTIVE = 0,
    A_CALL_HELD,
    A_CALL_DIALING,
    A_CALL_ALERTING,
    A_CALL_INCOMING,
    A_CALL_WAITING
} ACallState;

typedef enum {
    A_CALL_VOICE = 0,
    A_CALL_DATA,
    A_CALL_FAX,
    A_CALL_UNKNOWN = 9
} ACallMode;

#define  A_CALL_NUMBER_MAX_SIZE  16

typedef struct {
    int         id;
    ACallDir    dir;
    ACallState  state;
    ACallMode   mode;
    int         multi;
    char        number[ A_CALL_NUMBER_MAX_SIZE+1 ];
} ACallRec, *ACall;

typedef enum {
    A_CALL_OP_OK = 0,
    A_CALL_NUMBER_NOT_FOUND = -1,
    A_CALL_EXCEED_MAX_NUM = -2,
    A_CALL_RADIO_OFF = -3,
} ACallOpResult;

extern int    em_get_call_count( Express_Modem *modem );
extern ACall  em_get_call( Express_Modem *modem,  int  index );
extern ACall  em_find_call_by_number( Express_Modem *modem, const char*  number );
extern int    em_add_inbound_call( Express_Modem *modem, const char*  number );
extern int    em_update_call( Express_Modem *modem, const char*  number, ACallState  state );
extern int    em_disconnect_call( Express_Modem *modem, const char*  number );

// extern void   em_state_save( Express_Modem *modem, SysFile* file );
// extern int    em_state_load( Express_Modem *modem, SysFile* file, int version_id);


/** return ">> %CTZV: yy/mm/dd,hh:mm:ss(+/-)tz[tzname]"
 *   mm is 0-based
 *   tz is in number of quarter-hours"
 *   tzname is optional and will be in the format of Area!Location
 **/
extern const char* em_send_unsol_nitz( Express_Modem *modem );

/**/

/* 'mode' for +CREG/+CGREG commands */
typedef enum {
    A_REGISTRATION_UNSOL_DISABLED     = 0,
    A_REGISTRATION_UNSOL_ENABLED      = 1,
    A_REGISTRATION_UNSOL_ENABLED_FULL = 2
} ARegistrationUnsolMode;

/* Operator selection mode, see +COPS commands */
typedef enum {
    A_SELECTION_AUTOMATIC,
    A_SELECTION_MANUAL,
    A_SELECTION_DEREGISTRATION,
    A_SELECTION_SET_FORMAT,
    A_SELECTION_MANUAL_AUTOMATIC
} AOperatorSelection;

/* General error codes for AT commands, see 3gpp.org document 27.007 */
typedef enum {
    kCmeErrorMemoryFull = 20,
    kCmeErrorInvalidIndex = 21,
    kCmeErrorInvalidCharactersInTextString = 25,
    kCmeErrorNoNetworkService = 30,
    kCmeErrorNetworkNotAllowedEmergencyCallsOnly = 32,
    kCmeErrorUnknownError = 100,
    kCmeErrorSelectionFailureEmergencyCallsOnly = 529,
} CmeErrorCode;

/* Command APDU instructions, see ETSI 102 221 and globalplatform.org's
 * Secure Elements Access Control (SEAC) document for more instructions. */
typedef enum {
    kSimApduGetData = 0xCA, // Global Platform SEAC section 4.1 GET DATA Command
    kSimApduSelect = 0xA4, // Command: SELECT
    kSimApduReadBinary = 0xB0, // Command: READ_BINARY
    kSimApduStatus = 0xF2, // Command: STATUS
    kSimApduManageChannel = 0x70, // Command: MANAGE_CHANNEL
} SimApduInstruction;

/* APDU class, see ETSI 102 221 and globalplatform.org's
 * Secure Elements Access Control (SEAC) document for more instructions. */
typedef enum {
    kSimApduClaGetResponse = 0x00, // CLA_GET_RESPONSE
    kSimApduClaManageChannel = 0x00, // CLA_MANAGE_CHANNEL
    kSimApduClaReadBinary = 0x00, // CLA_READ_BINARY
    kSimApduClaSelect = 0x00, // CLA_SELECT
    kSimApduClaStatus = 0x80, // CLA_STATUS
} SimApduClass;

typedef struct AVoiceCallRec {
    ACallRec    call;
    // XXX: disable timer here
    // SysTimer    timer;
    Express_Modem *modem;
    char        is_remote;
} AVoiceCallRec, *AVoiceCall;

#define  MAX_OPERATORS  4

typedef enum {
    A_DATA_IP = 0,
    A_DATA_PPP,
    A_DATA_IPV6,
    A_DATA_IPV4V6,
} ADataType;

#define  A_DATA_APN_SIZE  32

typedef struct {
    int        id;
    int        active;
    ADataType  type;
    char       apn[ A_DATA_APN_SIZE ];
    int        connected;
} ADataContextRec, *ADataContext;

/* the spec says that there can only be a max of 4 contexts */
#define  MAX_DATA_CONTEXTS  16
#define  MAX_CALLS          4
#define  MAX_EMERGENCY_NUMBERS 16
#define  MAX_LOGICAL_CHANNELS 16

#define  A_MODEM_SELF_SIZE   3

typedef struct _signal {
    int gsm_rssi;
    int gsm_ber;
    int cdma_dbm;
    int cdma_ecio;
    int evdo_dbm;
    int evdo_ecio;
    int evdo_snr;
    int lte_rssi;
    int lte_rsrp;
    int lte_rsrq;
    int lte_rssnr;
    int lte_cqi;
    int lte_timing;
} signal_t;

typedef enum {
    NONE = 0,
    POOR = 1,
    MODERATE = 2,
    GOOD = 3,
    GREAT = 4,
} signal_strength;

/*
 * Values derived from the ranges used in the SignalStrength
 * class in the frameworks/base telephony framework.
 */
static const signal_t NET_PROFILES[5] = {
    /* NONE */
    {0, 7, 105, 160, 110, 160, 0, 105, 140, 3, -200, 0, 500},
    /* POOR (one bar) */
    {5, 5, 100, 150, 100, 150, 2, 100, 110,  5, 0, 2, 300},
    /* MODERATE (2 bars) */
    {12, 4, 90, 120, 80, 120, 4, 90, 100, 10, 30, 7, 200},
    /* GOOD (3 bars) */
    {20, 2, 80, 100, 70, 100, 6, 70, 90, 15, 100, 12, 100},
    /* GREAT (4 bars) */
    {30, 0, 70, 80, 60, 80, 7, 60, 80, 20, 200, 15, 50},
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

/** MODEM OBJECT
 **/
typedef struct Express_Modem
{
    int slot;

    /* For communication with express bridge */
    int               serial;
    char              in_buff[1024];
    int               in_pos;
    int               in_sms;
    int               out_size;
    char              out_buff[1024];

    /* Legacy support */
    char          supportsNetworkDataType;
    char          snapshotTimeUpdateRequested;

    /* Radio state */
    ARadioState   radio_state;
    int           area_code;
    int           cell_id;
    int           base_port;

    int           send_phys_channel_cfg_unsol;

    /* Signal strength variables */
    int             use_signal_profile;
    signal_strength quality;
    int             rssi;
    int             ber;

    /* SMS */
    int           wait_sms;

    /* SIM card */
    ASimCard      sim;
    SmsReceiver   sms_receiver;

    /* voice and data network registration */
    ARegistrationUnsolMode   voice_mode;
    ARegistrationState       voice_state;
    ARegistrationUnsolMode   data_mode;
    ARegistrationState       data_state;
    ADataNetworkType         data_network;
    int                      data_network_requested;

    /* operator names */
    AOperatorSelection  oper_selection_mode;
    ANameIndex          oper_name_index;
    int                 oper_index;
    int                 oper_count;
    AOperatorRec        operators[ MAX_OPERATORS ];
    bool                has_allowed_carriers;
    bool                has_excluded_carriers;

    /* data connection contexts */
    ADataContextRec     data_contexts[ MAX_DATA_CONTEXTS ];

    /* call */
    AVoiceCallRec       calls[ MAX_CALLS ];
    int                 call_count;
    bool                ring_type_enabled;
    int                 voice_domain_pref;

    /*
    * Hold non-volatile ram configuration for modem
    */
    Ruim *ruim;

    // XXX: Deprecated in Huawei modem
    AModemTech technology;
    /*
    * This is are really 4 byte-sized prioritized masks.
    * Byte order gives the priority for the specific bitmask.
    * Each bit position in each of the masks is indexed by the different
    * A_TECH_XXXX values.
    * e.g. 0x01 means only GSM is set (bit index 0), whereas 0x0f
    * means that GSM,WCDMA,CDMA and EVDO are set
    */
    int32_t preferred_mask;
    ACdmaSubscriptionSource subscription_source;
    ACdmaRoamingPref roaming_pref;
    int in_emergency_mode;
    int prl_version;

    const char *emergency_numbers[MAX_EMERGENCY_NUMBERS];
    int nr_emergency_numbers;

    /*
    * Call-back function to receive notifications of
    * changes in status
    */
    ModemCallback* notify_call_back; // The function
    void*          notify_user_data; // Some opaque data to give the function

    /* Logical channels */
    struct {
        char* df_name;
        bool is_open;
        uint16_t file_id;
    } logical_channels[MAX_LOGICAL_CHANNELS];

    /* Used in device input */
    char input_from_number[32];
    char input_sms_str[1024];
} Express_Modem;

void express_modem_receive_sms(Express_Modem *modem, char *from, char *sms);
void em_init(Express_Modem *modem);
ADataNetworkType dataNetworkTypeFromInt(int type);

#ifdef __cplusplus
}
#endif

#endif