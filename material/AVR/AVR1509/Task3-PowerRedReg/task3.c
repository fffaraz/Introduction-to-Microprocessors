/*************************************************************************
 *
 *
 *              Task3: Using Power Reduction Registers
 *
 *
 *************************************************************************/

// Include header files for GCC/IAR
#include "../avr_compiler.h"
#include "../sleepmgr.h"
#include "../lowpower_macros.h"

#define SWITCHPORT PORTF
#define LEDPORT PORTE
#define LEDMASK 0xF0

enum switch_mask {SW_GEN_TC = 0x01, SW_COM = 0x02, SW_ANLG = 0x04, SW_ON = 0x08};

// The SLEEP-instruction
#define sleep() __asm__ __volatile__ ("sleep")

void facilitatePowersaving(void);

int main(void)
{
	facilitatePowersaving();

	// Configure switches
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	SWITCHPORT.PIN0CTRL = (SWITCHPORT.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc; //Enable pull-up to get a defined level on the switches
	SWITCHPORT.DIRCLR = 0xff; // Set port as input

	// Set up interrupt on buttons
	SWITCHPORT.INTCTRL = (SWITCHPORT.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;
	SWITCHPORT.INT0MASK = 0x0F;

	// Configure LEDs
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1

	LEDPORT.DIRSET = 0xff; 	// Set port as output
	LEDPORT.OUT = 0x00;  // Set initial value


	// Set up RTC timer and overflow interrupt
	CLK.RTCCTRL = (CLK.RTCCTRL & ~CLK_RTCSRC_gm) | CLK_RTCSRC_RCOSC_gc; // Choose 1kHz input from internal 32kHz RC oscillator.
	CLK.RTCCTRL	|= CLK_RTCEN_bm;	// Enable RTC
	
	// Now we get 1024 ticks per second.	
	// We set the period to 1024, resulting in 1Hz toggling of the LED.
	RTC.PER = 1024;
	RTC.CTRL = (RTC.CTRL & ~RTC_PRESCALER_gm) | RTC_PRESCALER_DIV1_gc; // Set RTC prescaler 1
	RTC.INTCTRL = (RTC.INTCTRL & ~RTC_COMPINTLVL_gm) | RTC_COMPINTLVL_LO_gc; // Enable LO interrupt on compare match
	
	// Use IDLE mode between wakeups
	SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_IDLE_gc;
	SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep, else SLEEP-instruction is ineffective.

	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	char pressed = 0;

	// Main loop constantly checks buttons and toggles leds & respective PRR
	while (1) {
	    
// == Button 0 pressed
		if (((SWITCHPORT.IN & SW_GEN_TC) | pressed) == 0x00) {		
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling
			LEDPORT.OUTSET = SW_GEN_TC;

			DISABLE_GEN();
			DISABLE_TC();
			//DISABLE_JTAG();

// == Button 1 pressed
		} else if (((SWITCHPORT.IN & SW_COM) | pressed) == 0x00) {	
			_delay_ms(5);
			pressed = 1;
			LEDPORT.OUTSET = SW_COM;

			DISABLE_COM();
			

// == Button 2 pressed
		} else if (((SWITCHPORT.IN & SW_ANLG) | pressed) == 0x00) {
			_delay_ms(5);
			pressed = 1;
			LEDPORT.OUTSET = SW_ANLG;

			DISABLE_ANLG();

		
// == Button 3 pressed
		} else if (((SWITCHPORT.IN & SW_ON) | pressed) == 0x00) { 
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling.

			// Toggle between Extended Standby and clearing the PR Registers and enter IDLE
			if (!(SLEEP.CTRL & SLEEP_SMODE_gm)) {
				// Workaround: If RTC is disabled, going into ESTDBY will actually increase consumption vs IDLE on some devices
				PR.PRGEN &= ~PR_RTC_bm;	
				SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_ESTDBY_gc;
				LEDPORT.OUTSET = SW_ON;
			} else {
				SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_IDLE_gc;
				LEDPORT.OUT &= ~0x0F; 
				PR.PRGEN = PR.PRPA = PR.PRPB = PR.PRPC = PR.PRPD = PR.PRPE = PR.PRPF = 0x00;
			
				CCP=0xD8;
				MCU.MCUCR &= ~MCU_JTAGD_bm;	// Clear JTAG Disable bit.

				// Remembering to reinitialize RTC				
				RTC.PER = 1024;
				RTC.CTRL = (RTC.CTRL & ~RTC_PRESCALER_gm) | RTC_PRESCALER_DIV1_gc; // Set RTC prescaler 1
				RTC.INTCTRL = (RTC.INTCTRL & ~RTC_COMPINTLVL_gm) | RTC_COMPINTLVL_LO_gc; // Enable LO interrupt on compare match
			}

		} else if ( (SWITCHPORT.IN & (SW_GEN_TC | SW_COM | SW_ANLG | SW_ON)) == 0x0F) {
			pressed = 0;
			sleep();
		
		} // Button released.

	}
	
}

ISR(RTC_COMP_vect) {
	LEDPORT.OUTTGL = LEDMASK;
}

ISR(PORTF_INT0_vect) {
	// Empty
}

void facilitatePowersaving(void) {
	// Pullup on all ports, to ensure a defined state.
	PORTCFG.MPCMASK = 0xFF;
	PORTA.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTB.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTF.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTH.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTJ.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTK.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTQ.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTR.PIN0CTRL = PORT_OPC_PULLUP_gc;

	// Because of the audio amplifier, we have to define pulldown on PQ3
	// NTC is already disabled via pullup
	PORTQ.PIN3CTRL = PORT_OPC_PULLDOWN_gc;

	// The potentiometer has an analog voltage, thus both pullup and pulldown in tristate will source current. ~35µA.
	PORTB.DIRSET = (1<<PIN1);

}
