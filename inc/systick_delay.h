/*
 * systick_delay.h
 *
 *  Created on: Nov 4, 2014
 *      Author: dsolano
 */

#ifndef SYSTICK_DELAY_H_
#define SYSTICK_DELAY_H_
#ifdef __cplusplus
extern "C"
{
#endif


#include <LPC11xx.h>
#include <lpc_types.h>




//#define  usTICKS
#define  msTICKS


#ifdef usTICKS
#define SYSTICKS		500000

#define dly_2us			1
#define dly_4us			2
#define dly_10us		5
#define dly_20us		10
#define dly_50us		25
#define dly_100us		50
#define dly_200us		100
#define dly_1ms			500
#define dly_5ms			dly_1ms*5
#define dly_10ms		dly_1ms*10
#define dly_50ms		dly_10ms*5
#define dly_100ms		dly_1ms*100
#define dly_200ms		dly_100ms*2
#define dly_1sec		dly_1ms*1000
#define dly_half_sec	dly_100ms*5
#endif

#ifdef msTICKS
#define SYSTICKS		1000
#define dly_1ms			1
#define dly_5ms			5
#define dly_10ms		10
#define dly_100ms		100
#define dly_1sec		1000
#define dly_half_sec	500
#endif


__INLINE void systick_delay (uint32_t delayTicks);
void systick_cfg(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTICK_DELAY_H_ */
