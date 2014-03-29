
/**
 *  Task 2: Input capture
 */

#include "../avr_compiler.h"

#define LEDPORT PORTE
#define SWITCHPORT PORTF


int main( void ){

  volatile uint16_t error_count = 0;
  volatile uint16_t capture_value;


  /* To have some useful output we use the XPLAIN LEDs */

  LEDPORT.DIR = 0xFF;
  LEDPORT.OUT = 0xFF;

  /* Configure switches */
  PORTCFG.MPCMASK = 0xFF;
  SWITCHPORT.PIN0CTRL = PORT_OPC_PULLUP_gc;

  /* Just as in task1 we need to set up the event user
   * Which event channel should be input for the peripheral, and what should be the action. */

  TCC0.CTRLD |= TC_EVSEL_CH0_gc;     //event channel 0 as event source for TCC0
  TCC0.CTRLD |= TC_EVACT_CAPT_gc;   //input capture as event action.

  TCC0.CTRLB = TC0_CCAEN_bm;   //Enable TCC0 Compare on Capture Channel A.

  /* For the event system, we must select what event to route through the multiplexer 
   * for each event channel. This is the same as in task1. */

   /* Add code that select PF0 as event channel 0 multiplexer input.
    * This is done by setting the PORTF_PIN0 group mask for the CH0MUX register */

  //Add code here.

  /* The port pin used for input capture must be configured with the
   * correct pin change interrupt sensing. Events on an I/O pin is generated for the
   * same condition as the interrupt conditions for the pin.
  */

  SWITCHPORT.PIN0CTRL |= PORT_ISC_BOTHEDGES_gc;   //Configure pin to sense both edges.
  SWITCHPORT.DIRCLR = 0x01;                      //Clear direction register to ensure that pin 0 is input


  /* Start TCC0 */

  TCC0.PER = 0xFFFF;
  TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;

  while (1) {


    /*Read the capture_value and output on LED*/
    capture_value = TCC0.CCA;
    capture_value >>=8;
    LEDPORT.OUT = ~capture_value;


     /* Each pin low level generates an event and input capture. This will lead to buffer overflow if the CCA register is
     * not read in time.
     * The TC Error Flag (ERRIF) is set on buffer overflow so count how often this happens*/

    if((TCC0.INTFLAGS & TC0_ERRIF_bm) != 0)
    {
      TCC0.INTFLAGS = TC0_ERRIF_bm;
      error_count++;
    }

  }

}
