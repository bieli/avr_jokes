#include <avr/signal.h>
//#include <avr/interrupt.h>

void USART_Init( unsigned int baud )
{
/* Set baud rate */
UBRRH = (unsigned char)(baud>>8);
UBRRL = (unsigned char)baud;
/* Enable receiver and transmitter */
UCSRB = (1<<RXEN)|(1<<TXEN);
/* Set frame format: 8data, 2stop bit */
UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
/* Wait for empty transmit buffer */
while ( !( UCSRA & (1<<UDRE)) );
/* Put data into buffer, sends the data */
UDR = data;
}

void USART_Transmit_String( char *data )
{
	while (*data != 0)
	{
		USART_Transmit (*data);
		data++;
	};

}

unsigned int volatile counter;

void delayms ( unsigned long _counter )
{	
	counter = _counter/10;
	while ( counter );
}

SIGNAL ( SIG_OVERFLOW0 )
{
	counter--;
	TCNT0=0xb2;
}
