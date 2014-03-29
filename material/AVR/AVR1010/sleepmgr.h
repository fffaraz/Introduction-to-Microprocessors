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

#ifndef SLEEPMGR_H
#define SLEEPMGR_H

/*============================ INCLUDES ======================================*/
//#include "compiler.h"
#include "avr_compiler.h"
#include "config_sleepmgr.h"



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


#endif
/* EOF */
