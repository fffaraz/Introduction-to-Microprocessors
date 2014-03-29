// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief  XMega low power macros
 *
 *      This file defines the macros used by LOWPOWER_Init(), tailored to
 *      the individual XMEGA variants.
 *      Six macros are defined, of which the last five are device specific:
 *      <pre>
 *      DISABLE_JTAG() - disables JTAG interface
 *      DISABLE_GEN() - disables crypto, EBI, DMA and event system
 *      DISABLE_TC() - disables timer/counters
 *      DISABLE_COM() - disables TWI, SPI and UARTs
 *      DISABLE_ANLG() - disables ADC, DAC and comparators
 *      ENABLE_PULLUP() - enables pull-up on all available I/O pins
 *      </pre>
 *
 *      \note These macros are not created in lowpower.h since they are only
 *      meant for use in lowpower.c
 *
 * \par Application note:
 *      AVR1010: Minimizing the power consumption of XMEGA devices
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 5159 $
 * $Date: 2009-06-09 19:05:02 +0200 (ti, 09 jun 2009) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef LOWPOWER_MACROS_H
#define LOWPOWER_MACROS_H


/*============================ MACROS ========================================*/
//! Convenience macro to disable the JTAG interface.
#define DISABLE_JTAG( ) { \
	ENTER_CRITICAL_REGION(); \
	CCP = 0xD8; \
	MCU_MCUCR = MCU_JTAGD_bm; \
	LEAVE_CRITICAL_REGION(); \
}

//! Convenience macro for enabling pullups on a port.
#define __PORT_PULLUP(port, mask) { \
	PORTCFG.MPCMASK = mask ; \
	port.PIN0CTRL = PORT_OPC_PULLUP_gc; \
}


/* Create macros according to XMEGA device selection.
 *
 * Create macros that only set the PRR-bits for peripherals and enable pullup
 * on ports/pins that actually exist on the device, according to datasheets.
 * (This is a very cautious approach.)
 */

// A1
#if defined(__ATxmega64A1__) || defined(__ATxmega128A1__) || defined(__ATxmega192A1__) || defined(__ATxmega256A1__) || \
    defined(__AVR_ATxmega64A1__) || defined(__AVR_ATxmega128A1__) || defined(__AVR_ATxmega192A1__) || defined(__AVR_ATxmega256A1__)

#define DISABLE_GEN( ) { \
	PR.PRGEN |= PR_AES_bm | PR_DMA_bm | PR_EBI_bm | PR_EVSYS_bm | PR_RTC_bm; \
}

#define DISABLE_TC( ) { \
	PR.PRPC |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPD |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPE |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPF |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
}

#define DISABLE_COM( ) { \
	PR.PRPC |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPD |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPE |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPF |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
}

#define DISABLE_ANLG( ) { \
	PR.PRPA |= PR_AC_bm | PR_ADC_bm | PR_DAC_bm; \
	PR.PRPB |= PR_AC_bm | PR_ADC_bm | PR_DAC_bm; \
}

#define ENABLE_PULLUP( ) { \
	__PORT_PULLUP(PORTA, 0xFF); \
	__PORT_PULLUP(PORTB, 0xFF); \
	__PORT_PULLUP(PORTC, 0xFF); \
	__PORT_PULLUP(PORTD, 0xFF); \
	__PORT_PULLUP(PORTE, 0xFF); \
	__PORT_PULLUP(PORTF, 0xFF); \
	__PORT_PULLUP(PORTH, 0xFF); \
	__PORT_PULLUP(PORTJ, 0xFF); \
	__PORT_PULLUP(PORTK, 0xFF); \
	__PORT_PULLUP(PORTQ, 0x0F); \
	__PORT_PULLUP(PORTR, 0x03); \
}


// A3
#elif defined(__ATxmega64A3__) || defined(__ATxmega128A3__) || defined(__ATxmega192A3__) || defined(__ATxmega256A3__) || \
      defined(___AVR_ATxmega64A3__) || defined(__AVR_ATxmega128A3__) || defined(__AVR_ATxmega192A3__) || defined(__AVR_ATxmega256A3__)

#define DISABLE_GEN( ) { \
	PR.PRGEN |= PR_AES_bm | PR_DMA_bm | PR_EVSYS_bm | PR_RTC_bm; \
}

#define DISABLE_TC( ) { \
	PR.PRPC |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPD |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPE |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPF |= PR_HIRES_bm | PR_TC0_bm; \
}

#define DISABLE_COM( ) { \
	PR.PRPC |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPD |= PR_SPI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPE |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPF |= PR_USART0_bm; \
}

#define DISABLE_ANLG( ) { \
	PR.PRPA |= PR_AC_bm | PR_ADC_bm; \
	PR.PRPB |= PR_AC_bm | PR_ADC_bm | PR_DAC_bm; \
}

#define ENABLE_PULLUP( ) { \
	__PORT_PULLUP(PORTA, 0xFF); \
	__PORT_PULLUP(PORTB, 0xFF); \
	__PORT_PULLUP(PORTC, 0xFF); \
	__PORT_PULLUP(PORTD, 0xFF); \
	__PORT_PULLUP(PORTE, 0xFF); \
	__PORT_PULLUP(PORTF, 0xFF); \
	__PORT_PULLUP(PORTR, 0x03); \
}


// A3B
#elif defined(__ATxmega256A3B__) || defined(__AVR_ATxmega256A3B__)

#define DISABLE_GEN( ) { \
	PR.PRGEN |= PR_AES_bm | PR_DMA_bm | PR_EVSYS_bm | PR_RTC_bm; \
}

#define DISABLE_TC( ) { \
	PR.PRPC |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPD |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPE |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPF |= PR_HIRES_bm | PR_TC0_bm; \
}

#define DISABLE_COM( ) { \
	PR.PRPC |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPD |= PR_SPI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPE |= PR_TWI_bm | PR_USART0_bm; \
	PR.PRPF |= PR_USART0_bm; \
}

#define DISABLE_ANLG( ) { \
	PR.PRPA |= PR_AC_bm | PR_ADC_bm; \
	PR.PRPB |= PR_AC_bm | PR_ADC_bm | PR_DAC_bm; \
}

#define ENABLE_PULLUP( ) { \
	__PORT_PULLUP(PORTA, 0xFF); \
	__PORT_PULLUP(PORTB, 0xFF); \
	__PORT_PULLUP(PORTC, 0xFF); \
	__PORT_PULLUP(PORTD, 0xFF); \
	__PORT_PULLUP(PORTE, 0xFF); \
	__PORT_PULLUP(PORTF, 0xDF); \
	__PORT_PULLUP(PORTR, 0x03); \
}


// A4
#elif defined(__ATxmega16A4__) || defined(__ATxmega32A4__) || defined(__ATxmega64A4__) || defined(__ATxmega128A4__) || \
	  defined(__AVR_ATxmega16A4__) || defined(__AVR_ATxmega32A4__) || defined(__AVR_ATxmega64A4__) || defined(__AVR_ATxmega128A4__)

#define DISABLE_GEN( ) { \
	PR.PRGEN |= PR_AES_bm | PR_DMA_bm | PR_EVSYS_bm | PR_RTC_bm; \
}

#define DISABLE_TC( ) { \
	PR.PRPC |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPD |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPE |= PR_HIRES_bm | PR_TC0_bm; \
}

#define DISABLE_COM( ) { \
	PR.PRPC |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPD |= PR_SPI_bm | PR_USART0_bm | PR_USART1_bm; \
	PR.PRPE |= PR_TWI_bm | PR_USART0_bm; \
}

#define DISABLE_ANLG( ) { \
	PR.PRPA |= PR_AC_bm | PR_ADC_bm; \
	PR.PRPB |= PR_DAC_bm; \
}

#define ENABLE_PULLUP( ) { \
	__PORT_PULLUP(PORTA, 0xFF); \
	__PORT_PULLUP(PORTB, 0x0F); \
	__PORT_PULLUP(PORTC, 0xFF); \
	__PORT_PULLUP(PORTD, 0xFF); \
	__PORT_PULLUP(PORTE, 0x0F); \
	__PORT_PULLUP(PORTR, 0x03); \
}


// D3
#elif defined(__ATxmega64D3__) || defined(__ATxmega128D3__) || defined(__ATxmega192D3__) || defined(__ATxmega256D3__) || \
	  defined(__AVR_ATxmega64D3__) || defined(__AVR_ATxmega128D3__) || defined(__AVR_ATxmega192D3__) || defined(__AVR_ATxmega256D3__)
#warning Macros for D3 may be outdated!

#define DISABLE_GEN( ) { \
	PR.PRGEN |= PR_EVSYS_bm | PR_RTC_bm; \
}

#define DISABLE_TC( ) { \
	PR.PRPC |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPD |= PR_HIRES_bm | PR_TC0_bm; \
	PR.PRPE |= PR_HIRES_bm | PR_TC0_bm; \
	PR.PRPF |= PR_HIRES_bm | PR_TC0_bm; \
}

#define DISABLE_COM( ) { \
	PR.PRPC |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm; \
	PR.PRPD |= PR_SPI_bm | PR_USART0_bm; \
	PR.PRPE |= PR_USART0_bm; \
}

#define DISABLE_ANLG( ) { \
	PR.PRPA |= PR_AC_bm | PR_ADC_bm; \
}

#define ENABLE_PULLUP( ) { \
	__PORT_PULLUP(PORTA, 0xFF); \
	__PORT_PULLUP(PORTB, 0xFF); \
	__PORT_PULLUP(PORTC, 0xFF); \
	__PORT_PULLUP(PORTD, 0xFF); \
	__PORT_PULLUP(PORTE, 0xFF); \
	__PORT_PULLUP(PORTF, 0xFF); \
	__PORT_PULLUP(PORTR, 0x03); \
}


// D4
#elif defined(__ATxmega16D4__) || defined(__ATxmega32D4__) || defined(__ATxmega64D4__) || \
	  defined(__AVR_ATxmega16D4__) || defined(__AVR_ATxmega32D4__) || defined(__AVR_ATxmega64D4__)
#warning Macros for D4 may be outdated!

#define DISABLE_GEN( ) { \
	PR.PRGEN |= PR_EVSYS_bm | PR_RTC_bm; \
}

#define DISABLE_TC( ) { \
	PR.PRPC |= PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm; \
	PR.PRPD |= PR_TC0_bm; \
	PR.PRPE |= PR_TC0_bm; \
}

#define DISABLE_COM( ) { \
	PR.PRPC |= PR_SPI_bm | PR_TWI_bm | PR_USART0_bm; \
	PR.PRPD |= PR_SPI_bm | PR_USART0_bm; \
}

#define DISABLE_ANLG( ) { \
	PR.PRPA |= PR_AC_bm | PR_ADC_bm; \
}

#define ENABLE_PULLUP( ) { \
	__PORT_PULLUP(PORTA, 0xFF); \
	__PORT_PULLUP(PORTB, 0x0F); \
	__PORT_PULLUP(PORTC, 0xFF); \
	__PORT_PULLUP(PORTD, 0xFF); \
	__PORT_PULLUP(PORTE, 0x0F); \
	__PORT_PULLUP(PORTR, 0x03); \
}


// Unsupported device!
#else
#error Device not supported by lowpower module!
#endif // Device selection.


#endif // LOWPOWER_MACROS_H
