
/**
 *  Task 4: Manually generating events
 */

#include "../avr_compiler.h"

#define LEDPORT PORTE

/* Simple function that takes a timer and event channel as input
 * to configure timers of type 0 for RESTART.
 */

void TC0_init(TC0_t *tc, uint8_t EVSEL_CH)
{
	tc->CTRLD |= EVSEL_CH;   
	tc->CTRLD |= TC_EVACT_RESTART_gc;  
	tc->CTRLB = TC0_CCAEN_bm;   
	tc->CTRLA = TC_CLKSEL_DIV1_gc;
}

uint16_t capture_values[3];

int main( void )
{
  
	/* To have some useful output we use the LEDs */

	LEDPORT.DIR = 0xFF;
	LEDPORT.OUT = 0xFF;

	/* The initialization from task1 and task2 is moved to functions */

	TC0_init(&TCC0, TC_EVSEL_CH0_gc);
	TC0_init(&TCD0, TC_EVSEL_CH0_gc);
	TC0_init(&TCE0, TC_EVSEL_CH0_gc);

	TCC0.PER = 0x5000;
	TCD0.PER = 0x5000;
	TCE0.PER = 0x5000;

	/* Add code that writes the STROBE register to trigger 
	 * event on event channel 0
	*
	* This will reset all the timers, to ensure that they 
	* are started at the exact same clock cycle
	*
	* Note how the software events override the event channel 
	* mux configuration, in fact the muxes do not have to be 
	* configured at all for software events to be used.
	*/  
    //Add code here.
	


	// Reconfigure event action to do input capture
  	TCC0.CTRLD = (TCC0.CTRLD & ~TC0_EVACT_gm) | TC_EVACT_CAPT_gc;
  	TCD0.CTRLD = (TCD0.CTRLD & ~TC0_EVACT_gm) | TC_EVACT_CAPT_gc;
  	TCE0.CTRLD = (TCE0.CTRLD & ~TC0_EVACT_gm) | TC_EVACT_CAPT_gc;


	/* Start a slow timer to be used as a timebase for the 
	 * event generation 
	 */

	TCF0.PER = 0x4000;
	TCF0.CTRLA = TC_CLKSEL_DIV256_gc;

	/* Set timer overflow of TCF0 as event source 
	 * to event channel 0 
	 */
	EVSYS.CH0MUX = EVSYS_CHMUX_TCF0_OVF_gc;

	while (1)
	{
		// Wait for capture interrupt to update capture values
		do {} while ((TCC0.INTFLAGS & TC0_CCAIF_bm) == 0);

		TCC0.INTFLAGS = TC0_CCAIF_bm; 

		capture_values[0] = TCC0.CCA;
		capture_values[1] = TCD0.CCA;
		capture_values[2] = TCE0.CCA;
		LEDPORT.OUT = ~capture_values[0];
	}

}


