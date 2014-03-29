/***************************************************************/ 
// Timer for fencing - needs minutes, and seconds with full resolution
// interrupts every 1/512 second
// ports A and D are output - A selects digit, D displays digit
// Ports C0-6 and B0 are keypad input
/***************************************************************/ 

#include "fencetime.h"

volatile int8_t turn;

typedef struct{ 
unsigned char tick;	// a 1/512 second tick
signed char second_ld;
signed char second_hd;
signed char minute;
            }time;

volatile time t;

typedef struct{
signed char hd;
signed char ld;
		}score;

score red_score;
score green_score;

unsigned char mode;
unsigned char round_no;
unsigned char maxround;
unsigned char timer_on = FALSE;
unsigned char round_done = FALSE;
unsigned char timer_save;
unsigned char maxmin = TMAX;
unsigned char dsp[8], mask[8];

char oldkey, oldkeycount;
char oldskey, oldskeycount;

void init(void);
void init_rtc(void);
void init_io(void);
void keypad(void);
char skeypadscan(void);
char skeyscan(void);
char keypadscan(void);
char keyscan(void);
void display(void);
void increment_score(score*);
void decrement_score(score *);
void reset(void);   
                                                        
int main(void)
{
	init();
	reset();
	
	while(1)                     
	{
		sleep_mode();	//will wake up from timer interrupt

		keypad();

		display();

		if (round_done)
		{
			round_done = FALSE;
			if (mode == 1)
			{
				if (++round_no > maxround)round_no = 1;
//				dsp[7] = pgm_read_byte(&digit[round_no]);
				t.minute = maxmin;			}
		}
	}            
}

void init()
{
//	turn off unused modules
#ifdef __AVR_ATmega16__
	wdt_disable();
	ACSR = _BV(ACD);
	ADCSRA = _BV(ADEN);
#endif

#ifdef __AVR_ATmega644__
// disable watchdog timer
	MCUSR = 0;
	wdt_disable();

	power_all_enable();
	power_adc_disable();
	power_usart0_disable();
	power_twi_disable();
	power_timer1_disable();
	power_timer0_disable();
	power_spi_disable();
//	PRR0 = _BV(PRADC)|_BV(PRUSART0)|_BV(PRSPI)|_BV(PRTWI)|_BV(PRTIM0)|_BV(PRTIM1);
#endif

	init_io();
	init_rtc();

//	set sleep mode to wait out interrupts
	set_sleep_mode(SLEEP_MODE_IDLE);
}

void init_io()
{	
	DDRA = 0XFF;	// select digit
	PORTA = 0XFF;
	DDRB = 0X00;	// keypad start/stop (B0)
	PORTB = 0X00;
	DDRC = 0X00;	// keypad & timer
	PORTC = 0X0F;	// set pullups and tristate P6, P7
	DDRD = 0XFF;	// display digit
	PORTD = 0X00;
}

void init_rtc()
{
	cli();           
    ASSR |= (1<<AS2);		//set Timer/Counter0 to be asynchronous, driving a crystal

#ifdef __AVR_ATmega16__
	TCCR2 = _BV(WGM21) | _BV(CS20);		// set CTC mode
	_delay_ms(500);			// let oscillator stabilize
	TCNT2 = 0x00;
//	while(ASSR&0x07);		// Wait until TC0 is updated
//	TIMSK2 = _BV(TOIE2);	// set 8-bit Timer/Counter0 Overflow Interrupt Enable
	OCR2 = 63;				// set compare register 
	TIMSK = _BV(OCIE2);		// set interrupt mask to overflow
	TIFR=0;					// clear any pending timer interrupts
#endif

#ifdef __A_ATmega644__
	TCCR2A = _BV(WGM21);	// set CTC mode - was TCCR2A = 0
	_delay_ms(500);			// let oscillator stabilize
	TCNT2 = 0x00;
//	while(ASSR&0x07);		//Wait until TC0 is updated
//	TIMSK2 = _BV(TOIE2);	//set 8-bit Timer/Counter0 Overflow Interrupt Enable                             
	OCR2A = 63;				// set compare register
    TIMSK2 = _BV(OCIE2A);	// set interrupt mask to overflow
	TIFR2=0;				//clear any pending timer interrupts
	TCCR2B = _BV(CS20);		//prescale the timer to be clock source
#endif

	sei();					//enable interrpts                              
}

void display()
{
// this should be redistributed, eventually
	if (round_no) dsp[R_] = pgm_read_byte(&digit[round_no]);
	else dsp[R_] = 0;
	dsp[G_H] = pgm_read_byte(&digit[green_score.hd]);
	dsp[G_L] = pgm_read_byte(&digit[green_score.ld]);
	dsp[R_H] = pgm_read_byte(&digit[red_score.hd]);
	dsp[R_L] = pgm_read_byte(&digit[red_score.ld]);
	dsp[T_M] = pgm_read_byte(&digit[t.minute]);
	dsp[T_SH] = pgm_read_byte(&digit[t.second_hd]);
	dsp[T_SL] = pgm_read_byte(&digit[t.second_ld>>1]);
		
//	PORTD = ~((t.second_hd<<4) | t.second_ld);
	PORTD = 0;
	PORTA = ~(1<<turn);	// for direct drive
//	PORTA = 1<<turn;	// for optocouplers
	PORTD = dsp[turn] | mask[turn];
}


//ISR(TIMER2_OVF_vect)
#ifdef __AVR_ATmega16__
ISR(TIMER2_COMP_vect)	// for ATmega16
#endif

//- ISR(TIMER2_COMPA_vect)	// for ATMega644
#ifdef __AVR_ATmega644__
ISR(TIMER2_COMPA_vect)	// for ATMega644
#endif

{
	if (++turn==8)
	{
		turn = 0;
	}

	if (timer_on)
	{
		if (++t.tick==0XFF)
		{
//			t.tick=0;
			if (--t.second_ld<0)
			{
				t.second_ld=19;
				if (--t.second_hd<0)
				{
					t.second_hd=5;
					if (--t.minute<0) 
					{
						timer_on = FALSE;
						round_done = TRUE;
						t.second_ld = 0;
						t.second_hd = 0;
						t.minute = 0;
					}
				}
			}  
		}
	}
}

char skeyscan()
{
	char check;
	check = skeypadscan();

		if (check == 0XFF)  // key not pressed
	{
		oldskeycount = 0;
		oldskey = 0xFF;
	}
	else
	{
		if (check != oldskey)  // was same key pressed last time?
		{
			oldskey = check;
			oldskeycount = 0;
		}
		else					// yes, it was
		{
			if (++oldskeycount > DEBOUNCE)
			{
				oldskeycount = DEBOUNCE + 1;
			}
		}
	}

	if ((oldskeycount == DEBOUNCE) &&(check == 5))
	{
		if (timer_on) timer_save = TCNT2;
		else TCNT2 = timer_save;
		timer_on = !timer_on;
	}
	if (oldskeycount >= DEBOUNCE) return(check);
	else return(0XFF);
}

char keyscan()
{
	char check;
	check = keypadscan();

	if (check == 0XFF)  // key not pressed
	{
		oldkeycount = 0;
		oldkey = 0xFF;
	}
	else
	{
		if (check != oldkey)  // was same key pressed last time?
		{
			oldkey = check;
			oldkeycount = 0;
		}
		else					// yes, it was
		{
			if (++oldkeycount > DEBOUNCE)
			{
				oldkeycount = DEBOUNCE + 1;
			}
		}
	}
		if (oldkeycount == DEBOUNCE) return(check);
		else return(0XFF);
}

/***************************************************************/ 
// 3 x 4 keypad scanner
// Port C is switch register - pins 0, 1, 2, 3 are rows, 4, 5, 6 columns
/***************************************************************/ 

char keypadscan()
#define KEYMASK 0b00001111
// for multiple key presses, it returns the value
// in the LAST column in the code.
// Assumes rows (4) are the low order bits, PORT C
// columns are the next (3) higher order bits
{
	char check, out;

	out = 0XFF;
/*
	PORTC = 0b00001111;
	DDRC  = 0b01000000;
	_delay_us(6);
	check = ~PINC & KEYMASK;

	switch (check) {
		case 0B00000001: out = 2; break;	
		case 0B00000010: out = 0; break;
		case 0B00000100: out = 5; break;
		case 0B00001000: out = 8; break;
		default: break;
	}
*/
	DDRB &= 0b11111110;	// make sure B0 is not output
	DDRC = 0b00010000;
	_delay_us(6);
	check = ~PINC & KEYMASK;

	switch (check) {
		case 0B00000001: out = 1;	break;	
		case 0B00000010: out = 10; break;
		case 0B00000100: out = 4; break;
		case 0B00001000: out = 7; break;
		default: break;
	}

	DDRC = 0b00100000;
	_delay_us(6);
	check = ~PINC & KEYMASK;

	switch (check) {
		case 0B00000001: out = 3;	break;	
		case 0B00000010: out = 11; break;
		case 0B00000100: out = 6; break;
		case 0B00001000: out = 9; break;
		default: break;
	}

	return(out);
}

char skeypadscan()
#define KEYMASK 0b00001111
// for multiple key presses, it returns the value
// in the LAST column in the code.
// Assumes rows (4) are the low order bits, PORT C
// columns are the next (3) higher order bits
{
	char check, out;

	out = 0XFF;

	DDRC = 0X00;			// make sure port C is not output
	DDRB  |= 0b00000001;
	_delay_us(6);
	check = ~PINC & KEYMASK;

	switch (check) {
		case 0B00000001: out = 2; break;	
		case 0B00000010: out = 0; break;
		case 0B00000100: out = 5; break;
//		case 0B00001000: out = 8; break;
		case 0B00001000: out = 5; break;  // key 8 also returns 5
		default: break;
	}

	return(out);
}

void increment_score(score *s)
{
	if (++(s->ld) == 10)
	{
		s->ld = 0;
		if (++(s->hd) == 10)
		{
			s->ld = 9;
			s->hd = 9;	
		}
	}
}

void decrement_score(score *s)
{
	if (--(s->ld) < 0)
	{
		s->ld = 9;
		if (--(s->hd) < 0)
		{
			s->hd = 0;
			s->ld = 0;
		}
	}
}

void keypad()
{
	char key, key2;

	key = skeyscan();

	if (!timer_on)
	{
	// handle other key presses
		key2 = keyscan();
		switch (key2)
		{
			case 1:  // reset or full reset
			if  (key == 2)	// full reset
			{
				mode = 0;
				maxround = 0;
				round_no = 0;
			}
			reset();	// reset counter and others depending on mode
			break;

			case 3:	// set priority for overtime
					// also sets initial time to 1 minite
					// inv sets mode
			if  (key == 2)	//mode
			{
				mode = 1;
				round_no = 1;
				maxround = 3;
			}
			else
			{
				t.minute = 1;
				t.second_hd = 0;
				t.second_ld = 0;
				if (!(mask[R_L] | mask[G_L]))
				{
					if (turn & 0b00000001) mask[G_L] = DP;
					else mask[R_L] = DP;			
				}
			}
			break;
			
			case 4:  // card for red
				if (key == 2) // decrement card
				{
					if (mask[R_H] == DP) mask[R_H] = 0;
					if (mask[T_SL] == DP)
					{
						mask[T_SL] = 0;
						mask[R_H] = DP;
					}
				}
				else // increment card
				{
					if ((mask[R_H] != DP) && (mask[T_SL] != DP))  mask[R_H] = DP;
					else
					{
						mask[R_H] = 0;
						mask[T_SL] = DP;
						increment_score(&green_score);
					}
				}
			break;
			
			case 6:  // card for green 
				if (key == 2) // decrement card
				{
					if (mask[G_H] == DP) mask[G_H] = 0;
					if (mask[R_] == DP)
					{
						mask[R_] = 0;
						mask[G_H] = DP;
					}
				}
				else // increment card
				{
					if ((mask[G_H] != DP) &&(mask[R_] != DP)) mask[G_H] = DP;
					else
					{
						mask[G_H] = 0;
						mask[R_] = DP;
						increment_score(&red_score);
					}
				}
			break;

			case 7:	// change red score
			if (key == 2)
			{
				decrement_score(&red_score);
			}
			else
			{
				increment_score(&red_score);
			}
			break;
			
			case 9:	// change green score
			if (key == 2)
			{
				decrement_score(&green_score);
			}
			else
			{
				increment_score(&green_score);
			}
			break;
			case 10:  // increment minutes
			if (key == 2)
			{
				if (--t.minute<0) t.minute = 0;
			}
			else
			{
				if (++t.minute == 10) t.minute = 9;
			}
			break;

			case 11:  // increment seconds
			if (key == 2)
			{
				t.second_ld = t.second_ld -2;
				if (t.second_ld<0)
				{
					t.second_ld=19;
					if (--t.second_hd<0)
					{
						t.second_hd=5;
					}
				}
			}
			else
			{
				t.second_ld = t.second_ld + 2;
				if (t.second_ld > 19)
				{
					t.second_ld = 0;
					if (++t.second_hd >5) t.second_hd = 0;	
				}
			}
			default: break;
		}
	}
}

void reset()
{
//	if (mode == 0)	// don't reset score or cards in some modes
	{
		mask[G_H] = 0;
		mask[G_L] = 0;
		mask[R_H] = 0;
		mask[R_L] = 0;
		mask[T_M] = DP;
		mask[T_SH] = DP;
		mask[T_SL] = 0;
		mask[R_] = 0;

		red_score.hd = 0;
		red_score.ld = 0;
		green_score.hd = 0;
		green_score.ld = 0;
	}

	t.minute = maxmin;
	t.second_hd  = 0;
	t.second_ld = 0;

	timer_save = 0;

	if (round_no > 0) round_no = 1;
}
