#ifndef _NET_MANAGER_H
#define _NET_MANAGER_H

#include <ESP8266WiFi.h>
#include "ConfigMode.h"
#include "button.h"
#include "status.h"
#include "nonvol.h"

class _NetManager {

public:

    _NetManager()
    {
        sta_mode();
    }

    void periodic()
    {
        button_state state = Button.state();

        switch(state)
        {
            case CHANGE_ON:
                Serial.println("Wifi mode switch AP");
                reset_net();
                ConfigMode.start();
                break;

            case CHANGE_OFF:
                Serial.println("Wifi mode switch STA");
                ConfigMode.stop();
                reset_net();
                sta_mode();
                break;

            case NO_CHANGE_ON:
                ConfigMode.periodic();
                break;
        }

        if( WiFi.status() == WL_CONNECTED )
            Status.on(0);
    }


private:

    void reset_net()
    {
        Status.off(0);
        Status.off(5);
        WiFi.softAPdisconnect();
        WiFi.disconnect();
    }

    void sta_mode()
    {
        WiFi.mode(WIFI_STA);
        if( is_str(Nonvol.data.ssid, SSID_LEN) && is_str(Nonvol.data.apsk, APSK_LEN) )
            WiFi.begin(Nonvol.data.ssid, Nonvol.data.apsk);
        Status.blink(0);
    }

};

extern _NetManager NetManager;

#endif
