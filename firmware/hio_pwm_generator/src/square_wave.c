#include <avr/io.h>
#include <util/delay.h>


#define del 500

/*
   reset|PB5   .--|_|--.  VCC
   XTAL1|PB3   |       |  PB2|SCK
   XTAL2|PB4   |       |  PB1|MISO
         GND   |_______|  PB0|MOSI
*/


#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))

int main (void)
{
    DDRB = 0x01; 

    while (1)
    {   
        //pump the air solenoid while pin is high   
        if(PINB & 0x02)
        {
            sbi(PORTB,0);
            _delay_ms(100);
        
            cbi(PORTB,0);
            _delay_ms(del);

        }else
        {
            PORTB=0x00;
        }
 
    }

    return 1;
}


