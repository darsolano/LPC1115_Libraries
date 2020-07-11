/*
 * i2cscanner.c
 *
 *  Created on: Aug 6, 2014
 *      Author: dsolano
 */

#include  <i2cscanner.h>
#include  <timeout_delay.h>
#include  <debug_frmwrk.h>

#define SPC					0x20
#define ENTER				0x0d

int status;
uint8_t buff[3];

//*********************************************************
// Local Functions
//*********************************************************

/* I2C Write Data*/
static int I2CWriteData(uint8_t* buffer, uint32_t len, uint32_t addr) {
	/* Sets data to be send to RTC to init*/
	I2C_XFER_T i2ctx;	//Data structure to be used to send byte thru I2C Master Data Transfer
	// Fill Data Structure with proper data
	i2ctx.rxBuff = 0;
	i2ctx.rxSz = 0;
	i2ctx.slaveAddr = addr;
	i2ctx.txBuff = buffer;
	i2ctx.txSz = len;
	// Send data to I2C
	status = Chip_I2C_MasterTransfer(I2C0, &i2ctx);
	return status;
}
//*********************************************************
// End of Local Functions
//*********************************************************

//*********************************************************
// Public Functions
//*********************************************************


/* Function that probes all available slaves connected to an I2C bus */
void i2cscan(void)
{
	uint8_t i;
	xputs("\nNXP/Embbeded Artist LPCXpresso 1115 Cortex M0...\n");
	xputs("Probing available I2C devices on LPC Bus: 0\n");

	xputs("\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	xputs("\n====================================================");
	for (i = 0; i <= 0x7F; i++) {
		if (!(i & 0x0F)){
			xprintf("\n %2X " , i >> 4);
		}
		if (i <= 7 || i > 0x78) {
			xputs("   ");
			continue;
		}
		// Sets buffer to send
		buff[0] = 1;
		status = I2CWriteData(buff, 1, i);
		if(status == SUCCESS){
			xprintf(" %2X" , i);
		}
		else
			xputs(" --");
		_delay_ms(1);
	}
	xputs("\r\n");
}
