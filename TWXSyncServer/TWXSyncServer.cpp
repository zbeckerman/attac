#include <commctrl.h>
#include <winbase.h>

#include "stdafx.h"
#include "TWXSyncServer.h"
#include "TWXSyncIfc.h"
#include "resource.h"
#include "commctrl.h"
#include "encrypt.h"

#include "..\TWCommon\TWCommon.h"
#include "..\TWCommon\Authorize.h"

#define WM_SHELLNOTIFY WM_USER+5000
#define IDI_TRAY 42

#define ID_MSTART_SERVER 421
#define ID_MSTOP_SERVER 422
#define ID_MEXIT 423
#define ID_MABOUT 424
#define ID_MRESTORE 425


#define FOUND_SMTP_NOTHING 0
#define FOUND_SMTP_200 1
#define FOUND_SMTP_211 2
#define FOUND_SMTP_214 3
#define FOUND_SMTP_220 4
#define FOUND_SMTP_235 5
#define FOUND_SMTP_250 6
#define FOUND_SMTP_334 7
#define FOUND_SMTP_354 8

#define DATE "Today\r\n"
#define INIT "data\r\n"
#define SENDNOW "\r\n.\r\n"
#define BYE "quit\r\n"	

static const char base64digits[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define BAD     -1
static const char base64val[] = {
     BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
     BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
     BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD, 62, BAD,BAD,BAD, 63,
      52, 53, 54, 55,  56, 57, 58, 59,  60, 61,BAD,BAD, BAD,BAD,BAD,BAD,
     BAD,  0,  1,  2,   3,  4,  5,  6,   7,  8,  9, 10,  11, 12, 13, 14,
      15, 16, 17, 18,  19, 20, 21, 22,  23, 24, 25,BAD, BAD,BAD,BAD,BAD,
     BAD, 26, 27, 28,  29, 30, 31, 32,  33, 34, 35, 36,  37, 38, 39, 40,
      41, 42, 43, 44,  45, 46, 47, 48,  49, 50, 51,BAD, BAD,BAD,BAD,BAD
};
#define DECODE64(c)  (isascii(c) ? base64val[c] : BAD)


CTWXSyncIfc m_twxSyncServer;
int defaultPort=7777;
HWND gDlg;//global handle to dialog
int validRegCode=0;
int usersConnected=0;
char appDir[MAX_LENGTH];
SERVER_PREFERENCES serverPreferences;

NOTIFYICONDATA note;// notify icon data structure
HMENU hMaster;// handle for the menu for the popup
unsigned threadId=0;
unsigned long authThreadId=0;
char g_userId[100]="No Registration Id";
int g_isRegDeactivated=0;

//version of software
int releaseVersion=1;
int majorVersion=0;
int minorVersion=3;
char betaVersion[5]="";

LRESULT APIENTRY WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void  AuthorizeRegistrationThreadProc(void *d);

HWND hwndGlobal;// windows handle
HWND hdlg;//dialog handle
HINSTANCE hInst;// instance handle

HWND showmaintaingame;
HWND showcreategame;
HWND showmaintainallgame;
HWND showexportxml;
HWND showregcodes;
HWND showabout;
HWND showpreferences;

FARPROC lpfn;

int APIENTRY WinMain(HINSTANCE hInstx,HINSTANCE hPrev,LPSTR line,int CmdShow)
{

	MSG msg;// msg handle for the message loop
	hInst = hInstx;// pass the instance to the global instance handle

	InitCommonControls();

WNDCLASS wc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_TWXSYNC_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = (LPCSTR) 103;
	wc.lpszClassName = "TWXSync Server";
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszMenuName = (LPCSTR)IDR_TWXSYNC_MENU;

	RegisterClass(&wc);

	GetCurrentDirectory (MAX_LENGTH, appDir);
	strcat (appDir, "\\");

	loadPreferences();

	// create the window
	hwndGlobal=CreateWindow("TWXSync Server","TWXSync Server",NULL,0,0,0,0,0,0,hInst,0);

	isValidRegCode("", "", 1);

	{
		char *prog = strdup ("twgs");
		char *user = strdup (g_userId);
		char *beta = strdup (betaVersion);
//		authorizeProgramExternal(&authorizeCallback, prog, user, releaseVersion, majorVersion, minorVersion, beta);
//		jumpgateProgramExternal(&authorizeCallback, prog, user, releaseVersion, majorVersion, minorVersion, beta, "Testing", "<open>test</open>");
		pfree((void**)&prog);
		pfree((void**)&user);
		pfree((void**)&beta);
	}
	// the programs loop
	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void authorizeCallback (int isRegistrationValid, char *returnBuffer) {
	g_isRegDeactivated = isRegistrationValid;
	if (isRegistrationValid!=VALID_REGISTRATION) {
		validRegCode=0;
		SendMessage (gDlg, WM_COMMAND, WPARAM (IDC_STOP), LPARAM (0));
		SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, returnBuffer);
	}
	TWC_TWXSync_FreeMem((void **) &returnBuffer);
}


// program callback function
LRESULT APIENTRY WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int result=0;
	char buffer[128];

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

				HICON hIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_TWXSYNC_ICON));
				SendMessage (gDlg, WM_SETICON, WPARAM (ICON_SMALL), LPARAM (hIcon));


				hlist = GetDlgItem(gDlg, IDC_LCONNECTIONS);
				listview_insert_column (hlist, 0, 110, 0, "IP Address");
				listview_insert_column (hlist, 1, 90, 1, "Username");
				listview_insert_column (hlist, 2, 130, 2, "Date Connected");
				listview_insert_column (hlist, 3, 65, 3, "Helper");
				listview_insert_column (hlist, 4, 100, 4, "Game");

				hlist = GetDlgItem(gDlg, IDC_LOLD);
				listview_insert_column (hlist, 0, 110, 0, "IP Address");
				listview_insert_column (hlist, 1, 90, 1, "Username");
				listview_insert_column (hlist, 2, 130, 2, "Date Connected");
				listview_insert_column (hlist, 3, 65, 3, "Helper");
				listview_insert_column (hlist, 4, 100, 4, "Game");

				result = m_twxSyncServer.Init(1);

				if (result!=0) {
					sprintf (buffer, "Error Initializing Server: %d", result);
					MessageBox(hwnd,buffer,"TWX Sync",MB_ICONINFORMATION);
				}
				//tray icon
				ShowWindow (gDlg, SW_SHOWNORMAL);


				SendMessage (gDlg, WM_COMMAND, WPARAM (IDC_START), LPARAM (0));
		}
		break;

	case WM_SETFOCUS:
         SetFocus(hdlg);
         break;
		/*
	case WM_SIZE:
		{
			if(wParam == SIZE_MINIMIZED)
			{
				note.cbSize = sizeof(NOTIFYICONDATA);// size of
				note.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_TWXSYNC_ICON));// set the icon
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
					char *twCommonVersion = TWC_Version();
					sprintf (buffer, "TWX Sync Version %d.%d.%d%s\nUsing TWCommon.dll Version %s\nUsing Zlib v1.2.3\nUsing Expat 1.95.8\nwritten by: earth\nCopyright (c) 2005", releaseVersion, majorVersion, minorVersion, betaVersion, twCommonVersion);
					TWC_TWXSync_FreeMem((void**)&twCommonVersion);
					MessageBox(hwnd, buffer,"TWX Sync",MB_ICONINFORMATION);
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
	case WM_NOTIFY:
		if ((int)wParam == IDC_LCONNECTIONS) 
			{
				HWND hShowConnectionData = GetDlgItem(hDlgProc, IDC_LCONNECTIONS);
                LV_DISPINFO *lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
					case NM_CLICK:
						char sAddress[64]={0};
						char sIP[64]={0};
						int iIndex = -1;

						iIndex = ListView_GetNextItem(hShowConnectionData, iIndex, LVNI_SELECTED);
						
						if (iIndex >=0) {
							ListView_GetItemText(hShowConnectionData, iIndex, 0, sAddress, sizeof(sAddress));
							
							//determine other info by browsing linked list
							
							//address/port
							int i;
							for (i=0;i<(int)strlen(sAddress);i++)
							{
								if (sAddress[i]==':') 
								{
									sIP[i]=0;
									break;
								}else{
									sIP[i]=sAddress[i];
								}
							}
							SendMessage(GetDlgItem(hDlgProc,IDC_EIP),WM_SETTEXT,0,(LPARAM)sIP);
							//host name

							unsigned long addr = inet_addr(sIP);
							hostent * H = gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
							
							char sHName[255] = "Unknown Host Name.";//temporary formatting buffer

							if (H) 
							{
								memcpy(sHName,H->h_name,strlen(H->h_name)+1);
							}
							//

							SendMessage(GetDlgItem(hDlgProc,IDC_HN),WM_SETTEXT,0,(LPARAM)sHName);

							//time connected
							char sTimeMsg[64];
							ListView_GetItemText(hShowConnectionData, iIndex, 2, sTimeMsg, sizeof(sTimeMsg));
							SendMessage(GetDlgItem(hDlgProc,IDC_ETIME),WM_SETTEXT,0,(LPARAM)sTimeMsg);

							break;
						}                    
                }
		}
		if ((int)wParam == IDC_LOLD) 
			{
				HWND hShowConnectionData = GetDlgItem(hDlgProc, IDC_LOLD);
                LV_DISPINFO *lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
					case NM_CLICK:
						char sAddress[64]={0};
						char sIP[64]={0};
						int iIndex = -1;

						iIndex = ListView_GetNextItem(hShowConnectionData, iIndex, LVNI_SELECTED);
						if (iIndex >=0) {
							ListView_GetItemText(hShowConnectionData, iIndex, 0, sAddress, sizeof(sAddress));
							
							//determine other info by browsing linked list
							
							//address/port
							int i;
							for (i=0;i<(int)strlen(sAddress);i++)
							{
								if (sAddress[i]==':') 
								{
									sIP[i]=0;
									break;
								}else{
									sIP[i]=sAddress[i];
								}
							}
							SendMessage(GetDlgItem(hDlgProc,IDC_EIP),WM_SETTEXT,0,(LPARAM)sIP);
							//host name

							unsigned long addr = inet_addr(sIP);
							hostent * H = gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
							
							char sHName[255] = "Unknown Host Name.";//temporary formatting buffer

							if (H) 
							{
								memcpy(sHName,H->h_name,strlen(H->h_name)+1);
							}
							//

							SendMessage(GetDlgItem(hDlgProc,IDC_HN),WM_SETTEXT,0,(LPARAM)sHName);

							//time connected
							SendMessage(GetDlgItem(hDlgProc,IDC_ETIME),WM_SETTEXT,0,(LPARAM)"Disconnected");

							break;
                    
						}
                }
            }
		
		return true;

case WM_COMMAND:
	switch(LOWORD(wParam))
	{
	case IDCANCEL:
	case IDC_OK:
		DoTrayIcon(hwndGlobal);
		ShowWindow(hDlgProc,SW_HIDE);
		return true;
		
	case IDM_TWXSYNC_CLOSE:	//exit application
	    if (MessageBox(NULL,"Do you want to shutdown and close the server?", "Close and Exit", MB_YESNO | MB_ICONWARNING) == IDYES) {
			SendMessage(hwndGlobal, WM_DESTROY , 0, 0);
		}
		return true;
	case IDM_TWXSYNC_MINIMIZE:	//exit application
	    DoTrayIcon(hwndGlobal);
		return true;
	case IDC_TERM://terminate connection
		{
			int iIndex =-1;
			iIndex = ListView_GetNextItem(GetDlgItem(hDlgProc,IDC_LCONNECTIONS), iIndex, LVNI_SELECTED);
			if (iIndex != -1 && MessageBox(NULL,"Do you want to terminate this connection?", "Terminate Connection", MB_YESNO | MB_ICONWARNING) == IDYES) {
				char *tmp = listview_get_column_text( GetDlgItem(hDlgProc,IDC_LCONNECTIONS), iIndex, 0);
				
				m_twxSyncServer.TerminateConnection(tmp);

				pfree((void **) &tmp);
			}
			if (iIndex==-1) {
				MessageBox(hDlgProc,"Please select a connection to terminate","TWX Sync",MB_ICONWARNING);
			}
			return true;
		}
	case IDC_STOP://stop server

		menu.cbSize=sizeof(MENUITEMINFO);
		menu.fMask=MIIM_STATE;
		menu.fState=MFS_ENABLED;

		SetMenuItemInfo(hMaster, ID_MSTART_SERVER, 0, &menu);

		menu.fState=MFS_GRAYED;
		SetMenuItemInfo(hMaster, ID_MSTOP_SERVER, 0, &menu);

		EnableWindow(GetDlgItem(hDlgProc,IDC_STOP),false);
		EnableWindow(GetDlgItem(hDlgProc,IDC_START),true);

		///dont want this message if version deactivated
		if (g_isRegDeactivated == 0) {
			SetDlgItemText(hDlgProc, IDC_TWXSYNC_SERVER_STATUS, "Server Stopped");
		}	

		m_twxSyncServer.StopListen(threadId);

		return true;
	case IDC_START://start server
		if (g_isRegDeactivated != 2 && g_isRegDeactivated != 3) {
			menu.cbSize=sizeof(MENUITEMINFO);
			menu.fMask=MIIM_STATE;
			menu.fState=MFS_GRAYED;

			SetMenuItemInfo(hMaster, ID_MSTART_SERVER, 0, &menu);

			menu.fState=MFS_ENABLED;
			SetMenuItemInfo(hMaster, ID_MSTOP_SERVER, 0, &menu);

			EnableWindow(GetDlgItem(hDlgProc,IDC_STOP),true);
			EnableWindow(GetDlgItem(hDlgProc,IDC_START),false);

			SetDlgItemText(hDlgProc, IDC_TWXSYNC_SERVER_STATUS, "Server Started");
			

			threadId = m_twxSyncServer.Listen(serverPreferences.port);
		} else {
			SetDlgItemText(hDlgProc, IDC_TWXSYNC_SERVER_STATUS, "Please upgrade. Server will not start");
		}
		return true;
	case IDM_TWXSYNC_MAINTAIN:
		{
			showmaintainallgameform(hDlgProc);
		}
		return true;
	case IDM_TWXSYNC_EXPORTXML:
		{
			showexportxmlform(hDlgProc);
		}
		return true;
	case IDM_TWXSYNC_REGCODES:
		{
			showregcodesform(hDlgProc);
		}
		return true;
	case IDM_TWXSYNC_PREFERENCES:
		{
			showpreferencesform(hDlgProc);
		}
		return true;
	case IDM_TWXSYNC_ABOUT:
		{
			char buffer[1024];
			char *twCommonVersion = TWC_Version();
			sprintf (buffer, "TWX Sync Version %d.%d.%d%s\nUsing TWCommon.dll Version %s\nUsing Zlib v1.2.3\nUsing Expat 1.95.8\nwritten by: earth\nCopyright (c) 2005", releaseVersion, majorVersion, minorVersion, betaVersion, twCommonVersion);
			TWC_TWXSync_FreeMem((void**)&twCommonVersion);
			MessageBox(hDlgProc, buffer,"TWX Sync",MB_ICONINFORMATION);
		}
		return true;
	case ID_HELP_DEBUGPACKETFROMFILE:
	{
		OPENFILENAME of;
		char filename[100];

		memset(&of, 0, sizeof(of));
#ifdef OPENFILENAME_SIZE_VERSION_400
		of.lStructSize = OPENFILENAME_SIZE_VERSION_400;
#else
		of.lStructSize = sizeof(of);
#endif
		of.hwndOwner = gDlg;
		of.lpstrFilter = "XML Files\0*.xml;\0All Files\0*\0\0\0";
		of.lpstrCustomFilter = NULL;
		of.nFilterIndex = 1;
		strcpy (filename, "");
		of.lpstrFile = filename;
		of.nMaxFile = sizeof(filename);
		of.lpstrFileTitle = NULL;
		of.lpstrInitialDir = NULL;
		of.lpstrTitle = "Select XML File";
		of.Flags = 0;
		if (GetOpenFileName(&of)) {
			char *fileData;
			int existsFiles;
			FILE *fp;
			struct stat buf;

			fp = fopen (filename, "r+b");
			if (fp==NULL)
			{
				printf ("Error opening file %s\n", filename);
				return true;
			}

			existsFiles = stat(filename, &buf);
			fileData = (char *) malloc (buf.st_size+1);
			
			fseek(fp, 0, SEEK_SET);
			fread (fileData, buf.st_size, 1, fp);	
			fileData[buf.st_size]=0;
			fclose (fp);

			CTWXSyncIfc tmp_twxSyncServer;
			tmp_twxSyncServer.Init(0);
	
			tmp_twxSyncServer.IncomingData(fileData, buf.st_size);
			pfree((void **)&fileData);
			
		}
	}
		return true;

		
}
}
	return false;
}


void DoTrayIcon(HWND hwnd)
{
	note.cbSize = sizeof(NOTIFYICONDATA);// size of
	note.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_TWXSYNC_ICON));// set the icon
	note.hWnd = hwnd;// the window handle, for the callback
	note.uCallbackMessage = WM_SHELLNOTIFY;// the callback ID
	lstrcpy(note.szTip, "TWX Sync");// the note
	note.uID = IDI_TRAY;// set the ID
	note.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;// flags
	ShowWindow(gDlg,SW_HIDE);// hide the window
	Shell_NotifyIcon(NIM_ADD,&note);// add are icon to the system tray
}


int listview_delete_item(HWND hlistTmp, int index)
{
	return ListView_DeleteItem (hlistTmp, index);
}


int listview_find_item(HWND hlistTmp, char *text)
{
	LVFINDINFO ItemFindInfo;
	int        itemIndex=0;

	ItemFindInfo.flags = LVFI_STRING;
	ItemFindInfo.psz = text;
	itemIndex = ListView_FindItem (hlistTmp, -1, &ItemFindInfo);

	return itemIndex;
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

//caller frees memory
char * listview_get_column_text(HWND hlistTmp, int row, int column)
{
	char data[128];
	char *returnVal =NULL;
	ListView_GetItemText (hlistTmp, row, column, data, sizeof (data));

	returnVal = (char *) strdup (data);
	return (returnVal);
}


void listview_insert(HWND hlistTmp, int row, char *ipaddress, char* username, long date, char *helper, char *game)
{
	char buffer[50];
	time_t myTime;

	myTime = date;
	strcpy(buffer, ctime(&myTime));
	buffer[strlen(buffer)-1]=0;

	listview_insert_c(hlistTmp, row, ipaddress, username, buffer, helper, game);
}
void listview_insert_c(HWND hlistTmp, int row, char *ipaddress, char* username, char *date, char *helper, char *game)
{
    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = ipaddress;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, username);

	ListView_SetItemText(hlistTmp, item.iItem, 2, date);

	ListView_SetItemText(hlistTmp, item.iItem, 3, helper);

	ListView_SetItemText(hlistTmp, item.iItem, 4, game);

}

void listview_insert_view_games(HWND hlistTmp, int row, char *name, char* sessionId)
{
    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = name;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, sessionId);


}

void listview_update_item(HWND hlistTmp, int index, char *ipaddress, int port, char* username, long date, char *helper, char *game)
{
	char buffer[50];
	time_t myTime;

	/*
	//delete old item
	ListView_DeleteItem(hlistTmp, index);

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = index;
	item.lParam  = index;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = ipaddress;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);
*/
    ListView_SetItemText(hlistTmp, index, 1, username);

	myTime = date;
	strcpy(buffer, ctime(&myTime));
	buffer[strlen(buffer)-1]=0;

	ListView_SetItemText(hlistTmp, index, 3, helper);

	ListView_SetItemText(hlistTmp, index, 4, game);

}



long Get_Time()
{
	time_t data;

	time(&data);

	return data;
}

void showmaintaingameform(HWND hwnd, char *sessionId, char *name, long universeSize, char *password)
{
	char buffer[50];

    if (!showmaintaingame) 
	{
		showmaintaingame = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TWXSYNC_MAINTAIN_GAME), hwnd, TWXMaintainGameProc);
		if ( showmaintaingame !=NULL ) 
		{ 
			SetDlgItemText(showmaintaingame, IDC_TWXSYNC_MAINTAIN_SESSIONID, sessionId);
			SetDlgItemText(showmaintaingame, IDC_TWXSYNC_MAINTAIN_GAMENAME, name);
			sprintf (buffer, "%ld", universeSize);
			SetDlgItemText(showmaintaingame, IDC_TWXSYNC_MAINTAIN_UNIVERSE, buffer);
			SetDlgItemText(showmaintaingame, IDC_TWXSYNC_MAINTAIN_PASSWORD, password);

			ShowWindow(showmaintaingame,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window TWX Maintain Game Sync", "TWXSync Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showmaintaingame);
}

void showcreategameform(HWND hwnd)
{
    if (!showcreategame) 
	{
		showcreategame = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TWXSYNC_CREATE_GAME), hwnd, TWXCreateGameProc);
		if ( showcreategame !=NULL ) 
		{ 
			SetDlgItemText(showcreategame, IDC_TWXSYNC_CREATE_GAMENAME, "");
			SetDlgItemText(showcreategame, IDC_TWXSYNC_CREATE_UNIVERSE, "5000");
			SetDlgItemText(showcreategame, IDC_TWXSYNC_CREATE_PASSWORD, "");

			ShowWindow(showcreategame,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window TWX Create Game", "TWXSync Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showcreategame);
}


void showexportxmlform(HWND hwnd)
{
	HWND hlist=NULL;
	int gameTotal=0, i=0;

    if (!showexportxml) 
	{
		showexportxml = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TWXSYNC_EXPORT_XML), hwnd, TWXExportXMLProc);
		if ( showexportxml !=NULL ) 
		{ 
			hlist = GetDlgItem(showexportxml, IDC_TWXSYNC_VIEW_GAMES);
			listview_insert_column (hlist, 0, 110, 0, "Game Name");
			listview_insert_column (hlist, 1, 185, 1, "SessionId");
			
			gameTotal =  TWC_GetGameCount();
			
			for (i=0; i<gameTotal; i++) {
				char *tGameName = TWC_GetGameName(i);
				char *tSessionId = TWC_GetGameSessionId(i);
				
				listview_insert_view_games(hlist, i, tGameName, tSessionId);

				TWC_TWXSync_FreeMem((void**)&tGameName);
				TWC_TWXSync_FreeMem((void**)&tSessionId);
			}

			ShowWindow(showexportxml,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window TWX Sync Export XML", "TWXSync Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showexportxml);
}

void showmaintainallgameform(HWND hwnd)
{
	HWND hlist=NULL;
	int gameTotal=0, i=0;

	if (IsWindowEnabled(GetDlgItem(gDlg,IDC_STOP))) {
		MessageBox(NULL, "Please stop the server before making any game changes", "TWXSync Error", MB_ICONWARNING); 
		return;
	}

    if (!showmaintainallgame) 
	{
		showmaintainallgame = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TWXSYNC_VIEW), hwnd, TWXMaintainAllGameProc);
		if ( showmaintainallgame !=NULL ) 
		{ 
			hlist = GetDlgItem(showmaintainallgame, IDC_TWXSYNC_VIEW_GAMES);
			listview_insert_column (hlist, 0, 110, 0, "Game Name");
			listview_insert_column (hlist, 1, 185, 1, "SessionId");
			
			gameTotal =  TWC_GetGameCount();
			
			for (i=0; i<gameTotal; i++) {
				char *tGameName = TWC_GetGameName(i);
				char *tSessionId = TWC_GetGameSessionId(i);
				
				listview_insert_view_games(hlist, i, tGameName, tSessionId);

				TWC_TWXSync_FreeMem((void**)&tGameName);
				TWC_TWXSync_FreeMem((void**)&tSessionId);
			}

			ShowWindow(showmaintainallgame,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window TWX Maintain All Games", "TWXSync Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showmaintainallgame);
}

void showregcodesform(HWND hwnd)
{
 	char filename[MAX_LENGTH];
	char *token=NULL, tempString[2056];
	char *tmp=NULL;
	char *userID1 = NULL;
	char *regCode = NULL;
	FILE *fp=NULL;
	int i=0;
	int returnValue=0, skipToEnd=1;
	char fullFilePath[MAX_LENGTH];

	if (!showregcodes) 
	{
		showregcodes = CreateDialog(hInst, MAKEINTRESOURCE(IDD_REGISTRATION), hwnd, TWXRegistrationProc);
		if ( showregcodes !=NULL ) 
		{ 
			strcpy (fullFilePath, appDir);
			strcat (fullFilePath, "reg.txt");

			strcpy(filename, fullFilePath);

			fp = fopen(filename, "r");
			if (fp!=NULL)
			{
				for (i=0; i < (int) strlen (tempString); i++)
					tempString[i]=0;
				i=0;
				//count bytes by reading thru once
				while (!feof(fp))
				{
					fscanf(fp, "%c", &tempString[i]);
					i++;
				}
				tempString[i-1]=0;

				fclose (fp);

				i=0;
				token = strtok(tempString, "*");
				while(token != NULL)
				{
					if (i==0) {
						if (strcmp (token, "-1")!=0)
							userID1 = (char*) strdup(token);
						else
							userID1 = (char*) strdup("");
					}
					else if (i==1) {
						if (strcmp (token, "-1")!=0)
							regCode = (char*) strdup(token);
						else
							regCode = (char*) strdup("");
					}
					i++;

					token = strtok(NULL,"*");
				}
				if (userID1 != NULL) {
					SetDlgItemText(showregcodes, IDC_TWXSYNC_REG_NAME, userID1);
					free(userID1);
				}
				if (regCode != NULL) {
					SetDlgItemText(showregcodes, IDC_TWXSYNC_REG_CODE, regCode);
					free(regCode);
				}
			} 

			ShowWindow(showregcodes,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window TWX Sync Registration", "TWXSync Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showregcodes);
}

void showpreferencesform(HWND hwnd)
{
	char buffer[256];

	if (IsWindowEnabled(GetDlgItem(gDlg,IDC_STOP))) {
		MessageBox(NULL, "Please stop the server before making any preference changes", "TWXSync Error", MB_ICONWARNING); 
		return;
	}

    if (!showpreferences) 
	{
		showpreferences = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PREFERENCES), hwnd, TWXPreferencesProc);
		if ( showpreferences !=NULL ) 
		{ 

			sprintf (buffer, "%d", serverPreferences.port);
			SetDlgItemText(showpreferences, IDC_PREFERENCES_PORT, buffer);
			
			ShowWindow(showpreferences,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window TWX Sync Preferences", "TWXSync Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showpreferences);
}


int CALLBACK TWXCreateGameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char *tSessionId;
	char name[128];
	char universeSize[40];
	char password[40], buffer[40];
	int i=0, gameTotal=0;
	HWND hlist=NULL;

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
	  case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		  GetDlgItemText(hwnd, IDC_TWXSYNC_CREATE_GAMENAME, name, sizeof(name) - 1);
		  GetDlgItemText(hwnd, IDC_TWXSYNC_CREATE_UNIVERSE, universeSize, sizeof(universeSize) - 1);
		  GetDlgItemText(hwnd, IDC_TWXSYNC_CREATE_PASSWORD, password, sizeof(password) - 1);

		  tSessionId = TWC_CreateGame (-1, name, atol(universeSize), password);

		  if (dashCount(tSessionId)>1) {
			showcreategame = NULL;
			
			//update listview of maintenance screens
			if (showmaintainallgame != NULL) {
				hlist = GetDlgItem(showmaintainallgame, IDC_TWXSYNC_VIEW_GAMES);
				ListView_DeleteAllItems (hlist);

				gameTotal =  TWC_GetGameCount();
				
				for (i=0; i<gameTotal; i++) {
					char *tGameName = TWC_GetGameName(i);
					char *tSessionId = TWC_GetGameSessionId(i);
					
					listview_insert_view_games(hlist, i, tGameName, tSessionId);

					TWC_TWXSync_FreeMem((void**)&tGameName);
					TWC_TWXSync_FreeMem((void**)&tSessionId);
				}
			}
			//end update

			SetActiveWindow(GetParent(hwnd));
			DestroyWindow(hwnd);
		  } else {
			  sprintf (buffer, "Code %s\nError Creating Game", tSessionId);
			  MessageBox(hwnd,buffer,"TWX Sync",MB_ICONINFORMATION);
		  }
		return 0;
	  case IDCANCEL:
		showcreategame = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showcreategame = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


int CALLBACK TWXMaintainGameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char sessionId[128];
	char password[40];
	int i=0;
	HWND hlist=NULL;
	

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
	  case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		GetDlgItemText(hwnd, IDC_TWXSYNC_MAINTAIN_SESSIONID, sessionId, sizeof(sessionId) - 1);
		GetDlgItemText(hwnd, IDC_TWXSYNC_MAINTAIN_PASSWORD, password, sizeof(password) - 1);

		TWC_MaintainPassword(sessionId, password);

		showmaintaingame = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);

		return 0;
	  case IDCANCEL:
		showmaintaingame = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showmaintaingame = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

int CALLBACK TWXRegistrationProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i=0;
	HWND hlist=NULL;
	char name[128];
	char password[128];
	char fullFilePath[MAX_LENGTH];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
	  case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_TWXSYNC_REG_OK:
		GetDlgItemText(hwnd, IDC_TWXSYNC_REG_NAME, name, sizeof(name) - 1);
		GetDlgItemText(hwnd, IDC_TWXSYNC_REG_CODE, password, sizeof(password) - 1);

		if (isValidRegCode(name, password, 0)==1)
		{
			FILE *fp;
			validRegCode = 1;

			strcpy (fullFilePath, appDir);
			strcat (fullFilePath, "reg.txt");
			//create reg.txt file
			fp = fopen (fullFilePath, "w");
			if (fp ==NULL)
				printf ("Error opening %s to create registration\n", fullFilePath);
			else
			{
				fprintf (fp, "%s", name);
				fprintf (fp, "*");
				fprintf (fp, "%s", password);
				fprintf (fp, "*");
				fclose(fp);
			}
		}
		else
		{
			validRegCode = 0;
		}
		
		showregcodes = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	  return 0;
	  case IDC_TWXSYNC_REG_CANCEL:
		if (validRegCode == 0)
		{
			MessageBox(hwnd, "This is an unregistered version of TWXSync. Email tradewars@hotmail.com for registration information.", "TWXSync", MB_OK);
		}
		else
			MessageBox(hwnd, "No changes made to registration information", "TWXSync", MB_OK);

		showregcodes = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showregcodes = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


int CALLBACK TWXPreferencesProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i=0;
	HWND hlist=NULL;
	char port[40];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
	  case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		GetDlgItemText(hwnd, IDC_PREFERENCES_PORT, port, sizeof(port) - 1);
		
		serverPreferences.port = atoi(port);

		savePreferences();
		showpreferences = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	  return 0;
	  case IDCANCEL:

		showpreferences = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showpreferences = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


int CALLBACK TWXExportXMLProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i=0;
	HWND hlist=NULL;
	char gameName[128];
	char sessionId[128];
	int gameTotal=0;
	char buffer[MAX_LENGTH];
	char filename[MAX_LENGTH];
	int countSelected=0;
	OPENFILENAME of;


    switch (msg) {
      case WM_INITDIALOG:
	return 1;
	  case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_TWXSYNC_EXPORT_XML:
			hlist = GetDlgItem(showexportxml, IDC_TWXSYNC_VIEW_GAMES);
			for (i=0; i<ListView_GetItemCount(hlist); i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					ListView_GetItemText (hlist, i, 1, sessionId, sizeof (sessionId));
					ListView_GetItemText (hlist, i, 0, gameName, sizeof (gameName));
					countSelected++;
				}
			}

			if (countSelected==0) {
				MessageBox(hwnd,"Please select a game to export","TWX Sync",MB_ICONEXCLAMATION);
				return 0;
			} else if (countSelected>1) {
				MessageBox(hwnd,"Please select ONLY one game to export at a time","TWX Sync",MB_ICONEXCLAMATION);
				return 0;
			}

			memset(&of, 0, sizeof(of));
	#ifdef OPENFILENAME_SIZE_VERSION_400
			of.lStructSize = OPENFILENAME_SIZE_VERSION_400;
	#else
			of.lStructSize = sizeof(of);
	#endif
			of.hwndOwner = hwnd;
			of.lpstrFilter = "TWX Files\0*.TWX\0All Files\0*\0\0\0";
			of.lpstrCustomFilter = NULL;
			of.nFilterIndex = 1;
			strcpy (filename, "");
			of.lpstrFile = filename;
			of.nMaxFile = sizeof(filename);
			of.lpstrFileTitle = NULL;
			of.lpstrInitialDir = NULL;
			of.lpstrTitle = "Export TWX v2 File";
			of.Flags = 0;
			if (GetSaveFileName(&of)) {
				if (TWC_ExportTWXv2(sessionId, filename)==0) {
					sprintf (buffer, "TWX v2 XML file was exported successfully to %s", filename);
				}
				else {
					sprintf (buffer, "TWX v2 XML file failed exporting to %s", filename);
				}

				MessageBox(hwnd,buffer,"TWX Sync",MB_ICONEXCLAMATION);
				break;
			}

 		return 0;

	  case IDCANCEL:
		showexportxml = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showexportxml = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

int CALLBACK TWXMaintainAllGameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i=0;
	HWND hlist=NULL;
	char gameName[128];
	char sessionId[128];
	int gameTotal=0;
	long universeSize=0;
	char *password=NULL;

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
	  case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_TWXSYNC_CREATE:
			showcreategameform(hwnd);
		  return 0;
	  case IDC_TWXSYNC_EDIT:
			hlist = GetDlgItem(showmaintainallgame, IDC_TWXSYNC_VIEW_GAMES);
			for (i=0; i<ListView_GetItemCount(hlist); i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					ListView_GetItemText (hlist, i, 1, sessionId, sizeof (sessionId));
					ListView_GetItemText (hlist, i, 0, gameName, sizeof (gameName));
					
					universeSize = TWC_GetUniverseSize(i);
					password = TWC_GetGamePassword(i);
					break;
				}
			}

			showmaintaingameform(hwnd, sessionId, gameName, universeSize, password);
 		return 0;

	  case IDC_TWXSYNC_DELETE:
		if (MessageBox(NULL,"Are you sure you want to delete this game?", "TWX Sync", MB_YESNO | MB_ICONWARNING) == IDYES) {
			hlist = GetDlgItem(showmaintainallgame, IDC_TWXSYNC_VIEW_GAMES);
			for (i=0; i<ListView_GetItemCount(hlist); i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					ListView_GetItemText (hlist, i, 1, sessionId, sizeof (sessionId));
					TWC_DeleteGame(sessionId);
				}
			}

			//update listview of maintenance screens
			if (showmaintainallgame != NULL) {
				hlist = GetDlgItem(showmaintainallgame, IDC_TWXSYNC_VIEW_GAMES);
				ListView_DeleteAllItems (hlist);

				gameTotal =  TWC_GetGameCount();
				
				for (i=0; i<gameTotal; i++) {
					char *tGameName = TWC_GetGameName(i);
					char *tSessionId = TWC_GetGameSessionId(i);
					
					listview_insert_view_games(hlist, i, tGameName, tSessionId);

					TWC_TWXSync_FreeMem((void**)&tGameName);
					TWC_TWXSync_FreeMem((void**)&tSessionId);
				}
			}
			//end update
		}

		return 0;
	  case IDC_TWXSYNC_CLOSE:
		showmaintainallgame = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showmaintainallgame = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


int dashCount(char data[]) {
	int i=0;
	int dashCount=0;
	for (i=0; i<(int) strlen(data); i++) {
		if (data[i]=='-')
			dashCount++;
	}
	return dashCount;
}


int isValidRegCode (char* user1, char* reg, int useRegTxtFile)
{
	char *k_e_y=NULL;
	char *allUserIDs=NULL;
	char filename[MAX_LENGTH];
	char *token=NULL, tempString[2056];
	char *tmp=NULL;
	char *userID1 = NULL;
	char *regCode = NULL;
	FILE *fp=NULL;
	int i=0;
	int returnValue=0, skipToEnd=1;
	char data[128];
	char fullFilePath[MAX_LENGTH];

	strcpy(tempString, "\n");

	if (useRegTxtFile==1)
	{
		strcpy (fullFilePath, appDir);
		strcat (fullFilePath, "reg.txt");

		strcpy(filename, fullFilePath);

		fp = fopen(filename, "r");
		if (fp==NULL)
		{
			sprintf (data, "Invalid Registration Key. No reg.txt found at %s", fullFilePath);
			SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, data);
			validRegCode=0;
			return (0);
		}
		else
		{
			for (i=0; i < (int) strlen (tempString); i++)
				tempString[i]=0;
			i=0;
			//count bytes by reading thru once
			while (!feof(fp))
			{
				fscanf(fp, "%c", &tempString[i]);
				i++;
			}
			tempString[i-1]=0;

			fclose (fp);

			i=0;
			token = strtok(tempString, "*");
			while(token != NULL)
			{
				if (i==0) {
					if (strcmp (token, "-1")!=0)
						userID1 = (char*) strdup(token);
					else
						userID1 = (char*) strdup("");
				}
				else if (i==1) {
					if (strcmp (token, "-1")!=0)
						regCode = (char*) strdup(token);
					else
						regCode = (char*) strdup("");
				}
				i++;

				token = strtok(NULL,"*");
			}
		} 
	}
	else
	{
		/* if we dont read from file. allocate memory for handles and reg code */
		userID1 = (char *) strdup(user1);
		regCode = (char *) strdup(reg);
	}

	k_e_y = (char*) malloc (23);
	strcpy(k_e_y, "Tradewars 2002 Welcome");


	/* allocate memory for the concatenated handles */
	allUserIDs = (char*) malloc (strlen(userID1) + 1);

	/* if reg code is empty or handle1 is empty, return 0 */
	if (strcmp(userID1, "")==0)
	{
		SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "Invalid Registration Key");
		validRegCode=0;
		returnValue = 0;
		skipToEnd=0;
	}
	if (strcmp(regCode, "")==0)
	{
		SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "Invalid Registration Key");
		validRegCode=0;
		returnValue = 0;
		skipToEnd=0;
	}

	/* created concatenated handles */
	strcpy (allUserIDs, "");
	strcat (allUserIDs, userID1);

	/* be sure to NULL-terminate */
	allUserIDs[strlen(allUserIDs)] = 0; 

//	printf ("allUserIDs: %s\n", allUserIDs);
	if (skipToEnd==1)
	{
		/* decide whether or not to register the handles */
		tmp = encrypt_string(k_e_y, allUserIDs);
		if (strcmp(tmp, regCode)==0)
		{
			SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "Valid Registration Key");
			validRegCode=1;
			returnValue = 1;
			//for auth server
			strcpy (g_userId, userID1);
		}
		else
		{
			SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "Invalid Registration Key");
			validRegCode=0;
			returnValue = 0;
		}
		pfree((void **)&tmp);
	}


	pfree((void **)&k_e_y);
	pfree((void **)&allUserIDs);
	pfree((void **)&userID1);
	pfree((void **)&regCode);
	return (returnValue);
}




int savePreferences() {
	SERVER_PREFERENCES tmp_server_Preferences;
	char fullFilePath[MAX_LENGTH];
	FILE *fp;
	size_t write;

	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "serverPreferences.bin");

	// open file
	fp = fopen(fullFilePath, "wb");
	if (fp == NULL) {
		SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "Error opening serverPreferences.bin for write");
		return -1;
	}


	memset (&tmp_server_Preferences, 0, sizeof (SERVER_PREFERENCES));

	tmp_server_Preferences.port = htons(serverPreferences.port);
	strcpy (tmp_server_Preferences.reserved, serverPreferences.reserved);

	// write twx header
	fseek(fp, 0, SEEK_SET);
	write = fwrite(&tmp_server_Preferences, sizeof(SERVER_PREFERENCES), READ_WRITE_AMOUNT, fp);

	if (write < READ_WRITE_AMOUNT)
	{
		SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "Error writing data to serverPreferences.bin");
		fclose(fp);
		return -2;
	}

	fclose(fp);
	return 0;
}

void createPreferences () {
	serverPreferences.port=defaultPort;
	memset(serverPreferences.reserved, 0, sizeof(serverPreferences.reserved));
}

int loadPreferences () {
	SERVER_PREFERENCES tmp_server_Preferences;
	char fullFilePath[MAX_LENGTH];
	FILE *fp;
	size_t read;

	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "serverPreferences.bin");

	// open file
	fp = fopen(fullFilePath, "rb");
	if (fp == NULL) {
		createPreferences();
		savePreferences();
		return -1;
	}


	memset (&tmp_server_Preferences, 0, sizeof (SERVER_PREFERENCES));


	// write twx header
	fseek(fp, 0, SEEK_SET);
	read = fread(&tmp_server_Preferences, sizeof(SERVER_PREFERENCES), READ_WRITE_AMOUNT, fp);
	
	if (read < READ_WRITE_AMOUNT)
	{
		SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "Error reading data from serverPreferences.bin");
		fclose(fp);
		return -2;
	}


	serverPreferences.port = ntohs(tmp_server_Preferences.port);
	strcpy (serverPreferences.reserved, tmp_server_Preferences.reserved);


	fclose(fp);
	return 0;
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
