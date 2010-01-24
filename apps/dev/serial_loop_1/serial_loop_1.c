/**
 * Program generuje z predkoscia RS-232 ustawiana w VUART ciag napisow przy kwarcu 
 * FCPU. Ma on na celu przetestowanie odbioru znakow na RS-2323 w PC.
 */

/*
#include <inttypes.h>
#include <avr/io.h>
//#include <progmem.h>
#include <stdlib.h>
#include <avr/interrupt.h>
//#include <signal.h>
*/

//#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>   //potrzebne do przerwan
//#include <progmem.h>
#include <avr/pgmspace.h> 

#define F_CPU    8000000UL         // 8 MHz
#include <util/delay.h>
#include "../utils/iocompat.h"
#include "../utils/mb_avr_simple_board.h"

#define VUART 38400             // serial boud rate -> bps
#define VUBRR (F_CPU/(VUART*16)-1)


unsigned char romram;

char *pfifosio;
unsigned char volatile fodbznak = 0;

char komenda;
//char *fifosio[];

//? ????????? char *fifosio[255];

    char *info[3] = {
//        PSTR(
"\n\r-->     TEST 1234567890             <--\n\r"
//)
,
//        PSTR(
"\n\r---->   ABCDEFGHIJKLMNOPRSTUWXYZ  <----\n\r"
//)
,
//        PSTR(
"\n\r------> abcdefghijklmnoprsuwxyz <------\n\r"
//),
    };



//ISR (USART_RXC_vect)
SIGNAL(SIG_UART_RECV)
{
    komenda  = UDR;
    fodbznak = 1;
}

//ISR (USART_TXC_vect)
SIGNAL(SIG_UART_TRANS)
{
    char znak;

    if (romram)
    {
        znak = 
//PRG_RDB(
(char *) pfifosio++
//);
;
    }
    else
    {
        znak = *pfifosio++;
    }

    if (znak != 0)
    {
        UDR = znak;
    }
    else
    {
        cbi(UCR, TXEN);
    }
}

void czekaj(ms_delay)
{
    delay_ms(ms_delay);
}

void wysljtekstROM(char *text)
{
    romram   = 1;
    pfifosio = text;
    sbi(UCR, TXEN);

    // trzeba poczekac az zostane wyslana do konca
    while(bit_is_set(UCR, TXEN));
}
/*
void wyslijtekst(char *text)
{
    romram   = 0;
    pfifosio = text;

    sbi(UCR, TXEN);
    UCR = *pfifosio++;
}
*/

int main (void)
{
    unsigned int i;
    unsigned int volatile licznik1 = 0;

    char volatile przyrost = 1;

    // port B wyjscia
    DDRB  = 0xff;
    PORTB = 0xff;

     //PD1 - wy RXD, pozostale wejscia
    DDRD  = 0x02;
    // podciagniecie wejscia PD1 (RXD)
    PORTD = 0x02;

    // ustaw predkosc transmisji
    UBRR = VUBRR;

    // ZEZWOLENIE NA PRZERWANIA OPD ODBIORNIKA I NADAJNIKA, ZEZWOLENIE NA ODBIOR
    UCR =  1<<RXCIE | 1<<TXCIE | 1<<RXEN;

    // 
    //TCCR1A = 0x83 // pwm 10-BITOWY


    //wlacz przerwania
    sei();

    while (1)
    {
        wysljtekstROM(info[0]);
        wysljtekstROM(info[1]);
        wysljtekstROM(info[2]);
    }

    return 0;
}

