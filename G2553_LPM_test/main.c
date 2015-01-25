/*
*  �������� ��� ����� LPM ������ �� ��������� ������ �����/������.
*  ���������� ����� �������, ������������ � ������� P1.0 � P1.2, �� �����
*  ����������� ��� ������ ����� ������������ �� ������� P1.0 and P1.6,
*  �������� ������� ����������, ��� ���������� �� ��������.
*  ��� �������� ����� �������� ����������� SOUT ��
*  #define SOUT BIT6 � �������� �������, �������� �������� � TACCR0.

*/

#include <msp430g2553.h>

#define SPKR   BIT0
#define SOUT   BIT6

void main(void) {

        WDTCTL = WDTPW + WDTHOLD;       // ��������� ���������� ������

        P1OUT = SPKR;        // ���������� ���� �� ������ � ���������� SPKR,
                             // �� ��� ����� �������� ������� �����������.
        P1DIR = SPKR + SOUT; // ������ ����� �������� ������ � �����.
                             // ����� ��������� ���������, ����������
                             // SOUT � SPKR � ��������������� ���������, ������������.

        BCSCTL1 = CALBC1_1MHZ;          // ������� ��������� ������� 1 ���
        DCOCTL = CALDCO_1MHZ;

        TACCR0 = 64000;     // � ��������� ������� �� 8 (125 kHz), ��� ��������
                          // ��� ������� 125000/(TACCR0+1) ��.
                          // ��� TACCR0 = 144, �������� 862 ��.

        TACCTL0 = CCIE;   // ��������� ���������� ������� �� ���������� �������� CCR0.
        TACTL = TASSEL_2 + ID_3 + MC_1 + TACLR; // ��������� ������ ������ ������� Timer_A:
                                                // TASSEL_2 - �������� ������ SMCLK (SubMainCLocK),
                                                // �� ��������� ����������� �� ������ �� DCO
                                                // ID_3 - �������� ������� �� 8, �� 1MHz ��� ����� 125kHz
                                                // MC_1 - ����� ������� ����� (�� TACCR0)
                                                // TACLR - ��������� ��������� �������
        _BIS_SR(LPM2_bits + GIE);       // ������ � ����� LPM0 � ��������� ����������

} // main


/*  ����������� ����������  */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void) {
                P1OUT ^= SPKR + SOUT;   // ������-������������ ������� �� �������.
} // CCR0_ISR
