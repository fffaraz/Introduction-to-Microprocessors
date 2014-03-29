#include <avr/io.h>

int main(void)
{	
	DDRD=0x00;	// set port D to input
	PORTD=0xFF;	// set pullups
	DDRC=0xFF;	// set port C to output

	while (1)
	{
		PORTC=PIND;	// read port D and write
				// contents to port C
	}
	
}
