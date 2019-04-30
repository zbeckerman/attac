// TWXSyncServer.h: interface for the CTWXSyncIfc class.
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include <windows.h>
#include <stdio.h>

#define MAX_LENGTH 1024
#define READ_WRITE_AMOUNT 1
#define SMTP_PORT 25
#define HTTP_PORT 80

#define MAX_BUF_LEN 2048
#define DATA_COMPLETE "COMPLETED"
#define DATA_COMPLETE_LEN strlen(DATA_COMPLETE)
#define SOCKET_BUFFER 4096

class CUnsignedCharStr;
class CHolder;

extern HWND gDlg;//global handle to dialog
extern int validRegCode;
extern char appDir[MAX_LENGTH];
extern int usersConnected;

int CALLBACK MainDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DoTrayIcon(HWND hwnd);
void listview_update_item(HWND hlistTmp, int index, char *ipaddress, int port, char* username, long timeConnected, char *helper);
void listview_insert_column(HWND hlistTmp, int column, int col_width, int pos, char *text);
void listview_insert(HWND hlistTmp, int row, char *ipaddress, char* username, long date, char *helper);
void listview_insert_c(HWND hlistTmp, int row, char *ipaddress, char* username, char *date, char *helper);
int listview_find_item(HWND hlistTmp, char *text);
int listview_delete_item(HWND hlistTmp, int index);
void listview_insert_view_games(HWND hlistTmp, int row, char *name, char* sessionId);
char * listview_get_column_text(HWND hlistTmp, int row, int column);
long Get_Time();
void to64frombits(unsigned char *out, const unsigned char *in, int inlen);
int from64tobits(char *out, const char *in);
long Instr (long startPos, char stringBeingSearched[] , char stringToFind[] );
char* right (char tmp[], int len);
char* left (char tmp[], int len);
void errorFunction (char str[]);
int ReceiveAllDataWrapper(SOCKET sock, char **data);
int Receive(SOCKET sock, const char *data, int len);
int Send(SOCKET sock, const char *data, int len);
void listview_insert(HWND hlistTmp, int row, char *hostname, char* ipaddress, char *registrationId, char *version, char *program, char *verActive, char *regActive);
DWORD WINAPI ListenServerThreadProc(LPVOID iValue);
DWORD WINAPI IncomingDataThreadProc(LPVOID iValue);
void pfree (void **x);
long Instr (long startPos, char stringBeingSearched[] , char stringToFind[] );
char* right (char tmp[], int len);
char* left (char tmp[], int len);
