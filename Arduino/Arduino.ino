#include <ESP8266WiFi.h>
#include "status.h"
#include "net_manager.h"
#include "sensor.h"

#include "timer.h"

Timer reading;

void setup()
{
Serial.begin(115200);
reading.set(1000);
}

void loop()
{
    Status.periodic();
    NetManager.periodic();
    if(reading.expired())
    {
        Sensor.read();
        reading.set(1000);
    }
}
