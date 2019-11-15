#ifndef _TIMEOUT_H
#define _TIMEOUT_H

class Timeout {

public:

    void init(unsigned long wait_max_ms)
    {
        ready = true;
        wait_max = wait_max_ms;
        start_time = millis();
    }

    boolean valid()
    {
        if(! ready )
            return ready;
        yield();
        ready = millis() - start_time < wait_max;
        return ready;
    }

private:
    boolean ready;
    unsigned long wait_max;
    unsigned long start_time;

};

#endif
