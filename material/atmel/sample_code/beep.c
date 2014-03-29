// beeps when a button connected to pin B4 is pressed
// output is on pin B5
#include <avr/io.h>
#include <util/delay.h>
#include <avr/iom644.h>

// works, but interrupt doesn't simulate when sleep mode set

void buzz(void);

// input (button switch) is on pin PB4
// output (speaker) is on pin PB5

int main(void)
{
	// set up port B

	DDRB  = _BV(DDB5);             // set port B pin 5 as output
	PORTB = ~(_BV(DDB5));          // set pullups on inputs

	// set up interrupts 


	// infinite loop to wait until button is pressed
	while (1) {
		buzz();
	}
	return(1);
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
