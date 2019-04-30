#ifndef _C_HOLDER_
#define _C_HOLDER_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <winsock2.h>
#include "AuthenticationServer.h"

class CHolder
{
private:
	SOCKET m_socket;

public:
	CHolder();
	virtual ~CHolder();

	SOCKET GetSocket();
	void SetSocket(SOCKET socket);
};

#endif