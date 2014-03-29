/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA sleep manager example main program file for Xplain
 *
 *      This file contains an example program which initializes the XMEGA 128A1
 *      on the Xplain board to the least power consuming state, and puts the
 *      device to sleep.\n
 *      By default, the XMEGA is put in POWER-SAVE mode, and wakes up every
 *      five seconds. The device stays in ACTIVE mode for 0.5 sec before going
 *      back to sleep. This gives a duty cycle of 10%.
 *
 *      The interval timing is done with the RTC and external 32 kHz crystal
 *      oscillator as clock source, which should result in less than 1 µA
 *      current consumption.
 *
 *      \note The device is initialized with the LOWPOWER_Init(), which enables
 *      pull-up on all I/O pins and disables the JTAG-interface (if configured
 *      in lowpower.h). Some minor configuration specific for Xplain is then
 *      done to minimize the power consumption.
 *
 * \par Application note:
 *      AVR1010: Minimizing the power consumption of XMEGA devices
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 2940 $
 * $Date: 2009-11-02 09:55:56 +0100 (ma, 02 nov 2009) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/


#include "avr_compiler.h"
#include "ccpwrite.h"
#include "rtc_driver.h"
#include "sleepmgr.h"
#include "lowpower.h"


/*! Specify which sleep mode to use.
 *
 * The default alternatives are:<pre>
 * SLEEPMGR_IDLE - Idle
 * SLEEPMGR_ESTDBY - Extended standby
 * SLEEPMGR_SAVE - Power-save
 * SLEEPMGR_STDBY - Standby
 * SLEEPMGR_DOWN - Power-down</pre>\n
 *
 * \note If this is not defined, the sleep manager defaults to Power-down.
 *
 * \note The device cannot wake itself up from Standby or Power-down because
 * the RTC is disabled in these modes.
 */
#define SLEEP_MODE SLEEPMGR_SAVE


/*! Configure if RTC should be used. (Comment out otherwise.)
 *
 * This wakes the device up at the specified number of seconds.
 *
 * \note The RTC is disabled in Standby and Power-down!
 */
#define USE_RTC
//! Configure RTC wakeup period in seconds. (Approximate if ULP is used..)
#define RTC_PERIOD	5


//! Define the CPU frequency, for use with delay_us(). (2 MHz is default clock.)
#ifndef F_CPU
#define F_CPU 2000000
#endif // F_CPU


/*! \brief The main RTC example.
 *
 * This function initializes the XMEGA to the least power consuming state,
 * before initializing the sleep manager and RTC as configured above.\n
 * The main loop doesn't do anything but put the device back to the configured
 * sleep mode after a delay of 0.5 second.
 */
int main(void)
{
    // Initialize to the least power consuming state.
    LOWPOWER_Init();
    
    // Enable EEPROM and Flash power reduction mode.
    CCPWrite(&NVM.CTRLB, NVM_EPRM_bm | NVM_FPRM_bm);

    // Initialize the sleep manager, lock a sleep mode if configured.
    SLEEPMGR_Init();
#ifdef SLEEP_MODE
    SLEEPMGR_Lock( SLEEP_MODE );
#endif // SLEEP_MODE	
	
	
    // If use of the RTC as interval timer is configured, set it up.
#ifdef USE_RTC
    // Clear bit for RTC in PRR (set by LOWPOWER_Init()).
    PR.PRGEN &= ~PR_RTC_bm;

    // Enable external 32 kHz XTAL oscillator in low-power mode for RTC.
    OSC.XOSCCTRL = OSC_XOSCSEL_32KHz_gc | OSC_X32KLPM_bm;
    OSC.CTRL |= OSC_XOSCEN_bm;

    // Wait for oscillator to stabilize.
    do { } while (!( OSC.STATUS & OSC_XOSCRDY_bm ));
    
    // Set the oscillator as clock source for RTC.
    CLK.RTCCTRL = CLK_RTCSRC_TOSC_gc | CLK_RTCEN_bm;

    // Wait until RTC is ready.
    do { } while ( RTC_Busy() );

    // Configure RTC wakeup period.
    RTC_Initialize( RTC_PERIOD, 0, 0, RTC_PRESCALER_DIV1024_gc );
    
    // Enable the RTC compare interrupts so that the device can wake up.
    RTC_SetCompareIntLevel( RTC_COMPINTLVL_LO_gc );
    PMIC.CTRL |= PMIC_LOLVLEN_bm;
    sei();
#endif // USE_RTC

    // Main loop.
    do {
        /* On wake-up, stay in ACTIVE mode for 0.5 s and then go back to sleep.
         *
         * In an actual application, you would process events/data here.
         */
        delay_us(500000);
        
        // Due to errata, Flash power reduction mode should be disabled.
        CCPWrite(&NVM.CTRLB, NVM_EPRM_bm);

        SLEEPMGR_Sleep();
        
        // Re-enable EEPROM and Flash power reduction mode after sleep.
        CCPWrite(&NVM.CTRLB, NVM_EPRM_bm | NVM_FPRM_bm);
    } while (1);
}


/* RTC compare ISR
 *
 * The RTC is only used to wake the device up at intervals, so the ISR
 * does not need to do anything.
 */
ISR(RTC_COMP_vect)
{
}
