 
#define F_CPU 9600000UL // Define software reference clock for delay duration
#include <avr/io.h>
#include <util/delay.h>

#define LED PB0


int i ;


/*
   reset|PB5   .--|_|--.  VCC
   XTAL1|PB3   |       |  PB2|SCK
   XTAL2|PB4   |       |  PB1|MISO
         GND   |_______|  PB0|MOSI
*/

int main (void)
{
     DDRB |= (1 << LED); // OC0A on PB0
     
     TCCR0A |= ((1 << COM0A1) | (1 << COM0A0) // COM0A1 - COM0A0 (Set OC0A on Compare Match, clear OC0A at TOP)
     | (1 << WGM01) | (1 << WGM00)); // WGM01 - WGM00 (set fast PWM)
     OCR0A = 0; // initialize Output Compare Register A to 0
     TCCR0B |= (1 << CS01); // Start timer at Fcpu / 256
     
     for (;;)
     {
         
         for (i = 0 ; i < 255 ; i++ ) // For loop (Up counter 0 - 255)
         {
             OCR0A = i; // Update Output Compare Register (PWM 0 - 255)
             _delay_ms(1);
         }
         
         for (i = 255 ; i > 1 ; i-- ) // For loop (down counter 255 - 0 )
         {
             OCR0A = i; // Update Output Compare Register (PWM 0 - 255)
             _delay_ms(1);
         }
             

     }
}
 
