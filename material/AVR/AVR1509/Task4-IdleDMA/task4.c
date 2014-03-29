/*************************************************************************
 *
 *
 *              Task4: Utilizing the Event System and DMA in Idle Mode
 *
 *
 *************************************************************************/

// Include header files for GCC/IAR
#include "../avr_compiler.h"
#include "../adc_driver.h"
#include "../dma_driver.h"


#define SWITCHPORT PORTF
#define SWITCHMASK_ACTIVE 0x01
#define SWITCHMASK_IDLE 0x02

#define LEDPORT PORTE
#define LED_ACTIVE 0x01;
#define LED_IDLE 0x02;


// The SLEEP-instruction
#define sleep() __asm__ __volatile__ ("sleep")

void facilitatePowersaving(void);

int main(void)
{
	facilitatePowersaving();

	// Configure switches
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	SWITCHPORT.PIN0CTRL = (SWITCHPORT.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc; //Enable pull-up to get a defined level on the switches

	SWITCHPORT.DIRCLR = 0xff; // Set port as input

	// Configure LEDs
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1

	LEDPORT.DIRSET = 0xff; 	// Set port as output
	LEDPORT.OUT = 0x00;  // Set initial value


	// Set up ADCB0 on PB0 to read temp sensor. More of this can be achieved by using driver from appnote AVR1300
	PORTQ.PIN2CTRL = (PORTQ.PIN2CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLDOWN_gc;	// This pin must be grounded to "enable" NTC-resistor
	PORTB.DIRCLR = PIN0;
	PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_OPC_gm);

	ADC_CalibrationValues_Load(&ADCB);  // Load factory calibration data for ADC
	ADCB.CH0.CTRL = (ADCB.CH0.CTRL & ~ADC_CH_INPUTMODE_gm) | ADC_CH_INPUTMODE_SINGLEENDED_gc; // Single ended input
	ADCB.CH0.MUXCTRL = (ADCB.CH0.MUXCTRL & ~ADC_CH_MUXPOS_gm) | ADC_CH_MUXPOS_PIN0_gc; // Pin 0 is input	
	ADCB.REFCTRL = (ADCB.REFCTRL & ~ADC_REFSEL_gm) | ADC_REFSEL_VCC_gc;	// Internal AVCC/1.6 as reference

	ADCB.CTRLB |= ADC_FREERUN_bm; // Free running mode
	ADCB.PRESCALER = (ADCB.PRESCALER & ~ADC_PRESCALER_gm) | ADC_PRESCALER_DIV512_gc; // Divide clock by 1024.
	ADCB.CTRLB = (ADCB.CTRLB & ~ADC_RESOLUTION_gm) | ADC_RESOLUTION_8BIT_gc; // Set 8 bit resolution
	ADCB.CTRLA |= ADC_ENABLE_bm; // Enable ADC
	

	// Set up DMA CH0 to transfer from ADC to LEDS. We only read low byte.
	DMA_Enable();
	DMA_SetupBlock( 
                    &DMA.CH0,
			        (void const *) &(ADCB.CH0RES), 
                    DMA_CH_SRCRELOAD_NONE_gc, 
                    DMA_CH_SRCDIR_FIXED_gc,
			        (void const *) &(LEDPORT.OUT), 
                    DMA_CH_DESTRELOAD_NONE_gc, 
                    DMA_CH_DESTDIR_FIXED_gc,
			        1, 
                    DMA_CH_BURSTLEN_1BYTE_gc, 
                    0,
                    true
                );
    DMA_EnableSingleShot( &DMA.CH0 );
	DMA_SetTriggerSource( &DMA.CH0, DMA_CH_TRIGSRC_ADCB_CH0_gc ); // ADC Channel 0 is trigger source.
	


	// Set up interrupt on button 0, or else we can't come back from IDLE
	SWITCHPORT.INTCTRL = (SWITCHPORT.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;
	SWITCHPORT.INT0MASK = SWITCHMASK_ACTIVE;
	SWITCHPORT.PIN0CTRL = (SWITCHPORT.PIN0CTRL & ~PORT_ISC_gm) | PORT_ISC_FALLING_gc;


	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	// Main loop.
	while (1) {

		if ((SWITCHPORT.IN & SWITCHMASK_ACTIVE) == 0x00)
		{
		// Button 0 pressed. Enter ACTIVE mode again.
			DMA_DisableChannel( &DMA.CH0 );

			SLEEP.CTRL &= ~SLEEP_SEN_bm;	// Disable sleep. sleep() is now ineffective.

		} else if ((SWITCHPORT.IN & SWITCHMASK_IDLE) == 0x00)	
	    {	
		// Button 1 pressed. Enter Idle mode
			DMA_EnableChannel( &DMA.CH0 );			

			// Set and enable sleep.
			SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_IDLE_gc;
			SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep.

		} else if (SLEEP.CTRL & SLEEP_SEN_bm) {
			 	// We are in wanting-to-sleep mode, but were awake.
				sleep();
	
		} else { 
			// Do active sampling and transfer of ADC data.
			LEDPORT.OUT = ADCB.CH0RES & 0xFF;
		}

	}
	
}

ISR(PORTF_INT0_vect) {
	// Could have put actions for button 0 press here.
}

void facilitatePowersaving(void) {
	// Pullup on all ports, to ensure a defined state.
	PORTCFG.MPCMASK = 0xFF;
	PORTA.PIN0CTRL = (PORTA.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTD.PIN0CTRL = (PORTD.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTE.PIN0CTRL = (PORTE.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTF.PIN0CTRL = (PORTF.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTH.PIN0CTRL = (PORTH.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTJ.PIN0CTRL = (PORTJ.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTK.PIN0CTRL = (PORTK.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTQ.PIN0CTRL = (PORTQ.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTR.PIN0CTRL = (PORTR.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	// Because of the audio amplifier, we have to define pulldown on PQ3
	// NTC is already disabled via pullup
	PORTQ.PIN3CTRL = (PORTQ.PIN3CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLDOWN_gc;
	
	// The potentiometer has an analog voltage, thus both pullup and pulldown in tristate will source current. ~35µA.
	PORTB.DIRSET = (1<<PIN1);
	
	// Turn off unneeded peripherals. We are only using DMA/ADCB.
	PR.PRGEN = PR_AES_bm | PR_EBI_bm | PR_RTC_bm | PR_EVSYS_bm;
	PR.PRPA = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
	PR.PRPB = PR_DAC_bm | PR_AC_bm;
	PR.PRPC = PR.PRPD = PR.PRPE = PR.PRPF = PR_TWI_bm | PR_USART1_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
}
