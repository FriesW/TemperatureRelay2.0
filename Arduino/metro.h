#ifndef _METRO_H
#define _METRO_H

#include <Arduino.h>

class Metro
{

public:

    void set(unsigned long period)
    {
        p = period;
        reset();
    }

    void reset()
    {
        last = millis();
        cycles = 0;
    }

    void update()
    {
        unsigned long now = millis();
        unsigned long delta = now - last;
        cycles += delta / p;
        last = now - (delta % p);
    }

    unsigned long next()
    {
        unsigned long now = millis();
        unsigned long delta = now - last;
        return p - ( delta % p );
    }

    uint16_t elapsed()
    {
        update();
        uint16_t o = cycles;
        cycles = 0;
        return o;
    }

    uint16_t peek_elapsed()
    {
        update();
        return cycles;
    }

private:
    unsigned long p;
    unsigned long last;
    uint16_t cycles;



};

#endif
