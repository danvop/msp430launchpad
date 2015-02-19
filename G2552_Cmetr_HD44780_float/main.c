/* CMeterLCMG2553.c: Выводит результативное время измерения
 * ёмкости на 16x2 тектовый ЖК-дисплей.
 *
 * Требуется библиотека simple_LCM.
 */

#include <msp430g2553.h>
#include "simple_LCM.h"

#define VCTL    BIT1
#define AIN1    BIT2

/*  Глобальные переменные  */
unsigned char overflows;
long int time;

/*  Обьявление функций  */
void P1init(void);
void CAinit(void);
void TAinit(void);
void update_display(void);


void main(void) {

    WDTCTL = WDTPW + WDTHOLD;   // Отключаем сторожевой таймер

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;       // Такты, - калиброванный 1 мГц

    LCM_init();
    P1init();

    for(;;) {
        update_display();

        CAinit();
        TAinit();
        _enable_interrupt();    // Экономный режим LPM пока не нужен,
                                // но работающие прерывания нужны.
       // В IAR нужно использовать такое написание
//        __enable_interrupt();
        overflows = 0;      // Сброс счётчика переполнений.
        TACTL |= MC_2;      // Старт таймера в непрерывном режиме,
                            // выход TA0.0 устанавливается по переполнению.
        while (overflows < 30);

        CACTL1 |= CAON;     // Включаем компаратор.
        TACCTL0 = OUTMOD_5 + CCIE; // После следующего переполнения включаем разряд.
        TACCTL1 |= CM_2;    // Захват из TA1 по спадающему фронту сигнала.
        overflows = -1;     // Сброс счётчика переполнений (считаем одно переполнение
                            // и начинаем разряд конденсатора).
        _BIS_SR(LPM0_bits + GIE);
    }
} // main


void P1init(void) {
    P1OUT = 0;
    P1DIR |= VCTL;
    P1SEL = VCTL;    // Назначаем P1.1 на выход от TA0.0, для контроля заряда/разряда
} // P1init

void CAinit(void) {
    CACTL1 = CARSEL + CAREF_1;    // Опорное напряжение 0.25 Vcc,
                                  // на инвертирующем входе.
    CACTL2 = P2CA4 + P2CA0 + CAF; // Вывод CA2 неинвертирующий вход, фильтр на выходе.
    CAPD = AIN1;                  // Отключаем цифровой блок на P1.7 (технически
                                  // этот шаг избыточен).
} // CAinit

void TAinit(void) {
    TACTL = TASSEL_2 + ID_0 + MC_0;     // Используем SMCLK (калиброванные 1 MHz)
                                        // без делителя, таймер остановлен.
    TACCTL0 = OUTMOD_1 + CCIE;          // Выход TA0.0 устанавливается по достижении TACCR0.
    TACCTL1 = CCIS_1 + SCS + CAP + CCIE; // Используем CAOUT для входа,
                                         // синхронный захват, режим захвата
                                         // разрешаем прерывания TA1.
                                         // ВНИМАНИЕ: захват пока не работает.
} // TAinit

void update_display(void) {
    MoveCursor(0,0);
    __delay_cycles(10000);
    SendByte(0x06, FALSE);  // Двигаем курсор вперед
    PrintStr("Capacitance time");

    if (time == 0) {
        MoveCursor(1,5);
        __delay_cycles(10000);
        PrintStr("Ready");
    }
    else {
        MoveCursor(1,15);
        __delay_cycles(10000);
        SendByte(0x04, FALSE);  // Двигаем курсор назад
        SendByte(0x73, TRUE);   // печать 's'
        SendByte(0xE4, TRUE);   // печать '\mu'
        SendByte(0x20, TRUE);   // печать ' '

        while (time > 0) {
            SendByte(0x30 + (time%10), TRUE);   // вывод следующего числа
            time /= 10;
        }
        PrintStr("            ");   // очищаем любой текст, оставшийся с прошлого раза
    }
} // обновление экрана


/*  Обработчики прерываний  */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void) {
    overflows++;    // Прерывание TA0 возникает, когда Timer_A досчитывает до 2^16,
                    // без прерывания от Comparator_A
                    // счётчик TAR сбрасывается в 0, добавляем одно переполнение.
} // TA0_ISR

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TA1_ISR(void) {
    TACCTL1 &= ~(CM_2 + CCIFG); // Останавливаем захват для TA1, обнуляем флаг прерывания.
    TACTL &= ~MC_2;             // Останавливаем Timer_A.
    time = overflows*65536 + TACCR1; // Общее время: overflows * 2^16 + захваченное счётчиком
    __low_power_mode_off_on_exit();  // Продолжаем программу.
} // TA1_ISR
