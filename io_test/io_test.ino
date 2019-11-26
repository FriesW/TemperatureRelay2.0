// Sketch -> Include Library -> Manage libraries
// Search for "OneWire" by Paul Stoffregen et al.
//   https://github.com/PaulStoffregen/OneWire
// Search for "Dallas" by Miles Burton et al.
//   https://github.com/milesburton/Arduino-Temperature-Control-Library

#include <OneWire.h>
#include <DallasTemperature.h>


const byte button = 4;
const byte bus = 5;
const byte leds[] = {16, 0, 2, 14, 12, 13};

OneWire one_wire(bus);
DallasTemperature sensor(&one_wire);

DeviceAddress addr;

void setup()
{
    Serial.begin(115200);
    pinMode(button, INPUT_PULLUP);
    for(byte i = 0; i < sizeof(leds); i++)
        pinMode(leds[i], OUTPUT);

    boolean res = sensor.getAddress(addr, 0);
    if( !res )
        Serial.println("error getting sensor addr.");
    else
        Serial.println("Sensor addr found.");

    attachInterrupt(digitalPinToInterrupt(button), button_isr, CHANGE);
}

byte current = 0;
void loop()
{

    set_all(HIGH); //clear
    if( current < sizeof(leds))
        {
        digitalWrite( leds[current], LOW);
        current++;
        }
    else
        {
        set_all(LOW); //all on
        current = 0;
        }

    sensor.requestTemperatures();

    Serial.println(digitalRead(button));


    boolean res = sensor.setResolution(addr, 12, true);
    if( !res )
        Serial.println("error setting resolution");
    int16_t val = sensor.getTemp(addr);
    if( val == DEVICE_DISCONNECTED_RAW )
        Serial.println("sensor error");
    else
        Serial.println(val); // val/128 == Celsius

    delay(100);
}

void set_all(boolean state)
{
    for(byte i = 0; i < sizeof(leds); i++)
        digitalWrite(leds[i], state);
}

ICACHE_RAM_ATTR void button_isr()
{
    //static unsigned long last_time = 0;
    //static boolean last_state = false;
    Serial.println("isr");
}
