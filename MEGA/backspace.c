#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "keypad.h"

//uint8_t word[256];
char str;
//unsigned long int code;
char inputArray[69];

// system variables
bool armed = false;
bool buzzer = false;
bool timer = false;

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

bool checkPassword() {
	if(inputArray[0]=='6' && inputArray[1]=='9' && inputArray[2]=='6' && inputArray[3]=='9'){
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

void lcd_password() {
		lcd_clrscr();
		lcd_gotoxy(0,0);
		lcd_puts("Enter password:");
		lcd_gotoxy(0,1);
}

char* password_query(){
	int i = 0;
	while(1) {
		str = KEYPAD_GetKey();
		if(str) {
			if (i >= 4 && str == '#')
			{
				for(t=o; t<=2; t++){
					if 
				}
			}
			
			if (str=='*' && i>0)
			{
				inputArray[i-1] = NULL;
				i--;
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("Enter password:");
				lcd_gotoxy(0,1);
				lcd_puts(inputArray);
			}
			
			else if (str=='*' && i==0){
				continue;
			}
			
			else {
				inputArray[i] = str;
				i++;
				lcd_putc(str);
			}
			printf("Input value: %c, i=%d\n", str, i);
		}
	}
	return 0;
}

char* get_input(){
	int i = 0;
	//empty array always when asking for input
	memset(inputArray,0,69);
	
	while(1) {
		str = KEYPAD_GetKey();
		if(str) {
			if (i >= 4 && str == '#')
			{
				//printf(inputArray);
				//return inputArray;
				checkPassword();
				if (checkPassword()){
					break;
				}
				else {
					continue;
				}
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

int main(void)
{
	USART_init(MYUBRR);
	stdout = &uart_output;
	stdin = &uart_input;
	/* initialize display, cursor off */
	lcd_init(LCD_DISP_ON);
	lcd_password();
	KEYPAD_Init();
	while(1){
		password_query();
		
// 		if (!armed && !timer)
// 		{
// 			lcd_password();
// 			
// 			printf("Input is: %s\n", password_query());
// 			
	
//		}
	}
}
