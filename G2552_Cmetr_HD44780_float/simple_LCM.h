/* simple_LCM.h: ������������ ���� ��� ���������� simple_LCM.
 * ������ 1.0
 * 
 *
 * ��� ���������� ������������� 4-������ 6-��������� ���������
 * ��� ������������ ���������� ��-������. ������� ������
 * ������� ������������� ������� ������� � ������������ ������� ����� P1.
 * ��� ������������� ���� ���������� � ������ ������������� �������
 * ���������� ��������� �������� � �����������. ��� ������ ���������
 * ������� ���� 2 ���, ���������� ��������� �������� � _delay_cycles
 * � ����, ��� �� ���������� ������������ ��������.
 * 
 * ��� ������������� ����������, ��������� LCM_init(); � ����� ����.
 * ����� �� ������ ������������ ��� SendByte(<byte>, TRUE);
 * ��� � PrintStr("string"); ��� ������ ������ �� �����.
 * 
 * ������� ���������� �������� SendByte(<byte>, FALSE);
 * 
 * MoveCursor(row,col); ���������� ������ �� ����������� ������ � �������.
 * clear(); ������� �������, � ���������� ������ � ������� 0,0.
 *  
 */

#ifndef SIMPLE_LCM_H_
#define SIMPLE_LCM_H_

#endif /*SIMPLE_LCM_H_*/

#include <msp430.h>

/*  ���������� ��� ���������� ������������ ����������.  */
/*  ��������, ���� ����������.              */
#define LCM_DIR P1DIR
#define LCM_OUT P1OUT

#define RS  BIT0    // ����� RS (Register Select) �� P1.0 (������� ��������� ����������)
#define EN  BIT3    // ����� E (Enable) �� P1.3
#define D4  BIT4    // ������ 4-7 ���� ������ �� P1.4-P1.7 (������� ��������� ���������� �� P1.6)
#define D5  BIT5
#define D6  BIT6
#define D7  BIT7

/*  ���������� (��� ��������)  */
#define MASK    (RS + EN + D4 + D5 + D6 + D7)
#define FALSE 0
#define TRUE 1


void pulse(void);
void SendByte(char, char);
void LCM_init(void);
void clear(void);
void MoveCursor(char, char);
void PrintStr(char *);
