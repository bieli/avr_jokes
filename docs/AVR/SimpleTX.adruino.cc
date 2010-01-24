/*


 http://www.arduino.cc/playground/Main/SimpleTX
 http://www.arduino.cc/playground/Interfacing/LinuxTTY
 http://userwww.sfsu.edu/~infoarts/technical/arduino/wilson.arduinoresources.html
 http://sites.google.com/site/jpmzometa/arduino-mbrt/arduino-modbus-slave
 http://www.arduino.cc/playground/Code/SymbianS60Python
 http://www.mobilenin.com/pys60/info_standalone_application.htm
 http://tinker.it/now/2006/12/11/bluetooth-controlled-lamp/
 http://www.orangecone.com/archives/2006/10/the_coming_age.html
 http://www.dorkbot.org/dorkbotsf/archive/200901/
 http://www.timhunkin.com/ 
 
 
*/
#define myubbr (16000000/16/9600-1)
void simpletx( char * string ){
  if (UCSR0B != (1<<TXEN0)){ //do we need to init the uart?
    UBRR0H = (unsigned char)(myubbr>>8);
    UBRR0L = (unsigned char)myubbr;
    UCSR0B = (1<<TXEN0);//Enable transmitter
    UCSR0C = (3<<UCSZ00);//N81
  }
  while (*string){
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = *string++; //send the data
  }
}
