/**
 Program reaguje na odpowiednie nacisniecie klawiszy zapaleniem lub zgaszeniem diody LED
*/


/**
jak to jest z tymi portami

    * DDR - ustala kierunek transmisji ( 0 - wejście, 1 - wyjście)
    * PORT
         1. jeśli port jest ustawiony na wyjście to, podanie wartości na ten port skutkuje zmianą stanu na porcie
         2. jeśli port jest ustawiony na wejście to jeżeli wstawimy do rejestru wartość "1" to włączymy rezystory podciągające (pull-up)
    * PIN - z tej końcówki odczytujemy faktyczny stan portu
*/
#include <io.h>
#include <interrupt.h>
#include <signal.h>

#define LED1_ON (PORTC|=(0x04))
#define LED2_ON (PORTC|=(0x08))
#define LED1_OFF (PORTC&=~(0x04))
#define LED2_OFF (PORTC&=~(0x08))
#define SWITCH1 (PINC&(0x01))
#define SWITCH2 (PINC&(0x02))

int main(void)
{
    DDRC  = 0xFC;
    PORTC = 0x03;
    for (;;)
    {
        if (SWITCH1==0) LED1_OFF; else LED1_ON;
        if (SWITCH2==0) LED2_OFF; else LED2_ON;
    }
}

