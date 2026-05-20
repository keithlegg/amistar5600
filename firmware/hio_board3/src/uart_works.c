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

void USART_Init( unsigned int ubrr)
{
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    /*Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

}


/***********************************************/

static uint8_t USART_receive(void)
{
    while (!(UCSR0A & (1 << RXC0))) {/*Busy wait.*/}
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
    //USART_Transmit( CHAR_TERM ); //CHAR_TERM = new line  
    //USART_Transmit( 0xd ); //0xd = carriage return
}

/***********************************************/


void print_byte_16( uint16_t data){
   uint8_t i = 0;

   for (i=0; i<=15; i++) {
       //if ( !!(data & (1 << ii)) ){  // LSB
       if ( !!(data & (1 << (15 - i))) ){  // MSB
           USART_Transmit( BIT_OFF );
       }else{
           USART_Transmit( BIT_ON );
       }
    }
    //USART_Transmit( CHAR_TERM ); //CHAR_TERM = new line  
    //USART_Transmit( 0xd ); //0xd = carriage return
}


/******************************************************/

#define CE_PIN 1
#define OE_PIN 1
#define WE_PIN 1



/******************************************************/


int main (void)
{
   //DDRB =0b00001000;  //arduino pin 11 
   //DDRB |= (1 << 3);  //arduino pin 11
   //DDRB = 0xff;      
     
   DDRB = 0xff;     

   //DDRD |= (0x4); // PORTD!
   USART_Init(MYUBRR);

   
   while (1)
   {
       PORTB ^= 0xff;
       _delay_ms(200);
       
       USART_Transmit( 0x41 );         
       //USART_Transmit( 0x0d ); // CR          
       //USART_Transmit( 0x0a ); // \n

   }
} 

/******************************************************/



