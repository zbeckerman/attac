// TWCommon.h
#ifndef TW_COMMON_H
#define TW_COMMON_H

#define MAX_TAG_DEPTH 50

// Choose between C and C++
// Comment the following line for C only
//#define TWCOMMON_CPP

#ifdef TWCOMMON_EXPORTS
	#include "zlibengn.h"
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TWCOMMON_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TWCOMMON_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef TWCOMMON_EXPORTS
	#ifdef TWCOMMON_CPP
		#define TWCOMMON_API __declspec(dllexport)
		#define TWCOMMON_CLASS TWCOMMON_API
	#else
		#ifdef __cplusplus
			#define TWCOMMON_API extern "C" __declspec(dllexport)
		#else
			#define TWCOMMON_API __declspec(dllexport)
		#endif
		#define TWCOMMON_CLASS
	#endif
#else
	#ifdef TWCOMMON_CPP
		#define TWCOMMON_API __declspec(dllimport)
		#define TWCOMMON_CLASS TWCOMMON_API
	#else
		#ifdef __cplusplus
			#define TWCOMMON_API extern "C" __declspec(dllimport)
		#else
			#define TWCOMMON_API __declspec(dllimport)
		#endif
		#define TWCOMMON_CLASS
	#endif
#endif

//start authorization
#define VALID_REGISTRATION 0
#define INVALID_REGISTRATION 1
#define INVALID_VERSION 2

//////////////////////////////////////////////////////////////////////////
// TWCommon C functions

TWCOMMON_API void TWC_GetLastError(char *msg, int len);

TWCOMMON_API int TWC_EncryptMessage(unsigned char *data, int len,
									unsigned char *key, const char *name);
TWCOMMON_API int TWC_DecryptMessage(unsigned char *data, int len,
									unsigned char *key, const char *name);


//////////////////////////////////////////////////////////////////////////
// TWCommon object oriented C++ API
#ifdef TWCOMMON_CPP

class CTWXSync;
class CExpat;
class CDataPairAttr;
class CList;
class CHash;
class CZlibEngine;
class CUnsignedCharStr;

TWCOMMON_API CTWXSync* TWC_CreateTWXSyncSession();
TWCOMMON_API CExpat* TWC_CreateExpatParser();

#endif


//////////////////////////////////////////////////////////////////////////
// TWCommon object oriented C API
#ifndef TWCOMMON_CPP

//client methods for helpers to use
TWCOMMON_API long TWC_CreateTWXSyncSession();
TWCOMMON_API int TWC_TWXSync_StartServer(long hTWXSync, const char *m_appPath);
TWCOMMON_API int TWC_TWXSync_Init(long hTWXSync,
	void (*pIncomingDataCallback)(long hTWXSync, const char *data, int len),
	void (*pOutgoingDataCallback)(long hTWXSync, const char *data, int len),
	void (*pErrorCallback)(long hTWXSync, int errorCode, const char *msg, int len));
TWCOMMON_API unsigned TWC_TWXSync_Listen(long hTWXSync,
	void (*pNewConnectionCallback)(long hTWXSync, long hNewTWXSync, const char *host, int port, const char *username, const char *helper, const char *game),
	void (*pValidUserLoginCallback)(long hTWXSync, const char *host, int port, const char *username, long timeConnected, const char *helper, const char *game),
	unsigned short port, __int64 handle);
TWCOMMON_API int TWC_TWXSync_StopListen(long hTWXSync, unsigned threadId);
TWCOMMON_API int TWC_TWXSync_Connect(long hTWXSync, const char *host, int port);
TWCOMMON_API void TWC_TWXSync_Close(long hTWXSync);
TWCOMMON_API int TWC_TWXSync_TerminateConnection(long hTWXSync, const char *ipaddress);

TWCOMMON_API int TWC_TWXSync_Send(long hTWXSync, const char *data, int len);
TWCOMMON_API int TWC_TWXSync_Receive(long hTWXSync, const char *data, int len);
TWCOMMON_API void TWC_DeleteTWXSyncSession(long hTWXSync);

TWCOMMON_API int TWC_TWXSync_ReceiveAllDataWrapper(long hTWXSync, char **data);
TWCOMMON_API void TWC_TWXSync_FreeMem(void **data);

TWCOMMON_API int TWC_TWXSync_Encode(char *in, unsigned char **out);
TWCOMMON_API int TWC_TWXSync_Decode(unsigned char *in, int len, char **out);

#ifdef TWCOMMON_EXPORTS
TWCOMMON_API int TWC_TWXSync_Encode_With_Engine(CZlibEngine *internalEngine, char *in, unsigned char **out);
TWCOMMON_API int TWC_TWXSync_Decode_With_Engine(CZlibEngine *internalEngine, unsigned char *in, int len, char **out, char *isInvalidCrc32);
#endif

//expat methods - used internalled
TWCOMMON_API long TWC_CreateExpatParser();
TWCOMMON_API char * TWC_CreateGUID();
TWCOMMON_API void TWC_DeleteExpatParser(long hExpat);

TWCOMMON_API void TWC_HandleRequest(long hTWXSync, const char *data, int len);
TWCOMMON_API void TWC_HandleResponse(long hTWXSync, const char *data, int len);

//game maintenance methods - used by server
TWCOMMON_API int TWC_GetGameCount();
TWCOMMON_API char* TWC_GetGameName(long gameNumber);
TWCOMMON_API long TWC_GetUniverseSize(long gameNumber);
TWCOMMON_API char* TWC_GetGamePassword(long gameNumber);
TWCOMMON_API char* TWC_GetGameSessionId(long gameNumber);
TWCOMMON_API char * TWC_CreateGame(long hTWXSync, char *name, long universeSize, char *password);
TWCOMMON_API void TWC_MaintainPassword(char* sessionId, char *password);
TWCOMMON_API int TWC_DeleteGame(char *sessionId);
TWCOMMON_API int TWC_ExportTWXv2(char *sessionId, char *pathToWrite);
TWCOMMON_API char* TWC_Version();
TWCOMMON_API char* TWC_Encode64Export(char *data, int n);
TWCOMMON_API void authorizeProgramExternal(void (*m_pAuthorizeCallback)(int r_isRegDeactivated, char *r_returnBuffer), char *m_program, char *m_userId, int m_releaseVersion, int m_majorVersion, int m_minorVersion, char *m_betaVersion);

/**
  * This function accepts a callback that takes a isRegDeactivated and a reason, then the program name (TWGS), userId, release version, major version, minor version, beta version, action taken, and xmlData
  * 
  */
TWCOMMON_API void jumpgateProgramExternal(void (*m_pAuthorizeCallback)(int r_isRegDeactivated, char *r_returnBuffer), char *m_program, char *m_userId, int m_releaseVersion, int m_majorVersion, int m_minorVersion, char *m_betaVersion, char *m_action, char *m_xmlData);


void pfree (void **x);
char * appendData(char * oldData, char newData[]);
char * appendData2(char * oldData, char newData[]);


#endif
/*
void authorizeProgramExternal (void *val) {
	unsigned long authThreadId=0;
	if (!CreateThread(NULL, 0, AuthorizeRegistrationThreadProc, NULL, 0, &authThreadId)) {
		printf("Failed to create thread");
		return;
	}
}
*/

#endif
