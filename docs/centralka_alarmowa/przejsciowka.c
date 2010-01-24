/**
 * program dla mikrokontrolera Atmega8 stanowiącego interfejs
 * pomiędzy portem szeregowym RS, a 1wire, I2C, SPI, 8bit (równoległey)
 *
 * informacje z rs odbierane są w trybie przerwaniowym,
 * po odebraniu informacji sprawdzany jest stan PB0 (zworka CONF1) ->
 * jeżeli jest zwarta do GND następuje odczytanie informacji o trybie pracy
 * i wykonanie odpowiedniej akcji konfiguracyjnej, obsługiwane tryby
 *  0x01 -> SPI (dane z RS wysyłane na SPI, dane z SPI odbierane przez przerwanie i wysyłane do RS)
 *  0x02 -> I2C/TWI Master (jak wyżej)
 *  0x03 -> 1 wire (komunikat RS składa się w ogólności z kilku bajtów:
 *    pierwszy bajt informuje o trybie pracy 1wire {bit 0 = wykonywać reset, bit 1 = wysyłać
 *    pominięcie ROM /w przeciwnym razie oczekiwany i wysyłany ROM/ , bit 2 = wysłać 0xff,
 *    bit 3 = 1 /musi być ustawiony gdyby inaczej bajt miał wartość 0/, bity 4-7 = ilość bajtów komendy}
 *    następnie w zależności od wartości pierwszego bajtu następują kolejno: adres ROM (8 bajtów)
 *    i ciąg bajtów komendy {należy zaznaczyć że nie wszystkie ustawienia flag pierwszego bajtu mają sens,
 *    w szczególności ustawienie bitu 2 wymaga wyzerowania wszystkich pozostałych,
 *    wysłanie takiego pakietu powoduje odczyt 1 bajtu danych z 1-wire i wysłanie go na RS
 *    rozpoczęcie transmisji wymaga ustawienia bitu zerowego, może być także ustawiony bit pierwszy}
 *  0x04 -> 8bit OUT (zapis do RS powoduje wysłanie danych na wyjście równoległe)
 *  0x05 -> 8bit IN (zapis do RS powoduje odczyt danych z wejścia równoległego i wysłanie ich na RS)
 *  0x06 -> 8bit IN-OUT 1/2 (dla bitów 0..3 IN dla bitów 4..7 OUT)
 *  0x07 -> 8bit IN-OUT FULL (zapis do RS powoduje odczyt z równoległych,
 *    zapis nowej wartości na równoległe i wysłanie odczytanej na RS)
 *  0x08 -> 8bit PRZERWANIOWY (przerwanie INT0 powoduje odczyt danych z linii równoległych i wysłanie na RS)
 *  @ wszystkie tryby 8bit -> zapis na linie równoległe powoduje zmianę stanu linii INT1
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
 */


#include <inttypes.h>

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 1000000UL
#include <util/delay.h>

/** nagłówki OneWire **/
#define send_zero() DDRB |= 0x02; PORTB &= 0xfd
#define send_jeden() PORTB |= 0x02; DDRB &= 0xfd
#define read() (PINB & 0x02) >> 6
#include "onewire.c"

/// tryb w którym prcuje układ
volatile uint8_t config_mode;

/// licznik timera programowego
volatile int8_t soft_timer;

/** obsluga portu szeregowego **/
uint8_t uart_buf_head, uart_buf_body, chanel, cmd = 0;
ISR(USART_RXC_vect) {
	cli();
	uint8_t tmp, dane;
	uint8_t rom[8];
	uint8_t rom_num;
	if (PINC & 0x01) { /// TRYB KONFIGURACYJNY
		// zapamietanie identyfikatora trybu
		config_mode = UDR;

		// wyczyszczenie wszystkich wczesniejszych ustawień
		TCCR0 = 0;
		GIMSK = 0; MCUCR = 0;
		DDRB = 0; PORTB = 0x01;
		DDRC = 0; PORTC = 0;
		DDRD = 0; PORTD = 0;
		TWCR = 0;

		// wprowadzenie nowych ustawień
		switch config_mode {
			case 0x01:
				// podajemy RESET dla ISP, MOSI i SCK na output
				DDRB=0x2c; PORTB=0x01;
				// właczenie SPI jako master z zegarem = 1/128 zegara własnego i generowaniem przerwań
				SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1) | (1<<SPIE);
				//FIXME jakie ustawienia poniższych parametrów do programowania przez ISP ?
				//DORD - kolejnośc transmisji bitów w bajcie
				//CPOL - który stan zegara aktywny a który nie
				//CPHA - przy którym zboczu odbiór danych
				break;
			case 0x02:
				tmp = 0; // stan transmisji - nierozpoczęta
				PORTC = 0x30; // rezystory podciągające
				TWSR = 0xfa; // preskaler == 16
				TWBR = 100; // = CPU_clock / (SLC_clock*2*4^16) - 16, >10
				TIMSK = _BV(TOIE0); // przerwanie zegarowe
				break;
			case 0x03:
				tmp = 0;
				break;
			case 0x04:
				DDRC = 0x0f; PORTC = 0;
				DDRD = 0xf8; PORTD = 0;
				tmp = 0x00;
				break;
			case 0x05:
				DDRC = 0; PORTC = 0x0f;
				DDRD = 0; PORTD = 0xf8;
				break;
			case 0x06:
				DDRC = 0; PORTC = 0x0f;
				DDRD = 0xf8; PORTD = 0;
				tmp = 0x00;
				break;
			case 0x08:
				// wyjściowy (wejście z INT0 == PD2)
				DDRC = 0x0f; PORTC = 0;
				DDRD = 0xf8; PORTD = 0;
				// przerwania z int0
				GIMSK = (1 << INT0);
				MCUCR = (1 << ISC00) | (1 << ISC01);
				break;
		}
	} else { /// TRYB PRACY
		dane = UDR;
		switch config_mode {
			case 0x01:
				SPDR = dane;
				// czekanie na koniec
				while(!(SPSR & (1<<SPIF)));
				break;
			case 0x02:
				/// START
				if (tmp == 0) {
					// zaczynamy, master, włączony, przerwanie
					TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA);
					// czekamy na koniec inicjalizacji
					while(!(TWCR & (1<<TWINT)));
				}
				/// normalna transmisja
				TWDR = dane;
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA);
				while (!(TWCR & (1<<TWINT)));
				/// STOP w oparciu o przerwanie z TIMER 10s
				TCNT0 = 0; // ustawienie wartości początkowej TIMER0
				soft_timer = 120;
				TCCR0 = 0x05; // wzrost wartości TIMER0 co ... taktów zegara głównego (5 => 1024)
				break;
			case 0x03:
				if (tmp==0) {
					tmp = dane;
					soft_timer = (tmp >> 4) & 0x0f;
					rom_num = 0;
				} else if (tmp & 0x01) { // wykonujemy reset
					if (tmp & 0x02) { // wysyłamy ROM
						if(rom_num < 8) {
							rom[rom_num++] = dane;
						} else {
							onewire_command(dane, rom);
							while ( !onewire_bit(1) );
							soft_timer--;
						}
					} else {
						onewire_command(dane, NULL);
						while ( !onewire_bit(1) );
						soft_timer--;
					}
				} else if (tmp & 0x04) { // nie wykonujemy resetu, odczyt
					UDR = onewire_byte(0xff);
					tmp = 0;
				} else { // nie wykonujemy resetu
					onewire_byte(dane);
					soft_timer--;
				}
				if (soft_timer == 0)
					tmp = 0;
				break;
			case 0x07:
				DDRC = 0; PORTC = 0x0f;
				DDRD = 0; PORTD = 0xf8;

				_delay_us(2);
				UDR = (PINC & 0x0f) | (PIND & 0xf0);

				DDRC = 0x0f; DDRD = 0xf8;
				// nie przerywamy - trzeba jesze ustawić nową wartość
			case 0x04:
			case 0x08:
				tmp = ~tmp;
				PORTC = dane & 0x0f;
				PORTD = (dane & 0xf0) | (tmp & 0x08);
				break;
			case 0x06:
				tmp = ~tmp;
				UDR = (PINC & 0x0f);
				PORTD = (dane & 0xf0) | (tmp & 0x08);
				break;
			case 0x05:
				UDR = (PINC & 0x0f) | (PIND & 0xf0);
				break;
		}
	}
	sei();
}

/** obsluga przerwania z SPI/ISP **/
SIGNAL(SIG_SPI) {
	UDR = SPDR;
}

/** obsluga przerwania z I2C/TWI **/
ISR(TWI_vect) {
	UDR = TWDR;
	// wyczyszenie TWINT
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA);
}

/** obsluga przerwania z INT0 **/
SIGNAL (SIG_INTERRUPT0) {
	uint8_t c, d;
	c = PORTC; d = PORTD;
	DDRC = 0; PORTC = 0x0f;
	DDRD = 0; PORTD = 0xf8;

	_delay_us(2);
	UDR = (PINC & 0x0f) | (PIND & 0xf0);

	DDRC = 0x0f; PORTC = c;
	DDRD = 0xf8; PORTD = d;
}

/** obsługa przerwania z TIMER0 **/
SIGNAL (SIG_OVERFLOW0) {
	if (--soft_timer == 0) {
		/// wyłączamy TIMER0
		TCCR0 = 0;

		/// sygnał STOP dla I2C/TWI
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO) | (1<<TWIE);
		while ((TWCR & (1<<TWSTO)));
	}
}


main() {
	/** port szeregowy - inicjalizacja **/
	/// włączenie nadajnika, odbionika UART wraz z generowaniem przerwań
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	/// ustawienie prędkości (BAUD), UBRR = F_CPU / (16*BAUD) -1
	UBRRH = 0; UBRRL = 19; // BAUD = 9600 przy 3 MHz
	//UBRRH = 0; UBRRL = 207; // BAUD = 300 przy 1 MHz

	/** włączenie przerwań **/
	sei();

	/** konfiguracja portów IO mikrokontrolera **/
	/// PB0 - zworka konfiguracyjna
	DDRB = 0;
	PORTB = 0x01;

	/// PB1 - One Wire
	/// PC0-PC3, PD4-PD7 - 8 bit IO
	/// PD2, PD3 - zworki dodatkowe
	/// Konfigurowane w zależności od trybu pracy ...

	/** petla glowna **/
	while (1);
}
