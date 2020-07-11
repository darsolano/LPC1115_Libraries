/**********************************************************************
* $Id$		debug_frmwrk.h		2010-05-21
*//**
* @file		debug_frmwrk.h
* @brief	Contains some utilities that used for debugging through UART
* @version	2.0
* @date		21. May. 2010
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2010, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/
#ifndef DEBUG_FRMWRK_H_
#define DEBUG_FRMWRK_H_

#include <chip.h>
#include <xprintf.h>


#ifndef bool
#define bool unsigned char
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true !false
#endif

#define DEBUG_UART_PORT	LPC_USART

#define IER_RBR         (0x01<<0)
#define IER_THRE        (0x01<<1)
#define IER_RLS         (0x01<<2)
#define IER_ABEO        (0x01<<8)
#define IER_ABTO        (0x01<<9)


#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01
#define IIR_ABEO        (0x01<<8)
#define IIR_ABTO        (0x01<<9)

#define LSR_RDR         (0x01<<0)
#define LSR_OE          (0x01<<1)
#define LSR_PE          (0x01<<2)
#define LSR_FE          (0x01<<3)
#define LSR_BI          (0x01<<4)
#define LSR_THRE        (0x01<<5)
#define LSR_TEMT        (0x01<<6)
#define LSR_RXFE        (0x01<<7)

#define BUFSIZE         0x40

/* RS485 mode definition. */
#define RS485_NMMEN		(0x1<<0)
#define RS485_RXDIS		(0x1<<1)
#define RS485_AADEN		(0x1<<2)
#define RS485_SEL             (0x1<<3)
#define RS485_DCTRL		(0x1<<4)
#define RS485_OINV		(0x1<<5)

typedef enum
{
	STAT_RBDR = 0x1,
	STAT_OVRUN = 0x2,
	STAT_PARITY_ERROR = 0x4,
	STAT_FRM_ERROR = 0x8,
	STAT_TX_EMP = 0x20
}UART_STAT_eType;

#define TXEN	0x80ul

int  UARTprintf(const char *format, ...);
void UARTPutChar ( uint8_t ch);
void UARTPuts( const void *str);
void UARTPuts_( const void *str);
void UARTPutDec( uint8_t decnum);
void UARTPutDec99( uint8_t decnum);
void UARTPutDec16( uint16_t decnum);
void UARTPutDec32( uint32_t decnum);
void UARTPutHex_( uint8_t hexnum);
void UARTPutHex ( uint8_t hexnum);
void UARTPutHex16 ( uint16_t hexnum);
void UARTPutHex32 ( uint32_t hexnum);
uint8_t UARTGetChar ();
void debug_frmwrk_init(uint32_t baudrate);

#endif /* DEBUG_FRMWRK_H_ */
