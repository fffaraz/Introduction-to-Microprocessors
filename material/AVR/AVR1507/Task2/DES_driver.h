/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DES driver header file.
 *
 *      This file contains the function prototypes for the XMEGA DES driver.
 *
 *      The low level implementation of the driver is made in two versions,
 *      one optimized for size and one optimized for speed. The driver is
 *      intended for rapid prototyping and documentation purposes for getting
 *      started with the XMEGA DES crypto instruction.
 *
 * \par Application note:
 *      AVR1317 Using the XMEGA built in DES accelerator
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1544 $
 * $Date: 2008-04-21 08:36:12 +0200 (Mon, 21 Apr 2008) $  \n
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
#ifndef DES_DRIVER_H
#define DES_DRIVER_H

#include "avr_compiler.h"


/*! \brief  Function that does a DES decryption on one 64-bit data block.
 *
 *  \param  ciphertext  Pointer to the ciphertext that shall be decrypted.
 *  \param  plaintext   Pointer to where in memory the plaintext (answer) shall be stored.
 *  \param  key         Pointer to the DES key.
 *
 */
void DES_Decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * key);


/*! \brief  Function that does a DES encryption on one 64-bit data block.
 *
 *  \param  plaintext  Pointer to the plaintext that shall be encrypted.
 *  \param  ciphertext Pointer to where in memory the ciphertext (answer) shall be stored.
 *  \param  key        Pointer to the DES key.
 *
 */
void DES_Encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * key);


/*! \brief  Function that does a 3DES encryption on one 64-bit data block.
 *
 *  \param  plaintext   Pointer to the plaintext that shall be encrypted.
 *  \param  ciphertext  Pointer to where in memory the ciphertext (answer) shall be stored.
 *  \param  keys        Pointer to the array of the 3 DES keys.
 *
 */
void DES_3DES_Encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * keys);


/*! \brief  Function that does a 3DES decryption on one 64-bit data block.
 *
 *  \param  ciphertext  Pointer to the ciphertext that shall be decrypted.
 *  \param  plaintext   Pointer to where in memory the plaintext (answer) shall be stored.
 *  \param  keys        Pointer to the array of the 3 DES keys.
 *
 */
void DES_3DES_Decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * keys);


/*! \brief  Function that does DES Cipher Block Chaining encryption on a
 *          given number of 64-bit data block.
 *
 *  \param   plaintext    Pointer to the plaintext that shall be encrypted.
 *  \param   ciphertext   Pointer to where in memory the ciphertext(answer) shall be stored.
 *  \param   keys         Pointer to the array of the one or three DES keys needed.
 *  \param   init         Pointer to initial vector used in in CBC.
 *  \param   triple_DES   Bool that indicate if 3DES or DES shall be used.
 *  \param   block_length Value that tells how many blocks to encrypt.
 *
 */
void DES_CBC_Encrypt(uint8_t * plaintext, uint8_t * ciphertext,
                     uint8_t * keys, uint8_t * init,
                     bool triple_DES, uint16_t block_length);


/*! \brief  Function that does DES Cipher Block Chaining decryption on a
 *          given number of 64-bit data block.
 *
 *  \param   ciphertext    Pointer to the ciphertext that shall be decrypted.
 *  \param   plaintext     Pointer to where in memory the plaintext (answer) shall be stored.
 *  \param   keys          Pointer to the array of the one or three DES keys needed.
 *  \param   init          Pointer to initial vector used in in CBC.
 *  \param   triple_DES    Bool that indicate if 3DES or DES shall be used.
 *  \param   block_length  Value that tells how many blocks to encrypt.
 *
 *  \note    The pointer to the ciphertext and plaintext must not be the same
 *           because the CBC algorithm uses previous values of the ciphertext
 *           to calculate the plaintext.
 */
void DES_CBC_Decrypt(uint8_t * ciphertext, uint8_t * plaintext,
                     uint8_t * keys, uint8_t * init,
                     bool triple_DES, uint16_t block_length);


#endif /* DES_DRIVER_H */

