
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

//*****************************************************************
//            JUST FOR INFORMATION -- IMPORTANT TO KNOW
//*****************************************************************
/** \ingroup  CMSIS_core_register
    \defgroup CMSIS_SysTick     System Tick Timer (SysTick)
    \brief      Type definitions for the System Timer Registers.
  @{
 */

/** \brief  Structure type to access the System Timer (SysTick).
 */

//typedef struct
//{
//  __IO uint32_t CTRL;                    /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
//  __IO uint32_t LOAD;                    /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register       */
//  __IO uint32_t VAL;                     /*!< Offset: 0x008 (R/W)  SysTick Current Value Register      */
//  __I  uint32_t CALIB;                   /*!< Offset: 0x00C (R/ )  SysTick Calibration Register        */
//} SysTick_Type;
