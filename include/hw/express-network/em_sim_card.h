/**
 * @file em_sim_card.h
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef EM_SIM_CARD_H
#define EM_SIM_CARD_H

#include "hw/express-network/em_gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** ANDROID CONFIGURATION FILE SUPPORT
 **
 ** A configuration file is loaded as a simplre tree of (key,value)
 ** pairs. keys and values are simple strings
 **/
typedef struct AConfig  AConfig;

struct AConfig {
    const char *name;
    const char *value;
}; 

extern AConfig* aconfig_get();

/* parse a file into a config node tree, return 0 in case of success, -1 otherwise */
extern int    aconfig_load_file(AConfig*  root, const char*  path);

/* save a config node tree into a file, return 0 in case of success, -1 otherwise */
extern int aconfig_save_file(const AConfig* root, const char* path);

/* locate a named child of a config node */
extern const AConfig* aconfig_find_const(const AConfig* root, const char* name);

/* locate a named child of a config node. NULL if it doesn't exist. */
extern AConfig* aconfig_find(AConfig* root, const char* name);

/* add a named child to a config node (or modify it if it already exists) */
extern void aconfig_set(AConfig* root, const char* name, const char* value);

/* look up a child by name and return its value, eventually converted
 * into a boolean or integer */
extern int aconfig_bool(const AConfig* root, const char* name, int _default);
extern unsigned aconfig_unsigned(const AConfig* root,
                                 const char* name,
                                 unsigned _default);
extern int aconfig_int(const AConfig* root, const char* name, int _default);
extern const char* aconfig_str(const AConfig* root,
                               const char* name,
                               const char* _default);

typedef struct ASimCardRec_*    ASimCard;

extern ASimCard  asimcard_create( int sim_present );
extern void      asimcard_destroy( ASimCard  sim );

typedef enum {
    A_SIM_STATUS_ABSENT = 0,
    A_SIM_STATUS_NOT_READY,
    A_SIM_STATUS_READY,
    A_SIM_STATUS_PIN,
    A_SIM_STATUS_PUK,
    A_SIM_STATUS_NETWORK_PERSONALIZATION
} ASimStatus;

extern ASimStatus  asimcard_get_status( ASimCard  sim );
extern void        asimcard_set_status( ASimCard  sim, ASimStatus  status );

extern const char*  asimcard_get_pin( ASimCard  sim );
extern const char*  asimcard_get_puk( ASimCard  sim );
extern void         asimcard_set_pin( ASimCard  sim, const char*  pin );
extern void         asimcard_set_puk( ASimCard  sim, const char*  puk );
extern void         asimcard_set_fileid_status( ASimCard sim, const char* str );

extern int         asimcard_check_pin( ASimCard  sim, const char*  pin );
extern int         asimcard_check_puk( ASimCard  sim, const char*  puk, const char*  pin );

extern const char *asimcard_get_phone_number( ASimCard sim );
extern const char *asimcard_get_phone_number_prefix( ASimCard sim );

/* Restricted SIM Access command, as defined by 8.18 of 3GPP 27.007 */
typedef enum {
    A_SIM_CMD_READ_BINARY = 176,
    A_SIM_CMD_READ_RECORD = 178,
    A_SIM_CMD_GET_RESPONSE = 192,
    A_SIM_CMD_UPDATE_BINARY = 214,
    A_SIM_CMD_UPDATE_RECORD = 220,
    A_SIM_CMD_STATUS = 242
} ASimCommand;

extern const char*  asimcard_io( ASimCard  sim, const char*  cmd );

/* handle the authentication request only */
extern const char*  asimcard_csim( ASimCard  sim, const char*  cmd );

#ifdef __cplusplus
}
#endif

#endif