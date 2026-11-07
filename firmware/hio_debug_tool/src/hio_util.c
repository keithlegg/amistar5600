#include <avr/io.h>


#define F_CPU 16000000UL //AVR Clock Speed in MHZ
#define FOSC 16000000    // Clock Speed
#include <util/delay.h>

 
#include "hio_util.h"

//*******************************************//
//onboard LED indicator 
void init_ob_led(void)
{
    DDRB |= (1<<PB7);
}



//*******************************************//
/*
    wiring (arduino mega clone)
    
    # LCD SCREEN SPI CTRL
    PE4    (D2) - TFT_CS 
    PE5    (D3) - TFT_DC 
    
    # DIFFERENTIAL SERIAL ALS171/CTRL 
    PE1/TX (D1) - 171PIN 3 
    PG5    (D4) - 171PIN RE 
    PE3    (D5) - 171PIN DE
    PH3    (D6) - 171PIN CDE 
*/

//-------------------
//set the control lines on the 75ALS171 transciever chip
void init_transceiver(void)
{

    //  XXX &= ~(1 << DDD2);             // Clear the PD2 pin
    //  XXX |= (1 << PD3)|(1 << PD2);    // turn on the pull-ups    
    //set data direction registers
    XVR_DDR_CTRL_1 |= (1 << PG5); //PDG 
    XVR_DDR_CTRL_2 |= (1 << PE3); //PDE (shared with TFT)
    XVR_DDR_CTRL_3 |= (1 << PH3); //PFH    

}

//*******************************************//
//set the control lines on the 75ALS171 transciever chip
/*DE,CDE high, RE low */
void set_rxmode(void)
{

    cbi(XVR_CTRL_1,3);        //PDG
    XVR_CTRL_3 |= (1 << PH3); //PDH
    XVR_CTRL_2 |= (1 << PG5); //PEG
}


//*******************************************//
//set the control lines on the 75ALS171 transciever chip
/*DE,CDE,RE all high */
void set_txmode(void)
{
    XVR_CTRL_1 |= (1 << PG5); //PDG
    XVR_CTRL_2 |= (1 << PE3); //PDE (shared with TFT)
    XVR_CTRL_3 |= (1 << PH3); //PFH
}



//*******************************************//
void test_pins(void)
{
    sbi(PORTD,1);
    _delay_ms(200);
    
    cbi(PORTD,1);
    _delay_ms(200); 
}

//*******************************************//
//arduino mega pins 
void blink_led(void)
{
    PORTB |= (1<<7);
    _delay_ms(200);
    PORTB = 0x00;
    _delay_ms(200);     
}

