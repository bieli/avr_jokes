/*
************************************************************************  
  Nazwa pliku -  klawisze_4x4_uart_atm8.c
  Autor - Wojciech Chojnacki.
  Data -  25-11-2003
  Procesor Atmel z portami B i C  @ 6MHz 
  Klawiatura 4x4 przylączona do portów B i C
  Wysyłanie liczb do PC przez UART
  Licencja - GPL
  Modyfikowany 06-04-2005
************************************************************************ 
*/

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include "uart.h"

#define XTAL_CPU         6000000      /* 6Mhz */
#define UART_BAUD_RATE      9600      /* 9600 baud */

unsigned char licz_ob;			// Licznik obiegów pętli
unsigned char kol;             // Numer kolumny
volatile unsigned char licznik;			// Wartosc licznika spowalniajacego
unsigned char przycisk;		// Numer naciśniętego przycisku
unsigned int liczba_w;    	    // Liczba wprowadzana 
unsigned int p;    	        // Liczba wprowadzona 
unsigned char klawisze=0xFF;   // Bajt znaczników logiki klawiatury
char buffer_n[5];    	        // Bufor do przechowywania wyniku konwersji z "unsigned int" na ASCII

// Definicje makr operacji na bitach
#define SETBIT(x,y) (x |=(y))
#define CLEARBIT(x,y) (x &= (~y))
#define CHECKBIT(x,y) (x & (y))

// Definicje bitów bajtu znaczników klawiatury
#define dodawanie 0x01
#define odejmowanie 0x02
#define mnozenie 0x04
#define dzielenie 0x08
#define wynik 0x10
#define f_klaw 0x20
#define n_klaw 0x40
#define klaw 0x80

void system_init(void)			// Inicjalizacja systemu
{
  //Inicjalizacja portów B i C

	DDRB=0x0F;					// Port B bity 0, 1, 2, 3 jako wyjścia
	DDRC=0xF0;					// Port C bity 0, 1, 2, 3 jako wejścia
	PORTB=0x0F;					// Port B wyjscia w stanie wysokim
	PORTC=0x0F;					// Port C wejścia z podciąganiem
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,XTAL_CPU) ); 
  sei();
 }

void keypad(void)
{
	licz_ob++;               // Licznik obiegów pętli  
	if(licz_ob==20)        
    {
		kol++;            // Licznik kolumny
		licz_ob=0;
		if(kol==4)            // Przepełnienie licznika obiegów pętli
	    {
	      kol=0;
	      licznik++;
	      if(licznik==50) 		// Licznik spowalniajšcy
	        {
	          SETBIT(klawisze, klaw);		// Koniec impulsu "WCISNIETO KLAWISZ"
	          SETBIT(klawisze, n_klaw);		// Flaga naciśnięcia klawisza numerycznego
	          SETBIT(klawisze, f_klaw);		// Flaga naciśnięcia klawisza funkcyjnego
	          przycisk=0;
	          licznik=0;
	        }
	    }
		PORTB=0xFF;
		PORTB &= ~_BV(kol);
    }  
	
	if ((PINC<<4)!=240)		// Wcisnięto klawisz
    {		
		CLEARBIT(klawisze, klaw);		// Zerowanie flagi naciśnięcia klawisza
		licznik=0;
    }
	if((kol==0))			// Detekcja klawisza 1-szej ( od lewej) kolumny
    {
      if((PINC<<4)==224)
        {
			przycisk=0;     // Wciśnięto przycisk "0"
        }
      if((PINC<<4)==208)	 
        {
			przycisk=4;		// Wciśnięto przycisk "4"
        }
      if((PINC<<4)==176)
        {
			przycisk=8;		// Wciśnięto przycisk "8"
        }
      if((PINC<<4)==112)
        {
			przycisk=12;	// Wciśnięto przycisk "12" (C)
        }
    }
  if((kol==1))				// Detekcja klawisza 2-giej kolumny
    {
      if((PINC<<4)==224)
        {
			przycisk=1;
        }
      if((PINC<<4)==208)
        {
			przycisk=5;
        }
      if((PINC<<4)==176)
        {
			przycisk=9;
        }
      if((PINC<<4)==112)
        {
			przycisk=13;
        }
    }
  if((kol==2))				// Detekcja klawisza 3-ciej kolumny
    {
      if((PINC<<4)==224)
        {
			przycisk=2;
        }
      if((PINC<<4)==208)
        {
			przycisk=6;
        }
      if((PINC<<4)==176)
        {
			przycisk=10;
        }
      if((PINC<<4)==112)
        {
			przycisk=14;
        }
    }	
  if((kol==3))				// Detekcja klawisza 4-tej kolumny
    {
      if((PINC<<4)==224)
        {
			przycisk=3;
        }
      if((PINC<<4)==208)
        {
			przycisk=7;
        }
      if((PINC<<4)==176)
        {
			przycisk=11;
        }
      if((PINC<<4)==112)
        {
			przycisk=15;
        }
    }

  if((przycisk==10)&&(CHECKBIT(klawisze, wynik)))		// Obliczenie wyniku
    {
		CLEARBIT(klawisze, wynik);
		uart_puts_P("Wprowadziles: ");
		utoa(liczba_w, buffer_n, 10);
		uart_puts(buffer_n);
		uart_puts_P("\n"); 
		liczba_w=liczba_w+p;
		uart_puts_P("Wynik dodawania: ");
		utoa(liczba_w, buffer_n, 10);
		uart_puts(buffer_n);
		uart_puts_P("\n Wcisnij B \n");
    }
  		
  if((przycisk==11)&&(CHECKBIT(klawisze, f_klaw)))		// Kasowanie wprowadzonych wartoci
    {
		CLEARBIT(klawisze, f_klaw);       // Zerowanie flagi nasiśnięcia klawisza funkcyjnego
		liczba_w=0;
		p=0;
		SETBIT(klawisze, wynik); 
		SETBIT(klawisze, dodawanie);
		SETBIT(klawisze, odejmowanie);
		SETBIT(klawisze, mnozenie);
		SETBIT(klawisze, dzielenie);
		uart_puts_P("Wprowadz liczbe i wcisnij C\n");
    }
  if((przycisk==12)&&(CHECKBIT(klawisze, dodawanie))) 		// Wybranie dodawania
    {
		CLEARBIT(klawisze, dodawanie);		// Zerowanie flagi wybrania dodawania
		p=liczba_w;
		liczba_w=0;
		uart_puts_P("Wprowadziles: ");
		utoa(p, buffer_n, 10);
		uart_puts(buffer_n);
		uart_puts_P("\n Wprowadz druga liczbe i wcisnij A \n");
    }
  if((przycisk==13)&&(CHECKBIT(klawisze, odejmowanie)))
    {
		CLEARBIT(klawisze, odejmowanie);
		p=liczba_w;
		liczba_w=0;
    }
  if((przycisk==14)&&(CHECKBIT(klawisze, mnozenie)))
    {
		CLEARBIT(klawisze, mnozenie);
		p=liczba_w;
		liczba_w=0;
    }
  if((przycisk==15)&&(CHECKBIT(klawisze, dzielenie)))
    {
		CLEARBIT(klawisze, dzielenie);
		p=liczba_w;
		liczba_w=0;
    }
  if(!(CHECKBIT(klawisze, klaw))&&(CHECKBIT(klawisze, n_klaw))&&(przycisk<10))
    {
		liczba_w=liczba_w*10+przycisk;	// Obliczanie wprowadzanej liczby
		CLEARBIT(klawisze, n_klaw);		// Zerowanie flagi zadzialania klawisza numerycznego
    }
}

int main(void)
{
	system_init();		// Inicjalizacja systemu
	uart_puts_P("Demonstracja klawiatury 4x4\n Dodawanie:\n Wprowadz liczbe i wcisnij C \n");
	while(1)			// Nieskończona pętla
    {
		keypad();   	// Obsługa klawiszy
    }
}


