#ifndef _NONVOL_H
#define _NONVOL_H

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define SSID_LEN 32
#define APSK_LEN 32
#define HOST_LEN 50
#define HKEY_LEN 50

typedef struct
{
    char ssid[SSID_LEN];
    char apsk[APSK_LEN];
    char host[HOST_LEN];
    uint16_t port;
    char hmac_key[HKEY_LEN];
} nonvol_data;

boolean is_str(char * str_ptr, unsigned int str_len);


class _Nonvol {

public:

    _Nonvol()
    {
        EEPROM.begin(sizeof(data));
        EEPROM.get(0, data);
    }

    void save()
    {
        EEPROM.put(0, data);
        EEPROM.commit();
    }

    boolean is_valid()
    {
        return
        is_str(data.ssid, SSID_LEN) &&
        is_str(data.apsk, APSK_LEN) &&
        is_str(data.host, HOST_LEN) &&
        is_str(data.hmac_key, HKEY_LEN) &&
        data.port >= 1 && data.port <= 65535;
    }


    nonvol_data data;

};

extern _Nonvol Nonvol;

#endif
