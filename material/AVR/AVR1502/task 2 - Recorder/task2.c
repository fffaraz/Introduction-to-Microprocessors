/************************************************************************** 
 *
 *
 *                            Task2: Recorder
 *
 *
 **************************************************************************/

#include "../dma_driver.h"
//#include <avr/interrupt.h>
#include "../avr_compiler.h"
//#include <util/delay.h>

#define LEDPORT PORTE
#define SWITCHPORT PORTF

#define SAMPLE_COUNT 300 // How many switch state samples to store.
uint8_t samples[SAMPLE_COUNT]; // Store switch state samples here.


// The DMA channel is triggered by the Timer Overflow Interrupt Flag,
// and will be continuously re-triggered as long as the flag is set.
// We only want one single data transfer triggered for every overflow.
// Therefore we need to enable interrupts in order to automatically
// clear the Timer Overflow Interrupt Flag when the interrupt handler is
// called. However, we do not have to do anything inside the handler itself.
// That is why we need an empty handler below.


ISR(TCC0_OVF_vect)
{
    nop();
}


// Setup parameters for the channel that read switch states. The channel is
// configured to reload original destination address when the data block is finished.
// Single-shot is enabled so that only one byte gets copyed for every timer overflow.
void SetupReadChannel( DMA_CH_t * dmaChannel )
{
	DMA_SetupBlock( 
                    dmaChannel,
			        (void const *) &(SWITCHPORT.IN), 
                    DMA_CH_SRCRELOAD_NONE_gc, 
                    DMA_CH_SRCDIR_FIXED_gc,
			        samples, 
                    DMA_CH_DESTRELOAD_BLOCK_gc, 
                    DMA_CH_DESTDIR_INC_gc,
			        SAMPLE_COUNT, 
                    DMA_CH_BURSTLEN_1BYTE_gc, 
                    0,
                    false
                );
	
    DMA_EnableSingleShot( dmaChannel );
	DMA_SetTriggerSource( dmaChannel, DMA_CH_TRIGSRC_TCC0_OVF_gc ); // TCC0 Overflow as trigger source.
}


// Setup parameters for the channel that writes LED values. The channel is
// configured to reload orignal source address when the data block is finished.
// It is also configured to repeat the block copy operation indefinitely, by setting
// the Repeat Counter to 0 initially.
// Single-shot is enabled so that only one byte gets copyed for every timer overflow.
void SetupWriteChannel( DMA_CH_t * dmaChannel  )
{
	DMA_SetupBlock( 
                    dmaChannel,
			        samples, 
                    DMA_CH_SRCRELOAD_BLOCK_gc, 
                    DMA_CH_SRCDIR_INC_gc,
                    (void *) &(LEDPORT.OUT), 
                    DMA_CH_DESTRELOAD_NONE_gc, 
                    DMA_CH_DESTDIR_FIXED_gc,
                    SAMPLE_COUNT, 
                    DMA_CH_BURSTLEN_1BYTE_gc, 
                    0,
                    true
                );
	DMA_EnableSingleShot( dmaChannel );
	DMA_SetTriggerSource( dmaChannel, DMA_CH_TRIGSRC_TCC0_OVF_gc ); // TCC0 Overflow as trigger source.
}


// Setup a timer to give overflow interrupts at the desired sample rate.
// The default setup is to have no prescaling and full period time, which
// means a sample rate of 2MHz / 2^16 = approx. 30Hz.
void SetupSampleTimer( void )
{
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
	TCC0.PER = 0xffff;
	TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
}


// Provide a visible indication to the user that the application is ready
// to record, finished with recording etc.
void BlinkLEDs( void )
{
	LEDPORT.OUT = 0x00;
	delay_us( 1000000 ); // Wait one second
	LEDPORT.OUT = 0xff;		
}


int main( void )
{
    // The DMA channel to use for reading switch state.
    DMA_CH_t * ReadChannel = &DMA.CH0;
    // The DMA channel to use for writing data to LEDs.
    DMA_CH_t * WriteChannel = &DMA.CH1;

    // Setup IO ports.
	PORTCFG.MPCMASK = 0xff;
	SWITCHPORT.PIN0CTRL = PORT_OPC_PULLUP_gc;

    SWITCHPORT.DIR = 0x00;
    LEDPORT.DIR = 0xff;
	
    // Prepare DMA and sample timer tick.	
    DMA_Enable();
    SetupReadChannel(ReadChannel);
    SetupWriteChannel(WriteChannel);
    SetupSampleTimer();	
    // Enable interrupts;
    sei();

    while(1)
    {
		// Blink, wait for user to press a switch, blink again.
		BlinkLEDs();
		do {} while (SWITCHPORT.IN == 0xff);		
		BlinkLEDs();
		
		// Record switch state until buffer is full, then blink LEDs.
		DMA_EnableChannel( ReadChannel );
		DMA_ReturnStatus_blocking( ReadChannel );
		BlinkLEDs();
		
		// Wait for user to press a switch, then blink LEDs.
		do {} while (SWITCHPORT.IN == 0xff);
		BlinkLEDs();

		// Play back switch samples on LEDs over and over again, until
		// user presses a switch.
		DMA_EnableChannel( WriteChannel );
		do {} while (SWITCHPORT.IN == 0xff);
		DMA_DisableChannel( WriteChannel );
		
		// Setup write channel again, in case (very likely) the playback
		// was interrupted by the user in the middle of the buffer, which would
		// leave the channel in an unknown state.
		SetupWriteChannel(WriteChannel);
	}
}
