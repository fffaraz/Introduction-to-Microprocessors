//******************************************************************************
//*
//* XMEGA DES driver assembly source file optimized for size.
//*
//* This file contains the low-level implementations for the
//* XMEGA DES driver optimized for size. It is written for the GCC Assembler.
//*
//* Note on GCC calling convention:
//*      Scratch registers:   R0-R1, R18-R27, R30-R31
//*      Preserved registers: R2-R17, R28-R29
//*      Parameter registers: R25-R8 (2-byte alignment starting from R25)
//*      Return registers:    R25-R8 (2-byte alignment starting from R25)
//*
//* Application note:
//*      AVR1317: Using the XMEGA built in DES accelerator
//*
//* Documentation
//*      For comprehensive code documentation, supported compilers, compiler
//*      settings and supported devices see readme.html
//*
//*      Atmel Corporation: http://www.atmel.com \n
//*      Support email: avr@atmel.com
//*
//* $Revision: 1300 $
//* $Date: 2008-02-25 10:34:09 +0100 (ma, 25 feb 2008) $
//*
//* Copyright (c) 2008, Atmel Corporation All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without
//* modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice,
//* this list of conditions and the following disclaimer.
//*
//* 2. Redistributions in binary form must reproduce the above copyright notice,
//* this list of conditions and the following disclaimer in the documentation
//* and/or other materials provided with the distribution.
//*
//* 3. The name of ATMEL may not be used to endorse or promote products derived
//* from this software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
//* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
//* SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
//* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//******************************************************************************

/* Define 22 bit Program Counter if the device have 22 bits PC.
 * Comment out if it only have 16 bit PC.
 */
#define __22_BIT_PC__


// ----------
// This routine does a single DES encryption.
//
// Prototype:
//    void DES_Encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * key);
//
// Input:
//    - R25:R24 - pointer to plaintext buffer.
//    - R23:R22 - pointer to ciphertext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_Encrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_Encrypt
DES_Encrypt:
	rcall	DES_INTERNAL_Prolog
	rcall	DES_INTERNAL_Load_Data

	// Clear R16 register to tell the DES_INTERNAL_DES_Encrypt to do single DES. 
	clr	r16

	rcall	DES_INTERNAL_DES_Encrypt
	rcall	DES_INTERNAL_Store_Data
	rjmp	DES_INTERNAL_Epilog


// ----------
// This routine does a single DES decryption.
//
// Prototype:
//    void DES_Decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * key);
//
// Input:
//    - R25:R24 - pointer to ciphertext buffer.
//    - R23:R22 - pointer to plaintext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_Decrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_Decrypt
DES_Decrypt:
	rcall	DES_INTERNAL_Prolog
	rcall	DES_INTERNAL_Load_Data

	// Clear R16 register to tell the DES_INTERNAL_DES_Decrypt to do single DES. 
	clr	r16

	rcall	DES_INTERNAL_DES_Decrypt
	rcall	DES_INTERNAL_Store_Data
	rjmp 	DES_INTERNAL_Epilog


// ----------
// This routine does a triple DES encryption.
//
// Prototype:
//    void DES_3DES_Encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * keys);
//
// Input:
//    - R25:R24 - pointer to plaintext buffer.
//    - R23:R22 - pointer to ciphertext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_3DES_Encrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_3DES_Encrypt
DES_3DES_Encrypt:
	rcall	DES_INTERNAL_Prolog
	rcall	DES_INTERNAL_Load_Data

	// Set R16 register to non-zero to tell the DES_INTERNAL_DES_Encrypt to do triple DES. 
	ser	r16

	rcall	DES_INTERNAL_DES_Encrypt
	rcall	DES_INTERNAL_Store_Data
	rjmp	DES_INTERNAL_Epilog


// ----------
// This routine does a triple DES decryption.
//
// Prototype:
//    void DES_3DES_Decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * keys);
//
// Input:
//    - R25:R24 - pointer to ciphertext buffer.
//    - R23:R22 - pointer to plaintext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_3DES_Decrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_3DES_Decrypt
DES_3DES_Decrypt:
	rcall	DES_INTERNAL_Prolog
	rcall	DES_INTERNAL_Load_Data
	
	// Set R16 register to non-zero to tell the DES_INTERNAL_DES_Decrypt to do triple DES.
	ser	r16

	rcall	DES_INTERNAL_DES_Decrypt
	rcall	DES_INTERNAL_Store_Data
	rjmp	DES_INTERNAL_Epilog


// ----------
// This routine does cipher block chaining encoding using DES.
// The bool triple_DES decide if single DES or triple DES is used.
// The variable block_length decide the number of blocks encoded.
//
// Prototype:
//    void DES_CBC_Encrypt(uint8_t * plaintext, uint8_t * ciphertext,
//                         uint8_t * keys, uint8_t * init,
//                         bool triple_DES, uint16_t block_length);
//
// Input:
//    - R25:R24 - pointer to plaintext buffer.
//    - R23:R22 - pointer to ciphertext buffer.
//    - R21:R20 - pointer to key buffer.
//    - R19:R18 - pointer to initial vector (IV).
//    - R17:R16 - variable holding triple_DES bool.
//    - R15:R14 - variable holding block_length.
//
// Register usage during DES_CBC_Encrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R27:R26 (X) holding block_length variable (moved from R15:R14).
//   - R25:R24 points to the current position in the input buffer (plaintext)
//   - R23:R22 points to the current position in the output buffer (ciphertext)
//   - R21:R20 points to the key buffer
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_CBC_Encrypt
DES_CBC_Encrypt:
	rcall	DES_INTERNAL_Prolog

	// Move R15:R14 to R27:R26 to save the block_length during DES.
	movw	r26, r14

	rcall	DES_INTERNAL_Load_Data

	// Load the pointer to IV into the Z pointer, and load the IV to R15 - R8
	// to allow for XORing between the data and IV.
	movw	r30, r18
	rcall	DES_INTERNAL_Load_Into_R15_R8

DES_INTERNAL_CBC_Encrypt_Next:
	rcall	DES_INTERNAL_XOR_Routine
	rcall	DES_INTERNAL_DES_Encrypt
	rcall	DES_INTERNAL_Store_Data

	// Subtract one block from the counter for each pass and go to end if zero.
	sbiw	r26, 1
	breq	DES_INTERNAL_CBC_Encrypt_End

	// Load the pointer to data into the Z pointer, and load the data to R15 - R8
	// to allow for XORing between the last cipherblock and data.
	movw	r30, r24
	rcall	DES_INTERNAL_Load_Into_R15_R8
	movw	r24, r30	

	rjmp	DES_INTERNAL_CBC_Encrypt_Next

DES_INTERNAL_CBC_Encrypt_End:
	rjmp	DES_INTERNAL_Epilog


// ----------
// This routine does cipher block chaining decoding using DES.
// The bool triple_DES decide if single DES or triple DES is used.
// The variable block_length decide the number of blocks encoded.
//
// Prototype:
//    void DES_CBC_Decrypt(uint8_t * ciphertext, uint8_t * plaintext,
//                         uint8_t * keys, uint8_t * init,
//                         bool triple_DES, uint16_t block_length);
//
// Input:
//    - R25:R24 - pointer to ciphertext buffer.
//    - R23:R22 - pointer to plaintext buffer.
//    - R21:R20 - pointer to key buffer.
//    - R19:R18 - pointer to initial vector (IV).
//    - R17:R16 - variable holding triple_DES bool.
//    - R15:R14 - variable holding block_length.
//
// Register usage during DES_CBC_Decrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R27:R26 (X) holding block_length variable (moved from R15:R14).
//   - R25:R24 points to the current position in the input buffer (ciphertext)
//   - R23:R22 points to the current position in the output buffer (plaintext)
//   - R21:R20 points to the key buffer
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_CBC_Decrypt
DES_CBC_Decrypt:
	rcall	DES_INTERNAL_Prolog

	// Move R15:R14 to R27:26 to save the block_count during DES.
	movw	r26, r14

	// Load data and decrypt.
	rcall	DES_INTERNAL_Load_Data
	rcall	DES_INTERNAL_DES_Decrypt

	// Load the initial vector into Z pointer and do the first XORing with the IV.
	movw	r30, r18

DES_INTERNAL_CBC_Decrypt_Next:
	rcall	DES_INTERNAL_Load_Into_R15_R8
	rcall	DES_INTERNAL_XOR_Routine
	rcall	DES_INTERNAL_Store_Data

	// Subtract one block from the counter for each pass and go to end if zero.
	sbiw	r26, 1
	breq	DES_INTERNAL_CBC_Decrypt_End

	// Load the next cipher block into R7-R0 and decrypt.
	// The pointer to the ciphers are updated on each load.
	rcall	DES_INTERNAL_Load_Data
	rcall	DES_INTERNAL_DES_Decrypt

	// Move the updated cipher block pointer to Z pointer, and subtract the pointer
	// by 16 to get the previous cipher block to XOR with the decrypted data.
	movw	r30, r24
	sbiw	r30, 16
	rjmp	DES_INTERNAL_CBC_Decrypt_Next

DES_INTERNAL_CBC_Decrypt_End:
	rjmp	DES_INTERNAL_Epilog


// ----------
// This routine is called by several other routines, and contains common code
// for executing single/triple DES encryption.
//
// Note: The data need to be preloaded and R16 must be set to triple or single DES.
//       This function only load the keys and do the encryption.
//
// Input:
//    - R21:R20 - pointer to key buffer.
//    - R16 - if non-zero, triple DES is performed.
//
// Register usage during DES_Encryption:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
DES_INTERNAL_DES_Encrypt:
	// Load pointer to first key into Z pointer and store the key to R15-R8
	movw	r30, r20
	rcall	DES_INTERNAL_Load_Into_R15_R8

	clh
	rcall	DES_INTERNAL_DES_Routine

	// Test if register is zero, and go to end if single encryption only.
	tst	r16
	breq	DES_INTERNAL_DES_Single_Encrypt

	// Load pointer to second key into Z pointer and store the key to R15-R8
	movw	r30, r20
	adiw	r30, 8
	rcall	DES_INTERNAL_Load_Into_R15_R8

	seh
	rcall	DES_INTERNAL_DES_Routine

	// Load pointer to third key into Z pointer and store the key to R15-R8
	movw	r30, r20
	adiw	r30, 16
	rcall	DES_INTERNAL_Load_Into_R15_R8

	clh
	rcall	DES_INTERNAL_DES_Routine

DES_INTERNAL_DES_Single_Encrypt:
	ret


// ----------
// This routine is called by several other routines, and contains common code
// for executing single/triple DES decryption.
//
// Note: The data need to be preloaded and R16 must be set to triple or single DES.
//       This function only load the keys and do the decryption.
//
// Input:
//    - R21:R20 - pointer to key buffer.
//    - R16 - if non-zero, triple DES is performed.
//
// Register usage during DES_Decryption:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
DES_INTERNAL_DES_Decrypt:
	
	// Test if register is zero. Go to single decryption if zero.
	tst	r16
	breq	DES_INTERNAL_DES_Single_Decrypt

	// Load pointer to third key into Z pointer and store the key to R15-R8
	movw	r30, r20
	adiw	r30, 16
	rcall	DES_INTERNAL_Load_Into_R15_R8

	seh
	rcall	DES_INTERNAL_DES_Routine

	// Load pointer to second key into Z pointer and store the key to R15-R8
	movw	r30, r20
	adiw	r30, 8
	rcall	DES_INTERNAL_Load_Into_R15_R8

	clh
	rcall	DES_INTERNAL_DES_Routine

DES_INTERNAL_DES_Single_Decrypt:

	// Load pointer to first key into Z pointer and store the key to R15-R8
	movw	r30, r20
	rcall	DES_INTERNAL_Load_Into_R15_R8

	seh
	rcall	DES_INTERNAL_DES_Routine

	ret


// ----------
// This routine is called by several other routines, and contains common code
// for loading data to R7-R0.
//
// Input:
//    - R25:R24 - pointer to input buffer.
//
// Register usage during DES_INTERNAL_Load_Data:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R7 - R0 contains the data (plaintext or ciphertext)
//
// Returns:
//     R25:R24 - updated input pointer.
// ----------
DES_INTERNAL_Load_Data:
	movw	r30, r24
	ld	r7, Z+
	ld	r6, Z+
	ld	r5, Z+
	ld	r4, Z+
	ld	r3, Z+
	ld	r2, Z+
	ld	r1, Z+
	ld	r0, Z+
	movw	r24, r30
	ret


// ----------
// This routine is called by several other routines, and contains common code
// for loading data to R15-R8.
//
// Input:
//    - R31:R30 - pointer to data buffer.
//
// Register usage during DES_INTERNAL_Load_Into_R15_R8:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R15 - R8 contains the data (plaintext or ciphertext)
// ----------
DES_INTERNAL_Load_Into_R15_R8:
	ld	r15, Z+
	ld	r14, Z+
	ld	r13, Z+
	ld	r12, Z+
	ld	r11, Z+
	ld	r10, Z+
	ld	r9,  Z+
	ld	r8,  Z+
	ret


// ----------
// This routine is called by several other routines, and contains common code
// to XOR two 64 bits values.
//
// Input:
//     R15 - R8 - 64 bits value.
//     R7  - R0 - 64 bits value.
//
// Returns:
//     R7  - R0 - 64 bits xored value.
// ----------
DES_INTERNAL_XOR_Routine:
	eor	r7, r15
	eor	r6, r14
	eor	r5, r13
	eor	r4, r12
	eor	r3, r11
	eor	r2, r10
	eor	r1,  r9
	eor	r0,  r8
	ret		


// ----------
// This routine is called by several other routines, and contains common code
// for storing data located in register 7 to 0 to memory pointed to by Z.
//
// Input:
//     R23:R22 - pointer to ouput memory.
//
// Registers used during execution:
//     R31:R30 - Z pointer is used, and not preserved.
//
// Returns:
//     R23:R22 - updated output pointer.
// ----------
DES_INTERNAL_Store_Data:
	movw	r30, r22
	st	Z+,  r7
	st	Z+,  r6
	st	Z+,  r5
	st	Z+,  r4
	st	Z+,  r3
	st	Z+,  r2
	st	Z+,  r1
	st	Z+,  r0
	movw	r22, r30
	ret


// ----------
// This routine is called by several other routines, and contains common code
// doing the DES encryption/decryption.
//
// Input:
//     R15 - R8 - 64 bits key.
//     R7  - R0 - 64 bits plaintext/ciphertext.
//
// Returns:
//     R7 -  R0 - 64 bits ciphertext/plaintext.
//     R8 - R15 - 64 bits key.
// ----------
DES_INTERNAL_DES_Routine:
	des	0
	des	1
	des	2
	des	3
	des	4
	des	5
	des	6
	des	7
	des	8
	des	9
	des	10
	des	11
	des	12
	des	13
	des	14
	des	15
	ret


// ----------
// This routine is called by several other routines, and contains common code
// for preserving register defined in GCC.
//
// Note: The function call pushes the Program Counter on the stack, and this
//       must be popped out and placed after the preserved data on the stack.
//
// Note: If the Program Counter is 22-bits, 3 bytes has to be popped and pushed.
// ----------
DES_INTERNAL_Prolog:
#ifdef __22_BIT_PC__
	pop	r27
#endif
	pop	r30
	pop	r31

	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r16
	push	r17
	push	r28
	push	r29

	push	r31
	push	r30
#ifdef __22_BIT_PC__
	push	r27
#endif
	ret


// ----------
// This routine is referenced by several other routines, and contains common code
// for restoring register defined in GCC.
//
// Note: This is the last routine to perform in the other routines and should not
//       be called and a jump to this routine should be performed instead.
//       If a function call is done, this will push the Program Counter on the stack,
//       and this must be popped out and placed after the preserved data on the stack.
//
// Note: If the Program Counter is 22-bits, 3 bytes has to be popped and pushed.
// ----------
DES_INTERNAL_Epilog:
	pop	r29
	pop	r28
	pop	r17
	pop	r16
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	ret
