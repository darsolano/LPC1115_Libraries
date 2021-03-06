/*
 * serial_3w.h
 *
 *  Created on: Dec 4, 2015
 *      Author: dsolano
 */

#ifndef INC_SERIAL_3W_H_
#define INC_SERIAL_3W_H_

#include <lpc_types.h>
#include <chip.h>



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


void serial3wire_init(void);
void serial3wire_write_byte(uint8_t data);
void serial3wire_write_short(uint16_t data);

#endif /* INC_SERIAL_3W_H_ */
