/*
 * File:   mcp23008.h
 * Author: dsolano
 * For PIC32MX360F512L
 * Created on August 17, 2014, 6:07 PM
 */

#include "mcp23008.h"
#include <i2c_lpc11xx.h>

//*******************************************************************************
//				START OF LOCAL FUNCTIONS AND VARIABLES
//*******************************************************************************

PRIVATE uint8_t txbuff[24];
PRIVATE uint8_t rxbuff[12];
Status status;


//*******************************************************************************
//				END OF LOCAL FUNCTIONS AND VARIABLES
//*******************************************************************************

//*******************************************************************************
//				START OF PUBLIC FUNCTIONS AND VARIABLES
//*******************************************************************************

Status mcp23008_WriteGPIO ( uint8_t data, uint8_t addr )
{
    txbuff[0] = GPIO;
    txbuff[1] = data;
    status = i2c_master_send(addr,txbuff,2);
    return status;
}

uint8_t mcp23008_ReadGPIO ( uint8_t addr )
{
    rxbuff[0] = 0;
    i2c_master_cmd_read(addr , GPIO , rxbuff , 1);

    return rxbuff[0];
}

Status mcp23008_SetGPIODir ( MCP23008_IODIR_REG_sType *iodircfg, uint8_t addr )
{
    txbuff[0] = IODIR;
    txbuff[1] = iodircfg->IODir_reg;
    status = i2c_master_send(addr,txbuff,2);
    return status;
}

Status mcp23008_ConfigIOCON(MCP23008_IOCON_REG_sType *iocon, uint8_t addr){
    txbuff[0] = IOCON;
    txbuff[1] = iocon->IOCON_Reg;
    status = i2c_master_send(addr,txbuff,2);
    return status;
}

Status mcp23008_SetGPIOPullUps ( uint8_t pullup, uint8_t addr )
{
    txbuff[0] = GPPU;
    txbuff[1] = pullup;

    status = i2c_master_send(addr,txbuff,2);
    return status;

}

Status mcp23008_SetGPIOInt ( uint8_t config, uint8_t addr )
{
    txbuff[0] = DEFVAL;
    txbuff[1] = config;
    status = i2c_master_send(addr,txbuff,2);


    txbuff[0] = INTCON;
    txbuff[1] = config;
    status = i2c_master_send(addr,txbuff,2);

    mcp23008_WriteGPIO ( config, MCP23008_KP_ADDR );

    txbuff[0] = GPINTEN;
    txbuff[1] = config;
    status = i2c_master_send(addr,txbuff,2);

    return status;
}

uint8_t mcp23008_ReadINTCAP ( uint8_t addr )
{
    rxbuff[0] = 0;
    i2c_master_cmd_read(addr , INTCAP , rxbuff , 1);
    return rxbuff[0];
}

uint8_t mcp23008_ReadINTF ( uint8_t addr )
{
    rxbuff[0] = 0;
    i2c_master_cmd_read(addr , INTF , rxbuff , 1);
    return rxbuff[0];
}

Status mcp23008_SetIPOL ( uint8_t config, uint8_t addr )
{ // Polarity = 1 is inverted if 0 output = 1 and viceversa
    txbuff[0] = IPOL;
    txbuff[1] = config; // bitwise for set up all bits from 0 to 7

    return  i2c_master_send(addr,txbuff,2);
}

uint8_t mcp23008_ReadOLAT ( uint8_t addr )
{
	rxbuff[0] = 0;
    i2c_master_cmd_read(addr , OLAT , rxbuff , 1);
    return rxbuff[0];
}

Status mcp23008_WriteOLAT ( uint8_t data, uint8_t addr )
{ // Polarity = 1 is inverted if 0 output = 1 and viceversa
    txbuff[0] = OLAT;
    txbuff[1] = data; // bitwise for set up all bits from 0 to 7

    return i2c_master_send(addr,txbuff,2);
}

//*******************************************************************************
//				END OF PUBLIC FUNCTIONS AND VARIABLES
//*******************************************************************************
