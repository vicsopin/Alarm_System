/*
 * UNO_timerCounter_PWM.c
 *
 * Created: 20.6.2019 10:42:51
 * Author : Glutex
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

/* functions for the timer/counter1 compare match A interrupt vector */
ISR
(TIMER1_COMPA_vect)
{
    TCNT1 = 0; // reset timer counter
}

int 
main(void)
{
    /* set up the ports and pins */
    DDRB |= (1 << PB1); // OC1A is located in digital pin 9
    
    // Enable interrupts
    sei();
    
    /* set up the 16-bit timer/counter1, mode 9 used */
    TCCR1B  = 0; // reset timer/counter 1
    TCNT1   = 0;
    TCCR1A |= (1 << 6); // set compare output mode to toggle
    // TCCR1A |= 0b01000000;
    // TCCR1A |= 0x40;
        
    // mode 9 phase correct
    TCCR1A |= (1 << 0); // set register A WGM[1:0] bits
    // TCCR1A |= 0b00000001;
    TCCR1B |= (1 << 4); // set register B WBM[3:2] bits
    // TCCR1B |= 0b00010000;
        
    TIMSK1 |= (1 << 1); // enable compare match A interrupt
    // TIMSK1 |= 0b00000100;
        
    OCR1A = 15297; // C5 523 Hz, no prescaler
	//OCR1A = 2462;   // A7 3250 Hz, no prescaler, calculated 
    //OCR1A = 2440;   // A7 3250 Hz, no prescaler, empirical 
    //OCR1A = 1016;   // B2  123 Hz, 64 prescaler
    
    while(1) 
    {
        /* enable timer/counter1 */
        TCCR1B |= (1 << 0); // set prescaling to 1 (no prescaling)
        // TCCR1B |= 0b00000001;
		    
        // TCCR1B |= (1 << 0) | (1 << 1); // 64 prescaler
        // TCCR1B |= 0b00000011;
    }
    
    return 0;
}

