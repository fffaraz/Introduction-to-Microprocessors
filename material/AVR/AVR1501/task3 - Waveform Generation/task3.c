
// Setup for different compilers
#include "../avr_compiler.h"

// Include timer counter driver
#include "../tc_driver.h"

/*This code use Timer/Counter E0: TCE0 */

int main( void )
{

    /* To change the duty cycle for the PWM for the LED control, use a variable to increase/decrease the
    * CCx registers once for each period */

    int16_t pwm_delta = 300;

    // Add code to select a Single Slope PWM as Waveform Generation mode. 
    // This is done by setting the WGM bits,in the CTRLB register for the TC.



    /* Add code to enable the CC channels we wish to use. Each channel must be separately enabled
    * by setting the corresponding CCxEN bits in the CTRLB register for the TC.
    * Code for enabling CCA is already added, but add code to enable CCB, CCC and CCD */

    TCE0.CTRLB |= TC0_CCAEN_bm;

    //Insert code to enable CC for the other channels, B, C and D.




    /* The corresponding port pins MUST be ouput for the Waveform to be visible
    * on the pin. For TCE0 the corresponding port is PORTE, and pin 0 to 3 for
    * CC channel A to D */

    PORTE.DIRSET = 0x0F;

    /* Note how the inverted signal is always controlled from the Port pin configuration in XMEGA
    * This can be used with all other peripherals connected to the pin to have inverted output.
    * Below is example code on how you can set inverted ouput on a pin using the Pin Control Register*/

    //PORTE.PIN0CTRL &= ~PORT_INVEN_bm;

    /* Set a compare value for each compare channel, just as in task2.
    * The compare value decide the PWM duty cycle for the waveform.
    * 
    * Code for CC channel A is added, add code for channel B, C, and D
    * with the compare value 3000 */

    TC_SetCompareA(&TCE0, 3000);

    //Insert function calls to set the other compare values

	

    /* Using the TC_driver we set the Period and 
    * start the timer with no prescaling */

    TC_SetPeriod(&TCE0, 60000);
    TC0_ConfigClockSource(&TCE0, TC_CLKSEL_DIV1_gc);

    while(1)
    {
        /* The code check if the overflow flag is set,
         * if so it clears the flag and sets a new duty cycle for all
         * CC channels */

        /* Check if overflow flag (OVFIF) is set,
         * clear flag and set a new duty cycle
         */
        if(TC_GetOverflowFlag(&TCE0) != 0) 
        {
            TC_ClearOverflowFlag(&TCE0);      //Clear the IF by writing a logical 1 to the flag

            if(TCE0.CCA >= 59000)
            {		                        //Some "random" values above 0 and below TOP is selected
                pwm_delta = -300;            //for the PWM changes, and make the LED look ok.
            }
            else if(TCE0.CCA <= 5000)
            {
                pwm_delta = +300;
            }

            TCE0.CCABUF += pwm_delta;             //Change the compare value to change duty cycle
            TCE0.CCBBUF += pwm_delta;
            TCE0.CCCBUF += pwm_delta;
            TCE0.CCDBUF += pwm_delta;

        }

    }
    }
