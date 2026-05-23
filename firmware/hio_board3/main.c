/*
   Keith Legg 
   created  - Jan 7 - 2016
   modified - May 22, 2026 


   any function that begins with "scribe" draws on the LCD screen
   any function that begins with "read/write" operates on SRAM
   any function that begins with "send" sends data over UART

   //-----------------------------------
   // ORIGINAL ADAFRUIT NOTES:

    3-5V Vin connects to the Arduino 5V pin 
    GND  connects to Arduino ground 
    CLK  connects to SPI clock. Digital 13. 
    MOSI connects to SPI MOSI.  Digital 11. 
    CS   connects to our SPI Chip Select pin. 
    RST  connects to our TFT reset pin.
    D/C  connects to our SPI data/command select pin. 
 

   //-----------------------------------
   // MY NOTES 

   # running on this board 
   # https://www.amazon.com/CH340G-ATMEGA2560-16AU-pinheaders-Compatible-Mega2560/dp/B0GWX3BLY2
   (working) fuse bits -l 0xff -h 0x99
 
   SPI:
       4 bits per chunk 
       MSB first - Leading edge 
       ~2us per byte speed estimate 

       DC IS LOW FOR CMDS (ST7735_SWRESET, ETC) - FOR "ARGS" IT IS HIGH
       ADAFRUIT CODE CALLS DC  "RS" or "_rs"

   //-----------------------------------
   // HARDWARE WIRING 
   pin8   | PE4 | D/C          - (soft) data command - PORTE4, ARDUINO 2
   pin9   |  .  | NC           -        RESET HARDWIRED HIGH 
   pin10  | PE5 | SPI ENABLE   - (soft) TFT_CS      - PORTE4, ARDUINO 3
   pin11  | PB2 | SPI MOSI     -        RUNS AS SPI MASTER 
   pin12  | PB4 | NC - MISO    -        NOT USED 
   pin13  | PB1 | SPI CLOCK    - 
   +5/+3  |     | RESET 
          |  .  | NC - CARD_CS 

   //-----------------------------------

*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "src/ST7735.h"

#include <avr/pgmspace.h>

// UART STUFF
#define FOSC 16000000UL
#define BAUD 57600
#define MYUBRR FOSC/16/BAUD-1

#define BIT_ON 0x30 //ascii 1
#define BIT_OFF 0x31 //ascii 0

//-------------------
//SPI stuff 

#define CTRL_DDR DDRE    
#define SPI_DDR DDRB     


#include <util/delay.h>


uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

//standard buffers for 8 to 16 bit stitching and unwrapping
//often we just need 3 bytes (2 addr+1 data) in that case ignore com_msb2
uint8_t buf1_lsb = 0;
uint8_t buf1_msb = 0;
uint8_t buf2_lsb = 0;
uint8_t buf2_msb = 0;

/***********************************************/
//I never did figure out char mapping but it uses this:
//https://en.wikipedia.org/wiki/Code_page_437 
void scribe_str(char *data) 
{ 
   //uint8_t cursor_x = 0;
   //uint8_t cursor_y = 0;
  
   cursor_y+=10;
   if(cursor_y>=160){
       cursor_y=0;
       ST7735_FillScreen(1);
   }

   uint8_t ct = 0;
   while(*data){ 
      //ST7735_DrawCharS(8*ct, 10, *data++, 0x0ff0, 0x0000, 1);
      ST7735_DrawCharS(ct*7, cursor_y, *data++, 0x0ff0, 0x0000, 1);
      
      ct++; 
   }

}

//*******************************************//
 
//analog of send text byte - write a byte to screen as 1's and 0's

void scribe_byte_astext( uint8_t data) 
{ 
   uint8_t ct = 0;
   uint8_t i = 0;
   uint8_t ypos = cursor_y;

   cursor_y+=10;
   if(cursor_y>=160){
       cursor_y=0;
       ST7735_FillScreen(1);
   }

   for (i=0; i<=7; i++) {
       if ( !!(data & (1 << (7 - i))) ){  // MSB
           ST7735_DrawCharS(i+(ct*7), cursor_y, BIT_OFF, 0x33f0, 0x0000, 1);
       }else{
           ST7735_DrawCharS(i+(ct*7), cursor_y, BIT_ON , 0x33f0, 0x0000, 1);
       }
       ct++;//advance "cursor"
    }
 
}

//*******************************************//
void scribe_byte2_astext( uint16_t data) 
{ 
   uint8_t ct = 0;
   uint8_t i = 0;
   uint8_t ypos = cursor_y;

   cursor_y+=10;
   if(cursor_y>=160){
       cursor_y=0;
       ST7735_FillScreen(1);
   }

   for (i=0; i<=15; i++) {
       if ( !!(data & (1 << (15 - i))) ){  // MSB
           ST7735_DrawCharS(i+(ct*7), cursor_y, BIT_OFF, 0xf0ff, 0x0000, 1);
       }else{
           ST7735_DrawCharS(i+(ct*7), cursor_y, BIT_ON , 0xf0ff, 0x0000, 1);
       }
       ct++;//advance "cursor"
    }
 
}




/***********************************************/
/*THESE USE GLOBALS INSTEAD OF RETURNING A VALUE!!!*/
void cvt_16_to_8x2(uint16_t input){
   //break 16 bit into 2 bytes - output goes to global buffer
    if(input<256){
        buf1_lsb = input; 
        buf1_msb = 0; 
    }        
    if(input>=256){
        buf1_lsb = (uint8_t)(input & 0x00FF);
        buf1_msb = (uint8_t)(input >> 8);
    }
}

/***********************************************/
uint16_t cvt_8x2_to_16(uint8_t msbin, uint8_t lsbin){
   uint8_t dat_8;
   uint16_t dat_16;
   dat_8 = msbin;
   dat_16 = (lsbin<<8);
   return (dat_16 | dat_8);
}






/************************************************/

void SPI_Init(void)
{
   //hardware SPI lines
   SPI_DDR  = 0xff;  //hardware SPI lines on PORTB
   //software chip select lines (TFT_CS , DC) 
   CTRL_DDR = 0xff;  //software controlled SPI/CS lines on PORTE

   SPCR |= (1<<MSTR)|(1<<SPE);  // SPI Master, SPI Enable
   //SPSR |= SPI2X;

}





/***********************************************/

void bitmap_test(void){
   //DRAW AN IMAGE FROM AVR'S RAM 
   //IMAGE WILL BE UPSIDE DOWN (TOP LEFT ORIGIN)  

  const uint16_t imagedata[] = {   
      0x42CD, 0x42CD, 0x3A8C, 0x324C, 0x324B, 0x3A8C, 0x42ED, 0x4B0E, 0x42ED, 0x42ED,
      0x4B0E, 0x4B2E, 0x42CD, 0x42AC, 0x42CD, 0x3AAC, 0x3AAC, 0x3AAC, 0x3AAC, 0x3AAC,
      0x4AED, 0x4B0D, 0x4AED, 0x42CD, 0x42CD, 0x3ACD, 0x42CD, 0x42CD, 0x4A4B, 0x5907,
      0x4ACD, 0x4AED, 0x4B0E, 0x4B0D, 0x42ED, 0x42CD, 0x42CD, 0x42CD, 0x42CD, 0x4AED,
      0x3ACD, 0x3A8C, 0x3A8C, 0x3AAC, 0x42CD, 0x42CD, 0x42CD, 0x4AED, 0x42ED, 0x42ED,
      0x4B0D, 0x4ACC, 0x428B, 0x3A6B, 0x3A4A, 0x3A4A, 0x3A4A, 0x3A4A, 0x42AC, 0x326B,
      0x3A6C, 0x326C, 0x326B, 0x324B, 0x2A09, 0x29C9, 0x21A8, 0x29C8, 0x2167, 0x1946,
      0x1945, 0x10C4, 0x2146, 0x2145, 0x2145, 0x2145, 0x2166, 0x2146, 0x42ED, 0x42ED,
      0x42ED, 0x4B0E, 0x4B0D, 0x42ED, 0x4B0E, 0x4B0E, 0x532E, 0x534F, 0x4B0E, 0x4B0D,
      0x3AAD, 0x3AAD, 0x42CD, 0x42ED, 0x3ACD, 0x3A8C, 0x3A8C, 0x3AAD, 0x430D, 0x4B0D
   };
   
   const uint16_t *img = &imagedata;
   
   ST7735_DrawBitmap(20, 20, img, 10, 10);
}


/***********************************************/
/*
void blit_test(void){
   //COPY SRAM DATA FROM FROM AVR'S RAM , THEN DRAW FROM SRAM 
   //THIS IS THE FIRST HALF OF TRANSMIT BMP FUNCTION OVER UART
   //IMAGE WILL BE UPSIDE DOWN (TOP LEFT ORIGIN)  
   const uint16_t imagedata[] = {   
                              0x00aa,0x00ff,0x00aa,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0xffff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ea,0x00ff 
                             ,0x00ff,0x00ff,0xffff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ea,0x00ff 
                             ,0x00ff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ea,0x00ff 
                             ,0xffff,0x00ff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0xffff,0x00ff,0x00ff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00ff,0x00ff,0xffff,0xffff,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                             ,0x00aa,0x00ff,0x00ff,0x00aa,0x00ea,0x0ff0,0x00ff,0x00ff,0x00ff,0x00ea 
                          };
   const uint16_t *img = &imagedata;
   
   blit_SRAM( 0, 0, 0, img, 10, 10);
   ST7735_DrawBitmapSRAM(10, 10, 0, 10, 10);
}
*/

/***********************************************/
/*
void ST7735_DrawBitmapSRAM(uint8_t x, uint8_t y, uint16_t start, uint8_t w, uint8_t h){  
  int8_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping
  int8_t originalWidth = w;              // save this value; even if not all columns fit on the screen, the image is still this width in ROM
  uint8_t i = w*(h - 1);

  uint8_t _height = 160;
  uint8_t _width  = 128;


  if((x >= _width) || ((y - h + 1) >= _height) || ((x + w) <= 0) || (y < 0)){
    return;                             // image is totally off the screen, do nothing
  }
  if((w > _width) || (h > _height)){    // image is too wide for the screen, do nothing
    //***This isn't necessarily a fatal error, but it makes the
    //following logic much more complicated, since you can have
    //an image that exceeds multiple boundaries and needs to be
    //clipped on more than one side.
    return;
  }
  if((x + w - 1) >= _width){            // image exceeds right of screen
    skipC = (x + w) - _width;           // skip cut off columns
    w = _width - x;
  }
  if((y - h + 1) < 0){                  // image exceeds top of screen
    i = i - (h - y - 1)*originalWidth;  // skip the last cut off rows
    h = y + 1;
  }
  if(x < 0){                            // image exceeds left of screen
    w = w + x;
    skipC = -1*x;                       // skip cut off columns
    i = i - x;                          // skip the first cut off columns
    x = 0;
  }
  if(y >= _height){                     // image exceeds bottom of screen
    h = h - (y - _height + 1);
    y = _height - 1;
  }

  setAddrWindow(x, y-h+1, x+w-1, y);
  
  uint16_t pixel = 0;
  uint16_t idx    = start;

  for(y=0; y<h; y=y+1){
    for(x=0; x<w; x=x+1){

        pixel = read_ram(idx);
        writedata( pixel ); //msb
        writedata( pixel ); //lsb
        idx++;

        i = i + 1;// go to the next pixel
    }
    i = i + skipC;
    i = i - 2*originalWidth;
  }
}
*/

/***********************************************/


int main (void)
{
    SPI_Init();

    ST7735_InitR(INITR_BLACKTAB);  
    ST7735_FillScreen(0); //clear screen black 
   
    uint16_t foo = 0xaaaa;
 
    while(1)
    { 
        scribe_byte2_astext(foo);

        _delay_ms(300); 

    }//while 1

}//main


///////////////////////////////////////////////////////////////////////////////////////////////////