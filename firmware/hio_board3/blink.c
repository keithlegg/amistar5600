#include <avr/io.h>

#define F_CPU 16000000UL //AVR Clock Speed in MHZ
#define FOSC 16000000    // Clock Speed

#include <util/delay.h>

/////////////////
/*

 avr-g++ -S -DF_CPU=16000000UL -mmcu=atmega2560 -o blink.S blink.c
 avr-g++ -DF_CPU=16000000UL -mmcu=atmega2560 -c blink.S -o blink.o
 avr-objcopy -O ihex -R .eeprom blink.o blink.hex
 sudo avrdude -V -c usbtiny -p atmega2560 -b 19200  -U flash:w:blink.hex


 avr-g++ -S  -mmcu=atmega2560 -o blink.S blink.c
 avr-g++ -mmcu=atmega2560 -c blink.S -o blink.o
 avr-objcopy -O ihex -R .eeprom blink.o blink.hex
 sudo avrdude -V -c usbtiny -p atmega2560 -b 19200  -U flash:w:blink.hex

*/

 //#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)



int main (void)
{
   //DDRB =0b00001000;  //arduino pin 11 
   //DDRB |= (1 << 3);  //arduino pin 11
   //DDRB = 0xff;      
     
   //DDRD = 0xff;     
   DDRH = 0xff;   
   
   while (1)
   {
       PORTH^=0x01;

       /*
       uint16_t a = 0;
       for(a=0;a<60000;a++){
        _NOP();
       }
       */   
                                      
       //PORTD ^= 0xff;
       _delay_ms(10000);

   }
} 

/////////////////



