/* G2553: ���� ������ ������������� ������� ������� ������ Comparator A+
 *  ��������� (P1.0) ������, ����� ����������, �� ���������� ����� CA1 (P1.1),
 *  ���� �������� ���������� �������
 */

#include <msp430g2553.h>

#define RED    	BIT0
#define GREEN	BIT6
#define AIN1    BIT1

/*  ���������� ����������  */
char flash = 0;                         // �������� � �������� �����������

/*  ���������� �������  */

void main(void) {

        WDTCTL = WDTPW + WDTHOLD;       // ��������� ���������� ������

        P1OUT = 0;
        P1DIR = RED + GREEN;                   // enable launchpad red and green leds (p1.0 and p1.6)

CACTL1 = CAREF1 + CARSEL + CAIE; // ������� ���������� 0.5 Vcc,
                                 // �� �������������  �����,
                                 // ���������� ���������,
                                 // �� ������������ ������ (�� ���������)
CACTL2 = P2CA4 + CAF;// ���������������  ���� CA1, ����� �����������
CAPD = AIN1;         // ��������� �������� ���� �� P1.1 (����������
                     // ���� ��� ���������)

        TACCR0 = 60000;   // �������� ������� ��� �������, 60000 ������
        TACCTL0 = CCIE;   // ��������� ���������� ������ �� ���������� CCR0
        TACTL = TASSEL_2 + ID_3 + MC_1 + TACLR; // SMCLK, �������� 8,
                                                // ������ ����, ��������� �������
        CACTL1 |= CAON;           // �������� ����������
        _BIS_SR(LPM0_bits + GIE); // �������� ����� ��������  LPM0 � ��������� ����������

} // main

/*  ����������� ����������  */

#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void) {
        P1OUT ^= flash;                 // if flash == 0, RED LED does not blink
                                        // if flash == LED1, RED LED blink
} // CCR0_ISR

#pragma vector = COMPARATORA_VECTOR
__interrupt void COMPA_ISR(void) {
  if ((CACTL2 & CAOUT)==0x01) {

    flash = RED;       // enable blink
    P1OUT &= ~GREEN;
    CACTL1 |= CAIES;    // value high, so watch for falling edge

  }
  else {

    flash = 0;          // disable blink
    P1OUT = GREEN;          // P1OUT to light GREEN
    CACTL1 &= ~CAIES;   // value low (CAOUT=1) , watch for rising edge
  }
} // COMPA_ISR
