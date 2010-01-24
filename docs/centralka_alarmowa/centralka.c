/**
 * program centralki alarmowej opartej na mikrokontrolerze Atmega8
 * centralka kozysta z linii IO mikrokontrolera oraz portu szeregowego w trybie przerwaniowym
 *
 * w przypadku gdy system jest nieuzbrojony:
 *  - otrzymanie kodu, który zgadza się z kodem zapisanym w centralce
 *    powoduje uzbrojenie alarmu z opóźnieniem 10s
 *  - otrzymanie informacji o zamknieciu drzwi powoduje rozpoczącie 30 sekundowego testu,
 *    jeżeli w trakcie nie go nie został wysłany żaden sygnał z programatora
 *    oraz nie został zarejestrowany żaden sygnał z PIR następuje uzbrojenie alarmu
 *
 * w przypadku gdy system jest uzbrojony:
 *  - otrzymanie kodu, który zgadza się z kodem zapisanym w centralce
 *    powoduje natychmiastowe rozbrojenie alarmu
 *  - otrzymanie kodu, który zgadza się z kodem alarmowym
 *    powoduje natychmiastowe włączenie alarmu
 *  - otrzymanie informacji o otwarciu drzwipowoduje rozpoczęcie 30 sek oczekiwania na
 *    kod rozbrajający (wprowadzenie błędnego kodu powoduje zresetowanie licznika,
 *    3 wprowadzenie błędnego kodu powoduje właczenie alarmu), alarm pozostaje w tym
 *    okresie w stanie uzbrojonym (aktywacja PIR = właczenie alarmu)
 *
 * centralka wysyła przez RS informację o zmianie stanu alarmu (wraz z nowym stanem),
 * następnie oczekuje przez 500ms na potwierdzenie, w przypadku braku ponawia,
 * podejmuje 3 takie próby
 *
 *
 * kompilacja i wgranie przez ISP:
 *  avr-gcc -mmcu=atmega8 -Os -o sterownik.o sterownik.c
 *  avr-objcopy -O ihex sterownik.o sterownik.hex
 *  uisp -dprog=dapa -dlpt=0x378 --erase --segment=flash
 *  uisp -dprog=dapa -dlpt=0x378 --upload --segment=flash if=sterownik.hex
 *  uisp -dprog=dapa -dlpt=0x378 --verify --segment=flash if=sterownik.hex
 *  w razie problemow warto przeladowac modul odpowiedzialny za obsluge LPT
 *    rmmod lp parport_pc parport ; modprobe parport_pc
 *
 * konfiguracja układu na zewnętrzny kwarc 3MHz
 *  uisp -dprog=dapa -dlpt=0x378 --wr_fuse_l=0xEF
**/


#include <inttypes.h>

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/** nagłówki funkcji opóżniających **/
#define F_CPU 1000000UL
#include "opoznienia.c"

/** nagłówki OneWire **/
#define send_zero() DDRB |= 0x40; PORTB &= 0xbf
#define send_jeden() PORTB |= 0x40; DDRB &= 0xbf
#define read() (PINB & 0x40) >> 6
#include "onewire.c"

/** nagłówki mojego formatu transmisji przez RS485 **/
#define DRIVER_CTRL_PORT PORTD
#define DRIVER_CTRL_PIN PD2
#include "my_rs485.c"

/// identyfikator urządzenia liczba z zakresu 0 - 63
#define _DEV_ID_ 0x3f

/// identyfikator manipulatora
#define _KEY_PAD_ID_ 0x3e

/// identyfikator systemu liogowania i prezentacji
#define _INFO_MODULE_ID_ 0x3d

/// termometry One Wire
#define _LICZBA_TERMOMETROW_ 4
#define  _TERMOMETRY_ROM_ 0x28, 0xfd, 0x1e, 0x27, 0x01, 0x00, 0x00, 0xa5,\
	0x28, 0xd3, 0x88, 0x27, 0x01, 0x00, 0x00, 0x2b,\
	0x28, 0xea, 0xbc, 0x27, 0x01, 0x00, 0x00, 0xcb,\
	0x28, 0xa3, 0x00, 0x27, 0x01, 0x00, 0x00, 0x46
/* pok. komputerowy; pokój I; pokój II; przedpokój (wejście) */

/// status systemu
volatile int8_t system_status;
//   7  |     6, 5    |        4      |
// błąd | nie uzywane | alarm sabotaż |
//         3      |         2        |           1        |             0
// alarm pożarowy | alarm włamaniowy | uzbrojenie systemu | oczekiwanie na zmianę uzbr.

/// stany linii alarmowych
volatile uint8_t PIR_alarm, PIR_sabotaz, DYM;

/// temperatury
volatile int16_t temperatury[ _LICZBA_TERMOMETROW_ ];

/// kod
#define _MAX_CODE_LEN_ 4
volatile uint8_t kod[_MAX_CODE_LEN_];
volatile uint8_t kod_pozycja;

/// potwierdzenie odbioru przez RS (bit 0) / licznik prób kodu (bity 1 i 2)
volatile uint8_t potwierdzenie;

/// licznik timera programowego
volatile uint16_t soft_timer;


/** funkcja wysyłająca komendę RS do klawiatury i oczekująca na potwierdzenie **/
send_to_key_pad(int8_t cmd) {
	int8_t j;
	potwierdzenie &= 0xfe;
	for(j=0; j<3 && (potwierdzenie & 0x01); j++) {
		send_rs_cmd(_CENTR_ID_, 0x40, cmd); // stan kontaktronu
		_delay_cs(50); // 0.5s
	}

	/// sygnalizacja stanu
	if (! (potwierdzenie & 0x01))
		system_status |= 0x80;
}


/** obsluga portu szeregowego **/
uint8_t uart_buf_head, uart_buf_body, chanel, cmd = 0;
ISR(USART_RXC_vect) {
	cli();

	uart_buf_head = UDR;
	uart_buf_body = uart_buf_head & 0x3F;
	uart_buf_head = uart_buf_head & 0xC0;
	/// dwa pierwsze bity (uart_buf_head) oznaczają typ komunikatu:
	/// 00 (0x00) - identyfikator urzadzenia
	/// 01 (0x40) - identyfikator lini
	/// 10 (0x80) - komenda
	/// 11 (0xC0) - odpowiedz (ignorowane przez slave)

	/// kolejnosc nadawania pelnej komendy id urzadzania, id lini, komenda

	if (uart_buf_head != 0x00 && cmd != 0) {
		/// gdy nie ID urządzenia oraz wcześniejszy ID urządzenia pasował do naszego
		if (uart_buf_head == 0x40) {
			chanel = uart_buf_body;
			cmd = 2; /// przed komendą wymagany jest identyfikator linii
		} else if (uart_buf_head == 0x80 && cmd == 2) {
			switch (uart_buf_body) {
				case 0x01: /// zmiana stanu kontaktronu
					if ((system_status & 0x02) >> 1 != chanel) {
						// gdy uzbrojony i chanel 0 LUB nieuzbrojony i chanel 1
						/// odnotowujemy zmianę trybu -> oczekiwanie na ...
						system_status++;
						/// ustawiamy timer oczekiwania
						TCNT0 = 0; soft_timer = 360;// ustawienie wartości początkowej TIMER0 i soft_timer
						TCCR0 = 0x05; // wzrost wartości TIMER0 co taktów zegara głównego (5 => 1024)
					}
					send_rs_cmd(_KEY_PAD_ID_, 0x40, 0x85); // potwierdzenie
					break;
				case 0x02: /// sygnał z domofonu / dzwonka
					// FIXME funkcjonalność do zaimplementowania w następnej wersji ...
					send_rs_cmd(_INFO_MODULE_ID_, 0x40 | chanel, 0x85); // dzwonek/domofon
					break;
				case 0x03: /// ERROR
					system_status |= 0x80;
					break;
				case 0x04: /// przesyłanie kodu
					if (chanel==12) {
						// ponowne wprowadzanie kodu
						TCNT0 = 0; soft_timer = 360;// ustawienie wartości początkowej TIMER0 i soft_timer
						kod_pozycja = 0;
					} else if (chanel==11 && (kod_pozycja=_MAX_CODE_LEN_ || kod[kod_pozycja] == 0xff)) {
						// koniec (poprawnego) kodu
						TCNT0 = 0; TCCR0 = 0; // wyłączenie TIMER0
						if(system_status & 0x02) {
							system_status = 0;
							send_to_key_pad(0x81)
						} else {
							system_status = 2;
							send_to_key_pad(0x81)
						}
						potwierdzenie &= 0xf9;
					} else if (kod_pozycja==_MAX_CODE_LEN_ || chanel != kod[kod_pozycja++]) {
						// błędny kod
						TCNT0 = 0; soft_timer = 360;// ustawienie wartości początkowej TIMER0 i soft_timer
						kod_pozycja = 0;
						potwierdzenie += 2;
						if ((system_status & 0x02) && potwierdzenie >= 6) {
							system_status |= 0x10; // sabotaż bo 3 ktrotne wprowadzenie złego kodu
							potwierdzenie &= 0xf9;
						}
						send_rs_cmd(_KEY_PAD_ID_, 0x40, 0x84); // code error
					}
					break;
				case 0x05: /// potwierdzenie
					potwierdzenie |= 0x01;
					break;
				case 0x06: /// zmiana kodu
					if (kod_pozycja & 0xf0) { // zmiana kodu
						kod[kod_pozycja & 0x0f] = chanel;
						eeprom_write_byte(kod_pozycja & 0x0f, chanel);
						eeprom_busy_wait();
						kod_pozycja++;
					} else if (chanel==11 && (kod_pozycja=_MAX_CODE_LEN_ || kod[kod_pozycja] == 0xff)) {
						kod_pozycja = 0xf0
					} else if (kod_pozycja==_MAX_CODE_LEN_ || chanel != kod[kod_pozycja++]) { // także anulowanie chanel==12
						kod_pozycja = 0;
					}
					break;
				case 0x07: /// odczyt PIR
					send_rs((PIR_alarm >> chanel) & 0x01);
					break;
				case 0x08: /// odczyt PIR sabotarz
					send_rs((PIR_sabotaz >> chanel) & 0x01);
					break;
				case 0x09: /// odczyt czujek dymu
					send_rs((DYM >> chanel) & 0x01);
					break;
				case 0x0a: /// odczyt temperatury (z wskazanego w line termometru)
					send_rs((temperatury[chanel]) & 0x3f);
					send_rs((temperatury[chanel] >> 6) & 0x3f);
					break;
				case 0x0a: /// odczyt statusu_systemu
					send_rs( ((system_status & 0x80) >> 2) | (system_status & 0x1f) );
					break;
			}
			cmd = 0; /// po wykonaniu komendy oczekujemy na całą nowa paczkę
		}
	} else if (uart_buf_body == _DEV_ID_) {
		/// gdy ID naszego urządzenia
		cmd = 1;
	} else {
		/// gdy ID innego urządzenia
		cmd = 0;
	}
	sei();
}


/** obsługa przerwania z TIMER0 **/
SIGNAL (SIG_OVERFLOW0) {
	if (--soft_timer == 0) {
		/// wyłączamy TIMER0
		TCCR0 = 0;

		if ((system_status & 0x01) == 1) { // koniec oczekiwania na uzbrojenie / rozbrojenie
			// stan nie został zmieniony poprzez wykrycie ruchu ani kod
			//  => następuje uzbrojenie
			if (system_status == 1) {
				send_to_key_pad(0x81); // wysyłamy RS i czekamy na potwierdzenie
			}
			// LUB
			// stan nie został zmieniony wprowadzeniem kodu rozbrajającego
			//  => wracamy do stanu uzbrojenia
			system_status = 2;
		}
	}
}




main() {
	/** konfiguracja portów IO mikrokontrolera **/
	/// PIR - wejścia alarmowe (PA0..6), wejściowy bez podciągającego
	/// SPECJALNE (PA7), wyjściowy wyłączony
	DDRA = 0x80;
	PORTB = 0x00; //0x7f;

	/// PIR - wejścia sabotarzowe (PC0..6), wejściowy bez podciągającego
	/// SPECJALNE (PA7), wyjściowy wyłączony
	DDRC = 0x80;
	PORTC = 0x00; //0x7f;

	/// CZUJKI DYMU (PB0..5), wejściowy bez podciągającego
	/// ONE WIRE (PB6), wejściowy z podciągającym
	/// SPECJALNE (PB7), wyjściowy wyłączony
	DDRB = 0x80;
	PORTB = 0x40; //0x7f;

	/// STEROWANIE RS (PD2), wyjściowy wyłączony
	/// STEROWANIE ZASILANIEM (PD3..7), wyjściowy włączony
	DDRD = 0xfc;
	PORTD = 0xf8;

	/// RS485 (PD0, PD1) - inicalizacja portu
	/// włączenie nadajnika, odbionika UART wraz z generowaniem przerwań
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	/// ustawienie prędkości (BAUD), UBRR = F_CPU / (16*BAUD) -1
	UBRRH = 0; UBRRL = 19; // BAUD = 9600 przy 3 MHz
	//UBRRH = 0; UBRRL = 207; // BAUD = 300 przy 1 MHz

	/// TIMERY
	TIMSK = _BV(TOIE0); // przerwania z TIMER0 (gdy następi przepełnienie (0xff -> 0x00))

	/** włączenie przerwań **/
	sei();

	system_status = 0;

	uint8_t termometry[ _LICZBA_TERMOMETROW_ * 8 ] = { _TERMOMETRY_ROM_ };
	uint8_t term_num = 0;

	uint8_t i;

	for (i=0; i<_MAX_CODE_LEN_; i++) {
		kod[j] = eeprom_read_byte(i);
		eeprom_busy_wait();
	}

	/** petla glowna **/
	while (1) {
		// odczyt linii
		PIR_alarm = & 0x7f;
		PIR_sabotaz = & 0x7f;
		DYM = & 0x3f;

		/// odczyt linii alarmowych
		if ( ! DYM ) {
			system_status |= 0x08;
			for (i=0; DYM >> keyboard; i++);
			send_rs_cmd(_INFO_MODULE_ID_, 0x40 | i, 0x81); // dym
		}

		if ((system_status & 0x02) && PIR_alarm) {
			system_status |= 0x04;
			for (i=0; PIR_alarm >> keyboard; i++);
			send_rs_cmd(_INFO_MODULE_ID_, 0x40 | i, 0x82); // ruch
		}

		if ((system_status & 0x02) && PIR_sabotaz) {
			system_status |= 0x10;
			for (i=0; PIR_alarm >> keyboard; i++);
			send_rs_cmd(_INFO_MODULE_ID_, 0x40 | i, 0x83); // sabotaż
		}

		/// odczyt temperatur i ich porównywanie
		if (! (term_num && 0x0f)) {
			_delay_cs(50); // 0.5s
			onewire_command(0x44, NULL);

			while ( !onewire_bit(1) );
			send_jeden();

			_delay_ds(50); // 0.5s
			onewire_command(0xbe, termometry + 8*(term_num>>4));
			t0 = onewire_byte(0xff);
			t1 = onewire_byte(0xff);

			// alatm pozarowy
			if((t1 << 4 | t0 >> 4) > 65 && temperatury[term_num>>4] > 65) {
				system_status |= 0x08;
				send_rs_cmd(_INFO_MODULE_ID_, 0x40 | (t1 << 3 | t0 >> 5), 0x84); // temperatura
			}

			// zapamietanie temperatury
			temperatury[term_num>>4] = t1 << 8 | t0

			term_num++;
		} else if (term_num > _LICZBA_TERMOMETROW_ * 16) {
			term_num = 0;
		} else {
			term_num++;
		}

		/// obsługa ogólnego wyjścia sygnalizacji alarmowej
		if (system_status & 0x1c)
			PORTB |= 0x80; // PB7 = ON
		else
			PORTB &= 0x7f; // PB7 = OFF
	}
}
