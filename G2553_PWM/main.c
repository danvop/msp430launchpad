/* LEDPWM_G2553.c:
 *
 * Demo of pulse width modulation (edge aligned) using LED1 and LED2 on the
 * LaunchPad.
 *
 * Alternatively, external LEDs + current-limiting resistors can be used on
 * TA0.0 (either P1.1 or P1.5) and TA0.1 (either P1.6 or P2.6). If external
 * LEDs are used, the jumper for LED2 should be removed as should the Tx
 * jumper for P1.1 if that pin is used. Whichever of P1.1 and P1.5 is used
 * needs to be jumpered to LED1 on the LaunchPad if that LED is used.
 *
 * No need for a calibrated DCO; uses the default ~1.1 MHz. Optionally,
 * DCO(13,3) is used for ~7.8 MHz by uncommenting the lines in main().
 *
 */

#include <msp430g2553.h>

#define LED1	BIT5	// LED1 is on P1.5
#define LED2	BIT6	// LED2 is on P1.6

/*  Function Definitions  */
void TA_init(void);

void main(void) {
	int dir0=1, dir1=1;		// Direction for PWM duty cycle change. A +1
							// value increases, a -1 value decreases

	WDTCTL = WDTPW + WDTHOLD;

	P1OUT = 0;
	P1DIR = LED1 + LED2;
	P1SEL = LED1 + LED2;	// Set pin modes to TA0.x timer outputs
	TA_init();			// Configure Timer_A for PWM

//	BCSCTL1 = 0x8d;			// RSELx = 13 is 7.8 MHz.
//	DCOCTL = 0x60;			// From datasheet, typical value for DCOx = 3,

	__enable_interrupt();

	while(1) {
		__delay_cycles(327680);	// let PWM run for 5 PWM cycles before changing

	/*  Note that the values 771 and 1285 are specially chosen: they are common
	 *  multiples of 2^16 - 1. These values will let TACCRx reach both 0 and
	 *  65535, letting the PWM extend the full range of the timer's 2^16 bits.
	 *  If you change these, only use values drawn from the factors 3, 5, 17,
	 *  and 257. Change the corresponding starting point in TA_init() as well.
	 */

		TACCR0 += dir0*771;		// Change duty cycle 0 by 1.18%
		TACCR1 += dir1*1285;	// Change duty cycle 1 by 1.96%
		if (TACCR0 == 65535)
			dir0=-1;			// Decrease
		else if (TACCR0 == 771)
			dir0=1;				// Increase
		if (TACCR1 == 65535)
			dir1=-1;			// Decrease
		else if (TACCR1 == 1285)
			dir1=1;				// Increase
	}
} // main


void TA_init(void) {
	TACCR0 = 771;			// LED1 starts at 771/2^16 = 1.18% duty cycle
	TACCR1 = 1285;			// LED2 starts at 1285/2^16 = 1.96% duty cycle
	TACTL = TASSEL_2 + ID_0 + MC_2 + TAIE;	// SMCLK, div 1, Continuous Mode
											// Enable interrupts
	TACCTL0 = OUTMOD_5;		// Reset at TACCR0
	TACCTL1 = OUTMOD_5;		// Reset at TACCR1
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TA0_Rollover_ISR(void) {
	switch(TAIV) {
		case(0xA):
			TACCTL0 = OUTMOD_0 + OUT;	// Set TA0.0 Output High
			TACCTL1 = OUTMOD_0 + OUT;	// Set TA0.1 Output High
			TACCTL0 = OUTMOD_5;		// Return to Reset at TACCR0
			TACCTL1 = OUTMOD_5;		// Return to Reset at TACCR1
			break;
		default:
			break;
	}
}
