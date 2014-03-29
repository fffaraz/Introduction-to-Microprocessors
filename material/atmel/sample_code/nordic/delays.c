/*****************************************************************************
*
* File: delays.c
* 
* Copyright S. Brennen Ball, 2006-2007
* 
* The author provides no guarantees, warantees, or promises, implied or
*	otherwise.  By using this software you agree to indemnify the author
* 	of any damages incurred by using it.
* 
*****************************************************************************/

#include "delays.h"
#include "lpc214x.h"

void DelayUS(unsigned int microseconds)
{
double us, max;
	while (microseconds) {
		max  = 768000000/F_CPU;
		if (microseconds > max) {
			us = max;
			delay_us(us);
			microseconds -= max;
		}
		else {
		 	us = microseconds;
			delay_us(us);
			microseconds = 0;
		}
	}
}
