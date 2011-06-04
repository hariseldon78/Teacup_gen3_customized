#ifndef _DEBUG_LED__H_
#define _DEBUG_LED__H_

#include "config.h"

#if defined DEBUG_LED_FEATURE

void debug_led_init();
void debug_led_step();
void debug_led_set_pattern(uint16_t pattern);
extern uint16_t debug_led_pattern;

#else

#define debug_led_init() do {} while(0)
#define debug_led_step() do {} while(0)
#define debug_led_set_pattern( x ) do {} while(0)
#defin debug_led_pattern 0

#endif

#endif
