/*
 * MEGA_timerCounter_PWM.c
 *
 * Created: 20.6.2019 10:35:21
 * Author : Glutex
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

/* timer/counter1 compare match A interrupt vector */
ISR 
(TIMER3_COMPA_vect)
{
    TCNT3 = 0; // reset timer counter
}

int 
main(void)
{
    
    /* set up the ports and pins */
    DDRE |= (1 << PE3); // OC3A is located in digital pin 5
    
    // Enable interrupts command
    sei();
        
    /* set up the 16-bit timer/counter3, mode 9 */
    TCCR3B = 0; // reset timer/counter 3
    TCNT3  = 0;
    TCCR3A |= (1 << 6); // set compare output mode to toggle
    // TCCR3A |= 0b01000000;
    // TCCR3A |= 0x40;
        
    // mode 9 phase correct
    TCCR3A |= (1 << 0); // set register A WGM[1:0] bits
    //TCCR3A |= 0x01;
    // TCCR3A |= 0b00000001;
    TCCR3B |= (1 << 4); // set register B WBM[3:2] bits
    // TCCR3B |= 0b00010000;
        
    TIMSK3 |= (1 << 1); // enable compare match A interrupt
    // TIMSK3 |= 0x02;
    // TIMSK3 |= 0b00000010;
        
    OCR3A = 15296; //  C5 523 Hz, no prescaler
	//OCR3A = 2462;   // A7 3250 Hz, no prescaler, calculated
	//OCR3A = 2443;   // A7 3250 Hz, no prescaler, empirical
    //OCR3A = 1016;  //  B2  123 Hz, 64 prescaler

    while (1) 
    {
            /* enable timer/counter1 */
            TCCR3B |= (1 << 0); // set prescaling to 1 (no prescaling)
            // TCCR3B |= 0b00000001;
            
            // TCCR3B |= (1 << 0) | (1 << 1); // 64 prescaler
            // TCCR3B |= 0b00000011;

    }
    
    return 0;
}

