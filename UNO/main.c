#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static void
USART_init(uint16_t ubrr) // unsigned int
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
USART_Transmit(unsigned char data)
{
	/* Wait until the transmit buffer is empty*/
	while(!(UCSR0A & (1 << UDRE0)))
	{
		;
	}
	
	/* Put the data into a buffer, then send/transmit the data */
	UDR0 = data;
}

static char
USART_Receive()
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

int main(void)
{
	// initialize the UART with 9600 BAUD
	USART_init(MYUBRR);
	
	// redirect the stdin and stdout to UART functions
	stdout = &uart_output;
	stdin = &uart_input;
	
	//printf("Status: Ready\n");
	
	DDRD = 0b00000000; // inputs
	PORTD = 0b00000100; // enable only first pin
	bool pir_trigger = 0;
	
	while(1)
	{
		pir_trigger = (PIND & (1 << PD2));
		//printf("[PIR]	%d",pir_trigger);
		
		if(0 != pir_trigger) {
			//printf("	ALARM!!!");
			USART_Transmit(true);
		}
		else {
			USART_Transmit(false);
		}
		//printf("\n");
	} return 0;
}
