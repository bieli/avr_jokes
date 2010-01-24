/*********************************************
* vim:sw=8:ts=8:si:et
* To use the above modeline in vim you must have "set modeline" in your .vimrc
* Author: Guido Socher, Copyright: GPL V2
* This program is to test basic functionallity by getting an LED to blink.
* See http://tuxgraphics.org/electronics/
* Chip type           : ATMEGA88
*********************************************/
#include <avr/io.h>
#include <inttypes.h>
#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>


void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
        // we use a calibrated macro. This is more
        // accurate and not so much compiler dependent
        // as self made code.
        while(ms){
                _delay_ms(0.96);
                ms--;
        }
}

int main(void)
{
        /* INITIALIZE */
        // Be very careful with low frequencies (less than 1MHz). Modern and fast programmers 
        // can not supply such low programming clocks. It can lock you out!
        //
        // set the clock prescaler. First write CLKPCE to enable setting of clock the
        // next four instructions.
//        CLKPR=(1<<CLKPCE);
//        CLKPR=0; // 8 MHZ
        //CLKPR=(1<<CLKPS0); // 4MHz
        //CLKPR=((1<<CLKPS0)|(1<<CLKPS1)); // 1MHz
        //CLKPR=((1<<CLKPS0)|(1<<CLKPS2)); // 0.25MHz

        /* enable PB1 as output */
        DDRB|= (1<<DDB1);

        while (1) {
                    /* led on, pin=0 */
                    PORTB &= ~(1<<PB1);
                    delay_ms(500);
                    /* set output to 5V, LED off */
                    PORTB|= (1<<PB1);
                    delay_ms(500);
        }
        return(0);
}
