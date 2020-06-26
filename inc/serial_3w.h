/*
 * serial_3w.h
 *
 *  Created on: Dec 4, 2015
 *      Author: dsolano
 */

#ifndef INC_SERIAL_3W_H_
#define INC_SERIAL_3W_H_

#include <lpc_types.h>
#include <LPC11xx.h>


#define SERIAL_3_WIRE	LPC_GPIO2

typedef enum
{
	LOW = 0,
	HIGH
}LEVEL_e;

typedef enum
{
	INPUT = 0,
	OUTPUT = 1
}DIR_e;

#define Bit(n)		(1<<n)

/*
 * CS  - PIN 2.8
 * CLK - PIN 2.9
 * DOUT - PIN 2.10
 */
#define CS_MASK		(1<<8)
#define CLK_MASK	(1<<9)
#define DOUT_MASK	(1<<10)

#define CS_ON()				SERIAL_3_WIRE->DATA &= ~CS_MASK
#define CS_OFF()			SERIAL_3_WIRE->DATA |= CS_MASK	// same as above
#define SERIAL_CLK(x)		(x == 0) ? (SERIAL_3_WIRE->DATA &= ~CLK_MASK) :(SERIAL_3_WIRE->DATA |= CLK_MASK)
#define SERIAL_DOUT(x)		(x == 0) ? (SERIAL_3_WIRE->DATA &= ~DOUT_MASK):(SERIAL_3_WIRE->DATA |= DOUT_MASK)

void serial3wire_init(void);
void serial3wire_write_byte(uint8_t data);
void serial3wire_write_short(uint16_t data);

#endif /* INC_SERIAL_3W_H_ */
