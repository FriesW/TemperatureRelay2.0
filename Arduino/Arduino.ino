#include <ESP8266WiFi.h>
#include "status.h"
#include "net_manager.h"

void setup()
{
Serial.begin(115200);
}

void loop()
{
    Status.periodic();
    NetManager.periodic();
}
