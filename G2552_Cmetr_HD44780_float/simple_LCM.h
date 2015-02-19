/* simple_LCM.h: Заголовочный файл для библиотеки simple_LCM.
 * Версия 1.0
 * 
 *
 * Эта библиотека предоставляет 4-битный 6-проводной интерфейс
 * для стандартного текстового ЖК-модуля. Текущая версия
 * требует присоединения выводов дисплея к определенным выводам порта P1.
 * Для использования этой библиотеки с другой конфигурацией выводов
 * необходимо исправить значения в обьявлениях. Для частот тактового
 * сигнала выше 2 мГц, необходимо увеличить значения в _delay_cycles
 * в коде, что бы обеспечить соответствие тайминга.
 * 
 * Для использования библиотеки, запустите LCM_init(); в вашем коде.
 * Затем вы можете использовать как SendByte(<byte>, TRUE);
 * так и PrintStr("string"); для вывода текста на экран.
 * 
 * Команды посылаются функцией SendByte(<byte>, FALSE);
 * 
 * MoveCursor(row,col); перемещает курсор на специфичную строку и столбик.
 * clear(); очищает дисплей, и сбрасывает курсор в позицию 0,0.
 *  
 */

#ifndef SIMPLE_LCM_H_
#define SIMPLE_LCM_H_

#endif /*SIMPLE_LCM_H_*/

#include <msp430.h>

/*  Обьявления для конкретной конфигурации соединений.  */
/*  Измените, если необходимо.              */
#define LCM_DIR P1DIR
#define LCM_OUT P1OUT

#define RS  BIT0    // Вывод RS (Register Select) на P1.0 (уберите перемычку светодиода)
#define EN  BIT3    // Вывод E (Enable) на P1.3
#define D4  BIT4    // Выводы 4-7 шины данных на P1.4-P1.7 (уберите перемычку светодиода на P1.6)
#define D5  BIT5
#define D6  BIT6
#define D7  BIT7

/*  Сокращения (для удобства)  */
#define MASK    (RS + EN + D4 + D5 + D6 + D7)
#define FALSE 0
#define TRUE 1


void pulse(void);
void SendByte(char, char);
void LCM_init(void);
void clear(void);
void MoveCursor(char, char);
void PrintStr(char *);
