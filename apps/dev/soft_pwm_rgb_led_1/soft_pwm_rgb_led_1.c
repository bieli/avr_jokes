/**
 * Smart LED Prototype
 *  2007, Tod E. Kurt, ThingM, tod@todbot.com
 *
 * Function:
 *  - on startup, display hue blink?
 *  - input #1 (PB3) is used to select hue
 *  - input #2 (PB4) is used to select brightness
 *  - holding an input low begins value selection
 *  - releasing an input sets the value
 *  - holding both buttons does something too
 *
 * Based on Atmel AVR AppNote 136 "Low-Jitter Multi-Channel Software PWM"
 *                                  by  Andy Gayne 
 */

// NOTE: Be sure to burn fuses to select internal oscillator!
// 8MHz on default on internal oscillator
#define F_CPU 8000000

#include <ctype.h>

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/wdt.h>

#include <util/delay.h>
#include <stdlib.h> 

// Pin mappings
#define CHMAX       3    // maximum number of PWM channels

#define CH0_CLEAR (pinlevelB &= ~(1 << PB0)) // map CH0 to PB0
#define CH1_CLEAR (pinlevelB &= ~(1 << PB1)) // map CH1 to PB1
#define CH2_CLEAR (pinlevelB &= ~(1 << PB2)) // map CH2 to PB2

//#define CH0_SET   (pinlevelB |= (1 << PB0))  // map CH0 to PB0
//#define CH1_SET   (pinlevelB |= (1 << PB1))  // map CH1 to PB1
//#define CH2_SET   (pinlevelB |= (1 << PB2))  // map CH2 to PB2

// Set bits corresponding to pin usage above       this is to enable pullups
//#define PORTB_MASK  (1<< PB0)|(1<< PB1)|(1<< PB2)  |(1<<PB3)|(1<<PB4)
#define PORTB_MASK  (1<< PB0)|(1<< PB1)|(1<< PB2)
// these are the outputs
#define DDRB_MASK   (1<< PB0)|(1<< PB1)|(1<< PB2)
#define INPUT_MASK  (1<< PB3)|(1<< PB4)

// select ADC2, PB4, pin 3
#define ADC_CHAN 2

typedef uint8_t byte;

typedef struct _color { 
    byte r; byte g; byte b;
} color;

byte compare[CHMAX];
volatile byte compbuff[CHMAX];

#define red compbuff[0]
#define grn compbuff[1]
#define blu compbuff[2]

byte hue;
byte bright;
byte state;
int  keydown_counter;

#define STATE_START             0
#define STATE_SETTING_HUE       1
#define STATE_HUE_SET           2
#define STATE_RESETTING_HUE     3
#define STATE_SETTING_BRIGHT    4
#define STATE_BRIGHT_SET        5
#define STATE_END               6

#define DELAY 8

/*
 * Given a variable hue 'h', that ranges from 0-252,
 * set RGB color value appropriately.
 * Assumes maximum Saturation & maximum Value (brightness)
 * Performs purely integer math, no floating point.
 */
void h_to_rgb(byte h, color* c) 
{
    byte hd = h / 42;   // 42 == 252/6,  252 == H_MAX
    byte hi = hd % 6;   // gives 0-5
    byte f = h % 42; 
    byte fs = f * 6;
    switch( hi ) {
        case 0:
            c->r = 252;     c->g = fs;      c->b = 0;
           break;
        case 1:
            c->r = 252-fs;  c->g = 252;     c->b = 0;
            break;
        case 2:
            c->r = 0;       c->g = 252;     c->b = fs;
            break;
        case 3:
            c->r = 0;       c->g = 252-fs;  c->b = 252;
            break;
        case 4:
            c->r = fs;      c->g = 0;       c->b = 252;
            break;
        case 5:
            c->r = 252;     c->g = 0;       c->b = 252-fs;
            break;
    }
}

/* 
 * Given a hue 0-252, set the LEDs at maximum brightness for that hue
 */
void hset(byte h)
{
    color c;
    h_to_rgb(h,&c);
    red = c.r;
    grn = c.g; 
    blu = c.b;
}

/* 
 * Given a hue 0-252 and a brightness 0-255, set LEDs appropriately
 */
void hvset(byte h, byte v) 
{
    color c;
    h_to_rgb(h,&c);
    red = ((c.r * v) / 255);
    grn = ((c.g * v) / 255);
    blu = ((c.b * v) / 255);
}

/*
 *
 */
void init(void)
{
  DDRB = DDRB_MASK;            // set port pins to  output (and input)

//TODO: poprawic ustawianie rejestrow , aby program dzialal
/*
  TIFR0  = (1 << TOV0);          // clear interrupt flag
  TIMSK0 = (1 << TOIE0);         // enable overflow interrupt
  TCCR0B = (1 << CS00);          // start timer, no prescale
*/  
  sei();                      // enable interrupts
}

/*
 *
 */
int main(void)
{
    init();

    hue = 64;
    bright = 255;
    hvset(hue,bright);
    state = STATE_START;

    for(;;) {

        // little state machine: first detect button push,
        if( bit_is_clear(PINB,PB3) ) {  // PB3 is hue setting
            _delay_ms(DELAY);
            if( bit_is_clear(PINB,PB3) )
                state = STATE_SETTING_HUE;
        }
        else {
            if( state == STATE_SETTING_HUE ) 
                state = STATE_HUE_SET;
        }
        
        if( bit_is_clear(PINB,PB4) ) {  // PB4 is brightness setting
            _delay_ms(DELAY);
            if( bit_is_clear(PINB,PB4) )
                state = STATE_SETTING_BRIGHT;
        }
        else {
            if( state == STATE_SETTING_BRIGHT ) 
                state = STATE_BRIGHT_SET;
        }

        // handle actions within a state
        switch(state) { 
        case STATE_START:
            hue += 42;  // show off
            hset(hue);
	    int i=0;
            for(i=0; i<20;i++)
                _delay_ms(DELAY);
            break;
        case STATE_SETTING_HUE:
            hue++;
            hvset(hue,bright);
            break;
        case STATE_SETTING_BRIGHT:
            bright-=4;
            hvset(hue,bright);
            break;
        }

        _delay_ms(DELAY);
    }
    
}

/*
 *
 */
SIGNAL (SIG_OVERFLOW0)
{
    static byte pinlevelB = PORTB_MASK;
    static byte softcount = 0xFF;
   
    // common anode (+5V) means negative (~) logic
    PORTB = ~pinlevelB;            // update outputs
    
    // common cathode (GND) means positive logic
    //PORTB = pinlevelB;            // update outputs

    if(++softcount == 0) {         // increment modulo 256 counter and update
        // the compare values only when counter = 0.
        compare[0] = compbuff[0];   // verbose code for speed
        compare[1] = compbuff[1];
        compare[2] = compbuff[2];
        
        pinlevelB = PORTB_MASK;     // set all port pins high
    }
    // clear port pin on compare match (executed on next interrupt)
    if(compare[0] == softcount) CH0_CLEAR;
    if(compare[1] == softcount) CH1_CLEAR;
    if(compare[2] == softcount) CH2_CLEAR;

}


