/*
 * Program przeznaczony dla systemu Nut/OS ( http://www.ethernut.de/ ), służy do komunikacji
 * mikrokontrolera z serwerem informacji o wiadomościach IM sieci Jabber;
 * liczbę oczekujących wiadomości otrzymuje jako regularnie wysyłany pakiet UDP
 * oraz umożliwia przejrzenie pełnych treści wiadomości z wykorzystaniem protokołu TCP
 *
 * FUNKCJE PRZYCISKOW:
 * SW0 - wyswietl (nastepna) wiadomosc; gdy:
 *         wyswietlany komunikat, oczekiwanie na komunikat, informacja o koncu wiadomosci
 * SW0 - wyjscie do trybu komunikatów; gdy:
 *         wyswietlane info ze nie ma wiecej wiadomosci
 * SW1 - wyjscie do trybu komunikatów; gdy:
 *         informacja o koncu wiadomosci, wyswietlane info ze nie ma wiecej wiadomosci
 * SW1 - wyjscie do "koniec wiadomosci"; gdy:
 *         wyswietlana wiadomosc
 * SW2 / SW3 - przewijanie wiadomosci w przdd / tyl; gdy
 *         wyswietlana wiadomosc
 *
 */

/*
 */


#include <stdio.h>
#include <io.h>
#include <sys/timer.h>

// UART and Ethernet and LCD
#include <dev/board.h>
#include <dev/nicrtl.h>
#include <arpa/inet.h>
#include <sys/confnet.h>
#include <dev/hd44780_bus.h>
#include <dev/term.h>

#include <sys/socket.h>

#define _UDP_PORT_ 4444

// bezposredni dostep do pamieci po adresie, w oparciu o funkcje nic_inlb, nic_outlb z ethernut
#define mem_read_byte(reg) (*(volatile u_char *) (reg))
#define mem_write_byte(reg, val) (*(volatile u_char *) (reg) = (val))


// funkcja pobiera linię z UART
// jezeli podano out != NULL linia bedzie wypisana do tego pliku (np. na uart ...)
void UartGetLine(u_char *buff, int len, FILE *out) {
	int i=0;
	while (i<len) {
		while ((inb(USR) & 1<<RXC) == 0); // czekamy na znak
		buff[i] = inb(UDR);
		if (out!= NULL) fputc(buff[i], out);
		if (buff[i] == '\n')
			break;
		i++;
	}
	buff[i] = '\0';
}

// funkcja pobierajaca konfiguracje IP z UART
u_long UartGetConfig(u_char * msg, u_long val, FILE * out) {
	u_long tmp;
	char inbuff[32];
	while(1) {
		fprintf(out, "Podaj %s [%s]: ", msg, inet_ntoa(val));
		UartGetLine(inbuff, 32, out);
		if (inbuff[0]=='\0' || inbuff[1]=='\0') {
			fprintf(out, " ... bez zmian\n");
			return val;
		}
		tmp=inet_addr(inbuff);
		if (tmp != (u_long) -1) {
			fprintf(out, " ... zmieniono\n");
			return tmp;
		}
	}
}





void main(void) {
	/// konfiguracja trybu pracy kontrolera pamięci
	mem_write_byte(0xff01, 0x00);
	/// wylaczenie przetwornikow A/D
	sbi(ACSR, ACD);


	/// uruchamianie RS-232
	NutRegisterDevice(&DEV_UART, 0, 0);
	outb(UCR, 1<<RXEN | 1<<TXEN); //| 1 << RXCIE; // włączenie nadajnika, odbionika UART wraz z generowaniem przerwań
	outb(UBRR, 51); // szybkosc transmisji 9600 bps
	FILE *uart = fopen(DEV_UART_NAME, "w");

	/// wypisanie czegos przez RS-232
	fprintf(uart, "Hello World !!!\n");
	fprintf(uart, "connection detect ...\n");

	/// wykrywanie polaczenia przez RS-232
	uint32_t i;
	char uart_is_use=0;
	for (i=0; i<2000000; i++) { // ok 4 sek.
		if ((inb(USR) & 1<<RXC) != 0) {
			uart_is_use=1;
			break;
		}
	}

	if (!uart_is_use) {
		fprintf(uart, "UART is NOT use !\n");
	} else {
		fprintf(uart, "UART is use !\n");

		/// oproznienie bufora
		while ((inb(USR) & 1<<RXC) != 0) inb(UDR);

		fprintf(uart, "Napisz cos:\n");

		/// wczytanie czegos przez RS-232
		char inbuff[32];
		UartGetLine(inbuff, 32, uart);
		fprintf(uart, "Napisales: %s\n", inbuff);
	}


	/// konfigurator sieci
	fprintf(uart, "Konfigurowanie sieci\n");
	NutNetLoadConfig("eth0");
	u_long ip_add=confnet.cdn_cip_addr;
	u_long ip_mask=confnet.cdn_cip_addr;
	if (ip_add==0) {
		fprintf(uart, "  uzycie konfiguracji domyslnej\n");
		ip_add=0x6418A8C0; // 192.168.0.100
		ip_mask=0x0000ffff; // 255.255.0.0
	}

	if (uart_is_use) {
		ip_add = confnet.cdn_cip_addr = UartGetConfig("IP  ", ip_add, uart);
		ip_mask = confnet.cdn_ip_mask = UartGetConfig("Mask", ip_mask, uart);
		fprintf(uart, "zapisac [y/*]\n");
		char inbuff[3];
		UartGetLine(inbuff, 3, uart);
		if (inbuff[0]=='y') {
			fprintf(uart, " zapisano\n");
			NutNetSaveConfig();
		}
	}


	/// siec
	fprintf(uart, "Wczytana konfiguracja sieci:\n");
	fprintf(uart, "IP:     %s (%lx)\n", inet_ntoa(ip_add), ip_add);
	fprintf(uart, "Mask:   %s (%lx)\n", inet_ntoa(ip_mask), ip_mask);
	//jest tez: confnet.cdn_gateway oraz confnet.cdn_mac[0] ... confnet.cdn_mac[5]

	fprintf(uart, "Uruchamianie sieci\n");
	if (NutRegisterDevice(&DEV_ETHER, 0x8300, 5))
		fputs("Registering device failed", uart);

	if(NutNetIfConfig("eth0", NULL, ip_add, ip_mask))
	fprintf(uart, "Siec skonfigurowana\n");


	/// lcd
	fprintf(uart, "LCD Start\n");
	NutRegisterDevice(&devLcdBus, 0xff04, 0);
	FILE *lcd = fopen("lcdbus", "w");
	fprintf(lcd, ESC_CLR "oczekiwanie na\nkomunikat");

	DDRD = 0;
	PORTD = 0xff;

	UDPSOCKET* sock;
	sock = NutUdpCreateSocket(_UDP_PORT_);
	TCPSOCKET* sock_tcp;
	u_long addr;
	u_short port;
	u_char data[225];
	u_short size;
	u_char set_break;


	while (1) {
		while ((PIND & 0x01) != 0) {
			size = NutUdpReceiveFrom(sock,&addr,&port,data,sizeof(data),100);
			if (size > 0) {
				data[size]=0;
				fprintf(uart, "%i : %s", size, data);

				fprintf(lcd, ESC_CLR);
				fprintf(lcd, "%s", data);
			}
		}
		while ((PIND & 0x01) == 0);

		// po nacisnieciu przysisku

		sock_tcp = NutTcpCreateSocket();
		NutTcpConnect(sock_tcp, inet_addr("192.168.0.1"), 3333);
		set_break = 0;

		while (1) { // do nacisniecia guzika
			NutTcpSend(sock_tcp, "n\n", 2);
			do {
				size = NutTcpReceive(sock_tcp, data, sizeof(data)-1);
				data[size]='\0';
				fprintf(uart, "%i : %s", size, data);
				if (data[0]==0x04) {
					fprintf(lcd, ESC_CLR "Nie ma wiecej\nwiadomosci");
					set_break = 1;
				} else {
					// wyswietlanie z podzialem na linie
					i = 0;
					while(1) {
						if (i<0)
							i = 0;

						if (i+16<size)
							fprintf(lcd, ESC_CLR "%.16s\n%.16s", data+i, data+i+16);
						else if (i<size)
							fprintf(lcd, ESC_CLR "%.16s", data+i);
						else
							break;

						while ((PIND & 0x04) != 0 && (PIND & 0x08) != 0 && (PIND & 0x02) != 0);
							// czekamy na PD1, PD2 lub PD3 zwarte do masy
						if ((PIND & 0x04) == 0)
							i=i+32;
						else if ((PIND & 0x08) == 0)
							i=i-32;
						else if ((PIND & 0x02) == 0)
							break;

						NutSleep(100);
						while ((PIND & 0x04) == 0 || (PIND & 0x08) == 0 || (PIND & 0x02) == 0);
							// wszystkie 3 w stanie wysokim
					}
					if (size < 224)
						fprintf(lcd, ESC_CLR "koniec\nwiadomosci");
				}
			} while (size >= 224);

			while ((PIND & 0x01) != 0 && (PIND & 0x02) != 0);
				// czekamy na PD0 lub PD1 zwarte do masy
			if ((PIND & 0x02) == 0 || set_break == 1)
				break;

			NutSleep(100);
			while ((PIND & 0x01) == 0 || (PIND & 0x02) == 0);
				// oba w stanie wysokim
		}
		NutTcpCloseSocket(sock_tcp);
		fprintf(lcd, ESC_CLR "oczekiwanie na\nkomunikat");
	}
}

