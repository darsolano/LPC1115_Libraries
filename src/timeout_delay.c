/*
 * timeout_delay.c
 *
 *  Created on: Jun 27, 2020
 *      Author: dsola
 */
#include <chip.h>

// ****************
// _delay - creates a delay of the appropriate number of ticks (happens every 1us)
void _delay_uS (uint32_t delayTicks) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CT32B0);
	LPC_TIMER32_0->CTCR = 0x0;
	LPC_TIMER32_0->PR = SystemCoreClock/1000000;
	LPC_TIMER32_0->TCR = 0x02;	//reset timer match and counter and prescaler
	LPC_TIMER32_0->TCR = 0x01;	// Enable timer0
	while (LPC_TIMER32_0->TC < delayTicks);
	LPC_TIMER32_0->TCR = 0;	// timer0 disable
}

// ****************
// _delay - creates a delay of the appropriate number of ticks (happens every 1ms)
void _delay_ms (uint32_t delayTicks) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CT32B0);
	LPC_TIMER32_0->CTCR = 0x0;
	LPC_TIMER32_0->PR = SystemCoreClock/1000;
	LPC_TIMER32_0->TCR = 0x02;	//reset timer match and counter and prescaler
	LPC_TIMER32_0->TCR = 0x01;	// Enable timer0
	while (LPC_TIMER32_0->TC < delayTicks);
	LPC_TIMER32_0->TCR = 0;	// timer0 disable
}



/*
 * Setup the timer to start counting @ specified freq.
 * 1000 = milliseconds, 1000000 = microseconds
 */
void timeout_timer_init(uint32_t freq)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CT32B1);

	LPC_TIMER32_1->CTCR = 0x0;
	LPC_TIMER32_1->PR = SystemCoreClock / freq;//PRESCALE; //Increment TC at every 24999+1 clock cycles
	LPC_TIMER32_1->TCR = 0x02; //Reset Timer
}

/*
 * Enable timer and start counting the specified freq.
 */
void timeout_start(void)
{
	LPC_TIMER32_1->TCR = 0x02; //Reset Timer
	LPC_TIMER32_1->TCR = 0x01; //Enable timer
}

/*
 * Stop the timer and returns the value of the counter
 * at stop time, the you use the getTimer0_counter to verify
 * the counting time desired for your app
 */
uint_fast32_t timeout_stop(void)
{
	LPC_TIMER32_1->TCR = 0x00; //Disable timer
	return LPC_TIMER32_1->TC;
}

/*
 * Get the TC timer counter value for your app
 */
uint_fast32_t timeout_get_curr_value(void)
{
	return LPC_TIMER32_1->TC;
}

