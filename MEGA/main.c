#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)
#define FUNC_EXEC_TIME 5

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <time.h>
#include "lcd.h"
#include "keypad.h"

//uint8_t word[256];
char str;
//unsigned long int code;
char inputArray[69];
char code[69] = "6969";

// system variables
bool armed = true;
bool buzzer = false;
bool timer = false;
//clock_t clock(void);


static void USART_init(uint16_t ubrr) // unsigned int
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
static void USART_Transmit(unsigned char data, FILE *stream)
{
	/* Wait until the transmit buffer is empty*/
	while(!(UCSR0A & (1 << UDRE0)))
	{
		;
	}
	
	/* Put the data into a buffer, then send/transmit the data */
	UDR0 = data;
}
static char USART_Receive(FILE *stream)
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

//void timeout(double seconds, FN_PNTR) //Where FN_PNTR is a real function pointer
//{
	//time_t start = clock();
	//while ((clock() - start) < seconds * CLOCKS_PER_SEC)
	//{
		//if ( ! FN_PNTR) /* if the operation fails based on the return value*/
		//continue;
		//else
		//break; /* it worked */
	//}
	//exit(0); /* timed out... leave the program */
//}

bool checkPassword() {
	
	//this should be asynchronous, meaning that other code should continue when this executes
	if(clock() > FUNC_EXEC_TIME * 1000) return FALSE;
	
	get_input();
	if(strcmp(inputArray,code) == 0) {
		lcd_clrscr();
		lcd_gotoxy(0, 0);
		lcd_puts("Correct password");
		printf("Correct password\n");
		return TRUE;
	}
	else{
		lcd_clrscr();
		lcd_gotoxy(0, 0);
		lcd_puts("Incorrect password");
		printf("Incorrect password\n");
		return FALSE;
	}
}

void get_input(){
	int i = 0;
	//empty array always when asking for input
	memset(inputArray,0,69);
	
	while(1) {
		str = KEYPAD_GetKey();
		if(str) {
			if (str == 'A' || str == 'B' || str == 'C' || str == 'D') continue;

			if (i >= 4 && str == '#')
			{
				printf("[INPUT]	%s\n",inputArray);
				break;
			}
			if (str=='*' && i>0)
			{
				inputArray[i-1] = NULL;
				i--;
			}
			else if (str=='*' && i==0){
				continue;
			}
			else {
				inputArray[i] = str;
				i++;
			}
			printf("Input value: %c, i=%d\n", str, i);
		}
	}
}

void lcd_state_armed() {
	printf("[LCD]	system armed\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("System Armed");
	lcd_gotoxy(0,1);
	lcd_puts("Press D to disarm");
	
}

void lcd_state_disarmed() {
	printf("[LCD]	system disarmed\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("System Disarmed");
	lcd_gotoxy(0,1);
	lcd_puts("Press A to arm");
}

void lcd_ask_password() {
	printf("[LCD]	asking for password\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Enter password:");
	lcd_gotoxy(0,1);
}

void lcd_set_password() {
	printf("[LCD]	setting new password\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Enter old password:");
	lcd_gotoxy(0,1);
}

void lcd_print_help() {
	printf("[LCD]	printing help\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Enter new password:");
	lcd_gotoxy(0,1);
}

void lcd_print_status() {
	printf("[LCD]	SYSTEM IS %i\n",armed);
	lcd_clrscr();
	lcd_gotoxy(0,0);
	if(armed) {
		lcd_puts("System is armed");
	} else {
		lcd_puts("System is disarmed");
	}
	lcd_gotoxy(0,1);
}

void lcd_password_changed() {
	printf("[LCD]	password changed\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Password changed");
}


int main(void) {
	
	USART_init(MYUBRR);
	stdout = &uart_output;
	stdin = &uart_input;
	lcd_init(LCD_DISP_ON);
	KEYPAD_Init();
	
	while(1){
		// show system status at the start of the loop
		lcd_print_status();
		
		// get input from the user
		char input = KEYPAD_GetKey();
		printf("Input %c\n",input);
		
		
		switch(input)
		{
		case 'A': //arm the system
			if(!armed) {
				lcd_ask_password();
				if (checkPassword()) {
					lcd_state_armed();
					armed = true;
				}
				break;
			} else {
				printf("[LCD]	cannot arm an armed system\n");
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("Already armed");
				_delay_ms(1000);
			}
						
		case 'C': //change or set the password
			lcd_set_password();
			if (checkPassword()) {
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("New password");
				get_input();
				
				lcd_password_changed();
				strcpy(code, inputArray);
			}
			break;
			
		case 'D': //disarm the system
			lcd_ask_password();
			if (checkPassword()) {
				lcd_state_disarmed();
				armed = false;
			}
			break;
		default:
			lcd_print_help();
		}
			
	}	
}
