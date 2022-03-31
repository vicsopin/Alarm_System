#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int 
main(void)
{
    /* Set pins 8,9,10 and 11 as outputs, 1 = output, 0 = input */
    //DDRB |= 0b00001111; // set port B register bits 0:3 as outputs using an OR operand
    /* alternatively */
    //DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
    DDRB |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);

    while (1)
    {
        /* Set LEDs as HIGH */
        PORTB |= 0b00001111;
        _delay_ms(1000);
        /* set LEDs as LOW */
        PORTB  = 0b00000000;
        _delay_ms(1000);
        PORTB |= 0b00000001;
        _delay_ms(1000);
        PORTB |= 0b00000010;
        _delay_ms(1000);
        PORTB |= 0b00000100;
        _delay_ms(1000);
        PORTB |= 0b00001000;
        _delay_ms(1000);
        PORTB  = 0b00000000;
        _delay_ms(1000);
    }
}
