/**
 * ----------------------------------------------------------------------------
 * Modul dla prototypowej plytki programownaia AVR z ATtn2313 na pokladzie
 * oraz 1 LED, BUZZER, 4 SWITCHEs, USB buffor for CDC, removable XTAL AND goldpins
 * ----------------------------------------------------------------------------
 *
 * Definicje dla apikacji opartych o plytke prototypowa.
 *
 */

#if !defined(__MB_AVR_SIMPLE_BOARD__H)
#define  __MB_AVR_SIMPLE_BOARD__H

/*
Port Manipulation

Manipulating ports and pins with AVR code is faster than using Arduino digitalWrite() function. It is also possible to make two pins change states at exactly the same time. You can read about it here.

Setting Bits in Variables

cbi and spi are standard (AVR) methods for setting, or clearing, bits in PORT (and other) variables.

You will find them in lots of AVR code, posted in the forum and elsewhere. Both need to be defined within an Arduino sketch, so you'll need to paste in the following #define headers at the top of a sketch.
*/
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif 

//To use them pass them a PORT variable, and a pin to set (or clear). 


// inicjalizacja wejscia
#define INPUT_INIT(PORT_LETTER, PIN_NUMBER)			                \
    DDR ## PORT_LETTER &= ~_BV(PIN ## PORT_LETTER ## PIN_NUMBER);   \
    PORT ## PORT_LETTER |= _BV(PIN ## PORT_LETTER ## PIN_NUMBER)

#define INPUT_CHECK(PORT_LETTER, PIN_NUMBER)			                \
    if ( bit_is_clear(PIN ## PORT_LETTER, P ## PORT_LETTER ## PIN_NUMBER) ) { 1 } else { 0 }
    



/*
// Wpierw konfiguruje  linię PD0 jako wejście z  wewnętrznym  podciągnięciem do VCC
DDRD  &= ~_BV(PD0);
PORTD |=  _BV(PD0);


if(bit_is_clear(PIND, PD0)) {   }

// LUB  TAK 

if(!(PIND & _BV(PD0)))    {  }
*/

// inicjalizacja wyjscia
#define OUTPUT_INIT(PORT_LETTER, PIN_NUMBER)			                \
    PORT ## PORT_LETTER  |= _BV(PIN ## PORT_LETTER ## PIN_NUMBER)

// ustawienie stany wysokiego na okreslonym pinie na okrelsonym porcie
#define OUTPUT_SET_1(PORT_LETTER, PIN_NUMBER)			                \
    PORT ## PORT_LETTER  |= _BV(PIN ## PORT_LETTER ## PIN_NUMBER)

// ustawienie stany niskiego na okreslonym pinie na okrelsonym porcie
#define OUTPUT_SET_0(PORT_LETTER, PIN_NUMBER)			                \
    PORT ## PORT_LETTER &= ~_BV(PIN ## PORT_LETTER ## PIN_NUMBER)

/*

#define OUT_PORT__LED_RED    "B"
#define OUT__LED_RED          3
#define OUT_PORT__BUZZER     "D"
#define OUT__BUZZER           1


#define OUTPUT                "OUTPUT"
#define INPPUT                "INPPUT"

#define LED_RED              OUTPUT
#define PORT__LED_RED        "B"
#define PIN___LED_RED        (3)

#define CREATE_OUTPUT(TYPEOF, PORT_LETTER, PIN_NUMBER)			            \
    if ( OUTPUT == TYPEOF )                                                 \
    {                                                                       \
        define PORT__##TYPEOF  PORT_LETTER                             \
        define PIN__##TYPEOF  PIN_NUMBER                               \
        PORT ## (PORT__ ## TYPEOF)  |= _BV(PIN ## (PIN__ ## TYPEOF));       \
    }                                                                       \
    else if ( INPPUT == TYPEOF )                                            \
    {}


#define OUT__LED_RED          3

//#define OUTPUT__LED_RED    DDRB", "PINB3
//#define OUTPUT__BUZZER     DDRB, PINB1


//using:
//    CREATE_OUTPUT(LED_RED, "B", 4);


*/

/**
 * Wykonuje opoznienie w milisekundach
 *
 * @param unsigned int i_Miliseconds 
 */
void delay_ms(unsigned int i_Miliseconds)
{
        // we use a calibrated macro. This is more
        // accurate and not so much compiler dependent
        // as self made code.
        while(i_Miliseconds)
        {
                _delay_ms(0.96);
                i_Miliseconds--;
        }
}


#endif /* !defined(__MB_AVR_SIMPLE_BOARD__H) */
