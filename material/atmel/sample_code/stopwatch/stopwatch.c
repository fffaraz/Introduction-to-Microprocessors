/***************************************************************/ 
// RTC for fencing - needs minutes, and seconds with full resolution
// interrupts every 1/128 second
// ports A, B, and D are output
// Port C is switch input (and timer)
/***************************************************************/ 

#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>
#include <avr/power.h>

#define TMAX	3
#define TRUE 	1
#define FALSE	0
#define KEYMASK 0b11000000

unsigned char maxmin = TMAX;

typedef struct{ 
unsigned char tick;	// a 1/128 second tick
unsigned char second;
unsigned char minute;
            }time;

volatile time t;  
volatile unsigned char test;
signed char key_up = 2;
unsigned char timer_on = FALSE;
unsigned char timer_save = 0;

void init_rtc(void);
void init_io(void);
void keyscan(void);
void display(void);     
                                                        
int main(void)
{
	init_io();
	power_all_enable();                                
	init_rtc();
	
	set_sleep_mode(SLEEP_MODE_IDLE);
	while(1)                     
	{
//		sleep_mode();	//will wake up from time overflow interrupt 
		
		keyscan();
		display();
	}            
}

void init_io()
{
	DDRA = 0XFF;
	PORTA = 0X00;		// set pullups 
	DDRB = 0XFF;
	PORTB = 0X00;
	DDRC = 0X00;
	PORTC = 0X3F;	// set pullups and tristate P6, P7
	DDRD = 0XFF;
	PORTD = 0X00;
}

void init_rtc()
{
	cli();           
    ASSR |= (1<<AS2);           //set Timer/Counter0 to be asynchronous, driving a crystal
	TCNT2 = 0x00;
	TCCR2A = 0;
    TCCR2B = _BV(CS20);   //prescale the timer to be clock source
//    while(ASSR&0x07);           //Wait until TC0 is updated
    TIMSK2 = _BV(TOIE2);        //set 8-bit Timer/Counter0 Overflow Interrupt Enable                             
    TIFR2=0;			//clear any pending timer interrupts
	sei();                     //enable interrpts                              
}

void display()
{
	test = TRUE;
	while (test)
	{
		PORTD = t.second;
		_delay_us(10);
/*
		PORTC = 0B00000010;
		PORTD = t.minute;
		_delay_us(10);
*/
	}
}

void keyscan()
{
	char check;
	check = ~(PINC | KEYMASK);
	PORTA = check;
	PORTB = key_up;

	if (check)
	{
		if (key_up)
		{
			if (key_up++ > 3) key_up = 0;
		}
	}
	else
	{
		if (!key_up) key_up = 3;
		if (key_up-- <1) key_up = 1;
	}
	if (key_up == 3)
	{
		if (timer_on) timer_save = TCNT2;
		else TCNT2 = timer_save;
		timer_on = !timer_on;
	}
}

ISR(TIMER2_OVF_vect)
{
	test = FALSE;
	if (timer_on)
	{
		if (++t.tick==128)
		{
			t.tick=0;
			if (++t.second==60)
			{
				t.second=0;
				if (++t.minute==maxmin) 
				{
					t.minute=0;
				}
			}  
		}
	}
}
