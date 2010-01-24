/*-----------------------------------------------------------------------------
 * File: MR-2313.C
 * Author: MICROROBOT Company - http://www.microrobot.com
 * Copyright (c) July, 2003
 * Description: Turns on and off all the ports.
 *
 * Resonator frequency = 8 MHz
 *---------------------------------------------------------------------------*/

#include <avr\io.h>

typedef unsigned char byte;
typedef unsigned int word;

#define sei()  asm volatile ("sei" ::)
#define cli()  asm volatile ("cli" ::)


// my prototype board AVR_PB_001
#define LED_RED  	PB2
#define BUZZER   	PB1
#define KEY_LED_RED (PIND & _BV(PIND4))
#define KEY_BUZZER  (PIND & _BV(PIND5))



// 1msec UNIT delay function
void delay_1ms(unsigned int i)
{
	word j;
	while(i--)
	{
		j=2000;   // 8 Mhz
		while(j--);
	}
}

void port_init(void)
{
//	outp( 0xff, DDRB );	//Configure PORTB as an output port.
//	outp( 0xff, DDRD );	//Configure PORTD as an output port.

//	outp( 0xff, PORTB );	//Output 0xff to PORTB.
//	outp( 0xff, PORTD );	//Output 0xff to PORTD.

	DDRB = 0xff;
	DDRD = 0xff;

	//PORTB = 0xff;
	//PORTD = 0xff;
	PORTB = 0x00;
	PORTD = 0x00;
}

void all_port_set(void)
{
//	outp( 0xff, PORTB );	//Output 0xff to PORTB.
//	outp( 0xff, PORTD );	//Output 0xff to PORTD.
	PORTB = 0xff;
	PORTD = 0xff;
}

void all_port_clear(void)
{
//	outp( 0, PORTB );	//Output 0 to PORTB.
//	outp( 0, PORTD );	//Output 0 to PORTD.
	PORTB = 0;
	PORTD = 0;
}

int main(void)
{
	//port_init();			//Ports Initialization
	//word j = 10000, i = 100, a = 10000;
	unsigned int delay_const = 10000;
	
	DDRB = 0xff;
	PIND = 0xff;

	unsigned int i = 0;
	word j = 0;	

//	PORTB = 0xFF;
//	PORTD = 0xFF;

//	delay_const = 500000;

	

	while(1)
	{
		if ( KEY_LED_RED )
		{
			PORTB &= _BV(LED_RED);
		}
		else
		{
			PORTB |= _BV(LED_RED);
		}

		if ( KEY_BUZZER )
		{
			PORTB &= _BV(BUZZER);
		}
		else
		{
			PORTB |= _BV(BUZZER);
		}


//		PORTB |= _BV(LED_RED);
//		PORTB |= _BV(BUZZER);
//		PORTB = 0xff;
//		PORTD = 0x00;
/*		
		// delay test
		i = delay_const;
		while(i--)
		{
			j=2000;
			while(j--);
		}

//		PORTB &= _BV(LED_RED);
//		PORTB &= _BV(BUZZER);
		PORTB = 0xff;
		PORTD = 0xff;

		i = delay_const;
		// delay test
		while(i--)
		{
			j=2000;
			while(j--);
		}
*/		
/*
		a -= 100;
		
		i = a;

		all_port_clear();		
		
		while(i--)
		{
			j=2500;
			while(j--);
		}

		all_port_set();
		
		i = a;
		
		while(i--)
		{
			j=2500;
			while(j--);
		}
*/
		//all_port_set();
		//all_port_clear();
/*
		while(i--)
		{
			all_port_set();
			delay_1ms(i);
			all_port_clear();		
			//delay_1ms(i);
			//all_port_set();
		}
*/
//		delay_1ms(500);
//		all_port_set();
//		delay_1ms(500);
	}
}
