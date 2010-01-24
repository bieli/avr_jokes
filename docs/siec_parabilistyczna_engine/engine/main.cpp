#include <avr/io.h>
#include <string.h>
#include "config.hpp"

#include "probabilisticnetwork.cpp" 
#include "node.cpp"      


int main(void) {
    
	PORTA = 0xff;   
	DDRA = 0xFF; 
	TCNT0= 0xb2;
	TCCR0 = 0x05;   
	TIMSK = (1<<TOIE0);  

	DDRD = 0x02;
	PORTD = 0x02;
	USART_Init(51);  
	  
	  
	sei();       
	    
	   
	CProbabilisticNetwork network;
	network.Load();
	
      
	while(1)   
	{     
            
	} 
	return 0;
}