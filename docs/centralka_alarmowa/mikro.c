/**

 * program realizujący sterownik ogrzewania akwariowego na bazie mikrokontrolera AT90S2313

 * kompilacja i wgranie przez ISP:
 *  avr-gcc -mmcu=at90s2313 -Os -o mikro.o mikro.c
 *  avr-objcopy -O ihex mikro.o mikro.hex
 *  uisp -dprog=dapa -dlpt=0x378 --erase --segment=flash
 *  uisp -dprog=dapa -dlpt=0x378 --upload --segment=flash if=mikro.hex
 *  uisp -dprog=dapa -dlpt=0x378 --verify --segment=flash if=mikro.hex

**/


/** nagłówki ogólne **/

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/eeprom.h>

#define bool unsigned char
#define sti() sei()

/*
long long czas() { asm( "rdtsc" ); }
void opoznienie( long long l ) {
	l = l * 3012 + czas(); // prędkość procesora w MHz
	while( l > czas() );
}
#define opoznienie(czas) opozniacz(czas * 0.67)
void opozniacz (long l) {
	for (; l; --l ) asm volatile( "" );
}*/

#define F_CPU 7372800UL
#include "opoznienia.c"

/** nagłówki LCD **/

#define lcd_send(byte) PORTB = (PORTB | 0x3f) & (byte & 0x3f)
#include "lcd.c"


/** nagłówki OneWire **/

#define send_zero() DDRB |= 0x40; PORTB &= 0xbf
#define send_jeden() PORTB |= 0x40; DDRB &= 0xbf
#define read() (PINB & 0x40) >> 6
#include "old_onewire.c"


/** zmienne globalne **/

unsigned char ustawienia[28];/*={0, 0, 22, 25, 18, 33,\
       0x28, 0x46, 0xba, 0x89, 0x0, 0x0, 0x0, 0x61,\
                              0, 0, 22, 25, 18, 33,\
       0x28, 0x31, 0x8f, 0x89, 0x0, 0x0, 0x0, 0x2a};*/
unsigned char temperatura_ascii[24];
unsigned char temp, temp_old, dn, t0, t1, j;
char alarm[2];


/** port szeregowy **/

char uart_buf, uart_set;
SIGNAL( SIG_UART_RECV ) {
	cli();

	uart_buf = UDR;

	if (uart_set != -1) {
		ustawienia[ uart_set ] = uart_buf;
		uart_set = -1;
		UDR = 'e'; // end set mode
		goto END;
	}

	uart_set = (uart_buf & 0x0f) + ((uart_buf >> 4) & 0x01) * 14;

	switch (uart_buf & 0xc0) {
		case 0x40:
			UDR = ustawienia[ uart_set ];
			uart_set = -1;
			break;
		case 0x80:
			UDR = 's'; // start set mode
			break;
		case 0xc0:
			for (j=0; j<28; j++) {
				eeprom_write_byte(j, ustawienia[j]);
				eeprom_busy_wait();
			}
			uart_set = -1;
			UDR = 'o'; // ok
			break;
	}
END:
	sti();
}
/*
starsze 4 bity - okreslenie funkcji (get/set/save), urzadzenia
	funkca | urzadzenie
	funkcja:
		0x4 - get
		0x8 - set
		0xc - save
	urzadzenie
		numer urzadzenia od 0x0 do 0x1 (maksymalnie 0x3)
młodze 4 bity - numer wartosci:
	0x0 - temperatura
	0x1 - granica dolna
	0x2 - granica górna
	0x3 - granica alarmu dolna
	0x4 - granica alarmu górna
	0x5 - grzanie (dwa najstarsze bity: 00 = auto, 10 = on, 01 = off)
		// poprzecinkowa czesc temperatury (4 najmlodsze bity)
	0x6 - ROM (0)
	...
	0xd - ROM (7)
*/
/*
	UCR = 1 << TXEN | 1 << RXEN | 1 << RXCIE;
		// włączenie nadajnika, odbionika UART wraz z generowaniem przerwań
	UBRR = 47; // szybkosc transmisji 9600 bps
	uart_set = -1;
	sti(); // włączenie przerwań
*/


/** sterowanie grzalkami i alarmem **/

void grzanie(unsigned char device, bool on) {
	if (ustawienia[ 5 + 14*dn ] & 0x80) {
		on=1;
	} else if (ustawienia[ 5 + 14*device ] & 0x40) {
		on=0;
	}

	if (on) {
		PORTD |= 0x04 << device;
	} else {
		PORTD &= ~(0x04 << device);
	}
}
#define alarm_ON() PORTD |= 0x10
#define alarm_OFF() PORTD &= 0xef
// z jakis powodow nie chial dzialac na PB7
//#define alarm_ON() PORTB |= 0x80
//#define alarm_OFF() PORTB &= 0x7f
//#define alarm_ON() asm volatile( "sbi 0x18,7" )
//#define alarm_OFF() asm volatile( "cbi 0x18,7" )


/** funkcja główna **/

int main() {
	opoznienie (10240);
	DDRB = 0xbf;
	DDRD = 0x1c;
	PORTD = 0x0;
	alarm[0] = 0;
	alarm[1] = 0;

	/** odczyt konfiguracji **/
	for (j=0; j<28; j++) {
		ustawienia[j] = eeprom_read_byte(j);
		eeprom_busy_wait();
	}

	/** wyświetlacz - inicjalizacja **/
	opoznienie(1000000);
	lcd_send_comand2 (0x2); lcd_send_comand (0x28);
		// Function Set (2 linie 4 bit) = 0010, 0010 10**
	lcd_send_comand (0xc);
		// On-Off Control (bez pokazywania kursora) = 0000 1100
	lcd_send_comand (0x6);
		// Entry Mode Set = 0000 0110

	lcd_first_line();
	lcd_send_string ("   AKWARIUM  TERRARIUM  ", 24); // jeden wiersz po 24 znaki


	/** port szeregowy - inicjalizacja **/
	UCR = 1 << TXEN | 1 << RXEN | 1 << RXCIE;
		// włączenie nadajnika, odbionika UART wraz z generowaniem przerwań
	UBRR = 47; // szybkosc transmisji 9600 bps
	uart_set = -1;
	sti(); // włączenie przerwań


	/** pętla główna **/
	while(1) {
		/** termometry **/
		for (dn=0; dn<2; dn++) {
			// docelowo w glownej i=0,1 termometr + i*8 temperatura_ascii + i*5
			opoznienie(1000000);
			onewire_command(0x44, NULL);

			while ( !onewire_bit(1) );
			send_jeden();

			opoznienie(1000000);
			onewire_command(0xbe, ustawienia + 6 + 14*dn);
			t0 = onewire_byte(0xff);
			t1 = onewire_byte(0xff);

			temp_old = ustawienia[ 14*dn ];
			temp = ustawienia[ 14*dn ] = t1 << 4 | t0 >> 4;
			if (temp - temp_old > 10 || temp_old - temp > 10) continue;

			ustawienia[ 5 + 14*dn ] &= 0xf0;
				// wymazuje po przecinkowa czes temperatury (4 mlodsze bity)
			ustawienia[ 5 + 14*dn ] |= t0 & 0x0f;
				// i zapisuje ponownie

			/** generowanie napisu ASCII **/
			temperatura_ascii[ 4 + 10*dn ]='0'; temperatura_ascii[ 5 + 10*dn ]='0';
			itoa( temp , temperatura_ascii + 4 + 10*dn, 10);
			temperatura_ascii[ 6 + 10*dn ]='.';
			itoa( (t0 & 0x0f) * 625 , temperatura_ascii + 7 + 10*dn, 10);
			temperatura_ascii[ 8 + 10*dn ]=' ';

			/** podejmowanie decyzji i dzialan **/
			if ( temp < ustawienia[3 + 14*dn] ) {
				// alarm - za zimno
				temperatura_ascii[ 9 + 10*dn ]=185;
				grzanie(dn, 1);
				alarm[dn]++;
			} else if ( temp < ustawienia[1 + 14*dn] ) {
				// zaczynamy grzejemy
				temperatura_ascii[ 9 + 10*dn ]=27;
				grzanie(dn, 1);
				alarm[dn]=0;
			} else if ( temp < ustawienia[2 + 14*dn] ) {
				// temperatura dobra - nie zmieniamy stanu grzania
				temperatura_ascii[ 9 + 10*dn ]=205;
				alarm[dn]=0;
			} else if ( temp < ustawienia[4 + 14*dn] ) {
				// przestajemy grzac
				temperatura_ascii[ 9 + 10*dn ]=26;
				grzanie(dn, 0);
				alarm[dn]=0;
			} else {
				// alarm - za goraco
				temperatura_ascii[ 9 + 10*dn ]=186;
				grzanie(dn, 0);
				alarm[dn]++;
			}
		}

		/** obsługa alarmu **/
		if (alarm[0] == 0 && alarm[1] == 0) {
			alarm_OFF();
		} else if (alarm[0] > 0 || alarm[1] > 0) {
			alarm_ON();
		}

		/** obsługa LCD **/
		for (dn=0; dn<4; dn++) temperatura_ascii[dn] = ' ';
		for (dn=10; dn<14; dn++) temperatura_ascii[dn] = ' ';
		for (dn=20; dn<24; dn++) temperatura_ascii[dn] = ' ';
		lcd_second_line();
		lcd_send_string (temperatura_ascii, 24);
	}
}
