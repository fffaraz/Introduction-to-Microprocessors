// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief CCP (Configuration Change Protection) write helper function.
 *
 *      This file contains the definition of a function for writing to CCP
 *      registers with the correct timing.
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

#include "ccpwrite.h"

#ifdef __cplusplus
extern "C" {
#endif

  
/*! \brief CCP write helper function written in assembly.
 *
 *  This function is written in assembly because of the time-critial
 *  operation of writing to the registers.
 *
 *  \param address Pointer to the register to write to.
 *  \param value   Value to write to the register.
 */
void CCPWrite (volatile uint8_t *address, uint8_t value)
{
    ENTER_CRITICAL_REGION( );
#ifdef __ICCAVR__

    asm("movw r30, r16");
#ifdef RAMPZ
    RAMPZ = 0;
#endif
    asm("ldi  r16,  0xD8 \n"
        "out  0x34, r16  \n"
#if (__MEMORY_MODEL__ == 1)
        "st     Z,  r17  \n");
#elif (__MEMORY_MODEL__ == 2)
        "st     Z,  r18  \n");
#else /* (__MEMORY_MODEL__ == 3) || (__MEMORY_MODEL__ == 5) */
        "st     Z,  r19  \n");
#endif /* __MEMORY_MODEL__ */

#elif defined __GNUC__
    volatile uint8_t * tmpAddr = address;
#ifdef RAMPZ
    RAMPZ = 0;
#endif
    asm volatile(
        "movw r30,  %0"	      "\n\t"
        "ldi  r16,  %2"	      "\n\t"
        "out   %3, r16"	      "\n\t"
        "st     Z,  %1"       "\n\t"
        :
        : "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (&CCP)
        : "r16", "r30", "r31"
        );

#endif
    LEAVE_CRITICAL_REGION( );
}


#ifdef __cplusplus
}
#endif
