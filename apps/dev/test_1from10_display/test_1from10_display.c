

#include <avr/io.h>
#include <inttypes.h>
//#include <avr/interrupt.h>  // potrzebne do przerwan

#define FCPU  1000000

#define ONE_SECOND  1000
#define F_CPU FCPU
#include <util/delay.h>	


/* delay for a minimum of <ms> */
void delay_ms(unsigned int ms)
{
    // we use a calibrated macro. This is more
    // accurate and not so much compiler dependent
    // as self made code.
    while(ms)
    {
        _delay_ms(0.96);
        ms--;
    }
}

void set_led_1from10_7442(unsigned char led_nr)
{
    if ( 10 < led_nr )
    {
        return ;
    }

    int n = 0;

    static char bcd_1from10_7442[11][4] = {
            {0,0,0,0},
            {0,0,1,0},
            {0,0,0,1},
            {0,0,1,1},
            {0,1,0,0},
            {0,1,1,0},
            {0,1,0,1},
            {0,1,1,1},
            {1,0,0,0},
            {1,0,1,0},
            {1,0,0,1}
    };

    PORTD = bcd_1from10_7442[ led_nr ][ 0 ] * 8
          + bcd_1from10_7442[ led_nr ][ 1 ] * 4
          + bcd_1from10_7442[ led_nr ][ 2 ] * 2
          + bcd_1from10_7442[ led_nr ][ 3 ] * 1;
}


void flash_led(unsigned char nr, unsigned char bit)
{
    int n = 0;

    for ( n = 0; n < nr; n++ )
    {
        PORTD |= _BV(bit);
        delay_ms(ONE_SECOND);
        PORTD &= ~_BV(bit);
        delay_ms(ONE_SECOND);
    }
}

int main(void)
{
    int n = 0;

    DDRD = 0xFF;

    PORTD = 0;

    while(1)
    {
        if ( 10 < n)
        {
            n = 0;
        }

        set_led_1from10_7442(n);

        delay_ms(ONE_SECOND);
/*
// pokazuje skale - szybkie migniecie diody gornej i dolnej
        set_led_1from10_7442(0);
        delay_ms(10);
        set_led_1from10_7442(9);
        delay_ms(10);
*/
        n++;
    }

        return(0);
}

