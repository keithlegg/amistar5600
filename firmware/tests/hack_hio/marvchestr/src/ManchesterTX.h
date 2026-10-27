#ifndef _Manchester_h
#define _Manchester_h

#include <stdint.h>

#define TX_DDR DDRD
#define TX_DDB DDB2

#define TX_PORT PORTD
#define TX_PIN PD2

void initTX(void);

void sendByte(const uint16_t byte);


uint16_t computeChecksum(const uint8_t* data, uint16_t bytes);

#endif