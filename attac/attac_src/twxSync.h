#ifndef __TWX_SYNC_H__
#define __TWX_SYNC_H__

#ifdef _WINDOWS
#include <windows.h>
#endif

#include "./3PP/expat/expat.h"

#define SECTOR_DATA 1
#define PORT_DATA 2
#define TRADER_DATA 3

#define SOCKET_BUFFER 4096
#define WM_SEND_TWXSYNC_DATA WM_USER+10;

extern HANDLE twxSyncWaitEvent;
extern unsigned threadId;
extern int loginAgain;
extern int twxThreadStarted;
extern HANDLE buildTWXDataMutex;
extern int twxSyncPacketsSent;
extern int twxSyncPacketsReceived;

char * sendAllNewTWXSyncData();
void ErrorDataCallback(long hTWXSync, int errorCode, const char *msg, int len);
void IncomingDataCallback(long hTWXSync, const char *data, int len);
void NewConnectionCallback(long hTWXSync, const char *host, int port);
char* parseXMLData(char *data, XML_Parser parser);

void pIncomingDataCallback(long hTWXSync, const char *data, int len);
void pNewConnectionCallback(long hTWXSync, const char *host, int port);

void XMLCALL twxSyncStartElement(void *userData, const XML_Char *name, const XML_Char **atts);
void XMLCALL twxSyncEndElement(void *userData, const XML_Char *name);
void XMLCALL twxSyncCharacterData(void *userData, const XML_Char *s, int len);

char* parseXMLData(char *data, XML_Parser parser);
void CALLBACK twxSyncTimerProc(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );
void twxSyncThreadProc(void *dummy);
unsigned __stdcall twxSyncSendThreadProc(void *dummy);
void test_twc();
char * appendData(char * oldData, char *newData);
char * getFile (char* file);
int postSendMessageToTWXSync(unsigned t_threadId, char* msg);
char* escapeData (char* str);
char* unescapeData (char* str);
char* replace (char *str, char toReplace[], char replaceWith[]);


char* buildSendTWXData(long sectorNumber, int dataType);
void GetTWXGamesThreadProc (void *arg);

long buildSendTWXData_WaitForData(long timeout);
long buildSendTWXData_ReleaseData();

#endif