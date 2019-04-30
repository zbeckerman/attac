//stores the name/value pairs for the xml attributes

#include "ConnectionInfo.h" 

//constructor
CConnectionInfo::CConnectionInfo() {}

//destructor
CConnectionInfo::~CConnectionInfo() {}

SOCKET CConnectionInfo::getTWXSyncSocket() {
	return sock;
}

void CConnectionInfo::setTWXSyncSocket(SOCKET s) {
	s = sock;
}
