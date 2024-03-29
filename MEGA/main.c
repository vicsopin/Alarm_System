#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)
#define FUNC_EXEC_TIME 5

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <time.h>
#include "lcd.h"
#include "keypad.h"

//uint8_t word[256];
char str;
//unsigned long int code;
char inputArray[69];
char code[69] = "6969";

// global state variables
bool armed = false;
bool buzzer = false;
bool timeout = false;

// global timer variables
unsigned int seconds;

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
static void USART_Transmit(unsigned char data)
{
	/* Wait until the transmit buffer is empty*/
	while(!(UCSR0A & (1 << UDRE0)))
	{
		;
	}
	
	/* Put the data into a buffer, then send/transmit the data */
	UDR0 = data;
}
static char USART_Receive()
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
	
	timer_start();
		
	if (!timeout) {
		get_input();
		if(strcmp(inputArray,code) == 0) {
			lcd_correct_password();
			return TRUE;
		}
		else {
			lcd_incorrect_password();
			return FALSE;
		}
	} else {
		lcd_timeout();
		timeout = FALSE;
		timer_stop();
		return FALSE;
	}
}

void get_input(){
	int i = 0;
	//empty array always when asking for input
	memset(inputArray,0,69);
	
	while(1) {
		str = KEYPAD_GetKey();
		printf("Input value: %c, i=%d\n", str, i);
		
		// backspace
		if (str=='*' && i>0) {
			inputArray[i-1] = NULL;
			i--;
			lcd_gotoxy(i,1);
			lcd_putc(' ');
			continue;
		}
		// prevents oscillating i
		if (str=='*' && i==0) {
			continue;
		}
		// exit condition
		if (str == '#') {
			printf("[INPUT]	%s\n",inputArray);
			break;
		}
		// skip all alphabet
		if (isalpha(str) > 1) {
			printf("Skipping: %c, i=%d\n", str, i);
			continue;
		}
		// add digits to array
		if(isdigit(str)) {
			inputArray[i] = str;
			lcd_gotoxy(i,1);
			lcd_putc(str);
			i++;
		}
		
		if (strlen(inputArray) == 2) {
			printf("[INPUT]	3\n");
			break;
		}		
	}
}

void lcd_incorrect_password() {
	printf("[LCD]	Incorrect password\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Incorrect");
	lcd_gotoxy(0,1);
	lcd_puts("Password");
	_delay_ms(3000);
}

void lcd_correct_password() {
	printf("[LCD]	Correct password\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Correct");
	lcd_gotoxy(0,1);
	lcd_puts("Password");
	_delay_ms(3000);
}

void lcd_timeout() {
	printf("[LCD]	INPUT TIMEOUT\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Input");
	lcd_gotoxy(0,1);
	lcd_puts("Timeout");
	_delay_ms(3000);
}

void lcd_state_armed() {
	printf("[LCD]	system armed\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("System Armed");
	lcd_gotoxy(0,1);
	lcd_puts("Waiting motion");
	
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
		lcd_puts("System disarmed");
	}
	lcd_gotoxy(0,1);
}

void lcd_password_changed() {
	printf("[LCD]	password changed\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Password changed");
}

void lcd_motion_detected() {
	printf("[LCD]	motion detected...\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("Motion detected");
}

void lcd_alarm() {
	printf("[LCD]	ALARM\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("---- ALARM ----");
}



void buzz() {
	PORTF |= (1<<PF0);
	for (int i=0; i < 2500; ++i)
	{
		PORTF |= (1<<PF0);
		_delay_ms(1);
		PORTF &= ~(1<<PF0);
		_delay_ms(1);
	}
}

void grace_period(int duration) {
	printf("[LCD]	why are you running\n");
	lcd_clrscr();
	lcd_gotoxy(0,0);
	char row1[16];
	sprintf(row1, "%d s grace",duration);
	lcd_puts(row1);
	for (unsigned int i=duration; i != 0; i--)
	{
		printf("[LCD]	Time left: %d\n",i);
		lcd_gotoxy(0,1);
		char row2[20];
		sprintf(row2, "Time left: %d ",i);
		lcd_puts(row2);
		_delay_ms(1000);
	}
}

//interrupt function
ISR(TIMER0_COMPA_vect) {
	seconds++;
	printf("[TIMER]	time: %u\n",seconds);
	if(seconds == 5) {
		timeout = TRUE;		
	}
}

void timer_init() {
	//// enable interrupts
	cli();
	
	//set the entire register to 0
	TCCR3B = 0;
	TCCR3A = 0;
	
	//reset timer counter register
	TCNT3 = 0;
	
	// OCR3A = f_cpu / (2 * N * 0.1)
	// f_cpu = 16 000 000
	// N = 1024
	// 10 seconds = 0.1 Hz
	OCR3A = 15624; // ISR called every second
	
	// waveform generation mode
	TCCR3A |= (1 << 0); //set register A WGM bits
	TCCR3B |= (1 << WGM32); //set register B WGM bits
	
	// prescale time division by N = 1024
	TCCR3B |= ((1 << CS32) | (1 << CS30));

	sei();
}

void timer_start() {
	//enable timer
	TIMSK3 |= (1 << OCIE3A);
}

void timer_stop() {
	//disable timer
	TIMSK3 |= (0 << OCIE3A);
	
	//set the entire register to 0
	TCCR3B = 0;
	TCCR3A = 0;
	
	//reset timer counter register
	TCNT3 = 0;
}

int main(void) {
	
	USART_init(MYUBRR);
	stdout = &uart_output;
	stdin = &uart_input;
	lcd_init(LCD_DISP_ON);
	KEYPAD_Init();
	DDRF = 0b00000001; // buzzer output
	
	timer_init();
	
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
					
					grace_period(5);
					armed = TRUE;
					lcd_state_armed();
					while(1){
						_delay_ms(1000);
						
						bool msg;
						
						for(int i = 0; i > 1000; i++) {
							msg = USART_Receive();
						}
						
						printf("[PIR]	msg: %d\n",msg);
						// if movement detected
						if(msg == 0) {
							lcd_motion_detected();
							buzz();
							msg=NULL;
							//_delay_ms(1000);
							//enable timer
							//TCCR3B |= (1 << 0);
							
							lcd_ask_password();
							
							//exit loop on correct password
							if (checkPassword()) {
								TCNT3 = 0; //reset timer counter
								armed = FALSE;
								break;
							}
						}
						//_delay_ms(1000);						
					}
				}
				break;
			} else {
				printf("[LCD]	cannot arm an armed system\n");
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("Already armed");
				_delay_ms(1000);
			} break;
						
		case 'C': //change or set the password
			if(!armed) {
				lcd_set_password();
				if (checkPassword()) {
					lcd_clrscr();
					lcd_gotoxy(0,0);
					lcd_puts("New password:");
					get_input();
					lcd_password_changed();
					strcpy(code, inputArray);
				}
				break;
			}
			else {
				printf("[LCD]	Unavailable while armed\n");
				lcd_clrscr();
				lcd_gotoxy(0,0);
				lcd_puts("Unavailable");
				lcd_gotoxy(0,1);
				lcd_puts("while armed");
				_delay_ms(1000);
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
