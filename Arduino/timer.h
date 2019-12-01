#ifndef _TIMER_H
#define _TIMER_H

#include <Arduino.h>

class Timer {

public:

    ICACHE_RAM_ATTR void set(unsigned long wait_max_ms)
    {
        wait_max = wait_max_ms;
        restart();
    }

    ICACHE_RAM_ATTR void restart()
    {
        ready = true;
        start_time = millis();
    }

    boolean running()
    {
        if(! ready )
            return ready;
        yield();
        ready = millis() - start_time < wait_max;
        if(ready)
            delay(100);
        return ready;
    }

    boolean expired()
    {
        return !running();
    }

private:
    boolean ready;
    unsigned long wait_max;
    unsigned long start_time;

};

#endif
