/**
 * @file em_remote_call.h
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef EM_REMOTE_CALL_H
#define EM_REMOTE_CALL_H

#include "hw/express-network/em_sms.h"

#ifdef __cplusplus
extern "C" {
#endif

/* convert a base console port into a remote phone number, -1 on error */
extern int         remote_number_from_port( int  port );

/* convert a remote phone number into a remote console port, -1 on error */
extern int         remote_number_to_port( int  number );

extern int         remote_number_string_to_port( const char*  number );

typedef void   (*RemoteResultFunc)( void*  opaque, int  success );

typedef enum {
    REMOTE_CALL_DIAL = 0,
    REMOTE_CALL_BUSY,
    REMOTE_CALL_HANGUP,
    REMOTE_CALL_HOLD,
    REMOTE_CALL_ACCEPT,
    REMOTE_CALL_SMS
} RemoteCallType;

/* call this function when you need to dial a remote voice call.
 * this will try to connect to a remote emulator. the result function
 * is called to indicate success or failure after some time.
 *
 * returns 0 if the number is to a remote phone, or -1 otherwise
 */
extern  int     remote_call_dial( const char*       to_number,
                                  int               from_port,
                                  RemoteResultFunc  result_func,
                                  void*             result_opaque );

/* call this function to send a SMS to a remote emulator */
extern int      remote_call_sms( const char*   number, int  from_port, SmsPDU  pdu );

/* call this function to indicate that you're busy to a remote caller */
extern void     remote_call_other( const char*  to_number, int  from_port, RemoteCallType  type );

extern void     remote_call_cancel( const char*  to_number, int from_port );

#ifdef __cplusplus
}
#endif

#endif