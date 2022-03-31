/*
 * uno_port_input_output.c
 *
 * Created: 18.6.2019 13:14:18
 * Author : Glutex
 */ 

#define F_CPU 16000000UL
#include <stdlib.h> 
#include <stdbool.h> 
#include <avr/io.h>
#include <util/delay.h>



int 
main(void)
{
    /* Set inputs and outputs for the ports B and D */
    DDRB |=  (1 << PB1) |  (1 << PB2); // 0b00000110
    DDRD &= ~(1 << PD5) & ~(1 << PD3); // 0b00000000;
    /* 1 = output, 0 = input */
    //DDRB |=  (1 << 1) |  (1 << 2); // 0b00000110
    //DDRD &= ~(1 << 5) & ~(1 << 3); // 0b00000000;
    
    /* initialize variables  */
    bool b_button_state_1 = 0;
    bool b_button_state_2 = 0;
    
    while (1) 
    {
            
        /* read inputs */
        b_button_state_1 = (PIND & (1 << PD5)); // read the button 1 value from pin 5
        b_button_state_2 = (PIND & (1 << PD3)); // read the button 2 value from pin 3
        
        /* apply outputs, if button is pressed */
        if (0 != b_button_state_1)
        {
            PORTB |=  (1 << PB1); // turn the bit on with bitwise OR operand
        }
        else
        {
            PORTB &= ~(1 << PB1); // turn the bit off with NOT operand
        }
    
        if (0 != b_button_state_2)
        {
            PORTB |=  (1 << PB2);
        }
        else
        {
            PORTB &= ~(1 << PB2);
        }
        
        /* transistor will turn the LED ON if both button 1 and 2 are pressed */
        /* It will turn the LED OFF if buttons are not both pressed */
        _delay_ms(1000);
        
    }
}

