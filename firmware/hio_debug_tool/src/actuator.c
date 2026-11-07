
#include <avr/io.h>

#include <util/delay.h>


#include "actuator.h"


/***********************************************/
uint8_t idx_to_byte(uint8_t idx){
    return (1 << idx);
} 


/***********************************************/

void invert_on(void){
    int a=0;
    for(a=0;a<8;a++){
       PORTL = idx_to_byte(a);
       _delay_ms(100);
    }
    for(a=0;a<8;a++){
       PORTC = idx_to_byte(a);
       _delay_ms(100);
    }       
}



/***********************************************/

void test_ports(void){
    int a=0;
    for(a=0;a<8;a++){
       PORTL = idx_to_byte(a);
       _delay_ms(100);
    }
    for(a=0;a<8;a++){
       PORTC = idx_to_byte(a);
       _delay_ms(100);
    }       
}


/***********************************************/
unsigned char invert_bits_1(unsigned char val)
{
    unsigned char tmp = 0;
    unsigned char i;
    if ( val )
        return (0);
    for ( i = 0; i < 8; i++)
        if ( val & ( 1 << ( 7 - i ) ) )
            tmp |= 1 << i;
    return (tmp);
}

/***********************************************/
unsigned char invert_bits_2(unsigned char old_val)
{
    unsigned char new_val = 0;
    if (old_val & 0x01) new_val |= 0x80;
    if (old_val & 0x02) new_val |= 0x40;
    if (old_val & 0x04) new_val |= 0x20;
    if (old_val & 0x08) new_val |= 0x10;
    if (old_val & 0x10) new_val |= 0x08;
    if (old_val & 0x20) new_val |= 0x04;
    if (old_val & 0x40) new_val |= 0x02;
    if (old_val & 0x80) new_val |= 0x01;
    return(new_val);
}
