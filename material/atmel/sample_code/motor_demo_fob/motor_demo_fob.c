#include <avr/io.h>
#include<avr/sleep.h>
#include<avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "nordic-nRF24L01.c"

#define TRUE 1;
#define FALSE 0;

char old_index;
int8_t running = FALSE;

// define functions
void init(void);
void timer2_pwm_init(void);

void handle_key(char key);
char getkey(void);

void timer2_pwm_init(void)
{
	// set fast PWM, NON-INVERTING OUTPUT
	TCCR2A = _BV(COM2A1)|_BV(COM2B1)|_BV(WGM21)|_BV(WGM20);

	TCCR2B = 0;		// timer stopped
	OCR2A = 0;
	OCR2B = 0;
	DDRD |= _BV(PD6)|_BV(PD7);

	// enable interrupts - for sleep

	TIMSK2 = _BV(TOIE2);	//set 8-bit Timer/Counter2 Overflow Interrupt Enable                             
//    TIMSK2 = _BV(OCIE2A);	// set interrupt mask to overflow
	TIFR2=0;				//clear any pending timer interrupts
}

void init(void)
{
// disable watchdog timer
	MCUSR = 0;
	wdt_disable();

	timer2_pwm_init();
	
	init_nRF_pins();

	//	set sleep mode to wait out interrupts
	set_sleep_mode(SLEEP_MODE_IDLE);
}

ISR(TIMER2_OVF_vect)	// for ATMega644
{
}

char getkey()
{
	char rxkey;
	uint8_t incoming, index;

	incoming = rx_send_byte(0xFF);	// read status register
	if (incoming == 0xFF) return(0);	// receiver not installed
	if (!(incoming & 0x40)) return(0);	// no data received


	// otherwise we got data
	receive_data();
	index = data_array[2];
//	if (index == old_index) return(0);
//	old_index = index;

	rxkey = data_array[0] & 0x1F;	// assumes low order 5 bits only used
	return(rxkey);
}


int
main( void )
{
	char key;

	init();

	PORTC = 0x00;		// all off
	DDRC  = 0x3F; 		// show byte in leds
	DDRD  |= 0xFE;		// port D has timer and control, usart output (PD0 is RxD0)
	PORTD = 0;			// motor is in braking
	
	configure_receiver();	

	sei();	// enable interrupts

	PORTD |= _BV(PD2) | _BV(PD4);	
	PORTD &= ~(_BV(PD3) | _BV(PD5));

//	PORTD &= ~(_BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5));

	OCR2A =0;
	OCR2B = 0;

	//	start timer
//	TCCR2B = _BV(CS22)|_BV(CS20);	// update approx. 244 times per sec. at 8 MHz
	TCCR2B = _BV(CS21);				// update approx. 488 times per sec. at 1 MHz

// start control loop

    while ( 1 ) {
		key = getkey();		
		if (key) {
			PORTC = key;
			handle_key(key);
		}
		sleep_mode();	// sleep until timer interrupt
//		_delay_ms(5);	
	}

    return 0;
}


void handle_key(char key)
{
	switch (key)
	{
	case 0x0F:		// stop - center key
		PORTD &= ~(_BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5));
		OCR2A = 0;
		OCR2B = 0;
		running = FALSE;
		break;

	case 0x1D:		// forward or speed up - top key
		if (running)
		{
			if (OCR2A <251) OCR2A += 5;
			if (OCR2B <251) OCR2B += 5;
		}
		else
		{
			PORTD |= _BV(PD2) | _BV(PD4);	
			PORTD &= ~(_BV(PD3) | _BV(PD5));

			OCR2A = 50;
			OCR2B = 50;
			running = TRUE;
		}
		break;

	case 0x1E:		// backward or slow down - bottom key
		if (running)
		{
			if (OCR2A >4) OCR2A -= 5;
			if (OCR2B >4) OCR2B -= 5;
			}
		else
		{
			PORTD |= _BV(PD3) | _BV(PD5);
			PORTD &= ~(_BV(PD2) | _BV(PD4));

			OCR2A = 50;
			OCR2B = 50;
			running = TRUE;
		}
		break;

		case 0x17:		// increment left speed - left button
		if (OCR2A <251) OCR2A += 5;
		break;

		case 0x1B:	// increment right speed
		if (OCR2B <251) OCR2B += 5;
		break;

		default: break;
	}
}



