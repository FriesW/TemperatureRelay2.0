#ifndef _SYSTEM_TIME_H
#define _SYSTEM_TIME_H

#include <Arduino.h>

class _SystemTime {

public:

void periodic()
{
    unsigned long now = millis();
    unsigned long delta = now - last_millis;
    last_millis = now;
    
    last_ms += delta;
    last_s += last_ms / 1000;
    last_ms = last_ms % 1000;
}

unsigned long now_s()
{
    periodic();
    return last_s;
}

unsigned int now_ms()
{
    periodic();
    return last_ms;
}

private:

    unsigned long last_s = 0;
    unsigned long last_ms = 0;
    unsigned long last_millis = 0;


};

extern _SystemTime SystemTime;

#endif
