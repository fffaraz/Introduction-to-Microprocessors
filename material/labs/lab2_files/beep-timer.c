#include "beep-timer.h"

// input (button switch) is on pin PB4
// output (speaker) is on pin PB3 (timer 0 output OC0A)

int main(void)
{
	initialize();

	// infinite loop, sleeping until button is pressed
	while (1) {
		sleep_mode();
		buzz();
	}
	return(1);
}

void initialize(void)
{
	// set up port B

	DDRB  = _BV(DDB3);             // set port B pin 3 as output
	PORTB = ~(_BV(DDB4));          // set pullups on inputs

	// set up interrupts 

	PCMSK1 |= _BV(PCINT12);        // enable PCI interrupt on PB4
	PCICR  |= _BV(PCIE1);          // enable pin change interrupt 1
	PCIFR  |= _BV(PCIF1);          // clear pin change interrupt flag 1

	// set up timer 0 to CTC mode
	// initialize  timer0 to CTC mode
        TCCR0A = _BV(WGM01)|_BV(COM0A0);     // timer0 in CTC mode,
			                     // toggle OC0A on compare match
        TIFR1 = _BV(OCF1A);                  // clear timer flag

        OCR0A = 238;                         // set output frequency (note C7)

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // set sleep mode

	sei();                         // enable interrupts
}

ISR(PCINT1_vect)
// empty handler for PCI; the only  function is to
// wake up the processor
{
}

void buzz()
{
	TCCR0B = _BV(CS00);		// start timer, no prescale
	while(!(PINB & (_BV(PB4))));	// button still pressed?
	TCCR0B = 0;			// turn off timer	
	return;
}
