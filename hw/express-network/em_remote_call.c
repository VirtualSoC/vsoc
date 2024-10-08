/**
 * @file express_remote_call.c
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "hw/express-network/em_remote_call.h"

// #define STD_DEBUG_LOG
#define EM_DEBUG_SOCKET
#include "hw/teleport-express/express_log.h"

#include "hw/express-network/em_gsm.h"
#include "hw/express-network/em_utils.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define errno_str strerror(errno)

// Debug logs.
#ifdef EM_DEBUG_SOCKET
#define LOGS LOGD
#else
#define LOGS(...) ;
#endif

// XXX: Set phone number utilities as macros
#define get_phone_number_prefix() "861801234"
#define get_phone_number(from) "8618012345678" 

// XXX: fake sysdeps here
enum {
    SYS_EVENT_READ  = 0x01,
    SYS_EVENT_WRITE = 0x02,
    SYS_EVENT_ERROR = 0x04,
    SYS_EVENT_ALL   = 0x07
};
typedef int *SysChannel;
static int x;
#define sys_channel_close(a)
#define sys_channel_create_tcp_client(a, b) &x;
#define sys_channel_on(a, b, c, d)
#define sys_channel_read(a, b, c) 0
#define sys_channel_write(a, b, c) 0


/** By convention, remote numbers are the console ports, i.e. 5554, 5556, etc...
 **/
#define  REMOTE_NUMBER_BASE       5554
#define  REMOTE_NUMBER_MAX        16
#define  REMOTE_NUMBER_MAX_CHARS  4
#define  REMOTE_CONSOLE_PORT      5554

int
remote_number_from_port( int  port )
{
    if (port & 1)  /* must be even */
        return -1;

    port = (port - REMOTE_CONSOLE_PORT) >> 1;
    if ((unsigned)port >= REMOTE_NUMBER_MAX)
        return -1;

    return REMOTE_NUMBER_BASE + port*2;
}

int
remote_number_to_port( int  number )
{
    if (number & 1)  /* must be even */
        return -1;

    number = (number - REMOTE_NUMBER_BASE) >> 1;
    if ((unsigned)number >= REMOTE_NUMBER_MAX)
        return -1;

    return REMOTE_CONSOLE_PORT + number*2;
}

int
remote_number_string_to_port( const char*  number )
{
    char*  end;
    long   num;
    const char*  temp = number;
    int    len;

    len = strlen(number);
    if (len > 0 && number[len-1] == ';')
        len--;
    if (len > 0 && number[0] == '+') {
        len--;
        temp++;
    }
    if (len == 11 && !strncmp(temp, get_phone_number_prefix(), 7))
        temp += 7;
    num = strtol( temp, &end, 10 );

    if (end == NULL || *end || (int)num != num )
        return -1;

    return remote_number_to_port( (int)num );
}

/** REMOTE CALL OBJECTS
 **/

typedef struct RemoteCallRec {
    struct RemoteCallRec*   next;
    struct RemoteCallRec**  pref;
    RemoteCallType          type;
    int                     to_port;
    int                     from_port;
    SysChannel              channel;
    RemoteResultFunc        result_func;
    void*                   result_opaque;

    char                    quitting;

    /* the output buffer */
    char*                   buff;
    int                     buff_pos;
    int                     buff_len;
    int                     buff_size;
    char                    buff0[128];

} RemoteCallRec, *RemoteCall;

static void
remote_call_done( RemoteCall  call )
{
    call->pref[0] = call->next;
    call->next    = NULL;
    call->pref    = &call->next;

    if (call->buff && call->buff != call->buff0) {
        free(call->buff);
        call->buff      = call->buff0;
        call->buff_size = (int) sizeof(call->buff0);
    }

    if ( call->channel ) {
        sys_channel_close( call->channel );
        call->channel = NULL;
    }

    call->buff_pos = 0;
    call->buff_len = 0;
}


static void
remote_call_free( RemoteCall  call )
{
    if (call) {
        remote_call_done( call );
        free(call);
    }
}


static void  remote_call_event( void*  opaque, int  events );  /* forward */

static RemoteCall
remote_call_alloc( RemoteCallType  type, int  to_port, int  from_port )
{
    RemoteCall  rcall    = calloc( sizeof(*rcall), 1 );
    int         from_num = remote_number_from_port(from_port);
    (void) from_num; // XXX

    if (rcall != NULL) {
        char  *p, *end;

        rcall->pref      = &rcall->next;
        rcall->type      = type;
        rcall->to_port   = to_port;
        rcall->from_port = from_port;
        rcall->buff      = rcall->buff0;
        rcall->buff_size = sizeof(rcall->buff0);
        rcall->buff_pos  = 0;

        p   = rcall->buff;
        end = p + rcall->buff_size;

        // XXX
        // check if we need to authenticate first
        // char* auth_token = android_console_auth_get_token_dup();
        char *auth_token = NULL;
        if (auth_token) {
            if (auth_token[0] != 0) {
                // there's a token - add an 'auth' command
                p = bufprint(p, end, "auth %s\n", auth_token);
            }
            free(auth_token);
        } else {
            LOGD("%s: couldn't read an auth token, "
              "skipping authentication\n", __func__);
        }

        switch (type) {
            case REMOTE_CALL_DIAL:
                p = bufprint(p, end, "gsm call %s\n", get_phone_number(from_num) );
                break;

            case REMOTE_CALL_BUSY:
                p = bufprint(p, end, "gsm busy %s\n", get_phone_number(from_num) );
                break;

            case REMOTE_CALL_HOLD:
                p = bufprint(p, end, "gsm hold %s\n", get_phone_number(from_num) );
                break;

            case REMOTE_CALL_ACCEPT:
                p = bufprint(p, end, "gsm accept %s\n", get_phone_number(from_num) );
                break;

            case REMOTE_CALL_HANGUP:
                p = bufprint(p, end, "gsm cancel %s\n", get_phone_number(from_num) );
                break;

            default:
                ;
        }
        if (p >= end) {
            LOGD("%s: buffer too short\n", __FUNCTION__ );
            remote_call_free(rcall);
            return NULL;
        }

        rcall->buff_len = p - rcall->buff;

        rcall->channel = sys_channel_create_tcp_client( "localhost", to_port );
        if (rcall->channel == NULL) {
            LOGD("%s: could not create channel to port %d\n", __FUNCTION__, to_port);
            remote_call_free(rcall);
            return NULL;
        }

        sys_channel_on( rcall->channel, SYS_EVENT_WRITE, remote_call_event, rcall );
    }
    return  rcall;
}


static int
remote_call_set_sms_pdu( RemoteCall  call,
                         SmsPDU      pdu )
{
    char  *p, *end;
    int    msg2len, buff_msg_len;

    // Append an sms command to whatever is in the buffer already.
    buff_msg_len = strlen(call->buff);
    msg2len = 32 + smspdu_to_hex( pdu, NULL, 0 );
    if (msg2len > call->buff_size - buff_msg_len) {
        char*  new_buff = malloc(msg2len + call->buff_size);
        if (new_buff == NULL) {
            LOGD("%s: not enough memory to alloc %d bytes", __FUNCTION__, msg2len);
            return -1;
        }
        memcpy(new_buff, call->buff, buff_msg_len + 1);
        call->buff      = new_buff;
        call->buff_size = msg2len + call->buff_size;
    }

    p   = call->buff + buff_msg_len;
    end = call->buff + call->buff_size;

    p  = bufprint(p, end, "sms pdu ");
    assert(p < end);

    p += smspdu_to_hex( pdu, p, end-p );
    assert(p + 1 < end);

    *p++ = '\n';
    *p = 0;

    call->buff_len = p - call->buff;
    call->buff_pos = 0;
    return 0;
}


static void
remote_call_add( RemoteCall   call,
                 RemoteCall  *plist )
{
    RemoteCall  first = *plist;

    call->next = first;
    call->pref = plist;

    if (first)
        first->pref = &call->next;
}

static void // XXX
remote_call_event( void*  opaque, int  events )
{
    RemoteCall  call = opaque;

    LOGS("%s: called for call (%d,%d), events=%02x\n", __FUNCTION__,
       call->from_port, call->to_port, events);

    if (events & SYS_EVENT_READ) {
        /* simply drain the channel */
        char  temp[32];
        temp[0] = 0; // XXX
        int  n = sys_channel_read( call->channel, temp, sizeof(temp) );
        if (n <= 0) {
            /* remote emulator probably quitted */
            //LOGS("%s: emulator %d quitted with %d: %s\n", __FUNCTION__, call->to_port, errno, errno_str);
            remote_call_free( call );
            return;
        }
    }

    if (events & SYS_EVENT_WRITE) {
        int  n;

#ifdef EM_DEBUG_SOCKET
            int  nn;
            LOGS("%s: call (%d,%d) sending %d bytes '", __FUNCTION__,
            call->from_port, call->to_port, call->buff_len - call->buff_pos );
            for (nn = call->buff_pos; nn < call->buff_len; nn++) {
                int  c = call->buff[nn];
                if (c < 32) {
                    if (c == '\n')
                        LOGS("\\n");
                    else if (c == '\t')
                        LOGS("\\t");
                    else if (c == '\r')
                        LOGS("\\r");
                    else
                        LOGS("\\x%02x", c);
                } else
                    LOGS("%c", c);
            }
            LOGS("'\n");
#endif

        n = sys_channel_write( call->channel,
                               call->buff + call->buff_pos,
                               call->buff_len - call->buff_pos );
        if (n <= 0) {
            /* remote emulator probably quitted */
            LOGS("%s: emulator %d quitted unexpectedly with error %d: %s\n",
                    __FUNCTION__, call->to_port, errno, errno_str);
            if (call->result_func)
                call->result_func( call->result_opaque, 0 );
            remote_call_free( call );
            return;
        }
        call->buff_pos += n;

        if (call->buff_pos >= call->buff_len) {
            /* cool, we sent everything */
            LOGS("%s: finished sending data to %d\n", __FUNCTION__, call->to_port);
            if (!call->quitting) {
                    call->quitting = 1;
                    sprintf( call->buff, "quit\n" );
                    call->buff_len = strlen(call->buff);
                    call->buff_pos = 0;
            } else {
                call->quitting = 0;
                if (call->result_func)
                    call->result_func( call->result_opaque, 1 );

                sys_channel_on( call->channel, SYS_EVENT_READ, remote_call_event, call );
            }
        }
    }
}

static RemoteCall  _the_remote_calls;

static RemoteCall
remote_call_generic( RemoteCallType  type, const char*  to_number, int  from_port )
{
    int         to_port = remote_number_string_to_port(to_number);
    RemoteCall  call;

    if ( remote_number_from_port(from_port) < 0 ) {
        LOGD("%s: from_port value %d is not valid", __FUNCTION__, from_port);
        return NULL;
    }
    if ( to_port < 0 ) {
        LOGD("%s: phone number '%s' is not decimal or remote", __FUNCTION__, to_number);
        return NULL;
    }
    /* You _can_ send an SMS to yourself. Didja know? */
    if (to_port == from_port && type != REMOTE_CALL_SMS) {
        LOGD("%s: trying to call self\n", __FUNCTION__);
        return NULL;
    }
    call = remote_call_alloc( type, to_port, from_port );
    if (call == NULL) {
        return NULL;
    }
    remote_call_add( call, &_the_remote_calls );
    LOGD("%s: adding new call from port %d to port %d\n", __FUNCTION__, from_port, to_port);
    return call;
}


int
remote_call_dial( const char*       number,
                  int               from,
                  RemoteResultFunc  result_func,
                  void*             result_opaque )
{
    RemoteCall   call = remote_call_generic( REMOTE_CALL_DIAL, number, from );

    if (call != NULL) {
        call->result_func   = result_func;
        call->result_opaque = result_opaque;
    }
    return call ? 0 : -1;
}


void
remote_call_other( const char*  to_number, int  from_port, RemoteCallType  type )
{
    remote_call_generic( type, to_number, from_port );
}

/* call this function to send a SMS to a remote emulator */
int
remote_call_sms( const char*   number,
                 int           from,
                 SmsPDU        pdu )
{
    RemoteCall   call = remote_call_generic( REMOTE_CALL_SMS, number, from );

    if (call == NULL)
        return -1;

    if (call != NULL) {
        if ( remote_call_set_sms_pdu( call, pdu ) < 0 ) {
            remote_call_free(call);
            return -1;
        }
    }
    return call ? 0 : -1;
}


void
remote_call_cancel( const char*  to_number, int  from_port )
{
    remote_call_generic( REMOTE_CALL_HANGUP, to_number, from_port );
}
