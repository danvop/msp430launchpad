#include <msp430g2553.h>
#include <stdlib.h>

#define USCI_INPUT_CLK      (1000000UL)  // in Hz
#define USCI_BAUD_RATE      (9600)

#define USCI_DIV_INT              (USCI_INPUT_CLK/USCI_BAUD_RATE)
#define USCI_BR0_VAL              (USCI_DIV_INT & 0x00FF)
#define USCI_BR1_VAL              ((USCI_DIV_INT >> 8) & 0xFF)

#define USCI_DIV_FRAC_NUMERATOR   (USCI_INPUT_CLK - (USCI_DIV_INT*USCI_BAUD_RATE))
#define USCI_DIV_FRAC_NUM_X_8     (USCI_DIV_FRAC_NUMERATOR*8)
#define USCI_DIV_FRAC_X_8         (USCI_DIV_FRAC_NUM_X_8/USCI_BAUD_RATE)

#if (((USCI_DIV_FRAC_NUM_X_8-(USCI_DIV_FRAC_X_8*USCI_BAUD_RATE))*10)/USCI_BAUD_RATE < 5)
#define USCI_BRS_VAL              (USCI_DIV_FRAC_X_8<< 1)
#else
#define USCI_BRS_VAL              ((USCI_DIV_FRAC_X_8+1)<< 1)
#endif

volatile char temp = 24;
volatile	char* tempstr ;
char buffer[20];




void putchar0(char c) {
     while (!(IFG2&UCA0TXIFG));                //ждем завершения предыдущей передачи
     UCA0TXBUF=c;                           //отправляем символ
}

void puts0(char* string) {
    while(*string) {
        putchar0(*string);
        string++;
    }
}

int main(void){

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;

  //настройка порта на UART
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

  // setup USCI UART registers
  UCA0CTL1 |= UCSSEL_2 + UCSWRST;
  UCA0BR0 = USCI_BR0_VAL;
  UCA0BR1 = USCI_BR1_VAL;
  UCA0MCTL = USCI_BRS_VAL;
  UCA0CTL1 &= ~UCSWRST;

  IE2 |= UCA0RXIE;                          // прерывание по RX
 __bis_SR_register(LPM0_bits + GIE);       // глобальные прерывания включены

  while(1) {
//	  puts0("temp \t");
//	  puts0("pin1: \t");
//	  puts0("pin2: \t");
//	  puts0("pin3: ");
//	  putchar0('\n');

  }
}

//  RX прерывашка
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    char data; //инициализируем локальную переменную
    data=UCA0RXBUF; //присваиваем переменной значение символа, который только пришел

    if (data=='1') { //если символ есть "1"
    	puts0("DO SOMETHING!"); putchar0(10);

                        //пишем DO SOMETHING! и переходим на строку вниз
    } else {
    	puts0("WHAT DO YOU WANT?\n");
    	//puts0();
    	puts0("\n");
        }

}

