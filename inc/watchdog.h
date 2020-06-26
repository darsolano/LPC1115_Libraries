/*
 * watchdog.h
 *
 *  Created on: Mar 11, 2016
 *      Author: dsolano
 */

#ifndef INC_WATCHDOG_H_
#define INC_WATCHDOG_H_
#include <lpc_types.h>
/****************************************************************************
 *   $Id:: wdt.h 3635 2010-06-02 00:31:46Z usb00423                         $
 *   Project: NXP LPC11xx WDT example
 *
 *   Description:
 *     This file contains WDT code header definition.
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
****************************************************************************/

#define WDEN              (0x1<<0)
#define WDRESET           (0x1<<1)
#define WDTOF             (0x1<<2)
#define WDINT             (0x1<<3)
#define WDPROTECT         (0x1<<4)


typedef enum
{
	IRC = 0,
	PCLK = 1,
	WD_OSC = 3
}WDT_CLOCKSOURCE_e;

#define WDT_FEED_VALUE    	0x003FFFFF
#define WD_KickButt()		LPC_WDT->FEED = 0xAA; LPC_WDT->FEED = 0x55;

#define WINDOW_MODE       0
#define PROTECT_MODE      1
#define WATCHDOG_RESET    0

 void WDT_IRQHandler(void);
 void WDTInit( uint32_t interval );
 void WDTFeed( void );





#endif /* INC_WATCHDOG_H_ */
