// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>
#include <stdio.h>
#include "authorizeRegistration.h"
#ifdef WIN32
	#include <winsock2.h>
	#include <io.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#define closesocket(s) close(s)
#endif

#define SMTP_PORT 25


void AuthorizeRegistrationThreadProc( void *dummy )
{
	struct sockaddr_in server;
    struct hostent *host_info=NULL;
    unsigned long addr;
    int sock;
    char host[80], filename[256], buffer[8192], *token, tBuffer[100], *realString;
    int count, showReleaseNote=0, leftFind, rightFind, i;
	char *verRel, *verMaj, *verMin, *releaseNote;
	int totalSize=0;

	char smtpMailServer[30] = "mx01.hotmail.com";
	char mailFrom[12] = "mail from: ";
	char sendTo[10] = "rcpt to: ";
	char to[5] = "to: ";
	char from[7] = "from: ";
	char subject[10] = "subject: ";
	char messageBody[1000];
	char hostbuff[100], address[100], returnAddress[100], msgSubject[100], address2[100], msgBody[1000], mailfrom[100], bodyfrom[100], recvbuff2[200];
	

	#ifdef _WIN32  
		short wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD (1, 1);
		if (WSAStartup (wVersionRequested, &wsaData) != 0) {
			fprintf( stderr, "Failed to init windows sockets\n");
			return;
		}
	#endif

	strcpy(messageBody,"\r\n");

	strcpy(mailFrom,"ATTAC Authorization");
	strcat(mailFrom,"\r\n");

	strcat(from,bodyfrom);
	strcat(from,"\r\n");

	//@todo use config data here
	strcat(sendTo, "tradewars@hotmail.com");
	strcat(sendTo, "\r\n");

	strcat(to,"tradewars@hotmail.com");
	strcat(to,"\r\n");

	strcat(subject, "registration");
	strcat(subject, "\r\n");

	
	strcat(messageBody,"test body");
	strcat(messageBody,"\r\n");

	strcpy (host, "www.tw-attac.com");
	strcpy (filename, "/twxSyncAuth.txt");
	strcpy (buffer, "");

	sock = socket( AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
	{
        printf ( "failed to create socket");
		#ifdef _WIN32  
			WSACleanup();
		#endif
        return;
    }

    memset( &server, 0, sizeof (server));
    if ((addr = inet_addr( host)) != INADDR_NONE) {
        memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
    }
    else {
        host_info = gethostbyname( host);
        if (NULL == host_info) {
            printf("unknown server: %s\n", host);
			#ifdef _WIN32  
				WSACleanup();
			#endif
            return;
        }
        memcpy( (char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( SMTP_PORT);

	bind(sock,(struct sockaddr*)&server,sizeof(server));

    if ( connect( sock, (struct sockaddr*)&server, sizeof( server)) < 0) {
        printf( "can't connect to server");
		#ifdef _WIN32  
			WSACleanup();
		#endif
        return;
    }

	char recvbuff2[200];
	if(recv(sock,recvbuff2,200,0))
	{
		recvbuff2[0] = '0';
	}
	sprintf(buffer, "helo %s\r\n", smtpMailServer);
	send(sock,buffer,strlen(buffer),0);

	if(recv(sock,recvbuff2,200,0))
	{
		recvbuff2[0] = '0';
	}
	send(sock,MAILFRO2,strlen(MAILFRO2),0);

	if(recv(sock,recvbuff2,200,0))
	{
		recvbuff2[0] = '0';
	}
	send(sock,SENDTO2,strlen(SENDTO2),0);

	if(recv(sock,recvbuff2,200,0))
	{
		recvbuff2[0] = '0';
	}
	send(sock,INIT,strlen(INIT),0);

	if(recv(sock,recvbuff2,200,0))
	{
		recvbuff2[0] = '0';
	}

	send(sock,TO2,strlen(TO2),0);
	send(sock,BODFRO2,strlen(BODFRO2),0);
	send(sock,SUBJECT2,strlen(SUBJECT2),0);
	send(sock,DATE,strlen(DATE),0);
	send(sock,MSGBODY,strlen(MSGBODY),0);

	send(sock,SENDNOW,strlen(SENDNOW),0);
	if(recv(sock,recvbuff2,200,0))
	{
		recvbuff2[0] = '0';
	}
	send(sock,BYE,strlen(BYE),0);

	closesocket( sock);



	#ifdef _WIN32  
		WSACleanup();
	#endif

}
