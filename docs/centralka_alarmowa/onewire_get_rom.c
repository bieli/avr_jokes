/**

 * program do odczytu numeru ROM urządzenia OneWire
 * w tym celu pojedyńcze urządzenie podłączyć do komputera PC
 * (w obecnej konfiguracji linia danych do pinu 16 portu LPT,
 * należy też podłączyć masę, zasilania nie trzeba podłączać)
**/


/*  */

#include <stdio.h>
#include <sys/io.h>

#define bool char

#define cli() asm("cli")
#define sti() asm("sti")


long long czas() { asm( "rdtsc" ); }
void _delay_us( long long l ) {
	l = l * 3012 + czas(); // prędkość procesora w MHz
	while( l > czas() );
}
#define _delay_ms(a) _delay_us( 1000*(a) )


#define send_zero() outb(0x00, 0x37a)
#define send_jeden() outb(0x04, 0x37a)
#define read() (inb(0x37a) & 0x04) >> 2

#include "onewire.c"

main(){
	iopl (3);

	unsigned char i, c;
	onewire_reset();
	onewire_byte(0x33);

	printf("const char rom[8] = {");
	for ( i = 0; i < 8 ; ++i ) {
		c = onewire_byte(0xff);
		printf("0x%02x", c);
		if (i<7) printf(", ");
	}
	printf("};\n");
}
