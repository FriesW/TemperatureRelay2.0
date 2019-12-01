#include <ESP8266WiFi.h>
#include "status.h"
#include "sensor.h"
#include "pin_map.h"
#include "metro.h"
#include "nonvol.h"
#include "ConfigMode.h"

static Metro m_read;

void setup()
{
Serial.begin(115200);

pinMode(PIN_BUTTON, INPUT_PULLUP);
delay(100);
if( digitalRead(PIN_BUTTON) == LOW || !Nonvol.is_valid() )
{
    Status.all_blink();
    ConfigMode.start();
    while(true)
    {
        ConfigMode.periodic();
        Status.periodic();
        yield();
    }
}

WiFi.mode(WIFI_STA);
WiFi.begin(Nonvol.data.ssid, Nonvol.data.apsk);
Status.blink(0);

wifi_set_sleep_type(LIGHT_SLEEP_T);

m_read.set( 60 * 1000 ); // * 10
}

void loop()
{
    if( WiFi.status() == WL_CONNECTED )
            Status.on(0);

    if( m_read.elapsed() )
        Sensor.read();

    delay(1000);
}
