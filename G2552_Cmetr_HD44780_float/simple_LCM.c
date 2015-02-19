/* simple_LCM.c: Функции библиотеки 4-битного 6-проводного интерфейса
 * для стандартного текстового ЖК-модуля на MSP430.
 * 
 * Смотрите simple_LCM.h для информации по использованию.
 *
 * 
 */

#include "simple_LCM.h"

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
} // SendByte


void LCM_init(void) {
    LCM_DIR |= MASK;
    LCM_OUT &= ~MASK;
    
   __delay_cycles(100000);
   
   LCM_OUT = 0x20;
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
