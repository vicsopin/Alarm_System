#define F_CPU 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "keypad.h"

uint8_t key_1, key_2, key_3, key_4;
uint8_t key_arm = '0';

int c;
void check_password();
//void system_arm();

///////////// Menu items ///////////////

// check password ( 3 attempts)
void check_password() {

	for ( c = 0; c < 3; c++ ) // check password function
	{
		_delay_ms(1000);
		lcd_clrscr();
		lcd_puts("Enter password:");
		lcd_gotoxy(0,1);
		key_1 = KEYPAD_GetKey();
		lcd_putc(key_1);
		lcd_gotoxy(1,2);
		key_2 = KEYPAD_GetKey();
		lcd_putc(key_2);
		lcd_gotoxy(2,3);
		key_3 = KEYPAD_GetKey();
		lcd_putc(key_3);
		lcd_gotoxy(3,4);
		key_4 = KEYPAD_GetKey();
		lcd_putc(key_4);
		
		_delay_ms(1000);
		
		if(key_1=='6' && key_2=='9' && key_3=='6' && key_4=='9'){
			lcd_gotoxy(0,0);
			lcd_clrscr();
			lcd_puts("Correct password");
			break;
		}
		
		else {
			lcd_gotoxy(0,0);
			lcd_clrscr();
			lcd_puts("Wrong password");
			continue;
		}
		
	}
}

///////////// Menu items end ///////////////

//void system_arm()
int main()
{
	// initialize display, cursor off
	lcd_init(LCD_DISP_ON);
	KEYPAD_Init();
	while(1) {
		// main menu function here
		lcd_gotoxy(0,0);
		lcd_clrscr();
		lcd_puts("System disarmed");
		lcd_gotoxy(0,1);
		lcd_puts("To arm press 1");
		key_arm = KEYPAD_GetKey();

		while(key_arm=='1'){
		
			check_password();
			//system_arm();
			_delay_ms(1000);
			lcd_gotoxy(0,0);
			lcd_clrscr();
			lcd_puts("System is armed");
			lcd_gotoxy(0,1);
			lcd_puts("Press 0 to disarm");
			key_arm = KEYPAD_GetKey();
		
			if(key_arm=='0') {
				check_password();
				break;
				}
		}

	}

	return 0;
}