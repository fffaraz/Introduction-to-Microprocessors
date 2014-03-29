/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA power consumption validation main program file
 *
 *      This file contains a simple program for measuring the current
 *      consumption of the XMEGA 128A1 on the Xplain board in different sleep
 *      modes.\n
 *      The device cycles through ACTIVE, IDLE, POWER-SAVE and POWER-DOWN
 *      modes, spending 8 seconds in each mode.\n
 *      Note that the device will stay in POWER-DOWN.
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
#include "sleepmgr.h"
#include "lowpower.h"
#include "rtc_driver.h"

//! Configure RTC wakeup period in seconds. (Approximate if ULP is used..)
#define RTC_PERIOD	8

//! Define the CPU frequency, for use with delay_us(). (2 MHz is default clock.)
#ifndef F_CPU
#define F_CPU 2000000
#endif // F_CPU


//! Global variable to keep track of which state the program is in.
volatile uint8_t gState = 0;


/*! \brief The main RTC example.
 *
 * This function initializes the XMEGA to the least power consuming state,
 * before initializing the sleep manager and RTC.\n
 * The function waits for gState to change (due to RTC compare ISR), after
 * which an infinite loop is entered in which the device is put to sleep.
 */
int main(void)
{
    // Initialize XMEGA to the least power consuming state on the Xplain board.
    LOWPOWER_Init();
    
    // Enable EEPROM and Flash power reduction mode.
    CCPWrite(&NVM.CTRLB, NVM_EPRM_bm | NVM_FPRM_bm);
    
    // Enable pull-down on PORTQ3.
    PORTQ.PIN3CTRL = PORT_OPC_PULLDOWN_gc;
    // Set PORTB1 as input w/ buffer disabled.
    PORTB.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;

    // Initialize the sleep manager.
    SLEEPMGR_Init();

    // Clear bit for RTC in PRR (it is set by LOWPOWER_Init()).
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
    RTC_Initialize( RTC_PERIOD, 0, RTC_PERIOD-1, RTC_PRESCALER_DIV1024_gc );
    
    // Enable RTC compare interrupts.
    RTC_SetCompareIntLevel( RTC_COMPINTLVL_LO_gc );
    PMIC.CTRL |= PMIC_LOLVLEN_bm;
    sei();

    // The device should first spend time in ACTIVE, so wait for
    // the RTC compare ISR to change the state.
    do { } while(gState == 0);
    
    // Disable Flash power reduction mode due to errata.
    // (The device will spend most of its time in sleep from now on, so we
    // won't bother clearing/setting FPRM before and after sleep.)
    CCPWrite(&NVM.CTRLB, NVM_EPRM_bm);
    
    // Go to sleep. The RTC compare ISR configures the sleep modes.
    do {
        SLEEPMGR_Sleep();
    } while (1);
}


/*! RTC compare ISR
 *
 * This ISR simply updates gState so that the device cycles through the
 * different sleep modes.
 */
ISR(RTC_COMP_vect)
{
    switch(gState) {
        // The device starts out in active mode. Go to Idle.
        case 0:
            SLEEPMGR_Lock( SLEEPMGR_IDLE );
            ++gState;
            break;
                
        // Power-save follows after Idle.
        case 1:
            SLEEPMGR_Unlock( SLEEPMGR_IDLE );
            SLEEPMGR_Lock( SLEEPMGR_SAVE );
            ++gState;
            break;
                
        // Power-down follows after Power-save. The device won't wake up again.
        case 2:
            SLEEPMGR_Unlock( SLEEPMGR_SAVE );
            SLEEPMGR_Lock( SLEEPMGR_DOWN );
            ++gState;
            break;
        
        // Shouldn't end up here.. Go to Idle.
        default:
            SLEEPMGR_Init();
            SLEEPMGR_Lock( SLEEPMGR_IDLE );
            gState = 1;
    }
}
