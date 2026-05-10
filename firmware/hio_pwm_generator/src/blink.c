#include <avr/io.h>
#include <util/delay.h>


#define del 300

/*
   reset|PB5   .--|_|--.  VCC
   XTAL1|PB3   |       |  PB2|SCK
   XTAL2|PB4   |       |  PB1|MISO
         GND   |_______|  PB0|MOSI
*/


/*
void delayCycles(int cycles)
{
  //char jumpLength=50-cycles;
  int i = cycles;

  while(i>0){
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    i--;
  }
  return
}
*/

int main (void)
{
  /* set PORTB for output*/
  DDRB = 0xff; 
  
  while (1)
    {
      PORTB = 0x00;
      _delay_ms(del);

      PORTB = 0x01;
      _delay_ms(del);


    }

  return 1;
}
