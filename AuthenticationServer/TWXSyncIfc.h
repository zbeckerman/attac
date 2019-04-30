// TWXSyncIfc.h: interface for the CTWXSyncIfc class.
//

#if !defined(AFX_TWXSYNCIFC_H__D098F4E1_A71D_4E0C_A74D_B8E3165B3894__INCLUDED_)
#define AFX_TWXSYNCIFC_H__D098F4E1_A71D_4E0C_A74D_B8E3165B3894__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "List.h"
#include "resource.h"

class CTWXSyncIfc
{
public:
	CTWXSyncIfc();
	virtual ~CTWXSyncIfc();

	int Init(int i);
	BOOL Listen(int port);
	BOOL StopListen(unsigned threadId);
	BOOL TerminateConnection(char *ipaddress);

	void IncomingData(const char *data, int len);
	void OutgoingData(const char *data, int len);

	void Error(int errorCode, const char *msg, int len);
	void NewConnection(long hNewTWXSync, const char *host, int port, const char *username, const char *helper);

	long m_expat;
protected:
	long GetHandle();
	static CTWXSyncIfc *GetObject(long hTWXSync);

	static void IncomingDataCallback(long hTWXSync, const char *data, int len);
	static void OutgoingDataCallback(long hTWXSync, const char *data, int len);
	static void ErrorCallback(long hTWXSync, int errorCode, const char *msg, int len);
	static void NewConnectionCallback(long hTWXSync, long hNewTWXSync, const char *host, int port, const char *username, const char *helper);
	static void ValidUserLoginCallback(long hTWXSync, const char *host, int port, const char *username, long timeConnected, const char *helper);
				

private:
	static CList m_list;
	long m_hTWXSync;
	char *m_pHost;
	int m_nPort;
};

#endif // !defined(AFX_TWXSYNCIFC_H__D098F4E1_A71D_4E0C_A74D_B8E3165B3894__INCLUDED_)
