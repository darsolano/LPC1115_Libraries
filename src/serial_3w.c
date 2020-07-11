/*
 * serial_3w.c
 *
 *  Created on: Dec 4, 2015
 *      Author: dsolano
 */

#include <serial_3w.h>
#include <define_pins.h>

#define SERIAL_3_WIRE	LPC_GPIO[2]
#define Bit(n)		(1<<n)

/*
 * CS   - PIN 2.8
 * CLK  - PIN 2.9
 * DOUT - PIN 2.10
 */
#define CS_MASK		(1<<8)
#define CLK_MASK	(1<<9)
#define DOUT_MASK	(1<<10)

//#define CS_ON()				SERIAL_3_WIRE->DATA[CS_MASK] &= ~CS_MASK
//#define CS_OFF()			SERIAL_3_WIRE->DATA |= CS_MASK	// same as above
//#define SERIAL_CLK(x)		(x == 0) ? (SERIAL_3_WIRE->DATA &= ~CLK_MASK) :(SERIAL_3_WIRE->DATA |= CLK_MASK)
//#define SERIAL_DOUT(x)		(x == 0) ? (SERIAL_3_WIRE->DATA &= ~DOUT_MASK):(SERIAL_3_WIRE->DATA |= DOUT_MASK)

DEFINE_PIN(CS, 2, 8)
DEFINE_PIN(SERIAL_CLK, 2, 9)
DEFINE_PIN(SERIAL_DOUT, 2, 10)


void serial3wire_init(void)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
	CS_OUTPUT();
	SERIAL_CLK_OUTPUT();
	SERIAL_DOUT_OUTPUT();
	CS_OFF();
	SERIAL_CLK_ON();
	SERIAL_DOUT_LOW();
}

// SPI BitBang to send data 8 bits
void serial3wire_write_byte(uint8_t data)
{
	uint8_t temp , count;

	temp=data;
	CS_SELECTED();
	SERIAL_CLK_ASSERT();
	for( count=0; count<8; count++)
	{
		if(temp & 0x80)
			SERIAL_DOUT_HIGH();
		else
			SERIAL_DOUT_LOW();
		temp = temp << 1;
		// pseudo clock
		SERIAL_CLK_DEASSERT();
		__NOP();
		SERIAL_CLK_ASSERT();
		__NOP();
		SERIAL_CLK_DEASSERT();
		__NOP();
	}
	CS_DESELECTED();
}

// SPI BitBang to send data 16 bits
void serial3wire_write_short(uint16_t data)
{
	uint16_t temp , count;

	temp=data;
	CS_SELECTED();
	SERIAL_CLK_ASSERT();
	for( count=0; count<16; count++)
	{
		if(temp & 0x8000)
			SERIAL_DOUT_HIGH();
		else
			SERIAL_DOUT_LOW();
		temp = temp << 1;
		// pseudo clock
		SERIAL_CLK_DEASSERT();
		__NOP();
		SERIAL_CLK_ASSERT();
		__NOP();
		SERIAL_CLK_DEASSERT();
		__NOP();
	}
	CS_DESELECTED();
}

