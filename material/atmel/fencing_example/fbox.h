//  fbox.h for ATmega644
//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

// interrupt is every 0.1 ms.
#define FOIL_DEBOUNCE 140		// 14 ms (was 1-5 ms, now 13-15ms)
#define EPEE_DEBOUNCE 20		// 2 ms (2 - 10 ms)
#define FOIL_THROTTLE 3000		// 300 ms (was 700 - 800 ms, now 275-325 ms)
#define EPEE_THROTTLE 450		// 45 ms (40-50 ms)
#define MICROBREAK_MAX 2		// 1 + number of breaks before hit resets (foil)

#define SABRE_THROTTLE 1200		// 120ms (was 300 - 400 ms, now 110 - 130 ms)
#define PARRY_BLOCK_END 150		//  15 ms
#define PARRY_BLOCK_START 40	//   4 ms

#define BUZZMAX 20000			// number of ticks before buzzer stops

#define INPUT_DDR			DDRA	// INPUT should be a full (6 bit minimum) dedicated port
#define INPUT_PORT			PORTA	// INPUT is sometimes assigned directly
#define INPUT_PIN			PINA 

// both INPUT_DDR and INPUT_PORT are written without being read
// no other function should be an output on this port!!!

#define GUARD_RED			PA0		// ADC0 ?
#define GUARD_GREEN			PA1		// ADC1 ?
#define BLADE_RED_IN		PA2		// ADC2
#define BLADE_GREEN_IN		PA3		// ADC3
#define JACKET_RED			PA4		// ADC4
#define JACKET_GREEN		PA5		// ADC5
#define BLADE2_RED			PA4		// ADC4
#define BLADE2_GREEN		PA5		// ADC5

#define BLADE_PWR_DDR		DDRB	// should all be on same port
#define BLADE_PWR_PORT		PORTB	// 
#define GUARD_PWR_DDR		DDRB	//
#define GUARD_PWR_PORT		PORTB	// 
 
#define GUARD_RED_PWR		PB0		// 
#define GUARD_GREEN_PWR		PB1		//
#define BLADE_RED_PWR		PB2		//
#define BLADE_GREEN_PWR		PB3		//

#define LIGHT_DDR			DDRC
#define LIGHT_PORT			PORTC

#define YELLOW_RED			PC0
#define WHITE_RED			PC1
#define RED_LIGHT			PC2
#define YELLOW_GREEN		PC3
#define WHITE_GREEN			PC4
#define GREEN_LIGHT			PC5

#define BUZZER				PD7		// on timer 2

#define MODE_IN_DDR			DDRA
#define MODE_IN_PORT		PORTA
#define MODE_IN_PIN			PINA
#define MODE_FES_PIN		PA6
#define MODE_STATE_PIN		PA7

#define MODE_OUT_DDR		DDRC
#define MODE_OUT_PORT		PORTC
#define MODE_OUT_SABRE		PC6
#define MODE_OUT_FOIL		PC7

#define TIMER_OUT_DDR		DDRD
#define TCCRA 				TCCR2A
#define TCCRB				TCCR2B
#define OCRA				OCR2A

#define MAXMODES 3
#define TRUE 1
#define FALSE 0
