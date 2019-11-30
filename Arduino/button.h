#ifndef _BUTTON_H
#define _BUTTON_H

#include "timer.h"

typedef enum
{
    NO_CHANGE_OFF,
    NO_CHANGE_ON,
    CHANGE_OFF,
    CHANGE_ON
} button_state;

class _Button {

public:
    _Button();
    button_state state();

    void _isr();

private:
    Timer debounce;
    button_state p_state;

};


extern _Button Button;

#endif
