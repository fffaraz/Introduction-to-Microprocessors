/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA 32-bit RTC driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA 32-bit RTC driver.
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
#ifndef RTC32_DRIVER_H
#define RTC32_DRIVER_H

//#include "compiler.h"
#include "avr_compiler.h"

/* Definitions of macros. */

/*! \brief This macro returns the RTC32 CNT/CTRL write synchronization flag.
 *
 *  \return Non-Zero if RTC32 is busy synchronizing CNT/CTRL from system to
 *  RTC32 clock domain, and zero otherwise.
 */
#define RTC32_SyncBusy()               ( RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm )

/*! \brief This macro returns the XOSC/TOSC ready flag.
 *
 *  \return Non-Zero if TOSC is not ready, and zero otherwise.
 */
#define RTC32_ToscBusy()               !( VBAT.STATUS & VBAT_XOSCRDY_bm )

/*! \brief This macro initiates read synchronization of the RTC32 CNT register
 *
 *  Initiates synchronization of CNT register from RTC to system clock domain.
 */
#define RTC32_SyncCnt()                ( RTC32.SYNCCTRL |= RTC32_SYNCCNT_bm )

/*! \brief This macro returns the RTC32 CNT read synchronization flag.
 *
 *  \return Non-Zero if RTC32 is busy synchronizing CNT from RTC to system
 *  clock domain, zero otherwise.
 */
#define RTC32_SyncCntBusy()            ( RTC32.SYNCCTRL & RTC32_SYNCCNT_bm )

/*! \brief This macro returns the status of the RTC overflow flag.
 *
 *  \return Non-Zero if overflow has occured, zero otherwise.
 */
#define RTC32_GetOverflowFlag()    ( RTC32.INTFLAGS & RTC32_OVFIF_bm )

/*! \brief This macro returns the status of the RTC compare flag.
 *
 *  \return Non-Zero if an RTC compare has occured, zero otherwise.
 */
#define RTC32_GetCompareFlag()     ( RTC32.INTFLAGS & RTC32_COMPIF_bm )

/*! \brief This macro returns the current RTC period value.
 *
 *  This is the period value of the RTC, which is the top of the count sequence.
 *
 *  \return The current RTC period value.
 */
#define RTC32_GetPeriod()          ( RTC32.PER )

/*! \brief This macro sets a new RTC compare value.
 *
 *  This is the compare value of the RTC, which will be compared to the count
 *  sequence. The compare value must be less than the RTC period.
 *
 *  \param _cmpVal   The new compare value.
 */
#define RTC32_SetCompareValue( _cmpVal ) ( RTC32.COMP = (_cmpVal) )

/*! \brief This macro returns the current RTC compare value.
 *
 *  This is the compare value of the RTC, which will be compared to the count
 *  sequence. The compare value must be less than or equal to PER.
 *
 *  \return  The current compare value.
 */
#define RTC32_GetCompareValue()    ( RTC32.COMP )


/* Prototypes of functions. Documentation is found in source file. */
void RTC32_ToscEnable( bool use1khz );
void RTC32_Reset( void );
void RTC32_Initialize( uint32_t period,
                       uint32_t count,
                       uint32_t compareValue );
void RTC32_SetOverflowIntLevel( RTC32_OVFINTLVL_t intLevel );
void RTC32_SetCompareIntLevel( RTC32_COMPINTLVL_t intLevel );
void RTC32_SetIntLevels( RTC32_OVFINTLVL_t ovfIntLevel,
                       RTC32_COMPINTLVL_t compIntLevel );
void RTC32_SetAlarm( uint32_t alarmTimeout );
uint32_t RTC32_GetCount( void );
void RTC32_SetCount( uint32_t count );
void RTC32_SetPeriod( uint32_t period );

/*! This is the interrupt vector declaration. Copy it to your
 *  program code if you want to use it. Leave commented if it's not
 *  used to avoid overhead when compiling.
 *
   ISR(RTC_COMP_vect)
   {
   }

   ISR(RTC_OVF_vect)
   {
   }
 *
 */

#endif
