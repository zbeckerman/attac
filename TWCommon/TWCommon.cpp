// TWCommon.cpp : Defines the entry point for the DLL application.
//

#include <stdio.h>
#include <time.h>
#include <rpc.h>

#include "zlibengn.h"
#include "Common.h"
#include "TWCommon.h"
#include "TWXSync.h"
#include "Authorize.h"
#include "Expat.h"
#include "3PP/DES/DES.h"

//////////////////////////////////////////////////////////////////////////
// DLL specific functions

BOOL APIENTRY DllMain( HANDLE hModule, DWORD dwReason, LPVOID lpReserved )
{
//	int i;
    switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			freeMutex = CreateMutex(NULL, 0, NULL);
			SyncUsersMutex = CreateMutex(NULL, 0, NULL);
			ListOfConnectedUsersMutex = CreateMutex(NULL, 0, NULL);
			parseSafeMutex = CreateMutex(NULL, 0, NULL);
			zlibEngineMutex = CreateMutex(NULL, 0, NULL);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

APIENTRY DllRegisterServer()
{
    return TRUE;
}

APIENTRY DllUnregisterServer()
{
    return TRUE;
}



//////////////////////////////////////////////////////////////////////////
// TWCommon C functions

/**
 * Gets the last error message.
 *
 * @param msg The buffer that will receive the error message
 * @param len The length of the buffer
 */
TWCOMMON_API void TWC_GetLastError(char *msg, int len)
{
	if (msg == NULL) {
		return;
	}
	if (len == 0) {
		msg[0]=0;
		return;
	}
	strncpy(msg, s_error_msg, len);
	msg[len-1]=0;
}

/**
 * Encrypts a TW message. Max size is 53 characters.
 *
 * @param data The message
 * @param len The message length in bytes
 * @param key The 7 byte key
 * @param name The player name
 * @return The encrypted message length
 */
TWCOMMON_API int TWC_EncryptMessage(unsigned char *data, int len,
									unsigned char *key, const char *name)
{
	CDES des;
	unsigned char buf[256];
	unsigned char iv[8];
	int n;

	// Validate
	if (len > 53) len=53;
	data[53]=0;

	// Encrypt message
	memcpy(buf, "TW", 2);
	buf[2]=len;
	memcpy(buf+3, data, len);
	n=(len+3+7)/8;
	memset(iv, 0, 8);
	strncpy((char *)iv, name, 6);
	des.SetMode(DES_EC_S);
	des.SetKey(1, key);
	des.CBC(buf, n, iv, TRUE);
	len=n*8;
	n=(len+2)/3;
	memset(buf+len, 0, n*3-len);
	TWC_Encode64(buf, n);
	len=n*4;
	memcpy(data+1, buf, len+1);
	data[0]='!';
	len++;
	data[len]=0;

	return len;
}

/**
 * Decrypts a TW message.
 *
 * @param data The message
 * @param len The message length in bytes
 * @param key The 7 byte key
 * @param name The player name
 * @return The decrypted message length
 */
TWCOMMON_API int TWC_DecryptMessage(unsigned char *data, int len,
									unsigned char *key, const char *name)
{
	CDES des;
	unsigned char buf[256];
	unsigned char iv[8];
	int n;

	// Validate
	if (data[0] != '!') return -1;
	for (n=1; n<len; n++) {
		if (data[n] == 32) return -1;
	}
	len--;

	// Decrypt message
	memcpy(buf, data+1, len);
	n=len/4;
	TWC_Decode64(buf, n);
	n=n*3/8;
	memset(iv, 0, 8);
	strncpy((char *)iv, name, 6);
	des.SetMode(DES_EC_S);
	des.SetKey(1, key);
	des.CBC(buf, n, iv, FALSE);
	if (strncmp((char *)buf, "TW", 2) == 0) {
		len=buf[2];
		memcpy(data, buf+3, len);
		data[len]=0;
		return len;
	}
	else return -1;
}



//////////////////////////////////////////////////////////////////////////
// TWCommon object oriented C++ API

#ifdef TWCOMMON_CPP

/**
 * Creates a new TWX sync session object.
 *
 * @return The TWXSync object
 */
TWCOMMON_API CTWXSync* TWC_CreateTWXSyncSession()
{
	return new CTWXSync();
}

/**
 * Creates a new Expat parser object.
 *
 * @return The Expat object
 */
TWCOMMON_API CExpat* TWC_CreateExpatParser()
{
	return new CExpat();
}

#endif



//////////////////////////////////////////////////////////////////////////
// TWCommon object oriented C API

#ifndef TWCOMMON_CPP

/**
 * Creates a new TWX sync session.
 *
 * @return The TWXSync handle
 */
TWCOMMON_API long TWC_CreateTWXSyncSession()
{
	CTWXSync *pTWXSync = new CTWXSync();
	return pTWXSync->GetHandle();
}

TWCOMMON_API int TWC_TWXSync_StartServer(long hTWXSync, const char *m_appPath)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->StartServer(m_appPath);
}


TWCOMMON_API int TWC_TWXSync_Init(long hTWXSync,
	void (*pIncomingDataCallback)(long hTWXSync, const char *data, int len),
	void (*pOutgoingDataCallback)(long hTWXSync, const char *data, int len),
	void (*pErrorCallback)(long hTWXSync, int errorCode, const char *msg, int len))
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->Init(pIncomingDataCallback, pOutgoingDataCallback, pErrorCallback);
}

TWCOMMON_API unsigned TWC_TWXSync_Listen(long hTWXSync,
	void (*pNewConnectionCallback)(long hTWXSync, long hNewTWXSync, const char *host, int port, const char *username, const char *helper, const char *game),
	void (*pValidUserLoginCallback)(long hTWXSync, const char *host, int port, const char *username, long timeConnected, const char *helper, const char *game),
	unsigned short port, __int64 handle)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->Listen(pNewConnectionCallback, pValidUserLoginCallback, port, handle);
}

TWCOMMON_API int TWC_TWXSync_StopListen(long hTWXSync, unsigned threadId)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->StopListen(threadId);
}

TWCOMMON_API int TWC_TWXSync_TerminateConnection(long hTWXSync, const char *ipaddress)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->TerminateConnection(ipaddress);
}



TWCOMMON_API int TWC_TWXSync_Connect(long hTWXSync, const char *host, int port)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->Connect(host, port);
}

TWCOMMON_API void TWC_TWXSync_Close(long hTWXSync)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	pTWXSync->Close();
}

TWCOMMON_API int TWC_TWXSync_Send(long hTWXSync, const char *data, int len)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->Send(data, len);
}

/*
TWCOMMON_API int TWC_TWXSync_Receive(long hTWXSync, const char *data, int len)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	return pTWXSync->Receive(data, len);
}
*/

/**
 * Free memory. The client calls this to free memory that was allocation in the DLL
 * freeing it in the client exe would cause a heap error
 *
 * @param data the data we are freeing
 */
TWCOMMON_API void TWC_TWXSync_FreeMem(void **data)
{
	if (*data!=NULL)
		free(*data);
    *data = NULL;
}

//pass pointer to NULL char * and will return data by reference
TWCOMMON_API int TWC_TWXSync_ReceiveAllDataWrapper(long hTWXSync, char **data)
{
	char *returnData=NULL;
	int r=0;
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	r = pTWXSync->ReceiveAllDataWrapper(&returnData);
	*data = returnData;
	return r;
}

TWCOMMON_API int TWC_TWXSync_Encode(char *in, unsigned char **out)
{
	WaitForSingleObject(zlibEngineMutex, INFINITE);
	CZlibEngine *newEngine = new CZlibEngine();
	int x = TWC_TWXSync_Encode_With_Engine(newEngine, in, out);
	delete newEngine;
	ReleaseMutex(zlibEngineMutex);
	return x;
}

TWCOMMON_API int TWC_TWXSync_Encode_With_Engine(CZlibEngine *internalEngine, char *in, unsigned char **out)
{
	
	char buffer[64];

	CUnsignedCharStr* dataToCompress = new CUnsignedCharStr(in);
	//encrypt
	dataToCompress->encryptString();
	//zlib compression
	CUnsignedCharStr* compress = internalEngine->compressInMemory(dataToCompress);
	//calculate crc32
	sprintf (buffer, "%lu", compress->crc32());
	//append a token i can find
	compress->appendString((unsigned char*)CRC_32, CRC_32_LEN);
	//append the crc32 value
	compress->appendString((unsigned char*)buffer, strlen(buffer));

	int result = compress->hasString(DATA_COMPLETE);
	//if not, add it
	if (result==-1) {
		compress->appendString((unsigned char *)DATA_COMPLETE, DATA_COMPLETE_LEN);
		result=-1;
	}
	
	//return memory by reference
	*out = compress->getString();
	//return length by value
	int len = compress->getLength();
	//delete the data to compress memory
	delete dataToCompress;
	//delete the compressed memory
	delete compress;
	return len;
}

TWCOMMON_API int TWC_TWXSync_Decode(unsigned char *in, int len, char **out)
{
	char *isInvalidCrc32 = strdup("0");
	WaitForSingleObject(zlibEngineMutex, INFINITE);
	CZlibEngine *newEngine = new CZlibEngine();
	int x = TWC_TWXSync_Decode_With_Engine(newEngine, in, len, out, isInvalidCrc32);
	delete newEngine;
	ReleaseMutex(zlibEngineMutex);
	pfree ((void**) &isInvalidCrc32);
	return x;

}
TWCOMMON_API int TWC_TWXSync_Decode_With_Engine(CZlibEngine *internalEngine, unsigned char *in, int len, char **out, char *isInvalidCrc32)
{
	
	CUnsignedCharStr* dataToDecompress = new CUnsignedCharStr();
	dataToDecompress->appendString(in, len);

	//check if there is a complete token
	int result = dataToDecompress->hasString(DATA_COMPLETE);
	//if so remove it
	if (result!=-1) {
		dataToDecompress->trimStringRight(dataToDecompress->getLength()-result);
		result=-1;
	}

	//check if the crc32 token exists
	result = dataToDecompress->hasString(CRC_32);
	//default is that the packet is ok
	int validCrc32=1;
	if (result!=-1) {
		//if i found me token, then get the crc32 value and trim the crc token
		//and value from the string
		unsigned char *crc32 = dataToDecompress->getCrc32FromString(result);

		dataToDecompress->trimStringRight(dataToDecompress->getLength()-result);
		//confirm that the crc32 value is good
		validCrc32 = dataToDecompress->confirmCrc32(crc32);
		//free the memory from getting the crc32 value
		pfree((void **) &crc32);
	}
	
	//if the Crc32 check is valid or no crc32 was sent

	if (validCrc32==1) {
		//do our decompression
		CUnsignedCharStr* decompress = internalEngine->decompressInMemory(dataToDecompress);
		//decrypt string
		decompress->decryptString();
		//return string by reference
		*out = (char *)decompress->getString();
		//return len by value
		len = decompress->getLength();
		//delete memory we used
		delete decompress;
	} else {
		//if it was invalid, just pretend the packet was empty
		char buf[512];
		FILE *ff=NULL;
		srand( (unsigned)time( NULL ) );
		if (isInvalidCrc32 != NULL && strlen (isInvalidCrc32)>0)
			isInvalidCrc32[0]='1';

		//generate empty packet
		sprintf(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><TWXSync version=\"%d\"> <Session id=\"%s\"/> <Result code=\"200\" text=\"OK\"/></TWXSync>", TWXSYNX_VERSION, "");

		*out = strdup (buf);
		//get the length
		len = strlen (*out);

		//printout the debug info
		sprintf(buf, "%sTWXSyncDebug.txt", appDir);
		ff = fopen (buf, "a");
		if (ff!= NULL) {
			fprintf (ff,"\n\n------------TWC_TWXSync_Decode---------------\n");
			fprintf (ff,"Bad Packet crc32 Invalid: Length %d\n", dataToDecompress->getLength());
			fclose(ff);

		}
	}
	//delete data to decompress memory
	delete dataToDecompress;
	return len;
}


//expat functions
TWCOMMON_API void TWC_HandleRequest(long hTWXSync, const char *data, int len)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	pTWXSync->HandleRequest(data, len);
}
TWCOMMON_API void TWC_HandleResponse(long hTWXSync, const char *data, int len)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	pTWXSync->HandleResponse(data, len);
}

/**
 * Deletes the TWX sync session.
 *
 * @param hTWXSync The TWX sync handle
 */
TWCOMMON_API void TWC_DeleteTWXSyncSession(long hTWXSync)
{
	CTWXSync *pTWXSync = CTWXSync::GetObject(hTWXSync);
	delete pTWXSync;
}

/**
 * Creates a new Expat parser.
 *
 * @return The Expat handle
 */


TWCOMMON_API long TWC_CreateExpatParser()
{
	CExpat *pExpat = new CExpat();
	return pExpat->GetHandle();
}


/**
 * Deletes the Expat parser.
 *
 * @param hExpat The Expat handle
 */

TWCOMMON_API void TWC_DeleteExpatParser(long hExpat)
{
	CExpat *pExpat = CExpat::GetObject(hExpat);
	delete pExpat;
}

/**
 * Callee must free memory from this function call
 *
 * @return char*
 */
TWCOMMON_API char* TWC_CreateGUID() {
	char *returnValue=NULL;
	UUID u;
	RPC_STATUS stat = UuidCreate( &u );
	char buffer[40];
	srand( (unsigned)time( NULL ) );
	if ( stat == RPC_S_OK || stat==RPC_S_UUID_LOCAL_ONLY)
	{
		unsigned char* p;
		stat = UuidToString( &u, &p );
		if ( stat == RPC_S_OK )
		{
			returnValue = (char *) strdup ( (char *)p);
			stat = RpcStringFree( &p );
			// if ( stat == RPC_S_OK )
			// else
		}
	}
	sprintf (buffer, "%d%d", rand(), rand());
	if (returnValue==NULL)
		returnValue = (char *) strdup (buffer);

	return returnValue;
}

//game maintenance methods
TWCOMMON_API int TWC_GetGameCount() {
	return gameCount;
}

TWCOMMON_API char* TWC_GetGameName(long gameNumber) {
	char *returnVal;
	if (gameNumber<gameCount) {
		returnVal = (char *)strdup (headerArray[gameNumber].file_name);
		return (returnVal);
	} else {
		return (returnVal = (char *)strdup (" "));
	}
}

TWCOMMON_API long TWC_GetUniverseSize(long gameNumber) {
	if (gameNumber<gameCount) {
		return (headerArray[gameNumber].sector_cnt);
	} else {
		return (0);
	}
}


TWCOMMON_API char* TWC_GetGamePassword(long gameNumber) {
	char *returnVal;
	if (gameNumber<gameCount) {
		returnVal = (char *)strdup (headerArray[gameNumber].password);
		return (returnVal);
	} else {
		return (returnVal = (char *)strdup (" "));
	}
}

TWCOMMON_API char* TWC_GetGameSessionId(long gameNumber) {
	char *returnVal;
	if (gameNumber<gameCount) {
		returnVal = (char *)strdup (headerArray[gameNumber].sessionId);
		return (returnVal);
	} else {
		return (returnVal = (char *)strdup (" "));
	}
}



TWCOMMON_API char * TWC_CreateGame(long hTWXSync, char *name, long universeSize, char *password) {
	CTWXSync *pTWXSync;
	if (hTWXSync>0)
		pTWXSync = CTWXSync::GetObject(hTWXSync);
	else
		pTWXSync = NULL;
	char* sess = createTWXFile (pTWXSync,name, password, universeSize);

	return (sess);
}

TWCOMMON_API void TWC_MaintainPassword(char* sessionId, char *password) {
	int i=0;
	for (i=0; i<gameCount; i++) {
		if (stricmp (headerArray[i].sessionId, sessionId)==0) {
			strcpy (headerArray[i].password, password);
			saveTWXFile(NULL, sessionId);
			break;
		}
	}
}

TWCOMMON_API int TWC_ExportTWXv2(char *sessionId, char *pathToWrite) {
		return export_twxv2(pathToWrite, 0, Get_Time()+100000, 0, sessionId);
}
TWCOMMON_API int TWC_DeleteGame(char *sessionId) {
	int i=0, j=0;
	char fullFilePath[1024];

	for (i=0; i<gameCount; i++) {
		if (stricmp (headerArray[i].sessionId, sessionId)==0) {
			sprintf (fullFilePath, "%s%s", appDir, headerArray[i].file_name);
			//delete file		
			DeleteFile(fullFilePath);
			//free memory for sector data
			for (j=0; j<gameCount; j++)
				TWC_TWXSync_FreeMem((void**) &sectorsArray[j]);

			gameCount=0;

			openAllTWXFiles();
			break;
		}
	}
	return 0;
}

//need to free memory from caller
TWCOMMON_API char* TWC_Version() {
	char *buffer=NULL;
	buffer = (char *) malloc(128);
	sprintf (buffer, "%d.%d.%d", releaseVersion, majorVersion, minorVersion);

	return buffer;
}


  //implementation of pfree  since it is not in the file
void pfree (void **x)
{
        WaitForSingleObject(freeMutex, INFINITE);
        if (*x==NULL) {
			#ifdef ATTAC_CODE
					attacErrorMessage("THERE IS A FREE MEMORY ERROR");
			#else
					printf ("THERE IS A FREE MEMORY ERROR");
			#endif
		}
        else
                free(*x);
        *x = NULL;
        ReleaseMutex(freeMutex);
}

//i free old data here. caller must free newData
char * appendData(char * oldData, char newData[]) {
	char *returnData=NULL;
	WaitForSingleObject (SyncUsersMutex, INFINITE);
	if (oldData==NULL) {
		long newDataLen = strlen(newData);
		returnData = (char*) malloc (newDataLen+1);
		strcpy (returnData, newData);
		returnData[newDataLen]=0;
	} else {
		long oldDataLen = strlen(oldData);
		long newDataLen = strlen(newData);

		returnData = (char *) malloc (oldDataLen+newDataLen+1);
		strcpy (returnData, oldData);
		strcat (returnData, newData);
		returnData[oldDataLen+newDataLen]=0;
		pfree((void **)&oldData);
	}

	ReleaseMutex(SyncUsersMutex);
	return (returnData);
}

#endif
