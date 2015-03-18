#include <msp430g2553.h>
#include <stdlib.h>
#include <string.h>

#define BTN1 BIT3
#define LED1 BIT0
#define LED2 BIT6

void txuartch(char c); // tx uart char
void txuartstr(char* string); //tx uart string of char
void reverse(char * s);
void itoa(int n, char * s);
void P1init(void);

//variables
volatile int temp;
char buf[20];
volatile int pin1status, pin2status, pin3status;

int main(void){

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
	DCOCTL = CALDCO_1MHZ;

	P1init();

	//configure USCI--------------------------
	UCA0CTL1 |= UCSSEL_2;             // Have USCI use System Master Clock: AKA core clk 1MHz

	UCA0BR0 = 104;                    // 1MHz 9600, see user manual
	UCA0BR1 = 0;                      //

	UCA0MCTL = UCBRS0;                // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;
	//----------------------------------------------

  //настройка порта на UART
	P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

	IE2 |= UCA0RXIE;                          // прерывание по RX

	//__bis_SR_register(LPM0_bits + GIE);       // глобальные прерывания включены
	__bis_SR_register(GIE);
//ADC10 configure---------------
	ADC10CTL1 = INCH_10 + ADC10DIV_0;  // Temp Sensor ADC10CLK
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON;
	_delay_cycles(5);                  // Wait for ADC Ref to settle
	ADC10CTL0 |= ENC + ADC10SC + ADC10IE;        // Sampling and conversion start

			//ADC10CTL0 &= ~ENC;
			//ADC10CTL0 &= ~(REFON + ADC10ON);


///--------------------------------------

	while(1) {

		ADC10CTL0 |= ENC + ADC10SC;
		_delay_cycles(1000000);
		if (P1OUT & LED1) pin1status = 1; else pin1status = 0;
		if (P1OUT & LED2) pin2status = 1; else pin2status = 0;
		txuartstr("temp:");
		txuartstr(buf);
		txuartstr("\tpin1:");
		txuartch(pin1status + '0');
		txuartstr("\tpin2:");
		txuartch(pin2status + '0');
		txuartch('\n');


	}	//while

}	//main

//  RX interrupt
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    char data; //инициализируем локальную переменную
    data=UCA0RXBUF; //присваиваем переменной значение символа, который только пришел

    if (data=='1') P1OUT ^= LED1;
    if (data=='2') P1OUT ^= LED2;
}//USCI0RX_ISR

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void) {
	//tempRaw = ADC10MEM;

	temp = ((27069L * ADC10MEM) -  18169625L) >> 16;        // Vref = 1.5V
	//temp = ((45115L * tempRaw) -  18169625L) >> 16;      // Vref = 2.5V
	//ADC10CTL0 |= ADC10IE;
	//ADC10CTL0 |= ADC10IFG;
	itoa(temp,buf);
}
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void) {
    if (P1IFG & BTN1) {
    //        P1IFG &= ~BTN1;  // Обнуляем флаг прерывания
            P1OUT ^= LED2;
    }
    P1IFG = 0;  // Обнуляем все случайные флаги прерываний
    return;
} // P1_ISR

void txuartch(char c) {
	while (!(IFG2&UCA0TXIFG));                //ждем завершения предыдущей передачи
		UCA0TXBUF=c;                           //отправляем символ
}
void txuartstr(char* string) {
	while(*string) {
		txuartch(*string);
		string++;
	}
}

void reverse(char * s){
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
void itoa(int n, char * s){
     int i, sign;

     if ((sign = n) < 0)  /* записываем знак */
         n = -n;          /* делаем n положительным числом */
     i = 0;
     do {       /* генерируем цифры в обратном порядке */
         s[i++] = n % 10 + '0';   /* берем следующую цифру */
     } while ((n /= 10) > 0);     /* удаляем */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void P1init(void) {

    P1DIR = LED1 + LED2;
    // Следующие две строчки нужны для инициализации кнопки на LaunchPad 1.5
    P1REN |= BTN1;    // Разрешение подтяжки для P1.3
    P1OUT |= BTN1;    // Подтяжка P1.3 вверх

    P1IES = BTN1; // Прерывание по спадающему фронту для кнопки с подтяжкой вверх
    P1IFG &= ~BTN1; // Обнуляем флаг прерывания перед разрешением
    P1IE = BTN1;    // Разрешаем прерывание для BTN1 ------- Сюда надо добавить другие кнопки

} // P1init
