/*

void send_txt_byte( uint8_t data, uint8_t use_newline){
   uint8_t i = 0;

   for (i=0; i<=7; i++) {
       //if ( !!(data & (1 << ii)) ){  // LSB
       if ( !!(data & (1 << (7 - i))) ){  // MSB
           UART_Transmit( BIT_OFF );
       }else{
           UART_Transmit( BIT_ON );
       }
    }

    if(use_newline!=0){
        UART_Transmit( 0xa ); //CHAR_TERM = new line  
        UART_Transmit( 0xd ); //0xd = carriage return
    }
}


//for printing internal 16 bit numbers - all serial related I/O is bytesX2
void send_txt_2bytes( uint16_t data, uint8_t use_newline,  uint8_t use_space){
   uint8_t i = 0;

   for (i=0; i<=15; i++) {
       //if (i==8){  UART_Transmit(0x20); }//middle space 

       if ( !!(data & (1 << (15 - i))) ){  // MSB
           UART_Transmit( BIT_OFF );
       }else{
           UART_Transmit( BIT_ON );
       }
    }
    
    if(use_space!=0){
        UART_Transmit(0x20);    //SPACE 
    }

    if(use_newline!=0){
        UART_Transmit( 0xa ); //CHAR_TERM = new line  
        UART_Transmit( 0xd ); //0xd = carriage return
    }
}



void UART_Init( unsigned int ubrr)
{
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    
    //DOH !! RUN AT FULL SPEED 115K!!!
    UCSR0A |= (1<<U2X0); //DAMN DAMN DAMN - HOW DID I NOT FIND THIS SOONER????

}


//static uint8_t UART_receive(void)
uint8_t UART_receive(void)
{
    while (!(UCSR0A & (1 << RXC0))) {}
    return UDR0;
}

void UART_Transmit( unsigned char data )
{
  while ( !( UCSR0A & (1<<UDRE0)) );
  UDR0 = data;
}


void UARTWriteStr(char *data) 
{ 
   while(*data){ 
      UART_Transmit(*data++); 
   }
}



unsigned char y[20]; // in main
unsigned char len;

unsigned char uartrecieve(unsigned char *x, unsigned char size)
{
    unsigned char i = 0;

    if (size == 0) return 0;              // return 0 if no space

    while (i < size - 1) {                // check space is available (including additional null char at end)
        unsigned char c;
        while ( !(UCSR0A & (1<<RXC0)) );  // wait for another char - WARNING this will wait forever if nothing is received
        c = UDR0;
        if (c == '\0') break;             // break on NULL character
        x[i] = c;                         // write into the supplied buffer
        i++;
    }
    x[i] = 0;                             // ensure string is null terminated

    return i + 1;                         // return number of characters written
} 

//len = uartrecieve(y, 20);

//test for interrupt to halt on UART recieve
 
//Writing this bit to one enables interrupt on the RXCn Flag. A UART Receive Complete interrupt will be generated
//only if the RXCIEn bit is written to one, the Global Interrupt Flag in SREG is written to one and the RXCn bit in
//UCSRnA is set.

void init_uart_interrupt(){
   UCSR0B |= (1 << RXCIE0); // Enable the UART Recieve Complete interrupt (UART_RXC) 
   //sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed 
}




void echo_uart(void){
    uint8_t buf = UART_receive();
    send_txt_byte(buf, 1);
    scribe_byte_astext(buf);
}


//   DEBUG - NOT DONE YET 

void tx_bytes(uint16_t startaddr, uint16_t numbytes)
{
     uint16_t b = 0;

     for(b=startaddr;b<numbytes;b++)
     {
         uint8_t tmp = read_ram(b);
         while ( !( UCSR0A & (1<<UDRE0)) );
         UDR0 = tmp;

         //flash LEDS to indicate we are working on sending data 
         if(b%512==0){
             LED_RGB_PORT^=0x02;//red 
         } 
         if(b%2048==0){
             LED_RGB_PORT^=0x01; //green  
         } 
         
     }//B-Y
}


uint16_t rx_two_bytes(void){
    uint8_t c1 =  UART_receive();//lsb addr
    uint8_t c2 =  UART_receive();//msb addr
    //send_txt_2bytes(buf1_16, 1, 0);
    scribe_str("2 bytes received.");
    return cvt_8x2_to_16(c1, c2); //fills buffer buf1_16

}
 
 
 //helper to grab arguments for commands over serial port 
 void rx_three_bytes(void){
    uint8_t c1 =  UART_receive();//lsb addr
    uint8_t c2 =  UART_receive();//msb addr 
    buf2_lsb =  UART_receive();//data byte 

    buf1_16 = cvt_8x2_to_16(c1, c2); //fills buffer buf1_16
    scribe_str("3 bytes received.");
 }
 
 
//helper to grab arguments for commands over serial port 
void rx_four_bytes(void){
    // uint8_t c1 =  UART_receive();//lsb addr
    // uint8_t c2 =  UART_receive();//msb addr 
    // uint8_t d1 =  UART_receive();//lsb addr
    // uint8_t d2 =  UART_receive();//msb addr 
    // cvt_8x2_to_16(d1, d2); //fills buffer buf1_16
    // buf2_16 = buf1_16;     //copy buffer 
    // cvt_8x2_to_16(c1, c2); //fills buffer buf1_16
    // send_txt_2bytes(buf1_16, 1, 0);
    // send_txt_2bytes(buf2_16, 1, 0);
    
    buf1_lsb =  UART_receive();//lsb addr
    buf1_msb =  UART_receive();//msb addr 
    buf2_lsb =  UART_receive();//lsb addr
    buf2_msb =  UART_receive();//msb addr 
    scribe_str("4 bytes received.");
}

*/
