// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>
#include <math.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "attacGUI.h"
#include "twxSync.h"
#include "../PUTTY.H"


typedef struct _tagSORTDATA{
    HWND hwndList;
    int isortSubItem; 
    int iUPDOWN;
} SORTDATA;

// define _MT so that _beginthread( ) is available
#ifndef _MT
#define _MT
#endif


int nItem1=-1, nItem2;

int isFedCommVisible=0;
int isPrivateCommVisible=0;

int sortCount;
int showPPTab=1;


int SortSubno[NO_OF_SUBITEM];

    
short promptSave=0;

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

void listview_insert(HWND hlistTmp, int row, char *port1, char *port2, char *minamount, char *minpercent, char *dist)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = port1;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, port2);
    ListView_SetItemText(hlistTmp, item.iItem, 2, minamount);
    ListView_SetItemText(hlistTmp, item.iItem, 3, minpercent);
    ListView_SetItemText(hlistTmp, item.iItem, 4, dist);

}


void listview_insert_planet(HWND hlistTmp, int row, char *sector, char *planet, char * owner)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = sector;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, planet);
    ListView_SetItemText(hlistTmp, item.iItem, 2, owner);
}

void listview_insert_twxsync(HWND hlistTmp, int row, char *name, char *sessionId)
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

void listview_insert_bubble_entrances(HWND hlistTmp, int row, char *sector, char *size)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = sector;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, size);
}

void listview_insert_bubble_sectors(HWND hlistTmp, int row, char *sector)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = sector;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

}

void listview_insert_allied(HWND hlistTmp, int row, char *trader, char *status)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = trader;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, status);
}

void listview_insert_bust(HWND hlistTmp, int row, char *sector, char *daybusted, char *person, char *daysfromtoday)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = sector;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, daybusted);
    ListView_SetItemText(hlistTmp, item.iItem, 2, person);
    ListView_SetItemText(hlistTmp, item.iItem, 3, daysfromtoday);
}


void listview_insert_limpitscanner(HWND hlistTmp, int row, char *tkey, char *tdata)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = tkey;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, tdata);
}


void listview_insert_remote_connection(HWND hlistTmp, int row, char *num, char *ipaddress, char *time, char *status)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = num;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, ipaddress);
    ListView_SetItemText(hlistTmp, item.iItem, 2, time);
	ListView_SetItemText(hlistTmp, item.iItem, 3, status);
}


void listview_insert_macro(HWND hlistTmp, int row, char *tkey, char *tdata)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = tkey;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, tdata);
}


void listview_insert_blocked_port(HWND hlistTmp, int row, char *sector, char *timestamp)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = sector;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, timestamp);
}

void listview_insert_fighter(HWND hlistTmp, int row, char *sector, char *amount, char *owner, char* type)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = sector;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, amount);
    ListView_SetItemText(hlistTmp, item.iItem, 2, owner);
    ListView_SetItemText(hlistTmp, item.iItem, 3, type);
}

void listview_insert_deadend(HWND hlistTmp, int row, char *deadend)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = deadend;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);
}

void listview_insert_porttype(HWND hlistTmp, int row, char *sector, char *fuelamount, char *fuelpercent, char* orgamount, char* orgpercent, char* equipamount, char* equippercent, char* dist)
{

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
	item.iItem = row;
	item.lParam  = row;
	item.iSubItem = 0;
    item.mask       = LVIF_TEXT | LVIF_PARAM;
    item.pszText    = sector;
    item.cchTextMax = sizeof("Memory Leak");
    ListView_InsertItem(hlistTmp, &item);

    ListView_SetItemText(hlistTmp, item.iItem, 1, fuelamount);
    ListView_SetItemText(hlistTmp, item.iItem, 2, fuelpercent);
    ListView_SetItemText(hlistTmp, item.iItem, 3, orgamount);
    ListView_SetItemText(hlistTmp, item.iItem, 4, orgpercent);
    ListView_SetItemText(hlistTmp, item.iItem, 5, equipamount);
    ListView_SetItemText(hlistTmp, item.iItem, 6, equippercent);
    ListView_SetItemText(hlistTmp, item.iItem, 7, dist);
}

static int CALLBACK ShowListBoxData(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		listbox_data = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	listbox_data = NULL;


	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK MapProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  HDC hDC;
  PAINTSTRUCT ps;
  RECT rect;
  char mapSector[10];
  int xPos, yPos;
  long clickedSector=0;

  GetClientRect(hwnd, &rect);
  switch(msg)
  {
       case WM_CREATE:
	   case WM_SIZE:
		  if(wParam != SIZE_MINIMIZED)
		  {
			GetDlgItemText(hwnd, IDC_MAP_SECTOR, mapSector, sizeof(mapSector) - 1);
			MoveWindow(GetDlgItem(hwnd, IDC_MAP_LABEL), rect.left, rect.bottom-40, 50, 20, TRUE);
			MoveWindow(GetDlgItem(hwnd, IDC_MAP_LABEL2), rect.left, rect.bottom-20, 450, 20, TRUE);
			MoveWindow(GetDlgItem(hwnd, IDC_MAP_SECTOR), rect.left, rect.bottom-40, 50, 20, TRUE);
			InvalidateRect (hwnd, &rect, FALSE);
			buildMapDataStructure(hwnd, atol(mapSector), 5, rect.bottom, rect.right);
		  }
	   break;
	   case WM_LBUTTONDOWN:

		   xPos = LOWORD(lParam);  // horizontal position of cursor 
		   yPos = HIWORD(lParam);  // vertical position of cursor 

//		   printf ("X: %d\n", xPos);
//		   printf ("Y: %d\n", yPos);
			clickedSector = getClickMapSector(xPos, yPos);
			if (clickedSector>0 && clickedSector <= header.sector_cnt)
			{
				sprintf (mapSector, "%ld", clickedSector);
				SetDlgItemText(hwnd, IDC_MAP_SECTOR, mapSector);
				buildMapDataStructure(hwnd, clickedSector, 5, rect.bottom, rect.right);
				InvalidateRect (hwnd, &rect, FALSE);
			}

		   break;
	  case WM_CLOSE:
			showmap = NULL;
			if (showbubblefinder==NULL) {
				#ifdef P_FREE
					pfree(&mapLines);
				#else
					free(mapLines);
				#endif
			}

			SetActiveWindow(GetParent(hwnd));
			DestroyWindow(hwnd);
		break;
	  case WM_PAINT:
		hDC = BeginPaint (hwnd, &ps);
		GetDlgItemText(hwnd, IDC_MAP_SECTOR, mapSector, sizeof(mapSector) - 1);

		drawMap(hDC, hwnd, atol(mapSector), 0);
		EndPaint(hwnd, &ps);

		break;
      case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
		  case IDC_MAP_SECTOR:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				GetDlgItemText(hwnd, IDC_MAP_SECTOR, mapSector, sizeof(mapSector) - 1);
				if (atol(mapSector)>0 && atol(mapSector) <= header.sector_cnt)
				{
					buildMapDataStructure(hwnd, atol(mapSector), 5, rect.bottom, rect.right);
					InvalidateRect (hwnd, &rect, FALSE);
				}
			}
			return 0;
		}
	}
	return 0;
}


static int CALLBACK ShowRemoteConnectionProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	HWND hlist;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno, i;
	char buffer[20];

	hlist = GetDlgItem(hwnd, IDC_REMOTECONNECTION_LIST);

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_REMOTECONNECTION_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_REMOTECONNECTION_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Remote Connection Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_REMOTECONNECTION_ENABLE:
		if (IsDlgButtonChecked(hwnd, IDC_REMOTECONNECTION_ENABLE))
		{
			if (remoteConnectionEnabled==0)
			{
				if(_beginthread(remoteConnectionThreadProc, 0, NULL )==-1)
				{
					printf("Failed to create thread");
					return 1;
				}
			}
		}
		else
		{
			stopRemoteConnectionServer();
		}

		return 0;
	  case IDC_REMOTECONNECTION_KILL:
			for (i=0; i<ListView_GetItemCount(hlist); i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					ListView_GetItemText (hlist, i, 0, buffer, sizeof (buffer));
					i = ListView_GetItemCount(hlist);
				}
			}

			if ((strcmp (buffer, "") != 0) && atoi(buffer)-1< TOTAL_CONNECTIONS && theClients[atoi(buffer)-1].sock>0)
			{
				shutdown(theClients[atoi(buffer)-1].sock, SD_SEND);
				close (theClients[atoi(buffer)-1].sock);
				theClients[atoi(buffer)-1].sock=0;
				updateRemoteList();
			}

		return 0;
	  case IDOK:
  	    showremoteconnection = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showremoteconnection = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK MacroProcEdit(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	HWND hlist;
	int i;

	char buffer[64], buffer1[MAX_LENGTH];
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:

		hlist = GetDlgItem(showmacro, IDC_MACRO_LIST);
		GetDlgItemText(hwnd, IDC_MACROEDIT_CAPTION, buffer, sizeof(buffer) - 1);
		GetDlgItemText(hwnd, IDC_MACROEDIT_EDIT, buffer1, sizeof(buffer1) - 1);

		for (i=0; i<macroCount; i++)
		{
			if (strcmp (theMacros[i].key, buffer)==0)
			{
				strcpy (theMacros[i].data, buffer1);
				i=macroCount;
			}
		}

		writeMacros();
		readMacros();
		reDrawMacros (hlist);

		//update the form
		showmacroedit = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showmacroedit = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showmacroedit = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK LimpitScannerProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	HWND hlist;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	
	hlist = GetDlgItem(hwnd, IDC_LIMPITSCANNER_LIST);

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_LIMPITSCANNER_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_LIMPITSCANNER_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Limpit Scanner Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_LIMPITSCANNER_START:
		attac_send("k2");
		enableLimpitScanner=1;

		return 0;
	  case IDC_LIMPITSCANNER_STOP:
		  enableLimpitScanner=0;
	    return 0;
	  case IDC_LIMPITSCANNER_CLOSE:
		enableLimpitScanner=0;

		showlimpitscanner = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showlimpitscanner = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}




static int CALLBACK TWXSyncProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	HWND hlist;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	char password[40];
	char sessionId[128];
	char gameName[128];
	int i=0;
	int selected=0;
	
	hlist = GetDlgItem(hwnd, IDC_TWXSYNC_GAMELIST);

    switch (msg) {
      case WM_INITDIALOG:
		if(_beginthread(GetTWXGamesThreadProc, 0, NULL )==-1)
		{
			printf("Failed to create thread");
			return 1;
		}
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_TWXSYNC_GAMELIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_TWXSYNC_GAMELIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "TWX Sync Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_TWXSYNC_SAVE:
		  GetDlgItemText(hwnd, IDC_TWXSYNC_PASSWORD, password, sizeof(password) - 1);

			for (i=0; i<ListView_GetItemCount(hlist); i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					selected=1;
					
					ListView_GetItemText (hlist, i, 0, gameName, sizeof (gameName));
					ListView_GetItemText (hlist, i, 1, sessionId, sizeof (sessionId));
					strcpy (header.TWXSyncSession, sessionId);
					strcpy (header.TWXSyncPassword, password);
					do_writeMemory();
					loginAgain=1;
					SetDlgItemText(showtwxsync, IDC_TWXSYNC_CURRGAME, gameName);
					SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Settings changed. Restart ATTAC!");
					header.lastSyncTimeWithServer=0;
					break;
				}
			}
			
			if (selected==0)
				MessageBox(showtwxsync, "Please Select a game first", "Error", MB_OK);
		  return 0;
	  case IDC_TWXSYNC_RESEND_ALL:
		header.lastSyncTimeWithServer=0;
		SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "All data will be resent!");
		do_writeMemory();
		return 0;
	  case IDCANCEL:
		showtwxsync = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;

	}
	return 0;
      case WM_CLOSE:
		showtwxsync = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


static int CALLBACK MacroProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char key[64], data[MAX_LENGTH], buffer[MAX_LENGTH], buffer1[MAX_LENGTH];
	int i, j;
	HWND hlist;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;

	hlist = GetDlgItem(hwnd, IDC_MACRO_LIST);

    switch (msg) {
      case WM_INITDIALOG:
  	  return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_MACRO_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_MACRO_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Macro Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_MACRO_ADD:
			GetDlgItemText(hwnd, IDC_MACRO_FUNCTION, key, sizeof(key) - 1);
			GetDlgItemText(hwnd, IDC_MACRO_DATA, data, sizeof(data) - 1);

			if (strcmp (key, "")==0)
			{
				MessageBox(hwnd, "Please select a function key", "ATTAC Error", MB_ICONWARNING); 
				return 0;
			}
			
			if (strcmp (data, "")==0)
			{
				MessageBox(hwnd, "Please enter a macro", "ATTAC Error", MB_ICONWARNING); 
				return 0;
			}

			for (i=0; i<macroCount; i++)
			{
				if (strcmp (theMacros[i].key, key)==0)
				{
					MessageBox(hwnd, "One macro per function key. Please delete the existing key first", "ATTAC Error", MB_ICONWARNING); 
					return 0;
				}
			}
			strcpy (theMacros[macroCount].key, key);
			strcpy (theMacros[macroCount].data, data);
			macroCount++;

			writeMacros();
		    reDrawMacros (hlist);
		  return 0;
	  case IDC_MACRO_EDIT:
		    strcpy (buffer, "Macro Number?");
			strcpy (buffer1, "");
			for (i=0; i<macroCount; i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					ListView_GetItemText (hlist, i, 0, buffer, sizeof (buffer));
					ListView_GetItemText (hlist, i, 1, buffer1, sizeof (buffer1));
					i=macroCount;
				}
			}
			if (strcmp (buffer1, "")==0)
				MessageBox(hwnd, "Please select a macro to edit", "Error", MB_OK);
			else
				showmacroeditform(hwnd, buffer, buffer1);
		  return 0;
	  case IDC_MACRO_DELETE:
			for (i=0; i<macroCount; i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					for (j=0; j<macroCount; j++)
					{
						ListView_GetItemText (hlist, i, 0, buffer, sizeof (buffer));
						if (strcmp (theMacros[j].key, buffer)==0)
						{
							strcpy (theMacros[j].key, "");
							writeMacros();
							readMacros();
							reDrawMacros (hlist);
						}
					}
				}
			}


		  return 0;
	  case IDC_MACRO_CLOSE:
		showmacro = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showmacro = NULL;


	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

void readMacros()
{
	FILE *fp;
	int i, lineCount=0;
	char c, line[MAX_LENGTH], *token;

	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "macros.ini");

	fp = fopen (fullFilePath, "r");

	if (fp==NULL)
	{	
//		sprintf (buffer, "Could not open %s", fullFilePath);
//		attacErrorMessage (buffer);
	}
	else
	{

		for (i=0; i<MAX_MACROS; i++)
			strcpy (theMacros[i].key, "");

		macroCount=0;

		c = fgetc(fp);

		while( !feof(fp)) 
		{
			if (c != '\n')
			{
				line[lineCount++]=c;
			
			}
			else
			{
				line[lineCount]=0;
			
				token = strtok (line, "=");
				if (token != NULL)
				{
					if (strcmp (token, "FastMacro")==0)
					{
						token = strtok (NULL, "=");
						if (token != NULL)
							strcpy (fastMacroString, token);
					}
					else
					{
						strcpy (theMacros[macroCount].key, token);
						
						token = strtok (NULL, "=");
						if (token != NULL)
							strcpy (theMacros[macroCount].data, token);

						macroCount++;
					}
				}

				strcpy(line, "");
				lineCount=0;
			}
			c = fgetc(fp);
		}
		fclose (fp);
	}
}
void writeMacros ()
{
	FILE *fp;
	int i;
	char buffer[MAX_LENGTH*2];

	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "macros.ini");

	fp = fopen (fullFilePath, "w");

	if (fp==NULL)
	{	
		sprintf (buffer, "Could not open %s", fullFilePath);
		attacErrorMessage (buffer);
	}
	else
	{
		for (i=0; i<MAX_MACROS; i++)
		{
			if (strcmp (theMacros[i].key, "") != 0)
				fprintf (fp, "%s=%s\n", theMacros[i].key, theMacros[i].data);
		}
		fprintf (fp, "FastMacro=%s\n", fastMacroString);

		fclose(fp);
	}
}

void updateRemoteConnectionList (HWND remoteListHandle)
{
	int i=0;
	char *tmpA, *tmpB, *tmpC, *tmpD, count=0;
	char buffer[128];
	
	ListView_DeleteAllItems (remoteListHandle);
	for (i=0; i< TOTAL_CONNECTIONS; i++)
	{
		if (theClients[i].sock>0)
		{
			sprintf (buffer, "%d", theClients[i].num);

			tmpA = (char *) strdup(buffer);

			tmpB = (char *) strdup(theClients[i].hostname);

			strcpy(buffer, ctime(&theClients[i].theTime));
			buffer[strlen(buffer)-1]=0;
			//reformat for string with time function
//			sprintf (buffer, "%s", buffer);

			tmpC = (char *) strdup(buffer);

			tmpD = (char *) strdup("Active");

			listview_insert_remote_connection(remoteListHandle, count++, tmpA, tmpB, tmpC, tmpD);

			#ifdef P_FREE
				pfree(&tmpA);
				pfree(&tmpB);
				pfree(&tmpC);
				pfree(&tmpD);
			#else
				free(tmpA);
				free(tmpB);
				free(tmpC);
				free(tmpD);
			#endif
		}
	}
}

void reDrawMacros (HWND macrolisthandle)
{
	int i;
	char *tmpA, *tmpB;

	ListView_DeleteAllItems (macrolisthandle);

	for (i=0; i< macroCount; i++)
	{
		tmpA = (char *) strdup(theMacros[i].key);

		tmpB = (char *) strdup(theMacros[i].data);

		listview_insert_macro(macrolisthandle, i, tmpA, tmpB);

		#ifdef P_FREE
			pfree(&tmpA);
			pfree(&tmpB);
		#else
			free(tmpA);
			free(tmpB);
		#endif
	}
}

void updateLimpitScanner (HWND limpitscannerhandle, long foundSector)
{
	int i, limpitCount=0;
	char tmpA[20], tmpB[20];
	long *oldData;
	char buffer[20], amount[20];
	SORTDATA SortData;

	oldData = (long *) malloc(header.sector_cnt * sizeof(long));

	for (i=0; i<header.sector_cnt; i++)
		oldData[i]=0;

	for (i=0; i < ListView_GetItemCount(limpitscannerhandle); i++)
	{
		ListView_GetItemText (limpitscannerhandle, i, 0, buffer, sizeof (buffer));
		ListView_GetItemText (limpitscannerhandle, i, 1, amount, sizeof (amount));
		oldData[atol(buffer)-1] = atoi(amount);
	}

	ListView_DeleteAllItems (limpitscannerhandle);

	for (i=0; i< header.sector_cnt; i++)
	{
		if (i+1==foundSector)
		{
			oldData[i]++;
		}

		if (oldData[i]>0)
		{
			sprintf (tmpA, "%d", i+1);
			sprintf (tmpB, "%ld", oldData[i]);

			listview_insert_limpitscanner(limpitscannerhandle, limpitCount++, tmpA, tmpB);
		}
	}
	
	SortData.hwndList = limpitscannerhandle;
	SortData.isortSubItem = 1;
	SortData.iUPDOWN = UP;
	if(ListView_SortItems(limpitscannerhandle, MyCompProc, (LPARAM) &SortData) != TRUE)
		MessageBox(showlimpitscanner, "Limpit Scanner Sorting Error", "Error", MB_OK);


	#ifdef P_FREE
		pfree (&oldData);
	#else
		free (oldData);
	#endif
}


int CALLBACK MessageProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH defMBrush;
	static COLORREF msgColor;
	static HWND messageBox;
	TEXTMETRIC tm; 
    char tchBuffer[MAX_LENGTH]; 
	char *t=NULL;
	int y=0, i=0;

    switch (msg) {
      case WM_INITDIALOG:
		  {
			SetDlgItemText(hwnd, 100, ver);

			/* create the brushes we need */
			defMBrush = CreateSolidBrush(RGB(0,0,0));

			messageBox = GetDlgItem(hwnd, IDC_MESSAGES);

			return 1;
		  }

	  case WM_CTLCOLORLISTBOX:
		  return (BOOL)defMBrush;
	  case WM_MSG:
		  {
			  char* msg = (char*) wParam;

			  COLORREF color = (COLORREF) lParam;

			  msgColor = color;
		
			  SendMessage(messageBox, LB_INSERTSTRING, (WPARAM) 0, (LPARAM) msg);

			  //the msg is malloc'ed in info.c in the attacMessage function, we cant free it till we pass it to the control
				#ifdef P_FREE
					pfree (&msg);
				#else
					free (msg);
				#endif
		  }
		  break;
	  case WM_PARENTSIZED:
		  {
			RECT rect,myrect, lbrect, clientrect;
			HWND ctl;
			LONG initialwidth, widthdiff;

			GetWindowRect(GetParent(hwnd), &rect);
			GetWindowRect(hwnd, &myrect);
			GetClientRect(hwnd, &clientrect);

			initialwidth = myrect.right - myrect.left;

			/* resize the listbox */
			ctl = GetDlgItem(hwnd, IDC_MESSAGES);
			GetWindowRect(ctl, &lbrect);
	
			/* width difference */
			widthdiff = initialwidth - (rect.right - rect.left);

			if (widthdiff != 0) {
				/* message window size changed */
				MoveWindow(ctl, clientrect.left, clientrect.top, lbrect.right - lbrect.left - widthdiff, lbrect.bottom-lbrect.top, TRUE);
			}
			MoveWindow(hwnd, rect.left, rect.bottom,
			/* same width as main window */ rect.right - rect.left, myrect.bottom-myrect.top, TRUE);

		  }
		  break;
	  case WM_PARENTMOVED:
		  {
			  RECT rect, myrect;
			  LONG x,y,w,h;

			  GetWindowRect(GetParent(hwnd), &rect);
			  GetWindowRect(hwnd, &myrect);

			  x = rect.left;
			  y = rect.bottom;
			  w = myrect.right - myrect.left;
			  h = myrect.bottom - myrect.top;

			  MoveWindow(hwnd, x, y, w, h, TRUE);

			  return 0;
		  }
		  break;
      case WM_CLOSE:
		showmessagecenter = NULL;
		DeleteObject(defMBrush);
		DestroyWindow(hwnd);

		return 0;
	  case WM_UNHIDE:
		ShowWindow(hwnd, SW_NORMAL|SW_SHOW);
		return 0;
	  case WM_DRAWITEM:
		  {
			  /* Owner draw code */
			  LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
			  // If there are no list box items, skip this message. 
 
            if (lpdis->itemID == -1) 
            { 
                break; 
            } 
 
            // Draw the bitmap and text for the list box item. Draw a 
            // rectangle around the bitmap if it is selected. 
 
            switch (lpdis->itemAction) 
            { 
                case ODA_SELECT: 
                case ODA_DRAWENTIRE: 
				{
					COLORREF color;

					// Display the text associated with the item. 
					SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM) tchBuffer); 
				
					SetBkColor(lpdis->hDC, BLACK);

					color = GetColorForTextFromAnsiString(tchBuffer);

					t = StripAnsiFromString(tchBuffer);
					strcpy(tchBuffer, t);
					#ifdef P_FREE
						pfree (&t);
					#else
						free (t);
					#endif

					SetTextColor(lpdis->hDC, color);

					GetTextMetrics(lpdis->hDC, &tm); 
				
					y = (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2; 
				
					TextOut(lpdis->hDC, 6, y, tchBuffer, strlen(tchBuffer)); 
				}
                break; 
			}

		  }
		  break;
    }
    return 0;
}

static int CALLBACK InfoProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH defIBrush;
	char *strMessageCenter;

    switch (msg) {
      case WM_INITDIALOG:
		  {
			SetDlgItemText(hwnd, 100, ver);

			// create the brushes we need
			defIBrush = CreateSolidBrush(RGB(0,0,0));
			
			ctlAlign = GetDlgItem(hwnd, IDC_ALIGN);
			ctlFigs = GetDlgItem(hwnd, IDC_FIGS);
			ctlShields = GetDlgItem(hwnd, IDC_SHIELDS);
			ctlTurns = GetDlgItem(hwnd, IDC_TURNS);
			ctlCredits = GetDlgItem(hwnd, IDC_CREDITS);

			return 1;
		  }

	  case WM_PARENTSIZED:
		  {
			  //nothing
		  }
		  break;
	  case WM_PARENTMOVED:
		  {
			  RECT rect, parent;

			  GetWindowRect(GetParent(hwnd), &parent);
			  GetWindowRect(hwnd, &rect);
			  
			  MoveWindow(hwnd, parent.left - (rect.right - rect.left), 
				  parent.top, rect.right-rect.left,rect.bottom-rect.top, TRUE);
		  }
		  break;
      case WM_COMMAND:
		return 0;
      case WM_CLOSE:
		showinfocenter = NULL;

		DestroyWindow(hwnd);

		DeleteObject(defIBrush);
		return 0;

	  case WM_CTLCOLORSTATIC:
		  {
			  strMessageCenter =  (char *) malloc (128);
			  GetWindowText((HWND)lParam, strMessageCenter, 128);

			  // add black background to the appropriate areas of the info center
			  if (strMessageCenter && strMessageCenter[0] && (stricmp(strMessageCenter, "Script Not Running") == 0 || stricmp(strMessageCenter, "Running ...") == 0 || !isalpha(strMessageCenter[0]))) 
			  {
				COLORREF color = GetColorForText((HDC) wParam, (HWND) lParam);
				SetBkColor((HDC) wParam, BLACK);
				SetTextColor((HDC) wParam, color);
				#ifdef P_FREE
					pfree (&strMessageCenter);
				#else
					free (strMessageCenter);
				#endif
			  }
			  else
			  {
				#ifdef P_FREE
					pfree (&strMessageCenter);
				#else
					free (strMessageCenter);
				#endif
				return 0;
			  }
		  }
		  return (int) defIBrush;
	}
    return 0;

}


static int CALLBACK ShowPortTypeProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	HWND hlist;

	
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_PORTTYPE_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_PORTTYPE_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Port Type Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;

      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		showporttype = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showporttype = NULL;
	//say we closed out the zoc ask

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK MegaJettisonProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char rexxArgs[100][100];
	char fuel[30];
	char org[30];
	char equip[30];
	char type[30];
	char planet[30];
	char cycles[30];

	strcpy (cycles, "9999");

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_MEGAJETTISON_JETTISON:
		EnableWindow( GetDlgItem(hwnd,IDC_MEGAJETTISON_PLANET), FALSE ) ; /* disable a control */		
		break;
	  case IDC_MEGAJETTISON_DUMP:
		EnableWindow( GetDlgItem(hwnd,IDC_MEGAJETTISON_PLANET), TRUE ) ; /* enable a control */		
		break;
	  case IDC_MEGAJETTISON_DEFAULTS:
			SetDlgItemText(hwnd, IDC_MEGAJETTISON_FUEL_AMOUNT, "27");
			SetDlgItemText(hwnd, IDC_MEGAJETTISON_ORG_AMOUNT, "16");
			SetDlgItemText(hwnd, IDC_MEGAJETTISON_EQUIP_AMOUNT, "10");

			CheckDlgButton(hwnd, IDC_MEGAJETTISON_FUEL_CHECK, BST_CHECKED);
			CheckDlgButton(hwnd, IDC_MEGAJETTISON_ORG_CHECK, BST_CHECKED);
			CheckDlgButton(hwnd, IDC_MEGAJETTISON_EQUIP_CHECK, BST_CHECKED);
		return 0;
	  case IDOK:

		GetDlgItemText(hwnd, IDC_MEGAJETTISON_PLANET, planet, sizeof(planet) - 1);
		  
		if (IsDlgButtonChecked(hwnd, IDC_MEGAJETTISON_FUEL_CHECK))
			GetDlgItemText(hwnd, IDC_MEGAJETTISON_FUEL_AMOUNT, fuel, sizeof(fuel) - 1);
		else
			strcpy (fuel, "0");
		
		if (IsDlgButtonChecked(hwnd, IDC_MEGAJETTISON_ORG_CHECK))
			GetDlgItemText(hwnd, IDC_MEGAJETTISON_ORG_AMOUNT, org, sizeof(org) - 1);
		else
			strcpy (org, "0");

		if (IsDlgButtonChecked(hwnd, IDC_MEGAJETTISON_EQUIP_CHECK))
			GetDlgItemText(hwnd, IDC_MEGAJETTISON_EQUIP_AMOUNT, equip, sizeof(equip) - 1);
		else
			strcpy (equip, "0");

		if (IsDlgButtonChecked(hwnd, IDC_MEGAJETTISON_JETTISON))
			strcpy(type,"JETT");
		if (IsDlgButtonChecked(hwnd, IDC_MEGAJETTISON_DUMP))
			strcpy(type,"DUMP");

		strcpy (rexxArgs[0], fuel);
		strcpy (rexxArgs[1], org);
		strcpy (rexxArgs[2], equip);
		strcpy (rexxArgs[3], type);
		strcpy (rexxArgs[4], planet);
		strcpy (rexxArgs[5], cycles);

		strcpy (fullFilePath, scriptDir);
		strcat (fullFilePath, "StandardMegaJettison.erx");
		REXX_Start(fullFilePath, 1, 6, rexxArgs);

		ShowWindow(showmegajettison,SW_HIDE);
	    return 0;
	  case IDCANCEL:
		  ShowWindow(showmegajettison,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
		ShowWindow(showmegajettison,SW_HIDE);
	return 0;
    }
    return 0;
}

void startSysopPortScript ()
{
	char rexxArgs[100][100];

	strcpy (rexxArgs[0], "");

	strcpy (fullFilePath, scriptDir);
	strcat (fullFilePath, "StandardTEditPortsToAmount.erx");
	REXX_Start(fullFilePath, 1, 1, rexxArgs);
}


void startSysopAddToClosedGame ()
{
	char rexxArgs[100][100];

	strcpy (rexxArgs[0], "");

	strcpy (fullFilePath, scriptDir);
	strcat (fullFilePath, "StandardTEditAddToClosedGame.erx");
	REXX_Start(fullFilePath, 1, 1, rexxArgs);
}


int CALLBACK MyCompProc(LPARAM lp1, LPARAM lp2, LPARAM lp3)
{
    LVFINDINFO lvf;
    int nItem1, nItem2;
    char buf1[MAX_LENGTH], buf2[MAX_LENGTH];
    SORTDATA *lpsd;

    lpsd = (SORTDATA *)lp3;

    lvf.flags = LVFI_PARAM;
    lvf.lParam = lp1;
    nItem1 = ListView_FindItem(lpsd->hwndList, -1, &lvf);

    lvf.lParam = lp2;
    nItem2 = ListView_FindItem(lpsd->hwndList, -1, &lvf);
    
    ListView_GetItemText(lpsd->hwndList, nItem1, lpsd->isortSubItem, buf1, sizeof(buf1));

    ListView_GetItemText(lpsd->hwndList, nItem2, lpsd->isortSubItem, buf2, sizeof(buf2));

	if (strlen(buf1)>0 && strlen(buf2)>0 && !isdigit(buf1[0]) && !isdigit(buf2[0]))
	{
		if (lpsd->iUPDOWN == UP) 
		{
			return (stricmp (buf1, buf2));
		} 
		else 
		{
			return (stricmp (buf1, buf2) *-1);
		}
	}
	else
	{
		if (lpsd->iUPDOWN == UP) 
		{
			if (atol(buf1)<atol(buf2))
				return 1;
			else if (atol(buf1)>atol(buf2))
				return -1;
			else
				return 0;
		} 
		else 
		{
			if (atol(buf1)<atol(buf2))
				return -1;
			else if (atol(buf1)>atol(buf2))
				return 1;
			else
				return 0;
		}
	}
}



static int CALLBACK ShowPPProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	NMHDR FAR *tem;
	int num;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	char *tmpPP;

	char amount[256];
	char percent[256];
	char routesector[256];

    switch (msg) {
      case WM_INITDIALOG:

		hShowPairPort = GetDlgItem(hwnd, IDC_SHOW_PP_LIST);

		listview_insert_column (hShowPairPort, 0, 110, 0, "Port1");
		listview_insert_column (hShowPairPort, 1, 110, 1, "Port2");
		listview_insert_column (hShowPairPort, 2, 110, 2, "Minimum Amount");
		listview_insert_column (hShowPairPort, 3, 140, 3, "Minimum Percent");
		listview_insert_column (hShowPairPort, 4, 180, 4, "Distance from Route Sector");

		tmpPP = generate_pp(0, 0, 0, 0, 1);
		#ifdef P_FREE
			pfree (&tmpPP);
		#else
			free (tmpPP);
		#endif

		isortsubno=2;
		SortSubno[isortsubno] = UP;
		SortData.hwndList = hShowPairPort;
		SortData.isortSubItem = isortsubno;
		SortData.iUPDOWN = SortSubno[isortsubno];
		if(ListView_SortItems(hShowPairPort, MyCompProc, (LPARAM) &SortData) != TRUE)
			MessageBox(hwnd, "Pair Port Sorting Error", "Error", MB_OK);
		break;

	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_SHOW_PP_LIST) 
			{
				hShowPairPort = GetDlgItem(hwnd, IDC_SHOW_PP_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;
                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hShowPairPort;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hShowPairPort, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Pair Port Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		tem=(NMHDR FAR *)lParam;
		if (tem->code== TCN_SELCHANGE)
		{
			num=TabCtrl_GetCurSel(tem->hwndFrom);
			GetDlgItemText(hwnd, IDC_SHOW_PP_FILTER_AMOUNT, amount, sizeof(amount) - 1);
			GetDlgItemText(hwnd, IDC_SHOW_PP_FILTER_PERCENT, percent, sizeof(percent) - 1);
			GetDlgItemText(hwnd, IDC_SHOW_PP_ROUTESECTOR, routesector, sizeof(routesector) - 1);
			showPPTab = num+1;
			tmpPP = generate_pp(atol(amount), atol(percent), atol(routesector), 0, showPPTab);
			#ifdef P_FREE
				pfree (&tmpPP);
			#else
				free (tmpPP);
			#endif
		}
		return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_SHOW_PP_ROUTESECTOR:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    GetDlgItemText(hwnd, IDC_SHOW_PP_FILTER_AMOUNT, amount, sizeof(amount) - 1);
		    GetDlgItemText(hwnd, IDC_SHOW_PP_FILTER_PERCENT, percent, sizeof(percent) - 1);
			GetDlgItemText(hwnd, IDC_SHOW_PP_ROUTESECTOR, routesector, sizeof(routesector) - 1);
			if (atol(routesector)>0)
			{
				tmpPP = generate_pp(atol(amount), atol(percent), atol(routesector), 0, showPPTab);
				#ifdef P_FREE
					pfree (&tmpPP);
				#else
					free (tmpPP);
				#endif
			}
		}
		return 0;
	  case IDC_SHOW_PP_FILTER_AMOUNT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    GetDlgItemText(hwnd, IDC_SHOW_PP_FILTER_AMOUNT, amount, sizeof(amount) - 1);
			GetDlgItemText(hwnd, IDC_SHOW_PP_ROUTESECTOR, routesector, sizeof(routesector) - 1);
			if (atol(amount)>0)
			{
				tmpPP = generate_pp(atol(amount), 0, atol(routesector), 0, showPPTab);
				#ifdef P_FREE
					pfree (&tmpPP);
				#else
					free (tmpPP);
				#endif
			}
		}
		return 0;
	  case IDC_SHOW_PP_FILTER_PERCENT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    GetDlgItemText(hwnd, IDC_SHOW_PP_FILTER_PERCENT, percent, sizeof(percent) - 1);
			GetDlgItemText(hwnd, IDC_SHOW_PP_ROUTESECTOR, routesector, sizeof(routesector) - 1);
			if (atoi(percent)>0)
			{
				tmpPP = generate_pp(0, atol(percent), atol(routesector), 0, showPPTab);
				#ifdef P_FREE
					pfree (&tmpPP);
				#else
					free (tmpPP);
				#endif
			}
		}
		return 0;
	  case IDOK:
		showpp = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showpp = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showpp = NULL;
	//say we closed out the zoc ask

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}



static int CALLBACK ShowEvilPPProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	char amount[256];
	char percent[256];
	char routesector[256];
	char *tmpEPP;
	
    switch (msg) {
      case WM_INITDIALOG:

		hShowEvilPairPort = GetDlgItem(hwnd, IDC_SHOW_EVIL_PP_LIST);
		listview_insert_column (hShowEvilPairPort, 0, 110, 0, "Port1");
		listview_insert_column (hShowEvilPairPort, 1, 110, 1, "Port2");
		listview_insert_column (hShowEvilPairPort, 2, 110, 2, "Minimum Amount");
		listview_insert_column (hShowEvilPairPort, 3, 140, 3, "Minimum Percent");
		listview_insert_column (hShowEvilPairPort, 4, 180, 4, "Distance from Route Sector");

		tmpEPP = generate_evil_pp(0, 0, 0, 0);
		#ifdef P_FREE
			pfree (&tmpEPP);
		#else
			free (tmpEPP);
		#endif

		isortsubno=2;
		SortSubno[isortsubno] = UP;
		SortData.hwndList = hShowEvilPairPort;
		SortData.isortSubItem = isortsubno;
		SortData.iUPDOWN = SortSubno[isortsubno];
		if(ListView_SortItems(hShowEvilPairPort, MyCompProc, (LPARAM) &SortData) != TRUE)
			MessageBox(hwnd, "Evil Pair Port Sorting Error", "Error", MB_OK);
		break;
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_SHOW_EVIL_PP_LIST) 
			{
			    hShowEvilPairPort = GetDlgItem(hwnd, IDC_SHOW_EVIL_PP_LIST);

                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;
                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hShowEvilPairPort;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hShowEvilPairPort, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Evil Pair Port Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;

      case WM_COMMAND:
  switch (LOWORD(wParam)) {
	  case IDC_SHOW_EVIL_PP_ROUTESECTOR:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    GetDlgItemText(hwnd, IDC_SHOW_EVIL_PP_FILTER_AMOUNT, amount, sizeof(amount) - 1);
		    GetDlgItemText(hwnd, IDC_SHOW_EVIL_PP_FILTER_PERCENT, percent, sizeof(percent) - 1);
			GetDlgItemText(hwnd, IDC_SHOW_EVIL_PP_ROUTESECTOR, routesector, sizeof(routesector) - 1);
			if (atol(routesector)>0)
			{
				tmpEPP = generate_evil_pp(atol(amount), atol(percent), atol(routesector), 0);
				#ifdef P_FREE
					pfree (&tmpEPP);
				#else
					free (tmpEPP);
				#endif			
			}
		}
		return 0;
	  case IDC_SHOW_EVIL_PP_FILTER_AMOUNT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    GetDlgItemText(hwnd, IDC_SHOW_EVIL_PP_FILTER_AMOUNT, amount, sizeof(amount) - 1);
		    GetDlgItemText(hwnd, IDC_SHOW_EVIL_PP_ROUTESECTOR, routesector, sizeof(routesector) - 1);
			tmpEPP = generate_evil_pp(atol(amount), 0, atol(routesector), 0);
			#ifdef P_FREE
				pfree (&tmpEPP);
			#else
				free (tmpEPP);
			#endif			
		}
		return 0;
	  case IDC_SHOW_EVIL_PP_FILTER_PERCENT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    GetDlgItemText(hwnd, IDC_SHOW_EVIL_PP_FILTER_PERCENT, percent, sizeof(percent) - 1);
		    GetDlgItemText(hwnd, IDC_SHOW_EVIL_PP_ROUTESECTOR, routesector, sizeof(routesector) - 1);
			tmpEPP = generate_evil_pp(0, atol(percent), atol(routesector), 0);
			#ifdef P_FREE
				pfree (&tmpEPP);
			#else
				free (tmpEPP);
			#endif			
		}
		return 0;
	  case IDOK:
		showevilpp = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showevilpp = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showevilpp = NULL;
	//say we closed out the zoc ask

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK REXXAskProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;

    switch (msg) {
      case WM_INITDIALOG:
		  PostMessage(hwnd,WM_USER,0,0);

	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, ID_EDITREXXASK, rexxAskText, sizeof(rexxAskText) - 1);
		rexxask = NULL;

		rexxAskResult = (char *) strdup(rexxAskText);
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
		ATTACScripting_SignalATTACComplete();

		printf ("rexxAskResult: %s\n", rexxAskResult);
	    return 0;
	  case IDCANCEL:
		rexxask = NULL;
		//say we closed out the zoc ask
		rexxAskResult = (char *) strdup("##CANCEL##");
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
		ATTACScripting_SignalATTACComplete();
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	rexxask = NULL;
	//say we closed out the zoc ask
	rexxAskResult = (char *) strdup("");

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	ATTACScripting_SignalATTACComplete();
	return 0;
    }
    return 0;
}

static int CALLBACK PairPortProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char buffer[30];
	int index;
	char sector1[30];
	char buying1[30];
	char buying2[30];
	char cycles[30];
	char forcenohaggle[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
      case IDC_PP_OTHER_SECTOR:
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
		    index = SendDlgItemMessage(hwnd, IDC_PP_OTHER_SECTOR, CB_GETCURSEL, 0, 0);
		    SendDlgItemMessage(hwnd, IDC_PP_OTHER_SECTOR, CB_GETLBTEXT,
				       index, (LPARAM)buffer);
			
			updatepairportcheckboxes (showpairport, 2, atol(buffer));
		}
		else if (HIWORD(wParam) == CBN_EDITCHANGE) 
		{
		    GetDlgItemText(hwnd, IDC_PP_OTHER_SECTOR, buffer, sizeof(buffer) - 1);
			updatepairportcheckboxes (showpairport, 2, atol(buffer));
		}
		break;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_PP_OTHER_SECTOR, buffer, sizeof(buffer) - 1);
		strcpy(sector1, buffer);

		strcpy (buying1, "");
		strcpy (buying2, "");

		GetDlgItemText(hwnd, IDC_PP_CURRENT_FUEL, buffer, sizeof(buffer) - 1);
		if ((IsDlgButtonChecked(hwnd, IDC_PP_CURRENT_FUEL)) && (Instr(1, buffer, "(B)")))
			strcpy(buying2, "Fuel Ore");
		GetDlgItemText(hwnd, IDC_PP_CURRENT_ORG, buffer, sizeof(buffer) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_PP_CURRENT_ORG) && (Instr(1, buffer, "(B)")))
			strcpy(buying2, "Organics");
		GetDlgItemText(hwnd, IDC_PP_CURRENT_EQUIP, buffer, sizeof(buffer) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_PP_CURRENT_EQUIP) && (Instr(1, buffer, "(B)")))
			strcpy(buying2, "Equipment");

		GetDlgItemText(hwnd, IDC_PP_OTHER_FUEL, buffer, sizeof(buffer) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_PP_OTHER_FUEL) && 	(Instr(1, buffer, "(B)")))
			strcpy(buying1, "Fuel Ore");
		GetDlgItemText(hwnd, IDC_PP_OTHER_ORG, buffer, sizeof(buffer) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_PP_OTHER_ORG) && (Instr(1, buffer, "(B)")))
			strcpy(buying1, "Organics");
		GetDlgItemText(hwnd, IDC_PP_OTHER_EQUIP, buffer, sizeof(buffer) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_PP_OTHER_EQUIP) && (Instr(1, buffer, "(B)")))
			strcpy(buying1, "Equipment");

		if (strcmp (buying1, "")==0)
		{
			if (IsDlgButtonChecked(hwnd, IDC_PP_OTHER_FUEL))
				strcpy(buying1, "Fuel Ore");
			if (IsDlgButtonChecked(hwnd, IDC_PP_OTHER_ORG))
				strcpy(buying1, "Organics");
			if (IsDlgButtonChecked(hwnd, IDC_PP_OTHER_EQUIP))
				strcpy(buying1, "Equipment");
		}
		if (strcmp (buying2, "")==0)
		{
			if (IsDlgButtonChecked(hwnd, IDC_PP_CURRENT_FUEL))
				strcpy(buying2, "Fuel Ore");
			if (IsDlgButtonChecked(hwnd, IDC_PP_CURRENT_ORG))
				strcpy(buying2, "Organics");
			if (IsDlgButtonChecked(hwnd, IDC_PP_CURRENT_EQUIP))
				strcpy(buying2, "Equipment");
		}

  	    ok = GetDlgItemText(hwnd, IDC_PP_CYCLES, buffer, sizeof(buffer) - 1);
		strcpy(cycles, buffer);

		if (IsDlgButtonChecked(hwnd, IDC_PP_FORCE_HAGGLE))
		{
			strcpy(forcenohaggle, "0");
			header.forceNoHaggle=0;
		}
		else
		{
			strcpy(forcenohaggle, "1");
			header.forceNoHaggle=1;
		}

		strcpy (rexxArgs[0], sector1);
		strcpy (rexxArgs[1], buying1);
		strcpy (rexxArgs[2], buying2);
		strcpy (rexxArgs[3], cycles);
		strcpy (rexxArgs[4], forcenohaggle);
		sprintf (buffer, "%d", header.minPairPortPercent);
		strcpy (rexxArgs[5], buffer);

		strcpy (fullFilePath, scriptDir);
		strcat (fullFilePath, "StandardPairPort.erx");
		REXX_Start(fullFilePath, 1, 6, rexxArgs);

		showpairport = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
//		ShowWindow(showpairport,SW_HIDE);
	    return 0;
	  case IDCANCEL:
		showpairport = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
//		ShowWindow(showpairport,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showpairport = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
//	ShowWindow(showpairport,SW_HIDE);
	return 0;
    }
    return 0;
}


static int CALLBACK StealMoveProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char sector1[30];
	char sector2[30];
	char cargo1[30];
	char cargo2[30];
	char planet1[30];
	char planet2[30];
	char type1[30];
	char type2[30];
	char cycles[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:

		GetDlgItemText(hwnd, IDC_STEALMOVE_SECTOR1, sector1, sizeof(sector1) - 1);
		GetDlgItemText(hwnd, IDC_STEALMOVE_CARGO1, cargo1, sizeof(cargo1) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_STEALMOVE_SELL1))
			strcpy(type1,"SELL");
		if (IsDlgButtonChecked(hwnd, IDC_STEALMOVE_DUMP1))
			strcpy(type1,"DUMP");
		GetDlgItemText(hwnd, IDC_STEALMOVE_PLANET1, planet1, sizeof(planet1) - 1);
		  
		GetDlgItemText(hwnd, IDC_STEALMOVE_SECTOR2, sector2, sizeof(sector2) - 1);
		GetDlgItemText(hwnd, IDC_STEALMOVE_CARGO2, cargo2, sizeof(cargo2) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_STEALMOVE_SELL2))
			strcpy(type2,"SELL");
		if (IsDlgButtonChecked(hwnd, IDC_STEALMOVE_DUMP2))
			strcpy(type2,"DUMP");
		GetDlgItemText(hwnd, IDC_STEALMOVE_PLANET2, planet2, sizeof(planet2) - 1);

		GetDlgItemText(hwnd, IDC_STEALMOVE_CYCLES, cycles, sizeof(cycles) - 1);


		strcpy (rexxArgs[0], sector1);
		strcpy (rexxArgs[1], cargo1);
		strcpy (rexxArgs[2], type1);
		strcpy (rexxArgs[3], planet1);
		strcpy (rexxArgs[4], sector2);
		strcpy (rexxArgs[5], cargo2);
		strcpy (rexxArgs[6], type2);
		strcpy (rexxArgs[7], planet2);
		strcpy (rexxArgs[8], cycles);
		sprintf (rexxArgs[9], "%d", header.stealFactor);
/*
		printf ("sector1: %s\n", rexxArgs[0]);
		printf ("cargo1: %s\n", rexxArgs[1]);
		printf ("type1: %s\n", rexxArgs[2]);
		printf ("planet1: %s\n", rexxArgs[3]);
		printf ("sector2: %s\n", rexxArgs[4]);
		printf ("cargo2: %s\n", rexxArgs[5]);
		printf ("type2: %s\n", rexxArgs[6]);
		printf ("planet2: %s\n", rexxArgs[7]);
		printf ("cycles: %s\n", rexxArgs[8]);
		printf ("stealFactor: %s\n", rexxArgs[9]);
	*/
		strcpy (fullFilePath, scriptDir);
		strcat (fullFilePath, "StandardSellMove.erx");
		REXX_Start(fullFilePath, 1, 10, rexxArgs);

//		showstealmove = NULL;

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		ShowWindow(showstealmove,SW_HIDE);
	    return 0;
	  case IDCANCEL:
//		showstealmove = NULL;

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		ShowWindow(showstealmove,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showstealmove = NULL;

//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showstealmove,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK ShowDeadEndProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	NMHDR FAR *tem;
	int num;
	HANDLE hlist;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;

	
    switch (msg) {
      case WM_INITDIALOG:
	return 1;

      case WM_NOTIFY:
		if ((int)wParam == IDC_DEADEND_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_DEADEND_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Dead End Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }

		tem=(NMHDR FAR *)lParam;
		if (tem->code== TCN_SELCHANGE)
		{
			num=TabCtrl_GetCurSel(tem->hwndFrom);
			hlist = GetDlgItem(hwnd, IDC_DEADEND_LIST);
			showDeadEndData(hlist, num+1);
		}
		break;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	
	  case IDOK:
		showdeadends = NULL;
			

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showdeadends = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showdeadends = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


static int CALLBACK EtherExploreProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    OPENFILENAME of;
	char filename[1024];	
	char rexxArgs[100][100];
	char options[20];
	char file[1024];
	char class0[30];
	char class9[30];
	char beacons[30];
	char fighters[30];
	char mines[30];
	char navhaz[30];
	char planets[30];
	char ships[30];
	char traders[30];
	char cycles[10];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:

		  //tmp zev 
		strcpy (cycles, "9999");
		strcpy (file, "");
		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_UNEXPLORED))
			strcpy(options,"UNEXPLORED");
		else if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_RANDOM))
			strcpy(options,"RANDOM");
		else if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_FILE))
		{
			strcpy(options,"FILE");
			GetDlgItemText(hwnd, IDC_ETHERPROBE_FILE_EDIT, file, sizeof(file) - 1);
		}

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_CLASS0_CHECK))
			strcpy(class0,"YES");
		else
			strcpy(class0,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_CLASS9_CHECK))
			strcpy(class9,"YES");
		else
			strcpy(class9,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_BEACON_CHECK))
			strcpy(beacons,"YES");
		else
			strcpy(beacons,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_FIGHTERS_CHECK))
			strcpy(fighters,"YES");
		else
			strcpy(fighters,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_MINES_CHECK))
			strcpy(mines,"YES");
		else
			strcpy(mines,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_NAVHAZ_CHECK))
			strcpy(navhaz,"YES");
		else
			strcpy(navhaz,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_PLANETS_CHECK))
			strcpy(planets,"YES");
		else
			strcpy(planets,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_SHIPS_CHECK))
			strcpy(ships,"YES");
		else
			strcpy(ships,"NO");

		if (IsDlgButtonChecked(hwnd, IDC_ETHERPROBE_TRADERS_CHECK))
			strcpy(traders,"YES");
		else
			strcpy(traders,"NO");

		strcpy (rexxArgs[0], options);
		strcpy (rexxArgs[1], file);
		strcpy (rexxArgs[2], class0);
		strcpy (rexxArgs[3], class9);
		strcpy (rexxArgs[4], beacons);
		strcpy (rexxArgs[5], fighters);
		strcpy (rexxArgs[6], mines);
		strcpy (rexxArgs[7], navhaz);
		strcpy (rexxArgs[8], planets);
		strcpy (rexxArgs[9], ships);
		strcpy (rexxArgs[10], traders);
		strcpy (rexxArgs[11], cycles);
/*
		printf ("options: %s\n", rexxArgs[0]);
		printf ("file: %s\n", rexxArgs[1]);
		printf ("class0: %s\n", rexxArgs[2]);
		printf ("class9: %s\n", rexxArgs[3]);
		printf ("beacons: %s\n", rexxArgs[4]);
		printf ("fighters: %s\n", rexxArgs[5]);
		printf ("mines: %s\n", rexxArgs[6]);
		printf ("navhaz: %s\n", rexxArgs[7]);
		printf ("planets: %s\n", rexxArgs[8]);
		printf ("ships: %s\n", rexxArgs[9]);
		printf ("traders: %s\n", rexxArgs[10]);
		printf ("cycles: %s\n", rexxArgs[11]);
	*/
		strcpy (fullFilePath, scriptDir);
		strcat (fullFilePath, "StandardEtherProbe.erx");
		REXX_Start(fullFilePath, 1, 12, rexxArgs);

		
//		showetherexplore = NULL;

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		ShowWindow(showetherexplore,SW_HIDE);
	    return 0;
	  case IDC_ETHERPROBE_BROWSE:
		memset(&of, 0, sizeof(of));
#ifdef OPENFILENAME_SIZE_VERSION_400
		of.lStructSize = OPENFILENAME_SIZE_VERSION_400;
#else
		of.lStructSize = sizeof(of);
#endif
		of.hwndOwner = hwnd;
		of.lpstrFilter = "Text and Log files\0*.txt;*.log\0All Files\0*\0\0\0";
		of.lpstrCustomFilter = NULL;
		of.nFilterIndex = 1;
		strcpy (filename, "");
		of.lpstrFile = filename;
		of.nMaxFile = sizeof(filename);
		of.lpstrFileTitle = NULL;
		of.lpstrInitialDir = NULL;
		of.lpstrTitle = "Select File";
		of.Flags = 0;
		if (GetOpenFileName(&of)) {
			SetDlgItemText(hwnd, IDC_ETHERPROBE_FILE_EDIT, filename);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_UNEXPLORED, BST_UNCHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_RANDOM, BST_UNCHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_FILE, BST_CHECKED);
		}		
		return 0;
	  case IDCANCEL:
//		showetherexplore = NULL;

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		  ShowWindow(showetherexplore,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showetherexplore = NULL;

//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
		  ShowWindow(showetherexplore,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK EtherExploreLogProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char thedate[50];
	char option[50];
	int index, retVal=0;
	char name[30], buffer[50];

	strcpy (name, "ether_analysis.txt");

	strcpy (option, "Everything");
	strcpy (thedate, "Everything");

    switch (msg) {
      case WM_INITDIALOG:
		  loadEtherLog(hwnd, option, thedate);
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
      case IDC_ETHERPROBE_DATE:
		if (HIWORD(wParam) == CBN_SELCHANGE) 
		{
		    index = SendDlgItemMessage(hwnd, IDC_ETHERPROBE_DATE, CB_GETCURSEL, 0, 0);
		    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_DATE, CB_GETLBTEXT, index, (LPARAM)thedate);
		    index = SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_FILTERLIST, CB_GETCURSEL, 0, 0);
		    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_FILTERLIST, CB_GETLBTEXT, index, (LPARAM)option);
			loadEtherLog(hwnd, option, thedate);
		}
		return 0;
      case IDC_ETHERPROBE_LOG_FILTERLIST:
		if (HIWORD(wParam) == CBN_SELCHANGE) 
		{
		    index = SendDlgItemMessage(hwnd, IDC_ETHERPROBE_DATE, CB_GETCURSEL, 0, 0);
		    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_DATE, CB_GETLBTEXT, index, (LPARAM)thedate);

			index = SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_FILTERLIST, CB_GETCURSEL, 0, 0);
		    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_FILTERLIST, CB_GETLBTEXT, index, (LPARAM)option);

			loadEtherLog(hwnd, option, thedate);
		}
		return 0;
	  case IDC_ETHERPROBE_CLEAR:
		if (MessageBox(hwnd,
						"Are you sure you to clear the ether probe log?",
						"ATTAC Exit Confirmation",
						MB_ICONWARNING | MB_OKCANCEL) == IDOK)
		{
			retVal = unlink (name);

			if (retVal==0)
			{
				sprintf (buffer, "Ether Probe Log %s deleted", name);
				loadEtherLog(hwnd, option, thedate);
			}
			else
				sprintf (buffer, "Error Deleting Ether Probe Log %s", name);
				attacErrorMessage(buffer);		
		}
		return 0;
	  case IDOK:
		showetherexplorelog = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showetherexplorelog = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showetherexplorelog = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

void loadEtherLog(HWND hwnd, char option[], char thedate[])
{
	FILE *fp;
	char c;
	char line[MAX_LENGTH];
	int lineCount=0;
	int foundDate=0;

	SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_RESETCONTENT, 0, 0);

	if (strcmp (thedate, "Everything")==0)
		foundDate=2;
	else
		foundDate=0;

	fp = fopen ("ether_analysis.txt", "r");
	if (fp==NULL)
	{
		SetDlgItemText(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, "No data available");
	}
	else
	{	
		c = fgetc(fp);

		while( !feof(fp)) 
		{
			if (c != '\n')
				line[lineCount++]=c;							
			else
			{
				line[lineCount]=0;

				if (Instr(1, line, "TIME:")>0 && foundDate==1)
					return;
				else if (Instr(1, line, "TIME:")>0 && strcmp (thedate, line)==0)
				{
					if (foundDate==0)
						foundDate=1;
					else if (foundDate==1)
						return;
				}
				else
				{
					if (foundDate>=1)
					{
						if (strcmp (option, "Everything")==0 && Instr(1, line, "TIME") ==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Info")>0 && strcmp (option, "Info")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Class 0 Port")>0 && strcmp (option, "Class 0 Port")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Beacon")>0 && strcmp (option, "Beacon")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Fighters")>0 && strcmp (option, "Fighters")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Armid Mines")>0 && strcmp (option, "Armid Mines")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "NavHaz")>0 && strcmp (option, "NavHaz")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Planets")>0 && strcmp (option, "Planets")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Unmanned Ship")>0 && strcmp (option, "Unmanned Ship")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Traders")>0 && strcmp (option, "Traders")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Class 9 Port")>0 && strcmp (option, "Class 9 Port")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
						else if (Instr (1, line, "Attacked")>0 && strcmp (option, "Attacked")==0)
						    SendDlgItemMessage(hwnd, IDC_ETHERPROBE_LOG_LIST, LB_ADDSTRING, 0, (LPARAM) line);
					}
				}


				strcpy(line, "");
				lineCount=0;
			}
			c = fgetc(fp);
		}
		fclose(fp);
	}


}

static int CALLBACK ShowBlockedPortProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	HWND hlist;

	
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_BLOCKED_PORTS_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_BLOCKED_PORTS_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Blocked Port Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;

      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		showblockedport = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showblockedport = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showblockedport = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK ShowFighterProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	HWND hlist;

	 
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_FIGHTERS_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_FIGHTERS_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Fighter Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;

      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		showfighter = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showfighter = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showfighter = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


static int CALLBACK BustListProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char days[30];
	char sector[30];
	HANDLE hlist;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;


    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_BUSTLIST_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_BUSTLIST_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Bust List Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;

      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_BUSTLIST_APPLY:
		GetDlgItemText(hwnd, IDC_BUSTLIST_DAYS, days, sizeof(days) - 1);
		GetDlgItemText(hwnd, IDC_BUSTLIST_SECTOR, sector, sizeof(sector) - 1);

		hlist = GetDlgItem(showbustlist, IDC_BUSTLIST_LIST);
		findBusts(hlist, atol(days), atol(sector));
	    return 0;
	  case IDC_BUSTLIST_ADD:
		  showaddbustform(hwnd);
		  return 0;
	  case IDCANCEL:
		showbustlist = NULL;
		showaddbust = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showbustlist = NULL;
	showaddbust = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK StealTransportProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char ship1[30];
	char ship2[30];
	char ship3[30];
	char ship4[30];
	char password1[30];
	char password2[30];
	char password3[30];
	char password4[30];
	char type1[30];
	char type2[30];
	char type3[30];
	char type4[30];
	char planet1[30];
	char planet2[30];
	char planet3[30];
	char planet4[30];
	char cycles[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:

		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_SHIP1, ship1, sizeof(ship1) - 1);
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PASS1, password1, sizeof(password1) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_SELL1))
			strcpy(type1,"SELL");
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_DUMP1))
			strcpy(type1,"DUMP");
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PLANET1, planet1, sizeof(planet1) - 1);
		  
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_SHIP2, ship2, sizeof(ship2) - 1);
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PASS2, password2, sizeof(password2) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_SELL2))
			strcpy(type2,"SELL");
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_DUMP2))
			strcpy(type2,"DUMP");
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PLANET2, planet2, sizeof(planet2) - 1);

		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_SHIP3, ship3, sizeof(ship3) - 1);
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PASS3, password3, sizeof(password3) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_SELL3))
			strcpy(type3,"SELL");
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_DUMP3))
			strcpy(type3,"DUMP");
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PLANET3, planet3, sizeof(planet3) - 1);

		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_SHIP4, ship4, sizeof(ship4) - 1);
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PASS4, password4, sizeof(password4) - 1);
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_SELL4))
			strcpy(type4,"SELL");
		if (IsDlgButtonChecked(hwnd, IDC_STEALTRANSPORT_DUMP4))
			strcpy(type4,"DUMP");
		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_PLANET4, planet4, sizeof(planet4) - 1);

		GetDlgItemText(hwnd, IDC_STEALTRANSPORT_CYCLES, cycles, sizeof(cycles) - 1);

	

		strcpy (rexxArgs[0], ship1);
		strcpy (rexxArgs[1], password1);
		strcpy (rexxArgs[2], type1);
		strcpy (rexxArgs[3], planet1);
		strcpy (rexxArgs[4], ship2);
		strcpy (rexxArgs[5], password2);
		strcpy (rexxArgs[6], type2);
		strcpy (rexxArgs[7], planet2);
		strcpy (rexxArgs[8], ship3);
		strcpy (rexxArgs[9], password3);
		strcpy (rexxArgs[10], type3);
		strcpy (rexxArgs[11], planet3);
		strcpy (rexxArgs[12], ship4);
		strcpy (rexxArgs[13], password4);
		strcpy (rexxArgs[14], type4);
		strcpy (rexxArgs[15], planet4);
		strcpy (rexxArgs[16], cycles);
		strcpy (rexxArgs[17], "3");
		sprintf (rexxArgs[18], "%d", header.stealFactor);
/*
		printf ("ship1: %s\n", rexxArgs[0]);
		printf ("password1: %s\n", rexxArgs[1]);
		printf ("type1: %s\n", rexxArgs[2]);
		printf ("planet1: %s\n", rexxArgs[3]);
		printf ("ship2: %s\n", rexxArgs[4]);
		printf ("password2: %s\n", rexxArgs[5]);
		printf ("type2: %s\n", rexxArgs[6]);
		printf ("planet2: %s\n", rexxArgs[7]);
		printf ("ship3: %s\n", rexxArgs[8]);
		printf ("password3: %s\n", rexxArgs[9]);
		printf ("type3: %s\n", rexxArgs[10]);
		printf ("planet3: %s\n", rexxArgs[11]);
		printf ("ship4: %s\n", rexxArgs[12]);
		printf ("password4: %s\n", rexxArgs[13]);
		printf ("type4: %s\n", rexxArgs[14]);
		printf ("planet4: %s\n", rexxArgs[15]);
		printf ("cycles: %s\n", rexxArgs[16]);
		printf ("stealing: %s\n", rexxArgs[17]);
		printf ("stealFactor: %s\n", rexxArgs[18]);
	*/
		strcpy (fullFilePath, scriptDir);
		strcat (fullFilePath, "StandardSellTransport.erx");
		REXX_Start(fullFilePath, 1, 19, rexxArgs);

//		showstealtransport = NULL;

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		ShowWindow(showstealtransport,SW_HIDE);
	    return 0;
	  case IDCANCEL:
//		showstealtransport = NULL;

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		ShowWindow(showstealtransport,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showstealtransport = NULL;

//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showstealtransport,SW_HIDE);
	return 0;
    }
    return 0;
}


static int CALLBACK ZTMProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		forwardZTM=0;
		backwardZTM=0;
		oneWayZTM=0;
		randomZTM=0;

		  currentZTM=0;
		if (IsDlgButtonChecked(hwnd, IDC_ZTM_FORWARD_CHECK))
			forwardZTM=1;
		if (IsDlgButtonChecked(hwnd, IDC_ZTM_BACKWARD_CHECK))
			backwardZTM=1;
		if (IsDlgButtonChecked(hwnd, IDC_ZTM_ONEWAY_CHECK))
			oneWayZTM=1;
		if (IsDlgButtonChecked(hwnd, IDC_ZTM_RANDOM_CHECK))
			randomZTM=1;

		startZTM();

//		showztm = NULL;

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
	    return 0;
	  case IDC_ZTM_RESET:
		if (MessageBox(hwnd,
						"Are you sure you want to reset the ZTM?",
						"ATTAC Exit Confirmation",
						MB_ICONWARNING | MB_OKCANCEL) == IDOK)
		{
			header.lastZTMFromOneTillEnd=1;
			header.lastZTMFromEndTillOne=1;
			writeMemory();
		}
		return 0;
	  case IDCANCEL:
		//showztm = NULL;
		if (parseZTMData!=0)
			attac_send("qq");
		parseZTMData=0;
	//	SetActiveWindow(GetParent(hwnd));
	//	DestroyWindow(hwnd);
		ShowWindow(showztm,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
		//showztm = NULL;
		if (parseZTMData!=0)
			attac_send("qq");
		parseZTMData=0;
		//SetActiveWindow(GetParent(hwnd));
		//DestroyWindow(hwnd);
		ShowWindow(showztm,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK PayTollsProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		  
		if (IsDlgButtonChecked(hwnd, IDC_TOLL_OPTIONS_TOLL_PAY))
			header.tollTolls=TOLLSFIGPAY;

		if (IsDlgButtonChecked(hwnd, IDC_TOLL_OPTIONS_TOLL_KILL))
			header.tollTolls= TOLLSFIGKILL;

		if (IsDlgButtonChecked(hwnd, IDC_TOLL_OPTIONS_TOLL_NOTHING))
			header.tollTolls= TOLLSFIGNOTHING;

		if (IsDlgButtonChecked(hwnd, IDC_TOLL_OPTIONS_DEFENSIVE_KILL))
			header.defensiveTolls= DEFENSIVEFIGKILL;

		if (IsDlgButtonChecked(hwnd, IDC_TOLL_OPTIONS_DEFENSIVE_NOTHING))
			header.defensiveTolls= DEFENSIVEFIGNOTHING;


		writeMemory();
		showpaytolls = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showpaytolls = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showpaytolls = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK QuasarCalculatorProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	int MBBSIndex;
	char firstBlast[50];
	char secondBlast[50];
	char currentBlast[100];
	char dataBuffer[MAX_LENGTH*2];
	int isSectorOrAtmos=0;
	float MBBSMultiple=0.0;
	int quasarSetting=0;
	long nextBlast;
	long fuelLeft=0;
	int i;
	char buffer[50];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
			MBBSIndex = SendDlgItemMessage(hwnd, IDC_QUASAR_CALC_SETTING, CB_GETCURSEL, 0, 0);
			
			GetDlgItemText(hwnd, IDC_QUASAR_CALC_FIRSTBLAST, firstBlast, sizeof(firstBlast) - 1);
			GetDlgItemText(hwnd, IDC_QUASAR_CALC_SECONDBLAST, secondBlast, sizeof(secondBlast) - 1);

			if (IsDlgButtonChecked(hwnd, IDC_QUASAR_CALC_SECTOR))
				isSectorOrAtmos=0;
			if (IsDlgButtonChecked(hwnd, IDC_QUASAR_CALC_ATMOSPHERE))
				isSectorOrAtmos=1;

			if (MBBSIndex==-1)
				MBBSIndex=1;

			strcpy (dataBuffer, "");

			/* check if 2nd blast > 1st blast */
			if (atol(secondBlast) > atol(firstBlast))
				attacErrorMessage("The Second Blast must be less than the first");
			else
			{
				/* set MBBS mode multiple */
				if (MBBSIndex==1)
					MBBSMultiple=2.0;
				else
					MBBSMultiple=0.5;

				for (i=0; i< 10; i++)
				{
					/* dont print the labels if user enters no data */
					if (strcmp(firstBlast, "")!=0 && strcmp(secondBlast, "")!=0)
					{
						if (isSectorOrAtmos==0) /* if it is a sector blast */
						{
							quasarSetting =  (int) (100 - ((double)atol(secondBlast)/(double) atol(firstBlast) * 100));
							nextBlast = (long) ((double) atol(secondBlast) * ((double)(100-quasarSetting)/(double)100));
							fuelLeft = (long) ((double)(nextBlast*3)/(double)((double)quasarSetting/(double)100));
						}
						else  /* it is an atmospheric blast */
						{
							quasarSetting =  (int) (100 - ((double)atol(secondBlast)/(double) atol(firstBlast) * 100));
							nextBlast = (long) ((double) atol(secondBlast) * ((double)(100-quasarSetting)/(double)100));
							fuelLeft = (long) ((double)(nextBlast*MBBSMultiple)/(double)((double)quasarSetting/(double)100));
						}

						if (strcmp(dataBuffer, "")==0)
							sprintf (dataBuffer, "Quasar Setting: %d%%\n", quasarSetting);

						sprintf (currentBlast, "Blast #%d: %ld  --  Fuel Left: %ld\n", i+1, nextBlast, fuelLeft);
						strcat (dataBuffer, currentBlast);

						strcpy (firstBlast, secondBlast);

						sprintf (buffer, "%ld", nextBlast);
						strcpy (secondBlast, buffer);
						SetDlgItemText(hwnd, IDC_QUASAR_CALC_DATA, dataBuffer);
					}
	
				}  
			}  /* end else 2nd blast > 1st blast */

			return 0;
	  case IDCANCEL:
		showquasarcalculator = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showquasarcalculator = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK CIMOptionsProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		  
		GetCIMData = 0;

		if (IsDlgButtonChecked(hwnd, IDC_CIM_SECTOR))
			GetCIMData += 1;

		if (IsDlgButtonChecked(hwnd, IDC_CIM_PORT))
			GetCIMData += 2;

		if (GetCIMData!= 0)
			startCIM();

		showcimoptions = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	  case IDCANCEL:
		showcimoptions = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showcimoptions = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


static int CALLBACK MaxPortsProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char amount[30];
	char type[30];
	int index;
	HANDLE hlist;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;

	
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_MAXEDPORT_LIST) 
			{
				hlist = GetDlgItem(hwnd, IDC_MAXEDPORT_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Max Port Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;

      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_MAXEDPORT_AMOUNT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			hlist = GetDlgItem(hwnd, IDC_MAXEDPORT_LIST);

		    ok = GetDlgItemText(hwnd, IDC_MAXEDPORT_AMOUNT, amount, sizeof(amount) - 1);
  			index = SendDlgItemMessage(hwnd, IDC_MAXEDPORT_TYPE, CB_GETCURSEL, 0, 0);
			//set default to equipment
			if (index==-1)
				index=2;
			generate_maxed_ports(hlist, atol(amount), index);
		}
		return 0;
	  case IDC_MAXEDPORT_TYPE:
		if (HIWORD(wParam) == CBN_SELCHANGE) 
		{
			hlist = GetDlgItem(hwnd, IDC_MAXEDPORT_LIST);
		    index = SendDlgItemMessage(hwnd, IDC_MAXEDPORT_TYPE, CB_GETCURSEL, 0, 0);
			ok = GetDlgItemText(hwnd, IDC_MAXEDPORT_AMOUNT, amount, sizeof(amount) - 1);
			if (index==-1)
				index=2;
			generate_maxed_ports(hlist, atol(amount), index);
		}
		return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_MAXEDPORT_AMOUNT, amount, sizeof(amount) - 1);
		ok = GetDlgItemText(hwnd, IDC_MAXEDPORT_TYPE, type, sizeof(type) - 1);

	    return 0;
	  case IDCANCEL:
		showmaxports = NULL;
		//say we closed out the zoc ask

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showmaxports = NULL;
	//say we closed out the zoc ask
	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


static int CALLBACK SellPlanetToPortProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char sector[30];
	char cycles[30];
	char portsector[30];
	char takefuel[30];
	char fuelamount[30];
	char leavecargo[30];
	char resourceToLeave[30];
	char wayToTerra[30];
	char wayFromTerra[30];
	char haggleType[30];
	char rexxArgs[100][100];
	char buffer[30];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_SELLPLANETTOPORT_LEAVECARGO_EDIT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_LEAVECARGO_EDIT, leavecargo, sizeof(leavecargo) - 1);
			ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_TAKEFUEL_EDIT, takefuel, sizeof(takefuel) - 1);
			SetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_TAKEFUEL_EDIT, leavecargo);
		}
		return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_SECTOR_EDIT, sector, sizeof(sector) - 1);
		ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_CYCLES_EDIT, cycles, sizeof(cycles) - 1);
		ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_PORT_EDIT, portsector, sizeof(portsector) - 1);
		ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_LEAVECARGO_EDIT, leavecargo, sizeof(leavecargo) - 1);
		ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_TAKEFUEL_EDIT, takefuel, sizeof(takefuel) - 1);
		ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_FUELAMOUNT_EDIT, fuelamount, sizeof(fuelamount) - 1);
		ok = GetDlgItemText(hwnd, IDC_SELLPLANETTOPORT_HAGGLE, haggleType, sizeof(haggleType) - 1);
		
		if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_FUEL_RADIO))
			strcpy (resourceToLeave, "Fuel Ore");
		else if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_ORG_RADIO))
			strcpy (resourceToLeave, "Organics");
		else if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_EQUIP_RADIO))
			strcpy (resourceToLeave, "Equipment");
		
		strcpy (wayToTerra, "");
		if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_TOTWARP_RADIO))
			strcpy(wayToTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_TOPWARP_RADIO))
			strcpy(wayToTerra, "Planetary Transport");
		else if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_TOWARP_RADIO))
			strcpy(wayToTerra, "Regular Warping");

		if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_FROMTWARP_RADIO))
			strcpy(wayFromTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_SELLPLANETTOPORT_FROMWARP_RADIO))
			strcpy(wayFromTerra, "Regular Warping");

		if (atol(sector)<=0 || atol(sector)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Sector", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(portsector)<=0)
		{
			MessageBox(NULL, "Invalid Port Sector", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(takefuel)<=0)
		{
			MessageBox(NULL, "Invalid Planet to Take Fuel", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(fuelamount)<=0 && strcmp (fuelamount, "AUTO") != 0)
		{
			MessageBox(NULL, "Invalid Amount of Fuel to Take", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(leavecargo)<0)
		{
			MessageBox(NULL, "Invalid Place to leave cargo", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (strcmp (haggleType, "Worst Price")!=0 &&
				 strcmp (haggleType, "Asking Price")!=0 &&
				 strcmp (haggleType, "Best Price")!=0)
		{
			MessageBox(NULL, "Invalid Haggle Type", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}

		else
		{
			strcpy (rexxArgs[0], sector);
			strcpy (rexxArgs[1], portsector);
			strcpy (rexxArgs[2], cycles);
			strcpy (rexxArgs[3], leavecargo);
			strcpy (rexxArgs[4], resourceToLeave);
			strcpy (rexxArgs[5], wayToTerra);
			strcpy (rexxArgs[6], wayFromTerra);
			strcpy (rexxArgs[7], fuelamount);
			strcpy (rexxArgs[8], takefuel);
			sprintf (buffer, "%d", header.worstFactor);
			strcpy (rexxArgs[9], buffer);
			strcpy (rexxArgs[10], haggleType);
/*
			printf ("sector: %s\n", rexxArgs[0]);
			printf ("portsector: %s\n", rexxArgs[1]);
			printf ("cycles: %s\n", rexxArgs[2]);
			printf ("leavecargo: %s\n", rexxArgs[3]);
			printf ("resourceToLeave: %s\n", rexxArgs[4]);
			printf ("wayToTerra: %s\n", rexxArgs[5]);
			printf ("wayFromTerra: %s\n", rexxArgs[6]);
			printf ("fuelamount: %s\n", rexxArgs[7]);
			printf ("takefuel: %s\n", rexxArgs[8]);
			printf ("worstFactor: %s\n", rexxArgs[9]);
*/

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardDumpToPort.erx");
			REXX_Start(fullFilePath, 1, 11, rexxArgs);
			ShowWindow(showsellplanettoport,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
			ShowWindow(showsellplanettoport,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
		ShowWindow(showsellplanettoport,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK BuyDumpProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char sector[30];
	char cycles[30];
	char portsector[30];
	char takefuel[30];
	char fuelamount[30];
	char leavecargo[30];
	char resourceToLeave[30];
	char wayToTerra[30];
	char wayFromTerra[30];
	char haggleType[30];
	char rexxArgs[100][100];
	char buffer[30];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_BUYDUMP_LEAVECARGO_EDIT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		    ok = GetDlgItemText(hwnd, IDC_BUYDUMP_LEAVECARGO_EDIT, leavecargo, sizeof(leavecargo) - 1);
			ok = GetDlgItemText(hwnd, IDC_BUYDUMP_TAKEFUEL_EDIT, takefuel, sizeof(takefuel) - 1);
			SetDlgItemText(hwnd, IDC_BUYDUMP_TAKEFUEL_EDIT, leavecargo);
		}
		return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_BUYDUMP_SECTOR_EDIT, sector, sizeof(sector) - 1);
		ok = GetDlgItemText(hwnd, IDC_BUYDUMP_CYCLES_EDIT, cycles, sizeof(cycles) - 1);
		ok = GetDlgItemText(hwnd, IDC_BUYDUMP_PORT_EDIT, portsector, sizeof(portsector) - 1);
		ok = GetDlgItemText(hwnd, IDC_BUYDUMP_LEAVECARGO_EDIT, leavecargo, sizeof(leavecargo) - 1);
		ok = GetDlgItemText(hwnd, IDC_BUYDUMP_TAKEFUEL_EDIT, takefuel, sizeof(takefuel) - 1);
		ok = GetDlgItemText(hwnd, IDC_BUYDUMP_FUELAMOUNT_EDIT, fuelamount, sizeof(fuelamount) - 1);
		ok = GetDlgItemText(hwnd, IDC_BUYDUMP_HAGGLE, haggleType, sizeof(haggleType) - 1);
		
		if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_FUEL_RADIO))
			strcpy (resourceToLeave, "Fuel Ore");
		else if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_ORG_RADIO))
			strcpy (resourceToLeave, "Organics");
		else if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_EQUIP_RADIO))
			strcpy (resourceToLeave, "Equipment");
		
		strcpy (wayToTerra, "");
		if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_TOTWARP_RADIO))
			strcpy(wayToTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_TOPWARP_RADIO))
			strcpy(wayToTerra, "Planetary Transport");
		else if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_TOWARP_RADIO))
			strcpy(wayToTerra, "Regular Warping");

		if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_FROMTWARP_RADIO))
			strcpy(wayFromTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_BUYDUMP_FROMWARP_RADIO))
			strcpy(wayFromTerra, "Regular Warping");

		if (atol(sector)<=0 || atol(sector)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Sector", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(portsector)<=0)
		{
			MessageBox(NULL, "Invalid Port Sector", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(takefuel)<=0)
		{
			MessageBox(NULL, "Invalid Planet to Take Fuel", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(fuelamount)<=0 && strcmp (fuelamount, "AUTO") != 0)
		{
			MessageBox(NULL, "Invalid Amount of Fuel to Take", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(leavecargo)<0)
		{
			MessageBox(NULL, "Invalid Place to leave cargo", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (strcmp (haggleType, "Worst Price")!=0 &&
				 strcmp (haggleType, "Asking Price")!=0 &&
				 strcmp (haggleType, "Best Price")!=0)
		{
			MessageBox(NULL, "Invalid Haggle Type", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}

		else
		{
			strcpy (rexxArgs[0], sector);
			strcpy (rexxArgs[1], portsector);
			strcpy (rexxArgs[2], cycles);
			strcpy (rexxArgs[3], leavecargo);
			strcpy (rexxArgs[4], resourceToLeave);
			strcpy (rexxArgs[5], wayToTerra);
			strcpy (rexxArgs[6], wayFromTerra);
			strcpy (rexxArgs[7], fuelamount);
			strcpy (rexxArgs[8], takefuel);
			sprintf (buffer, "%d", header.worstFactor);
			strcpy (rexxArgs[9], buffer);
			strcpy (rexxArgs[10], haggleType);
			sprintf (buffer, "%d", header.minPairPortPercent);
			strcpy (rexxArgs[11], buffer);
/*
			printf ("sector: %s\n", rexxArgs[0]);
			printf ("portsector: %s\n", rexxArgs[1]);
			printf ("cycles: %s\n", rexxArgs[2]);
			printf ("leavecargo: %s\n", rexxArgs[3]);
			printf ("resourceToLeave: %s\n", rexxArgs[4]);
			printf ("wayToTerra: %s\n", rexxArgs[5]);
			printf ("wayFromTerra: %s\n", rexxArgs[6]);
			printf ("fuelamount: %s\n", rexxArgs[7]);
			printf ("takefuel: %s\n", rexxArgs[8]);
			printf ("worstFactor: %s\n", rexxArgs[9]);
*/

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardBuyDump.erx");
			REXX_Start(fullFilePath, 1, 12, rexxArgs);
	//			showbuydump = NULL;
	//		SetActiveWindow(GetParent(hwnd));
	//		DestroyWindow(hwnd);
			ShowWindow(showbuydump,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
	//	showbuydump = NULL;
		//say we closed out the zoc ask

	//	SetActiveWindow(GetParent(hwnd));
	//	DestroyWindow(hwnd);
		ShowWindow(showbuydump,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showbuydump = NULL;
	//say we closed out the zoc ask
//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showbuydump,SW_HIDE);
	return 0;
    }
    return 0;
}


static int CALLBACK BuyPlanetShieldsProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char sector[30];
	char portsector[30];
	char takefuel[30];
	char fuelamount[30];
	char leaveshields[30];
	char numberOfShields[30];
	char wayToTerra[30];
	char wayFromTerra[30];
	char cycles[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_PLANETSHIELDS_LEAVESHIELDS:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_LEAVESHIELDS, leaveshields, sizeof(leaveshields) - 1);
			SetDlgItemText(hwnd, IDC_PLANETSHIELDS_TAKEFUEL, leaveshields);
		}
		return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_PLANETSECTOR, sector, sizeof(sector) - 1);
		ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_CYCLES, cycles, sizeof(cycles) - 1);
		ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_PORTSECTOR, portsector, sizeof(portsector) - 1);
		ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_LEAVESHIELDS, leaveshields, sizeof(leaveshields) - 1);
		ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_TAKEFUEL, takefuel, sizeof(takefuel) - 1);
		ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_FUELAMOUNT, fuelamount, sizeof(fuelamount) - 1);
		ok = GetDlgItemText(hwnd, IDC_PLANETSHIELDS_COUNT_SHIELDS, numberOfShields, sizeof(numberOfShields) - 1);

		
		if (IsDlgButtonChecked(hwnd, IDC_PLANETSHIELDS_TO_TWARP))
			strcpy(wayToTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_PLANETSHIELDS_TO_PWARP))
			strcpy(wayToTerra, "Planetary Transport");
		else if (IsDlgButtonChecked(hwnd, IDC_PLANETSHIELDS_TO_WARP))
			strcpy(wayToTerra, "Regular Warping");

		if (IsDlgButtonChecked(hwnd, IDC_PLANETSHIELDS_FROM_TWARP))
			strcpy(wayFromTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_PLANETSHIELDS_FROM_WARP))
			strcpy(wayFromTerra, "Regular Warping");

		if (atol(sector)<=0 || atol(sector)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Sector", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(portsector)<=0)
		{
			MessageBox(NULL, "Invalid Port Sector", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(takefuel)<=0)
		{
			MessageBox(NULL, "Invalid Planet to Take Fuel", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(fuelamount)<=0 && strcmp (fuelamount, "AUTO") != 0)
		{
			MessageBox(NULL, "Invalid Amount of Fuel to Take", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(leaveshields)<=0)
		{
			MessageBox(NULL, "Invalid Planet to leave shields", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(numberOfShields)<=0)
		{
			MessageBox(NULL, "Invalid Number of Shields to take", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			strcpy (rexxArgs[0], sector);
			strcpy (rexxArgs[1], portsector);
			strcpy (rexxArgs[2], cycles);
			strcpy (rexxArgs[3], leaveshields);
			strcpy (rexxArgs[4], numberOfShields);
			strcpy (rexxArgs[5], wayToTerra);
			strcpy (rexxArgs[6], wayFromTerra);
			strcpy (rexxArgs[7], fuelamount);
			strcpy (rexxArgs[8], takefuel);
/*
			printf ("sector: %s\n", rexxArgs[0]);
			printf ("portsector: %s\n", rexxArgs[1]);
			printf ("cycles: %s\n", rexxArgs[2]);
			printf ("leaveshields: %s\n", rexxArgs[3]);
			printf ("numberOfShields: %s\n", rexxArgs[4]);
			printf ("wayToTerra: %s\n", rexxArgs[5]);
			printf ("wayFromTerra: %s\n", rexxArgs[6]);
			printf ("fuelamount: %s\n", rexxArgs[7]);
			printf ("takefuel: %s\n", rexxArgs[8]);
*/

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardBuyPlanetaryShields.erx");
			REXX_Start(fullFilePath, 1, 9, rexxArgs);
		//	showbuyplanetshields = NULL;
		//	SetActiveWindow(GetParent(hwnd));
		//	DestroyWindow(hwnd);
			ShowWindow(showbuyplanetshields,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
//		showbuyplanetshields = NULL;
		//say we closed out the zoc ask

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		ShowWindow(showbuyplanetshields,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showbuyplanetshields = NULL;
	//say we closed out the zoc ask
//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showbuyplanetshields,SW_HIDE);
	return 0;
    }
    return 0;
}

//IDD_BUBBLE_FINDER
static int CALLBACK BubbleFinderProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	HWND hlist, hlist2;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno, i, j, k, count, pos, correctBubble=0;
	char buffer[30], *token, bubbleChar[MAX_LENGTH];
	char bubble_sectors[MAX_LENGTH*3][20];
	char entMult[30];
    RECT rect;
    HDC hDC;
    PAINTSTRUCT ps;

	hlist = GetDlgItem(hwnd, IDC_BUBBLE_FINDER_LIST_ENTRANCE);
	hlist2 = GetDlgItem(hwnd, IDC_BUBBLE_FINDER_LIST_SECTORS);
    switch (msg) {
      case WM_INITDIALOG:

		listview_insert_column (hlist, 0, 60, 0, "Sector");
		listview_insert_column (hlist, 1, 40, 1, "Size");

		listview_insert_column (hlist2, 0, 100, 0, "Sectors in Bubble");

	return 1;
	/*
	   case WM_SIZE:
		  if(wParam != SIZE_MINIMIZED)
		  {
			GetClientRect(hwnd, &rect);	

			for (i=0; i<ListView_GetItemCount(hlist); i++)
			{
				if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
				{
					ListView_GetItemText (hlist, i, 0, buffer, sizeof (buffer));
					i = ListView_GetItemCount(hlist);
				}
			}
			printf ("buffer %s\n", buffer);
//			MoveWindow(GetDlgItem(hwnd, IDOK), rect.right-200-39, rect.bottom+40, 70, 20, TRUE);
//			MoveWindow(GetDlgItem(hwnd, IDCANCEL), rect.right-bubbleOffset+39, rect.bottom+40, 50, 14, TRUE);
//			MoveWindow(GetDlgItem(hwnd, IDC_BUBBLE_FINDER_LABEL), rect.right-bubbleOffset, rect.bottom+5, 300, 15, TRUE);
			buildMapDataStructure(hwnd, atol(buffer), 5, rect.bottom, rect.right);
			InvalidateRect (hwnd, &rect, FALSE);
		  }
	   break;
	   */
      case WM_NOTIFY:
		if ((int)wParam == IDC_BUBBLE_FINDER_LIST_ENTRANCE) 
			{
				hlist = GetDlgItem(hwnd, IDC_BUBBLE_FINDER_LIST_ENTRANCE);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hlist;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hlist, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Bubble Sorting Error", "Error", MB_OK);
                        break;
					case NM_CLICK:

						for (i=0; i<ListView_GetItemCount(hlist); i++)
						{
							if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
							{
								ListView_GetItemText (hlist, i, 0, buffer, sizeof (buffer));
								i = ListView_GetItemCount(hlist);
							}
						}
						//strcpy (mapSector, buffer);
						GetClientRect(hwnd, &rect);		
						buildMapDataStructure(hwnd, atol(buffer), 5, 400, 400);
						rect.left=bubbleOffset;
						if (bubbleOffset>0)
						{
							rect.right=rect.right-10;
							rect.top=rect.top+10;
							rect.bottom=rect.bottom-10;
						}
						InvalidateRect (hwnd, &rect, FALSE);

						ListView_DeleteAllItems (hlist2);

						j=0;
						count=0;
						for (i=0; i<(int)strlen(bubbleCharDataStorage); i++)
						{
							if (bubbleCharDataStorage[i]==';')
							{
								bubbleChar[j]=0;
								pos=0;
								j=0;

								token = strtok(bubbleChar, ":");
								while (token != NULL)
								{
									strcpy (bubble_sectors[pos], token);
									token = strtok(NULL, ":");
									pos++;
								}	
								if (strcmp (buffer, bubble_sectors[1])==0)
									correctBubble=1;

								k=2;
								while (k<pos)
								{
									if (correctBubble==1)
										listview_insert_bubble_sectors(hlist2, count++, bubble_sectors[k]);
									k++;
								}
								correctBubble=0;
							}
							else
								bubbleChar[j++] = bubbleCharDataStorage[i];
						}
							
						break;

                }
            }
		return 1;
	  case WM_PAINT:
		for (i=0; i<ListView_GetItemCount(hlist); i++)
		{
			if (ListView_GetItemState(hlist, i, LVIS_SELECTED)==LVIS_SELECTED)
			{
				ListView_GetItemText (hlist, i, 0, buffer, sizeof (buffer));
				i = ListView_GetItemCount(hlist);
			}
		}

		hDC = BeginPaint (hwnd, &ps);

		drawMap(hDC, hwnd, atol(buffer), bubbleOffset);
		EndPaint(hwnd, &ps);

		break;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		    GetDlgItemText(hwnd, IDC_BUBBLE_FINDER_ENTRANCE_MULT, entMult, sizeof(entMult) - 1);
			
			if (atoi(entMult)<2 || atoi(entMult) > 1000)
				MessageBox(hwnd, "Entrance Multiplier must be between 2 and 1000", "Error", MB_OK);
			else
			{
				ListView_DeleteAllItems (hlist);
				ListView_DeleteAllItems (hlist2);
				if(_beginthread(findBubblesThreadProc, 0, NULL )==-1)
				{
					printf("Failed to create thread");
					return 1;
				}
			}
		return 0;
	  case IDCANCEL:
		showbubblefinder = NULL;
		if (showmap==NULL) {
			#ifdef P_FREE
				pfree (&mapLines);
			#else
				free (mapLines);
			#endif			
		}
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
		if (bubbleCharDataStorage != NULL)
		{
			#ifdef P_FREE
				pfree (&bubbleCharDataStorage);
			#else
				free (bubbleCharDataStorage);
			#endif			
			bubbleCharDataStorage=NULL;
		}
		
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showbubblefinder = NULL;
	if (bubbleCharDataStorage != NULL)
	{
		#ifdef P_FREE
			pfree (&bubbleCharDataStorage);
		#else
			free (bubbleCharDataStorage);
		#endif
	}
	if (showmap==NULL) {
		#ifdef P_FREE
			pfree (&mapLines);
		#else
			free (mapLines);
		#endif
	}
	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;

}



static int CALLBACK NearestFighterProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char buffer[100];
	char buffer1[30];
	long nearestSector=0, theSector;

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:

			GetDlgItemText(hwnd, IDC_NEARESTFIGHTER_SECTOR, buffer1, sizeof(buffer1) - 1);

			theSector = atol(buffer1);
			if (theSector >0 && theSector <= header.sector_cnt )
			{
				nearestSector =  findNearestFigs (theSector);

				if (nearestSector==0)
					sprintf (buffer, "No fighters found in approximately a 10 sector radius\n");
				else
					sprintf (buffer, "Nearest Sector with Fighters is: %ld\n", nearestSector);
			}
			else
				sprintf (buffer, "Please enter a sector between 1 and %ld\n", header.sector_cnt);


			SetDlgItemText(hwnd, IDC_NEARESTFIGHTER_DATA, buffer);
		  return 0;
	  case IDCANCEL:
	//	shownearestfig = NULL;
//
//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		  ShowWindow(shownearestfig,SW_HIDE); 
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	shownearestfig = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


//
static int CALLBACK ShowPlanetProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	HWND hShowPlanets;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;

    switch (msg) {
      case WM_INITDIALOG:

		 hShowPlanets = GetDlgItem(hwnd, IDC_SHOWPLANETS);

		listview_insert_column (hShowPlanets, 0, 50, 0, "Sector");
		listview_insert_column (hShowPlanets, 1, 250, 1, "Planet");
		listview_insert_column (hShowPlanets, 2, 150, 2, "Sector Fig Owner");
		loadPlanetList(hShowPlanets);
		
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_SHOWPLANETS) 
			{
				hShowPlanets = GetDlgItem(hwnd, IDC_SHOWPLANETS);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hShowPlanets;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hShowPlanets, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Planet Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		showplanet = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	
	    return 0;
	  case IDCANCEL:
		showplanet = NULL;

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showplanet = NULL;

	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

static int CALLBACK MoverProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char sector1[30];
	char takefuel1[30];
	char leavecargo1[30];
	char sector2[30];
	char takefuel2[30];
	char leavecargo2[30];
	char warptobeforeterra[30];
	char resourceToLeave[30];
	char leavecolonists[30];
	char wayToTerra[30];
	char wayFromTerra[30];
	char cycles[30];
	char fuelamount[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_MOVER_SECTOR1:
		if (HIWORD(wParam) == EN_CHANGE)
		{
//			ok = GetDlgItemText(hwnd, IDC_MOVER_SECTOR1, sector1, sizeof(sector1) - 1);
//			ok = GetDlgItemText(hwnd, IDC_MOVER_SECTOR1, sector2, sizeof(sector1) - 2);
//			if (strcmp (sector1, sector2)==0)
//				SetDlgItemText(hwnd, IDC_MOVER_SECTOR2, sector1);
		}
		return 0;
	  case IDC_MOVER_LEAVE1:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			ok = GetDlgItemText(hwnd, IDC_MOVER_LEAVE1, leavecargo1, sizeof(leavecargo1) - 1);
			SetDlgItemText(hwnd, IDC_MOVER_TAKEFUEL1, leavecargo1);
		}
		return 0;
	  case IDC_MOVER_LEAVE2:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			ok = GetDlgItemText(hwnd, IDC_MOVER_LEAVE2, leavecargo2, sizeof(leavecargo2) - 1);
			SetDlgItemText(hwnd, IDC_MOVER_TAKEFUEL2, leavecargo2);
		}
		return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_MOVER_SECTOR1, sector1, sizeof(sector1) - 1);
  	    ok = GetDlgItemText(hwnd, IDC_MOVER_SECTOR2, sector2, sizeof(sector2) - 1);

  	    ok = GetDlgItemText(hwnd, IDC_MOVER_LEAVE1, leavecargo1, sizeof(leavecargo1) - 1);
  	    ok = GetDlgItemText(hwnd, IDC_MOVER_LEAVE2, leavecargo2, sizeof(leavecargo2) - 1);

  	    ok = GetDlgItemText(hwnd, IDC_MOVER_TAKEFUEL1, takefuel1, sizeof(takefuel1) - 1);
  	    ok = GetDlgItemText(hwnd, IDC_MOVER_TAKEFUEL2, takefuel2, sizeof(takefuel2) - 1);
		
		ok = GetDlgItemText(hwnd, IDC_MOVER_CYCLES, cycles, sizeof(cycles) - 1);
		ok = GetDlgItemText(hwnd, IDC_MOVER_FUELAMOUNT, fuelamount, sizeof(fuelamount) - 1);

		if (atoi(warptobeforeterra)==0)
			strcpy (warptobeforeterra, "1");

		if (IsDlgButtonChecked(hwnd, IDC_MOVER_FUEL_RADIO))
			strcpy (resourceToLeave, "Fuel Ore");
		else if (IsDlgButtonChecked(hwnd, IDC_MOVER_ORG_RADIO))
			strcpy (resourceToLeave, "Organics");
		else if (IsDlgButtonChecked(hwnd, IDC_MOVER_EQUIP_RADIO))
			strcpy (resourceToLeave, "Equipment");
		
		strcpy (wayToTerra, "");
		if (IsDlgButtonChecked(hwnd, IDC_MOVER_TOTWARP_RADIO))
			strcpy(wayToTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_MOVER_TOPWARP_RADIO))
			strcpy(wayToTerra, "Planetary Transport");
		else if (IsDlgButtonChecked(hwnd, IDC_MOVER_TOWARP_RADIO))
			strcpy(wayToTerra, "Regular Warping");

		if (IsDlgButtonChecked(hwnd, IDC_MOVER_FROMTWARP_RADIO))
			strcpy(wayFromTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_MOVER_FROMPWARP_RADIO))
			strcpy(wayFromTerra, "Planetary Transport");
		else if (IsDlgButtonChecked(hwnd, IDC_MOVER_FROMWARP_RADIO))
			strcpy(wayFromTerra, "Regular Warping");

		if (IsDlgButtonChecked(hwnd, IDC_MOVER_COLONISTS_CHECK))
			strcpy(leavecolonists, "Colonists");
		else
			strcpy(leavecolonists, "Cargo");

		if (atol(sector1)<=0 || atol(sector1)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Sector #1", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(sector2)<=0 || atol(sector2)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Sector #2", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles for Colonization", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(leavecargo1)<=0)
		{
			MessageBox(NULL, "Invalid Planet #1 to Leave Stuff", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(leavecargo2)<=0)
		{
			MessageBox(NULL, "Invalid Planet #2 to Leave Stuff", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(takefuel1)<=0)
		{
			MessageBox(NULL, "Invalid Planet #1 to Take Fuel", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(takefuel2)<=0)
		{
			MessageBox(NULL, "Invalid Planet #2 to Take Fuel", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(fuelamount)<=0 && strcmp (fuelamount, "AUTO") != 0)
		{
			MessageBox(NULL, "Invalid Amount of Fuel to Take", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			strcpy (rexxArgs[0], sector1);
			strcpy (rexxArgs[1], sector2);
			strcpy (rexxArgs[2], leavecargo1);
			strcpy (rexxArgs[3], leavecargo2);
			strcpy (rexxArgs[4], takefuel1);
			strcpy (rexxArgs[5], takefuel2);
			strcpy (rexxArgs[6], resourceToLeave);
			strcpy (rexxArgs[7], leavecolonists);
			strcpy (rexxArgs[8], wayToTerra);
			strcpy (rexxArgs[9], wayFromTerra);
			strcpy (rexxArgs[10], fuelamount);
			strcpy (rexxArgs[11], cycles);
/*
			printf ("sector1: %s\n", rexxArgs[0]);
			printf ("sector2: %s\n", rexxArgs[1]);
			printf ("leavecargo1: %s\n", rexxArgs[2]);
			printf ("leavecargo2: %s\n", rexxArgs[3]);
			printf ("takefuel1: %s\n", rexxArgs[4]);
			printf ("takefuel2: %s\n", rexxArgs[5]);
			printf ("resourceToLeave: %s\n", rexxArgs[6]);
			printf ("leavecolonists: %s\n", rexxArgs[7]);
			printf ("wayToTerra: %s\n", rexxArgs[8]);
			printf ("wayFromTerra: %s\n", rexxArgs[9]);
			printf ("fuelamount: %s\n", rexxArgs[10]);
			printf ("cycles: %s\n", rexxArgs[11]);
*/
			
			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardMover.erx");
			REXX_Start(fullFilePath, 1, 12, rexxArgs);

//			showmover = NULL;
//			SetActiveWindow(GetParent(hwnd));
//			DestroyWindow(hwnd);
			ShowWindow(showmover,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
//		showmover = NULL;
		//say we closed out the zoc ask

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		  ShowWindow(showmover,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showmover = NULL;
	//say we closed out the zoc ask
//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showmover,SW_HIDE);
	return 0;
    }
    return 0;
}


static int CALLBACK WorldSSMProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char sdSector[30];
	char class0Sector[30];
	char useSDToRefurb[30];
	char planetSector[30];
	char planetNumber[30];
	char dropCash[30];
	char equipPort[30];
	char fuelPort[30];
	char tWarpPort[30];
	char cycles[30];
	char dropFigs[30];
	char dropFigAmount[30];
	char dropFigType[30];
	char dropFigOwner[30];

	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {

	  case IDC_WORLDSSM_DROPFIGS_CHECK:
		if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_DROPFIGS_CHECK))
		{
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_DROPFIGS_AMOUNT), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_CORPORATE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_PERSONAL), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_OFFENSIVE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_TOLL), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_DEFENSIVE), 1);
		}
		else 
		{
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_DROPFIGS_AMOUNT), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_CORPORATE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_PERSONAL), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_OFFENSIVE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_TOLL), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDSSM_DEFENSIVE), 0);
		}
		break;
	  case IDC_WORLDSSM_EQUIP:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			ok = GetDlgItemText(hwnd, IDC_WORLDSSM_EQUIP, equipPort, sizeof(equipPort) - 1);
			SetDlgItemText(hwnd, IDC_WORLDSSM_FUEL, equipPort);
		}
		return 0;
	  case IDC_WORLDSSM_PLANETSECTOR:
	  case IDC_WORLDSSM_PLANETNUMBER:
			GetDlgItemText(hwnd, IDC_WORLDSSM_PLANETSECTOR, planetSector, sizeof(planetSector) - 1);
			GetDlgItemText(hwnd, IDC_WORLDSSM_PLANETNUMBER, planetNumber, sizeof(planetNumber) - 1);
			if (strlen (planetSector)==0 && strlen(planetNumber)==0)
			{
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_PLANETNUMBER), FALSE ) ; /* disable a control */
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_DROPCASH), FALSE ) ; /* disable a control */
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_TWARP), FALSE ) ; /* disable a control */
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_FUEL), FALSE ) ; /* disable a control */
			}
			else
			{
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_PLANETNUMBER), TRUE ) ; /* enable a control */
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_DROPCASH), TRUE ) ; /* enable a control */
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_TWARP), TRUE ) ; /* enable a control */
				EnableWindow( GetDlgItem(hwnd,IDC_WORLDSSM_FUEL), TRUE ) ; /* enable a control */
			}
		  return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_WORLDSSM_SD, sdSector, sizeof(sdSector) - 1);
		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_CLASS0, class0Sector, sizeof(class0Sector) - 1);

		strcpy (useSDToRefurb, "");
		if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_SD_CHECK))
			strcpy (useSDToRefurb, "0");
		else if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_CLASS0_CHECK))
			strcpy (useSDToRefurb, "1");

		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_PLANETSECTOR, planetSector, sizeof(planetSector) - 1);
		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_PLANETNUMBER, planetNumber, sizeof(planetNumber) - 1);

		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_EQUIP, equipPort, sizeof(equipPort) - 1);
		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_FUEL, fuelPort, sizeof(fuelPort) - 1);
		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_TWARP, tWarpPort, sizeof(tWarpPort) - 1);
		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_DROPCASH, dropCash, sizeof(dropCash) - 1);
		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_CYCLES, cycles, sizeof(cycles) - 1);

		strcpy (dropFigs, "");
		if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_DROPFIGS_CHECK))
			strcpy (dropFigs, "1");
		else 
			strcpy (dropFigs, "0");

		ok = GetDlgItemText(hwnd, IDC_WORLDSSM_DROPFIGS_AMOUNT, dropFigAmount, sizeof(dropFigAmount) - 1);

		strcpy (dropFigType, "");
		if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_TOLL))
			strcpy (dropFigType, "T");
		else if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_DEFENSIVE))
			strcpy (dropFigType, "D");
		else if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_OFFENSIVE))
			strcpy (dropFigType, "O");

		strcpy (dropFigOwner, "");
		if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_CORPORATE))
			strcpy(dropFigOwner, "C");
		else if (IsDlgButtonChecked(hwnd, IDC_WORLDSSM_PERSONAL))
			strcpy(dropFigOwner, "P");

		if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles for World SSM", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (strcmp(useSDToRefurb, "") ==0)
		{
			MessageBox(NULL, "You must check either SD or Class 0 to refurb", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atoi(useSDToRefurb) ==0 && atol(sdSector)<=0)
		{
			MessageBox(NULL, "You must enter an SD sector if you checked off SD to refurb", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atoi(useSDToRefurb) ==1 && atol(class0Sector)<=0)
		{
			MessageBox(NULL, "You must enter an class 0 sector if you checked off class 0 to refurb", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(equipPort) <=0)
		{
			MessageBox(NULL, "You must enter an equipment port to restock your equipment", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(dropCash) <=0)
		{
			MessageBox(NULL, "The number of cycles before we drop off cash must be greater than zero", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(planetSector) >0 && (atol(planetNumber) <=0 || atol(fuelPort) <=0 || atol(tWarpPort)<=0 || atol(dropCash)<=0))
		{
			MessageBox(NULL, "If you enter a planet sector, you must also enter the planet number, the fuel sector, the twarp sector and the number of cycles before dropping off cash", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atoi(dropFigs)==1 && atol(dropFigAmount)<=0)
		{
			MessageBox(NULL, "Invalid Number of Figs to drop. Must be zero or greater", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			strcpy (rexxArgs[0], sdSector);
			strcpy (rexxArgs[1], class0Sector);
			strcpy (rexxArgs[2], useSDToRefurb);
			strcpy (rexxArgs[3], planetSector);
			strcpy (rexxArgs[4], planetNumber);
			strcpy (rexxArgs[5], equipPort);
			strcpy (rexxArgs[6], fuelPort);
			strcpy (rexxArgs[7], tWarpPort);
			strcpy (rexxArgs[8], dropCash);
			strcpy (rexxArgs[9], cycles);
			sprintf (rexxArgs[10], "%d", header.stealFactor);
			strcpy (rexxArgs[11], dropFigs);
			strcpy (rexxArgs[12], dropFigAmount);
			strcpy (rexxArgs[13], dropFigOwner);
			strcpy (rexxArgs[14], dropFigType);

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardWorldSSM.erx");
			REXX_Start(fullFilePath, 1, 15, rexxArgs);
//			showcolonization = NULL;
//			SetActiveWindow(GetParent(hwnd));
//			DestroyWindow(hwnd);
			ShowWindow(showworldssm,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
//		showcolonization = NULL;
		//say we closed out the zoc ask

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		  ShowWindow(showworldssm,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showcolonization = NULL;
	//say we closed out the zoc ask
//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showworldssm,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK AutoExplorerProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char startSector[30];
	char portAndTrade[30];
	char haggle[30];
	char avoidPlanetsLimpits[30];
	char avoidsSectorsWithNFigs[30];
	char avoidsSectorsFigsAmount[30];
	char dropFig[30];
	char figamount[30];
	char figStyle[30];
	char figType[30];
	char figOwner[30];
	char exploreAll[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_AUTOEXPLORE_DROPFIGS:
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_DROPFIGS))
		{
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_FIGAMOUNT), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_FIGSTYLE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_TOLL), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_DEFENSIVE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_OFFENSIVE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_CORPORATE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_PERSONAL), 1);
		}
		else 
		{
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_FIGAMOUNT), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_FIGSTYLE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_TOLL), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_DEFENSIVE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_OFFENSIVE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_CORPORATE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_PERSONAL), 0);
		}
		break;
	  case IDC_AUTOEXPLORE_MAXFIGSAVOID:
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_MAXFIGSAVOID))
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_NUMFIGSAVOID), 1);
		else 
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_NUMFIGSAVOID), 0);
		break;
	  case IDC_AUTOEXPLORE_PORTTRADE:
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_PORTTRADE))
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_HAGGLE), 1);
		else 
			EnableWindow(GetDlgItem(hwnd, IDC_AUTOEXPLORE_HAGGLE), 0);
		break;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_AUTOEXPLORE_STARTSECTOR, startSector, sizeof(startSector) - 1);

		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_PORTTRADE))
			strcpy (portAndTrade, "##YES##");
		else 
			strcpy (portAndTrade, "##NO##");
		
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_HAGGLE))
			strcpy(haggle, "1");
		else
			strcpy(haggle, "0");

		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_PLANETAVOID))
			strcpy(avoidPlanetsLimpits, "1");
		else 
			strcpy(avoidPlanetsLimpits, "0");
		
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_MAXFIGSAVOID))
			strcpy(avoidsSectorsWithNFigs, "1");
		else 
			strcpy(avoidsSectorsWithNFigs, "0");
		
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_EXPLOREALL))
			strcpy(exploreAll, "1");
		else 
			strcpy(exploreAll, "0");
		

		ok = GetDlgItemText(hwnd, IDC_AUTOEXPLORE_NUMFIGSAVOID, avoidsSectorsFigsAmount, sizeof(avoidsSectorsFigsAmount) - 1);

		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_DROPFIGS))
			strcpy(dropFig, "1");
		else 
			strcpy(dropFig, "0");

		ok = GetDlgItemText(hwnd, IDC_AUTOEXPLORE_FIGAMOUNT, figamount, sizeof(figamount) - 1);

		ok = GetDlgItemText(hwnd, IDC_AUTOEXPLORE_FIGSTYLE, figStyle, sizeof(figStyle) - 1);
		if (strcmp (figStyle, "At Ports") != 0 && strcmp (figStyle,"Everywhere") != 0)
			strcpy (figStyle, "No");

		strcpy (figType, "");
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_TOLL))
			strcpy (figType, "Toll");
		else if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_DEFENSIVE))
			strcpy (figType, "Defensive");
		else if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_OFFENSIVE))
			strcpy (figType, "Offensive");

		strcpy (figOwner, "");
		if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_CORPORATE))
			strcpy(figOwner, "Corporate");
		else if (IsDlgButtonChecked(hwnd, IDC_AUTOEXPLORE_PERSONAL))
			strcpy(figOwner, "Personal");


		if (atol(startSector)<=0 || atol(startSector)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Start Sector for Auto Explorer", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atoi(dropFig)==1 && atol(figamount)<=0)
		{
			MessageBox(NULL, "Invalid Number of Figs to drop. Must be zero or greater", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atoi(avoidsSectorsWithNFigs)==1 && atol(avoidsSectorsFigsAmount)<=0)
		{
			MessageBox(NULL, "Invalid Number of Figs to drop. Must be zero or greater", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			strcpy (rexxArgs[0], startSector);
			strcpy (rexxArgs[1], portAndTrade);
			strcpy (rexxArgs[2], haggle);
			strcpy (rexxArgs[3], avoidPlanetsLimpits);
			strcpy (rexxArgs[4], avoidsSectorsWithNFigs);
			strcpy (rexxArgs[5], avoidsSectorsFigsAmount);
			strcpy (rexxArgs[6], dropFig);
			strcpy (rexxArgs[7], figStyle);
			strcpy (rexxArgs[8], figamount);
			strcpy (rexxArgs[9], figType);
			strcpy (rexxArgs[10], figOwner);
			strcpy (rexxArgs[11], exploreAll);

//			printf ("startSector: %s\n", startSector);
//			printf ("portAndTrade: %s\n", portAndTrade);
//			printf ("haggle: %s\n", haggle);
//			printf ("avoidPlanetsLimpits: %s\n", avoidPlanetsLimpits);
//			printf ("avoidsSectorsWithNFigs: %s\n", avoidsSectorsWithNFigs);
//			printf ("avoidsSectorsFigsAmount: %s\n", avoidsSectorsFigsAmount);
//			printf ("dropFig: %s\n", dropFig);
//			printf ("figStyle: %s\n", figStyle);
//			printf ("figamount: %s\n", figamount);
//			printf ("figType: %s\n", figType);
//			printf ("figOwner: %s\n\n\n", figOwner);

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardExplorer.erx");
			REXX_Start(fullFilePath, 1, 12, rexxArgs);
			SetActiveWindow(GetParent(hwnd));
			ShowWindow(showautoexplorer,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
//		showcolonization = NULL;
		//say we closed out the zoc ask

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		  ShowWindow(showautoexplorer,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showcolonization = NULL;
	//say we closed out the zoc ask
//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showautoexplorer,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK WorldTradeProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char figamount[30];
	char cycles[30];
	char figType[30];
	char figOwner[50];
	char pairType[50];
	char dropFig[30];
	char worldHaggle[30];
	char buffer[50];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_WORLDTRADE_DROPFIGS:
		if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_DROPFIGS))
		{
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_FIGAMOUNT), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_TOLL), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_DEFENSIVE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_OFFENSIVE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_CORPORATE), 1);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_PERSONAL), 1);
		}
		else 
		{
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_FIGAMOUNT), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_TOLL), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_DEFENSIVE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_OFFENSIVE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_CORPORATE), 0);
			EnableWindow(GetDlgItem(hwnd, IDC_WORLDTRADE_PERSONAL), 0);
		}
		break;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_WORLDTRADE_FIGAMOUNT, figamount, sizeof(figamount) - 1);
		ok = GetDlgItemText(hwnd, IDC_WORLDTRADE_CYCLES, cycles, sizeof(cycles) - 1);

		strcpy (figType, "");
		if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_TOLL))
			strcpy (figType, "T");
		else if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_DEFENSIVE))
			strcpy (figType, "D");
		else if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_OFFENSIVE))
			strcpy (figType, "O");
		
		strcpy (figOwner, "");
		if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_CORPORATE))
			strcpy(figOwner, "C");
		else if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_PERSONAL))
			strcpy(figOwner, "P");

		if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_DROPFIGS))
			strcpy(dropFig, "1");
		else 
			strcpy(dropFig, "0");

		if (IsDlgButtonChecked(hwnd, IDC_WORLDTRADE_HAGGLE))
		{
			strcpy(worldHaggle, "1");
			header.forceNoHaggle=0;
		}
		else
		{
			strcpy(worldHaggle, "0");
			header.forceNoHaggle=1;
		}


		ok = GetDlgItemText(hwnd, IDC_WORLDTRADE_PAIRTYPE, pairType, sizeof(pairType) - 1);

		if (strcmp(pairType, "Equipment-Organic")==0)
			strcpy(pairType, "1");
		else if (strcmp(pairType, "Equipment-Fuel")==0)
			strcpy(pairType, "2");
		else if (strcmp(pairType, "Organic-Fuel")==0)
			strcpy(pairType, "3");
		else
			strcpy(pairType, "1");

		if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles for World Trade", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(figamount)<=0)
		{
			MessageBox(NULL, "Invalid Number of Figs. Must be zero or greater", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			strcpy (rexxArgs[0], figamount);
			strcpy (rexxArgs[1], cycles);
			strcpy (rexxArgs[2], dropFig);
			strcpy (rexxArgs[3], figOwner);
			strcpy (rexxArgs[4], figType);
			strcpy (rexxArgs[5], worldHaggle);
			sprintf (buffer, "%d", header.minPairPortPercent);
			strcpy (rexxArgs[6], buffer);
			strcpy (rexxArgs[7], pairType);
//			printf ("figamount: %s\n", figamount);
//			printf ("cycles: %s\n", cycles);
//			printf ("dropFig: %s\n", dropFig);
//			printf ("figOwner: %s\n", figOwner);
//			printf ("figType: %s\n", figType);
//			printf ("worldHaggle: %s\n", worldHaggle);
//			printf ("minPairPortPercent: %s\n", minPairPortPercent);

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardTradeAtEveryPort.erx");
			REXX_Start(fullFilePath, 1, 8, rexxArgs);
//			showcolonization = NULL;
//			SetActiveWindow(GetParent(hwnd));
//			DestroyWindow(hwnd);
			ShowWindow(showworldtrade,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
//		showcolonization = NULL;
		//say we closed out the zoc ask

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		  ShowWindow(showworldtrade,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showcolonization = NULL;
	//say we closed out the zoc ask
//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showworldtrade,SW_HIDE);
	return 0;
    }
    return 0;
}


static int CALLBACK PlanetBustProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char planetName[40];
	char cycles[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_PLANETBUST_NAME:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			ok = GetDlgItemText(hwnd, IDC_PLANETBUST_NAME, planetName, sizeof(planetName) - 1);
			if (strlen(planetName)>35)
			{
				MessageBox(NULL, "The planet name may not be more than 35 characters", "ATTAC Error", MB_ICONWARNING); 
				planetName[strlen(planetName)-1]=0;
				SetDlgItemText(hwnd, IDC_PLANETBUST_NAME, planetName);
			}
		}
		return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_PLANETBUST_NAME, planetName, sizeof(planetName) - 1);
		ok = GetDlgItemText(hwnd, IDC_PLANETBUST_CYCLES, cycles, sizeof(cycles) - 1);

		if (strlen(planetName)<10)
		{
			MessageBox(NULL, "The planet name must be more than 10 characters. This insures the name is unique", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles for Planet Bust", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			strcpy (rexxArgs[0], planetName);
			strcpy (rexxArgs[1], cycles);

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardBustPlanets.erx");
			REXX_Start(fullFilePath, 1, 2, rexxArgs);

			ShowWindow(showplanetbust,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
		  ShowWindow(showplanetbust,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
		  ShowWindow(showplanetbust,SW_HIDE);
	return 0;
    }
    return 0;
}


static int CALLBACK AddBustProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char sector[30];
	long tempCurrentSector;
	HANDLE hlist;

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_ADDBUST_SECTOR, sector, sizeof(sector) - 1);

		if (atol(sector)<=0 || atol(sector)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Sector to add to Bust List", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			tempCurrentSector = atol(sector);
			if (isValidSectorOneOffset(tempCurrentSector)) {
				sectors[tempCurrentSector-1].lastBusted=ATTAC_Time();
				sectors[tempCurrentSector-1].playerIdBusted=0;
			}
			if (showbustlist)
			{
				hlist = GetDlgItem(showbustlist, IDC_BUSTLIST_LIST);

				findBusts(hlist, -1, -1);
			}
			showaddbust = NULL;
			SetActiveWindow(GetParent(hwnd));
			DestroyWindow(hwnd);
		}
	    return 0;
	  case IDCANCEL:
		showaddbust = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
		showaddbust = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}



static int CALLBACK ColonizationProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
    BOOL ok;
	char sector[30];
	char cycles[30];
	char leavecolonists[30];
	char takefuel[30];
	char fuelamount[30];
	char warptobeforeterra[30];
	char resourceToLeave[30];
	char wayToTerra[30];
	char wayFromTerra[30];
	char rexxArgs[100][100];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDC_COLONIZE_LEAVECOLOS_EDIT:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			ok = GetDlgItemText(hwnd, IDC_COLONIZE_LEAVECOLOS_EDIT, leavecolonists, sizeof(leavecolonists) - 1);
			SetDlgItemText(hwnd, IDC_COLONIZE_TAKEFUEL_EDIT, leavecolonists);
		}
		return 0;
	  case IDOK:
  	    ok = GetDlgItemText(hwnd, IDC_COLONIZE_SECTOR_EDIT, sector, sizeof(sector) - 1);
		ok = GetDlgItemText(hwnd, IDC_COLONIZE_CYCLES_EDIT, cycles, sizeof(cycles) - 1);
		ok = GetDlgItemText(hwnd, IDC_COLONIZE_LEAVECOLOS_EDIT, leavecolonists, sizeof(leavecolonists) - 1);
		ok = GetDlgItemText(hwnd, IDC_COLONIZE_TAKEFUEL_EDIT, takefuel, sizeof(takefuel) - 1);
		ok = GetDlgItemText(hwnd, IDC_COLONIZE_FUELAMOUNT_EDIT, fuelamount, sizeof(fuelamount) - 1);
		ok = GetDlgItemText(hwnd, IDC_COLONIZE_WARPTO_EDIT, warptobeforeterra, sizeof(warptobeforeterra) - 1);

		if (atoi(warptobeforeterra)==0)
			strcpy (warptobeforeterra, "1");

		if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_FUEL_RADIO))
			strcpy (resourceToLeave, "1");
		else if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_ORG_RADIO))
			strcpy (resourceToLeave, "2");
		else if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_EQUIP_RADIO))
			strcpy (resourceToLeave, "3");
		
		strcpy (wayToTerra, "");
		if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_TOTWARP_RADIO))
			strcpy(wayToTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_TOPWARP_RADIO))
			strcpy(wayToTerra, "Planetary Transport");
		else if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_TOWARP_RADIO))
			strcpy(wayToTerra, "Regular Warping");

		if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_FROMTWARP_RADIO))
			strcpy(wayFromTerra, "Transwarp");
		else if (IsDlgButtonChecked(hwnd, IDC_COLONIZE_FROMWARP_RADIO))
			strcpy(wayFromTerra, "Regular Warping");

		if (atol(sector)<=0 || atol(sector)>header.sector_cnt)
		{
			MessageBox(NULL, "Invalid Sector for Colonization", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(cycles)<=0)
		{
			MessageBox(NULL, "Invalid Number of Cycles for Colonization", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(leavecolonists)<=0)
		{
			MessageBox(NULL, "Invalid Planet to Leave Colonists", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(takefuel)<=0)
		{
			MessageBox(NULL, "Invalid Planet to Take Fuel", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(fuelamount)<=0 && strcmp (fuelamount, "AUTO") != 0)
		{
			MessageBox(NULL, "Invalid Amount of Fuel to Take", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (atol(warptobeforeterra)<0)
		{
			MessageBox(NULL, "Invalid Sector to warp to before terra", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else if (strcmp (wayToTerra, "") == 0)
		{
			MessageBox(NULL, "Invalid Way to warp to terra", "ATTAC Error", MB_ICONWARNING); 
			return 0;
		}
		else
		{
			strcpy (rexxArgs[0], sector);
			strcpy (rexxArgs[1], cycles);
			strcpy (rexxArgs[2], leavecolonists);
			strcpy (rexxArgs[3], takefuel);
			strcpy (rexxArgs[4], fuelamount);
			strcpy (rexxArgs[5], warptobeforeterra);
			strcpy (rexxArgs[6], resourceToLeave);
			strcpy (rexxArgs[7], wayToTerra);
			strcpy (rexxArgs[8], wayFromTerra);

			strcpy (fullFilePath, scriptDir);
			strcat (fullFilePath, "StandardColonizer.erx");
			REXX_Start(fullFilePath, 1, 9, rexxArgs);
//			showcolonization = NULL;
//			SetActiveWindow(GetParent(hwnd));
//			DestroyWindow(hwnd);
			ShowWindow(showcolonization,SW_HIDE);
		}
	    return 0;
	  case IDCANCEL:
//		showcolonization = NULL;
		//say we closed out the zoc ask

//		SetActiveWindow(GetParent(hwnd));
//		DestroyWindow(hwnd);
		  ShowWindow(showcolonization,SW_HIDE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showcolonization = NULL;
	//say we closed out the zoc ask
//	SetActiveWindow(GetParent(hwnd));
//	DestroyWindow(hwnd);
	ShowWindow(showcolonization,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK RexxRequestProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char buf[256];
	HWND hList;
	int  index;

	switch (msg) {

	case WM_INITDIALOG:
	{
	    static int tabs[4] = { 78, 108 };
	    SendDlgItemMessage(hwnd, IDD_REXXREQUEST, LB_SETTABSTOPS, 2,
			       (LPARAM) tabs);
	}

	return 1;

      case WM_COMMAND:
	if (HIWORD (wParam)==LBN_DBLCLK)
	{
		LOWORD(wParam) = IDOK;
	}
	switch (LOWORD(wParam)) {
	  case IDOK:
			hList = GetDlgItem(hwnd, ID_LISTREXXREQUEST);

			index = SendMessage(hList, LB_GETCURSEL, 0, 0);

			SendMessage(hList, LB_GETTEXT, (WPARAM)index, (LPARAM)&buf);

			rexxrequest = NULL;

			rexxRequestResult = (char *) strdup(buf);
			SetActiveWindow(GetParent(hwnd));
			DestroyWindow(hwnd);
			ATTACScripting_SignalATTACComplete();
			
			return(0);
	  case IDCANCEL:
	    rexxrequest = NULL;
		rexxRequestResult = (char *) strdup("##CANCEL##");
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
		ATTACScripting_SignalATTACComplete();

		return 0;
	}
	return 0;
      case WM_CLOSE:
	rexxrequest = NULL;
	rexxRequestResult = (char *) strdup("");
	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	ATTACScripting_SignalATTACComplete();
	return 0;
    }
    return 0;
}

static int CALLBACK RexxRequestListProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char buf[256];
	HWND hList;
	int  index;

    switch (msg) {
      case WM_INITDIALOG:
	{
	    static int tabs[4] = { 78, 108 };
	    SendDlgItemMessage(hwnd, IDD_REXXREQUEST, LB_SETTABSTOPS, 2,
			       (LPARAM) tabs);
	}

	return 1;
      case WM_COMMAND:
	if (HIWORD (wParam)==LBN_DBLCLK)
	{
		LOWORD(wParam) = IDOK;
	}

	switch (LOWORD(wParam)) {
	  case IDOK:
			hList = GetDlgItem(hwnd, ID_LISTREXXREQUEST);

			index = SendMessage(hList, LB_GETCURSEL, 0, 0);
			sprintf (buf, "%d", index);
			rexxrequest = NULL;

			rexxRequestResult = (char *) strdup(buf);
			SetActiveWindow(GetParent(hwnd));
			DestroyWindow(hwnd);
			ATTACScripting_SignalATTACComplete();
			return(0);
	  case IDCANCEL:
	    rexxrequest = NULL;
		rexxRequestResult = (char *) strdup("-1");
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
		ATTACScripting_SignalATTACComplete();
		return 0;
	}
	return 0;
      case WM_CLOSE:
	rexxrequest = NULL;
	rexxRequestResult = (char *) strdup("");
	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	ATTACScripting_SignalATTACComplete();	
	return 0;
    }
    return 0;
}

static int CALLBACK RegistrationProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char buffer[256];
	char *userID1, *userID2, *userID3, *userID4, *regCode, *twxRegCode;
	FILE *fp;

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		GetDlgItemText(hwnd, IDC_REGISTRATION_ID1, buffer,  sizeof(buffer) - 1);
		userID1 = (char *) strdup (buffer);
		GetDlgItemText(hwnd, IDC_REGISTRATION_ID2, buffer,  sizeof(buffer) - 1);
		userID2 = (char *) strdup (buffer);
		GetDlgItemText(hwnd, IDC_REGISTRATION_ID3, buffer,  sizeof(buffer) - 1);
		userID3 = (char *) strdup (buffer);
		GetDlgItemText(hwnd, IDC_REGISTRATION_ID4, buffer,  sizeof(buffer) - 1);
		userID4 = (char *) strdup (buffer);
		GetDlgItemText(hwnd, IDC_REGISTRATION_CODE, buffer,  sizeof(buffer) - 1);
		regCode = (char *) strdup (buffer);
		GetDlgItemText(hwnd, IDC_REGISTRATION_TWXSYNC_CODE, buffer,  sizeof(buffer) - 1);
		twxRegCode = (char *) strdup (buffer);

		

		if (isValidRegCode(userID1,userID2,userID3,userID4,regCode, twxRegCode, 0, "")==1)
		{
			validRegCode = 1;
			isRegistered = 0;

			strcpy (fullFilePath, appDir);
			strcat (fullFilePath, "reg.txt");
			//create reg.txt file
			fp = fopen (fullFilePath, "w");
			if (fp ==NULL)
				printf ("Error opening %s to create registration\n", fullFilePath);
			else
			{
				fprintf (fp, "%s", userID1);
				fprintf (fp, "*");
				if (strcmp(userID2, "") != 0)
					fprintf (fp, "%s", userID2);
				else
					fprintf (fp, "-1");
				fprintf (fp, "*");
				if (strcmp(userID3, "") != 0)
					fprintf (fp, "%s", userID3);
				else
					fprintf (fp, "-1");
				fprintf (fp, "*");
				if (strcmp(userID4, "") != 0)
					fprintf (fp, "%s", userID4);
				else
					fprintf (fp, "-1");
				fprintf (fp, "*");
				fprintf (fp, "%s", regCode);
				fprintf (fp, "*");
				if (strcmp(twxRegCode, "") != 0)
					fprintf (fp, "%s", twxRegCode);
				else
					fprintf (fp, "-1");
				
				fprintf (fp, "*");
				fclose(fp);
			}
		}
		else
		{
			validRegCode = 0;
			isRegistered = 0;
			validTWXSyncRegCode = 0;
		}

		#ifdef P_FREE
			pfree(&userID1);
			pfree(&userID2);
			pfree(&userID3);
			pfree(&userID4);
			pfree(&regCode);
			pfree(&twxRegCode);
		#else
			free(userID1);
			free(userID2);
			free(userID3);
			free(userID4);
			free(regCode);
			free(twxRegCode);
		#endif
		

		showregistration = NULL;
		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
		return(0);
	  case IDCANCEL:
	    showregistration = NULL;
		if (validRegCode == 0)
		{
			isRegistered = 0;
			MessageBox(hwnd, "This is an unregistered version of ATTAC. Email tradewars@hotmail.com for registration information.", "ATTAC Registration", MB_OK);
		}
		else
			MessageBox(hwnd, "No changes made to registration information", "ATTAC Registration", MB_OK);

		SetActiveWindow(GetParent(hwnd));
	    DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showregistration = NULL;
	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


static int CALLBACK SetAlliesProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char trader[256];
	char status[50];
	int i;
	HWND hShowAllied;
    LV_DISPINFO *lvinfo;
    NM_LISTVIEW *pNMLV;
    SORTDATA SortData;
    int isortsubno;
	int nItem = -1;
	char data[128];

    switch (msg) {
      case WM_INITDIALOG:

		hShowAllied = GetDlgItem(hwnd, IDC_ALLIED_LIST);

		listview_insert_column (hShowAllied, 0, 200, 0, "Trader");
		listview_insert_column (hShowAllied, 1, 50, 1, "Status");
		loadAlliedList(hShowAllied);
		
	return 1;
      case WM_NOTIFY:
		if ((int)wParam == IDC_ALLIED_LIST) 
			{
				hShowAllied = GetDlgItem(hwnd, IDC_ALLIED_LIST);
                lvinfo = (LV_DISPINFO *)lParam;
                switch (lvinfo->hdr.code) 
				{
					case NM_CLICK:

						nItem = ListView_GetNextItem(hShowAllied, nItem, LVNI_SELECTED);
						ListView_GetItemText(hShowAllied, nItem, 0, data, sizeof(data));
						SetDlgItemText(hwnd, IDC_ALLIED_TRADER, data);
						break;
                    case LVN_COLUMNCLICK:

                        pNMLV = (NM_LISTVIEW *)lParam;
                        isortsubno = pNMLV->iSubItem;

                        if (SortSubno[isortsubno] == UP)
                            SortSubno[isortsubno] = DOWN;
                        else
                            SortSubno[isortsubno] = UP;
                        SortData.hwndList = hShowAllied;
                        SortData.isortSubItem = isortsubno;
                        SortData.iUPDOWN = SortSubno[isortsubno];
                        if(ListView_SortItems(hShowAllied, MyCompProc, (LPARAM) &SortData) != TRUE)
                            MessageBox(hwnd, "Allied List Sorting Error", "Error", MB_OK);
                        break;
                    

                }
            }
		return 1;

      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
//	    index = SendDlgItemMessage(hwnd, IDC_SWITCH_DATABASE_NAME, CB_GETCURSEL, 0, 0);
//	    SendDlgItemMessage(hwnd, IDC_SWITCH_DATABASE_NAME, CB_GETLBTEXT, index, (LPARAM)buffer);
		GetDlgItemText(hwnd, IDC_ALLIED_TRADER, trader,  sizeof(trader) - 1);
		GetDlgItemText(hwnd, IDC_ALLIED_STATUS, status,  sizeof(status) - 1);

		printf ("traders: %s\n", trader);
		printf ("status: %s\n", status);

		for (i=0; i<MAX_PLAYERS; i++)
		{
			if (players[i].autonum >=0 && strcmp (trader, players[i].name)==0)
			{
				if (strcmp (status, "Enemy")==0)
					players[i].alliance='1';
				else 
					players[i].alliance='0';
			}
		}

		hShowAllied = GetDlgItem(hwnd, IDC_ALLIED_LIST);
		loadAlliedList(hShowAllied);
		
		writeMemory();

		return(0);
	  case IDCANCEL:
	    setallies = NULL;
	    SetActiveWindow(GetParent(hwnd));
	    DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	setallies = NULL;
	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}

void loadAlliedList(HWND AlliedHwnd)
{
	int i, allied_count=0;

	ListView_DeleteAllItems (AlliedHwnd);
	
	for (i=0; i<MAX_PLAYERS; i++)
	{
		if (players[i].autonum >=0)
		{
			if (players[i].alliance=='0')
			{
				listview_insert_allied(AlliedHwnd, allied_count++, players[i].name, "Allied");
			}
			else
			{
				listview_insert_allied(AlliedHwnd, allied_count++, players[i].name, "Enemy");
			}
		}
	}
}

void loadPlanetList(HWND PlanetHwnd)
{
	int i, j, planet_count=0, tmpInt=0;
	char buffer[256], player[44];
	char *tmpStr, *playerStr;

	ListView_DeleteAllItems (PlanetHwnd);
	
	for (i=0; i<header.sector_cnt; i++)
	{
		if (listPlanets[i] != NULL)
		{
			tmpInt = get_link_list_length(i);
			for (j=0; j<tmpInt; j++)
			{
				sprintf (buffer, "%d", i+1);

				if (!isValidSectorZeroOffset(i)) {
					continue;
				}

				tmpStr = (char *) get_link_list_nth_data (i, j);
				if (sectors[i].ftr_owner>=0)
				{
					if (sectors[i].ftr_owner < CORP_CONSTANT)
					{
						if (sectors[i].ftr_owner==0)
							strcpy (player, "ME");
						else
							strcpy (player, players[sectors[i].ftr_owner].name);
					}
					else
					{
						if ((sectors[i].ftr_owner-CORP_CONSTANT)==players[0].corpNumber)
							sprintf (player, "Corp #%d (MY CORP)", (sectors[i].ftr_owner-CORP_CONSTANT));		
						else
							sprintf (player, "Corp #%d", (sectors[i].ftr_owner-CORP_CONSTANT));
					}
					playerStr = (char *) strdup (player);
				}
				else
				{
					if (i==0)
						playerStr = (char *) strdup ("Federation Planet");
					else
						playerStr = (char *) strdup ("Unknown");			
				}

				listview_insert_planet(PlanetHwnd, planet_count++, buffer, tmpStr, playerStr);
			
				#ifdef P_FREE
					pfree(&tmpStr);
					pfree(&playerStr);
				#else
					free(tmpStr);
					free(playerStr);
				#endif
			}
		}
	}
}





static int CALLBACK SwitchDatabaseProc(HWND hwnd, UINT msg,
			    WPARAM wParam, LPARAM lParam)
{
	char buffer[256];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		GetDlgItemText(hwnd, IDC_SWITCH_DATABASE_NAME, buffer,  sizeof(buffer) - 1);

		strcpy (gameName, buffer);

		if (strstr(gameName, ".atc")==NULL)
			strcat (gameName, ".atc");

		strcpy (fullFilePath, appDir);
		strcat (fullFilePath, gameName);

		if (load_twx(fullFilePath)==ERROR_FILE_ACCESS)
		{
			MessageBox(hwnd, "The Database file was not found.", "Game File Not Found", MB_OK);
		}

		clearBustsFunc();

		writeSetReady();

		showswitchdatabase = NULL;

		if (isRegistered)
		{
			set_title(NULL, "ATTAC (Registered)");
		}
		else
		{
			set_title(NULL, "ATTAC (Unregistered)");
		}

		SetActiveWindow(GetParent(hwnd));
		DestroyWindow(hwnd);
		return(0);
	  case IDCANCEL:
	    showswitchdatabase = NULL;
	    SetActiveWindow(GetParent(hwnd));
	    DestroyWindow(hwnd);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	showswitchdatabase = NULL;
	SetActiveWindow(GetParent(hwnd));
	DestroyWindow(hwnd);
	return 0;
    }
    return 0;
}


static int CALLBACK SectorInfoProc(HWND hwnd, UINT msg,
				WPARAM wParam, LPARAM lParam)
{

	long sector;
	char buffer[20], *buffer1;
	int i;

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:

		GetDlgItemText(hwnd, IDC_SECTOR_INFO_SECTOR, buffer, sizeof(buffer) - 1);
		sector = atol(buffer);
		//start show sector

		//break out if the sector is invalid
		if (!isValidSectorOneOffset(sector))
		{
			attacErrorMessage ("Invalid Sector");
			return 1;
		}

		  buffer1 = (char *) malloc(200);
		  addToMainWindow("", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
		  addToMainWindow("", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
		  sprintf(buffer1, "Sector  ");
		  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
		  sprintf(buffer1, ":");
		  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
		  sprintf(buffer1, " %ld", sector);
		  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
		  sprintf(buffer1, " in");
		  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
		  sprintf(buffer1, " uncharted space");
		  addToMainWindow(buffer1, ATTAC_BLUE, ATTAC_BLACK, TRUE, 1);
		  if ((sector>=1 && sector <=10) || sector == header.stardock)
		  {
			  sprintf(buffer1, "Beacon  ");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, ":");
			  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " FedSpace, FedLaw Enforced");
			  addToMainWindow(buffer1, ATTAC_RED, ATTAC_BLACK, TRUE, 1);

		  }
		  if (sectors[sector-1].info >=0 && sectors[sector-1].info != 10 && sectors[sector-1].info != 11 && sectors[sector-1].info != 12)
		  {
			  sprintf(buffer1, "Ports   ");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, ":");
			  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " Class");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " %d", sectors[sector-1].info > 9 ?sectors[sector-1].info-UNDER_CONSTRUCTION:sectors[sector-1].info);
			  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " (");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  switch (sectors[sector-1].info)
			  {
				  case 1:
				  case 1+UNDER_CONSTRUCTION:
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 2:
				  case 2+UNDER_CONSTRUCTION:
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 3:
				  case 3+UNDER_CONSTRUCTION:
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 4:
				  case 4+UNDER_CONSTRUCTION:
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 5:
				  case 5+UNDER_CONSTRUCTION:
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 6:
				  case 6+UNDER_CONSTRUCTION:
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 7:
				  case 7+UNDER_CONSTRUCTION:
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "S");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 8:
				  case 8+UNDER_CONSTRUCTION:
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					sprintf(buffer1, "B");
			  		addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
					break;
				  case 9:
				  case 0:
					sprintf(buffer1, "Special");
			  		addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
					break;
			  }
			  sprintf(buffer1, ")");
			  if (sectors[sector-1].info>12)
			  {
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, " (Under Construction)");
				addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 1);
			  }
			  else if (sectors[sector-1].info==9)
			  {
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, " (Stardock)");
				addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 1);
			  }
			  else if (sector==header.cls0port_sol)
			  {
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, " (Sol)");
				addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 1);
			  }
			  else if (sector==header.cls0port_alpha)
			  {
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, " (Alpha Centauri)");
				addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 1);
			  }
			  else if (sector==header.cls0port_rylos)
			  {
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, " (Rylos)");
				addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 1);
			  }
			  else
				  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 1);

			  sprintf(buffer1, " ");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 1);

			  if (sectors[sector-1].info >0)
			  {
				  sprintf(buffer1, "       Items     Status  Trading %% of max");
				  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
				  sprintf(buffer1, "       -----     ------  ------- --------");
				  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 1);

				  sprintf(buffer1, "      Fuel Ore   ");
				  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%s", sectors[sector-1].info==1 || sectors[sector-1].info==2 || sectors[sector-1].info==6 || sectors[sector-1].info==8 || sectors[sector-1].info==13 || sectors[sector-1].info==14 || sectors[sector-1].info==18 || sectors[sector-1].info==20 || sectors[sector-1].info==9?"Buying   ":"Selling  ");
				  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%5ld", sectors[sector-1].port_amt[0]);
				  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "   %3d", sectors[sector-1].port_per[0]);
				  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%%");
				  addToMainWindow(buffer1, ATTAC_RED, ATTAC_BLACK, TRUE, 1);
				  
				  sprintf(buffer1, "      Organics   ");
				  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%s", sectors[sector-1].info==1 || sectors[sector-1].info==3 || sectors[sector-1].info==5 || sectors[sector-1].info==8 || sectors[sector-1].info==13 || sectors[sector-1].info==15 || sectors[sector-1].info==17 || sectors[sector-1].info==20 || sectors[sector-1].info==9?"Buying   ":"Selling  ");
				  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%5ld", sectors[sector-1].port_amt[1]);
				  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "   %3d", sectors[sector-1].port_per[1]);
				  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%%");
				  addToMainWindow(buffer1, ATTAC_RED, ATTAC_BLACK, TRUE, 1);
				  
				  sprintf(buffer1, "      Equipment  ");
				  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%s", sectors[sector-1].info==2 || sectors[sector-1].info==3 || sectors[sector-1].info==4 || sectors[sector-1].info==8 || sectors[sector-1].info==14 || sectors[sector-1].info==15 || sectors[sector-1].info==16 || sectors[sector-1].info==20 || sectors[sector-1].info==9?"Buying   ":"Selling  ");
				  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%5ld", sectors[sector-1].port_amt[2]);
				  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "   %3d", sectors[sector-1].port_per[2]);
				  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
				  sprintf(buffer1, "%%");
				  addToMainWindow(buffer1, ATTAC_RED, ATTAC_BLACK, TRUE, 1);
				  sprintf(buffer1, " ");
				  addToMainWindow(buffer1, ATTAC_RED, ATTAC_BLACK, TRUE, 1);
			  }
		  }

		  if (sectors[sector-1].ftrs>0)
		  {
			  sprintf(buffer1, "Fighters");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, ":");
			  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " %ld", sectors[sector-1].ftrs);
			  addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
			  if (sectors[sector-1].ftr_owner==0)
				sprintf(buffer1, " (yours)");
			  else if (sectors[sector-1].ftr_owner-CORP_CONSTANT==players[0].corpNumber)
				sprintf(buffer1, " (belong to your Corp)");
			  else if (sectors[sector-1].ftr_owner>CORP_CONSTANT)
				sprintf(buffer1, " (Belongs to Corp %d)", sectors[sector-1].ftr_owner-CORP_CONSTANT);
			  else
				sprintf(buffer1, " (Belongs to %s)", players[sectors[sector-1].ftr_owner].name);
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  switch (sectors[sector-1].ftr_type)
			  {
			  case 1:
				  sprintf(buffer1, "  [Toll]");
				  break;
			  case 2:
				  sprintf(buffer1, "  [Offensive]");
				  break;
			  case 3:
				  sprintf(buffer1, "  [Defensive]");
				  break;
			  case 4:
				  sprintf(buffer1, "  [Rogue Mercenary]");
				  break;
			  case 5:
				  sprintf(buffer1, "  [Unknown]");
				  break;
			  }
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 1);

		  }

		  if (sectors[sector-1].armids>0)
		  {
			  sprintf(buffer1, "Mines   ");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, ":");
			  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " %d", sectors[sector-1].armids);
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);

			  sprintf(buffer1, " (");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, "Type");
			  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " 1 ");
			  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, "Armid");
			  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, ")");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 1);
		  }
		  if (sectors[sector-1].limpets>0)
		  {
			  if (sectors[sector-1].armids<=0)
			  {
				sprintf(buffer1, "Mines   ");
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, ":");
				addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  }
			  else
			  {
				sprintf(buffer1, "        ");
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, ":");
				addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  }
				
			  sprintf(buffer1, " %d", sectors[sector-1].limpets);
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);

			  sprintf(buffer1, " (");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, "Type");
			  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, " 2 ");
			  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, "Limpit");
			  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
			  sprintf(buffer1, ")");
			  addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 1);

		  }


		  sprintf(buffer1, "Warps to Sector(s) :  ");
		  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);

		  if (sectors[sector-1].warps==0)
		  {
			  sprintf(buffer1, "No Known Warps");
			  addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
		  }
		  for (i=0; i < sectors[sector-1].warps; i++)
		  {
			if (sectors[sectors[sector-1].warp_sect[i]-1].warp_sect[0]==0)  //unexploATTAC_RED
			{
				sprintf(buffer1, "(");
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, "%ld", sectors[sector-1].warp_sect[i]);
				addToMainWindow(buffer1, ATTAC_RED, ATTAC_BLACK, TRUE, 0);
				sprintf(buffer1, ")");
				addToMainWindow(buffer1, ATTAC_MAGENTA, ATTAC_BLACK, TRUE, 0);

			}
			else		//exploATTAC_RED
			{
				sprintf(buffer1, "%ld", sectors[sector-1].warp_sect[i]);
				addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
			}
			if (i != sectors[sector-1].warps-1)
			{
				sprintf(buffer1, " - ");
				addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 0);
			}
		  }
		  
		  sprintf(buffer1, " ");
		  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 1);
		  addToMainWindow(buffer1, ATTAC_YELLOW, ATTAC_BLACK, TRUE, 1);

		#ifdef P_FREE
			pfree(&buffer1);
		#else
			free(buffer1);
		#endif
		//end show sector
		showsectorinfo=NULL;
		SetActiveWindow(GetParent(hwnd));
	    EndDialog(hwnd, 1);
	    return 0;
	  case IDCANCEL:
	    EndDialog(hwnd, TRUE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
		showsectorinfo=NULL;
		SetActiveWindow(GetParent(hwnd));
		EndDialog(hwnd, 1);
	return 0;
    }


	return 0;
}

static int CALLBACK LicenceProc(HWND hwnd, UINT msg,
				WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
	    EndDialog(hwnd, 1);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	EndDialog(hwnd, 1);
	return 0;
    }
    return 0;
}

static int CALLBACK FedCommProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char temp[MAX_LENGTH], *Msg, *tmp, *tmp1;
	char buffer[MAX_LENGTH-5], buffer1[MAX_LENGTH-5];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		
		GetDlgItemText(hwnd, IDC_FEDCOMM_EDIT, buffer,  sizeof(buffer) - 1);
		strcpy (buffer1, buffer);
		if (IsDlgButtonChecked(hwnd, IDC_FEDCOMM_ENCRYPT))
		{
			if (strlen(buffer)<=53)
			{
		        attac_send_char('`');
				tmp1 = (char*) strdup (players[0].name);
				tmp = EncryptCommMessage(buffer1, tmp1);
				#ifdef P_FREE
					pfree(&tmp1);
				#else
					free(tmp1);
				#endif
			}
			else
			{
				MessageBox(hwnd, "Encrypted Communication Can be a maximum of 53 characters", "ATTAC Error", MB_OK | MB_ICONERROR);
				return 0;
			}
		}
		else
		{
	        attac_send_char('`');
			tmp = (char *) strdup (buffer);
		}

		strcpy(temp, tmp);	
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif

		if (IsDlgButtonChecked(hwnd, IDC_FEDCOMM_ENCRYPT))
		{
			tmp1 = (char *) malloc (strlen (buffer) + strlen ("[Encrypted] ") +1);
			strcpy (tmp1, "[Encrypted] ");
			strcat (tmp1, buffer);
			strcpy (buffer, tmp1);
			#ifdef P_FREE
				pfree(&tmp1);
			#else
				free(tmp1);
			#endif
		}

		attacFedCommMessage(buffer, "ME");

		//add a CR LF to the end of the Msg
		buffer[0]=13;
		buffer[1]=10;
		buffer[2]=0;
		strcat (temp, buffer);

        Msg = Translate_String(temp, 0, 0);

		attac_send(Msg);

		#ifdef P_FREE
			pfree(&Msg);
		#else
			free(Msg);
		#endif
		  
		showfedcomm=NULL;
		SetActiveWindow(GetParent(hwnd));
	    EndDialog(hwnd, 1);
		isFedCommVisible=0;
	    return 0;
	  case IDCANCEL:
//		showfedcomm=NULL;
//		SetActiveWindow(GetParent(hwnd));
//	    EndDialog(hwnd, TRUE);
		ShowWindow(showfedcomm,SW_HIDE);
		isFedCommVisible=0;
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showfedcomm=NULL;
//	SetActiveWindow(GetParent(hwnd));
//	EndDialog(hwnd, 1);
	ShowWindow(showfedcomm,SW_HIDE);
	isFedCommVisible=0;
	return 0;
    }
    return 0;
}


static int CALLBACK PrivateCommProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char temp[MAX_LENGTH], *Msg, *tmp, *tmp1;
	char buffer[MAX_LENGTH-5], buffer1[MAX_LENGTH-5];

    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
		
		GetDlgItemText(hwnd, IDC_PRIVATECOMM_EDIT, buffer,  sizeof(buffer) - 1);

		strcpy (buffer1, buffer);
		if (IsDlgButtonChecked(hwnd, IDC_PRIVATECOMM_ENCRYPT))
		{
			if (strlen(buffer)<=53)
			{
		        attac_send_char('\'');
				tmp1 = (char*) strdup (players[0].name);
				tmp = EncryptCommMessage(buffer1, tmp1);
				#ifdef P_FREE
					pfree(&tmp1);
				#else
					free(tmp1);
				#endif
			}
			else
			{
				MessageBox(hwnd, "Encrypted Communication Can be a maximum of 53 characters", "ATTAC Error", MB_OK | MB_ICONERROR);
				return 0;
			}
		}
		else
		{
	        attac_send_char('\'');
			tmp = (char *) strdup (buffer);
		}

		strcpy(temp, tmp);	
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif


		if (IsDlgButtonChecked(hwnd, IDC_PRIVATECOMM_ENCRYPT))
		{
			tmp1 = (char *) malloc (strlen (buffer) + strlen ("[Encrypted] ") +1);
			strcpy (tmp1, "[Encrypted] ");
			strcat (tmp1, buffer);
			strcpy (buffer, tmp1);
			#ifdef P_FREE
				pfree(&tmp1);
			#else
				free(tmp1);
			#endif

		}
		attacSubSpaceMessage(buffer, "ME", -1);

		//add a CR LF to the end of the Msg
		buffer[0]=13;
		buffer[1]=10;
		buffer[2]=0;
		strcat (temp, buffer);

        Msg = Translate_String(temp, 0, 0);

		attac_send(Msg);

		#ifdef P_FREE
			pfree(&Msg);
		#else
			free(Msg);
		#endif


		showprivatecomm=NULL;
		SetActiveWindow(GetParent(hwnd));
	    EndDialog(hwnd, 1);
		isPrivateCommVisible=0;
	    return 0;
	  case IDCANCEL:
		ShowWindow(showprivatecomm,SW_HIDE);
		isPrivateCommVisible=0;
//		showprivatecomm=NULL;
//		SetActiveWindow(GetParent(hwnd));
//	    EndDialog(hwnd, TRUE);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
//	showprivatecomm=NULL;
//	SetActiveWindow(GetParent(hwnd));
//	EndDialog(hwnd, 1);
	isPrivateCommVisible=0;
	ShowWindow(showprivatecomm,SW_HIDE);
	return 0;
    }
    return 0;
}

static int CALLBACK ATTACLicenceProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
      case WM_INITDIALOG:
	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
	    EndDialog(hwnd, 1);
	    return 0;
	  case IDPUTTY:
	    EnableWindow(hwnd, 0);
	    DialogBox(hinst, MAKEINTRESOURCE(IDD_LICENCEBOX), NULL, LicenceProc);
	    EnableWindow(hwnd, 1);
	    SetActiveWindow(hwnd);
		return 0;
	}
	return 0;
      case WM_CLOSE:
	EndDialog(hwnd, 1);
	return 0;
    }
    return 0;
}



int CALLBACK AboutProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HBITMAP hBMP;


    switch (msg) {
      case WM_INITDIALOG:
	SetDlgItemText(hwnd, IDA_VERSION, ver);
	SetDlgItemText(hwnd, IDA_VERSIONREGINA, reginaver);
	
	hBMP=LoadBitmap(hinst,MAKEINTRESOURCE(IDB_POWEREDBYREXX));

	SendMessage(GetDlgItem(hwnd, IDC_POWEREDBYREXX), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBMP);

//	DeleteObject(hBMP);                     // free memory!

	return 1;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	  case IDOK:
	  case IDCANCEL:
	    EndDialog(hwnd, TRUE);
	    return 0;
	  case IDA_LICENCE:
	    EnableWindow(hwnd, 0);
	    DialogBox(hinst, MAKEINTRESOURCE(IDD_ATTAC_LICENSE  ), NULL, ATTACLicenceProc);
	    EnableWindow(hwnd, 1);
	    SetActiveWindow(hwnd);
	    return 0;

	  case IDA_WEB:
	    /* Load web browser */
	    ShellExecute(hwnd, "open",
			 "http://www.tw-attac.com",
			 0, 0, SW_SHOWDEFAULT);
	    return 0;
	  case IDC_POWEREDBYREXX:
	    /* Load web browser */
	    ShellExecute(hwnd, "open",
			 "http://regina-rexx.sourceforge.net/",
			 0, 0, SW_SHOWDEFAULT);
	    return 0;
	}
	return 0;
      case WM_CLOSE:
	EndDialog(hwnd, TRUE);
	return 0;
    }
    return 0;
}




void showztmform(HWND hwnd)
{
	HANDLE hProgress;

	if (!showztm) 
	{
		showztm = CreateDialog(hinst, MAKEINTRESOURCE(IDD_ZTM), hwnd, ZTMProc);
		
		if ( showztm!=NULL ) 
		{ 
			hProgress = GetDlgItem(showztm, IDC_ZTM_PROGRESS);
			// Set the range and increment of the progress bar. 
			SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
			SendMessage(hProgress, PBM_SETSTEP, (WPARAM) 1, 0);

			ShowWindow(showztm,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window ZTM", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(showztm, SW_SHOWNORMAL); 

    SetActiveWindow(showztm);
}

void showpaytollsform(HWND hwnd)
{
	if (!showpaytolls) 
	{
		showpaytolls = CreateDialog(hinst, MAKEINTRESOURCE(IDD_TOLL_OPTIONS), hwnd, PayTollsProc);
		
		if ( showpaytolls!=NULL ) 
		{ 
			ShowWindow(showpaytolls,SW_SHOWNORMAL); 
//			printf ("TOLL: %d\n", header.tollTolls);
		    CheckRadioButton(showpaytolls, IDC_TOLL_OPTIONS_TOLL_PAY, IDC_TOLL_OPTIONS_TOLL_NOTHING,
									header.tollTolls == TOLLSFIGPAY ? IDC_TOLL_OPTIONS_TOLL_PAY :
									header.tollTolls == TOLLSFIGKILL ? IDC_TOLL_OPTIONS_TOLL_KILL : IDC_TOLL_OPTIONS_TOLL_NOTHING);

		    CheckRadioButton(showpaytolls, IDC_TOLL_OPTIONS_DEFENSIVE_KILL, IDC_TOLL_OPTIONS_DEFENSIVE_NOTHING,
									header.defensiveTolls == DEFENSIVEFIGKILL ? IDC_TOLL_OPTIONS_DEFENSIVE_KILL : IDC_TOLL_OPTIONS_DEFENSIVE_NOTHING);
									
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Pay Tolls", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(showpaytolls, SW_SHOWNORMAL); 

    SetActiveWindow(showpaytolls);
}

void showdeadendform(HWND hwnd)
{
	HANDLE hlist, htab;
	TC_ITEM item;

	if (!showdeadends) 
	{
		showdeadends = CreateDialog(hinst, MAKEINTRESOURCE(IDD_DEADEND), hwnd, ShowDeadEndProc);
		
		if ( showdeadends!=NULL ) 
		{ 
			htab = GetDlgItem(showdeadends, IDC_DEADEND_TAB);
			// to use an image set this to TCIF_IMAGE  
			item.mask= TCIF_TEXT; 
			// index into tabs image list, set to -1 when not in use
			item.iImage = -1;
			item.pszText ="1 Sector";
			TabCtrl_InsertItem(htab,0,&item);
			item.pszText ="2 Sector";
			TabCtrl_InsertItem(htab, 1, &item);
			item.pszText ="3 Sector";
			TabCtrl_InsertItem(htab, 2, &item);
			item.pszText ="4 Sector";
			TabCtrl_InsertItem(htab, 3, &item);
			item.pszText ="5 Sector";
			TabCtrl_InsertItem(htab, 4, &item);
			item.pszText ="6 Sector";
			TabCtrl_InsertItem(htab, 5, &item);
			item.pszText ="7 Sector";
			TabCtrl_InsertItem(htab, 6, &item);

			hlist = GetDlgItem(showdeadends, IDC_DEADEND_LIST);

			listview_insert_column (hlist, 0, 500, 0, "Dead Ends");
			showDeadEndData(hlist, 1);

			ShowWindow(showdeadends,SW_SHOWNORMAL); 

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Dead Ends", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showdeadends);
}

void showDeadEndData(HWND hdeadEndList, int depth)
{
	MAP_INFO *deadEndInfo;
	char buffer[128];
	long warpOneSector=0, warpTwoSector=0;
	long pSector=0, w1=0, w2=0, w3=0, w4=0, w5=0, w6=0, w7=0;
	long t1=0, t2=0, t3=0, t4=0, t5=0, t6=0, t7=0;
	char *tmp[1], title[64];  
	int i;
	int deadendcount=0;

	deadEndInfo = (MAP_INFO *) malloc((header.sector_cnt+1) * sizeof(MAP_INFO));

	ListView_DeleteAllItems (hdeadEndList);

	/* starting sector */
	for(i=0;isValidSectorZeroOffset(i) && i<header.sector_cnt;i++) 
	{
		deadEndInfo[i+1].sector=i+1;
		deadEndInfo[i+1].warps=sectors[i].warps;
		deadEndInfo[i+1].warpOne=sectors[i].warp_sect[0];
		deadEndInfo[i+1].warpTwo=sectors[i].warp_sect[1];
		deadEndInfo[i+1].warpThree=sectors[i].warp_sect[2];
		deadEndInfo[i+1].warpFour=sectors[i].warp_sect[3];
		deadEndInfo[i+1].warpFive=sectors[i].warp_sect[4];
		deadEndInfo[i+1].warpSix=sectors[i].warp_sect[5];
	}


	for (i=0; i< header.sector_cnt; i++)
	{
		if (deadEndInfo[i].sector != 0)
		{
			if (deadEndInfo[i].warps==1)
			{
				pSector = deadEndInfo[i].sector;
				t1++;

				/* now check if it is a 2 sector dead end */
				w1 = deadEndInfo[i].warpOne;
				if  (deadEndInfo[w1].warps==2)
				{
					t2++;
					warpOneSector = deadEndInfo[w1].warpOne;
					warpTwoSector = deadEndInfo[w1].warpTwo;
					if (pSector != warpOneSector)
					{
						w2 = warpOneSector;
					}
					else
					{
						w2 = warpTwoSector;
					}
				}
				pSector = w1;		  
				/* now check if it is a 3 sector dead end */
				if  (deadEndInfo[w2].warps==2)
				{
					t3++;
					warpOneSector = deadEndInfo[w2].warpOne;
					warpTwoSector = deadEndInfo[w2].warpTwo;
					if (pSector != warpOneSector)
					{
						w3 = warpOneSector;
					}
					else
					{
						w3 = warpTwoSector;
					}
				}
				pSector = w2;
				/* now check if it is a 4 sector dead end */
				if  (deadEndInfo[w3].warps==2)
				{
					t4++;
					warpOneSector = deadEndInfo[w3].warpOne;
					warpTwoSector = deadEndInfo[w3].warpTwo;
					if (pSector != warpOneSector)
					{
						w4 = warpOneSector;
					}
					else
					{
						w4 = warpTwoSector;
					}
				}

				pSector = w3;
				/* now check if it is a 5 sector dead end */
				if  (deadEndInfo[w4].warps==2)
				{
					t5++;
					warpOneSector = deadEndInfo[w4].warpOne;
					warpTwoSector = deadEndInfo[w4].warpTwo;
					if (pSector != warpOneSector)
					{
						w5 = warpOneSector;
					}
					else
					{
						w5 = warpTwoSector;
					}
				}

				pSector = w4;
				/* now check if it is a 6 sector dead end */
				if  (deadEndInfo[w5].warps==2)
				{
					t6++;
					warpOneSector = deadEndInfo[w5].warpOne;
					warpTwoSector = deadEndInfo[w5].warpTwo;
					if (pSector != warpOneSector)
					{
						w6 = warpOneSector;
					}
					else
					{
						w6 = warpTwoSector;
					}
				}

				pSector = w5;
				/* now check if it is a 7 sector dead end */
				if  (deadEndInfo[w6].warps==2)
				{
					t7++;
					warpOneSector = deadEndInfo[w6].warpOne;
					warpTwoSector = deadEndInfo[w6].warpTwo;
					if (pSector != warpOneSector)
					{
						w7 = warpOneSector;
					}
					else
					{
						w7 = warpTwoSector;
					}
				}
			}
		}
		
		if (w1==0) {;}
		//do nothing since it is not a dead end
		else if (w1>0 && w2==0 && depth==1)  // 1 sector dead end
		{
			sprintf (title, "Number of Dead Ends: %ld", t1);
			SetDlgItemText(GetParent(hdeadEndList), IDC_DEADEND_COUNT_STATIC, title);
			sprintf (buffer, "%ld => %ld", 
			deadEndInfo[i].sector, w1);
			tmp[0] = (char *) strdup(buffer);		    
			listview_insert_deadend(hdeadEndList, deadendcount++, tmp[0]);
			#ifdef P_FREE
				pfree(&tmp[0]);
			#else
				free(tmp[0]);
			#endif
		}
		else if (w2>0 && w3==0 && depth==2)  // 2 sector dead end
		{
			sprintf (title, "Number of Dead Ends: %ld", t2);
			SetDlgItemText(GetParent(hdeadEndList), IDC_DEADEND_COUNT_STATIC, title);
			sprintf (buffer, "%ld => %ld => %ld",
			deadEndInfo[i].sector, w1, w2);
			tmp[0] = (char *) strdup(buffer);		    
			listview_insert_deadend(hdeadEndList, deadendcount++, tmp[0]);
			#ifdef P_FREE
				pfree(&tmp[0]);
			#else
				free(tmp[0]);
			#endif
		}
		else if (w3>0 && w4==0 && depth==3)  // 3 sector dead end
		{
			sprintf (title, "Number of Dead Ends: %ld", t3);
			SetDlgItemText(GetParent(hdeadEndList), IDC_DEADEND_COUNT_STATIC, title);
			sprintf (buffer, "%ld => %ld => %ld => %ld",
			deadEndInfo[i].sector, w1, w2, w3);
			tmp[0] = (char *) strdup(buffer);		    
			listview_insert_deadend(hdeadEndList, deadendcount++, tmp[0]);
			#ifdef P_FREE
				pfree(&tmp[0]);
			#else
				free(tmp[0]);
			#endif
		}
		else if (w4>0 && w5==0 && depth==4)  // 4 sector dead end
		{
			sprintf (title, "Number of Dead Ends: %ld", t4);
			SetDlgItemText(GetParent(hdeadEndList), IDC_DEADEND_COUNT_STATIC, title);
			sprintf (buffer, "%ld => %ld => %ld => %ld => %ld",
			deadEndInfo[i].sector, w1, w2, w3, w4);
			tmp[0] = (char *) strdup(buffer);		    
			listview_insert_deadend(hdeadEndList, deadendcount++, tmp[0]);
			#ifdef P_FREE
				pfree(&tmp[0]);
			#else
				free(tmp[0]);
			#endif
		}
		else if (w5>0 && w6==0 && depth==5)  // 5 sector dead end
		{
			sprintf (title, "Number of Dead Ends: %ld", t5);
			SetDlgItemText(GetParent(hdeadEndList), IDC_DEADEND_COUNT_STATIC, title);
			sprintf (buffer, "%ld => %ld => %ld => %ld => %ld => %ld", 
			deadEndInfo[i].sector, w1, w2, w3, w4, w5);
			tmp[0] = (char *) strdup(buffer);		    
			listview_insert_deadend(hdeadEndList, deadendcount++, tmp[0]);
			#ifdef P_FREE
				pfree(&tmp[0]);
			#else
				free(tmp[0]);
			#endif
		}
		else if (w6>0 && w7==0 && depth==6)  // 6 sector dead end
		{
			sprintf (title, "Number of Dead Ends: %ld", t6);
			SetDlgItemText(GetParent(hdeadEndList), IDC_DEADEND_COUNT_STATIC, title);
			sprintf (buffer, "%ld => %ld => %ld => %ld => %ld => %ld => %ld", 
			deadEndInfo[i].sector, w1, w2, w3, w4, w5, w6);
			tmp[0] = (char *) strdup(buffer);		    
			listview_insert_deadend(hdeadEndList, deadendcount++, tmp[0]);
			#ifdef P_FREE
				pfree(&tmp[0]);
			#else
				free(tmp[0]);
			#endif
		}
		else if (w7>0 && depth==7)           // 7 sector dead end
		{
			sprintf (title, "Number of Dead Ends: %ld", t7);
			SetDlgItemText(GetParent(hdeadEndList), IDC_DEADEND_COUNT_STATIC, title);
			sprintf (buffer, "%ld => %ld => %ld => %ld => %ld => %ld => %ld => %ld", 
			deadEndInfo[i].sector, w1, w2, w3, w4, w5, w6, w7);
			tmp[0] = (char *) strdup(buffer);		    
			listview_insert_deadend(hdeadEndList, deadendcount++, tmp[0]);
			#ifdef P_FREE
				pfree(&tmp[0]);
			#else
				free(tmp[0]);
			#endif
		}

		w1=0; w2=0; w3=0; w4=0; w5=0; w6=0; w7=0; pSector=0;

		//check for registration or restrict report
		//we multiply by 3 since there are three reports
		if ( isRegistered == 0 && ((t1+t2+t3+t4+t5+t6+t7) > (MAXCYCLES*3)))

		{
			if (validRegCode == FALSE || isConnected == TRUE)
				break;
		}
	}

	#ifdef P_FREE
		pfree(&deadEndInfo);
	#else
		free(deadEndInfo);
	#endif
}

void showblockedportform(HWND hwnd)
{
	HANDLE hlist;
	if (!showblockedport) 
	{
		showblockedport = CreateDialog(hinst, MAKEINTRESOURCE(IDD_BLOCKED_PORTS), hwnd, ShowBlockedPortProc);
		
		if ( showblockedport!=NULL ) 
		{ 
			hlist = GetDlgItem(showblockedport, IDC_BLOCKED_PORTS_LIST);
			listview_insert_column (hlist, 0, 50, 0, "Sector");
			listview_insert_column (hlist, 1, 195, 1, "Last Updated");

			showBlockedPortData(hlist);

			ShowWindow(showblockedport,SW_SHOWNORMAL); 

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Blocked Ports", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showblockedport);
}

void showBlockedPortData(HANDLE hlistBlockedPort)
{
	char *tmp[2];
	char buffer[100];
	long i;
	int showblockedportcount=0;
	time_t myTime;

	for (i=0; i<header.sector_cnt; i++)
	{
		if (isValidSectorZeroOffset(i) && sectors[i].port_update >0 && sectors[i].port_update < header.lastTimeCIMRun)
		{

			sprintf (buffer, "%ld", i+1);
			tmp[0] = (char *) strdup(buffer);		    

			//reformat for string with time function
			myTime = sectors[i].port_update;
			strcpy(buffer, ctime(&myTime));
			buffer[strlen(buffer)-1]=0;
			tmp[1] = (char *) strdup(buffer);

			listview_insert_blocked_port(hlistBlockedPort, showblockedportcount++, tmp[0], tmp[1]);
			#ifdef P_FREE
				pfree(&tmp[0]);
				pfree(&tmp[1]);
			#else
				free(tmp[0]);
				free(tmp[1]);
			#endif
		}
	}
}


void showfighterform(HWND hwnd)
{
	HANDLE hlist;
	if (!showfighter) 
	{
		showfighter = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SHOW_FIGHTERS), hwnd, ShowFighterProc);
		
		if ( showfighter!=NULL ) 
		{ 
			hlist = GetDlgItem(showfighter, IDC_FIGHTERS_LIST);
			listview_insert_column (hlist, 0, 50, 0, "Sector");
			listview_insert_column (hlist, 1, 50, 1, "Amount");
			listview_insert_column (hlist, 2, 170, 2, "Owner");
			listview_insert_column (hlist, 3, 100, 3, "Type");

			showFighterData(hlist);

			ShowWindow(showfighter,SW_SHOWNORMAL); 

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Fighter", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showfighter);
}



void showFighterData(HANDLE hlistFig)
{
	char *tmp[4];
	char buffer[100];
	long i;
	int showfightercount=0;


	for (i=0; isValidSectorZeroOffset(i) && i<header.sector_cnt; i++)
	{
		if ((long) sectors[i].ftrs >0)
		{
			sprintf (buffer, "%ld", i+1);
			tmp[0] = (char *) strdup(buffer);		    

			sprintf (buffer, "%ld", sectors[i].ftrs);
			tmp[1] = (char *) strdup(buffer);		    

			if (sectors[i].ftr_owner < CORP_CONSTANT)
			{
				if (sectors[i].ftr_owner<0)
					strcpy (buffer, "Unknown");
				else if (sectors[i].ftr_owner==0)
					strcpy (buffer, "ME");
				else
					strcpy (buffer, players[sectors[i].ftr_owner].name);
			}
			else
			{
				if ((sectors[i].ftr_owner-CORP_CONSTANT)==players[0].corpNumber)
					sprintf (buffer, "Corp #%d (MY CORP)", (sectors[i].ftr_owner-CORP_CONSTANT));		
				else
					sprintf (buffer, "Corp #%d", (sectors[i].ftr_owner-CORP_CONSTANT));
			}

			tmp[2] = (char *) strdup(buffer);

			switch (sectors[i].ftr_type)
			{
				case 1:
				strcpy (buffer, "Toll");
				break;
				case 2:
				strcpy(buffer, "Offensive");
				break;
				case 3:
				strcpy (buffer, "Defensive");
				break;
				case 4:
				strcpy (buffer, "Mercenary");
				break;
				default:
				strcpy (buffer, "Unknown");
				break;
			}

			tmp[3] = (char *) strdup(buffer);		    

			listview_insert_fighter(hlistFig, showfightercount++, tmp[0], tmp[1], tmp[2], tmp[3]);
			#ifdef P_FREE
				pfree(&tmp[0]);
				pfree(&tmp[1]);
				pfree(&tmp[2]);
				pfree(&tmp[3]);
			#else
				free(tmp[0]);
				free(tmp[1]);
				free(tmp[2]);
				free(tmp[3]);
			#endif
		}
	}


}

void showporttypeform(HWND hwnd, int type)
{
	HANDLE hlist;

	if (!showporttype) 
	{
		showporttype = CreateDialog(hinst, MAKEINTRESOURCE(IDD_PORTTYPE), hwnd, ShowPortTypeProc);
		
		if ( showporttype!=NULL ) 
		{ 
			hlist = GetDlgItem(showporttype, IDC_PORTTYPE_LIST);
			listview_insert_column (hlist, 0, 50, 0, "Sector");
			listview_insert_column (hlist, 1, 90, 1, "Fuel Amount");
			listview_insert_column (hlist, 2, 50, 2, "Fuel %");
			listview_insert_column (hlist, 3, 100, 3, "Org. Amount");
			listview_insert_column (hlist, 4, 50, 4, "Org %");
			listview_insert_column (hlist, 5, 110, 5, "Equip. Amount");
			listview_insert_column (hlist, 6, 60, 6, "Equip. %");
			listview_insert_column (hlist, 7, 60, 7, "Distance");

			showPortTypeData(hlist, type);

			ShowWindow(showporttype,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Port Type", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showporttype);
}


void show_evil_pair_ports(HWND hwnd)
{
	char buffer[50];

	if (!showevilpp) 
	{
		showevilpp = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SHOW_EVIL_PP), hwnd, ShowEvilPPProc);

		if ( showevilpp!=NULL ) 
		{ 

			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			sprintf (buffer, "%ld", currentSector);

			SetDlgItemText(showevilpp, IDC_SHOW_EVIL_PP_FILTER_PERCENT, "0");
			SetDlgItemText(showevilpp, IDC_SHOW_EVIL_PP_FILTER_AMOUNT, "0");
			SetDlgItemText(showevilpp, IDC_SHOW_EVIL_PP_ROUTESECTOR, buffer);

			ShowWindow(showevilpp,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Evil Pair Port", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 
	}
    SetActiveWindow(showevilpp);
}

void show_pair_ports(HWND hwnd)
{
	char buffer[50];

	HANDLE htab;
	TC_ITEM item;

	if (!showpp) 
	{
		showpp = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SHOW_PP), hwnd, ShowPPProc);
		
		if ( showpp!=NULL ) 
		{ 

			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			htab = GetDlgItem(showpp, IDC_SHOW_PP_TAB);
			// to use an image set this to TCIF_IMAGE  
			item.mask= TCIF_TEXT; 
			// index into tabs image list, set to -1 when not in use
			item.iImage = -1;
			item.pszText ="Equipment - Organics Pair Ports";
			TabCtrl_InsertItem(htab,0,&item);
			item.pszText ="Equipment - Fuel Pair Ports";
			TabCtrl_InsertItem(htab, 1, &item);
			item.pszText ="Organics - Fuel Pair Ports";
			TabCtrl_InsertItem(htab, 2, &item);

			sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showpp, IDC_SHOW_PP_FILTER_PERCENT, "0");
			SetDlgItemText(showpp, IDC_SHOW_PP_FILTER_AMOUNT, "0");
			SetDlgItemText(showpp, IDC_SHOW_PP_ROUTESECTOR, buffer);
			
				
			ShowWindow(showpp,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Pair Port", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showpp);
}


void show_listbox_data(HWND hwnd, int type)
{
	long i, j;
	int isRegularWarp;
	char buffer[50];

	if (!listbox_data) 
	{
		listbox_data = CreateDialog(hinst, MAKEINTRESOURCE(IDD_LISTBOX_DATA), hwnd, ShowListBoxData);
		
		if ( listbox_data!=NULL ) 
		{ 
			ShowWindow(listbox_data,SW_SHOWNORMAL); 
			if (type==1)
			{
				SetWindowText(listbox_data, "Ports Under Construction");
				for (i=0; isValidSectorZeroOffset(i) && i<header.sector_cnt; i++)
				{
					if (sectors[i].info >UNDER_CONSTRUCTION)
					{
						switch (sectors[i].info-UNDER_CONSTRUCTION)
						{
							case 1:
							sprintf (buffer, "Sector %ld: Class 1 (BBS)", i+1);
							break;
							case 2:
							sprintf (buffer, "Sector %ld: Class 2 (BSB)", i+1);
							break;
							case 3:
							sprintf (buffer, "Sector %ld: Class 3 (SBB)", i+1);
							break;
							case 4:
							sprintf (buffer, "Sector %ld: Class 4 (SSB)", i+1);
							break;
							case 5:
							sprintf (buffer, "Sector %ld: Class 5 (SBS)", i+1);
							break;
							case 6:
							sprintf (buffer, "Sector %ld: Class 6 (BSS)", i+1);
							break;
							case 7:
							sprintf (buffer, "Sector %ld: Class 7 (SSS)", i+1);
							break;
							case 8:
							sprintf (buffer, "Sector %ld: Class 8 (BBB)", i+1);
							break;
							default:
							sprintf (buffer, "Unknown");
						}
						SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);
					}
				}
			}
			else if (type==2)
			{
				SetWindowText(listbox_data, "Class 0 Ports and Stardock");
				if (header.stardock != -1)
					sprintf (buffer, "Stardock => %ld", header.stardock);
				else
					sprintf (buffer, "Stardock => unknown");

				SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);

				if (header.cls0port_sol != -1)
					sprintf (buffer, "Class 0 (Sol) => %ld", header.cls0port_sol);
				else
					sprintf (buffer, "Class 0 (Sol) => unknown");

				SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);

				if (header.cls0port_rylos != -1)
					sprintf (buffer, "Class 0 (Rylos) => %ld", header.cls0port_rylos);
				else
					sprintf (buffer, "Class 0 (Rylos) => unknown");

				SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);

				if (header.cls0port_alpha != -1)
					sprintf (buffer, "Class 0 (Alpha Centauri) => %ld", header.cls0port_alpha);
				else
					sprintf (buffer, "Class 0 (Alpha Centauri) => unknown");

				SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);

			}
			else if (type==3)
			{
				SetWindowText(listbox_data, "Known Avoids");
				for (i=0; isValidSectorZeroOffset(i) && i<header.sector_cnt; i++)
				{
					if (sectors[i].avoided==1)
					{
						sprintf (buffer, "Sector: %ld is avoided", i+1);
						SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);
					}
				}
			}
			else if (type==4)
			{
				SetWindowText(listbox_data, "One Way Sectors");
				for (i=0; isValidSectorZeroOffset(i) && i<header.sector_cnt; i++)
				{
					for (j=0; j<6 && j<(long)sectors[i].warps; j++)
					{
						isRegularWarp =isOneWayWarp(i+1, sectors[i].warp_sect[j]);
						if (isRegularWarp==1)
						{
							sprintf (buffer, "%ld -> %ld may be a one-way warp", i+1, sectors[i].warp_sect[j]);
							SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);
						}
						if (isRegularWarp==2)
						{
							sprintf (buffer, "%ld -> %ld is a one-way warp", i+1, sectors[i].warp_sect[j]);
							SendDlgItemMessage(listbox_data, IDC_LISTBOX_DATA, LB_ADDSTRING, 0, (LPARAM) buffer);
						}
					}
				}
			}
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Default Window", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(listbox_data);
}



void showmegajettisonform(HWND hwnd)
{
    if (!showmegajettison) 
	{
		showmegajettison = CreateDialog(hinst, MAKEINTRESOURCE(IDD_MEGAJETTISON), hwnd, MegaJettisonProc);
		if ( showmegajettison!=NULL ) 
		{ 
			SetDlgItemText(showmegajettison, IDC_MEGAJETTISON_FUEL_AMOUNT, "27");
			SetDlgItemText(showmegajettison, IDC_MEGAJETTISON_ORG_AMOUNT, "16");
			SetDlgItemText(showmegajettison, IDC_MEGAJETTISON_EQUIP_AMOUNT, "10");

			CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_FUEL_CHECK, BST_CHECKED);
			CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_ORG_CHECK, BST_CHECKED);
			CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_EQUIP_CHECK, BST_CHECKED);

			CheckRadioButton(showmegajettison, IDC_MEGAJETTISON_JETTISON, IDC_MEGAJETTISON_DUMP, IDC_MEGAJETTISON_JETTISON);	
			EnableWindow( GetDlgItem(showmegajettison,IDC_MEGAJETTISON_PLANET), FALSE ) ; /* enable a control */		

			if (currentSector>0 && currentSector <= header.sector_cnt)
			{
				if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info==1)
				{
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_FUEL_CHECK, BST_UNCHECKED);
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_ORG_CHECK, BST_UNCHECKED);
				}
				else if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info==2)
				{
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_FUEL_CHECK, BST_UNCHECKED);
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_EQUIP_CHECK, BST_UNCHECKED);
				}
				else if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info==3)
				{
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_ORG_CHECK, BST_UNCHECKED);
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_EQUIP_CHECK, BST_UNCHECKED);
				}
				else if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info==4)
				{
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_EQUIP_CHECK, BST_UNCHECKED);
				}
				else if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info==5)
				{
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_ORG_CHECK, BST_UNCHECKED);
				}
				else if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info==6)
				{
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_FUEL_CHECK, BST_UNCHECKED);
				}
				else if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info==8)
				{
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_FUEL_CHECK, BST_UNCHECKED);
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_ORG_CHECK, BST_UNCHECKED);
					CheckDlgButton(showmegajettison, IDC_MEGAJETTISON_EQUIP_CHECK, BST_UNCHECKED);
				}
			}
			
			ShowWindow(showmegajettison,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Mega Jettison", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(showmegajettison, SW_SHOWNORMAL); 

    SetActiveWindow(showmegajettison);
}

void showrexxnotify(HWND hwnd, char *promptData)
{
    if (!rexxnotify) 
	{
		rexxnotify = CreateDialog(hinst, MAKEINTRESOURCE(IDD_REXXNOTIFY), hwnd, NULL);
		if ( rexxnotify!=NULL ) 
		{ 
			ShowWindow(rexxnotify,SW_SHOWNORMAL); 

			if (strcmp (promptData, "") != 0)
				SetDlgItemText(rexxnotify, IDC_REXXNOTIFYSTATIC, promptData);
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Rexx Notify", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(rexxnotify);
}
void showregistrationform(HWND hwnd)
{
	char *token, tempString[2056];
	FILE *fp;
	int i=0;

    if (!showregistration) 
	{
		showregistration = CreateDialog(hinst, MAKEINTRESOURCE(IDD_REGISTRATION), hwnd, RegistrationProc);
		if ( showregistration!=NULL ) 
		{ 
			strcpy (fullFilePath, appDir);
			strcat (fullFilePath, "reg.txt");

			if ((fp = fopen(fullFilePath, "r"))==NULL)
			{
				printf ("NO reg.txt found\n");
				validRegCode = FALSE;
				isRegistered = FALSE;
			}
			else
			{
				for (i=0; i < (int)strlen (tempString); i++)
					tempString[i]=0;

				i=0;      
		
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
						if (strcmp (token, "-1") != 0)
							SetDlgItemText(showregistration, IDC_REGISTRATION_ID1, token);
					}
					else if (i==1) {
						if (strcmp (token, "-1") != 0)
							SetDlgItemText(showregistration, IDC_REGISTRATION_ID2, token);
					}
					else if (i==2) {
						if (strcmp (token, "-1") != 0)
							SetDlgItemText(showregistration, IDC_REGISTRATION_ID3, token);
					}
					else if (i==3) {
						if (strcmp (token, "-1") != 0)
							SetDlgItemText(showregistration, IDC_REGISTRATION_ID4, token);
					}
					else if (i==4) {
						if (strcmp (token, "-1") != 0)
							SetDlgItemText(showregistration, IDC_REGISTRATION_CODE, token);
					}
					else if (i==5) {
						if (strcmp (token, "-1") != 0)
							SetDlgItemText(showregistration, IDC_REGISTRATION_TWXSYNC_CODE, token);
					}
					token = strtok(NULL, "*");
					i++;
				}

			}

			ShowWindow(showregistration,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Registration", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showregistration);
}


void showsetallies(HWND hwnd)
{
	int i;
	int addedOne=0;

    if (!setallies) 
	{
		setallies = CreateDialog(hinst, MAKEINTRESOURCE(IDD_ALLIED), hwnd, SetAlliesProc);
		if ( setallies!=NULL ) 
		{ 
			for (i = 1; i < MAX_PLAYERS; i++)
			{
				if (players[i].autonum != -1 && strcmp (players[i].name, "ME") != 0)
				{
					SendDlgItemMessage(setallies, IDC_ALLIED_TRADER, CB_ADDSTRING, 0, (LPARAM) (players[i].name));
					if (addedOne==0)
					{
						SetDlgItemText(setallies, IDC_ALLIED_TRADER, players[i].name);
						addedOne++;
					}
				}
			}

			SendDlgItemMessage(setallies, IDC_ALLIED_STATUS, CB_ADDSTRING, 0, (LPARAM) "Allied");
			SendDlgItemMessage(setallies, IDC_ALLIED_STATUS, CB_ADDSTRING, 0, (LPARAM) "Enemy");
			SetDlgItemText(setallies, IDC_ALLIED_STATUS, "Allied");

			ShowWindow(setallies,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Set Allies", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(setallies);
}

void showswitchdatabaseform(HWND hwnd)
{
	int i;
	int addedOne=0;
	struct sesslist sesslist;

	get_sesslist(&sesslist, TRUE);

    if (!showswitchdatabase) 
	{
		showswitchdatabase = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SWITCH_DATABASE), hwnd, SwitchDatabaseProc);
		if ( showswitchdatabase!=NULL ) 
		{ 
			for (i = 0; i < sesslist.nsessions; i++)
			{
				if (strcmp(sesslist.sessions[i], "Default Settings") != 0)
				{
					SendDlgItemMessage(showswitchdatabase, IDC_SWITCH_DATABASE_NAME, CB_ADDSTRING, 0, (LPARAM) (sesslist.sessions[i]));
					if (addedOne==0)
					{
						SetDlgItemText(showswitchdatabase, IDC_SWITCH_DATABASE_NAME, sesslist.sessions[i]);
						addedOne++;
					}
				}
			}

			ShowWindow(showswitchdatabase,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Switch Database", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showswitchdatabase);
}

void showmapform(HWND hwnd)
{
	RECT rect;
	HWND sect, lbl, lbl2;
	char mapSector[10];

    if (!showmap) 
	{
		showmap = CreateDialog(hinst, MAKEINTRESOURCE(IDD_MAP), hwnd, MapProc);

		if ( showmap!=NULL ) 
		{ 
			if (showbubblefinder==NULL)
				mapLines = (MAP_LINES *) malloc(header.sector_cnt * sizeof(MAP_LINES));
			GetClientRect(showmap, &rect);

			lbl = CreateWindow("STATIC", "Sector:",
					 WS_CHILD | WS_VISIBLE | WS_BORDER,
					 rect.left, rect.bottom-40, 50, 20,
					 showmap, (HMENU)IDC_MAP_LABEL, hinst, NULL);

			sect = CreateWindow("EDIT", "1",
					 WS_CHILD | WS_VISIBLE | WS_BORDER,
					 rect.left+50, rect.bottom-40, 50, 20,
					 showmap, (HMENU)IDC_MAP_SECTOR, hinst, NULL);

			lbl2 = CreateWindow("STATIC", "Grey=No Figs; Green=My Figs; Blue=Allied Figs; Red=Enemy Figs",
					 WS_CHILD | WS_VISIBLE | WS_BORDER,
					 rect.left, rect.bottom-20, 450, 20,
					 showmap, (HMENU)IDC_MAP_LABEL2, hinst, NULL);

			SendDlgItemMessage(showmap, IDC_MAP_LABEL, WM_SETFONT,
					(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));


			SendDlgItemMessage(showmap, IDC_MAP_SECTOR, WM_SETFONT,
					(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));


			GetDlgItemText(showmap, IDC_MAP_SECTOR, mapSector, sizeof(mapSector) - 1);

			buildMapDataStructure(hwnd, atol(mapSector), 5, rect.bottom, rect.right);

			ShowWindow(showmap,SW_SHOWNORMAL); 
			ShowWindow(sect,SW_SHOWNORMAL); 
			ShowWindow(lbl,SW_SHOWNORMAL); 
			ShowWindow(lbl2,SW_SHOWNORMAL); 
			SetFocus(sect);
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Map", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showmap);
}

void showbubblefinderform(HWND hwnd)
{
	RECT rect;
	HWND lbl;

    if (!showbubblefinder) 
	{
		showbubblefinder = CreateDialog(hinst, MAKEINTRESOURCE(IDD_BUBBLE_FINDER), hwnd, BubbleFinderProc);
		if ( showbubblefinder!=NULL ) 
		{ 
			if (showmap==NULL)
				mapLines = (MAP_LINES *) malloc(header.sector_cnt * sizeof(MAP_LINES));

			GetClientRect(hwnd, &rect);
			bubbleOffset = (int) ((rect.right-rect.left)*.48);

			SetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_MIN, "5");
			SetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_MAX, "50");
			SetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_MAX_ENTRANCES, "1");
			EnableWindow( GetDlgItem(showbubblefinder,IDC_BUBBLE_FINDER_MAX_ENTRANCES), FALSE ) ; /* disable a control */
			SetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_ENTRANCE_MULT, "2");
			
			lbl = CreateWindow("STATIC", "Hit Find Bubbles ONCE then give it a minute.",
					 WS_CHILD | WS_VISIBLE | WS_BORDER,
					 rect.right-bubbleOffset, rect.bottom+5, 300, 15,
					 showbubblefinder, (HMENU)IDC_BUBBLE_FINDER_LABEL, hinst, NULL);


			buildMapDataStructure(hwnd, atol("-1"), 5, rect.bottom, rect.right);
			
			ShowWindow(lbl,SW_SHOWNORMAL); 
			ShowWindow(showbubblefinder,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Bubble Finder", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showbubblefinder);
}

void showsectorinfoform(HWND hwnd)
{
	HANDLE htmp;
    if (!showsectorinfo) 
	{
		showsectorinfo = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SECTOR_INFO), hwnd, SectorInfoProc);

		if ( showsectorinfo!=NULL ) 
		{ 
			htmp = GetDlgItem(showsectorinfo, IDC_SECTOR_INFO_SECTOR);
			SetFocus(htmp);

			ShowWindow(showsectorinfo,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Sector Info", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showsectorinfo);
}


void showremoteconnectionform(HWND hwnd)
{
	HWND hlist;

    if (!showremoteconnection) 
	{
		showremoteconnection = CreateDialog(hinst, MAKEINTRESOURCE(IDD_REMOTECONNECTION), hwnd, ShowRemoteConnectionProc);

		if ( showremoteconnection!=NULL ) 
		{ 
			hlist = GetDlgItem(showremoteconnection, IDC_REMOTECONNECTION_LIST);

			if (remoteConnectionEnabled)
				CheckDlgButton(showremoteconnection, IDC_REMOTECONNECTION_ENABLE, BST_CHECKED);	//colonists

			listview_insert_column (hlist, 0, 20, 0, "#");
			listview_insert_column (hlist, 1, 90, 1, "IP Address");
			listview_insert_column (hlist, 2, 170, 2, "Connect Time");
			listview_insert_column (hlist, 3, 50, 3, "Status");

			updateRemoteConnectionList(hlist);

			ShowWindow(showremoteconnection,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Remote Connection", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showremoteconnection);
}

void showplanetform (HWND hwnd)
{
    if (!showplanet) 
	{
		showplanet = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SHOWPLANETS), hwnd, ShowPlanetProc);
		if ( showplanet!=NULL ) 
		{ 
			ShowWindow(showplanet,SW_SHOWNORMAL); 
			
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Show Window", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showplanet);
}


void shownearestfigform(HWND hwnd)
{
    if (!shownearestfig) 
	{
		shownearestfig = CreateDialog(hinst, MAKEINTRESOURCE(IDD_NEARESTFIGHTERS), hwnd, NearestFighterProc);
		if ( shownearestfig!=NULL ) 
		{ 
			ShowWindow(shownearestfig,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window NearestFig", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(shownearestfig,SW_SHOWNORMAL); 
    SetActiveWindow(shownearestfig);
}


void showplanetbustform(HWND hwnd)
{
    if (!showplanetbust) 
	{
		showplanetbust = CreateDialog(hinst, MAKEINTRESOURCE(IDD_PLANETBUST), hwnd, PlanetBustProc);
		if ( showplanetbust!=NULL ) 
		{ 
			ShowWindow(showplanetbust,SW_SHOWNORMAL); 
			
			SetDlgItemText(showplanetbust, IDC_PLANETBUST_NAME, "ATTAC Planet Bust: tw-attac.com");
			SetDlgItemText(showplanetbust, IDC_PLANETBUST_CYCLES, "50");

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Planet Bust", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		ShowWindow(showplanetbust, SW_SHOWNORMAL); 
	}

    SetActiveWindow(showplanetbust);
}


void showaddbustform(HWND hwnd)
{
	char buffer[20];
	HANDLE cCtrl;

    if (!showaddbust) 
	{
		showaddbust = CreateDialog(hinst, MAKEINTRESOURCE(IDD_ADDBUST), hwnd, AddBustProc);
		if ( showaddbust!=NULL ) 
		{ 
			ShowWindow(showaddbust,SW_SHOWNORMAL); 
			
			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			if (currentSector==0)
				strcpy (buffer, "");
			else
				sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showaddbust, IDC_ADDBUST_SECTOR, buffer);

			cCtrl = GetDlgItem(showaddbust, IDC_ADDBUST_SECTOR);
			SetFocus(cCtrl);

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Add Bust", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 
	}
	else
	{
		sprintf (buffer, "%ld", currentSector);
		SetDlgItemText(showaddbust, IDC_ADDBUST_SECTOR, buffer);
		ShowWindow(showaddbust, SW_SHOWNORMAL); 
	}

    SetActiveWindow(showaddbust);
}


void showcolonizationform(HWND hwnd)
{
	char buffer[20];

    if (!showcolonization) 
	{
		showcolonization = CreateDialog(hinst, MAKEINTRESOURCE(IDD_COLONIZATION), hwnd, ColonizationProc);
		if ( showcolonization!=NULL ) 
		{ 
			ShowWindow(showcolonization,SW_SHOWNORMAL); 
			
			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showcolonization, IDC_COLONIZE_SECTOR_EDIT, buffer);
			SetDlgItemText(showcolonization, IDC_COLONIZE_FUELAMOUNT_EDIT, "AUTO");
			SetDlgItemText(showcolonization, IDC_COLONIZE_WARPTO_EDIT, "0");
			SetDlgItemText(showcolonization, IDC_COLONIZE_CYCLES_EDIT, "9999");
			CheckRadioButton(showcolonization, IDC_COLONIZE_FUEL_RADIO, IDC_COLONIZE_EQUIP_RADIO, IDC_COLONIZE_FUEL_RADIO);	
			CheckRadioButton(showcolonization, IDC_COLONIZE_TOTWARP_RADIO, IDC_COLONIZE_TOPWARP_RADIO, IDC_COLONIZE_TOTWARP_RADIO);	
			CheckRadioButton(showcolonization, IDC_COLONIZE_FROMTWARP_RADIO, IDC_COLONIZE_FROMWARP_RADIO, IDC_COLONIZE_FROMTWARP_RADIO);	


		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Colonization", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		sprintf (buffer, "%ld", currentSector);
		SetDlgItemText(showcolonization, IDC_COLONIZE_SECTOR_EDIT, buffer);
		ShowWindow(showcolonization, SW_SHOWNORMAL); 
	}

    SetActiveWindow(showcolonization);
}


void showworldssmform(HWND hwnd)
{
	char buffer[20];
	int checkSD=-1;

    if (!showworldssm) 
	{
		showworldssm = CreateDialog(hinst, MAKEINTRESOURCE(IDD_WORLDSSM), hwnd, WorldSSMProc);
		if ( showworldssm!=NULL ) 
		{ 
			ShowWindow(showworldssm,SW_SHOWNORMAL); 

			if (header.stardock>0)
			{
				sprintf (buffer, "%ld", header.stardock);
				SetDlgItemText(showworldssm, IDC_WORLDSSM_SD, buffer);
				checkSD=1;
			}

			if (header.cls0port_rylos>0)
			{
				sprintf (buffer, "%ld", header.cls0port_rylos);
				SetDlgItemText(showworldssm, IDC_WORLDSSM_CLASS0, buffer);
				checkSD=0;
			}
			else if (header.cls0port_alpha>0)
			{
				sprintf (buffer, "%ld", header.cls0port_alpha);
				SetDlgItemText(showworldssm, IDC_WORLDSSM_CLASS0, buffer);
				checkSD=0;
			}
			else if (header.cls0port_sol>0)
			{
				sprintf (buffer, "%ld", header.cls0port_sol);
				SetDlgItemText(showworldssm, IDC_WORLDSSM_CLASS0, buffer);
				checkSD=0;
			}

			if (checkSD==0)
				CheckDlgButton(showworldssm, IDC_WORLDSSM_CLASS0_CHECK, BST_CHECKED);	//class0
			else
				CheckDlgButton(showworldssm, IDC_WORLDSSM_SD_CHECK, BST_CHECKED);	//class0

			EnableWindow( GetDlgItem(showworldssm,IDC_WORLDSSM_PLANETNUMBER), FALSE ) ; /* disable a control */
			EnableWindow( GetDlgItem(showworldssm,IDC_WORLDSSM_DROPCASH), FALSE ) ; /* disable a control */
			EnableWindow( GetDlgItem(showworldssm,IDC_WORLDSSM_DROPCASH), FALSE ) ; /* disable a control */
			EnableWindow( GetDlgItem(showworldssm,IDC_WORLDSSM_TWARP), FALSE ) ; /* disable a control */
			EnableWindow( GetDlgItem(showworldssm,IDC_WORLDSSM_FUEL), FALSE ) ; /* disable a control */

			SetDlgItemText(showworldssm, IDC_WORLDSSM_DROPCASH, "10");
			SetDlgItemText(showworldssm, IDC_WORLDSSM_CYCLES, "9999");

			SetDlgItemText(showworldssm, IDC_WORLDSSM_DROPFIGS_AMOUNT, "1");		//drop amount
			CheckRadioButton(showworldssm, IDC_WORLDSSM_DEFENSIVE, IDC_WORLDSSM_TOLL, IDC_WORLDSSM_TOLL);	
			CheckRadioButton(showworldssm, IDC_WORLDSSM_CORPORATE, IDC_WORLDSSM_PERSONAL, IDC_WORLDSSM_CORPORATE);	
			CheckDlgButton(showworldssm, IDC_WORLDSSM_DROPFIGS_CHECK, BST_CHECKED);	//drop fig

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window World SSM", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		if (header.stardock>0)
		{
			sprintf (buffer, "%ld", header.stardock);
			SetDlgItemText(showworldssm, IDC_WORLDSSM_SD, buffer);
			checkSD=1;
		}

		if (header.cls0port_rylos>0)
		{
			sprintf (buffer, "%ld", header.cls0port_rylos);
			SetDlgItemText(showworldssm, IDC_WORLDSSM_CLASS0, buffer);
			checkSD=0;
		}
		else if (header.cls0port_alpha>0)
		{
			sprintf (buffer, "%ld", header.cls0port_alpha);
			SetDlgItemText(showworldssm, IDC_WORLDSSM_CLASS0, buffer);
			checkSD=0;
		}
		else if (header.cls0port_sol>0)
		{
			sprintf (buffer, "%ld", header.cls0port_sol);
			SetDlgItemText(showworldssm, IDC_WORLDSSM_CLASS0, buffer);
			checkSD=0;
		}

		ShowWindow(showworldssm, SW_SHOWNORMAL); 
	}

    SetActiveWindow(showworldssm);
}

void showworldtradeform(HWND hwnd)
{
    if (!showworldtrade) 
	{
		showworldtrade = CreateDialog(hinst, MAKEINTRESOURCE(IDD_WORLDTRADE), hwnd, WorldTradeProc);
		if ( showworldtrade!=NULL ) 
		{ 
			ShowWindow(showworldtrade,SW_SHOWNORMAL); 
			
			
			if (header.forceNoHaggle==1)
				CheckDlgButton(showworldtrade, IDC_WORLDTRADE_HAGGLE, BST_UNCHECKED);
			else
				CheckDlgButton(showworldtrade, IDC_WORLDTRADE_HAGGLE, BST_CHECKED);

			CheckDlgButton(showworldtrade, IDC_WORLDTRADE_DROPFIGS, BST_CHECKED);	//colonists
			SetDlgItemText(showworldtrade, IDC_WORLDTRADE_FIGAMOUNT, "1");
			SetDlgItemText(showworldtrade, IDC_WORLDTRADE_CYCLES, "9999");
			CheckRadioButton(showworldtrade, IDC_WORLDTRADE_CORPORATE, IDC_WORLDTRADE_PERSONAL, IDC_WORLDTRADE_CORPORATE);	
			CheckRadioButton(showworldtrade, IDC_WORLDTRADE_TOLL, IDC_WORLDTRADE_OFFENSIVE, IDC_WORLDTRADE_TOLL);	
			
			SendDlgItemMessage(showworldtrade, IDC_WORLDTRADE_PAIRTYPE, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(showworldtrade, IDC_WORLDTRADE_PAIRTYPE, CB_ADDSTRING, 0, (LPARAM) ("Equipment-Organic"));
			SendDlgItemMessage(showworldtrade, IDC_WORLDTRADE_PAIRTYPE, CB_ADDSTRING, 0, (LPARAM) ("Equipment-Fuel"));
			SendDlgItemMessage(showworldtrade, IDC_WORLDTRADE_PAIRTYPE, CB_ADDSTRING, 0, (LPARAM) ("Organic-Fuel"));
			SetDlgItemText(showworldtrade, IDC_WORLDTRADE_PAIRTYPE, "Equipment-Organic");
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window World Trade", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		ShowWindow(showworldtrade, SW_SHOWNORMAL); 
	}

    SetActiveWindow(showworldtrade);
}

void showautoexplorerform(HWND hwnd)
{
    if (!showautoexplorer) 
	{
		showautoexplorer = CreateDialog(hinst, MAKEINTRESOURCE(IDD_AUTOEXPLORE), hwnd, AutoExplorerProc);
		if ( showautoexplorer!=NULL ) 
		{ 
			ShowWindow(showautoexplorer,SW_SHOWNORMAL); 

			SetDlgItemText(showautoexplorer, IDC_AUTOEXPLORE_STARTSECTOR, "1");

			CheckDlgButton(showautoexplorer, IDC_AUTOEXPLORE_PORTTRADE, BST_CHECKED);
			if (header.forceNoHaggle==1)
				CheckDlgButton(showautoexplorer, IDC_AUTOEXPLORE_HAGGLE, BST_UNCHECKED);
			else
				CheckDlgButton(showautoexplorer, IDC_AUTOEXPLORE_HAGGLE, BST_CHECKED);

			
			CheckDlgButton(showautoexplorer, IDC_AUTOEXPLORE_EXPLOREALL, BST_UNCHECKED);
			CheckDlgButton(showautoexplorer, IDC_AUTOEXPLORE_PLANETAVOID, BST_CHECKED);
			CheckDlgButton(showautoexplorer, IDC_AUTOEXPLORE_MAXFIGSAVOID, BST_CHECKED);
			SetDlgItemText(showautoexplorer, IDC_AUTOEXPLORE_NUMFIGSAVOID, "500");

			CheckDlgButton(showautoexplorer, IDC_AUTOEXPLORE_DROPFIGS, BST_CHECKED);

			SetDlgItemText(showautoexplorer, IDC_AUTOEXPLORE_FIGAMOUNT, "1");
			CheckRadioButton(showautoexplorer, IDC_AUTOEXPLORE_CORPORATE, IDC_AUTOEXPLORE_PERSONAL, IDC_AUTOEXPLORE_CORPORATE);
			CheckRadioButton(showautoexplorer, IDC_AUTOEXPLORE_TOLL, IDC_AUTOEXPLORE_OFFENSIVE, IDC_AUTOEXPLORE_TOLL);	

			SendDlgItemMessage(showautoexplorer, IDC_AUTOEXPLORE_FIGSTYLE, CB_ADDSTRING, 0, (LPARAM) ("Everywhere"));
			SendDlgItemMessage(showautoexplorer, IDC_AUTOEXPLORE_FIGSTYLE, CB_ADDSTRING, 0, (LPARAM) ("At Ports"));
			SetDlgItemText(showautoexplorer, IDC_AUTOEXPLORE_FIGSTYLE, "Everywhere");

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Auto Explorer", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		ShowWindow(showautoexplorer, SW_SHOWNORMAL); 
	}

    SetActiveWindow(showautoexplorer);
}

void showfedcommform(HWND hwnd)
{
	HWND htmp;
    if (!showfedcomm) 
	{
		showfedcomm = CreateDialog(hinst, MAKEINTRESOURCE(IDD_FEDCOMM), hwnd, FedCommProc);
		if ( showfedcomm!=NULL ) 
		{ 
			ShowWindow(showfedcomm,SW_SHOWNORMAL); 
			isFedCommVisible=1;
			
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Fed Comm", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 
	}
	else
	{
		ShowWindow(showfedcomm, SW_SHOWNORMAL); 
		isFedCommVisible=1;
	}


	htmp = GetDlgItem(showfedcomm, IDC_FEDCOMM_EDIT);
	SetFocus(htmp);
    SetActiveWindow(showfedcomm);
}

void showprivatecommform(HWND hwnd)
{
	HWND htmp;
    if (!showprivatecomm) 
	{
		showprivatecomm = CreateDialog(hinst, MAKEINTRESOURCE(IDD_PRIVATECOMM), hwnd, PrivateCommProc);
		if ( showprivatecomm!=NULL ) 
		{ 
			ShowWindow(showprivatecomm,SW_SHOWNORMAL); 
			isPrivateCommVisible=1;
			
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Private Comm", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 
	}
	else
	{
		ShowWindow(showprivatecomm, SW_SHOWNORMAL); 
		isPrivateCommVisible=1;
	}

	htmp = GetDlgItem(showprivatecomm, IDC_PRIVATECOMM_EDIT);
	SetFocus(htmp);

    SetActiveWindow(showprivatecomm);
}

void showmoverform(HWND hwnd, int type)
{
	char buffer[20];

    if (!showmover) 
	{
		showmover = CreateDialog(hinst, MAKEINTRESOURCE(IDD_RESOURCEMOVER), hwnd, MoverProc);
		if ( showmover!=NULL ) 
		{ 
			ShowWindow(showmover,SW_SHOWNORMAL); 
			
			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showmover, IDC_MOVER_SECTOR1, buffer);
			SetDlgItemText(showmover, IDC_MOVER_SECTOR2, buffer);
			SetDlgItemText(showmover, IDC_MOVER_FUELAMOUNT, "AUTO");
			SetDlgItemText(showmover, IDC_MOVER_CYCLES, "9999");
			CheckRadioButton(showmover, IDC_MOVER_FUEL_RADIO, IDC_MOVER_EQUIP_RADIO, IDC_MOVER_FUEL_RADIO);
			CheckRadioButton(showmover, IDC_MOVER_TOTWARP_RADIO, IDC_MOVER_TOPWARP_RADIO, IDC_MOVER_TOTWARP_RADIO);
			CheckRadioButton(showmover, IDC_MOVER_FROMTWARP_RADIO, IDC_MOVER_FROMWARP_RADIO, IDC_MOVER_FROMTWARP_RADIO);

			if (type==1)
				CheckDlgButton(showmover, IDC_MOVER_COLONISTS_CHECK, BST_CHECKED);	//colonists
			else
				CheckDlgButton(showmover, IDC_MOVER_COLONISTS_CHECK, BST_UNCHECKED);	//resources
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Resource Mover", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		sprintf (buffer, "%ld", currentSector);
		SetDlgItemText(showmover, IDC_MOVER_SECTOR1, buffer);
		ShowWindow(showmover, SW_SHOWNORMAL); 
    }
	SetActiveWindow(showmover);
}


void showlimpitscannerform(HWND hwnd)
{
	HWND hlist;

    if (!showlimpitscanner) 
	{
		showlimpitscanner = CreateDialog(hinst, MAKEINTRESOURCE(IDD_LIMPITSCANNER), hwnd, LimpitScannerProc);
		if ( showlimpitscanner !=NULL ) 
		{ 
			hlist = GetDlgItem(showlimpitscanner, IDC_LIMPITSCANNER_LIST);

			listview_insert_column (hlist, 0, 100, 0, "Sector");
			listview_insert_column (hlist, 1, 100, 1, "Times Visited");

			ShowWindow(showlimpitscanner,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Limpit Scanner", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showlimpitscanner);
}


void showtwxsyncform(HWND hwnd)
{
	HWND hlist;

    if (!showtwxsync) 
	{
		showtwxsync = CreateDialog(hinst, MAKEINTRESOURCE(IDD_TWXSYNC), hwnd, TWXSyncProc);
		if ( showtwxsync !=NULL ) 
		{ 
			hlist = GetDlgItem(showtwxsync, IDC_TWXSYNC_GAMELIST);

			listview_insert_column (hlist, 0, 200, 0, "Game Name");
			listview_insert_column (hlist, 1, 125, 1, "Unique Identifier");

			SetDlgItemText(showtwxsync, IDC_TWXSYNC_PASSWORD, header.TWXSyncPassword);
			SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Connecting to Server... Please Wait...");
			

			ShowWindow(showtwxsync,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window TWX Sync", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showtwxsync);
}


void showmacroform(HWND hwnd)
{
	HWND hlist;

    if (!showmacro) 
	{
		showmacro = CreateDialog(hinst, MAKEINTRESOURCE(IDD_MACRO), hwnd, MacroProc);
		if ( showmacro !=NULL ) 
		{ 
			hlist = GetDlgItem(showmacro, IDC_MACRO_LIST);

			listview_insert_column (hlist, 0, 50, 0, "Key");
			listview_insert_column (hlist, 1, 280, 1, "Data");

			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F5"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F6"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F7"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F8"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F9"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F10"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F11"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("F12"));

			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F3"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F4"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F5"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F6"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F7"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F8"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F9"));
			SendDlgItemMessage(showmacro, IDC_MACRO_FUNCTION, CB_ADDSTRING, 0, (LPARAM) ("Shift F10"));

			readMacros();
			reDrawMacros (hlist);

			ShowWindow(showmacro,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Macro", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showmacro);
}

void showmacroeditform(HWND hwnd, char *caption, char *data)
{
    if (!showmacroedit) 
	{
		showmacroedit = CreateDialog(hinst, MAKEINTRESOURCE(IDD_MACRO_EDIT), hwnd, MacroProcEdit);
		if ( showmacroedit !=NULL ) 
		{ 

			SetDlgItemText(showmacroedit, IDC_MACROEDIT_CAPTION, caption);
			SetDlgItemText(showmacroedit, IDC_MACROEDIT_EDIT, data);

			ShowWindow(showmacroedit,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Macro Edit", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showmacroedit);
}


void showbuyplanetshieldsform(HWND hwnd)
{
	char buffer[20];

    if (!showbuyplanetshields) 
	{
		showbuyplanetshields = CreateDialog(hinst, MAKEINTRESOURCE(IDD_BUYPLANET_SHIELDS), hwnd, BuyPlanetShieldsProc);
		if ( showbuyplanetshields!=NULL ) 
		{ 
			ShowWindow(showbuyplanetshields,SW_SHOWNORMAL); 
			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showbuyplanetshields, IDC_PLANETSHIELDS_PLANETSECTOR, buffer);
			SetDlgItemText(showbuyplanetshields, IDC_PLANETSHIELDS_PORTSECTOR, buffer);
			SetDlgItemText(showbuyplanetshields, IDC_PLANETSHIELDS_CYCLES, "9999");
			CheckRadioButton(showbuyplanetshields, IDC_PLANETSHIELDS_TO_TWARP, IDC_PLANETSHIELDS_TO_PWARP, IDC_PLANETSHIELDS_TO_TWARP);	
			CheckRadioButton(showbuyplanetshields, IDC_PLANETSHIELDS_FROM_TWARP, IDC_PLANETSHIELDS_FROM_WARP, IDC_PLANETSHIELDS_FROM_TWARP);	
			SetDlgItemText(showbuyplanetshields, IDC_PLANETSHIELDS_FUELAMOUNT, "AUTO");


		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Buy Planet Shields", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		sprintf (buffer, "%ld", currentSector);
		SetDlgItemText(showbuyplanetshields, IDC_PLANETSHIELDS_PLANETSECTOR, buffer);
		ShowWindow(showbuyplanetshields, SW_SHOWNORMAL); 
	}
    
	SetActiveWindow(showbuyplanetshields);
}


void showsellplanettoportform(HWND hwnd)
{
	char buffer[20];

    if (!showsellplanettoport) 
	{
		showsellplanettoport = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SELLPLANETTOPORT), hwnd, SellPlanetToPortProc);
		if ( showsellplanettoport!=NULL ) 
		{ 
			ShowWindow(showsellplanettoport,SW_SHOWNORMAL); 
			
			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showsellplanettoport, IDC_SELLPLANETTOPORT_SECTOR_EDIT, buffer);
			SetDlgItemText(showsellplanettoport, IDC_SELLPLANETTOPORT_PORT_EDIT, buffer);
			SetDlgItemText(showsellplanettoport, IDC_SELLPLANETTOPORT_CYCLES_EDIT, "9999");
			CheckRadioButton(showsellplanettoport, IDC_SELLPLANETTOPORT_FUEL_RADIO, IDC_SELLPLANETTOPORT_EQUIP_RADIO, IDC_SELLPLANETTOPORT_FUEL_RADIO);	
			CheckRadioButton(showsellplanettoport, IDC_SELLPLANETTOPORT_TOTWARP_RADIO, IDC_SELLPLANETTOPORT_TOPWARP_RADIO, IDC_SELLPLANETTOPORT_TOTWARP_RADIO);	
			CheckRadioButton(showsellplanettoport, IDC_SELLPLANETTOPORT_FROMTWARP_RADIO, IDC_SELLPLANETTOPORT_FROMWARP_RADIO, IDC_SELLPLANETTOPORT_FROMTWARP_RADIO);	
			SetDlgItemText(showsellplanettoport, IDC_SELLPLANETTOPORT_FUELAMOUNT_EDIT, "AUTO");

			SendDlgItemMessage(showsellplanettoport, IDC_SELLPLANETTOPORT_HAGGLE, CB_ADDSTRING, 0, (LPARAM) ("Asking Price"));
			SendDlgItemMessage(showsellplanettoport, IDC_SELLPLANETTOPORT_HAGGLE, CB_ADDSTRING, 0, (LPARAM) ("Best Price"));
			SendDlgItemMessage(showsellplanettoport, IDC_SELLPLANETTOPORT_HAGGLE, CB_ADDSTRING, 0, (LPARAM) ("Worst Price"));
			SetDlgItemText(showsellplanettoport, IDC_SELLPLANETTOPORT_HAGGLE, "Best Price");


		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Sell Planet to Port", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		sprintf (buffer, "%ld", currentSector);
		SetDlgItemText(showsellplanettoport, IDC_SELLPLANETTOPORT_SECTOR_EDIT, buffer);
		ShowWindow(showsellplanettoport, SW_SHOWNORMAL); 
	}
    
	SetActiveWindow(showsellplanettoport);
}


void showbuydumpform(HWND hwnd)
{
	char buffer[20];

    if (!showbuydump) 
	{
		showbuydump = CreateDialog(hinst, MAKEINTRESOURCE(IDD_BUYDUMP), hwnd, BuyDumpProc);
		if ( showbuydump!=NULL ) 
		{ 
			ShowWindow(showbuydump,SW_SHOWNORMAL); 
			
			if (currentSector < 1 && currentSector > header.sector_cnt)
				currentSector = 0;

			sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showbuydump, IDC_BUYDUMP_SECTOR_EDIT, buffer);
			SetDlgItemText(showbuydump, IDC_BUYDUMP_PORT_EDIT, buffer);
			SetDlgItemText(showbuydump, IDC_BUYDUMP_CYCLES_EDIT, "9999");
			CheckRadioButton(showbuydump, IDC_BUYDUMP_FUEL_RADIO, IDC_BUYDUMP_EQUIP_RADIO, IDC_BUYDUMP_FUEL_RADIO);	
			CheckRadioButton(showbuydump, IDC_BUYDUMP_TOTWARP_RADIO, IDC_BUYDUMP_TOPWARP_RADIO, IDC_BUYDUMP_TOTWARP_RADIO);	
			CheckRadioButton(showbuydump, IDC_BUYDUMP_FROMTWARP_RADIO, IDC_BUYDUMP_FROMWARP_RADIO, IDC_BUYDUMP_FROMTWARP_RADIO);	
			SetDlgItemText(showbuydump, IDC_BUYDUMP_FUELAMOUNT_EDIT, "AUTO");

			SendDlgItemMessage(showbuydump, IDC_BUYDUMP_HAGGLE, CB_ADDSTRING, 0, (LPARAM) ("Asking Price"));
			SendDlgItemMessage(showbuydump, IDC_BUYDUMP_HAGGLE, CB_ADDSTRING, 0, (LPARAM) ("Best Price"));
			SendDlgItemMessage(showbuydump, IDC_BUYDUMP_HAGGLE, CB_ADDSTRING, 0, (LPARAM) ("Worst Price"));
			SetDlgItemText(showbuydump, IDC_BUYDUMP_HAGGLE, "Worst Price");


		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Buy Dump", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		sprintf (buffer, "%ld", currentSector);
		SetDlgItemText(showbuydump, IDC_BUYDUMP_SECTOR_EDIT, buffer);
		ShowWindow(showbuydump, SW_SHOWNORMAL); 
	}
    
	SetActiveWindow(showbuydump);
}

void showpairportform(HWND hwnd)
{
	int i;
	char buffer[50];
	long tmp;

    if (!showpairport) 
	{
		showpairport = CreateDialog(hinst, MAKEINTRESOURCE(IDD_PAIR_PORT), hwnd, PairPortProc);
		if ( showpairport!=NULL ) 
		{ 
			ShowWindow(showpairport,SW_SHOWNORMAL); 

			if (currentSector < 1 || currentSector > header.sector_cnt)
				currentSector = 0;

		
			if (header.forceNoHaggle==1)
				CheckDlgButton(showpairport, IDC_PP_FORCE_HAGGLE, BST_UNCHECKED);
			else
				CheckDlgButton(showpairport, IDC_PP_FORCE_HAGGLE, BST_CHECKED);

			sprintf (buffer, "Current Sector (%ld)", currentSector);
			SetDlgItemText(showpairport, IDC_CURRENT_GROUP_STATIC, buffer);

			if (currentSector>0 && currentSector <= header.sector_cnt)
			{
				updatepairportcheckboxes (showpairport, 1, currentSector);

				SendDlgItemMessage(showpairport, IDC_PP_OTHER_SECTOR, CB_RESETCONTENT, 0, 0);


				tmp =0;
				if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].warps > 0)
				{
					for(i=0; i<6; i++)
					{
						tmp = sectors[currentSector-1].warp_sect[i];
						if (tmp > 0 && isValidPairPort (sectors[currentSector-1].info, sectors[tmp-1].info))
						{
							sprintf (buffer, "%ld", tmp);
							SetDlgItemText(showpairport, IDC_PP_OTHER_SECTOR, buffer);
							i=6;
						}
					}
					
					if (tmp > 0)
						updatepairportcheckboxes (showpairport, 2, tmp);

				}

				if (currentSector>0 && currentSector <= header.sector_cnt)
				{
					for (i = 0; isValidSectorOneOffset(currentSector) && i<6 && i < sectors[currentSector-1].warps; i++)
					{
						printf ("%ld - %ld\n", currentSector, sectors[currentSector-1].warp_sect[i]);
						if (sectors[currentSector-1].warp_sect[i]>0 && sectors[currentSector-1].warp_sect[i]<=header.sector_cnt )
							printf ("%d - %d\n", sectors[currentSector-1].info, sectors[sectors[currentSector-1].warp_sect[i]-1].info);
						if (sectors[currentSector-1].warp_sect[i]>0 && sectors[currentSector-1].warp_sect[i]<=header.sector_cnt 
							&& isValidPairPort (sectors[currentSector-1].info, sectors[sectors[currentSector-1].warp_sect[i]-1].info))
						{
							sprintf (buffer, "%ld", sectors[currentSector-1].warp_sect[i]);
							SendDlgItemMessage(showpairport, IDC_PP_OTHER_SECTOR, CB_ADDSTRING, 0, (LPARAM) (buffer));
						}
					}
				}

				if (isValidSectorOneOffset(currentSector) && sectors[currentSector-1].info ==1)
				{
					sprintf (buffer, "%ld", sectors[currentSector-1].warp_sect[0]);
					SetDlgItemText(showpairport, IDC_PP_OTHER_SECTOR, buffer);
				}
			}

			SetDlgItemText(showpairport, IDC_PP_CYCLES, "9999");
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Pair Port", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(showpairport, SW_SHOWNORMAL); 

    SetActiveWindow(showpairport);
}

int isValidPairPort(long firstPort, long secondPort)
{
	if (firstPort==0 || secondPort==0)
		return (0);

	if (firstPort==1 && (secondPort==2 || secondPort==3 || secondPort==4))
		return (1);
	if (firstPort==2 && (secondPort==1 || secondPort==3 || secondPort==5))
		return (1);
	if (firstPort==3 && (secondPort==1 || secondPort==2 || secondPort==6))
		return (1);
	if (firstPort==4 && (secondPort==1 || secondPort==5 || secondPort==6))
		return (1);
	if (firstPort==5 && (secondPort==2 || secondPort==4 || secondPort==6))
		return (1);
	if (firstPort==6 && (secondPort==3 || secondPort==4 || secondPort==5))
		return (1);

	return(0);
}

void updatepairportcheckboxes (HANDLE frmHnd, int type, long curSector)
{
	char buffer[20];
	char buffer1[20];
	char buffer2[20];
	int checkCount=0;
	int portOneBuyingCount=0;
	int portTwoBuyingCount=0;

	if (curSector<=0 || curSector> header.sector_cnt)
		return;
	sprintf (buffer, "(B) Fuel Ore");
	sprintf (buffer1, "(B) Organics");
	sprintf (buffer2, "(B) Equipment");

	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==-1)
	{
		sprintf (buffer, "(?) Fuel Ore");
		sprintf (buffer1, "(?) Organics");
		sprintf (buffer2, "(?) Equipment");
	}
	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==1)
		sprintf (buffer2, "(S) Equipment");
	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==2)
		sprintf (buffer1, "(S) Organics");
	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==3)
		sprintf (buffer, "(S) Fuel Ore");
	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==4)
	{
		sprintf (buffer, "(S) Fuel Ore");
		sprintf (buffer1, "(S) Organics");
	}
	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==5)
	{
		sprintf (buffer, "(S) Fuel Ore");
		sprintf (buffer2, "(S) Equipment");
	}			
	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==6)
	{
		sprintf (buffer1, "(S) Organics");
		sprintf (buffer2, "(S) Equipment");
	}
	if (isValidSectorOneOffset(currentSector) && sectors[curSector-1].info==7)
	{
		sprintf (buffer, "(S) Fuel Ore");
		sprintf (buffer1, "(S) Organics");
		sprintf (buffer2, "(S) Equipment");
	}

	if (type==1)
	{
		SetDlgItemText(frmHnd, IDC_PP_CURRENT_FUEL, buffer);
		SetDlgItemText(frmHnd, IDC_PP_CURRENT_ORG, buffer1);
		SetDlgItemText(frmHnd, IDC_PP_CURRENT_EQUIP, buffer2);
	}
	else
	{
		SetDlgItemText(frmHnd, IDC_PP_OTHER_FUEL, buffer);
		SetDlgItemText(frmHnd, IDC_PP_OTHER_ORG, buffer1);
		SetDlgItemText(frmHnd, IDC_PP_OTHER_EQUIP, buffer2);
	}

	GetDlgItemText(frmHnd, IDC_PP_CURRENT_EQUIP, buffer, sizeof(buffer) - 1);
	GetDlgItemText(frmHnd, IDC_PP_OTHER_EQUIP, buffer1, sizeof(buffer1) - 1);

	CheckDlgButton(frmHnd, IDC_PP_CURRENT_FUEL, BST_UNCHECKED);
	CheckDlgButton(frmHnd, IDC_PP_CURRENT_ORG, BST_UNCHECKED);
	CheckDlgButton(frmHnd, IDC_PP_CURRENT_EQUIP, BST_UNCHECKED);
	CheckDlgButton(frmHnd, IDC_PP_OTHER_FUEL, BST_UNCHECKED);
	CheckDlgButton(frmHnd, IDC_PP_OTHER_ORG, BST_UNCHECKED);
	CheckDlgButton(frmHnd, IDC_PP_OTHER_EQUIP, BST_UNCHECKED);

	if (strcmp (buffer, buffer1) == 0)
	{
		// disable both equips
	}
	else
	{
		if (checkCount<2)
		{
			if ((Instr(1, buffer, "(B)") && portOneBuyingCount==0) ||
				(Instr(1, buffer1, "(B)") && portTwoBuyingCount==0))
			{	
				CheckDlgButton(frmHnd, IDC_PP_CURRENT_EQUIP, BST_CHECKED);
				CheckDlgButton(frmHnd, IDC_PP_OTHER_EQUIP, BST_CHECKED);
				checkCount++;
			}

			if (Instr(1, buffer, "(B)"))
				portOneBuyingCount++;
			if (Instr(1, buffer1, "(B)"))
				portTwoBuyingCount++;
		}
	}

	GetDlgItemText(frmHnd, IDC_PP_CURRENT_ORG, buffer, sizeof(buffer) - 1);
	GetDlgItemText(frmHnd, IDC_PP_OTHER_ORG, buffer1, sizeof(buffer1) - 1);

	if (strcmp (buffer, buffer1) == 0)
	{
		// disable both orgs
	}
	else
	{
		if (checkCount<2)
		{
			if ((Instr(1, buffer, "(B)") && portOneBuyingCount==0) ||
				(Instr(1, buffer1, "(B)") && portTwoBuyingCount==0))
			{
				CheckDlgButton(frmHnd, IDC_PP_CURRENT_ORG, BST_CHECKED);
				CheckDlgButton(frmHnd, IDC_PP_OTHER_ORG, BST_CHECKED);
				checkCount++;
			}

			if (Instr(1, buffer, "(B)"))
				portOneBuyingCount++;
			if (Instr(1, buffer1, "(B)"))
				portTwoBuyingCount++;
		}
	}

	GetDlgItemText(frmHnd, IDC_PP_CURRENT_FUEL, buffer, sizeof(buffer) - 1);
	GetDlgItemText(frmHnd, IDC_PP_OTHER_FUEL, buffer1, sizeof(buffer1) - 1);

	if (strcmp (buffer, buffer1) == 0)
	{
		// disable both fuels
	}
	else
	{
		if (checkCount<2)
		{
			if ((Instr(1, buffer, "(B)") && portOneBuyingCount==0) ||
				(Instr(1, buffer1, "(B)") && portTwoBuyingCount==0))
			{
				CheckDlgButton(frmHnd, IDC_PP_CURRENT_FUEL, BST_CHECKED);
				CheckDlgButton(frmHnd, IDC_PP_OTHER_FUEL, BST_CHECKED);
				checkCount++;
			}

			if (Instr(1, buffer, "(B)"))
				portOneBuyingCount++;
			if (Instr(1, buffer1, "(B)"))
				portTwoBuyingCount++;
		}
	}

}

void showstealtransportform(HWND hwnd)
{
    if (!showstealtransport) 
	{
		showstealtransport = CreateDialog(hinst, MAKEINTRESOURCE(IDD_SELLTRANSPORT), hwnd, StealTransportProc);
		if ( showstealtransport!=NULL ) 
		{ 
			CheckRadioButton(showstealtransport, IDC_STEALTRANSPORT_SELL1, IDC_STEALTRANSPORT_DUMP1, IDC_STEALTRANSPORT_SELL1);	
			CheckRadioButton(showstealtransport, IDC_STEALTRANSPORT_SELL2, IDC_STEALTRANSPORT_DUMP2, IDC_STEALTRANSPORT_SELL2);	
			CheckRadioButton(showstealtransport, IDC_STEALTRANSPORT_SELL3, IDC_STEALTRANSPORT_DUMP3, IDC_STEALTRANSPORT_SELL3);	
			CheckRadioButton(showstealtransport, IDC_STEALTRANSPORT_SELL4, IDC_STEALTRANSPORT_DUMP4, IDC_STEALTRANSPORT_SELL4);	

//			SendDlgItemMessage(showstealtransport, IDC_STEALTRANSPORT_STEALING, CB_ADDSTRING, 0, (LPARAM) ("Fuel Ore"));
//			SendDlgItemMessage(showstealtransport, IDC_STEALTRANSPORT_STEALING, CB_ADDSTRING, 0, (LPARAM) ("Organics"));
			SendDlgItemMessage(showstealtransport, IDC_STEALTRANSPORT_STEALING, CB_ADDSTRING, 0, (LPARAM) ("Equipment"));
			SetDlgItemText(showstealtransport, IDC_STEALTRANSPORT_STEALING, "Equipment");

			SetDlgItemText(showstealtransport, IDC_STEALTRANSPORT_CYCLES, "9999");
			
			ShowWindow(showstealtransport,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Steal Transport Window", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(showstealtransport, SW_SHOWNORMAL); 
    
	SetActiveWindow(showstealtransport);
}

void showbustlistform(HWND hwnd)
{
	HANDLE hlist;

    if (!showbustlist) 
	{
		showbustlist = CreateDialog(hinst, MAKEINTRESOURCE(IDD_BUSTLIST), hwnd, BustListProc);
		if ( showbustlist!=NULL ) 
		{ 
			SetDlgItemText(showbustlist, IDC_BUSTLIST_DAYS, "14");
			SetDlgItemText(showbustlist, IDC_BUSTLIST_SECTOR, "0");

			hlist = GetDlgItem(showbustlist, IDC_BUSTLIST_LIST);
			listview_insert_column (hlist, 0, 100, 0, "Sector");
			listview_insert_column (hlist, 1, 100, 1, "Player");
			listview_insert_column (hlist, 2, 170, 2, "Day Busted");
			listview_insert_column (hlist, 3, 50, 3, "Day From Today");

			findBusts(hlist, -1, -1);

			ShowWindow(showbustlist,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Bust List", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showbustlist);
}


//build the bust list
void findBusts(HANDLE bustlisthandle, long bustDaysToClear_int, long sectorToClear_int)
{
	char *tmp[4];
	int i, bustCount=0;
	long secondsInDay = 86400;
	char buffer[50];
	long timeNow;
	time_t myTime;

	//find and clear any busts first
	if (bustDaysToClear_int != -1 && sectorToClear_int != -1)
	{
		timeNow = ATTAC_Time();

		for (i=0; isValidSectorZeroOffset(i) && i < header.sector_cnt; i++)
		{
			if (sectors[i].playerIdBusted==0 && 
				sectors[i].lastBusted != -1 &&
				(sectors[i].lastBusted+((bustDaysToClear_int+1)*secondsInDay)) < timeNow)
			{
				sectors[i].lastBusted=-1;
				sectors[i].playerIdBusted=-1;

				//clear last busted sector
				if (header.lastBustedSector-1==i)
					header.lastBustedSector=-1;
			}
		}

		if (isValidSectorZeroOffset(sectorToClear_int))
			sectors[sectorToClear_int-1].lastBusted=-1;
	}


	ListView_DeleteAllItems (bustlisthandle);


	for (i=0; isValidSectorZeroOffset(i) && i<header.sector_cnt; i++)
	{
		//only show my busts
		if (isBustedSector(i+1))
		{
			sprintf (buffer, "%d", i+1);
			tmp[0] = (char *) strdup(buffer);

			if (sectors[i].playerIdBusted==0)
				sprintf (buffer, "ME");
			else
				sprintf (buffer, "%s", players[sectors[i].playerIdBusted].name);
			tmp[1] = (char *) strdup(buffer);

			myTime = sectors[i].lastBusted;
			strcpy(buffer, ctime(&myTime));
			buffer[strlen(buffer)-1]=0;
			//reformat for string with time function
			sprintf (buffer, "%s", buffer);
			tmp[2] = (char *) strdup(buffer);

			sprintf (buffer, "%ld", (ATTAC_Time()-sectors[i].lastBusted)/secondsInDay);
			tmp[3] = (char *) strdup(buffer);

			listview_insert_bust(bustlisthandle, bustCount++, tmp[0], tmp[1], tmp[2], tmp[3]);

			#ifdef P_FREE
				pfree(&tmp[0]);
				pfree(&tmp[1]);
				pfree(&tmp[2]);
				pfree(&tmp[3]);
			#else
				free(tmp[0]);
				free(tmp[1]);
				free(tmp[2]);
				free(tmp[3]);
			#endif
		}
	}

}


//build the bust list
void clearBustsFunc()
{
	int i;

	long secondsInDay = 86400;
	long timeNow;

	timeNow = ATTAC_Time();
  
	for (i=0; isValidSectorZeroOffset(i) && i < header.sector_cnt; i++)
	{
		if (
			sectors[i].lastBusted != -1 &&
			(sectors[i].lastBusted+((header.daysToClearBust+1)*secondsInDay)) < timeNow)
		{
			sectors[i].lastBusted=-1;
			sectors[i].playerIdBusted=-1;
 
			//clear last busted sector
			if (header.lastBustedSector-1==i)
				header.lastBustedSector=-1;
		}
	}
  
}


void showmaxportsform(HWND hwnd)
{
	HANDLE hlist;

    if (!showmaxports) 
	{
		showmaxports = CreateDialog(hinst, MAKEINTRESOURCE(IDD_MAXEDPORT), hwnd, MaxPortsProc);
		if ( showmaxports!=NULL ) 
		{ 
			hlist = GetDlgItem(showmaxports, IDC_MAXEDPORT_LIST);

			listview_insert_column (hlist, 0, 50, 0, "Sector");
			listview_insert_column (hlist, 1, 90, 1, "Fuel Amount");
			listview_insert_column (hlist, 2, 50, 2, "Fuel %");
			listview_insert_column (hlist, 3, 90, 3, "Org. Amount");
			listview_insert_column (hlist, 4, 50, 4, "Org %");
			listview_insert_column (hlist, 5, 105, 5, "Equip. Amount");
			listview_insert_column (hlist, 6, 60, 6, "Equip. %");
			listview_insert_column (hlist, 7, 60, 7, "Distance");

			SendDlgItemMessage(showmaxports, IDC_MAXEDPORT_TYPE, CB_ADDSTRING, 0, (LPARAM) ("Fuel Ore"));
			SendDlgItemMessage(showmaxports, IDC_MAXEDPORT_TYPE, CB_ADDSTRING, 0, (LPARAM) ("Organics"));
			SendDlgItemMessage(showmaxports, IDC_MAXEDPORT_TYPE, CB_ADDSTRING, 0, (LPARAM) ("Equipment"));
			SetDlgItemText(showmaxports, IDC_MAXEDPORT_TYPE, "Equipment");

			SetDlgItemText(showmaxports, IDC_MAXEDPORT_AMOUNT, "10000");

			//0=fuel, 1=org, 2=equip
			generate_maxed_ports(hlist, 10000, 2);

			ShowWindow(showmaxports,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Max Ports", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showmaxports);
}


void showstealmoveform(HWND hwnd)
{
	char buffer[20];

    if (!showstealmove) 
	{
		showstealmove = CreateDialog(hinst, MAKEINTRESOURCE(IDD_STEALMOVE), hwnd, StealMoveProc);
		if ( showstealmove!=NULL ) 
		{ 
			sprintf (buffer, "%ld", currentSector);
			SetDlgItemText(showstealmove, IDC_STEALMOVE_SECTOR1, buffer);
//			SendDlgItemMessage(showstealmove, IDC_STEALMOVE_CARGO1, CB_ADDSTRING, 0, (LPARAM) ("Fuel Ore"));
//			SendDlgItemMessage(showstealmove, IDC_STEALMOVE_CARGO1, CB_ADDSTRING, 0, (LPARAM) ("Organics"));
			SendDlgItemMessage(showstealmove, IDC_STEALMOVE_CARGO1, CB_ADDSTRING, 0, (LPARAM) ("Equipment"));
			SetDlgItemText(showstealmove, IDC_STEALMOVE_CARGO1, "Equipment");
			CheckRadioButton(showstealmove, IDC_STEALMOVE_SELL1, IDC_STEALMOVE_DUMP1, IDC_STEALMOVE_SELL1);	

//			SendDlgItemMessage(showstealmove, IDC_STEALMOVE_CARGO2, CB_ADDSTRING, 0, (LPARAM) ("Fuel Ore"));
//			SendDlgItemMessage(showstealmove, IDC_STEALMOVE_CARGO2, CB_ADDSTRING, 0, (LPARAM) ("Organics"));
			SendDlgItemMessage(showstealmove, IDC_STEALMOVE_CARGO2, CB_ADDSTRING, 0, (LPARAM) ("Equipment"));
			SetDlgItemText(showstealmove, IDC_STEALMOVE_CARGO2, "Equipment");
			CheckRadioButton(showstealmove, IDC_STEALMOVE_SELL2, IDC_STEALMOVE_DUMP2, IDC_STEALMOVE_SELL2);	

			SetDlgItemText(showstealmove, IDC_STEALMOVE_CYCLES, "9999");
			
			ShowWindow(showstealmove,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Steal Window", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
	{
		sprintf (buffer, "%ld", currentSector);
		SetDlgItemText(showstealmove, IDC_STEALMOVE_SECTOR1, buffer);
		ShowWindow(showstealmove, SW_SHOWNORMAL); 
	}

    SetActiveWindow(showstealmove);
}

void showquasarcalculatorform(HWND hwnd)
{
    if (!showquasarcalculator) 
	{
		showquasarcalculator = CreateDialog(hinst, MAKEINTRESOURCE(IDD_QUASAR_CALC ), hwnd, QuasarCalculatorProc);
		if ( showquasarcalculator!=NULL ) 
		{ 
			ShowWindow(showquasarcalculator,SW_SHOWNORMAL); 
			SetDlgItemText(showquasarcalculator, IDC_QUASAR_CALC_SETTING, "Use NON-MBBS Settings (Atmos Blast * 2)");
			SendDlgItemMessage(showquasarcalculator, IDC_QUASAR_CALC_SETTING, CB_ADDSTRING, 0, (LPARAM) ("Use NON-MBBS Settings (Atmos Blast * 2)"));
			SendDlgItemMessage(showquasarcalculator, IDC_QUASAR_CALC_SETTING, CB_ADDSTRING, 0, (LPARAM) ("Use MBBS Settings (Atmos Blast / 2)"));
			CheckRadioButton(showquasarcalculator, IDC_QUASAR_CALC_SECTOR, IDC_QUASAR_CALC_ATMOSPHERE, IDC_QUASAR_CALC_SECTOR);			
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Quasar Calculator", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(showquasarcalculator);
}

void showetherexplorelogform(HWND hwnd)
{
	FILE * fp;
	char c;
	char line[1024];
	int lineCount=0;

	if (!showetherexplorelog) 
	{
		showetherexplorelog = CreateDialog(hinst, MAKEINTRESOURCE(IDD_ETHERPROBE_LOG), hwnd, EtherExploreLogProc);
		if ( showetherexplorelog!=NULL ) 
		{ 

			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_DATE, CB_RESETCONTENT, 0, 0);

			SetDlgItemText(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, "Everything");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Everything");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Info");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Class 0 Port");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Class 9 Port");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Beacon");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Fighters");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Armid Mines");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "NavHaz");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Planets");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Unmanned Ship");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Traders");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_LOG_FILTERLIST, CB_ADDSTRING, 0, (LPARAM) "Attacked");

			SetDlgItemText(showetherexplorelog, IDC_ETHERPROBE_DATE, "Everything");
			SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_DATE, CB_ADDSTRING, 0, (LPARAM) "Everything");

			fp = fopen ("ether_analysis.txt","r");
			if (fp==NULL)
			{
				attacErrorMessage ("Invalid filename: ether_analysis.txt");
			}
			else
			{
				c = fgetc(fp);

				while( !feof(fp)) 
				{
					if (c != '\n')
						line[lineCount++]=c;							
					else
					{
						line[lineCount]=0;

						if (Instr (1, line, "TIME:")>0)
							SendDlgItemMessage(showetherexplorelog, IDC_ETHERPROBE_DATE, CB_ADDSTRING, 0, (LPARAM) line);
						strcpy(line, "");
						lineCount=0;
					}
					c = fgetc(fp);
				}
				fclose(fp);
			}

			ShowWindow(showetherexplorelog,SW_SHOWNORMAL); 

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Ether Explore Log", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(showetherexplorelog, SW_SHOWNORMAL); 

    SetActiveWindow(showetherexplorelog);
}

void showetherexploreform(HWND hwnd)
{
	HICON hIcon;
    if (!showetherexplore) 
	{
		showetherexplore = CreateDialog(hinst, MAKEINTRESOURCE(IDD_ETHERPROBE), hwnd, EtherExploreProc);
		if ( showetherexplore!=NULL ) 
		{ 
			hIcon = (HICON)LoadImage(hinst,"IDI_OPENFOLDER",IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
			SendDlgItemMessage(showetherexplore, IDC_ETHERPROBE_BROWSE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			DestroyIcon(hIcon);

			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_UNEXPLORED, BST_CHECKED);

			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_CLASS0_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_CLASS9_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_BEACON_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_FIGHTERS_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_MINES_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_NAVHAZ_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_PLANETS_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_SHIPS_CHECK, BST_CHECKED);
			CheckDlgButton(showetherexplore, IDC_ETHERPROBE_TRADERS_CHECK, BST_CHECKED);

			ShowWindow(showetherexplore,SW_SHOWNORMAL); 
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Ether Explore", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
	else
		ShowWindow(showetherexplore, SW_SHOWNORMAL); 

    SetActiveWindow(showetherexplore);
}

void showmessagecenterform(HWND hwnd)
{

    if (!showmessagecenter) 
	{
		showmessagecenter = CreateDialog(hinst, MAKEINTRESOURCE(IDD_MESSAGE), hwnd, MessageProc);
		if ( showmessagecenter!=NULL ) 
		{ 
			set_MessageWindow_title ("ATTAC Message Center");
			ShowWindow(showmessagecenter,SW_SHOWNORMAL); 

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window messagecenterform", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }

    SetActiveWindow(showmessagecenter);
}

void showinfocenterform(HWND hwnd)
{

    if (!showinfocenter) 
	{
		showinfocenter = CreateDialog(hinst, MAKEINTRESOURCE(IDD_INFO), hwnd, InfoProc);
		if ( showinfocenter!=NULL ) 
		{ 
			ShowWindow(showinfocenter,SW_SHOWNORMAL); 

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window showinfocenter", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }

    SetActiveWindow(showinfocenter);
}

void showcimoptionsform(HWND hwnd)
{

    if (!showcimoptions) 
	{
		showcimoptions = CreateDialog(hinst, MAKEINTRESOURCE(IDD_CIM_OPTIONS), hwnd, CIMOptionsProc);
		if ( showcimoptions!=NULL ) 
		{ 
			ShowWindow(showcimoptions,SW_SHOWNORMAL); 

			if (header.cimUpdateSectorsDefault== 1)
				CheckDlgButton(showcimoptions, IDC_CIM_SECTOR, BST_CHECKED);
			if (header.cimUpdatePortsDefault == 1)
				CheckDlgButton(showcimoptions, IDC_CIM_PORT, BST_CHECKED);
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window CIM Options", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }

    SetActiveWindow(showcimoptions);
}

//we use timers to add these functions to the main message queue since they come from another thread
void CALLBACK ShowRexxAskTimer(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime )
{
	showrexxask(hWnd, rexxAskPrompt, rexxAskDefault);
	KillTimer(hWnd, nIDEvent);

	#ifdef P_FREE
		pfree(&rexxAskPrompt);
		pfree(&rexxAskDefault);
	#else
		free(rexxAskPrompt);
		free(rexxAskDefault);
	#endif
}


void showrexxask(HWND hwnd, char *promptData, char *defaultData)
{
	HANDLE htmp;
    if (!rexxask) 
	{
		rexxask = CreateDialog(hinst, MAKEINTRESOURCE(IDD_REXXASK), hwnd, REXXAskProc);
		if ( rexxask!=NULL ) 
		{ 
			ShowWindow(rexxask,SW_SHOWNORMAL); 

			if (strcmp (promptData, "") != 0)
				SetDlgItemText(rexxask, ID_REXXASKSTATIC, promptData);
			if (strcmp (defaultData, "") != 0)
				SetDlgItemText(rexxask, ID_EDITREXXASK, defaultData);

			htmp = GetDlgItem(rexxask, ID_EDITREXXASK);
			SetFocus(htmp);

		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Rexx Ask", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }

    SetActiveWindow(rexxask);
}

//we use timers to add these functions to the main message queue since they come from another thread
void CALLBACK ShowRexxAskPassTimer(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime )
{
	showrexxaskpassword(hWnd, rexxAskPrompt, rexxAskDefault);
	KillTimer(hWnd, nIDEvent);

	#ifdef P_FREE
		pfree(&rexxAskPrompt);
		pfree(&rexxAskDefault);
	#else
		free(rexxAskPrompt);
		free(rexxAskDefault);
	#endif
}

void showrexxaskpassword(HWND hwnd, char *promptData, char *defaultData)
{
	HANDLE htmp;
    if (!rexxask) 
	{
		rexxask = CreateDialog(hinst, MAKEINTRESOURCE(IDD_REXXASK_PASS), hwnd, REXXAskProc);
		if ( rexxask!=NULL ) 
		{ 
			ShowWindow(rexxask,SW_SHOWNORMAL); 

			if (strcmp (promptData, "") != 0)
				SetDlgItemText(rexxask, ID_REXXASKSTATIC, promptData);
			if (strcmp (defaultData, "") != 0)
				SetDlgItemText(rexxask, ID_EDITREXXASK, defaultData);

			htmp = GetDlgItem(rexxask, ID_EDITREXXASK);
			SetFocus(htmp);
		} 
		else 
		{ 
			MessageBox(NULL, "Error opening window Rexx Ask Password", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 

    }
    SetActiveWindow(rexxask);
}

//we use timers to add these functions to the main message queue since they come from another thread
void CALLBACK ShowRexxRequestTimer(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime )
{
	int I;

	showrexxrequest(hWnd, rexxRequestArgumentCount, rexxRequestArguments, rexxRequestList);
	KillTimer(hWnd, nIDEvent);

	for (I = 0; I < rexxRequestArgumentCount; I++)
	{
		#ifdef P_FREE
			pfree(&rexxRequestArguments[I]);
		#else
			free(rexxRequestArguments[I]);
		#endif
	}
}


void showrexxrequest(HWND hwnd, long RequestArgumentCount, char *RequestArguments[], int RequestList)
{
	int i;
	char *t;
	char *token;

    if (!rexxrequest) 
	{
		if (RequestList==1)
			rexxrequest = CreateDialog(hinst, MAKEINTRESOURCE(IDD_REXXREQUEST), hwnd, RexxRequestListProc);
		else
			rexxrequest = CreateDialog(hinst, MAKEINTRESOURCE(IDD_REXXREQUEST), hwnd, RexxRequestProc);
	    if (rexxrequest != NULL) 
		{
			ShowWindow(rexxrequest, SW_SHOWNORMAL);

			if (RequestArgumentCount>0)
				SetDlgItemText( rexxrequest, ID_REXXREQUESTSTATIC, RequestArguments[0]);
 
			if (RequestList==1 && RequestArgumentCount==2 && Instr (1, RequestArguments[1], "|")>0)
			{
				t = (char *) strdup (RequestArguments[1]);
				if (t != NULL)
				{
					token = strtok(t, "|");
					while (token != NULL)
					{
						SendDlgItemMessage(rexxrequest, ID_LISTREXXREQUEST, LB_ADDSTRING, 0, (LPARAM) token);
						token = strtok(NULL, "|");

					}	
					#ifdef P_FREE
						pfree(&t);
					#else
						free(t);
					#endif
				}

			}
			else
			{
				for (i = 1; i < RequestArgumentCount; i++)
				{
					SendDlgItemMessage(rexxrequest, ID_LISTREXXREQUEST, LB_ADDSTRING, 0, (LPARAM) RequestArguments[i]);
				}
			}
			if (RequestArgumentCount>0)
				SendDlgItemMessage(rexxrequest, ID_LISTREXXREQUEST, LB_SETCURSEL, 0, 0);
		}
		else 
		{ 
			MessageBox(NULL, "Error opening window Rexx Request", "ATTAC Error", MB_ICONWARNING); 
			return; 
		} 
	}

    SetActiveWindow(rexxrequest);
}

void set_MessageWindow_title(char *title)
{
    sfree(message_center_name);
    message_center_name = smalloc(1 + strlen(title));
    strcpy(message_center_name, title);
    if (!IsIconic(showmessagecenter))
	SetWindowText(showmessagecenter, title);
}

