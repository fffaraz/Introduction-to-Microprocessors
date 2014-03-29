; read port D, write contents to port C
; infinite loop

#include <m644pdef.inc>
      
VECTS:
      jmp    RESET
      jmp    EXT_INT0
      jmp    EXT_INT1
      jmp    EXT_INT2
      jmp    PC_INT0
      jmp    PC_INT1
      jmp    PC_INT2
      jmp    PC_INT3
      jmp    WDT
      jmp    TIMER2_COMPA
      jmp    TIMER2_COMPB
      jmp    TIMER2_OVF
      jmp    TIMER1_CAPT
      jmp    TIMER1_COMPA
      jmp    TIMER1_COMPB
      jmp    TIMER1_OVF
      jmp    TIMER0_COMPA
      jmp    TIMER0_COMPB
      jmp    TIMER0_OVF
      jmp    SPI_STC
      jmp    USART0_RX
      jmp    USART0_UDRE
      jmp    USART0_TX
      jmp    ANALOG_COMP
      jmp    ADC_INT
      jmp    EE_READY
      jmp    TWI
      jmp    SPM_READY

EXT_INT0:
EXT_INT1:
EXT_INT2:
PC_INT0:
PC_INT1:
PC_INT2:
PC_INT3:
WDT:
TIMER2_COMPA:
TIMER2_COMPB:
TIMER2_OVF:
TIMER1_CAPT:
TIMER1_COMPA:
TIMER1_COMPB:
TIMER1_OVF:
TIMER0_COMPA:
TIMER0_COMPB:
TIMER0_OVF:
SPI_STC:
USART0_RX:
USART0_UDRE:
USART0_TX:
ANALOG_COMP:
ADC_INT:
EE_READY:
TWI:
SPM_READY:
       reti

RESET:
	ldi R16, 0X00	; load register 16 with zero
	out DDRD, r16	; set port D to input (all 0's)
	ser R16			; load register 16 to all 1's
	out PORTD, r16	; set pullups (1's) on port D inputs
	out DDRC, R16	; set port C to output

LOOP:				; infinite loop
	in r16, PIND	; read port D
	out PORTC, r16	; output to port C
	rjmp LOOP		; repeat loop
