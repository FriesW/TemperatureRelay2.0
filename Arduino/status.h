#ifndef _STATUS_H
#define _STATUS_H

#include "metro.h"
#include "timer.h"
#include "pin_map.h"

class _Status {

public:

    _Status()
    {
        pinMode(PIN_BUTTON, INPUT_PULLUP);
        for(byte i = 0; i < sizeof(PIN_LEDS); i++)
        {
            pinMode(PIN_LEDS[i], OUTPUT);
            digitalWrite(PIN_LEDS[i], HIGH);
            s_active[i] = OFF;
        }
        blinker.set(500);
        static_flash.set(5*1000);
    }

    void on(byte idx)
    {
        if( idx < sizeof(PIN_LEDS) )
            s_active[idx] = ON;
    }

    void off(byte idx)
    {
        if( idx < sizeof(PIN_LEDS) )
            s_active[idx] = OFF;
    }

    void blink(byte idx)
    {
        if( idx < sizeof(PIN_LEDS) )
            s_active[idx] = BLINK;
    }

    void all_on()
    {
        for(uint i = 0; i < sizeof(PIN_LEDS); i++) on(i);
    }

    void all_off()
    {
        for(uint i = 0; i < sizeof(PIN_LEDS); i++) off(i);
    }

    void all_blink()
    {
        for(uint i = 0; i < sizeof(PIN_LEDS); i++) blink(i);
    }

    void periodic()
    {
        if( blinker.elapsed() )
        {
            for(uint i = 0; i < sizeof(PIN_LEDS); i++)
                if(s_active[i] == BLINK)
                    digitalWrite(PIN_LEDS[i], blink_state);
            blink_state = !blink_state;
        }
        if( static_flash.expired() )
        {
            for(uint i = 0; i < sizeof(PIN_LEDS); i++)
                if(s_active[i] != BLINK )
                    digitalWrite(PIN_LEDS[i], HIGH);
        }
    }

    void flash()
    {
        for(uint i = 0; i < sizeof(PIN_LEDS); i++)
        {
            if(s_active[i] == ON )
                digitalWrite(PIN_LEDS[i], LOW);
            if(s_active[i] == OFF )
                digitalWrite(PIN_LEDS[i], HIGH);
        }
        static_flash.restart();
    }


private:

    typedef enum {OFF, ON, BLINK} state_type;

    state_type s_active[sizeof(PIN_LEDS)];
    boolean blink_state;
    uint flash_state;
    Metro blinker;
    Timer static_flash;

};


extern _Status Status;

#endif
