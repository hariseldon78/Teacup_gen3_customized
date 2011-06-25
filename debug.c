#include	"debug.h"

volatile uint8_t	debug_flags;
#ifdef DEBUG_MODE
void debug_modes_start() {
        power_on();
#if DEBUG_MODE == 1
        io_init();
        int i;
        for (i=0;;i++){
                WRITE(DEBUG_LED, 1);
                delay_ms(i);
                WRITE(DEBUG_LED, 0);
                delay_ms(i);
        }
#endif
#if DEBUG_MODE == 2
#ifdef HOST__
        serial_init();
        io_init();
        sei();
        serial_writestr_P(PSTR("start\nok\n"));
#else 
        io_init();
#endif
        disable_transmit();
        WRITE(RX_ENABLE_PIN,0);
        SET_INPUT(RXD1);

        for(;;){
                if (READ(RXD1))
                        WRITE(DEBUG_LED, 1);
                else
                        WRITE(DEBUG_LED,0);
#ifdef HOST__
                uint8_t d=DDRD;
                uint8_t p=PORTD;
                sersendf_P(PSTR("DDRD: %d PORTD: %d\n"),d,p);   
                delay_ms(250);
#endif
        }                
#endif
#if DEBUG_MODE == 3
        enable_transmit();
        WRITE(TX_ENABLE_PIN,1);
        WRITE(RX_ENABLE_PIN,1);
        SET_OUTPUT(TX_ENABLE_PIN);
        SET_OUTPUT(RX_ENABLE_PIN);
        SET_OUTPUT(TXD1);
        for(;;){
                WRITE(TXD1,1);
                WRITE(DEBUG_LED, 1);
                delay_ms(5000);
                WRITE(TXD1,0);
                WRITE(DEBUG_LED, 0);
                delay_ms(5000);
        }
#endif
#if DEBUG_MODE == 4
#ifdef HOST
        serial_init();
        io_init();
        sei();
        serial_writestr_P(PSTR("start\nok\n"));
#else
        io_init();
#endif
        int i;
        for (i=0;;i++){
                delay_ms(2000);
#ifdef HOST
                uint8_t c='a'+i%26;
                sersendf_P(PSTR("char: %c hex: %x dec: %d\n"),c,c,c,d  );   
#endif
                WRITE(DEBUG_LED, i%2);

        }

#endif                
#if DEBUG_MODE == 5
//OK scollegando A
        io_init();
        //                enable_transmit();
        for(;;){
                WRITE(TXD,1);
                enable_transmit();
                //          WRITE(TXD1,1);
                WRITE(DEBUG_LED, 1);
                delay_ms(2000);
                
                WRITE(TXD,0);
                disable_transmit();
                //          WRITE(TXD1,0);
                WRITE(DEBUG_LED, 0);
                delay_ms(2000);
        }
#endif
#if DEBUG_MODE == 6
        serial_init();
        for(;;){
             	if (serial_rxchars() != 0) {
                        uint8_t c = serial_popchar();
                        sersendf_P(PSTR("%su\n"),c+1);
                }
        }
#endif
}
#endif

