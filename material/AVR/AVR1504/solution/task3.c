
/**
 *  Task 3: Synchronized triggering
 */

#include "../avr_compiler.h"


#define LEDPORT PORTE

volatile uint16_t capture_values[3];


/* Simple function that takes a timer and event channel as input
 * to configure timers of type 0 for input capture. */

void TC0_init(TC0_t *tc, uint8_t EVSEL_CH)
{
  tc->CTRLD |= EVSEL_CH;   
  tc->CTRLD |= TC_EVACT_CAPT_gc;  
  tc->CTRLB = TC0_CCAEN_bm;   
  tc->CTRLA = TC_CLKSEL_DIV64_gc;
}


int main( void )
{
  
  volatile uint16_t capture_values[3];
  
  /* To have some useful output we use the STK600 LEDs */
  
  LEDPORT.DIR = 0xFF;
  LEDPORT.OUT = 0xFF;
  
  /* The initialization from task1 and task2 is moved to functions */
 
  TC0_init(&TCC0, TC_EVSEL_CH0_gc);

  /* Add code to set up timer TCD0, TCE0 to use event channel 0 as input */
  // Add code here
  TC0_init(&TCD0, TC_EVSEL_CH0_gc);
  TC0_init(&TCE0, TC_EVSEL_CH0_gc);
  
  TCC0.PER = 0x5000;
  /* Add code to set the period to the same as TCC0 for TCD0 and TCE0 */
  //Add code here
  TCD0.PER = 0x5000;
  TCE0.PER = 0x5000;

  /* Start a slow timer to be used as a timebase for the event generation */
  
  TCF0.PER = 0x4000;
  TCF0.CTRLA = TC_CLKSEL_DIV256_gc;
  
  /* Add code to set timer overflow of TCF0 as event source to event channel 0 */
  // Add code here:
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
