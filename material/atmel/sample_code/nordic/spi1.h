/*****************************************************************************
*
* File: spi1.h
* 
* Copyright S. Brennen Ball, 2006-2007
* 
* The author provides no guarantees, warantees, or promises, implied or
*	otherwise.  By using this software you agree to indemnify the author
* 	of any damages incurred by using it.
* 
*****************************************************************************/

#ifndef _SPI_H_
#define _SPI_H_

#include <stddef.h>

#ifndef bool
#define bool unsigned char
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true !false
#endif

void spi1_open();
int spi1_read();
unsigned char spi1_send_read_byte(unsigned char byte);

#endif //_SPI_H_
