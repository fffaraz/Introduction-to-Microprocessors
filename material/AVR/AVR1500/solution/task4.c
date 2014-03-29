
#include "../avr_compiler.h"
#include "../TC_driver.h"


/*This code use Timer/Counter E0: TCE0 */


#define SWITCHPORT PORTC

/* This function initialize the timer. This is basically doing the same as in task3.
 * expect only CCA is used in this task
 */

void TCE0_init()
{
  
 /* Select a Waveform Genreation mode and the CC channels to use*/

  TC0_ConfigWGM( &TCE0, TC_WGMODE_SS_gc );
  TC0_EnableCCChannels( &TCE0, TC0_CCAEN_bm); // only CCA is used
  TC_SetPeriod( &TCE0, 60000);


  /* The corresponding port pins MUST be output for the Waveform to be visible */
  
  PORTE.DIRSET = 0xFF;
  PORTE.OUTSET = 0xFF; //set port high to switch LEDs off, take INVEN into account
  
  
  /* To have inverted ouput on some pins we set the corresponding INVEN bit for the pin*/
  
  
  TC0_ConfigClockSource(&TCE0, TC_CLKSEL_DIV1_gc);

}

/* This function calculates a new ouput pattern based on an
 * input pattern index
 */

uint8_t GetNewPattern(uint8_t pattern_index)
{
 
  uint8_t new_pattern=0;
  
  switch(pattern_index)
  {
      
    case 0:
      new_pattern = 0x0F;
      break;
    case 1:
      new_pattern = 0x3C;
      break;
    case 2:
      new_pattern = 0xF0;
      break;
    case 3:
      new_pattern = 0xC3;
      break;
  }

  return new_pattern;
}


int main( void )
{

  /* These variables are used to keep track og index 0 to 3) and the pattern to output
   * for the different indexes. */
  
  volatile uint8_t pattern_index=0;
  volatile uint8_t new_pattern=0;
  
    /* To change the duty cycle for the PWM for the LED control, use a variable to increase/decrease the
   * CCx registers once for each period */
  
  volatile int16_t pwm_delta = 300;
  
  /* To use the AWeX it need to be set in the correct mode.
   * For this task we will use the AWEX in Common Waveform Channel Mode (CWCM) and
   * Pattern Generation Mode (PGM)
   */
  
  /*The CWCM mode is enabled by setting the CWCM bit the the AWEX.CTRL register */
  
  //Add code here to enable CWCM
  AWEXE.CTRL |= AWEX_CWCM_bm;

  
  /*The PGM mode is enabled by setting the PGM bit the the AWEX.CTRL register */
  
    //Add code here that enable PGM
  AWEXE.CTRL |= AWEX_PGM_bm;
    

    
  /* Just as for the timer in PWM mode, the ouput channels the AWEX will use must be enabled
   * In this task we will use all channels */
  
  AWEXE.CTRL |= AWEX_DTICCDEN_bm | AWEX_DTICCCEN_bm | AWEX_DTICCBEN_bm | AWEX_DTICCAEN_bm;
  
  /* Just as for TC, the AWEX must set which pins it wish to override
   * (Remember the pin must still be output)
   */
  
  AWEXE.OUTOVEN = 0x0F; //Initial override value.

  TCE0_init();        //init the timer
  
  while(1)
  {
     
    /* Check if switch is pressed */
    
    if((SWITCHPORT.IN & 0x01) == 0)
    {     
      //__delay_cycles(1000000); // About half a second @ default 2 MHz for XMEGA.
	  _delay_ms(500);

      pattern_index++;
      pattern_index = (pattern_index & 0x03);  //we only use 2 bits for the mask 
      
      /* in PGM we use the DTI buffer register to set the pattern. The buffer values is copied
       * into the OUTOVEN register for each UPDATE condition, and this will set a new
       * pattern for the ouput. This ensure that for BLDC motor control for exmaple, the commutation sequence 
       * can be synchronised with the UPDATE condition for the time */
      
      new_pattern = GetNewPattern(pattern_index); 
      
      AWEXE.DTLSBUF =  new_pattern;
    }
     
    /* Check is overflow flag (OVFIF) is set,
     * clear flag and set a new duct cycle
     */
       
     if(TC_GetOverflowFlag(&TCE0) != 0)
     {
          TC_ClearOverflowFlag(&TCE0);      //Clear the IF by writing a logical 1 to the flag
         
          if(TCE0.CCA >= 59000){         //Some "random" values above 0 and below TOP is selected
            pwm_delta = -300;           //for the PWM changes, and make the LED look ok.
          }
          else if(TCE0.CCA <= 5000){
            pwm_delta = +300;
          }
          
          TCE0.CCABUF += pwm_delta;             //Change the compare value to change duty cycle
     }
     
  }

}






