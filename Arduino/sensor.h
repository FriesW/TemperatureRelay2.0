#ifndef _SENSOR_H
#define _SENSOR_H

#include <Arduino.h>

#define SENSOR_ERROR (0xFFFF)

class _Sensor {

public:
    _Sensor();
    int16_t read();
private:
    boolean init();
    boolean ready;

};

extern _Sensor Sensor;

#endif
