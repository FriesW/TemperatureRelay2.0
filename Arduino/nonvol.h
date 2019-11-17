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
    char host[HOST_LEN];
    unsigned int port;
} nonvol_data;

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
        is_ascii_str(data.ssid, SSID_LEN) &&
        is_ascii_str(data.apsk, APSK_LEN) &&
        is_ascii_str(data.host, HOST_LEN) &&
        data.port >= 0 && data.port <= 65535;
    }


    nonvol_data data;

private:
    boolean is_ascii_str(char * str_ptr, unsigned int str_len)
    {
        for(unsigned int i = 0; i < str_len; i++)
        {
            char c = str_ptr[i];
            if( c < 32 || c > 126 ) return false;
            if( c == 0 ) return true;
        }
        return false;
    }

};

extern _Nonvol Nonvol;

#endif
