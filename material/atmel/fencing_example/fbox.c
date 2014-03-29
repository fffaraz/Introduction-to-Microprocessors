//------------------------------------------------
// Affordable Fencing Box
//
// Microcontroller code
//
// Adapted from PIC code by David W. Green
//             April 3, 2003
//
// Recoded I/O for ATMEL AVR processors April 27, 2006  PG
// Rewrote foil state machine May 20, 2006  PG
// Rewrote sabre state machine June 5, 2006
// Last updated: July 6, 2006   P.G.
//------------------------------------------------


#include "fbox.h"

// Interrupt-driven timer functionality
volatile int16_t timer[2];
volatile int16_t timerOff[2];
volatile short parrytimer;

// Enumerated type for mode of operation
// enum BoxMode {modeEpee=0, modeFoil, modeSabre } mode;

// Enumerated types for players
enum Player {RED=0, GREEN} player;

// Enumerated types for state machines
enum EpeeState { eInit = 0, eTransient, eHit, eThrottle } epee[2];
enum FoilState { fInit = 0, fTransient, fCheckTouch, fHit, fThrottle } foil[2];
enum SabreCheckTouch { sInit = 0, sStart, sTransient, sHit, sThrottle } sabreCheck[2];
enum SabreParryCheck { sParryInit = 0, sParry, sParryBlock } sabreParry;

unsigned short fOntarget[2], microbreak[2], oldparry, PARRY_INTERRUPT_COUNT, THROTTLE, BLOCK, BUZZ, NO_RED_HIT=TRUE, NO_GREEN_HIT=TRUE;

// Function prototypes; descriptions of the functions are
// found next to the definitions.

// void OSCCAL_calibration( void);
void restartStates ( void );
void Delay(unsigned int);

// Individual weapon state machine progression functions
void epeeProcess ( void );
void foilProcess ( void);
void sabreProcess ( void );

// Main function
int main() {
	short test;
	enum BoxMode {modeEpee=0, modeFoil, modeSabre } mode;

// calibrate oscillator
// OSCCAL_calibration();

// set oscillator speed to 8MHz
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;    //   8 MHz    1MHz = (_BV(CLKPS1) | _BV(CLKPS0))

	// Set sleep mode to wait out interrupts
	set_sleep_mode(SLEEP_MODE_IDLE);	  

// Set up timer 1 to interrupt every 0.1 ms	
	OCR1A  =  99; 
//	TCCR1A = _BV(COM1A0);
	TCCR1B = _BV(WGM12)|_BV(CS11);			// CTC mode, clk/8 prescaler -- clock = 1MHz
	TIMSK1 = _BV(OCIE1A);

// Set up timer to generate a square wave output at 4KHz
// (The 644 uses timer 2, the 168 uses timer 0 - both are similar
// and set in the include file.)
//

	TIMER_OUT_DDR |= _BV(BUZZER);
	TCCRA = _BV(WGM21)|_BV(COM2A0);    //set CTC mode, toggle OC2A (PD6) on compare match
	TCCRB = _BV(CS21);    // clk/8 prescaler -- clock = 1MHz
	OCRA  = 63;            // sets the output half frequency (63 is about 2.5KHz)

// Set up output ports (lights)
	LIGHT_DDR  =  (short) _BV(RED_LIGHT)|_BV(GREEN_LIGHT)|_BV(WHITE_RED)|_BV(WHITE_GREEN)|_BV(YELLOW_RED)|_BV(YELLOW_GREEN);
	MODE_OUT_DDR |= _BV(MODE_OUT_SABRE) | _BV(MODE_OUT_FOIL);

	TIFR1 = _BV(OCF1A);  // clear timer flag
	sei();      // enable global interrupts (basically, the timer)

// Test output lights and buzzer
	timer[0] = 0;
		
	LIGHT_PORT =  _BV(GREEN_LIGHT)|_BV(RED_LIGHT);
	while (timer[0] <= 5000) sleep_mode();

	LIGHT_PORT =  _BV(WHITE_GREEN)|_BV(WHITE_RED);
	while (timer[0] <= 10000) sleep_mode();
	
	LIGHT_PORT =  _BV(YELLOW_GREEN)|_BV(YELLOW_RED);
	while (timer[0] <= 15000) sleep_mode();

// Turn off lights and buzzer
	LIGHT_PORT =  ~(_BV(RED_LIGHT)|_BV(GREEN_LIGHT)|_BV(WHITE_RED)|_BV(WHITE_GREEN)|_BV(YELLOW_RED)|_BV(YELLOW_GREEN));
	TCCRB = 0;             // turn off buzzer

	MODE_IN_DDR &= ~(_BV(MODE_FES_PIN) | _BV(MODE_STATE_PIN));
	MODE_IN_PORT |= _BV(MODE_FES_PIN) | _BV(MODE_STATE_PIN);  // set pullups 

//	set mode output to default mode - EPEE (quiet)
	mode = modeEpee;
	MODE_OUT_PORT = _BV(MODE_OUT_SABRE) | _BV(MODE_OUT_FOIL);

	// Infinite loop which handles state machine progressions
	while (TRUE) {

// Retrieve the new mode if it has changed (default is epee --- quiet)
		MODE_IN_PORT |= _BV(MODE_FES_PIN) | _BV(MODE_STATE_PIN);
		test = MODE_IN_PIN & _BV(MODE_FES_PIN);
		if (!test) {
			timer[0] = 0;
			mode = (mode+1)%MAXMODES;
			 
			while(!test) {      // debounce switch
				while (timer[0] < 1000) sleep_mode();		// wait 0.1 s
				test = MODE_IN_PIN & _BV(MODE_FES_PIN);
			}

			switch(mode) {
			case modeEpee:
				MODE_OUT_PORT = _BV(MODE_OUT_SABRE) | _BV(MODE_OUT_FOIL);
				break;

			case modeFoil:	
				MODE_OUT_PORT = _BV(MODE_OUT_FOIL);
				break;

			case modeSabre:
				MODE_OUT_PORT = _BV(MODE_OUT_SABRE);
				break;
			}
			restartStates();
		}

		// Progress the state machine according to the mode
		switch ( mode ) {

			case modeEpee:	
				epeeProcess();
				break;

			case modeFoil:	
				foilProcess();
				break;

			case modeSabre:
				sabreProcess();
				break;
		}

	sleep_mode();    // sleep until timer 0 interrrupts
	}
}


// Process foil events and progress the state machine as required.
// normally closed switch - a hit is recognized when the switch is
// open 14 +-1 ms.
// To detect A hit, the blade wire is powered, and the guard grounded,
// pulling the blade wire low. When the switch opens, the blade wire goes high.
// To detect if a hit is on target, the opponents jacket is grounded, and
// the jacket pulls the blade wire low (if the jacket resistance < 500 ohms).
// Subsequent hits by the opponent are valid only for the next 300 +-25 ms.
// Blocking is detected as for a hit, but between the guard and players jacket.
//
void foilProcess ( void ) {
	short test, player, touch[2], offtarget[2];

	INPUT_DDR  =  _BV(JACKET_GREEN); // set JACKET_RED to output, else input
	INPUT_PORT = _BV(GUARD_GREEN);   // set pullup, all other inputs to high-Z

	BLADE_PWR_DDR  &= ~(_BV(BLADE_RED_PWR)|_BV(BLADE_GREEN_PWR)); // can be combined with below
	GUARD_PWR_DDR  &= ~(_BV(GUARD_RED_PWR)|_BV(GUARD_GREEN_PWR)); // set to inputs
	BLADE_PWR_PORT &= ~(_BV(BLADE_RED_PWR)|_BV(BLADE_GREEN_PWR));
	GUARD_PWR_PORT &= ~(_BV(GUARD_RED_PWR)|_BV(GUARD_GREEN_PWR));// tristate

	// Check for GREEN blocking

	// Turn on GREEN guard power, set jacket to ground, and test blade
	// (or guard) input ---  tri-state everything else

	GUARD_PWR_DDR  |= _BV(GUARD_GREEN_PWR);
	GUARD_PWR_PORT |=  _BV(GUARD_GREEN_PWR); // output_high ( GUARD_RED_PWR );
	
	_delay_us(3);
// GUARD_GREEN_PWR output high, JACKET_GREEN output low, GUARD_GREEN input

	test = INPUT_PIN & _BV(GUARD_GREEN);  // input(GUARD_GREEN)
	if ( test )  {   // GREEN is not blocking
		LIGHT_PORT &= ~_BV(YELLOW_GREEN); // turn off yellow lamp
	}
	else {        // GREEN is blocking
		LIGHT_PORT |= _BV(YELLOW_GREEN);   //turn on yellow lamp
	}

	INPUT_DDR  &= ~_BV(JACKET_GREEN);   // set jacket to high-Z
	GUARD_PWR_DDR  &= ~_BV(GUARD_GREEN_PWR); // turn off GUARD_GREEN_PWR
	GUARD_PWR_PORT &= ~_BV(GUARD_GREEN_PWR);  // tristate

	// Check for hits - turn on power RED's foil
	// Disable player GREEN's weapon (already done), check for hits on him.
	// Ground GREEN's guard if not blocking
	// first see if tip is depressed - ground guard, and test input
	
	BLADE_PWR_DDR |= _BV(BLADE_RED_PWR);
	BLADE_PWR_PORT |= _BV(BLADE_RED_PWR);
	INPUT_DDR   =  _BV(GUARD_RED);  // ground RED guard
	if (test) INPUT_DDR   |= _BV(GUARD_GREEN);	// ground GREEN guard if not blocking
	INPUT_PORT = _BV(BLADE_RED_IN);  // set pullup
	// INPUT_PORT &= ~_BV(GUARD_RED); // output_low ( GUARD_RED );
	_delay_us(3);
// BLADE_RED_PWR output high, GUARDS RED & GREEN output low, BLADE_RED_IN input
	
	touch[RED]  = INPUT_PIN & _BV(BLADE_RED_IN);	// input(BLADE_RED_IN)

	if (touch[RED]) {          // a hit - on or off target
//		LIGHT_PORT |= _BV(RED_LIGHT);  //test
		// test for valid hit - ground green jacket, test blade
		INPUT_DDR  |=  _BV(JACKET_GREEN);
		INPUT_PORT &= ~_BV(JACKET_GREEN);      // Set GREEN jacket to output 0 (ground)		
		_delay_us(3);
		offtarget[RED] = INPUT_PIN & _BV(BLADE_RED_IN);// input(BLADE_RED)
//		if (offtarget[RED]) 	LIGHT_PORT |= _BV(WHITE_RED);  // test 
	}

	BLADE_PWR_DDR &= ~_BV(BLADE_RED_PWR);
	BLADE_PWR_PORT &= ~_BV(BLADE_RED_PWR); // Red blade set to high-Z

// GUARD_RED output low, BLADE_RED_IN input --- RED inputs finished ---
	// Check for RED blocking

	// Turn on RED guard power, set jacket to ground, and test blade
	// (or guard) input --- tri-state everything else

	GUARD_PWR_DDR  |= _BV(GUARD_RED_PWR);
	GUARD_PWR_PORT |=  _BV(GUARD_RED_PWR); // power to (GUARD_RED_PWR);
	
	INPUT_DDR  = _BV(JACKET_RED);
	INPUT_PORT = _BV(GUARD_RED);    // set pullup on GUARD_RED input
					// Set RED jacket to output 0 (ground)
	_delay_us(3);
// GUARD_RED_PWR output high, JACKET_RED output low, GUARD_RED input

	test = INPUT_PIN & _BV(GUARD_RED);  // input(GUARD_RED)
	if ( test )  {
		//  not blocking; turn off the yellow lamp.
		LIGHT_PORT &= ~_BV(YELLOW_RED);  // output_low(YELLOW_RED)
	}
	else {        //  blocking
		LIGHT_PORT |= _BV(YELLOW_RED);  // turn on yellow lamp
	}


	INPUT_DDR  &= ~_BV(JACKET_RED);   // set jacket to high-Z
	GUARD_PWR_DDR  &= ~_BV(GUARD_RED_PWR);  // turn off GUARD_RED_PWR
	GUARD_PWR_PORT &= ~_BV(GUARD_RED_PWR);   // tristate GUARD_RED_PWR

	// Check for hits - turn on power GREEN's foil
	// Disable player RED's weapon (already done), check for hits on him.
	// first see if tip is depressed - ground guard, and test input

	BLADE_PWR_DDR |= _BV(BLADE_GREEN_PWR);
	BLADE_PWR_PORT |= _BV(BLADE_GREEN_PWR);
	INPUT_DDR   =  _BV(GUARD_GREEN);
	if (test) INPUT_DDR   |= _BV(GUARD_RED);	// ground RED guard if not blocking
	INPUT_PORT  =  _BV(BLADE_GREEN_IN); // set pullup on BLADE_GREEN
					  // set GUARD_GREEN output low
	_delay_us(3);
// BLADE_GREEN_PWR output high, GUARDS GREEN & RED output low, BLADE_GREEN_IN input
	
	touch[GREEN]  = INPUT_PIN & _BV(BLADE_GREEN_IN);// input(BLADE_GREEN_IN)

	if (touch[GREEN]) {          // a hit - on or off target
//		LIGHT_PORT |= _BV(GREEN_LIGHT);  // test
			// test for valid hit - ground RED jacket, test blade
		INPUT_DDR  =  _BV(JACKET_RED);  // set JACKET_RED output low
		_delay_us(3);

		offtarget[GREEN] = INPUT_PIN & _BV(BLADE_GREEN_IN);  // input(BLADE_GREEN)
//		if (offtarget[GREEN]) LIGHT_PORT |= _BV(WHITE_GREEN);  // test
	}
// BLADE_GREEN_PWR output high, JACKET_RED output low, BLADE_GREEN_IN input

if (!(touch[RED]) && !(touch[GREEN]) ) BUZZ = TRUE;

// -------  GREEN inputs finished  --------------------------------------

// foil state machine

for (player=0; player <=1; player++) {
	// State machine progression
	switch ( foil[player] ) {
		case fInit:
			if ( touch[player] && !THROTTLE ) {
				foil[player] = fTransient;
				timer[player] = 0;
				fOntarget[player] = 0;
				microbreak[player] = MICROBREAK_MAX;
			}
			break;
		case fTransient:
			if (touch[player]) {
				microbreak[player] = MICROBREAK_MAX;
				if (!offtarget[player]) fOntarget[player]++;
			}
			else {
				microbreak[player]--;
			}
			if ( !microbreak[player] ) {
				foil[player] = fInit;
				break;
			}		
			if ( timer[player] > FOIL_DEBOUNCE ) {
				foil[player] = fCheckTouch;
			}
			break;
		case fCheckTouch:
			foil[player] = fHit;
			if (BUZZ) TCCRB = _BV(CS01);    // turn on buzzer
			BUZZ = FALSE;
			timer[player] = 0;
			if ( player == RED ) {
				LIGHT_PORT |= _BV(RED_LIGHT);
				if (( fOntarget[player]) < (FOIL_DEBOUNCE >>1) ) { 			
					LIGHT_PORT |= _BV(WHITE_RED);
				}
			}
			else {
				LIGHT_PORT |= _BV(GREEN_LIGHT);
				if (( fOntarget[player]) < (FOIL_DEBOUNCE >>1) ) {
					LIGHT_PORT |= _BV(WHITE_GREEN);
				}
			}
			break;
		case fHit:
			if ( timer[player] > FOIL_THROTTLE ) {
				foil[player] = fThrottle;
				THROTTLE = TRUE;
			}
			break;
		case fThrottle:
			if (timer[player] > BUZZMAX) restartStates();
			break;
		}
	}
}


// Process epee events and progress the state machine as required.
// epee - normally open switch; switch closure for 2 - 10 ms establishes a hit
// the blade wire is powered, and if the other blade wire (blade2) 
// is high for 2 ms, then a hit is output
// double hits are possible within a 40 - 50 ms window, otherwise only one
// hit can be recorded
//
void epeeProcess ( void) {

	short touch[2];

	// Set up inputs
	if (!THROTTLE) {
	INPUT_DDR      = _BV(GUARD_RED)|_BV(GUARD_GREEN);
	BLADE_PWR_DDR |= _BV(BLADE_RED_PWR)|_BV(BLADE_GREEN_PWR);
	// power to blades
	BLADE_PWR_PORT |= _BV(BLADE_RED_PWR)|_BV(BLADE_GREEN_PWR);
	// set guards low - ground guards
	INPUT_PORT &= ~(_BV(GUARD_RED)|_BV(GUARD_GREEN));
	_delay_us(3);

	// hit if BLADE2 is high - blades wires are connected - tip is pressed
	touch[RED] = INPUT_PIN & _BV(BLADE2_RED);  // input(BLADE2_RED)
	touch[GREEN] = INPUT_PIN & _BV(BLADE2_GREEN);  // input(BLADE2_GREEN)
 	}
	for (player=0; player <=1; player++) {
		// State machine progression
		switch ( epee[player]) {
			case eInit:
				if ( touch[player] && !THROTTLE) {
					epee[player] = eTransient;
					timer[player] = 0;
				}
				break;
			case eTransient:
				if ( !touch[player] ) {
					epee[player] = eInit;
					break;
				}
				if ( (timer[player] >= EPEE_DEBOUNCE) ) {
					epee[player] = eHit;
					timer[player] = 0;
					TCCRB = _BV(CS01);    // turn on buzzer
				}
				break;
			case eHit:
				if ( player == RED ) {
					LIGHT_PORT |= _BV(RED_LIGHT);   // turn on red light
				}
				else {
					LIGHT_PORT |= _BV(GREEN_LIGHT);   // turn on green light
				}
				if ( timer[player] >= EPEE_THROTTLE ) {
					epee[player] = eThrottle;
					THROTTLE = TRUE;
				}
				break;
			case eThrottle:

			if (timer[player] >= BUZZMAX) restartStates();
			break;
		}			
	}
}


// Process sabre events and progress the state machine  as required.
//
// a hit is determined by contact with the opponents jacket for >0.1ms
// a disconnect occurs when the resistance between blade & guard is > 250 ohms
// for  3 +- 2 ms. (the white lamp is lit).
// blocking is determined as in foil
// a touch can be made on the guard or blade of a player blocking, if the
// increase of resistance through the jacket is < 250 ohms (not implemented).
// A parry is detected by contact between the blades??
// A hit "through the blade" is registered between 0 and 4 +1 ms after contact
// A hit is not registered (blocked) between 4 and 15 +5 ms after contact,
// provided that contact has not been broken more than 10 times
// A hit is registered normally after this period.
// A touch on the piste is identified first - it would otherwise mask a hit.
//
// rule 6 is still unclear to me
//
// Rule M51 - "A touch made on the metallic strip --- must not be registered
// nor  may  it prevent the registering of a touch made ... by the opponent."
//
// Here, a touch on the piste is checked first - it would otherwise mask a hit.
//
void sabreProcess ( void ) {

	short test, notouch[2], noparry, piste, player;

	INPUT_DDR  = 0;			 // set all inputs to input mode
	INPUT_PORT = _BV(BLADE_RED_IN);    // set pullup resistor on blade
                                      	   // and tristate all others

	GUARD_PWR_DDR &= ~(_BV(GUARD_RED_PWR) | _BV(GUARD_GREEN_PWR));
	GUARD_PWR_PORT &= ~(_BV(GUARD_RED_PWR) | _BV(GUARD_GREEN_PWR));

	BLADE_PWR_DDR &= ~(_BV(BLADE_GREEN_PWR));  // turn off  GREEN blade power
	BLADE_PWR_DDR |= _BV(BLADE_RED_PWR);  // turn on  RED blade power

	BLADE_PWR_PORT &= ~(_BV(BLADE_GREEN_PWR)); // tristate GREEN blade power
	BLADE_PWR_PORT |= _BV(BLADE_RED_PWR);  // power RED blade

// BLADE_RED_PWR is on, BLADE_RED_IN is input, all other inputs tri-state
	_delay_us(3);

	piste  = INPUT_PIN & _BV(BLADE_RED_IN);  // input ( BLADE_RED_IN );
	if (!piste) {
		noparry = TRUE;
		notouch[RED] = TRUE;
	}

	else {
	// Detect parry
	INPUT_DDR = _BV(BLADE_GREEN_IN);	// set GREEN blade output (low)

// BLADE_RED_PWR is on, BLADE_RED_IN is input, BLADE_GREEN_IN is output low
	_delay_us(3);

	noparry = INPUT_PIN & _BV(BLADE_RED_IN);  // input ( BLADE_RED_IN );
//	if (noparry) TCCRB = 0;		// test parry - turn off buzzer
//	else TCCRB = _BV(CS01);    // test - turn on buzzer
// player RED
        // Check for unplugged cord -- open between blade & guard

	INPUT_DDR = _BV(GUARD_RED);  // set RED guard to output low

// here, BLADE_RED_PWR is on, BLADE_RED_IN is input, GUARD_RED is output low
	_delay_us(3);

	test = INPUT_PIN & _BV(BLADE_RED_IN);  // input ( BLADE_RED_IN )
        					// if low, RED is unplugged
	if ( !test ) {
		timerOff[RED] = 0;
	}

	if ( timerOff[RED] > 30 ) {
		timerOff[RED] = 32;
		LIGHT_PORT |= _BV(RED_LIGHT) | _BV(WHITE_RED);  // white light (red side)
	}
	else {
		LIGHT_PORT &= ~( _BV(WHITE_RED));  // light off
		if (NO_RED_HIT) LIGHT_PORT &= ~( _BV(RED_LIGHT));
	}
	
        // check for blocking - as in foil --- red blade is already powered
        INPUT_DDR = _BV(JACKET_RED);  // set RED jacket to output low
        _delay_us(3);

// here, BLADE_RED_PWR is on, BLADE_RED_IN is input, JACKET_RED is output low
        test = INPUT_PIN & _BV(BLADE_RED_IN);  // input ( BLADE_RED_IN )
        if ( test ) {
                LIGHT_PORT &= ~_BV(YELLOW_RED);  // RED not blocking
        }
        else {
                LIGHT_PORT |= _BV(YELLOW_RED);  // RED is blocking
        }

	// Check for hits
	// Disable player GREEN's weapon, check for hits on him.

		INPUT_DDR = _BV(JACKET_GREEN);  // set GREEN jacket to output low
		_delay_us(3);

// here, BLADE_RED_PWR is on, BLADE_RED_IN is input, JACKET_GREEN is output low
        notouch[RED] = INPUT_PIN & _BV(BLADE_RED_IN); // input(BLADE_RED_IN);
//		if (notouch[RED]) LIGHT_PORT &= ~_BV(RED_LIGHT) ;	// test for red hit
//		else LIGHT_PORT |= _BV(RED_LIGHT) ;					// test
	}

// player GREEN

	INPUT_DDR  = 0;			 // set all inputs to input mode
	INPUT_PORT = _BV(BLADE_GREEN_IN);    // set pullup resistor on blade
                                      	   // and tristate all others
	BLADE_PWR_DDR &= ~(_BV(BLADE_RED_PWR));  // turn off red blade
 	BLADE_PWR_DDR |= _BV(BLADE_GREEN_PWR);  // enable GREEN blade power

	BLADE_PWR_PORT |= _BV(BLADE_GREEN_PWR); // turn on GREEN blade
	BLADE_PWR_PORT &= ~(_BV(BLADE_RED_PWR)); // tristate RED blade

// BLADE_GREEN_PWR is on, BLADE_GREEN_IN is input, all other inputs tri-state
	_delay_us(3);

	piste  = INPUT_PIN & _BV(BLADE_GREEN_IN);  // input BLADE_GREEN_IN
	if (!piste) {
		noparry = TRUE;
		notouch[GREEN] = TRUE;
	}

	else {
	// check for unplugged cord -- open between blade and guard

        INPUT_DDR = _BV(GUARD_GREEN);  // GREEN guard is set to input low

// BLADE_GREEN_PWR is on, BLADE_GREEN_IN is input, GUARD_GREEN is output low
        _delay_us(3);
	
	test = INPUT_PIN & _BV(BLADE_GREEN_IN);  // input ( BLADE_GREEN_IN )
	if ( !test ) {
		timerOff[GREEN] = 0;
	}

	if ( timerOff[GREEN] > 30 ) {
		timerOff[GREEN] = 32;
		LIGHT_PORT |= _BV(GREEN_LIGHT) | _BV(WHITE_GREEN); // white light (green)
	}
	else {
		LIGHT_PORT &= ~( _BV(WHITE_GREEN)); //  turn off white light
		if (NO_GREEN_HIT) LIGHT_PORT &= ~( _BV(GREEN_LIGHT));
	}

	// check for GREEN blocking
	INPUT_DDR = _BV(JACKET_GREEN);  // set GREEN jacket to output low
	_delay_us(3);

// BLADE_GREEN_PWR is on, BLADE_GREEN_IN is input, JACKET_GREEN is output low
	test = INPUT_PIN & _BV(BLADE_GREEN_IN);  // input ( BLADE_GREEN_IN )
	if ( test ) {
		LIGHT_PORT &= ~_BV(YELLOW_GREEN); //GREEN is not blocking
	}
	else {
		LIGHT_PORT |= _BV(YELLOW_GREEN); // GREEN is blocking
 	}

	// Check for hits
        // Disable player RED's weapon, (done), check for hits on him.

        INPUT_DDR = _BV(JACKET_RED);

// BLADE_GREEN_PWR is on, BLADE_GREEN_IN is input, JACKET_RED is output low
        _delay_us(3);

        notouch[GREEN] = INPUT_PIN & _BV(BLADE_GREEN_IN);

	}
//	if (notouch[GREEN]) LIGHT_PORT &= ~_BV(GREEN_LIGHT);	//test green hit
//	else 	LIGHT_PORT |= _BV(GREEN_LIGHT);					// test

//   -----   all sabre inputs obtained   ---

// A valid hit is >0.1 ms.
// If there is a parry, then a subsequent hit is valid if
//    1) between 0 - 4 ms (+1ms) following the parry
//    2) after 15 ms (+- 5 ms)
//    3) a parried hit is invalid between 4 and 15 ms on condition that
//       contact between the blades is not interrupted more than 10
//       times in that interval
//       
//  Basically, a parry ends after 15 ms (+- 5 ms)
//
// Sabre state machine
//

	for (player=0; player <=1; player++) {
	switch ( sabreCheck[player] ) {
		case sInit:
			if ( !notouch[player] && !(THROTTLE || BLOCK)) {
				sabreCheck[player] = sStart;
			}
			break;
		case sStart:
			if ( notouch[player] ) {
				sabreCheck[player] = sInit;
			}
			else {
				sabreCheck[player] = sTransient;
			}
			break;
		case sTransient:
			sabreCheck[player] = sHit;
			timer[player] = 0;
			TCCRB = _BV(CS01);    // turn on buzzer
			// turn on red or green light
				if ( player == RED ) {
					LIGHT_PORT |= _BV(RED_LIGHT);   // turn on red light
					NO_RED_HIT = FALSE;
				}
				else {
				LIGHT_PORT |= _BV(GREEN_LIGHT);   // turn on green light
					NO_GREEN_HIT = FALSE;
				}
			break;
		case sHit:
			if ( timer[player] >= SABRE_THROTTLE ) {
				sabreCheck[player]   = sThrottle;
				THROTTLE = TRUE;
			}
			break;
		case sThrottle:
			if ( timer[player] >= BUZZMAX ) {
				restartStates();
			}
			break;
		}
	}
	
//-------------------- end for  ---------------------------------------

	switch ( sabreParry) {

		case sParryInit:
			if ( !noparry && !(THROTTLE) ) {
				parrytimer = 0;
				oldparry = noparry;
				sabreParry = sParry;
			}
			break;

		case sParry:
			if (parrytimer >= 40) {
				sabreParry = sParryBlock;
				oldparry = noparry;
				BLOCK = TRUE;
			}
			break;

		case sParryBlock:
			if (oldparry != noparry) {
				PARRY_INTERRUPT_COUNT++;
				oldparry = noparry;
				if (PARRY_INTERRUPT_COUNT > PARRY_BLOCK_START) {
					sabreParry = sParryInit;
					PARRY_INTERRUPT_COUNT = 0;
					BLOCK = FALSE;
				}
			}
			if (parrytimer >= PARRY_BLOCK_END) {
				sabreParry = sParryInit;
				PARRY_INTERRUPT_COUNT = 0;
				BLOCK = FALSE;
			}
			break;
	}

}


// restartStates
// Restarts the state machines and turns off all lights and the buzzer
void restartStates ( void ) {
	THROTTLE = FALSE;
//	switch ( mode ) {
//		case modeFoil:
			foil[RED]   = fInit;
			foil[GREEN] = fInit;
//			break;
//		case modeEpee:
			epee[RED]   = eInit;
			epee[GREEN] = eInit;
//			break;
//		case modeSabre:
			sabreCheck[RED]   = sInit;
			sabreCheck[GREEN] = sInit;
			sabreParry        = sParryInit;
			BLOCK = FALSE;
			NO_RED_HIT = TRUE;
			NO_GREEN_HIT = TRUE;
//			break;
//	}

	// Turn the lights and buzzer off
	LIGHT_PORT &= ~(_BV(RED_LIGHT)|_BV(GREEN_LIGHT)|_BV(WHITE_RED)|_BV(WHITE_GREEN)|_BV(YELLOW_RED)|_BV(YELLOW_GREEN));
	TCCRB = 0;				// turn off buzzer
	sleep_mode();			// sleep until interrupt
}


//SIGNAL (TIMER0_COMPA_vect)
SIGNAL (TIMER1_COMPA_vect)
{
	timer[RED]++;
	timer[GREEN]++;
	timerOff[RED]++;
	timerOff[GREEN]++;
	parrytimer++;	
}

/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = FALSE;
    int temp;
    unsigned char tempL;

    CLKPR = _BV(CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = _BV(CLKPS1) | _BV(CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = _BV(AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCRA = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = _BV(CS10);     // start timer1 with no prescaling
    TCCR2A = _BV(CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        cli(); // mt __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        // shc/mt while ( !(TIFR2 && _BV(OCF2A)) );   // wait for timer2 compareflag    
        while ( !(TIFR2 & _BV(OCF2A)) );   // wait for timer2 compareflag

        TCCR1B = 0; // stop timer1

        sei(); // __enable_interrupt();  // enable global interrupt
    
        // shc/mt if ( (TIFR1 && _BV(TOV1)) )
        if ( (TIFR1 & _BV(TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = TRUE;   // the interRC is correct

        TCCR1B = _BV(CS10); // start timer1
    }
}

/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop
*
*****************************************************************************/
void Delay(unsigned int millisec)
{
	// mt, int i did not work in the simulator:  int i; 
	uint8_t i;

	while (millisec--) {
		for (i=0; i<125; i++) {
			asm volatile ("nop"::);
		}
	}
}
