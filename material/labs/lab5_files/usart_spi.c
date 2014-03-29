// Accepts input from usart0, outputs to SPI
// uses functions defined earlier

#include <stdio.h>
#include "spi_p.h"
#include "usart0.h"

void Init_IO (void)
{
	DDRD  = 0x00;		// setb port D to input 
	PORTD = 0xFF;		// set pullups on port D
				// PD1 is set to output later
	DDRC  = 0xFF;		// set port C as output
	PORTC = 0xFF;
}

// Intialization Routine Master Mode (polling)
void Spi_p_Init (void)
{
        char IOReg;
        // set PB4(/SS), PB5(MOSI), PB7(SCK) as output 
        DDRB    = _BV(PB4)|_BV(PB5)|_BV(PB7);
        // enable SPI in Master Mode with SCK = CK/4
        SPCR    = _BV(SPE)|_BV(MSTR);
        IOReg   = SPSR;                 	// clear SPIF bit in SPSR
        IOReg   = SPDR;
}


// Sending Routine Master Mode (polling)
unsigned char Spi_p_Send (unsigned char byte)
{
	SPDR  = byte;			// send Character
	while (!(SPSR & _BV(SPIF)));    // wait until Char is sent
	return(SPDR);
}


void
usart0_init( uint16_t baud )
{
    DDRD |= _BV(1) ; // enable tx output
    /* UBRR0H = HI(baud); UBRR0L = LO(baud) */
    UBRR0 = baud;
    /* Asynchronous mode, no parity, 1-stop bit, 8-bit data */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00 );

    /* no 2x mode, no multi-processor mode */
    UCSR0A = 0x00;

    /* interrupts disabled, rx and tx enabled, 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

void
usart0_put( uint8_t b )
{
    // wait for data register to be ready
	while ( (UCSR0A & _BV(UDRE0)) == 0 )
		;
	// load b for transmission
	UDR0 = b;
}
    
uint8_t
usart0_get( void )
{
	// poll for data available
	while ( (UCSR0A & _BV(RXC0)) == 0 )
		;
	return UDR0;
}

void Init(void)
{
    CLKPR = _BV(CLKPCE); // enable clock scalar update
    CLKPR = 0x00;	// set to 8Mhz
    PORTC = 0xff;	// all off
    DDRC  = 0xff; 	// show byte in leds
    usart0_init( 51 );	// 9600 baud at 8MHz

    Init_IO ();
    Spi_p_Init ();      // Initialization (polling)

}

static int my_putchar(char c, FILE * stream)
{
	if (c == '\n')
		usart0_put('\r');
	usart0_put(c);
	return(0);
}

static int my_getchar(char c, FILE * stream)
{
	return (usart0_get());
}

int8_t usart0_rx_data_ready ()
{
	// test for data available
	return  (UCSR0A & _BV(RXC0));
}

static FILE my_stream = FDEV_SETUP_STREAM(my_putchar, my_getchar, _FDEV_SETUP_RW);

int
main( void )
{
	unsigned char data;	// register to hold sent
						// and received char
	Init();
	stdout = &my_stream;
	stdin = &my_stream;

    while ( 1 ) {
		while (usart0_rx_data_ready ()) {
		data = getchar();	// get data from usart
		PORTC = ~data;		// display input on LEDs
		PORTB &= ~_BV(PB4);	// select SPI output device
		data = Spi_p_Send (data);	// send/receive data
		PORTB |= _BV(PB4);	// deselect SPI output device
		printf("%c", data);	// print the received data
		}
    }
    return 0;
}


