#ifndef _STOPWATCH_H
#define _STOPWATCH_H

#include <Arduino.h>

class StopWatch {

public:

    void restart()
    {
        start_time = millis();
    }

    unsigned long elapsed()
    {
        return millis() - start_time;
    }

    unsigned long elapsed_s()
    {
        return elapsed() / 1000;
    }

private:
    unsigned long start_time;

};

#endif
