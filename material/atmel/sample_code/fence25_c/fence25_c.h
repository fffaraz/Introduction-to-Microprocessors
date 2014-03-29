#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>
#include<avr/pgmspace.h>
#include<avr/eeprom.h>

void int_handle(void);
uint8_t getmode(void);
void foil_handle(void);
void epee_handle(void);
void hit(void);
void set_tune(void);

#define INPIN           PB3
#define OUTPIN          PB1
#define TESTIN          PB0
#define PICKTUNE        PB2
#define FOILEPEE        PB3

#define FOIL_HITTIME   140
#define EPEE_HITTIME    20

#define B6     252   //  1976.28 Hz.
#define C7     238   //  2092.05 Hz.
#define Cs7    224   //  2222.22 Hz.
#define D7     212   //  2347.42 Hz.
#define Ds7    200   //  2487.56 Hz.
#define E7     189   //  2631.58 Hz.
#define F7     178   //  2793.30 Hz.
#define Fs7    168   //  2958.58 Hz.
#define G7     158   //  3144.65 Hz.
#define Gs7    149   //  3333.33 Hz.
#define A7     141   //  3521.13 Hz.
#define As7    133   //  3731.34 Hz.
#define B7     126   //  3937.01 Hz.
#define C8     118   //  4201.68 Hz.
#define Cs8    112   //  4424.78 Hz.
#define D8     105   //  4716.98 Hz.
#define Ds8     99   //  5000.00 Hz.
#define E8      94   //  5263.16 Hz.
#define F8      88   //  5617.98 Hz.
#define Fs8     83   //  5952.38 Hz.
#define G8      79   //  6250.00 Hz.
#define Gs8     74   //  6666.67 Hz.
#define A8      70   //  7042.25 Hz.
#define As8     66   //  7462.69 Hz.
#define B8      62   //  7936.51 Hz.
#define C9      59   //  8333.33 Hz.
#define Cs9     55   //  8928.57 Hz.
#define D9      52   //  9433.96 Hz.
#define Ds9     49   // 10000.00 Hz.
#define B6     252   // 1976.28 Hz.
#define R        1   // rest
