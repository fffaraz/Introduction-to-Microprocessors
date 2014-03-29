#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>
#include<avr/pgmspace.h>
#include<avr/iotn45.h>


#define INPIN	PB3
#define OUTPIN	PB1
#define TESTIN	PB0
#define TESTFREQ 64;
#define FOIL_T 140;
#define EPEE_HITTIME  20;

void int_handle(void);
short getmode(void);
void foil_handle(void);
void epee_handle(void);
void hit(void);

char* PLAYTIME;
volatile short BUZZTIME;
const uint8_t foil_song[] PROGMEM = {  // mary had a little lamb
32,127,32,142,32,159,32,142,28,127,8,1,28,127,8,1,60,127,0,1};

const uint8_t epee_song[] PROGMEM = {  //the minstrel boy
32, 253, 48, 190, 16, 150, 16, 142, 16, 150, 16, 169, 16, 190, 32, 150, 32, 127, 32, 95, 0, 1};

int main(void)
{
short  mode;

// set up port B

	DDRB  = _BV(OUTPIN);	// set output pin
	PORTB = ~_BV(OUTPIN);	// set pullups on inputs

// set up timer 1 for test mode --- turns oscillator on
	GTCCR = 0;
	OCR1C = TESTFREQ;
	// set mode mode CTC (clear on match), toggle output, source clk/2
	TCCR1 = _BV(CTC1)|_BV(COM1A0)|_BV(CS11); 

	while (!PINB & _BV(TESTIN)) {
	}

	TCCR1 = _BV(CTC1)|_BV(COM1A0);	// turn off timer 1

// infinite loop to wait for PCI interrupt and process hit

	while(1) {
		cli();
		mode = getmode();
	// got mode --- foil or !foil

		DDRB = 0;	    // set all pins to input to save power
		PORTB = 0b111111;	// set pullups

		set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // set sleep mode

	// set up PCI interrupts 

		PCMSK |= _BV(INPIN);	// enable PCI interrupt on INPIN
		GIFR   = _BV(PCIF);	// clear any pending PCI interrupts
		GIMSK |= _BV(PCIE);     // enable pin change interrupt

		sei();                         // enable interrupts

	// go to sleep until there is an interrupt
		sleep_mode();

		if (mode) {
			epee_handle();
		}
		else {
			foil_handle();
		}
	
}	// end infinite while loop
}

ISR(PCINT0_vect)
{
}

ISR(TIM0_COMPA_vect)
{
	BUZZTIME--;
}


void foil_handle()
{
// switch normally closed. For hit, switch must be open for 13 -15 ms.
//  enable interupt on switch open - switch reopenings restart the process
//  uses a delay loop to time hit
//  after detecting a valid hit, output "hit" for 2 s
	short i, test;

	PLAYTIME = foil_song;
	for (i = 0; i < 168; i++) {
		test = PINB & _BV(INPIN);
		if (!test) return;
		_delay_loop_1(25);	// delay 100 cycles
	}
	cli();		// 	disable interrupts
	hit();
}

void epee_handle()
{
// switch normally open. For hit, switch must be open for 2-4 ms.
//  enable interupt on switch closed - switch reclosings restart the process
//  uses a delay loop to time hit
//  after detecting a valid hit, output "hit" for 2 s
	short i, test;

	PLAYTIME = epee_song;
	for (i = 0; i < 25; i++) {
		test = PINB & _BV(INPIN);
		if (test) return;
		_delay_loop_1(25);	// delay 100 cycles
	}
	cli();		// 	disable interrupts
	hit();
}

void hit()
{
// timer 1 outputs a tone,
// timer 0 is set to interrupt about every 1/128 second
// timer 0 is used to time the 2 second buzzer
	uint8_t time;
	short i;
	// set OUTPIN to output
	DDRB = _BV(OUTPIN);

	// set up oscillator with timer 1
	GTCCR = 0;
	// set mode mode CTC (clear on match), toggle output, source clk/2
	TCCR1 = _BV(CTC1)|_BV(COM1A0)|_BV(CS11); 

	set_sleep_mode(SLEEP_MODE_IDLE); 

	// set up timer0 (2 seconds)
	TCCR0A = _BV(WGM01);				// timer0 in CTC mode
	TCCR0B = _BV(CS01) | _BV(CS00);		// clk/64
//	OCR0A = 122;						// 1/128 second timer
	OCR0A = 140;
	TIMSK = _BV(OCIE0A);				// interrupt at count
	TIFR = 0;				// clear any pending timer interrrupts
	GIMSK = 0;				// disable external interrupts
	sei();
	time = pgm_read_byte(&PLAYTIME[0]);
	i = 0;
while (time)  {
	BUZZTIME = time;
	i++;
	OCR1C = pgm_read_byte(&PLAYTIME[i]);
	i++;

	while (BUZZTIME >0) {
		sleep_mode();
	}
	time = pgm_read_byte(&PLAYTIME[i]);
}

	DDRB = 0;
	TCCR1 = 0;			// turn off buzzer
	BUZZTIME = 30;			// wait 1/4 second
		while (BUZZTIME >0) {
		sleep_mode();
	}
	TIMSK = 0;			// turn off timer interrupt
	TCCR0B = 0;			// turn off timer 0

	return;
}


short getmode()	// get initial state --- foil or epee
{
	short test;
	short  foil;

	test = PINB & _BV(INPIN);
	_delay_loop_1(255);	// debounce input
	foil =  PINB & _BV(INPIN);
	while (foil != test) {
		_delay_loop_1(255);
		test = foil;
		foil = PINB & _BV(INPIN);
	}
return (foil);
}

