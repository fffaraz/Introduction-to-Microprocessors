
//***************************************************************************

// Modified from code for appnote AVR151

//**************************************************************************

#include <avr/io.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include<avr/interrupt.h>


// Intialization Routine Master Mode (polling)
void Init_Master (void)
{
        volatile char IOReg;
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
        char KeyPressed = 0x00;
        DDRD  = 0xFF;          // set port D as output
        PORTD = 0x00;

        Init_Master ();                         // Initialization (polling)

        while (1){
                if (PINA != 0xFF){
                        if (KeyPressed == 0x00){
                                KeyPressed = 0x01;
                               // PORTD = ~PORTD; // toggles Port D 

                                Master_Send ('a');
								PORTD = SPDR;
						//		_delay_loop_1(250);    // delay_us(250);

                        }
                }       
               // else KeyPressed = 0x00;
        }
}

