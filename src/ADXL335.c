/*
 * LPCXpresso LPC1115
 * Author: dsolano
   ADXL335.h - library for ADXL335 Accelerometer GY-61 Board
 * Created on June 26, 2014, 12:36 AM
*/

#include <ADXL335.h>
#include <timer32_lpc11xx.h>
#include <lpc11xx_syscon.h>
#include <float.h>
#include <math.h>


uint32_t* ptr_ADDRx = (uint32_t*)((&ADXL335->DR[0])); // pointer to ADDR0 register in ADC + the channel to be checked
uint32_t x_axis = 0;	//value of axis after read from adc.
uint32_t y_axis = 0;	//value of axis after read from adc.
uint32_t z_axis = 0;	//value of axis after read from adc.
Bool adc_DONE;

/*
 * Initialization of ADC for ADXL335
 * 	// First lets init tpin to be used
	// ADC1 = Pins 1.0
	// ADC2 = Pins 1.1
	// ADC3 = Pins 1.2
 */
void adxl335_init(void)
{
	// First lets init tpin to be used
	/*
		// ADC1 = Pins 1.0
		// ADC2 = Pins 1.1
		// ADC3 = Pins 1.2
	 */
	// ADC1 = Pins 1.0
	LPC_IOCON->R_PIO1_0 = 0ul; //Clean register
	LPC_IOCON->R_PIO1_0 |= (2<<0) ;	// as AD pin function
	LPC_IOCON->R_PIO1_0 &= ~(1<<7);	// as AD input mode
	// ADC2 = Pins 1.1
	LPC_IOCON->R_PIO1_1 = 0ul; //Clean register
	LPC_IOCON->R_PIO1_1 |= (2<<0) ;	// as AD pin function
	LPC_IOCON->R_PIO1_1 &= ~(1<<7);	// as AD input mode
	// ADC3 = Pins 1.2
	LPC_IOCON->R_PIO1_2 = 0ul; //Clean register
	LPC_IOCON->R_PIO1_2 |= (2<<0) ;	// as AD pin function
	LPC_IOCON->R_PIO1_2 &= ~(1<<7);	// as AD input mode

	ADXL335->CR |= CLOCKS_CONV_BITS(ADC_CLK_10Bits);	// clocks for 10 bit conversion
	ADXL335->INTEN |= 0ul;	// Disable all interrupts for ADC
	/* Disable Power down bit to the ADC block. */
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<4);

	/* Enable AHB clock to the ADC. */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<AHB_ADC);
	ADXL335->CR &= ADC_STOP;	// No conversion at this time for Burst mode
}



/*
 * Read from ADC each axis at a time from AD0 thru AD2
 * store reading in data structure ACC_DATA_t
 */
void adxl335_ReadAxis(void)
{
	int i = 0;
	int result=0;
	ADXL335->CR |= ADC_CR_BURST_ON;// Start Burst conversion for the channel
	for (i=0;i<3;i++)
	{
		int count = SAMPLE_RATE;
		result = 0;

		ADXL335->CR |= ADC_CR_CH_SEL(i);// Enable the channel for conversion
		while (count--) // read 16 time for accuracy
		{
			delay32us(TIMER0 , 70);
			result += ADC_DR_RESULT(*(ptr_ADDRx+i));	// read and add 16 times the result to get the media
		}
		// get the axis reading from ADC 0 thru 2
		if (i==0) acc.Xread = result/SAMPLE_RATE; // save result per Axis X
		if (i==1) acc.Yread = result/SAMPLE_RATE; // save result per Axis Y
		if (i==2) acc.Zread = result/SAMPLE_RATE; // save result per Axis Z
		ADXL335->CR &= ~ADC_CR_CH_SEL(i);// Disable the channel
	}
	// Shutdown ADC converter
	ADXL335->CR &= ADC_CR_BURST_OFF;// Stop Burst conversion for the channel

	//convert to voltage readings @ 0g conversions for each axis
	acc.Xvolt = (Vx(acc.Xread)) - X_0g;
	acc.Yvolt = (Vy(acc.Yread)) - Y_0g;
	acc.Zvolt = (Vz(acc.Zread)) - Z_0g;

	// Convert to g = 9.8m/s^2
	acc.Xg = acc.Xvolt / X_SENS;
	acc.Yg = acc.Yvolt / Y_SENS;
	acc.Zg = acc.Zvolt / Z_SENS;

	// Get Resulting Vector
	acc.Rvector = sqrt((pow(acc.Xg , 2) +
						pow(acc.Yg , 2) +
						pow(acc.Zg , 2))); // 3D Pythagorean Theorem

	// Get the inclination angle relative to R vector
	acc.Xangle = acos(acc.Xg / acc.Rvector);
	acc.Yangle = acos(acc.Yg / acc.Rvector);
	acc.Zangle = acos(acc.Zg / acc.Rvector);

	// Direction Cosine
	acc.Dir_Cosine = sqrt(	pow(cos(acc.Xg / acc.Rvector) , 2) +
							pow(cos(acc.Yg / acc.Rvector) , 2) +
							pow(cos(acc.Zg / acc.Rvector) , 2) );

	return;
}


