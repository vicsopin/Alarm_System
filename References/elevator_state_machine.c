/*
 * sm_elevator_atmel.c
 *
 * Created: 23.1.2020 22.47.12
 * Author : Glutex
 */ 

#define F_CPU 16000000UL
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

// define states for the states machine
#define Idle 0
#define GOINGUP 1
#define GOINGDOWN 2
#define DOOROPEN 3
#define FAULT 4
volatile int8_t g_STATE = 0;

// reads buttonstates and returns the pushed button. 
// If several buttons are pressed, the lowest one is returned. 
// arguments: current floor, int8_t
// returns: none
int 
floor_button_choice(int8_t *current_floor_button)
{
    //
    if ( (PINC & (1 << PC0)) )
    {
        *current_floor_button = 1;
    }
    else if ( (PINC & (1 << PC1)) )
    {
        *current_floor_button = 2;
    }
    else if ( (PINC & (1 << PC2)) ) 
    {
        *current_floor_button = 3;
    }
    else if ( (PINC & (1 << PC3)) )
    {
        *current_floor_button = 4;
    }
    else if ( (PINC & (1 << PC4)) ) 
    {
        *current_floor_button = 5;
    }
    else 
    {
        return 0;
    }
    
}

// Takes the current floor and turns floor indication LEDs on accordingly.
// arguments: current floor, int8_t
// returns: void
int
floor_led_on(int8_t *current_floor_led)
{
    //
    PORTA |= (1 << (*current_floor_led -1) );
    
}

// Takes the current floor and turns floor indication LEDs off accordingly.
// arguments: current floor, int8_t
// returns: void
int
floor_led_off(int8_t *current_floor_led)
{
    //
    PORTA &= ~(1 << (*current_floor_led -1) );
    
}


int 
main(void)
{
    
    /* Set inputs and outputs for the ports A, C and H */
    DDRA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5); // Floor LEDs
    DDRH |= (1 << PH3) | (1 << PH4); // Door LED and movement LED, respectively
    
    DDRC &= ~(1 << PC0) & ~(1 << PC1) & ~(1 << PC2) & ~(1 << PC3) & ~(1 << PC4); // Floor buttons
    /* 1 = output, 0 = input */
    
    /* elevator variables, elevator has 5 floors */
    static int8_t requested_floor = 1;
    static int8_t floor_current = 1;
    static bool  b_doors_opened = false;
    floor_led_on(&floor_current);

    while (1) 
    {
           
        /* State machine - switch case */
        switch(g_STATE) {
            case Idle:
                
                if (requested_floor == floor_current)
                {
                    g_STATE = Idle;
                    //int8_t request_floor = 1;
                    floor_button_choice(&requested_floor);
                    _delay_ms(10);
                    if ((requested_floor > 5) || (requested_floor < 1))
                    {
                        g_STATE = FAULT;
                        
                    }
                    
                }
                else if (requested_floor < floor_current)
                {
                    g_STATE = GOINGDOWN;
                }
                else if (requested_floor > floor_current)
                {
                    g_STATE = GOINGUP;
                }
            break;
            
            case GOINGUP:
                if (requested_floor > floor_current)
                {
                    //GOING UP TO THE NEXT FLOOR
                    g_STATE = GOINGUP;
                    floor_current++;
                    PORTH |= (1 << PH3); // turn movement LED ON
                    _delay_ms(3000);
                    floor_led_on(&floor_current);
                    

                }
                else if (requested_floor == floor_current)
                {
                    //FLOOR REACHED
                    g_STATE = DOOROPEN;
                }
            break;
            
            case GOINGDOWN:
                if (requested_floor < floor_current)
                {
                    // GOING DOWN TO THE NEXT FLOOR
                    g_STATE = GOINGDOWN;
                    PORTH |= (1 << PH3); // turn movement LED ON
                    _delay_ms(3000);
                    if (1 == floor_current)
                    {
                        ;
                    }
                    else
                    {
                        floor_led_off(&floor_current);
                    }
                    
                    floor_current--;
                    
                }
                else if (requested_floor == floor_current)
                {
                    // FLOOR REACHED
                    g_STATE = DOOROPEN;
                }
            break;
            
            case DOOROPEN:
                if (!b_doors_opened)
                {
                    // DOORS OPEN
                    PORTH |= (1 << PH4); // turn door open LED ON
                    PORTH &= ~(1 << PH3); // turn movement LED OFF
                    _delay_ms(5000);
                    b_doors_opened = true;
                    g_STATE = DOOROPEN;
                }
                else
                {
                    // DOORS CLOSED
                    PORTH &= ~(1 << PH4);
                    g_STATE = Idle;
                    b_doors_opened = false;
                }
            break;
            
            case FAULT:
                // WRONG FLOOR INPUT
                requested_floor = floor_current;
                
                // Blink movement LED to indicate fault. 
                for (int8_t blink_amount = 0; blink_amount < 20; blink_amount++)
                {
                    PORTH |= (1 << PH3);
                    _delay_ms(50);
                    PORTH &= ~(1 << PH3);
                    _delay_ms(50);
                }                 

                g_STATE = Idle;
            break;
                
            default:
                    // This state should not be reached. 
                    g_STATE = FAULT;
            break;
        }
        
    }
}

