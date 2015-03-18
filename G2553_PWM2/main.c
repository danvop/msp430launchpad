#/* LEDPWM2_G2553.c:
 *
 * Demo of pulse width modulation (edge aligned) using LED2 on the LaunchPad.
 *
 * Alternatively, an external LED + current-limiting resistor can be used on
 * TA0.1 (either P1.6 or P2.6). If an external LED is used, the jumper for
 * LED2 should be removed.
 *
 * No need for a calibrated DCO; uses the default ~1.1 MHz.
 *
 */

#include <msp430g2553.h>

#define LED2	BIT6	// LED2 is on P1.6
#define PWMTop	500	// Set PWM Frequency: DCO/PWMTop

/*  Function Definitions  */
void TA_init(void);

void main(void) {
	int dir1=1;		// Direction for PWM duty cycle change. A +1
					// value increases, a -1 value decreases

	WDTCTL = WDTPW + WDTHOLD;

	P1OUT = 0;
	P1DIR = LED2;
	P1SEL = LED2;		// Set pin mode to TA0.1 timer output
	TA_init();			// Configure Timer_A for PWM, using up-mode

	while(1) {
		__delay_cycles(PWMTop*5);	// let PWM run for 5 PWM cycles
		TACCR1 += dir1;			// Change duty cycle 1 by 1%
		if (TACCR1 == PWMTop)
			dir1=-1;			// Decrease
		else if (TACCR1 == 0)
			dir1=1;				// Increase
	}
} // main


void TA_init(void) {
	TACCR0 = PWMTop;		// TACCR0 controls the PWM frequency
	TACCR1 = 1;				// LED2 starts at 1% duty cycle for PWMTop = 1000
	TACTL = TASSEL_2 + ID_0 + MC_1;	// SMCLK, div 1, Up Mode
									// No interrupts needed!
	TACCTL1 = OUTMOD_7;		// Reset/Set: Sets at TACCR0, resets at TACCR1
}
