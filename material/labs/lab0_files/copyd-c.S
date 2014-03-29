; read port D, write contents to port C
; infinite loop
;
; commands to assemble, and flash this program
; the C compiler gcc will also accept assembler code, wiyh a .S extension
; avr-gcc -Os -mmcu=atmega1284p -Wa,-gstabs copyd-c.S -o copyd-c.elf
; avr-objcopy -O ihex copyd-c.elf copyd-c.hex
; avrdude -p m1284p -c stk500 -P /dev/ttyS0 -e -U flash:w:copyd-c.hex
;
	.arch atmega1284p
;
; io1284p data registers C and D
;
        .set PINC, 0x06
        .set DDRC, 0x07
        .set PORTC, 0x08
        .set PIND, 0x09
        .set DDRD, 0x0A
        .set PORTD, 0x0B

.global main

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
      jmp    USART1_RX
      jmp    USART1_UDRE
      jmp    USART1_TX

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
USART1_RX:
USART1_UDRE:
USART1_TX:
       reti
main:
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
