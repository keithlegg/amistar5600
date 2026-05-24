#ifndef _SERIALH_
#define _SERIALH_






void print_byte( uint8_t data);

void send_txt_byte( uint8_t data, uint8_t use_newline);

void send_txt_2bytes( uint16_t data, uint8_t use_newline,  uint8_t use_space);


void UART_Init( unsigned int ubrr);

uint8_t UART_receive(void);

void UART_Transmit( unsigned char data );


void UARTWriteStr(char *data);

unsigned char uartrecieve(unsigned char *x, unsigned char size);

void echo_uart(void);

uint16_t rx_two_bytes(void);
 
void rx_three_bytes(void);

void rx_four_bytes(void);







#endif

