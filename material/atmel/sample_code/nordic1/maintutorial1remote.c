/*****************************************************************************
*
* File: maintutorial1remote.c
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

//main routine
void main(void) 
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
       
      //wait until the packet has been sent 
      while(!(nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active())); 

      nrf24l01_irq_clear_all(); //clear interrupts again 
      nrf24l01_set_as_rx(true); //resume normal operation as an RX 

      ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed 
   } 
} 

//initialize routine
void Initialize(void) 
{ 
   InitializeIO(); //set up IO (directions and functions)
   OpenSPI(SPI_FOSC_16, MODE_00, SMPMID); //open SPI1 
   nrf24l01_initialize_debug(true, 1, false); //initialize the 24L01 to the debug configuration as TX, 1 data byte, and auto-ack disabled
} 

//initialize IO pins
void InitializeIO(void) 
{ 
   ADCON1 = 0x7; //disable AD converter functionality on PORTA
   TRISAbits.TRISA0 = 0; //make PORTA.0 an output to control LED
   PORTAbits.RA0 = 1; //turn on LED
    
   TRISBbits.TRISB0 = 1; //make sure that PORTB.0 is input since it is IRQ pin
    
   TRISC = 0xD1; //make CSN, CE, SCK, and MOSI (SDO) outputs
   PORTC = 0x04; //set CSN bit
}    

//toggles on-board LED
void ToggleLED(void) 
{ 
   PORTAbits.RA0 = ~PORTAbits.RA0; 
}    