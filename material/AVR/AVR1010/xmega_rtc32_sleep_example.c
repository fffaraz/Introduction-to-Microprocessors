/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA sleep manager example main program file for 32-bit RTC
 *
 *      This file contains an example program which initializes the XMEGA
 *      to the least power consuming state, and puts the device to sleep.\n
 *      By default, the XMEGA is put in POWER-DOWN mode, and wakes up every
 *      five seconds. The device stays in ACTIVE mode for 0.5 sec before going
 *      back to sleep. This gives a duty cycle of 10%.
 *
 *      The interval timing is done with the 32-bit RTC, clocked by the external 
 *		32 kHz crystal oscillator.
 *
 *      \note The device is initialized with the LOWPOWER_Init(), which enables
 *      pull-up on all I/O pins and disables the JTAG-interface (if configured
 *      in lowpower.h). This results in the lowest current consumption if
 *      nothing is connected to the I/O pins.
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
#include "rtc32_driver.h"
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
 * \note The device can wake itself up from Power-down due to the battery
 * backup module leaving the RTC32 running in all sleep modes.
 */
#define SLEEP_MODE SLEEPMGR_DOWN


/*! Configure if RTC should be used. (Comment out otherwise.)
 *
 * This wakes the device up at intervals with the specified number of seconds.
 *
 * \note The RTC is disabled in Standby and Power-down!
 */
#define USE_RTC
//! Configure RTC wakeup period in seconds.
#define RTC_PERIOD	5


//! Define the CPU frequency, for use with delay_us(). (2 MHz is default clock.)
#ifndef F_CPU
#define F_CPU 2000000
#endif // F_CPU


/*! \brief The main RTC example.
 *
 * This function initializes the XMEGA to the least power consuming state,
 * before initializing the sleep manager and RTC32 as configured above.\n
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
    // Clear bit for RTC in PRR (it is set by LOWPOWER_Init()).
    PR.PRGEN &= ~PR_RTC_bm;
    
    // Reset the battery backup module.
    RTC32_Reset();

    // Configure and enable TOSC, then set up and enable the RTC32 module.
    RTC32_ToscEnable( false );
    RTC32_Initialize( RTC_PERIOD, 0, 0 );

    // Enable RTC compare interrupts.
    RTC32_SetCompareIntLevel( RTC32_COMPINTLVL_LO_gc );
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
        
        // Due to errata, disable Flash power reduction before sleep.
        CCPWrite(&NVM.CTRLB, NVM_EPRM_bm);

        SLEEPMGR_Sleep();
        
        // Re-enable Flash power reduction mode after sleep.
        CCPWrite(&NVM.CTRLB, NVM_EPRM_bm | NVM_FPRM_bm);
    } while (1);
}


/* RTC32 compare ISR
 *
 * The RTC is only used to wake the device up at intervals, so the ISR
 * does not need to do anything.
 */
ISR(RTC32_COMP_vect)
{
}
