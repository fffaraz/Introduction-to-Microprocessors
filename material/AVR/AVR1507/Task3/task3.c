#include "avr_compiler.h"
#include "AES_driver.h"

#define BLOCK_LENGTH  16
#define BLOCK_COUNT   3

/* Variables containing our data blocks */
uint8_t indata[BLOCK_LENGTH] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
uint8_t cipherdata[BLOCK_LENGTH];
uint8_t    outdata[BLOCK_LENGTH];

/* AES encryption key */
uint8_t aeskey[BLOCK_LENGTH] = {0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
                                0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2};
/* Last subKey (RoundKey) of the AES Key */
uint8_t  lastsubkey[BLOCK_LENGTH];

/* Variable used to check if decrypted answer is equal original data. */
bool success;

int main( void )
{
  	/* Assume that everything is ok*/
  	success = true;

	/* Before using the AES it is recommended to do an AES software reset to put
	 * the module in known state, in case other parts of your code has accessed
	 * the AES module. */
	AES_software_reset();

	/* Generate last subkey, to be used later by decrypt function. */
	AES_lastsubkey_generate(aeskey, lastsubkey);

	/* Do AES encryption and decryption of a single block. */
	success = AES_encrypt(indata, cipherdata, aeskey);
	success = AES_decrypt(cipherdata, outdata, lastsubkey);

	/* Check if decrypted answer is equal to plaintext. */
	for(uint8_t i = 0; i < BLOCK_LENGTH ; i++ ){
		if (indata[i] != outdata[i]){
			success = false;
		}
	}

	if(success){
	    	while(true){
			/* If the example ends up here every thing is ok. */
                        nop();
		}
	}else{
		while(true){
			/* If the example ends up here something is wrong. */
                        nop();
		}
	}
}
