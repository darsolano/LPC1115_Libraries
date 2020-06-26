/*
 * LPCXpresso LPC1115 NXP
  EEP24C32.h - library for 24C32 EEPROM from TinyRTC v1.1 by Weiman
 *Created on June 26, 2014, 12:36 AM
 * Author: Darwin O. Solano
 */

#include <EEP24C32.h>
#include <timer32_lpc11xx.h>
#include <debug_frmwrk.h>
#include <i2c_lpc11xx.h>

#define SPC					0x20
#define ENTER				0x0d


Status status;
uint8_t txbuff[32];
uint8_t rxbuff[32];

/** I2C Write Data*/
static Status I2CWriteData ( uint8_t* buffer, uint8_t len )
{
    /* Sets data to be send to RTC to init*/
    I2C_MASTER_DATA_Typedef i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2ctx.rxbuff = 0;
    i2ctx.rxlen = 0;
    i2ctx.slv_addr = I2C_24C32;
    i2ctx.txbuff = buffer;
    i2ctx.txlen = len;
    // Send data to I2C
    status = i2cmaster_data_xfer( &i2ctx);
    return status;
}

/****
 * Send data thru I2C module stored in buffer, with Len, at i2c Device addr
 */
static Status I2CReadData ( uint8_t* buffer, uint8_t len )
{
    /* Sets data to be send to RTC to init*/
	I2C_MASTER_DATA_Typedef i2crx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2crx.rxbuff = buffer;
    i2crx.rxlen = len;
    i2crx.slv_addr = I2C_24C32;
    i2crx.txbuff = 0;
    i2crx.txlen = 0;
    // Send data to I2C
    status = i2cmaster_data_xfer ( &i2crx );
    return status;
}

/******
 * Read a single byte from pointed address of EEPROM
 */
uint8_t EEPReadByte (  uint16_t address )
{
	uint8_t addrl;
	uint8_t addrh;

    addrl = address & 0x00ff;
    addrh = ((address & 0xff00) >> 8);

    txbuff[0] = addrh;
    txbuff[1] = addrl;
    status = I2CWriteData ( txbuff, 2);
    delay32us(TIMER0,dly_10ms);
    status = I2CReadData (  rxbuff, 1 );
    return (rxbuff[0]);
}

/***
 * Write a single byte to EEPROM
 */
uint8_t EEPWriteByte (  uint16_t addr, uint8_t data )
{
	uint8_t addrl;
	uint8_t addrh;

    addrl = addr & 0x00ff;
    addrh = ((addr & 0xff00) >> 8);

    txbuff[0] = addrh;
    txbuff[1] = addrl;
    txbuff[2] = data;
    status = I2CWriteData (  txbuff, 3 );
    delay32us(TIMER0,dly_10ms); // 1ms
    return (data);
}

/****
 * Write Multiple bytes to EEPROM 1 page + 32 bytes
 */
void EEPWriteString ( uint16_t addr, uint8_t* buff, uint8_t size )
{

}

/**
 * Read multiple bytes from EEPROM
 */
uint8_t* EEPReadString ( uint16_t addr, uint8_t size, uint8_t *ptr )
{
    return (ptr);
}

/***********
 * Read multiple page from EEPROM
 */
void EEPReadPage (  uint16_t page)
{
    uint16_t fulladdr;
    uint8_t addr;
    uint8_t ptrBuff[64];
    uint8_t* ptr;

    xprintf ( "\nEEPROM 24C32 Full Page %02X read", page);
    UARTPuts ( "\n----------------------------------------------------");
    UARTPuts ( "\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
    UARTPuts ( "\n====================================================" );

    ptr = ptrBuff;
    fulladdr = page << 5;
    for (addr = 0; addr < 32; addr++) {
        if (!(addr & 0x0F)) {
        	UARTPutChar(ENTER);
        	UARTPutHex( addr >> 4 );
        	UARTPutChar(SPC);

        }
        *ptr = EEPReadByte (  fulladdr | (addr & 0x1f) );
        UARTPutHex_( *ptr );
        UARTPutChar(SPC);
        ptr++;
    }
    UARTPutChar( ENTER);
}


/********
 * Read multiple bytes from address and len from EEPROM
 */

void EEPReadByteLen (  uint16_t address, uint16_t len )
{
    /* Sets data to be send to RTC to init*/
    I2C_MASTER_DATA_Typedef i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

    uint8_t addrl;
    uint8_t addrh;
    uint16_t ctr;
    uint8_t page;
    uint8_t *ptr;

    page  = (address >> 5)  & 0x001f ;
    addrl = address   & 0x00ff;
    addrh = ((address & 0xff00) >> 8);


    xprintf (  "\nEEPROM 24C32 Full %d bytes on Page:%02X read", len,page);
    UARTPuts (  "\n----------------------------------------------------");
    UARTPuts (  "\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
    UARTPuts (  "\n====================================================" );

    txbuff[0] = addrh;
    txbuff[1] = addrl;
    rxbuff[0] = 0;

    // Fill Data Structure with proper data
    i2ctx.rxbuff = rxbuff;
    i2ctx.rxlen = len;
    i2ctx.slv_addr = I2C_24C32;
    i2ctx.txbuff = txbuff;
    i2ctx.txlen = 2;
    // Send data to I2C
    status = i2cmaster_data_xfer (  &i2ctx );
    ptr = rxbuff;
    for (ctr = 0; ctr < len; ctr++) {
        if (!(ctr & 0x0F)){
        	UARTPutChar( ENTER);
        	UARTPutHex16( ctr >> 4);
        	UARTPutChar( ':');
    	}

        UARTPutHex_(  *ptr );
        UARTPutChar( SPC);
        ptr++;
    }
    UARTPutChar( ENTER );
}
