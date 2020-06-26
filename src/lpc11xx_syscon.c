/*
 * lpc11xx_syscon.c
 *
 *  Created on: Feb 12, 2015
 *      Author: dsolano
 */

#include <lpc11xx_syscon.h>

/*
 * Enable or Disable System Clock for indicated module
 */
void syscon_PeripheralClock(SYSAHBCLKCTRL_eType module , FunctionalState NewState)
{
	if (NewState) LPC_SYSCON->SYSAHBCLKCTRL |= (1<<module);
	else LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<module);
}

