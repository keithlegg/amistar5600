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

    XVR_DDR_CTRL_1 = 0xff; //PDE
    XVR_DDR_CTRL_2 = 0xff; //PDG
    XVR_DDR_CTRL_3 = 0xff; //PFH    

    XVR_CTRL_1 = 0xff; //PDE
    XVR_CTRL_2 = 0xff; //PDG
    XVR_CTRL_3 = 0xff; //PFH
         

}

//-------------------
//set the control lines on the 75ALS171 transciever chip
/*DE,CDE high, RE low */
void set_rxmode(void)
{

    // cbi(PORTD,2); //recieve mode is active low 
    // PORTD |=  (1<<PD3) | (1<<PD4);


}

//*******************************************//
//set the control lines on the 75ALS171 transciever chip
/*DE,CDE,RE all high */
void set_txmode(void)
{
    // RE  = PD2
    // CDE = PD3
    // 1DE = PD4

    //PORTD |= (1<<PD2) | (1<<PD3) | (1<<PD4);
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

