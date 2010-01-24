/* Programik przykladowy */
/* typ uC:  ATmega8 1MHz */
/* licznik/timer1  w trybie  PWM  */
/* Sygnal impulsowy (1kHz)   na wyprowadzeniu  PB1(OC1A) */
/* Przyciski sterujace szerokoscia impulsu przylaczone miedzy PD0 i mase  oraz  PD1 i mase */


#define F_CPU 1000000L
#include <avr/io.h>
	

int main(void) 
{
  int p = 0;

  DDRD =0x00;
  PORTD=0x03;
  DDRB =0x02;
 
  ICR1 =500;
  OCR1A=0;
   
  /* Tryb  PWM phase and frequency correct , patrz str. 93  ATmega8 datasheet */ 

  
  TCCR1A=0x80;
  TCCR1B=0x11;

  for (;;)
  {
     if(bit_is_clear(PIND, 0))
     {
         p+=10;
         if(p > 500) p=500;

         OCR1A=p;  
         loop_until_bit_is_set(PIND, 0); /* Czeka az przycisk zostanie zwolniony */
     }
     if(bit_is_clear(PIND, 1))
     {
         p-=10;
         if(p < 0) p=0;

         OCR1A=p;  
         loop_until_bit_is_set(PIND, 1);
     }
  }
}
