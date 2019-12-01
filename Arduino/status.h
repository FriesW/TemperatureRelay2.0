#ifndef _STATUS_H
#define _STATUS_H

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
            t_active[i] = false;
        }
        blinker.set(500);
    }

    void on(byte idx)
    {
        if( idx < sizeof(PIN_LEDS) )
        {
            digitalWrite(PIN_LEDS[idx], LOW);
            t_active[idx] = false;
        }
    }

    void off(byte idx)
    {
        if( idx < sizeof(PIN_LEDS) )
        {
            digitalWrite(PIN_LEDS[idx], HIGH);
            t_active[idx] = false;
        }
    }

    void blink(byte idx)
    {
        if( idx < sizeof(PIN_LEDS) )
            t_active[idx] = true;
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
        if( blinker.expired() )
        {
            for(uint i = 0; i < sizeof(PIN_LEDS); i++)
                if(t_active[i])
                    digitalWrite(PIN_LEDS[i], state);
            state = !state;
            blinker.restart();
        }
    }


private:

    boolean t_active[sizeof(PIN_LEDS)];
    boolean state;
    Timer blinker;

};


extern _Status Status;

#endif
