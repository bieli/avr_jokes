/*

A simple program to demonstrate the use of USART of AVR micro
*************************************************************

See: www.eXtremeElectronics.co.in for more info

Author : Avinash Gupta
E-Mail: me@avinashgupta.com
Date : 29 Dec 2008

Hardware:
	ATmega8 @ 16MHz
	Suitable level convertor on RX/TX line
	Connected to PC via RS232
	PC Software : 	Hyperterminal @ 19200 baud
					No Parity,1 Stop Bit,
					Flow Control = NONE

*/

#include <avr/io.h>
#include <inttypes.h>

//This function is used to initialize the USART
//at a given UBRR value
void USARTInit(uint16_t ubrr_value)
{

   //Set Baud rate
   UBRRL = ubrr_value;
   UBRRH = (ubrr_value>>8);

   /*Set Frame Format


   >> Asynchronous mode
   >> No Parity
   >> 1 StopBit
   >> char size 8

   */

   UCSRC=(1<<URSEL)|(3<<UCSZ0);


   //Enable The receiver and transmitter
   UCSRB=(1<<RXEN)|(1<<TXEN);


}


//This function is used to read the available data
//from USART. This function will wait untill data is
//available.
char USARTReadChar()
{
	//Wait untill a data is available
	while(!(UCSRA & (1<<RXC)))
	{
		//Do nothing
	}

	//Now USART has got data from host
	//and is available is buffer

	return UDR;
}


//This fuction writes the given "data" to
//the USART which then transmit it via TX line
void USARTWriteChar(char data)
{
	//Wait untill the transmitter is ready
	while(!(UCSRA & (1<<UDRE)))
	{
		//Do nothing
	}

	//Now write the data to USART buffer

	UDR=data;
}

void main()
{
	//This DEMO program will demonstrate the use of simple
	//function for using the USART for data communication

	//Varriable Declaration
	char data;

	/*First Initialize the USART with baud rate = 19200bps
	
	for Baud rate = 19200bps
	UBRR value = 51

	*/

	USARTInit(51);		//UBRR = 51

	//Loop forever
	while(1)
	{
		//Read data
		data=USARTReadChar();

		/* Now send the same data but but surround it in
		square bracket. For example if user sent 'a' our
		system will echo back '[a]'.
		*/

		USARTWriteChar('[');
		USARTWriteChar(data);
		USARTWriteChar(']');

	}
}

	
	
