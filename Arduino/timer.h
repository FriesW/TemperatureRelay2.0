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
        return ready;
    }

    boolean expired()
    {
        return !running();
    }

    ICACHE_RAM_ATTR boolean fast_running()
    {
        if(! ready )
            return ready;
        ready = millis() - start_time < wait_max;
        return ready;
    }

    ICACHE_RAM_ATTR boolean fast_expired()
    {
        return !fast_running();
    }

private:
    boolean ready;
    unsigned long wait_max;
    unsigned long start_time;

};

#endif
