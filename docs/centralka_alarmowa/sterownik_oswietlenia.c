/*
 * program sterownika oświetlenia z funkcją zdalnego sterowania poprzez port RS
 * dla mikrokontrolera Atmega8
 *
 *
 * kompilacja i wgranie przez ISP:
 *  avr-gcc -mmcu=atmega8 -Os -o sterownik.o sterownik_oswietlenia.c
 *  avr-objcopy -O ihex sterownik.o sterownik.hex
 *  uisp -dprog=dapa -dlpt=0x378 --erase --upload --verify --segment=flash if=sterownik.hex
 *  w razie problemow warto przeladowac modul odpowiedzialny za obsluge LPT
 *    rmmod lp parport_pc parport ; modprobe parport_pc
 *
 * konfiguracja układu na zewnętrzny kwarc 3MHz
 *  uisp -dprog=dapa -dlpt=0x378 --wr_fuse_l=0xEF
 */


#include <inttypes.h>

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/** nagłówki funkcji opóżniających **/
#define F_CPU 3000000UL
#include "opoznienia.c"

/** nagłówki mojego formatu transmisji przez RS485 **/
#define DRIVER_CTRL_PORT PORTD
#define DRIVER_CTRL_PIN PD7
//#define DRIVER_CTRL_PIN_SET_DDR DDRD
#include "my_rs485.c"

#define ZMIANA_OPOZNIENIA 0.05

/// identyfikator urządzenia liczba z zakresu 0 - 63
#define _DEV_ID_ 0x01

/// komendy RS wysyłane przez guziki specjalne
#define PD6_RS_CMD send_rs_cmd(0x01, 0x46, 0x82)

/// tryb debugowania (nie używać w sieci RS)
//#define DEBUG 1

#ifdef DEBUG
# warning "Enable DEBUG MODE - don't use in RS network"
# define DEBUG_SEND_OPOZNIENIE (uint8_t)(opoznienie/ZMIANA_OPOZNIENIA)
uint8_t kontr=0;
#endif


/// maska bitow przechowuąca informacje o zablokowanych liniach przełącznika
volatile uint16_t block_mask = 0;

/// maska bitowa przechowująca informację o stanie linii
volatile uint8_t line_status=0x00;

/// opoznienie sciemnienia
volatile double opoznienie=0.00;
volatile uint8_t opoznienie_ctrl=0;

// wykozystywane w powyższych deklaracjach volatile pozwala na
// kozystanie z tej samej zmiennej w przerwaniach i w main()
// umożliwia uniknięcie stosowania sztuczek wskaznikowych typu:
//  double * opoznienie_wsk = &opoznienie;
//  opoznienie = *opoznienie_wsk;

/** obsluga portu szeregowego **/
uint8_t uart_buf_head, uart_buf_body, chanel, cmd = 0;
ISR(USART_RXC_vect) {
	cli();

	uart_buf_head = UDR;
#ifdef DEBUG
	UDR = uart_buf_head; // echo
#endif
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
				case 0x01: /// read
					if ( (line_status & (1 << (chanel - 1))) != 0 )
						send_rs(0x01); /// YES
					else
						send_rs(0x02); /// NO
					break;
				case 0x02: /// on
					line_status = line_status | (1 << (chanel - 1));
					break;
				case 0x03: /// off
					line_status = line_status & ~(1 << (chanel - 1));
					break;

				case 0x04: /// block read
					if ( (block_mask & (1 << (chanel - 1))) != 0 )
						send_rs(0x01); /// YES
					else
						send_rs(0x02); /// NO
					break;
				case 0x05: /// block on
					// chanel == 7,8  => linie specjalne (PD6, PD3)
					// chanel == 9,10 => ściemnienie (up, down)
					block_mask = block_mask | (1 << (chanel - 1));
					break;
				case 0x06: /// block off
					block_mask = block_mask & ( ~ (1 << (chanel - 1)) );
					break;

				case 0x07: /// ustawienie wartosci sciemnienia
					opoznienie = chanel * ZMIANA_OPOZNIENIA;
					opoznienie_ctrl = chanel;
					break;
				case 0x08: /// odczyt wartosci sciemnienia
					send_rs( opoznienie_ctrl );
					break;

				case 0x09: /// zmiana stanu linii
					line_status = line_status ^ (1 << (chanel - 1));
					break;
#ifdef DEBUG
				case 0x0a:
					UDR=PIND;
					break;
				case 0x0b:
					MCUCR = (1 << ISC00) | (1 << ISC01); // narastajace
					opoznienie=0.00; opoznienie_ctrl = 0;
					break;
				case 0x0c:
					MCUCR = 0; // niski
					opoznienie=0.00; opoznienie_ctrl = 0;
					break;
				case 0x0d:
					MCUCR = (1 << ISC00); // zmiana stanu
					opoznienie=0.00; opoznienie_ctrl = 0;
					break;
				case 0x0e:
					MCUCR = (1 << ISC01); // opadajace
					opoznienie=0.00; opoznienie_ctrl = 0;
					break;
#endif
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

/** obsluga przerwania powodowanego przejsciem przez zero fazowe **/
SIGNAL (SIG_INTERRUPT0) {
	_delay_ms(0.00 + opoznienie); // MAX 0x3f * ZMIANA_OPOZNIENIA < 4ms
	PORTB = line_status;
	_delay_us(10); // 10us

	if (opoznienie_ctrl != 0)
		PORTB = line_status & 0xdf; // wyłaczamy PIN6

#ifdef DEBUG
	//if (kontr++ == 100) {kontr=0; send_rs(DEBUG_SEND_OPOZNIENIE);}
#endif
}


main() {
	/** konfiguracja portów IO mikrokontrolera **/
	/// port B - wyjściowy
	DDRB = 0xff;
	PORTB = 0;

	/// port C - wejściowy z rezystorami podciągającymi
	DDRC = 0;
	PORTC = 0xff;

	/// port D - wyjściowo/ wejściowy:
	///  PD2 wejsciowy bez podciagajacego
	///  PD3 wejsciowy z podciagajacym
	///  PD4-PD5 wyjściowe/wejsciowe z podciagajacym
	///  PD6 wejsciowy z podciagajacym
	///  PD7 - wyjsciowy, wyłączony
	DDRD = (1 << PD4) | (1 << PD5) | (1 << PD7);
	PORTD = (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6);

	/** port szeregowy - inicjalizacja **/
	/// włączenie nadajnika, odbionika UART wraz z generowaniem przerwań
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	/// ustawienie prędkości (BAUD), UBRR = F_CPU / (16*BAUD) -1
#if F_CPU == 3000000UL
	UBRRH = 0; UBRRL = 19; // BAUD = 9600 przy 3 MHz
#elif F_CPU == 1000000UL
	UBRRH = 0; UBRRL = 207; // BAUD = 300 przy 1 MHz
#	warning "BAUD = 300 < 9600"
#else
#	error "Nieznane ustawienie UBRR dla zadanego F_CPU"
#endif

	/** przerwania z int0 **/
	GIMSK = (1 << INT0);
	MCUCR = (1 << ISC01);

	/** włączenie przerwań **/
	sei();

	/** petla glowna **/
	uint8_t i, port_in, last=0x00;
	while (1) {
		/// sterowanie ściemnieniem
		port_in = PIND;

		// świecenie LED na liniach ściemniania + na razie wygaszamy
		DDRD = (1 << PD4) | (1 << PD5) | (1 << PD7);
		PORTD = (1 << PD3) | (1 << PD6);

		//gdy możemy zmniejszać
		if ( (opoznienie_ctrl > 0) && (0 == (block_mask & (1 << 8))) ) {
			// gdy polecono zmniejszanie
			if ( ((port_in & 0x10) == 0) ) {
				opoznienie_ctrl--;
				opoznienie = opoznienie_ctrl * ZMIANA_OPOZNIENIA;
				#ifdef DEBUG_SEND_OPOZNIENIE
				UDR=DEBUG_SEND_OPOZNIENIE;
				#endif
			}
			PORTD = PORTD | (1 << PD4);
		}

		// gdy możemy zwiększać
		if ( (opoznienie_ctrl < 0x3f) && (0 == (block_mask & (1 << 9))) ) {
			// gdy polecono zwiększanie
			if ( (port_in & 0x20) == 0 ) {
				opoznienie_ctrl++;
				opoznienie = opoznienie_ctrl * ZMIANA_OPOZNIENIA;
				#ifdef DEBUG_SEND_OPOZNIENIE
				UDR=DEBUG_SEND_OPOZNIENIE;
				#endif
			}
			PORTD = PORTD | (1 << PD5);
		}


		/// sterowanie on-off
		port_in = (PINC & 0x3f) | (port_in & 0x40) | ((port_in << 4) & 0x80); // zapamietujemy stan portu
		for (i=0; i<6; i++) {
			if ( 0 != (block_mask & (1 << i)) )
				continue;

			if ( (0 == (port_in & (1 << i))) && (0 != (last & (1 << i))) ) {
				// jest stan niski a poprzednio byl wysoki wiec zmieniamy stan wyjscia
				line_status = line_status ^ (1 << i);
				#ifdef DEBUG
				UDR=line_status;
				#endif
			}
		}


		/// wysyłanie komend RS
		if ( (0 == (port_in & (1 << 6))) && (0 != (last & (1 << 6))) && (0 == (block_mask & (1 << 6))) ) {
			PD6_RS_CMD;
		}
		/* nie działa poprawnie :-( ... konflikt z int0 :-s
		if ( (0 == (port_in & (1 << 7))) && (0 != (last & (1 << 7))) && (0 == (block_mask & (1 << 7))) ) {
			PD3_RS_CMD;
		}*/
		last = port_in; // zapamietujemy poprzedni stan portu


		/// zapobieganie zawieszeniom ...
		PORTC = 0xff;

		/// zapobieganie szumom z przelacznikow oraz opoznienie
		/// dla regulacji ciągłej ściemnienia i sygnalizacji LED
		_delay_cs(5);

		/// musi być możliwość odczytu linii ściemniania - koniec świecenia, ustawienie podciągających
		DDRD = (1 << PD7);
		PORTD =  (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6);
		_delay_us(10);
	}
}
