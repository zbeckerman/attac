/* httpget.c
 * Demoprogramm zur Programmierung von Netzwerkclients
 * Es wird ein GET request via http an einen Webserver 
 * gesendet und das Ergebnis auf der Konsole ausgegeben. */
#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
/* Headerfiles für Windows */
#include <winsock.h>
#include <io.h>

#else
/* Headerfiles für Unix/Linux */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket(s) close(s)
#endif

/* http requests werden normalerweise auf Port 80 
 * vom Server entgegengenommen */
#define HTTP_PORT 80

/****************** MAIN *********************/
int main( int argc, char **argv)
{
	char	line[]="--------------------------------------------------";
    struct sockaddr_in server;
    struct hostent *host_info;
    unsigned long addr;
    int sock;
    char buffer[8192];
    int count;
    
    
#ifdef _WIN32  
    /* Initialisiere TCP für Windows ("winsock") */
    short wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD (1, 1);
    if (WSAStartup (wVersionRequested, &wsaData) != 0) {
        fprintf( stderr, "Failed to init windows sockets\n");
        exit(1);
    }
#endif

    /* Sind die erforderlichen Kommandozeilenargumente vorhanden? */
    if (argc != 3) {
        fprintf( stderr, "usage: httpget server file\n");
        exit(1);
    }

    /* Erzeuge das Socket */
    sock = socket( PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror( "failed to create socket");
        exit(1);
    }

    /* Erzeuge die Socketadresse des Servers 
     * Sie besteht aus Typ, IP-Adresse und Portnummer */
    memset( &server, 0, sizeof (server));
    if ((addr = inet_addr( argv[1])) != INADDR_NONE) {
        /* argv[1] ist eine numerische IP-Adresse */
        memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
    }
    else {
        /* Wandle den Servernamen in eine IP-Adresse um */
        host_info = gethostbyname( argv[1]);
        if (NULL == host_info) {
            fprintf( stderr, "unknown server: %s\n", argv[1]);
            exit(1);
        }
        memcpy( (char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( HTTP_PORT);

    /* Baue die Verbindung zum Server auf */
    if ( connect( sock, (struct sockaddr*)&server, sizeof( server)) < 0) {
        perror( "can't connect to server");
        exit(1);
    }

	/* Prepare HTTP buffer */
	sprintf(buffer, "GET %s HTTP/1.1\r\nHost: %s\r\n", argv[2], argv[1]);
	  /* User-agent */
	strcat(buffer, "User-Agent: Mozilla/4.0 http_get");
	strcat(buffer, "\r\n");
 	strcat(buffer, "Accept: */*\r\n");
 	strcat(buffer, "Connection: close\r\n"); 
	strcat(buffer, "\r\n");
	printf("%s\nREQUEST\n\n%s%s\n", line, buffer, line);

    send( sock, buffer, strlen( buffer), 0);

    /* Hole die Serverantwort und gib sie auf Konsole aus */
    do {
        count = recv( sock, buffer, sizeof(buffer), 0);
        write( 1, buffer, count);
		printf ("count: %d\n", count);
    }
    while (count > 0);

    /* Schließe Verbindung und Socket */
    closesocket( sock);
    return count;
}
