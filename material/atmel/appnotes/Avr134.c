 /***************************************************************/ 
// RTC for fencing - needs minutes, and seconds with full resolution
// interrupts every 1/128 second
 /***************************************************************/ 


typedef struct{ 
unsigned char tick;	// a 1/128 second tick
unsigned char second;
unsigned char minute;
            }time;

 time t;      
                                                        
int main(void)
{                                
	init_rtc();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	while(1)                     
	{
		sleep_mode();	//will wake up from time overflow interrupt  
		_NOP();
		TCCR0=0x05;           // Write dummy value to Control register
		while(ASSR&0x07);     //Wait until TC0 is updated
	}            
}

void init_rtc()
{
    int temp0,temp1;   
     
    for(temp0=0;temp0<0x0040;temp0++)   // Wait for external clock crystal to stabilize
    {
        for(temp1=0;temp1<0xFFFF;temp1++);
    }
    DDRB=0xFF;           
    TIMSK &=~((1<<TOIE0)|(1<<OCIE0));     //Disable TC0 interrupt
    ASSR |= (1<<AS0);           //set Timer/Counter0 to be asynchronous, driving a crystal
    TCNT0 = 0x00;
    TCCR0 = 0x01;                 //prescale the timer to be clock source
    while(ASSR&0x07);           //Wait until TC0 is updated
    TIMSK |= (1<<TOIE0);        //set 8-bit Timer/Counter0 Overflow Interrupt Enable                             
    sei();                     //enable interrpts
                              
}

interrupt [TIMER0_OVF_vect] void counter(void) //overflow interrupt vector
{ 
	if ++t.tick==128)
	{
		if (++t.second==60)
		{
			t.second=0;
			if (++t.minute==TMAX) 
			{
				t.minute=0;
			}
		}  
	}
}
