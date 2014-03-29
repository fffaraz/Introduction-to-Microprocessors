/**
 *  Task 1: 32 bit timer/counter
 */

// Setup for different compilers
#include "../avr_compiler.h"

#define LEDPORT PORTE

int main( void )
{
 
  /* To have some useful output we use the XPLAIN LEDs */

  LEDPORT.DIR = 0xFF;
  LEDPORT.OUT = 0xFF;
  
  
  /* We need to set up the event user
   * Which event channel should be input for the peripheral, and what should be the action. */
  
  TCC1.CTRLA = TC_CLKSEL_EVCH0_gc; //Select event channel 0 as clock source for TCC1.

  /* For the event system, it must be selected what event is to be routed through the multiplexer for each
   * event channel used */
  
   /* Select TCC0 overflow as event channel 0 multiplexer input.
    * This is all code required to configure one event channel */
  
  EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
    
  /* TCC0 is used as the 16 LSB for the timer. This runs from the normal (prescaled) Peripheral Clock */  
  
  //Select system clock divided by 2 as clock source for TCC0.
  TCC0.PER = 0xFFFF;
  TCC0.CTRLA = TC_CLKSEL_DIV2_gc;
  
  while (1)
  {
    /* Since the event system does not require any code to route the events, and increment the Most Significant TC,
     * some debug code is required to see the changes. You can put a breakpoint below to see when TCC0 is about to overflow */
    
    if(TCC0.CNT >= 0xFFF0)
    {
      nop();
    }
     
    // Show the value of counter TCC1 on the LEDs:
    LEDPORT.OUT = ~TCC1.CNTL;
    
  }

}
