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

#include "hw/express-network/em_core.h"

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

#include "hw/express-network/em_input.h"
#include "hw/express-network/em_ruim.h"
#include "hw/express-network/express_modem.h"

#include "hw/express-network/em_remote_call.h"
#include "hw/express-network/em_sim_card.h"
#include "hw/express-network/em_sms.h"
#include "hw/express-network/em_utils.h"

#include "hw/express-network/express_bridge.h"

#include "qemu/sockets.h"
#include "qemu/thread.h"

#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define  CALL_DELAY_DIAL   1000
#define  CALL_DELAY_ALERT  1000

/* The interface exposed to QEMU command line arguments */
char *express_ruim_file = NULL;

#include "em_config.h"

#define EM_RET(str) return em_printf(modem, str) 

static const char* _em_switch_technology(Express_Modem *modem, AModemTech newtech, int32_t newpreferred,
        bool new_data_network);
static void adjustNetDataNetwork(Express_Modem *modem);
static int _em_set_cdma_subscription_source( Express_Modem *modem, ACdmaSubscriptionSource ss);
static int _em_set_cdma_prl_version( Express_Modem *modem, int prlVersion);

static const char*  quote( const char*  line )
{
    static char  temp[1024];
    const char*  hexdigits = "0123456789abcdef";
    char*        p = temp;
    int          c;

    while ((c = *line++) != 0) {
        c &= 255;
        if (c >= 32 && c < 127) {
            *p++ = c;
        }
        else if (c == '\r') {
            memcpy( p, "<CR>", 4 );
            p += 4;
        }
        else if (c == '\n') {
            memcpy( p, "<LF>", 4 );strcat( p, "<LF>" );
            p += 4;
        }
        else {
            p[0] = '\\';
            p[1] = 'x';
            p[2] = hexdigits[ (c) >> 4 ];
            p[3] = hexdigits[ (c) & 15 ];
            p += 4;
        }
    }
    *p = 0;
    return temp;
}

extern AModemTech
android_parse_modem_tech( const char * tech )
{
    const struct { const char* name; AModemTech  tech; }  techs[] = {
        { "gsm", A_TECH_GSM },
        { "lte", A_TECH_LTE },
        { NULL, 0 }
    };
    int  nn;

    for (nn = 0; techs[nn].name; nn++) {
        if (!strcmp(tech, techs[nn].name))
            return techs[nn].tech;
    }
    /* not found */
    return A_TECH_UNKNOWN;
}

extern ADataNetworkType
android_parse_network_type( const char*  speed )
{
    const struct { const char* name; ADataNetworkType  type; }  types[] = {
        { "gsm",   A_DATA_NETWORK_GPRS },
        { "hscsd", A_DATA_NETWORK_GPRS },
        { "gprs",  A_DATA_NETWORK_GPRS },
        { "edge",  A_DATA_NETWORK_EDGE },
        { "umts",  A_DATA_NETWORK_UMTS },
        { "hsdpa", A_DATA_NETWORK_UMTS },  /* not handled yet by Android GSM framework */
        { "lte",   A_DATA_NETWORK_LTE },
        { "full",  A_DATA_NETWORK_LTE },
        { "5g",   A_DATA_NETWORK_NR   },  /* non-standalone 5g, based on lte, there is no 5g sa yet */
        { NULL, 0 }
    };
    int  nn;

    if (!speed || !speed[0]) {
        // XXX: hardcoded here
        speed = "full";
    }

    for (nn = 0; types[nn].name; nn++) {
        if (!strcmp(speed, types[nn].name)){
            return types[nn].type;
        }
    }

    /* not found, be conservative */
    return A_DATA_NETWORK_GPRS;
}

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

/* send unsolicited messages to the device */
static void
em_unsol( Express_Modem *modem, const char* format, ... )
{
    va_list  args;
    va_start(args, format);
    int len = vsnprintf( modem->out_buff, sizeof(modem->out_buff), format, args );
    va_end(args);
    send(modem->serial, (const uint8_t*)format, len, 0);
}

// A SIM APDU data structure as described in the document ETSI TS 102 221
// "Smart Cards; UICC-Terminal interface; Physical and logical characteristics"
// available from https://www.etsi.org/
typedef struct SIM_APDU {
    uint8_t cla; // Class of instruction
    uint8_t instruction;
    uint8_t param1;
    uint8_t param2;
    uint8_t param3;
    char* data;
} SIM_APDU;


void em_set_notification_callback(Express_Modem *modem,
                                      ModemCallback* callback_func,
                                      void* user_data)
{
    modem->notify_call_back = callback_func;
    modem->notify_user_data = user_data;
}

static bool parseHexCharsToBuffer(const char* str, int length, char* output) {
    int i;
    for (i = 0; i < length; ++i) {
        int value = hex2int((const uint8_t*)str + i * 2, 2);
        if (value < 0) {
            return false;
        }
        output[i] = value;
    }
    return true;
}

// Free data allocated by parseSimApduCommand, note that this only frees the
// data contained in the struct, not the struct itself.
static void freeSimApduCommand(SIM_APDU* apdu) {
    free(apdu->data);
    apdu->data = NULL;
}

// Parse a command string into an APDU struct. After using the apdu struct
// the caller should call freeSimApduCommand to free the data allocated by
// this function.
static bool
parseSimApduCommand(const char* command, int length, SIM_APDU* apdu) {
    const uint8_t* commandData = (const uint8_t*)command;
    if (command == NULL || apdu == NULL || (unsigned) length != strlen(command)) {
        // Invalid or mismatching parameters
        return false;
    }
    if (length < 8) {
        // Less than minimal length for an APDU
        return false;
    }

    apdu->cla = hex2int(commandData, 2);
    apdu->instruction = hex2int(commandData + 2, 2);
    apdu->param1 = hex2int(commandData + 4, 2);
    apdu->param2 = hex2int(commandData + 6, 2);
    if (length > 8) {
        apdu->param3 = hex2int(commandData + 8, 2);
    }
    if (length > 10) {
        apdu->data = (char*)malloc(length - 10);
        parseHexCharsToBuffer(command + 10, length - 10, apdu->data);
    } else {
        apdu->data = NULL;
    }
    return true;
}

void
em_receive_sms( Express_Modem *modem, SmsPDU  sms )
{
#define  SMS_UNSOL_HEADER  "+CMT: 0\r\n"

    int    len, max;
    char*  p;

    strcpy( modem->out_buff, SMS_UNSOL_HEADER );
    p   = modem->out_buff + (sizeof(SMS_UNSOL_HEADER)-1);
    max = sizeof(modem->out_buff) - 3 - (sizeof(SMS_UNSOL_HEADER)-1);
    len = smspdu_to_hex( sms, p, max );
    if (len > max) /* too long */
        return;
    p[len]   = '\r';
    p[len+1] = '\n';
    p[len+2] = 0;

    LOGD( "SMS>> %s\n", p );

    em_unsol( modem, modem->out_buff );
}

static const char*
em_printf( Express_Modem *modem, const char*  format, ... )
{
    va_list  args;
    va_start(args, format);
    vsnprintf( modem->out_buff, sizeof(modem->out_buff), format, args );
    va_end(args);

    return modem->out_buff;
}

static void
em_begin_line( Express_Modem *modem )
{
    modem->out_size = 0;
}

static void
em_add_line( Express_Modem *modem, const char*  format, ... )
{
    va_list  args;
    va_start(args, format);
    modem->out_size += vsnprintf( modem->out_buff + modem->out_size,
                                  sizeof(modem->out_buff) - modem->out_size,
                                  format, args );
    va_end(args);
}

static const char*
em_end_line( Express_Modem *modem )
{
    modem->out_buff[ modem->out_size ] = 0;
    return modem->out_buff;
}

static ACdmaSubscriptionSource _em_get_cdma_subscription_source( Express_Modem *modem )
{
   int iss = -1;
   iss = ruim_get_int(modem->ruim, RUIM_CDMA_SUBSCRIPTION_SOURCE, DEFCONF_CDMA_SUBSCRIPTION_SOURCE);
   if (iss >= A_SUBSCRIPTION_UNKNOWN || iss < 0) {
       iss = A_SUBSCRIPTION_RUIM;
   }

   return iss;
}

static ACdmaRoamingPref _em_get_cdma_roaming_preference( Express_Modem *modem )
{
   int rp = -1;
   rp = ruim_get_int(modem->ruim, RUIM_CDMA_ROAMING_PREF, DEFCONF_CDMA_ROAMING_PREF);
   if (rp >= A_ROAMING_PREF_UNKNOWN || rp < 0) {
       rp = A_ROAMING_PREF_ANY;
   }

   return rp;
}

static ADataNetworkType
dataNetworkTypeFromInt(int type)
{
    switch (type) {
        case 1: return A_DATA_NETWORK_GPRS;
        case 2: return A_DATA_NETWORK_EDGE;
        case 3: return A_DATA_NETWORK_UMTS;
        case 4: return A_DATA_NETWORK_LTE;
        case 5: return A_DATA_NETWORK_NR;
        default: return A_DATA_NETWORK_UNKNOWN;
    }
}

static int
intFromDataNetworkType(ADataNetworkType type)
{
    switch (type) {
        case A_DATA_NETWORK_GPRS: return 1;
        case A_DATA_NETWORK_EDGE: return 2;
        case A_DATA_NETWORK_UMTS: return 3;
        case A_DATA_NETWORK_LTE: return 4;
        case A_DATA_NETWORK_NR: return 5;
        default: return 0;
    }
}

static void
em_reset( Express_Modem *modem )
{
    const char *tmp;
    int i;
    modem->ruim = ruim_create_from_file(express_ruim_file ? express_ruim_file : "./ruim.txt");
    modem->radio_state = A_RADIO_STATE_OFF;
    modem->send_phys_channel_cfg_unsol = 0;
    modem->wait_sms    = 0;
    
    strcpy(modem->input_from_number, "(+86)15638566351");
    strcpy(modem->input_sms_str, "Hello OpenHarrmony OS!");

    modem->use_signal_profile = 1;
    modem->quality = ruim_get_int(modem->ruim, RUIM_SIGNAL_QUALITY, DEFCONF_SIGNAL_QUALITY);    // Two signal strength bars
    modem->rssi = ruim_get_int(modem->ruim, RUIM_RSSI, 7);   // Two signal strength bars
    modem->ber = 0;  

    modem->oper_name_index     = ruim_get_int(modem->ruim, RUIM_OPER_NAME_INDEX, 2);
    modem->oper_selection_mode = ruim_get_int(modem->ruim, RUIM_SELECTION_MODE, A_SELECTION_AUTOMATIC);
    modem->oper_index          = ruim_get_int(modem->ruim, RUIM_OPER_INDEX, 0);
    modem->oper_count          = ruim_get_int(modem->ruim, RUIM_OPER_COUNT, 2);
    modem->in_emergency_mode   = ruim_get_int(modem->ruim, RUIM_IN_ECBM, 0);
    modem->prl_version         = ruim_get_int(modem->ruim, RUIM_PRL_VERSION, 0);

    modem->emergency_numbers[0] = "911";
    modem->emergency_numbers[1] = "110";
    modem->emergency_numbers[2] = "119";
    modem->emergency_numbers[3] = "120";
    modem->nr_emergency_numbers = 4;
    char key_name[MAX_KEY_NAME + 1];
    for (i = 4; i < MAX_EMERGENCY_NUMBERS; i++) {
        snprintf(key_name,MAX_KEY_NAME, RUIM_EMERGENCY_NUMBER_FMT, i);
        const char *str = ruim_get_str(modem->ruim, key_name, NULL);
        if (str) {
            modem->nr_emergency_numbers++;
        }
        modem->emergency_numbers[i] = ruim_get_str(modem->ruim, key_name, NULL);
    }

    modem->area_code = ruim_get_int(modem->ruim, RUIM_AREA_CODE, DEFCONF_AREA_CODE);
    modem->cell_id   = ruim_get_int(modem->ruim, RUIM_CELL_ID, DEFCONF_CELL_ID);

    tmp = ruim_get_str(modem->ruim, RUIM_OPERATOR_HOME_LONG_NAME, DEFCONF_OPERATOR_HOME_LONG_NAME);
    strcpy(modem->operators[DEFCONF_OPERATOR_HOME_INDEX].name[0], tmp);
    tmp = ruim_get_str(modem->ruim, RUIM_OPERATOR_HOME_SHORT_NAME, DEFCONF_OPERATOR_HOME_SHORT_NAME);
    strcpy(modem->operators[DEFCONF_OPERATOR_HOME_INDEX].name[1], tmp);
    tmp = ruim_get_str(modem->ruim, RUIM_OPERATOR_HOME_MCCMNC, DEFCONF_OPERATOR_HOME_MCCMNC);
    strcpy(modem->operators[DEFCONF_OPERATOR_HOME_INDEX].name[2], tmp);
    modem->operators[0].status        = A_STATUS_AVAILABLE;

    tmp = ruim_get_str(modem->ruim, RUIM_OPERATOR_ROAMING_LONG_NAME, DEFCONF_OPERATOR_ROAMING_LONG_NAME);
    strcpy(modem->operators[DEFCONF_OPERATOR_ROAMING_INDEX].name[0], tmp);
    tmp = ruim_get_str(modem->ruim, RUIM_OPERATOR_ROAMING_SHORT_NAME, DEFCONF_OPERATOR_ROAMING_SHORT_NAME);
    strcpy(modem->operators[DEFCONF_OPERATOR_ROAMING_INDEX].name[1], tmp);
    tmp = ruim_get_str(modem->ruim, RUIM_OPERATOR_ROAMING_MCCMNC, DEFCONF_OPERATOR_ROAMING_MCCMNC);
    strcpy(modem->operators[DEFCONF_OPERATOR_ROAMING_INDEX].name[2], tmp);
    modem->operators[1].status        = A_STATUS_AVAILABLE;

    modem->has_allowed_carriers = false;
    modem->has_excluded_carriers = false;

    modem->voice_mode   = A_REGISTRATION_UNSOL_ENABLED_FULL;
    modem->voice_state  = ruim_get_int(modem->ruim, RUIM_VOICE_STATE, DEFCONF_VOICE_STATE);
    modem->data_mode    = A_REGISTRATION_UNSOL_ENABLED_FULL;
    modem->data_state   = ruim_get_int(modem->ruim, RUIM_DATA_STATE, DEFCONF_DATA_STATE);
    modem->data_network = ruim_get_int(modem->ruim, RUIM_DATA_NETWORK, DEFCONF_DATA_NETWORK);
    modem->data_network_requested = intFromDataNetworkType(modem->data_network);

    tmp = ruim_get_str(modem->ruim, RUIM_MODEM_TECHNOLOGY, DEFCONF_MODEM_TECHNOLOGY );
    modem->technology = android_parse_modem_tech( tmp );
    if (modem->technology == A_TECH_UNKNOWN) {
        ruim_set_str(modem->ruim, RUIM_MODEM_TECHNOLOGY, DEFCONF_MODEM_TECHNOLOGY);
        modem->technology = A_TECH_GSM;
    }
    // Support GSM, WCDMA, CDMA, EvDo
    modem->preferred_mask = ruim_get_int(modem->ruim, RUIM_PREFERRED_MODE, DEFCONF_PREFERRED_MASK );

    modem->subscription_source = _em_get_cdma_subscription_source( modem );
    modem->roaming_pref = _em_get_cdma_roaming_preference( modem );

    modem->voice_domain_pref = ruim_get_int(modem->ruim, RUIM_VOICE_DOMAIN_PREFERENCE, DEFCONF_VOICE_DOMAIN_PREF);

    // Clear out all logical channels, none of them are open, they have no names
    memset(modem->logical_channels, 0, sizeof(modem->logical_channels));
    // channel 0 is the basic channel and it is always open
    modem->logical_channels[0].is_open = true;
    modem->logical_channels[0].df_name = strdup("");
    modem->logical_channels[0].file_id = 0x3F00;
}

static AVoiceCall em_alloc_call( Express_Modem *modem );
static void em_free_call( Express_Modem *modem, AVoiceCall  call );

/*
void em_state_save(Express_Modem *modem, SysFile* file)
{
    // TODO: save more than just calls and call_count - rssi, power, etc.

    sys_file_put_byte(file, modem->call_count);

    int nn;
    for (nn = modem->call_count - 1; nn >= 0; nn--) {
      AVoiceCall  vcall = modem->calls + nn;
      // Note: not saving timers or remote calls.
      ACall       call  = &vcall->call;
      sys_file_put_byte(file, call->dir);
      sys_file_put_byte(file, call->state);
      sys_file_put_byte(file, call->mode);
      sys_file_put_be32(file, call->multi);
      sys_file_put_buffer(file, (uint8_t *)call->number,
                          A_CALL_NUMBER_MAX_SIZE+1);
    }
    sys_file_put_byte(file, modem->radio_state);
    sys_file_put_byte(file, modem->send_phys_channel_cfg_unsol);
    sys_file_put_byte(file, modem->data_network);
    sys_file_put_byte(file, modem->data_network_requested);
}

int em_state_load(Express_Modem *modem, SysFile* file, int version_id)
{
    // In case there are timers or remote calls.
    int nn;
    for (nn = modem->call_count - 1; nn >= 0; nn--) {
      em_free_call( modem, modem->calls + nn);
    }

    int call_count = sys_file_get_byte(file);
    for (nn = call_count; nn > 0; nn--) {
      AVoiceCall vcall = em_alloc_call( modem );
      ACall      call  = &vcall->call;
      call->dir   = sys_file_get_byte(file);
      call->state = sys_file_get_byte(file);
      call->mode  = sys_file_get_byte(file);
      call->multi = sys_file_get_be32(file);
      sys_file_get_buffer(file, (uint8_t *)call->number, A_CALL_NUMBER_MAX_SIZE+1 );
    }
    if (version_id == MODEM_DEV_STATE_SAVE_VERSION) {
        ARadioState radio_state = sys_file_get_byte(file);
        modem->radio_state = radio_state;
        modem->send_phys_channel_cfg_unsol = sys_file_get_byte(file);
        modem->data_network = sys_file_get_byte(file);;
        modem->data_network_requested = sys_file_get_byte(file);
    } else {
        // In the past, we didn't save radio state in amode_state_save(),
        // we will by default set radio state on in this case.
        modem->radio_state = A_RADIO_STATE_ON;
    }

    return 0; // >=0 Happy
}
*/

static void 
em_init( Express_Modem *modem )
{
    // Delegate base_port and sim_present here
    int sim_present = true;

    modem->base_port    = 114514;

    modem->in_pos = 0;
    modem->in_sms = 0;

    em_reset( modem );
    modem->supportsNetworkDataType = 1;

    modem->sim = asimcard_create(sim_present);

    // XXX: do not use sysdeps here
    // sys_main_init();
}

void
em_set_legacy( Express_Modem *modem )
{
    modem->supportsNetworkDataType = 0;
}

void
em_destroy( Express_Modem *modem )
{
    asimcard_destroy( modem->sim );
    modem->sim = NULL;
}

void em_set_sim_present( Express_Modem *modem, int is_present ) {
    asimcard_set_status( modem->sim, is_present ? A_SIM_STATUS_READY : A_SIM_STATUS_ABSENT );
}

static int
em_has_network( Express_Modem *modem )
{
    return !(modem->radio_state == A_RADIO_STATE_OFF   ||
             modem->oper_index < 0                  ||
             modem->oper_index >= modem->oper_count ||
             modem->oper_selection_mode == A_SELECTION_DEREGISTRATION );
}


ARadioState
em_get_radio_state( Express_Modem *modem )
{
    return modem->radio_state;
}

void
em_set_radio_state( Express_Modem *modem, ARadioState  state )
{
    modem->radio_state = state;
}

ASimCard
em_get_sim( Express_Modem *modem )
{
    return  modem->sim;
}

ARegistrationState
em_get_voice_registration( Express_Modem *modem )
{
    return modem->voice_state;
}

void
em_set_voice_registration( Express_Modem *modem, ARegistrationState  state )
{
    modem->voice_state = state;

    if (state == A_REGISTRATION_HOME)
        modem->oper_index = DEFCONF_OPERATOR_HOME_INDEX;
    else if (state == A_REGISTRATION_ROAMING)
        modem->oper_index = DEFCONF_OPERATOR_ROAMING_INDEX;

    switch (modem->voice_mode) {
        case A_REGISTRATION_UNSOL_ENABLED:
            em_unsol( modem, "+CREG: %d,%d\r",
                          modem->voice_mode, modem->voice_state );
            break;

        case A_REGISTRATION_UNSOL_ENABLED_FULL:
            // XXX: hard code
            em_unsol( modem, "+CREG: %d,%d,\"%04x\",\"%04x\",3\r",
                          modem->voice_mode, modem->voice_state,
                          modem->area_code & 0xffff, modem->cell_id & 0xffff);
            break;
        default:
            ;
    }
}

ARegistrationState
em_get_data_registration( Express_Modem *modem )
{
    return modem->data_state;
}

void
em_set_meter_state( Express_Modem *modem, int meteron )
{
    (void) modem, (void) meteron;
    // XXX
    // set_mobile_data_meterness(meteron);
}

void
em_set_data_registration( Express_Modem *modem, ARegistrationState  state )
{
    modem->data_state = state;

    switch (modem->data_mode) {
        case A_REGISTRATION_UNSOL_ENABLED:
            em_unsol( modem, "+CGREG: %d,%d\r",
                          modem->data_mode, modem->data_state );
            break;

        case A_REGISTRATION_UNSOL_ENABLED_FULL:
            if (modem->supportsNetworkDataType) {
                adjustNetDataNetwork(modem);
                // XXX: hard code
                em_unsol( modem, "+CGREG: %d,%d,\"%04x\",\"%04x\",3\r",
                            modem->data_mode, modem->data_state,
                            modem->area_code & 0xffff, modem->cell_id & 0xffff );
            }
            else
                em_unsol( modem, "+CGREG: %d,%d,\"%04x\",\"%04x\"\r",
                            modem->data_mode, modem->data_state,
                            modem->area_code & 0xffff, modem->cell_id & 0xffff );
            break;

        default:
            ;
    }
}

static AModemTech
tech_from_network_type( ADataNetworkType type )
{
    switch (type) {
        case A_DATA_NETWORK_GPRS:
        case A_DATA_NETWORK_EDGE:
        case A_DATA_NETWORK_UMTS:
            return A_TECH_GSM;
        case A_DATA_NETWORK_LTE:
        case A_DATA_NETWORK_NR:
            return A_TECH_LTE;
        case A_DATA_NETWORK_UNKNOWN:
            return A_TECH_UNKNOWN;
    }
    return A_TECH_UNKNOWN;
}

void
em_set_data_network_type( Express_Modem *modem, ADataNetworkType   type )
{
    AModemTech modemTech;
    bool new_data_network = (modem->data_network != type);
    // XXX: Be careful of unimplemented NR
    modem->data_network = type;
    em_set_data_registration( modem, modem->data_state );
    modemTech = tech_from_network_type(type);
    if (modemTech != A_TECH_UNKNOWN) {
        if (_em_switch_technology( modem, modemTech, modem->preferred_mask, new_data_network)) {
            em_unsol( modem, modem->out_buff );
        }
    }
}

int
em_get_operator_name ( Express_Modem *modem, ANameIndex  index, char*  buffer, int  buffer_size )
{
    AOperator  oper;
    int        len;

    if ( (unsigned)modem->oper_index >= (unsigned)modem->oper_count ||
         (unsigned)index > 2 )
        return 0;

    oper = modem->operators + modem->oper_index;
    len  = strlen(oper->name[index]) + 1;

    if (buffer_size > len)
        buffer_size = len;

    if (buffer_size > 0) {
        memcpy( buffer, oper->name[index], buffer_size-1 );
        buffer[buffer_size] = 0;
    }
    return len;
}

/* reset one operator name from a user-provided buffer, set buffer_size to -1 for zero-terminated strings */
void
em_set_operator_name( Express_Modem *modem, ANameIndex  index, const char*  buffer, int  buffer_size )
{
    AOperator  oper;
    int        avail;

    if ( (unsigned)modem->oper_index >= (unsigned)modem->oper_count ||
         (unsigned)index > 2 )
        return;

    oper = modem->operators + modem->oper_index;

    avail = sizeof(oper->name[0]);
    if (buffer_size < 0)
        buffer_size = strlen(buffer);
    if (buffer_size > avail-1)
        buffer_size = avail-1;
    memcpy( oper->name[index], buffer, buffer_size );
    oper->name[index][buffer_size] = 0;
}

/** CALLS
 **/
int
em_get_call_count( Express_Modem *modem )
{
    return modem->call_count;
}

ACall
em_get_call( Express_Modem *modem, int  index )
{
    if ((unsigned)index >= (unsigned)modem->call_count)
        return NULL;

    return &modem->calls[index].call;
}

static AVoiceCall
em_alloc_call( Express_Modem *modem )
{
    AVoiceCall  call  = NULL;
    int         count = modem->call_count;

    if (count < MAX_CALLS) {
        int  id;

        /* find a valid id for this call */
        for (id = 0; id < modem->call_count; id++) {
            int  found = 0;
            int  nn;
            for (nn = 0; nn < count; nn++) {
                if ( modem->calls[nn].call.id == (id+1) ) {
                    found = 1;
                    break;
                }
            }
            if (!found)
                break;
        }
        call          = modem->calls + count;
        call->call.id = id + 1;
        call->modem   = modem;

        modem->call_count += 1;
        if (modem->notify_call_back) {
            modem->notify_call_back(modem->notify_user_data, modem->call_count);
        }
    }
    return call;
}


static void
em_free_call( Express_Modem *modem, AVoiceCall  call )
{
    int  nn;

    // XXX: disable timer here
    // if (call->timer) {
    //     sys_timer_destroy( call->timer );
    //     call->timer = NULL;
    // }

    if (call->is_remote) {
        remote_call_cancel( call->call.number, modem->base_port );
        call->is_remote = 0;
    }

    for (nn = 0; nn < modem->call_count; nn++) {
        if ( modem->calls + nn == call )
            break;
    }
    assert( nn < modem->call_count );

    memmove( modem->calls + nn,
             modem->calls + nn + 1,
             (modem->call_count - 1 - nn)*sizeof(*call) );

    modem->call_count -= 1;
    if (modem->notify_call_back) {
        modem->notify_call_back(modem->notify_user_data, modem->call_count);
    }
}


static AVoiceCall
em_find_call( Express_Modem *modem, int  id )
{
    int  nn;

    for (nn = 0; nn < modem->call_count; nn++) {
        AVoiceCall call = modem->calls + nn;
        if (call->call.id == id)
            return call;
    }
    return NULL;
}

static void
em_send_calls_update( Express_Modem *modem )
{
   /* despite its name, this really tells the system that the call
    * state has changed */
    em_unsol( modem, "RING\r" );
}

int
em_add_inbound_call( Express_Modem *modem, const char*  number )
{
    if (modem->radio_state == A_RADIO_STATE_OFF)
        return A_CALL_RADIO_OFF;

    AVoiceCall  vcall = em_alloc_call( modem );
    ACall       call  = &vcall->call;
    int         len;

    if (call == NULL)
        return A_CALL_EXCEED_MAX_NUM;

    call->dir   = A_CALL_INBOUND;
    call->state = A_CALL_INCOMING;
    call->mode  = A_CALL_VOICE;
    call->multi = 0;

    vcall->is_remote = (remote_number_string_to_port(number) > 0);

    len  = strlen(number);
    if (len >= (int) sizeof(call->number))
        len = sizeof(call->number)-1;

    memcpy( call->number, number, len );
    call->number[len] = 0;

    em_send_calls_update( modem );
    return A_CALL_OP_OK;
}

ACall
em_find_call_by_number( Express_Modem *modem, const char*  number )
{
    AVoiceCall  vcall = modem->calls;
    AVoiceCall  vend  = vcall + modem->call_count;

    if (!number)
        return NULL;

    for ( ; vcall < vend; vcall++ )
        if ( !strcmp(vcall->call.number, number) )
            return &vcall->call;

    return  NULL;
}

void
em_set_signal_strength( Express_Modem *modem, int rssi, int ber )
{
    modem->rssi = rssi;
    modem->ber = ber;
    modem->use_signal_profile = 0;
}

void
em_set_signal_strength_profile( Express_Modem *modem, int quality )
{
    if (quality >= NONE && quality <= GREAT) {
        modem->quality = quality;
        modem->use_signal_profile = 1;
    }
}

static void
acall_set_state( AVoiceCall    call, ACallState  state )
{
    if (state != call->call.state)
    {
        if (call->is_remote)
        {
            const char*  number = call->call.number;
            int          port   = call->modem->base_port;

            switch (state) {
                case A_CALL_HELD:
                    remote_call_other( number, port, REMOTE_CALL_HOLD );
                    break;

                case A_CALL_ACTIVE:
                    remote_call_other( number, port, REMOTE_CALL_ACCEPT );
                    break;

                default: ;
            }
        }
        call->call.state = state;
    }
}


int
em_update_call( Express_Modem *modem, const char*  fromNumber, ACallState  state )
{
    AVoiceCall  vcall = (AVoiceCall) em_find_call_by_number(modem, fromNumber);

    if (vcall == NULL)
        return A_CALL_NUMBER_NOT_FOUND;

    acall_set_state( vcall, state );
    em_send_calls_update(modem);
    return 0;
}


int
em_disconnect_call( Express_Modem *modem, const char*  number )
{
    AVoiceCall  vcall = (AVoiceCall) em_find_call_by_number(modem, number);

    if (!vcall)
        return A_CALL_NUMBER_NOT_FOUND;

    em_free_call( modem, vcall );
    em_send_calls_update(modem);
    return 0;
}

/** COMMAND HANDLERS
 **/

static const char*
unknownCommand( const char*  cmd, Express_Modem *modem )
{
    (void) modem;
    fprintf(stderr, ">>> unknown command '%s'\n", cmd );
    return "ERROR: unknown command\r";
}

/*
 * Tell whether the specified tech is valid for the preferred mask.
 * @pmask: The preferred mask
 * @tech: The AModemTech we try to validate
 * return: If the specified technology is not set in any of the 4
 *         bitmasks, return 0.
 *         Otherwise, return a non-zero value.
 */
static int matchPreferredMask( int32_t pmask, AModemTech tech )
{
    int ret = 0;
    int i;
    for ( i=3; i >= 0 ; i-- ) {
        if (pmask & (1 << (tech + i*8 ))) {
            ret = 1;
            break;
        }
    }
    return ret;
}

static AModemTech
chooseTechFromMask( Express_Modem *modem, int32_t preferred )
{
    (void) modem;
    int i, j;

    /* TODO: Current implementation will only return the highest priority,
     * lowest numbered technology that is set in the mask.
     * However the implementation could be changed to consider currently
     * available networks set from the console (or somewhere else)
     */
    for ( i=3 ; i >= 0; i-- ) {
        for ( j=0 ; j < A_TECH_UNKNOWN ; j++ ) {
            if (preferred & (1 << (j + 8 * i)))
                return (AModemTech) j;
        }
    }
    assert("This should never happen" == 0);
    // This should never happen. Just to please the compiler.
    return A_TECH_UNKNOWN;
}

static const char*
_em_switch_technology( Express_Modem *modem, AModemTech newtech, int32_t newpreferred, bool new_data_network)
{
    LOGD("_em_switch_technology: oldtech: %d, newtech %d, preferred: %d. newpreferred: %d\n",
                      modem->technology, newtech, modem->preferred_mask, newpreferred);
    const char *ret = "+CTEC: DONE";
    assert( modem );

    if (!newpreferred) {
        return "ERROR: At least one technology must be enabled";
    }
    if (modem->preferred_mask != newpreferred) {
        char value[MAX_KEY_NAME + 1];
        modem->preferred_mask = newpreferred;
        snprintf(value, MAX_KEY_NAME, "%d", newpreferred);
        ruim_set_str(modem->ruim, RUIM_PREFERRED_MODE, value);
        if (!matchPreferredMask(modem->preferred_mask, newtech)) {
            newtech = chooseTechFromMask(modem, newpreferred);
        }
    }

    if (modem->technology != newtech || new_data_network) {
        modem->technology = newtech;
        ret = em_printf(modem, "+CTEC: %d", modem->technology);
    }
    return ret;
}

static int
parsePreferred( const char *str, int *preferred )
{
    char *endptr = NULL;
    int result = 0;
    if (!str || !*str) { *preferred = 0; return 0; }
    if (*str == '"') str ++;
    if (!*str) return 0;

    result = strtol(str, &endptr, 16);
    if (*endptr && *endptr != '"') {
        return 0;
    }
    if (preferred)
        *preferred = result;
    return 1;
}

void
em_set_cdma_prl_version( Express_Modem *modem, int prlVersion)
{
    LOGD("em_set_prl_version()\n");
    if (!_em_set_cdma_prl_version( modem, prlVersion)) {
        em_unsol(modem, "+WPRL: %d", prlVersion);
    }
}

static int
_em_set_cdma_prl_version( Express_Modem *modem, int prlVersion)
{
    LOGD("_em_set_cdma_prl_version");
    if (modem->prl_version != prlVersion) {
        modem->prl_version = prlVersion;
        return 0;
    }
    return -1;
}

void
em_set_cdma_subscription_source( Express_Modem *modem, ACdmaSubscriptionSource ss)
{
    LOGD("em_set_cdma_subscription_source()\n");
    if (!_em_set_cdma_subscription_source( modem, ss)) {
        em_unsol(modem, "+CCSS: %d", (int)ss);
    }
}

#define MAX_INT_DIGITS 10
static int
_em_set_cdma_subscription_source( Express_Modem *modem, ACdmaSubscriptionSource ss)
{
    LOGD("_em_set_cdma_subscription_source()\n");
    char value[MAX_INT_DIGITS + 1];

    if (ss != modem->subscription_source) {
        snprintf( value, MAX_INT_DIGITS + 1, "%d", ss );
        ruim_set_str( modem->ruim, RUIM_CDMA_SUBSCRIPTION_SOURCE, value );
        modem->subscription_source = ss;
        return 0;
    }
    return -1;
}

static const char*
handleSubscriptionSource( const char*  cmd, Express_Modem *modem )
{
    int newsource;
    // TODO: Actually change subscription depending on source
    LOGD("handleSubscriptionSource(%s)\n",cmd);

    assert( !memcmp( "+CCSS", cmd, 5 ) );
    cmd += 5;
    if (cmd[0] == '?') {
        return em_printf( modem, "+CCSS: %d", modem->subscription_source );
    } else if (cmd[0] == '=') {
        switch (cmd[1]) {
            case '0':
            case '1':
                newsource = (ACdmaSubscriptionSource)cmd[1] - '0';
                _em_set_cdma_subscription_source( modem, newsource );
                return em_printf( modem, "+CCSS: %d", modem->subscription_source );
                break;
        }
    }
    return em_printf( modem, "ERROR: Invalid subscription source");
}

static const char*
handleRoamPref( const char * cmd, Express_Modem *modem )
{
    int roaming_pref = -1;
    char *endptr = NULL;
    LOGD("handleRoamPref(%s)\n", cmd);
    assert( !memcmp( "+WRMP", cmd, 5 ) );
    cmd += 5;
    if (cmd[0] == '?') {
        return em_printf( modem, "+WRMP: %d", modem->roaming_pref );
    }

    if (!strcmp( cmd, "=?")) {
        return em_printf( modem, "+WRMP: 0,1,2" );
    } else if (cmd[0] == '=') {
        cmd ++;
        roaming_pref = strtol( cmd, &endptr, 10 );
         // Make sure the rest of the command is the number
         // (if *endptr is null, it means strtol processed the whole string as a number)
        if(endptr && !*endptr) {
            modem->roaming_pref = roaming_pref;
            ruim_set_str( modem->ruim, RUIM_CDMA_ROAMING_PREF, cmd );
            return NULL;
        }
    }
    return em_printf( modem, "ERROR");
}
static const char*
handleTech( const char*  cmd, Express_Modem *modem )
{
    AModemTech newtech = modem->technology;
    int pt = modem->preferred_mask;
    int havenewtech = 0;
    LOGD("handleTech. cmd: %s\n", cmd);
    assert( !memcmp( "+CTEC", cmd, 5 ) );
    cmd += 5;
    if (cmd[0] == '?') {
        return em_printf( modem, "+CTEC: %d,%x",modem->technology, modem->preferred_mask );
    }
    em_begin_line( modem );
    if (!strcmp( cmd, "=?")) {
        return em_printf( modem, "+CTEC: 0,1,2,3" );
    }
    else if (cmd[0] == '=') {
        switch (cmd[1]) {
            case '0':
            case '1':
            case '2':
            case '3':
                havenewtech = 1;
                newtech = cmd[1] - '0';
                cmd += 1;
                break;
        }
        cmd += 1;
    }
    if (havenewtech) {
        LOGD( "cmd: %s\n", cmd );
        if (cmd[0] == ',' && ! parsePreferred( ++cmd, &pt ))
            return em_printf( modem, "ERROR: invalid preferred mode" );
        return _em_switch_technology( modem, newtech, pt, false );
    }
    return em_printf( modem, "ERROR: %s: Unknown Technology", cmd + 1 );
}

static const char*
handleEmergencyMode( const char* cmd, Express_Modem *modem )
{
    long arg;
    char *endptr = NULL;
    assert ( !memcmp( "+WSOS", cmd, 5 ) );
    cmd += 5;
    if (cmd[0] == '?') {
        return em_printf( modem, "+WSOS: %d", modem->in_emergency_mode);
    }

    if (cmd[0] == '=') {
        if (cmd[1] == '?') {
            return em_printf(modem, "+WSOS: (0)");
        }
        if (cmd[1] == 0) {
            return em_printf(modem, "ERROR");
        }
        arg = strtol(cmd+1, &endptr, 10);

        if (!endptr || endptr[0] != 0) {
            return em_printf(modem, "ERROR");
        }

        arg = arg? 1 : 0;

        if ((!arg) != (!modem->in_emergency_mode)) {
            modem->in_emergency_mode = arg;
            return em_printf(modem, "+WSOS: %d", arg);
        }
    }
    return em_printf(modem, "ERROR");
}

static const char*
handlePrlVersion( const char* cmd, Express_Modem *modem )
{
    assert ( !memcmp( "+WPRL", cmd, 5 ) );
    cmd += 5;
    if (cmd[0] == '?') {
        return em_printf( modem, "+WPRL: %d", modem->prl_version);
    }

    return em_printf(modem, "ERROR");
}

static const char*
enableGoldfishPhysicalChannelConfigUnsol( const char*  cmd, Express_Modem *modem )
{
    modem->send_phys_channel_cfg_unsol = 1;
    return NULL;
}

static const char*
handleRadioPower( const char*  cmd, Express_Modem *modem )
{
    int fun = -1;
    sscanf(cmd, "+CFUN=%d", &fun);

    if (fun == 0) {
        /* turn radio off */
        modem->radio_state = A_RADIO_STATE_OFF;
    } else if (fun == 1) {
        /* turn radio on */
        modem->radio_state = A_RADIO_STATE_ON;
    } else {
        goto CFUN_error;
    }

    return NULL;

CFUN_error:
    return em_printf(modem, "ERROR");
}

static const char*
handleRadioPowerReq( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    if (modem->radio_state != A_RADIO_STATE_OFF)
        return "+CFUN: 1";
    else
        return "+CFUN: 0";
}

static const char*
handleOpenLogicalChannel(const char* cmd, Express_Modem *modem)
{
    int channel;
    char* df_name = NULL;
    char* divider = strchr(cmd, '=');
    if (divider == NULL) {
        return em_printf(modem, "+CME ERROR: %d",
                            kCmeErrorInvalidCharactersInTextString);
    }
    df_name = divider + 1;
    for (channel = 0; channel < MAX_LOGICAL_CHANNELS; ++channel) {
        if (!modem->logical_channels[channel].is_open) {
            modem->logical_channels[channel].is_open = true;
            modem->logical_channels[channel].df_name = strdup(df_name);
            modem->logical_channels[channel].file_id = 0x3F00;
            break;
        }
    }
    if (channel >= MAX_LOGICAL_CHANNELS) {
        // Could not find an available channel, we're probably leaking channels
        return em_printf(modem, "+CME ERROR: %d", kCmeErrorMemoryFull);
    }

    return em_printf(modem, "%u", channel);
}

static const char*
handleCloseLogicalChannel(const char* cmd, Express_Modem *modem)
{
    int channel;
    char dummy;
    char* channel_str = NULL;
    char* divider = strchr(cmd, '=');


    if (divider == NULL) {
        return em_printf(modem, "+CME ERROR: %d", kCmeErrorInvalidCharactersInTextString);
    }
    channel_str = divider + 1;
    if (sscanf(channel_str, "%d%c", &channel, &dummy) != 1) {
        return em_printf(modem, "+CME ERROR: %d", kCmeErrorInvalidCharactersInTextString);
    }
    if (channel <= 0 ||
            channel >= MAX_LOGICAL_CHANNELS ||
            !modem->logical_channels[channel].is_open) {
        return em_printf(modem, "+CME ERROR: %d", kCmeErrorInvalidIndex);
    }
    modem->logical_channels[channel].is_open = false;
    free(modem->logical_channels[channel].df_name);
    modem->logical_channels[channel].df_name = NULL;

    return "+CCHC";
}

// XXX: hard-coded here
static char* sim_get_fcp(uint16_t fileid)
{
    (void) fileid;
    return NULL;
}

static const char*
handleTransmitLogicalChannel(const char* cmd, Express_Modem *modem) {
    const char* result = NULL;
    char command[1024];
    char scan_string[128];
    int channel = -1;
    int length = -1;
    char dummy = 0;
    uint8_t apduClass;
    SIM_APDU apdu;


    // Create a scan string with the size of the command array in it
    snprintf(scan_string, sizeof(scan_string),
             "+CGLA=%%d,%%d,%%%ds%%c", (int)(sizeof(command) - 1));
    // Then scan the AT string to get the components
    if (sscanf(cmd, scan_string, &channel, &length, command, &dummy) != 3) {
        return em_printf( modem, "+CME ERROR: %d",
                              kCmeErrorInvalidCharactersInTextString);
    }

    // Validate the channel number and ensure the channel is open
    if (channel < 0 ||
            channel >= MAX_LOGICAL_CHANNELS ||
            !modem->logical_channels[channel].is_open) {
        return em_printf(modem, "+CME ERROR: %d", kCmeErrorInvalidIndex);
    }

    // Parse the command part of the AT string into a SIM APDU struct
    if (!parseSimApduCommand(command, length, &apdu)) {
        return em_printf(modem, "+CME ERROR: %d",
                             kCmeErrorInvalidCharactersInTextString);
    }

    // Get the instruction classs, the lower four bits are used to encode
    // secure messaging settings and channel number and do not matter for this.
    apduClass = apdu.cla & 0xf0;

    // Now see if it's a supported instruction
    switch (apdu.instruction) {
    case kSimApduGetData:
        if (apduClass == kSimApduClaStatus && apdu.param1 == 0xFF && apdu.param2 == 0x40) {
            // Get Data (from class and instrcution) ALL (from params) command
            // XXX: hard-coded here
            // char* df_name = modem->logical_channels[channel].df_name;
            // char* rules = sim_get_access_rules(df_name);
            char* rules = strdup("FF403eE23cE12eC11461ed377e85d386a8dfee6b864bd85b0bfaa5af81CA16616e64726f69642e636172726965726170692e637473E30aDB080000000000000000");
            if (rules) {
                result = em_printf(modem, "+CGLA: 144,0,%s", rules);
                free(rules);
                rules = NULL;
            }
        }
        break;
    case kSimApduSelect:
        if (apduClass == kSimApduClaSelect && apdu.param1 == 0x00 && apdu.param2 == 0x0C && apdu.param3 == 2) {
            uint16_t *file_id = &(modem->logical_channels[channel].file_id);
            memcpy(file_id, apdu.data, 2);
            // change to little endian
            *file_id = ntohs(*file_id);

            char* fcpstr = sim_get_fcp(*file_id);
            if (fcpstr == NULL) {
                result = em_printf(modem, "+CGLA: %d,%d", 0x6a, 0x82);
            } else {
                // save the fileid select status for later fetch
                asimcard_set_fileid_status(modem->sim, fcpstr);
                result = em_printf(modem, "+CGLA: 144,0");
                free(fcpstr);
            }
        } else if (apduClass == kSimApduClaSelect && apdu.param1 == 0x00 && apdu.param2 == 0x04 && apdu.param3 == 2) {
            uint16_t *file_id = &(modem->logical_channels[channel].file_id);
            memcpy(file_id, apdu.data, 2);
            *file_id = ntohs(*file_id);

            // when p2 is 0x004, we need to return the respond right away
            char* fcpstr = sim_get_fcp(*file_id);
            if (fcpstr == NULL) {
                result = em_printf(modem, "+CGLA: %d,%d", 0x6a, 0x82);
            } else {
                result = em_printf(modem, "+CGLA: 144,0,%s", fcpstr);
                free(fcpstr);
            }
        }
        break;
    case kSimApduReadBinary:
        if (apduClass == kSimApduClaReadBinary && apdu.param1 == 0x00 && apdu.param2 == 0x00 && apdu.param3 == 0x00) {
            uint16_t file_id = modem->logical_channels[channel].file_id;
            if (file_id == 0x2FE2) {
                // return hardcoded ICCID file content
                result = em_printf(modem, "+CGLA: 144,0,%s", "98942000001081853911");
            }
        }
        break;
    case kSimApduStatus:
        if (apduClass != kSimApduClaStatus && apduClass != kSimApduClaGetResponse) {
            result = em_printf(modem, "+CGLA: %d,%d", 0x6e, 0x00);
        } else if (apduClass == kSimApduClaStatus && apdu.param1 == 0x00 && apdu.param2 == 0x00 && apdu.param3 == 0x00) {
            char* fcpstr = sim_get_fcp(modem->logical_channels[channel].file_id);
            if (fcpstr == NULL) {
                result = em_printf(modem, "+CGLA: %d,%d", 0x6a, 0x82);
            } else {
                result = em_printf(modem, "+CGLA: 144,0,%s", fcpstr);
                free(fcpstr);
            }
        } else if (apdu.param1 != 0x00 && apdu.param1 != 0x01 && apdu.param1 != 0x02) {
            result = em_printf(modem, "+CGLA: %d,%d", 0x6a, 0x86);
        }
        break;
    case kSimApduManageChannel:
        if (apduClass == kSimApduClaManageChannel && apdu.param1 == 0x00 && apdu.param2 == 0x00 && apdu.param3 == 0x00) {
            int channel = -1;
            for (channel = 0; channel < MAX_LOGICAL_CHANNELS; ++channel) {
                if (!modem->logical_channels[channel].is_open) {
                    modem->logical_channels[channel].is_open = true;
                    modem->logical_channels[channel].df_name = strdup("");
                    modem->logical_channels[channel].file_id = 0x3F00;
                    break;
                }
            }
            if (channel >= MAX_LOGICAL_CHANNELS) {
                result = em_printf(modem, "+CME ERROR: %d", kCmeErrorMemoryFull);
            } else {
                result = em_printf(modem, "+CGLA: 144,0,%02x", channel);
            }
        }
        else if (apduClass == kSimApduClaManageChannel && apdu.param1 == 0x80 && apdu.param2 > 0x00 && apdu.param3 == 0x00) {
            int channel = apdu.param2; // to close this channel
            if (channel <= 0 || channel >= MAX_LOGICAL_CHANNELS || !modem->logical_channels[channel].is_open) {
                result = em_printf(modem, "+CME ERROR: %d", kCmeErrorInvalidIndex);
            } else {
                modem->logical_channels[channel].is_open = false;
                free(modem->logical_channels[channel].df_name);
                modem->logical_channels[channel].df_name = NULL;
                result = em_printf(modem, "+CGLA: 144,0");
            }
        }
        break;
    }
    if (result == NULL) {
        result = em_printf(modem, "+CME ERROR: %d (%d, %d) (%s)",
                               kCmeErrorUnknownError, apdu.param1, apdu.param2,
                               cmd);
    }
    freeSimApduCommand(&apdu);
    return result;
}

static const char*
handleSIMStatusReq( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    const char*  answer = NULL;

    switch (asimcard_get_status(modem->sim)) {
        case A_SIM_STATUS_ABSENT:    answer = "+CPIN: ABSENT"; break;
        case A_SIM_STATUS_READY:
            if (modem->has_allowed_carriers || modem->has_excluded_carriers) {
                answer = "+CPIN: RESTRICTED";
            } else {
                answer = "+CPIN: READY";
            }
            break;
        case A_SIM_STATUS_NOT_READY: answer = "+CMERROR: NOT READY"; break;
        case A_SIM_STATUS_PIN:       answer = "+CPIN: SIM PIN"; break;
        case A_SIM_STATUS_PUK:       answer = "+CPIN: SIM PUK"; break;
        case A_SIM_STATUS_NETWORK_PERSONALIZATION: answer = "+CPIN: PH-NET PIN"; break;
        default:
            answer = "ERROR: internal error";
    }
    return answer;
}

static const char*
handleSetCarrierRestrictionsReq( const char*  cmd, Express_Modem *modem )
{
    int len_allowed_carriers;
    int len_excluded_carriers;

    if (sscanf(cmd, "+CRRSTR=%d,%d",
               &len_allowed_carriers,
               &len_excluded_carriers) == 2) {
        modem->has_allowed_carriers = len_allowed_carriers > 0;
        modem->has_excluded_carriers = len_excluded_carriers > 0;
        return NULL;
    } else {
        return em_printf( modem, "+CME ERROR: %d",
                              kCmeErrorInvalidCharactersInTextString);
    }
}

static void adjustNetDataNetwork(Express_Modem *modem)
{
    /* ignore system that does not support 5g*/
    modem->data_network = dataNetworkTypeFromInt(modem->data_network_requested);
    if (modem->send_phys_channel_cfg_unsol == 0 && modem->data_network == A_DATA_NETWORK_NR) {
         modem-> data_network = A_DATA_NETWORK_LTE;
    }
}

/* TODO: Will we need this?
static const char*
handleSRegister( const char * cmd, Express_Modem *modem )
{
    char *end;
    assert( cmd[0] == 'S' || cmd[0] == 's' );

    ++ cmd;

    int l = strtol(cmd, &end, 10);
} */

static const char*
handleNetworkRegistration( const char*  cmd, Express_Modem *modem )
{
    if ( !memcmp( cmd, "+CREG", 5 ) ) {
        cmd += 5;
        if (cmd[0] == '?') {
            if (modem->voice_mode == A_REGISTRATION_UNSOL_ENABLED_FULL)
                // XXX: hard code
                return em_printf( modem, "+CREG: %d,%d,\"%04x\",\"%04x\",3",
                                       modem->voice_mode, modem->voice_state,
                                       modem->area_code, modem->cell_id );
            else
                return em_printf( modem, "+CREG: %d,%d",
                                       modem->voice_mode, modem->voice_state );
        } else if (cmd[0] == '=') {
            switch (cmd[1]) {
                case '0':
                    modem->voice_mode  = A_REGISTRATION_UNSOL_DISABLED;
                    break;

                case '1':
                    modem->voice_mode  = A_REGISTRATION_UNSOL_ENABLED;
                    break;

                case '2':
                    modem->voice_mode = A_REGISTRATION_UNSOL_ENABLED_FULL;
                    break;

                case '?':
                    return "+CREG: (0-2)";

                default:
                    return "ERROR: BAD COMMAND";
            }
        } else {
            assert( 0 && "unreachable" );
        }
    } else if ( !memcmp( cmd, "+CGREG", 6 ) ) {
        cmd += 6;
        if (cmd[0] == '?') {
            if (modem->supportsNetworkDataType) {
                adjustNetDataNetwork(modem);
                // XXX: hard code
                return em_printf( modem, "+CGREG: %d,%d,\"%04x\",\"%04x\",3",
                                    modem->data_mode, modem->data_state,
                                    modem->area_code, modem->cell_id);
            }
            else
                return em_printf( modem, "+CGREG: %d,%d,\"%04x\",\"%04x\"",
                                    modem->data_mode, modem->data_state,
                                    modem->area_code, modem->cell_id );
        } else if (cmd[0] == '=') {
            switch (cmd[1]) {
                case '0':
                    modem->data_mode  = A_REGISTRATION_UNSOL_DISABLED;
                    break;

                case '1':
                    modem->data_mode  = A_REGISTRATION_UNSOL_ENABLED;
                    break;

                case '2':
                    modem->data_mode = A_REGISTRATION_UNSOL_ENABLED_FULL;
                    break;

                case '?':
                    return "+CGREG: (0-2)";

                default:
                    return "ERROR: BAD COMMAND";
            }
        } else {
            assert( 0 && "unreachable" );
        }
    }
    return NULL;
}

static const char*
handleSetDialTone( const char*  cmd, Express_Modem *modem )
{
    /* XXX: TODO */
    (void) cmd, (void) modem;
    return NULL;
}

static const char*
handleDeleteSMSonSIM( const char*  cmd, Express_Modem *modem )
{
    /* XXX: TODO */
    (void) cmd, (void) modem;
    return NULL;
}

static const char*
handleSIM_IO( const char*  cmd, Express_Modem *modem )
{
    return asimcard_io( modem->sim, cmd );
}

static const char*
handleCSIM( const char*  cmd, Express_Modem *modem )
{
    return asimcard_csim( modem->sim, cmd );
}


static const char*
handleOperatorSelection( const char*  cmd, Express_Modem *modem )
{
    assert( !memcmp( "+COPS", cmd, 5 ) );
    cmd += 5;
    if (cmd[0] == '?') { /* ask for current operator */
        AOperator  oper = &modem->operators[ modem->oper_index ];

        if ( !em_has_network( modem ) )
        {
            // No network. Give a canned response rather than signaling an error.
            // (VTS tests don't allow an error. See b/62137708.)
            return em_printf(modem, "+COPS: 0,0,0");
        }

        oper = &modem->operators[ modem->oper_index ];

        if ( modem->oper_name_index == 2 )
            return em_printf( modem, "+COPS: %d,2,%s",
                                  modem->oper_selection_mode,
                                  oper->name[2] );

        return em_printf( modem, "+COPS: %d,%d,\"%s\"",
                              modem->oper_selection_mode,
                              modem->oper_name_index,
                              oper->name[ modem->oper_name_index ] );
    }
    else if (cmd[0] == '=' && cmd[1] == '?') {  /* ask for all available operators */
        const char*  comma = "+COPS: ";
        int          nn;
        em_begin_line( modem );
        for (nn = 0; nn < modem->oper_count; nn++) {
            AOperator  oper = &modem->operators[nn];
            em_add_line( modem, "%s(%d,\"%s\",\"%s\",\"%s\")", comma,
                             oper->status, oper->name[0], oper->name[1], oper->name[2] );
            comma = ", ";
        }
        return em_end_line( modem );
    }
    else if (cmd[0] == '=') {
        switch (cmd[1]) {
            case '0':
                modem->oper_selection_mode = A_SELECTION_AUTOMATIC;
                return NULL;

            case '1':
                {
                    int  format, nn, len, found = -1;

                    if (cmd[2] != ',')
                        goto BadCommand;
                    format = cmd[3] - '0';
                    if ( (unsigned)format > 2 )
                        goto BadCommand;
                    if (cmd[4] != ',')
                        goto BadCommand;
                    cmd += 5;
                    len  = strlen(cmd);
                    if (*cmd == '"') {
                        cmd++;
                        len -= 2;
                    }
                    if (len <= 0)
                        goto BadCommand;

                    for (nn = 0; nn < modem->oper_count; nn++) {
                        AOperator    oper = modem->operators + nn;
                        char*        name = oper->name[ format ];

                        if ( !memcpy( name, cmd, len ) && name[len] == 0 ) {
                            found = nn;
                            break;
                        }
                    }

                    if (found < 0) {
                        /* Selection failed */
                        return em_printf(modem, "+CME ERROR: %d",
                            kCmeErrorSelectionFailureEmergencyCallsOnly);
                    } else if (modem->operators[found].status == A_STATUS_DENIED) {
                        return em_printf(modem, "+CME ERROR: %d",
                            kCmeErrorNetworkNotAllowedEmergencyCallsOnly);
                    }
                    modem->oper_index = found;

                    /* set the voice and data registration states to home or roaming
                     * depending on the operator index
                     */
                    if (found == DEFCONF_OPERATOR_HOME_INDEX) {
                        modem->voice_state = A_REGISTRATION_HOME;
                        modem->data_state  = A_REGISTRATION_HOME;
                    } else if (found == DEFCONF_OPERATOR_ROAMING_INDEX) {
                        modem->voice_state = A_REGISTRATION_ROAMING;
                        modem->data_state  = A_REGISTRATION_ROAMING;
                    }
                    return NULL;
                }

            case '2':
                modem->oper_selection_mode = A_SELECTION_DEREGISTRATION;
                return NULL;

            case '3':
                {
                    int format;

                    if (cmd[2] != ',')
                        goto BadCommand;

                    format = cmd[3] - '0';
                    if ( (unsigned)format > 2 )
                        goto BadCommand;

                    modem->oper_name_index = format;
                    return NULL;
                }
            default:
                ;
        }
    }
BadCommand:
    return unknownCommand(cmd,modem);
}

static const char*
handleRequestOperator( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    AOperator  oper;

    if ( !em_has_network(modem) )
        return em_printf(modem, "+CME ERROR: %d",
                             kCmeErrorNoNetworkService);

    oper = modem->operators + modem->oper_index;
    modem->oper_name_index = 2;
    return em_printf( modem, "+COPS: 0,0,\"%s\"\r"
                          "+COPS: 0,1,\"%s\"\r"
                          "+COPS: 0,2,\"%s\"",
                          oper->name[0], oper->name[1], oper->name[2] );
}

static const char*
handleSendSMStoSIM( const char*  cmd, Express_Modem *modem )
{
    /* XXX: TODO */
    (void) cmd, (void) modem;
    return "ERROR: unimplemented";
}

static const char*
handleSendSMS( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    modem->wait_sms = 1;
    return "> ";
}

#if 0
static void
sms_address_dump( SmsAddress  address, FILE*  out )
{
    int  nn, len = address->len;

    if (address->toa == 0x91) {
        fprintf( out, "+" );
    }
    for (nn = 0; nn < len; nn += 2)
    {
        static const char  dialdigits[16] = "0123456789*#,N%";
        int  c = address->data[nn/2];

        fprintf( out, "%c", dialdigits[c & 0xf] );
        if (nn+1 >= len)
            break;

        fprintf( out, "%c", dialdigits[(c >> 4) & 0xf] );
    }
}

static void
smspdu_dump( SmsPDU  pdu, FILE*  out )
{
    SmsAddressRec    address;
    unsigned char    temp[256];
    int              len;

    if (pdu == NULL) {
        fprintf( out, "SMS PDU is (null)\n" );
        return;
    }

    fprintf( out, "SMS PDU type:       " );
    switch (smspdu_get_type(pdu)) {
        case SMS_PDU_DELIVER: fprintf(out, "DELIVER"); break;
        case SMS_PDU_SUBMIT:  fprintf(out, "SUBMIT"); break;
        case SMS_PDU_STATUS_REPORT: fprintf(out, "STATUS_REPORT"); break;
        default: fprintf(out, "UNKNOWN");
    }
    fprintf( out, "\n        sender:   " );
    if (smspdu_get_sender_address(pdu, &address) < 0)
        fprintf( out, "(N/A)" );
    else
        sms_address_dump(&address, out);
    fprintf( out, "\n        receiver: " );
    if (smspdu_get_receiver_address(pdu, &address) < 0)
        fprintf(out, "(N/A)");
    else
        sms_address_dump(&address, out);
    fprintf( out, "\n        text:     " );
    len = smspdu_get_text_message( pdu, temp, sizeof(temp)-1 );
    if (len > sizeof(temp)-1 )
        len = sizeof(temp)-1;
    fprintf( out, "'%.*s'\n", len, temp );
}
#endif

static const char*
handleSendSMSText( const char*  cmd, Express_Modem *modem )
{
    SmsAddressRec  address;
    char           temp[16];
    char           number[16];
    int            numlen;
    int            len = strlen(cmd);
    SmsPDU         pdu;

    /* get rid of trailing escape */
    if (len > 0 && cmd[len-1] == 0x1a)
        len -= 1;

    pdu = smspdu_create_from_hex( cmd, len );
    if (pdu == NULL) {
        LOGD("%s: invalid SMS PDU ?: '%s'\n", __FUNCTION__, cmd);
        return "+CMS ERROR: INVALID SMS PDU";
    }
    if (smspdu_get_receiver_address(pdu, &address) < 0) {
        LOGD("%s: could not get SMS receiver address from '%s'\n",
          __FUNCTION__, cmd);
        return "+CMS ERROR: BAD SMS RECEIVER ADDRESS";
    }

    do {
        int  index;

        numlen = sms_address_to_str( &address, temp, sizeof(temp) );
        if (numlen > (int)sizeof(temp)-1)
            break;
        temp[numlen] = 0;

        /* Converts 4, 7, and 10 digits number to full number */
        const char* phone_prefix = asimcard_get_phone_number_prefix(modem->sim);
        int prefix_len = strlen(phone_prefix);
        assert(prefix_len >= 7 && prefix_len <= 11);

        if ((numlen == 4 || numlen == 7 || numlen == 10) &&
             !strncmp(temp, &phone_prefix[prefix_len-(numlen-4)], numlen-4)) {
            int addr_prefix_len = prefix_len-(numlen-4);
            memcpy( number, phone_prefix, addr_prefix_len );
            memcpy( number+addr_prefix_len, temp, numlen );
            number[numlen+addr_prefix_len] = 0;
        }   else {
            memcpy( number, temp, numlen );
            number[numlen] = 0;
        }

        if ( remote_number_string_to_port( number ) < 0 )
            break;

        if (modem->sms_receiver == NULL) {
            modem->sms_receiver = sms_receiver_create();
            if (modem->sms_receiver == NULL) {
                LOGD( "%s: could not create SMS receiver\n", __FUNCTION__ );
                break;
            }
        }

        index = sms_receiver_add_submit_pdu( modem->sms_receiver, pdu );
        if (index < 0) {
            LOGD( "%s: could not add submit PDU\n", __FUNCTION__ );
            break;
        }
        /* the PDU is now owned by the receiver */
        pdu = NULL;

        if (index > 0) {
            SmsAddressRec  from[1];
            char           temp[15];
            memset(temp, '\0', 15);
            SmsPDU*        deliver;
            int            nn;
            // Use international number format that starts with "+" to be consistent
            // with the MSISDN value sim_card.c returns for request +CRSM=178,28480,1,4,32.
            // (b/109671863)
            snprintf( temp, sizeof(temp), "+%s", asimcard_get_phone_number(modem->sim) );
            sms_address_from_str( from, temp, strlen(temp) );

            deliver = sms_receiver_create_deliver( modem->sms_receiver, index, from );
            if (deliver == NULL) {
                LOGD( "%s: could not create deliver PDUs for SMS index %d\n",
                   __FUNCTION__, index );
                break;
            }

            for (nn = 0; deliver[nn] != NULL; nn++) {
                if ( remote_call_sms( number, modem->base_port, deliver[nn] ) < 0 ) {
                    LOGD( "%s: could not send SMS PDU to remote emulator\n",
                       __FUNCTION__ );
                    break;
                }
            }

            smspdu_free_list(deliver);
        }

    } while (0);

    if (pdu != NULL)
        smspdu_free(pdu);

    return "+CMGS: 0\rOK\r";
}

static const char*
handleChangeOrEnterPIN( const char*  cmd, Express_Modem *modem )
{
    assert( !memcmp( cmd, "+CPIN=", 6 ) );
    cmd += 6;

    switch (asimcard_get_status(modem->sim)) {
        case A_SIM_STATUS_ABSENT:
            return "+CME ERROR: SIM ABSENT";

        case A_SIM_STATUS_NOT_READY:
            return "+CME ERROR: SIM NOT READY";

        case A_SIM_STATUS_READY:
            /* this may be a request to change the PIN */
            {
                if (strlen(cmd) == 9 && cmd[4] == ',') {
                    char  pin[5];
                    memcpy( pin, cmd, 4 ); pin[4] = 0;

                    if ( !asimcard_check_pin( modem->sim, pin ) )
                        return "+CME ERROR: BAD PIN";

                    memcpy( pin, cmd+5, 4 );
                    asimcard_set_pin( modem->sim, pin );
                    return "+CPIN: READY";
                }
            }
            break;

        case A_SIM_STATUS_PIN:   /* waiting for PIN */
            if ( asimcard_check_pin( modem->sim, cmd ) )
                return "+CPIN: READY";
            else
                return "+CME ERROR: BAD PIN";

        case A_SIM_STATUS_PUK:
            if (strlen(cmd) == 9 && cmd[4] == ',') {
                char  puk[5];
                memcpy( puk, cmd, 4 );
                puk[4] = 0;
                if ( asimcard_check_puk( modem->sim, puk, cmd+5 ) )
                    return "+CPIN: READY";
                else
                    return "+CME ERROR: BAD PUK";
            }
            return "+CME ERROR: BAD PUK";

        default:
            return "+CPIN: PH-NET PIN";
    }

    return "+CME ERROR: BAD FORMAT";
}


static const char*
handleListCurrentCalls( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    int  nn;
    em_begin_line( modem );
    for (nn = 0; nn < modem->call_count; nn++) {
        AVoiceCall  vcall = modem->calls + nn;
        ACall       call  = &vcall->call;
        if (call->mode == A_CALL_VOICE)
            em_add_line( modem, "+CLCC: %d,%d,%d,%d,%d,\"%s\",%d\r\n",
                             call->id, call->dir, call->state, call->mode,
                             call->multi, call->number, 129 );
    }
    return em_end_line( modem );
}

static void
em_addOnePhysChanCfgUpdate(int status, int bandwidth, int rat, int freq, int id, Express_Modem *modem )
{
    em_add_line( modem, "%%CGFPCCFG: %d,%d,%d,%d,%d\r\n", status, bandwidth, rat, freq, id);
}

static void
em_addPhysChanCfgUpdate( Express_Modem *modem )
{
    if (modem->send_phys_channel_cfg_unsol == 0 ) {
        return;
    }

    const int PRIMARY_SERVING = 1;
    const int SECONDARY_SERVING = 2;
    int cellBandwidthDownlink = 5000;
    const int UNKNOWN = 0;
    const int MMWAVE = 4;
    int freq = UNKNOWN;
    if (modem->data_network == A_DATA_NETWORK_NR) {
        freq = MMWAVE;
        cellBandwidthDownlink = 50000;
    }
    int nn;
    for (nn = 0; nn < MAX_DATA_CONTEXTS; nn++) {
        ADataContext  data = modem->data_contexts + nn;
        if (!data->active || data->id <= 0)
            continue;
        em_addOnePhysChanCfgUpdate(PRIMARY_SERVING, cellBandwidthDownlink, modem->data_network,
                freq, data->id, modem);
        em_addOnePhysChanCfgUpdate(SECONDARY_SERVING, cellBandwidthDownlink, modem->data_network,
                freq, data->id, modem);
    }
}

// XXX: a function from timezone.h
static long
android_tzoffset_in_seconds(time_t* utc_time)
{
    struct tm local = *localtime(utc_time);
    time_t local_time = mktime(&local);
    // mktime() converts struct tm according to local timezone.
    time_t gmt_time = mktime(gmtime(utc_time));
    return (long)difftime(gmt_time, local_time);
}

/* Add a(n unsolicited) time response.
 *
 * retrieve the current time and zone in a format suitable
 * for %CTZV: unsolicited message
 *  "yy/mm/dd,hh:mm:ss(+/-)tz"
 *   mm is 0-based
 *   tz is in number of quarter-hours
 *
 * it seems reference-ril doesn't parse the comma (,) as anything else than a token
 * separator, so use a column (:) instead, the Java parsing code won't see a difference
 *
 */
static void
em_addTimeUpdate( Express_Modem *modem )
{
    time_t       now = time(NULL);
    struct tm    utc;
    long         tzdiff;
    char         tzname[64];
    int          isdst;

    utc   = *gmtime(&now);
    tzdiff = android_tzoffset_in_seconds(&now) / (15 * 60);  /* timezone offset is in number of quater-hours */
    isdst = localtime(&now)->tm_isdst;

    /* retrieve a zoneinfo-compatible name for the host timezone
     */
    {
        char*  end = tzname + sizeof(tzname);
        // XXX: hard-coded here
        // char*  p = bufprint_zoneinfo_timezone( tzname, end );
        char *p = "Asia/Shanghai";
        if (p >= end)
            strcpy(tzname, "Unknown/Unknown");

        /* now replace every / in the timezone name by a "!"
         * that's because the code that reads the CTZV line is
         * dumb and treats a / as a field separator...
         */
        p = tzname;
        while (1) {
            p = strchr(p, '/');
            if (p == NULL)
                break;
            *p = '!';
            p += 1;
        }
    }

   /* as a special extension, we append the name of the host's time zone to the
    * string returned with %CTZ. the system should contain special code to detect
    * and deal with this case (since it normally relied on the operator's country code
    * which is hard to simulate on a general-purpose computer
    */
    em_add_line( modem, "%%CTZV: %02d/%02d/%02d:%02d:%02d:%02d%c%d:%d:%s\r\n",
             (utc.tm_year + 1900) % 100, utc.tm_mon + 1, utc.tm_mday,
             utc.tm_hour, utc.tm_min, utc.tm_sec,
             (tzdiff >= 0) ? '+' : '-', (tzdiff >= 0 ? tzdiff : -tzdiff),
             (isdst > 0),
             tzname );
}

static const char*
handleEndOfInit( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    em_begin_line( modem );
    // em_addTimeUpdate( modem );
    return em_end_line( modem );
}


static const char*
handleListPDPContexts( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    int  nn;
    assert( !memcmp( cmd, "+CGACT?", 7 ) );
    em_begin_line( modem );
    for (nn = 0; nn < MAX_DATA_CONTEXTS; nn++) {
        ADataContext  data = modem->data_contexts + nn;
        if (!data->active)
            continue;
        em_add_line( modem, "+CGACT: %d,%d\r\n", data->id, data->active );
    }
    return em_end_line( modem );
}

static const char*
handleDefinePDPContext( const char*  cmd, Express_Modem *modem )
{
    assert( !memcmp( cmd, "+CGDCONT=", 9 ) );
    cmd += 9;
    if (cmd[0] == '?') {
        /* +CGDCONT=? is used to query the ranges of supported PDP Contexts.
         * We only really support IP ones in the emulator, so don't try to
         * fake PPP ones.
         */
        return "+CGDCONT: (1-1),\"IP\",,,(0-2),(0-4)\r\n";
    } else {
        /* template is +CGDCONT=<id>,"<type>","<apn>",,0,0 */
        int              id;
        ADataContext     data;
        char *prot, *apn;

        char buf[128];
        strcpy(buf, cmd);

        id = atoi(strtok(buf, ","));
        if (id >= MAX_DATA_CONTEXTS)
            goto BadCommand;

        data = modem->data_contexts + id;
        data->id = id;

        prot = strtok(NULL, ",");
        if (!prot)
            goto BadCommand;
        if (!strcmp(prot, "\"IP\"")) {
            data->type = A_DATA_IP;
        } else if (!strcmp(prot, "\"PPP\"")) {
            data->type = A_DATA_PPP;
        } else if (!strcmp(prot, "\"IPV6\"")) {
            data->type = A_DATA_IPV6;
        } else if (!strcmp(prot, "\"IPV4V6\"")) {
            data->type = A_DATA_IPV4V6;
        } else {
            goto BadCommand;
        }

        apn = strtok(NULL, ",");
        if (!apn || apn[0] != '"')
            goto BadCommand;
        ++apn;
        char* apn_end = strchr( cmd, '"' );
        if (!apn_end)
            goto BadCommand;
        *apn_end = 0;
        // embraced double quotes have been removed
        strcpy(data->apn, apn);

        data->active = 1;
        data->connected = 0;
    }
    return NULL;
BadCommand:
    return "ERROR: BAD COMMAND";
}

static const char*
get_network_type_str(ADataType type)
{
    if (type == A_DATA_IP)
        return "IP";
    else if (type == A_DATA_PPP)
        return "PPP";
    else if (type == A_DATA_IPV6)
        return "IPV6";
    else if (type == A_DATA_IPV4V6)
        return "IPV4V6";
    else
        return "Error";
}

static const char*
handleQueryPDPContext( const char* cmd, Express_Modem *modem )
{
    (void) cmd;
    /* WiFi uses a different gateway because there is NAT involved to get
     * both WiFi and radio networks to coexist on the single ethernet connection
     * connecting the guest to the outside world */
    // XXX: Set Wi-Fi enabled here
    // const char* gateway = feature_is_enabled(kFeature_Wifi) ? "192.168.200.2/24"
    //                                                         : "10.0.2.15/24";
    const char* gateway = "10.0.2.15/24";
    int  nn;

    em_begin_line(modem);
    for (nn = 0; nn < MAX_DATA_CONTEXTS; nn++) {
        ADataContext  data = modem->data_contexts + nn;
        if (!data->active)
            continue;
        em_add_line( modem, "+CGDCONT: %d,\"%s\",\"%s\",\"%s\",0,0\r\n",
                         data->id,
                         get_network_type_str(data->type),
                         data->apn,
                         /* Note: For now, hard-code the IP address of our
                          *       network interface
                          */
                         (data->type != A_DATA_PPP) ? gateway : "");
    }
    return em_end_line(modem);
}

static const char*
handleNDISBasedDialing( const char*  cmd, Express_Modem *modem )
{
    int cid, connect;
    sscanf(cmd, "^NDISDUP=%d,%d", &cid, &connect);
    if (cid < 0 || cid >= MAX_DATA_CONTEXTS) {
        return "ERROR: bad context number";
    }
    if (modem->data_contexts[cid].active) {
        modem->data_contexts[cid].connected = connect;
        return NULL;
    } else {
        return "ERROR: context is not defined";
    }
}

static const char*
handleQueryDHCP( const char*  cmd, Express_Modem *modem )
{
    // XXX: hard-code the ip address
    unsigned clip = 0x0f02000a;
    unsigned netmask = 0x00ffffff;
    unsigned gate = 0x0202000a;
    unsigned dhcp = 0x0f02000a;
    unsigned pDNS = 0x0302000a;
    unsigned sDNS = 0x0302000a;
    unsigned long max_rx_data = 20000000000;
    unsigned long max_tx_data = 20000000000;
    return em_printf(modem, "^DHCP: %08X,%08X,%08X,%08X,%08X,%08X,%lu,%lu",
        clip, netmask, gate, dhcp, pDNS, sDNS, max_rx_data, max_tx_data);
}

static const char*
handleStartPDPContext( const char*  cmd, Express_Modem *modem )
{
    /* XXX: TODO: handle PDP start appropriately */
    (void) cmd, (void) modem;
    return NULL;
}


static void
remote_voice_call_event( void*  _vcall, int  success )
{
    AVoiceCall  vcall = _vcall;
    Express_Modem *modem = vcall->modem;

    /* NOTE: success only means we could send the "gsm in new" command
     * to the remote emulator, nothing more */

    if (!success) {
        /* aargh, the remote emulator probably quitted at that point */
        em_free_call(modem, vcall);
        em_send_calls_update(modem);
    }
}

static void
voice_call_event( void*  _vcall )
{
    AVoiceCall  vcall = _vcall;
    ACall       call  = &vcall->call;

    switch (call->state) {
        case A_CALL_DIALING:
            call->state = A_CALL_ALERTING;

            if (vcall->is_remote) {
                if ( remote_call_dial( call->number,
                                       vcall->modem->base_port,
                                       remote_voice_call_event, vcall ) < 0 )
                {
                   /* we could not connect, probably because the corresponding
                    * emulator is not running, so simply destroy this call.
                    * XXX: should we send some sort of message to indicate BAD NUMBER ? */
                    /* it seems the Android code simply waits for changes in the list   */
                    em_free_call( vcall->modem, vcall );
                }
            } else {
               /* this is not a remote emulator number, so just simulate
                * a small ringing delay */
                // XXX: disable timer here
                // sys_timer_set( vcall->timer, sys_time_ms() + CALL_DELAY_ALERT,
                //                voice_call_event, vcall );
            }
            break;

        case A_CALL_ALERTING:
            call->state = A_CALL_ACTIVE;
            break;

        default:
            assert( 0 && "unreachable event call state" );
    }
    em_send_calls_update(vcall->modem);

}

static int em_is_emergency( Express_Modem *modem, const char *number )
{
    int i;

    if (!number) return 0;
    for (i = 0; i < MAX_EMERGENCY_NUMBERS; i++) {
        if ( modem->emergency_numbers[i] && !strcmp( number, modem->emergency_numbers[i] )) break;
    }

    if (i < MAX_EMERGENCY_NUMBERS) return 1;

    return 0;
}

static const char*
handleDial( const char*  cmd, Express_Modem *modem )
{
    AVoiceCall  vcall = em_alloc_call( modem );
    ACall       call  = &vcall->call;
    int         len;

    if (call == NULL)
        return "ERROR: TOO MANY CALLS";

    assert( cmd[0] == 'D' );
    call->dir   = A_CALL_OUTBOUND;
    call->state = A_CALL_DIALING;
    call->mode  = A_CALL_VOICE;
    call->multi = 0;

    cmd += 1;
    len  = strlen(cmd);
    if (len > 0 && cmd[len-1] == ';')
        len--;
    if (len >= (int)sizeof(call->number))
        len = sizeof(call->number)-1;

    /* Converts short number to full number */
    const char* phone_prefix = asimcard_get_phone_number_prefix(modem->sim);
    int prefix_len = strlen(phone_prefix);
    assert(prefix_len >= 7 && prefix_len <= 11);

    if((len == 4 || len == 7 || len == 10) &&
     !strncmp(cmd, &phone_prefix[prefix_len-(len-4)], len-4)) {
        int addr_prefix_len = prefix_len-(len-4);
        memcpy( call->number, phone_prefix, addr_prefix_len );
        memcpy( call->number+addr_prefix_len, cmd, len );
        call->number[len+addr_prefix_len] = 0;
    }   else {
        memcpy( call->number, cmd, len );
        call->number[len] = 0;
    }

    em_begin_line( modem );
    if (em_is_emergency(modem, call->number)) {
        modem->in_emergency_mode = 1;
        em_add_line( modem, "+WSOS: 1" );
    }
    vcall->is_remote = (remote_number_string_to_port(call->number) > 0);

    // XXX: disable timer here
    // vcall->timer = sys_timer_create();
    // sys_timer_set( vcall->timer, sys_time_ms() + CALL_DELAY_DIAL,
    //                voice_call_event, vcall );

    return em_end_line( modem );
}


static const char*
handleAnswer( const char*  cmd, Express_Modem *modem )
{
    int  nn;
    for (nn = 0; nn < modem->call_count; nn++) {
        AVoiceCall  vcall = modem->calls + nn;
        ACall       call  = &vcall->call;

        if (cmd[0] == 'A') {
            if (call->state == A_CALL_INCOMING) {
                acall_set_state( vcall, A_CALL_ACTIVE );
            }
            else if (call->state == A_CALL_ACTIVE) {
                acall_set_state( vcall, A_CALL_HELD );
            }
        } else if (cmd[0] == 'H') {
            /* ATH: hangup, since user is busy */
            if (call->state == A_CALL_INCOMING) {
                em_free_call( modem, vcall );
                break;
            }
        }
    }
    return NULL;
}

static const char *
handleCringEnabled( const char*  cmd, Express_Modem *modem )
{
    ATCommandType type = get_command_type(cmd, "+CRC");
    if (type == AT_WRITE_CMD) {
        int enabled = -1;
        sscanf(cmd, "+CRC=%d", &enabled);
        if (enabled == -1) EM_RET("ERROR: invalid arg");
        modem->ring_type_enabled = enabled;
    } else {
        EM_RET("ERROR: Unsupported type");
    }
    return NULL;
}

int android_snapshot_update_time = 0;
static time_t android_last_signal_time = 0;

static bool wakeup_from_sleep() {
    // it has not called once yet
    if (android_last_signal_time == 0) {
        return false;
    }
    // heuristics: if guest has not asked for signal strength
    // for 2 minutes, we assume it is caused by host sleep
    time_t now = time(NULL);
    const bool wakeup_from_sleep = (now > android_last_signal_time + 120);
    return wakeup_from_sleep;
}


static bool firstSignalStrengthRequest = true;
static const char*
handleSignalStrength( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    em_begin_line( modem );

    /* Sneak time updates into the SignalStrength request, because it's periodic.
     * Ideally, we'd be able to prod the guest into asking immediately on restore
     * from snapshot, but that'd require a driver.
     */
    if (android_snapshot_update_time && modem->snapshotTimeUpdateRequested) {
      em_addTimeUpdate(modem);
      modem->snapshotTimeUpdateRequested = 0;
    } else if (wakeup_from_sleep()) {
        em_addTimeUpdate(modem);
    } else if (firstSignalStrengthRequest) {
        firstSignalStrengthRequest = false;
        em_addTimeUpdate(modem);
    }

    if(modem->send_phys_channel_cfg_unsol) {
        em_addPhysChanCfgUpdate( modem );
        if(dataNetworkTypeFromInt(modem->data_network_requested) != modem->data_network) {
            em_set_data_registration( modem, modem->data_state );
        }
    }
    android_last_signal_time = time(NULL);

    if (modem->use_signal_profile) {
        signal_t current_signal = NET_PROFILES[modem->quality];
        em_add_line(modem, "+CSQ: %i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
                        current_signal.gsm_rssi, current_signal.gsm_ber,
                        current_signal.cdma_dbm, current_signal.cdma_ecio,
                        current_signal.evdo_dbm, current_signal.evdo_ecio, current_signal.evdo_snr,
                        current_signal.lte_rssi, current_signal.lte_rsrp, current_signal.lte_rsrq,
                        current_signal.lte_rssnr, current_signal.lte_cqi, current_signal.lte_timing);
    }
    else {
        // rssi = 0 (<-113dBm) 1 (<-111) 2-30 (<-109--53) 31 (>=-51) 99 (?!)
        // ber (bit error rate) - always 99 (unknown), apparently.
        // TODO: return 99 if modem->radio_state==A_RADIO_STATE_OFF, once radio_state is in snapshot.
        int rssi = modem->rssi;
        int ber = modem->ber;
        rssi = (0 > rssi || rssi > 31) ? 99 : rssi;
        ber = (0 > ber || ber > 7 ) ? 99 : ber;
        em_add_line(modem, "+CSQ: %i,%i,85,130,90,6,4,25,9,50,68,12\r\n", rssi, ber);
    }
    return em_end_line( modem );
}

static const char*
handleGetModemActivityInfo( const char*  cmd, Express_Modem *modem )
{
    (void) cmd;
    uint32_t sleep_mode_time_ms = 1000;
    uint32_t idle_mode_time_ms = 100;
    uint32_t rx_mode_time_ms = 19;
    uint32_t tx_mode_time_ms_0 = 5;
    uint32_t tx_mode_time_ms_1 = 8;
    uint32_t tx_mode_time_ms_2 = 2;
    uint32_t tx_mode_time_ms_3 = 3;
    uint32_t tx_mode_time_ms_4 = 3;

    em_begin_line( modem );

    em_add_line(modem, "+MAI: sleep=%u idle=%u rx=%u tx0=%u tx1=%u tx2=%u tx3=%u tx4=%u\r\n",
        sleep_mode_time_ms,
        idle_mode_time_ms,
        rx_mode_time_ms,
        tx_mode_time_ms_0,
        tx_mode_time_ms_1,
        tx_mode_time_ms_2,
        tx_mode_time_ms_3,
        tx_mode_time_ms_4);

    return em_end_line( modem );
}

static const char*
handleHangup( const char*  cmd, Express_Modem *modem )
{
    if ( !memcmp(cmd, "+CHLD=", 6) ) {
        int  nn;
        cmd += 6;
        switch (cmd[0]) {
            case '0':  /* release all held, and set busy for waiting calls */
                for (nn = 0; nn < modem->call_count; nn++) {
                    AVoiceCall  vcall = modem->calls + nn;
                    ACall       call  = &vcall->call;
                    if (call->mode != A_CALL_VOICE)
                        continue;
                    if (call->state == A_CALL_HELD    ||
                        call->state == A_CALL_WAITING ||
                        call->state == A_CALL_INCOMING) {
                        em_free_call(modem, vcall);
                        nn--;
                    }
                }
                break;

            case '1':
                if (cmd[1] == 0) { /* release all active, accept held one */
                    for (nn = 0; nn < modem->call_count; nn++) {
                        AVoiceCall  vcall = modem->calls + nn;
                        ACall       call  = &vcall->call;
                        if (call->mode != A_CALL_VOICE)
                            continue;
                        if (call->state == A_CALL_ACTIVE) {
                            em_free_call(modem, vcall);
                            nn--;
                        }
                        else if (call->state == A_CALL_HELD     ||
                                 call->state == A_CALL_WAITING) {
                            acall_set_state( vcall, A_CALL_ACTIVE );
                        }
                    }
                } else {  /* release specific call */
                    int  id = cmd[1] - '0';
                    AVoiceCall  vcall = em_find_call( modem, id );
                    if (vcall != NULL)
                        em_free_call( modem, vcall );
                }
                break;

            case '2':
                if (cmd[1] == 0) {  /* place all active on hold, accept held or waiting one */
                    for (nn = 0; nn < modem->call_count; nn++) {
                        AVoiceCall  vcall = modem->calls + nn;
                        ACall       call  = &vcall->call;
                        if (call->mode != A_CALL_VOICE)
                            continue;
                        if (call->state == A_CALL_ACTIVE) {
                            acall_set_state( vcall, A_CALL_HELD );
                        }
                        else if (call->state == A_CALL_HELD     ||
                                 call->state == A_CALL_WAITING) {
                            acall_set_state( vcall, A_CALL_ACTIVE );
                        }
                    }
                } else {  /* place all active on hold, except a specific one */
                    int   id = cmd[1] - '0';
                    for (nn = 0; nn < modem->call_count; nn++) {
                        AVoiceCall  vcall = modem->calls + nn;
                        ACall       call  = &vcall->call;
                        if (call->mode != A_CALL_VOICE)
                            continue;
                        if (call->state == A_CALL_ACTIVE && call->id != id) {
                            acall_set_state( vcall, A_CALL_HELD );
                        }
                    }
                }
                break;

            case '3':  /* add a held call to the conversation */
                for (nn = 0; nn < modem->call_count; nn++) {
                    AVoiceCall  vcall = modem->calls + nn;
                    ACall       call  = &vcall->call;
                    if (call->mode != A_CALL_VOICE)
                        continue;
                    if (call->state == A_CALL_HELD) {
                        acall_set_state( vcall, A_CALL_ACTIVE );
                        break;
                    }
                }
                break;

            case '4':  /* connect the two calls */
                for (nn = 0; nn < modem->call_count; nn++) {
                    AVoiceCall  vcall = modem->calls + nn;
                    ACall       call  = &vcall->call;
                    if (call->mode != A_CALL_VOICE)
                        continue;
                    if (call->state == A_CALL_HELD) {
                        acall_set_state( vcall, A_CALL_ACTIVE );
                        break;
                    }
                }
                break;
        }
    }
    else
        return "ERROR: BAD COMMAND";

    return NULL;
}

static const char*
handleExtendSystemInfoQuery( const char*  cmd, Express_Modem *modem )
{
    return NULL;
}

static const char*
handleExtendSystemConfigure( const char*  cmd, Express_Modem *modem )
{
    ATCommandType type = get_command_type(cmd, "^SYSCFGEX");
    if (type == AT_READ_CMD) {
        EM_RET("^SYSCFGEX: \"00\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF\r\n");
    } else {
        EM_RET("ERROR: Unsupported");
    }
}

static const char*
handleIndicateNewMessage(const char *cmd, Express_Modem *modem)
{
    ATCommandType type = get_command_type(cmd, "+CNMI");
    if (type == AT_WRITE_CMD) {
        int num, mode, mt, bm, ds, bfr;
        num = sscanf(cmd, "+CNMI=%d,%d,%d,%d,%d", &mode, &mt, &bm, &ds, &bfr);
        if (num == 5 && mode == 1 && mt == 2 && (bm == 0 || bm == 1) && ds == 1 && bfr == 1) {
            return NULL;
        }
        EM_RET("ERROR: Unsupported");
    } else {
        EM_RET("ERROR: Unsupported");
    }
}

static const char*
handleQueryAndReportSignalStrength(const char *cmd, Express_Modem *modem)
{
    ATCommandType type = get_command_type(cmd, "^HCSQ");
    if (type == AT_READ_CMD) {
        // TODO!
        EM_RET("^HCSQ: \"WCDMA\",30,30,58,0");
    } else {
        EM_RET("ERROR: Unsupported");
    }
}

static const char*
handleSelectMessageStorage(const char *cmd, Express_Modem *modem)
{
    EM_RET("+CPMS: 2,40,2,40,2,40");
}

static const char*
handleVoiceDomainPreference(const char *cmd, Express_Modem *modem)
{
    ATCommandType type = get_command_type(cmd, "+CEVDP");
    if (type == AT_READ_CMD) {
        EM_RET("+CEVDP: 3");
    } else {
        EM_RET("ERROR: Unsupported");
    }
}

static const char*
handleRequestEmergencyNumber(const char *cmd, Express_Modem *modem)
{
    em_begin_line(modem);
    for (int i = 0; modem->emergency_numbers[i]; ++i) {
        em_add_line(modem, "^XLEMA: %d,%d,%s,0,1,fff,0\r\n", i + 1, modem->nr_emergency_numbers, modem->emergency_numbers[i]);
    }
    return em_end_line(modem);
}

static const char*
handleMonitoringServingCellInformation(const char *cmd, Express_Modem *modem)
{
    // XXX: Hard-coded here!
    EM_RET("^MONSC: WCDMA,472,99,10738,304,120514,2513,-79,-73,-6,6,0");
}

static const char*
handleNVM(const char *cmd, Express_Modem *modem)
{
    EM_RET(NULL);
}

static const char*
handleRequestCurrentTime(const char *cmd, Express_Modem *modem)
{
    time_t currentTime;
    struct tm *timeInfo;
    char formattedTime[80];

    // Get current time
    time(&currentTime);
    // Convert current time to struct tm
    timeInfo = localtime(&currentTime);
    // Format time as a string
    strftime(formattedTime, sizeof(formattedTime), "%Y/%m/%d,%H:%M:%S", timeInfo);

#ifdef __APPLE__
    int tz_offset_hours = timeInfo->tm_gmtoff / 3600;
#else
    // XXX: hard-coding here
    int tz_offset_hours = 8;
#endif

    return em_printf(modem, "+CCLK: \"%s%+d\"", formattedTime, tz_offset_hours);
}

/* a function used to deal with a non-trivial request */
typedef const char*  (*ResponseHandler)(const char*  cmd, Express_Modem *modem);

static const struct {
    const char*      cmd;     /* command coming from qemu-ril.so, if first
                                 character is '!', then the rest is a prefix only */

    const char*      answer;  /* default answer, NULL if needs specific handling or
                                 if OK is good enough */

    ResponseHandler  handler; /* specific handler, ignored if 'answer' is not NULL,
                                 NULL if OK is good enough */
} sDefaultResponses[] =
{
    /* see onRadioPowerOn() */
    { "%CPHS=1", NULL, NULL },
    { "%CTZV=1", NULL, NULL },
    { "%CGFPCCFG=1", NULL, enableGoldfishPhysicalChannelConfigUnsol},

    /* SMS */
    { "+CSMS=1", "+CSMS: 1, 1, 1", NULL },
    { "!+CNMI", NULL, handleIndicateNewMessage},
    {"!+CPMS", NULL, handleSelectMessageStorage},

    /* see requestRadioPower() */
    { "!+CFUN=", NULL, handleRadioPower },

    /* Signal */
    {"^HCSQ=3,10", NULL, NULL},
    {"^HCSQ?", NULL, handleQueryAndReportSignalStrength},
    
    /* Call */
    {"!+CRC", NULL, handleCringEnabled},
    {"!+CEVDP", NULL, handleVoiceDomainPreference},
    {"^XLEMA?", NULL, handleRequestEmergencyNumber},
    {"!^NVM=", NULL, NULL},

    /* Network */
    {"^MONSC", NULL, handleMonitoringServingCellInformation},

    /* MT Control */
    {"^CARDMODE", "^CARDMODE: 1", NULL}, /* SIM card type == 1 */
    {"+CCLK?", NULL, handleRequestCurrentTime},

    /* STK command */
    {"+SPUSATPROFILE?", "+SPUSATPROFILE: 1", NULL},

    /* Modem command */
    {"+MEID", "449144930318378", NULL},

    /* Data command */
    {"!+CGDCONT=", NULL, handleDefinePDPContext},
    {"+CGDCONT?", NULL, handleQueryPDPContext},
    {"!^AUTHDATA=", NULL, NULL},
    {"!^NDISDUP=", NULL, handleNDISBasedDialing},
    {"^DHCP?", NULL, handleQueryDHCP},

    /* Misc */
    {"^SIMST=1", NULL, NULL},
    {"^CURCEX=2,F7FFFFFFFFFFFF", NULL, NULL},
    {"+CIREG=2", NULL, NULL},
    {"+CSCS=\"IRA\"", NULL, NULL},
    {"^TIME=1", NULL, NULL},
    {"+CTZR=1", NULL, NULL},
    {"+CIREP=1", NULL, NULL},

    { "+CTEC=?", "+CTEC: 0,1,2,3", NULL }, /* Query available Techs */
    { "!+CTEC", NULL, handleTech }, /* Set/get current Technology and preferred mode */

    { "+WRMP=?", "+WRMP: 0,1,2", NULL }, /* Query Roam Preference */
    { "!+WRMP", NULL, handleRoamPref }, /* Set/get Roam Preference */

    { "+CCSS=?", "+CTEC: 0,1", NULL }, /* Query available subscription sources */
    { "!+CCSS", NULL, handleSubscriptionSource }, /* Set/Get current subscription source */

    { "+WSOS=?", "+WSOS: 0", NULL}, /* Query supported +WSOS values */
    { "!+WSOS=", NULL, handleEmergencyMode },

    { "+WPRL?", NULL, handlePrlVersion }, /* Query the current PRL version */

    /* see requestOrSendPDPContextList() */
    { "+CGACT?", NULL, handleListPDPContexts },

    /* see requestOperator() */
    { "+COPS=3,0;+COPS?;+COPS=3,1;+COPS?;+COPS=3,2;+COPS?", NULL, handleRequestOperator },

    /* see requestQueryNetworkSelectionMode() */
    { "!+COPS", NULL, handleOperatorSelection },

    /* see requestGetCurrentCalls() */
    { "+CLCC", NULL, handleListCurrentCalls },

    /* see requestWriteSmsToSim() */
    { "!+CMGW=", NULL, handleSendSMStoSIM },

    /* see requestHangup() */
    { "!+CHLD=", NULL, handleHangup },

    /* see requestSignalStrength() */
    { "+CSQ", NULL, handleSignalStrength },

    /* modem activity info */
    { "+MAI", NULL, handleGetModemActivityInfo },

    /* see requestRegistrationState() */
    { "!+CREG", NULL, handleNetworkRegistration },
    { "!+CGREG", NULL, handleNetworkRegistration },

    /* see requestSendSMS() */
    { "!+CMGS=", NULL, handleSendSMS },

    /* see requestSetupDefaultPDP() */
    { "%CPRIM=\"GMM\",\"CONFIG MULTISLOT_CLASS=<10>\"", NULL, NULL },
    { "%DATA=2,\"UART\",1,,\"SER\",\"UART\",0", NULL, NULL },

    { "+CGQREQ=1", NULL, NULL },
    { "+CGQMIN=1", NULL, NULL },
    { "+CGEREP=1,0", NULL, NULL },
    { "+CGACT=1,0", NULL, NULL },
    { "D*99***1#", NULL, handleStartPDPContext },

    /* see requestDial() */
    { "!D", NULL, handleDial },  /* the code says that success/error is ignored, the call state will
                              be polled through +CLCC instead */

    /* see requestSMSAcknowledge() */
    { "+CNMA=1", NULL, NULL },
    { "+CNMA=2", NULL, NULL },

    /* see requestSIM_IO() */
    { "!+CRSM=", NULL, handleSIM_IO },

    /* b/37719621 */
    { "!+CSIM=", NULL, handleCSIM},

    /* see onRequest() */
    { "+CHLD=0", NULL, handleHangup },
    { "+CHLD=1", NULL, handleHangup },
    { "+CHLD=2", NULL, handleHangup },
    { "+CHLD=3", NULL, handleHangup },
    { "A", NULL, handleAnswer },  /* answer the call */
    { "H", NULL, handleAnswer },  /* user is busy */
    { "!+VTS=", NULL, handleSetDialTone },
    { "+CIMI", DEFCONF_OPERATOR_HOME_MCCMNC "000000000", NULL },   /* request internation subscriber identification number */

    /* request model version:
     *          35824005        - Nexus 5 (https://en.wikipedia.org/wiki/Type_Allocation_Code)
     *                  111111  - serial (random)
     *                        0 - check digit (http://www.imei.info/calc)
     */
    // XXX: change model number here
    { "+CGSN", "358240051111110", NULL },
    { "+CUSD=2",NULL, NULL }, /* Cancel USSD */
    { "+COPS=0", NULL, handleOperatorSelection }, /* set network selection to automatic */
    { "!+CMGD=", NULL, handleDeleteSMSonSIM }, /* delete SMS on SIM */
    { "!+CPIN=", NULL, handleChangeOrEnterPIN },

    /* see getSIMStatus() */
    { "+CPIN?", NULL, handleSIMStatusReq },
    { "+CNMI?", "+CNMI: 1,2,2,1,1", NULL },

    /* set carrier restrictions */
    { "!+CRRSTR=", NULL, handleSetCarrierRestrictionsReq },

    /* see isRadioOn() */
    { "+CFUN?", NULL, handleRadioPowerReq },

    /* see initializeCallback() */
    { "E0Q0V1", NULL, NULL },
    { "S0=0", NULL, NULL },
    { "+CMEE=1", NULL, NULL },
    { "+CCWA=1", NULL, NULL },
    { "+CMOD=0", NULL, NULL },
    { "+CMUT=0", NULL, NULL },
    { "+CSSN=0,1", NULL, NULL },
    { "+COLP=0", NULL, NULL },
    { "+CSCS=\"HEX\"", NULL, NULL },
    { "+CUSD=1", NULL, NULL },
    { "+CGEREP=1,0", NULL, NULL },
    { "+CMGF=0", NULL, handleEndOfInit },  /* now is a good time to send the current time and timezone */
    { "%CPI=3", NULL, NULL },
    { "%CSTAT=1", NULL, NULL },

    /* Logical channels */
    { "!+CCHO=", NULL, handleOpenLogicalChannel },
    { "!+CCHC=", NULL, handleCloseLogicalChannel },
    { "!+CGLA=", NULL, handleTransmitLogicalChannel },

    /* Open Harmony OS RIL */
    {"^SYSINFOEX", NULL, handleExtendSystemInfoQuery },
    {"!^SYSCFGEX", NULL, handleExtendSystemConfigure },

    /* end of list */
    {NULL, NULL, NULL}
};


#define  REPLY(str)  do { const char*  s = (str); LOGD(">> %s\n", quote(s)); return s; } while (0)

const char*  em_send( Express_Modem *modem, const char*  cmd )
{
    const char*  answer;

    if ( modem->wait_sms != 0 ) {
        modem->wait_sms = 0;
        LOGD( "SMS<< %s\n", quote(cmd) );
        answer = handleSendSMSText( cmd, modem );
        REPLY(answer);
    }

    /* everything that doesn't start with 'AT' is not a command, right ? */
    if ( cmd[0] != 'A' || cmd[1] != 'T' || cmd[2] == 0 ) {
        /* LOGD( "-- %s\n", quote(cmd) ); */
        return NULL;
    }
    LOGD( "<< %s\n", quote(cmd) );

    cmd += 2;

    /* TODO: implement command handling */
    {
        int  nn, found = 0;

        for (nn = 0; ; nn++) {
            const char*  scmd = sDefaultResponses[nn].cmd;

            if (!scmd) /* end of list */
                break;

            if (scmd[0] == '!') { /* prefix match */
                int  len = strlen(++scmd);

                if ( !memcmp( scmd, cmd, len ) ) {
                    found = 1;
                    break;
                }
            } else { /* full match */
                if ( !strcmp( scmd, cmd ) ) {
                    found = 1;
                    break;
                }
            }
        }

        if ( !found )
        {
            LOGD( "** UNSUPPORTED COMMAND **\n" );
            REPLY( "ERROR: UNSUPPORTED" );
        }
        else
        {
            const char*      answer  = sDefaultResponses[nn].answer;
            ResponseHandler  handler = sDefaultResponses[nn].handler;

            if ( answer != NULL ) {
                REPLY( em_printf( modem, "%s\rOK", answer ) );
            }

            if (handler == NULL) {
                REPLY( "OK" );
            }

            answer = handler( cmd, modem );
            if (answer == NULL)
                REPLY( "OK" );

            if ( !memcmp( answer, "> ", 2 )     ||
                 !memcmp( answer, "ERROR", 5 )  ||
                 !memcmp( answer, "+CME ERROR", 6 ) )
            {
                REPLY( answer );
            }

            if (answer != modem->out_buff)
                REPLY( em_printf( modem, "%s\rOK", answer ) );

            strcat( modem->out_buff, "\rOK" );
            REPLY( answer );
        }
    }
}

const char* em_send_unsol_nitz( Express_Modem *modem )
{
    em_addTimeUpdate(modem);
    REPLY(em_end_line(modem));
}

static void
express_modem_receive_sms( Express_Modem *modem, char *from, char *sms)
{
    // Get the "from" number
    SmsAddressRec sender;
    int retVal = sms_address_from_str(&sender, from, strlen(from));
    if (retVal < 0  ||  sender.len <= 0) {
        LOGE("The \"From\" number is invalid.");
        return;
    }

    // Convert the message text to UTF-8
    unsigned char utf8Message[MAX_SMS_MSG_SIZE+1];
    int           nUtf8Chars;
    nUtf8Chars = sms_utf8_from_message_str(sms, strlen(sms),
                                           utf8Message, MAX_SMS_MSG_SIZE);
    if (nUtf8Chars == 0) {
        LOGE("The message is empty. Please enter a message.");
        return;
    }

    if (nUtf8Chars < 0) {
        LOGE("The message contains invalid characters.");
        return;
    }

    // Create a list of SMS PDUs, then send them
    SmsPDU *pdus = smspdu_create_deliver_utf8(utf8Message, nUtf8Chars,
                                              &sender, NULL);
    if (pdus == NULL) {
        LOGE("The message contains invalid characters.");
        return;
    }

    if (em_get_radio_state(modem) == A_RADIO_STATE_OFF) {
        LOGE("Cannot send message, radio is off.");
        return;
    }

    for (int idx = 0; pdus[idx] != NULL; idx++) {
        em_receive_sms(modem, pdus[idx]);
    }

    smspdu_free_list( pdus );
}

struct Express_Modem *modems = NULL;

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

    LOGE("RIL connection closed, restarting...");
}

#define RIL_MODEM_PORT 28256

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
    // .option_name = "modem",
    .device_id = EXPRESS_MODEM_DEVICE_ID,
    .device_type = INPUT_DEVICE_TYPE | OUTPUT_DEVICE_TYPE,

    .static_prop = NULL,
    .static_prop_size = 0,
};

static void __attribute__((constructor))
express_thread_init_express_modem(void) {
    express_device_init_common(&express_modem_info);
    modems = g_malloc(NR_MODEM * sizeof(Express_Modem));
    memset(modems, 0, NR_MODEM * sizeof(Express_Modem));
    for (unsigned long long i = 0; i < NR_MODEM; ++i) {
        qemu_thread_create(&em_thread_id[i], "modem", 
                        em_thread, (void*)i, QEMU_THREAD_DETACHED);
    }
}
