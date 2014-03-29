// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief  XMega low power initialization
 *
 *      This file contains a function for initializing the XMEGA device
 *      to the least power consuming state possible.\n
 *      This is simply done by setting the PRR-bits for all available
 *      peripherals and enabling pullup on all available I/O pins.\n
 *      The function is actually created in lowpower_macros.h according to
 *      which XMEGA device is selected for the project.
 *
 *      \note This function disables the RTC and JTAG interface by default.\n
 *      To leave the JTAG interface enabled, comment out the NO_TAG define in
 *      lowpower.h.\n
 *      If the RTC is to be used, either modify lowpower_macros.h or simply
 *      clear its PRR-bit afterwards.
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
//#include "compiler.h"
#include "avr_compiler.h"
#include "lowpower.h"
#include "lowpower_macros.h"



/*=========================== DEFINITIONS ====================================*/

/*! Perform initialization of XMEGA device
 *
 * This function merely contains macros, which are defined in lowpower_macros.h
 * depending on which XMEGA device the project is compiled for.\n
 * The JTAG interface is disabled if \ref NO_JTAG is defined in lowpower.h.
 */
void LOWPOWER_Init( void )
{
#ifdef NO_JTAG
	DISABLE_JTAG();
#endif // NO_JTAG

	DISABLE_GEN();
	DISABLE_TC();
	DISABLE_COM();
	DISABLE_ANLG();
	ENABLE_PULLUP();
}
