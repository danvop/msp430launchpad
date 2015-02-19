/* Vmeter G2553: ���������� ������ ��� ADC10 ��� ��������� ����������
 * � ������ ���������� �� ��-����� ����� ���������� simple_LCM.
 */

#include <msp430g2553.h>
#include "simple_LCM.h"

#define A1  BIT1
#define dv  0.0032258

/*  ���������� ����������  */
float volts=0.0;

/*  ���������� �������  */
void ADC_init(void);
void update_display(void);

void main(void) {

    WDTCTL = WDTPW + WDTHOLD;

    LCM_init();
    ADC_init();

    for (;;) {
        ADC10CTL0 |= ADC10SC;   // �������� ����� ��������������
        while ((ADC10CTL1 & ADC10BUSY) == 0x01); // ����, ����� �������������� ����������
        volts=ADC10MEM*dv;  // ������������ ��������� � ���������� � ���������
        update_display();   // ������� �� ��-�����
    }

} // main

void ADC_init(void) {
            // ���������� Vcc/Vss(���������� �����) ��� ��������/������� ���,
            // 16 x ADC10CLKs (������� �� 16 ������), �������� ���.
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;
            // ���� A1, �������� ADC10CLK �� 1, ������������� �����.
    ADC10CTL1 =  INCH_1 + SHS_0 + ADC10SSEL_0 + ADC10DIV_0 + CONSEQ_0;
    ADC10AE0 = A1;      // ��������� ���� ��� �� ����� P1.1

    ADC10CTL0 |= ENC;     // ��������� ��������������.
} // ADC_init

void update_display(void) {
    unsigned int ipart,fpart;

    ipart=(int)volts;   // ����� �����
    fpart=(int)((volts-ipart)*1000);   // ������� �����, 3 �����

    MoveCursor(0,0);
    __delay_cycles(10000);
    SendByte(0x06, FALSE);  // �������� ������ ������.
    PrintStr("Voltmeter:");

    MoveCursor(1,15);
    __delay_cycles(10000);
    SendByte(0x04, FALSE);  // �������� ������ �����.
    SendByte(0x56, TRUE);   // �������� 'V'
    SendByte(0x20, TRUE);   // �������� ' '

    while (fpart > 0) {
        SendByte(0x30 + (fpart%10), TRUE);
        fpart /= 10;
    }
    SendByte(0x2E, TRUE);   // �������� '.'
    SendByte(0x30 + ipart, TRUE);
    PrintStr("            ");

} // ���������� ������
