//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2010 Code Red Technologies Ltd.
//
// leds.c provided functions to access LED2 (P0_22) on the
// LPCXpresso1768 processor board.
//
//
// Software License Agreement
// 
// The software is owned by Code Red Technologies and/or its suppliers, and is 
// protected under applicable copyright laws.  All rights are reserved.  Any 
// use in violation of the foregoing restrictions may subject the user to criminal 
// sanctions under applicable laws, as well as to civil liability for the breach 
// of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD. 

#include "chip.h"
#include "leds.h"
#include <timeout_delay.h>


// Function to initialize GPIO to access LED2 0.7
void led2_init (void)
{
	// Enable GPIO & IOCON modules clock
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	LPC_GPIO[0].DIR |= (1<<7);	// As output
}

// Function to invert current state of LED2
void led2_toggle (void)
{
	LPC_GPIO[0].DATA[1<<7] ^= 1<<7;
}

void led2_BlinkTimes (uint8_t times)
{
	while (times--){
		led2_toggle();
		_delay_ms(250);
	}
}

