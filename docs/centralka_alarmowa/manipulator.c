/**
 * program manipulatora systemu alarmowego opartego na mikrokontrolerze Atmega8
 * komunikującego się z centralką poprzez RS485
 *
 * do wejścia przerwaniowego INT0 podłączony jest kontrakton, każda zmiana stanu
 * na tym wejściu powoduje wysłanie informacji do centralki z aktualnym stanem,
 * po wysłaniu oczekujemy na odpowiedź przez 500ms,
 * jeżeli nie dostajemy potwierdzenia ponawiamy 3 krotnie
 * jeżeli po 3 próbach nie otrzymaliśmy potwierdzenia wchodzimy w stan awarii
 * (naprzemienne miganie obu diód, wysyłanie infa o stanie awari na RS co 1 minutę)
 *
 * otwarcie drzwi przy załączonym alarmie powoduje wyemitowanie sygnału dźwiekowego
 * i miganie diody rozbrojenia alarmu (PB1)
 * zamknięcie drzwi przy nieuzbrojonym alarmie powoduje wyemitowanie sygnału dźwiekowego
 * i miganie diody uzbrojenia alarmu (PB2)
 *
 * odbierane dane z RS (w przerwaniu) o stanie alarmu (uzbrojeniu/rozbrojeniu) powodują
 * ustawienie odpowiedniego stanu diód i wyłączenie sygnału akustycznego,
 * a następnie wysłanie potwierdzania
 *
 * w pętli głównej następuje odczyt danych z klawiatury
 * kod wysyłany jest w postaci:
 *   urzadzenie - kanal (pierwsza_cyfra_kodu) - cmd (ozn. że kod)
 *   urzadzenie - kanal (druga_cyfra_kodu) - cmd (ozn. że kod) itd
 *  -> wprowadzenie dowolnej liczby powduje ustawienie timera na 15 sek, po ich upływie wysłane jest
 *     kasowanie kodu i nadany sygnał aktustyczny o tym informujący
 * w pętli głównej następuje równierz odczyt wejścia z dzwonka i domofonu
 * reakcją na nie jest wysłanie komunikatu RS z informacją o źródle sygnału
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

/** nagłówki mojego formatu transmisji przez RS485 **/
#define DRIVER_CTRL_PORT PORTD
#define DRIVER_CTRL_PIN PD3
#include "my_rs485.c"

/// identyfikator urządzenia liczba z zakresu 0 - 63
#define _DEV_ID_ 0x3e

/// identyfikator manipulatora
#define _CENTR_ID_ 0x3f

/// status systemu
volatile int8_t system_status;
// -1 == awaria, 0 == rozbrojony, 1 == czekamy na uzbrojenie, 2 == uzbrojony 3 == czekamy na rozbrojenie
// volatile umożliwia korzystanie zarówno w przerwaniach jak i w main,
// zastępuje zabawę z wskażnikami jaką stosowałem w sterowniku oświetlenia

/// potwierdzenie odbioru przez RS
volatile int8_t potwierdzenie;

/// licznik timera programowego
volatile int8_t soft_timer;

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
				case 0x01: /// uzbrojony
					system_status = 2
					send_rs_cmd(_CENTR_ID_, 0x40, 0x85); // potwierdzenie
					break;
				case 0x02: /// rozbrojony
					system_status = 0
					send_rs_cmd(_CENTR_ID_, 0x40, 0x85); // potwierdzenie
					break;
				case 0x03: /// ERROR
					system_status = -1
					break;
				case 0x04: /// code ERROR
					/// sygnał dźwiękowy
					PORTB=PORTB | 0x08; // PB3 = ON
					_delay_cs(200); // 2s
					PORTB=PORTB & 0x7f; // PB3 = OFF
					_delay_cs(50); //0.5s
					PORTB=PORTB | 0x08; // PB3 = ON
					_delay_cs(100); // 1s
					PORTB=PORTB & 0x7f; // PB3 = OFF
					break;
				case 0x05: /// potwierdzenie
					potwierdzenie = 1;
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

/** obsluga przerwania powodowanego kontaktronem **/
SIGNAL (SIG_INTERRUPT0) {
	uint8_t kontr_stat = PIND & 0x04 >> 2;

	/// wysłanie informacji wraz z oczekiwaniem na potwierdzenie
	int8_t j;
	potwierdzenie = 0;
	for(j=0; j<3 && potwierdzenie; j++) {
		send_rs_cmd(_CENTR_ID_, 0x40 | kontr_stat, 0x81); // stan kontaktronu
		_delay_cs(50); // 0.5s
	}

	/// sygnalizacja stanu
	if (! potwierdzenie)
		system_status = -1;
	else if ( (system_status == 2 && kontr_stat == 1) || // kontaktron zwarty / drzwi otwarte + uzbrojony
	     (system_status == 0 && kontr_stat == 0) ) // kontaktron rozwarty / drzwi zamknięte + rozbrojony
			system_status++;
}

/** obsługa przerwania z TIMER0 **/
SIGNAL (SIG_OVERFLOW0) {
	if (--soft_timer == 0) {
		/// wyłączamy TIMER0
		TCCR0 = 0;

		/// sygnał dźwiękowy
		PORTB=PORTB | 0x08; // PB3 = ON
		_delay_cs(100); // 1s
		PORTB=PORTB & 0x7f; // PB3 = OFF
	}
}

main() {
	/** konfiguracja portów IO mikrokontrolera **/
	/// SYGNALIZACJA - led + akustyczna (PB1..3), wyjściowy wyłączony
	DDRB = 0x0e;
	PORTB = 0x00;

	/// WEJŚCIA KLAWIATUROWE (PC0..3), wejściowy bez podciągającego
	/// WEJŚCIE z dzwonka i domofonu (PC4, PC5), wejściowy bez podciągającego
	DDRC = 0x00;
	PORTC = 0x00;

	/// WEJŚCIE Z KONTRAKTONU - przerwaniowe (PD2), wejściowy bez podciągającego
	/// WYJŚĆIE STEROWANIA RS (PD3), wyjściowy wyłączony
	/// WYJŚCIA KLAWIATUROWE (PD4..6), wyjściowy wyłączony
	DDRD = 0x78;
	PORTD = 0x00;

	/// PRZERWANIA Z INT0 (PD2)
	GIMSK = (1 << INT0);
	MCUCR = (1 << ISC00) /*| (1 << ISC01)*/;

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

	/** petla glowna **/
	uint8_t keyboard, old_keyboard;
	uint8_t port_sygn, old_port_sygn;
	int8_t j, i;
	while (1) {
		if (system_status <0) {
			TCCR1A = 0; TCCR1B = 0;
			/// system w stanie awarii
			for (i=0; i<60; i++) {
				PORTB=0x02; // PB1 = ON
				_delay_cs(50); // 0.5s
				PORTC=0x04; // PB2 = ON
				_delay_cs(50); // 0.5s
			}
			send_rs_cmd(_CENTR_ID_, 0x40, 0x83); // błąd systemu
		} else {
			/// system w stanie normalnego działania
			keyboard = 0;
			for (i=0; i<3 && keyboard == 0; i++) {
				// ustawiamy kolejne linie klawiatury jako up
				PORTD = 1 << i
				_delay_ms(5); // 5ms
				// dokonujemy odczytu wartości
				j = PINC & 0x0f;
				// wyszukujemy numer bitu który ma wartośc 1
				while (j >> keyboard) {keyboard++;};
				// uwzgledniamy skale dawana przez linię zasilającą
				keyboard = (i+1) * keyboard
			}
			/**
				1 => 1
				...
				9 => 9
				10 => 0
				11 => akceptacja (#)
				12 => kasowanie (*)
			**/


			if (keyboard != old_keyboard) {
				/// ustawienie timera
				TCNT0 = 0; // ustawienie wartości początkowej TIMER0
				soft_timer = 120;
				TCCR0 = 0x05; // wzrost wartości TIMER0 co taktów zegara głównego (5 => 1024)
				/// 1024*255*120*1/3000000 = 10.4 s

				/// sygnał dźwiękowy
				PORTB=PORTB | 0x08; // PB3 = ON
				_delay_cs(10); // 100ms
				PORTB=PORTB & 0x7f; // PB3 = OFF

				/// wysłanie
				send_rs_cmd(_CENTR_ID_, 0x40 | keyboard, 0x84); // "cyfra" kodu

				/// koniec wysyłania (kasyjemy timer)
				if (keyboard>10) {
					TCCR0 = 0; // wzrost wartości TIMER0 co taktów zegara głównego (0 => timer wyłączony)
					TCNT0 = 0; // ustawienie wartości początkowej TIMER0
				}
			}
			old_keyboard = keyboard;

			port_sygn = PINC & 0x30;
			if (port_sygn > old_port_sygn) {
				for (i=6; i>=5; i--) {
					if ((port_sygn >> i) & 0x01) {
						send_rs_cmd(_CENTR_ID_, 0x40 | i, 0x82); // sygnał na linii domofonu/dzwonka
						break;
					}
				}
			}
			old_port_sygn = port_sygn;

			switch system_status {
				case 0:
					TCCR1A = 0; TCCR1B = 0;
					PORTB=0x02; // PB1 = ON
					break;
				case 1:
					/// sygnał dźwiękowy
					PORTB=PORTB 0x08;
					/// PWM Phase Correct na PB2
					// największy okres to 2*1024*1024*1/3000000 = 0.69905066s i takiego używamy
					TCCR1A = _BV(COM1A1)|_BV(PWM10)|_BV(PWM11);
					TCCR1B = _BV(CS12)|_BV(CS10);
					// przełączenie w 50%
					OCR1AH = 0x01;
					OCR1AL = 0xff;
					break;
				case 2:
					TCCR1A = 0; TCCR1B = 0;
					PORTB=0x04; // PB2 = ON
					break;
				case 3:
					/// sygnał dźwiękowy
					PORTB=PORTB 0x08;
					/// PWM Phase Correct na PB2
					// największy okres to 2*1024*1024*1/3000000 = 0.69905066s
					TCCR1A = _BV(COM1B1)|_BV(PWM10)|_BV(PWM11);
					TCCR1B = _BV(CS12)|_BV(CS10);
					// przełączenie w 50%
					OCR1BH = 0x01;
					OCR1BL = 0xff;
					break;
			}
		}
	}
}
