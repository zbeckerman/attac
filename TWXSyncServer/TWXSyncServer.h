// TWXSyncServer.h: interface for the CTWXSyncIfc class.
//

#if !defined(TWXSYNCSERVER_H)
#define TWXSYNCSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>

#define MAX_LENGTH 1024
#define READ_WRITE_AMOUNT 1
#define SMTP_PORT 25
#define HTTP_PORT 80

typedef struct {
  short port;
  char reserved[1022];
} SERVER_PREFERENCES;


extern HWND gDlg;//global handle to dialog
extern int validRegCode;
extern char appDir[MAX_LENGTH];
extern int usersConnected;
extern SERVER_PREFERENCES serverPreferences;

void authorizeCallback (int isRegistrationValid, char *returnBuffer);
int CALLBACK MainDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DoTrayIcon(HWND hwnd);
void listview_update_item(HWND hlistTmp, int index, char *ipaddress, int port, char* username, long timeConnected, char *helper, char *game);
void listview_insert_column(HWND hlistTmp, int column, int col_width, int pos, char *text);
void listview_insert(HWND hlistTmp, int row, char *ipaddress, char* username, long date, char *helper, char *game);
void listview_insert_c(HWND hlistTmp, int row, char *ipaddress, char* username, char *date, char *helper, char *game);
int listview_find_item(HWND hlistTmp, char *text);
int listview_delete_item(HWND hlistTmp, int index);
void listview_insert_view_games(HWND hlistTmp, int row, char *name, char* sessionId);
char * listview_get_column_text(HWND hlistTmp, int row, int column);
long Get_Time();
void showcreategameform(HWND hwnd);
void showmaintaingameform(HWND hwnd, char *sessionId, char *name, long universeSize, char *password);
void showmaintainallgameform(HWND hwnd);
void showregcodesform(HWND hwnd);
void showpreferencesform(HWND hwnd);
void showexportxmlform(HWND hwnd);
int CALLBACK TWXCreateGameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK TWXMaintainGameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK TWXMaintainAllGameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK TWXRegistrationProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK TWXPreferencesProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK TWXExportXMLProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int dashCount(char data[]);
int isValidRegCode (char* user1, char* reg, int useRegTxtFile);
int loadPreferences();
int savePreferences();
int invalidateRegistration (char *invalidReg);
long Instr (long startPos, char stringBeingSearched[] , char stringToFind[] );
char* right (char tmp[], int len);
char* left (char tmp[], int len);

#endif // !defined(TWXSYNCSERVER_H)
