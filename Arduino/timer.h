#ifndef _TIMER_H
#define _TIMER_H

class Timer {

public:

    void set(unsigned long wait_max_ms)
    {
        wait_max = wait_max_ms;
        restart();
    }

    void restart()
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

private:
    boolean ready;
    unsigned long wait_max;
    unsigned long start_time;

};

#endif
