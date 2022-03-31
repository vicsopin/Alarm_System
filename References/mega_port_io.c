/*
 * mega_port_input_output.c
 *
 * Created: 18.6.2019 17:05:30
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
    /* Set inputs and outputs for the ports A and H*/
    DDRH |= (1 << PH3) | (1 << PH4); // 0b00011000
    DDRA &= ~(1 << PA2) & ~(1 << PA4); // 0b00010100;
    /* 1 = output, 0 = input */
    
    /* initialize variables  */
    bool b_button_state_1 = 0;
    bool b_button_state_2 = 0;
    
    while (1) 
    {
        /* read inputs */
        b_button_state_1 = (PINA & (1 << PA2)); // read the button 1 value from pin 5
        b_button_state_2 = (PINA & (1 << PA4)); // read the button 2 value from pin 3
        
        /* apply outputs, if button is pressed */
        if (0 != b_button_state_1)
        {
            PORTH |=  (1 << PH3); // turn the bit on with bitwise OR operand
        }
        else
        {
            PORTH &= ~(1 << PH3); // turn the bit off with NOT operand
        }
            
        if (0 != b_button_state_2)
        {
            PORTH |=  (1 << PH4);
        }
        else
        {
            PORTH &= ~(1 << PH4);
        }
        /* transistor will turn the LED ON if both button 1 and 2 are pressed */
        /* It will turn the LED OFF if buttons are not both pressed */
        _delay_ms(1000);
        
    }
    
    return 0;
}

