
#include <avr/io.h>
#include <util/delay.h>

// #define F_CPU 16000000UL //AVR Clock Speed in MHZ
// #define FOSC 16000000    // Clock Speed
// #include <util/delay.h>

 
#include "hio_util.h"




//*******************************************//
void test_pins(void)
{
    sbi(PORTD,1);
    _delay_ms(200);
    
    cbi(PORTD,1);
    _delay_ms(200); 
}

//*******************************************//
void blink_led(void)
{
    PORTB |= (1<<5);
    _delay_ms(200);
    PORTB = 0x00;
    _delay_ms(200);     
}

