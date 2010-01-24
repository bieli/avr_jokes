/**
 *
 * Miganie dioda na okrelonym porcie
 *
 * Gdy swieci dioda wygaszony jest buzzer, lecz gdy buzzer wydaje dziwek wygaszona jest dioda
 *
 */

#include <inttypes.h>
#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <avr/sleep.h>

#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>

#include "../utils/iocompat.h"

#include "../utils/mb_avr_simple_board.h"

// my prototype board AVR_PB_001
//#define LED_RED         PB2
//#define BUZZER          PB1
//#define KEY_LED_RED (PIND & _BV(PIND4))
//#define KEY_BUZZER  (PIND & _BV(PIND5))

// definicja podlaczenia czerownej diody
#define INIT__LED_RED       OUTPUT_INIT(B, 3);

// definicja podlaczenia buzzera
#define INIT__BUZZER        OUTPUT_INIT(B, 1);

// klawisze podciągnięte do zasilania
#define INIT__KEY_LEFT      INPUT_INIT(D, 4);
#define INIT__KEY_DOWN      INPUT_INIT(D, 5);


// definicja sterowania czerowna dioda
#define SET__LED_RED(STATE)       OUTPUT_SET_ ## STATE (B, 3);

// definicja sterowania czerowna buzerem
#define SET__BUZZER(STATE)        OUTPUT_SET_ ## STATE (B, 1);

// definicja sprawdzania czy wcisnieto klawisz
#define CHECK__KEY_LEFT           INPUT_CHECK(D, 4);
#define CHECK__KEY_DOWN           INPUT_CHECK(D, 5);




void ioinit (void)
{
    // RED LED
    INIT__LED_RED;

    // BUZZER
    INIT__BUZZER;

    // ncjalizuej klawisz LEFT
    INIT__KEY_LEFT;

    // ncjalizuej klawisz DOWN
    INIT__KEY_DOWN;
}

void fast_blinking (unsigned char repeat, unsigned char repeats)
{
    unsigned int delay = 50;
    unsigned char n = 0;
    unsigned char n1 = 0;
    unsigned char d = 0;

    for ( n1 = 0; n1 <= repeats; n1++ )
    {
        for ( n = 0; n <= repeat; n++ )
        {
            d++;
            SET__LED_RED(0);

            delay_ms(delay * d);

            SET__LED_RED(1);

            delay_ms(delay * d);
        }

        for ( n = 0; n <= repeat; n++ )
        {
            d--;
            SET__LED_RED(0);

            delay_ms(delay * d);

            SET__LED_RED(1);

            delay_ms(delay * d);
        }
    }
}


int main (void)
{
    unsigned char key;
    unsigned int delay = 500;
    unsigned int step  = 100;

    ioinit ();

//    SET__LED_RED(1);
//    delay_ms(3000);

//    fast_blinking(7, 100);

    for (;;)
    {
//        if ( CHECK__KEY_LEFT )
        if ( bit_is_clear(PIND, PD4) )
        {
            if ( (step * 100) < delay )
            {
                delay += step;
            }
            else
            {
                delay = step * 100;
            }
        }
        
//        if ( CHECK__KEY_DOWN ) 
        if ( bit_is_clear(PIND, PD5) )
        {
            if ( (step * 2) < delay )
            {
                delay -= step;
            }
            else
            {
                delay = step;
            }
        }


        // led on, pin=0
        SET__LED_RED(1);

        // buzzer off
        SET__BUZZER(0);

        delay_ms(delay);

        // led off, pin=1
        SET__LED_RED(0);

        // buzzer on
        SET__BUZZER(1);

        delay_ms(delay);
    }

    return (0);
}

