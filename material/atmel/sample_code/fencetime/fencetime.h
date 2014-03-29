#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#define TMAX	3
#define TRUE 	1
#define FALSE	0
#define DEBOUNCE 3

/*
#define A  0b00000001
#define G  0b00000010
#define B  0b00000100
#define F  0b00001000
#define C  0b00010000
#define E  0b00100000
#define DP 0b01000000
#define D  0b10000000
*/
#define G  0b00000001
#define A  0b00000010
#define F  0b00000100
#define B  0b00001000
#define E  0b00010000
#define C  0b00100000
#define D 0b01000000
#define DP  0b10000000

#define G_H	0  // green score high digit, and green card yellow
#define G_L	1  // green score low digit, green priority
#define R_H	2  // red score high digit, and red card yellow
#define R_L	3  // red score low digit, red priority
#define T_SH	4  // seconds, high digit
#define T_SL	5  // seconds, low digit, and red card red
#define R_	6  // round, and green card red
#define T_M	7  // minutes

const unsigned char digit[16] PROGMEM = {
A|B|C|D|E|F,     // 0
B|C,             // 1
A|B|G|E|D,       // 2
A|B|G|C|D,       // 3
F|G|B|C,         // 4
A|F|G|C|D,       // 5
A|F|E|D|C|G,     // 6
A|B|C,           // 7
A|F|G|C|D|E|B,   // 8
A|F|G|B|C,       // 9
A|F|G|B|E|C,     // A
F|E|D|C|G,       // b
A|F|E|D,         // C
G|E|D|C|B,       // d
A|F|G|E|D,       // E
A|F|G|E          // F
// A|B|F|G|E,       // P
// E|F|G            // t
};  // hex digits for 7 segment display
