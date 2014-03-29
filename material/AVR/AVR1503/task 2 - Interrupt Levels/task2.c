
/**
 * Task 2: Interrupt levels
 */


// Compability macros for both IAR and GCC compilers
#include "../avr_compiler.h"


// Defines for PORTS to use

#define LEDPORT PORTE
#define LEDMASKA 0x01 // LED toggling on Compare Match A interrupt.
#define LEDMASKB 0x02 // LED toggling on Compare Match B interrupt.
#define LEDMASKC 0x04 // LED toggling on Compare Match C interrupt.


#define SWITCHPORT PORTF
#define SWITCHMASKA 0x01 // Switch to block Compare Match A interrupt.
#define SWITCHMASKB 0x02 // Switch to block Compare Match B interrupt.
#define SWITCHMASKC 0x04 // Switch to block Compare Match C interrupt.


// Interrupt handler for Timer/Counter 0 on Port C, Compare Channel A.
ISR(TCC0_CCA_vect)
{
	// Toggle correct LED.
	LEDPORT.OUTTGL = LEDMASKA;	
	// Loop forever while switch is held down.
	do {} while ((SWITCHPORT.IN & SWITCHMASKA) == 0x00);
}


// Interrupt handler for Timer/Counter 0 on Port C, Compare Channel B.
ISR(TCC0_CCB_vect)
{
	// Toggle correct LED.
	LEDPORT.OUTTGL = LEDMASKB;
	// Loop forever while switch is held down.
	do {} while ((SWITCHPORT.IN & SWITCHMASKB) == 0x00);
}


// Interrupt handler for Timer/Counter 0 on Port C, Compare Channel C.
ISR(TCC0_CCC_vect)
{
	// Toggle correct LED.
	LEDPORT.OUTTGL = LEDMASKC;
	// Loop forever while switch is held down.
	do {} while ((SWITCHPORT.IN & SWITCHMASKC) == 0x00);
}


int main( void )
{
	// Set up IO ports.
	LEDPORT.DIRSET = LEDMASKA | LEDMASKB | LEDMASKC;
	LEDPORT.OUTSET = LEDMASKA | LEDMASKB | LEDMASKC;

	// Configure switches with internal pullups
	PORTCFG.MPCMASK=0xFF;
	SWITCHPORT.PIN0CTRL = PORT_OPC_PULLUP_gc;

	SWITCHPORT.DIRCLR = SWITCHMASKA | SWITCHMASKB | SWITCHMASKC;
	
	// Set up Timer/Counter 0.
	TCC0.PER = 10000;
	TCC0.CCA = 5000; // Let all compare channels have same value
	TCC0.CCB = 5000; // so that they fire interrupts simultaneously.
	TCC0.CCC = 5000; // Value needs to be less than or equal to TCC0.PER register.
	TCC0.CTRLB = TC0_CCCEN_bm | TC0_CCBEN_bm | TC0_CCAEN_bm; // Enable all compare channels
	// Set up levels for all compare channel interrupts:
	TCC0.INTCTRLB = (unsigned char) TC_CCCINTLVL_LO_gc |
	                                TC_CCBINTLVL_MED_gc |
	                                TC_CCAINTLVL_HI_gc;
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc;
	
	// Enable all interrupt levels in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	sei();
	
	// Wait forever and let interrupts do the job.
	while(1) {}
}
