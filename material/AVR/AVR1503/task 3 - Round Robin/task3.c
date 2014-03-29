
/**
 * Task 3: Round robin
 */


// Compability macros for both IAR and GCC compilers
#include "../avr_compiler.h"


// Defines for PORTS to use

#define LEDPORT PORTE
#define LEDMASK0 0x01 // LED associated with Timer/Counter 0 on Port C.
#define LEDMASK1 0x02 // LED associated with Timer/Counter 1 on Port C.

#define SWITCHPORT PORTF
#define SWITCHMASK_OFF 0x01 // The switch used to turn off Round Robin scheduling.
#define SWITCHMASK_ON 0x02 // The switch used to turn on Round Robin scheduling.


// Interrupt handler for Timer/Counter 0 on Port C, Overflow Interrupt.
ISR(TCC0_OVF_vect)
{
	// As this handler executes very often, we use a counter to
	// decrease the LED toggle rate to a comfortable viewable rate.	
	static long countdown = 0;
	if (countdown-- == 0) {
		countdown = 1000;
		LEDPORT.OUTTGL = LEDMASK0;
	}
}


// Interrupt handler for Timer/Counter 1 on Port C, Overflow Interrupt.
ISR(TCC1_OVF_vect)
{
	LEDPORT.OUTTGL = LEDMASK1;
}


int main( void )
{
	// Set up IO port.
	LEDPORT.DIRSET = LEDMASK0 | LEDMASK1;

	// Configure pullups on switches
	PORTCFG.MPCMASK = 0xff;
	SWITCHPORT.PIN0CTRL = PORT_OPC_PULLUP_gc;
	
	// Set up Timer/Counter 0 on Port C to fire interrupts very often.
	TCC0.PER = 1;
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV1_gc;
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_LO_gc;

	// Set up Timer/Counter 1 on Port C to fire interrupts not so often.
	TCC1.PER = 10000;
	TCC1.CTRLA = ( TCC1.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc;
	TCC1.INTCTRLA = ( TCC1.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_LO_gc;
	
	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();
	
	// Wait forever and let interrupts do the job.
	while(1) 
	{
		// Read switch and turn on or off Round Robin scheduling accordingly.
		// Since the CPU will execute at least one instruction between two
		// interrupt calls, the code below will get executed, however rather slowly.
		if ((SWITCHPORT.IN & SWITCHMASK_ON) == 0x00) {
			PMIC.CTRL |= PMIC_RREN_bm;
		} else if ((SWITCHPORT.IN & SWITCHMASK_OFF) == 0x00)
        {
			PMIC.CTRL &= ~PMIC_RREN_bm;
			PMIC.INTPRI = 0; // Reset priority to default setting. PMIC will not
		}                    // do this for us automatically.
	}
}         
