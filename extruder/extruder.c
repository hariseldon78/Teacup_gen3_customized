#include	<stdint.h>
#include	<string.h>

#include	<avr/interrupt.h>
static uint8_t deb=0;

#include	"intercom.h"
#include	"analog.h"
#include	"config.h"
#include	"watchdog.h"
#include	"heater.h"
#include	"temp.h"
#include	"timer.h"

#include        "delay.h"

static uint8_t motor_pwm;

void io_init(void) {
        // setup I/O pins
        WRITE(DEBUG_LED, 0); 
        SET_OUTPUT(DEBUG_LED);
        WRITE(H1D,0); 
        SET_OUTPUT(H1D);
        WRITE(H1E,0); 
        SET_OUTPUT(H1E);
        WRITE(H2D,0); 
        SET_OUTPUT(H2D);
        WRITE(H2E,0); 
        SET_OUTPUT(H2E);

        SET_INPUT(TRIM_POT);
        SET_INPUT(TEMP_PIN);
        SET_INPUT(TEMP_BED_PIN);

#if defined E_STEP_PIN && defined E_DIR_PIN
        SET_INPUT(E_STEP_PIN);
        SET_INPUT(E_DIR_PIN);

        // use pull up resistors to avoid noise
        WRITE(E_STEP_PIN, 1);
        WRITE(E_DIR_PIN, 1);
#endif

        //Enable the RS485 transceiver
        SET_OUTPUT(RX_ENABLE_PIN);
        SET_OUTPUT(TX_ENABLE_PIN);
        WRITE(RX_ENABLE_PIN,0);
        disable_transmit();

#ifdef	HEATER_PIN
        WRITE(HEATER_PIN, 0); 
        SET_OUTPUT(HEATER_PIN);
#endif

#ifdef BED_PIN
        WRITE(BED_PIN, 0); 
        SET_OUTPUT(BED_PIN);
#endif

#ifdef FAN_PIN
        WRITE(FAN_PIN, 0); 
        SET_OUTPUT(FAN_PIN);
#endif

        // 	#if defined(HEATER_PWM) || defined(FAN_PWM) || defined(BED_PWM)
        // setup PWM timer: fast PWM, no prescaler
        TCCR2A = MASK(WGM21) | MASK(WGM20);
        TCCR2B = MASK(CS22);
        TIMSK2 = 0;
        OCR2A = 0;
        OCR2B = 0;
        // 	#endif

#if defined(H1E_PWM) && defined(H2E_PWM)
        TCCR0A = MASK(WGM01) | MASK(WGM00);
        TCCR0B = MASK(CS20);
        TIMSK0 = 0;
        OCR0A = 0;
        OCR0B = 0;
#endif
}

void motor_init(void) {
        //Enable an interrupt to be triggered when the step pin changes
        //This will be PCIE0
#ifndef MOTOR_OVER_INTERCOM
        PCICR = MASK(PCIE0);
        PCMSK0 = MASK(PCINT2);
#endif
}

#ifdef MOTOR_OVER_INTERCOM

uint8_t old_motor;

void motor_move() {
#else
ISR(PCINT0_vect) {
#endif
        static uint8_t coil_pos, pwm;

        //if the step pin is high, we advance the motor
#ifdef MOTOR_OVER_INTERCOM
        if (get_motor_step()) {
#else
        if (READ(E_STEP_PIN)) {
#endif

                //Turn on motors only on first tick to save power I guess
                enable_motors();

                //Advance the coil position
#ifdef MOTOR_OVER_INTERCOM
                if (get_motor_dir())
#else
                if (READ(E_DIR_PIN))
#endif 
                        coil_pos++;
                else
                        coil_pos--;

                coil_pos &= 7;

                //Grab the latest motor power to use
                pwm = motor_pwm;

                switch(coil_pos) {
                case 0:
                        WRITE(H1D, 0);
                        WRITE(H2D, 0);
                        H1E_PWM = 0;
                        H2E_PWM = pwm;
                        break;
                case 1:
                        WRITE(H1D, 1);
                        WRITE(H2D, 0);
                        H1E_PWM = pwm;
                        H2E_PWM = pwm;
                        break;
                case 2:
                        WRITE(H1D, 1);
                        WRITE(H2D, 0);
                        H1E_PWM = pwm;
                        H2E_PWM = 0;
                        break;
                case 3:
                        WRITE(H1D, 1);
                        WRITE(H2D, 1);
                        H1E_PWM = pwm;
                        H2E_PWM = pwm;
                        break;
                case 4:
                        WRITE(H1D, 1);
                        WRITE(H2D, 1);
                        H1E_PWM = 0;
                        H2E_PWM = pwm;  
                        break;
                case 5:
                        WRITE(H1D, 0);
                        WRITE(H2D, 1);
                        H1E_PWM = pwm;
                        H2E_PWM = pwm;  
                        break;
                case 6:
                        WRITE(H1D, 0);
                        WRITE(H2D, 1);
                        H1E_PWM = pwm;
                        H2E_PWM = 0;  
                        break;
                case 7:
                        WRITE(H1D, 0);
                        WRITE(H2D, 0);
                        H1E_PWM = pwm;
                        H2E_PWM = pwm;  
                        break;
                }
        }
}

void init(void) {

        // set up watchdog
        wd_init();

        // setup analog reading
        analog_init();

        // set up serial
        intercom_init();

        // set up inputs and outputs
        io_init();

        // temp sensor
        temp_init();

        // heater
        heater_init();

        // set up extruder motor driver
        motor_init();

        // set up clock
        timer_init();

        // enable interrupts
        sei();

        // reset watchdog
        wd_reset();

}


int main (void)
{
#ifndef DEBUG_MODE
        UCSR0B=0;
        //        uint8_t deb=0;
        uint32_t a=0;

        SET_OUTPUT(DEBUG_LED);
        init();

        enable_heater();

        // main loop
        for (;;)
        {
                if (++a>100000)
                {
                        a=0;
                        //                        WRITE(DEBUG_LED,++deb%2);
                        WRITE(DEBUG_LED,0);
                }       

                wd_reset();

                //Read motor PWM
                motor_pwm = analog_read(TRIM_POT_CHANNEL) >> 2;

                ifclock(CLOCK_FLAG_10MS) {
                        // check temperatures and manage heaters
                        temp_sensor_tick();

                }
                // check if we've had a new intercom packet
                if (intercom_flags & FLAG_NEW_RX) {
                        intercom_flags &= ~FLAG_NEW_RX;
                        
#ifdef MOTOR_OVER_INTERCOM

                        if (get_motor_step())
                        {
                                motor_move();
                                set_motor_step(0);
                       }
#endif


                        switch (rx.packet.control_word) {
                                // M105- read temperatures
                        case 105:
                                send_temperature(0, temp_get(0));
                                temp_set(0, read_temperature(0));
                                send_temperature(1, temp_get(1));
                                temp_set(1, read_temperature(1));
                                start_send();
                                break;
                        case 106:
#ifdef FAN_PIN
                                WRITE(FAN_PIN, 1);
#endif
                                break;
                        case 107:
#ifdef FAN_PIN
                                WRITE(FAN_PIN, 0);
#endif
                                break;
                                // M130 - set PID P factor
                        case 130:
                                pid_set_p(rx.packet.control_index, rx.packet.control_data_int32);
                                // M131 - set PID I factor
                        case 131:
                                pid_set_i(rx.packet.control_index, rx.packet.control_data_int32);
                                // M132 - set PID D factor
                        case 132:
                                pid_set_d(rx.packet.control_index, rx.packet.control_data_int32);
                                // M133 - set PID I limit
                        case 133:
                                pid_set_i_limit(rx.packet.control_index, rx.packet.control_data_int32);
                                // M134 - save PID values to eeprom
                        case 134:
                                heater_save_settings();
                                break;
                        case 299:
                                digitalWrite(DEBUG_PIN, 1);
                                delay(2);
                                digitalWrite(DEBUG_PIN, 0);
                                break;

                        }
                }

                if (intercom_flags & FLAG_TX_FINISHED) {
                        WRITE(TX_ENABLE_PIN,0);
                }
        }
#else
#if DEBUG_MODE == 1
        init();
        enable_heater();
        uint8_t deb=0;
        uint8_t count=0;

        int i;
        for (i=0;;i++){
                wd_reset();
                motor_pwm = analog_read(TRIM_POT_CHANNEL) >> 2;
                ifclock(CLOCK_FLAG_10MS) {
                        // check temperatures and manage heaters
                        temp_sensor_tick();
                        if (++count>100)
                        {
                                count=0;
                                SET_OUTPUT(DEBUG_LED);
                                WRITE(DEBUG_LED, (++deb) % 2);
                        }

                }

                // check if we've had a new intercom packet
                if (intercom_flags & FLAG_NEW_RX) {
                        intercom_flags &= ~FLAG_NEW_RX;

                        /*        switch (rx.packet.control_word) {
                         // M105- read temperatures
                         case 105:
                         send_temperature(0, temp_get(0));
                         temp_set(0, read_temperature(0));
                         send_temperature(1, temp_get(1));
                         temp_set(1, read_temperature(1));
                         start_send();
                         break;
                         // M130 - set PID P factor
                         case 130:
                         pid_set_p(rx.packet.control_index, rx.packet.control_data_int32);
                         // M131 - set PID I factor
                         case 131:
                         pid_set_i(rx.packet.control_index, rx.packet.control_data_int32);
                         // M132 - set PID D factor
                         case 132:
                         pid_set_d(rx.packet.control_index, rx.packet.control_data_int32);
                         // M133 - set PID I limit
                         case 133:
                         pid_set_i_limit(rx.packet.control_index, rx.packet.control_data_int32);
                         // M134 - save PID values to eeprom
                         case 134:
                         heater_save_settings();
                         break;
                         case 299:
                         digitalWrite(DEBUG_PIN, 1);
                         delay(2);
                         digitalWrite(DEBUG_PIN, 0);
                         break;
                         
                         }*/
                }
                /*                WRITE(DEBUG_LED, 1);
                 delay_ms(3);
                 WRITE(DEBUG_LED, 0);
                 delay_ms(3);*/
        }
#endif
#if DEBUG_MODE == 2
        intercom_init();
        io_init();
        WRITE(RX_ENABLE_PIN,0);
        WRITE(RXD,0);
        SET_INPUT(RXD);
        for(;;){
                if (READ(RXD))
                        WRITE(DEBUG_LED, 1);
                else
                        WRITE(DEBUG_LED,0);
        }                
#endif
#if DEBUG_MODE == 3
        //   init();
        UCSR0B=0;
        enable_transmit();
        WRITE(TX_ENABLE_PIN,1);
        WRITE(RX_ENABLE_PIN,1);
        SET_OUTPUT(TX_ENABLE_PIN);
        SET_OUTPUT(RX_ENABLE_PIN);
        SET_OUTPUT(TXD);
        for(;;){
                WRITE(TXD,1);
                WRITE(DEBUG_LED, 1);
                delay_ms(30000);
                WRITE(TXD,0);
                WRITE(DEBUG_LED, 0);
                delay_ms(30000);
        }
#endif
#if DEBUG_MODE == 5
        //OK scollegando A
        io_init();
        for(;;){
                enable_transmit();
                //                WRITE(TXD,0);
                WRITE(DEBUG_LED, 1);
                delay_ms(10000);
                disable_transmit();
                //                WRITE(TXD,0);
                WRITE(DEBUG_LED, 0);
                delay_ms(10000);
        }
#endif
#endif
}







