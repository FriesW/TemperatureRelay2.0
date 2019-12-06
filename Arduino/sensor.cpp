// Sketch -> Include Library -> Manage libraries
// Search for "OneWire" by Paul Stoffregen et al.
//   https://github.com/PaulStoffregen/OneWire
// Search for "Dallas" by Miles Burton et al.
//   https://github.com/milesburton/Arduino-Temperature-Control-Library

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "pin_map.h"
#include "sensor.h"
#include "status.h"

static OneWire one_wire(PIN_D_BUS);
static DallasTemperature d_temp(&one_wire);
static DeviceAddress addr;


_Sensor::_Sensor()
{
    ready = false;
    read();
}

int16_t _Sensor::read()
{
    Status.off(4);
    if(!init()) return SENSOR_ERROR;

    d_temp.setWaitForConversion(false);
    d_temp.requestTemperatures();
    d_temp.setWaitForConversion(true);
    delay(1000);
    int16_t val = d_temp.getTemp(addr);
    if( val == DEVICE_DISCONNECTED_RAW )
    {
        Serial.println("Sensor read error.");
        ready = false;
        return SENSOR_ERROR;
    }

    Serial.printf("Sensor read: %d\n", val);

    Status.on(4);
    return val;
}

boolean _Sensor::init()
{
    if( ready ) return true;

    boolean res = d_temp.getAddress(addr, 0);
    if( !res )
    {
        Serial.println("Sensor address error.");
        return false;
    }
    res = d_temp.setResolution(addr, 12, true);
    if( !res )
    {
        Serial.println("Sensor resolution error.");
        return false;
    }

    ready = true;
    return true;
}



_Sensor Sensor;
