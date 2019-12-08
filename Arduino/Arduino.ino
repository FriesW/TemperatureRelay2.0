#include <ESP8266WiFi.h>
#include "pin_map.h"
#include "nonvol.h"
#include "ConfigMode.h"
#include "normal_mode.h"

static void config_mode();

void setup()
{
Serial.begin(115200);

pinMode(PIN_BUTTON, INPUT_PULLUP);
delay(100);
if( digitalRead(PIN_BUTTON) == LOW || !Nonvol.is_valid() )
{
    config_mode();
}
pinMode(PIN_BUTTON, INPUT);

delay(10 * 1000);

normal_mode();

}

void loop(){}

#include "status.h"
static void config_mode()
{
    Status.all_on();
    ConfigMode.start();
    while(true)
    {
        ConfigMode.periodic();
        Status.periodic();
        yield();
    }
}
