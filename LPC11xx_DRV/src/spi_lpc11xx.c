/*
 * spi_lpc11xx.c
 *
 *  Created on: Feb 20, 2015
 *      Author: dsolano
 */

#include <spi_lpc11xx.h>

Status spiinit(uint8_t module, uint32_t rate)
{
	/*
	 * Initialization for SSP device and CE and CSN pins
	 */
	syscon_PeripheralClock(AHB_IOCON , ENABLE);
	syscon_PeripheralClock(AHB_GPIO , ENABLE);

	if (module == SSP0)
	{
		LPC_SYSCON->SSP0CLKDIV = 0x01; // Activate clock divisor by 1 same as SYSCORECLOCK
		syscon_PeripheralClock(AHB_SSP0 , ENABLE); // Activate clock for SSP0 module
		LPC_SYSCON->PRESETCTRL |= (1 << 0);
		// SPI 0 Pin Selection
		LPC_IOCON->PIO0_8 = (1 << 0);	// MISO_0
		LPC_IOCON->PIO0_9 = (1 << 0);	// MOSI_0
		LPC_IOCON->SCK_LOC = (1 << 0);	// SCK0 mapped on pin 2.11
		LPC_IOCON->PIO2_11 = (1 << 0);	// SCLK0 pin function for 2.11
		LPC_IOCON->PIO0_2 = (0 << 0) | (2 << 3);// CS gpio + pullup resistor as 1 is normal state
		//********************
		LPC_SSP0->CPSR = 2;				// bitrate clock
		LPC_SSP0->CR0 = 0x0707; //<< 0); 	// 8bit As SPI, CPOL,CPHA = 0
		if (rate < 15000000) LPC_SSP0->CR0 |= (((SystemCoreClock / (2*rate)) - 1) << 8);
		LPC_SSP0->CR1 &= ~(1 << 2);		// As a Master
		LPC_SSP0->CR1 |= (1 << 1);	// SPI0 Enable
	}
	else if (module == SSP1)
	{
		LPC_SYSCON->SSP1CLKDIV = 0x01; // Activate clock divisor by 1 same as SYSCORECLOCK
		syscon_PeripheralClock(AHB_SSP1 , ENABLE); // Activate clock for SSP1 module
		LPC_SYSCON->PRESETCTRL |= (1 << 2);
		// SPI 1 Pin Selection
		LPC_IOCON->PIO2_2 = 0;			// MISO1 Pin 2.2
		LPC_IOCON->MISO1_LOC = 0; 		// MISO1 Loc 2.2
		LPC_IOCON->PIO2_3 = 0;			// MOSI1 Pin 2.3
		LPC_IOCON->MOSI1_LOC = 0; 		// MOSI1 Loc 2.3
		LPC_IOCON->SCK1_LOC = 0;		// SCK1 mapped on pin 2.1
		LPC_IOCON->PIO2_1 = 2;			// SCLK1 pin function for Pin 2.1
		LPC_IOCON->PIO2_0 = (0 << 0) | (2 << 3);// CS gpio + pullup resistor as 1 is normal state
		//********************
		LPC_SSP1->CPSR = 2;	// bitrate clock
		LPC_SSP1->CR0 = (0x0707 << 0); 	// 8bit As SPI, CPOL,CPHA = 0
		//LPC_SSP1->CR0 |= (((SystemCoreClock / (2*rate)) - 1) << 8);
		LPC_SSP1->CR1 &= ~(1 << 2);		// As a Master
		LPC_SSP1->CR1 |= (1 << 1);	// SPI0 Enable

	}
	return SUCCESS;
}


uint8_t spirw(uint8_t module, uint8_t data)
{
	LPC_SSP_TypeDef* SSPx;
	if (module)
	{
		SSPx = LPC_SSP1;
	}else{
		SSPx = LPC_SSP0;
	}

	while (!(SSPx->SR & SPI_STAT_TFE));
	SSPx->DR = data;		// send a byte
	while (!(SSPx->SR & SPI_STAT_RNE));
	return SSPx->DR;		// Receive a byte
}

/* Embed: transmit and receive len bytes
 * Remember:
 *   SPI transmits and receives at the same time
 *   If tx == NULL and you are only receiving then transmit all 0xFF
 *   If rx == NULL and you are only transmitting then dump all recieved bytes
 */
void spi_txrx(uint8_t* tx, uint8_t* rx, uint16_t len)
{
	uint8_t dummy;

	while (len--)
	{
		if (tx == NULL)
		{
			LPC_SSP0->DR = 0xff;
		}
		else
		{
			LPC_SSP0->DR = *tx++;
		}

		while ((LPC_SSP0->SR & (SPI_STAT_BSY | SPI_STAT_RNE)) != SPI_STAT_RNE)
			;

		if (rx == NULL)
		{
			dummy = LPC_SSP0->DR;
		}
		else
		{
			*rx++ = LPC_SSP0->DR;
		}
	}
}

