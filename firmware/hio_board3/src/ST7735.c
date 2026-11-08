
/*
    Built on code from Adafruit and Jonathan Valvano.  

    https://github.com/adafruit/Adafruit-ST7735-Library
    http://users.ece.utexas.edu/~valvano/arm/
*/

#include <avr/pgmspace.h>   

#include <avr/io.h>
#define F_CPU 16000000UL

#include <util/delay.h>
 
#include <stdio.h>
#include <stdint.h>

#include "ST7735.h"

/*******************/
//set up the software control lines for SPI 
#define CTRL_PORT PORTE  // USE THIS PORT FOR SOFTWARE CONTROLED SPI CS LINES
#define TFT_RST 0x04     // DONT THINK WE NEED THIS?
#define TFT_DC 0x10      // SEEMS TO PULSE EVERY 4 OR SO BYTES - 32bit? 
#define TFT_CS 0x20      // SPI SS/CS 

  

uint16_t ColStart, RowStart; // some displays need this changed
uint16_t Rotation;           // 0 to 3
enum initRFlags TabColor; //ADAFRUIT IS uint8_t

/*******************/

//IF I PASS A VARIABLE TO THIS IT GETS SET TO WRONG BITDEPTH
//RUNS TWICE AS SLOW AS EXPLICITLY TYPING NUMBERS  
//THIS WAS uint_16 because ADAFRUITS CODE HAS TO WORK ON SCREENS LARGER THAN 256 PX/SIDE
uint8_t _width  = 128;//ST7735_TFTWIDTH;   // this could probably be a constant, except it is used in Adafruit_GFX and depends on image rotation
uint8_t _height = 160;//ST7735_TFTHEIGHT;

// graphics routines
// y coordinates 0 to 31 used for labels and messages
// y coordinates 32 to 159  128 pixels high
// x coordinates 0 to 127   128 pixels wide

int8_t Ymax,Ymin,X;        // X goes from 0 to 127
int8_t Yrange; //YrangeDiv2;


/*******************/

// 16 rows (0 to 15) and 21 characters (0 to 20)
// Requires (11 + size*size*6*8) bytes of transmission for each character
uint8_t StX=0; // position along the horizonal axis 0 to 20
uint8_t StY=0; // position along the vertical axis 0 to 15
uint8_t StTextColor = ST7735_YELLOW;

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09


#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13


#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29


#define ST7735_CASET   0x2A //<-------
#define ST7735_RASET   0x2B //<-------
#define ST7735_RAMWR   0x2C //<-------
#define ST7735_RAMRD   0x2E


#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36


#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2 //<-------
#define ST7735_FRMCTR3 0xB3 //<-------
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6


#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5


#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD


#define ST7735_PWCTR6  0xFC


#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

///////////////////////////////

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

// standard ascii 5x7 font
// originally from glcdfont.c from Adafruit project
static const uint8_t Font[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00,
  0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
  0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
  0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
  0x18, 0x3C, 0x7E, 0x3C, 0x18,
  0x1C, 0x57, 0x7D, 0x57, 0x1C,
  0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
  0x00, 0x18, 0x3C, 0x18, 0x00,
  0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
  0x00, 0x18, 0x24, 0x18, 0x00,
  0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
  0x30, 0x48, 0x3A, 0x06, 0x0E,
  0x26, 0x29, 0x79, 0x29, 0x26,
  0x40, 0x7F, 0x05, 0x05, 0x07,
  0x40, 0x7F, 0x05, 0x25, 0x3F,
  0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
  0x7F, 0x3E, 0x1C, 0x1C, 0x08,
  0x08, 0x1C, 0x1C, 0x3E, 0x7F,
  0x14, 0x22, 0x7F, 0x22, 0x14,
  0x5F, 0x5F, 0x00, 0x5F, 0x5F,
  0x06, 0x09, 0x7F, 0x01, 0x7F,
  0x00, 0x66, 0x89, 0x95, 0x6A,
  0x60, 0x60, 0x60, 0x60, 0x60,
  0x94, 0xA2, 0xFF, 0xA2, 0x94,
  0x08, 0x04, 0x7E, 0x04, 0x08,
  0x10, 0x20, 0x7E, 0x20, 0x10,
  0x08, 0x08, 0x2A, 0x1C, 0x08,
  0x08, 0x1C, 0x2A, 0x08, 0x08,
  0x1E, 0x10, 0x10, 0x10, 0x10,
  0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
  0x30, 0x38, 0x3E, 0x38, 0x30,
  0x06, 0x0E, 0x3E, 0x0E, 0x06,
  0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x5F, 0x00, 0x00,
  0x00, 0x07, 0x00, 0x07, 0x00,
  0x14, 0x7F, 0x14, 0x7F, 0x14,
  0x24, 0x2A, 0x7F, 0x2A, 0x12,
  0x23, 0x13, 0x08, 0x64, 0x62,
  0x36, 0x49, 0x56, 0x20, 0x50,
  0x00, 0x08, 0x07, 0x03, 0x00,
  0x00, 0x1C, 0x22, 0x41, 0x00,
  0x00, 0x41, 0x22, 0x1C, 0x00,
  0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
  0x08, 0x08, 0x3E, 0x08, 0x08,
  0x00, 0x80, 0x70, 0x30, 0x00,
  0x08, 0x08, 0x08, 0x08, 0x08,
  0x00, 0x00, 0x60, 0x60, 0x00,
  0x20, 0x10, 0x08, 0x04, 0x02,
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0 //index 49 
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x72, 0x49, 0x49, 0x49, 0x46, // 2
  0x21, 0x41, 0x49, 0x4D, 0x33, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x31, // 6
  0x41, 0x21, 0x11, 0x09, 0x07, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x46, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x00, 0x14, 0x00, 0x00,
  0x00, 0x40, 0x34, 0x00, 0x00,
  0x00, 0x08, 0x14, 0x22, 0x41,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x00, 0x41, 0x22, 0x14, 0x08,
  0x02, 0x01, 0x59, 0x09, 0x06,
  0x3E, 0x41, 0x5D, 0x59, 0x4E,
  0x7C, 0x12, 0x11, 0x12, 0x7C, // A  //index 66
  0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x7F, 0x41, 0x41, 0x41, 0x3E, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x3E, 0x41, 0x41, 0x51, 0x73, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x02, 0x1C, 0x02, 0x7F, // M
  0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x26, 0x49, 0x49, 0x49, 0x32, // S
  0x03, 0x01, 0x7F, 0x01, 0x03, // T
  0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x3F, 0x40, 0x38, 0x40, 0x3F, // W
  0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x03, 0x04, 0x78, 0x04, 0x03, // Y
  0x61, 0x59, 0x49, 0x4D, 0x43, // Z
  0x00, 0x7F, 0x41, 0x41, 0x41,
  0x02, 0x04, 0x08, 0x10, 0x20,
  0x00, 0x41, 0x41, 0x41, 0x7F,
  0x04, 0x02, 0x01, 0x02, 0x04,
  0x40, 0x40, 0x40, 0x40, 0x40,
  0x00, 0x03, 0x07, 0x08, 0x00,
  0x20, 0x54, 0x54, 0x78, 0x40, // a //100
  0x7F, 0x28, 0x44, 0x44, 0x38, // b
  0x38, 0x44, 0x44, 0x44, 0x28, // c
  0x38, 0x44, 0x44, 0x28, 0x7F, // d
  0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x00, 0x08, 0x7E, 0x09, 0x02, // f
  0x18, 0xA4, 0xA4, 0x9C, 0x78, // g
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x20, 0x40, 0x40, 0x3D, 0x00, // j
  0x7F, 0x10, 0x28, 0x44, 0x00, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x78, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0x38, 0x44, 0x44, 0x44, 0x38, // o
  0xFC, 0x18, 0x24, 0x24, 0x18, // p
  0x18, 0x24, 0x24, 0x18, 0xFC, // q
  0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x48, 0x54, 0x54, 0x54, 0x24, // s
  0x04, 0x04, 0x3F, 0x44, 0x24, // t
  0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x44, 0x28, 0x10, 0x28, 0x44, // x
  0x4C, 0x90, 0x90, 0x90, 0x7C, // y
  0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x00, 0x08, 0x36, 0x41, 0x00,
  0x00, 0x00, 0x77, 0x00, 0x00,
  0x00, 0x41, 0x36, 0x08, 0x00,
  0x02, 0x01, 0x02, 0x04, 0x02,
  0x3C, 0x26, 0x23, 0x26, 0x3C,
  0x1E, 0xA1, 0xA1, 0x61, 0x12,
  0x3A, 0x40, 0x40, 0x20, 0x7A,
  0x38, 0x54, 0x54, 0x55, 0x59,
  0x21, 0x55, 0x55, 0x79, 0x41,
  0x21, 0x54, 0x54, 0x78, 0x41,
  0x21, 0x55, 0x54, 0x78, 0x40,
  0x20, 0x54, 0x55, 0x79, 0x40,
  0x0C, 0x1E, 0x52, 0x72, 0x12,
  0x39, 0x55, 0x55, 0x55, 0x59,
  0x39, 0x54, 0x54, 0x54, 0x59,
  0x39, 0x55, 0x54, 0x54, 0x58,
  0x00, 0x00, 0x45, 0x7C, 0x41,
  0x00, 0x02, 0x45, 0x7D, 0x42,
  0x00, 0x01, 0x45, 0x7C, 0x40,
  0xF0, 0x29, 0x24, 0x29, 0xF0,
  0xF0, 0x28, 0x25, 0x28, 0xF0,
  0x7C, 0x54, 0x55, 0x45, 0x00,
  0x20, 0x54, 0x54, 0x7C, 0x54,
  0x7C, 0x0A, 0x09, 0x7F, 0x49,
  0x32, 0x49, 0x49, 0x49, 0x32,
  0x32, 0x48, 0x48, 0x48, 0x32,
  0x32, 0x4A, 0x48, 0x48, 0x30,
  0x3A, 0x41, 0x41, 0x21, 0x7A,
  0x3A, 0x42, 0x40, 0x20, 0x78,
  0x00, 0x9D, 0xA0, 0xA0, 0x7D,
  0x39, 0x44, 0x44, 0x44, 0x39,
  0x3D, 0x40, 0x40, 0x40, 0x3D,
  0x3C, 0x24, 0xFF, 0x24, 0x24,
  0x48, 0x7E, 0x49, 0x43, 0x66,
  0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
  0xFF, 0x09, 0x29, 0xF6, 0x20,
  0xC0, 0x88, 0x7E, 0x09, 0x03,
  0x20, 0x54, 0x54, 0x79, 0x41,
  0x00, 0x00, 0x44, 0x7D, 0x41,
  0x30, 0x48, 0x48, 0x4A, 0x32,
  0x38, 0x40, 0x40, 0x22, 0x7A,
  0x00, 0x7A, 0x0A, 0x0A, 0x72,
  0x7D, 0x0D, 0x19, 0x31, 0x7D,
  0x26, 0x29, 0x29, 0x2F, 0x28,
  0x26, 0x29, 0x29, 0x29, 0x26,
  0x30, 0x48, 0x4D, 0x40, 0x20,
  0x38, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x38,
  0x2F, 0x10, 0xC8, 0xAC, 0xBA,
  0x2F, 0x10, 0x28, 0x34, 0xFA,
  0x00, 0x00, 0x7B, 0x00, 0x00,
  0x08, 0x14, 0x2A, 0x14, 0x22,
  0x22, 0x14, 0x2A, 0x14, 0x08,
  0xAA, 0x00, 0x55, 0x00, 0xAA,
  0xAA, 0x55, 0xAA, 0x55, 0xAA,
  0x00, 0x00, 0x00, 0xFF, 0x00,
  0x10, 0x10, 0x10, 0xFF, 0x00,
  0x14, 0x14, 0x14, 0xFF, 0x00,
  0x10, 0x10, 0xFF, 0x00, 0xFF,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x14, 0x14, 0x14, 0xFC, 0x00,
  0x14, 0x14, 0xF7, 0x00, 0xFF,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x14, 0x14, 0xF4, 0x04, 0xFC,
  0x14, 0x14, 0x17, 0x10, 0x1F,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0x1F, 0x00,
  0x10, 0x10, 0x10, 0xF0, 0x00,
  0x00, 0x00, 0x00, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0xF0, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x10,
  0x10, 0x10, 0x10, 0x10, 0x10,
  0x10, 0x10, 0x10, 0xFF, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x14,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x00, 0x00, 0x1F, 0x10, 0x17,
  0x00, 0x00, 0xFC, 0x04, 0xF4,
  0x14, 0x14, 0x17, 0x10, 0x17,
  0x14, 0x14, 0xF4, 0x04, 0xF4,
  0x00, 0x00, 0xFF, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x14, 0x14, 0xF7, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x17, 0x14,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0xF4, 0x14,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x00, 0x00, 0x1F, 0x10, 0x1F,
  0x00, 0x00, 0x00, 0x1F, 0x14,
  0x00, 0x00, 0x00, 0xFC, 0x14,
  0x00, 0x00, 0xF0, 0x10, 0xF0,
  0x10, 0x10, 0xFF, 0x10, 0xFF,
  0x14, 0x14, 0x14, 0xFF, 0x14,
  0x10, 0x10, 0x10, 0x1F, 0x00,
  0x00, 0x00, 0x00, 0xF0, 0x10,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFF, 0xFF,
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0x38, 0x44, 0x44, 0x38, 0x44,
  0x7C, 0x2A, 0x2A, 0x3E, 0x14,
  0x7E, 0x02, 0x02, 0x06, 0x06,
  0x02, 0x7E, 0x02, 0x7E, 0x02,
  0x63, 0x55, 0x49, 0x41, 0x63,
  0x38, 0x44, 0x44, 0x3C, 0x04,
  0x40, 0x7E, 0x20, 0x1E, 0x20,
  0x06, 0x02, 0x7E, 0x02, 0x02,
  0x99, 0xA5, 0xE7, 0xA5, 0x99,
  0x1C, 0x2A, 0x49, 0x2A, 0x1C,
  0x4C, 0x72, 0x01, 0x72, 0x4C,
  0x30, 0x4A, 0x4D, 0x4D, 0x30,
  0x30, 0x48, 0x78, 0x48, 0x30,
  0xBC, 0x62, 0x5A, 0x46, 0x3D,
  0x3E, 0x49, 0x49, 0x49, 0x00,
  0x7E, 0x01, 0x01, 0x01, 0x7E,
  0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
  0x44, 0x44, 0x5F, 0x44, 0x44,
  0x40, 0x51, 0x4A, 0x44, 0x40,
  0x40, 0x44, 0x4A, 0x51, 0x40,
  0x00, 0x00, 0xFF, 0x01, 0x03,
  0xE0, 0x80, 0xFF, 0x00, 0x00,
  0x08, 0x08, 0x6B, 0x6B, 0x08,
  0x36, 0x12, 0x36, 0x24, 0x36,
  0x06, 0x0F, 0x09, 0x0F, 0x06,
  0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x00, 0x10, 0x10, 0x00,
  0x30, 0x40, 0xFF, 0x01, 0x01,
  0x00, 0x1F, 0x01, 0x01, 0x1E,
  0x00, 0x19, 0x1D, 0x17, 0x12,
  0x00, 0x3C, 0x3C, 0x3C, 0x3C,
  0x00, 0x00, 0x00, 0x00, 0x00,
};


//-------------------------------------------

//__attribute__((always_inline))
inline void spiwrite(uint8_t c) 
{
    SPDR = c;                    // send the data
    while(!(SPSR & (1<<SPIF)));  // wait until transmission is complete  
}

//-------------------------------------------
// This ensures that the Data/Command pin status matches the byte
// that is actually being transmitted.
// The write command operation waits until all data has been
// sent, configures the Data/Command pin for commands, sends
// the command, and then waits for the transmission to
// finish.

void writecommand(uint16_t c) {
    CTRL_PORT &=~ TFT_DC;    
    CTRL_PORT &=~ TFT_CS;  
    spiwrite(c);
    CTRL_PORT |= TFT_CS;  

}

//-------------------------------------------

// The write data operation waits until there is room in the
// transmit FIFO, configures the Data/Command pin for data,
// and then adds the data to the transmit FIFO.

void writedata(uint8_t c) {
    CTRL_PORT |= TFT_DC;
    CTRL_PORT &=~ TFT_CS; //TFT_CS_LOW  
    spiwrite(c);
    CTRL_PORT |= TFT_CS; //TFT_CS HIGH

}

//-------------------------------------------

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in ROM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.

#define DELAY 0x80

static const uint8_t PROGMEM
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  //0xe1 - 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay
  //////////////////////////////////////////////////////////////////////////////
  Rcmd1[] = {                 //       Init for 7735R, part 1 (red or green tab)
    15,                       //       15 commands in list:
    // DC IS LOW FOR CMDS (ST7735_SWRESET, ETC) - FOR "ARGS" IT IS HIGH
    // ARGS HAVE VARRYING NUMBER OF BYTES 
    ST7735_SWRESET,   DELAY,  //0x01 - 1: Software reset, 0 args, w/delay
      150,                    //       150 ms delay
    ST7735_SLPOUT ,   DELAY,  //0x11 - 2: Out of sleep mode, 0 args, w/delay
      255,                    //       500 ms delay
    ST7735_FRMCTR1, 3      ,  //0xb1 - 3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //       Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //0xb2 - 4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //       Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //0xb3 - 5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //       Dot inversion mode
      0x01, 0x2C, 0x2D,       //       Line inversion mode
    ST7735_INVCTR , 1      ,  //0xb4 - 6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //       No inversion
    ST7735_PWCTR1 , 3      ,  //0xc0 - 7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //       -4.6V
      0x84,                   //       AUTO mode
    ST7735_PWCTR2 , 1      ,  //0xc1 - 8: Power control, 1 arg, no delay:
      0xC5,                   //       VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //0xc2 - 9: Power control, 2 args, no delay:
      0x0A,                   //       Opamp current small
      0x00,                   //       Boost frequency
    ST7735_PWCTR4 , 2      ,  //0xc3 - 10: Power control, 2 args, no delay:
      0x8A,                   //       BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  //0xc4 - 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  //0xc5 - 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  //0x20 - 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  //0x36 - 14: Memory access control (directions), 1 arg:
      0xC8,                   //           row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  //0x3a - 15: set color mode, 1 arg, no delay:
      0x05 },                 //           16-bit color
  //////////////////////////////////////////////////////////////////////////////
  Rcmd2green[] = {            //       Init for 7735R, part 2 (green tab only)
    2,                        //       2 commands in list:
    ST7735_CASET  , 4      ,  //       1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //          XSTART = 0
      0x00, 0x7F+0x02,        //          XEND = 127
    ST7735_RASET  , 4      ,  //       2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //          XSTART = 0
      0x00, 0x9F+0x01 },      //          XEND = 159
  Rcmd2red[] = {              //       Init for 7735R, part 2 (red tab only)
    2,                        //       2 commands in list:
    ST7735_CASET  , 4      ,  //0x2a-  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //           XSTART = 0
      0x00, 0x7F,             //           XEND = 127
    ST7735_RASET  , 4      ,  //0x2b-  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //           XSTART = 0
      0x00, 0x9F },           //           XEND = 159

  Rcmd2green144[] = {                  // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //       2 commands in list:
    ST7735_CASET  , 4      ,  //0x2a-  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //         XSTART = 0
      0x00, 0x7F,             //         XEND = 127
    ST7735_RASET  , 4      ,  //  2:   Row addr set, 4 args, no delay:
      0x00, 0x00,             //         XSTART = 0
      0x00, 0x7F },           //         XEND = 127

  //////////////////////////////////////////////////////////////////////////////

  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //0xe0-  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


//-------------------------------------------

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in ROM byte array.
void commandList(const uint8_t *addr) {
  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
        writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }

    if(ms) {
        ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
        if(ms == 255) ms = 500;     // If 255, delay for 500 ms
        _delay_ms(ms);
    }  
  }

}


//-------------------------------------------
/* 
     Initialization code common to both 'B' and 'R' type displays

     A majority of my headaches porting this from ARM32 were caused by bitdepth 
     conversions for variables 
     This code was ported from 32 bit ARM so it was bound to cause trouble all over
     Thank the good Lord for my Salea Logic analyzer. Best 150 dollars I ever spent
*/ 

void commonInit(const uint8_t *cmdList) {
  CTRL_PORT = 0x00; // I was setting DC low but Adafruit says CS low. Logic says ALL low.??

  // toggle RST low to reset; CS low so it'll listen to us
  CTRL_PORT |= TFT_RST; //RESET = RESET_HIGH;
  _delay_ms(500);  
  CTRL_PORT &=~ TFT_RST; //RESET = RESET_LOW;
  _delay_ms(500);  
  CTRL_PORT |= TFT_RST; //RESET = RESET_HIGH;
  _delay_ms(500);  

  if(cmdList) commandList(cmdList);

}

//------------ST7735_InitB------------
void ST7735_InitB(void) {
  commonInit(Bcmd);
  ST7735_SetCursor(0,0);
  StTextColor = ST7735_YELLOW;
  ST7735_FillScreen(0);                 // set screen to black
}


//------------ST7735_InitR------------
void ST7735_InitR(enum initRFlags option) {
    commonInit(Rcmd1);
    if(option == INITR_GREENTAB) {
        commandList(Rcmd2green);
        ColStart = 2;
        RowStart = 1;
    } else {
      // colstart, rowstart left at default '0' values
      commandList(Rcmd2red);
    }
    commandList(Rcmd3);

    // if black, change MADCTL color filter
    if (option == INITR_BLACKTAB) {
      writecommand(ST7735_MADCTL);
      writedata(0xC0);
    }
    TabColor = option;

    //NOT IN ADAFRUITS CODE! (BUT A GOOD IDEA)
    //ST7735_SetCursor(0,0);
    //StTextColor = ST7735_YELLOW;
    //ST7735_FillScreen(0);                 // set screen to black
}


//--------------------------------------

// Set the region of the screen RAM to be modified
// Pixel colors are sent left to right, top to bottom
// (same as Font table is encoded; different from regular bitmap)
// Requires 11 bytes of transmission
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writecommand(ST7735_CASET); // 0x2a -  Column addr set
    writedata(0x00);
    writedata(x0+ColStart);     // XSTART
    writedata(0x00);
    writedata(x1+ColStart);     // XEND

    writecommand(ST7735_RASET); // 0x2b - Row addr set
    writedata(0x00);
    writedata(y0+RowStart);     // YSTART
    writedata(0x00);
    writedata(y1+RowStart);     // YEND

    writecommand(ST7735_RAMWR); // 0x2c - write to RAM
}


//--------------------------------------

// Send two bytes of data, most significant byte first
// Requires 2 bytes of transmission
void pushColor(uint16_t color) {
    CTRL_PORT |= TFT_DC;  //DC LOW       keith was here
    CTRL_PORT &=~ TFT_CS; //TFT_CS_LOW   keith was here

    spiwrite(color >> 8);
    spiwrite(color);
    CTRL_PORT |= TFT_CS; //TFT_CS HIGH   keith was here    
}


//------------ST7735_DrawPixel------------
// Color the pixel at the given coordinates with the given color.
// Requires 13 bytes of transmission
// Input: x     horizontal position of the pixel, columns from the left edge
//               must be less than 128
//               0 is on the left, 126 is near the right
//        y     vertical position of the pixel, rows from the top edge
//               must be less than 160
//               159 is near the wires, 0 is the side opposite the wires
//        color 16-bit color, which can be produced by ST7735_Color565()
// Output: none
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x,y,x+1,y+1); //BUG??
    pushColor(color);
}


//------------ST7735_DrawFastVLine------------
// Draw a vertical line at the given coordinates with the given height and color.
// A vertical line is parallel to the longer side of the rectangular display
// Requires (11 + 2*h) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        h     vertical height of the line
//        color 16-bit color, which can be produced by ST7735_Color565()
// Output: none
void ST7735_DrawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color) {
  uint8_t hi = color >> 8, lo = color;

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  setAddrWindow(x, y, x, y+h-1);

  CTRL_PORT |= TFT_DC;  //DC LOW      
  CTRL_PORT &=~ TFT_CS; //TFT_CS_LOW   

  while (h--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  CTRL_PORT |= TFT_CS; //TFT_CS HIGH   
}


//------------ST7735_DrawFastHLine------------
// Draw a horizontal line at the given coordinates with the given width and color.
// A horizontal line is parallel to the shorter side of the rectangular display
// Requires (11 + 2*w) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the start of the line, columns from the left edge
//        y     vertical position of the start of the line, rows from the top edge
//        w     horizontal width of the line
//        color 16-bit color, which can be produced by ST7735_Color565()
// Output: none
void ST7735_DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  uint8_t hi = color >> 8, lo = color;

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  CTRL_PORT |= TFT_DC;  //DC LOW       
  CTRL_PORT &=~ TFT_CS; //TFT_CS_LOW   
  while (w--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  CTRL_PORT |= TFT_CS; //TFT_CS HIGH   
}


//------------ST7735_FillScreen------------
// Fill the screen with the given color.
// Requires 40,971 bytes of transmission
// Input: color 16-bit color, which can be produced by ST7735_Color565()
// Output: none
//  screen is actually 129 by 161 pixels, x 0 to 128, y goes from 0 to 160

void ST7735_FillScreen(uint16_t color) {
    //IF I PASS A VARIABLE TO THIS IT GETS SET TO WRONG BITDEPTH
    //RUNS TWICE AS SLOW AS EXPLICITLY TYPING NUMBERS  
    //ST7735_FillRect(0, 0, _width, _height, color);  // original
    ST7735_FillRect(0, 0, 128, 160, color);    

}


//------------ST7735_FillRect------------

//DEBUG HARDCODED VALUES NEED TO BE FIXED
//COLOR SHIFTING IS ALL FUQED UP AS WELL

// Draw a filled rectangle at the given coordinates with the given width, height, and color.
// Requires (11 + 2*w*h) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the top left corner of the rectangle, columns from the left edge
//        y     vertical position of the top left corner of the rectangle, rows from the top edge
//        w     horizontal width of the rectangle
//        h     vertical height of the rectangle
//        color 16-bit color, which can be produced by ST7735_Color565()
// Output: none
void ST7735_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  uint8_t hi = color >> 8, lo = color;

  //   rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  // setAddrWindow(x, y, x+w-1, y+h-1); <- JEBUS, I THINK THIS WAS CRASHING IT (254 instead of 127)
  setAddrWindow(x, y, _width-1, _height-1);  
 
  CTRL_PORT |= TFT_DC;  //keith was here
  CTRL_PORT &=~ TFT_CS; //TFT_CS_LOW  //keith was here

  uint8_t i = 0;
  uint8_t j = 0;

  for(i=h; i>0; i--) {
    for(j=w; j>0; j--) {
      spiwrite(hi);
      spiwrite(lo);
    }
  }

  CTRL_PORT |= TFT_CS;//high - keith was here  
}


//------------ST7735_Color565------------
// Pass 8-bit (each) R,G,B and get back 16-bit packed color.
// Input: r red value
//        g green value
//        b blue value
// Output: 16-bit color
uint16_t ST7735_Color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}


//------------ST7735_SwapColor------------
// Swaps the red and blue values of the given 16-bit packed color;
// green is unchanged.
// Input: x 16-bit color in format B, G, R
// Output: 16-bit color in format R, G, B
uint16_t ST7735_SwapColor(uint16_t x) {
    return (x << 11) | (x & 0x07E0) | (x >> 11);
}


//------------ST7735_DrawBitmap------------
// Displays a 16-bit color BMP image.  A bitmap file that is created
// by a PC image processing program has a header and may be padded
// with dummy columns so the data have four byte alignment.  This
// function assumes that all of that has been stripped out, and the
// array image[] has one 16-bit halfword for each pixel to be
// displayed on the screen (encoded in reverse order, which is
// standard for bitmap files).  An array can be created in this
// format from a 24-bit-per-pixel .bmp file using the associated
// converter program.
// (x,y) is the screen location of the lower left corner of BMP image
// Requires (11 + 2*w*h) bytes of transmission (assuming image fully on screen)
// Input: x     horizontal position of the bottom left corner of the image, columns from the left edge
//        y     vertical position of the bottom left corner of the image, rows from the top edge
//        image pointer to a 16-bit color BMP image
//        w     number of pixels wide
//        h     number of pixels tall
// Output: none
// Must be less than or equal to 128 pixels wide by 160 pixels high

//void ST7735_DrawBitmap(uint8_t x, uint8_t y, const uint16_t *image, uint8_t w, uint8_t h){
void ST7735_DrawBitmap(uint8_t x, uint8_t y, const uint16_t *image, uint8_t w, uint8_t h){  
  int8_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping
  int8_t originalWidth = w;              // save this value; even if not all columns fit on the screen, the image is still this width in ROM
  uint8_t i = w*(h - 1);

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

  for(y=0; y<h; y=y+1){
    for(x=0; x<w; x=x+1){
      // send the top 8 bits
      writedata((uint8_t)(image[i] >> 8));
       // send the bottom 8 bits
      writedata((uint8_t)image[i]);
      i = i + 1;// go to the next pixel
    }
    i = i + skipC;
    i = i - 2*originalWidth;
  }
}


//------------ST7735_DrawCharS------------
// Simple character draw function.  This is the same function from
// Adafruit_GFX.c but adapted for this processor.  However, each call
// to ST7735_DrawPixel() calls setAddrWindow(), which needs to send
// many extra data and commands.  If the background color is the same
// as the text color, no background will be printed, and text can be
// drawn right over existing images without covering them with a box.
// Requires (11 + 2*size*size)*6*8 (image fully on screen; textcolor != bgColor)
// Input: x         horizontal position of the top left corner of the character, columns from the left edge
//        y         vertical position of the top left corner of the character, rows from the top edge
//        c         character to be printed
//        textColor 16-bit color of the character
//        bgColor   16-bit color of the background
//        size      number of pixels per character pixel (e.g. size==2 prints each pixel of font as 2x2 square)
// Output: none
void ST7735_DrawCharS(uint8_t x, uint8_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size){
    const uint8_t *addr = Font;
    uint8_t line; // vertical column of pixels of character in font
    uint8_t i=0;
    uint8_t j=0;

    if((x >= _width)            || // Clip right
       (y >= _height)           || // Clip bottom
       ((x + 5 * size - 1) < 0) || // Clip left
       ((y + 8 * size - 1) < 0))   // Clip top
      return;

    for (i=0; i<6; i++ ) {
        if (i == 5)      //what the hell
            line = 0x0;  //is this crap for? blank space???
        else
            line = pgm_read_byte( (addr+(c*5)+i) );//like a framebuffer

        for (j = 0; j<8; j++) {
            if (line & 0x1) {
                if (size == 1) // default size
                    ST7735_DrawPixel(x+i, y+j, textColor);
                else {  // big size
                    ST7735_FillRect(x+(i*size), y+(j*size), size, size, textColor);
                }
            } else if (bgColor != textColor) {
                if (size == 1) // default size
                    ST7735_DrawPixel(x+i, y+j, bgColor);
                   
                else {  // big size
                    ST7735_FillRect(x+i*size, y+j*size, size, size, bgColor);
                }
            }
            line >>= 1;
        }
    }
}


//------------ST7735_DrawChar------------
// Advanced character draw function.  This is similar to the function
// from Adafruit_GFX.c but adapted for this processor.  However, this
// function only uses one call to setAddrWindow(), which allows it to
// run at least twice as fast.
// Requires (11 + size*size*6*8) bytes of transmission (assuming image fully on screen)
// Input: x         horizontal position of the top left corner of the character, columns from the left edge
//        y         vertical position of the top left corner of the character, rows from the top edge
//        c         character to be printed
//        textColor 16-bit color of the character
//        bgColor   16-bit color of the background
//        size      number of pixels per character pixel (e.g. size==2 prints each pixel of font as 2x2 square)
// Output: none
void ST7735_DrawChar(uint8_t x, uint8_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size){
    uint8_t line; // horizontal row of pixels of character
    int8_t col, row, i, j;// loop indices
    if(((x + 5*size - 1) >= _width)  || // Clip right
       ((y + 8*size - 1) >= _height) || // Clip bottom
       ((x + 5*size - 1) < 0)        || // Clip left
       ((y + 8*size - 1) < 0)){         // Clip top
      return;
    }

    setAddrWindow(x, y, x+6*size-1, y+8*size-1);

    line = 0x01;        // print the top row first
    // print the rows, starting at the top
    for(row=0; row<8; row=row+1){
      for(i=0; i<size; i=i+1){
        // print the columns, starting on the left
        for(col=0; col<5; col=col+1){
          if(Font[(c*5)+col]&line){
            // bit is set in Font, print pixel(s) in text color
            for(j=0; j<size; j=j+1){
              pushColor(textColor);
            }
          } else{
            // bit is cleared in Font, print pixel(s) in background color
            for(j=0; j<size; j=j+1){
              pushColor(bgColor);
            }
          }
        }
        // print blank column(s) to the right of character
        for(j=0; j<size; j=j+1){
          pushColor(bgColor);
        }
      }
      line = line<<1;   // move up to the next row
    }
}


//------------ST7735_DrawString------------
// String draw function.
// 16 rows (0 to 15) and 21 characters (0 to 20)
// Requires (11 + size*size*6*8) bytes of transmission for each character
// Input: x         columns from the left edge (0 to 20)
//        y         rows from the top edge (0 to 15)
//        pt        pointer to a null terminated string to be printed
//        textColor 16-bit color of the characters
// bgColor is Black and size is 1
// Output: number of characters printed

uint16_t ST7735_DrawString(uint8_t x, uint8_t y, char *pt, int16_t textColor){
    uint16_t count = 0;
    if(y>15) return 0;
    while(*pt){
        ST7735_DrawCharS(x*6, y*10, *pt, textColor, ST7735_BLACK, 1);
        pt++;
        x = x+1;
        if(x>20) return count;  // number of characters printed
        count++;
    }
    return count;  // number of characters printed
}

//-----------------------fillmessage-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
char Message[12];
uint16_t Messageindex;

void fillmessage(uint16_t n){
    // This function uses recursion to convert decimal number
    //   of unspecified length as an ASCII string
    if(n >= 10){
      fillmessage(n/10);
      n = n%10;
    }
    Message[Messageindex] = (n+'0'); /* n is between 0 and 9 */
    if(Messageindex<11)Messageindex++;
}


//----------ST7735_SetCursor------------

// Move the cursor to the desired X- and Y-position.  The
// next character will be printed here.  X=0 is the leftmost
// column.  Y=0 is the top row.
// inputs: newX  new X-position of the cursor (0<=newX<=20)
//         newY  new Y-position of the cursor (0<=newY<=15)
// outputs: none
void ST7735_SetCursor(uint16_t newX, uint16_t newY){
    if((newX > 20) || (newY > 15)){       // bad input
        return;                           // do nothing
    }
    StX = newX;
    StY = newY;
}




//------------ST7735_SetRotation------------
// Change the image rotation.
// Requires 2 bytes of transmission
// Input: m new rotation value (0 to 3)
// Output: none
void ST7735_SetRotation(uint8_t m) {

  writecommand(ST7735_MADCTL);
  Rotation = m % 4; // can't be higher than 3
  switch (Rotation) {
   case 0:
     if (TabColor == INITR_BLACKTAB) {
       writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
     } else {
       writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
     }
     _width  = ST7735_TFTWIDTH;
     _height = ST7735_TFTHEIGHT;
     break;
   case 1:
     if (TabColor == INITR_BLACKTAB) {
       writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
     } else {
       writedata(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
     }
     _width  = ST7735_TFTHEIGHT;
     _height = ST7735_TFTWIDTH;
     break;
  case 2:
     if (TabColor == INITR_BLACKTAB) {
       writedata(MADCTL_RGB);
     } else {
       writedata(MADCTL_BGR);
     }
     _width  = ST7735_TFTWIDTH;
     _height = ST7735_TFTHEIGHT;
    break;
   case 3:
     if (TabColor == INITR_BLACKTAB) {
       writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
     } else {
       writedata(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
     }
     _width  = ST7735_TFTHEIGHT;
     _height = ST7735_TFTWIDTH;
     break;
  }
}


//------------ST7735_InvertDisplay------------
// Send the command to invert all of the colors.
// Requires 1 byte of transmission
// Input: i 0 to disable inversion; non-zero to enable inversion
// Output: none
void ST7735_InvertDisplay(int i) {
    if(i){
        writecommand(ST7735_INVON);
    } else{
        writecommand(ST7735_INVOFF);
    }
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


// *************** ST7735_PlotClear ********************
// Clear the graphics buffer, set X coordinate to 0
// This routine clears the display
// Inputs: ymin and ymax are range of the plot
// Outputs: none
void ST7735_PlotClear(uint8_t ymin, uint8_t ymax){
  ST7735_FillRect(0, 32, 128, 128, ST7735_Color565(228,228,228)); // light grey
  if(ymax>ymin){
    Ymax = ymax;
    Ymin = ymin;
    Yrange = ymax-ymin;
  } else{
    Ymax = ymin;
    Ymin = ymax;
    Yrange = ymax-ymin;
  }
  //YrangeDiv2 = Yrange/2;
  X = 0;
}

// *************** ST7735_PlotPoint ********************
// Used in the voltage versus time plot, plot one point at y
// It does output to display
// Inputs: y is the y coordinate of the point plotted
// Outputs: none
void ST7735_PlotPoint(uint8_t y){uint8_t j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;
  // X goes from 0 to 127
  // j goes from 159 to 32
  // y=Ymax maps to j=32
  // y=Ymin maps to j=159
  j = 32+(127*(Ymax-y))/Yrange;
  if(j<32) j = 32;
  if(j>159) j = 159;
  ST7735_DrawPixel(X,   j,   ST7735_BLUE);
  ST7735_DrawPixel(X+1, j,   ST7735_BLUE);
  ST7735_DrawPixel(X,   j+1, ST7735_BLUE);
  ST7735_DrawPixel(X+1, j+1, ST7735_BLUE);
}


// *************** ST7735_PlotLine ********************
/*
// Used in the voltage versus time plot, plot line to new point
// It does output to display
// Inputs: y is the y coordinate of the point plotted
// Outputs: none
int8_t lastj=0;
void ST7735_PlotLine(int8_t y){int8_t i,j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;
  // X goes from 0 to 127
  // j goes from 159 to 32
  // y=Ymax maps to j=32
  // y=Ymin maps to j=159
  j = 32+(127*(Ymax-y))/Yrange;
  if(j < 32) j = 32;
  if(j > 159) j = 159;
  if(lastj < 32) lastj = j;
  if(lastj > 159) lastj = j;
  if(lastj < j){
    for(i = lastj+1; i<=j ; i++){
      ST7735_DrawPixel(X,   i,   ST7735_BLUE) ;
      ST7735_DrawPixel(X+1, i,   ST7735_BLUE) ;
    }
  }else if(lastj > j){
    for(i = j; i<lastj ; i++){
      ST7735_DrawPixel(X,   i,   ST7735_BLUE) ;
      ST7735_DrawPixel(X+1, i,   ST7735_BLUE) ;
    }
  }else{
    ST7735_DrawPixel(X,   j,   ST7735_BLUE) ;
    ST7735_DrawPixel(X+1, j,   ST7735_BLUE) ;
  }
  lastj = j;
}
*/

// *************** ST7735_PlotPoints ********************
/*
// Used in the voltage versus time plot, plot two points at y1, y2
// It does output to display
// Inputs: y1 is the y coordinate of the first point plotted
//         y2 is the y coordinate of the second point plotted
// Outputs: none
void ST7735_PlotPoints(int8_t y1,int8_t y2){
  int8_t j;
  if(y1<Ymin) y1=Ymin;
  if(y1>Ymax) y1=Ymax;
  // X goes from 0 to 127
  // j goes from 159 to 32
  // y=Ymax maps to j=32
  // y=Ymin maps to j=159
  j = 32+(127*(Ymax-y1))/Yrange;
  if(j<32) j = 32;
  if(j>159) j = 159;
  ST7735_DrawPixel(X, j, ST7735_BLUE);
  if(y2<Ymin) y2=Ymin;
  if(y2>Ymax) y2=Ymax;
  j = 32+(127*(Ymax-y2))/Yrange;
  if(j<32) j = 32;
  if(j>159) j = 159;
  ST7735_DrawPixel(X, j, ST7735_BLACK);
}
*/

// *************** ST7735_PlotBar ********************
// Used in the voltage versus time bar, plot one bar at y
// It does not output to display until RIT128x96x4ShowPlot called
// Inputs: y is the y coordinate of the bar plotted
// Outputs: none
/*
void ST7735_PlotBar(int8_t y){
  int8_t j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;
  // X goes from 0 to 127
  // j goes from 159 to 32
  // y=Ymax maps to j=32
  // y=Ymin maps to j=159
  j = 32+(127*(Ymax-y))/Yrange;
  ST7735_DrawFastVLine(X, j, 159-j, ST7735_BLACK);

}


// full scaled defined as 3V
// Input is 0 to 511, 0 => 159 and 511 => 32
uint8_t const dBfs[512]={
159, 159, 145, 137, 131, 126, 123, 119, 117, 114, 112, 110, 108, 107, 105, 104, 103, 101,
  100, 99, 98, 97, 96, 95, 94, 93, 93, 92, 91, 90, 90, 89, 88, 88, 87, 87, 86, 85, 85, 84,
  84, 83, 83, 82, 82, 81, 81, 81, 80, 80, 79, 79, 79, 78, 78, 77, 77, 77, 76, 76, 76, 75,
  75, 75, 74, 74, 74, 73, 73, 73, 72, 72, 72, 72, 71, 71, 71, 71, 70, 70, 70, 70, 69, 69,
  69, 69, 68, 68, 68, 68, 67, 67, 67, 67, 66, 66, 66, 66, 66, 65, 65, 65, 65, 65, 64, 64,
  64, 64, 64, 63, 63, 63, 63, 63, 63, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 60,
  60, 60, 60, 60, 60, 59, 59, 59, 59, 59, 59, 59, 58, 58, 58, 58, 58, 58, 58, 57, 57, 57,
  57, 57, 57, 57, 56, 56, 56, 56, 56, 56, 56, 56, 55, 55, 55, 55, 55, 55, 55, 55, 54, 54,
  54, 54, 54, 54, 54, 54, 53, 53, 53, 53, 53, 53, 53, 53, 53, 52, 52, 52, 52, 52, 52, 52,
  52, 52, 52, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 50, 50, 50, 50, 50, 50, 50, 50, 50,
  50, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
  48, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 46, 46, 46, 46, 46, 46, 46, 46, 46,
  46, 46, 46, 46, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 44, 44, 44, 44, 44,
  44, 44, 44, 44, 44, 44, 44, 44, 44, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
  43, 43, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 41, 41, 41, 41, 41,
  41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
  40, 40, 40, 40, 40, 40, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
  39, 39, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 37,
  37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 36, 36, 36, 36,
  36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 35, 35, 35, 35, 35,
  35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 34, 34, 34, 34, 34, 34,
  34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 33, 33, 33, 33, 33,
  33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 32,
  32, 32, 32, 32, 32, 32, 32, 32, 32, 32
};
*/


// *************** ST7735_PlotdBfs ********************
/*
// Used in the amplitude versus frequency plot, plot bar point at y
// 0 to 0.625V scaled on a log plot from min to max
// It does output to display
// Inputs: y is the y ADC value of the bar plotted
// Outputs: none
void ST7735_PlotdBfs(int8_t y){
  int8_t j;
  y = y/2; // 0 to 2047
  if(y<0) y=0;
  if(y>511) y=511;
  // X goes from 0 to 127
  // j goes from 159 to 32
  // y=511 maps to j=32
  // y=0 maps to j=159
  j = dBfs[y];
  ST7735_DrawFastVLine(X, j, 159-j, ST7735_BLACK);

}
*/

// *************** ST7735_PlotNext ********************
// Used in all the plots to step the X coordinate one pixel
// X steps from 0 to 127, then back to 0 again
// It does not output to display
// Inputs: none
// Outputs: none
void ST7735_PlotNext(void){
    if(X==127){
        X = 0;
    } else{
        X++;
    }
}

// *************** ST7735_PlotNextErase ********************
// Used in all the plots to step the X coordinate one pixel
// X steps from 0 to 127, then back to 0 again
// It clears the vertical space into which the next pixel will be drawn
// Inputs: none
// Outputs: none
void ST7735_PlotNextErase(void){
    if(X==127){
        X = 0;
    } else{
        X++;
    }
    ST7735_DrawFastVLine(X,32,128,ST7735_Color565(228,228,228));
}

// Used in all the plots to write buffer to LCD
// Example 1 Voltage versus time
//    ST7735_PlotClear(0,4095);  // range from 0 to 4095
//    ST7735_PlotPoint(data); ST7735_PlotNext(); // called 128 times

// Example 2a Voltage versus time (N data points/pixel, time scale)
//    ST7735_PlotClear(0,4095);  // range from 0 to 4095
//    {   for(j=0;j<N;j++){
//          ST7735_PlotPoint(data[i++]); // called N times
//        }
//        ST7735_PlotNext();
//    }   // called 128 times

// Example 2b Voltage versus time (N data points/pixel, time scale)
//    ST7735_PlotClear(0,4095);  // range from 0 to 4095
//    {   for(j=0;j<N;j++){
//          ST7735_PlotLine(data[i++]); // called N times
//        }
//        ST7735_PlotNext();
//    }   // called 128 times

// Example 3 Voltage versus frequency (512 points)
//    perform FFT to get 512 magnitudes, mag[i] (0 to 4095)
//    ST7735_PlotClear(0,1023);  // clip large magnitudes
//    {
//        ST7735_PlotBar(mag[i++]); // called 4 times
//        ST7735_PlotBar(mag[i++]);
//        ST7735_PlotBar(mag[i++]);
//        ST7735_PlotBar(mag[i++]);
//        ST7735_PlotNext();
//    }   // called 128 times

// Example 4 Voltage versus frequency (512 points), dB scale
//    perform FFT to get 512 magnitudes, mag[i] (0 to 4095)
//    ST7735_PlotClear(0,511);  // parameters ignored
//    {
//        ST7735_PlotdBfs(mag[i++]); // called 4 times
//        ST7735_PlotdBfs(mag[i++]);
//        ST7735_PlotdBfs(mag[i++]);
//        ST7735_PlotdBfs(mag[i++]);
//        ST7735_PlotNext();
//    }   // called 128 times

// *************** ST7735_OutChar ********************
// Output one character to the LCD
// Position determined by ST7735_SetCursor command
// Color set by ST7735_SetTextColor
// Inputs: 8-bit ASCII character
// Outputs: none

void ST7735_OutChar(char ch){
  if((ch == 10) || (ch == 13) || (ch == 27)){
    StY++; StX=0;
    if(StY>15){
      StY = 0;
    }
    ST7735_DrawString(0,StY,"                     ",StTextColor);
    return;
  }
  ST7735_DrawCharS(StX*6,StY*10,ch,ST7735_YELLOW,ST7735_BLACK, 1);
  StX++;
  if(StX>20){
    StX = 20;
    ST7735_DrawCharS(StX*6,StY*10,'*',ST7735_RED,ST7735_BLACK, 1);
  }
  return;
}


//********ST7735_OutString*****************
// Print a string of characters to the ST7735 LCD.
// Position determined by ST7735_SetCursor command
// Color set by ST7735_SetTextColor
// The string will not automatically wrap.
// inputs: ptr  pointer to NULL-terminated ASCII string
// outputs: none
/*
void ST7735_OutString(char *ptr){
  while(*ptr){
    ST7735_OutChar(*ptr);
    ptr = ptr + 1;
  }
}
*/

// ************** ST7735_SetTextColor ************************
// Sets the color in which the characters will be printed
// Background color is fixed at black
// Input:  16-bit packed color
// Output: none
// ********************************************************
void ST7735_SetTextColor(uint16_t color){
    StTextColor = color;
}

// ********************************************************
/*
// Print a character to ST7735 LCD.
int fputc(int ch, FILE *f){
  ST7735_OutChar(ch);
  return 1;
}
*/

// ********************************************************

// No input from Nokia, always return data.
// int fgetc (FILE *f){
//   return 0;
// }

// Function called when file error occurs.
// int ferror(FILE *f){
//   /* Your implementation of ferror */
//   return EOF;
// }

// Abstraction of general output device
// Volume 2 section 3.4.5


// set the color for future output
// Background color is fixed at black
// Input:  16-bit packed color
// Output: none
void Output_Color(uint16_t newColor){ // Set color of future output
    ST7735_SetTextColor(newColor);
}






/*
//-----------------------ST7735_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Position determined by ST7735_SetCursor command
// Color set by ST7735_SetTextColor
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void ST7735_OutUDec(uint32_t n){
  Messageindex = 0;
  fillmessage(n);
  Message[Messageindex] = 0; // terminate
  ST7735_DrawString(StX,StY,Message,StTextColor);
  StX = StX+Messageindex;
  if(StX>20){
    StX = 20;
    ST7735_DrawCharS(StX*6,StY*10,'*',ST7735_RED,ST7735_BLACK, 1);
  }
}
*/


