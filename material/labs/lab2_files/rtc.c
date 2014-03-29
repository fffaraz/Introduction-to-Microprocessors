/***************************************************************/ 
// RTC  - needs minutes, and seconds with full resolution
// interrupts every 1/128 second
// port A, B, and D are output

/***************************************************************/ 

#include "rtc.h"     
                                                        
int main(void)
{
	init_io();
	init_rtc();
	
	set_sleep_mode(SLEEP_MODE_IDLE);
	while(1)                     
	{
		sleep_mode();	//will wake up from time overflow interrupt 
		display();
	}            
}

void init_io()
{
	DDRD = 0XFF;		// port D is output
	PORTD = 0X00;
}

void init_rtc()
{
	cli();           
	ASSR |= _BV(AS2);	// set Timer/Counter2 to be asynchronous,
 				// driving a crystal
	TCNT2 = 0x00;
	TCCR2A = 0;
	TCCR2B = _BV(CS20);	//prescale the timer to be clock source
	TIMSK2 = _BV(TOIE2);	//set 8-bit Timer/Counter0 Overflow Interrupt Enable                             
	TIFR2=0;		//clear any pending timer interrupts
	_delay_ms(1000);	// let crystal stabilize

	sei();			//enable interrpts                              
}

void display()
{
	PORTD = t.second;
	// PORTA = t.minute;	// to output minutes, too
	_delay_us(10);		// not really necessary
}


ISR(TIMER2_OVF_vect)
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
