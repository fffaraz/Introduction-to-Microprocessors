#ifndef UART0_H_
#define UART0_H_

#include <stdarg.h>

#ifndef bool
#define bool unsigned char
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true !false
#endif

void uart0_open(void);
void uart0_printf(auto const MEM_MODEL rom char *format, ...);
void uart0_send(unsigned char data);
void uart0_send_string(auto const MEM_MODEL rom char * data);
void uart0_send_buffer(char * data, unsigned long len);
void uart0_send_hex(unsigned long dataword);
void uart0_send_uint(unsigned long dataword);
void uart0_send_int(long dataword);
void uart0_send_crlf(void);

bool uart0_rx_data_ready();

unsigned char uart0_get_rx_data();

#endif /*UART0_H_*/
