#ifndef USART0_H
#define USART0_H

#include <stdint.h>

void usart0_open( );

void usart0_put( uint8_t b );

uint8_t usart0_get( void );

uint8_t usart0_get_rx_data( void );

uint8_t usart0_rx_data_ready( );

int16_t usart0_get_delay( uint16_t max_delay );

#define BAUD_RATE(clockFreq, baud) ((uint16_t)(clockFreq/(16L*(baud))-1))

#endif
