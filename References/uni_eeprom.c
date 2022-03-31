/*
 * uno_eeprom.c
 *
 * Created: 3.7.2019 10:05:32
 * Author : Glutex
 */ 

#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static void
USART_init(uint16_t ubrr)
{
    /* Set baud rate in the USART Baud Rate Registers (UBRR) */
    UBRR0H = (unsigned char) (ubrr >> 8);
    UBRR0L = (unsigned char) ubrr;
    
    /* Enable receiver and transmitter on RX0 and TX0 */
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0); //NOTE: the ATmega328p has 1 UART: 0
    // UCSR0B |= (1 << 4) | (1 << 3);
    
    /* Set frame format: 8 bit data, 2 stop bit */
    UCSR0C |= (1 << USBS0) | (3 << UCSZ00);
    // UCSR0C |= (1 << 3) | (3 << 1);
    
}

static void
USART_Transmit(unsigned char data, FILE *stream)
{
    /* Wait until the transmit buffer is empty*/
    while(!(UCSR0A & (1 << UDRE0)))
    {
        ;
    }
    
    /* Puts the data into a buffer, then sends/transmits the data */
    UDR0 = data;
}

static char
USART_Receive(FILE *stream)
{
    /* Wait until the transmit buffer is empty*/
    while(!(UCSR0A & (1 << UDRE0)))
    {
        ;
    }
    
    /* Get the received data from the buffer */
    return UDR0;
}

// Setup the stream functions for UART
FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);

int
main(void)
{
    
    /* MEGA button pin port manipulation */
    DDRB &= ~(1 << PB0); // pin 8
    DDRD &= ~(1 << PD5); // pin 5
    
    // initialize the UART with 9600 BAUD
    USART_init(MYUBRR);
    
    // redirect the stdin and stdout to UART functions
    stdout = &uart_output;
    stdin = &uart_input;

    // push button variable
    bool b_button_state_read = 0;
    bool b_button_state_write = 0;
    

    /* UNO has 1 kB (1023 bytes) of EEPROM memory and MEGA 2560 has 4 kB (4096 B) */
    uint16_t memory_address_max = 32; // NOTE: the actual max memory address is larger, but in this task we are
    //       keeping it smaller for simplicity
    char memory_data[32]; // data read from eeprom
    char user_data[32] = "This data was saved to EEPROM"; // data to be written to eeprom
    char user_data_1[32] = "This data was saved to EEPROM";
    char user_data_2[32] = "Next we saved this to EEPROM";
    
    while (1)
    {
        
        /* read EEPROM if button is pushed and print it using UART */
        b_button_state_read = (PINB & (1 << PB0));
        
        /* write user data to EEPROM if button is pushed */
        b_button_state_write = (PIND & (1 << PD5));
        
        
        if (1 == b_button_state_read)
        {

            for (uint16_t address_index = 0; address_index < memory_address_max; address_index++)
            {
                
                while(EECR & (1 << 1))
                {
                    /* wait for the previous write operation to end */
                }
                
                EEAR  = address_index;
                EECR |= 0x01; // enable EEPROM read
                memory_data[address_index] = EEDR;
            }
            
            printf("Memory data was: ");
            printf(memory_data);
            printf("\n");
            
        }
        else if (1 == b_button_state_write)
        {
            
            printf("User Data that was saved: ");
            printf(user_data);
            printf("\n");
            
            // Write data to eeprom, which can then be read using the push button
            for (uint16_t address_index = 0; address_index < sizeof(user_data); address_index++)
            {
                
                
                while(EECR & (1 << 1))
                {
                    /* wait for the previous write operation to end */
                }
                
                EEAR = address_index;
                EEDR = user_data[address_index];
                EECR |= (1 << 2); // master programming enable
                // EECR |= 0b000001100;
                EECR |= (1 << 1); // EEPROM programming enable
                // EECR |= 0x02;
                // EECR |= 0b00000010
            }
            
            // Switch the data to be saved between button presses
            bool b_switch_text = 0;
            
            if(0 == b_switch_text)
            {
                strcpy(user_data, user_data_2);
                b_switch_text != b_switch_text;
            }
            else if (1 == b_switch_text)
            {
                strcpy(user_data, user_data_1); // data to be written to eeprom
                b_switch_text != b_switch_text;
            }
            
        }
          
    }
    
    return 0;
}

