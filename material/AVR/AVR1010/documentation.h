/* Doxygen documentation mainpage ********************************************/
/*! \mainpage
 * \section intro Introduction
 * This documents the firmware for a power consumption and sleep example code.\n
 * \n
 * Note that to get the lowest power consumption it is recommended to: 
 * - Disable the JTAG interface. (Enabled from factory.)
 * - Disable the Watchdog. (Disabled from factory.)
 * - Disable the Brown Out Detection (BOD) circuit. (Disabled from factory.)
 * 
 * These settings are all controlled with the fuses.\n
 * The JTAG interface can also be disabled from software, which is done in these
 * code examples.\n
 * Further, it is important to disable all unused peripherals/modules and define
 * the state of all I/O pins (don't leave them floating).\n
 * Only VCC and GND are assumed to be connected in the generic code examples, so
 * pull-up is enabled on all I/O pins.
 *
 * \note When the JTAG interface is disabled, the PDI interface can still be
 * used for programming and debugging.
 *
 *
 * \section compinfo Compilation Info
 * Most of this firmware compiles with IAR EWAVR 5.30.0 and AVR Studio 4.16 with
 * WinAVR-20090313. The only exceptions are the examples with 32-bit RTC, which
 * do not compile with this version of WinAVR.\n
 * If a new project is created with the supplied code, make sure to:
 * - Set device to the correct XMEGA device.
 * - Set device frequency. (For IAR: define F_CPU in main program file.)
 * - Enable bit definitions. (IAR only)
 * - Use normal DLIB. (IAR only)
 *
 * \note If compiling for different devices in the same directory, it may be
 * necessary to perform a project clean to avoid conflict for compiled modules.
 *
 * 
 * \section deviceinfo Device Info
 * All XMEGA devices can be used with the supplied example codes.
 * Note that there are separate code examples for devices with 32-bit RTC, such
 * as the A3B-variants.\n
 * There is also a code example which is specific for the Xplain board.\n
 *
 *
 * \section contactinfo Contact Info
 * For more info about Atmel AVR visit http://www.atmel.com/products/AVR/ \n
 * For application notes visit
 * http://www.atmel.com/dyn/products/app_notes.asp?family_id=607 \n
 * Support mail: avr@atmel.com \n
 *
 * $Revision: 2770 $
 * $Date: 2009-09-11 10:55:22 +0200 (fr, 11 sep 2009) $
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved. 
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. 
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. The name of Atmel may not be used to endorse or promote products derived 
 * from this software without specific prior written permission.  
 * 
 * 4. This software may only be redistributed and used in connection with an Atmel 
 * AVR product. 
 * 
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE 
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR 
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *****************************************************************************/
