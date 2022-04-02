/*
 PIR Motion Sensor Interface with Uno
 added LED to test
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB = 0b00000000; // inputs
    PORTB = 0b00000001; // enable only first pin

    DDRD = 0b00001100; // outputs
    //PORTD = 0x00; // keep buzzer and LED off 
    int i;

    while(1)
    {
        if(PINB & (1<<PB0)) {
            
            PORTD |= (1<<PD3);
            
            // I have a passive buzzer so need to oscillate
            for (i=0; i < 10; ++i)
            {
                PORTD |= (1<<PD2);
                _delay_ms(1);
                PORTD &= ~(1<<PD2);
                _delay_ms(1);
            }

        }
        else
            PORTD &= ~(1<<PD2);
            PORTD &= ~(1<<PD3);
	} return 0;
}