/*
 * max7219.h
 *
 *  Created on: Dec 4, 2015
 *      Author: dsolano
 */

#ifndef INC_MAX7219_H_
#define INC_MAX7219_H_

#include <lpc_types.h>
#include <LPC11xx.h>

#define CMD_MASK	0x0f00
#define DATA_MASK	0x00ff
#define CLEAR_MASK	0x00

// Characters		  pABCDEFG
#define DASH	  	0b00000001
#define DOT			0b10000000
#define DEGREE		0b01100011

typedef enum
{
	SCROLL_LEFT = 0,
	SCROLL_RIGHT = 1
}SCROLL_DIR_e;

typedef enum
{
	LETTER,
	NUMBER
}TYPEOFCHAR_t;

typedef enum
{
	NOP = 0x00,
	DIG0 = 0x01,
	DIG1 = 0x02,
	DIG2 = 0x03,
	DIG3 = 0x04,
	DIG4 = 0x05,
	DIG5 = 0x06,
	DIG6 = 0x07,
	DIG7 = 0x08,
	DECODE_MODE = 	0x09,
	INTENSITY 	= 	0x0A,
	SCAN_LIMIT 	= 	0x0B,
	SHUTDOWN 	= 	0x0C,
	DISPLAY_TEST = 0x0F
}MAX7219_REGS_t;

typedef enum
{
	INT_MIN = 0,
	INT_3 = 1,
	INT_5 = 2,
	INT_7 = 3,
	INT_9 = 4,
	INT_11 = 5,
	INT_13 = 6,
	INT_15 = 7,
	INT_MED = 8,
	INT_19 = 9,
	INT_21 = 10,
	INT_23 = 11,
	INT_25 = 12,
	INT_27 = 13,
	INT_29 = 14,
	INT_MAX = 15
}INTENSISTY_e;

// Shutdown parameter
typedef enum
{
	SHUTDOWN_MODE =		0x00,
	NORMAL_OPERATION =	0x01
}SHUTDOWN_e;


// DEcode register parameters
typedef enum
{
	NO_DECODE_DIG_0_7 =	0x00,
	CODE_B_DIG_0 =		0x01,
	CODE_B_DIG_0_3 =	0x0F,
	CODE_B_ALL_DIG =	0xFF
}DECODE_e;

// Test register parameter
typedef enum
{
	DISP_TEST_NORMAL_OP =	0x00,
	DISP_TEST_TEST_MODE =	0x01
}DISP_TEST_e;

// Determine how many digits to use are connected to MAX7219
typedef enum
{
	DISP_ONLY_DIG0 = 0,
	DISP_DIG0_DIG1 = 1,
	DISP_DIG0_to_DIG2 = 2,
	DISP_DIG0_to_DIG3 = 3,
	DISP_DIG0_to_DIG4 = 4,
	DISP_DIG0_to_DIG5 = 5,
	DISP_DIG0_to_DIG6 = 6,
	DISP_ALL_DIGITS = 7
}SCAN_LIMIT_e;

typedef struct
{
	uint8_t 		no_op_mode;
	uint8_t 		decode_mode;
	INTENSISTY_e 	intensity;
	SCAN_LIMIT_e 	scan_limit;
	DISP_TEST_e 	disp_test;
	SHUTDOWN_e 		shutdown_mode;
}MAX7219_s;

#define MAX7219(reg,data)	serial3wire_write_short(((reg<<8)&CMD_MASK) | (data&DATA_MASK))

void max7219_display_print(int32_t number);
void max7219_scroll_display(SCROLL_DIR_e direction);
void max7219_blink(int timeinmilli, int blink_times);
void max7219_test_cycle(void);
void max7219_clear_all_display(void);
void max7219_init(SCAN_LIMIT_e digits , INTENSISTY_e intensity , DECODE_e decode);


#endif /* INC_MAX7219_H_ */