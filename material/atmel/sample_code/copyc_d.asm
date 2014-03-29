; read port C, write contents to port D
; infinite loop

#include <m644def.inc>
      
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
	out DDRC, r16	; set port C to input (all 0's)
	ser R16			; load register 16 to all 1's
	out PORTC, r16	; set pullups (1's) on port C inputs
	out DDRD, R16	; set port D to output

LOOP:				; infinite loop
	in r16, PINC	; read port C
	out PORTD, r16	; output to port D
	rjmp LOOP		; repeat loop
