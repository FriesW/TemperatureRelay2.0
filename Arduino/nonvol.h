#ifndef _NONVOL_H
#define _NONVOL_H

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define SSID_LEN 32
#define APSK_LEN 32
#define HOST_LEN 50

typedef struct
{
    char ssid[SSID_LEN];
    char apsk[APSK_LEN];
    unsigned int port;
    boolean use_host;
    char host[HOST_LEN];
    IPAddress address;
} nonvol_data;

class _Nonvol {

public:

    _Nonvol()
    {
        EEPROM.get(0, data);
    }

    void save()
    {
        EEPROM.put(0, data);
    }

    nonvol_data data;

};

_Nonvol Nonvol;

#endif
