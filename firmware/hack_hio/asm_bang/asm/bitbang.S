#include <avr/io.h>


#define __SFR_OFFSET 0  // keep io.h from adding an incorrect offset to our port addresses



/*
#define temp         = r16
#define stack        = r17
#define overflows    = r18
#define milliseconds = r19
#define analyzer     = r20
#define press        = r21
#define buttonH      = r22
*/


#define low(x)   ((x) & 0xFF)
#define high(x)   (((x)>>8) & 0xFF)


.global main

;.equ iVal, 39998     ; inner loop value


#define temp         r16
#define stack        r17
#define overflows    r18
#define milliseconds r19
#define analyzer     r20
#define press        r21
#define buttonH      r22


.org 0x0000
    rjmp reset
.org 0x0002
    rjmp pulse_out ; the INT0 external interrupt


main:
    ;set data direction register 
    ldi r17, 0xff
    out DDRD,r17
    rcall start


reset:  
    rjmp myloop

start:  

    //-----------------------------------
    //  Init stack ( interrupt return adress)
    //-----------------------------------  
   
    ldi temp, low(RAMEND)
    out SPL, temp
    ldi temp, high(RAMEND)
    out SPL, temp

    //-----------------------------------  

    // Digital Input Disable Register
    // DIDR0 = [-,-,ADC5D,ADC4D,ADC3D,ADC2D,ADC1D,ADC0D] page 251
    // ADC0D = 1 
    // in the DIDR0 register which disables digital input on PC0
    // we use OR so that it doesn't change any of the other bits
    lds temp,DIDR0         ; load Digital Input Disable register 0
    ori temp,(1<<ADC0D)    ; turn off digital input on PC0
    sts DIDR0,temp         ; and leave the rest alone    

    // setup interrupts for reading two quadrature lines  
    ldi temp, (1<<ISC01) | (1<<ISC01)    // Interrupt 0 Sense Control
    sts EICRA, temp

    // Interrupt 1 Sense Control
    ldi temp, 1<<ISC11                   // trigger on falling edge
    sts EICRA, temp

    // External Interrupt Mask Register
    ldi temp, (1 << INT0)|(1 << INT1);   // Turns on INT0 and INT1
    sts EIMSK, temp


    ;enable interrupts
    SEI
 

    rjmp myloop



;load value into r16 before running this 
delay:
    dec temp
    brne delay
    ;pop r16
    ret 





myloop:  
    rcall pulse_out
    rjmp   myloop    



pulse_out:

    ;initial long pulse 
    ldi r17, 0x04

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
    ldi r17, 0x04
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
    ldi r17,0x04
    out PORTD,r17
    ldi r17,0x04
    out PORTD,r17
    ;2 bits of address 
    ldi r17,0x04
    out PORTD,r17


    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17        


    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17   


    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17        


    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17        
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  
   
    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17        
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  


    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17   
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17        
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17        
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17        
    ldi r17, 0x04
    out PORTD,r17  


    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  


    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
    out PORTD,r17  

    ;data packet 
    ldi r17,0x00
    out PORTD,r17
    ldi r17,0x00
    out PORTD,r17    
    ldi r17, 0x04
    out PORTD,r17  
    ldi r17, 0x04
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


