#ifndef _METRO_H
#define _METRO_H

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

    uint16_t elapsed()
    {
        update();
        uint16_t o = cycles;
        cycles = 0;
        return o;
    }

private:
    unsigned long p;
    unsigned long last;
    uint16_t cycles;



};

#endif
