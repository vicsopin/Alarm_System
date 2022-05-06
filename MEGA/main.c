#define F_CPU 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/array.h>
#include "lcd.h"
#include "keypad.h"

uint8_t key_1, key_2, key_3, key_4;
uint8_t key_arm = '0';


int c;
int i;
int password[50]; 

int get_password();
void check_password();
//void system_arm(); // function that initiates arm procedures

///////////// Menu items ///////////////

// check password[i( 3 attempts)
int get_password() {

	for ( c = 0; c < 3; c++ ) // check password[ifunction
	{
		lcd_clrscr();
		lcd_puts("Enter password:");

		for ( i = 0; i++ ){ 

			while(1) {
				lcd_gotoxy(0,i);

				switch(KEYPAD_GetKey()){
					case '1':
						password[i] = '1';
						i++;
					case '2':
						password[i] = '2';
						i++;
					case '3':
						password[i] = '3';
						i++;
					case '4':
						password[i] = '4';
						i++;
					case '5':
						password[i] = '5';
						i++;
					case '6':
						password[i] = '6';
						i++;
					case '7':
						password[i] = '7';
						i++;
					case '8':
						password[i] = '8';
						i++;
					case '9':
						password[i] = '9';
						i++;
					case '0':
						password[i] = '0';
						i++;
					case '/':
						password[-1] = password[i+1];
						i--;
						lcd_gotoxy(0,i);
						lcd_putc('');
					case '*':
						if check_password {
							c==3;

						}
						return password;
					default:
						continue;
				}


		}
		// _delay_ms(1000);
		// lcd_clrscr();
		// lcd_puts("Enter password[i");
		// lcd_gotoxy(0,1);
		// key_1 = KEYPAD_GetKey();
		// if(0 <= key_1 <= 9) {
		// 	continue
		// }
		// else if(key_1 == '*')
		// lcd_putc(key_1);
		// lcd_gotoxy(1,2);
		// key_2 = KEYPAD_GetKey();
		// lcd_putc(key_2);
		// lcd_gotoxy(2,3);
		// key_3 = KEYPAD_GetKey();
		// lcd_putc(key_3);
		// lcd_gotoxy(3,4);
		// key_4 = KEYPAD_GetKey();
		// lcd_putc(key_4);
		
		// _delay_ms(1000);
		
		// if(key_1=='6' && key_2=='9' && key_3=='6' && key_4=='9'){
		// 	lcd_gotoxy(0,0);
		// 	lcd_clrscr();
		// 	lcd_puts("Correct password[i);
		// 	break;
		// }
		
		// else {
		// 	lcd_gotoxy(0,0);
		// 	lcd_clrscr();
		// 	lcd_puts("Wrong password[i);
		// 	continue;
		// }
		
	}
}

void check_password(password) {

	if (sizeof(password) == 4 ){

	}

	else {

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