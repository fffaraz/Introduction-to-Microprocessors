; output a time varying 0-1 sequence
; on pin 5 of port B for as long as
; a button connected to pin B 4 is pressed

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
                            ; set up pin change interrupt 1
       ldi r28, PCMSK1      ; load address of PCMSK1 in Y low
       clr r29              ; load high byte with 0
       ld r16, Y            ; read value in PCMSK1
       sbr r16,0b00010000   ; allow pin change interrupt on portB pin 4
       st Y, r16            ; store new PCMSK1 

       ldi r28, PCICR
	   sbr r16, 0b00000010
	   st  Y, r16
;       sbi PCICR, PCIE1     ; enable pin change interrupt 1
       sbi PCIFR, PCIF1     ; clear pin change interrupt flag 1

       ldi r16, 0xff        ; set stack pointer
       out SPL, r16
       ldi r16, 0x04
       out SPH, r16

       ldi R16, 0b00100000  ; load register 16 to set portb registers
       out DDRB, r16        ; set portb 5 to output, others to input
       ser R16              ; 
       out PORTB, r16       ; set pullups (1's) on inputs

       sei                  ; enable interrupts

       ldi R16, 0b00000001  ; set sleep mode
       out SMCR, R16
       rjmp loop

PCINT_1:
      reti
	  rjmp LOOP   

SNOOZE:
      sleep

LOOP:
      sbic PINB, 4         ; skip next line if button pressed
      rjmp SNOOZE          ; go back to sleep if button not pressed

      cbi PORTB, 5         ; set speaker input to 0
      ldi R16, 128         ;

SPIN1:                     ; wait a few cycles
      subi R16, 1
      brne SPIN1

      sbi PORTB, 5         ; set speaker input to 1
      ldi R16, 128

SPIN2:
      subi R16, 1
      brne SPIN2

      rjmp LOOP            ; speaker buzzed 1 cycle,
                           ; see if button still pressed
