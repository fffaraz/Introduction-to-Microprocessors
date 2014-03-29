
/**
 * Task 1: Overflow Interrupt
 */


// Compability macros for both IAR and GCC compilers
#include "../avr_compiler.h"


// Defines for PORTS to use

#define LEDPORT PORTE
#define LEDMASK 0x01 // The LED to use for visual feedback.


// Tell compiler to associate this interrupt handler with the TCC0_OVF_vect vector.
ISR(TCC0_OVF_vect)
{
	LEDPORT.OUTTGL = LEDMASK; // Just toggle LED(s) when interrupt is executed.
}


int main( void )
{
	// Set up LED output port.
	LEDPORT.DIRSET = LEDMASK;
	LEDPORT.OUTSET = LEDMASK;
	
	// Set up Timer/Counter 0.
	TCC0.PER = 10000; // Count to 10000 instead of 65536 to speed things up a bit.
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc; // Work from CPUCLK/64.
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_LO_gc; // Enable overflow interrupt.--
	
	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();
	
	// Wait forever and let interrupts do the job.
	while(1) {}
}







