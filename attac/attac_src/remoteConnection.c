// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#define IPV6

#ifdef IPV6
#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <tpipv6.h>
#else
#include <winsock.h>
#endif
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <errno.h>       /* obligatory includes */
#include <signal.h>
#include <time.h> 
#include <process.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "attacGUI.h"
#include "remoteConnection.h"
#include "parsedatathread.h"
#include "win_res.h"
#include "rexx.h"
#include "util.h"

	// define _MT so that _beginthread( ) is available
	#ifndef _MT
	#define _MT
	#endif

int i, n;			// Our temporary variables


 typedef u_long IOCTL_TYPE; 
 typedef char SOCKOPT_TYPE; 
 typedef unsigned int pid_t; 

int remoteConnectionEnabled=0;
int nextConnection=0;
CLIENTS_CONNECTED theClients[TOTAL_CONNECTIONS];

SOCKET remoteSocket; 
struct sockaddr_in remote_addr; 



 int sockEnd(void) {
	shutdown(remoteSocket, SD_SEND);
	closesocket(remoteSocket);
	return 0;
 } 


void remoteConnectionThreadProc(void *dummy)
{
	remoteConnectionEnabled=1;
	startRemoteConnectionServer();
	_endthread();
}


void stopRemoteConnectionServer()
{
	int i;

	for (i=0; i<TOTAL_CONNECTIONS; i++)
	{
		if (theClients[i].sock>0)
		{
			attacErrorMessage("closing client.\n");
			shutdown(theClients[i].sock, SD_SEND);
			close (theClients[i].sock);
			theClients[i].sock=0;
		}
		theClients[i].sock=0;
	}


	for (i=0; i < TOTAL_CONNECTIONS; i++)
		theClients[i].sock=0;

	updateRemoteList();
	remoteConnectionEnabled=0;
	sockEnd();
}

void startRemoteConnectionServer()
{
	struct hostent *hp; 
	struct sockaddr_in any_addr; 
	int addrlen; 
	SOCKET ss; 
	int namelen, i; // length of client name 
	char *tmp, buf[128];

	remoteSocket=INVALID_SOCKET; 
	hp=NULL; 
	CLEAR_ADDR(&any_addr); 
	CLEAR_ADDR(&remote_addr); 
	addrlen=sizeof(struct sockaddr_in); 

	/* specifies host and port */ 
	any_addr.sin_family=AF_INET; 
	any_addr.sin_port= htons((short)header.remoteConnectionPort); 

	/* accepts connections from any client */ 
	any_addr.sin_addr.s_addr=htonl ( INADDR_ANY ); 

	/* creates socket */ 
	remoteSocket = socket(AF_INET,SOCK_STREAM,0); 

	if(remoteSocket==INVALID_SOCKET) 
	{ 
		attacErrorMessage("Can't create socket"); 
		sockEnd(); 
		return; 
	} 


	/* links socket to address and port */ 
	if(bind(remoteSocket,(struct sockaddr *) &any_addr, sizeof(struct sockaddr_in))) 
	{ 
		attacErrorMessage("Can't bind socket to address"); 
		sockEnd(); 
		return; 
	} 

	// find out what port was assigned 
	namelen = sizeof( any_addr );
	if( getsockname( remoteSocket, (struct sockaddr *) &any_addr, &namelen) < 0 ) 
	{
		attacErrorMessage("getsockname() failed to get port number");
	}

	sprintf (buf, "The assigned port is %d", ntohs( any_addr.sin_port));
	attacInfoMessage(buf);

	/* listens on port; maximum 5 requests pending */ 
	listen(remoteSocket,5); 

	while(1)
	{ 
		/* when someone connects creates a new socket for data exchange */ 
		ss=accept(remoteSocket,(struct sockaddr *) &remote_addr,&addrlen); 
		
		if (remoteConnectionEnabled==0)
		{
			break;
		}
		else
		{
			if (ss>0)
			{
				nextConnection=-1;
				for (i=0; i<TOTAL_CONNECTIONS; i++)
				{
					if (theClients[i].sock==0)
					{
						nextConnection=i;
						i=TOTAL_CONNECTIONS;
					}
				}
				if (nextConnection==-1)
				{
					//too many connections
						attacInfoMessage("Max Connections Have Been Reached! Connection Rejected");
						
						strcpy (buf, " ");
						tmp = createANSIText(buf, ATTAC_BLACK, ATTAC_BLACK, 1, 1);
						send(ss, tmp, strlen(tmp), 0);
						#ifdef P_FREE
							pfree(&tmp);
						#else
							free(tmp);
						#endif		
						strcpy (buf, "Max Connections Have Been Reached!");
						tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 1);
						send(ss, tmp, strlen(tmp), 0);
						#ifdef P_FREE
							pfree(&tmp);
						#else
							free(tmp);
						#endif		
						strcpy (buf, "Connection Closed");
						tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 1);
						send(ss, tmp, strlen(tmp), 0);
						#ifdef P_FREE
							pfree(&tmp);
						#else
							free(tmp);
						#endif		

						shutdown(ss, SD_SEND);
						closesocket (ss);
				}
				else
				{
					if(_beginthread(processConnection, 0, (void*)ss )==-1)
					{
						printf("Failed to create thread");
						return;
					}
				}
			}
		}
	} 

	/* if no error occurs we never reach this point */ 
	sockEnd(); 
}


int checkRemoteConnectionPassword(int clntSocket)
{
	char echoBuffer[128]; /* Buffer for echo string */
	int recvMsgSize, passVal=0; /* Size of received message */
	char userPass[40];

	while(1) 
	{
		/* Receive message from client */
		if ((recvMsgSize = recv(clntSocket, echoBuffer, 128, 0)) < 0)
		{
			printf("recv() failed");
			break;
		}
		/* Send received string and receive again until end of
		transmission */
		//printf ("echoBuffer: %d\n", echoBuffer[0]);

		if (echoBuffer[0]!=13 && echoBuffer[0]!=8)
			userPass[passVal++]=echoBuffer[0];
		else if (echoBuffer[0]==8)
			userPass[--passVal]='\x0';

		if (strlen(echoBuffer)>0 && echoBuffer[0]==13)
			break;
	}
	userPass[passVal]='\x0';
	if (strcmp (userPass, header.remoteConnectionPassword)==0)
		return 1;
	else
		return 0;
}	

void updateRemoteList()
{
	HWND hlist;

	if (showremoteconnection)
	{
		hlist = GetDlgItem(showremoteconnection, IDC_REMOTECONNECTION_LIST);
		updateRemoteConnectionList(hlist);
	}
}

void processConnection(void *dummy)
{
	char buf[128], *tmp;
	int validPassword=0, theLen;
	struct sockaddr_in addr;
	int nextConn, i;

	SOCKET ss = (SOCKET)dummy;

	if (debugMe==1 && remote==1) {
		nextConn=-1;
		for (i=0; i<TOTAL_CONNECTIONS; i++)
		{
			if (theClients[i].sock==0)
			{
				nextConn=i;
				i=TOTAL_CONNECTIONS;
			}
		}
		if (nextConn==1) {
			strcpy (buf, "!! DEBUG MODE ACTIVATED !!\n\nDebugging Messages are shown for the second client connected!\n\n");
			tmp = createANSIText(buf, ATTAC_RED, ATTAC_BLACK, 0, 0);
			send(ss, tmp, strlen(tmp), 0);
			#ifdef P_FREE
				pfree(&tmp);
			#else
				free(tmp);
			#endif		
		}
	}

	strcpy (buf, "Welcome To ");
	tmp = createANSIText(buf, ATTAC_CYAN, ATTAC_BLACK, 0, 0);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "A");
	tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 0);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "T");
	tmp = createANSIText(buf, ATTAC_RED, ATTAC_BLACK, 1, 0);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "T");
	tmp = createANSIText(buf, ATTAC_GREEN, ATTAC_BLACK, 1, 0);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "A");
	tmp = createANSIText(buf, ATTAC_BLUE, ATTAC_BLACK, 1, 0);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "C");
	tmp = createANSIText(buf, ATTAC_MAGENTA, ATTAC_BLACK, 1, 0);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, " Remote Connection");
	tmp = createANSIText(buf, ATTAC_CYAN, ATTAC_BLACK, 0, 1);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "Please enter the Remote Connection Password");
	tmp = createANSIText(buf, ATTAC_CYAN, ATTAC_BLACK, 0, 1);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "Password is not shown for security");
	tmp = createANSIText(buf, ATTAC_CYAN, ATTAC_BLACK, 0, 1);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	strcpy (buf, "?");
	tmp = createANSIText(buf, ATTAC_CYAN, ATTAC_BLACK, 0, 0);
	send(ss, tmp, strlen(tmp), 0);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		
	validPassword = checkRemoteConnectionPassword(ss);
	if (validPassword==1)
	{
		nextConn=-1;
		for (i=0; i<TOTAL_CONNECTIONS; i++)
		{
			if (theClients[i].sock==0)
			{
				nextConn=i;
				i=TOTAL_CONNECTIONS;
			}
		}
		if (nextConn==-1)
		{
			//too many connections
				attacInfoMessage("Max Connections Have Been Reached! Connection Rejected");
				
				strcpy (buf, " ");
				tmp = createANSIText(buf, ATTAC_BLACK, ATTAC_BLACK, 1, 1);
				send(ss, tmp, strlen(tmp), 0);
				#ifdef P_FREE
					pfree(&tmp);
				#else
					free(tmp);
				#endif		
				strcpy (buf, "Max Connections Have Been Reached!");
				tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 1);
				send(ss, tmp, strlen(tmp), 0);
				#ifdef P_FREE
					pfree(&tmp);
				#else
					free(tmp);
				#endif		
				strcpy (buf, "Connection Closed");
				tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 1);
				send(ss, tmp, strlen(tmp), 0);
				#ifdef P_FREE
					pfree(&tmp);
				#else
					free(tmp);
				#endif		

				shutdown(ss, SD_SEND);
				close (ss);
		}

		theClients[nextConn].sock = ss;

		theLen = sizeof(addr);
		getsockname(ss, (struct sockaddr *) &addr, &theLen);

		theClients[nextConn].num = nextConn+1;
		strcpy (theClients[nextConn].hostname, (char *)inet_ntoa(remote_addr.sin_addr));
		theClients[nextConn].theTime = ATTAC_Time();

		strcpy (buf, " ");
		tmp = createANSIText(buf, ATTAC_BLACK, ATTAC_BLACK, 1, 1);
		send(theClients[nextConn].sock, tmp, strlen(tmp), 0);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		strcpy (buf, "Password Accepted. Welcome!");
		tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 1);
		send(theClients[nextConn].sock, tmp, strlen(tmp), 0);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		strcpy (buf, " ");
		tmp = createANSIText(buf, ATTAC_BLACK, ATTAC_BLACK, 1, 1);
		send(theClients[nextConn].sock, tmp, strlen(tmp), 0);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		strcpy (buf, "Data will appear as soon as the host start moving or doing something");
		tmp = createANSIText(buf, ATTAC_BLUE, ATTAC_BLACK, 1, 1);
		send(theClients[nextConn].sock, tmp, strlen(tmp), 0);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

	}
	else
	{
		strcpy (buf, " ");
		tmp = createANSIText(buf, ATTAC_BLACK, ATTAC_BLACK, 1, 1);
		send(ss, tmp, strlen(tmp), 0);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		
		strcpy (buf, "Invalid Password");
		tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 1);
		send(ss, tmp, strlen(tmp), 0);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		
		strcpy (buf, "Connection Closed");
		tmp = createANSIText(buf, ATTAC_YELLOW, ATTAC_BLACK, 1, 1);
		send(ss, tmp, strlen(tmp), 0);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		shutdown(ss, SD_SEND);
		close (ss);
	}

	updateRemoteList();

	_endthread();
}
