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

#include <spi.h>
#include <p18f452.h>

unsigned char spi1_send_read_byte(unsigned char byte)
{
	SSPBUF = byte;
	
	while(!DataRdySPI());
	
	return SSPBUF;
}	
