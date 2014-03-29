/*
    5-30-04
    Copyright Spark Fun Electronics© 2004
*/

#define RX_PORT 	PORTB
#define RX_PORT_PIN PINB
#define RX_PORT_DD	DDRB

/*
#define RX_CE	1 //Output
#define RX_CSN  2 //Output
#define RX_SCK  5 //Output
#define RX_MOSI 3 //Output
#define RX_MISO	4 //Input
//#define RX_IRQ	3 //Input
*/

#define RX_CE	3	//Output
#define RX_CSN  4	//Output
#define RX_SCK  7	//Output
#define RX_MOSI 5	//Output
#define RX_MISO	6	//Input

#define RF_DELAY	5

uint8_t data_array[4];

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

uint8_t rx_address[5] = {0XE7, 0XE7, 0XE7, 0XE7, 0XE7};	// default
uint8_t rx_channel = 2;	// default

// uint8_t rx_address[5] = {0XC6, 0XC6, 0XE7, 0XC6, 0XE7}; // reprogrammed address
// uint8_t rx_channel = 83;	// highest legal channel 

//2.4G Configuration - Receiver
void configure_receiver(void);
//Sends one byte to nRF
uint8_t rx_send_byte(uint8_t cmd);
//Sends command to nRF
uint8_t rx_send_command(uint8_t cmd, uint8_t data);
//Set RX address - stored in eeprom
void rx_send_rx_address(void);
//Sends the 4 bytes of payload
void rx_send_payload(uint8_t cmd);
//Basic SPI to nRF
uint8_t rx_spi_byte(uint8_t outgoing);


void init_nRF_pins(void)
{
	volatile char IOReg;
	//1 = Output, 0 = Input
	//	RX_PORT_DD = 0xFF & ~(1<<RX_MISO);// | 1<<RX_IRQ);
	RX_PORT_DD |= (1<<RX_CE) | (1<<RX_MOSI) | (1<<RX_CSN) | (1<<RX_SCK);
	//	Enable pull-up resistors
	RX_PORT = 0b11111110;

	// enable SPI in Master Mode with SCK = CK/4
	// SPR1, SPR0 = 00 - divide by 4. 01 - by 16, 10 - by 64, 11 by 128
	SPCR    = (1<<SPE)|(1<<MSTR);			// CK/4
//	SPCR    = (1<<SPE)|(1<<MSTR)|(1<<SPR0);	// CK/16
//	SPCR    = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// CK/64
	IOReg   = SPSR;      // clear SPIF bit in SPSR
	IOReg   = SPDR;

	cbi(RX_PORT, RX_CE); //Stand by mode
}

//RX Functions
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Reads the current RX buffer into the data array
//Forces an RX buffer flush

void receive_data(void)
{
	cbi(RX_PORT, RX_CSN); //Stand by mode
    rx_spi_byte(0x61); //Read RX Payload
	data_array[0] = rx_spi_byte(0xFF);
	data_array[1] = rx_spi_byte(0xFF);
	data_array[2] = rx_spi_byte(0xFF);
	data_array[3] = rx_spi_byte(0xFF);
	sbi(RX_PORT, RX_CSN);
	
	rx_send_byte(0xE2); //Flush RX FIFO
	
	rx_send_command(0x27, 0x40); //Clear RF FIFO interrupt

    sbi(RX_PORT, RX_CE); //Go back to receiving!
}

//2.4G Configuration - Receiver
//This setups up a RF-24G for receiving at 1mbps
void configure_receiver(void)
{
	uint8_t channel;

    cbi(RX_PORT, RX_CE); //Go into standby mode

	rx_send_command(0x20, 0x39); //Enable RX IRQ, CRC Enabled, be a receiver

	rx_send_command(0x21, 0x00); //Disable auto-acknowledge

	rx_send_command(0x23, 0x03); //Set address width to 5bytes (default, not really needed)

	rx_send_command(0x26, 0x07); //Air data rate 1Mbit, 0dBm, Setup LNA

	rx_send_command(0x31, 0x04); //4 byte receive payload

	channel = rx_channel;
	if (channel < 84)	// channels 84-127 are illegal
	{
	rx_send_command(0x25, channel);
	}
	else
	{
	rx_send_command(0x25, 0x02); //RF Channel 2 (default, not really needed)
	}

	rx_send_rx_address(); //Set RX address

	rx_send_command(0x20, 0x3B); //RX interrupt, power up, be a receiver

    sbi(RX_PORT, RX_CE); //Start receiving!
}    

//Sends the 4 bytes of payload
void rx_send_payload(uint8_t cmd)
{
	uint8_t i;

	cbi(RX_PORT, RX_CSN); //Select chip
	rx_spi_byte(cmd);
	
	for(i = 0 ; i < 4 ; i++)
		rx_spi_byte(data_array[i]);

	sbi(RX_PORT, RX_CSN); //Deselect chip
}

void rx_send_rx_address() //Set RX address - stored in eeprom
{
	uint8_t i,address;

	cbi(RX_PORT, RX_CSN);	// Select chip
	rx_spi_byte(0x2A);		// pipe 0 read address
	
	for(i = 0 ; i < 5 ; i++)
	{
		address = rx_address[i];
		rx_spi_byte(address);
	}

	sbi(RX_PORT, RX_CSN); //Deselect chip
}

//Sends command to nRF
uint8_t rx_send_command(uint8_t cmd, uint8_t data)
{
	uint8_t status;

	cbi(RX_PORT, RX_CE); //Stand by mode

	cbi(RX_PORT, RX_CSN); //Select chip
	rx_spi_byte(cmd);
	status = rx_spi_byte(data);
	sbi(RX_PORT, RX_CSN); //Deselect chip
	
	return(status);
}

//Sends one byte to nRF
uint8_t rx_send_byte(uint8_t cmd)
{
	uint8_t status;

	cbi(RX_PORT, RX_CSN); //Select chip
	status = rx_spi_byte(cmd);
	sbi(RX_PORT, RX_CSN); //Deselect chip
	
	return(status);
}

//Basic SPI to nRF
uint8_t rx_spi_byte(uint8_t byte)
// unsigned char spi1_send_read_byte(unsigned char byte)
{
	SPDR  = byte;          // send Character
        while (!(SPSR & (1<<SPIF)));    // wait until Char is sent
	return (uint8_t) SPDR;
}

/*
uint8_t rx_spi_byte(uint8_t outgoing)
{
    uint8_t i, incoming;
	incoming = 0;

    //Send outgoing byte
    for(i = 0 ; i < 8 ; i++)
    {

		if(outgoing & 0b10000000)
			sbi(RX_PORT, RX_MOSI);
		else
			cbi(RX_PORT, RX_MOSI);

		sbi(RX_PORT, RX_SCK); //RX_SCK = 1;
		_delay_us(RF_DELAY);

		incoming <<= 1;
		if( RX_PORT_PIN & (1<<RX_MISO) ) incoming |= 0x01;
		
		cbi(RX_PORT, RX_SCK); //RX_SCK = 0; 
		_delay_us(RF_DELAY);

        outgoing <<= 1;

    }

	return(incoming);
}

*/
