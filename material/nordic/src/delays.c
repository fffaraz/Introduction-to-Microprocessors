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

//This code depends on a clock frequency of 40 MHz.

#include "delays.h"
#include <delays.h>

void DelayUS(unsigned long microseconds)
{
	unsigned long count;
	
	for(count = 0; count < microseconds; count++)
		Delay10TCY();
}

void DelayMS(unsigned long milliseconds)
{
	unsigned long count;
	
	for(count = 0; count < milliseconds; count++)
		Delay10KTCYx(1);
}

void DelayS(unsigned long seconds)
{
	unsigned long count;
	
	for(count = 0; count < seconds; count++)
	{
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
	}
}
