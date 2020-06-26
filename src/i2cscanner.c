/*
 * i2cscanner.c
 *
 *  Created on: Aug 6, 2014
 *      Author: dsolano
 */

#include  <i2cscanner.h>
#include  <timer32_lpc11xx.h>
#include  <debug_frmwrk.h>
#include  <i2c_lpc11xx.h>

#define SPC					0x20
#define ENTER				0x0d

Status status;
uint8_t buff[3];

//*********************************************************
// Local Functions
//*********************************************************

/* I2C Write Data*/
static Status I2CWriteData(uint8_t* buffer, uint32_t len, uint32_t addr) {
	/* Sets data to be send to RTC to init*/
	I2C_MASTER_DATA_Typedef i2ctx;	//Data structure to be used to send byte thru I2C Master Data Transfer
	// Fill Data Structure with proper data
	i2ctx.rxbuff = 0;
	i2ctx.rxlen = 0;
	i2ctx.slv_addr = addr;
	i2ctx.txbuff = buffer;
	i2ctx.txlen = len;
	// Send data to I2C
	status = i2cmaster_data_xfer(&i2ctx);
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
		delay32Ms(TIMER0 , 1);
	}
	xputs("\r\n");
}
