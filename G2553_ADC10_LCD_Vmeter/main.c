/* Vmeter G2553: Использует модуль АЦП ADC10 для измерения напряжения
 * и вывода результата на ЖК-экран через библиотеку simple_LCM.
 */

#include <msp430g2553.h>
#include "simple_LCM.h"

#define A1  BIT1
#define dv  0.0032258

/*  Глобальные переменные  */
float volts=0.0;

/*  Обьявление функций  */
void ADC_init(void);
void update_display(void);

void main(void) {

    WDTCTL = WDTPW + WDTHOLD;

    LCM_init();
    ADC_init();

    for (;;) {
        ADC10CTL0 |= ADC10SC;   // начинаем новое преобразование
        while ((ADC10CTL1 & ADC10BUSY) == 0x01); // ждем, когда преобразование закончится
        volts=ADC10MEM*dv;  // конвертируем результат в напряжение и сохраняем
        update_display();   // выводим на ЖК-экран
    }

} // main

void ADC_init(void) {
            // Используем Vcc/Vss(аналоговая земля) для верхнего/нижнего ИОН,
            // 16 x ADC10CLKs (выборка за 16 тактов), включаем АЦП.
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;
            // Вход A1, делитель ADC10CLK на 1, одноканальный режим.
    ADC10CTL1 =  INCH_1 + SHS_0 + ADC10SSEL_0 + ADC10DIV_0 + CONSEQ_0;
    ADC10AE0 = A1;      // Разрешаем вход АЦП на порту P1.1

    ADC10CTL0 |= ENC;     // Разрешаем преобразования.
} // ADC_init

void update_display(void) {
    unsigned int ipart,fpart;

    ipart=(int)volts;   // целая часть
    fpart=(int)((volts-ipart)*1000);   // дробная часть, 3 цифры

    MoveCursor(0,0);
    __delay_cycles(10000);
    SendByte(0x06, FALSE);  // Сдвигаем курсор вперед.
    PrintStr("Voltmeter:");

    MoveCursor(1,15);
    __delay_cycles(10000);
    SendByte(0x04, FALSE);  // Сдвигаем курсор назад.
    SendByte(0x56, TRUE);   // печатаем 'V'
    SendByte(0x20, TRUE);   // печатаем ' '

    while (fpart > 0) {
        SendByte(0x30 + (fpart%10), TRUE);
        fpart /= 10;
    }
    SendByte(0x2E, TRUE);   // печатаем '.'
    SendByte(0x30 + ipart, TRUE);
    PrintStr("            ");

} // обновление экрана
