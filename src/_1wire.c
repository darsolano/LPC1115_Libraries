/*********************************************************************
 *
 *                 1-Wire Communication Protocol
 *
 *********************************************************************
 * FileName:        1wire.c
 * Dependencies:
 * Processor:       LPC-1769
 * Complier:        LPCXpresso 7
 * Company:         NXP Semiconductors
 *
 * Software License Agreement
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sasha     			12/20/07    Original
 ********************************************************************/
 /****** I N C L U D E S **********************************************************/
#include <_1wire.h>
#include <timer32_lpc11xx.h>
#include <lpc11xx_syscon.h>


/**PORT D E F I N I T I O N S ****************************************************/

//ONE WIRE PORT PIN DEFINITION
///****************************************************
// This Configuration is required to make any PIC MicroController
// I/O pin as Open drain to drive 1-wire.
///****************************************************

//#define OW_PIN_DIRECTION 		//LATDbits.LATD8
//#define OW_WRITE_PIN  		//TRISDbits.TRISD8
//#define OW_READ_PIN			//PORTDbits.RD8


#define OW_PIN				2
//#define OW_PORT				3
#define OW_PORT				LPC_GPIO3
#define OW_PIN_MASK			Bit(OW_PIN)
#define OW_PIN_DIR_OUT()	OW_PORT->DIR |= (1 << OW_PIN)
#define OW_PIN_DIR_IN()		OW_PORT->DIR &= ~(1 << OW_PIN)


// Input or Output
/**********************************************************************
* Function:        static void OW_PIN_DIRECTION (DIRECTION_eType direction)
* PreCondition:    None
* Input:		   pin direction as INPUT or OUTPUT
* Output:		   None
* Overview:		   Set the designated PIN as INPUT or OUTPUT
***********************************************************************/
static void OW_PIN_DIRECTION (DIRECTION_eType direction)
{
	if (direction) OW_PORT->DIR |= (1 << OW_PIN);
	else OW_PORT->DIR &= ~(1 << OW_PIN);
}

/**********************************************************************
* Function:        static void OW_WRITE_PIN(LOGIC_LEVEL_eType bitstate)
* PreCondition:    Pin must has being set as Output first
* Input:		   Pin logical level as High or Low
* Output:		   None
* Overview:		   Set the designated Pin as High Level or Low level
***********************************************************************/
static void OW_WRITE_PIN(LOGIC_LEVEL_eType bitstate)
{
	switch (bitstate){
		case LOW:
			OW_PORT->DATA &= ~(OW_PIN_MASK);
			break;
		case HIGH:
			OW_PORT->DATA |= (OW_PIN_MASK);
			break;
		default:
			break;
	}
}

/**********************************************************************
* Function:        static uint8_t OW_READ_PIN(void)
* PreCondition:    Pin must has being set as Input first
* Input:		   None
* Output:		   LOgical level state of the Pin
* Overview:		   Read the designated pin and return the state back to the caller
* 				   High or Low
***********************************************************************/
static uint8_t OW_READ_PIN(void)
{
	return (OW_PORT->DATA & OW_PIN_MASK) ? HIGH:LOW;
}

//****** V A R I A B L E S ********************************************************/
unsigned char macro_delay;

/**********************************************************************
* Function:        void OW_PinInit(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Configure the designated GPIO as the 1wire Pin
***********************************************************************/
void OW_PinInit(uint8_t port, uint8_t pin)
{
	// PORT 3 PIN 2 for 1 wire
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<AHB_IOCON);// Enable IOCON Clock source
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<AHB_GPIO);	// Enable GPIO Clock source
	LPC_IOCON->PIO3_2 = 0;						// GPIO, Normal, No OD
	LPC_GPIO3->DIR |= (1<<2);					// As output
}

/**********************************************************************
* Function:        void drive_OW_low (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Configure the OW_PIN as Output and drive the OW_PIN LOW.	
***********************************************************************/
void drive_OW_low (void)
{
	OW_PIN_DIRECTION (OUTPUT);
	OW_WRITE_PIN(LOW);
}

/**********************************************************************
* Function:        void drive_OW_high (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Configure the OW_PIN as Output and drive the OW_PIN HIGH.	
***********************************************************************/
void drive_OW_high (void)
{
	OW_PIN_DIRECTION (OUTPUT);
	OW_WRITE_PIN(HIGH);
}

/**********************************************************************
* Function:        unsigned char read_OW (void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return the status of OW pin.	
* Overview:		   Configure as Input pin and Read the status of OW_PIN 	
***********************************************************************/
SetState read_OW (void){

	OW_PIN_DIRECTION(INPUT);
	 if (OW_READ_PIN())
	 	return SET;
	 else 	
		return RESET;
}

/**********************************************************************
* Function:        unsigned char OW_reset_pulse(void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return the Presense Pulse from the slave.	
* Overview:		   Initialization sequence start with reset pulse.
*				   This code generates reset sequence as per the protocol
***********************************************************************/

PRESENCE_t OW_reset_pulse(void)
{

	PRESENCE_t presence;
  	drive_OW_low(); 				// Drive the bus low
  	delay32us(TIMER0 , 480);	  	// delay 480 microsecond (us)
  	drive_OW_high();				// Release the bus to high
	delay32us(TIMER0 ,70);		// delay 70 microsecond (us)
	presence = read_OW();			//Sample for presence pulse from slave
	delay32us(TIMER0 ,410);	  			// delay 410 microsecond (us)
	drive_OW_high();		    	// Release the bus
	if (presence) return (OW_DEV_NOT_PRESENT);
		else return (OW_DEV_PRESENT);
}	

/**********************************************************************
* Function:        void OW_write_bit (unsigned char write_data)
* PreCondition:    None
* Input:		   Write a bit to 1-wire slave device.
* Output:		   None
* Overview:		   This function used to transmit a single bit to slave device.
*				   
***********************************************************************/

void OW_write_bit (uint8_t write_bit){
	if (write_bit)
	{
		//writing a bit '1'
		drive_OW_low(); 				// Drive the bus low
		delay32us(TIMER0 ,10);		// delay 15 microsecond (us)
		drive_OW_high();  				// Release the bus
		delay32us(TIMER0 ,20);		// delay 45 microsecond (us)
	}
	else{
		//writing a bit '0'
		drive_OW_low(); 				// Drive the bus low
		delay32us(TIMER0 ,40);		// delay 60 microsecond (us)
		drive_OW_high();  				// Release the bus
		delay32us(TIMER0 ,10);		// delay 10 microsecond (us)
	}
}	


/**********************************************************************
* Function:        unsigned char OW_read_bit (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the status of the OW PIN
* Overview:		   This function used to read a single bit from the slave device.
*				   
***********************************************************************/

uint8_t OW_read_bit (void)
{
	uint8_t read_data;
	//reading a bit 
	drive_OW_low(); 						// Drive the bus low
	delay32us(TIMER0 ,15);				// delay 6 microsecond (us)
	read_data = read_OW();					// Read the status of OW_PIN
	delay32us(TIMER0 ,55);				// delay 55 microsecond (us)
	return read_data;
}

/**********************************************************************
* Function:        void OW_write_byte (unsigned char write_data)
* PreCondition:    None
* Input:		   Send byte to 1-wire slave device
* Output:		   None
* Overview:		   This function used to transmit a complete byte to slave device.
*				   
***********************************************************************/
void OW_write_byte (uint8_t write_data)
{
	uint8_t loop;
	for (loop = 0; loop < 8; loop++)
	{
		OW_write_bit(write_data & 0x01); 	//Sending LS-bit first
		write_data >>= 1;					// shift the data byte for the next bit to send
	}	
}	

/**********************************************************************
* Function:        unsigned char OW_read_byte (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the read byte from slave device
* Overview:		   This function used to read a complete byte from the slave device.
*				   
***********************************************************************/

uint8_t OW_read_byte (void)
{
	uint8_t loop, result=0;
	for (loop = 0; loop < 8; loop++)
	{
		result >>= 1; 				// shift the result to get it ready for the next bit to receive
		if (OW_read_bit())
		result |= 0x80;				// if result is one, then set MS-bit
	}
	return result;					
}	

/********************************************************************************************
                  E N D     O F     1 W I R E . C  
*********************************************************************************************/
