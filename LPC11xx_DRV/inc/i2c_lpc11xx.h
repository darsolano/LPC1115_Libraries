/*
 * i2c_lpc11xx.h
 *
 *  Created on: Feb 15, 2015
 *      Author: dsolano
 */

#ifndef I2C_LPC11XX_H_
#define I2C_LPC11XX_H_

#include <LPC11xx.h>
#include <lpc_types.h>

#define I2C_NORMAL_FAST		~(0x3<<8)
#define I2C_FAST_PLUS		(2<<8)

#define I2C_BITRATE_100KHz	100000
#define I2C_BITRATE_400KHz	400000
#define I2C_BITRATE_1MHz	1000000

#define I2C_CONSET_AA		(1<<2)
#define I2C_CONSET_SI		(1<<3)
#define I2C_CONSET_STO		(1<<4)
#define I2C_CONSET_STA		(1<<5)
#define I2C_CONSET_I2EN		(1<<6)

#define I2C_CONCLR_AAC		(1<<2)
#define I2C_CONCLR_SIC		(1<<3)
#define I2C_CONCLR_STAC		(1<<5)
#define I2C_CONCLR_I2ENC	(1<<6)

#define I2C_STAT_CODE_BITMASK		((0xF8))
// Status register codes
typedef enum
{
	STAT_GENERAL_ERROR			= 0x00,
	STAT_START_TX 				= 0x08,
	STAT_RESTART_TX 			= 0x10,
	STAT_SLVADDR_W_TX_w_ACK		= 0x18,
	STAT_SLVADDR_W_TX_w_NACK	= 0x20,
	STAT_DATA_TX_w_ACK			= 0x28,
	STAT_DATA_TX_w_NACK			= 0x30,
	STAT_ARB_LOSS				= 0x38,
	STAT_SLVADDR_R_TX_w_ACK		= 0x40,
	STAT_SLVADDR_R_RX_w_NACK	= 0x48,
	STAT_DATA_RX_w_SENT_ACK		= 0x50,
	STAT_DATA_RX_w_SENT_NACK	= 0x58,
	STAT_DATA_I2C_IDLE			= 0xF8
}I2CSTATUS;

// Result response from I2C activities
typedef enum
{
	I2C_IDLE 			=	0,
	I2C_STARTED_ERROR 	=	1,
	I2C_RESTARTED_ERROR =	2,
	I2C_REPEATED_START_ERROR 	=	3,
	TX_DATA_ACK_ERROR 	=	4,
	TX_DATA_NACK_ERROR 	=	5,
	I2C_BUSY			=	6,
	I2C_STOPPED			=	7,
	RX_DATA_ACK			=	8,
	RX_DATA_NACK		=	9,
	I2C_GENERAL_ERROR	=	10
}I2C_RESPONSE;

// Master data structure t handle all I2C Tasks
typedef struct sI2C_MASTER_DATA
{
	uint8_t 	slv_addr;	// Slave address to send data
	uint8_t 	*txbuff;	// Transmit buffer pointer
	uint8_t 	*rxbuff;	// Receive buffer pointer
	I2CSTATUS 	i2cstatus;	// Status of the transaction
	int16_t 	txlen;		// Amount of bytes to transmit
	int16_t		txcount;	// Count number of sent bytes
	int16_t 	rxlen;		// Amount of data to receive or received
	int16_t		rxcount;	// Count number of received bytes

}I2C_MASTER_DATA_Typedef;



void 	i2cinit				(uint32_t rate);
Status 	i2cmaster_data_xfer	(I2C_MASTER_DATA_Typedef*);
int 	i2c_master_send		( uint8_t slaveAddr, uint8_t *buff, uint8_t len);
int 	i2c_master_cmd_read	( uint8_t slaveAddr, uint8_t cmd, uint8_t *buff, int len);
int 	i2c_master_read		( uint8_t slaveAddr, uint8_t *buff, int len);

#endif /* I2C_LPC11XX_H_ */
