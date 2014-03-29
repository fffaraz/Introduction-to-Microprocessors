#include "avr_compiler.h"
#include "AES_driver.h"

#define BLOCK_LENGTH  16
#define BLOCK_COUNT   3

/* AES encryption key */
uint8_t key[BLOCK_LENGTH] = {0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
                             0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2};
/* i.e. AES decrytion key. */
uint8_t  lastsubkey[BLOCK_LENGTH];

/* Initialisation vector used during Cipher Block Chaining. */
uint8_t init[BLOCK_LENGTH] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                              0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

/* Plain text used during Cipher Block Chaining(contains 3 blocks). */
uint8_t data_block[BLOCK_LENGTH * BLOCK_COUNT] =
                        {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                         0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                         0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                         0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                         0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                         0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};


/* Variable used to store ciphertext from Cipher Block Chaining. */
uint8_t  cipher_block[BLOCK_LENGTH * BLOCK_COUNT];

/* Variable used to store decrypted plaintext from Cipher Block Chaining. */
uint8_t  block_ans[BLOCK_LENGTH * BLOCK_COUNT];

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

	/* Generate last subkey. */
	AES_lastsubkey_generate(key, lastsubkey);

	/* Do AES Cipher Block Chaining encryption and decryption on three blocks. */
	
	success = AES_CBC_encrypt(data_block, cipher_block, key, init,
                                BLOCK_COUNT); /* MODIFY THE FUNCTION CALL */
	success = AES_CBC_decrypt(cipher_block, block_ans, lastsubkey,
                                init, BLOCK_COUNT); /* MODIFY THE FUNCTION CALL */

	/* Check if decrypted answer is equal to plaintext. */
	for(uint8_t i = 0; i < BLOCK_LENGTH * BLOCK_COUNT ; i++ ){
		if (data_block[i] != block_ans[i]){
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
