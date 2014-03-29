// bluetooth control for motor
//
#include <avr/io.h>
#include<avr/sleep.h>
#include<avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define TRUE 1;
#define FALSE 0;

void init(void);
void timer2_pwm_init(void);
void usart0_init (uint16_t);
void usart0_put( char );
char usart0_get( void );

void handle_usart_0 (char key);

