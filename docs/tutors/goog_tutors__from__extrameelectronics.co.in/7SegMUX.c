/*

Description:	Program to demonstrate the use of Seven
				Segment Displays In Multiplexed Mode.
________________________________________________________

Author:	Avinash Gupta
Date:	11 Oct 08
Web		www.eXtremeElectronics.co.in

	
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>


#define SEVEN_SEGMENT_PORT PORTD
#define SEVEN_SEGMENT_DDR DDRD

volatile uint8_t digits[3];


void SevenSegment(uint8_t n,uint8_t dp)
{
/*
This function writes a digits given by n to the display
the decimal point is displayed if dp=1

Note:
n must be less than 9
*/
	if(n<10)
	{
		switch (n)
		{
			case 0:
			SEVEN_SEGMENT_PORT=0b00000011;
			break;

			case 1:
			SEVEN_SEGMENT_PORT=0b10011111;
			break;

			case 2:
			SEVEN_SEGMENT_PORT=0b00100101;
			break;

			case 3:
			SEVEN_SEGMENT_PORT=0b00001101;
			break;

			case 4:
			SEVEN_SEGMENT_PORT=0b10011001;
			break;

			case 5:
			SEVEN_SEGMENT_PORT=0b01001001;
			break;

			case 6:
			SEVEN_SEGMENT_PORT=0b01000001;
			break;

			case 7:
			SEVEN_SEGMENT_PORT=0b00011111;
			break;

			case 8:
			SEVEN_SEGMENT_PORT=0b00000001;
			break;

			case 9:
			SEVEN_SEGMENT_PORT=0b00001001;
			break;
		}
		if(dp)
		{
			//if decimal point should be displayed
			//make 0th bit Low
			SEVEN_SEGMENT_PORT&=0b11111110;
		}
	}
	else
	{
		//This symbol on display tells that n was greater than 9
		//so display can't handle it
		SEVEN_SEGMENT_PORT=0b11111101;
	}
}

void Wait()
{ 
	uint8_t i;
	for(i=0;i<10;i++)
	{
		_delay_loop_2(0);
	}
}

void Print(uint16_t num)
{
	/* 
	
	This function breaks apart a given integer into separete digits
	and writes them to the display array i.e. digits[]
	
	*/ 
	uint8_t i=0;
	uint8_t j;
	if(num>999) return;


	while(num)
	{
		digits[i]=num%10;
		i++;

		num=num/10;
	}
	for(j=i;j<3;j++) digits[j]=0;
}
	 
	


void main()
{
	uint16_t i;

	// Prescaler = FCPU/1024
	TCCR0|=(1<<CS02);

	//Enable Overflow Interrupt Enable
	TIMSK|=(1<<TOIE0);

	//Initialize Counter
	TCNT0=0;

	//Port B[2,1,0] as out put
	DDRB|=0b00000111;

	PORTB=0b00000110;

	//Port D
	SEVEN_SEGMENT_DDR=0XFF;

	//Turn off all segments
	SEVEN_SEGMENT_PORT=0XFF;

	//Enable Global Interrupts
	sei();

	//Infinite loop
	//Print a number from 1 to 999
	while(1)
	{
		for(i=0;i<1000;i++)
		{
			Print(i);
			Wait();
		}

	}
}

ISR(TIMER0_OVF_vect)
{
	/*

	This interrup service routine (ISR)
	Updates the displays

	*/
	static uint8_t i=0;

	if(i==2)
	{
		//If on last display then come
		//back to first.
		i=0;
	}
	else
	{
		//Goto Next display
		i++;
	}

	//Acivate a display according to i
	PORTB=~(1<<i);

	//Write the digit[i] in the ith display.
	SevenSegment(digits[i],0);

}


	
