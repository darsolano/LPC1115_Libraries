
#include <systick_delay.h>

volatile uint32_t usTicks; // counter for 1ms SysTicks

// ****************
//  SysTick_Handler - just increment SysTick counter
void SysTick_Handler(void) {
  usTicks++;
}

// ****************
// systick_delay - creates a delay of the appropriate number of Systicks (happens every 1 us)
__INLINE void systick_delay (uint32_t delayTicks) {
  uint32_t currentTicks;

  currentTicks = usTicks;	// read current tick counter
  // Now loop until required number of ticks passes.
  while ((usTicks - currentTicks) < delayTicks);
}

//****************
// enable systick interrupt at interval set by SYSTICKxxxx
void systick_cfg(void){
	if (SysTick_Config(SystemCoreClock / SYSTICKS)) {	// Configure SYS TICK for 1uS or 1ms interrupt
		while (1);  // Capture error

	}
}

