#include <inttypes.h>
#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <avr/sleep.h>

#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>

#include "iocompat.h"



#define INP_PORT0     PORTD
#define INP_DDR0      DDRD
#define INP_REG0      PIND
#define INP_PIN0      PIND4

#define INP_PORT1     PORTD
#define INP_DDR1      DDRD
#define INP_REG1      PIND
#define INP_PIN1      PIND5

#define OUT_PORT0     PORTB
#define OUT_DDR0      DDRB
#define OUT_PIN0      PINB1

#define OUT_PORT1     PORTB
#define OUT_DDR1      DDRB
#define OUT_PIN1      PINB2

#define OUT_PORT2     PORTB
#define OUT_DDR2      DDRB
#define OUT_PIN2      PINB3



#define INP_INIT(PIN)				                \
    INP_DDR   ## PIN &= ~_BV(INP_PIN ## PIN);	    \
    INP_PORT  ## PIN |=  _BV(INP_PIN ## PIN)
    
#define OUT_INIT(PIN)				                \
    OUT_DDR ## PIN |= _BV(OUT_PIN ## PIN)


#define KEY_TEST(PIN)				                    \
    if ( !(INP_REG ## PIN & _V(INP_PIN ## PIN) )  \
    {                                                   \
        key = 0;                                        \
    }                                                   \
    else                                                \
    {                                                   \
        key = 1;                                        \
    }                                                   \

#define SET_OUTPUT(PIN, STATE)  	                    \
    if ( 1 == STATE )                                   \
    {                                                   \
        OUT_PORT  ## PIN |=  _BV(OUT_PIN ## PIN);       \
    }                                                   \
    else                                                \
    {                                                   \
        OUT_PORT  ## PIN &= ~_BV(OUT_PIN ## PIN);       \
    }                                                   \

#define OUT__LED_RED    2
#define OUT__BUZZER     0


#define INPUT_INIT(PORT, PIN)   		                \
    PORT &= ~_BV(PIN)
    
#define OUTPUT_INIT(PORT, PIN)			                \
    PORT  |= _BV(PIN)

#define OUTPUT_INIT2(PORT_LETTER, PIN_NUMBER)			                \
    PORT ## PORT_LETTER  |= _BV(PIN ## PORT_LETTER ## PIN_NUMBER)

#define OUTPUT_SET_1(PORT_LETTER, PIN_NUMBER)			                \
    PORT ## PORT_LETTER  |= _BV(PIN ## PORT_LETTER ## PIN_NUMBER)

#define OUTPUT_SET_0(PORT_LETTER, PIN_NUMBER)			                \
    PORT ## PORT_LETTER &= ~_BV(PIN ## PORT_LETTER ## PIN_NUMBER)


#define OUTPUT__LED_RED    DDRB", "PINB3
#define OUTPUT__BUZZER     DDRB, PINB1


// my prototype board AVR_PB_001
//#define LED_RED         PB2
//#define BUZZER          PB1
//#define KEY_LED_RED (PIND & _BV(PIND4))
//#define KEY_BUZZER  (PIND & _BV(PIND5))

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


void ioinit (void)
{
//    INP_INIT(0);
//    INP_INIT(1);

//    OUT_INIT(OUT__LED_RED);
//    OUT_INIT(OUT__BUZZER);

//    OUT_INIT(0);
//    OUT_INIT(2);
  
//    OUTPUT_INIT(DDRB, PINB3);

    // enable PB3 as output for LED
//    DDRB |= (1<<DDB3);
    // enable PB3 as output for BUZER
//    DDRB |= (1<<DDB1);
//    OUTPUT_INIT(DDRB, PINB3);
//    OUTPUT_INIT(DDRB, PINB1);
    OUTPUT_INIT2(B, 3);
//    OUTPUT_INIT2(B, 1);
    OUTPUT_INIT2(D, 5);

/*
    // led on, pin=0 
    PORTB &= ~(1<<PB1);
    delay_ms(500);
    // set output to 5V, LED off
    PORTB|= (1<<PB1);
    delay_ms(500);
*/
}


int main (void)
{

//    ioinit ();
    unsigned char key;
    unsigned int delay = 500;
//    unsigned int n = 0; 

    // enable PB3 as output for LED
//    DDRB |= (1<<DDB3);

    // enable PB3 as output for BUZER
//    DDRB |= (1<<DDB1);

    for (;;)
    {
//        for ( n = 0; n <= 1000; n++ )
//        {
//            delay_ms(100);

            // led on, pin=0
//            PORTB &= ~(1<<PB3);
            // buzzer off
//            PORTB |= (1<<PB1);

            OUTPUT_SET_1(B, 3);
//            OUTPUT_SET_0(B, 1);
            OUTPUT_SET_0(D, 5);

//            SET_OUTPUT(2, 0);
//            SET_OUTPUT(0, 1);

            delay_ms(delay);

            // set output to 5V, LED off
//            PORTB |= (1<<PB3);
            // buzzer on
//            PORTB &= ~(1<<PB1);
            // buzzer off
//            PORTB |= (1<<PB1);
            OUTPUT_SET_0(B, 3);
//            OUTPUT_SET_1(B, 1);
            OUTPUT_SET_1(D, 5);

//            SET_OUTPUT(2, 1);
//            SET_OUTPUT(0, 0);

            delay_ms(delay);
//        }
/*

if(!(keyportpin & (1<<col3)))
{
    //Check COL3
    key += 2;
    //Third key pressed
    while(!(keyportpin & (1<<col3)));
}


        key = 1;
//        sleep_mode();
//        KEY_TEST(0);
if(bit_is_clear(PIND, PD4))
{
      delay_ms(30);
      if(bit_is_set(PIND, PD4))
      {
        // enable PB1 as output
        DDRB|= (1<<DDB1);
     }
} 
*/

/*
        if ( key == 0 )
        {        
//            SET_OUTPUT(0, 1);
           PORTB &= ~(1<<PB1);
        }
        key = 1;
//        sleep_mode();

        KEY_TEST(1);
        if ( key == 0 )
        {        
//            SET_OUTPUT(1, 1);
            PORTB &= ~(1<<PB1);
        }
*/
    }

    return (0);
}

