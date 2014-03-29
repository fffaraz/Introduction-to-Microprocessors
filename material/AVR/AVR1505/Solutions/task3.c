/*
 *                                    -= Task 3 =-
 *
 *  In this task we will learn about Virtual Channels.
 *  Each virtual channel is fully independent of each other. They all have separate:
 *     -	Input selection through dedicate CHnMUXCTRL register.
 *     -	RESULT registers.
 *     -    Interrupt control/vectors.
 *	   -    Start conversion bits.
 */


#include <avr/io.h>
#include "../adc_driver.h"

#define LEDPORT PORTE      //Define port for connection of LEDS

 // Variables for use when we read the result from the ADC channels
volatile uint16_t ADC_result_CH0;
volatile uint16_t ADC_result_CH1;

int main(void)
{
	// Add code to sweep CH0 and CH1 in free running mode
	// Use the function call in the adc_driver.h
	ADC_SweepChannels_Config( &ADCB, ADC_SWEEP_01_gc);


	//Enable internal temperature sensor, to be used by CH1
	ADC_TempReference_Enable(&ADCB);

	// Setup CH1 to have single ended input as in task1 and task2
	ADC_Ch_InputMode_and_Gain_Config(&ADCB.CH0,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_CH_GAIN_1X_gc);

	// Set input to CH0 in ADC B to be PIN 1
	ADC_Ch_InputMux_Config(&ADCB.CH0,
	                       ADC_CH_MUXPOS_PIN1_gc,
	                       0);

	// Setup CH1 to read internal signal
	ADC_Ch_InputMode_and_Gain_Config(&ADCB.CH1,
	                                 ADC_CH_INPUTMODE_INTERNAL_gc,
	                                 ADC_CH_GAIN_1X_gc);

	// CH1 is set up to measure the internal temperature sensor
	ADC_Ch_InputMux_Config(&ADCB.CH1,
	                       ADC_CH_MUXINT_TEMP_gc,
	                       0);

	// Set up ADC B to have unsigned conversion mode and 12 bit resolution
	ADC_ConvMode_and_Resolution_Config(&ADCB, false, ADC_RESOLUTION_12BIT_gc);

	// Set reference voltage on ADC B to be VCC/1.6 V
	ADC_Reference_Config(&ADCB, ADC_REFSEL_VCC_gc);

	// Sample rate is CPUFREQ/16.
	ADC_Prescaler_Config(&ADCB, ADC_PRESCALER_DIV16_gc);

	// Enable ADC B
	ADC_Enable(&ADCB);

	// Wait until common mode voltage is stable. Default clk is 2MHz and
	// therefore within the maximum frequency to use this function.
	ADC_Wait_8MHz(&ADCB);

	// Enable ADC B free running mode
	ADC_FreeRunning_Enable(&ADCB);

	// Set the LEDPORT as output
	LEDPORT.DIR = 0xFF;

	while(1) {
		// When CH1IF is set, both conversions are done since CH0 is started first
		// Wait for CH1IF to be set
		do {
		} while ((ADCB.INTFLAGS & ADC_CH1IF_bm) != ADC_CH1IF_bm);

		// Clear CH1 Interrupt Flag
		ADCB.INTFLAGS |= ADC_CH1IF_bm;

		// Read the CH0 result register, 12 bit unsigned (0-4095)
		ADC_result_CH0 = ADCB.CH0RES;
		// Read the CH1 result register
		ADC_result_CH1 = ADCB.CH1RES;

		// Shift CH0 result to get the 4 MSB of the input signal on the 4 LSB of the LEDs
		ADC_result_CH0 >>= 8;
		// Shift CH1 result to get the 4 MSB of the temperature on the 4 MSB of the LEDs
		// Also downscale it to 4 bit. Try touching the Xmega to warm it up.
		ADC_result_CH1 = ((ADC_result_CH1 - 1) / 16);
		ADC_result_CH1 <<= 4;

		// Output on the LEDs, the 4 MSB is the internal temperature reading,
		// and the 4 LSB is the single ended input reading,
		LEDPORT.OUT = ~( (ADC_result_CH1 & 0x00F0) | (ADC_result_CH0 & 0x000F));
	}
}
