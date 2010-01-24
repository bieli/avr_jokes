





/**
 * Plik stanowi zestaw funkcji do obsługi interfejsu 1-wire
 * do wykorzystania zarówno w mikrokontrolerach jak i sprzęcie PC
 * konieczne jest wstawienie odpowiedniej wersji definiowania funkcji:
 *   send_zero() send_jeden() read() - patrz poniżej
 *
 * a także włączenie "opoznienia.c"
 * lub odpowiednika (np. dla PC - patrz onewire_get_rom.c)
**/

/*
#define send_zero() outb(0x00, 0x37a)
#define send_jeden() outb(0x04, 0x37a)
#define read() (inb(0x37a) & 0x04) >> 2

#define send_zero() DDRB |= 0x40; PORTB &= 0xbf
#define send_jeden() PORTB |= 0x40; DDRB &= 0xbf
#define read() (PINB & 0x40) >> 6
*/

void onewire_reset() {
	cli();

	send_zero();
	_delay_ms( 0.5 ); // 500 us
	send_jeden();
	sti();
	_delay_ms( 10 );
}

bool onewire_bit(bool b) {
	cli();

	send_zero();
	if (b) send_jeden();
	_delay_us( 20 );
	b = read();
	_delay_us( 50 );
	send_jeden();

	sti();
	return b;
}

unsigned char onewire_byte(unsigned char b) {
	char c;
	for (c=8; c; c--)
		b = ( onewire_bit( b & 0x01 ) << 7 ) | ( b >> 1 );
		// w najstarszy bit zapisuje zczytana wartosc
		// wykonuje binarne lub z
		// poprzednia wartoscia b przesunieta o 1 w prawo
	return b;
}

void onewire_command(unsigned char command, unsigned char *adres) {
	onewire_reset();

	if (adres == NULL) {
		onewire_byte( 0xcc ); // skip room
	} else {
		char c;
		onewire_byte( 0x55 ); // match rom
		for (c=8; c; c--) {
			onewire_byte(*adres);
			adres++;
		}
	}
	onewire_byte(command);
}

