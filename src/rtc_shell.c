/*
 * rtc_shell.c
 *
 *  Created on: Mar 8, 2016
 *      Author: dsolano
 */

#include <tinyrtc.h>
#include <keypad_4x4.h>
#include <T_18B20.h>
#include <EEP24C32.h>	// In TinyRTC board there is a EEPROM to use
#include <debug_frmwrk.h>
#include <timer32_lpc11xx.h>
#include <rtc_shell.h>
#include <ssd1306.h>
#include <string.h>
#include <oled/font_OCR_Extended.h>
#include <oled/font_TinyFont.h>


// special character on LCD defines
#define sfaranheit			0x00
#define sCelcius			0x01
#define sAM_indicator		0x02
#define sPM_indicator		0x03
#define sDegrees			0x04
#define sBars				0x05
#define sAntenna			0x06
#define sBattery			0x07

// Menu lines
const char Menu[] = {"Menu - press *"};
const char MenuLine1[] = {"1-SetTime 2-SetDate"};
const char MenuLine2[] = {"3-Celcius 4-Farenht"};
const char MenuLine3[] = {"5-12hour  6-24hour "};

// GLobal rtc_shell variables
extern void ResetISR(void);
DS18b20_t temp;
uint8_t key;
int lasttemp = 0;
int8_t lasthour = 0;
int8_t lastminute = 0;
uint8_t lasthourtype = 0;
uint8_t last_ampm = 0;
RTC_s rtc;
FONTCFG_Typedef font;
uint8_t prtbuf[64];
uint8_t degree_char;


typedef void (*menuoption) (void* args);	// Function prototype for menu to execute

typedef struct
{
	uint8_t opt_selected;
	uint8_t menu_level;

}MENU_s;

void rtc_hwd_setup(void)
{
	debug_frmwrk_init(115200);
	DS1307_Init(ENABLE);
	kp_Init();
	DS18B20Init(&temp);
    temp.Temperature_s.Temp_Type = CELSIUS;
    degree_char = 'c';
    delay32Ms(0,100);
	GLCD_SSD1306Init(DISABLE);
}

Status rtc_read_ram(uint8_t addr , int8_t len)
{
	uint8_t rxbuf[RAM_SIZE] = {0};
	int8_t counter = 0;

	DS1307SaveByte(0x08, rxbuf , 55);


	DS1307GetByte(addr , rxbuf , len);
    xprintf (  "\n\nRTC DS1307 RAM Full %d bytes read", len);
    xputs   (  "\n----------------------------------------------------");
    xputs   (  "\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
    xputs   (  "\n====================================================\n" );

    for (counter = 0; counter < (len); counter++)
    {
         if (!(counter & 0x0F))
         {
        	xputc('\n');
         	UARTPutHex( addr+counter );
         	xputc(' ');
         }
         xputc( rxbuf[counter]);
         xputc(' ');
     }
    return SUCCESS;

}


void rtc_display_time_date(void)
{
	DS18B20GetTemperature(&temp);

	if (lasttemp != temp.Temperature_s.Temp_Whole)
	{
		//xsprintf((char*)prtbuf , "temp:%d%c", temp.Whole,degree_char);
		//GLCD_putString(1,10,prtbuf,OLED_COLOR_WHITE,OLED_COLOR_BLACK,&font);
    	lasttemp = temp.Temperature_s.Temp_Whole;
	}
	DS1307GetDateTime(&rtc);
	if ( lasthour != rtc.hour || lastminute != rtc.min ||
			lasthourtype != rtc.mode_12h_24h || last_ampm != rtc.am_pm)
	{
		//Time
		GLCD_FontSetup(OCR_A_Extended_M , &font ,  OCR_A_Extended_M_param);
		xsprintf((char*)prtbuf , "%d:%d",rtc.hour,rtc.min);
		GLCD_putString(1,1,prtbuf,OLED_COLOR_WHITE , OLED_COLOR_BLACK , &font);

		GLCD_FontSetup(TinyFont , &font ,  TinyFont_Param);
		GLCD_putString(104,20,rtc.str_am_pm,OLED_COLOR_WHITE,OLED_COLOR_BLACK,&font);
		GLCD_putString(104,29,rtc.str_dow,OLED_COLOR_WHITE,OLED_COLOR_BLACK,&font);



		//date
		xsprintf(prtbuf , "%d/%d/%d%d %s" , rtc.date,rtc.month,rtc.century,rtc.year,rtc.str_month);
		GLCD_putString(1,45,prtbuf,OLED_COLOR_WHITE,OLED_COLOR_BLACK,&font);

		lasthour = rtc.hour;
		lastminute = rtc.min;
		lasthourtype = rtc.mode_12h_24h;
		last_ampm = rtc.am_pm;
	}
	switch (kp_GetKeyPressed())
	{
	case 'A':
		DS1307SwitchHourMode(HOUR_MODE_24H);
		break;
	case 'B':

		DS1307SwitchHourMode(HOUR_MODE_12H);

		break;
	case 'C':

		DS1307ChangeAMPM(HOUR_12_AM);	// Change to AM

		break;
	case 'D':

		DS1307ChangeAMPM(HOUR_12_PM);	// Change to PM

		break;
	case '0':
		temp.Temperature_s.Temp_Type = CELSIUS;
		degree_char = 'c';
		break;
	case '*':
		NVIC_SystemReset();
		break;
	case '#':
		temp.Temperature_s.Temp_Type = FARENHEIT;
		degree_char = 'f';
		break;
	default:

		break;
	}

}


void rtc_time_init(void)
{
	// init time and date
	rtc.sec = 30;
	rtc.min = 46;
	rtc.hour = 2;
	rtc.dow = Thursday;
	rtc.date = 15;
	rtc.month = 3;
	rtc.year = 16;
	rtc.am_pm = HOUR_12_PM;
	rtc.mode_12h_24h = HOUR_MODE_12H;

	DS1307SetTime(&rtc);

	DS1307SetDate(&rtc);

	//rtc_read_ram(0x08 , 55);

}

void rtc_LCDMenu(void)
{
	LCD1602_SetCursorPos(1,2);
	LCD1602_Printrs(MenuLine1);
	LCD1602_SetCursorPos(1,3);
	LCD1602_Printrs(MenuLine2);
	LCD1602_SetCursorPos(1,4);
	LCD1602_Printrs(MenuLine3);

}
