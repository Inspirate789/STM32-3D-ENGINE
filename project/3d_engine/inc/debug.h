#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "mbed.h"

#define BLINKING_RATE_MS 1000

inline void dbg_signal(void)
{
    static DigitalOut led(LED1);
    wait_ms(BLINKING_RATE_MS);
    led = !led;
    wait_ms(BLINKING_RATE_MS);
    led = !led;
}

#endif