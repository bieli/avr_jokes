
#include <avr/io.h>
#include <inttypes.h>
//TODO: #include <avr/interrupt.h>  // potrzebne do przerwan

#define FCPU  1000000
#define F_CPU FCPU

#define ONE_SECOND  1000

#include <util/delay.h>
//TODO: #include <util/ds18b20_temperature_sensor.h>
//TODO: #include <util/fan_controller.h>
//TODO: #include <util/10_leds_display_line.h>

typedef enum {
    MODE_CONTROLL = 0,
    MODE_SET_LOW_TEMP_FAN_SPEED = 1,
    MODE_SET_HIGH_TEMP_FAN_SPEED = 2,
    MODE_SET_CONTROLL_HYSTERESIS_IN_SECOND = 4,
    MODE_RUN_DISPLAY_1FROM10_TEST = 8
} eMode;


typedef struct {
    eMode           currentSelectedMode;
    unsigned int    currentTempValue;
    unsigned int    tempDisplayValue;
    unsigned char   lowTempFanSpeedValue;
    unsigned char   highTempFanSpeedValue;
} tController;

tController controller;

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

void set_led_mask_1from10_7442_1(unsigned char led_mask, unsigned int delay)
{
    int n = 0;

    unsigned char led_mask_shift = led_mask;
    unsigned char b = 1;

    for ( n = 0; n < delay; n++ )
    {
        for ( n = 0; n < 8; n++ )
        {
            b *= 2;
            if  ( b & ( led_mask_shift << n ) )
            {
                set_led_1from10_7442(n);
            }
        }

        delay_ms(1);
    }
}

void set_led_mask_1from10_7442(unsigned int led_mask, unsigned int delay)
{
    unsigned char n = 0;

    unsigned int led_mask_shift = led_mask;
    unsigned int b = 1;

//    for ( n = 0; n < delay; n++ )
//    {
        for ( n = 0; n < 10; n++ )
        {
            b *= 2;
            if  ( b & ( led_mask_shift << 1 ) )
            {
                set_led_1from10_7442(n);
                delay_ms(delay);
//                delay_ms(1);
            }
        }
//    }
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
    DDRD = 0xFF;
    PORTD = 0;

// swieca tylko skrajne
//    unsigned int ch = 513;
// swieci co druga
//    unsigned int ch = 682; 
// swieca wszystkie
//    unsigned int ch = 1023;
// swieca skrajne i dwie wskazane bitowo 
//    unsigned int ch = 513 + 4 + 32;
// nieswieci jedna - reszta swieci
// swieca wszystkie oprucz jednego
    unsigned int ch = 1023 - 64;

    controller.currentSelectedMode = MODE_CONTROLL;
    controller.currentTempValue = 25;
    controller.tempDisplayValue = ch;
    controller.lowTempFanSpeedValue = 9;
    controller.highTempFanSpeedValue = 1;

    while ( 1 ) {
//        set_led_mask_1from10_7442_1(ch, 100);
        set_led_mask_1from10_7442(controller.tempDisplayValue, 1);
    }

    return 0;
}

