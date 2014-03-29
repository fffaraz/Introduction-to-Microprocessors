// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief  Sleep manager header file.
 *
 *      This file contains the function prototypes of the sleep manager.
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
 * $Revision: 5159 $
 * $Date: 2009-06-09 19:05:02 +0200 (ti, 09 jun 2009) $  \n
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

#ifndef SLEEPMGR_H
#define SLEEPMGR_H

/*============================ INCLUDES ======================================*/
#include "avr_compiler.h"

/*============================ DEFINITIONS ===================================*/

//! Sleep mode control register.
#define SLEEPMGR_CTRL_REG SLEEP.CTRL
//! Bitmask for Sleep Enable bit.
#define SLEEPMGR_ENABLE_MASK SLEEP_SEN_bm
//! Bitmask for all sleep mode bits.
#define SLEEPMGR_MODE_MASK SLEEP_SMODE_gm



/*============================ TYPES =========================================*/

//! Type holding sleep mode lock count. This decides the lock count limit.
typedef uint8_t SLEEPMGR_lock_t;

/*! \brief  Sleep mode names.
 *
 * This enumeration holds the identifiers used throughout the application to
 * refer to sleep modes when locking and unlocking. Make sure the contents of
 * this enumeration always correspond to the contents of the SLEEPMGR_modes[]
 * table, which is defined below. Do not touch the last entry SLEEPMGR_NUM_MODES
 * as it is used to indicate the number of sleep modes used in this application.
 */
typedef enum SLEEPMGR_mode_enum
{
	SLEEPMGR_IDLE,
	SLEEPMGR_ESTDBY,
	SLEEPMGR_SAVE,
	SLEEPMGR_STDBY,
	SLEEPMGR_DOWN,
	SLEEPMGR_NUM_MODES //!< Do not change! This equals the sleep mode count.
} SLEEPMGR_mode_t;



/*============================ MACROS ========================================*/

/*! \brief  Sleep mode configuration values.
 *
 * This table is defined as a macro, which in turn is used in the sleepmgr.c
 * file. This way, the table will be local to the sleep manager module.
 * This table contais the sleep mode configuration values for all relevant
 * sleep modes. Only include the sleep modes you intend to use, and make sure
 * the shallowest (most "awake") sleep mode is the first one listed, and the
 * deepest mode the last one listed. Do not include active mode, as that is
 * not regarded as a sleep mode as such. Make sure that the SLEEPMGR_mode_enum
 * is updated accordingly.
 */
#define SLEEPMGR_DEFINE_MODES \
static uint8_t PROGMEM_DECLARE(SLEEPMGR_modes[SLEEPMGR_NUM_MODES]) = \
{ \
	SLEEP_SMODE_IDLE_gc, \
	SLEEP_SMODE_ESTDBY_gc, \
	SLEEP_SMODE_PSAVE_gc, \
	SLEEP_SMODE_STDBY_gc, \
	SLEEP_SMODE_PDOWN_gc \
};


//! Prepare sleep configuration. Used before actually entering sleep mode.
#define SLEEPMGR_PREPARE_SLEEP( sleepMode ) \
{ \
	SLEEPMGR_CTRL_REG = \
		(SLEEPMGR_CTRL_REG & ~SLEEPMGR_MODE_MASK) | \
		(sleepMode) | SLEEPMGR_ENABLE_MASK; \
}

//! Disable sleep, so that subsequent sleep attempts will fail, until reenabled.
#define SLEEPMGR_DISABLE_SLEEP() \
{ \
	SLEEPMGR_CTRL_REG &= ~SLEEPMGR_ENABLE_MASK; \
}


#endif


/*============================ PROTOTYPES ====================================*/
#ifdef __cplusplus
extern "C" {
#endif


//! Initialize sleep manager. Call this before any other manager function.	
void SLEEPMGR_Init( void );
//! Increase lock for a mode, i.e. you need at least this awareness.
void SLEEPMGR_Lock( SLEEPMGR_mode_t mode );
//! Decrease lock for a mode, i.e. you no longer need this awareness.
void SLEEPMGR_Unlock( SLEEPMGR_mode_t mode );
//! Enter the deepest possible sleep mode possible with current lock state.
void SLEEPMGR_Sleep( void );
//! Cancel pending sleep attempt, e.g. when work is added from a device driver.
void SLEEPMGR_CancelSleep( void );


#ifdef __cplusplus
} /* extern "C" */
#endif
/* EOF */
