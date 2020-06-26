/*
 * LPCXpresso1769
 T_18B20.c - library for 18B20 Thermometer
 * Created on June 26, 2014, 12:36 AM
 */

#define _1ENABLED
#ifdef _1ENABLED

#include <T_18B20.h>
#include <timer32_lpc11xx.h>
#include <debug_frmwrk.h>

PRESENCE_t DS18B20Init(void)
{
	ROMADDR_sType rom;
	SCRATCHPAD_sType scratchp;
	OW_PinInit(3,2);
	if (OW_reset_pulse() == OW_DEV_PRESENT)
	{
		UARTPuts_("Found 1-Wire device....");
		DS18B20CmdGetROMIDCode(&rom);
		DS18B20GetScrachtPadReg(&scratchp);
		return OW_DEV_PRESENT;
	}
	else
	{
		UARTPuts_( "NOT Found 1-Wire device....");
		return OW_DEV_NOT_PRESENT;
	}
}

void DS18B20CmdGetROMIDCode(ROMADDR_sType* romid)
{
	int loop;
	OW_reset_pulse();
	OW_write_byte(READ_ROM);
	for (loop = 0; loop < 8; loop++)
	{
		romid->ROM_ID_BYTE[loop] = OW_read_byte();
	}
	UARTPuts( "\nDevice ID: ");
	loop = 7;
	while (loop>=0)
	{
		UARTPutHex_( romid->ROM_ID_BYTE[loop]);
		UARTPutChar( '.');
		loop--;
	}
}

Bool DS18B20ConfirmIDofDevice(ROMADDR_sType* romid)
{
	uint8_t i;
	if (OW_reset_pulse())
		return FALSE;
	OW_write_byte(0x55); // match ROM

	for (i = 0; i < 8; i++)
	{
		OW_write_byte(romid->ROM_ID_BYTE[i]); //send ROM code
		delay32us(TIMER0 ,DELAY_10Us);
	}
	return TRUE;
}
/*
Bool DS18B20WriteAlarm_CfgReg(uint16_t Alarm, uint8_t Config)
{

}
*/
void DS18B20GetTemperature(Temperature_s *temp)
{

	int c16, c2, f10;

	OW_reset_pulse();
	OW_write_byte(0xCC); //Skip ROM
	OW_write_byte(0x44); // Start Conversion
	delay32us(TIMER0 ,dly_4us);
	OW_reset_pulse();
	OW_write_byte(0xCC); // Skip ROM
	OW_write_byte(0xBE); // Read Scratch Pad
	//from scratchpad read, the first 2 byte are from temp register, others are dropped
	temp->Temp_LowByte = OW_read_byte();
	temp->Temp_HighByte = OW_read_byte();
	OW_reset_pulse();

	// Celsius calculation
	if (temp->Temp_Type == Celcius)
	{
		temp->Whole = (temp->Temp_HighByte & 0x07) << 4;// grab lower 3 bits of t1
		temp->Whole |= temp->Temp_LowByte >> 4;		// and upper 4 bits of t0
		temp->Decimal = temp->Temp_LowByte & 0x0F;// decimals in lower 4 bits of t0
		temp->Decimal *= 625;		// conversion factor for 12-bit resolution
	}

	// Farenheit convertion
	if (temp->Temp_Type == Farenheit)
	{
		c16 = (temp->Temp_HighByte << 8) + temp->Temp_LowByte;// result is temp*16, in celcius
		c2 = c16 / 8;					// get t*2, with fractional part lost
		f10 = c16 + c2 + 320;	// F=1.8C+32, so 10F = 18C+320 = 16C + 2C + 320
		temp->Whole = f10 / 10;								// get whole part
		temp->Decimal = f10 % 10;						// get fractional part
	}
}
/*
void DS18B20RestoreEEPROMValues(void)
{

}

void DS18B20SaveSRAMValues(void)
{

}

POWERTYPE_t DS18B20GetPWRSupplyStatus(void)
{

}

uint8_t DS18B20CalcAndCompareCRC(uint8_t _crc)
{

}
*/
void DS18B20GetScrachtPadReg(SCRATCHPAD_sType* spad)
{
	uint8_t i=0;
	UARTPuts( "\nReading ScratchPad Data\n");
	OW_reset_pulse();
	OW_write_byte(0xCC); // Skip ROM
	OW_write_byte(0xBE); // Read Scratch Pad

	for(i=0;i<9;i++)
	{
		spad->ScratchBYTES[i] = OW_read_byte();
	}
	UARTPuts( "\nScratchPAD DATA:\n");

	UARTPuts( "CRC:-------------> ");
	UARTPutHex( spad->CRC);

	UARTPuts( "\nReserved(0x10):--> ");
	UARTPutHex( spad->reserved2);

	UARTPuts( "\nReserved:--------> ");
	UARTPutHex( spad->reserved1);

	UARTPuts( "\nReserved(0xFF):--> ");
	UARTPutHex( spad->reserved0);

	UARTPuts( "\nConfig Register:-> ");
	UARTPutHex( spad->Config);

	UARTPuts( "\nUser Byte 2:-----> ");
	UARTPutHex( spad->UserByte2);

	UARTPuts( "\nUser Byte 1:-----> ");
	UARTPutHex( spad->UserByte1);

	UARTPuts( "\nTemp MSB:--------> ");
	UARTPutHex( spad->TempHi);

	UARTPuts( "\nTemp LSB:--------> ");
	UARTPutHex( spad->TempLow);
}

#endif
