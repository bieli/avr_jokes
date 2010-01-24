
#include <avr/io.h>
#include <avr/delay.h>
//#include "lcd.h"

#define OWPORT PORTB
#define OWPIN PB0
#define OWPININ PINB
#define OWDDR DDRB
#define OWIN 1       // 2^OWPIN

#define sbi(p,b) (p) |= (1<<(b))
#define cbi(p,b) (p) &= ~(1<<(b))

#define delay_us(x) _delay_loop_2(x*2)
#define ow_lo() sbi(OWDDR, OWPIN)
#define ow_hi() cbi(OWDDR, OWPIN)

uint8_t ow_reset(void){
                uint8_t a;             
                ow_lo();
                delay_us(480);
                ow_hi();
                delay_us(70);
                a = OWPININ & OWIN;   // 0 - obecny, else - nieobecny
                delay_us(410);
                return a;

}

void ow_wrbit(uint8_t bit){
        ow_lo();
        if(bit == 0){
                delay_us(60);
                ow_hi();
                delay_us(10);
        }
        else {
                delay_us(6);
                ow_hi();
                delay_us(64);
        }

}

uint8_t ow_rdbit(void){
                uint8_t a;
                ow_lo();
                delay_us(6);
                ow_hi();
                delay_us(9);
                a = OWPININ & OWIN;   //0 - 0, else -1
                delay_us(55);
                return a;

}

uint8_t ow_rdbyte(void){
        uint8_t a, b;
        a = 0;
        for(b = 0; b <8; b++){
                a >>= 1;
                if (ow_rdbit()) a += 128;
        }     
        return a;

}

void ow_wrbyte(uint8_t data){
        uint8_t a;
        for(a = 0; a < 8; a++){
                ow_wrbit(data & 1);
                data >>= 1;     
        }

}

void lcd_putuint8(uint8_t a){
        uint8_t b;
        b = a  / 100;
        if (b) lcd_putc(b + 0x30);
        a -= b * 100;
        b = a / 10;
        lcd_putc(b + 0x30);
        a -= b * 10;
        lcd_putc(a + 0x30);

}

void lcd_putuint16(uint16_t a){
        uint8_t b;
        b = a / 10000;
//      lcd_putc(b + 0x30);
        a -= b * 10000;
        b = a / 1000;
        lcd_putc(b + 0x30);
        a -= b * 1000;
        b = a / 100;
        lcd_putc(b + 0x30);
        a -= b * 100;
        b = a;
        lcd_putuint8(b);

}

int main(void){
        uint8_t a, b, c;
        uint16_t e;

        lcd_init( LCD_DISP_ON);
        cbi(OWPORT, OWPIN);
        ow_hi();               
        for(;;){
        ow_reset();
        ow_wrbyte(0xCC);                        //SKIP ROM
        ow_wrbyte(0x44);                        //CONVERT T
        for(a = 1; a <100; a++){
                delay_us(10000);
        }
        ow_reset();
        ow_wrbyte(0xCC);                        //SKIP ROM
        ow_wrbyte(0xBE);                        //READ SCRATCHPAD
        a = ow_rdbyte();
        b = ow_rdbyte();
        c = a &  (0x0F);                                    // wartości dziesiętne (po przecinku)
        a  >>=  4;
        b  <<=  4;
        a = a | b;
        e = 625 * c;
        lcd_clrscr();
        lcd_home();
        lcd_putuint8(a);
        lcd_puts(",");
        lcd_putuint16(e);
        }
        return 0;
} 
