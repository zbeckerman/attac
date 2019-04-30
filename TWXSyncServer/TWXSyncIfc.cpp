// TWXSyncIfc.cpp: implementation of the CTWXSyncIfc class.
//

#include "stdafx.h"
#include "TWXSyncServer.h"
#include "TWXSyncIfc.h"
#include "..\TWCommon\TWCommon.h"

CList CTWXSyncIfc::m_list;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CTWXSyncIfc::CTWXSyncIfc()
{
	m_hTWXSync=NULL;
	m_pHost=NULL;
	m_nPort=0;

	// Add to static list
	m_list.Add(this);
}

CTWXSyncIfc::~CTWXSyncIfc()
{
	if (m_hTWXSync != NULL) {
		TWC_DeleteTWXSyncSession(m_hTWXSync);
	}
	if (m_pHost != NULL) {
		free(m_pHost);
	}

	// Remove from static list
	m_list.Remove(this);
}

int CTWXSyncIfc::Init(int startServer)
{
	char appDir[1024];
	int result=-1;
	m_hTWXSync=TWC_CreateTWXSyncSession();
	if (m_hTWXSync == NULL) {
		return FALSE;
	}

	result = TWC_TWXSync_Init(m_hTWXSync, &IncomingDataCallback, &OutgoingDataCallback, &ErrorCallback);
	if (result != 0) {
		return result;
	}
	
	GetCurrentDirectory (1024, appDir);
	strcat (appDir, "\\");

	if (startServer==1) {
		result = TWC_TWXSync_StartServer(m_hTWXSync, appDir);
		if (result != 0) {
			return result;
		}
	}
	return result;
}

long CTWXSyncIfc::GetHandle()
{
	return m_hTWXSync;
}

CTWXSyncIfc *CTWXSyncIfc::GetObject(long hTWXSync)
{
	CTWXSyncIfc *pTWXSyncIfc;
	int i,n;

	n=m_list.GetCount();
	for (i=0; i<n; i++) {
		pTWXSyncIfc=(CTWXSyncIfc *)m_list.GetAt(i);
		if (pTWXSyncIfc->GetHandle() == hTWXSync) {
			return pTWXSyncIfc;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////

BOOL CTWXSyncIfc::Listen(int port)
{
	if (TWC_TWXSync_Listen(m_hTWXSync, &NewConnectionCallback, &ValidUserLoginCallback, port, (__int64) 5515596313) != 0) {
		return FALSE;
	}

	return TRUE;
}

BOOL CTWXSyncIfc::StopListen(unsigned threadId)
{
	if (TWC_TWXSync_StopListen(m_hTWXSync, threadId) != 0) {
		return FALSE;
	}

	return TRUE;
}

BOOL CTWXSyncIfc::TerminateConnection(char *ipaddress)
{
	if (TWC_TWXSync_TerminateConnection(m_hTWXSync, ipaddress) != 0) {
		return FALSE;
	}

	return TRUE;
}

void CTWXSyncIfc::IncomingData(const char *data, int len)
{
	// Print the data
//	printf(data);
//	printf ("Receiving Data...\n");
	TWC_HandleRequest(m_hTWXSync, data, len);
	

}

void CTWXSyncIfc::OutgoingData(const char *data, int len)
{
	// Print the data
//	printf ("Sending Data...\n");

}

void CTWXSyncIfc::Error(int errorCode, const char *msg, int len)
{
	char buffer[256];
	sprintf (buffer, "Message: %s", msg);
	SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, buffer);
}

void CTWXSyncIfc::NewConnection(long hNewTWXSync, const char *host, int port, const char *username, const char *helper, const char *game)
{
	char buffer[50];
	// New connection
	m_hTWXSync=hNewTWXSync;
	m_pHost=strdup(host);
	m_nPort=port;

	sprintf (buffer, "%s:%d", host, port);
	listview_insert(GetDlgItem(gDlg, IDC_LCONNECTIONS), 0, buffer, (char *) "Connecting...", Get_Time(), (char *) "n/a", (char *)game);
}

//////////////////////////////////////////////////////////////////////

void CTWXSyncIfc::OutgoingDataCallback(long hTWXSync, const char *data, int len)
{
	CTWXSyncIfc *pTWXSyncIfc=GetObject(hTWXSync);
	if (pTWXSyncIfc == NULL) {
		fprintf(stderr, "Error: Unknown call to IncomingDataCallback\n");
		return;
	}

	pTWXSyncIfc->OutgoingData(data, len);
}


void CTWXSyncIfc::IncomingDataCallback(long hTWXSync, const char *data, int len)
{
	CTWXSyncIfc *pTWXSyncIfc=GetObject(hTWXSync);
	char buffer[50];
	int indexId=0;
	HWND hlist=NULL;

	if (pTWXSyncIfc == NULL) {
		fprintf(stderr, "Error: Unknown call to IncomingDataCallback\n");
		return;
	}

	if (len == 0) {
		// Connection closed
		sprintf (buffer, "%s:%d", pTWXSyncIfc->m_pHost, pTWXSyncIfc->m_nPort);
		if (usersConnected>0)
			usersConnected--;

		hlist = GetDlgItem(gDlg, IDC_LCONNECTIONS);
		
		if (hlist != NULL) {
			//find item in connections
			indexId = listview_find_item(hlist, buffer);

			//add item to history 
			if (indexId!=-1)	//delete item
			{
				char host[128];
				char user[128];
				char time[128];
				char helper[128];
				char game[128];
				char *tt=NULL;

				tt = listview_get_column_text (hlist, indexId, 0);
				strcpy (host, tt);
				free(tt);
				tt = listview_get_column_text (hlist, indexId, 1);
				strcpy (user, tt);
				free(tt);
				tt = listview_get_column_text (hlist, indexId, 2);
				strcpy (time, tt);
				free(tt);
				tt = listview_get_column_text (hlist, indexId, 3);
				strcpy (helper, tt);
				free(tt);
				tt = listview_get_column_text (hlist, indexId, 4);
				strcpy (game, tt);
				free(tt);

				listview_insert_c(GetDlgItem(gDlg, IDC_LOLD), 0, host, user, time, helper, game);
			}
			else
				listview_insert(GetDlgItem(gDlg, IDC_LOLD), 0, buffer, "n/a", Get_Time(), "n/a", "n/a");


			if (indexId!=-1)	//delete item
				listview_delete_item(hlist, indexId);

		}

		delete pTWXSyncIfc;
	}
	else {
		pTWXSyncIfc->IncomingData(data, len);
	}
}

void CTWXSyncIfc::ErrorCallback(long hTWXSync, int errorCode, const char *msg, int len)
{
	CTWXSyncIfc *pTWXSyncIfc=GetObject(hTWXSync);
	if (pTWXSyncIfc == NULL) {
		fprintf(stderr, "Error: Unknown call to ErrorCallback\n");
		return;
	}

	pTWXSyncIfc->Error(errorCode, msg, len);
}

void CTWXSyncIfc::NewConnectionCallback(long hTWXSync, long hNewTWXSync, const char *host, int port, const char *username, const char *helper, const char *game)
{
	// New connection
	printf("\nNew connection: %s:%d\n", host, port);
	
	if (validRegCode==1 || usersConnected==0) {
		CTWXSyncIfc *pTWXSyncIfc=new CTWXSyncIfc();
		pTWXSyncIfc->NewConnection(hNewTWXSync, host, port, username, helper, game);
		usersConnected++;
	} else {
		SetDlgItemText(gDlg, IDC_TWXSYNC_SERVER_STATUS, "unregistered. only 1 user connection allowed");
	}

}

void CTWXSyncIfc::ValidUserLoginCallback(long hTWXSync, const char *host, int port, const char *username, long timeConnected, const char *helper, const char *game)
{
	char buffer[128];
	sprintf (buffer, "%s:%d", host, port);
	//find item in connections
	int indexId = listview_find_item(GetDlgItem(gDlg, IDC_LCONNECTIONS), (char *) buffer);
	if (indexId!=-1)	//delete item
		listview_update_item (GetDlgItem(gDlg, IDC_LCONNECTIONS), indexId, (char *) host, port, (char *)username, timeConnected, (char *)helper, (char *) game);
}
