/*************************************************************************
 *
 *
 *              Task5: TWI Address match wakeup
 *
 *			- In this task you will send button presses via TWI
 *			
 *
 *************************************************************************/

// Include header files for GCC/IAR
#include "../avr_compiler.h"
#include "../twi_master_driver.h"
#include "../twi_slave_driver.h"

#define OWN_ADDRESS    0x66		// Everything up to 0x7F goes. 7-bit hw-support for address
#define OTHER_ADDRESS  0x55
#define NUM_BYTES        3
#define BAUDRATE	100000
#define TWI_BAUDSETTING TWI_BAUD(F_CPU, BAUDRATE)


TWI_Master_t twiMaster;    /* TWI master module. */
TWI_Slave_t twiSlave;      /* TWI slave module. */
uint8_t sendBuffer[NUM_BYTES] = {0};	   /* Buffer to hold data to send */


#define SWITCHPORT PORTF
#define LEDPORT PORTE

#define sleep() __asm__ __volatile__ ("sleep")

void TWID_SlaveProcessData(void);	// Function to process data received on the slave. Given to slave init function.
void facilitatePowersaving(void);
void initHardware(void);


int main(void)
{
	facilitatePowersaving();		// Pullups and PR Registers
	initHardware();					// Switches, LEDs, Sleep, Interrupt

	sei();							// Enable global interrupts

/* === Initialization of TWI Module === */

	// Enable internal pull-up on PD0, PD1 for correct TWI operation
	PORTCFG.MPCMASK = 0x03;
	PORTD.PIN0CTRL = (PORTD.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	/* Initialize TWI master. */
	TWI_MasterInit(&twiMaster,
	               &TWID,
	               TWI_MASTER_INTLVL_LO_gc,
	               TWI_BAUDSETTING);

	/* Initialize TWI slave. */
	TWI_SlaveInitializeDriver(&twiSlave, &TWID, TWID_SlaveProcessData);
	TWI_SlaveInitializeModule(&twiSlave,
	                          OWN_ADDRESS,
	                          TWI_SLAVE_INTLVL_LO_gc);

/* === End Initialization === */


	while (1) {
			
			// Try to sleep while we are waiting for button press
			while (SWITCHPORT.IN == 0x00)  sleep();
			_delay_ms(5);	// Debounce switch			

			sendBuffer[0] = SWITCHPORT.IN;
			TWI_MasterWrite(&twiMaster,		// Module
	                    OTHER_ADDRESS,		// Which slave
	                    &sendBuffer[0],		// What to send
	                    1);					// Send how much

			/* Wait until transaction is complete. Required TWI interrupts will be executed while waiting */
			while (twiMaster.status != TWIM_STATUS_READY);
			
			// Wait for user to release button.
			while (SWITCHPORT.IN != 0x00);
	}
	
}



ISR(PORTF_INT0_vect) {
	// Empty
}

// TWIC Master Interrupt vector.
ISR(TWID_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}

// TWIC Slave Interrupt vector.
ISR(TWID_TWIS_vect)
{
	SLEEP.CTRL &= ~SLEEP_SEN_bm;
	
	TWI_SlaveInterruptHandler(&twiSlave);

	/* When a transaction is complete (START+ADDRESS+READ/WRITE+NACK/STOP),
	 * the twiSlave.status member will be set to ready by the driver.
	 * ... Add code below to enable sleep when transaction is complete
	 */


}

void TWID_SlaveProcessData(void)
{
	LEDPORT.OUTTGL = twiSlave.receivedData[0];
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
	
	// The potentiometer has an analog voltage, thus both pullup and pulldown will source current when tristated.
	PORTB.DIRSET = (1<<PIN1);	// Set as output

	
	// Turn off unneeded peripherals. Using TWI on Port D.
	PR.PRGEN = PR_AES_bm | PR_EBI_bm | PR_RTC_bm | PR_DMA_bm | PR_EVSYS_bm;
	PR.PRPA = PR.PRPB = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
	PR.PRPD = PR_USART1_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
	PR.PRPC = PR.PRPE = PR.PRPF = PR_TWI_bm | PR_USART1_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
}

void initHardware(void) {
	// Configure switches
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	SWITCHPORT.PIN0CTRL = (SWITCHPORT.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	SWITCHPORT.DIRCLR = 0xff; // Set port as input
	PORTCFG.MPCMASK = 0xFF;
	SWITCHPORT.PIN0CTRL |= PORT_INVEN_bm;

	// Set up interrupt on buttons
	SWITCHPORT.INTCTRL = (SWITCHPORT.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;
	SWITCHPORT.INT0MASK = 0xFF;

	// Configure LEDs
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1

	LEDPORT.DIRSET = 0xff; 	// Set port as output
	LEDPORT.OUT = 0x02;  // Set initial value

	SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_PDOWN_gc;
	SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep.

	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
}

