#ifndef _CONFIGMODE_H
#define _CONFIGMODE_H

#include <Arduino.h>

class _ConfigMode {

public:
    _ConfigMode();

    void start();

    void stop();

    void periodic();

private:
    boolean running;

};

extern _ConfigMode ConfigMode;

#endif
