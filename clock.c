#include	"clock.h"

/** \file
 * 	\brief Do stuff periodically
 */

#include	"pinio.h"
#include	"sersendf.h"
#include	"dda_queue.h"
#include	"watchdog.h"
#include	"temp.h"
#include	"timer.h"
#include	"debug.h"
#include	"heater.h"
#include	"serial.h"
#include        "dda_queue.h"
#ifdef	TEMP_INTERCOM
#include	"intercom.h"
#endif
#include	"memory_barrier.h"
#include        "debug_led.h"

/*!	do stuff every 1/4 second
 
 	called from clock_10ms(), do not call directly
 */
 
uint32_t seconds_counter=0;
#ifdef REST_TIME
uint32_t working_seconds=0;
uint32_t idle_seconds=0;

void reset_idle()
{
	idle_seconds=0;
}
#endif

uint32_t seconds_from_boot()
{
        return seconds_counter;
}

void clock_250ms() {
        debug_led_step();

        if (steptimeout > (30 * 4)) {
                power_off();
        }
        else if (heaters_all_off())	{
                uint8_t save_reg = SREG;
                cli();
                CLI_SEI_BUG_MEMORY_BARRIER();
                steptimeout++;
                MEMORY_BARRIER();
                SREG = save_reg;
        }
        
#ifdef	TEMP_INTERCOM
        start_send();
#endif

        ifclock(clock_flag_1s) {
                if (DEBUG_POSITION && (debug_flags & DEBUG_CLOCK)) sersendf_P(PSTR("1s"));
                if (DEBUG_POSITION && (debug_flags & DEBUG_POSITION)) {
                        // current position
                        sersendf_P(PSTR("Pos: %ld,%ld,%ld,%ld,%lu\n"), current_position.X, current_position.Y, current_position.Z, current_position.E, current_position.F);

                        // target position
                        sersendf_P(PSTR("Dst: %ld,%ld,%ld,%ld,%lu\n"), movebuffer[mb_tail].endpoint.X, movebuffer[mb_tail].endpoint.Y, movebuffer[mb_tail].endpoint.Z, movebuffer[mb_tail].endpoint.E, movebuffer[mb_tail].endpoint.F);

                        // Queue
                        print_queue();

                        // newline
                        serial_writechar('\n');
                }

                check_temp_achieved();
                if (DEBUG_POSITION && (debug_flags & DEBUG_CLOCK)) sersendf_P(PSTR("t"));
                
                
                ++seconds_counter;
                
                #if defined REST_TIME
                
                if (++idle_seconds>60)
                	working_seconds=0;
               	else
	                ++working_seconds;
                #endif

                if (DEBUG_POSITION && (debug_flags & DEBUG_CLOCK)) sersendf_P(PSTR("r\n"));
        }
        
}

/*! do stuff every 10 milliseconds
 
 	call from ifclock(CLOCK_FLAG_10MS) in busy loops
 */
void clock_10ms() {
        // reset watchdog
        wd_reset();

        temp_tick();

#ifdef MOTOR_OVER_INTERCOM
        send_motor_if_new();
#endif
        ifclock(clock_flag_250ms) {
                clock_250ms();
        }

        update_position();

}




