
void usart1_init( uint16_t baud )
{
DDRD |= _BV(PD3) ; // enable tx output
	/* UBRR1H = HI(baud); UBRR1L = LO(baud) */
	UBRR1 = baud;
	/* Asynchronous mode, no parity, 1-stop bit, 8-bit data */
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10 );

	/* no 2x mode, no multi-processor mode */
	UCSR1A = 0x00;

	/* interrupts disabled, rx and tx enabled, 8-bit data */
	UCSR1B = _BV(RXEN1) | _BV(TXEN1);
}

void usart1_put( char b )
{ 
	// wait for data register to be ready
	while ( (UCSR1A & _BV(UDRE1)) == 0 )
		;
		// load b for transmission
		UDR1 = b;
}

char usart1_get( void )
{
	// poll for data available
	while ( (UCSR1A & _BV(RXC1)) == 0 )
		;
	return UDR1;
}
