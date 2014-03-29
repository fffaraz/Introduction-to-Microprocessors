#include "avr_compiler.h"
#include "DES_driver.h"

#define DES_BLOCK_LENGTH  8
#define DES_BLOCK_COUNT   3

/* We define three arrays, in/out data and the encrypted cipher data */
uint8_t        indata[DES_BLOCK_LENGTH] = {"AVRXMEGA"};
uint8_t    cipherdata[DES_BLOCK_LENGTH];
uint8_t       outdata[DES_BLOCK_LENGTH];

/* DES key to be used */  
uint8_t    deskeys[DES_BLOCK_LENGTH * DES_BLOCK_COUNT] =
                  {0x94,0x74,0xB8,0xE8,0xC7,0x3B,0xCA,0x7D,
                   0x28,0x34,0x76,0xAB,0x38,0xCF,0x37,0xC2,
                   0xFE,0x98,0x6C,0x38,0x23,0xFC,0x2D,0x23};

/* Variables for "manual" implementation */
uint8_t  des_stage_1[DES_BLOCK_LENGTH];
uint8_t  des_stage_2[DES_BLOCK_LENGTH];
uint8_t  des_stage_3[DES_BLOCK_LENGTH];

/* Same keys separated in 3 variables */
uint8_t  KEY1[DES_BLOCK_LENGTH] = {0x94,0x74,0xB8,0xE8,0xC7,0x3B,0xCA,0x7D};
uint8_t  KEY2[DES_BLOCK_LENGTH] = {0x28,0x34,0x76,0xAB,0x38,0xCF,0x37,0xC2};
uint8_t  KEY3[DES_BLOCK_LENGTH] = {0xFE,0x98,0x6C,0x38,0x23,0xFC,0x2D,0x23};

int main( void )
{
	bool success = true;

	/* Example of how to use 3DES encryption and decryption functions. */
	DES_3DES_Encrypt(indata, cipherdata, deskeys);
	DES_3DES_Decrypt(cipherdata, outdata, deskeys);

	/* Check if decrypted answer is equal to plaintext. */
	for (uint8_t i = 0; i < DES_BLOCK_LENGTH ; i++ ){
		if (indata[i] != outdata[i]){
			success = false;
			break;
		}
	}

	/* "Manual" 3DES implenemtation */
	/* cipher -> DES_DK3 -> DES_EK2 -> DES_DK1 -> plaintext */

	DES_Decrypt(cipherdata,  des_stage_1, KEY3);
	DES_Encrypt(des_stage_1, des_stage_2, KEY2);
	DES_Decrypt(des_stage_2, des_stage_3, KEY1);


	/* Check if this "manual" decrypt is equal to indata */
	for (uint8_t i = 0; i < DES_BLOCK_LENGTH ; i++ ){
		if (indata[i] != des_stage_3[i]){
			success = false;
			break;
		}
	}

	if (success){
	  	while (true){
		/* If the example ends up here everything is ok. */
             nop();
		}
	}else{
		while (true){
		/* If the example ends up here something is wrong. */
             nop();
		}
	}
}
