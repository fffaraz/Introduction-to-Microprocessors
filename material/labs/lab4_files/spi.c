
//***************************************************************************

// Modified from code for appnote AVR151

//**************************************************************************

#include "spi.h"
void Init_IO (void)
{
	DDRD  = 0x00;		// setb port D to input
	PORTD = 0xFF;		// set pullups on port D
	DDRC  = 0xFF;		// set port C as output
	PORTC = 0xFF;
}

// Intialization Routine Master Mode (polling)
void Init_Master (void)
{
        char IOReg;
        // set PB4(/SS), PB5(MOSI), PB7(SCK) as output 
        DDRB    = (1<<PB4)|(1<<PB5)|(1<<PB7);
        // enable SPI in Master Mode with SCK = CK/4
        SPCR    = (1<<SPE)|(1<<MSTR);
        IOReg   = SPSR;                 	// clear SPIF bit in SPSR
        IOReg   = SPDR;
}



// Sending Routine Master Mode (polling)
unsigned char Master_Send (unsigned char byte)
{
	SPDR  = byte;			// send Character
	while (!(SPSR & (1<<SPIF)));    // wait until Char is sent
	return(SPDR);
}


int main (void)
{
	Init_IO ();
	Init_Master ();                         // Initialization (polling)

	while (1){
		if (PIND != 0xFF){
			PORTC = Master_Send (PIND);
			_delay_us(250);
		}
		else PORTC = 0xFF;
	}
}

