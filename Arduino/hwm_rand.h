#ifndef _HWM_RAND_H
#define _HWM_RAND_H

#include <Arduino.h>

inline volatile uint32_t hwm_rand() {
    return *((volatile uint32_t*)0x3FF20E44);
}

#endif
