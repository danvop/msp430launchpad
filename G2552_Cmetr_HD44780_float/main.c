/* CMeterLCMG2553.c: ������� �������������� ����� ���������
 * ������� �� 16x2 �������� ��-�������.
 *
 * ��������� ���������� simple_LCM.
 */

#include <msp430g2553.h>
#include "simple_LCM.h"

#define VCTL    BIT1
#define AIN1    BIT2

/*  ���������� ����������  */
unsigned char overflows;
long int time;

/*  ���������� �������  */
void P1init(void);
void CAinit(void);
void TAinit(void);
void update_display(void);


void main(void) {

    WDTCTL = WDTPW + WDTHOLD;   // ��������� ���������� ������

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;       // �����, - ������������� 1 ���

    LCM_init();
    P1init();

    for(;;) {
        update_display();

        CAinit();
        TAinit();
        _enable_interrupt();    // ��������� ����� LPM ���� �� �����,
                                // �� ���������� ���������� �����.
       // � IAR ����� ������������ ����� ���������
//        __enable_interrupt();
        overflows = 0;      // ����� �������� ������������.
        TACTL |= MC_2;      // ����� ������� � ����������� ������,
                            // ����� TA0.0 ��������������� �� ������������.
        while (overflows < 30);

        CACTL1 |= CAON;     // �������� ����������.
        TACCTL0 = OUTMOD_5 + CCIE; // ����� ���������� ������������ �������� ������.
        TACCTL1 |= CM_2;    // ������ �� TA1 �� ���������� ������ �������.
        overflows = -1;     // ����� �������� ������������ (������� ���� ������������
                            // � �������� ������ ������������).
        _BIS_SR(LPM0_bits + GIE);
    }
} // main


void P1init(void) {
    P1OUT = 0;
    P1DIR |= VCTL;
    P1SEL = VCTL;    // ��������� P1.1 �� ����� �� TA0.0, ��� �������� ������/�������
} // P1init

void CAinit(void) {
    CACTL1 = CARSEL + CAREF_1;    // ������� ���������� 0.25 Vcc,
                                  // �� ������������� �����.
    CACTL2 = P2CA4 + P2CA0 + CAF; // ����� CA2 ��������������� ����, ������ �� ������.
    CAPD = AIN1;                  // ��������� �������� ���� �� P1.7 (����������
                                  // ���� ��� ���������).
} // CAinit

void TAinit(void) {
    TACTL = TASSEL_2 + ID_0 + MC_0;     // ���������� SMCLK (������������� 1 MHz)
                                        // ��� ��������, ������ ����������.
    TACCTL0 = OUTMOD_1 + CCIE;          // ����� TA0.0 ��������������� �� ���������� TACCR0.
    TACCTL1 = CCIS_1 + SCS + CAP + CCIE; // ���������� CAOUT ��� �����,
                                         // ���������� ������, ����� �������
                                         // ��������� ���������� TA1.
                                         // ��������: ������ ���� �� ��������.
} // TAinit

void update_display(void) {
    MoveCursor(0,0);
    __delay_cycles(10000);
    SendByte(0x06, FALSE);  // ������� ������ ������
    PrintStr("Capacitance time");

    if (time == 0) {
        MoveCursor(1,5);
        __delay_cycles(10000);
        PrintStr("Ready");
    }
    else {
        MoveCursor(1,15);
        __delay_cycles(10000);
        SendByte(0x04, FALSE);  // ������� ������ �����
        SendByte(0x73, TRUE);   // ������ 's'
        SendByte(0xE4, TRUE);   // ������ '\mu'
        SendByte(0x20, TRUE);   // ������ ' '

        while (time > 0) {
            SendByte(0x30 + (time%10), TRUE);   // ����� ���������� �����
            time /= 10;
        }
        PrintStr("            ");   // ������� ����� �����, ���������� � �������� ����
    }
} // ���������� ������


/*  ����������� ����������  */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void) {
    overflows++;    // ���������� TA0 ���������, ����� Timer_A ����������� �� 2^16,
                    // ��� ���������� �� Comparator_A
                    // ������� TAR ������������ � 0, ��������� ���� ������������.
} // TA0_ISR

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TA1_ISR(void) {
    TACCTL1 &= ~(CM_2 + CCIFG); // ������������� ������ ��� TA1, �������� ���� ����������.
    TACTL &= ~MC_2;             // ������������� Timer_A.
    time = overflows*65536 + TACCR1; // ����� �����: overflows * 2^16 + ����������� ���������
    __low_power_mode_off_on_exit();  // ���������� ���������.
} // TA1_ISR
