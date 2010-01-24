/*
 * program stworzony w oparciu o program mojego autorstwa "jabber_view.c"
 * (pobierający i wyświetlający komunikaty IM sieci Jabber z bazy danych MySQL)
 * jest serwerem informacji dla (energooszczędnego) klijenta opartego na
 * mikrokontrolerze z interfejsem Ethernetowskim
 *
 * program wysyła okresowo komunikaty UDP z informacją o aktualnym czasie (UTC)
 * oraz liczba wiadomości przechowywanych dla danego użytkownika,
 * umożliwia on ponadto połączenie się po TCP celem pobrania treści przechowywanych wiadomości
 *
 *
 * program wywolany bez żadnych parametrów spróbuje pozyskać nazwę użytkownika
 * uruchamiającego i wykozystać ją do pobrania danych o wiadomościach jabbera
 *
 * kompilacja: gcc jabber_info_server.c -o jabber_info_server -lmysqlclient -lpam -lpam_misc -lpthread
 *
 */

// UWAGA: wkompilowane na stale do pliku binarnego haslo jest bardzo latwo z niego wyciagnac !!!
// dlatego znacznie lepiej aby bylo ono w osobnym pliku do odczytu tylko dla uzytkownika
// na prawch ktorego chodzi ten program (z ustawionym suid - oczywiscie nie root)
// i aby bylo czytane z tego pliku konfiguracyjnego ...
//#define _haslo_do_bazy_danych_ ""
//#define _plik_z_haslem_do_bazy_danych_ "/etc/jabberd2/db_pass"

#define _SERWER_NAME_ "n17.waw.pl"
#define _PORT_ 3333
#define _PORT_UDP_ 4444
#define _IP_ "192.168.0.100"
#define BUF_SIZE 255
#define QUERY_SIZE 10

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <mysql/mysql.h>
#include <stdio.h>

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

/** zmienne globalne **/
char not_send_udp;
char buf[BUF_SIZE];
char query[100];
char query_udp[100];
#ifndef _haslo_do_bazy_danych_
	char _haslo_do_bazy_danych_ [255];
#endif


/** przetwarzanie wiadomosci zapisanej w XML **/
void parse_jabber_xml(char *napis0, char **parse_res){
	char *napis1, *napis2, *napis3;

	// find time info
	napis2 = strstr(napis0, "stamp='");
	if (napis2 != NULL) {
		napis2 += 7;
		parse_res[3] = napis2;

		if ( strstr(napis1, ">") < napis2 )
			parse_res[0] = NULL;

		// find end from time stamp
		napis3 = strstr(napis2, "'");
	}

	// find start from JID
	napis1 = strstr(napis0, "<message ");
	if (napis1 == NULL)
		return;
	napis2 = strstr(napis1, "from='");
	if (napis2 != NULL) {
		napis2 += 6;
		parse_res[0] = napis2;

		if ( strstr(napis1, ">") < napis2 )
			parse_res[0] = NULL;

		// find end from JID
		napis1 = strstr(napis2, "'");
		napis2 = strstr(napis2, "/");
		if (napis2<napis1) {
			*napis2 = '\0';
			napis0 = napis2 +1;
		} else {
			*napis1 = '\0';
			napis0 = napis1 +1;
		}
	}

	// find start subject
	napis1 = strstr(napis0, "<subject>");
	if (napis1 != NULL) {
		napis1 += 6;
		parse_res[1] = napis1;

		// find end subject
		napis1 = strstr(napis1, "</subject>");
		*napis2 = '\0';
		napis0 = napis2 + 1;
	}

	// find start message
	napis1 = strstr(napis0, "<body>");
	if (napis1 != NULL) {
		napis1 += 6;
		parse_res[2] = napis1;

		// find end message
		napis1 = strstr(napis1, "</body>");
		*napis1 = '\0';
	}

	// mark end of time
	*napis3 = '\0';
}

/** wątek odpowiedzialny z wysyłanie informacji UDP **/
int send_udp_info() {
	char udp_buf[36];
	time_t timestamp;
	struct tm czas;

	int sh_udp = socket(PF_INET, SOCK_DGRAM, 0);
	if (sh_udp<0) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in to;
	to.sin_family=PF_INET;
	to.sin_port=htons(_PORT_UDP_);
	inet_aton(_IP_, &to.sin_addr);

	/** pobieranie dancyh **/
	while (1) {
		MYSQL mysql_connection_udp;
		MYSQL_RES *result_udp;
		MYSQL_ROW row_udp;
		mysql_init(&mysql_connection_udp);
		mysql_real_connect(&mysql_connection_udp, "localhost", "jabberd2",
			_haslo_do_bazy_danych_, "jabberd2", 3306, NULL, 0);

		mysql_real_query(&mysql_connection_udp, query_udp, strlen(query_udp));
		result_udp = mysql_use_result(&mysql_connection_udp);
		row_udp=mysql_fetch_row(result_udp);

		/** czas i wysylka **/
		time(&timestamp);
		gmtime_r(&timestamp, &czas);

		sprintf(udp_buf, "%.4d-%.2d-%.2d %.2d:%.2d\n%s wiadomosc%s",
			czas.tm_year+1900, czas.tm_mon+1, czas.tm_mday, czas.tm_hour, czas.tm_min,
			row_udp[0], atoi(row_udp[0])==1?"":"i");
		//puts(udp_buf);
		sendto(sh_udp, udp_buf, strlen(udp_buf), 0, (struct sockaddr *) &to, sizeof(struct sockaddr_in));

		mysql_close(&mysql_connection_udp);

		sleep(60);
		while(not_send_udp)
			sleep(60);
	}
}

int main(int argc, char *argv[]) { // pierwszy argument - uzytkownik , STDIN - haslo
	char * username;
	struct passwd *userstr;

	if (argc == 2) {
		/** autoryzacja użytkownika **/

		username = argv[1];

		struct pam_conv conv = {misc_conv, NULL};
		pam_handle_t *pam = NULL;

		int pam_connection = pam_start("jabberd", username, &conv, &pam);

		if (pam_authenticate(pam, 0) != PAM_SUCCESS) {
			fprintf(stderr, "Błąd autoryzacji użytkownika\n");
			return -1;
		} else fprintf(stderr, "OK\n");

		pam_end(pam, pam_connection);
	} else {
		/** pobieranie nazwy aktualnego uzytkownika **/

		userstr = getpwuid( getuid() );
		if (userstr)
			username = userstr->pw_name;
	}

	sprintf(query,
		"SELECT `xml` FROM `queue` WHERE `collection-owner`='%s@%s' ORDER BY `object-sequence`",
		username, _SERWER_NAME_);
	sprintf(query_udp,
		"SELECT COUNT(*) FROM `queue` WHERE `collection-owner`='%s@%s'",
		username, _SERWER_NAME_);

	/** polaczenie i obsluga MySQL **/

#ifndef _haslo_do_bazy_danych_
	FILE *plik;
	plik=fopen(_plik_z_haslem_do_bazy_danych_, "r");
	fgets( _haslo_do_bazy_danych_, 254, plik );
	int ostatni = strlen(_haslo_do_bazy_danych_) - 1;
	_haslo_do_bazy_danych_[ostatni]='\0';
#endif


	/** wątek informacji UDP **/
	pthread_t watek1;
	not_send_udp = 0;
	pthread_create( &watek1, 0, send_udp_info, NULL );


	/** nasłuchiwanie TCP **/
	int res;

	// gniazdo ...
	int sh = socket(PF_INET, SOCK_STREAM, 0);
	if (sh<0) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// utworzenie struktury opisującej adres
	struct sockaddr_in serwer;
	serwer.sin_family=AF_INET;
	serwer.sin_port=htons(_PORT_);
	serwer.sin_addr.s_addr=INADDR_ANY;

	// przypisanie adresu ...
	if (bind(sh, (struct sockaddr *) &serwer, sizeof(struct sockaddr_in)) < 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while(1) {
		// otwarcie portu do nasluchiwania
		if (listen(sh, QUERY_SIZE) < 0) {
			fprintf(stderr, "%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// odebranie połączenia
		struct sockaddr_in from;
		socklen_t fromlen=sizeof(struct sockaddr_in);
		int sh2 = accept(sh, (struct sockaddr *) &from, &fromlen);
//		not_send_udp = 1;

		if (from.sin_addr.s_addr != inet_addr(_IP_)) {
			close(sh2);
			continue;
		}

		FILE * net;
		net=fdopen(sh2, "r+");

		fgets(buf, BUF_SIZE, net);
		if (buf[0] == 'n') {
			MYSQL mysql_connection;
			MYSQL_RES *result;
			MYSQL_ROW row;
			mysql_init(&mysql_connection);
			mysql_real_connect(&mysql_connection, "localhost", "jabberd2",
				_haslo_do_bazy_danych_, "jabberd2", 3306, NULL, 0);

			mysql_real_query(&mysql_connection, query, strlen(query));
			result = mysql_use_result(&mysql_connection);
			do {
				/** pobieranie, przetwarzanie i wysylanie dancyh **/
				char *parse_res[4], *czas;
				if (row=mysql_fetch_row(result)) {
					parse_res[0] = NULL; parse_res[1] = NULL; parse_res[2] = NULL; parse_res[3] = NULL;
					parse_jabber_xml(row[0], parse_res);
					czas = parse_res[3];
					czas[14] = '\0';
					if (parse_res[1] != NULL) {
						fprintf(net, "%s %s [%s] %s", czas+9, parse_res[0], parse_res[1], parse_res[2]);
					} else {
						fprintf(net, "%s %s %s", czas+9, parse_res[0], parse_res[2]);
					}
					fflush(net);
				} else {
					fputc(0x04, net); // EOT
					fflush(net);
					break;
				}
				fgets(buf, BUF_SIZE, net);
			} while (buf[0] == 'n');

			mysql_close(&mysql_connection);
		}

		fclose(net);
		not_send_udp = 0;
	}

	// zamkniecie gniazda
	close(sh);
}

