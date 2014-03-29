/*****************************************************************************
*
* File: maintutorial2local.c
* 
* Copyright S. Brennen Ball, 2007
* 
* The author provides no guarantees, warantees, or promises, implied or
*	otherwise.  By using this software you agree to indemnify the author
* 	of any damages incurred by using it.
* 
*****************************************************************************/
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "spi1.h"
#include "nrf24l01.h"
#include "usart0.h"

void Initialize();
//void InitializeIO();

//void ToggleLED(); //toggle the current state of the on-board LED

// set up stdio streams
static int my_putchar(char c, FILE * stream)
{
	if (c == '\n')
		usart0_put('\r');
	usart0_put(c);
	return 0;
}

static int my_getchar(FILE * stream)
{
	return usart0_get();
}

static FILE my_stream =
FDEV_SETUP_STREAM (my_putchar, my_getchar, _FDEV_SETUP_RW);


//main routine
int	main() 
{	
	unsigned char data; //register to hold letter sent and received
	unsigned int count; //counter for for loop
	
	Initialize(); //initialize PLL, IO, UART, SPI, set up nRF24L01 as TX

	stdout = &my_stream;
	stdin = &my_stream;

	printf_P(PSTR(" type character to send \n"));

	//main program loop
	while(1)
	{	
		//check UART status register to see if data has been received.  if so, process
		while(usart0_rx_data_ready())
		{
//			data = usart0_get_rx_data(); //get data from UART
			data = getchar(); //get data from UART

			PORTC = data;

			nrf24l01_write_tx_payload(&data, 1, true); //transmit received char over RF

			//wait until the packet has been sent or the maximum number of retries has been active
			while(!(nrf24l01_irq_pin_active() && (nrf24l01_irq_tx_ds_active() || nrf24l01_irq_max_rt_active())));

			//check to see if the maximum number of retries has been hit.  if not, wait for the RX device
			// to send the char back.  if so, assume the packet is lost and send "*" back to UART
			if(!nrf24l01_irq_max_rt_active())
			{
				nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01
				nrf24l01_set_as_rx(true); //change the device to an RX to get the character back from the other 24L01

				//wait a while to see if we get the data back (change the loop maximum and the lower if
				//  argument (should be loop maximum - 1) to lengthen or shorten this time frame
				for(count = 0; count < 25000; count++)
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
					if(count == 24999)
						data = '?';
				}
				
				nrf24l01_irq_clear_all(); //clear interrupts again
				printf("%c", data); //print the received data (or ? if none) to the screen
			
//				DelayUS(130); //wait for receiver to come from standby to RX
				_delay_us(35);
				_delay_us(35);
				_delay_us(35);
				_delay_us(25);

				nrf24l01_set_as_tx(); //resume normal operation as a TX
			}
			else
			{
				nrf24l01_flush_tx(); //get the unsent character out of the TX FIFO
				nrf24l01_irq_clear_all(); //clear all interrupts
				printf("*"); //print "*" to the screen to show that the receiver did not receive the packet
			}
									
//			ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed
		}
	}
}

//initialize routine
void Initialize()
{
//	InitializeIO(); //set up IO (directions and functions)
	CLKPR = _BV(CLKPCE); // enable clock scalar update
    CLKPR = 0x00; // set to 8Mhz

	DDRC = 0XFF;	// set port C to output

	usart0_open(); //open UART0
	spi1_open(); //open SPI1
	nrf24l01_initialize_debug(false, 1, true); //initialize the 24L01 to the debug configuration as TX, 1 data byte, and auto-ack enabled
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
