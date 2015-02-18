/* G2553_HD44780_demo.c: test example using lcd HD44780
 * ЖК-дисплея выводя на нем простое сообщение.
 *
 *
 */

#include <msp430g2553.h>

#define LCM_DIR P1DIR
#define LCM_OUT P1OUT

#define RS  BIT0
#define EN  BIT3
#define D4  BIT4
#define D5  BIT5
#define D6  BIT6
#define D7  BIT7
#define MASK    (RS + EN + D4 + D5 + D6 + D7)

#define FALSE 0
#define TRUE 1

/*  Глобальные переменные  */
unsigned char overflows;

/*  Объявления функций  */
void LCM_init(void);
void pulse(void);
void clear(void);
void SendByte(char, char);
void MoveCursor(char, char);
void PrintStr(char *);

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;

    P1OUT = 0;
    P1DIR = 0xF;

    TACTL = TASSEL_2 + ID_2 + MC_2; // Use SMCLK, div 4, autoincrement mode
    TACCTL0 = CCIE;          // Прерывание по достижении TACCR0
    overflows = 0;

    __enable_interrupt();

    LCM_init();
    clear();
    while(overflows < 10);

    for(;;) {
        while(overflows < 10);
        overflows = 0;
        MoveCursor(0,2);
        PrintStr("First string");

        while(overflows < 14);
        overflows = 0;
        MoveCursor(1,1);
        __delay_cycles(10000);
        PrintStr("Second "); PrintStr("string0");

        while(overflows <28){
        	__delay_cycles(800000);
        	SendByte(0x1C, FALSE); //move display left
        }

        while(overflows < 30);
        overflows = 0;
        clear();
    }
} // main

void pulse(void) {
    LCM_OUT |= EN;
    __delay_cycles(200);

    LCM_OUT &= ~EN;
    __delay_cycles(200);
} // pulse

void SendByte(char ByteToSend, char IsData) {
    LCM_OUT &= ~MASK;
    LCM_OUT |= (ByteToSend & 0xF0);

    if (IsData == TRUE) LCM_OUT |= RS;
    else LCM_OUT &= ~RS;
    pulse();

    LCM_OUT &= ~MASK;
    LCM_OUT |= ((ByteToSend & 0x0F) << 4);

    if (IsData == TRUE) LCM_OUT |= RS;
    else LCM_OUT &= ~RS;
    pulse();
} // pulse

void LCM_init(void) {
    LCM_DIR |= MASK;			// MASK eq (RS + EN + D4 + D5 + D6 + D7)
    LCM_OUT &= ~MASK;

   __delay_cycles(100000);

   LCM_OUT = 0x20; //BIT5
   pulse();

   SendByte(0x28, FALSE);
   SendByte(0x0C, FALSE);
   SendByte(0x06, FALSE);
} // LCM_init

void clear(void) {
    SendByte(0x01, FALSE);
    SendByte(0x02, FALSE);
    __delay_cycles(100000);
} // clear

void MoveCursor(char Row, char Col) {
    char address;
    if (Row == 0) address = 0;
    else address = 0x40;
    address |= Col;
    SendByte(0x80|address, FALSE);
} // MoveCursor

void PrintStr(char *Text) {
    char *c;
    c = Text;
    while ((c != 0) && (*c != 0)) {
        SendByte(*c, TRUE);
        c++;
    }
} // PrintStr

/*  Обработчики прерываний  */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void) {
    overflows++;    // Считаем переполнения TACCR0.
} // TA0_ISR
