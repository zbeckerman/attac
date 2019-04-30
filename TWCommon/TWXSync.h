// TWXSync.h: interface for the CTWXSync class.
//

#if !defined(AFX_TWXSYNC_H__467EBDB8_8884_47C2_A746_ADC739B62D88__INCLUDED_)
#define AFX_TWXSYNC_H__467EBDB8_8884_47C2_A746_ADC739B62D88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TWCommon.h"
#include "Expat.h"
#include "List.h"
#include "Hash.h"
//#include "zlibengn.h"

#define T(x) x
#define NSSEP T('\001')

#define RECURSIVE_TAGS 50
#define MAX_CONNECTIONS 10
#define MAX_READ_SIZE 1024
#define TWXDATAFROMFILE "TWXDataFromFile"

unsigned __stdcall ListenServerThreadProc(void *arg);
void IncomingDataThreadProc(void *arg);



class TWCOMMON_CLASS CTWXSync
{
friend unsigned __stdcall ListenServerThreadProc(void *arg);
friend void IncomingDataThreadProc(void *arg);
friend char* createTWXFile(CTWXSync *pTWXSync, char *name, char *password, long sectorCount);
friend int saveTWXFile(CTWXSync *pTWXSync, char* sessionId);
friend int openAllTWXFiles();
friend int hasCharacters (const char *buffer);
friend int isValidSector (long theSector, long universeSize);
friend int getTradeNumber (char *sessionId, char *name);
friend int export_twxv2 (char *filename, long startTime, long endTime, int exportToTWXFile, char* sessionId);
friend char * getFile (char* file);
friend char* escapeData (char* str);
friend char* unescapeData (char* str);
friend char* replace (char *str, char toReplace[], char replaceWith[]);

public:
	CTWXSync();
	virtual ~CTWXSync();

	long GetHandle();
	static CTWXSync *GetObject(long hTWXSync);

	int Init(void (*pIncomingDataCallback)(long hTWXSync, const char *data, int len),
			 void (*pOutgoingDataCallback)(long hTWXSync, const char *data, int len),
			 void (*pErrorCallback)(long hTWXSync, int errorCode, const char *msg, int len));
	int Init(void (*pIncomingDataCallback)(long hTWXSync, char **data, int len),
			 void (*pErrorCallback)(long hTWXSync, int errorCode, const char *msg, int len));
	unsigned Listen(void (*pNewConnectionCallback)(long hTWXSync, long hNewTWXSync, const char *host, int port, const char *username, const char *helper, const char *game),
					void (*pValidUserLoginCallback)(long hTWXSync, const char *host, int port, const char *username, long timeConnected, const char *helper, const char *game),
			   unsigned short port, __int64 handle);
	int StopListen(unsigned threadId);
	int Connect(const char *host, int port);
	int TerminateConnection(const char *ipaddress);
	int StartServer(const char *m_appDir);
	void Close();
	int Send(const char *data, int len);
	int ReceiveAllDataWrapper(char **data);
	

	// TWX sync protocol
	void HandleRequest(const char *data, int len);
	void HandleResponse(const char *data, int len);

    CList GetAttrList();
    void SetAttrList(CList c);

    int GetDepth();
    void SetDepth(int c);

    int GetSkipParse();
    void SetSkipParse(int c);

	char *m_tags[MAX_TAG_DEPTH];
	int m_emptyTag;

	XML_Parser parser;
	long number;
	long lastUpdated;
	long twxVersion;
	char twxCreatedBy[MAX_LENGTH];
	long twxCreatedOn;
	char traderName[MAX_LENGTH];
	char ships[MAX_LENGTH];
	int corp;
	long finished;
	long hops;
	char loginId[128];
	char uniqueId[128];
	char username[MAX_LENGTH];
	char password[MAX_LENGTH];
	char sessionId[128];
	char gameName[MAX_LENGTH];
	char helper[50];
	char requestType[MAX_LENGTH];
	int loginPassCheck;
	long begin;
	long end;

	char returnBuffer[MAX_LENGTH];
	int returnError;
	long packetCountSent;
	long packetCountReceived;
	long timeConnected;

	HANDLE threadSafeMutex;
	///need this since recv blocks
	HANDLE recvThreadSafeMutex;
	HANDLE xmlImportThreadSafeMutex;
	CZlibEngine *engine;
	int compareObject(CTWXSync *userData);

protected:

    // Client
    SOCKET GetClientSocket();
	void SetClientSocket(SOCKET s);
	unsigned short GetClientPortNumber();
    void SetClientPortNumber(unsigned short port);
    const char* GetClientHost();
    void SetClientHost(const char* hostname);

	// Server
    unsigned short GetServerPortNumber();
    void SetServerPortNumber(unsigned short port);
    SOCKET GetServerSocket();
    void SetServerSocket(SOCKET s);

	// Callbacks
	int FireIncomingDataCallback(const char *data, int len);
	int FireOutgoingDataCallback(const char *data, int len);
	int FireErrorCallback(int errorCode, const char *msg, int len);
	int FireNewConnectionCallback(CTWXSync *pNewTWXSync);
	int FireValidUserLoginCallback(CTWXSync *pNewTWXSync);


protected:
	/*
	static void ReqStartElement(void *userData, const char *tag, const char **atts);
	static void ReqEndElement(void *userData, const char *tag);
	static void ReqCharacterData(void *userData, const char *tag, const char *data);
*/
	static void ReqStartElement(void *userData, const XML_Char *tag, const XML_Char **atts);
	static void ReqEndElement(void *userData, const XML_Char *tag);
	static void ReqCharacterData(void *userData, const XML_Char *s, int len);

	static void RespStartElement(void *userData, const char *tag, const char **atts);
	static void RespEndElement(void *userData, const char *tag);
	static void RespCharacterData(void *userData, const char *tag, const char *data);

private:
	CList m_list;
	int m_depth;
	int m_skipParse;

	// Client
	unsigned short m_clientPort;
	char *m_clientHostname;
	SOCKET m_clientSocket;

	// Server
	unsigned short m_serverPort;
	SOCKET m_serverSocket;

	int Receive(const char *data, int len);
	void (*m_pIncomingDataCallback)(long hTWXSync, const char *data, int len);
	void (*m_pOutgoingDataCallback)(long hTWXSync, const char *data, int len);
	void (*m_pErrorCallback)(long hTWXSync, int errorCode, const char *msg, int len);
	void (*m_pNewConnectionCallback)(long hTWXSync, long hNewTWXSync, const char *host, int port, const char *username, const char *helper, const char *game);
	void (*m_pValidUserLoginCallback)(long hTWXSync, const char *host, int port, const char *username, long timeConnected, const char *helper, const char *game);
	
};





#endif // !defined(AFX_TWXSYNC_H__467EBDB8_8884_47C2_A746_ADC739B62D88__INCLUDED_)
