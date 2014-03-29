#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>
#include <avr/power.h>

#define TMAX	3

unsigned char maxmin = TMAX;

typedef struct{ 
unsigned char tick;	// a 1/128 second tick
unsigned char second;
unsigned char minute;
            }time;

volatile time t;  

void init_rtc(void);
void init_io(void);
void display(void);
