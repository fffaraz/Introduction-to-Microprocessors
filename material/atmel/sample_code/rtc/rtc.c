/***************************************************************/ 
// RTC  - gives minutes, and seconds with full resolution
// interrupts every 1/128 second
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
    cli();                // didable interrupts
    ASSR |= (1<<AS2);     // set Timer/Counter2 to be asynchronous,
                          // driving a crystal
	TCCR2A = 0;       // normal mode
    TIMSK2 = _BV(TOIE2);  //set 8-bit Timer/Counter0 Overflow Interrupt Enable                             
    _delay_ms(1000);	  // let oscillator stabilize
    TCNT2 = 0x00;         // set counter to 0
    TCCR2B = _BV(CS20);   // start counter, no prescaling
//    while(ASSR&0x07);   //Wait until TC0 is updated
    TIFR2=0;		  //clear any pending timer interrupts

    sei();                //enable interrpts                              
}

void display()
{
	PORTD = ~t.second;
	_delay_us(10);
/*
	PORTD = t.minute;
	_delay_us(10);
*/
}


ISR(TIMER2_OVF_vect)
{
	if (++t.tick==128)
	{
		t.tick=0;
		if (++t.second==60)
		{
			t.second=0;
			if (++t.minute==60) 
			{
				t.minute=0;
			}  
		}
	}
}
