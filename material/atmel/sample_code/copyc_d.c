#include <avr/io.h>

int main(void)
{	
	DDRC=0x00;	// set port C to input
	PORTC=0xFF;	// set pullups
	DDRD=0xFF;	// set port D to output

	while (1)
	{
		PORTD=PINC;	// read port C and write
				// contents to port D
	}
	
}
