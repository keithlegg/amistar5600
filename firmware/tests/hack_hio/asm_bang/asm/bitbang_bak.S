#include <avr/io.h>

#define __SFR_OFFSET 0	// keep io.h from adding an incorrect offset to our port addresses



.global main

;.equ iVal, 39998     ; inner loop value


//-----------------------

main:
    ldi r17, 0xff
    out DDRD,r17
    rcall Start

Start:  
    rjmp myloop



;predelay:
;    ;push r16
;    ldi r16, 255
;    ret
delay:
    dec r16
    brne delay
    ;pop r16
    ret 

 


myloop:  
    
    pulse_out

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


