/*****************************************************************************
*
* File: maintutorial1local.c
* 
* Copyright S. Brennen Ball, 2007
* 
* The author provides no guarantees, warantees, or promises, implied or
*	otherwise.  By using this software you agree to indemnify the author
* 	of any damages incurred by using it.
* 
*****************************************************************************/

//This code depends on a clock frequency of 40 MHz (I use a 10 MHz crystal and
//  the PLL.  If you are using a slower clock, then you must change the SPI and
//  UART rates, as well as the delay lengths in delays.c

#include <p18f452.h>
#include <stdio.h>
#include <usart.h>
#include <spi.h>
#include "delays.h"
#include "nrf24l01.h"

void Initialize(void);
void InitializeIO(void);

void ToggleLED(void); //toggle the current state of the on-board LED
void CheckErrorsUSART(void); //fix any framing or overrun errors in the USART

//main routine
void main(void)
{
	unsigned char data; //register to hold letter sent and received
	unsigned int count; //counter for for loop
	
	Initialize(); //initialize IO, UART, SPI, set up nRF24L01 as TX

	while(1)
	{
		CheckErrorsUSART();
		  
		//check UART status register to see if data has been received.  if so, process
		while(DataRdyUSART())
		{
			data = ReadUSART(); //get data from UART
			
			nrf24l01_write_tx_payload(&data, 1, true); //transmit received char over RF

			//wait until the packet has been sent or the maximum number of retries has been reached
			while(!(nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active()));

			nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01
			nrf24l01_set_as_rx(true); //change the device to an RX to get the character back from the other 24L01

			//wait a while to see if we get the data back (change the loop maximum and the lower if
			//  argument (should be loop maximum - 1) to lengthen or shorten this time frame
			for(count = 0; count < 20000; count++)
			{
				//check to see if the data has been received.  if so, get the data and exit the loop.
				//  if the loop is at its last count, assume the packet has been lost and set the data
				//  to go to the UART to "?".  If neither of these is true, keep looping.
				if((nrf24l01_irq_pin_active() && nrf24l01_irq_rx_dr_active()))
				{
					nrf24l01_read_rx_payload(&data, 1); //get the payload into data
					break;
				}
				
				//if loop is on its last iteration, assume packet has been lost.
				if(count == 19999)
					data = '?';
			}
			
			nrf24l01_irq_clear_all(); //clear interrupts again
			printf("%c", data); //print the received data (or ? if none) to the screen
		
			DelayUS(130); //wait for receiver to come from standby to RX
			nrf24l01_set_as_tx(); //resume normal operation as a TX
											
			ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed
		}
	}
}

//initialize routine
void Initialize(void)
{
	InitializeIO(); //set up IO (directions and functions)
	OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 64); //open UART
	OpenSPI(SPI_FOSC_16, MODE_00, SMPMID); //open SPI1
	nrf24l01_initialize_debug(false, 1, false); //initialize the 24L01 to the debug configuration as TX, 1 data byte, and auto-ack disabled
}

//initialize IO pins
void InitializeIO(void)
{
	ADCON1 = 0x7; //disable AD converter functionality on PORTA
	TRISAbits.TRISA0 = 0; //make PORTA.0 an output to control LED
	PORTAbits.RA0 = 1; //turn on LED
	
	TRISBbits.TRISB0 = 1; //make sure that PORTB.0 is input since it is IRQ pin
	
	TRISC = 0x91; //make CSN, CE, SCK, MOSI (SDO), and TX outputs
	PORTC = 0x04; //set CSN bit
}	

//toggles on-board LED
void ToggleLED(void)
{
	PORTAbits.RA0 = ~PORTAbits.RA0; //invert the bit that controls the LED
}

//fixes any framing or overrun errors that would otherwise halt the USART
void CheckErrorsUSART(void)
{
	unsigned char data; //dummy data variable
	
	//check for overrun error
	if(RCSTAbits.OERR)
	{
		RCSTAbits.CREN = 0; //clearing CREN clears OERR
		RCSTAbits.CREN = 1; //re-establish communications
		data = ReadUSART(); //make a dummy read in case there is data in the USART
	}
	 
	//check for framing error  
	if(RCSTAbits.FERR)
		data = ReadUSART(); //reading the USART clears this error
}		
	