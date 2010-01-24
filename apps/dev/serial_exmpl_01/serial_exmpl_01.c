/*
 * Na podstawie http://www.elektroda.pl/rtvforum/topic653122.html#3377863
 */

#include <io.h>
#include <progmem.h>
#include <stdlib.h>
#include <interrupt.h>
#include <signal.h> 
//#include <sig-avr.h>  [b] <===doszedlem juz ze dla atmegi8l trzeba zamienic signal.h na sig-avr.h[/b]

#define FCPU      8000000    //częstotliwość oscylatora CPU
#define VUART     38400      //prędkość transmisji [bit/s]
#define VUBRR     FCPU/(VUART*16)-1    //wpis do UBRR dla VUART

unsigned char romram;        //romram=1 => dane z pamięci programu
                             //romram=0 => dane z RAM-u
char *pfifosio;              //wskaźnik na kolejkę UART-u
unsigned char volatile fodbznak=0;    //flaga: "odebrano znak"
char komenda;                //odebrana komenda z PC-ta
char *fifosio[];            //wskaźnik na kolejkę UART-u


SIGNAL(SIG_UART_RECV)        //procedura obsługi odbiornika UART-u
{
 komenda=UDR;                //zapamiętaj odebraną komendę
 fodbznak=1;                 //ustaw flagę odebrania znaku
}

SIGNAL(SIG_UART_TRANS)       //procedura obsługi nadajnika UART
{                            //wywoływana po wysłaniu znaku
 char znak;

 if(romram)                  //skąd pobierać dane?
 {
  znak=PRG_RDB(pfifosio++); //pobierz daną z pamięci programu
 }
 else
 {
  znak=*pfifosio++;         //pobierz dane z pamięci RAM
 }
 if(znak!=0)                 //czy koniec pobierania danych?
 {
  UDR=znak;                  //nie, wyślij znak pobrany z kolejki
 }
 else
 {
  cbi(UCR,TXEN);             //tak, wyłącz nadajnik
 }
}

void czekaj(unsigned long zt) //procedura wytracania czasu
{
 #define tau 10.38
 unsigned char zt1;
 for(;zt>0;zt--)
 {
  for(zt1=255;zt1!=0;zt1--);
 }
}

void wyslijtekstROM(char *tekst)  //wysyłanie danych z pamięci programu
{
 romram=1;                   //dane będą z pamięci programu
 pfifosio=tekst;            //ustaw wskaźnik na dane do wysłania
 sbi(UCR,TXEN);              //włącz nadajnik
 UDR=PRG_RDB(pfifosio++);   //wyślij pierwszy znak, pozostałe będą pobierane
                             //w procedurze obsługi przerwania TXC
}

void wyslijtekst(char *tekst)     //wysyłanie danych z pamięci programu
{
 romram=0;                   //dane będą z pamięci danych
 pfifosio=tekst;            //ustaw wskaźnik na dane do wysłania
 sbi(UCR,TXEN);              //włącz nadajnik
 UDR=*pfifosio++;           //wyślij pierwszy znak, pozostałe będą pobierane
                             //w procedurze obsługi przerwania TXC
}

int main(void)               
{
 unsigned char i;
 unsigned char volatile licznikkl=0;  //zmienna wykorzystywana do pomiaru czasu
                             //naciśnięcia przycisków

 char volatile przyrost=1;  //przyrost zmiany współczynnika wypełnienia sygnału PWM
 //tablica komunikatów do wysłania
 char *info[5]={
                PSTR("\n\rRegulator obrotów silnika DC\n\r"),
                PSTR("0 - zatrzymanie silnika\n\r"),
                PSTR("1 - start z max. obrotami\n\r"),
                PSTR("N - podaj aktualne parametry sterownika\n\r\n"),
                PSTR("\n\rAktualne parametry PWM:")
               };

 union                       //unia pozwala na bajtowy dostęp do zmiennej int
      {
       unsigned int pwm;
       unsigned char pwmc[2];
      }volatile upwm;        //aktualny współczynnik wypełnienia sygn. PWM

 DDRB=0xff;    //PORTB - wy
 PORTB=0xff;   
 DDRD=0x02;    //PD1 - wy (RXD), pozostałe we
 PORTD=0x02;   //podciągania wejścia PD1 (RXD)
 UBRR=VUBRR;   //ustaw prędkość transmisji
 UCR=1<<RXCIE | 1<<TXCIE  | 1<<RXEN;   //zezwolenie na przerwania od
                   //odbiornika i nadajnika, zezwolenie na odbiór i nadawanie
 TCCR1A=0x83;      //PWM 10 bitowy
                   //zerowanie OC1 po spełnieniu warunku równości podczas liczenia
                   //w górę, ustawiane podczas liczenia w dół
 TCCR1B=0x01;      //preskaler=3, co przy 10-bit PWM daje Fwy=ok. 61Hz @8MHz
 TCNT1L=0x00;      //wstępne ustawienie licznika 1
 TCNT1H=0x00;
 upwm.pwm=0x3ff;   //początkowo silnik włączony, wartość TOP odpowiada wysokiemu
                   //poziomowi na wyjściu OC1 (PB3)
 OCR1H=upwm.pwmc[1];         //wpisz aktualnie ustawiony współczynnik do rejestrów
 OCR1L=upwm.pwmc[0];         //OCR1 timera1
 sei();            //włącz przerwania

 for(i=0;i<5;i++) //wyślij winietkę
 {
  wyslijtekstROM(info[i]);   //wysłanie pojedynczej linii tekstu
  while(bit_is_set(UCR,TXEN));    //trzeba zaczekać, aż zostanie wysłana do końca
 }

 while(1)          //główna pętla programu
 {
  if(fodbznak)          //czy odebrano jakiś znak?
  {
   fodbznak=0;          //tak
   switch (komenda)     //interpretacja komendy i wykonanie odpowiedniej akcji
   {
    case '.':                 //odebrano "." - zwiększ prędkość
              upwm.pwm+=przyrost;       //zwiększ PWM
              if(upwm.pwm>0x3ff)
              {
               upwm.pwm=0x3ff;    //jeśli przekroczono wartość TOP, to ustaw TOP
              }
              czekaj(150*tau);    //eliminacja powtórnej interpretacji
                                  //naciśnięcia przycisku
              licznikkl++;              //mierz długość naciśnięcia przycisku
              break;
    case ',':                 //odebrano "," - zmniejsz prędkość
              upwm.pwm-=przyrost;      //zmniejsz PWM
              if(upwm.pwm>0x3ff)
              {
               upwm.pwm=0;        //jeśli przekroczono wartość zero to ustaw zero
              }
              czekaj(150*tau);    //eliminacja powtórnej interpretacji
                                  //naciśnięcia przycisku
              licznikkl++;        //mierz długość naciśnięcia przycisku
              break;
    case '0':                //odebrano "0" - zatrzymaj silnik
              upwm.pwm=0;    //silnik STOP
              break;
    case '1':                //odebrano "1" - ustaw max obroty silnika
              upwm.pwm=0x3ff;     //silnik na MAX
              break;
    case 'n':
    case 'N':
              wyslijtekstROM(info[4]);      //wysłanie pojedynczej linii tekstu
              while(bit_is_set(UCR,TXEN));  //trzeba zaczekać,
                                            //aż zostanie wysłana do końca
              wyslijtekst("0x");       //wyślij prefiks dla liczb heksadecymalnych
              while(bit_is_set(UCR,TXEN));  //trzeba zaczekać,
                                            //aż zostanie wysłana do końca
              itoa(upwm.pwm,fifosio,16);   //konwersja liczby int (hex)
                                            //na łańcuch znakowy
              wyslijtekst(fifosio);        //wyślij aktualną wartość PWM do PC-ta
              while(bit_is_set(UCR,TXEN));  //trzeba zaczekać,
                                            //aż zostanie wysłana do końca
              break;
   }
   if(licznikkl>6)
   {
    przyrost=+16;             //wykryto długie naciśnięcie klawisza,
                              //zwiększ krok regulacji
    licznikkl=6;              //dalej już nie zwiększaj kroku
    cbi(PORTB,1);             //zapal diodę LED2
   }
   OCR1H=upwm.pwmc[1];        //wpisz aktualnie ustawiony współczynnik do rejestrów
   OCR1L=upwm.pwmc[0];        //OCR1 timera1
  }
  else
  {                           //jeśli cisza na linii, ustaw parametry spoczynkowe
   licznikkl=0;
   przyrost=1;
   sbi(PORTB,1);              //zgaś diodę LED2
  }
 }
} 
