/**
 * @file em_sim_card.h
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-7-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "hw/express-network/em_sim_card.h"

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static AConfig config_database[1000] = {};
static int initialized = 0;

AConfig* aconfig_get()
{
    if (!initialized) {
        // TODO: preset values
        // aconfig_set(config_database, "")
    }
    return config_database;
}

/* parse a file into a config node tree, return 0 in case of success, -1 otherwise */
int    aconfig_load_file(AConfig*  root, const char*  path)
{
    (void) root, (void) path;
    return 0;
}

/* save a config node tree into a file, return 0 in case of success, -1 otherwise */
int aconfig_save_file(const AConfig* root, const char* path)
{
    (void) root, (void) path;
    return 0;
}

/* locate a named child of a config node */
const AConfig* aconfig_find_const(const AConfig* root, const char* name)
{
    for (int i = 0; root[i].name; ++i) {
        if (!(strcmp(root[i].name, name))) {
            return &root[i];
        }
    }
    return NULL;
}

static AConfig* _aconfig_find_create(AConfig* root, const char* name) {
    int i;

    for(i = 0; root[i].name; ++i) {
        if (!strcmp(root[i].name, name)) {
            return &root[i];
        }
    }

    root[i].name = name;

    return &root[i];
}


/* add a named child to a config node (or modify it if it already exists) */
void aconfig_set(AConfig* root, const char* name, const char* value)
{
    AConfig* node = _aconfig_find_create(root, name);
    node->value = value;
}

int aconfig_int(const AConfig* root, const char* name, int _default)
{
    const AConfig* n = aconfig_find_const(root, name);
    if(n == 0) {
        return _default;
    } else {
        return strtol(n->value, 0, 0);
    }
}

const char* aconfig_str(const AConfig* root,
                               const char* name,
                               const char* _default)
{
    const AConfig* n = aconfig_find_const(root, name);
    if(n == 0) {
        return _default;
    } else {
        return n->value;
    }
}

/* set ENABLE_DYNAMIC_RECORDS to 1 to enable dynamic records
 * for now, this is an experimental feature that needs more testing
 */
#define  ENABLE_DYNAMIC_RECORDS  0

#define  A_SIM_PIN_SIZE  4
#define  A_SIM_PUK_SIZE  8
#define  A_MAX_PHONE_NUMBER_SIZE 15

typedef struct ASimCardRec_ {
    ASimStatus  status;
    char        pin[ A_SIM_PIN_SIZE+1 ];
    char        puk[ A_SIM_PUK_SIZE+1 ];
    char        number[ A_MAX_PHONE_NUMBER_SIZE+1 ];
    int         pin_retries;
    char        curr_fileid_status[256];

    char        out_buff[ 256 ];
    int         out_size;

} ASimCardRec;

static ASimCardRec  _s_card[1];

ASimCard
asimcard_create(int sim_present)
{
    ASimCard  card    = _s_card;
    card->status      = (sim_present ? A_SIM_STATUS_READY : A_SIM_STATUS_ABSENT);
    card->pin_retries = 0;
    strncpy(card->pin, "0000", sizeof(card->pin));
    strncpy(card->puk, "12345678", sizeof(card->puk));
    const char *number = "8618012345678";
    strncpy(card->number, "fffffffffffffff", sizeof(card->number));
    // Use memcpy to avoid copying the trailing '\0' to card->number.
    memcpy(card->number, number, strlen(number));
    return card;
}

void
asimcard_destroy( ASimCard  card )
{
    /* nothing really */
    (void) card;
}

void
asimcard_set_fileid_status( ASimCard sim, const char* str )
{
    snprintf(sim->curr_fileid_status, sizeof(sim->curr_fileid_status), "%s", str);
}

static __inline__ int
asimcard_ready( ASimCard  card )
{
    return card->status == A_SIM_STATUS_READY;
}

ASimStatus
asimcard_get_status( ASimCard  sim )
{
    return sim->status;
}

void
asimcard_set_status( ASimCard  sim, ASimStatus  status )
{
    sim->status = status;
}

const char*
asimcard_get_pin( ASimCard  sim )
{
    return sim->pin;
}

const char*
asimcard_get_puk( ASimCard  sim )
{
    return sim->puk;
}

void
asimcard_set_pin( ASimCard  sim, const char*  pin )
{
    strncpy( sim->pin, pin, A_SIM_PIN_SIZE );
    sim->pin_retries = 0;
}

void
asimcard_set_puk( ASimCard  sim, const char*  puk )
{
    strncpy( sim->puk, puk, A_SIM_PUK_SIZE );
    sim->pin_retries = 0;
}


int
asimcard_check_pin( ASimCard  sim, const char*  pin )
{
    if (sim->status != A_SIM_STATUS_PIN   &&
        sim->status != A_SIM_STATUS_READY )
        return 0;

    if ( !strcmp( sim->pin, pin ) ) {
        sim->status      = A_SIM_STATUS_READY;
        sim->pin_retries = 0;
        return 1;
    }

    if (sim->status != A_SIM_STATUS_READY) {
        if (++sim->pin_retries == 3)
            sim->status = A_SIM_STATUS_PUK;
    }
    return 0;
}


int
asimcard_check_puk( ASimCard  sim, const char* puk, const char*  pin )
{
    if (sim->status != A_SIM_STATUS_PUK)
        return 0;

    if ( !strcmp( sim->puk, puk ) ) {
        strncpy( sim->puk, puk, A_SIM_PUK_SIZE );
        strncpy( sim->pin, pin, A_SIM_PIN_SIZE );
        sim->status      = A_SIM_STATUS_READY;
        sim->pin_retries = 0;
        return 1;
    }

    if ( ++sim->pin_retries == 6 ) {
        sim->status = A_SIM_STATUS_ABSENT;
    }
    return 0;
}

const char *asimcard_get_phone_number( ASimCard sim )
{
    return sim->number;
}

const char *asimcard_get_phone_number_prefix( ASimCard sim )
{
    static char prefix[15];
    if (prefix[0] == 0) {
        int len = strlen(sim->number);
        strncpy(prefix, sim->number, len - 4);
    }
    return prefix;
}

typedef enum {
    SIM_FILE_DM = 0,
    SIM_FILE_DF,
    SIM_FILE_EF_DEDICATED,
    SIM_FILE_EF_LINEAR,
    SIM_FILE_EF_CYCLIC
} SimFileType;

typedef enum {
    SIM_FILE_READ_ONLY       = (1 << 0),
    SIM_FILE_NEED_PIN = (1 << 1),
} SimFileFlags;

/* descriptor for a known SIM File */
#define  SIM_FILE_HEAD       \
    SimFileType     type;    \
    unsigned short  id;      \
    unsigned short  flags;

typedef struct {
    SIM_FILE_HEAD
} SimFileAnyRec, *SimFileAny;

typedef struct {
    SIM_FILE_HEAD
    cbytes_t   data;
    int        length;
} SimFileEFDedicatedRec, *SimFileEFDedicated;

typedef struct {
    SIM_FILE_HEAD
    byte_t     rec_count;
    byte_t     rec_len;
    cbytes_t   records;
} SimFileEFLinearRec, *SimFileEFLinear;

typedef SimFileEFLinearRec   SimFileEFCyclicRec;
typedef SimFileEFCyclicRec*  SimFileEFCyclic;

typedef union {
    SimFileAnyRec          any;
    SimFileEFDedicatedRec  dedicated;
    SimFileEFLinearRec     linear;
    SimFileEFCyclicRec     cyclic;
} SimFileRec, *SimFile;


#if ENABLE_DYNAMIC_RECORDS
/* convert a SIM File descriptor into an ASCII string,
   assumes 'dst' is NULL or properly sized.
   return the number of chars, or -1 on error */
static int
sim_file_to_hex( SimFile  file, bytes_t  dst )
{
    SimFileType  type   = file->any.type;
    int          result = 0;

    /* see 9.2.1 in TS 51.011 */
    switch (type) {
        case SIM_FILE_EF_DEDICATED:
        case SIM_FILE_EF_LINEAR:
        case SIM_FILE_EF_CYCLIC:
            {
                if (dst) {
                    int  file_size, perm;

                    memcpy(dst, "0000", 4);  /* bytes 1-2 are RFU */
                    dst += 4;

                    /* bytes 3-4 are the file size */
                    if (type == SIM_FILE_EF_DEDICATED)
                        file_size = file->dedicated.length;
                    else
                        file_size = file->linear.rec_count * file->linear.rec_len;

                    gsm_hex_from_short( dst, file_size );
                    dst += 4;

                    /* bytes 5-6 are the file id */
                    gsm_hex_from_short( dst, file->any.id );
                    dst += 4;

                    /* byte 7 is the file type - always EF, i.e. 0x04 */
                    dst[0] = '0';
                    dst[1] = '4';
                    dst   += 2;

                    /* byte 8 is RFU, except bit 7 for cyclic files, which indicates
                       that INCREASE is allowed. Since we don't support this yet... */
                    dst[0] = '0';
                    dst[1] = '0';
                    dst   += 2;

                    /* byte 9-11 are access conditions */
                    if (file->any.flags & SIM_FILE_READ_ONLY) {
                        if (file->any.flags & SIM_FILE_NEED_PIN)
                            perm = 0x1a;
                        else
                            perm = 0x0a;
                    } else {
                        if (file->any.flags & SIM_FILE_NEED_PIN)
                            perm = 0x11;
                        else
                            perm = 0x00;
                    }
                    gsm_hex_from_byte(dst, perm);
                    memcpy( dst+2, "a0aa", 4 );
                    dst += 6;

                    /* byte 12 is file status, we don't support invalidation */
                    dst[0] = '0';
                    dst[1] = '0';
                    dst   += 2;

                    /* byte 13 is length of the following data, always 2 */
                    dst[0] = '0';
                    dst[1] = '2';
                    dst   += 2;

                    /* byte 14 is struct of EF */
                    dst[0] = '0';
                    if (type == SIM_FILE_EF_DEDICATED)
                        dst[1] = '0';
                    else if (type == SIM_FILE_EF_LINEAR)
                        dst[1] = '1';
                    else
                        dst[1] = '3';

                    /* byte 15 is lenght of record, or 0 */
                    if (type == SIM_FILE_EF_DEDICATED) {
                        dst[0] = '0';
                        dst[1] = '0';
                    } else
                        gsm_hex_from_byte( dst, file->linear.rec_len );
                }
                result = 30;
            }
            break;

        default:
            result = -1;
    }
    return result;
}


static const byte_t  _const_spn_cphs[20] = {
    0x41, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static const byte_t  _const_voicemail_cphs[1] = {
    0x55
};

static const byte_t  _const_iccid[10] = {
    0x98, 0x10, 0x14, 0x30, 0x12, 0x11, 0x81, 0x15, 0x70, 0x02
};

static const byte_t  _const_cff_cphs[1] = {
    0x55
};

static SimFileEFDedicatedRec  _const_files_dedicated[] =
{
    { SIM_FILE_EF_DEDICATED, 0x6f14, SIM_FILE_READ_ONLY | SIM_FILE_NEED_PIN,
      _const_spn_cphs, sizeof(_const_spn_cphs) },

    { SIM_FILE_EF_DEDICATED, 0x6f11, SIM_FILE_NEED_PIN,
      _const_voicemail_cphs, sizeof(_const_voicemail_cphs) },

    { SIM_FILE_EF_DEDICATED, 0x2fe2, SIM_FILE_READ_ONLY,
      _const_iccid, sizeof(_const_iccid) },

    { SIM_FILE_EF_DEDICATED, 0x6f13, SIM_FILE_NEED_PIN,
      _const_cff_cphs, sizeof(_const_cff_cphs) },

    { 0, 0, 0, NULL, 0 }  /* end of list */
};
#endif /* ENABLE_DYNAMIC_RECORDS */

static char s_buffer[1024];

static int proxy_base64_encode( const char*  src, int  srclen,
                     char*        dst, int  dstlen )
{
    static const char cb64[64]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char*       srcend = src + srclen;
    int               result = 0;

    while (src+3 <= srcend && result+4 <= dstlen)
    {
        dst[result+0] = cb64[ src[0] >> 2 ];
        dst[result+1] = cb64[ ((src[0] & 3) << 4) | ((src[1] & 0xf0) >> 4) ];
        dst[result+2] = cb64[ ((src[1] & 0xf) << 2) | ((src[2] & 0xc0) >> 6) ];
        dst[result+3] = cb64[ src[2] & 0x3f ];
        src    += 3;
        result += 4;
    }

    if (src < srcend) {
        unsigned char  in[4];

        if (result+4 > dstlen)
            return -1;

        in[0] = src[0];
        in[1] = src+1 < srcend ? src[1] : 0;
        in[2] = src+2 < srcend ? src[2] : 0;

        dst[result+0] = cb64[ in[0] >> 2 ];
        dst[result+1] = cb64[ ((in[0] & 3) << 4) | ((in[1] & 0xf0) >> 4) ];
        dst[result+2] = (unsigned char) (src+1 < srcend ? cb64[ ((in[1] & 0xf) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
        dst[result+3] = (unsigned char) (src+2 < srcend ? cb64[ in[2] & 0x3f ] : '=');
        result += 4;
    }
    return result;
}

static void make_SRES_Kc(char* data, int len, char* sres, char* kc) {
    // here we just fill in the sres and kc with original data
    // real USIM card will create appropriate SRES and Kc here
    // 3GPP TS 31.102 7.1.2
    int i = 0;
    int j = 0;
    for (i = 0; i < 4; ++i, ++j) {
        sres[i] = (data[j % len]);
    }

    for (i = 0; i < 8; ++i, ++j) {
        kc[i] = (data[j % len]);
    }
}

const char*
asimcard_csim( ASimCard sim, const char*  cmd )
{
    /* b/37719621
       for now, we only handle the authentication in GSM context;
       the response to authentication request is just fake data:
       we dont fully simulate a real modem at the moment.

       example input:
       AT+CSIM=29,"0088008011abcdefghijklmnopq00"
        29: length of quoted input(exclude quotes)
        00: APDU instruction class = 00 here
        88: instruction code = 88 (authenticate)
        00: first parameter of authenticate, 00 here
        80: second parameter of authenticate (80 means GSM context)
        11: authentication challenge string length (here it is 17 bytes)
        abcdefghijklmnopq: 17 bytes authentication challenge string, base64 encoded
        00: means getting all the response; we dont care about this for now, and just
            send back all the response to the authentication challenge, since it is
            very short (14 bytes before base64 encoding).

        the response is as follows
        +CSIM=<length-of-reply-without-quotes>,
        "<base64-response><status1-in-hexstring><status2-in-hexstring>"

       some references:
       3GPP TS 31.102 7.1.2
       http://www.tml.tkk.fi/Studies/T-110.497/2003/lecture4.pdf
       http://wiki.openmoko.org/wiki/Hardware:AT_Commands
       http://m2msupport.net/m2msupport/atcsim-generic-sim-access/
     */

    char * first_double_quote_ptr = strchr(cmd, '"');

    // sanity check: the authentication challenge should be at least 1 byte long
    // that makes the total length of quoted string 17
    if (!first_double_quote_ptr || strlen(first_double_quote_ptr) < 17
            || strncmp("00880080", first_double_quote_ptr + 1, 8)) {
        // instruction code not supported or invalid
        snprintf(s_buffer, sizeof(s_buffer), "+CSIM:4, \"6D06\"");
        return s_buffer;
    }

    char * length_ptr = first_double_quote_ptr + 1 /* 1 char for " */
        + 8 /* 8 char for the 00880080 */
        ;

    int data_len = 0;
    sscanf(length_ptr, "%02x", &data_len);

    char* data_ptr = length_ptr + 2;
    char response[14];
    response[0] = 0x4; //SRES is 4 bytes
    response[5] = 0x8; //Kc is 8 bytes
    char * SRES = response + 1;
    char * Kc = response + 6;
    make_SRES_Kc(data_ptr, data_len, SRES, Kc);
    char base64_response[1024];
    unsigned resp_len = proxy_base64_encode(response, sizeof(response), base64_response,
            sizeof(base64_response));
    if (resp_len >= sizeof(base64_response)) {
        snprintf(s_buffer, sizeof(s_buffer), "+CSIM:4, \"9866\""); // no more memory
        return s_buffer;
    }
    base64_response[resp_len] = '\0';
    snprintf(s_buffer, sizeof(s_buffer), "+CSIM:%d, \"%s0900\"", resp_len + 4, base64_response);
    return s_buffer;
}

const char*
asimcard_io( ASimCard  sim, const char*  cmd )
{
    int  nn;
#if ENABLE_DYNAMIC_RECORDS
    int  command, id, p1, p2, p3;
#endif
    static const struct { const char*  cmd; const char*  answer; } answers[] =
    {
        { "+CRSM=192,28436,0,0,15", "+CRSM: 144,0,000000146f1404001aa0aa01020000" },
        { "+CRSM=176,28436,0,0,20", "+CRSM: 144,0,416e64726f6964ffffffffffffffffffffffffff" },

        { "+CRSM=192,28433,0,0,15", "+CRSM: 144,0,000000016f11040011a0aa01020000" },
        { "+CRSM=176,28433,0,0,1", "+CRSM: 144,0,55" },

        { "+CRSM=192,12258,0,0,15", "+CRSM: 144,0,0000000a2fe204000fa0aa01020000" },
        { "+CRSM=176,12258,0,0,10", "+CRSM: 144,0,98101430121181157002" },

        { "+CRSM=192,28435,0,0,15", "+CRSM: 144,0,000000016f13040011a0aa01020000" },
        { "+CRSM=176,28435,0,0,1",  "+CRSM: 144,0,55" },

        { "+CRSM=192,28472,0,0,15", "+CRSM: 144,0,0000000f6f3804001aa0aa01020000" },
        { "+CRSM=176,28472,0,0,15", "+CRSM: 144,0,ff30ffff3c003c03000c0000f03f00" },

        { "+CRSM=192,28617,0,0,15", "+CRSM: 144,0,000000086fc9040011a0aa01020104" },
        { "+CRSM=178,28617,1,4,4",  "+CRSM: 144,0,01000000" },

        { "+CRSM=192,28618,0,0,15", "+CRSM: 144,0,0000000a6fca040011a0aa01020105" },
        { "+CRSM=178,28618,1,4,5",  "+CRSM: 144,0,0000000000" },

        { "+CRSM=192,28589,0,0,15", "+CRSM: 144,0,000000046fad04000aa0aa01020000" },
        { "+CRSM=176,28589,0,0,4",  "+CRSM: 144,0,00000003" },

        { "+CRSM=192,28438,0,0,15", "+CRSM: 144,0,000000026f1604001aa0aa01020000" },
        { "+CRSM=176,28438,0,0,2",  "+CRSM: 144,0,0233" },

        { "+CRSM=192,28486,0,0,15", "+CRSM: 148,4" },
        { "+CRSM=192,28621,0,0,15", "+CRSM: 148,4" },

        { "+CRSM=192,28613,0,0,15", "+CRSM: 144,0,000000f06fc504000aa0aa01020118" },
        { "+CRSM=178,28613,1,4,24", "+CRSM: 144,0,43058441aa890affffffffffffffffffffffffffffffffff" },

        { "+CRSM=192,28480,0,0,15", "+CRSM: 144,0,000000806f40040011a0aa01020120" },
        { "+CRSM=178,28480,1,4,32", "+CRSM: 144,0,ffffffffffffffffffffffffffffffffffff07815155258131f5ffffffffffff" },

        { "+CRSM=192,28615,0,0,15", "+CRSM: 144,0,000000406fc7040011a0aa01020120" },
        { "+CRSM=178,28615,1,4,32", "+CRSM: 144,0,566f6963656d61696cffffffffffffffffff07915155125740f9ffffffffffff" },

        /* b/37718561
           192, 28539 is for querying forbidden PLMN; the response is fake data.
           176, 28539 is also for forbidden PLMN, but in binary format
           the 15's f is simply copied from my own phone; the value does not seem
           to matter that much; but the length has to be a multiple of 5's
         */
        { "+CRSM=192,28539,0,0,15", "+CRSM: 144,0,000000406fc7040011a0aa01000000" },
        { "+CRSM=176,28539,0,0,64", "+CRSM: 144,0,ffffffffffffffffffffffff" },

        { NULL, NULL }
    };

    assert( memcmp( cmd, "+CRSM=", 6 ) == 0 );

#if ENABLE_DYNAMIC_RECORDS
    if ( sscanf(cmd, "+CRSM=%d,%d,%d,%d,%d", &command, &id, &p1, &p2, &p3) == 5 ) {
        switch (command) {
            case A_SIM_CMD_GET_RESPONSE:
                {
                    const SimFileEFDedicatedRec*  file = _const_files_dedicated;

                    assert(p1 == 0 && p2 == 0 && p3 == 15);

                    for ( ; file->id != 0; file++ ) {
                        if (file->id == id) {
                            int    count;
                            char*  out = sim->out_buff;
                            strcpy( out, "+CRSM: 144,0," );
                            out  += strlen(out);
                            count = sim_file_to_hex( (SimFile) file, out );
                            if (count < 0)
                                return "ERROR: INTERNAL SIM ERROR";
                            out[count] = 0;
                            return sim->out_buff;
                        }
                    }
                    break;
                }

            case A_SIM_CMD_READ_BINARY:
                {
                    const SimFileEFDedicatedRec*  file = _const_files_dedicated;

                    assert(p1 == 0 && p2 == 0);

                    for ( ; file->id != 0; file++ ) {
                        if (file->id == id) {
                            char*  out = sim->out_buff;

                            if (p3 > file->length)
                                return "ERROR: BINARY LENGTH IS TOO LONG";

                            strcpy( out, "+CRSM: 144,0," );
                            out  += strlen(out);
                            gsm_hex_from_bytes( out, file->data, p3 );
                            out[p3*2] = 0;
                            return sim->out_buff;
                        }
                    }
                    break;
                }

            case A_SIM_CMD_READ_RECORD:
                break;

            default:
                return "ERROR: UNSUPPORTED SIM COMMAND";
        }
    }
#endif

    if (!strcmp("+CRSM=178,28480,1,4,32", cmd)) {
        const char* phone_number = sim->number;
        assert(strlen(phone_number) == 15);
        snprintf( sim->out_buff, sizeof(sim->out_buff), "+CRSM: 144,0,"
                  "ffffffffffffffffffffffffffffffffffff"
                  "09"  // length field (it denotes number of octets following this length field)A
                        // Note: each number fits in a nibble, we need 8 octets
                        // for (max) 15 digit number + 1 octet for type of
                        // number and telephone numbering plan info.
                  "9" // type of number(9 = 1001 =>
                      // (first bit always one) 001 == international number and
                      // plan identification (don't worry about it).
                  "1" // == telephone numbering plan.
                  // Phone number begins here.
                  "%c%c%c%c" "%c%c%c%c" "%c%c%c%c" "%c%cf%c" "ffffffff",
                  // Reverse BCD notation in action.
                  phone_number[1],  phone_number[0],
                  phone_number[3],  phone_number[2],
                  phone_number[5],  phone_number[4],
                  phone_number[7],  phone_number[6],
                  phone_number[9],  phone_number[8],
                  phone_number[11], phone_number[10],
                  phone_number[13], phone_number[12],
                                    phone_number[14]);
        return sim->out_buff;
        }

    if (!strcmp("+CRSM=242,0,0,0,0", cmd)) {
        snprintf( sim->out_buff, sizeof(sim->out_buff), "+CRSM: 144,0,%s", sim->curr_fileid_status);
        return sim->out_buff;
    }

    for (nn = 0; answers[nn].cmd != NULL; nn++) {
        /* Only match prefix */
        // int len = strlen(answers[nn].cmd);
        int len = 15; // Only match <command> and <fileid>
        if ( !memcmp( answers[nn].cmd, cmd, len ) ) {
            return answers[nn].answer;
        }
    }
    return "ERROR: BAD COMMAND";
}
