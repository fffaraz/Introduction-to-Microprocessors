/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA 32-bit RTC driver source file.
 *
 *      This file contains the function implementations the XMEGA 32-bit RTC
 *      driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead decreases code performance.
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

#include "avr_compiler.h"
#include "rtc32_driver.h"


/*! \brief This function resets the battery backup module and disables the RTC
 *
 *  \note This function should be called before any of the other RTC32 driver
 *  functions.
 */
void RTC32_Reset( void )
{
	/* Enable R/W access to the battery backup module. */
	VBAT.CTRL = VBAT_ACCEN_bm;

    /* Reset the module. (Reset bit is protected by CCP.) */
    ENTER_CRITICAL_REGION();
    CCP = 0xD8;
	VBAT.CTRL = VBAT_RESET_bm;
    LEAVE_CRITICAL_REGION();

	/* The RTC32 module may be enabled after reset. Disable it now. */
	RTC32.CTRL &= ~RTC32_ENABLE_bm;
	do { } while ( RTC32_SyncBusy() );
}

/*! \brief This function starts the 32 kHz crystal oscillator with 1 or 1024 Hz
 *  clock output.
 *
 *  \param use1khz Boolean for selecting 1 kHz or 1 Hz RTC clock rate.
 */
void RTC32_ToscEnable( bool use1khz )
{
	/* Enable 32 kHz XTAL oscillator, with 1 kHz or 1 Hz output. */
	if (use1khz)
		VBAT.CTRL |= ( VBAT_XOSCEN_bm | VBAT_XOSCSEL_bm );
	else
		VBAT.CTRL |= ( VBAT_XOSCEN_bm );

	/* Wait for oscillator to stabilize before returning. */
	do { } while ( RTC32_ToscBusy() );
}


/*! \brief This function initializes the RTC with period, initial count and
 *         compare value.
 *
 *  All the synchronized registers are written at the same time to save time.
 *
 *  \param period         RTC period. Topvalue = Period - 1.
 *  \param count          Initial RTC count.
 *  \param compareValue   Compare value.
 */
void RTC32_Initialize( uint32_t period,
                       uint32_t count,
                       uint32_t compareValue )
{
	/* Disable the RTC32 module before writing to it. Wait for synch. */
	RTC32.CTRL &= ~RTC32_ENABLE_bm;
	do { } while ( RTC32_SyncBusy() );
	
	/* Write PER, COMP and CNT. */
	RTC32.PER = period - 1;
	RTC32.COMP = compareValue;
	RTC32.CNT = count;

	/* Re-enable the RTC32 module, synchronize before returning. */
	RTC32.CTRL |= RTC32_ENABLE_bm;
	do { } while ( RTC32_SyncBusy() );
}


/*! \brief This function sets the RTC overflow interrupt level.
 *
 *  \param intLevel The overflow interrupt level.
 */
void RTC32_SetOverflowIntLevel( RTC32_OVFINTLVL_t intLevel )
{
	RTC32.INTCTRL = ( RTC32.INTCTRL & ~RTC32_OVFINTLVL_gm ) | intLevel;
}


/*! \brief This function sets the RTC compare interrupt level.
 *
 *  \param intLevel The compare interrupt level.
 */
void RTC32_SetCompareIntLevel( RTC32_COMPINTLVL_t intLevel )
{
	RTC32.INTCTRL = ( RTC32.INTCTRL & ~RTC32_COMPINTLVL_gm ) | intLevel;
}


/*! \brief This function sets both compare and overflow interrupt levels in
 *         one go.
 *
 *  \param ovfIntLevel  The overflow interrupt level.
 *  \param compIntLevel The compare interrupt level.
 */
void RTC32_SetIntLevels( RTC32_OVFINTLVL_t ovfIntLevel,
                         RTC32_COMPINTLVL_t compIntLevel )
{
	RTC32.INTCTRL = ( RTC32.INTCTRL &
	                ~( RTC32_COMPINTLVL_gm | RTC32_OVFINTLVL_gm ) ) |
	                ovfIntLevel |
	                compIntLevel;
}

/*! \brief This function sets a timeout alarm.
 *
 *  This function sets a timeout alarm by adding the timeout to the current
 *  count value. If the resulting alarm value is larger than the RTC period
 *  value, it will wrap around. An RTC compare interrupt will be triggered¨
 *  after the specified timeout.
 *
 *  \note The program code needs to check if the RTC is busy before calling
 *        this function. The RTC_Busy() function can be used to do this.
 *
 *  \note The timeout must be less than the timer period. The timeout should not
 *        be set too low, as the timeout may be missed, depending on CPU speed.
 *
 *  \param alarmTimeout Timeout time in RTC clock cycles (scaled).
 */
void RTC32_SetAlarm( uint32_t alarmTimeout )
{
	uint32_t compareValue;
	
	/* Synchronize CNT from RTC to system clock domain. */
	RTC32_SyncCnt();
	do { } while ( RTC32_SyncCntBusy() );
	
	/* Calculate compare time. */
	compareValue = RTC32.CNT + alarmTimeout;

	/* Wrap on period. */
	if (compareValue > RTC32.PER){
		compareValue -= RTC32.PER;
	}

	/* Add the timeout value to get the absolute time of the alarm. */
	RTC32.COMP = compareValue;
}

/*! \brief This function sets a new RTC count value.
 *
 *  This function waits for the RTC32 module to finish synchronizing the CNT
 *  and CTRL registers before writing the new count.\n
 *  The function does not return until the CNT register has synchronized from
 *  the system to RTC clock domain.
 *
 *  \param count The new RTC count value.
 */
void RTC32_SetCount( uint32_t count )
{
	/* Make sure that CNT is not currently synchronizing, or write will fail. */
	do { } while ( RTC32_SyncBusy() );

	/* Write new count value and wait for synchronization before returning. */
	RTC32.CNT = count;
	do { } while ( RTC32_SyncBusy() );
}

/*! \brief This function returns the current RTC count value.
 *
 *  This function synchronizes the RTC32 module's CNT value to the system
 *  clock domain, then returns its value.
 *
 *  \return The current RTC count value.
 */
uint32_t RTC32_GetCount( void )
{
	/* Synchronize the RTC module's CNT value to the system clock domain.  */
	RTC32_SyncCnt();
	do { } while ( RTC32_SyncCntBusy() );
	
	return RTC32.CNT;
}

/*! \brief This function sets a new RTC period.
 *
 *  This function disables the RTC32 module, writes the new period to its PER
 *  register, then re-enables the module.\n
 *
 *  \param period The new RTC period.
 */
void RTC32_SetPeriod( uint32_t period )
{
	/* Disable the RTC32 module before writing to it. Wait for synch. */
	RTC32.CTRL &= ~RTC32_ENABLE_bm;
	do { } while ( RTC32_SyncBusy() );
	
	RTC32.PER = period;
	
	/* Enable the RTC32 module. Wait for synch. */
	RTC32.CTRL |= RTC32_ENABLE_bm;
	do { } while ( RTC32_SyncBusy() );
}
