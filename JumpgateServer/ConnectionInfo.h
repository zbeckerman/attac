#ifndef _C4_DATA_PAIR 
#define _C4_DATA_PAIR 

#include <windows.h> 

class CConnectionInfo {

 public:

	CConnectionInfo();
	virtual ~CConnectionInfo();

	SOCKET getTWXSyncSocket();
	void setTWXSyncSocket(SOCKET s);

 private:
	SOCKET sock;
	long timeConnected;

};

#endif  // _C4_DATA_PAIR 

