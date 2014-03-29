// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief  Sleep manager source file.
 *
 *      This file contains the function implementations of the sleep manager.
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
 * $Revision: 2770 $
 * $Date: 2009-09-11 10:55:22 +0200 (fr, 11 sep 2009) $  \n
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

/*============================ INCLUDES ======================================*/
#include <stdint.h>
//#include "assert.h"
//#include "compiler.h"
#include "avr_compiler.h"
#include "sleepmgr.h"
#include "config_sleepmgr.h"



/*============================ PRIVATE VARIABLES AND CONSTANTS ===============*/

// Use macro from "config_sleepmgr.h" to define sleep mode config values.
SLEEPMGR_DEFINE_MODES;
//! Sleep mode lock counters.
SLEEPMGR_lock_t SLEEPMGR_locks[SLEEPMGR_NUM_MODES];



/*============================ IMPLEMENTATION (PUBLIC FUNCTIONS) =============*/

/*!
 * This function initializes the sleep manager. The sleep manager must be
 * initialized before any other modules can use it.
 */
void SLEEPMGR_Init( void )
{
	// Set all mode locks to zero, except the last one, which is handled below.
	for (uint8_t index = 0; index < (SLEEPMGR_NUM_MODES - 1); ++index) {
		SLEEPMGR_locks[index] = 0;
	}
	
	// Lock the deepest sleep mode once and for all, to ease the search
	// implementation in SLEEPMGR_Sleep() later.
	SLEEPMGR_locks[SLEEPMGR_NUM_MODES - 1] = 1;
}


/*!
 * This function increases the lock count for one particular sleep mode. This
 * essentially tells the sleep manager that it should not enter deeper sleep
 * modes than that.
 *
 * It is ok to lock several modes from one module, and locking and unlocking
 * need not be done in any particular order. However, it is important to unlock
 * all modes previously locked.
 *
 * If you want to change your requirements to a deeper sleep mode, it is not
 * sufficient to just lock the deeper mode. You also need to unlock the
 * shallower mode.
 *
 * \param  mode  The sleep mode to lock.
 */
void SLEEPMGR_Lock( SLEEPMGR_mode_t mode )
{
	// The following must be an atomic operation, to avoid race conditions.
	ENTER_CRITICAL_REGION();
	
		// Check that lock has not reached max value for its datatype.
		// By casting "-1L" to the correct datatype, we always get max value.
//		assert( SLEEPMGR_locks[mode] != (SLEEPMGR_mode_t) -1L );
		// Now it's safe to increase the lock counter.
		++SLEEPMGR_locks[mode];
	
	LEAVE_CRITICAL_REGION();
}


/*!
 * This function decreases the lock count for one particular sleep mode. This
 * essentially tells the sleep manager that you are ok with deeper sleep modes,
 * as long as no more locks exist on this or shallower sleep modes.
 *
 * It is ok to lock several modes from one module, and locking and unlocking
 * need not be done in any particular order. However, it is important to unlock
 * all modes previously locked.
 *
 * If you want to change your requirements to a deeper sleep mode, it is not
 * sufficient to just lock the deeper mode. You also need to unlock the
 * shallower mode.
 *
 * \param  mode  The sleep mode to unlock.
 */
void SLEEPMGR_Unlock( SLEEPMGR_mode_t mode )
{
	// The following must be an atomic operation, to avoid race conditions.
	ENTER_CRITICAL_REGION();
	
		// Check that lock is not already zero,
		// which would mean lock/unlock has not been handle correctly.
//		assert( SLEEPMGR_locks[mode] != 0 );
		// Now it's safe to decrease the lock counter.
		--SLEEPMGR_locks[mode];
	
	LEAVE_CRITICAL_REGION();
}


/*!
 * This function computes the deepest sleep mode possible and enters that.
 * If interrupt handlers want to cancel the sleep attempt, it can only be
 * canceled by called SLEEPMGR_CancelSleep().
 */
void SLEEPMGR_Sleep( void )
{
	// The following must be an atomic operation, to avoid race conditions.
	// No need to save the interrupt state, as we must enable global interrupts
	// anyway to be able to wake up from sleep.
	cli();
	
	// Make sure the deepest sleep mode is actually locked, which should have
	// been done in SLEEPMGR_Init at least. This ensures that the search
	// below will succeed.
//	assert( SLEEPMGR_locks[SLEEPMGR_NUM_MODES - 1] > 0 );
	
	// Search from shallowest sleep mode until a non-zero lock is found.
	SLEEPMGR_lock_t const * lockPtr = SLEEPMGR_locks;
	uint8_t PROGMEM_PTR_T modePtr = SLEEPMGR_modes;
	while (*lockPtr == 0) {
		++lockPtr;
		++modePtr;
	}
	
	// Prepare sleep configuration, not touching other fields in register.
	uint8_t modeConfig = PROGMEM_READ_BYTE( modePtr );
	SLEEPMGR_PREPARE_SLEEP( modeConfig );

	// Enable interrupts before sleeping, otherwise we won't wake up.
	sei();

	// Now, enter sleep mode. Any pending interrupts will cause the device
	// to instantaneously wake up.
	cpu_sleep();
	
	// After waking up, we disable sleep.
	SLEEPMGR_DISABLE_SLEEP();
}


/*!
 * This function cancels a sleep attempt, if called from a handler that
 * interrupted the SLEEPMGR_Sleep() function. This function has no effect if
 * called from elsewhere. It is intended to be called from interrupt domain
 * whenever an interrupt causes more work, which means that sleeping is not ok.
 * Normally, this function will be called from within WORKQUEUE_AddWork() and
 * SOFTIRQ_Raise(), which in turn are called from device driver ISRs.
 */
void SLEEPMGR_CancelSleep( void )
{
	// Disable sleep, so that any SLEEP instruction will fail.
	SLEEPMGR_DISABLE_SLEEP();
}


/* EOF */
