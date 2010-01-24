/**
 * Firmware dla ATTINY2313 do biernego sterowania urządzeniem mini_ploter
 * Ploter działa zgodnie z lista zadeklarowanych komend.
 * UWAGA !
 *  W tej wersji nie zwracana jest informacja o pozycji (położeniu) osi !
 */

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>  // potrzebne do przerwan

#define FCPU  8000000       // kwarc 8MHz
#define VUART 9600          // predkosc transmisji[b/s]
#define VUBRR 51            // wpis dla VUART = 9600 gwarantujacy 0.2% bledu
//#define FCPU  11059200       // kwarc 11.059 MHz
//#define VUART 57600          // 57600 - 0.0% błędu     
//#define VUBRR 11            

//#define FCPU  11059200       // kwarc 11.059 MHz
//#define VUART 115200         // 115200 - 0.0% błędu     
//#define VUBRR 5        

// wymagane tylko dla funkcji _delay_ms() oraz _delay_us()
#define F_CPU FCPU
#include <util/delay.h>	
//#include "../../../lib/iocompat.h"



unsigned char volatile flagrx=0;   //flaga odebrano znak

short int m=1;

unsigned char motor_speed_delay = 0;

char volatile recived_cmd;      // komenda odebrana z pc

char bufor[20];                 // bufor

/* Definicje dla obslugi silnikow osi X i Y mini plotera */
#define     MOTOR_XY_PORT_REGS      DDRB
#define     MOTOR_XY_PORT           PORTB
#define     MOTOR_XY_PORT_INIT_CMD   1

#define     MOTOR_X_LEFT_PIN        0
#define     MOTOR_X_RIGHT_PIN       1
#define     MOTOR_Y_LEFT_PIN        2
#define     MOTOR_Y_RIGHT_PIN       3

#define     MOTOR_X_STOP_CMD        2
#define     MOTOR_X_STEPUP_CMD      3
#define     MOTOR_X_STEPDOWN_CMD    4

#define     MOTOR_Y_STOP_CMD        8
#define     MOTOR_Y_STEPUP_CMD      16
#define     MOTOR_Y_STEPDOWN_CMD    32


#define     LED_PORTD_PIN           6

#define     MOTOR_STEP_DELAY_50     50
#define     MOTOR_STEP_DELAY_20     20
#define     MOTOR_STEP_DELAY_10     10
#define     MOTOR_STEP_DELAY_7      7
#define     MOTOR_STEP_DELAY_5      5

/* Definicje poleceń na podstawie klasy Python:
class MiniPloterSerialInterface(object):
    CMD_SET_HOME_X  = 'x'
    CMD_SET_HOME_Y  = 'y'
    CMD_SET_UP_X    = 'l'
    CMD_SET_DOWN_X  = 'j'
    CMD_SET_UP_Y    = 'i'
    CMD_SET_DOWN_Y  = 'k'
    CMD_SET_UP_X_SPEED    = 't'
    CMD_SET_DOWN_X_SPEED  = 'g'
    CMD_SET_UP_Y_SPEED    = 'f'
    CMD_SET_DOWN_Y_SPEED  = 'h'

    CMD_TEST_1      = '1'
    CMD_TEST_2      = '2'
    CMD_TEST_3      = '3'
    CMD_TEST_4      = '4'

    CMD_SET_MOTOR_SPEED_5   = '5'
    CMD_SET_MOTOR_SPEED_7   = '6'
    CMD_SET_MOTOR_SPEED_10  = '7'
    CMD_SET_MOTOR_SPEED_20  = '8'
    CMD_SET_MOTOR_SPEED_50  = '9'
*/

#define     CMD_SET_HOME_X  'x'
#define     CMD_SET_HOME_Y  'y'
#define     CMD_SET_UP_X    'l'
#define     CMD_SET_DOWN_X  'j'
#define     CMD_SET_UP_Y    'i'
#define     CMD_SET_DOWN_Y  'k'
#define     CMD_SET_UP_X_SPEED    't'
#define     CMD_SET_DOWN_X_SPEED  'g'
#define     CMD_SET_UP_Y_SPEED    'f'
#define     CMD_SET_DOWN_Y_SPEED  'h'

#define     CMD_TEST_1      '1'
#define     CMD_TEST_2      '2'
#define     CMD_TEST_3      '3'
#define     CMD_TEST_4      '4'

#define     CMD_SET_MOTOR_SPEED_5   '5'
#define     CMD_SET_MOTOR_SPEED_7   '6'
#define     CMD_SET_MOTOR_SPEED_10  '7'
#define     CMD_SET_MOTOR_SPEED_20  '8'
#define     CMD_SET_MOTOR_SPEED_50  '9'



char responde_start_msg[] = "\n\rMINI_PLOTER_01";
char responde_crlf[] = "\n\r";
char response_ok[]   = "\n\rOK\n\r";
char response_cmd_not_implemented[]   = "\r\nCMD_NOTIMPL\n\r";

/////   procedura obslugi odbiornika UART   /////
SIGNAL(SIG_UART_RECV)
{
   cli();
   recived_cmd = UDR;   // odebrane zapamietuje w zmiennej recived_cmd
   flagrx  = 1;         // ustawia flage odebrania znaku
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
void send_to_pc(char dane[])
{
   short int n;

   for(n=0;dane[n]!='\0';n++)   //kopiuje dane[] do bufora[]
      bufor[n]=dane[n];

   bufor[n] = dane[n];      //dodaje na koniec bufora '\0'
   UCR      |= _BV(TXEN);   //wlacza nadajnik
   UDR      = bufor[0];     //wysyla pierwszy znak reszta wysyla w obsludze przerwania
}

//				PORTB &= ~_BV(4);  // clear bit, turn LED off
//			} else {
//				PORTB |= _BV(4);  // set bit, turn LED on
// PORTB &= ~(_BV(BIT));
// delay_us(NOSNA);
// PORTB |= _BV(BIT); 

void motor_driver(motor_cmd)
{
    switch ( motor_cmd )
    {
        case MOTOR_XY_PORT_INIT_CMD:
            MOTOR_XY_PORT_REGS |= _BV(MOTOR_X_LEFT_PIN);
            MOTOR_XY_PORT_REGS |= _BV(MOTOR_X_RIGHT_PIN);
            MOTOR_XY_PORT &= ~_BV(MOTOR_X_LEFT_PIN);
            MOTOR_XY_PORT &= ~_BV(MOTOR_X_RIGHT_PIN);
        case MOTOR_X_STOP_CMD:
            MOTOR_XY_PORT &= ~_BV(MOTOR_X_LEFT_PIN);
            MOTOR_XY_PORT &= ~_BV(MOTOR_X_RIGHT_PIN);
            break;
        case MOTOR_X_STEPUP_CMD:
            MOTOR_XY_PORT &= ~_BV(MOTOR_X_LEFT_PIN);
            MOTOR_XY_PORT |= _BV(MOTOR_X_RIGHT_PIN);
            break;
        case MOTOR_X_STEPDOWN_CMD:
            MOTOR_XY_PORT |= _BV(MOTOR_X_LEFT_PIN);
            MOTOR_XY_PORT &= ~_BV(MOTOR_X_RIGHT_PIN);
            break;
        case MOTOR_Y_STOP_CMD:
            MOTOR_XY_PORT &= ~_BV(MOTOR_Y_LEFT_PIN);
            MOTOR_XY_PORT &= ~_BV(MOTOR_Y_RIGHT_PIN);
            break;
        case MOTOR_Y_STEPUP_CMD:
            MOTOR_XY_PORT &= ~_BV(MOTOR_Y_LEFT_PIN);
            MOTOR_XY_PORT |= _BV(MOTOR_Y_RIGHT_PIN);
            break;
        case MOTOR_Y_STEPDOWN_CMD:
            MOTOR_XY_PORT |= _BV(MOTOR_Y_LEFT_PIN);
            MOTOR_XY_PORT &= ~_BV(MOTOR_Y_RIGHT_PIN);
            break;
        default:
            break;
    }
}

void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
        // we use a calibrated macro. This is more
        // accurate and not so much compiler dependent
        // as self made code.
        while(ms){
                _delay_ms(0.96);
                ms--;
        }
}

void flash_led(unsigned char nr)
{
    int n = 0;

    for ( n = 0; n < nr; n++ )
    {
        PORTD |= _BV(LED_PORTD_PIN);
        delay_ms(60);
        PORTD &= ~_BV(LED_PORTD_PIN);
        delay_ms(60);
    }
}

void test_rectangle_program(unsigned char size_x, unsigned char size_y)
{
    int n = 0;

    for ( n = 0; n < size_x; n++ )
    {
        motor_driver(MOTOR_X_STEPUP_CMD);
        delay_ms(motor_speed_delay);
        motor_driver(MOTOR_X_STOP_CMD);
    }

    for ( n = 0; n < size_y; n++ )
    {
        motor_driver(MOTOR_Y_STEPUP_CMD);
        delay_ms(motor_speed_delay);
        motor_driver(MOTOR_X_STOP_CMD);
    }

    for ( n = 0; n < size_x; n++ )
    {
        motor_driver(MOTOR_X_STEPDOWN_CMD);
        delay_ms(motor_speed_delay);
        motor_driver(MOTOR_X_STOP_CMD);
    }

    for ( n = 0; n < size_y; n++ )
    {
        motor_driver(MOTOR_Y_STEPDOWN_CMD);
        delay_ms(motor_speed_delay);
        motor_driver(MOTOR_X_STOP_CMD);
    }
}

int main(void)
{
    int n = 0;

    motor_speed_delay = MOTOR_STEP_DELAY_10;

    motor_driver(MOTOR_XY_PORT_INIT_CMD);

    DDRD = 0xFF;             // wyjscia na PORTD

    UBRR = VUBRR;            // ustawianie predkosci transmisji   
    UCR  = 1<<RXCIE | 1<<RXEN | 1<<TXCIE;   // zezwolenie na przerwania od odb nad i zezw. na odb

    sei();                   // wlacz przerwania

    send_to_pc(responde_crlf);
    send_to_pc(responde_start_msg);
    send_to_pc(responde_crlf);

    while(1)
    {
      if ( flagrx )
      {
         switch ( recived_cmd )
         {
            case CMD_SET_HOME_X:
                for ( n = 0; n < 6; n++ )
                {
                    PORTD |= _BV(LED_PORTD_PIN);
                    delay_ms(100);
                    PORTD &= ~_BV(LED_PORTD_PIN);
                    delay_ms(100);
                }
//                send_to_pc(response_ok);
               break;
            case CMD_SET_HOME_Y:
                for ( n = 0; n < 3; n++ )
                {
                    PORTD |= _BV(LED_PORTD_PIN);
                    delay_ms(100);
                    PORTD &= ~_BV(LED_PORTD_PIN);
                    delay_ms(100);
                }
//                send_to_pc(response_ok);
               break;
            case CMD_SET_UP_X:
                motor_driver(MOTOR_X_STEPUP_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_X_STOP_CMD);
//                send_to_pc(response_ok);
               break;
            case CMD_SET_DOWN_X:
                motor_driver(MOTOR_X_STEPDOWN_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_X_STOP_CMD);
//                send_to_pc(response_ok);
               break;
            case CMD_SET_UP_Y:
                motor_driver(MOTOR_Y_STEPUP_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_Y_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_SET_DOWN_Y:
                motor_driver(MOTOR_Y_STEPDOWN_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_Y_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_SET_DOWN_X_SPEED:
                motor_driver(MOTOR_X_STEPDOWN_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_X_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_SET_UP_X_SPEED:
                motor_driver(MOTOR_X_STEPUP_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_X_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_SET_DOWN_Y_SPEED:
                motor_driver(MOTOR_Y_STEPDOWN_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_Y_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_SET_UP_Y_SPEED:
                motor_driver(MOTOR_Y_STEPUP_CMD);
                delay_ms(motor_speed_delay);
                motor_driver(MOTOR_Y_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_TEST_1:
                //flash_led(1);
                motor_driver(MOTOR_X_STEPDOWN_CMD);
                delay_ms(5);
                motor_driver(MOTOR_X_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_TEST_2:
                //flash_led(2);
                motor_driver(MOTOR_X_STEPUP_CMD);
                delay_ms(5);
                motor_driver(MOTOR_X_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_TEST_3:
                //flash_led(3);
                motor_driver(MOTOR_Y_STEPDOWN_CMD);
                delay_ms(5);
                motor_driver(MOTOR_Y_STOP_CMD);
                send_to_pc(response_ok);
               break;
            case CMD_TEST_4:
/*
                //flash_led(4);
                motor_driver(MOTOR_Y_STEPUP_CMD);
                delay_ms(5);
                motor_driver(MOTOR_Y_STOP_CMD);
                send_to_pc(response_ok);
*/
                test_rectangle_program(10, 10);
               break;
            case CMD_SET_MOTOR_SPEED_5:
                motor_speed_delay = MOTOR_STEP_DELAY_5;
               break;
            case CMD_SET_MOTOR_SPEED_7:
                motor_speed_delay = MOTOR_STEP_DELAY_7;
               break;
            case CMD_SET_MOTOR_SPEED_10:
                motor_speed_delay = MOTOR_STEP_DELAY_10;
               break;
            case CMD_SET_MOTOR_SPEED_20:
                motor_speed_delay = MOTOR_STEP_DELAY_20;
               break;
            case CMD_SET_MOTOR_SPEED_50:
                motor_speed_delay = MOTOR_STEP_DELAY_50;
               break;
            default:
                send_to_pc(response_cmd_not_implemented);
                break;
         }

         flagrx=0;   //zeruje flage odbioru zeby juz nie wchodzilo w tego if-a
      }

    }

    return 0;
}

