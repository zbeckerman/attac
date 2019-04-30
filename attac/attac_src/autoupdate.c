// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>
#include <stdio.h>

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

#include <process.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "rexx.h"
#include "parsedatathread.h"
#include "attacUtils.h"

#define HTTP_PORT 80


void AutoUpdateThreadProc( void *dummy )
{
	struct sockaddr_in server;
    struct hostent *host_info=NULL;
    unsigned long addr;
    int sock;
    char host[80], filename[256], buffer[8192], *token, tBuffer[100], *realString;
    int count, showReleaseNote=0, leftFind, rightFind, i;
	char *verRel, *verMaj, *verMin, *releaseNote;
	int totalSize=0;

	#ifdef _WIN32  
		short wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD (1, 1);
		if (WSAStartup (wVersionRequested, &wsaData) != 0) {
			fprintf( stderr, "Failed to init windows sockets\n");
			return;
		}
	#endif

	if (ATTAC_Time() < (header.lastUpdateTime + 604800))
	{
		#ifdef _WIN32  
			WSACleanup();
		#endif
		return;
	}
	strcpy (host, "www.tw-attac.com");
	strcpy (filename, "/update.txt");
	strcpy (buffer, "");

	sock = socket( PF_INET, SOCK_STREAM, 0);
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
    server.sin_port = htons( HTTP_PORT);

    if ( connect( sock, (struct sockaddr*)&server, sizeof( server)) < 0) {
        printf( "can't connect to server");
		#ifdef _WIN32  
			WSACleanup();
		#endif
        return;
    }

	// Prepare HTTP buffer
	sprintf(buffer, "GET %s HTTP/1.1\r\nHost: %s\r\n", filename, host);
	  // User-agent
	strcat(buffer, "User-Agent: Mozilla/4.0 http_get");
	strcat(buffer, "\r\n");
 	strcat(buffer, "Accept: */*\r\n");	
 	strcat(buffer, "Connection: close\r\n"); 
	strcat(buffer, "\r\n");

    send( sock, buffer, strlen( buffer), 0);

    do {
        count = recv( sock, buffer, sizeof(buffer), 0);
		totalSize+=count;
        write( 1, buffer, count);
    }
    while (count > 0);
	closesocket( sock);

	buffer[totalSize]=0;

	leftFind = strlen("Content-Length: ") + Instr (1, buffer, "Content-Length: ");

	rightFind = Instr (leftFind+1, buffer, "\n");

	leftFind=leftFind-1;
	rightFind=rightFind+1;


	for (i=0; i< rightFind-leftFind && i<100; i++)
	{
		tBuffer[i] = buffer[leftFind];
		leftFind=leftFind+1;
	}
	tBuffer[i]=0;

	realString = (char *) right (buffer, atoi(tBuffer));
	token = strtok(realString, "^");

	if (token==NULL)
	{
		#ifdef P_FREE
			pfree(&realString);
		#else
			free(realString);
		#endif
		#ifdef _WIN32  
			WSACleanup();
		#endif
		return;
	}

	releaseNote = strtok(NULL, "^");

	verRel = strtok(token, ".");
	verMaj = strtok(NULL, ".");
	verMin = strtok(NULL, ".");

	showReleaseNote=0;
	if (atol(verRel)>atol(releaseNumber))
		showReleaseNote=1;
	else if (atol(verRel)==atol(releaseNumber))
	{
		if (atol(verMaj)>atol(majorNumber))
			showReleaseNote=1;
		else if (atol(verMaj)==atol(majorNumber))
		{
			if (atol(verMin)>atol(minorNumber))
				showReleaseNote=1;
		}
	}

	if (showReleaseNote==1)
	{
		MessageBox(NULL, releaseNote, "ATTAC Update Available", MB_OK);
		header.lastUpdateTime = ATTAC_Time();
		writeSetReady();
	}

	#ifdef P_FREE
		pfree(&realString);
	#else
		free(realString);
	#endif

	#ifdef _WIN32  
		WSACleanup();
	#endif

	_endthread();
}
