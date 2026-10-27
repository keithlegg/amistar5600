#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_ON  PORTB |= (1<<PORTB5)
#define LED_OFF PORTB &= ~(1<<PORTB5)
#define LED_TOGGLE  PINB |= (1<<PINB5)
#define SWITCH_PRESSED !(PINB & (1<<PINB7))


ISR(PCINT0_vect){
    
}

int main(void) {
    DDRD |= (1 << PD5); // set PB5 as output pin
    DDRB &= ~(1<<DDB7); //set PB7 as an input pin
    
    PCMSK0 |= (1<<PCINT7);
    PCICR |= (1<<PCIE0);
    
    sei();
    
    while (1) {
        
        
         
    }
}