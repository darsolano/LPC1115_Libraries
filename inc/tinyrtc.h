/*
 * tinyrtc.h
 *
 *  Created on: Aug 4, 2014
 *      Author: dsolano
 */

#ifndef TINYRTC_H_
#define TINYRTC_H_

#include  <LPC11xx.h>
#include  <lpc_types.h>


//********************************************************
//Definition for DS1307Z 64 x 8, Serial, I2C Real-Time Clock + 24C32WP + 18B09 1 wire
//********************************************************

// I2C addres for DS1307
#define DS1307_I2C_ADDR             0x68
#define DS1307_I2C_BUS              LPC_I2C		// AS we will I2C0, we must PinMux the locations and Config Pads in the Init routine

#define RAM_START	0x08
#define RAM_END		0x3f
#define RAM_SIZE	(RAM_END-RAM_START)

// Days Of the Week
typedef enum Days{
    Sunday      =   1,
    Monday      =   2,
    Tuesday     =   3,
    Wednesday   =   4,
    Thursday    =   5,
    Friday      =   6,
    Saturday    =   7
}e_DaysOfWeek_Type;

// Control Register of DS1307
typedef enum Register_Addr{
	Secs_Reg = 0,
	Mins_Reg = 1,
	Hour_Reg = 2,
	Day_Reg = 3,
	Date_Reg = 4,
	Month_Reg = 5,
	Year_Reg = 6,
	Control_Reg = 7
}e_DS1307_REG_Type;


#define CONTROL_SQWE			(1<<4)
#define CONTROL_OUT				(1<<7)
#define CONTROL_RS_1Hz			(0x0<<0)
#define CONTROL_RS_4_096kHz		(0x1<<0)
#define CONTROL_RS_8_192kHz		(0x2<<0)
#define CONTROL_RS_32_768kHz	(0x3<<0)

#define HOUR_PM			0x20
#define HOUR_AM			0xdf
#define MODE_12H		0x40
#define MODE_24h		0xbf

#define HOUR_12_MASK	0x1f
#define HOUR_24_MASK	0x3f
#define HOUR_12_AM		0
#define HOUR_12_PM		1
#define HOUR_MODE_12H	1
#define HOUR_MODE_24H	0

#define RTC_DISABLE		(1<<7)
#define RTC_ENABLE		(0<<7)
#define CENTURY			20


typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t dow;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t am_pm;
	uint8_t mode_12h_24h;
	int8_t century;
	uint8_t *str_am_pm;
	uint8_t *str_dow;
	uint8_t *str_month;
}RTC_s;

/* Function Prototypes*/
//********DS1307 RTC I2C Address 0x68

void 	DS1307_Init(FunctionalState I2CINIT);
void	DS1307_DeInit(void);
Status  DS1307SaveByte(uint8_t address, uint8_t* buff , int8_t len);
Status  DS1307GetByte(uint8_t address, uint8_t* buff , int8_t len);
Status 	DS1307SetDate(RTC_s *date);
Status 	DS1307SetTime(RTC_s *time);
Status  DS1307GetDateTime(RTC_s* rtc);
Status  DS1307SwitchHourMode(uint8_t hour_mode);
Status DS1307ChangeAMPM(uint8_t ampm);
uint8_t  bcd2bin(uint8_t bcd_value);
uint8_t  bin2bcd(uint8_t binary_value);


#endif /* TINYRTC_H_ */
