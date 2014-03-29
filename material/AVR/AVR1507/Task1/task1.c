#include "avr_compiler.h"
#include "DES_driver.h"

#define DES_BLOCK_LENGTH  8

/* We define three arrays, in/out data and the encrypted cipher data */
uint8_t     indata[DES_BLOCK_LENGTH] = {'A','V','R','X','M','E','G','A'};
uint8_t cipherdata[DES_BLOCK_LENGTH];
uint8_t    outdata[DES_BLOCK_LENGTH];

/* DES key to be used */  
uint8_t     deskey[DES_BLOCK_LENGTH] = {0x94,0x74,0xB8,0xE8,0xC7,0x3B,0xCA,0x7D};
uint8_t decryptkey[DES_BLOCK_LENGTH] = {0x94,0x74,0xB8,0xE8,0xC7,0x3B,0xCA,0x7D};

int main( void )
{
	bool success = true;

	/* Example of how to use Single DES encryption and decryption functions. */
	DES_Encrypt(indata, cipherdata, deskey);
	DES_Decrypt(cipherdata, outdata, decryptkey);

	/* Check if decrypted answer is equal to plaintext. */
	for (uint8_t i = 0; i < DES_BLOCK_LENGTH ; i++ ){
		if (indata[i] != outdata[i]){
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
