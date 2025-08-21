/**
 * @file em_utils.h
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef EM_UTILS_H
#define EM_UTILS_H

#include <stdint.h>
#include <stdarg.h>

/** DECIMAL AND HEXADECIMAL CHARACTER SEQUENCES
 **/

/* decodes a sequence of 'len' hexadecimal chars from 'hex' into
 * an integer. returns -1 in case of error (i.e. badly formed chars)
 */
extern int    hex2int( const uint8_t*  hex, int  len );

/* encodes an integer 'val' into 'len' hexadecimal charaters into 'hex' */
extern void   int2hex( uint8_t*  hex, int  len, int  val );

/** FORMATTED BUFFER PRINTING
 **
 **  bufprint() allows your to easily and safely append formatted string
 **  content to a given bounded character buffer, in a way that is easier
 **  to use than raw snprintf()
 **
 **  'buffer'  is the start position in the buffer,
 **  'buffend' is the end of the buffer, the function assumes (buffer <= buffend)
 **  'format'  is a standard printf-style format string, followed by any number
 **            of formatting arguments
 **
 **  the function returns the next position in the buffer if everything fits
 **  in it. in case of overflow or formatting error, it will always return "buffend"
 **
 **  this allows you to chain several calls to bufprint() and only check for
 **  overflow at the end, for exemple:
 **
 **     char   buffer[1024];
 **     char*  p   = buffer;
 **     char*  end = p + sizeof(buffer);
 **
 **     p = bufprint(p, end, "%s/%s", first, second);
 **     p = bufprint(p, end, "/%s", third);
 **     if (p >= end) ---> overflow
 **
 **  as a convenience, the appended string is zero-terminated if there is no overflow.
 **  (this means that even if p >= end, the content of "buffer" is zero-terminated)
 **
 **  vbufprint() is a variant that accepts a va_list argument
 **/

extern char*   vbufprint(char*  buffer, char*  buffend, const char*  fmt, va_list  args );
extern char*   bufprint (char*  buffer, char*  buffend, const char*  fmt, ... );

typedef enum {
    AT_READ_CMD,
    AT_WRITE_CMD,
    AT_EXEC_CMD,
    AT_TEST_CMD,

    AT_INVALID_CMD
} ATCommandType;

ATCommandType get_command_type(const char *cmd, const char *prefix);

#endif