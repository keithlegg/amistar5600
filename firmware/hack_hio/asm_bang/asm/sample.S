#include <avr/io.h>

#define __SFR_OFFSET 0  // keep io.h from adding an incorrect offset to our port addresses


#define low(x)   ((x) & 0xFF)
#define high(x)   (((x)>>8) & 0xFF)




#define temp         r16
#define stack        r17
#define overflows    r18
#define milliseconds r19
#define analyzer     r20
#define press        r21
#define buttonH      r22

.org 0x0000
  rjmp Reset
.org 0x0002
  rjmp Keypress_int ; the INT0 external interrupt
.org 0x0020
  rjmp timer_overflow_int
.org 0x002A
  rjmp ADC_int      ; The ADC interrupt



Reset:  

   ldi temp,0b00000011
   out TCCR0B,temp     ; TCNT0 in FCPU/64 mode, 250000 cnts/sec
   ldi temp,249
   out OCR0A, temp     ; top of counter at 250 counts/overflow
                       ;   so overflow occurs every 1/1000 sec
                       ;   this means an overflow every 1ms
   ldi temp,0b00000010 ; 
   out TCCR0A,temp     ; reset clock at top of OCR0A
   sts TIMSK0, temp    ; Enable Timer Overflow Interrupts

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Power Reduction Register PRR
;  PRR=[PRTWI,PRTIM2,PRTIM0,-,PRTIM1,PRSPI,PRUSART0,PRADC] page 42
;  PRADC = 0 in PRR enables the ADC
;;;;;;;;;;;;;;;;;;;;;;;;;;

  lds temp,PRR           ; copy contents of Power Reduction Register
  andi temp,0b11111110   ; logical AND clears bit 0, others unchanged
  sts PRR,temp           ; now the PRADC bit is off which enables ADC

;;;;;;;;;;;;;;;;;;;;;;;;;;
; ADC Multiplexer Selection Register
;  ADMUX=[REFS1,REFS0,ADLAR,-,MUX3,MUX2,MUX1,MUX0] page 248
;  REFS1 = 0, 
;  REFS0 = 1, AVCC is our reference voltage 
;  ADLAR = 1, left adjust the result
;  MUX3:0 = 0, ADC0 is our input pin (which is PC0)
;;;;;;;;;;;;;;;;;;;;;;;;;;

  ldi temp,0b01100000
  sts ADMUX,temp

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Digital Input Disable Register
;  DIDR0 = [-,-,ADC5D,ADC4D,ADC3D,ADC2D,ADC1D,ADC0D] page 251
;  ADC0D = 1 
;  in the DIDR0 register which disables digital input on PC0
;  we use OR so that it doesn't change any of the other bits
;;;;;;;;;;;;;;;;;;;;;;;;;; 

  lds temp,DIDR0         ; load Digital Input Disable register 0
  ori temp,(1<<ADC0D)    ; turn off digital input on PC0
  sts DIDR0,temp         ; and leave the rest alone

;;;;;;;;;;;;;;;;;;;;;;;;;;
; External Interrupt Control Register A
;  EICRA=[-,-,-,-,ISC11,ISC10,ISC01,ISC00] page 71
;  ISC01 = 1
;  ISC00 = 1 
;  to indicate we trigger on a rising edge in to INT0.
;;;;;;;;;;;;;;;;;;;;;;;;;;

  ldi temp,(1<<ISC01)|(1<<ISC00)
  sts EICRA,temp

;;;;;;;;;;;;;;;;;;;;;;;;;;
; External Interrupt Mask Register
;  EIMSK=[-,-,-,-,-,-,INT1,INT0] page 72
;  INT1 = 0
;  INT0 = 1 
;  to indicate we are using the interrupt
;  on the INT0 pin at interrupt address 0x0002
;;;;;;;;;;;;;;;;;;;;;;;;;;

  ldi temp,0b00000001
  out EIMSK,temp

;;;;;;;;;;;;;;;;;;;;;;;;;;
; ADC Control and Status Register A
;  ADCSRA=[ADEN,ADSC,ADATE,ADIF,ADIE,ADPS2,ADPS1,ADPS0] page 249
;  ADEN = 1, enables the ADC
;  ADSC = 0, don't start a conversion yet
;  ADATE = 0, disable autotrigger
;  ADIF = 0, this will be set by hardware when a conversion completes
;  ADIE = 1, enable the ADC Conversion Complete interrupt
;  ADPS2:0 = 111, prescaler factor of 128 between CPU clock and ADC clock
;;;;;;;;;;;;;;;;;;;;;;;;;; 

  ldi temp,0b10001111
  sts ADCSRA,temp

;;;;;;;;;;;;;;;;;;;;;;;;;; 
; ADC Control and Status Register B
;  ADCSRB=[-,ACME,-,-,-,ADTS2,ADTS1,ADTS0] page 251
;  This register has no effect since we have 
;  disabled ADATE
;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set the SREG I bit to
; turn on global interrupts
;;;;;;;;;;;;;;;;;;;;;;;;;;

  sei

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Initialize the stack pointer
;;;;;;;;;;;;;;;;;;;;;;;;;;

  ldi temp,low(RAMEND)
  out SPL,temp
  ldi temp,high(RAMEND)
  out SPL,temp

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Initialize the ports
;;;;;;;;;;;;;;;;;;;;;;;;;;

  ser temp
  out DDRB,temp          ; set PortB to output
  ldi temp, 0b11111110
  out DDRC,temp          ; set PortC to output, ADC0 input
  clr temp
  out PortB,temp         ; set PortB to 0V
  out PortC,temp         ; set PortC to 0V
  cbi DDRD,2             ; PD2 is input
  cbi PortD,2            ; PD2 is at 0V
  clr analyzer
  ser press

main:
  cpi press,0
  brne wait          ; if no button press just wait
  //delay 20           ; delay to allow button to debounce
  lds temp,ADCSRA
  ori temp,(1<<ADSC)
  sts ADCSRA,temp    ; call a single ADC conversion
  rcall analyze      ; display analyzer value
  ser press          ; reset press
  wait:
rjmp main

analyze:
  clr temp
  out portb,temp
  out portc,temp
  sbrc analyzer,7
  sbi portb,1
  sbrc analyzer,6
  sbi portb,2
  sbrc analyzer,5
  sbi portb,3
  sbrc analyzer,4
  sbi portb,4
  sbrc analyzer,3
  sbi portc,1
  sbrc analyzer,2
  sbi portc,2
  sbrc analyzer,1
  sbi portc,3
  sbrc analyzer,0
  sbi portc,4
ret

ADC_int:
  lds buttonH,ADCH

  clc
  cpi buttonH,239
  brlo PC+3          ; if ADCH is bigger then it's a 1
  ldi analyzer,1     ; so load analyzer with a 1
	rjmp return

  clc
  cpi buttonH,230    ; if ADCH is bigger then a 2
  brlo PC+3
  ldi analyzer,2
  rjmp return

  clc
  cpi buttonH,217
  brlo PC+3
  ldi analyzer,3
  rjmp return

  clc
  cpi buttonH,203
  brlo PC+3
  ldi analyzer,4
  rjmp return

  clc
  cpi buttonH,187
  brlo PC+3
  ldi analyzer,5
  rjmp return

  clc
  cpi buttonH,155
  brlo PC+3
  ldi analyzer,6
  rjmp return

  clc
  cpi buttonH,126
  brlo PC+3
  ldi analyzer,255    ; we will set flash as all on
  rjmp return

  clc
  cpi buttonH,115
  brlo PC+3
  ldi analyzer,7
  rjmp return

  clc
  cpi buttonH,94
  brlo PC+3
  ldi analyzer,8
  rjmp return

  clc
  cpi buttonH,62
  brlo PC+3
  ldi analyzer,9
  rjmp return

  clc
  cpi buttonH,37
  brlo PC+3
  ldi analyzer,0b11110000 ; asterisk is top half on
  rjmp return

  clc
  cpi buttonH,28
  brlo PC+3
  ldi analyzer,0
  rjmp return

  clc
  cpi buttonH,17
  brlo PC+3
  ldi analyzer,0b00001111 ; hash sign is bottom half on
  rjmp return

  clc
  cpi buttonH,2
  brlo PC+3
  ldi analyzer,0b11000011 ; redial is top 2 bottom 2

  return:
reti

Keypress_int:
  clr press     ; clr press to indicate button press 
reti

timer_overflow_int: 
  inc overflows   ; increment 1000 times/sec
reti

