/*
 * tinyrtc.c
 *
 *  Created on: Aug 4, 2014
 *      Author: dsolano
 */

#include <tinyrtc.h>
#include <timeout_delay.h>
#include <string.h>


// private functions
static void 	DS1307InitI2C(void);
static void    	StartRTC(void);
static void    	StopRTC(void);
// end of private functions

Status				status;
const char  *dayofweek[7] = {"sun","mon","tue","wed","thu","fri","sat"};
const char  *hourmode[2]  = {"am","pm"};
const char  *month_str[12] = {"ene","feb","mar","abr","may","jun","jul","ago","sep","oct","nov","dic"};

//*********************************************************
// Local Functions
//*********************************************************

static void DS1307InitI2C(void){

	// Set the I2C initialization
	Chip_I2C_Init(DS1307_I2C_BUS);
	Chip_I2C_SetClockRate(DS1307_I2C_BUS, 100000);
	Chip_I2C_SetMasterEventHandler(DS1307_I2C_BUS, Chip_I2C_EventHandlerPolling);
}

static void StartRTC(){
	/* Sets data to be send to RTC to init*/
	uint8_t tmp;
	uint8_t buff[2] = {0};
	Chip_I2C_MasterCmdRead(DS1307_I2C_BUS, DS1307_I2C_ADDR, Secs_Reg, buff, 1);
	tmp = buff[0];
	if (tmp & 0x80)
	{
		uint8_t buff[2] = {0};
		buff[0] = Secs_Reg;	// Address for minutes register
		buff[1] &= (0x7f & tmp);
		// Send data to I2C
		Chip_I2C_MasterSend(DS1307_I2C_BUS, DS1307_I2C_ADDR, buff, 2);
	}
}


static void StopRTC(){
	/* Sets data to be send to RTC to init*/
	uint8_t buff[2] = {0};
	buff[0] = Secs_Reg;	// Address for minutes register
	buff[1] |= RTC_DISABLE;

	// Send data to I2C
	Chip_I2C_MasterSend(DS1307_I2C_BUS, DS1307_I2C_ADDR, buff, 2);
}
//*********************************************************
// End Local Functions
//*********************************************************




//*********************************************************
// Public Functions
//*********************************************************
//Properly init DS1307 I2C RTC, Packed BCD
// I2CINIT = disable no init for i2c is done
void DS1307_Init(FunctionalState I2CINIT){
	uint8_t buff[2] = {0};
	if (I2CINIT) DS1307InitI2C();			//Init I2C for TinyRTC
	/* Sets data to be send to RTC to init*/

	buff[0] = Control_Reg;
	buff[1] |= (CONTROL_OUT | CONTROL_SQWE) & CONTROL_RS_1Hz;
	Chip_I2C_MasterSend(DS1307_I2C_BUS, DS1307_I2C_ADDR, buff, 2);

	StartRTC();
}

void	DS1307_DeInit(void)
{
	StopRTC();
}


Status DS1307ChangeAMPM(uint8_t ampm)
{
	uint8_t hour[2] = {0};
	uint8_t hour_tmp;
	Chip_I2C_MasterCmdRead(DS1307_I2C_BUS, DS1307_I2C_ADDR, Hour_Reg, hour, 1);
	if (!(hour[0] & MODE_12H)) return ERROR;
	hour_tmp = hour[0] & HOUR_12_MASK;
	switch (ampm)
	{
		case (HOUR_12_PM):	// Change to PM
			if (!(hour[0] & HOUR_PM)) hour[1] = MODE_12H | hour_tmp | HOUR_PM;	 // Change to PM
			else goto error;
			break;
		case (HOUR_12_AM):	// Change to AM
			if (hour[0] & HOUR_PM) hour[1] = (MODE_12H | hour_tmp) & HOUR_AM; // Change to AM
			else goto error;
			break;
		default:
			return ERROR;
			break;
	}
	hour[0] = Hour_Reg;
	Chip_I2C_MasterSend(DS1307_I2C_BUS, DS1307_I2C_ADDR, hour, 2);
	return SUCCESS;
error:
	return ERROR;
}

Status  DS1307GetDateTime(RTC_s* rtc)
{
	uint8_t buff[7] = {0};
	if (rtc == 0) return ERROR;
	//Set sequence to get time data
	if (Chip_I2C_MasterCmdRead(DS1307_I2C_BUS, DS1307_I2C_ADDR, Secs_Reg, buff, 7)) return ERROR;

	rtc->sec  = bcd2bin(buff[0]);
	rtc->min  = bcd2bin(buff[1]);

	// Hour definition values
	if (buff[2] & MODE_12H)
	{
		rtc->hour = bcd2bin(buff[2] & HOUR_12_MASK);
		rtc->mode_12h_24h = HOUR_MODE_12H;

		if (buff[2] & HOUR_PM)
		{
			rtc->am_pm = HOUR_12_PM;
			rtc->str_am_pm = (uint8_t*) hourmode[rtc->am_pm];
		}
		else
		{
			rtc->am_pm = HOUR_12_AM;
			rtc->str_am_pm = (uint8_t*) hourmode[rtc->am_pm];
		}
	}
	else
	{
		rtc->hour = bcd2bin(buff[2] & HOUR_24_MASK);
		rtc->mode_12h_24h = HOUR_MODE_24H;
	}
	rtc->dow = bcd2bin(buff[3]);
	rtc->str_dow = (uint8_t*) dayofweek[rtc->dow-1];
	rtc->date = bcd2bin(buff[4]);
	rtc->month = bcd2bin(buff[5]);
	rtc->str_month = (uint8_t*) month_str[rtc->month-1];
	rtc->year = bcd2bin(buff[6]);
	rtc->century = CENTURY;
	return (SUCCESS);
}



/*
 * Set date to TinyRTC DS1307
 * remember to send the DOW (Day of Week) first
 * Date
 * Month
 * Year
 * return the result of the I2C write
 */
Status DS1307SetDate(RTC_s *date)
{
	uint8_t buff[5] = {0};
	if (date==0) return ERROR;
	buff[0] = Day_Reg;
	buff[1] = bin2bcd(date->dow);
	buff[2] = bin2bcd(date->date);
	buff[3] = bin2bcd(date->month);
	buff[4] = bin2bcd(date->year);

	if (Chip_I2C_MasterSend(DS1307_I2C_BUS, DS1307_I2C_ADDR, buff, 5)) return ERROR;
	else return SUCCESS;
}

/*
 *
 */
Status DS1307SetTime(RTC_s *time)
{
	uint8_t buff[4] = {0};
	if (time == 0) return ERROR;
	if (Chip_I2C_MasterCmdRead(DS1307_I2C_BUS, DS1307_I2C_ADDR, Secs_Reg, buff, 3)) return ERROR;

	buff[0] = Secs_Reg;
	buff[1] = bin2bcd(time->sec) & 0x7f;
	buff[2] = bin2bcd(time->min);
	if (time->mode_12h_24h)
	{
		buff[3] = bin2bcd(time->hour & HOUR_12_MASK);
		buff[3] |= MODE_12H;
		if (time->am_pm) buff[3] |= HOUR_PM;
		else buff[3] &= HOUR_AM;
	}
	else
	{
		buff[3] = bin2bcd(time->hour & HOUR_24_MASK);
		buff[3] &=  MODE_24h;
	}
	Chip_I2C_MasterSend(DS1307_I2C_BUS, DS1307_I2C_ADDR, buff, 4);
	return SUCCESS;
}

Status  DS1307SwitchHourMode(uint8_t hour_mode)
{
	uint8_t buff[2] = {0};
	uint8_t hour_tmp;

	if (!Chip_I2C_MasterCmdRead(DS1307_I2C_BUS, DS1307_I2C_ADDR, Hour_Reg, buff, 1)) return ERROR; // get register from rtc

	if (hour_mode && (buff[0] & MODE_12H))	// If hour mode is  12hour and is already 12h do nothing and return
		return ERROR;

	else if (!(hour_mode) && !(buff[0] & MODE_12H)) // if hour mode is 24h and is already 24h, do nothing and return error
		return ERROR;


	switch (hour_mode)
	{
		case HOUR_MODE_24H:	// request to change to 24h, then the content is 12h. From 12h to 24h
			hour_tmp = bcd2bin(buff[0] & HOUR_12_MASK);	// save hour in binary
			if ((buff[0] & HOUR_PM) && (hour_tmp < 12)) // if PM and is not midnight, add 12 from hour number
				hour_tmp += 12 ;

			if (!(buff[0] & HOUR_PM) && (hour_tmp == 12)) // If AM hour and hour equals 12 midnight, change to 00
				hour_tmp = 00 ;

			buff[1] = bin2bcd(hour_tmp & HOUR_24_MASK);
			break;
		case HOUR_MODE_12H:	// request to change to 12 hour mode, the is in 24 mode. From 24h to 12h
			hour_tmp = bcd2bin(buff[0] & HOUR_24_MASK);	// save hour in binary
			if (hour_tmp > 12)
			{
				hour_tmp -= 12;
				buff[1] = bin2bcd(hour_tmp & HOUR_12_MASK) | HOUR_PM | MODE_12H;
			}
			else if (hour_tmp < 12)
			{
				if (hour_tmp == 00) hour_tmp = 12;
				buff[1] = (bin2bcd(hour_tmp & HOUR_12_MASK) | MODE_12H) & HOUR_AM ;
			}
			break;
		default:
			return ERROR;
			break;
	}
	buff[0] = Hour_Reg;
	if (i2c_master_send(DS1307_I2C_ADDR, buff,2)) return ERROR;
	else return SUCCESS;
}

Status  DS1307SaveByte(uint8_t address, uint8_t* buff , int8_t len)
{
	uint8_t rxbuf[RAM_SIZE];
	if ((address < 0x08) && (address > 0x3f)) return ERROR;
	if(i2c_master_send(DS1307_I2C_ADDR , buff , len)) return ERROR;

	// read back all bytes written in order to compare and verify everything is OK
	i2c_master_cmd_read(DS1307_I2C_ADDR , address , rxbuf , len);
	while (len)	// compare buffer content
		if (rxbuf[len] != buff[len]) return ERROR;
	return SUCCESS;
}

/*
 * Retrieve bytes from RAM on RTC ds1307
 */
Status  DS1307GetByte(uint8_t address, uint8_t* buff, int8_t len)
{
	if (address < 0x08 && address > 0x3f) return ERROR;

	if (i2c_master_cmd_read(DS1307_I2C_ADDR , address , buff , len)) return ERROR;

	return SUCCESS;

}


//-------------------------------------------------------------
// This function converts an 8 bit binary value
// to an 8 bit BCD value.
// The input range must be from 0 to 99.

uint8_t bin2bcd(uint8_t binary_value){
	uint8_t temp;
	uint8_t retval;
	if (binary_value > 99) return -1;
	temp = binary_value;
	retval = 0;

	while(1){
		// Get the tens digit by doing multiple subtraction
		// of 10 from the binary value.
		if(temp >= 10){
			temp -= 10;
			retval += 0x10;
		}
		else // Get the ones digit by adding the remainder.
		{
			retval += temp;
			break;
			}
	}
	return(retval);
}

//--------------------------------------------------------------
// This function converts an 8 bit BCD value to
// an 8 bit binary value.
// The input range must be from 00 to 99.

uint8_t bcd2bin(uint8_t bcd_value){
	uint8_t temp;
	temp = bcd_value;
	// Shifting upper digit right by 1 is same as multiplying by 8.
	temp >>= 1;
	// Isolate the bits for the upper digit.
	temp &= 0x78;
	// Now return: (Tens * 8) + (Tens * 2) + Ones
	return(temp + (temp >> 2) + (bcd_value & 0x0f));
}

/*
 * Convert BCD to binary simple code
 * Number from 00 to 99
 */
uint8_t convert_bcd_to_bin(uint8_t bcd_value)
{
	if (bcd_value > 0x99) return 0;
	return (((bcd_value & 0xf0) >> 4) * 10) + (0x0f & bcd_value);
}

//*********************************************************
// End Public Functions
//*********************************************************
