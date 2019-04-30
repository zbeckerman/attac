#include <commctrl.h>
#include <winbase.h>
#include <winsock2.h>
#include <time.h>
#include <process.h>    /* _beginthread, _endthread */
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

#include "stdafx.h"
#include "JumpgateServer.h"
#include "UnsignedCharStr.h"
#include "Holder.h"
#include "resource.h"
#include "commctrl.h"
#include "ADO.h"
#include "xml.h"
#include "zlibengn.h"
#include "./3PP/expat/expat.h"

#define WM_SHELLNOTIFY WM_USER+5000
#define IDI_TRAY 42

#define ID_MSTART_SERVER 421
#define ID_MSTOP_SERVER 422
#define ID_MEXIT 423
#define ID_MABOUT 424
#define ID_MRESTORE 425
#define MAX_CONNECTIONS 10

int portNumber=7001;
HANDLE hThreadListen;

NOTIFYICONDATA note;// notify icon data structure
HMENU hMaster;// handle for the menu for the popup
char appDir[MAX_LENGTH];


//version of software
int releaseVersion=1;
int majorVersion=0;
int minorVersion=2;

HANDLE recvThreadSafeMutex = CreateMutex(NULL, 0, NULL);
HANDLE freeMutex = CreateMutex(NULL, 0, NULL);
HANDLE threadSafeMutex = CreateMutex(NULL, 0, NULL);
HANDLE syncUsersMutex = CreateMutex(NULL, 0, NULL);
HANDLE zlibEngineMutex = CreateMutex(NULL, 0, NULL);;


LRESULT APIENTRY WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

HWND gDlg;// windows handle
HWND hwndGlobal;// windows handle
HINSTANCE hInst;// instance handle

HWND showAdo=NULL;	//preference handle

FARPROC lpfn;

int APIENTRY WinMain(HINSTANCE hInstx,HINSTANCE hPrev,LPSTR line,int CmdShow)
{

	MSG msg;// msg handle for the message loop
	hInst = hInstx;// pass the instance to the global instance handle

	::CoInitialize(NULL);

	syncUsersMutex = CreateMutex(NULL, 0, NULL);

	InitCommonControls();

WNDCLASS wc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_OLD_KEY));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = (LPCSTR) 103;
	wc.lpszClassName = "Jumpgate Server";
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszMenuName = (LPCSTR)IDR_JUMPGATE_MENU;

	RegisterClass(&wc);

	GetCurrentDirectory (MAX_LENGTH, appDir);
	strcat (appDir, "\\");

	// create the window
	hwndGlobal=CreateWindow("Jumpgate Server","Jumpgate Server",NULL,0,0,0,0,0,0,hInst,0);

	// the programs loop
	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	::CoUninitialize();

	return msg.wParam;
}


	// program callback function
LRESULT APIENTRY WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int result=0;

	switch(msg)
	{
	case WM_CLOSE:
	case WM_QUERYENDSESSION:
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE,&note);// delete from systray (if its still in there)
		DestroyMenu(hMaster);
		PostQuitMessage(0);// post quit messsage
		break;
	case WM_CREATE:
		{
			HWND st=NULL;// handle of static
			HWND hlist =NULL;
			st = CreateWindow("Static",NULL,WS_CHILD | WS_VISIBLE | SS_BITMAP,0,0,0,0,hwnd,0,hInst,0);

			// create are popupmenu (no resource!)

				hMaster = CreatePopupMenu();// create the popupmenu
				AppendMenu(hMaster,MF_STRING ,ID_MSTART_SERVER,"Start Server");// append menu 
				AppendMenu(hMaster,MF_STRING ,ID_MSTOP_SERVER,"Stop Server");// append menu 
				AppendMenu(hMaster,MF_SEPARATOR,0,0);// append menu
				AppendMenu(hMaster,MF_STRING ,ID_MRESTORE,"Restore");// append menu 
				AppendMenu(hMaster,MF_STRING ,ID_MEXIT,"Exit");// append menu 
				AppendMenu(hMaster,MF_SEPARATOR,0,0);// append menu
				AppendMenu(hMaster,MF_STRING,ID_MABOUT,"About");// append menu

				//create dialog:
				gDlg = CreateDialog(hInst,MAKEINTRESOURCE(IDM_MAINFORM),hwndGlobal,MainDialogProc);

				HICON hIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_OLD_KEY));
				SendMessage (gDlg, WM_SETICON, WPARAM (ICON_SMALL), LPARAM (hIcon));

				hlist = GetDlgItem(gDlg, IDC_LCONNECTIONS);
				listview_insert_column (hlist, 0, 110, 0, "Hostname");
				listview_insert_column (hlist, 1, 100, 1, "IP Address");
				listview_insert_column (hlist, 2, 130, 2, "Registration Id");
				listview_insert_column (hlist, 3, 50, 3, "Version");
				listview_insert_column (hlist, 4, 100, 4, "Program");
				listview_insert_column (hlist, 5, 100, 5, "Action");
				listview_insert_column (hlist, 6, 150, 6, "Time");
				

				//tray icon
				ShowWindow (gDlg, SW_SHOWNORMAL);


				SendMessage (gDlg, WM_COMMAND, WPARAM (IDC_START), LPARAM (0));
		}
		break;

	case WM_SETFOCUS:
         SetFocus(gDlg);
         break;
		/*
	case WM_SIZE:
		{
			if(wParam == SIZE_MINIMIZED)
			{
				note.cbSize = sizeof(NOTIFYICONDATA);// size of
				note.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_OLD_KEY));// set the icon
				note.hWnd = hwnd;// the window handle, for the callback
				note.uCallbackMessage = WM_SHELLNOTIFY;// the callback ID
				lstrcpy(note.szTip, "TWX");// the note
				note.uID = IDI_TRAY;// set the ID
				note.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;// flags
				ShowWindow(hwnd,SW_HIDE);// hide the window
				Shell_NotifyIcon(NIM_ADD,&note);// add are icon to the system tray
			}
		}
		break;
		*/
	case WM_SHELLNOTIFY:
		{
		
		// if they right click, or left button double click then
		if((lParam == WM_RBUTTONDOWN))
		{
			
			// load menu
			POINT pt;// point structure
			GetCursorPos(&pt);// get the cursors position
			SetForegroundWindow(hwnd);// set the foreground window
			TrackPopupMenu(hMaster,TPM_RIGHTALIGN,pt.x,pt.y,NULL,hwnd,NULL);// track the popup
			PostMessage(hwnd,WM_NULL,0,0);// post a NULL message				              
		}

		else if((lParam == WM_LBUTTONDBLCLK)) {

				    Shell_NotifyIcon(NIM_DELETE,&note);// delete the icon from the systray
				    ShowWindow(gDlg,SW_SHOW);// show are window
				    ShowWindow(gDlg,SW_RESTORE);// restore are window
		}

		}			
		break;
	case WM_COMMAND:
		{
		switch (wParam) 
			{
			case ID_MSTART_SERVER:
				{
					SendMessage (gDlg, WM_COMMAND, WPARAM (IDC_START), LPARAM (0));
				}
				break;
			case ID_MSTOP_SERVER:
				{
					SendMessage (gDlg, WM_COMMAND, WPARAM (IDC_STOP), LPARAM (0));
				}
				break;
			case ID_MABOUT:
				{
					char buffer[1024];
					sprintf (buffer, "Jumpgate Server Version %d.%d.%d\nwritten by: earth\nCopyright (c) 2006", releaseVersion, majorVersion, minorVersion);
					MessageBox(hwnd, buffer,"Jumpgate Server",MB_ICONINFORMATION);
				}
				break;
			case ID_MEXIT:
				{
					//send the destroy message so memory is properly cleaned up and 
					//the icon is removed from the system tray
					SendMessage(hwnd, WM_DESTROY , 0, 0);
				}
				break;
			case ID_MRESTORE:
				{
				    Shell_NotifyIcon(NIM_DELETE,&note);// delete the icon from the systray
				    ShowWindow(gDlg,SW_SHOW);// show are window
				    ShowWindow(gDlg,SW_RESTORE);// restore are window
				}
				break;
			}
		}
		break;
	
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

int CALLBACK MainDialogProc(HWND hDlgProc, UINT msg, WPARAM wParam, LPARAM lParam)
{
MENUITEMINFO menu; 

switch(msg)
{
case WM_INITDIALOG:
	//(listening)
		EnableWindow(GetDlgItem(hDlgProc,IDC_STOP),false);
		EnableWindow(GetDlgItem(hDlgProc,IDC_START),true);
	//load icons for taskbar


	return true;
case WM_COMMAND:
	switch(LOWORD(wParam))
	{
	case IDCANCEL:
	case IDC_OK:
		DoTrayIcon(hwndGlobal);
		ShowWindow(hDlgProc,SW_HIDE);
		return true;
	case IDM_JUMPGATE_PREFERENCES:
		showAdoForm(hDlgProc);
		break;
		
	case IDD_TEST:
		testFunction(hDlgProc);
		break;
		
		
	case IDM_JUMPGATE_CLOSE:	//exit application
	    if (MessageBox(NULL,"Do you want to shutdown and close the server?", "Close and Exit", MB_YESNO | MB_ICONWARNING) == IDYES) {
			SendMessage(hwndGlobal, WM_DESTROY , 0, 0);
		}
		return true;
	case IDM_JUMPGATE_MINIMIZE:	//exit application
	    DoTrayIcon(hwndGlobal);
		return true;
	case IDM_JUMPGATE_ABOUT:
		{
			char buffer[1024];
			sprintf (buffer, "Jumpgate Server Version %d.%d.%d\nwritten by: earth\nCopyright (c) 2006", releaseVersion, majorVersion, minorVersion);
			MessageBox(hDlgProc, buffer,"Jumpgate Server",MB_ICONINFORMATION);
		}
		return true;
	case IDC_STOP://stop server

		menu.cbSize=sizeof(MENUITEMINFO);
		menu.fMask=MIIM_STATE;
		menu.fState=MFS_ENABLED;

		SetMenuItemInfo(hMaster, ID_MSTART_SERVER, 0, &menu);

		menu.fState=MFS_GRAYED;
		SetMenuItemInfo(hMaster, ID_MSTOP_SERVER, 0, &menu);

		EnableWindow(GetDlgItem(hDlgProc,IDC_STOP),false);
		EnableWindow(GetDlgItem(hDlgProc,IDC_START),true);

		SetDlgItemText(hDlgProc, IDC_JUMPGATE_SERVER_STATUS, "Server Stopped");

		DWORD lpExitCode;
		GetExitCodeThread(hThreadListen, &lpExitCode);

		TerminateThread(hThreadListen, lpExitCode);

		return true;
	case IDC_START://start server
		menu.cbSize=sizeof(MENUITEMINFO);
		menu.fMask=MIIM_STATE;
		menu.fState=MFS_GRAYED;

		SetMenuItemInfo(hMaster, ID_MSTART_SERVER, 0, &menu);

		menu.fState=MFS_ENABLED;
		SetMenuItemInfo(hMaster, ID_MSTOP_SERVER, 0, &menu);

		EnableWindow(GetDlgItem(hDlgProc,IDC_STOP),true);
		EnableWindow(GetDlgItem(hDlgProc,IDC_START),false);

		SetDlgItemText(hDlgProc, IDC_JUMPGATE_SERVER_STATUS, "Server Started");
		

		// Start a new thread to deal with the new connection
		DWORD dwGenericThread;
		hThreadListen = CreateThread(NULL,0,ListenServerThreadProc,NULL,0,&dwGenericThread);
		return true;
	}
		return true;

		
}
	return false;
}

void testFunction (HWND hwnd) {
	char buffer[MAX_LENGTH];
	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}

	CADO *cAdo = new CADO(buffer);
	cAdo->Connect("","");

	if (cAdo->Execute("insert into tblSmut (Smut,Word_replace) values ('test1','test2');")==0) {
		char * msg = cAdo->GetErrorMessage();
		MessageBox (NULL, msg, "Error Message", MB_ICONERROR);
	}
	cAdo->Disconnect();
}
void showAdoForm(HWND hwnd)
{
    if (!showAdo) 
	{
		showAdo = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ADO), hwnd, AdoProc);
		if ( showAdo !=NULL ) 
		{ 
			char buffer[MAX_LENGTH];
			FILE *fp;

			fp = fopen ("c:\\ado.txt","r");
			if (fp != NULL) {
				fgets (buffer, MAX_LENGTH, fp);
				fclose (fp);
			}

			SetDlgItemText(showAdo, IDC_ADO_TEXT, buffer);

			ShowWindow(showAdo,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Macro Edit", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showAdo);
}


int CALLBACK AdoProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buffer[MAX_LENGTH];
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:

		GetDlgItemText(hwnd, IDC_ADO_TEXT, buffer, sizeof(buffer) - 1);

		FILE *fp;
		fp = fopen ("c:\\ado.txt","w");
		if (fp != NULL) {
			fputs (buffer, fp);
			fclose (fp);
		}
		//update the form
		showAdo = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showAdo = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showAdo = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

void DoTrayIcon(HWND hwnd)
{
	note.cbSize = sizeof(NOTIFYICONDATA);// size of
	note.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_OLD_KEY));// set the icon
	note.hWnd = hwnd;// the window handle, for the callback
	note.uCallbackMessage = WM_SHELLNOTIFY;// the callback ID
	lstrcpy(note.szTip, "Jumpgate Server");// the note
	note.uID = IDI_TRAY;// set the ID
	note.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;// flags
	ShowWindow(gDlg,SW_HIDE);// hide the window
	Shell_NotifyIcon(NIM_ADD,&note);// add are icon to the system tray
}


void listview_insert_column(HWND hlistTmp, int column, int col_width, int pos, char *text)
{
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvcol.fmt= LVCFMT_LEFT;
	lvcol.cx = col_width; 
	lvcol.iSubItem = pos; 
	lvcol.pszText = text;
	ListView_InsertColumn(hlistTmp, column, &lvcol);
}

						
void listview_insert(HWND hlistTmp, int row, char *hostname, char* ipaddress, char *registrationId, char *version, char *program, char *verActive, char *regActive, char *action)
{
	char timeBuffer[50];
	time_t myTime;
	FILE *fp=NULL;

    LVITEM item;
	ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = hostname;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, ipaddress);

	ListView_SetItemText(hlistTmp, item.iItem, 2, registrationId);

	ListView_SetItemText(hlistTmp, item.iItem, 3, version);

	ListView_SetItemText(hlistTmp, item.iItem, 4, program);

	ListView_SetItemText(hlistTmp, item.iItem, 5, action);

	time(&myTime);

	strcpy(timeBuffer, ctime(&myTime));
	timeBuffer[strlen(timeBuffer)-1]=0;

	ListView_SetItemText(hlistTmp, item.iItem, 6, timeBuffer);


	fp = fopen ("authenticatedUsers.csv", "a+");
	if (fp != NULL) {
		fprintf (fp, "%s,%s,%s,%s,%s,%s,%s,%s,%s\n", hostname, ipaddress, registrationId, version, program, verActive, regActive, timeBuffer, action);
		fclose (fp);
	}
}


//unsigned __stdcall ListenServerThreadProc(void *arg) {
DWORD WINAPI ListenServerThreadProc(LPVOID iValue) {
	WSADATA wsaData;
    struct sockaddr_in our_addr;
	SOCKET sock =NULL;
	char buffer[MAX_BUF_LEN];

	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) {
		sprintf(buffer, "Winsock startup error: %d",WSAGetLastError());
		errorFunction(buffer);
		return -1;
	}

	// create the socket 
	sock = socket(AF_INET,SOCK_STREAM,0);

	/* Make listening socket's port reusable - must appear before bind */
	int option_value = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option_value,
				 sizeof(option_value)) < 0) {
	sprintf(buffer, "Winsock setsockopt error: %d",WSAGetLastError());
	errorFunction(buffer);
	WSACleanup();
	return -1;
	}

	// set up our address
	our_addr.sin_family = AF_INET;
	our_addr.sin_addr.s_addr = INADDR_ANY;
	our_addr.sin_port = htons(portNumber);
	// bind to it
	if (bind(sock, (sockaddr*)&our_addr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		sprintf(buffer, "Winsock binding error: %d",WSAGetLastError());
		errorFunction(buffer);
		WSACleanup();
		return -1;
	}

	// set it to a passive mode
	if (listen(sock,MAX_CONNECTIONS) == SOCKET_ERROR) {     
		sprintf(buffer, "Winsock listening error: %d",WSAGetLastError());
		errorFunction(buffer);
		WSACleanup();
		return -1;
	}

	while (true) {	//need a stopping clause
		SOCKET c_sock;
		struct sockaddr_in client;
		int client_len = sizeof(sockaddr_in); 
		// Accept the connection check for socket error here????
		c_sock = accept(sock, (sockaddr*)&client, &client_len);

		if (c_sock==INVALID_SOCKET)
			break;
		// New connection
		// Start a new thread to deal with the new connection
		{
			DWORD dwGenericThread;
			CHolder *holder = new CHolder();
			holder->SetSocket(c_sock);
			HANDLE hThreadIncoming = CreateThread(NULL,0,IncomingDataThreadProc,(void*) holder,0,&dwGenericThread);
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}

DWORD WINAPI IncomingDataThreadProc(void *aholder) {
	CHolder *holder =(CHolder*) aholder;
	char incomingData[MAX_LENGTH];
	char *token=NULL, *tmp=NULL;
	int i=0, val=0;
	char buffer[MAX_LENGTH];
	char msgSubject[MAX_LENGTH];
	char *msgBody=NULL;
	char hostname[MAX_LENGTH];
	char ipaddress[MAX_LENGTH];
	char registrationId[MAX_LENGTH];
	char version[MAX_LENGTH];
	char program[MAX_LENGTH];
	char action[MAX_LENGTH];
	char *xmlData=NULL;
	char registrationDeactivated[MAX_LENGTH];
	char versionDeactivated[MAX_LENGTH];
	CTWServer* twServer;
	SOCKET sock = holder->GetSocket();
	HWND hlist =NULL;
		
	int r=-1;

	strcpy (program, "");
	strcpy (action, "");
	
	
	sprintf (buffer, "200 Welcome To The Jumpgate Server");
	Send(sock, buffer, strlen(buffer));
	
	//receive the EHLO
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	

	sprintf (buffer, "200 Hello back!");
	Send(sock, buffer, strlen(buffer));

	//receive the AUTH LOGIN
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	
	sprintf (buffer, "334 VXNlcm5hbWU6");
	Send(sock, buffer, strlen(buffer));

	//receive the userId
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}

	sprintf (buffer, "334 UGFzc3dvcmQ6");
	Send(sock, buffer, strlen(buffer));

	//receive the password
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	
	sprintf (buffer, "200 Authenticated");
	Send(sock, buffer, strlen(buffer));

	//receive the MAIL FROM
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	
	sprintf (buffer, "200 OK");
	Send(sock, buffer, strlen(buffer));

	//receive the SEND TO
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	
	sprintf (buffer, "200 OK");
	Send(sock, buffer, strlen(buffer));

	//receive the INIT
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	
	sprintf (buffer, "200 OK");
	Send(sock, buffer, strlen(buffer));

	//receive the SUBJECT
	r = Receive(sock, msgSubject, sizeof(msgSubject)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	
	//if it came in one packet
	if (Instr (1, msgSubject, "HostName:")>0) {
		msgBody = (char *) strdup (msgSubject);
		goto parseData;
	}

	//receive the MESSAGE BODY
	r = ReceiveAllDataWrapper(sock, &msgBody);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}

	//if it came in one packet
	if (msgBody != NULL && Instr (1, msgBody, ".")>0) {
		goto parseData;
	}

	//receive the SENDNOW
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}	

parseData:	
	sprintf (buffer, "200 OK");
	Send(sock, buffer, strlen(buffer));

	//receive the BYE
	r = Receive(sock, incomingData, sizeof(incomingData)-1);
	if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
		goto error_socket;
	}
	

	//hostname
	token = strtok (msgBody, "\r");
	while (Instr (1, token, "HostName:")==0)
		token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (hostname, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (hostname, token);
		}
	}

	//ipaddress
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (ipaddress, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (ipaddress, token);
		}
	}

	//registrationId
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (registrationId, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (registrationId, token);
		}
	}

	//version
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (version, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (version, token);
		}
	}

	//registration deactivated
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (registrationDeactivated, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (registrationDeactivated, token);
		}
	}

	//version deactivated
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (versionDeactivated, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (versionDeactivated, token);
		}
	}
	//program
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (program, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (program, token);
		}
	}

	//action
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			strcpy (action, tmp);
			pfree((void**)&tmp);
		} else {
			strcpy (action, token);
		}
	}
	//xml
	token = strtok (NULL, "\r");
	if (token != NULL) {
		val = Instr (1, token, ":");
		if (val>0) {
			tmp = right (token, strlen (token)-val-1);
			xmlData = (char *) strdup (tmp);
			pfree((void**)&tmp);
		} else {
			xmlData = (char *) strdup (token);
		}
	}

	twServer = parseXML(xmlData, strlen(xmlData));
	if (xmlData != NULL) {
		FILE *fp=NULL;
		fp = fopen ("c:\\jumpdateXML.xml","w");
		fprintf (fp, "%s", xmlData);
		fclose (fp);

		pfree ((void **) &xmlData);
	}

	try {
		twServer->insertOrUpdateData();
	} catch(const char *str) {
		FILE *fp=NULL;
		fp = fopen ("c:\\jumpdateXML.xml","w");
		fprintf (fp, "ERROR: %s", str);
		fclose (fp);

	}


	delete twServer;

	hlist = GetDlgItem(gDlg, IDC_LCONNECTIONS);
	listview_insert(hlist, 0, hostname, ipaddress, registrationId, version, program, versionDeactivated, registrationDeactivated, action);

	
error_socket:
	closesocket(sock);
	if (msgBody != NULL)
		pfree((void **) &msgBody);

	//clean up object allocated after connection
	return 0;
}

void errorFunction (char str[]) {

}


//i free old data here. caller must free newData
char * appendData(char * oldData, char *newData) {
	char *returnData=NULL;
	if (oldData==NULL) {
		long newDataLen = strlen(newData);
		returnData = (char*) malloc (newDataLen+1);
		strcpy (returnData, newData);
		returnData[newDataLen]=0;
	} else {
		long oldDataLen = strlen(oldData);
		long newDataLen = strlen(newData);

		returnData = (char *) malloc (oldDataLen+newDataLen+1);
		strcpy (returnData, oldData);
		strcat (returnData, newData);
		returnData[oldDataLen+newDataLen]=0;
		#ifdef P_FREE
			pfree(&oldData);
		#else
			free(oldData);
		#endif	
	}

	return (returnData);
}

char* escapeData (char* str) {
	char *val= NULL;
	char strT[2];

	int i=0;
	for (i=0; i<(int)strlen(str); i++) {
		switch (str[i]) {
			case '<':
				val = appendData(val, "&lt;");
				break;
			case '>':
				val = appendData(val, "&gt;");
				break;
			case '&':
				val = appendData(val, "&amp;");
				break;
			case '\'':
				val = appendData(val, "&apos;");
				break;
			case '\"':
				val = appendData(val, "&quot;");
				break;
			default: 
			{
				strT[0]=str[i];
				strT[1]='\x0';
				val = appendData(val, strT);
				break;
			}
		}		
	}
	pfree((void**)&str);
	return(val);
}


char* unescapeData (char* str) {
	int i=0;
	char* newStr = strdup (str);

	i = Instr(1, newStr, "&lt;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&lt;", "<");
			i = Instr(1, newStr, "&lt;");
		} 
	}

	i = Instr(1, newStr, "&gt;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&gt;", ">");
			i = Instr(1, newStr, "&gt;");
		} 
	}

	i = Instr(1, newStr, "&amp;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&amp;", "&");
			i = Instr(1, newStr, "&amp;");
		} 
	}

	i = Instr(1, newStr, "&apos;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&apos;", "\'");
			i = Instr(1, newStr, "&apos;");
		} 
	}

	i = Instr(1, newStr, "&quot;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&quot;", "\"");
			i = Instr(1, newStr, "&quot;");
		} 
	}


	pfree ((void **) &str);
	return(newStr);
}

char* replace (char *str, char toReplace[], char replaceWith[]) {
	char *val=NULL;

	int i = Instr(1, str, toReplace);

	if (i>0) {
		char *leftVal = left (str, i-1); 
		char *rightVal = right (str, strlen(str)-(i+strlen(toReplace)-1)); 
		
		val = appendData ( val, leftVal);
		val = appendData ( val, replaceWith);
		val = appendData ( val, rightVal);

		pfree((void **) &leftVal);
		pfree((void **) &rightVal);
	}
	else {
		val = (char *) strdup (str);
	}
	pfree((void **) &str);
	return val;
}


//caller must free tmp value passed in
char *fixSQLValue (char *tmp) {
	char *replaceSQL = NULL;
	
	if (tmp==NULL) {
		replaceSQL = (char *) strdup ("NULL");
	} else {
		char*theVal =(char*)strdup (tmp);
		
		replaceSQL = replace(theVal, "'", "''");
		replaceSQL = replace(replaceSQL, "\r", "");
		replaceSQL = replace(replaceSQL, "\n", "");
		
		int len = strlen(replaceSQL)+3;
		char *tmpVal = (char *) malloc (len);

		strcpy (tmpVal, "'");
		strcat (tmpVal, replaceSQL);
		strcat (tmpVal, "'");
		tmpVal[len-1]=0;
		pfree ((void **) &replaceSQL);

		replaceSQL = tmpVal;

	}

	return replaceSQL;
}

CTWServer* parseXML(char *xmlData, int len) {
	CTWServer* newServer = new CTWServer();
	XML_Parser parser;
	char s_error_msg[1024];

	WaitForSingleObject (syncUsersMutex, INFINITE);
	parser = XML_ParserCreate(NULL);

	XML_SetUserData(parser, newServer);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, characterData);

	int status = XML_Parse(parser, xmlData, len, len);
	if (status == XML_STATUS_ERROR) {
		sprintf(s_error_msg, "XML parse error at line %d: %s",
		XML_GetCurrentLineNumber(parser),
		XML_ErrorString(XML_GetErrorCode(parser)));

		
		char buf[128];
		FILE *ff;
		srand( time(NULL) );

		sprintf(buf, "%sJumpgateDebug.txt", appDir);
		ff = fopen (buf, "a+");
		if (ff!= NULL) {
			fprintf (ff,"\n\n------------XML_Parse---------------\n");
			fprintf (ff,"XML Parse Error: %s\nError is:%s\n", xmlData, s_error_msg);
			fclose(ff);

		}

	
		ReleaseMutex(syncUsersMutex);
	}
	//clear out the memory
	XML_ParserReset(parser, NULL);
	XML_SetUserData(parser, newServer);
	XML_SetElementHandler(parser, startElement,endElement);
	XML_SetCharacterDataHandler(parser, characterData);


	XML_ParserFree(parser);

	ReleaseMutex(syncUsersMutex);
	return newServer;
}

/**
 * Receive TWX v2 XML data to the other side.
 *
 * @param data The data to send
 * @param len The length of the data
 * @return Nonzero if failed
 */
int ReceiveAllDataWrapper(SOCKET sock, char **data)
{
	int n=0;
	int completed=0;
	char *returnValue=NULL;
	int r=1;
	char buffer[SOCKET_BUFFER];

	WaitForSingleObject(recvThreadSafeMutex, INFINITE);


	CUnsignedCharStr* incomingData=new CUnsignedCharStr();
	while (!completed && !(r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET)) {
		r = Receive(sock, buffer, sizeof(buffer)-1);
		if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
			// Connection closed by other side
			if (r == SOCKET_ERROR || r == WSAECONNRESET)
				n=-1;
			
			ReleaseMutex(recvThreadSafeMutex);
			return n;
		}
		
		buffer[r]=0;

		incomingData->appendString((unsigned char *)buffer, r);
		
		if (incomingData->endsWithString(DATA_COMPLETE)==0) {
			incomingData->trimStringRight(DATA_COMPLETE_LEN);
			completed=1;
			n-=DATA_COMPLETE_LEN;
		}
		n+=r;
	}

	if (n>0) {
		char returnBuffer[MAX_LENGTH];
		char *res = strdup (""); 
		char *isInvalidCrc32 = strdup ("0");
		unsigned char *ptr = incomingData->getString();
	
		WaitForSingleObject(zlibEngineMutex, INFINITE);
		CZlibEngine *engine = new CZlibEngine();
		n=TWC_TWXSync_Decode_With_Engine(engine, ptr, incomingData->getLength(), &res, isInvalidCrc32); 
		delete engine;
		ReleaseMutex(zlibEngineMutex);
		if (strcmp (isInvalidCrc32, "1")==0) {
			strcpy (returnBuffer, res);
		}
	
		pfree((void **) &ptr);

		*data = res;

	} else {
		*data = (char *)strdup ("");
		n=0;
	}

	delete incomingData;

	ReleaseMutex(recvThreadSafeMutex);
	return n;
}

int TWC_TWXSync_Decode_With_Engine(CZlibEngine *internalEngine, unsigned char *in, int len, char **out, char *isInvalidCrc32)
{
	
	CUnsignedCharStr* dataToDecompress = new CUnsignedCharStr();
	dataToDecompress->appendString(in, len);

	//check if there is a complete token
	int result = dataToDecompress->hasString(DATA_COMPLETE);
	//if so remove it
	if (result!=-1) {
		dataToDecompress->trimStringRight(dataToDecompress->getLength()-result);
		result=-1;
	}

	//check if the crc32 token exists
	result = dataToDecompress->hasString(CRC_32);
	//default is that the packet is ok
	int validCrc32=1;
	if (result!=-1) {
		//if i found me token, then get the crc32 value and trim the crc token
		//and value from the string
		unsigned char *crc32 = dataToDecompress->getCrc32FromString(result);

		dataToDecompress->trimStringRight(dataToDecompress->getLength()-result);
		//confirm that the crc32 value is good
		validCrc32 = dataToDecompress->confirmCrc32(crc32);
		//free the memory from getting the crc32 value
		pfree((void **) &crc32);
	}
	
	//if the Crc32 check is valid or no crc32 was sent

	if (validCrc32==1) {
		//do our decompression
		CUnsignedCharStr* decompress = internalEngine->decompressInMemory(dataToDecompress);
		//decrypt string
		decompress->decryptString();
		//return string by reference
		*out = (char *)decompress->getString();
		//return len by value
		len = decompress->getLength();
		//delete memory we used
		delete decompress;
	} else {
		//if it was invalid, just pretend the packet was empty
		char buf[512];
		FILE *ff=NULL;
		srand( (unsigned)time( NULL ) );
		if (isInvalidCrc32 != NULL && strlen (isInvalidCrc32)>0)
			isInvalidCrc32[0]='1';

		//generate empty packet
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><Jumpgate version=\"%d\"> <Session id=\"%s\"/> <Result code=\"200\" text=\"OK\"/></Jumpgate>", "1.x", "");

		*out = strdup (buf);
		//get the length
		len = strlen (*out);

		//printout the debug info
		sprintf(buf, "%sTWXSyncDebug.txt", appDir);
		ff = fopen (buf, "a");
		if (ff!= NULL) {
			fprintf (ff,"\n\n------------TWC_TWXSync_Decode---------------\n");
			fprintf (ff,"Bad Packet crc32 Invalid: Length %d\n", dataToDecompress->getLength());
			fclose(ff);

		}
	}
	//delete data to decompress memory
	delete dataToDecompress;
	return len;
}

/**
 * Receive TWX v2 XML data to the other side.
 * Private function. Not callable from client so no Mutex here
 *
 * @param data The data to send
 * @param len The length of the data
 * @return Nonzero if failed
 */
int Receive(SOCKET sock, const char *data, int len)
{
	int n=-1;

	if (sock != NULL) {
		n = recv( sock, (char *)data, len, 0);
		((char *) data)[n]=0;
	}

	return n;
}

/**
 * Sends TWX v2 XML data to the other side.
 *
 * @param data The data to send
 * @param len The length of the data
 * @return Nonzero if failed
 */
int Send(SOCKET sock, const char *data, int len)
{

	int n=0;

	if (threadSafeMutex==NULL) {
		errorFunction("NULL threadSafeMutex");
		return -1;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	if (sock!= NULL) {
		//compress data
		CUnsignedCharStr* dataToCompress = new CUnsignedCharStr((char *)data);

		dataToCompress->appendString((unsigned char *)DATA_COMPLETE, DATA_COMPLETE_LEN);

		//get compressed string. new memory is allocated
		unsigned char *tmp = dataToCompress->getString();

		//send compressed data. add null char
		n = send( sock, (char *)tmp, dataToCompress->getLength(), 0);

		//free the compressed string since we send then one with data_complete
		pfree ((void **) &tmp);	

		//delete compressed data
		delete dataToCompress;
	}
	
	ReleaseMutex(threadSafeMutex);
	return n;
}

void pfree (void **x)
{
        WaitForSingleObject(freeMutex, INFINITE);
        if (*x!=NULL) {
			free(*x);
		}
        *x = NULL;
        ReleaseMutex(freeMutex);
}


long Instr (long startPos, char stringBeingSearched[] , char stringToFind[] )
{
	int i, j;
	int firstMatch=0, match=0;
	int stringToFindLen;
	int stringBeingSearchedLen;

	stringBeingSearchedLen = strlen(stringBeingSearched);
	stringToFindLen = strlen(stringToFind);

	if (stringBeingSearchedLen==0 || stringToFindLen==0)
		return(0);

	for (i=(startPos-1); i<stringBeingSearchedLen; i++)
	{
		match=0;
		if (stringBeingSearched[i]==stringToFind[0])
		{
			match++;
			firstMatch = i+1;
			for (j=1; j<stringToFindLen && i+j<stringBeingSearchedLen; j++)
				if (stringToFind[j]==stringBeingSearched[i+j])
					match++;
		}
		if (match==stringToFindLen)
			return(firstMatch);
		firstMatch=0;
	}
	return(firstMatch);
}


char* right (char tmp[], int len)
{
	char *newtmp;
	int i, lenTemp=0;

	lenTemp = (int) strlen(tmp);

	if (len >= lenTemp)
		newtmp = (char *) strdup (tmp);
	else
	{
		newtmp = (char*) malloc (len+1);
		for (i=0; i<(int)len && i <(int)strlen (tmp); i++)
			newtmp[(len-1)-i] = tmp[(lenTemp-1)-i];
		newtmp[i]='\x0';
	}
	return (newtmp);
}

char* left (char tmp[], int len)
{
	char *newtmp;
	int i, lenTemp=0;

	lenTemp = (int) strlen(tmp);

	if (len >= lenTemp)
		newtmp = (char *) strdup (tmp);
	else
	{
		newtmp = (char*) malloc (len+1);
		for (i=0; i<len && i <lenTemp; i++)
			newtmp[i] = tmp[i];
		newtmp[i]='\x0';
	}
	return (newtmp);
}

