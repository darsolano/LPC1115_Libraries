/* 
 * File:   24C32.h
 * Author: dsolano
 *
 * Created on June 26, 2014, 12:36 AM
 */
#pragma once
#ifndef EEP24C32_H
#define	EEP24C32_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>

    //#define DBGPRINT
    // Peripheral ID for EEPROM 24C32 4KBytes from ATMEL
#define I2C_24C32                           0x50
#define I2C_24C32_BUS                       I2C0
#define EE_READ                             1
#define EE_WRITE                            0
//Control & Commands for RTC - to be ored or andded with content to be sent
#define ROM_START               	0x08
#define ROM_END                 	0x3f

//SETUP FOR RS BIT IN CONTROL REGISTER ADDRESS 0x07
#define OUT_DISABLE             	0

#define EEP_RTC_START           	0x08
#define EEP_RTC_END             	0x3f

#define PAGE_BYTES_SIZE                     0x001F
#define MAX_8BYTE_PAGES                     0xEF
#define FIRST_PAGE                          0x000
#define LAST_PAGE                           0xFE0
#define PAGE_INCREMENT                      

    // bitwise and for those mask
#define LOW_ADDRESS_MASK                    0b0000000000011111
#define PAGE_ADDRESS_MASK                   0b0000111111100000
#define PAGE(p)                             (p << 5)

#define EEP_ACK                             1
#define EEP_NACK                            0

typedef union {
	struct EE_Address {
		unsigned Low : 5;
		unsigned High : 7;
		unsigned : 4;
	} EE_Address_t;

	struct EE_WordAddr {
		unsigned Addr : 12;
		unsigned : 4;
	} EE_WordAddr_t;

	struct WordAddr {
		uint16_t w_addr;
	} WordAddr_t;
} EE24C32_Addr_t;


/*
 * Function Prototypes
 */
uint8_t  EEPReadByte(uint16_t address);
uint8_t  EEPWriteByte(uint16_t addr, uint8_t data);
void     EEPWriteString(uint16_t addr, uint8_t* buff, uint8_t size);
uint8_t* EEPReadString(uint16_t addr, uint8_t size, uint8_t* ptr);
void     EEPReadPage(uint16_t page);
void     EEPReadByteLen(uint16_t addr, uint16_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* EEP24C32_H */

