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
#include <util/delay.h>
#include "delays.h"

void DelayUS(unsigned int microseconds)
{
unsigned int max;
double us;
	max  = 768000000/F_CPU;
	while (microseconds) {
		if (microseconds > max) {
			us = max;
			_delay_us(us);
			microseconds -= max;
		}
		else {
		 	us = microseconds;
			_delay_us(us);
			microseconds = 0;
		}
	}
}
