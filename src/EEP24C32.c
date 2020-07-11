/*
 * LPCXpresso LPC1115 NXP
  EEP24C32.h - library for 24C32 EEPROM from TinyRTC v1.1 by Weiman
 *Created on June 26, 2014, 12:36 AM
 * Author: Darwin O. Solano
 */

#include <EEP24C32.h>
#include <debug_frmwrk.h>
#include <timeout_delay.h>

#define SPC					0x20
#define ENTER				0x0d


int status;
uint8_t txbuff[32];
uint8_t rxbuff[32];

/** I2C Write Data*/
static int I2CWriteData ( uint8_t* buffer, uint8_t len )
{
    /* Sets data to be send to RTC to init*/
    I2C_XFER_T i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2ctx.rxBuff = 0;
    i2ctx.rxSz = 0;
    i2ctx.slaveAddr = I2C_24C32;
    i2ctx.txBuff = buffer;
    i2ctx.txSz = len;
    // Send data to I2C
    status = Chip_I2C_MasterTransfer(I2C_24C32_BUS, &i2ctx);
    return status;
}

/****
 * Send data thru I2C module stored in buffer, with Len, at i2c Device addr
 */
static int I2CReadData ( uint8_t* buffer, uint8_t len )
{
    /* Sets data to be send to RTC to init*/
    I2C_XFER_T i2crx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2crx.rxBuff = buffer;
    i2crx.rxSz = len;
    i2crx.slaveAddr = I2C_24C32;
    i2crx.txBuff = 0;
    i2crx.txSz = 0;

    // Send data to I2C
    status = Chip_I2C_MasterTransfer(I2C_24C32_BUS, &i2crx);
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
    _delay_ms(10);
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
    _delay_ms(1); // 1ms
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
    I2C_XFER_T i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

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
    i2ctx.rxBuff = rxbuff;
    i2ctx.rxSz = len;
    i2ctx.slaveAddr = I2C_24C32;
    i2ctx.txBuff = txbuff;
    i2ctx.txSz = 2;

    // Send data to I2C
    status = Chip_I2C_MasterTransfer(I2C_24C32_BUS, &i2ctx);

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
