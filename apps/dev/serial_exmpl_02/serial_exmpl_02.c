/*
 * Test wysyłania po RS232 na podstawie odebranej komendy napisów.
 * Petla oczekuje na podanie rozkazu znaku z terminala PC "a" lub "b"
 * Wypisuje po rozpoznanu znaku info. o powodzenu odbioru.
 * Dodatkowo po kazdym odebranym znaku ustawia lub zalacza port B.
 * Testowanie:
 *   screen /dev/ttyS0 -p 9600
 * 
 * Działa OK lecz zawiesza sie program lub terminal po kilkudziesiecu komendach ????
 * //TODO: problem z zawieszaniem sie, przy szybkim wypisywaniu komend - wymagane bedzie wylaczanie gdzes przerwan
 * //toDO: warto dodac watchdoga :-)
 * 
 */

#include<avr/io.h>
#include<avr/interrupt.h>   //potrzebne do przerwan

#define FCPU  8000000   //kwarc 8MHz
#define VUART 9600      //predkosc transmisji[b/s]
#define VUBRR 51      //wpis dla VUART=9600 gwarantujacy 0.2% bledu

unsigned char volatile flagrx=0;   //flaga odebrano znak

short int m=1;

char volatile komenda;      //odebrana komenda z pc

char bufor[20];   //bufor

#define CMD_SET_PORT    '1'
#define CMD_CLEAR_PORT  '2'

char dane1[]="\n\rwcisnales: 1\n\r";   //na końcu dołączany jest automatycznie znak '\0', który oznacza koniec łańcucha znaków w języku C
char dane2[]="\n\rwcisnales: 2\n\r";
char dane3[]="\n\rwcisnij klawisz 1 lub 2 ...\n\r";

/////   procedura obslugi odbiornika UART   /////
SIGNAL(SIG_UART_RECV)
{
   cli();
   komenda = UDR;  //odebrane zapamietuje w zmiennej komenda
   flagrx  = 1;    //ustawia flage odebrania znaku
   sei();
}

/////   procedura obslugi nadajnika UART   /////
SIGNAL(SIG_UART_TRANS)
{   
   if ( bufor[m] != '\0' )
   {
      UDR=bufor[m];
      m++;
   }
   else if ( bufor[m] == '\0' )
   {
      //   UDR='\0';   //wysyla jeszcze znak konca'\0' i wylacza nad
      //do poprawki   
      UCR &= ~_BV(TXEN);   //zeruje bitTXEN wylacza nadajnik   
      m   = 1;
   }
}

/////   funkcja kopiujaca tablice dane do bufora i inicjujaca wysylanie      ////
void wyslij(char dane[])
{
   short int n;

   for(n=0;dane[n]!='\0';n++)   //kopiuje dane[] do bufora[]
      bufor[n]=dane[n];

   bufor[n] = dane[n];      //dodaje na koniec bufora '\0'
   UCR      |= _BV(TXEN);   //wlacza nadajnik
   UDR      = bufor[0];     //wysyla pierwszy znak reszta wysyla w obsludze przerwania
}

int main(void)
{
   DDRB = 0xff;       //wyjscie na leda   
   UBRR = VUBRR;      //ustaw predkosc transmisji   
   UCR  = 1<<RXCIE | 1<<RXEN | 1<<TXCIE;   //zezwolenie na przerwania od odb nad i zezw. na odb

   sei();      //wlacz przerwania

   while(1)
   {
      if(flagrx)
      {
         switch(komenda)
         {
            case CMD_SET_PORT:
               wyslij(dane1);
//               PORTB = 0x00;
               PORTB ^= 0xFF;
               break;
            case CMD_CLEAR_PORT:
               wyslij(dane2);
//               PORTB = 0xff;
               PORTB ^= 0xFF;
               break;
            default:
               wyslij(dane3);
         }

         flagrx=0;   //zeruje flage odbioru zeby juz nie wchodzilo w tego if-a
      }

   }

   return 0;
}

