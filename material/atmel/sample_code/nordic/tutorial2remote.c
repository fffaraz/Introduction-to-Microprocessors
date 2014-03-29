/*****************************************************************************
*
* File: maintutorial2remote.c
* 
* Copyright S. Brennen Ball, 2007
* 
* The author provides no guarantees, warantees, or promises, implied or
*	otherwise.  By using this software you agree to indemnify the author
* 	of any damages incurred by using it.
* 
*****************************************************************************/

#include "spi1.h"
#include "delays.h"
#include "nrf24l01.h"


void Initialize();
//void InitializeIO();

//void ToggleLED(); //toggle the current state of the on-board LED

//main routine
int	main() 
{	
	unsigned char data; //register to hold letter received and sent
	
	Initialize(); //initialize PLL, IO, SPI, set up nRF24L01 as RX

	//main program loop
	while(1)
	{
		//wait until a packet has been received
		while(!(nrf24l01_irq_pin_active() && nrf24l01_irq_rx_dr_active()));
		
		nrf24l01_read_rx_payload(&data, 1); //read the packet into data
		nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01
		
		DelayUS(130); //wait for the other 24L01 to come from standby to RX
		
		nrf24l01_set_as_tx(); //change the device to a TX to send back from the other 24L01
		nrf24l01_write_tx_payload(&data, 1, true); //transmit received char over RF
		
		//wait until the packet has been sent or the maximum number of retries has been reached
		while(!(nrf24l01_irq_pin_active() && (nrf24l01_irq_tx_ds_active() || nrf24l01_irq_max_rt_active())));

		nrf24l01_irq_clear_all(); //clear interrupts again
		nrf24l01_set_as_rx(true); //resume normal operation as an RX

//		ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed
	}
}

//initialize routine
void Initialize()
{
//	InitializeIO(); //set up IO (directions and functions)
	spi1_open(); //open SPI1
	nrf24l01_initialize_debug(true, 1, true); //initialize the 24L01 to the debug configuration as RX, 1 data byte, and auto-ack enabled
}


//toggles on-board LED
/*
void ToggleLED()
{
	if((FIO1PIN3 & 0x01) == 0x00)
		FIO1PIN3 = 0x01;
	else
		FIO1PIN3 = 0x00;
}
*/
