/*
 * lpc11xx_syscon.h
 *
 *  Created on: Feb 12, 2015
 *      Author: dsolano
 */

#ifndef LPC11XX_SYSCON_H_
#define LPC11XX_SYSCON_H_

#include <LPC11xx.h>
#include <lpc_types.h>
#include <stdint.h>



// Enum to turn on peripheral clock for various modules
typedef enum
{
	AHB_SYS = 0,
	AHB_ROM = 1,
	AHB_RAM = 2,
	AHB_FLASHREG = 3,
	AHB_FLASHARRAY = 4,
	AHB_I2C = 5,
	AHB_GPIO = 6,
	AHB_CT16B0 = 7,
	AHB_CT16B1 = 8,
	AHB_CT32B0 = 9,
	AHB_CT32B1 = 10,
	AHB_SSP0 = 11,
	AHB_UART = 12,
	AHB_ADC = 13,
	AHB_RESERVED = 14,
	AHB_WDT = 15,
	AHB_IOCON = 16,
	AHB_CAN = 17,
	AHB_SSP1 = 18,
	AHB_INVALID
}SYSAHBCLKCTRL_eType;




void syscon_PeripheralClock(SYSAHBCLKCTRL_eType module , FunctionalState NewState);

#endif /* LPC11XX_SYSCON_H_ */
