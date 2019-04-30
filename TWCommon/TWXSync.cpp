// TWXSync.cpp: implementation of the CTWXSync class.
//used
//http://www.winimage.com/zLibDll/


#include <winsock2.h>
#include <time.h>
#include <process.h>    /* _beginthread, _endthread */
#include <sys/stat.h>

#include "Common.h"
#include "TWXSync.h"
#include "Expat.h"
#include "List.h"
#include "Hash.h"
#include "Authorize.h"

#include "./3PP/expat/expat.h"

CList mTWXSyncConnectedList;
CTWXSync *m_ServerPointer=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction


CTWXSync::CTWXSync()
{
	char *tmpGUID=NULL;
	

	m_clientSocket=NULL;
	m_serverSocket=NULL;
	m_clientHostname=NULL;
	m_depth=0;
	m_emptyTag=0;
	parser = XML_ParserCreate(NULL);

	parseSafeMutex = CreateMutex(NULL, 0, NULL);
	threadSafeMutex = CreateMutex(NULL, 0, NULL);
	recvThreadSafeMutex = CreateMutex(NULL, 0, NULL);
	xmlImportThreadSafeMutex = CreateMutex(NULL, 0, NULL);

	tmpGUID = TWC_CreateGUID();
	strcpy (uniqueId, tmpGUID);
	pfree((void **)&tmpGUID);

	strcpy (returnBuffer, "");
	returnError=0;
	loginPassCheck=0;
	packetCountSent=0;
	strcpy (username, "");
	strcpy (helper, "");
	strcpy (requestType, "");
	strcpy (gameName, "n/a");
	SetSkipParse(0);
	begin=0;
	end=Get_Time();
	
	WaitForSingleObject(zlibEngineMutex, INFINITE);
	engine = new CZlibEngine();
	ReleaseMutex(zlibEngineMutex);

	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, ReqStartElement, ReqEndElement);
	XML_SetCharacterDataHandler(parser, ReqCharacterData);
}


CTWXSync::~CTWXSync()
{
	XML_ParserFree(parser);

	CloseHandle (threadSafeMutex);
	CloseHandle (recvThreadSafeMutex);
	CloseHandle (xmlImportThreadSafeMutex);
	
	threadSafeMutex=NULL;
	recvThreadSafeMutex=NULL;
	xmlImportThreadSafeMutex=NULL;

	WaitForSingleObject(zlibEngineMutex, INFINITE);
	if (engine != NULL)
		delete engine;
	ReleaseMutex(zlibEngineMutex);

	if (m_clientHostname != NULL) {
		pfree((void **)&m_clientHostname);
	}
}

/**
 * Gets the handle for this TWX sync object.
 *
 * @return The TWX sync handle
 */
long CTWXSync::GetHandle()
{
	return (long)this;
}

/**
 * Gets the TWX sync object from a given handle.
 *
 * @param The TWX sync handle
 * @return The TWX sync object
 */
CTWXSync *CTWXSync::GetObject(long hTWXSync)
{
	return (CTWXSync *)hTWXSync;
}

/**
 * Initializes the TWX sync object.
 *
 * @param pIncomingDataCallback A callback function for incoming data
 * @param pErrorCallback A callback function for errors
 * @return Nonzero if failed
 */
int CTWXSync::Init(void (*pIncomingDataCallback)(long hTWXSync, const char *data, int len),
				   void (*pOutgoingDataCallback)(long hTWXSync, const char *data, int len),
				   void (*pErrorCallback)(long hTWXSync, int errorCode, const char *msg, int len))
{
	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return -1;
	}
	WaitForSingleObject(threadSafeMutex, INFINITE);

	if (pIncomingDataCallback == NULL) {
		sprintf(s_error_msg, "No callback function for incoming data");
		ReleaseMutex(threadSafeMutex);
		return -1;
	}
	if (pOutgoingDataCallback == NULL) {
		sprintf(s_error_msg, "No callback function for outgoing data");
		ReleaseMutex(threadSafeMutex);
		return -1;
	}
	if (pErrorCallback == NULL) {
		sprintf(s_error_msg, "No callback function for errors");
		ReleaseMutex(threadSafeMutex);
		return -1;
	}

	m_pIncomingDataCallback=pIncomingDataCallback;
	m_pOutgoingDataCallback=pOutgoingDataCallback;
	m_pErrorCallback=pErrorCallback;

	ReleaseMutex(threadSafeMutex);
	return 0;
}


/**
 * Opens a port and starts listen in a thread.
 *
 * @param pNewConnectionCallback A callback function for new connections
 * @param port The port number to listen on
 * @return Nonzero if failed
 */
int CTWXSync::StopListen(unsigned threadId)
{
	int n=0, i=0;

	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return -1;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	WaitForSingleObject(ListOfConnectedUsersMutex, INFINITE);
	for (i=0; i<mTWXSyncConnectedList.GetCount(); i++) {
		CTWXSync *tempTWXSync = (CTWXSync*) mTWXSyncConnectedList.GetAt(i);
		closesocket(tempTWXSync->GetClientSocket());
	}
	ReleaseMutex(ListOfConnectedUsersMutex);

	SOCKET s = m_ServerPointer->GetServerSocket();
	n = closesocket(s);

	ReleaseMutex(threadSafeMutex);
	return n;
}


/**
 * Opens a port and starts listen in a thread.
 *
 * @param pNewConnectionCallback A callback function for new connections
 * @param port The port number to listen on
 * @return Nonzero if failed
 */
int CTWXSync::TerminateConnection(const char *ipaddress)
{
	int n=0, i=0;

	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return -1;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	WaitForSingleObject(ListOfConnectedUsersMutex, INFINITE);
	for (i=0; i<mTWXSyncConnectedList.GetCount(); i++) {
		char buffer[128];
		CTWXSync *tempTWXSync = (CTWXSync*) mTWXSyncConnectedList.GetAt(i);
		sprintf (buffer, "%s:%d", tempTWXSync->m_clientHostname, tempTWXSync->m_clientPort);
		if (strcmp (buffer, ipaddress)==0) {
			closesocket(tempTWXSync->GetClientSocket());
		}
	}
	ReleaseMutex(ListOfConnectedUsersMutex);

	ReleaseMutex(threadSafeMutex);
	return n;
}

/**
 * Opens a port and starts listen in a thread.
 *
 * @param pNewConnectionCallback A callback function for new connections
 * @param port The port number to listen on
 * @return Nonzero if failed
 */
unsigned CTWXSync::Listen(void (*pNewConnectionCallback)(long hTWXSync, long hNewTWXSync, const char *host, int port, const char *username, const char *helper, const char *game),
						  void (*pValidUserLoginCallback)(long hTWXSync, const char *host, int port, const char *username, long timeConnected, const char *helper, const char *game),
					 unsigned short port, __int64 handle)
{
	unsigned threadId=-1;

	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return -1;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	if (handle % 71593!=0) {
		ReleaseMutex(threadSafeMutex);
		return rand();
	}

	if (pNewConnectionCallback == NULL) {
		sprintf(s_error_msg, "No callback function for new connections");
		ReleaseMutex(threadSafeMutex);
		return -1;
	}
	if (pValidUserLoginCallback == NULL) {
		sprintf(s_error_msg, "No callback function for valid users");
		ReleaseMutex(threadSafeMutex);
		return -1;
	}
	m_pNewConnectionCallback=pNewConnectionCallback;
	m_pValidUserLoginCallback=pValidUserLoginCallback;

	SetServerPortNumber(port);

	if(_beginthreadex(NULL, 0, ListenServerThreadProc, (void *)this, 0, &threadId )==-1)
	{
		printf("Failed to create thread");
		ReleaseMutex(threadSafeMutex);
		return -1;
	}

	ReleaseMutex(threadSafeMutex);
	return threadId;
}


/**
 * Connects to a remote TWX sync connection.
 *
 * @param host The remote host address
 * @param port The remote port number
 * @return Nonzero if failed
 */
int CTWXSync::StartServer(const char *m_appDir)
{

	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return -1;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	strcpy (appDir, m_appDir);
	int returnVal = openAllTWXFiles();

	ReleaseMutex(threadSafeMutex);
	return returnVal;
}
/**
 * Connects to a remote TWX sync connection.
 *
 * @param host The remote host address
 * @param port The remote port number
 * @return Nonzero if failed
 */
int CTWXSync::Connect(const char *host, int port)
{
	WSADATA wsaData;
    struct sockaddr_in our_addr;
	SOCKET sock=NULL;
	struct hostent *host_info=NULL;

	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return -1;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	SetClientHost(host);
	SetClientPortNumber((unsigned short)port);
	SetClientSocket(NULL);

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) 	{     
		sprintf(s_error_msg, "Winsock startup error: %d", WSAGetLastError());
		ReleaseMutex(threadSafeMutex);
		return -1;
	}

	// Create the socket 
	sock = socket(AF_INET,SOCK_STREAM,0);
	// Set up our address
	our_addr.sin_family = AF_INET;
	// Translate the host into the structure	
	host_info = gethostbyname(GetClientHost());
    if (host_info == NULL) {
        sprintf(s_error_msg, "Unknown server: %s\n", GetClientHost());
		WSACleanup();
		ReleaseMutex(threadSafeMutex);
		return -1;
    }

	// Memcopy data
    memcpy((char *)&our_addr.sin_addr, host_info->h_addr, host_info->h_length);
	// Set the client port
	our_addr.sin_port = htons(GetClientPortNumber());

	// Connect to server
	if (connect(sock, (struct sockaddr*)&our_addr, sizeof( sockaddr_in)) == SOCKET_ERROR) {
		sprintf(s_error_msg, "Winsock connection error: %d", WSAGetLastError());
		WSACleanup();
		ReleaseMutex(threadSafeMutex);
		return -1;
	}

	SetClientSocket(sock);

	ReleaseMutex(threadSafeMutex);
	return 0;
}

/**
 * Closes the current TWX sync connection.
 */
void CTWXSync::Close()
{
	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	SOCKET sock;
	if ((sock=GetClientSocket()) != NULL) {
		closesocket(sock);
		SetClientSocket(NULL);
	}

	ReleaseMutex(threadSafeMutex);
}

/**
 * Sends TWX v2 XML data to the other side.
 *
 * @param data The data to send
 * @param len The length of the data
 * @return Nonzero if failed
 */
int CTWXSync::Send(const char *data, int len)
{
/*
	CUnsignedCharStr* dataToCompress = new CUnsignedCharStr("TEST123 this is CRC_321029384785");

	unsigned char * x2 = dataToCompress->getCrc32FromString(16);
	free(x2);

    CUnsignedCharStr* compress = engine.compressInMemory(dataToCompress);

	unsigned long x = compress->crc32();
	CUnsignedCharStr* decompress = engine.decompressInMemory(compress);

	CUnsignedCharStr* reCompress = engine.compressInMemory(decompress);

	unsigned long o = reCompress->crc32();

	delete reCompress;
	delete dataToCompress;
	delete compress;
	delete decompress;
*/
  
	int distance=0;
	int m=0, i=0;
//	char buffer[64];
	unsigned char *ptr; 
	int n;
	
	n=0;

	if (threadSafeMutex==NULL) {
		sprintf(s_error_msg, "NULL threadSafeMutex");
		return -1;
	}

	WaitForSingleObject(threadSafeMutex, INFINITE);

	SOCKET sock=GetClientSocket();

	if (sock!= NULL) {
		WaitForSingleObject(zlibEngineMutex, INFINITE);
		n=TWC_TWXSync_Encode_With_Engine(engine, (char *)data, &ptr); 
		ReleaseMutex(zlibEngineMutex);
		/*
		//compress data
		CUnsignedCharStr* dataToCompress = new CUnsignedCharStr((char *)data);

		//encrypt before zlib compression
		dataToCompress->encryptString();

		CUnsignedCharStr* compress = engine.compressInMemory(dataToCompress);

		//encrypt after zlib compression
		sprintf (buffer, "%lu", compress->crc32());

		compress->appendString((unsigned char *)CRC_32, CRC_32_LEN);

		compress->appendString((unsigned char*)buffer, strlen(buffer));
		
		compress->appendString((unsigned char *)DATA_COMPLETE, DATA_COMPLETE_LEN);

		//get compressed string. new memory is allocated
		unsigned char *tmp = compress->getString();
*/
		//send compressed data. add null char
		m = send( sock, (char *)ptr, n, 0);

		//free the compressed string since we send then one with data_complete
		pfree ((void **) &ptr);	

		//delete compressed data
//		delete dataToCompress;
//		delete compress;
		
		if (n>0)
			packetCountSent++;
	}
	
	ReleaseMutex(threadSafeMutex);
	return m;
}

/**
 * Receive TWX v2 XML data to the other side.
 *
 * @param data The data to send
 * @param len The length of the data
 * @return Nonzero if failed
 */
int CTWXSync::ReceiveAllDataWrapper(char **data)
{
	int n=0;
	int completed=0;
	char *returnValue=NULL;
	int r=1;

	WaitForSingleObject(recvThreadSafeMutex, INFINITE);

	SOCKET sock = GetClientSocket();
	char buffer[SOCKET_BUFFER];

	CUnsignedCharStr* incomingData=new CUnsignedCharStr();
	while (!completed && !(r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET)) {
		r = Receive(buffer, sizeof(buffer)-1);
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
		packetCountReceived++;
		char *res = strdup (""); 
		char *isInvalidCrc32 = strdup ("0");
		unsigned char *ptr = incomingData->getString();
	
		WaitForSingleObject(zlibEngineMutex, INFINITE);
		n=TWC_TWXSync_Decode_With_Engine(engine, ptr, incomingData->getLength(), &res, isInvalidCrc32); 
		if (strcmp (isInvalidCrc32, "1")==0) {
			strcpy (returnBuffer, res);
			SetSkipParse(1);
		}
		ReleaseMutex(zlibEngineMutex);
	
		pfree((void **) &ptr);

		*data = res;
		/*
		CUnsignedCharStr* test=new CUnsignedCharStr();
		test->appendString(incomingData->getString(), incomingData->getLength());

		//check if the crc32 token exists
		int result = incomingData->hasString(CRC_32);
		//default is that the packet is ok
		int validCrc32=1;
		unsigned char* crc32=NULL;
		if (result!=-1) {
			//if i found me token, then get the crc32 value and trim the crc token
			//and value from the string
			crc32 = incomingData->getCrc32FromString(result);
			incomingData->trimStringRight(incomingData->getLength()-result);
			//confirm that the crc32 value is good
			validCrc32 = incomingData->confirmCrc32(crc32);
			//free the memory from getting the crc32 value
			pfree((void **) &crc32);
		}

		if (validCrc32==0) {
			//if i found me token, then get the crc32 value and trim the crc token
			//and value from the string
			crc32 = test->getCrc32FromString(result);
			test->trimStringRight(test->getLength()-result);
			//confirm that the crc32 value is good
			validCrc32 = test->confirmCrc32(crc32);
			//free the memory from getting the crc32 value
			pfree((void **) &crc32);

		}
		//if the Crc32 check is valid or no crc32 was sent
		if (validCrc32==1) {
			//decompress the data
			CUnsignedCharStr* decompress = engine.decompressInMemory(incomingData);

			//decrypt after zlib decompression 
			decompress->decryptString();

			//save the decompress string. new memory is allocated
			*data = (char *) decompress->getString();

			n=decompress->getLength();
			//delete decompression memory
			delete decompress;
		} else {
			//if it was invalid, just pretend the packet was empty
			char buf[256];
			FILE *ff=NULL;
			srand( (unsigned)time( NULL ) );
			
			sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"></TWXSync>", TWXSYNX_VERSION);
			*data = strdup (buf);
			n = strlen (*data);

			//printout the debug info
			sprintf(buf, "c:\\TWXSyncDebug%d.txt", rand());
			ff = fopen (buf, "a");
			if (ff!= NULL) {
				fprintf (ff,"\n\n------------ReceiveAllDataWrapper---------------\n");
				fprintf (ff,"Bad Packet crc32 Invalid: Length %d; crc32 %s\n", incomingData->getLength(), (crc32==NULL?"(char*)(null val)":(char*)crc32));
				fclose(ff);

			}
		}
			*/
	} else {
		*data = (char *)strdup ("");
		n=0;
	}

	delete incomingData;

	ReleaseMutex(recvThreadSafeMutex);
	return n;
}


/**
 * Receive TWX v2 XML data to the other side.
 * Private function. Not callable from client so no Mutex here
 *
 * @param data The data to send
 * @param len The length of the data
 * @return Nonzero if failed
 */
int CTWXSync::Receive(const char *data, int len)
{
	int n=0;

	SOCKET sock=GetClientSocket();
	if (sock != NULL) {
		n = recv( sock, (char *)data, len, 0);
		((char *) data)[n]=0;
	}

	return n;
}


//////////////////////////////////////////////////////////////////////

int CTWXSync::GetDepth()
{
	return m_depth;
}

void CTWXSync::SetDepth(int d)
{
	m_depth = d;
}

int CTWXSync::GetSkipParse()
{
	return m_skipParse;
}

void CTWXSync::SetSkipParse(int d)
{
	m_skipParse = d;
}


CList CTWXSync::GetAttrList()
{
	return m_list;
}

void CTWXSync::SetAttrList(CList t_list)
{
	m_list = t_list;
}


unsigned short CTWXSync::GetClientPortNumber()
{
	return m_clientPort;
}

void CTWXSync::SetClientPortNumber(unsigned short port)
{
	m_clientPort = port;
}

const char* CTWXSync::GetClientHost()
{
	return m_clientHostname;
}

void CTWXSync::SetClientHost(const char *hostname)
{
	m_clientHostname = strdup(hostname);
}



unsigned short CTWXSync::GetServerPortNumber()
{
	return m_serverPort;
}

void CTWXSync::SetServerPortNumber(unsigned short port)
{
	m_serverPort = port;
}

SOCKET CTWXSync::GetClientSocket()
{
	return m_clientSocket;
}

void CTWXSync::SetClientSocket(SOCKET s)
{
	m_clientSocket = s;
}

SOCKET CTWXSync::GetServerSocket()
{
	return m_serverSocket;
}

void CTWXSync::SetServerSocket(SOCKET s)
{
	m_serverSocket = s;
}

int CTWXSync::compareObject(CTWXSync *userData) {
	CTWXSync *pTWXSync = (CTWXSync*)userData;

	int thisLen = strlen (uniqueId);
	int thatLen = strlen (userData->uniqueId);

	return (memcmp(uniqueId, userData->uniqueId, (thisLen>thatLen? thatLen: thisLen)));
}

//////////////////////////////////////////////////////////////////////

int CTWXSync::FireIncomingDataCallback(const char *data, int len)
{
	if (m_pIncomingDataCallback == NULL) {
		sprintf(s_error_msg, "No callback function for incoming data");
		return -1;
	}

	(*m_pIncomingDataCallback)(GetHandle(), data, len);

	return 0;
}

int CTWXSync::FireOutgoingDataCallback(const char *data, int len)
{
	if (m_pOutgoingDataCallback == NULL) {
		sprintf(s_error_msg, "No callback function for outgoing data");
		return -1;
	}

	(*m_pOutgoingDataCallback)(GetHandle(), data, len);

	return 0;
}

int CTWXSync::FireErrorCallback(int errorCode, const char *msg, int len)
{
	if (m_pErrorCallback == NULL) {
		sprintf(s_error_msg, "No callback function for errors");
		return -1;
	}

	(*m_pErrorCallback)(GetHandle(), errorCode, msg, len);

	return 0;
}

int CTWXSync::FireNewConnectionCallback(CTWXSync *pNewTWXSync)
{
	if (m_pNewConnectionCallback == NULL) {
		sprintf(s_error_msg, "No callback function for new connections");
		return -1;
	}
	if (pNewTWXSync == NULL) {
		sprintf(s_error_msg, "Invalid TWX sync object");
		return -1;
	}

	(*m_pNewConnectionCallback)(GetHandle(), pNewTWXSync->GetHandle(),
		pNewTWXSync->GetClientHost(), pNewTWXSync->GetClientPortNumber(), pNewTWXSync->username, pNewTWXSync->helper, pNewTWXSync->gameName);

	return 0;
}

int CTWXSync::FireValidUserLoginCallback(CTWXSync *pTWXSync)
{
	if (m_pValidUserLoginCallback == NULL) {
		sprintf(s_error_msg, "No callback function for new connections");
		return -1;
	}
	if (pTWXSync == NULL) {
		sprintf(s_error_msg, "Invalid TWX sync object");
		return -1;
	}

	(*m_pValidUserLoginCallback)(GetHandle(),
		(const char *) pTWXSync->GetClientHost(), pTWXSync->GetClientPortNumber(), pTWXSync->username, timeConnected, pTWXSync->helper, pTWXSync->gameName);

	return 0;
}

//////////////////////////////////////////////////////////////////////
void CTWXSync::HandleRequest(const char *data, int len)
{
	WaitForSingleObject(SyncUsersMutex, INFINITE);
	WaitForSingleObject(this->xmlImportThreadSafeMutex, INFINITE);

	int status = XML_Parse(parser, data, len, len);
	if (status == XML_STATUS_ERROR) {
		sprintf(s_error_msg, "XML parse error at line %d: %s",
		XML_GetCurrentLineNumber(parser),
		XML_ErrorString(XML_GetErrorCode(parser)));

		
		char buf[128];
		FILE *ff;
		srand( time(NULL) );

		sprintf(buf, "%sTWXSyncDebug.txt", appDir);
		ff = fopen (buf, "a+");
		if (ff!= NULL) {
			fprintf (ff,"\n\n------------XML_Parse---------------\n");
			fprintf (ff,"XML Parse Error: %s\nError is:%s\n", data, s_error_msg);
			fclose(ff);

		}

		FireErrorCallback(0, s_error_msg, strlen(s_error_msg));
	
		ReleaseMutex(this->xmlImportThreadSafeMutex);
		ReleaseMutex(SyncUsersMutex);
		return;
	}
	//clear out the memory
	XML_ParserReset(parser, NULL);
	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, ReqStartElement, ReqEndElement);
	XML_SetCharacterDataHandler(parser, ReqCharacterData);
	ReleaseMutex(this->xmlImportThreadSafeMutex);
	ReleaseMutex(SyncUsersMutex);

	return;

}

void CTWXSync::HandleResponse(const char *data, int len)
{
}

//no mutex here
void CTWXSync::ReqStartElement(void *userData, const XML_Char *tag, const XML_Char **atts)
{
	char * escapedName = NULL;
	char * unescapedName = NULL;
	CTWXSync *pTWXSync = (CTWXSync*)userData;

	pTWXSync->m_tags[pTWXSync->GetDepth()]=(char *)strdup(tag);

	while (*atts) {

		if (strcmp (*atts,"nr")==0)
			pTWXSync->number = atol((*(atts+1)));
		else if (strcmp (*atts,"updated")==0) {
			pTWXSync->lastUpdated = atol((*(atts+1)));

			if (strcmp(tag,"Trader") ==0) {
				int n = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
				
				int gameIndex=-1, i;

				for (i=0; i<gameCount; i++) {
					if (stricmp (headerArray[i].sessionId, pTWXSync->loginId)==0) {
						gameIndex=i;
						break;
					}
				}
				if (n>=0  && strcmp (pTWXSync->traderName, "") != 0  && strcmp(playersArray[gameIndex][n].name, "")!=0) {
					playersArray[gameIndex][n].last_update = pTWXSync->lastUpdated;
				}
			}
		}
		else if (strcmp (*atts,"discovered")==0)
			pTWXSync->lastUpdated = atol((*(atts+1)));
		
		else if (strcmp(tag,"Login") ==0 && strcmp (*atts,"check")==0) {
			if (stricmp("true", (char *) (*(atts+1)))==0)
				pTWXSync->loginPassCheck=1;
		}
		else if (strcmp(tag,"Trader") ==0 && strcmp (*atts,"name")==0) {
			escapedName = strdup ((char *) (*(atts+1)));
			unescapedName = unescapeData(escapedName);
			strcpy (pTWXSync->traderName, unescapedName);
			pfree((void**) &unescapedName);

			int n = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
			if (n>=0 && strcmp (pTWXSync->traderName, "") != 0) {
				int gameIndex=-1, i;

				for (i=0; i<gameCount; i++) {
					if (stricmp (headerArray[i].sessionId, pTWXSync->loginId)==0) {
						gameIndex=i;
						break;
					}
				}
				strcpy (playersArray[gameIndex][n].name, pTWXSync->traderName);
			}
		}
		else if (strcmp (*atts,"ships")==0)
			strcpy (pTWXSync->ships, (char *) (*(atts+1)));
		else if (strcmp (*atts,"corp")==0)
			if (strcmp((*(atts+1)), "true")==0 || strcmp((*(atts+1)), "True")==0)
				pTWXSync->corp= 1;
			else
				pTWXSync->corp=0;
		else if (strcmp (*atts,"finished")==0)
			pTWXSync->finished= atol((*(atts+1)));
		else if (strcmp (*atts,"hops")==0)
			pTWXSync->hops= atol((*(atts+1)));
		else if (strcmp (*atts,"begin")==0)
			pTWXSync->begin= atol((*(atts+1)));
		else if (strcmp (*atts,"end")==0)
			pTWXSync->end= atol((*(atts+1)));
		else if (strcmp (*atts,"version")==0)
			pTWXSync->twxVersion= atol((*(atts+1)));
		else if (strcmp(tag,"Session") ==0 && strcmp (*atts,"id")==0)
			strcpy (pTWXSync->sessionId, (*(atts+1)));
		else if (strcmp(tag,"Login") ==0 && strcmp (*atts,"id")==0)
			strcpy (pTWXSync->loginId, (*(atts+1)));
		else if (strcmp(tag,"Login") ==0 && strcmp (*atts,"user")==0) {
			escapedName = strdup ((char *) (*(atts+1)));
			unescapedName = unescapeData(escapedName);
			strcpy (pTWXSync->username, unescapedName);
			pfree((void**) &unescapedName);
		}
		else if (strcmp(tag,"Login") ==0 && strcmp (*atts,"password")==0)
			strcpy(pTWXSync->password, (*(atts+1)));
		else if (strcmp(tag,"Login") ==0 && strcmp (*atts,"helper")==0)
			strcpy(pTWXSync->helper, (*(atts+1)));
		else if (strcmp(tag,"Game") ==0 && strcmp (*atts,"name")==0)
			strcpy(pTWXSync->gameName, (*(atts+1)));
		else if (strcmp(tag,"Request") ==0 && strcmp (*atts,"type")==0)
			strcpy(pTWXSync->requestType, (*(atts+1)));
		else if (strcmp(tag,"LastBust") ==0 && strcmp (*atts,"trader")==0) {
			escapedName = strdup ((char *) (*(atts+1)));
			unescapedName = unescapeData(escapedName);
			strcpy (pTWXSync->traderName, unescapedName);
			pfree((void**) &unescapedName);

			int n = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
			if (n>=0 && strcmp (pTWXSync->traderName, "") != 0) {
				int gameIndex=-1, i;

				for (i=0; i<gameCount; i++) {
					if (stricmp (headerArray[i].sessionId, pTWXSync->loginId)==0) {
						gameIndex=i;
						break;
					}
				}
				strcpy (playersArray[gameIndex][n].name, pTWXSync->traderName);
			}
		}
		

		atts+=2;
	}
/*
	if (pTWXSync->GetDepth()>0 && strcmp(pTWXSync->m_tags[0], "TWXSync") == 0 &&
		pTWXSync->GetDepth() >1 && strcmp(pTWXSync->m_tags[1], "Update") == 0) {
		sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"%s\"/> <Result code=\"200\" text=\"OK\"/></TWXSync>", TWXSYNX_VERSION, pTWXSync->sessionId);
	}
*/
	//increment depth of tree
	pTWXSync->SetDepth(pTWXSync->GetDepth()+1);

	pTWXSync->m_emptyTag=1;


}

//no mutex here
void CTWXSync::ReqEndElement(void *userData, const XML_Char *tag)
{
	CTWXSync *pTWXSync = (CTWXSync*)userData;


	//it is only 1 if the node has no data. so stay at the same level
	//dont pfree the memory and then character call will free it.
	if (pTWXSync->m_emptyTag==1) {
		pTWXSync->m_emptyTag=2;
	}

	else {
		if (pTWXSync->GetDepth()>0)
			pTWXSync->SetDepth(pTWXSync->GetDepth()-1);

		pfree((void **)&pTWXSync->m_tags[pTWXSync->GetDepth()]);
		pTWXSync->m_tags[pTWXSync->GetDepth()]=NULL;
	}

}

//no mutex here
void CTWXSync::ReqCharacterData(void *userData, const XML_Char *s, int len)
{
	CTWXSync *pTWXSync = (CTWXSync*)userData;
	int i=0;
	long tmpLong=0;
	int tmpInt=0, tmpInt2=0;
	int offset=1;
	char data[256];


	for (; len > 0; --len, ++s) {
		switch (*s) {
			//    case T('&'):
			//      fputs(T("&amp;"), fp);
			  break;
			case T('<'):
			//      fputs(T("&lt;"), fp);
			  break;
			case T('>'):
			//      fputs(T("&gt;"), fp);
			  break;
			#ifdef W3C14N
			case 13:
			#ifdef USE_FAKE_LF
			//      fputs(T("&#xD;"), fp);
			#else
			//		fputs(T("\n"), fp);
			#endif

			  break;
			#else
			//    case T('"'):
			//    fputs(T("&quot;"), fp);
			  break;
			case 9:
			case 10:
			case 13:
			#ifdef USE_FAKE_LF
			//      fprintf(fp, T("&#%d;"), *s);
			#else
			//		fputs(T("\n"), fp);
			#endif
			  break;
			#endif
			default:
			  data[i++] = *s;
			  break;
		}
	}
	data[i++]=0;

	int hasChars = hasCharacters(data);

 	if (stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-1], "TWXSync") == 0) {
		// ...
	}
	else if (stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-1], "Session") == 0) {
		// ...
	}

	else if (pTWXSync->GetDepth()>=2 && stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-2], "TWXSync") == 0 &&
			 stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-1], "Login") == 0) {

		int gameIndex=-1, i;

		for (i=0; i<gameCount; i++) {
			if (stricmp (headerArray[i].sessionId, pTWXSync->loginId)==0) {
				gameIndex=i;
				break;
			}
		}
		
		
		if (pTWXSync->twxVersion>TWXSYNX_VERSION) {
			sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"-1\"/><Result code=\"505\" text=\"Version Not Supported \"/></TWXSync>", TWXSYNX_VERSION);
		}
		else if (gameIndex<0) {		//couldnt find the game the user is interested in
			sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"-1\"/><Result code=\"404\" text=\"Not Found\"/></TWXSync>", TWXSYNX_VERSION);
		}
		else if (strcmp(pTWXSync->password, headerArray[gameIndex].password)==0) {	//authorize
			sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"%s\"/> <Result code=\"200\" text=\"OK\"/></TWXSync>", TWXSYNX_VERSION, pTWXSync->loginId);
			WaitForSingleObject(ListOfConnectedUsersMutex, INFINITE);
			mTWXSyncConnectedList.Add(pTWXSync);
			char *t = TWC_GetGameName(gameIndex);
			strcpy(pTWXSync->gameName, t);
			pfree((void **) &t);
			//fire validuser login connection callback
			if (pTWXSync->loginPassCheck==0)
				m_ServerPointer->FireValidUserLoginCallback(pTWXSync);
			pTWXSync->loginPassCheck=0;
			ReleaseMutex(ListOfConnectedUsersMutex);
		} else {	//deny
			sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"-1\"/> <Result code=\"401\" text=\"Unauthorized\"/></TWXSync>", TWXSYNX_VERSION);
		}

	}
	else if (pTWXSync->GetDepth()>=3 && stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-3], "Create") == 0 &&
			 stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-2], "Game") == 0 &&
			 stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-1], "Sectors") == 0) {
		if (pTWXSync->gameName != NULL) {
			char* returnedSessionId = createTWXFile(pTWXSync, pTWXSync->gameName, "test", atol(data)) ;
			if (returnedSessionId>0) {
				sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"%ld\"/> <Result code=\"201\" text=\"Created\"/></TWXSync>", TWXSYNX_VERSION, returnedSessionId);
			}
			else if (strcmp (returnedSessionId, "-403")==0) {
				sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Result code=\"403\" text=\"Forbidden\"/></TWXSync>", TWXSYNX_VERSION);
			}
			else {
				sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\">  <Result code=\"500\" text=\"Internal Server Error\"/></TWXSync>", TWXSYNX_VERSION);
			}
		}
	}
	else if (stricmp(pTWXSync->m_tags[pTWXSync->GetDepth()-1], "Request") == 0) {
		char dataBuffer[MAX_LENGTH];
		char tempBuffer[256];
		if (strcmp (pTWXSync->requestType, "GameIndex")==0) {
			sprintf(dataBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Result code=\"200\" text=\"OK\"/>", TWXSYNX_VERSION);
	
			//update the GUI that this is a GameIndex request				
			WaitForSingleObject(ListOfConnectedUsersMutex, INFINITE);
			strcpy (pTWXSync->username, "GameIndex Request");
			strcpy (pTWXSync->helper, "n/a");
			//fire validuser login connection callback
			m_ServerPointer->FireValidUserLoginCallback(pTWXSync);
			ReleaseMutex(ListOfConnectedUsersMutex);
			
			strcat (dataBuffer, "<GameIndex>");
			for (i=0; i<gameCount; i++) {
				sprintf (tempBuffer, "<Game id=\"%s\">", headerArray[i].sessionId);
				strcat (dataBuffer, tempBuffer);
				sprintf (tempBuffer, "<Name>%s</Name>", headerArray[i].file_name);
				strcat (dataBuffer, tempBuffer);
				sprintf (tempBuffer, "<Sectors>%ld</Sectors>", headerArray[i].sector_cnt);
				strcat (dataBuffer, tempBuffer);
				strcat (dataBuffer, "</Game>");
			}
			strcat (dataBuffer, "</GameIndex>");
			strcat (dataBuffer, "</TWXSync>");
			strcpy (pTWXSync->returnBuffer, dataBuffer);
		} else if (strcmp (pTWXSync->requestType, "TWXDataRequest")==0) {
			//check to see if they want the latest data
			//trigger to tell main loop to read data from file to send
			strcpy (pTWXSync->returnBuffer, TWXDATAFROMFILE);
		} else
			sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"-1\"/><Result code=\"404\" text=\"Not Found\"/></TWXSync>", TWXSYNX_VERSION);
	}
	else if ((pTWXSync->GetDepth()-1)>(0+offset-1) && strcmp(pTWXSync->m_tags[offset], "Update") == 0) {
		int Depth = pTWXSync->GetDepth()-1;

//		{
		int gameIndex=-1;

		for (i=0; i<gameCount; i++) {
			if (stricmp (headerArray[i].sessionId, pTWXSync->sessionId)==0) {
				gameIndex=i;
				break;
			}
		}


		if (gameIndex!= -1 && Depth>(0+offset-1) && strcmp (pTWXSync->m_tags[0+offset],"Update")==0 && hasChars) {
			if (Depth>(1+offset-1) && strcmp (pTWXSync->m_tags[1+offset],"Header")==0 && hasChars) {
				if (Depth>(2+offset-1) && strcmp (pTWXSync->m_tags[2+offset],"Created")==0 && hasChars) {
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Date")==0 && hasChars) {
						pTWXSync->twxCreatedOn = atol (data);
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"By")==0 && hasChars) {
						strcpy(pTWXSync->twxCreatedBy, data);
					}		
				}	
				if (Depth>(2+offset-1) && strcmp (pTWXSync->m_tags[2+offset],"Game")==0 && hasChars) {
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Name")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Host")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Port")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Version")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"MBBS")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Gold")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Sectors")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"StarDock")==0 && hasChars) {
						tmpLong = atol(data);
					if (tmpLong>=-1 && tmpLong < headerArray[gameIndex].sector_cnt)
						headerArray[gameIndex].stardock = tmpLong;
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Class0Port")==0 && hasChars) {
						tmpLong = atol(data);
						if (tmpLong>=-1 && tmpLong < headerArray[gameIndex].sector_cnt)
							headerArray[gameIndex].cls0port_alpha = tmpLong;
					}		
				}		
				
			}
//zzz
			if (Depth>(1+offset-1) && strcmp (pTWXSync->m_tags[1+offset],"TraderData")==0 && hasChars) {
				if (Depth>(2+offset-1) && strcmp (pTWXSync->m_tags[2+offset],"Trader")==0 && hasChars) {
					tmpInt = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Experience")==0 && hasChars) {
						playersArray[gameIndex][tmpInt].experience = atol(data);
						playersArray[gameIndex][tmpInt].last_update = pTWXSync->lastUpdated;
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Alignment")==0 && hasChars) {
						tmpLong = atol(data);
						if (tmpLong>=-1 && tmpLong<5000000)
							playersArray[gameIndex][tmpInt].alignment = atol(data);
						
						playersArray[gameIndex][tmpInt].last_update = pTWXSync->lastUpdated;
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Corporation")==0 && hasChars) {
						tmpInt2 = atoi(data);
						if (tmpInt2>=-1 && tmpInt2<20000)
							playersArray[gameIndex][tmpInt].corpNumber = tmpInt2;
						playersArray[gameIndex][tmpInt].last_update = pTWXSync->lastUpdated;
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Rank")==0 && hasChars) {
						playersArray[gameIndex][tmpInt].last_update = pTWXSync->lastUpdated;
					}		
				}	
			}
			if (Depth>(1+offset-1) && strcmp (pTWXSync->m_tags[1+offset],"CorporationData")==0 && hasChars) {
				if (Depth>(2+offset-1) && strcmp (pTWXSync->m_tags[2+offset],"Corporation")==0 && hasChars) {
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Name")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"CEO")==0 && hasChars) {
					}		
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Corporation")==0 && hasChars) {
					}		
				}	
			}
			if (Depth>(1+offset-1) && strcmp (pTWXSync->m_tags[1+offset],"ShipData")==0 && hasChars) {
				//not storing this
			}
			if (Depth>(1+offset-1) && strcmp (pTWXSync->m_tags[1+offset],"PlanetData")==0 && hasChars) {
				//not storing this
			}
//zzz
			int validSector = isValidSector (pTWXSync->number, headerArray[gameIndex].sector_cnt);
			if (Depth>(1+offset-1) && strcmp (pTWXSync->m_tags[1+offset],"SectorData")==0 && hasChars && validSector) {
				if (Depth>(2+offset-1) && strcmp (pTWXSync->m_tags[2+offset],"Sector")==0 && hasChars && validSector) {
					if (sectorsArray[gameIndex][pTWXSync->number-1].info==-1) {
						sectorsArray[gameIndex][pTWXSync->number-1].info=10;
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Space")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Beacon")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
				
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Warps")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
						//if there is no sector data added, then add it.
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Warp")==0 && hasChars && validSector) {
							tmpLong = atol(data);
							if (tmpLong>0 && tmpLong<headerArray[gameIndex].sector_cnt) {
								i=0;
								int foundWarp=0;
								//check if the warp exists
								for (i=0; foundWarp==0 && i<6; i++) {
									if 	(sectorsArray[gameIndex][pTWXSync->number-1].warp_sect[i] == tmpLong) {
										foundWarp=1;
									}
								}
								//means the warp was no there
								if (foundWarp==0) {
									int val = sectorsArray[gameIndex][pTWXSync->number-1].warps++;
									if (val<=5)
									{
										sectorsArray[gameIndex][pTWXSync->number-1].warp_sect[val]=tmpLong;
										sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
									}
									else {
										val=7;
									}
								}
							}
						}
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Port")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].port_update) {
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Name")==0 && hasChars && validSector) {
							
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Class")==0 && hasChars && validSector) {
							tmpInt = atoi(data);
							if (tmpInt>-10 && tmpInt <40)
								sectorsArray[gameIndex][pTWXSync->number-1].info=tmpInt;
							sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"UnderConstruction")==0 && hasChars && validSector) {
							tmpInt = atoi(data);
							if (tmpInt>-10 && tmpInt <40) {
								if (sectorsArray[gameIndex][pTWXSync->number-1].info<UNDER_CONSTRUCTION)
									sectorsArray[gameIndex][pTWXSync->number-1].info+=UNDER_CONSTRUCTION;
							}
							sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Destroyed")==0 && hasChars && validSector) {
							tmpInt = atoi(data);
							if (tmpInt>-10 && tmpInt <40) {
								sectorsArray[gameIndex][pTWXSync->number-1].info=12;
							sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
							}
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"FuelOre")==0 && hasChars && validSector) {
							if (Depth>(5+offset-1) && strcmp (pTWXSync->m_tags[5+offset],"Amount")==0 && hasChars && validSector) {
								tmpLong = atol(data);
								if (tmpLong>=-1 && tmpLong <= 1000000)
									sectorsArray[gameIndex][pTWXSync->number-1].port_amt[0] = tmpLong;
								sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
							}
							if (Depth>(5+offset-1) && strcmp (pTWXSync->m_tags[5+offset],"Percentage")==0 && hasChars && validSector) {
								tmpInt = atoi(data);
								if (tmpInt>=-1 && tmpInt <= 100)
									sectorsArray[gameIndex][pTWXSync->number-1].port_per[0] = tmpInt;
								sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
							}
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Organics")==0 && hasChars && validSector) {
							if (Depth>(5+offset-1) && strcmp (pTWXSync->m_tags[5+offset],"Amount")==0 && hasChars && validSector) {
								tmpLong = atol(data);
								if (tmpLong>=-1 && tmpLong <= 1000000)
									sectorsArray[gameIndex][pTWXSync->number-1].port_amt[1] = tmpLong;
								sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
							}
							if (Depth>(5+offset-1) && strcmp (pTWXSync->m_tags[5+offset],"Percentage")==0 && hasChars && validSector) {
								tmpInt = atoi(data);
								if (tmpInt>=-1 && tmpInt <= 100)
									sectorsArray[gameIndex][pTWXSync->number-1].port_per[1] = tmpInt;
								sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
							}
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Equipment")==0 && hasChars && validSector) {
							if (Depth>(5+offset-1) && strcmp (pTWXSync->m_tags[5+offset],"Amount")==0 && hasChars && validSector) {
								tmpLong = atol(data);
								if (tmpLong>=-1 && tmpLong <= 1000000)
									sectorsArray[gameIndex][pTWXSync->number-1].port_amt[2] = tmpLong;
								sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
							}
							if (Depth>(5+offset-1) && strcmp (pTWXSync->m_tags[5+offset],"Percentage")==0 && hasChars && validSector) {
								tmpInt = atoi(data);
								if (tmpInt>=-1 && tmpInt <= 100)
									sectorsArray[gameIndex][pTWXSync->number-1].port_per[2] = tmpInt;
								sectorsArray[gameIndex][pTWXSync->number-1].port_update = pTWXSync->lastUpdated;
							}
						}
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Fighters")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Amount")==0 && hasChars && validSector) {
							tmpLong = atol(data);
							if (tmpLong>=-1 && tmpLong <= 10000000000)
								sectorsArray[gameIndex][pTWXSync->number-1].ftrs = tmpLong;
							sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Type")==0 && hasChars && validSector) {
							if (strcmp (data, "Toll")==0)
								sectorsArray[gameIndex][pTWXSync->number-1].ftr_type = 1;
							else if (strcmp (data, "Offensive")==0)
								sectorsArray[gameIndex][pTWXSync->number-1].ftr_type = 2;
							else if (strcmp (data, "Defensive")==0)
								sectorsArray[gameIndex][pTWXSync->number-1].ftr_type = 3;
							else if (strcmp (data, "Mercenary")==0)
								sectorsArray[gameIndex][pTWXSync->number-1].ftr_type = 0;
							else 
								sectorsArray[gameIndex][pTWXSync->number-1].ftr_type = -1;
							sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Owner")==0 && hasChars && validSector) {
							tmpInt = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
							if (pTWXSync->corp==1)
								sectorsArray[gameIndex][pTWXSync->number-1].ftr_owner = atol(data)+CORP_CONSTANT;
							else
								sectorsArray[gameIndex][pTWXSync->number-1].ftr_owner = tmpInt;
							sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
						}
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"ArmidMines")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Amount")==0 && hasChars && validSector) {
							tmpInt = atoi(data);
							if (tmpInt>=-1 && tmpInt<= 10000)
								sectorsArray[gameIndex][pTWXSync->number-1].armids = tmpInt;
							sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Owner")==0 && hasChars && validSector) {
							tmpInt = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
							sectorsArray[gameIndex][pTWXSync->number-1].ftr_owner = tmpInt + (pTWXSync->corp * CORP_CONSTANT);
							sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
						}
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"LimpetMines")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Amount")==0 && hasChars && validSector) {
							tmpInt = atoi(data);
							if (tmpInt>=-1 && tmpInt<= 10000)
								sectorsArray[gameIndex][pTWXSync->number-1].limpets = tmpInt;
							sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
						}
						if (Depth>(4+offset-1) && strcmp (pTWXSync->m_tags[4+offset],"Owner")==0 && hasChars && validSector) {
							tmpInt = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
							sectorsArray[gameIndex][pTWXSync->number-1].ftr_owner = tmpInt + (pTWXSync->corp * CORP_CONSTANT);
							sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
						}
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Density")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
						tmpLong = atol(data);
						if (tmpLong>=-1 && tmpLong <= 10000000000)
							sectorsArray[gameIndex][pTWXSync->number-1].density = tmpLong;		
						sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
					}

					//last busted
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"LastBust")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
						tmpLong = atol(data);
						//get the trader number.
						tmpInt = getTradeNumber (pTWXSync->sessionId, pTWXSync->traderName);
						//if i was the last busted player, reset it else set to 0
						sectorsArray[gameIndex][pTWXSync->number-1].playerIdBusted=tmpInt;	
						sectorsArray[gameIndex][pTWXSync->number-1].lastBusted=tmpLong;	
						sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
					}

					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"Anomaly")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
				
					}
					if (Depth>(3+offset-1) && strcmp (pTWXSync->m_tags[3+offset],"NavHaz")==0 && hasChars && validSector && pTWXSync->lastUpdated >= sectorsArray[gameIndex][pTWXSync->number-1].sector_update) {
						tmpInt = atoi(data);
						if (tmpInt>=-1 && tmpInt<= 10000)
							sectorsArray[gameIndex][pTWXSync->number-1].navhaz = tmpInt;
						sectorsArray[gameIndex][pTWXSync->number-1].sector_update = pTWXSync->lastUpdated;
					}
				}
			}
		}

		sprintf(pTWXSync->returnBuffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"%s\"/> <Result code=\"200\" text=\"OK\"/></TWXSync>", TWXSYNX_VERSION, pTWXSync->sessionId);
	}

	//when there was no data. free the memory and decrement the count
	if (pTWXSync->m_emptyTag==2) {
		//decrement depth of tree
		if (pTWXSync->GetDepth()>0) {
			pTWXSync->SetDepth(pTWXSync->GetDepth()-1);

		}

		pfree((void **)&pTWXSync->m_tags[pTWXSync->GetDepth()]);
		pTWXSync->m_tags[pTWXSync->GetDepth()]=NULL;
	}

	//found data
	pTWXSync->m_emptyTag=0;


}

//no mutex
void CTWXSync::RespStartElement(void *userData, const char *tag, const char **atts)
{
	CTWXSync *pTWXSync = (CTWXSync*)userData;

	if (stricmp(tag, "TWXSync") == 0) {
		// ...
	}
	else if (stricmp(tag, "Session") == 0) {
		// ...
	}
	else if (stricmp(tag, "Result") == 0) {
		// ...
	}
}

//no mutex
void CTWXSync::RespEndElement(void *userData, const char *tag)
{
	CTWXSync *pTWXSync = (CTWXSync*)userData;

	// ...
}

//no mutex
void CTWXSync::RespCharacterData(void *userData, const char *tag, const char *data)
{
	CTWXSync *pTWXSync = (CTWXSync*)userData;

	// ...
}

//////////////////////////////////////////////////////////////////////
//no mutex
unsigned __stdcall ListenServerThreadProc(void *arg) {
	CTWXSync *pTWXSync = (CTWXSync *)arg;
	WSADATA wsaData;
    struct sockaddr_in our_addr;
	SOCKET sock =NULL;
	char buffer[MAX_BUF_LEN];

	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) {
		sprintf(buffer, "Winsock startup error: %d",WSAGetLastError());
		pTWXSync->FireErrorCallback(0, buffer, strlen(buffer));
		return -1;
	}

	// create the socket 
	sock = socket(AF_INET,SOCK_STREAM,0);
	pTWXSync->SetServerSocket(sock);

	  /* Make listening socket's port reusable - must appear before bind */
	  int option_value = 1;
	  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option_value,
					 sizeof(option_value)) < 0) {
		sprintf(buffer, "Winsock setsockopt error: %d",WSAGetLastError());
		pTWXSync->FireErrorCallback(0, buffer, strlen(buffer));
		WSACleanup();
		return -1;
	  }


	// set up our address
	our_addr.sin_family = AF_INET;
	our_addr.sin_addr.s_addr = ADDR_ANY;
	our_addr.sin_port = htons(pTWXSync->GetServerPortNumber());
	// bind to it
	if (bind(sock, (sockaddr*)&our_addr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		sprintf(buffer, "Winsock binding error: %d",WSAGetLastError());
		pTWXSync->FireErrorCallback(0, buffer, strlen(buffer));
		WSACleanup();
		return -1;
	}

	// set it to a passive mode
	if (listen(sock,MAX_CONNECTIONS) == SOCKET_ERROR) {     
		sprintf(buffer, "Winsock listening error: %d",WSAGetLastError());
		pTWXSync->FireErrorCallback(0, buffer, strlen(buffer));
		WSACleanup();
		return -1;
	}

	m_ServerPointer = pTWXSync;
	while (true) {	//need a stopping clause
		SOCKET c_sock;
		struct sockaddr_in client;
		int client_len = sizeof(sockaddr_in); 
		// Accept the connection check for socket error here????
		c_sock = accept(sock, (sockaddr*)&client, &client_len);

		if (c_sock==INVALID_SOCKET)
			break;
		// New connection
		CTWXSync *pNewTWXSync = new CTWXSync();
		pNewTWXSync->Init(pTWXSync->m_pIncomingDataCallback,
						  pTWXSync->m_pOutgoingDataCallback,
						  pTWXSync->m_pErrorCallback);
		pNewTWXSync->SetClientHost(inet_ntoa(client.sin_addr));
 		pNewTWXSync->SetClientPortNumber(client.sin_port);
		pNewTWXSync->SetClientSocket(c_sock);

		pTWXSync->FireNewConnectionCallback(pNewTWXSync);
		pTWXSync->timeConnected = Get_Time();

		// Start a new thread to deal with the new connection
		_beginthread(IncomingDataThreadProc, 0, (void *)pNewTWXSync);
	}

	closesocket(sock);
	pTWXSync->SetServerSocket(NULL);
	WSACleanup();

	return 0;
}

void IncomingDataThreadProc(void *arg) {
	CTWXSync *pTWXSync = (CTWXSync *)arg;
	SOCKET sock = (SOCKET)pTWXSync->GetClientSocket();
	char * incomingData=NULL;
	int i=0;
	char buffer[MAX_LENGTH];


	while (true) {
		char *dataFromClient=NULL;

		int r = pTWXSync->ReceiveAllDataWrapper(&dataFromClient);
		if (r == 0 || r == SOCKET_ERROR || r == WSAECONNRESET) {
			pfree((void **)&dataFromClient);
			break;
		}

		if (dataFromClient==NULL)
			break;

		if (pTWXSync->GetSkipParse()==0)
			pTWXSync->FireIncomingDataCallback(dataFromClient, strlen(dataFromClient));
		else
			pTWXSync->SetSkipParse(0);

		WaitForSingleObject(ListOfConnectedUsersMutex, INFINITE);
		for (i=0; i<mTWXSyncConnectedList.GetCount(); i++) {
			char *ackDataFromClient=NULL;
			CTWXSync *tempTWXSync = (CTWXSync*) mTWXSyncConnectedList.GetAt(i);
			//do not send it to yourself
			if (pTWXSync != NULL && tempTWXSync != NULL && pTWXSync->compareObject(tempTWXSync)) {
				tempTWXSync->Send(dataFromClient, strlen(dataFromClient));
//				int q = tempTWXSync->ReceiveAllDataWrapper(&ackDataFromClient);
//				pfree((void **)&ackDataFromClient);
			}
		}
		ReleaseMutex(ListOfConnectedUsersMutex);
		
		//see if we are supposed to send ALL the data by first dumping to file and then
		//sending it.
		if (strcmp (pTWXSync->returnBuffer, TWXDATAFROMFILE)==0) {
			char *dataToSendToClient=NULL, tmp[256];
			char *filename=NULL, *fileData=NULL;
			int result=0;
			srand( (unsigned)time( NULL ) );

			//send all the stuff since the last connection.
			//instead of mallocing a huge amount of memory, we stream it to a file.
			//and then send the file over the network stream
			sprintf (buffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			dataToSendToClient = appendData(dataToSendToClient, buffer);
			sprintf (buffer, "<TWXSync version=\"%ld\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" >\n", TWXSYNX_VERSION);
			dataToSendToClient = appendData(dataToSendToClient, buffer);

			sprintf (buffer, "<Session id=\"%s\"/>\n", pTWXSync->sessionId);
			dataToSendToClient = appendData(dataToSendToClient, buffer);
			sprintf (buffer, "<Result code=\"200\" text=\"OK\"/>");
			dataToSendToClient = appendData(dataToSendToClient, buffer);
			
			dataToSendToClient = appendData(dataToSendToClient, "<Update>\n");
			/* twx xml v2 here */
			sprintf(tmp, "tmp_twxSync%d.xml", rand());

			result = export_twxv2(tmp, pTWXSync->begin, (pTWXSync->end !=0 ?pTWXSync->end: Get_Time()), 1, pTWXSync->sessionId);
			
			filename =  strdup (tmp);
			fileData = getFile(filename);
			pfree((void**)&filename);
			unlink(tmp);	//delete file

			dataToSendToClient = appendData(dataToSendToClient, fileData);		
			pfree((void**)&fileData);
			dataToSendToClient = appendData(dataToSendToClient, "\n</Update>\n");
			dataToSendToClient = appendData(dataToSendToClient, "</TWXSync>\n");

			pTWXSync->begin = 0;
			pTWXSync->end = Get_Time();
			//update the last time we sync'ed with the server
			//send updates
			result = pTWXSync->Send(dataToSendToClient, strlen(dataToSendToClient));
			
		} else {
			pTWXSync->Send (pTWXSync->returnBuffer, strlen(pTWXSync->returnBuffer));
		}

		strcpy (pTWXSync->returnBuffer, "");

		//increment packet count
		pTWXSync->packetCountSent++;
		
		//save data every 4th time, every 2 minutes
		if (pTWXSync->packetCountSent%100==0)
			saveTWXFile(pTWXSync, pTWXSync->sessionId);
		pTWXSync->FireOutgoingDataCallback(dataFromClient, strlen(dataFromClient));
		pfree((void **)&dataFromClient);

	}
	
	WaitForSingleObject(ListOfConnectedUsersMutex, INFINITE);
	mTWXSyncConnectedList.Remove(pTWXSync);
	ReleaseMutex(ListOfConnectedUsersMutex);

	closesocket(sock);
	
	//close when client disconnects
	saveTWXFile(pTWXSync, pTWXSync->sessionId);
	pTWXSync->SetClientSocket(NULL);
	pTWXSync->FireIncomingDataCallback("", 0);

	//clean up object allocated after connection
}


//make sure to check for null on the pTWXSync object
char* createTWXFile(CTWXSync *pTWXSync, char *name, char *password, long sectorCount) {
	char *sessionId=NULL;
	int i=0;
	FILE *fp;
	char fullFilePath[1024];

	WaitForSingleObject(SyncUsersMutex, INFINITE);

	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, name);
	strcat (fullFilePath, gameExtension);


	//dont overwrite games!!
	fp = fopen(fullFilePath, "rb");
	if (fp != NULL) {
		ReleaseMutex(SyncUsersMutex);
		return "-403";
	}

	sessionId = TWC_CreateGUID();
	//set initial values for header
	strncpy(headerArray[gameCount].id, "ATTC", 4);
	headerArray[gameCount].ver = 2;
	headerArray[gameCount].sector_cnt=sectorCount;
	headerArray[gameCount].player_cnt=MAX_PLAYERS;
	headerArray[gameCount].planet_cnt=MAX_PLANETS;
	strcpy (headerArray[gameCount].sessionId, sessionId);
	headerArray[gameCount].lastUpdateTime=0;
	headerArray[gameCount].stardock=-1;
	headerArray[gameCount].cls0port_alpha=-1;
	headerArray[gameCount].cls0port_rylos=-1;
	headerArray[gameCount].cls0port_sol=-1;
	strcpy( headerArray[gameCount].password, password);
	strcpy( headerArray[gameCount].file_name, name);
	strcat( headerArray[gameCount].file_name, gameExtension);
	memset(&headerArray[gameCount].reserved, 0, sizeof(headerArray[gameCount].reserved));

	traderHashArray[gameCount] = new CHash();

	pfree((void **)&sessionId);
	// get memory for session info and clear it
	sectorsArray[gameCount] = (TWX_SECTOR *) malloc(sectorCount * sizeof(TWX_SECTOR));
	for (i = 0; i < headerArray[gameCount].sector_cnt; i++)
	{

		sectorsArray[gameCount][i].info=-1;
		sectorsArray[gameCount][i].navhaz= -1;
		sectorsArray[gameCount][i].sector_update=0;
		sectorsArray[gameCount][i].ftrs=-1;
		sectorsArray[gameCount][i].ftr_owner=-1;
		sectorsArray[gameCount][i].ftr_type=-1;
		sectorsArray[gameCount][i].anom=-1;
		sectorsArray[gameCount][i].armids=-1;
		sectorsArray[gameCount][i].armid_owner=-1;
		sectorsArray[gameCount][i].limpets=-1;
		sectorsArray[gameCount][i].limpet_owner=-1;
		sectorsArray[gameCount][i].port_amt[0]=-1;
		sectorsArray[gameCount][i].port_amt[1]=-1;
		sectorsArray[gameCount][i].port_amt[2]=-1;
		sectorsArray[gameCount][i].port_per[0]=-1;
		sectorsArray[gameCount][i].port_per[1]=-1;
		sectorsArray[gameCount][i].port_per[2]=-1;
		sectorsArray[gameCount][i].warps=0;
		sectorsArray[gameCount][i].reserved2=0;
		sectorsArray[gameCount][i].warp_sect[0]=0;
		sectorsArray[gameCount][i].warp_sect[1]=0;
		sectorsArray[gameCount][i].warp_sect[2]=0;
		sectorsArray[gameCount][i].warp_sect[3]=0;
		sectorsArray[gameCount][i].warp_sect[4]=0;
		sectorsArray[gameCount][i].warp_sect[5]=0;
		sectorsArray[gameCount][i].port_update=0;
		sectorsArray[gameCount][i].density=-1;
		sectorsArray[gameCount][i].lastBusted=-1;
		sectorsArray[gameCount][i].playerIdBusted=-1;
		sectorsArray[gameCount][i].avoided=0;
		memset(&sectorsArray[gameCount][i].reserved, 0, sizeof(sectorsArray[gameCount][i].reserved));

	}

	playersArray[gameCount] = (TWX_PLAYER *) malloc(headerArray[gameCount].player_cnt * sizeof(TWX_PLAYER));
	for (i = 0; i < headerArray[gameCount].player_cnt; i++)
	{
		strcpy(playersArray[gameCount][i].id, "PLAY");
		playersArray[gameCount][i].autonum = -1;
		playersArray[gameCount][i].corpNumber = -1;
		playersArray[gameCount][i].alliance = '1';	//set everyone to an enemy
		playersArray[gameCount][i].reserved2 = 0;	
		playersArray[gameCount][i].experience = -1;
		playersArray[gameCount][i].alignment = -1;
		playersArray[gameCount][i].lastKnownSector = -1;
		playersArray[gameCount][i].last_update = -1;
		memset(&playersArray[gameCount][i].shipName, 0, sizeof(playersArray[gameCount][i].shipName));
		memset(&playersArray[gameCount][i].name, 0, sizeof(playersArray[gameCount][i].name));
		memset(&playersArray[gameCount][i].reserved, 0, sizeof(playersArray[gameCount][i].reserved));
	}

	planetsArray[gameCount] = (TWX_PLANET *) malloc(headerArray[gameCount].planet_cnt * sizeof(TWX_PLANET));
	for (i = 0; i < headerArray[gameCount].planet_cnt; i++)
	{
		strcpy(planetsArray[gameCount][i].id, "PLAN");
		planetsArray[gameCount][i].autonum = -1;
		planetsArray[gameCount][i].planetNumber = -1;
		planetsArray[gameCount][i].sector = -1;
		planetsArray[gameCount][i].createdBy = -1;
		planetsArray[gameCount][i].ownedBy = -1;
		planetsArray[gameCount][i].citadel = -1;
		planetsArray[gameCount][i].citclass = 0;
		planetsArray[gameCount][i].mReaction = -1;
		planetsArray[gameCount][i].iGenerator = -1;
		planetsArray[gameCount][i].transwarpPower = -1;
		planetsArray[gameCount][i].transportPower = -1;
		planetsArray[gameCount][i].qCannonPower = -1;
		planetsArray[gameCount][i].qCannonAtmos = -1;
		planetsArray[gameCount][i].qCannonSector = -1;
		planetsArray[gameCount][i].reserved2 = 0;
		planetsArray[gameCount][i].fighters = -1;
		planetsArray[gameCount][i].shields = -1;
		planetsArray[gameCount][i].treasury = -1;
		planetsArray[gameCount][i].figsPerDay = -1;
		planetsArray[gameCount][i].totalColonists = -1;
		planetsArray[gameCount][i].last_update = -1;    
		memset(&planetsArray[gameCount][i].name, 0, sizeof(planetsArray[gameCount][i].name));
		memset(&planetsArray[gameCount][i].reserved, 0, sizeof(planetsArray[gameCount][i].reserved));
	}

	gameCount++;
	ReleaseMutex(SyncUsersMutex);

	if (saveTWXFile (pTWXSync, headerArray[gameCount-1].sessionId)==0)
		return headerArray[gameCount-1].sessionId;
	else
		return "-1";
}

//make sure to check for null on the pTWXSync object
int saveTWXFile(CTWXSync *pTWXSync, char* sessionId) {
	TWX_FILE_HEADER tmp_header;
	TWX_SECTOR tmp_sectors;
	TWX_PLAYER tmp_players;
	TWX_PLANET tmp_planets;
	int gameIndex=-1, i=0, k=0, j=0;
	FILE *fp;
	char fullFilePath[1024];
	size_t write;
	
	for (i=0; i<gameCount; i++) {
		if (stricmp (headerArray[i].sessionId, sessionId)==0) {
			gameIndex=i;
			break;
		}
	}
	if (gameIndex<0) {
		return -1;
	}

	WaitForSingleObject(SyncUsersMutex, INFINITE);
	memset (&tmp_header, 0, sizeof (TWX_FILE_HEADER));
	memset (&tmp_sectors, 0, sizeof (TWX_SECTOR));

	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, headerArray[gameIndex].file_name);


	fp = fopen(fullFilePath, "wb");
	if (fp == NULL) {
		ReleaseMutex(SyncUsersMutex);
		return 1;
	} else {
      
		strcpy (tmp_header.id, headerArray[gameIndex].id);
		tmp_header.time_created = htonl(headerArray[gameIndex].lastUpdateTime);
		tmp_header.ver = htonl(headerArray[gameIndex].ver);
		tmp_header.sector_cnt = htonl(headerArray[gameIndex].sector_cnt);
		tmp_header.player_cnt = htonl(headerArray[gameIndex].player_cnt);
		tmp_header.planet_cnt = htonl(headerArray[gameIndex].planet_cnt);
		strcpy (tmp_header.sessionId, headerArray[gameIndex].sessionId);
		tmp_header.stardock = htonl(headerArray[gameIndex].stardock);
		tmp_header.cls0port_alpha = htonl(headerArray[gameIndex].cls0port_alpha);
		tmp_header.cls0port_rylos = htonl(headerArray[gameIndex].cls0port_rylos);
		tmp_header.cls0port_sol = htonl(headerArray[gameIndex].cls0port_sol);
		strcpy( tmp_header.password, headerArray[gameIndex].password);
		strcpy( tmp_header.file_name, headerArray[gameIndex].file_name);
		strcpy(tmp_header.reserved, headerArray[gameIndex].reserved);

		// write twx header
		fseek(fp, 0, SEEK_SET);
		write = fwrite(&tmp_header, sizeof(TWX_FILE_HEADER), READ_WRITE_AMOUNT, fp);
  
		if (write < READ_WRITE_AMOUNT)
		{
//			restoreDBFromBackup();
			fclose(fp);

			ReleaseMutex(SyncUsersMutex);
			return 1;
		}

		// write sector info
		for (i = 0; i < headerArray[gameIndex].sector_cnt; i++)
		{
			tmp_sectors.info = sectorsArray[gameIndex][i].info;
			tmp_sectors.navhaz = sectorsArray[gameIndex][i].navhaz;
			tmp_sectors.reserved2 = htons(sectorsArray[gameIndex][i].reserved2);
			tmp_sectors.ftr_type = sectorsArray[gameIndex][i].ftr_type;
			tmp_sectors.anom = sectorsArray[gameIndex][i].anom;
			tmp_sectors.port_per[0] = sectorsArray[gameIndex][i].port_per[0];
			tmp_sectors.port_per[1] = sectorsArray[gameIndex][i].port_per[1];
			tmp_sectors.port_per[2] = sectorsArray[gameIndex][i].port_per[2];
			tmp_sectors.warps = sectorsArray[gameIndex][i].warps;

			tmp_sectors.sector_update = htonl(sectorsArray[gameIndex][i].sector_update);
			tmp_sectors.ftrs = htonl(sectorsArray[gameIndex][i].ftrs);
			tmp_sectors.port_amt[0] = htonl(sectorsArray[gameIndex][i].port_amt[0]);
			tmp_sectors.port_amt[1] = htonl(sectorsArray[gameIndex][i].port_amt[1]);
			tmp_sectors.port_amt[2] = htonl(sectorsArray[gameIndex][i].port_amt[2]);
			tmp_sectors.warp_sect[0] = htonl(sectorsArray[gameIndex][i].warp_sect[0]);
			tmp_sectors.warp_sect[1] = htonl(sectorsArray[gameIndex][i].warp_sect[1]);
			tmp_sectors.warp_sect[2] = htonl(sectorsArray[gameIndex][i].warp_sect[2]);
			tmp_sectors.warp_sect[3] = htonl(sectorsArray[gameIndex][i].warp_sect[3]);
			tmp_sectors.warp_sect[4] = htonl(sectorsArray[gameIndex][i].warp_sect[4]);
			tmp_sectors.warp_sect[5] = htonl(sectorsArray[gameIndex][i].warp_sect[5]);
			tmp_sectors.port_update = htonl(sectorsArray[gameIndex][i].port_update);
			tmp_sectors.density = htonl(sectorsArray[gameIndex][i].density);
			tmp_sectors.lastBusted = htonl(sectorsArray[gameIndex][i].lastBusted);
			tmp_sectors.playerIdBusted = htonl(sectorsArray[gameIndex][i].playerIdBusted);

			tmp_sectors.ftr_owner = htons(sectorsArray[gameIndex][i].ftr_owner);
			tmp_sectors.armids = htons(sectorsArray[gameIndex][i].armids);
			tmp_sectors.armid_owner = htons(sectorsArray[gameIndex][i].armid_owner);
			tmp_sectors.limpets = htons(sectorsArray[gameIndex][i].limpets);
			tmp_sectors.limpet_owner = htons(sectorsArray[gameIndex][i].limpet_owner);
			tmp_sectors.avoided = sectorsArray[gameIndex][i].avoided;
			strcpy(tmp_sectors.reserved, sectorsArray[gameIndex][i].reserved);

			write = fwrite(&tmp_sectors, sizeof(TWX_SECTOR), READ_WRITE_AMOUNT, fp);
			if (write < READ_WRITE_AMOUNT)
			{
//				restoreDBFromBackup();

				fclose(fp);
				ReleaseMutex(SyncUsersMutex);
				return 1;
			}
		}

		// write player info
		for (i = 0; i < headerArray[gameIndex].player_cnt; i++)
		{
			tmp_players.autonum = htons(playersArray[gameIndex][i].autonum);	  
			tmp_players.corpNumber = htons(playersArray[gameIndex][i].corpNumber);
			tmp_players.alliance = playersArray[gameIndex][i].alliance;
			tmp_players.reserved2 = playersArray[gameIndex][i].reserved2;
			tmp_players.experience = htonl(playersArray[gameIndex][i].experience);
			tmp_players.alignment = htonl(playersArray[gameIndex][i].alignment);
			tmp_players.lastKnownSector = htonl(playersArray[gameIndex][i].lastKnownSector);
			tmp_players.last_update = htonl(playersArray[gameIndex][i].last_update);
			strcpy(tmp_players.name, playersArray[gameIndex][i].name);
			strcpy(tmp_players.reserved, playersArray[gameIndex][i].reserved);

			write = fwrite(&tmp_players, sizeof(TWX_PLAYER), READ_WRITE_AMOUNT, fp);
			if (write < READ_WRITE_AMOUNT)
			{
			  fclose(fp);
			  ReleaseMutex(SyncUsersMutex);
			  return -500;
			}
		}

		// write planet info
		for (i = 0; i < headerArray[gameIndex].planet_cnt; i++)
		{
			tmp_planets.autonum = htons(planetsArray[gameIndex][i].autonum);
			tmp_planets.planetNumber = htons(planetsArray[gameIndex][i].planetNumber);
			tmp_planets.createdBy = htons(planetsArray[gameIndex][i].createdBy);
			tmp_planets.ownedBy = htons(planetsArray[gameIndex][i].ownedBy);
			tmp_planets.citadel = planetsArray[gameIndex][i].citadel;
			tmp_planets.citclass = planetsArray[gameIndex][i].citclass;
			tmp_planets.mReaction = planetsArray[gameIndex][i].mReaction;
			tmp_planets.iGenerator = planetsArray[gameIndex][i].iGenerator;
			tmp_planets.transwarpPower = htons(planetsArray[gameIndex][i].transwarpPower);
			tmp_planets.transportPower = htons(planetsArray[gameIndex][i].transportPower);
			tmp_planets.qCannonPower = planetsArray[gameIndex][i].qCannonPower;
			tmp_planets.qCannonAtmos = planetsArray[gameIndex][i].qCannonAtmos;
			tmp_planets.qCannonSector = planetsArray[gameIndex][i].qCannonSector;
			tmp_planets.reserved2 = planetsArray[gameIndex][i].reserved2;
			tmp_planets.sector = htonl(planetsArray[gameIndex][i].sector);
			tmp_planets.fighters = htonl(planetsArray[gameIndex][i].fighters);
			tmp_planets.shields = htonl(planetsArray[gameIndex][i].shields);
			tmp_planets.treasury = htonl(planetsArray[gameIndex][i].treasury);
			tmp_planets.figsPerDay = htonl(planetsArray[gameIndex][i].figsPerDay);
			tmp_planets.totalColonists = htonl(planetsArray[gameIndex][i].totalColonists);
			tmp_planets.last_update = htonl(planetsArray[gameIndex][i].last_update);
			strcpy(tmp_planets.name, planetsArray[gameIndex][i].name);
			strcpy(tmp_planets.reserved, planetsArray[gameIndex][i].reserved);

  
			write = fwrite(&tmp_planets, sizeof(TWX_PLANET), READ_WRITE_AMOUNT, fp);
			if (write < READ_WRITE_AMOUNT)
			{
				fclose(fp);
				ReleaseMutex(SyncUsersMutex);
				return -500;
			}
		}
	}

	fclose (fp);

	ReleaseMutex(SyncUsersMutex);
	return 0;
}


int openAllTWXFiles() {
	int i=0;
	FILE *fp;
	char fullFilePath[1024], buffer[100];
	size_t read;
	HANDLE hFindFileHandle=NULL;
	HANDLE hFindInvalidValue = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA findData;
	int foundFile=1;

	TWX_FILE_HEADER tmp_header;
	TWX_SECTOR tmp_sectors;
	TWX_PLAYER tmp_players;
	TWX_PLANET tmp_planets;

	srand( (unsigned)time( NULL ) );

	WaitForSingleObject(SyncUsersMutex, INFINITE);
	
	sprintf (fullFilePath, "%s*%s", appDir, gameExtension);
	hFindFileHandle = FindFirstFile(fullFilePath, &findData);

	while (foundFile && hFindFileHandle != hFindInvalidValue) {
		strcpy (fullFilePath, appDir);
		strcat (fullFilePath, findData.cFileName);

		//dont overwrite games!!
		fp = fopen(fullFilePath, "rb");
		if (fp == NULL) {
			ReleaseMutex(SyncUsersMutex);
			return -404;
		}

		read = fread(&tmp_header, sizeof(TWX_FILE_HEADER), READ_WRITE_AMOUNT, fp);
		if (read < READ_WRITE_AMOUNT)
		{
			fclose(fp);
			ReleaseMutex(SyncUsersMutex);
			return -500;
		}

		//set initial values for header
		strncpy(headerArray[gameCount].id, tmp_header.id, 4);
		headerArray[gameCount].ver = ntohl(tmp_header.ver);
		headerArray[gameCount].sector_cnt=ntohl(tmp_header.sector_cnt);
		headerArray[gameCount].player_cnt=ntohl(tmp_header.player_cnt);
		headerArray[gameCount].planet_cnt=ntohl(tmp_header.planet_cnt);
		strcpy (headerArray[gameCount].sessionId, tmp_header.sessionId);
		headerArray[gameCount].stardock=ntohl(tmp_header.stardock);
		headerArray[gameCount].cls0port_alpha=ntohl(tmp_header.cls0port_alpha);
		headerArray[gameCount].cls0port_rylos=ntohl(tmp_header.cls0port_rylos);
		headerArray[gameCount].cls0port_sol=ntohl(tmp_header.cls0port_sol);
		headerArray[gameCount].lastUpdateTime=ntohl(tmp_header.lastUpdateTime);
		strcpy( headerArray[gameCount].password, tmp_header.password);
		strcpy( headerArray[gameCount].file_name, tmp_header.file_name);

		playersArray[gameCount] = (TWX_PLAYER *) malloc(MAX_PLANETS * sizeof(TWX_PLAYER));
		planetsArray[gameCount] = (TWX_PLANET *) malloc(MAX_PLAYERS * sizeof(TWX_PLANET));

		traderHashArray[gameCount] = new CHash();

		// get memory for session info and clear it
		sectorsArray[gameCount] = (TWX_SECTOR *) malloc(headerArray[gameCount].sector_cnt * sizeof(TWX_SECTOR));
		for (i = 0; i < headerArray[gameCount].sector_cnt; i++)
		{

			read = fread(&tmp_sectors, sizeof(TWX_SECTOR), READ_WRITE_AMOUNT, fp);

			if (read < READ_WRITE_AMOUNT)
			{
				fclose(fp);
				ReleaseMutex(SyncUsersMutex);
				return -500;
			}

			sectorsArray[gameCount][i].info=tmp_sectors.info;
			sectorsArray[gameCount][i].navhaz= tmp_sectors.navhaz;
			sectorsArray[gameCount][i].sector_update=ntohl(tmp_sectors.sector_update);
			sectorsArray[gameCount][i].ftrs=ntohl(tmp_sectors.ftrs);
			sectorsArray[gameCount][i].ftr_owner=ntohs(tmp_sectors.ftr_owner);
			sectorsArray[gameCount][i].ftr_type=tmp_sectors.ftr_type;
			sectorsArray[gameCount][i].anom=tmp_sectors.anom;
			sectorsArray[gameCount][i].armids=ntohs(tmp_sectors.armids);
			sectorsArray[gameCount][i].armid_owner=ntohs(tmp_sectors.armid_owner);
			sectorsArray[gameCount][i].limpets=ntohs(tmp_sectors.limpets);
			sectorsArray[gameCount][i].limpet_owner=ntohs(tmp_sectors.limpet_owner);
			sectorsArray[gameCount][i].port_amt[0]=ntohl(tmp_sectors.port_amt[0]);
			sectorsArray[gameCount][i].port_amt[1]=ntohl(tmp_sectors.port_amt[1]);
			sectorsArray[gameCount][i].port_amt[2]=ntohl(tmp_sectors.port_amt[2]);
			sectorsArray[gameCount][i].port_per[0]=tmp_sectors.port_per[0];
			sectorsArray[gameCount][i].port_per[1]=tmp_sectors.port_per[1];
			sectorsArray[gameCount][i].port_per[2]=tmp_sectors.port_per[2];
			sectorsArray[gameCount][i].warps=tmp_sectors.warps;
			sectorsArray[gameCount][i].reserved2=ntohs(tmp_sectors.reserved2);
			sectorsArray[gameCount][i].warp_sect[0]=ntohl(tmp_sectors.warp_sect[0]);
			sectorsArray[gameCount][i].warp_sect[1]=ntohl(tmp_sectors.warp_sect[1]);
			sectorsArray[gameCount][i].warp_sect[2]=ntohl(tmp_sectors.warp_sect[2]);
			sectorsArray[gameCount][i].warp_sect[3]=ntohl(tmp_sectors.warp_sect[3]);
			sectorsArray[gameCount][i].warp_sect[4]=ntohl(tmp_sectors.warp_sect[4]);
			sectorsArray[gameCount][i].warp_sect[5]=ntohl(tmp_sectors.warp_sect[5]);
			sectorsArray[gameCount][i].port_update=ntohl(tmp_sectors.port_update);
			sectorsArray[gameCount][i].density=ntohl(tmp_sectors.density);
			sectorsArray[gameCount][i].lastBusted=ntohl(tmp_sectors.lastBusted);
			sectorsArray[gameCount][i].playerIdBusted=ntohl(tmp_sectors.playerIdBusted);
			sectorsArray[gameCount][i].avoided=tmp_sectors.avoided;
			strcpy(sectorsArray[gameCount][i].reserved, tmp_sectors.reserved);
		}

		//save player data
		// read player info
		playersArray[gameCount] = (TWX_PLAYER *) malloc(headerArray[gameCount].player_cnt * sizeof(TWX_PLAYER));
		for (i = 0; i < headerArray[gameCount].player_cnt; i++)
		{
			read = fread(&tmp_players, sizeof(TWX_PLAYER), READ_WRITE_AMOUNT, fp);
			if (read < READ_WRITE_AMOUNT)
			{
				fclose(fp);
				ReleaseMutex(SyncUsersMutex);
				return -500;
			}
			playersArray[gameCount][i].experience = ntohl(tmp_players.experience);
			playersArray[gameCount][i].alignment = ntohl (tmp_players.alignment);
			playersArray[gameCount][i].lastKnownSector = ntohl(tmp_players.lastKnownSector);
			playersArray[gameCount][i].last_update = ntohl(tmp_players.last_update);
			playersArray[gameCount][i].reserved2 = tmp_players.reserved2;
			playersArray[gameCount][i].corpNumber = ntohs(tmp_players.corpNumber);
			playersArray[gameCount][i].alliance = tmp_players.alliance;
			strcpy(playersArray[gameCount][i].name, tmp_players.name);
			playersArray[gameCount][i].autonum = ntohs(tmp_players.autonum);
			strcpy(playersArray[gameCount][i].reserved, tmp_players.reserved);

			//add planets to the hash
			if (playersArray[gameCount][i].autonum>0) {
				sprintf (buffer, "%d", playersArray[gameCount][i].autonum);
				traderHashArray[gameCount]->Add(playersArray[gameCount][i].name, buffer);
			}
		}

		//planetListArray[gameCount] = 
		// read planet info
		planetsArray[gameCount] = (TWX_PLANET *) malloc(headerArray[gameCount].planet_cnt * sizeof(TWX_PLANET));
		for (i = 0; i < headerArray[gameCount].planet_cnt; i++)
		{
			read = fread(&tmp_planets, sizeof(TWX_PLANET), READ_WRITE_AMOUNT, fp);
			if (read < READ_WRITE_AMOUNT)
			{
				fclose(fp);
				ReleaseMutex(SyncUsersMutex);
				return -500;
			}

			planetsArray[gameCount][i].autonum = ntohs(tmp_planets.autonum);
			planetsArray[gameCount][i].planetNumber = ntohs(tmp_planets.planetNumber);
			planetsArray[gameCount][i].createdBy = ntohs(tmp_planets.createdBy);
			planetsArray[gameCount][i].ownedBy = ntohs(tmp_planets.ownedBy);
			planetsArray[gameCount][i].citadel = tmp_planets.citadel; 
			planetsArray[gameCount][i].citclass = tmp_planets.citclass;
			planetsArray[gameCount][i].mReaction = tmp_planets.mReaction;
			planetsArray[gameCount][i].iGenerator = tmp_planets.iGenerator;
			planetsArray[gameCount][i].transwarpPower = ntohs(tmp_planets.transwarpPower);
			planetsArray[gameCount][i].transportPower = ntohs(tmp_planets.transportPower);
			planetsArray[gameCount][i].qCannonPower = tmp_planets.qCannonPower;
			planetsArray[gameCount][i].qCannonAtmos = tmp_planets.qCannonAtmos;
			planetsArray[gameCount][i].qCannonSector = tmp_planets.qCannonSector;
			planetsArray[gameCount][i].reserved2 = tmp_planets.reserved2;
			planetsArray[gameCount][i].sector = ntohl (tmp_planets.sector);
			planetsArray[gameCount][i].fighters = ntohl (tmp_planets.fighters); 
			planetsArray[gameCount][i].shields = ntohl (tmp_planets.shields); 
			planetsArray[gameCount][i].treasury = ntohl (tmp_planets.treasury);
			planetsArray[gameCount][i].figsPerDay = ntohl (tmp_planets.figsPerDay);
			planetsArray[gameCount][i].totalColonists = ntohl (tmp_planets.totalColonists);
			planetsArray[gameCount][i].last_update = ntohl (tmp_planets.last_update);
			strcpy(planetsArray[gameCount][i].name, tmp_planets.name);
			strcpy(planetsArray[gameCount][i].reserved, tmp_planets.reserved);
		
		}

		gameCount++;

		fclose(fp);

		foundFile = FindNextFile(hFindFileHandle, &findData);
	}
	
	FindClose(hFindFileHandle);

	ReleaseMutex(SyncUsersMutex);

	return 0;
}

int hasCharacters (const char *buffer) {
	int i=0;
	
	for (i=0; i<(int) strlen (buffer); i++)
		if (buffer[i] != ' ')
			return 1;

	return 0;
}

int isValidSector (long theSector, long universeSize) {
	if (theSector-1>=0 && theSector-1<universeSize)
		return 1;
	else 
		return 0;
}


int getTradeNumber (char *sessionId, char *name) {
	int gameIndex=-1;
	int i=0, returnVal=-1;
	char buffer[100];

	WaitForSingleObject(SyncUsersMutex, INFINITE);

	for (i=0; i<gameCount; i++) {
		if (stricmp (headerArray[i].sessionId, sessionId)==0) {
			gameIndex=i;
			break;
		}
	}
	if (gameIndex<0) {
		ReleaseMutex(SyncUsersMutex);
		return -1;
	}

	/* try looking up this key */
	char *hashVal = traderHashArray[gameIndex]->FindValue(name);

	/* if we didn't find anything, insert the new key and value */
	if(hashVal == NULL)
	{
		for(i=0; i< MAX_PLAYERS; i++)
		{
			if (playersArray[gameIndex][i].autonum==-1)
				break;
		}
		playersArray[gameIndex][i].autonum=i;
		strcpy(playersArray[gameIndex][i].name, name);

		sprintf (buffer, "%d", i);
		returnVal = i;

		traderHashArray[gameIndex]->Add(playersArray[gameIndex][i].name, buffer);

	}
	else
	{
		returnVal = playersArray[gameIndex][atoi(hashVal)].autonum;
		free(hashVal);
	}

	ReleaseMutex (SyncUsersMutex);

	return(returnVal);

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
	char* str1 = NULL;
	char* str2 = NULL;
	char* str3 = NULL;
	char* str4 = NULL;
	char* str5 = NULL;

	i = Instr(1, str, "&lt;");
	if (i>0) {
		str1 = replace (str, "&lt;", "<");
	} else {
		str1 = strdup(str);
	}
	pfree((void**) &str);

	i = Instr(1, str1, "&gt;");
	if (i>0) {
		str2 = replace (str1, "&gt;", ">");
	} else {
		str2 = strdup(str1);
	}
	pfree((void**) &str1);

	i = Instr(1, str2, "&amp;");
	if (i>0) {
		str3 = replace (str2, "&amp;", "&");
	} else {
		str3 = strdup(str2);
	}
	pfree((void**) &str2);

	i = Instr(1, str3, "&apos;");
	if (i>0) {
		str4 = replace (str3, "&apos;", "\'");
	} else {
		str4 = strdup(str3);
	}
	pfree((void**) &str3);

	i = Instr(1, str4, "&quot;");
	if (i>0) {
		str5 = replace (str4, "&quot;", "\"");
	} else {
		str5 = strdup(str4);
	}
	pfree((void**) &str4);
	
	return(str5);
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


int export_twxv2 (char *filename, long startTime, long endTime, int exportToTWXSync, char* sessionId) {
	FILE *fp=NULL;
	long i=0, j=0;
	int gameIndex=-1;
	char * unescapedName = NULL;
	char * escapedName = NULL;
	
	for (i=0; i<gameCount; i++) {
		if (stricmp (headerArray[i].sessionId, sessionId)==0) {
			gameIndex=i;
			break;
		}
	}
	if (gameIndex<0) {
		return -1;
	}

	WaitForSingleObject(SyncUsersMutex, INFINITE);

	fp = fopen (filename, "wt");
	if (fp==NULL) {
		ReleaseMutex(SyncUsersMutex);
		return (-1);
	}

	if (exportToTWXSync==0) {
		fprintf (fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf (fp, "<TWX version=\"2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.tw-attac.com/TWX.xsd\">\n");
	}


	
	//header
    fprintf (fp, "  <Header>\n");
    fprintf (fp, "    <Created>\n");
    fprintf (fp, "      <Date>%ld</Date>\n", Get_Time());
    fprintf (fp, "      <By>TWXSync</By>\n");
    fprintf (fp, "    </Created>\n");
    fprintf (fp, "    <Game>\n");
//    fprintf (fp, "      <Version>%s</Version>\n", headerArray[gameIndex].twVersion);
//    fprintf (fp, "      <MBBS>%s</MBBS>\n", headerArray[gameIndex].isMBBS);
//    fprintf (fp, "      <Gold>%s</Gold>\n", headerArray[gameIndex].isGoldVersion);
    fprintf (fp, "      <Sectors>%ld</Sectors>\n", headerArray[gameIndex].sector_cnt);
	if (headerArray[gameIndex].stardock>0) {
	    fprintf (fp, "      <StarDock>%ld</StarDock>\n", headerArray[gameIndex].stardock);

	}
	if (headerArray[gameIndex].cls0port_sol>0) {
		fprintf (fp, "      <Class0Port>%ld</Class0Port>\n", headerArray[gameIndex].cls0port_sol);
	}
	if (headerArray[gameIndex].cls0port_alpha>0) {
		fprintf (fp, "      <Class0Port>%ld</Class0Port>\n", headerArray[gameIndex].cls0port_alpha);
	}
	if (headerArray[gameIndex].cls0port_rylos>0) {
	    fprintf (fp, "      <Class0Port>%ld</Class0Port>\n", headerArray[gameIndex].cls0port_rylos);
	}
    fprintf (fp, "    </Game>\n");
    fprintf (fp, "  </Header>\n");
	//trader data
    fprintf (fp, "  <TraderData>\n");

	for (i=0; i<MAX_PLAYERS; i++) {
		if (playersArray[gameIndex][i].autonum >=0 && playersArray[gameIndex][i].last_update >= startTime && playersArray[gameIndex][i].last_update <= endTime) {
			unescapedName = strdup ((char *) playersArray[gameIndex][i].name);
			escapedName = escapeData(unescapedName);

			fprintf (fp, "  <Trader name=\"%s\" updated=\"%ld\">\n", escapedName, playersArray[gameIndex][i].last_update);

			pfree((void**) &escapedName);

			if (playersArray[gameIndex][i].experience>0) {
				fprintf (fp, "	  <Experience>%ld</Experience>\n", playersArray[gameIndex][i].experience);
			}
			if (playersArray[gameIndex][i].alignment>0) {
				fprintf (fp, "	  <Alignment>%ld</Alignment>\n", playersArray[gameIndex][i].alignment);
			}
			if (playersArray[gameIndex][i].corpNumber>0) {
				fprintf (fp, "	  <Corporation>%ld</Corporation>\n", playersArray[gameIndex][i].corpNumber);
			}
//		    fprintf (fp, "	  <Rank>%ld</Rank>\n");
			fprintf (fp, "  </Trader>\n");
		}
	}
  
	fprintf (fp, "  </TraderData>\n");


    fprintf (fp, "  <SectorData>\n");

	for (i=0; i<headerArray[gameIndex].sector_cnt; i++) {
		if (sectorsArray[gameIndex][i].info>=0 &&
				(sectorsArray[gameIndex][i].sector_update >= startTime && sectorsArray[gameIndex][i].sector_update <= endTime ||
				sectorsArray[gameIndex][i].port_update >= startTime && sectorsArray[gameIndex][i].port_update <= endTime )
				)
		{
			fprintf (fp, "  <Sector nr=\"%ld\" updated=\"%ld\">\n", i+1, sectorsArray[gameIndex][i].sector_update);
//			if (strcmp (sectorsArray[gameIndex][i].space, "")==0) {
//			    fprintf (fp, "	  <Space>%ld</Space>\n", sectorsArray[gameIndex][i].space);
//		}
//			if (strcmp (sectorsArray[gameIndex][i].beacon, "")==0) {
//			    fprintf (fp, "	  <Beacon>%ld</Beacon>\n", sectorsArray[gameIndex][i].space);
//		}


			//sector warps
			if (sectorsArray[gameIndex][i].sector_update >= startTime && sectorsArray[gameIndex][i].sector_update <= endTime) {
				fprintf (fp, "	  <Warps>\n");
				for (j=0; j<sectorsArray[gameIndex][i].warps && j<6; j++) {
					fprintf (fp, "	    <Warp>%ld</Warp>\n", sectorsArray[gameIndex][i].warp_sect[j]);
				}
				fprintf (fp, "	  </Warps>\n");
			}
			
			//port data
			if (sectorsArray[gameIndex][i].port_update >= startTime && sectorsArray[gameIndex][i].port_update <= endTime) {
				if ((sectorsArray[gameIndex][i].info>0 && sectorsArray[gameIndex][i].info < 10) || sectorsArray[gameIndex][i].info > UNDER_CONSTRUCTION) {
					fprintf (fp, "	  <Port updated=\"%ld\">\n", sectorsArray[gameIndex][i].port_update);
	//				fprintf (fp, "	    <Name>%s</Name>\n", sectorsArray[gameIndex][i].portname);
					fprintf (fp, "	    <Class>%d</Class>\n", sectorsArray[gameIndex][i].info % UNDER_CONSTRUCTION);
					//how do you deal with underconstruction
					if (sectorsArray[gameIndex][i].info > UNDER_CONSTRUCTION) {
						fprintf (fp, "	    <UnderConstruction>true</UnderConstruction>\n");

					}
					if (sectorsArray[gameIndex][i].info ==12) {
						fprintf (fp, "	    <Destroyed>true</Destroyed>\n");
				
					}
					//fuel ore
					if (sectorsArray[gameIndex][i].port_amt[0]>=0 || sectorsArray[gameIndex][i].port_per[0]>=0) {
						fprintf (fp, "	    <FuelOre>\n");
			
						if (sectorsArray[gameIndex][i].port_amt[0]>=0) {
							fprintf (fp, "	      <Amount>%ld</Amount>\n", sectorsArray[gameIndex][i].port_amt[0]);
						}
						if (sectorsArray[gameIndex][i].port_per[0]>=0) {
							fprintf (fp, "	      <Percentage>%d</Percentage>\n", sectorsArray[gameIndex][i].port_per[0]);
						}
						fprintf (fp, "	    </FuelOre>\n");
					}
					//organics
					if (sectorsArray[gameIndex][i].port_amt[1]>=0 || sectorsArray[gameIndex][i].port_per[1]>=0) {
						fprintf (fp, "	    <Organics>\n");
						if (sectorsArray[gameIndex][i].port_amt[1]>=0) {
							fprintf (fp, "	      <Amount>%ld</Amount>\n", sectorsArray[gameIndex][i].port_amt[1]);
						}
						if (sectorsArray[gameIndex][i].port_per[1]>=0) {
							fprintf (fp, "	      <Percentage>%d</Percentage>\n", sectorsArray[gameIndex][i].port_per[1]);
						}
						fprintf (fp, "	    </Organics>\n");
					}
					//equipment
					if (sectorsArray[gameIndex][i].port_amt[2]>=0 || sectorsArray[gameIndex][i].port_per[2]>=0) {
						fprintf (fp, "	    <Equipment>\n");
						if (sectorsArray[gameIndex][i].port_amt[2]>=0) {
							fprintf (fp, "	      <Amount>%ld</Amount>\n", sectorsArray[gameIndex][i].port_amt[2]);
						}
						if (sectorsArray[gameIndex][i].port_per[2]>=0) {
							fprintf (fp, "	      <Percentage>%d</Percentage>\n", sectorsArray[gameIndex][i].port_per[2]);
						}
						fprintf (fp, "	    </Equipment>\n");
					}
					fprintf (fp, "	  </Port>\n");
				}
			}

			//fighters data
			if (sectorsArray[gameIndex][i].sector_update >= startTime && sectorsArray[gameIndex][i].sector_update <= endTime) {
				if (sectorsArray[gameIndex][i].ftrs>0) {
					fprintf (fp, "	  <Fighters>\n");
					fprintf (fp, "	    <Amount>%ld</Amount>\n", sectorsArray[gameIndex][i].ftrs);
					if (sectorsArray[gameIndex][i].ftr_type>=0) {
						if (sectorsArray[gameIndex][i].ftr_type==0) {
							fprintf (fp, "	    <Type>Mercenary</Type>\n");
						}
						else if (sectorsArray[gameIndex][i].ftr_type==1) {
							fprintf (fp, "	    <Type>Toll</Type>\n");
						}
						else if (sectorsArray[gameIndex][i].ftr_type==2) {
							fprintf (fp, "	    <Type>Offensive</Type>\n");
						}
						else if (sectorsArray[gameIndex][i].ftr_type==3) {
							fprintf (fp, "	    <Type>Defensive</Type>\n");
						}
					}
					if (sectorsArray[gameIndex][i].ftr_owner>0) {
						if (sectorsArray[gameIndex][i].ftr_owner>CORP_CONSTANT) {
							fprintf (fp, "	    <Owner corp=\"true\">%d</Owner>\n", sectorsArray[gameIndex][i].ftr_owner-CORP_CONSTANT);
						}
						else {
							if (sectorsArray[gameIndex][i].ftr_owner>=0 && sectorsArray[gameIndex][i].ftr_owner<MAX_PLAYERS) {
//fix								fprintf (fp, "	    <Owner corp=\"false\">%s</Owner>\n", playersArray[sectorsArray[gameIndex][i].ftr_owner].name);
							}
						}
					}
					fprintf (fp, "	  </Fighters>\n");
				}
				//armid mines
				if (sectorsArray[gameIndex][i].armids>0) {
					fprintf (fp, "	  <ArmidMines>\n");
					fprintf (fp, "	    <Amount>%ld</Amount>\n", sectorsArray[gameIndex][i].armids);
					if (sectorsArray[gameIndex][i].armid_owner>0) {
						if (sectorsArray[gameIndex][i].armid_owner>CORP_CONSTANT) {
							fprintf (fp, "	    <Owner corp=\"true\">%d</Owner>\n", sectorsArray[gameIndex][i].armid_owner-CORP_CONSTANT);
						}
						else {
							if (sectorsArray[gameIndex][i].armid_owner>=0 && sectorsArray[gameIndex][i].armid_owner<MAX_PLAYERS) {
//								fprintf (fp, "	    <Owner corp=\"false\">%s</Owner>\n", playersArray[sectorsArray[gameIndex][i].armid_owner].name);
							}
						}

					}
					fprintf (fp, "	  </ArmidMines>\n");
				}

				//limpit mines
				if (sectorsArray[gameIndex][i].limpets>0) {
					fprintf (fp, "	  <LimpetMines>\n");
					fprintf (fp, "	    <Amount>%ld</Amount>\n", sectorsArray[gameIndex][i].limpets);
					if (sectorsArray[gameIndex][i].limpet_owner>0) {
						if (sectorsArray[gameIndex][i].limpet_owner>CORP_CONSTANT) {
							fprintf (fp, "	    <Owner corp=\"true\">%d</Owner>\n", sectorsArray[gameIndex][i].limpet_owner-CORP_CONSTANT);
						}
						else {
							if (sectorsArray[gameIndex][i].limpet_owner>=0 && sectorsArray[gameIndex][i].limpet_owner<MAX_PLAYERS) {
//								fprintf (fp, "	    <Owner corp=\"false\">%s</Owner>\n", playersArray[sectorsArray[gameIndex][i].limpet_owner].name);
							}
						}
					}
					fprintf (fp, "	  </LimpetMines>\n");
				}

				//density
				if (sectorsArray[gameIndex][i].density>0) {
					fprintf (fp, "	  <Density>%ld</Density>\n", sectorsArray[gameIndex][i].density);
				}

				//last bust
				//null error here
				if (sectorsArray[gameIndex][i].lastBusted>0) {
					if (sectorsArray[gameIndex][i].playerIdBusted>=0)
						fprintf (fp, "	  <LastBust trader=\"%s\">%ld</LastBust>\n", playersArray[gameIndex][sectorsArray[gameIndex][i].playerIdBusted].name, sectorsArray[gameIndex][i].lastBusted);
					else
						fprintf (fp, "	  <LastBust >%ld</LastBust>\n", sectorsArray[gameIndex][i].lastBusted);
				}

				//Anomaly
				if (sectorsArray[gameIndex][i].anom>=0) {
					if (sectorsArray[gameIndex][i].anom==1) {
						fprintf (fp, "	  <Anomaly>true</Anomaly>\n");
					}
					else {
						fprintf (fp, "	  <Anomaly>false</Anomaly>\n");
					}
				}
				//navhaz
				if (sectorsArray[gameIndex][i].navhaz>0) {
					fprintf (fp, "	  <NavHaz>%d</NavHaz>\n", sectorsArray[gameIndex][i].navhaz);
				}
			}

			fprintf (fp, "  </Sector>\n");
		}
	}
    
	fprintf (fp, "  </SectorData>\n");

	if (exportToTWXSync==0) {
		fprintf (fp, "</TWX>\n");
	}

	fclose (fp);

	ReleaseMutex(SyncUsersMutex);

	return (0);
	
}

char * getFile (char* file)
{
	char *fileData;
	int existsFiles;
	FILE *fp;
	struct stat buf;
	long fileSize=0;
	int numread=0;

	WaitForSingleObject(SyncUsersMutex, INFINITE);

	fp = fopen (file, "rb");
	if (fp==NULL)
	{
		printf ("Error opening file %s\n", file);
		fileData = (char *) strdup (" ");
		ReleaseMutex(SyncUsersMutex);
		return(fileData);
	}

	existsFiles = stat(file, &buf);
	fileSize = buf.st_size;
	fileData = (char *) calloc (fileSize+1, sizeof(char));
	
	fseek(fp, 0L, SEEK_SET);
	numread = fread (fileData, fileSize, 1, fp);	
	fileData[fileSize]=0;
	fclose (fp);

	ReleaseMutex(SyncUsersMutex);
	return (fileData);
}
