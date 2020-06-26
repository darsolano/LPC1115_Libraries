/*
 * max7219.c
 *
 *  Created on: Dec 4, 2015
 *      Author: dsolano
 */

#include <max7219.h>
#include <serial_3w.h>
#include <timer32_lpc11xx.h>



//Number definition
const uint8_t display7s[] =
{
//        pABCDEFG
		0b01111110,				// 0
		0b00110000,             // 1
		0b01101101,             // 2
		0b01111001,             // 3
		0b00110011,             // 4
		0b01011011,             // 5
		0b01011111,             // 6
		0b01110000,             // 7
		0b01111111,             // 8
		0b01110011,             // 9
		0b01110111,             // A
		0b00011111,             // B
		0b01001110,             // C
		0b00111101,             // D
		0b01001111,             // E
		0b01000111,             // F
		0b10000000				// Dot
//        pABCDEFG
		};

/*
 *        a
        ------
 *      |    |b
 *     f| g  |
 *      ------
 *      |    |c
 *    e |    |
 *      ------ o h
 *         d
 */
const uint8_t display7sLetters[] =
{
//        pABCDEFG
		0b00000000,				// space
		0b01110111,             // A
		0b00011111,             // B
		0b01001110,             // C
		0b00111101,             // D
		0b01001111,             // E
		0b01000111,             // F
		0b01111011,				// G
		0b00110111,				// H
		0b00000110,				// I
		0b00111100,				// J
		0b00000001,				// K
		0b00001110,				// L
		0b00000001,				// M
		0b00010101,				// N
		0b00011101,				// O
		0b01100111,				// P
		0b11111110,				// Q
		0b00000101,				// R
		0b01011011,				// S
		0b00001111,				// T
		0b00111110,				// U
		0b00011100,				// V
		0b00000001,				// W
		0b00000001,				// X
		0b00111011,				// Y
		0b00000001				// Z
//		  pABCDEFG
		};



static uint8_t max7219_convert_digits(uint8_t character, TYPEOFCHAR_t type)
{
	uint8_t data;
	if (type == LETTER)
	{
		if (character == 0x20)
			character = 0x41;
		if (character > 0x40 && character < 0x5a)
		{
			data = (character - 0x40);
			return display7sLetters[data];
		}
	}
	if (type == NUMBER)
	{
		data = 0x0f & character;
		return display7s[data];
	}
	return -1;
}


// Initial configuration for MAX7219, go for no config to use 7Segment array
void max7219_init(SCAN_LIMIT_e digits , INTENSISTY_e intensity , DECODE_e decode)
{
	serial3wire_init();

	MAX7219(DISPLAY_TEST , DISP_TEST_NORMAL_OP);
	MAX7219(SHUTDOWN , NORMAL_OPERATION);
	MAX7219(SCAN_LIMIT , digits);
	MAX7219(INTENSITY , intensity);
	MAX7219(DECODE_MODE , decode);
	max7219_test_cycle();

}


void max7219_clear_all_display(void)
{
	int i = 8;
	while (i--)	// i = digit register. BUT 0 IS NOT A DIGIT REGISTER
	{
		serial3wire_write_short( ((i<<8)&CMD_MASK) | (CLEAR_MASK&DATA_MASK) );
	}

}

void max7219_test_cycle(void)
{
	MAX7219(DISPLAY_TEST , DISP_TEST_TEST_MODE);
	delay32Ms(TIMER0,1000);
	MAX7219(DISPLAY_TEST , DISP_TEST_NORMAL_OP);

	// circle around display
	int i = 0;
	uint8_t digit = 0;

	// round display from a to dot
	for (i=0;i<9;i++)
	{
		serial3wire_write_short((digit<<8)|digit);
		MAX7219(DIG0,digit);
		MAX7219(DIG1,digit);
		MAX7219(DIG2,digit);
		MAX7219(DIG3,digit);
		MAX7219(DIG4,digit);
		digit = (1<<i);
		delay32Ms(TIMER0,200);
	}

	max7219_blink(500,1);

	//Count to 0-9 for each display from display 0 to 7

	// count from 0-9 every display together
}

void max7219_blink(int timeinmilli, int blink_times)
{
	MAX7219(DIG0,0xff);
	MAX7219(DIG1,0xff);
	MAX7219(DIG2,0xff);
	MAX7219(DIG3,0xff);
	MAX7219(DIG4,0xff);
	while (blink_times--)
	{
		MAX7219(SHUTDOWN , SHUTDOWN_MODE);
		delay32Ms(TIMER0,timeinmilli);
		MAX7219(SHUTDOWN , NORMAL_OPERATION);
		delay32Ms(TIMER0,timeinmilli);
	}
}

void max7219_scroll_display(SCROLL_DIR_e direction)
{

}

void max7219_display_print(int32_t number)
{
	// Digit 0
	uint8_t c1 = number % 10;
	MAX7219(DIG0 , max7219_convert_digits(c1,NUMBER));
	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	MAX7219(DIG1 , max7219_convert_digits(c2,NUMBER));
	// Digit 2
	uint8_t c3 = (number / 100) % 10;
	MAX7219(DIG2 , max7219_convert_digits(c3,NUMBER));
	// Digit 3
	uint8_t c4 = (number / 1000) % 10;
	MAX7219(DIG3 , max7219_convert_digits(c4,NUMBER));
	// Digit 4
	uint8_t c5 = (number / 10000) % 10;
	MAX7219(DIG4 , max7219_convert_digits(c5,NUMBER));

	/*uint8_t c6 = (number / 100000) % 10;
	uint8_t c7 = (number / 1000000) % 10;
	uint8_t c8 = (number / 10000000) % 10;
	uint8_t c9 = (number / 100000000) % 10;
	uint8_t c10 = (number / 1000000000) % 10;
	if (c10 != 0) LCD1602_Putc('0' + c10);
	if (c9 != 0) LCD1602_Putc('0' + c9);
	if (c8 != 0) LCD1602_Putc('0' + c8);
	if (c7 != 0) LCD1602_Putc('0' + c7);
	if (c6 != 0) LCD1602_Putc('0' + c6);
	if (c5 != 0) LCD1602_Putc('0' + c5);
	if (c4 != 0) LCD1602_Putc('0' + c4);
	if (c3 != 0) LCD1602_Putc('0' + c3);
	if (c2 != 0) LCD1602_Putc('0' + c2);
	LCD1602_Putc('0' + c1);*/

}
