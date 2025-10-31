#include <avr/io.h>

#define __SFR_OFFSET 0  // keep io.h from adding an incorrect offset to our port addresses


#define low(x)   ((x) & 0xFF)
#define high(x)   (((x)>>8) & 0xFF)


.global main

;.equ iVal, 39998     ; inner loop value


#define ledpin  0x10

#define temp         r16
#define stack        r17
#define overflows    r18


.org 0x0000
    rjmp reset
.org 0x0002
    rjmp foo ; the INT0 external interrupt

//.org 0x0004
//    rjmp pulse_out ; the INT1 external interrupt



/*
    //sts VS out 
    //https://www.avrfreaks.net/s/topic/a5C3l000000U9upEAC/t051913

    .macro InReg 
        .if @1 < 0x40 
            in @0, @1 
        .elif ((@1 >= 0x60) && (@1 < SRAM_START)) 
            lds @0,@1 
        .else 
           .error  "InReg: Invalid I/O register address " 
        .endif 
    .endmacro 

    ; usage: OutReg addr, reg 
    .macro OutReg 
        .if @0 < 0x40 
            out @0, @1 
        .elif ((@0 >= 0x60) && (@0 < SRAM_START)) 
            sts @0,@1 
        .else 
           .error  "OutReg: Invalid I/O register address " 
        .endif 
    .endmacro 
*/


main:
    ;set data direction register 
    ldi r17, 0b11110000   
    out DDRD,r17
    
    //set pull ups on input pins 
    ldi r17, 0b00001111  
    out PORTD,r17


    rcall start


reset:  

    ldi temp, low(RAMEND)
    out SPL, temp
    ldi temp, high(RAMEND)
    out SPL, temp

    rjmp start

start:  

    //-----------------------------------
    //  Init stack ( interrupt return adress)
    //-----------------------------------  
    
  
   

   


    //-----------------------------------  
    
    /*
    // Digital Input Disable Register
    ; DIDR0 = [-,-,ADC5D,ADC4D,ADC3D,ADC2D,ADC1D,ADC0D] page 251
    ; ADC0D = 1 
    ; in the DIDR0 register which disables digital input on PC0
    ; we use OR so that it doesn't change any of the other bits
    // lds temp, DIDR0         ; load Digital Input Disable register 0
    // ori temp, (1<<ADC0D)    ; turn off digital input on PC0
    // sts DIDR0,temp          ; and leave the rest alone    
    
    //-----------------------------------  
    ; External Interrupt Control Register A
    ;  EICRA=[-,-,-,-,ISC11,ISC10,ISC01,ISC00] page 71
    ;  ISC01 = 1
    ;  ISC00 = 1 
    ;  to indicate we trigger on a rising edge in to INT0.
    ldi temp, (1<<ISC01)|(1<<ISC00)
    sts EICRA, temp
    //----------------------------------- 
    ; External Interrupt Mask Register
    ;  EIMSK=[-,-,-,-,-,-,INT1,INT0] page 72
    ;  INT1 = 0
    ;  INT0 = 1 
    ;  to indicate we are using the interrupt
    ;  on the INT0 pin at interrupt address 0x0002
    ldi temp, 0b00000001
    out EIMSK,temp
    */

    
    /*
    //PCMSK0 |= (1<<PCINT0);
    ldi temp,(1<<PCINT0)
    sts EICRA,temp
    //PCICR |= (1<<PCIE0);
    ldi temp,(1<<PCIE0)
    sts EICRA,temp
    */

  
    // // ldi temp, (1<<PCINT0)    
    // // sts PCMSK0, temp
    // // ldi temp, (1<<PCIE0)   
    // // sts PCICR, temp
    // setup interrupts for reading two quadrature lines  
    ldi temp, 1<<ISC01             // Interrupt 0 Sense Control
    sts EICRA, temp
    // Interrupt 1 Sense Control
    ldi temp, 1<<ISC11               // trigger on falling edge
    sts EICRA, temp
    // External Interrupt Mask Register
    ldi temp, (1 << INT0)|(1 << INT1);   // Turns on INT0 and INT1
    sts EIMSK, temp
    


    ;enable interrupts
    sei

    rjmp myloop





;load value into r16 before running this 
delay:
    dec temp
    brne delay
    ;pop r16
    ret 

myloop:  
    //rcall pulse_out
    //nop
    rjmp foo 
    rjmp   myloop    

foo:
    ldi r17, ledpin
    out PORTD,r17  
    ret



/*
pulse_out:
    ;initial long pulse 
    ldi r17, ledpin

    out PORTD,r17
    ;set the delay value 
    ldi r16, 2
    rcall delay  
    ldi r17,0x00
    out PORTD,r17     
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17

    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17

    ;2 bits of address 
    ldi r17,ledpin
    out PORTD,r17
    ldi r17,ledpin
    out PORTD,r17
    ;2 bits of address 
    ldi r17,ledpin
    out PORTD,r17

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17        

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17   

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17        

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17        
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  
   
    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17        
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17   
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17        
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17        
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17        
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, ledpin
    out PORTD,r17  
    ldi r17, ledpin
    out PORTD,r17  
             
    ;####################

    ;end on a low 
    ldi r17, 0x00
    out PORTD,r17  

    ;final delay between data frames
    ;set the delay value 
    ;ldi r16, 24
    ;rcall delay  
    ret
*/
