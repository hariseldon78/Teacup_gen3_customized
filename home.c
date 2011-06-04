#include	"home.h"

/** \file
 * 	\brief Homing routines
 */

#include	"dda.h"
#include	"dda_queue.h"
#include	"delay.h"
#include	"pinio.h"
#include	"sersendf.h"
#include        "timer.h"
#include        "clock.h"

/// home all 3 axes
void home_all() {
#if defined	X_MIN_PIN
        home_x_negative();
#elif defined X_MAX_PIN
        home_x_positive();
#endif

#if defined	Y_MIN_PIN
        home_y_negative();
#elif defined Y_MAX_PIN
        home_y_positive();
#endif

#if defined	Z_MIN_PIN
        home_z_negative();
#elif defined Z_MAX_PIN
        home_z_positive();
#endif
}

/// find X MIN endstop
void home_x_negative() {
        queue_wait();

#if defined X_MIN_PIN
        uint8_t	denoise_count = 0;

        // home X
        x_enable();
        // hit home hard
#ifdef DEBUG  
        sersendf_P(PSTR("MAXIMUM_FEEDRATE_X=%lu, SEARCH_FEEDRATE_X=%lu\n"),(uint32_t)MAXIMUM_FEEDRATE_X,(uint32_t)SEARCH_FEEDRATE_X);
        sersendf_P(PSTR("1; x_min=%u\n"),(uint8_t)x_min());
#endif
        x_direction(0);
        while (denoise_count < 8) {
                // denoise
                if (x_min())
                        denoise_count++;
                else
                        denoise_count = 0;
                // step
                x_step();
                delay(5);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_X / ((float) MAXIMUM_FEEDRATE_X)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }
        denoise_count = 0;

        // back off slowly
#ifdef DEBUG  
        sersendf_P(PSTR("2; x_min=%u\n"),(uint8_t)x_min());
#endif
        x_direction(1);
        while (x_min() != 0) {
                // step
                x_step();
                delay(5);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_X / ((float) SEARCH_FEEDRATE_X)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }
#ifdef DEBUG  
        sersendf_P(PSTR("3; x_min=%u\n"),(uint8_t)x_min());
#endif
        // set X home
        startpoint.X = current_position.X = 0;
#endif
}

/// find X_MAX endstop
void home_x_positive() {
        queue_wait();

#if defined X_MAX_PIN
        uint8_t	denoise_count = 0;

        // home X
        x_enable();
        // hit home hard
        x_direction(1);
        while (denoise_count < 8) {
                // denoise
                if (x_max())
                        denoise_count++;
                else
                        denoise_count = 0;
                // step
                x_step();
                delay(5);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_X / ((float) MAXIMUM_FEEDRATE_X)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }
        denoise_count = 0;

        // back off slowly
        x_direction(0);
        while (x_max() != 0) {
                // step
                x_step();
                delay(5);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_X / ((float) SEARCH_FEEDRATE_X)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }

        // set X home
        TARGET t = {
                0, 0, 0, 0, 0        };
        // set position to MAX
        startpoint.X = current_position.X = (int32_t) (X_MAX * STEPS_PER_MM_X);
        // go to zero
        t.F = MAXIMUM_FEEDRATE_X;
        enqueue(&t);
#endif
}

/// fund Y MIN endstop
void home_y_negative() {
        queue_wait();

#if defined Y_MIN_PIN
        uint8_t	denoise_count = 0;

        // home Y
        y_enable();
        // hit home hard
        y_direction(0);
        while (denoise_count < 8) {
                // denoise
                if (y_min())
                        denoise_count++;
                else
                        denoise_count = 0;
                // step
                y_step();
                delay(5);
                unstep();
                // wait until neyt step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Y / ((float) MAXIMUM_FEEDRATE_Y)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }
        denoise_count = 0;

        // back off slowly
        y_direction(1);
        while (y_min() != 0) {
                // step
                y_step();
                delay(5);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Y / ((float) SEARCH_FEEDRATE_Y)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }

        // set Y home
        startpoint.Y = current_position.Y = 0;
#endif
}

/// find Y MAX endstop
void home_y_positive() {
        queue_wait();

#if defined Y_MAX_PIN
        uint8_t	denoise_count = 0;

        // home Y
        y_enable();
        // hit home hard
        y_direction(1);
        while (denoise_count < 8) {
                // denoise
                if (y_max())
                        denoise_count++;
                else
                        denoise_count = 0;
                // step
                y_step();
                delay(5);
                unstep();
                // wait until neyt step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Y / ((float) MAXIMUM_FEEDRATE_Y)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }
        denoise_count = 0;

        // back off slowly
        y_direction(0);
        while (y_max() != 0) {
                // step
                y_step();
                delay(5);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Y / ((float) SEARCH_FEEDRATE_Y)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }

        // set Y home
        TARGET t = {
                0, 0, 0, 0, 0        };
        // set position to MAX
        startpoint.Y = current_position.Y = (int32_t) (Y_MAX * STEPS_PER_MM_Y);
        // go to zero
        t.F = MAXIMUM_FEEDRATE_Y;
        enqueue(&t);
#endif
}

/// find Z MIN endstop
void home_z_negative() {
        queue_wait();

#if defined Z_MIN_PIN
        uint8_t	denoise_count = 0;

        // home Z
        z_enable();
#ifdef DEBUG  
        sersendf_P(PSTR("MAXIMUM_FEEDRATE_Z=%lu, SEARCH_FEEDRATE_Z=%lu\n"),(uint32_t)MAXIMUM_FEEDRATE_Z,(uint32_t)SEARCH_FEEDRATE_Z);
 
  /*      z_direction(1);
        uint16_t i=0;
        for (i=0;i<5000;i++) {
                
                
                z_step();
                delay(5);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Z / ((float) MAXIMUM_FEEDRATE_Z)));
        }*/
        
       sersendf_P(PSTR("hit home hard; z_min=%u\n"),(uint8_t)z_min());
#endif

        // hit home hard
        z_direction(0);
        while (denoise_count < 8) {
                // denoise
                if (z_min())
                        denoise_count++;
                else
                        denoise_count = 0;
                // step
                z_step();
                delay(4);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Z / ((float) MAXIMUM_FEEDRATE_Z)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }
        denoise_count = 0;
#ifdef DEBUG
        sersendf_P(PSTR("back off slowly; z_min=%u\n"),(uint8_t)z_min());
#endif
        // back off slowly
        z_direction(1);
        while (z_min() != 0) {
                // step
                z_step();
                delay(4);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Z / ((float) SEARCH_FEEDRATE_Z)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }

#ifdef DEBUG  
        sersendf_P(PSTR("home; z_min=%u\n"),(uint8_t)z_min());
#endif
        // set Z home
        startpoint.Z = current_position.Z = 0;
        z_disable();
#endif
}

/// find Z MAX endstop
void home_z_positive() {
        queue_wait();

#if defined Z_MAX_PIN
        uint8_t	denoise_count = 0;

        // home Z
        z_enable();
#ifdef DEBUG  
        sersendf_P(PSTR("1; z_max=%u\n"),(uint8_t)z_max());
#endif

        // hit home hard
        z_direction(1);
        while (denoise_count < 8) {
                // denoise
                if (z_max())
                        denoise_count++;
                else
                        denoise_count = 0;
                // step
                z_step();
                delay(4);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Z / ((float) MAXIMUM_FEEDRATE_Z)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }

        denoise_count = 0;

#ifdef DEBUG  
        sersendf_P(PSTR("2; z_max=%u\n"),(uint8_t)z_max());
#endif
        // back off slowly
        z_direction(0);
        while (z_max() != 0) {
                // step
                z_step();
                delay(4);
                unstep();
                // wait until next step time
                delay((uint32_t) (60.0 * 1000000.0 / STEPS_PER_MM_Z / ((float) SEARCH_FEEDRATE_Z)));
                ifclock(clock_flag_often) {
                        clock_often();
                }                
        }

#ifdef DEBUG  
        sersendf_P(PSTR("3; z_max=%u\n"),(uint8_t)z_max());
#endif
        // set Z home
        TARGET t = {
                0, 0, 0, 0, 0        };
        // set position to MAX
        startpoint.Z = current_position.Z = (int32_t) (Z_MAX * STEPS_PER_MM_Z);
        // go to zero
        t.F = MAXIMUM_FEEDRATE_Z;
        enqueue(&t);
#endif
}


