#ifndef EM_INPUT_H
#define EM_INPUT_H

typedef struct Modem_Data {
    int slot;
    int rssi;
    int ber;
    int quality;
    char from_number[32];
    char sms_str[1024];
    int sms_len;

} Modem_Data;

#endif