/*************************************************************************
 *
 *
 *              Task2: Generic Drivers
 *
 *
 *************************************************************************/

// The avr_compiler.h header file includes the MCU specific header 
// file configured in the project options and it also contains 
// compatibility definitions to be able to compile the same code 
// for both the GCC and the IAR compiler. 

#include "../avr_compiler.h"


// This function sets all pins to output and uses the "out"
// parameter to set output levels. By having a pointer-to-a-struct
// as a parameter, this function can be re-used for any IO port without
// extra code. This method can also be used for other peripherals such
// as ADCs, DACs, and Timer/Counters. This is possible since all port
// modules have similar internal register layout (same goes for ADCs,
// DACs, Timer/Counters etc).
void SetLEDs( PORT_t * ledPort, unsigned char out )
{
	ledPort->DIR = 0xff; // Make sure all pins are outputs.
	ledPort->OUT = out; // Output requested value.
}


// Same method used on this function, but now for reading
// input values from an IO port.
unsigned char GetSwitches( PORT_t * switchPort )
{
	switchPort->DIR = 0x00; // Make sure all pins are inputs.
	return switchPort->IN; // Read value and return.
}


volatile unsigned char value;

int main( void )
{
	// Prepare pointers to the peripheral port modules we want to use for LEDs and switches.
	PORT_t * ledPort = &PORTE;
	PORT_t * switchPort = &PORTF;

	// Enable pullups for switches. MPCMASK will be explained in task 4.
	PORTCFG.MPCMASK = 0xff;
	switchPort->PIN0CTRL = PORT_OPC_PULLUP_gc;	

	// Now copy switch state to LEDs again and again and again...
	while(1) 
    {
		value = GetSwitches( switchPort );
		SetLEDs( ledPort, value );
	}
}
