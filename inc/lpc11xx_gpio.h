/*
 * lpc11xx_gpio.h
 *
 *  Created on: Oct 3, 2016
 *      Author: dsolano
 */

#ifndef INC_LPC11XX_GPIO_H_
#define INC_LPC11XX_GPIO_H_

#define PORT0		0
#define PORT1		1
#define PORT2		2
#define PORT3		3
static LPC_GPIO_TypeDef (* const LPC_GPIO[4]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3 };
void 	PIOINT0_IRQHandler(void);
void 	PIOINT1_IRQHandler(void);
void 	PIOINT2_IRQHandler(void);
void 	PIOINT3_IRQHandler(void);
void 	GPIOInit( void );
void 	GPIOSetInterrupt( uint32_t portNum, uint32_t bitPosi, uint32_t sense,uint32_t single, uint32_t event );
void 	GPIOIntEnable( uint32_t portNum, uint32_t bitPosi );
void 	GPIOIntDisable( uint32_t portNum, uint32_t bitPosi );
uint32_t GPIOIntStatus( uint32_t portNum, uint32_t bitPosi );
void 	GPIOIntClear( uint32_t portNum, uint32_t bitPosi );
uint32_t GPIOGetValue( uint32_t portNum, uint32_t bitPosi );
void 	GPIOSetValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal );
void 	GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir );

#endif /* INC_LPC11XX_GPIO_H_ */
