
#define F_CPU 16000000L // Define software reference clock for delay duration
#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/io.h>
#include <util/delay.h>


/*******************************/

/*

    based on differential pulse in servokit 

    wiring to DYN4:

   DB25 Pin 5  - DYN4 COM - (Interface Type B)
   DB25 Pin 9  - DYN4 GND 
   DB25 Pin 10 - DYN4 DIR-
   DB25 Pin 11 - DYN4 PUL+
   -- --
   DB25 Pin 15 - ENA  -  (24V HIGH)   
   DB25 Pin 16 - DYN4 -  +24V PWR    
   //DB25 Pin 18 - ALM - DYN4 OUT 24V
   -- -- 

   DB25 Pin 22 - DYN4 DIR+
   DB25 Pin 23 - DYN4 PUL-


   OUTPUTS:
    
       differential pulse, direction, enable  
       direction changes in a loop, enable ON/OFF between cycles 


   HARDWARE PINOUT:

       DDRB |= (1 << 5);  //PB5 arduino led

       sbi(DDRB, 3 );     //PB5 arduino pin 11
       DDRB =0b00001000;  //PB3 arduino pin 11 
       DDRB |= (1 << 3);  //PB3 arduino pin 11

*/   



/*******************************/



#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))


#define MOTOR_ENABLE_PORT PORTD
#define MOTOR_ENABLE_PIN_P 1
#define MOTOR_ENABLE_PIN_N 0

#define MOTOR_PULSE_PORT PORTD
#define MOTOR_PULSE_PIN_P 3
#define MOTOR_PULSE_PIN_N 2

#define MOTOR_DIR_PORT PORTD
#define MOTOR_DIR_PIN_P 5
#define MOTOR_DIR_PIN_N 4


/*******************************/
void shutdown(void)
{
    MOTOR_ENABLE_PORT = 0x00;
    MOTOR_PULSE_PORT  = 0x00;
    MOTOR_DIR_PORT    = 0x00;    
}


/*******************************/
void enable_motor(void)
{
    sbi(MOTOR_ENABLE_PORT, MOTOR_ENABLE_PIN_P );
    cbi(MOTOR_ENABLE_PORT, MOTOR_ENABLE_PIN_N );
}


/*******************************/
void disable_motor(void)
{
    shutdown();

    cbi(MOTOR_ENABLE_PORT, MOTOR_ENABLE_PIN_P );
    sbi(MOTOR_ENABLE_PORT, MOTOR_ENABLE_PIN_N );
} 

/*******************************/
void set_dir(uint8_t dir)
{
    if(dir==0){
        sbi(MOTOR_DIR_PORT, MOTOR_DIR_PIN_P );
        cbi(MOTOR_DIR_PORT, MOTOR_DIR_PIN_N );
    }else{
        cbi(MOTOR_DIR_PORT, MOTOR_DIR_PIN_P );
        sbi(MOTOR_DIR_PORT, MOTOR_DIR_PIN_N );
    }

}

/*******************************/

void gen_pulses(uint16_t num, uint16_t del)
{
    uint8_t usedelay = 1;

    for(uint16_t c=0;c<num;c++)
    {

        sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_P );
        cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_N );
        if(usedelay) _delay_us(del);

        cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_P );
        sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_N );
        if(usedelay) _delay_us(del); 
    } 


}




/*******************************/
int main (void)
{
   
    //DDRB =0b00001000;  //arduino pin 11 
    //DDRB |= (1 << 3);  //arduino pin 11

    //DDRD = 0xff;        
    //DDRD |= (0x4);  
    

    DDRD  = 0xff;
    PORTD = 0x00;

    //shutdown();
    //enable_motor();

    while (1)
    {
        //set_dir(0);         
        //gen_pulses(2000, 100);
         
        //PD3,4,5,6 

         PORTD = 0b00001000;
         _delay_us(50);
         
         PORTD = 0b00000100;
         _delay_us(50);


        /*
        disable_motor();
        _delay_ms(1000);

        enable_motor();
    
        set_dir(1);         
        gen_pulses(2000, 100);
        _delay_ms(1000);
        */
    }

} 

/*******************************/



