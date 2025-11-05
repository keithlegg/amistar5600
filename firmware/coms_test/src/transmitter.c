#define F_CPU 16e6
#include <avr/io.h>
#include <util/delay.h>

//page 111 in datasheet 


#define FOSC 16000000 // Clock Speed
#define BAUD 57600
#define MYUBRR FOSC/16/BAUD-1


//*************************************************************
//USART_init: initializes the USART system
//*************************************************************

//page 183 of datasheet
void USART_Init( unsigned int ubrr)
{

	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}



static uint8_t USART_receive(void)
{
    while (!(UCSR0A & (1 << RXC0))) {/*Busy wait.*/}
    return UDR0;
}



void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}



void USART_tx_string( char *data )
{
while ((*data != '\0'))
   {
      while (!(UCSR0A & (1 <<UDRE0)));
      UDR0 = *data;
      data++;
   }   
}




//*******************************************//
//*******************************************//
void echo_uart(){
     //echo what we hear 
     unsigned char inout ;
     inout = USART_receive();
     USART_Transmit( inout );

}


//*******************************************//

int main(){
  DDRB = 0xFF; 

  //USART_Init(103);
  USART_Init(MYUBRR);
  
  //uint8_t inout = 0;
  //unsigned char inout ;

  USART_Transmit( 0x42 );
  USART_Transmit( 0x43 );
  USART_Transmit( 0x44 );


  while (1)
  {
     echo_uart();
     //_delay_ms(1000);
  }
  

 return 1;
}




