/* G2553: Этот пример демонстрирует базовые функции модуля Comparator A+
 *  Светодиод (P1.0) мигает, когда напряжение, на аналоговом входе CA1 (P1.1),
 *  ниже половины напряжения питания
 */

#include <msp430g2553.h>

#define RED    	BIT0
#define GREEN	BIT6
#define AIN1    BIT1

/*  Глобальные переменные  */
char flash = 0;                         // начинаем с потухшим светодиодом

/*  Объявление функций  */

void main(void) {

        WDTCTL = WDTPW + WDTHOLD;       // отключаем сторожевой таймер

        P1OUT = 0;
        P1DIR = RED + GREEN;                   // enable launchpad red and green leds (p1.0 and p1.6)

CACTL1 = CAREF1 + CARSEL + CAIE; // опорное напряжение 0.5 Vcc,
                                 // на инвертирующем  входе,
                                 // прерывание разрешено,
                                 // по нарастающему фронту (по умолчанию)
CACTL2 = P2CA4 + CAF;// неинвертирующий  вход CA1, выход фильтруется
CAPD = AIN1;         // отключаем цифровой блок на P1.1 (технически
                     // этот шаг избыточен)

        TACCR0 = 60000;   // Задержка таймера для мигания, 60000 циклов
        TACCTL0 = CCIE;   // разрешаем прерывание таймер по достижении CCR0
        TACTL = TASSEL_2 + ID_3 + MC_1 + TACLR; // SMCLK, делитель 8,
                                                // прямой счёт, обнуление таймера
        CACTL1 |= CAON;           // включаем компаратор
        _BIS_SR(LPM0_bits + GIE); // включаем режим экономии  LPM0 и разрешаем прерывания

} // main

/*  Обработчики прерываний  */

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
