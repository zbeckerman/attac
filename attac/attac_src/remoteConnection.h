#ifndef __remoteConnection_h__
#define __remoteConnection_h__

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <winsock2.h>

#define TOTAL_CONNECTIONS 10
#define MAXHOSTNAME 128
#define PORTNUM 2004 /* random port number, we need something */

typedef struct {
  int num;
  char hostname[MAXHOSTNAME];
  long theTime;
  SOCKET sock;
} CLIENTS_CONNECTED;

void stopRemoteConnectionServer();
void startRemoteConnectionServer();

#define CLEAR_ADDR(addr) memset(addr,0,sizeof(struct sockaddr_in)) 
void remoteConnectionThreadProc(void *dummy);
void updateRemoteList();
int sockInit(void); 
int sockEnd(void); 
void processConnection(void *dummy);

extern 	SOCKET remoteSocket; 
extern int remoteConnectionEnabled;
extern int nextConnection;
extern CLIENTS_CONNECTED theClients[TOTAL_CONNECTIONS];

#endif
