#include <msp430g2553.h>

#define grn_led	BIT6
#define red_led BIT0
#define BTN		BIT3


int main(void) {
	unsigned int count;

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
	P1OUT = 0;
	P1DIR = grn_led | red_led; //led pins to out

	P1REN |= BTN; //enable p1.3 pullup
	P1OUT |= BTN; //p1.3 pullup



	while((P1IN & BTN) == BTN);

	while(1){
		while((P1IN & BTN) == BTN){
			P1OUT ^= grn_led;
			for (count = 0; count < 60000; count++);
			for (count = 0; count < 60000; count++);
			for (count = 0; count < 60000; count++);
		}
   }




	return 0;
}
