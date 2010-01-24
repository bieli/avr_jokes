/**
 * plik biblioteczki opóźniającej
 * zawiera funkcję _delay_cs(uint8_t) wstawiającą opóźnienie do 2.56 sekundy
 * inkluduje także util/delay.h z funkcjami
 *  void _delay_ms (double ms)
 *   -> MAX: 262.14 ms / F_CPU in MHz
 *      for 22MHz MAX=11.915
 *  void _delay_us (double us)
 *   -> MAX: 768 us / F_CPU in MHz
 *      for 22MHz MAX=34.909
 **/

#ifndef F_CPU
	#error "F_CPU must be declared before use opoznienia.c"
#endif

#if F_CPU > 22000000L
	#error "F_CPU must be < 22000000L (22MHz)"
#endif

#include <inttypes.h>
#include <util/delay.h>

/** pętla opóźniająca w centy-sekundach (0.01s) - bez gwarancji precyzji
	maksymalna wartość opóżnienia to 256 cs czyli 2.56 sekund **/
void _delay_cs(uint8_t licz) {
	for (; licz>0; licz--){
		_delay_ms(10);
	}
}
