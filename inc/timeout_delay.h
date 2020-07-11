/*
 * timeout_delay.h
 *
 *  Created on: Jun 27, 2020
 *      Author: dsola
 */

#ifndef INC_TIMEOUT_DELAY_H_
#define INC_TIMEOUT_DELAY_H_

#include <chip.h>
#include <lpc_types.h>


void _delay_uS (uint32_t delayTicks) ;
void _delay_ms (uint32_t delayTicks) ;
void timeout_timer_init(uint32_t freq);
void timeout_start(void);
uint_fast32_t timeout_stop(void);
uint_fast32_t timeout_get_curr_value(void);



#endif /* INC_TIMEOUT_DELAY_H_ */
