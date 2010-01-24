/**
 * Plik stanowi zestaw funkcji do obsługi wyświetlacza na bazie HD44780
 * do wykorzystania zarówno w mikrokontrolerach jak i sprzęcie PC
 * konieczne jest wstawienie odpowiedniej wersji definiowania funkcji:
 *   lcd_send() - patrz poniżej
 *
 * a także włączenie "opoznienia.c"
 * lub odpowiednika (np. dla PC - patrz onewire_get_rom.c)
**/


/*
#define lcd_send(byte) outb(byte, 0x378)

#define lcd_send(byte) PORTB = byte
*/

void lcd_send_comand2(unsigned char val) {
	val &= 0x0f;

	lcd_send(0x00); // RS = 0 E = 0
	lcd_send(val | 0x20); // E =1
	_delay_ms(1);
	lcd_send(0x00); // E = 0
}

void lcd_send_data2(unsigned char val) {
	val &= 0x0f;

	lcd_send(0x10); // RS = 1 E = 0
	lcd_send(val | 0x30); // E = 1
	_delay_ms(1);
	lcd_send(0x10); // E = 0
}

/*
#define lcd_send_comand(val) lcd_send_comand2( val >> 4 ); lcd_send_comand2( val & 0xf )
#define lcd_send_data(val) lcd_send_data2( val >> 4 ); lcd_send_data2( val & 0xf )
#define lcd_send_string(napis, dlugosc) {unsigned char lcd_i;\
 for (lcd_i=0; lcd_i<dlugosc; lcd_i++) {lcd_send_data2( napis[lcd_i] >> 4 );\
 lcd_send_data2( napis[lcd_i] & 0xf );}}
*/

void lcd_send_comand(unsigned char val) {
	lcd_send_comand2( val >> 4 );
	lcd_send_comand2( val & 0xf );
}

void lcd_send_data(unsigned char val) {
	lcd_send_data2( val >> 4 );
	lcd_send_data2( val & 0xf );
}

void lcd_send_string(const unsigned char *napis, unsigned char dlugosc) {
	unsigned char lcd_i;
	for (lcd_i=0; lcd_i<dlugosc; lcd_i++) {
		lcd_send_data2( napis[lcd_i] >> 4 );
		lcd_send_data2( napis[lcd_i] & 0xf );
	}
}

// First line (Return Home) = 0000 0010
#define lcd_first_line() lcd_send_comand(0x2)
// Second line (Return Home 2) = 1100 0000
#define lcd_second_line() lcd_send_comand(0xc0)

/* void lcd_start() {
	lcd_send_comand2 (0x2); lcd_send_comand (0x28); // Function Set (2 linie 4 bit) = 0010, 0010 10**
	lcd_send_comand (0xc); // On-Off Control (bez pokazywania kursora) = 0000 1100
	lcd_send_comand (0x6); // Entry Mode Set = 0000 0110
} */
