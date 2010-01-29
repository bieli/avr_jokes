
#include <avr/io.h>
#include <inttypes.h>
//TODO: #include <avr/interrupt.h>  // potrzebne do przerwan

#define FCPU  1000000
#define F_CPU FCPU

#define DISPLAY_1FROM10_BCD_INPUT_A0     PIND0
#define DISPLAY_1FROM10_BCD_INPUT_A1     PIND1
#define DISPLAY_1FROM10_BCD_INPUT_A2     PIND2
#define DISPLAY_1FROM10_BCD_INPUT_A3     PIND3

#define ONE_SECOND  1000

//TODO: uzycie pamieci EEPROM do przechowania nastaw granicznych obrotow
//    unsigned int EEMEM fanLowRangeValue __attribute__ = 56;
//   unsigned int EEMEM fanHighRangeValue __attribute__ = 123;

#include <util/delay.h>
//TODO: #include <util/ds18b20_temperature_sensor.h>
//TODO: #include <util/fan_controller.h>
//TODO: #include <util/leds_10_display_line.h>

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

void leds_10_display_line__set_led(unsigned char led_nr)
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

    PORTD = bcd_1from10_7442[ led_nr ][ DISPLAY_1FROM10_BCD_INPUT_A0 ] * 8
          + bcd_1from10_7442[ led_nr ][ DISPLAY_1FROM10_BCD_INPUT_A1 ] * 4
          + bcd_1from10_7442[ led_nr ][ DISPLAY_1FROM10_BCD_INPUT_A2 ] * 2
          + bcd_1from10_7442[ led_nr ][ DISPLAY_1FROM10_BCD_INPUT_A3 ] * 1;
}

void leds_10_display_line__set_led_mask(unsigned int led_mask, unsigned int delay)
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
                leds_10_display_line__set_led(n);
                delay_ms(delay);
//                delay_ms(1);
            }
        }
//    }
}

void system_init()
{
    DDRD = 0xFF;
    PORTD = 0;
}

void controller_init(tController *pcontroller)
{
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

    pcontroller->currentSelectedMode = MODE_CONTROLL;
    pcontroller->currentTempValue = 25;
    pcontroller->tempDisplayValue = ch;
    pcontroller->lowTempFanSpeedValue = 9;
    pcontroller->highTempFanSpeedValue = 1;
}



int main(void)
{
    system_init();

    controller_init(&controller);

    while ( 1 )
    {
        leds_10_display_line__set_led_mask(controller.tempDisplayValue, 1);
    }

    return 0;
}

