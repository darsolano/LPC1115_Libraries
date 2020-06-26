/**********************************************************************
* $Id$		debug_frmwrk.c				2010-05-21
*//**
* @file		debug_frmwrk.c
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

#include <debug_frmwrk.h>
#include <stdarg.h>
#include <lpc11xx_syscon.h>
#include <stdio.h>





/*********************************************************************//**
 * @brief		Puts a character to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	ch		Character to put
 * @return		None
 **********************************************************************/
void UARTPutChar (uint8_t ch)
{
	while (!(DEBUG_UART_PORT->LSR & STAT_TX_EMP));
	DEBUG_UART_PORT->THR = ch;
}


/*********************************************************************//**
 * @brief		Get a character to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @return		character value that returned
 **********************************************************************/
uint8_t UARTGetChar (void)
{
	while (!(DEBUG_UART_PORT->LSR & STAT_RBDR));
	return DEBUG_UART_PORT->RBR;
}


/*********************************************************************//**
 * @brief		Puts a string to UART port
 * @param[in]	UARTx 	Pointer to UART peripheral
 * @param[in]	str 	string to put
 * @return		None
 **********************************************************************/
void UARTPuts( const void *str)
{
	uint8_t *s = (uint8_t *) str;

	while (*s)
	{
		UARTPutChar( *s++);
	}
}


/*********************************************************************//**
 * @brief		Puts a string to UART port and print new line
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	str		String to put
 * @return		None
 **********************************************************************/
void UARTPuts_( const void *str)
{
	UARTPuts ( str);
	UARTPuts ( "\n\r");
}


/*********************************************************************//**
 * @brief		Puts a decimal number to UART port just 2 digits instead of 3
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		just numbers less that 100
 **********************************************************************/
void UARTPutDec99(uint8_t decnum)
{
	uint8_t c1 = decnum % 10;
	uint8_t c2 = (decnum / 10) % 10;
	uint8_t c3 = (decnum / 100) % 10;
	if (c3 != 0) UARTPutChar('0' + c3);
	UARTPutChar('0' + c2);
	UARTPutChar('0' + c1);
}


/*********************************************************************//**
 * @brief		Puts a decimal number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutDec(uint8_t decnum)
{
	uint8_t c1=decnum%10;
	uint8_t c2=(decnum/10)%10;
	uint8_t c3=(decnum/100)%10;
	if (c3 != 0) UARTPutChar('0'+c3);
	if (c2 != 0) UARTPutChar('0'+c2);
	UARTPutChar('0'+c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutDec16(uint16_t decnum)
{
	uint8_t c1=decnum%10;
	uint8_t c2=(decnum/10)%10;
	uint8_t c3=(decnum/100)%10;
	uint8_t c4=(decnum/1000)%10;
	uint8_t c5=(decnum/10000)%10;
	if (c5 != 0) UARTPutChar('0'+c5);
	if (c4 != 0) UARTPutChar('0'+c4);
	if (c3 != 0) UARTPutChar('0'+c3);
	if (c2 != 0) UARTPutChar('0'+c2);
	UARTPutChar('0'+c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutDec32(uint32_t decnum)
{
	uint8_t c1=decnum%10;
	uint8_t c2=(decnum/10)%10;
	uint8_t c3=(decnum/100)%10;
	uint8_t c4=(decnum/1000)%10;
	uint8_t c5=(decnum/10000)%10;
	uint8_t c6=(decnum/100000)%10;
	uint8_t c7=(decnum/1000000)%10;
	uint8_t c8=(decnum/10000000)%10;
	uint8_t c9=(decnum/100000000)%10;
	uint8_t c10=(decnum/1000000000)%10;
	if (c10 != 0) UARTPutChar('0'+c10);
	if (c9 != 0) UARTPutChar('0'+c9);
	if (c8 != 0) UARTPutChar('0'+c8);
	if (c7 != 0) UARTPutChar('0'+c7);
	if (c6 != 0) UARTPutChar('0'+c6);
	if (c5 != 0) UARTPutChar('0'+c5);
	if (c4 != 0) UARTPutChar('0'+c4);
	if (c3 != 0) UARTPutChar('0'+c3);
	if (c2 != 0) UARTPutChar('0'+c2);
	UARTPutChar('0'+c1);
}


/*********************************************************************//**
 * @brief		Puts a hex number to UART port without the 0x chars
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex_(uint8_t hexnum)
{
	uint8_t nibble, i;

	//UARTPuts("0x");
	i = 1;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		UARTPutChar((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}



/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex (uint8_t hexnum)
{
	uint8_t nibble, i;

	UARTPuts("0x");
	i = 1;
	do {
		nibble = (hexnum >> (4*i)) & 0x0F;
		UARTPutChar((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}


/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex16 (uint16_t hexnum)
{
	uint8_t nibble, i;

	UARTPuts("0x");
	i = 3;
	do {
		nibble = (hexnum >> (4*i)) & 0x0F;
		UARTPutChar((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex32 (uint32_t hexnum)
{
	uint8_t nibble, i;

	UARTPuts("0x");
	i = 7;
	do {
		nibble = (hexnum >> (4*i)) & 0x0F;
		UARTPutChar((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}


int UARTprintf(const char *format, ...)
{
	unsigned int index = 0;
	va_list argptr;

	va_start(argptr, format);

	while(format[index] != '\0')
	{
		if(format[index] == '%')
		{
			index++;

			if(format[index] == '\0')
				return 0;

			switch(format[index])
			{
				case 'd':
					UARTPutDec32(va_arg(argptr, int));
					break;
				case 'i':
					UARTPutDec32( va_arg(argptr, int));
					break;
				case 'u':
					UARTPutDec32(va_arg(argptr, unsigned int));
					break;
				case 'c':
					UARTPutChar( va_arg(argptr, int));
					break;
				case 's':
					UARTPuts( va_arg(argptr, char *));
					break;
				case 'x':
					UARTPutHex( va_arg(argptr, unsigned int));
					break;
				default:
					UARTPutChar(format[index]);
			}
		}
		else
			UARTPutChar(format[index]);

		index++;
	}

    va_end(argptr);
    return 0;
}

///*********************************************************************//**
// * @brief		print function that supports format as same as printf()
// * 				function of <stdio.h> library
// * @param[in]	None
// * @return		None
// **********************************************************************/
//void  _printf (const  char *format, ...)
//{
//    static  char  buffer[512 + 1];
//            va_list     vArgs;
//            char	*tmp;
//    va_start(vArgs, format);
//    vsprintf((char *)buffer, (char const *)format, vArgs);
//    va_end(vArgs);
//
//    _DBG(buffer);
//}

/*********************************************************************//**
 * @brief		Initialize Debug frame work through initializing UART port
 * @param[in]	None
 * @return		None
 **********************************************************************/
void debug_frmwrk_init(uint32_t baudrate)
{
	uint32_t Fdiv;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<AHB_UART);	// enable device on chip UART
	LPC_SYSCON->UARTCLKDIV = 1ul;	//Enable input clock divider
	LPC_IOCON->PIO1_6 &= ~0x07;    /*  UART I/O config */
	LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
	LPC_IOCON->PIO1_7 &= ~0x07;
	LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */

	DEBUG_UART_PORT->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */

    Fdiv = (((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/ LPC_SYSCON->UARTCLKDIV)/16)/baudrate ;	/*baud rate */
    DEBUG_UART_PORT->DLM = Fdiv / 256;
    DEBUG_UART_PORT->DLL = Fdiv % 256;
    LPC_UART->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */


    DEBUG_UART_PORT->TER = TXEN;
	xdev_in(UARTGetChar);
	xdev_out(UARTPutChar);

}


///////////////////////////////////////////////////////////////////////////////////////////////
// Function __write() / __sys_write
//
// Called by bottom level of printf routine within RedLib C library to write
// a character. With the default semihosting stub, this would write the character
// to the debugger console window . But this version writes
// the character to the LPC1768/RDB1768 UART.
int __sys_write (int iFileHandle, char *pcBuffer, int iLength)
{
	int i = 0;
	while (i < iLength)
	{
		UARTPutChar( pcBuffer[i++]);
	}
	return iLength;
}

// Function __readc() / __sys_readc
//
// Called by bottom level of scanf routine within RedLib C library to read
// a character. With the default semihosting stub, this would read the character
// from the debugger console window (which acts as stdin). But this version reads
// the character from the LPC1768/RDB1768 UART.
int __sys_readc (void)
{
	//-- UARTGetChar --
	//uint8_t tmp = 0;
	return (int)UARTGetChar();
	//return (int) tmp;
}

/* --------------------------------- End Of File ------------------------------ */
