/*
 *                                    -= Task 2 =-
 *
 *  In addition to normal single ended conversions the ADC also has a compare function. 
 *  When this is used, the ADC can be set up to do continues conversions, but it will 
 *  not signal a result (i.e. set the channel’s interrupt flag) before the result is 
 *  above or below a selectable threshold value. This feature is very useful for 
 *  applications where you only wish to check that the input signal is not higher or 
 *  lower than a specific voltage. Without the compare function, it would be  natural 
 *  to check the result in software for every conversion. With the compare function in 
 *  the XMEGA ADC, the application will signal an interrupt or event when the threshold 
 *  is reached. This reduces the CPU load, and keeps the resource use to a minimum. 
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../adc_driver.h"

#define LEDPORT PORTE


static int8_t offset;
static int16_t ADC_result;

ISR(ADCB_CH0_vect)
{
	// Getting the 8MSB of the result
	ADC_ResultCh_GetWord_Unsigned(&ADCB.CH0, offset);
	ADC_result = ((ADC_result - 1) / 8);
	LEDPORT.OUT = ADC_result & 0xFF;
}

int main(void)
{
	/* Move stored calibration values to ADC B. */
	ADC_CalibrationValues_Load(&ADCB);

	// Set as output
	LEDPORT.DIR = 0xFF;
	// Turn off the LEDs as default
	LEDPORT.OUT = 0xAA;

	// Set up ADC B to have unsigned conversion mode and a adjusted 12 bit resolution
	ADC_ConvMode_and_Resolution_Config(&ADCB, false, ADC_RESOLUTION_12BIT_gc);

	// Sample rate is CPUFREQ/8
	ADC_Prescaler_Config(&ADCB, ADC_PRESCALER_DIV8_gc);

	// Set reference voltage on ADC B to be internal Vcc
	ADC_Reference_Config(&ADCB, ADC_REFSEL_VCC_gc);

	// Setup channel 0 to have single ended input
	ADC_Ch_InputMode_and_Gain_Config(&ADCB.CH0,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_CH_GAIN_1X_gc);

	// Set positive input to the channels in ADCB to be PIN 1
	ADC_Ch_InputMux_Config(&ADCB.CH0,
	                       ADC_CH_MUXPOS_PIN1_gc,
	                       ADC_CH_MUXNEG_PIN0_gc);

	// The ADC Compare (CMP) register must be set with the threshold value when
	// compare mode is used. Add code that sets the CMP register to the half of 
	// the ADCB positive resolution
	ADCB.CMP = 2024;

	// Wait until common mode voltage is stable. Default clk is 2MHz and
	// therefore below the maximum frequency to use this function.
	ADC_Wait_8MHz(&ADCB);

	// Setup Interrupt Mode Above Compare Value (ADCB.CMP)
	ADC_Ch_Interrupts_Config(&ADCB.CH0,
	                         ADC_CH_INTMODE_BELOW_gc,
	                         ADC_CH_INTLVL_MED_gc);

	// Enable the Medium Level interrupts needed in this task
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;

	// Enable free running mode
	ADC_FreeRunning_Enable(&ADCB);
	// Enable ADC A
	ADC_Enable(&ADCB);

	/* Get offset value for ADC B.  */
	offset = ADC_Offset_Get_Unsigned(&ADCB, &(ADCB.CH0), true);

	// Enable Global interrupts
	sei();

	// Let the ISR take care of the ADC results
	while(1);
}
