#ifndef _UTILH_
#define _UTILH_


/*******************/
//set up the software control lines for TFT LCD/SPI and ALS171 transciever chip (serial) 
 
//arduino mega pins D2,D3 control the TFT screen 
//arduino mega pins D4,D5 control the TFT screen


#define XVR_DDR_CTRL_1 DDRG  
#define XVR_DDR_CTRL_2 DDRE // USE THIS PORT FOR SOFTWARE CONTROLED CTRL LINES (shared with TFT) 
#define XVR_DDR_CTRL_3 DDRH 

#define XVR_CTRL_1 PORTG  
#define XVR_CTRL_2 PORTE  // USE THIS PORT FOR SOFTWARE CONTROLED CTRL LINES (shared with TFT)
#define XVR_CTRL_3 PORTH 

#define XVR_RE 0x10   // PG5 (arduino D4 ,active low) recieve enable  
#define XVR_1DE 0x04  // PE3 (arduino D5)    
#define XVR_CDE 0x04  // PH3 (arduino D6) 

#define XVR_2DE 0x10   // PH4 (arduino D5)  


#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))


/*******************/
void init_transceiver(void);
void init_ob_led(void);

void set_rxmode(void);
void set_txmode(void);

void blink_led(void);


void test_pins(void);






#endif
