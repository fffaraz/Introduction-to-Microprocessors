#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>
#include <avr/iom644.h>

void initialize(void);
void int_handle(void);
void buzz(void);
