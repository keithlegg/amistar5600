#include <avr/io.h>

#define F_CPU 16000000UL //AVR Clock Speed in MHZ
#define FOSC 16000000    // Clock Speed

//UART STUFF
#define BAUD 57600
#define MYUBRR FOSC/16/BAUD-1




#define BIT_ON 0x30 //logic high
#define BIT_OFF 0x31 //logic low

#include <util/delay.h>


/***********************************************/

// UTIL functions 


uint8_t idx_to_byte(uint8_t idx){
    return (1 << idx);
} 




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


/***********************************************/
/***********************************************/



/***********************************************/

void USART_Init( unsigned int ubrr)
{
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

}


/***********************************************/

static uint8_t USART_receive(void)
{
    while (!(UCSR0A & (1 << RXC0))) {}
    return UDR0;
}

/***********************************************/

void USART_Transmit( unsigned char data )
{
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = data;
}


/***********************************************/

void print_byte( uint8_t data){
   uint8_t i = 0;

   for (i=0; i<=7; i++) {
       //if ( !!(data & (1 << ii)) ){  // LSB
       if ( !!(data & (1 << (7 - i))) ){  // MSB
           USART_Transmit( BIT_OFF );
       }else{
           USART_Transmit( BIT_ON );
       }
    }
    USART_Transmit( 0xa ); //CHAR_TERM = new line  
    USART_Transmit( 0xd ); //0xd = carriage return
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

int main (void)
{

    DDRC = 0xff;     
    DDRL = 0xff;  
    //DDRL = 0xff; 

    //PORTD |= 0x08; 
    //DDRD |= (0x4); // PORTD!
    
    //USART_Init(MYUBRR);
    //USART_Transmit(0x41); 

    uint16_t a = 0;

    uint8_t foo = 0x08;

    while(1)
    {   
        PORTL = invert_bits_2(foo); 
        _delay_ms(1000);

        PORTL = foo; 
        _delay_ms(1000);

    }


} 

/***********************************************/






/***********************************************/
/*
uint8_t read_ram(uint16_t address){
    
    uint8_t out = 0x00;

    DDRK = 0x00; 
    PORTD |= CS_PIN; 
    PORTK = 0x00; //clear data port
    set_address(address);

    PORTD |= WE_PIN; //WE high when reading 
    PORTD |= OE_PIN; 

    PORTD &= ~CS_PIN; 
    PORTD &= ~OE_PIN; 

    out = PINK;
    
    PORTK = 0x00; //clear data port

    PORTD |= OE_PIN; 
    PORTD |= CS_PIN; 

    return out;
}

void write_ram(uint16_t address, uint8_t byte){
    
    DDRK = 0xff;
    PORTD |= CS_PIN;  
    PORTD |= WE_PIN; 
    PORTD |= OE_PIN;     

    set_address(address);

    PORTD &= ~CS_PIN;  

    PORTK = byte;

    PORTD &= ~WE_PIN;     
    PORTD |= WE_PIN;  

}
*/


