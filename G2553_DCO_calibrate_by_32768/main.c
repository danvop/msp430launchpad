/* DCOcalibrate.c: Uses the watch crystal to identify specific configurations
 * of the DCO for any given desired frequency. The nearest integer to the
 * division of the desired frequency with 4096 is passed to Set_DCO(), and the
 * results are stored in the array CAL_DATA[]. The examples below give 8 and
 * 12 MHz, as well as the standard UART value 7.3728 MHz.
 *
 * The LED on P1.0 flashed to indicate when the calibration values have been
 * found.
 *
 * Based on code from:
    //  A. Dannenberg
    //  Texas Instruments Inc.
    //  May 2007
 */

#include <msp430g2553.h>    // change this to the particular device

#define DELTA_UART    1800                  // 1800 x 4096Hz = 7.3728MHz
#define DELTA_8MHZ    1953                  // 1953 x 4096Hz = 7.99MHz
#define DELTA_12MHZ   2930                  // 2930 x 4096Hz = 12.00MHz
#define DELTA_1MHZ   244                  // 244 x 4096Hz = 0.999MHz
#define DELTA_16MHZ   3906                  // 3906 x 4096Hz = 15.998MHz

unsigned char DCO_VALS[3];  // DCOCTL settings
unsigned char BC1_VALS[3];  // BCSCTL1 settings
volatile unsigned int i;
unsigned char j;


/*  Function Definitions  */
void Set_DCO(unsigned int Delta);

void main(void) {

    WDTCTL = WDTPW + WDTHOLD;

    BCSCTL3 |= XCAP_3;      // Launchpad watch crystals need ~12.5 pF
    for (i = 0; i < 0xffff; i++);  // Delay for XTAL stabilization

    P1OUT = 0;
    P1SEL = BIT4;           // P1.4 SMCLK output, if you want to see it on
                            // an oscilloscope.
    P1DIR = BIT0 + BIT4;    // P1.0,4 output

    j = 0;                  // Reset index

    Set_DCO(DELTA_8MHZ);    // Find DCO for 8 MHz
    DCO_VALS[j] = DCOCTL;
    BC1_VALS[j] = BCSCTL1;
    j++;

    Set_DCO(DELTA_12MHZ);   // Find DCO for 12 MHz
    DCO_VALS[j] = DCOCTL;
    BC1_VALS[j] = BCSCTL1;
    j++;

    Set_DCO(DELTA_1MHZ);    // Find DCO for 7.3728 MHz
    DCO_VALS[j] = DCOCTL;
    BC1_VALS[j] = BCSCTL1;


  for (;;) {
    P1OUT ^= BIT0;                  // Toggle LED
    for (i = 0; i < 0xffff; i++);
  }
} // main

void Set_DCO(unsigned int Delta) {      // Set DCO to selected frequency
    unsigned int Compare, Oldcapture = 0;

    BCSCTL1 |= DIVA_3;                  // ACLK = LFXT1CLK/8
    TACCTL0 = CM_1 + CCIS_1 + CAP;      // CAP, ACLK
    TACTL = TASSEL_2 + MC_2 + TACLR;    // SMCLK, cont-mode, clear

    while (1) {
        while (!(CCIFG & TACCTL0));     // Wait until capture occured
        TACCTL0 &= ~CCIFG;              // Capture occured, clear flag
        Compare = TACCR0;               // Get current captured SMCLK
        Compare = Compare - Oldcapture; // SMCLK difference
        Oldcapture = TACCR0;            // Save current captured SMCLK

        if (Delta == Compare)
            break;                      // If equal, leave "while(1)"
        else if (Delta < Compare) {
            DCOCTL--;                   // DCO is too fast, slow it down
            if (DCOCTL == 0xFF)         // Did DCO roll under?
                if (BCSCTL1 & 0x0f)
                    BCSCTL1--;          // Select lower RSEL
        }
        else {
            DCOCTL++;                   // DCO is too slow, speed it up
            if (DCOCTL == 0x00)         // Did DCO roll over?
                if ((BCSCTL1 & 0x0f) != 0x0f)
                    BCSCTL1++;          // Sel higher RSEL
        }
    }
    TACCTL0 = 0;                        // Stop TACCR0
    TACTL = 0;                          // Stop Timer_A
    BCSCTL1 &= ~DIVA_3;                 // ACLK = LFXT1CLK
} // Set_DCO
