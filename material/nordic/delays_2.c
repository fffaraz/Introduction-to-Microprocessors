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
	T0IR = 0;
	T0CTCR = 0;
	T0PR = 60; //60 clock cycles = 1 uS
	T0PC = 0;
	T0TC = 0;
	
	T0TCR = 0x01; //enable timer
	
	//wait until timer counter reaches the desired delay
	while(T0TC < microseconds);
	
	T0TCR = 0x00; //disable timer
}

void DelayMS(unsigned int milliseconds)
{
	T0IR = 0;
	T0CTCR = 0;
	T0PR = 60000; //60000 clock cycles = 1 mS
	T0PC = 0;
	T0TC = 0;
	
	T0TCR = 0x01; //enable timer
	
	//wait until timer counter reaches the desired delay
	while(T0TC < milliseconds);
	
	T0TCR = 0x00; //disable timer
}

void DelayS(unsigned int seconds)
{
	T0IR = 0;
	T0CTCR = 0;
	T0PR = 60000000; //60000000 clock cycles = 1 S
	T0PC = 0;
	T0TC = 0;
	
	T0TCR = 0x01; //enable timer
	
	//wait until timer counter reaches the desired delay
	while(T0TC < seconds);
	
	T0TCR = 0x00; //disable timer
}
