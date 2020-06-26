/* 
 * File:   ADXL335.h
 * Author: dsolano
 * LPCXpresso LPC1769
   ADXL335.h - library for ADXL335 Accelerometer
 * Created on June 27, 2014, 6:39 PM
 */

#ifndef ADXL335_H
#define	ADXL335_H

#ifdef	__cplusplus
extern "C" {
#endif

//#define INT

#include <LPC11xx.h>
#include <lpc_types.h>



#define ADXL335			LPC_ADC
#define ADC_RATE		200000


/*
 * Used to point to ADC address register in MCU
 */
#define X_AXIS			0
#define Y_AXIS			1
#define Z_AXIS			2

/*
 * Times that we sample the data from ADC
 * in order to get an accurate reading
 */
#define SAMPLE_RATE		16

/*
 * Constants reference from MCU configuration
 */
#define ADC_VREF_MAX		3.0
#define MAX_CONV			4095


#define Vx(x)				(x*ADC_VREF_MAX)/MAX_CONV
#define Vy(y)				(y*ADC_VREF_MAX)/MAX_CONV
#define Vz(z)				(z*ADC_VREF_MAX)/MAX_CONV

/*
 * Sensitivity from Datasheet
 */
#define X_SENS				.295	//.330mV/g -----> g = 9.8m/s^2
#define Y_SENS				.295	//.330mV/g -----> g = 9.8m/s^2
#define Z_SENS				.295	//.330mV/g -----> g = 9.8m/s^2

/*
 * Zero g reference from Datasheet
 */
#define X_0g				(ADC_VREF_MAX / 2)
#define Y_0g				(ADC_VREF_MAX / 2)
#define Z_0g				(ADC_VREF_MAX / 2)

typedef struct{
	uint32_t Xread;
	uint32_t Yread;
	uint32_t Zread;
	float Xvolt;
	float Yvolt;
	float Zvolt;
	float Xg;
	float Yg;
	float Zg;
	float Xangle;
	float Yangle;
	float Zangle;
	float Rvector;
	float Dir_Cosine;
}ACC_DATA_t;
ACC_DATA_t acc;


#define AXIS(x)					(1<<x)
#define ADC_STOP				~(1<<24)
#define ADC_START_NOW			(1<<24)
#define ADC_CR_BURST_ON			(1<<16)
#define ADC_CR_BURST_OFF		~(1<<16)
#define ADC_CR_CH_SEL(x)		(1<<x)
#define ADC_DONE				(1<<31)
#define ADC_DR_RESULT(r)		(((r&0x0000FFC0)>>6)&0x000003FF)

typedef enum
{
	ADC_CLK_10Bits = 0x0, 	// 11 clocks / 10 bits
	ADC_CLK_9Bits = 0x1, 	// 10 clocks / 9 bits
	ADC_CLK_8Bits = 0x2, 	// 9 clocks / 8 bits
	ADC_CLK_7Bits = 0x3, 	// 8 clocks / 7 bits
	ADC_CLK_6Bits = 0x4, 	// 7 clocks / 6 bits
	ADC_CLK_5Bits = 0x5, 	// 6 clocks / 5 bits
	ADC_CLK_4Bits = 0x6, 	// 5 clocks / 4 bits
	ADC_CLK_3Bits = 0x7 	// 4 clocks / 3 bits
}ADC_CLK_Typedef;

#define CLOCKS_CONV_BITS(clk)	(clk<<17)

/*
 * Initialization of ADC for ADXL335
 * 	// First lets init tpin to be used
	// ADC1 = Pins 1.0
	// ADC2 = Pins 1.1
	// ADC3 = Pins 1.2
 */
void 		adxl335_init(void);

void adxl335_ReadAxis(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ADXL335_H */

