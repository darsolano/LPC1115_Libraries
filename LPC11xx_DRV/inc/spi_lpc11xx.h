/*
 * spi_lpc11xx.h
 *
 *  Created on: Feb 20, 2015
 *      Author: dsolano
 */

#ifndef SPI_LPC11XX_H_
#define SPI_LPC11XX_H_

#include <LPC11xx.h>
#include <lpc_types.h>

typedef enum
{
	SSP0 = 0,
	SSP1 = 1
}SSP_MODULE_e;

#define SPI_STAT_TFE			0x01
#define SPI_STAT_TNF			0x02
#define SPI_STAT_RNE			0x04
#define SPI_STAT_RFF			0x08
#define SPI_STAT_BSY			0x10


Status spiinit(SSP_MODULE_e module , uint32_t rate);
uint8_t spirw(SSP_MODULE_e module, uint8_t data);

#endif /* SPI_LPC11XX_H_ */
