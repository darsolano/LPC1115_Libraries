/****************************************************************************
 *   $Id:: wdt.c 9386 2012-04-19 23:09:38Z nxp41306                         $
 *   Project: NXP LPC11xx WDT(Watchdog timer) example
 *
 *   Description:
 *     This file contains WDT code example which include WDT
 *     initialization, WDT interrupt handler, and APIs for WDT
 *     reading.
 *
 ****************************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.

* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
****************************************************************************/
#include "chip.h"			/* LPC11xx Peripheral Registers */
#include <watchdog.h>
#include <debug_frmwrk.h>
#include <tinyrtc.h>

volatile uint32_t wdt_counter;

// Hard Fault handler in C, with stack frame location and LR value
// extracted from the assembly wrapper as input parameters
void WatchDog_handler_c(unsigned int * hardfault_args, unsigned lr_value)
{
	LPC_WWDT->MOD &= ~WDTOF;		/* clear the time-out flag and interrupt flag */
	LPC_WWDT->MOD &= ~WDINT;		/* clear the time-out flag and interrupt flag */
	LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
	LPC_WWDT->FEED = 0x55;

	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;
	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);
	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);
	xprintf ("\n\n[**WatchDog Handler Report**]\n");
	xprintf ("R0 = %x\n", stacked_r0);
	xprintf ("R1 = %x\n", stacked_r1);
	xprintf ("R2 = %x\n", stacked_r2);
	xprintf ("R3 = %x\n", stacked_r3);
	xprintf ("R12 = %x\n", stacked_r12);
	xprintf ("Stacked LR = %x\n", stacked_lr);
	xprintf ("Stacked PC = %x\n", stacked_pc);
	xprintf ("Stacked PSR = %x\n", stacked_psr);
	xprintf ("Current LR = %x\n", lr_value);
}



/*****************************************************************************
** Function name:		WDTHandler
**
** Descriptions:		Watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void WDT_IRQHandler(void)
{
	// Hard Fault handler wrapper in assembly
	// It extracts the location of stack frame and passes it to handler
	// in C as a pointer. We also extract the LR value as second
	// parameter.
	__asm__("MOVS r0, #4\n");
	__asm__("MOV r1, LR\n");
	__asm__("TST r0, r1\n");
	__asm__("BEQ stacking_used_MSP\n");
	__asm__("MRS R0, PSP\n");	//first parameter - stacking was using PSP
	__asm__("B get_LR_and_branch\n");
	__asm__("stacking_used_MSP:\n");
	__asm__("MRS R0, MSP\n"); //first parameter - stacking was using MSP
	__asm__("get_LR_and_branch:\n");
	__asm__("MOV R1, LR\n");	//second parameter is LR current value
	__asm__("LDR R2,=WatchDog_handler_c\n");
	__asm__("BX R2\n");
	xprintf("\nWatchdog Interrupt Warning Window Report\n");
	LPC_WWDT->MOD &= ~WDTOF;		/* clear the time-out flag and interrupt flag */
	LPC_WWDT->MOD &= ~WDINT;		/* clear the time-out flag and interrupt flag */
	LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
	LPC_WWDT->FEED = 0x55;
}

/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**						watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void WDTInit( uint32_t interval )
{
	uint32_t wdt_clock;

	/* Enable clock to WDT */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_WDT);
	LPC_SYSCTL->PDRUNCFG &= ~(0x1<<6);

	LPC_SYSCTL->WDTCLKSEL = PCLK;		/* Select watchdog osc as PCLK */
	LPC_SYSCTL->WDTCLKDIV = 10;			/* Divided by 128 */
	LPC_SYSCTL->WDTCLKUEN = 0x01;		/* Update clock */
	LPC_SYSCTL->WDTCLKUEN = 0x00;		/* Toggle update register once */
	LPC_SYSCTL->WDTCLKUEN = 0x01;
	while ( !(LPC_SYSCTL->WDTCLKUEN & 0x01) );		/* Wait until updated */

	wdt_clock = (((SystemCoreClock / LPC_SYSCTL->WDTCLKDIV) / 4) / 1000) * interval;

	LPC_WWDT->TC = wdt_clock;	// this calc is in milliseconds
	//LPC_WWDT->WARNINT = 0x3ff;
	NVIC_EnableIRQ(WDT_IRQn);

	/* For WDRESET test */
	LPC_WWDT->MOD = WDEN | WDRESET;

	LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
	LPC_WWDT->FEED = 0x55;

	return;
}


/*****************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void WDTFeed( void )
{
  LPC_WWDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WWDT->FEED = 0x55;
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
