#include <Arduino.h>
#include "button.h"
#include "timer.h"
#include "pin_map.h"

static void button_isr();

_Button::_Button()
{
    p_state = NO_CHANGE_OFF;
    debounce.set(0);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), button_isr, RISING);
}

button_state _Button::state()
{
    button_state out;
    noInterrupts();
    out = p_state;
    switch(p_state)
    {
        case CHANGE_ON:
            p_state = NO_CHANGE_ON; break;
        case CHANGE_OFF:
            p_state = NO_CHANGE_OFF; break;
    }
    interrupts();
    return out;
}

ICACHE_RAM_ATTR void _Button::_isr()
{
noInterrupts();
if(debounce.fast_expired())
    {
        debounce.set(100);
        switch(p_state)
        {
            case NO_CHANGE_OFF:
                p_state = CHANGE_ON; break;
            case NO_CHANGE_ON:
                p_state = CHANGE_OFF; break;
            case CHANGE_ON:
                p_state = NO_CHANGE_OFF; break;
            case CHANGE_OFF:
                p_state = NO_CHANGE_ON; break;
        }
    }
interrupts();
}

_Button Button;

ICACHE_RAM_ATTR static void button_isr()
{
Button._isr();
}
