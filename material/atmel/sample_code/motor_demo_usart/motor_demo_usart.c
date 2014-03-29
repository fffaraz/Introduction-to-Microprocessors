// bluetooth control for motors - two motors controlled from
// one timer.
// 
// to use with bluetooth, connect bluetooth Tx to RxD0 (PD0).
// Rx needs connection only if the controller needs to communicate
// with the bluetooth device (say, for configuration)
//
#include "motor_demo_usart.h"
#include "usart0.c"

char old_index;
int8_t running = FALSE;

void timer2_pwm_init(void)
{
	// set fast PWM, NON-INVERTING OUTPUT
	TCCR2A = _BV(COM2A1)|_BV(COM2B1)|_BV(WGM21)|_BV(WGM20);

	TCCR2B = 0;		// timer stopped
	OCR2A = 0;
	OCR2B = 0;
	DDRD |= _BV(PD6)|_BV(PD7);

	// enable interrupts - for sleep - not needed here

//	TIMSK2 = _BV(TOIE2);	//set 8-bit Timer/Counter2 Overflow Interrupt Enable                             
//	TIMSK2 = _BV(OCIE2A);	// set interrupt mask to overflow
	TIFR2=0;				//clear any pending timer interrupts
}

void init(void)
{
// disable watchdog timer
	MCUSR = 0;
	wdt_disable();

	timer2_pwm_init();
//	usart0_init(9600);

	//	set sleep mode to wait out interrupts
	set_sleep_mode(SLEEP_MODE_IDLE);
}

/* interrupts not needed
ISR(TIMER2_OVF_vect)	// for ATMega644p
{
}
*/

int
main( void )
{
	char key;

	init();
	CLKPR = _BV(CLKPCE); // enable clock scalar update
    CLKPR = 0x00;		// set to 8Mhz
    PORTC = 0xff;		// all off
    DDRC  = 0xff; 		// show byte in leds
    usart0_init( 51 );	// 9600 baud at 8MHz


	PORTC = 0x00;		// all off
	DDRC  = 0x3F; 		// show byte in leds
	DDRD  |= 0xFE;		// port D has timer and control, usart output (PD0 is RxD0)
	PORTD = 0;		// motors are off
	
	PORTD |= _BV(PD2) | _BV(PD4);		// set motor direction	
	PORTD &= ~(_BV(PD3) | _BV(PD5));

	OCR2A =0;
	OCR2B = 0;

	//	start timer
//	TCCR2B = _BV(CS22)|_BV(CS20);	// update approx. 244 times per sec. at 8 MHz
	TCCR2B = _BV(CS21);				// update approx. 488 times per sec. at 1 MHz

// start control loop

    while ( 1 ) {
		key = usart0_get();	
		if (key) {
			PORTC = key;
			handle_usart_0(key);
		}
	}

    return 0;
}

//	usart code
void handle_usart_0 (char key)
{
	if ((key == 'S') | (key == 's')) {
		PORTD &= ~(_BV(PD2)|_BV(PD3)|_BV(PD4)|_BV(PD5));
	}

	if ((key == 'F') | (key == 'f')) {
		PORTD |= _BV(PD2)|_BV(PD4);
		PORTD &= ~(_BV(PD3)|_BV(PD5));			
	}
	if ((key == 'B') | (key == 'b')) {
		PORTD |= _BV(PD3)|_BV(PD5);
		PORTD &= ~(_BV(PD2)|_BV(PD4));
	}
	if (key == '*') {
		if (OCR2A < 250) OCR2A += 5;
		if (OCR2B < 250) OCR2B += 5;
	}
	if (key == '-') {
		if (OCR2A >5) OCR2A -= 5;
		if (OCR2B >5) OCR2B -= 5;
	}

	if (key == 'l') {
		if (OCR2A >5) OCR2A -= 5;
	}
	if (key == 'L') {
		if (OCR2A <250) OCR2A += 5;
	}

	if (key == 'r') {
		if (OCR2B >5) OCR2B -= 5;
	}
	if (key == 'R') {
		if (OCR2B <250) OCR2B += 5;
	}
}

