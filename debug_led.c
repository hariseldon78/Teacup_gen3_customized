#include "debug_led.h"
#if defined DEBUG_LED_FEATURE

uint16_t debug_led_pattern;
uint8_t remaining_ticks;
uint8_t auto_off;
uint16_t last_step;

void debug_led_init()
{
        debug_led_pattern = 0;
        last_step=0;
        auto_off=0;
        remaining_ticks=0;
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
        if (auto_off) {
                remaining_ticks--;
                if (!remaining_ticks) {
                        auto_off=0;
                        debug_led_pattern=0;
                }
        }
}

void debug_led_set_pattern(uint16_t pattern, uint8_t ticks)
{
        debug_led_pattern=pattern;
        remaining_ticks=ticks;
        auto_off=remaining_ticks>0;
}

#endif
