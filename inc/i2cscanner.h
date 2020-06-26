/*
 * i2cscanner.h
 *
 *  Created on: Aug 6, 2014
 *      Author: dsolano
 */

#ifndef I2CSCANNER_H_
#define I2CSCANNER_H_

#include  <LPC11xx.h>
#include  <lpc_types.h>


#define I2CDEV				LPC_I2C
#define I2C_CLOCK_FREQ		400000


void i2cscan(void);

#endif /* I2CSCANNER_H_ */
