// Holder.cpp: implementation of the CHolder class.
//

#include "stdafx.h"
#include "Holder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CHolder::CHolder()
{
}

CHolder::~CHolder()
{
}

//////////////////////////////////////////////////////////////////////

SOCKET CHolder::GetSocket()
{
	return m_socket;
}

void CHolder::SetSocket(SOCKET socket)
{
	m_socket = socket;
}
