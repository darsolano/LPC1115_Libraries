/*
 * serial_3w.c
 *
 *  Created on: Dec 4, 2015
 *      Author: dsolano
 */

#include <lpc11xx_syscon.h>
#include <serial_3w.h>
#include <timer32_lpc11xx.h>




void serial3wire_init(void)
{
	syscon_PeripheralClock(AHB_IOCON,ENABLE);
	syscon_PeripheralClock(AHB_GPIO,ENABLE);
	SERIAL_3_WIRE->DIR |= CS_MASK;	// CS DIR
	SERIAL_3_WIRE->DIR |= CLK_MASK;	// CLK
	SERIAL_3_WIRE->DIR |= DOUT_MASK;	// DOUT		Serial Data OUT
	CS_OFF();
	SERIAL_CLK(SET);
	SERIAL_DOUT(RESET);
}

// SPI BitBang to send data 8 bits
void serial3wire_write_byte(uint8_t data)
{
	uint8_t temp , count;

	temp=data;
	CS_ON();
	SERIAL_CLK(SET);
	for( count=0; count<8; count++)
	{
		if(temp & 0x80)
			SERIAL_DOUT(SET);
		else
			SERIAL_DOUT(RESET);
		temp = temp << 1;
		// pseudo clock
		SERIAL_CLK(RESET);
		__NOP();
		SERIAL_CLK(SET);
		__NOP();
		SERIAL_CLK(RESET);
		__NOP();
	}
	CS_OFF();
}

// SPI BitBang to send data 16 bits
void serial3wire_write_short(uint16_t data)
{
	uint16_t temp , count;

	temp=data;
	CS_ON();
	SERIAL_CLK(SET);
	for( count=0; count<16; count++)
	{
		if(temp & 0x8000)
			SERIAL_DOUT(SET);
		else
			SERIAL_DOUT(RESET);
		temp = temp << 1;
		// pseudo clock
		SERIAL_CLK(RESET);
		__NOP();
		SERIAL_CLK(SET);
		__NOP();
		SERIAL_CLK(RESET);
		__NOP();
	}
	CS_OFF();
}

