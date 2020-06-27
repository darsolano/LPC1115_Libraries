/*
 * i2c_lpc11xx.c
 *
 *  Created on: Feb 15, 2015
 *      Author: dsolano
 */

#include <i2c_lpc11xx.h>
#include <lpc11xx_syscon.h>

#define STAT_REG	LPC_I2C->STAT & I2C_STAT_CODE_BITMASK

// Start I2C bus communication transfer
static I2CSTATUS i2cstart(void)
{
	I2CSTATUS status;
	LPC_I2C->CONCLR = I2C_CONCLR_SIC;		// Clear interrupt
	LPC_I2C->CONSET = I2C_CONSET_STA;		// Set start flag
	while (!(LPC_I2C->CONSET & I2C_CONSET_SI));	// Wait for Interrupt flag to turn on
	status = STAT_REG;
	LPC_I2C->CONCLR = I2C_CONCLR_SIC;
	return status;
}

// Send stop transfer
static I2CSTATUS i2cstop(void)
{
	I2CSTATUS status;
	LPC_I2C->CONCLR = I2C_CONCLR_STAC;	// Clear start bit
	LPC_I2C->CONSET |= I2C_CONSET_STO;	// Set stop bit
	while (!(LPC_I2C->CONSET & I2C_CONSET_SI));	// wait for stop to complete
	status = STAT_REG;
	LPC_I2C->CONCLR = I2C_CONCLR_SIC;	// Clear interrupt flag s Stop bit clears itself
	return status; // send back the response
}

/*
 * Send byte thru I2C
 * byte = data to be sent
 * response = status register content
 */
static I2CSTATUS i2csendbyte(uint8_t byte)
{
	LPC_I2C->CONCLR = I2C_CONCLR_STAC | I2C_CONCLR_SIC;	// ensure that no start flag and interrupt
	LPC_I2C->DAT = byte;	// send the byte
	while (!(LPC_I2C->CONSET & I2C_CONSET_SI));	// wait for data to be actually sent
	return STAT_REG;	// return stat reg content
}

/*
 *
 */
I2CSTATUS i2cgetbyte(uint8_t *byte, Bool ack)
{
	if (ack == TRUE)
		LPC_I2C->CONSET = I2C_CONSET_AA;	// to send ack after getting the byte
	else
		LPC_I2C->CONCLR = I2C_CONCLR_AAC;	// set the nack flag
	LPC_I2C->CONCLR = I2C_CONCLR_SIC;	// ensure that interrupt flag is off
	while (!(LPC_I2C->CONSET & I2C_CONSET_SI));
	*byte = (uint8_t) (LPC_I2C->DAT & 0xFF);	// get the data byte automatic will be send the ack or nack
	return STAT_REG;
}




// PUBLIC FUNCTIONS
void i2cinit(uint32_t bitrate)
{
	//0.4 SCL
	//0.5 SDA
	LPC_IOCON->PIO0_4 &= ~(0x303); //clear "FUNC" and "I2CMODE" bits (sec 7.4.11)
	LPC_IOCON->PIO0_4 |=  (2<<8) | (1<<0); // Fast mode  I2C SCL
	LPC_IOCON->PIO0_5 &= ~(0x303); //clear "FUNC" and "I2CMODE" bits (sec 7.4.12)
	LPC_IOCON->PIO0_5 |=  (2<<8) | (1<<0); // Fast mode  I2C SDA

	LPC_SYSCON->PRESETCTRL    |= (1<<1);	//De reset I2C module
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<AHB_I2C);	// Enable Clock for I2C

	// I2C Configuration
	// 2SCLH = SCLL then bitrate = Pclk / 3SCLH -> SCLH = Pclk / (3*bitrate) ; SCLL = 2 * SCLH
	LPC_I2C->SCLH = (SystemCoreClock/(3 * bitrate)); // Speed setup
	LPC_I2C->SCLL = 2 * LPC_I2C->SCLH;

	LPC_I2C->CONCLR = 0;
	LPC_I2C->CONSET |= I2C_CONSET_I2EN;   //put I2C unit in master transmit mode (sec 15.8.1 and 15.7.1)
}

Status i2cmaster_data_xfer(I2C_MASTER_DATA_Typedef* x_i2chdlr)
{
	I2CSTATUS status;
	uint8_t *ptx = x_i2chdlr->txbuff;
	uint8_t *prx = x_i2chdlr->rxbuff;
	uint8_t slvaddrread = (x_i2chdlr->slv_addr<<1) | (1<<0); // To write data to I2C device
	uint8_t slvaddrwrite = (x_i2chdlr->slv_addr<<1);// To read data from I2C device
	x_i2chdlr->txcount = 0;
	x_i2chdlr->rxcount = 0;

	// Send Start flag
	status = i2cstart();		// Start the I2C communication
	if (status != STAT_START_TX)
	{
		x_i2chdlr->i2cstatus = status;
		goto error;
	}
	if ((ptx != 0) && (x_i2chdlr->txlen != 0))	// Transmission of data from Master to Slave
	{

		// Send Slave address
		status = i2csendbyte(slvaddrwrite);	// send slave address with read flag
		if (status != STAT_SLVADDR_W_TX_w_ACK){
			x_i2chdlr->i2cstatus = status;
			goto error;
		}

		//start sending data
		while(x_i2chdlr->txcount < x_i2chdlr->txlen)
		{
			status = i2csendbyte(*ptx);
			if (status != STAT_DATA_TX_w_ACK){
				x_i2chdlr->i2cstatus = status;
				goto error;
			}
			x_i2chdlr->txcount++;
			ptx++;
		}
	}

	// To restart after sending data in order to get data from slave device
	// if that is the sequence
	if ((ptx != 0) && (x_i2chdlr->txlen != 0) &&
			(prx != 0) && (x_i2chdlr->rxlen !=0))
	{
		status = i2cstart();
		if (status != STAT_RESTART_TX)
		{
			x_i2chdlr->i2cstatus = status;
			goto error;
		}
	}

	// to Receive data from Slave device
	if ((prx != 0) && (x_i2chdlr->rxlen !=0))
	{
		// sned slave address with read flag
		status = i2csendbyte(slvaddrread);
		if (status != STAT_SLVADDR_R_TX_w_ACK)
		{
			x_i2chdlr->i2cstatus = status;
			goto error;
		}

		// Recieve all data bytes needed
		while (x_i2chdlr->rxcount < x_i2chdlr->rxlen)
		{
			if (x_i2chdlr->rxcount < (x_i2chdlr->rxlen - 1))
			{
				status = i2cgetbyte(prx , TRUE);
				if (status != STAT_DATA_RX_w_SENT_ACK)
				{
					x_i2chdlr->i2cstatus = status;
					goto error;
				}
			}
			else
			{
				status = i2cgetbyte(prx , FALSE);
				if (status != STAT_DATA_RX_w_SENT_NACK)
				{
					x_i2chdlr->i2cstatus = status;
					goto error;
				}
			}
			prx++;
			x_i2chdlr->rxcount++;
		}
	}
	i2cstop();
	return SUCCESS;
error:
	i2cstop();
	return ERROR;

}

/* Master tx only */
int i2c_master_send( uint8_t slaveAddr,  uint8_t *buff, uint8_t len)
{
	I2C_MASTER_DATA_Typedef xfer = {0};
	xfer.slv_addr = slaveAddr;
	xfer.txbuff = buff;
	xfer.txlen = len;
	if (i2cmaster_data_xfer(&xfer) == ERROR) return 1;
	return len - xfer.txlen;
}

/* Transmit one byte and receive an array of bytes after a repeated start condition is generated in Master mode.
 * This function is useful for communicating with the I2C slave registers
 */
int i2c_master_cmd_read( uint8_t slaveAddr, uint8_t cmd, uint8_t *buff, int len)
{
	I2C_MASTER_DATA_Typedef xfer = {0};
	xfer.slv_addr = slaveAddr;
	xfer.txbuff = &cmd;
	xfer.txlen = 1;
	xfer.rxbuff = buff;
	xfer.rxlen = len;
	if (i2cmaster_data_xfer(&xfer) == ERROR) return 1;
	return len - xfer.rxlen;
}

/* Sequential master read */
int i2c_master_read( uint8_t slaveAddr, uint8_t *buff, int len)
{
	I2C_MASTER_DATA_Typedef xfer = {0};
	xfer.slv_addr = slaveAddr;
	xfer.rxbuff = buff;
	xfer.rxlen = len;
	if (i2cmaster_data_xfer(&xfer) == ERROR) return 1;
	return len - xfer.rxlen;
}

