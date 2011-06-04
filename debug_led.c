#include "debug_led.h"
#if defined DEBUG_LED_FEATURE

uint16_t debug_led_pattern;
uint16_t last_step;

void debug_led_init()
{
        debug_led_pattern = 0;
        last_step=0;
}

void debug_led_step()
{
        uint16_t actual_step=debug_led_pattern >> 15;
        if (actual_step!=last_step) {
                WRITE(DEBUG_LED,actual_step);
                last_step=actual_step;
        }
        // cycle the pattern
        debug_led_pattern= (debug_led_pattern<<1) | actual_step ;
}

void debug_led_set_pattern(uint16_t pattern)
{
        debug_led_pattern=pattern;
}

#endif
