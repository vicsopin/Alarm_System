/*
 * esys-project-work.c
 *
 * Created: 29.3.2022 12:18:25
 * Author : Niklas Kettunen, Victor Sopin, Umair Raihan
 * For: ATMEGA328P
 */ 
#define KEY_PRT PORTD	//WRITE
#define KEY_DDR	DDRD	//DIRECTION
#define KEY_PIN PIND	//READ

#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

/*unsigned char keypad[4][4] = {	
	{'#','7','4','1'},
	{'0','8','5','2'},
	{'*','9','6','3'},
	{'D','C','B','A'}
};*/
unsigned char keypad[4][4] = {	
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};
unsigned char colloc, rowloc;
unsigned long int code = 0;		// Initial value for the password
const int grace_period = 10;	// Waiting time in seconds

// System states
bool armed = false;
bool buzzer = false;
bool timer = false;

char keyfind()
{
	while(1)
	{					//0xROWS|COLS
		KEY_DDR = 0xF0; //0x1111|0000, 1=output, 0=input  set port direction as input-output
		KEY_PRT = 0xFF; //0x1111|1111

		do
		{
			KEY_PRT &= 0x0F; //0x00001111      /* mask PORT for column read only */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F); /* read status of column */
		}while(colloc != 0x0F);
		
		do
		{
			do
			{
				_delay_ms(20);             /* 20ms key debounce time */
				colloc = (KEY_PIN & 0x0F); /* read status of column */
				//printf("colloc: 0x%02x PIND: 0x%02x \n",colloc,PIND);
				}while(colloc == 0x0F);        /* check for any key press */
				//printf("PRESS\n");
				//printf("colloc: 0x%02x PIND: 0x%02x \n",colloc,PIND);
				_delay_ms (40);	            /* 20 ms key debounce time */
				colloc = (KEY_PIN & 0x0F);
			}while(colloc == 0x0F);

			/* now check for rows */
			KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 0;
				break;
			}

			KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 1;
				break;
			}
			
			KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 2;
				break;
			}

			KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 3;
				break;
			}
		}
		//printf("Row: %d ", rowloc);
		//printf("Col: 0x%02x ", colloc);
		if(colloc == 0x0E)
		return(keypad[rowloc][0]);
		else if(colloc == 0x0D)
		return(keypad[rowloc][1]);
		else if(colloc == 0x0B)
		return(keypad[rowloc][2]);
		else
		return(keypad[rowloc][3]);
	}
		
	
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
USART_Transmit(unsigned char data, FILE *stream)
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

FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);

unsigned long int concat(unsigned long int a, int b)
{
	//printf("%i\n", a);
	//printf("%i\n", b);
	if(a == 0) return b;
	char s1[60];
	char s2[60];
	
	// Convert both the integers to string
	sprintf(s1, "%lu", a);
	sprintf(s2, "%i", b);
	
	// Concatenate both strings
	strcat(s1, s2);
	// Convert the concatenated string
	// to integer
	char *ptr;
	unsigned long int c = strtol(s1, &ptr, 10);
	
	
	// return the formed integer
	return c;
}
unsigned long int combine_digits()
{
	int count = 1;
	
	while(1){
		char key = keyfind();

		colloc = (KEY_PIN & 0x0F);
		if (colloc != 0x0F)
		{
			if(key == '*' && count > 4) break;
			else if(isdigit(key))
			{
				count++;
				int ia = key - '0';
				code = concat(code, ia);
				printf("%lu\n", code);
			}
			if(key == '#')
			{
				count--;
				code = code/10;
				printf("%lu\n", code);
			}

			//printf("Last Key: %c\n", key);
			//printf("Is digit: %i\n", isdigit(key));
			//printf("\n");
			
			if(count > 8) break;
		}	
	}
	return code;
}

bool detectMovement()
{
	_delay_ms(5000);
	return true;
}

void timerLoop(int duration)
{
	for(int i=duration-1; i > 0; i--)
	{
		// Flash timer LED every second
		printf("%d..",i);
		PORTB |= _BV(PB3);
		_delay_ms(100);
		PORTB &= ~_BV(PB3);
		_delay_ms(900);
	}
}
void fastBlink()
{
	// simulates alarm
	PORTB |= _BV(PB3);
	_delay_ms(50);
	PORTB &= ~_BV(PB3);
	_delay_ms(50);
}
void printWelcomeMessage()
{
	_delay_ms(1000);
	printf("WARNING:  Unauthorized access to this system is forbidden and \n");
	printf("will be prosecuted by law. By accessing this system, you agree\n");
	printf("that your actions may be monitored if unauthorized usage is suspected.\n");
}
	
void printAlarmMessage()
{
	printf("===================\n");
	printf("      ALARM\n");
	printf("===================\n\n");
}
int main(void)
{
	// Init serial terminal
	USART_init(MYUBRR);
	stdout = &uart_output;
	stdin = &uart_input;
	
	// Init LED output pins
	DDRB |= (1 << 3) | (1 << 4) | (1 << 5);
	
	printf("\n");
	printWelcomeMessage();
    /* Replace with your application code */
    while (1) 
    {
		// System is armed, show it to the user with LED
		if(armed) PORTB |= _BV(PB5);
		
		/* FOR VICTOR:
		- detectMovement function currently returns true after 5 seconds
		- add your sensor state here, return true if movement
		*/
		
		// System is armed and movement is detected
		if(armed && detectMovement())
		{


			/* FOR VICTOR AND UMAIR: Figure out how to
			- start timer here
			- while true:
				- if timeout > grace_period:
					- fastBlink();
					- this needs to be called asyncronously OR
					- we then use another LED to show that grace period ended
				- ask for password
				- if(password == code):
					- armed = false
					- stop timer
					- break
			- 
			*/
			// Turn on LED for motion detected, ask for password
			PORTB |= _BV(PB4);
			printf("\nMotion detected! Input password: \n\n");
			
			_delay_ms(10000);
			printAlarmMessage();
			// Timeout occured, blink fast and still ask for password
			while(1) 
			{
				fastBlink();
			}
			

			
			
			
			
			

		}
		// System is waiting to be armed, ask password
		if(!armed && !timer)
		{
			PORTB &= ~_BV(PB5); //turn off armed LED
			PORTB &= ~_BV(PB4); //turn off motion LED
			
			printf("Input password: \n");
			printf("Arming the system with: %lu\n\n",combine_digits());
			timer = true;
		}
		// Grace period before arming
		if(timer && !armed)
		{
			printf("System arming in %i seconds! \n", grace_period);
			timerLoop(grace_period);
			printf("\nArmed!");
			timer = false;
			armed = true;
		}
    }
	return 0;
}

