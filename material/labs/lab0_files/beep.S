; output a time varying 0-1 sequence
; on pin 5 of port B for as long as
; a button connected to pin B 4 is pressed
;
; commands to assemble, and flash this program
; avr-gcc -Os -mmcu=atmega1284p -Wa,-gstabs beep.S -o beep.elf
; avr-objcopy -O ihex beep.elf beep.hex
; avrdude -p m1284p -c stk500 -P /dev/ttyS0 -e -U flash:w:beep.hex
;
	.arch atmega1284p
;
; io1284p data register B
;
	.set PINB, 0x03
	.set DDRB, 0x04
	.set PORTB, 0x05

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
       ldi R16, 0b00100000  ; load register 16 to set portb registers
       out DDRB, r16        ; set portb 5 to output, others to input
       ser R16              ; load register 16 to all 1's
       out PORTB, r16       ; set pullups (1's) on inputs

LOOP:
      sbic PINB, 4          ; skip next line if button pressed
      rjmp LOOP             ; repeat test

      cbi PORTB, 5          ; set speaker input to 0
      ldi R16, 128          ;

SPIN1:                      ; wait a few cycles
      subi R16, 1
      brne SPIN1

      sbi PORTB, 5          ; set speaker input to 1
      ldi R16, 128

SPIN2:
      subi R16, 1
      brne SPIN2

      rjmp LOOP             ; speaker buzzed 1 cycle,
                            ; see if button still pressed
