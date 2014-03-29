#include <avr/io.h>
#include <avr/interrupt.h>
#include "../adc_driver.h"

#define LEDPORT PORTE      //Define port for connection of LEDS

// Variable for use when we read the result from an ADC channel
volatile uint8_t ADC_result;

void InitADC(ADC_t *ADC_Pointer)
{
	// Initialize sweep for channel 0, 1, 2 and 3
	//ADD CODE: ADC_SweepChannels_Config( ADD_CODE, ADD_CODE );

	// Setup event to start synchronized sweep
	//ADD CODE: ADC_Events_Config( ADC_Pointer, ADC_EVSEL_0123_gc, ADD_CODE );

	// Initialize the four channels to convert in single ended mode
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH0, ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc );
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH1, ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc );
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH2, ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc );
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH3, ADC_CH_INPUTMODE_SINGLEENDED_gc, ADC_CH_GAIN_1X_gc );

	// Route the channels to different pins
	// Note that in Single Ended Mode, there is no negative input
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH0, ADC_CH_MUXPOS_PIN1_gc, 0 );
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH1, ADC_CH_MUXPOS_PIN2_gc, 0 );
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH2, ADC_CH_MUXPOS_PIN3_gc, 0 );
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH3, ADC_CH_MUXPOS_PIN4_gc, 0 );

	// Sample rate is CPUFREQ / 32. @ 2 MHz this equals 62,5ksps
	ADC_Prescaler_Config( ADC_Pointer, ADC_PRESCALER_DIV32_gc);

	// Set up ADCx  to have unsigned conversion mode and 8 bit resolution
	ADC_ConvMode_and_Resolution_Config( ADC_Pointer, false, ADC_RESOLUTION_8BIT_gc );

	// Set reference voltage on ADCx to be VCC/1.6 V
	ADC_Reference_Config( ADC_Pointer, ADC_REFSEL_VCC_gc );

	// Enable the ADC
	ADC_Enable( ADC_Pointer );

	// Wait until common mode voltage is stable. Default clk is 2MHz and
	// therefore within the maximum frequency to use this function.
	ADC_Wait_8MHz( ADC_Pointer );
}

int main(void)
{
	// Set up the ADC_Pointer to the ADC module to use
	//  ADD CODE HERE

	// We will output the result to the LEDs
	LEDPORT.DIR = 0xFF;

	// Set default state to off for the LEDs
	LEDPORT.OUT = 0xFF;

	// Initialize the ADC module by calling the InitADC function
	InitADC( ADC_Pointer );

	// Initialize the peripherals needed for this task:

	// We will use TCC0 overflows to trigger events on channel 0
	//ADD CODE: EVSYS.CH0MUX = // ADD CODE HERE;

	// Start Timer/Counter C0 with main clock (2MHz) divided by 8
	TCC0.CTRLA = (TCC0.CTRLA & ~TC0_CLKSEL_gm) | TC_CLKSEL_DIV8_gc;

	while (1) {
		// When Channel 3 has finished, all the others have finished too
		// Wait for channel 3 interrupt flag
		do {
		} while ((ADC_Pointer->INTFLAGS & ADC_CH3IF_bm) != ADC_CH3IF_bm);

		// Clear the Interrupt Flag
		ADC_Pointer->INTFLAGS |= ADC_CH3IF_bm;

		// Reading out the result
		ADC_result = ADC_Pointer->CH0RES;

		// Invert since a LED is on when a pin is low
		LEDPORT.OUT = ~ADC_result;

		// Note how little code is required to make this run, and start all the ADC conversions.
	}
}
