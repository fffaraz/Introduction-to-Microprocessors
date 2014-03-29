#include "servo.h"

volatile uint16_t position = 1000;
int
main( void )
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    timer1_init();

    while ( 1 ) {
        sleep_mode();
    }
    return 0;
}

void
timer1_init( void )
{
    /* output is clear on match, set on bottom,
     * mode is 14, top is ICR1, prescale = 1
     */
    TCCR1A = _BV(COM1A1)|_BV(WGM11);
    TCCR1B = _BV(WGM13)|_BV(WGM12)|_BV(CS10);

    /* assuming a 1Mhz system clock,
     * a 20 millisecond period is
     * 0.020 / 0.000001 = 20000
     */
    ICR1 = 20000;
    OCR1A = 1000;    // set at minimum

    TIMSK1 = _BV(OCIE1A);  // allow compare match A interrupts
    TIFR1  = _BV(OCF1A);  // clear interrupts

    DDRD  |= _BV(PD5); // enable timer1 pwm output
    sei();
}

ISR(TIMER1_COMPA_vect)
{
   // this could be done outside the interrupt routine
    if (position < 2000) position++;
    else (position = 1000);
    OCR1A = position;
}


EMPTY_INTERRUPT(BADISR_vect)
