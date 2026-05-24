#ifndef _ACTUATORH_
#define _ACTUATORH_



uint8_t idx_to_byte(uint8_t idx);



void invert_on(void);

void test_ports(void);

unsigned char invert_bits_1(unsigned char val);

unsigned char invert_bits_2(unsigned char old_val);














#endif
