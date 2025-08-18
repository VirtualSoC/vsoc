/**
 * @file em_utils.c
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-26
 *
 * @copyright Copyright (c) 2022
 *
 */

 /** HEXADECIMAL CHARACTER SEQUENCES
 **/

#include "hw/express-network/em_utils.h"
// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

#include <stdio.h>

static int
hexdigit( int  c )
{
    unsigned  d;

    d = (unsigned)(c - '0');
    if (d < 10) return d;

    d = (unsigned)(c - 'a');
    if (d < 6) return d+10;

    d = (unsigned)(c - 'A');
    if (d < 6) return d+10;

    return -1;
}

int
hex2int( const uint8_t*  hex, int  len )
{
    int  result = 0;
    while (len > 0) {
        int  c = hexdigit(*hex++);
        if (c < 0)
            return -1;

        result = (result << 4) | c;
        len --;
    }
    return result;
}

void
int2hex( uint8_t*  hex, int  len, int  val )
{
    static const uint8_t  hexchars[16] = "0123456789abcdef";
    while ( --len >= 0 )
        *hex++ = hexchars[(val >> (len*4)) & 15];
}

/** USEFUL STRING BUFFER FUNCTIONS
 **/

char*
vbufprint( char*        buffer,
           char*        buffer_end,
           const char*  fmt,
           va_list      args )
{
    int  len = vsnprintf( buffer, buffer_end - buffer, fmt, args );
    if (len < 0 || buffer+len >= buffer_end) {
        if (buffer < buffer_end)
            buffer_end[-1] = 0;
        return buffer_end;
    }
    return buffer + len;
}

char*
bufprint(char*  buffer, char*  end, const char*  fmt, ... )
{
    va_list  args;
    char*    result;

    va_start(args, fmt);
    result = vbufprint(buffer, end, fmt, args);
    va_end(args);
    return  result;
}

ATCommandType get_command_type(const char *cmd, const char *prefix)
{
    int prefix_len = strlen(prefix);
    if (memcmp(cmd, prefix, prefix_len) != 0)
        return AT_INVALID_CMD;

    cmd += prefix_len;
    if (*cmd == '=') {
        ++cmd;
        if (*cmd == '?') {
            return AT_TEST_CMD;
        } else {
            return AT_WRITE_CMD;
        }
    } else if (*cmd == '?') {
        return AT_READ_CMD;
    } else {
        return AT_EXEC_CMD;
    }
}
