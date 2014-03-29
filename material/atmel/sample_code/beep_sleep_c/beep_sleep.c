#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>
#include <avr/iom644.h>

// works, but interrupt doesn't simulate when sleep mode set

void int_handle(void);
void buzz(void);

// input (button switch) is on pin PB4
// output (speaker) is on pin PB5

int main(void)
{
	// set up port B

	DDRB  = _BV(DDB5);             // set port B pin 5 as output
	PORTB = ~(_BV(DDB5));          // set pullups on inputs

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // set sleep mode

	// set up interrupts 

	PCMSK1 |= _BV(PCINT12);        // enable PCI interrupt on PB4

	PCICR  |= _BV(PCIE1);          // enable pin change interrupt 1
	PCIFR  |= _BV(PCIF1);          // clear pin change interrupt flag 1

	sei();                         // enable interrupts

	// infinite loop to wait until button is pressed
	while (1) {
		sleep_mode();
		buzz();
	}
	return(1);
}

ISR(SIG_PIN_CHANGE1)
{
}

void buzz()
{
	while(!(PINB & (_BV(PB4))))  {
	// alternately write 1 and 0 to PB5
		PORTB = _BV(PB5);
		_delay_loop_1(128);
		PORTB = 0;
		_delay_loop_1(128);
	}
	return;
}
