#include <p18f452.h>
#include <usart.h>
#include "uart0.h"

void uart0_open()
{
	OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 64);
}

void uart0_printf(auto const MEM_MODEL rom char *format, ...)
{
	unsigned long index = 0;
	va_list argptr;
	
	va_start(argptr, format); 
	
	while(format[index] != '\0')
	{
		if(format[index] == '%')
		{
			index++;
			
			if(format[index] == '\0')
				return;
				
			switch(format[index])
			{
				case 'd':
					uart0_send_int(va_arg(argptr, long));
					break;
				case 'i':
					uart0_send_int(va_arg(argptr, long));
					break;
				case 'u':
					uart0_send_uint(va_arg(argptr, unsigned long));
					break;
				case 'c':
					uart0_send(va_arg(argptr, long));
					break;
				case 's':
					uart0_send_string(va_arg(argptr, char *));
					break;
				case 'x':
					uart0_send_hex(va_arg(argptr, unsigned long));
					break;
				default:
					uart0_send(format[index]);
			}			
		}
		else
			uart0_send(format[index]);
	
		index++;
	}

    va_end(argptr);  
}

void uart0_send(unsigned char data)
{
	while(BusyUSART());
	
	WriteUSART(data);
}

void uart0_send_string(auto const MEM_MODEL rom char * data)
{
	unsigned long loc = 0;

	while(data[loc] != '\0')
	{
		uart0_send(data[loc]);
		loc++;
	}
}

void uart0_send_buffer(char * data, unsigned long len)
{
	unsigned long loc;

	for(loc = 0; loc < len; loc++)
		uart0_send(data[loc]);
}	

void uart0_send_hex(unsigned long dataword)
{
	char buffer[11];
	char temp;
	unsigned long count;
	unsigned long len;
	unsigned long halflen;
	
	buffer[0] = '0';
	buffer[1] = 'x';
	
	if(dataword == 0)
	{
		buffer[2] = '0';
		buffer[3] = '\0';
	}
	else
	{
		for(count = 2; count < 11; count++)
		{
			buffer[count] = (dataword % 16) + 48;
			
			if(buffer[count] > '9')
				buffer[count] += 7; //10 would be ':', so adding 7 gives 'A'
			
			dataword = dataword >> 4;
			
			if (dataword == 0)
				break;
		}
		
		buffer[count + 1] = '\0';
		
		len = count;
		halflen = ((len + 1) / 2) + 1;
		
		for(count = 2; count < halflen; count++)
		{
			temp = buffer[count];
			buffer[count] = buffer[len];
			buffer[len] = temp;
			len -= 1;
		}
	}
	
	uart0_send_string(buffer);
}

void uart0_send_uint(unsigned long dataword)
{
	char buffer[11];
	char temp;
	unsigned long size;
	unsigned long count;
	unsigned long len;
	unsigned long halflen;
	
	size = 1;
	
	if(dataword == 0)
	{
		buffer[0] = '0';
		buffer[1] = '\0';
	}
	else
	{
		for(count = 0; count < 10; count++)
		{
			buffer[count] = (dataword % 10) + 48;
			dataword /= 10;
			
			if (dataword == 0)
				break;
		}
		
		buffer[count + 1] = '\0';
		len = count;
		halflen = (len + 1) / 2;
		
		for(count = 0; count < halflen; count++)
		{
			temp = buffer[count];
			buffer[count] = buffer[len];
			buffer[len] = temp;
			len -= 1;
		}
	}
	
	uart0_send_string(buffer);
}

void uart0_send_int(long dataword)
{
	if(dataword < 0)
	{
		uart0_send('-');
		uart0_send_uint(-dataword);
	}
	else
		uart0_send_uint(dataword);
}

void uart0_send_crlf()
{
	uart0_send(0x0D);
	uart0_send(0x0A);
}

bool uart0_rx_data_ready()
{
	return DataRdyUSART();
}

unsigned char uart0_get_rx_data()
{
	return ReadUSART();
}
