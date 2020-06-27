/*
 * timer32_lpc11xx.h
 *
 *  Created on: Nov 27, 2015
 *      Author: dsolano
 */

#ifndef INC_TIMER32_LPC11XX_H_
#define INC_TIMER32_LPC11XX_H_

/****************************************************************************
 *   $Id:: timer32.h 9370 2012-04-19 22:54:23Z nxp41306                     $
 *   Project: NXP LPC11xx software example
 *
 *   Description:
 *     This file contains definition and prototype for 32-bit timer
 *     configuration.
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
#include <lpc_types.h>
#define TIMER0	0
#define TIMER1	1

/* The test is either MAT_OUT or CAP_IN. Default is MAT_OUT. */
#define TIMER_MATCH		0

#define EMC0	4
#define EMC1	6
#define EMC2	8
#define EMC3	10

#define MATCH0	(1<<0)
#define MATCH1	(1<<1)
#define MATCH2	(1<<2)
#define MATCH3	(1<<3)

#define dly_2us			2
#define dly_4us			4
#define dly_10us		10
#define dly_20us		20
#define dly_50us		50
#define dly_100us		100
#define dly_200us		200
#define dly_1ms			1000
#define dly_5ms			dly_1ms*5
#define dly_10ms		dly_1ms*10
#define dly_50ms		dly_10ms*5
#define dly_100ms		dly_1ms*100
#define dly_200ms		dly_100ms*2
#define dly_1sec		dly_1ms*1000
#define dly_half_sec	dly_100ms*5

#ifndef TIME_INTERVAL
#define TIME_INTERVAL	(SystemCoreClock/1000000)
/* depending on the SystemFrequency and SystemAHBFrequency setting,
if SystemFrequency = 60Mhz, SystemAHBFrequency = 1/4 SystemAHBFrequency,
10mSec = 150.000-1 counts */
#endif

void delay32Ms(uint8_t timer_num, uint32_t delayInMs);
void delay32us(uint8_t timer_num, uint32_t delayInus);
void TIMER32_0_IRQHandler(void);
void TIMER32_1_IRQHandler(void);
void enable_timer32(uint8_t timer_num);
void disable_timer32(uint8_t timer_num);
void reset_timer32(uint8_t timer_num);
void init_timer32(uint8_t timer_num, uint32_t timerInterval);
void init_timer32PWM(uint8_t timer_num, uint32_t period, uint8_t match_enable);
void setMatch_timer32PWM (uint8_t timer_num, uint8_t match_nr, uint32_t value);

/*****************************************************************************
**                            End Of File
******************************************************************************/



#endif /* INC_TIMER32_LPC11XX_H_ */
