/*****************************************************************************
*
* File: spi1.c
* 
* Copyright S. Brennen Ball, 2006-2007
* 
* The author provides no guarantees, warantees, or promises, implied or
*	otherwise.  By using this software you agree to indemnify the author
* 	of any damages incurred by using it.
* 
*****************************************************************************/

#include "spi1.h"
#include <avr/io.h>

void spi1_open()
{
        volatile char IOReg;
        // set PB3(CE), PB4(SS), PB5(MOSI), PB7(SCK) as output
        DDRB    = (1<<PB3)|(1<<PB4)|(1<<PB5)|(1<<PB7);
		// set initial values (CE, SCK =0, CSN = 1) and pullups
		PORTB  = _BV(PB2)|_BV(PB4)|_BV(PB6);
        // enable SPI in Master Mode with SCK = CK/16
        SPCR    = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
        IOReg   = SPSR;      // clear SPIF bit in SPSR
        IOReg   = SPDR;
}

int spi1_read()
{
	if(!(SPSR & _BV(SPIF)))
		return -1;
	
	return SPDR;
}

unsigned char spi1_send_read_byte(unsigned char byte)
{
	SPDR  = byte;          // send Character
        while (!(SPSR & (1<<SPIF)));    // wait until Char is sent
	return (unsigned char) SPDR;
}
