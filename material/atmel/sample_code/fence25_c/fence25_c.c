#include "fence25_c.h"

void int_handle(void);
uint8_t getmode(void);
void foil_handle(void);
void epee_handle(void);
void hit(void);
void set_tune(void);

uint8_t* PLAYTIME;
uint8_t* foil_song;
uint8_t* epee_song;
volatile uint8_t BUZZTIME;

const uint8_t buzz1[] PROGMEM = {  // a siren?
32,B7, 8,G7, 32,B7, 8,G7, 32,B7, 8,G7, 32,B7, 8,G7, 32,B7, 8,G7, 32,B7, 8,G7, 0,R};

const uint8_t buzz2[] PROGMEM = {  // another siren?
8,B7, 32,A7, 8,B7, 32,A7, 8,B7, 32,A7, 8,B7, 32,A7, 8,B7, 32,A7, 8,B7, 32,A7, 8,B7, 0,R};

const uint8_t mary[] PROGMEM = {  // mary had a little lamb
32,B7, 32,A7, 32,G7, 32,A7, 28,B7, 8,R, 28,B7, 8,R, 60,B7, 0,R};

const uint8_t minstrel[] PROGMEM = {  //the minstrel boy
32,B6, 48,E7, 16,Gs7, 16,A7, 16,Gs7, 16,Fs7, 16,E7, 32,Gs7, 32,B7, 32,E8, 0,R};

const uint8_t twinkle[] PROGMEM = {  // twinkle twinkle little star
28,C7, 8,R, 28,C7, 8,R, 28,G7, 8,R, 28,G7, 8,R, 28,A7, 8,R, 28,A7, 60,G7, 0,R};

const uint8_t ode_to_joy[] PROGMEM = {  // ode to joy
28,E7,  8,R, 28,E7, 8,R, 28,F7, 8,R, 28,G7, 8,R, 28,G7, 8,R, 28,F7, 8,R, 28,E7, 8,R, 28,D7, 8,R, 28,C7, 8,R, 28,C7, 8,R, 28,D7, 8,R, 28,E7, 8,R, 28,E7, 8,R, 28,D7, 8,R, 60,D7 ,0,R};

const uint8_t beethoven5[] PROGMEM = {  // Beethoven's fifth
14,E7, 4,R, 14,E7, 4,R, 14,E7, 4,R, 72,C7,  18,R, 14,D7, 4,R, 14,D7, 4,R, 14,D7, 4,R, 72,B6 ,0,R};

const uint8_t rockyou[] PROGMEM = {  // we will rock you
56,C8, 8,R, 56,B7, 8,R, 56,A7, 8,R, 56,G7, 4,R, 24,A7, 4,R, 24,A7, 4,R, 0,R};

const uint8_t fanfare[] PROGMEM = {  // fanfare
56,C7, 4,R, 28,C7, 4,R, 28,C7, 4,R, 56, E7, 8,R, 56,C7, 8,R, 56,G7, 4,R, 28,G7, 4,R, 28,G7, 4,R, 56,B7, 8,R, 56,G7, 0,R};

const uint8_t cucaracha[] PROGMEM = {  // cucaracha
21,G7, 4,R, 21,G7, 2,R, 21,C9, 4,R, 42,C8, 8,R, 21,E8, 2,R, 21,E8, 4,R, 21,G7, 4,R, 21,G7, 4,R, 42,C8, 6,R, 21,E8, 2,R, 84,E8, 21, C8, 4,R, 21,C8, 4,R, 21,B7, 2,R, 21,B7, 2,R, 21,A7, 2,R, 21,A7, 2,R, 84,G7,  0,R };

const uint8_t* list[] PROGMEM = {buzz1, buzz2, mary, minstrel, twinkle, ode_to_joy, beethoven5, rockyou, fanfare, cucaracha};

// uint8_t ee_foilsong __attribute__((section(".eeprom"))) = 6;
// uint8_t ee_epeesong __attribute__((section(".eeprom"))) = 9;
uint8_t ee_foilsong EEMEM = 3;
uint8_t ee_epeesong EEMEM = 6;

int main(void)
{
	uint8_t  mode;

// set which tunes are played for foil or epee modes
	set_tune();

// infinite loop to wait for PCI interrupt and process hit

	while(1) {
		cli();
		mode = getmode();
	// got mode --- foil or !foil

		DDRB = 0;	    // set all pins to input to save power
		PORTB = 0xFF;	    // set pullups

		set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // set sleep mode

	// set up PCI interrupts 

		PCMSK |= _BV(INPIN);	// enable PCI interrupt on INPIN
		GIFR   = _BV(PCIF);	    // clear any pending PCI interrupts
		GIMSK |= _BV(PCIE);     // enable pin change interrupt

		sei();                         // enable interrupts

	// go to sleep until there is an interrupt
		sleep_mode();

		if (mode) {
			PLAYTIME = epee_song;
			epee_handle();
		}
		else {
			PLAYTIME = foil_song;
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
	uint8_t i, test;

	for (i = 0; i < FOIL_HITTIME; i++) {
		test = PINB & _BV(INPIN);
		if (!test) return;
		_delay_us(100);	// delay 0.1 ms
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
	uint8_t i, test;

	for (i = 0; i < EPEE_HITTIME; i++)
	{
		test = PINB & _BV(INPIN);
		if (test) return;
		_delay_us(100);	// delay 0.1 ms
	}
	cli();		// 	disable interrupts
	hit();
}

void hit()
{
// timer 1 outputs a tone,
// timer 0 is set to interrupt about every 1/128 second

	uint8_t time, i;

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


uint8_t getmode()	// get initial state --- foil or epee
{
	uint8_t test, foil;

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

void set_tune()
{
uint8_t i;
// set up port B

	DDRB  = _BV(OUTPIN);	// set output pin
	PORTB = ~_BV(OUTPIN);	// set pullups on inputs

// set test mode --- play all "hit" sounds if TESTIN is 0

	while (!(PINB & _BV(TESTIN))) {
               for (i=0; i<(sizeof(list)/sizeof(list[0])); i++) {
                        PLAYTIME = (uint8_t*) (pgm_read_word(list+i));
                        hit();
			_delay_loop_1(250);	// delay 1000 cycles

			// select tune using PICKTUNE
                        if (!(PINB & _BV(PICKTUNE))){
                                if (PINB & _BV(FOILEPEE))
                                        eeprom_write_byte(&ee_epeesong, i);
                                else
                                        eeprom_write_byte(&ee_foilsong, i);
                                break;
                        }
                }
	}

	i = eeprom_read_byte(&ee_foilsong);
	if (i < (sizeof(list)/sizeof(list[0])))
		foil_song = (uint8_t*) (pgm_read_word(list+i));
	else
		foil_song = (uint8_t*) (pgm_read_word(list));

	i = eeprom_read_byte(&ee_epeesong);
	if (i < (sizeof(list)/sizeof(list[0])))
		epee_song = (uint8_t*) (pgm_read_word(list+i));
	else
		epee_song = (uint8_t*) (pgm_read_word(list+1));

	return;
}
