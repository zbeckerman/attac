#include <sys/stat.h>
#include <time.h>

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include "attacGUI.h"
#include "util.h"
#include "twxSync.h"
#include "attacGlobal.h"
#include "info.h"
#include "xml.h"
#include "./3PP/expat/expat.h"
#include "rexx.h"
#include "win_res.h"
#include "../../TWCommon/TWCommon.h"

#ifndef _MT
#define _MT
#endif

HANDLE twxSyncWaitEvent;
HANDLE twxSyncSendThreadWaitEvent;
HANDLE buildTWXDataMutex;
int TwxSyncDepth=0;
char *TwxSyncXMLTreeTag[RECURSIVE_TAGS];
long twxSyncHandle =0;
long twxVersionSupported=1;
unsigned threadId=0;
long hTWXSync=0;
XML_Parser parser=NULL;
int loginAgain=0;
int twxThreadStarted=0;
int twxSyncPacketsSent=0;
int twxSyncPacketsReceived=0;

int resultCode=0;
char resultText[BUFFSIZE];
char  sessionId[128]="-1";
char gameId[128];

void ErrorDataCallback(long tTWXSync, int errorCode, const char *msg, int len)
{
	char buffer[1024];
	sprintf (buffer, "Error code[%d]: %s", errorCode, msg);
#ifdef ATTAC_CODE
		attacScriptMessage("TWXSyncError", buffer, ANSI_WHITE, BLACK, TRUE);
#else
		printf ("%s\n", buffer);
#endif

}

void IncomingDataCallback(long tTWXSync, const char *data, int len)
{
	char buffer[SOCKET_BUFFER];
	sprintf (buffer, "Data received: %s - size %d", data, len);
#ifdef ATTAC_CODE
		attacScriptMessage("TWXSyncData", buffer, ANSI_WHITE, BLACK, TRUE);
#else
		printf ("%s\n", buffer);
#endif

}

void OutgoingDataCallback(long tTWXSync, const char *data, int len)
{


}

void NewConnectionCallback(long tTWXSync, const char *host, int port)
{
	char buffer[256];
	sprintf (buffer, "Connection from %s at port %d was accepted", host, port);
#ifdef ATTAC_CODE
		attacScriptMessage("TWXSync", buffer, ANSI_WHITE, BLACK, TRUE);
#else
		printf ("%s\n", buffer);
#endif
}

unsigned __stdcall twxSyncSendThreadProc(void *dummy) {
	MSG msg;
	char *dataFromServer=NULL;
	char *returnData=NULL;
	long result=0;



	// This is a do-nothing call which is only here because the system will
	// not create message queues UNTIL the thread makes it first USER or GDI
	// call.  So this call forces the system to make message queues.
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	//ok, the thread has initialized, we have set up or message queue.. lets go
	SetEvent(twxSyncSendThreadWaitEvent);

	// Now keep retrieving messages and write out the wParams of WM_USER
	// messages.
	while (1) {

		if (GetMessage(&msg, 0, 0, 0) == -1) {
			attacErrorMessage("Fatal Error");
			break;
		}
		if (msg.message == WM_QUIT) {
			break;
		}
		else if (msg.message == WM_USER+10) {
			char* data = (char*) msg.wParam;
/*			FILE *fp;

			fp = fopen (" test.xml","w");
			fprintf (fp, "%s", data);
			fclose (fp);
*/
			result = TWC_TWXSync_Send(hTWXSync, data, strlen(data));
			//attacErrorMessage("send data");
			twxSyncPacketsSent++;
			update_status_bar();
			#ifdef P_FREE
				pfree(&data);
			#else
				free(data);
			#endif	
		}
	
	}
	return 0;

}

void twxSyncThreadProc(void *dummy) {
	char *dataFromServer=NULL;
	int result=0;
	char * returnData=NULL;
	char tmp[256];
	char buffer[1024];

	strcpy (helper, "");
	strcpy (user, "");
	strcpy (someoneElseLoginId, "");

	//create game
//	test_twc();

	//make sure the send thread starts up before we continue.
	twxSyncSendThreadWaitEvent = CreateEvent(0, TRUE, FALSE, 0);
	
	if(_beginthreadex(NULL, 0, twxSyncSendThreadProc, NULL, 0, &threadId )==-1)
	{
		printf("Failed to create thread");
		return;
	}
	
	//lets wait until our send thread is fully operational before we continue
	WaitForSingleObject(twxSyncSendThreadWaitEvent, INFINITE);

	twxThreadStarted=1;

	//lets be sure our thread starts up... so we slow ourselves down by 1 second
	Sleep(1000);

	srand( (unsigned)time( NULL ) );
	
	if (parser==NULL) {
		//make sure that we are first ones starting the parser
		parser = XML_ParserCreate(NULL);
		
		XML_SetElementHandler(parser, twxSyncStartElement, twxSyncEndElement);
		XML_SetCharacterDataHandler(parser, twxSyncCharacterData);
	}

	twxSyncWaitEvent = CreateEvent(NULL, 0, 0, NULL);

	//loop to try to keep logging into the server
	while (1) {
		char *dataToSendToServerLogin=NULL;
		char *dataToSendToServer=NULL;
		char *fileData=NULL;
		FILE *importData;
		char * unescapedName = NULL;
		char * escapedName = NULL;


		//get GUID to use as identifier
		//try and log into the server
		//build the xml for the login
		sprintf (buffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		dataToSendToServerLogin = appendData(dataToSendToServerLogin, buffer);

		unescapedName = strdup (players[0].name);
		escapedName = escapeData(unescapedName);
		
		sprintf(buffer, "<TWXSync version=\"%d\">\n<Login id=\"%s\" user=\"%s\" password=\"%s\" helper=\"ATTAC %s\"/>\n </TWXSync>", twxVersionSupported, header.TWXSyncSession, escapedName, header.TWXSyncPassword, ver);
		dataToSendToServerLogin = appendData(dataToSendToServerLogin, buffer);
		//dll must free this memory since it allocated it.

		pfree((void**) &escapedName);

		//create the TWX Sync session
		hTWXSync = TWC_CreateTWXSyncSession();
		//initialize the callbacks
		result = TWC_TWXSync_Init(hTWXSync, &IncomingDataCallback, &OutgoingDataCallback, &ErrorDataCallback);
		//connect to the server
		result = TWC_TWXSync_Connect(hTWXSync, header.TWXSyncServer, (int) header.TWXSyncPort);

		//if we connected ....
		if (result==0) {
			//send the login information. contains loginId and password
			result = TWC_TWXSync_Send(hTWXSync, dataToSendToServerLogin, strlen(dataToSendToServerLogin));
			//data is malloced in dll
			//receive the acknowledgement for the login
			result = TWC_TWXSync_ReceiveAllDataWrapper(hTWXSync, &dataFromServer);

			//if we got data
			if (result>0) {
				//parse the data from the login
				returnData = parseXMLData(dataFromServer, parser);
				//must let the DLL free the memory since the DLL allocated it.
				//we use this method since the DLL can be called from C or C++ programs
				//and not all programs calling it will use MFC
				TWC_TWXSync_FreeMem(&dataFromServer);
			} else {
				TWC_TWXSync_FreeMem(&dataFromServer);
				goto skip_to_login_again;
			}

			//start printing data for user
			sprintf (tmp, "sessionId: %s; resultCode: %d; resultText: %s", sessionId, resultCode, resultText);

			//if there was an error
			if (returnData != NULL && strcmp(returnData, "")!=0) {
				//print on error
				#ifdef ATTAC_CODE
				attacErrorMessage(tmp);
				#else
				printf ("%s\n", tmp);
				#endif
			} else	{
				if (strcmp (sessionId,"-1")!=0){
					sprintf (buffer, "Successful Connection to TWX Sync server '%s:%d' established", header.TWXSyncServer, header.TWXSyncPort);
					#ifdef ATTAC_CODE
					attacScriptMessage("TWXSync", buffer, ANSI_WHITE, BLACK, TRUE);
					#else
					printf ("%s\n", buffer);
					#endif
				} else {
					sprintf (buffer, "Error Code: [%d] %s -- Error connecting to TWX Sync server '%s:%d'", resultCode, resultText, header.TWXSyncServer, header.TWXSyncPort);
					#ifdef ATTAC_CODE
					attacScriptMessage("TWXSync", buffer, ANSI_WHITE, BLACK, TRUE);
					#else
					printf ("%s\n", buffer);
					#endif
				}
			}
			//end printing data for user

			//free data from xml parsing
			#ifdef P_FREE
				pfree(&returnData);
			#else
				free(returnData);
			#endif	

			//no need to log in again
			loginAgain=0;

			//if we connected successfully
			if (strcmp(sessionId, "-1") != 0) {
				//send all the stuff since the last connection.
				//instead of mallocing a huge amount of memory, we stream it to a file.
				//and then send the file over the network stream
				dataToSendToServer = sendAllNewTWXSyncData();

				//send updates
				result = TWC_TWXSync_Send(hTWXSync, dataToSendToServer, strlen(dataToSendToServer));

				//free memory
				#ifdef P_FREE
					pfree(&dataToSendToServer);
				#else
					free(dataToSendToServer);
				#endif	


				if (result<=0)
					goto skip_to_login_again;
				//receive data responses
 				result = TWC_TWXSync_ReceiveAllDataWrapper(hTWXSync, &dataFromServer);

				//parse results
				if (result>0) {
					returnData = parseXMLData(dataFromServer, parser);
					TWC_TWXSync_FreeMem(&dataFromServer);
				}
				else {
					//let the DLL free the memory
					TWC_TWXSync_FreeMem(&dataFromServer);
					goto skip_to_login_again;
				}

				//print debug info
				sprintf (tmp, "sessionId: %ld; resultCode: %d; resultText: %s", sessionId, resultCode, resultText);

				//print any error message we get
				if (returnData != NULL && strcmp(returnData, "")!=0) {
		#ifdef ATTAC_CODE
					attacErrorMessage(tmp);
					attacErrorMessage(returnData);
		#else
					printf ("%s\n", tmp);
					printf ("%s\n", returnData);
		#endif
					#ifdef P_FREE
						pfree(&returnData);
					#else
						free(returnData);
					#endif	
					goto skip_to_login_again;
				}
				//free data from parsing
				#ifdef P_FREE
					pfree(&returnData);
				#else
					free(returnData);
				#endif	
			}

			{
				char *dataToGetServerData=NULL;

				sprintf (buffer, "<TWXSync version=\"%d\">\n  <Session id=\"%s\"/>\n  <Request type=\"TWXDataRequest\" begin=\"%ld\" end=\"%ld\"/>\n</TWXSync>\n", twxVersionSupported, header.TWXSyncSession, header.lastSyncTimeWithServer, ATTAC_Time());

				dataToGetServerData = (char *)strdup(buffer);
				result = TWC_TWXSync_Send(hTWXSync, dataToGetServerData, strlen(dataToGetServerData));

				//free memory
				#ifdef P_FREE
					pfree(&dataToGetServerData);
				#else
					free(dataToGetServerData);
				#endif	

				//receive data responses
 				result = TWC_TWXSync_ReceiveAllDataWrapper(hTWXSync, &dataFromServer);

				sprintf(tmp, "tmp_twxSync%d.xml", rand());
				importData = fopen (tmp, "w");
				if (importData != NULL) {
					fprintf (importData, "%s", dataFromServer);
					fclose(importData);
					//import the data file
					import_twxv2(tmp, TWXSYNC_XML_IMPORT_OFFSET);
					unlink(tmp);	//delete file
				}

				//import xml
				TWC_TWXSync_FreeMem(&dataFromServer);
			}
			//now we loop and wait to syncronize data from the server
			//we do a full sync every 5 minutes so make sure that the server is fully
			//up to date.
			while (1) {
				//receive data responses
 				result = TWC_TWXSync_ReceiveAllDataWrapper(hTWXSync, &dataFromServer);

				/*
				{
					FILE *fp2;
					if (dataFromServer!=NULL) {
						sprintf(tmp, "tmp_twxSyncRec%d.xml", rand());
						fp2 = fopen (tmp, "w");
						if (fp2 != NULL) {
							fprintf (fp2, "%s", dataFromServer);
							fclose(fp2);
						}
					}
				}
				*/				

				//attacErrorMessage("receive data");
				
				//if we are forced to log back in
				if (loginAgain) {
					TWC_TWXSync_FreeMem(&dataFromServer);
					goto skip_to_login_again;
				}

				//parse results
				if (result>0) {
					returnData = parseXMLData(dataFromServer, parser);
	
					//let the DLL free the memory
					TWC_TWXSync_FreeMem(&dataFromServer);
				} else {		//lost connection
					TWC_TWXSync_FreeMem(&dataFromServer);
					goto skip_to_login_again;
				}

				//print any error message we get
				if (returnData != NULL && strcmp(returnData, "")!=0) {
		#ifdef ATTAC_CODE
					attacErrorMessage(tmp);
					attacErrorMessage(returnData);
		#else
					printf ("%s\n", tmp); 
					printf ("%s\n", returnData);
		#endif
					sprintf(buffer, "<TWXSync version=\"%d\"> <Session id=\"-1\"/><Result code=\"404\" text=\"Not Found\"/></TWXSync>", twxVersionSupported);
					//free data from parsing
					#ifdef P_FREE
						pfree(&returnData);
					#else
						free(returnData);
					#endif	
					goto skip_to_login_again;
				} else {
					sprintf(buffer, "<TWXSync version=\"%d\"> <Session id=\"%ld\"/> <Result code=\"200\" text=\"OK\"/></TWXSync>", twxVersionSupported, sessionId);
					//free data from parsing
					#ifdef P_FREE
						pfree(&returnData);
					#else
						free(returnData);
					#endif	
				}

				//update the last time we sync'ed with the server
				header.lastSyncTimeWithServer = ATTAC_Time();

				//send acknowledgement
	//			result = TWC_TWXSync_Send(hTWXSync, buffer, strlen(buffer));
	//			if (result==-1)		//lost connection
	//				goto skip_to_login_again;
	//			attacErrorMessage("send ack");

			}
		} else {
			TWC_TWXSync_Close(hTWXSync);
			TWC_DeleteTWXSyncSession(hTWXSync);
			hTWXSync=-1;
		}

		skip_to_login_again:

		if (sessionId != NULL) {
			strcpy(sessionId, "-1");
		}
		//only display if there was an actual connection
		if (hTWXSync>0) {
			sprintf (buffer, "Connection Closed to TWX Sync server '%s:%d'", header.TWXSyncServer, header.TWXSyncPort);
			#ifdef ATTAC_CODE
				attacScriptMessage("TWXSync", buffer, ANSI_WHITE, BLACK, TRUE);
			#else
				printf ("%s\n", buffer);
			#endif
		}

		if (hTWXSync!=-1) {
			TWC_TWXSync_Close(hTWXSync);
			TWC_DeleteTWXSyncSession(hTWXSync);
			hTWXSync=-1;
		}
		
		hTWXSync=0;

		//free data that we logged in with
		#ifdef P_FREE
			pfree(&dataToSendToServerLogin);
		#else
			free(dataToSendToServerLogin);
		#endif	

		//wait 5 minutes before attempting to login again
		WaitForSingleObject(twxSyncWaitEvent, INFINITE);
	}

	//free expat parser
	XML_ParserFree(parser);

	//close connection to server
	TWC_TWXSync_Close(hTWXSync);
	TWC_DeleteTWXSyncSession(hTWXSync);

	_endthread();

	//free memory for twx sync object
}

char * getFile (char* file)
{
	char *fileData;
	int existsFiles;
	FILE *fp;
	struct stat buf;
	long fileSize=0;
	int numread=0;

	fp = fopen (file, "rb");
	if (fp==NULL)
	{
		printf ("Error opening file %s\n", file);
		fileData = (char *) strdup (" ");
		return(fileData);
	}

	existsFiles = stat(file, &buf);
	fileSize = buf.st_size;
	fileData = (char *) calloc (fileSize+1, sizeof(char));
	
	fseek(fp, 0L, SEEK_SET);
	numread = fread (fileData, fileSize, 1, fp);	
	fileData[fileSize]=0;
	fclose (fp);

	return (fileData);
}

//i free old data here. caller must free newData
char * appendData(char * oldData, char *newData) {
	char *returnData=NULL;
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
		#ifdef P_FREE
			pfree(&oldData);
		#else
			free(oldData);
		#endif	
	}

	return (returnData);
}


 // handle of CWnd that called SetTimer
 // WM_TIMER
 // timer identification
 // system time
void CALLBACK twxSyncTimerProc(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime )
{
	//pulse the event every 5 minutes
	PulseEvent (twxSyncWaitEvent);
}

char* parseXMLData(char *data, XML_Parser parser) {
	char buffer[MAX_LENGTH];
	int done=0;
	
	if (data != NULL) {
		XML_ParserReset(parser, NULL);

		XML_SetElementHandler(parser, twxSyncStartElement, twxSyncEndElement);
		XML_SetCharacterDataHandler(parser, twxSyncCharacterData);
		
		twxSyncPacketsReceived++;
		update_status_bar();
		if (XML_Parse(parser, data, strlen(data), done) == XML_STATUS_ERROR) {
			sprintf(buffer, "Parse error at line %d:\n%s\n",
							XML_GetCurrentLineNumber(parser),
							XML_ErrorString(XML_GetErrorCode(parser)));
			return ((char *) strdup(buffer));
		}
	}
	return ((char *) strdup(""));
}



void test_twc() {
	long hTWXSync;
	char buffer[SOCKET_BUFFER];
	char *dataFromServer=NULL;
	int result;
	strcpy (buffer, "<TWXSync version=\"1\">\n  <Session id=\"0\"/>\n  <Create type=\"Game\">\n    <Game Name=\"attacGame\">\n      <Sectors>5000</Sectors>\n    </Game>\n  </Create>\n</TWXSync>\n");


//  hTWXSync = TWC_CreateTWXSyncSession();
//  TWC_TWXSync_Init(hTWXSync, &IncomingDataCallback, &ErrorDataCallback);
//  TWC_TWXSync_Listen(hTWXSync, &NewConnectionCallback, 3000);
//  TWC_DeleteTWXSyncSession(hTWXSync);

	hTWXSync = TWC_CreateTWXSyncSession();
	TWC_TWXSync_Init(hTWXSync, &IncomingDataCallback, &OutgoingDataCallback, &ErrorDataCallback);
	TWC_TWXSync_Connect(hTWXSync, "localhost", 7777);
	TWC_TWXSync_Send(hTWXSync, buffer, sizeof(buffer)-1);
	result = TWC_TWXSync_ReceiveAllDataWrapper(hTWXSync, &dataFromServer);

	TWC_TWXSync_FreeMem(&dataFromServer);

	//let the DLL free the memory
	TWC_TWXSync_FreeMem(&dataFromServer);
	buffer[strlen(buffer)]=0;
	attacErrorMessage(buffer);

	TWC_TWXSync_Close(hTWXSync);
	TWC_DeleteTWXSyncSession(hTWXSync);

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
	char* newStr = strdup (str);

	i = Instr(1, newStr, "&lt;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&lt;", "<");
			i = Instr(1, newStr, "&lt;");
		} 
	}

	i = Instr(1, newStr, "&gt;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&gt;", ">");
			i = Instr(1, newStr, "&gt;");
		} 
	}

	i = Instr(1, newStr, "&amp;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&amp;", "&");
			i = Instr(1, newStr, "&amp;");
		} 
	}

	i = Instr(1, newStr, "&apos;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&apos;", "\'");
			i = Instr(1, newStr, "&apos;");
		} 
	}

	i = Instr(1, newStr, "&quot;");
	if (i>0) {
		while (i>0) {
			newStr = replace (newStr, "&quot;", "\"");
			i = Instr(1, newStr, "&quot;");
		} 
	}


	pfree ((void **) &str);
	return(newStr);
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

void XMLCALL twxSyncStartElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	char * escapedName = NULL;
	char * unescapedName = NULL;
  TwxSyncXMLTreeTag[TwxSyncDepth] = (char *) strdup (name);

  while (*atts) {
	if (strcmp (TwxSyncXMLTreeTag[TwxSyncDepth],"Result")==0 && strcmp (*atts,"text")==0)
		strcpy(resultText,(*(atts+1)));
	else if (strcmp (TwxSyncXMLTreeTag[TwxSyncDepth],"Result")==0 && strcmp (*atts,"code")==0)
		resultCode = atol((*(atts+1)));
	else if (strcmp (TwxSyncXMLTreeTag[TwxSyncDepth],"Session")==0 && strcmp (*atts,"id")==0)
		strcpy(sessionId,(*(atts+1)));
	else if (strcmp (TwxSyncXMLTreeTag[TwxSyncDepth],"Game")==0 && strcmp (*atts,"id")==0)
		strcpy(gameId,(*(atts+1)));

	//for twxv2 import data
	if (strcmp (*atts,"nr")==0)
		twxSectorNumber = atol((*(atts+1)));
	else if (strcmp (*atts,"updated")==0)
		lastUpdated = atol((*(atts+1)));
	else if (strcmp (*atts,"discovered")==0)
		lastUpdated = atol((*(atts+1)));
	else if (strcmp (*atts,"name")==0) {
		escapedName = strdup ((char *) (*(atts+1)));
		unescapedName = unescapeData(escapedName);
		strcpy (traderName, unescapedName);
		pfree((void**) &unescapedName);
		getTradeNumber (traderName);
	}
	else if (strcmp (*atts,"ships")==0)
		strcpy (ships, (char *) (*(atts+1)));
	else if (strcmp (TwxSyncXMLTreeTag[TwxSyncDepth],"Login")==0 && strcmp (*atts,"id")==0) {
		strcpy (someoneElseLoginId, (char *) (*(atts+1)));
	}
	else if (strcmp (TwxSyncXMLTreeTag[TwxSyncDepth],"Login")==0 && strcmp (*atts,"user")==0) {
		strcpy (user, (char *) (*(atts+1)));
		if (strcmp (user, "") != 0 && strcmp (helper, "") != 0 && stricmp(someoneElseLoginId, header.TWXSyncSession)==0) {
			char buf[256];
			sprintf (buf, "User %s has connected to current TWXSync game with %s", user, helper);
			attacInfoMessage(buf);
			strcpy (user, "");
			strcpy (helper, "");
		}
	}
	else if (strcmp (TwxSyncXMLTreeTag[TwxSyncDepth],"Login")==0 && strcmp (*atts,"helper")==0) {
		strcpy (helper, (char *) (*(atts+1)));
		if (strcmp (user, "") != 0 && strcmp (helper, "") != 0 && stricmp(someoneElseLoginId, header.TWXSyncSession)==0) {
			char buf[256];
			sprintf (buf, "User %s has connected to current TWXSync game with %s", user, helper);
			attacInfoMessage(buf);
			strcpy (user, "");
			strcpy (helper, "");
		}
	}
	else if (strcmp (*atts,"corp")==0)
		if (strcmp((*(atts+1)), "true")==0 || strcmp((*(atts+1)), "True")==0)
			corp= 1;
		else
			corp=0;
	else if (strcmp (*atts,"finished")==0)
		finished= atol((*(atts+1)));
	else if (strcmp (*atts,"hops")==0)
		hops= atol((*(atts+1)));
	else if (strcmp (*atts,"version")==0)
		twxVersion= atol((*(atts+1)));
	else if (strcmp (*atts,"trader")==0)
		strcpy (trader, (char *) (*(atts+1)));

	//end twxv2 import data
    
	atts++;
  }


  TwxSyncDepth++;
}


void XMLCALL twxSyncEndElement(void *userData, const XML_Char *name)
{

	TwxSyncDepth--;
  	#ifdef P_FREE
		pfree(&TwxSyncXMLTreeTag[TwxSyncDepth]);
	#else
		free(TwxSyncXMLTreeTag[TwxSyncDepth]);
	#endif	
}


void XMLCALL twxSyncCharacterData(void *userData, const XML_Char *s, int len)
{
	int i=0;
	char buffer[MAX_LENGTH];
	long tmpLong=0;
	int tmpInt=0, tmpInt2=0;

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
		buffer[i++] = *s;
		break;
		}
	}
	buffer[i++]=0;

	if (TwxSyncDepth>0 && strcmp (TwxSyncXMLTreeTag[0],"TWXSync")==0 && hasCharacters(buffer)) {
		if (TwxSyncDepth>1 && strcmp (TwxSyncXMLTreeTag[1],"GameIndex")==0 && hasCharacters(buffer)) {
			if (TwxSyncDepth>2 && strcmp (TwxSyncXMLTreeTag[2],"Game")==0 && hasCharacters(buffer)) {
				if (TwxSyncDepth>3 && strcmp (TwxSyncXMLTreeTag[3],"Name")==0 && hasCharacters(buffer)) {
					HWND hlist = GetDlgItem(showtwxsync, IDC_TWXSYNC_GAMELIST);
					if (hlist != NULL) {
						listview_insert_twxsync(hlist, 0, buffer, gameId);
					}
				}
			}
		}
		if (TwxSyncDepth>1 && strcmp (TwxSyncXMLTreeTag[1],"Update")==0) {
			parseXMLImportData(TWXSYNC_XML_OFFSET, TwxSyncDepth, TwxSyncXMLTreeTag, buffer);
			writeSetReady();
		}
	}
}

int postSendMessageToTWXSync(unsigned t_threadId, char* msg)
{
	char buffer[256];
	int resultPost=0;
	if (strcmp(sessionId, "-1")!=0) {
		resultPost = PostThreadMessage(t_threadId, WM_USER+10, (WPARAM) msg, (LPARAM) 0);

		if (resultPost == 0) {
			int error = GetLastError();
			sprintf(buffer, "Couldn't post. Error Number: %d", error);
			attacErrorMessage(buffer);
		}
	} else {
		if (msg != NULL) {
			#if P_FREE
				pfree(&msg);
			#else
				free(msg);
			#endif
		}
	}
	return resultPost;
}

char* buildSendTWXData(long exportNumber, int dataType) {
	char *returnValue=NULL;
	char buffer[1024];
	int i=0, j=0;
	char * unescapedName = NULL;
	char * escapedName = NULL;


	sprintf (buffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	returnValue = appendData(returnValue, buffer);
	sprintf (buffer, "<TWXSync version=\"%ld\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" >\n", twxVersionSupported);
	returnValue = appendData(returnValue, buffer);

	sprintf (buffer, "<Session id=\"%s\"/>\n", sessionId);
	returnValue = appendData(returnValue, buffer);
	returnValue = appendData(returnValue, "<Update>\n");

	//data here
	if (dataType==SECTOR_DATA && isValidSector(exportNumber+1)) {
//		sprintf (buffer, "<TWX version=\"2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.tw-attac.com/TWX.xsd\">\n");
//		returnValue = appendData(returnValue, buffer);
		sprintf (buffer, "  <SectorData>\n");
		returnValue = appendData(returnValue, buffer);
		sprintf (buffer, "  <Sector nr=\"%ld\" updated=\"%ld\">\n", exportNumber+1, sectors[exportNumber].sector_update);
		returnValue = appendData(returnValue, buffer);
//			if (strcmp (sectors[exportNumber].space, "")==0) {
//			    sprintf (buffer, "	  <Space>%ld</Space>\n", sectors[exportNumber].space);
//				returnValue = appendData(returnValue, buffer);
//		}
//			if (strcmp (sectors[exportNumber].beacon, "")==0) {
//			    sprintf (buffer, "	  <Beacon>%ld</Beacon>\n", sectors[exportNumber].space);
//				returnValue = appendData(returnValue, buffer);
//		}


		//sector warps
		sprintf (buffer, "	  <Warps>\n");
		returnValue = appendData(returnValue, buffer);
		for (j=0; j<sectors[exportNumber].warps ; j++) {
			if (isValidSector(sectors[exportNumber].warp_sect[j])) {
				sprintf (buffer, "	    <Warp>%ld</Warp>\n", sectors[exportNumber].warp_sect[j]);
				returnValue = appendData(returnValue, buffer);
			}
		}
		sprintf (buffer, "	  </Warps>\n");
		returnValue = appendData(returnValue, buffer);
		
		//fighters data
		if (sectors[exportNumber].ftrs>0) {
			sprintf (buffer, "	  <Fighters>\n");
			returnValue = appendData(returnValue, buffer);
			sprintf (buffer, "	    <Amount>%ld</Amount>\n", sectors[exportNumber].ftrs);
			returnValue = appendData(returnValue, buffer);
			if (sectors[exportNumber].ftr_type>=0) {
				if (sectors[exportNumber].ftr_type==0) {
					sprintf (buffer, "	    <Type>Mercenary</Type>\n");
					returnValue = appendData(returnValue, buffer);
				}
				else if (sectors[exportNumber].ftr_type==1) {
					sprintf (buffer, "	    <Type>Toll</Type>\n");
					returnValue = appendData(returnValue, buffer);
				}
				else if (sectors[exportNumber].ftr_type==2) {
					sprintf (buffer, "	    <Type>Offensive</Type>\n");
					returnValue = appendData(returnValue, buffer);
				}
				else if (sectors[exportNumber].ftr_type==3) {
					sprintf (buffer, "	    <Type>Defensive</Type>\n");
					returnValue = appendData(returnValue, buffer);
				}
			}
			if (sectors[exportNumber].ftr_owner>0) {
				if (sectors[exportNumber].ftr_owner>CORP_CONSTANT) {
					sprintf (buffer, "	    <Owner corp=\"true\">%d</Owner>\n", sectors[exportNumber].ftr_owner-CORP_CONSTANT);
					returnValue = appendData(returnValue, buffer);
				}
				else {
					if (sectors[exportNumber].ftr_owner>=0 && sectors[exportNumber].ftr_owner<MAX_PLAYERS) {
						sprintf (buffer, "	    <Owner corp=\"false\">%s</Owner>\n", players[sectors[exportNumber].ftr_owner].name);
						returnValue = appendData(returnValue, buffer);
					}
				}
			}
			sprintf (buffer, "	  </Fighters>\n");
			returnValue = appendData(returnValue, buffer);
		}
		//armid mines
		if (sectors[exportNumber].armids>0) {
			sprintf (buffer, "	  <ArmidMines>\n");
			returnValue = appendData(returnValue, buffer);
			sprintf (buffer, "	    <Amount>%ld</Amount>\n", sectors[exportNumber].armids);
			returnValue = appendData(returnValue, buffer);
			if (sectors[exportNumber].armid_owner>0) {
				if (sectors[exportNumber].armid_owner>CORP_CONSTANT) {
					sprintf (buffer, "	    <Owner corp=\"true\">%d</Owner>\n", sectors[exportNumber].armid_owner-CORP_CONSTANT);
					returnValue = appendData(returnValue, buffer);
				}
				else {
					if (sectors[exportNumber].armid_owner>=0 && sectors[exportNumber].armid_owner<MAX_PLAYERS) {
						sprintf (buffer, "	    <Owner corp=\"false\">%s</Owner>\n", players[sectors[exportNumber].armid_owner].name);
						returnValue = appendData(returnValue, buffer);
					}
				}

			}
			sprintf (buffer, "	  </ArmidMines>\n");
			returnValue = appendData(returnValue, buffer);
		}

		//limpit mines
		if (sectors[exportNumber].limpets>0) {
			sprintf (buffer, "	  <LimpetMines>\n");
			returnValue = appendData(returnValue, buffer);
			sprintf (buffer, "	    <Amount>%ld</Amount>\n", sectors[exportNumber].limpets);
			returnValue = appendData(returnValue, buffer);
			if (sectors[exportNumber].limpet_owner>0) {
				if (sectors[exportNumber].limpet_owner>CORP_CONSTANT) {
					sprintf (buffer, "	    <Owner corp=\"true\">%d</Owner>\n", sectors[exportNumber].limpet_owner-CORP_CONSTANT);
					returnValue = appendData(returnValue, buffer);
				}
				else {
					if (sectors[exportNumber].limpet_owner>=0 && sectors[exportNumber].limpet_owner<MAX_PLAYERS) {
						sprintf (buffer, "	    <Owner corp=\"false\">%s</Owner>\n", players[sectors[exportNumber].limpet_owner].name);
						returnValue = appendData(returnValue, buffer);
					}
				}
			}
			sprintf (buffer, "	  </LimpetMines>\n");
			returnValue = appendData(returnValue, buffer);
		}

		//density
		if (sectors[exportNumber].density>0) {
			sprintf (buffer, "	  <Density>%ld</Density>\n", sectors[exportNumber].density);
			returnValue = appendData(returnValue, buffer);
		}

		//last bust
		if (sectors[exportNumber].lastBusted>0) {
			sprintf (buffer, "	  <LastBust trader=\"%s\">%ld</LastBust>\n", players[0].name, sectors[exportNumber].lastBusted);
			returnValue = appendData(returnValue, buffer);
		}
		
		//Anomaly
		if (sectors[exportNumber].anom>=0) {
			if (sectors[exportNumber].anom==1) {
				sprintf (buffer, "	  <Anomaly>true</Anomaly>\n");
				returnValue = appendData(returnValue, buffer);
			}
			else {
				sprintf (buffer, "	  <Anomaly>false</Anomaly>\n");
				returnValue = appendData(returnValue, buffer);
			}
		}
		//navhaz
		if (sectors[exportNumber].navhaz>0) {
			sprintf (buffer, "	  <NavHaz>%d</NavHaz>\n", sectors[exportNumber].navhaz);
			returnValue = appendData(returnValue, buffer);
		}

		sprintf (buffer, "  </Sector>\n");
		returnValue = appendData(returnValue, buffer);
    
		sprintf (buffer, "  </SectorData>\n");
		returnValue = appendData(returnValue, buffer);

//		sprintf (buffer, "</TWX>\n");
//		returnValue = appendData(returnValue, buffer);
	}
	else if (dataType==PORT_DATA && isValidSector(exportNumber+1)) {
//		sprintf (buffer, "<TWX version=\"2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.tw-attac.com/TWX.xsd\">\n");
//		returnValue = appendData(returnValue, buffer);

		//port data
		if ((sectors[exportNumber].info>0 && sectors[exportNumber].info < 10) || sectors[exportNumber].info > UNDER_CONSTRUCTION) {
			sprintf (buffer, "  <SectorData>\n");
			returnValue = appendData(returnValue, buffer);
			
			sprintf (buffer, "  <Sector nr=\"%ld\" updated=\"%ld\">\n", exportNumber+1, sectors[exportNumber].sector_update);
			returnValue = appendData(returnValue, buffer);

			sprintf (buffer, "	  <Port updated=\"%ld\">\n", sectors[exportNumber].port_update);
			returnValue = appendData(returnValue, buffer);
			//sprintf (buffer, "	    <Name>%s</Name>\n", sectors[exportNumber].portname);
			//returnValue = appendData(returnValue, buffer);
			sprintf (buffer, "	    <Class>%d</Class>\n", sectors[exportNumber].info % UNDER_CONSTRUCTION);
			returnValue = appendData(returnValue, buffer);

			if (sectors[exportNumber].info > UNDER_CONSTRUCTION) {
				sprintf (buffer, "	    <UnderConstruction>true</UnderConstruction>\n");
				returnValue = appendData(returnValue, buffer);
			}

			if (sectors[exportNumber].info ==12) {
				sprintf (buffer, "	    <Destroyed>true</Destroyed>\n");
				returnValue = appendData(returnValue, buffer);
			}

			//fuel ore
			if (sectors[exportNumber].port_amt[0]>=0 || sectors[exportNumber].port_per[0]>=0) {
				sprintf (buffer, "	    <FuelOre>\n");
				returnValue = appendData(returnValue, buffer);
	
				if (sectors[exportNumber].port_amt[0]>=0) {
					sprintf (buffer, "	      <Amount>%ld</Amount>\n", sectors[exportNumber].port_amt[0]);
					returnValue = appendData(returnValue, buffer);
				}
				if (sectors[exportNumber].port_per[0]>=0) {
					sprintf (buffer, "	      <Percentage>%d</Percentage>\n", sectors[exportNumber].port_per[0]);
					returnValue = appendData(returnValue, buffer);
				}
				sprintf (buffer, "	    </FuelOre>\n");
				returnValue = appendData(returnValue, buffer);
			}
			
			//organics
			if (sectors[exportNumber].port_amt[1]>=0 || sectors[exportNumber].port_per[1]>=0) {
				sprintf (buffer, "	    <Organics>\n");
				returnValue = appendData(returnValue, buffer);
				if (sectors[exportNumber].port_amt[1]>=0) {
					sprintf (buffer, "	      <Amount>%ld</Amount>\n", sectors[exportNumber].port_amt[1]);
					returnValue = appendData(returnValue, buffer);
				}
				if (sectors[exportNumber].port_per[1]>=0) {
					sprintf (buffer, "	      <Percentage>%d</Percentage>\n", sectors[exportNumber].port_per[1]);
					returnValue = appendData(returnValue, buffer);
				}
				sprintf (buffer, "	    </Organics>\n");
				returnValue = appendData(returnValue, buffer);
			}

			//equipment
			if (sectors[exportNumber].port_amt[2]>=0 || sectors[exportNumber].port_per[2]>=0) {
				sprintf (buffer, "	    <Equipment>\n");
				returnValue = appendData(returnValue, buffer);
				if (sectors[exportNumber].port_amt[2]>=0) {
					sprintf (buffer, "	      <Amount>%ld</Amount>\n", sectors[exportNumber].port_amt[2]);
					returnValue = appendData(returnValue, buffer);
				}
				if (sectors[exportNumber].port_per[2]>=0) {
					sprintf (buffer, "	      <Percentage>%d</Percentage>\n", sectors[exportNumber].port_per[2]);
					returnValue = appendData(returnValue, buffer);
				}
				sprintf (buffer, "	    </Equipment>\n");
				returnValue = appendData(returnValue, buffer);
			}
			sprintf (buffer, "	  </Port>\n");
			returnValue = appendData(returnValue, buffer);

			sprintf (buffer, "  </Sector>\n");
			returnValue = appendData(returnValue, buffer);
    
			sprintf (buffer, "  </SectorData>\n");
			returnValue = appendData(returnValue, buffer);

		}

//		sprintf (buffer, "</TWX>\n");
//		returnValue = appendData(returnValue, buffer);
	}
	else if (dataType==TRADER_DATA) {
//		sprintf (buffer, "<TWX version=\"2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.tw-attac.com/TWX.xsd\">\n");
//		returnValue = appendData(returnValue, buffer);

		//trader data
		sprintf (buffer, "  <TraderData>\n");
		returnValue = appendData(returnValue, buffer);

		if (players[exportNumber].autonum >=0) {
			unescapedName = strdup ((char *) players[exportNumber].name);
			escapedName = escapeData(unescapedName);

			sprintf (buffer, "  <Trader name=\"%s\" updated=\"%ld\">\n", escapedName, players[exportNumber].last_update);
			returnValue = appendData(returnValue, buffer);

			pfree((void**) &escapedName);
			
			if (players[exportNumber].experience>0) {
				sprintf (buffer, "	  <Experience>%ld</Experience>\n", players[exportNumber].experience);
				returnValue = appendData(returnValue, buffer);
			}
			
			if (players[exportNumber].alignment>0) {
				sprintf (buffer, "	  <Alignment>%ld</Alignment>\n", players[exportNumber].alignment);
				returnValue = appendData(returnValue, buffer);
			}
			
			if (players[exportNumber].corpNumber>0) {
				sprintf (buffer, "	  <Corporation>%ld</Corporation>\n", players[exportNumber].corpNumber);
				returnValue = appendData(returnValue, buffer);
			}
			
			//sprintf (buffer, "	  <Rank>%ld</Rank>\n");
			//returnValue = appendData(returnValue, buffer);
			sprintf (buffer, "  </Trader>\n");
			returnValue = appendData(returnValue, buffer);
		}
    
		sprintf (buffer, "  </TraderData>\n");
		returnValue = appendData(returnValue, buffer);

//		sprintf (buffer, "</TWX>\n");
//		returnValue = appendData(returnValue, buffer);
	}
		
	returnValue = appendData(returnValue, "\n</Update>\n");
	returnValue = appendData(returnValue, "</TWXSync>\n");
	return (returnValue);
}

void GetTWXGamesThreadProc (void *arg) {
	char buffer[MAX_LENGTH];
	char *dataFromServer;
	char *returnData;

	long hTWXSync = TWC_CreateTWXSyncSession();
	int result=-1;
	strcpy (buffer, "<TWXSync version=\"1\">\n  <Session id=\"0\"/>\n  <Request type=\"GameIndex\"/>\n</TWXSync>\n");

	result = TWC_TWXSync_Init(hTWXSync, &IncomingDataCallback, &OutgoingDataCallback, &ErrorDataCallback);
	if (result!=0) {
		if (showtwxsync != NULL)
			SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Error! Cannot initialize server connection");
		return;
	}
	result = TWC_TWXSync_Connect(hTWXSync, header.TWXSyncServer, header.TWXSyncPort);
	if (result!=0) {
		if (showtwxsync != NULL)
			SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Error! Cannot connect to server");
		return;
	}
	result = TWC_TWXSync_Send(hTWXSync, buffer, sizeof(buffer)-1);
	if (result==0) {
		if (showtwxsync != NULL)
			SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Error! Cannot send data to server");
		return;
	}

	result = TWC_TWXSync_ReceiveAllDataWrapper(hTWXSync, &dataFromServer);
	if (dataFromServer != NULL)
		result= strlen(dataFromServer);
	else
		result=0;
	if (result==0) {
		if (showtwxsync != NULL)
			SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Error! Cannot receive data from server");
		TWC_TWXSync_FreeMem(&dataFromServer);
		return;
	}

	//parse results
	if (result>0) {

		if (parser==NULL) {
			//if parser was not created yet, since we have fully connected and 
			//started the twx sync thread, generate the parser
			parser = XML_ParserCreate(NULL);
			
			XML_SetElementHandler(parser, twxSyncStartElement, twxSyncEndElement);
			XML_SetCharacterDataHandler(parser, twxSyncCharacterData);
		}

		returnData = parseXMLData(dataFromServer, parser);
  		#ifdef P_FREE
			pfree(&returnData);
		#else
			free(returnData);
		#endif	

		//let the DLL free the memory
		TWC_TWXSync_FreeMem(&dataFromServer);

		if (showtwxsync != NULL) {
			
			HWND hlist = GetDlgItem(showtwxsync, IDC_TWXSYNC_GAMELIST);
			int count = ListView_GetItemCount(hlist);
			int i=0;
			char buffer[128];

			for (i=0; i<count; i++) {
				ListView_GetItemText (hlist, i, 1, buffer, sizeof(buffer));
				if (strcmp (buffer, header.TWXSyncSession)==0) {
					SendMessage(hlist,LVM_ENSUREVISIBLE , (WPARAM)i,FALSE); // if item is far, scroll to it
///					ListView_SetItemState(hlist,i, LVIS_SELECTED ,LVIS_SELECTED); // select item
///					ListView_SetItemState(hlist,i, LVIS_FOCUSED ,LVIS_FOCUSED); // optional

					ListView_GetItemText (hlist, i, 0, buffer, sizeof(buffer));
					SetDlgItemText(showtwxsync, IDC_TWXSYNC_CURRGAME, buffer);

					break;
				}
			}
			if (count==0)
				SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Success! No games found on server");
			else
				SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Success! Games on server displayed");
		}
	} else {
		if (showtwxsync != NULL)
			SetDlgItemText(showtwxsync, IDC_TWXSYNC_STATUS, "Error! Cannot display game list");
	}

	TWC_TWXSync_Close(hTWXSync);	

	TWC_DeleteTWXSyncSession(hTWXSync);

	_endthread();
}

long buildSendTWXData_WaitForData(long timeout)
{
	int result;

	if (timeout == 0)
		timeout = 50000;
	result = WaitForSingleObject(buildTWXDataMutex, timeout);

	return result;
}

long buildSendTWXData_ReleaseData()
{
	int result;

	result = ReleaseMutex(buildTWXDataMutex);

	return result;
}

char * sendAllNewTWXSyncData() {
	char *dataToSendToServer=NULL, tmp[256], *filename=NULL, *fileData=NULL;
	char buffer[256];
	int result;

	time_t data;

	srand( (unsigned)time( NULL ) );

	//send all the stuff since the last connection.
	//instead of mallocing a huge amount of memory, we stream it to a file.
	//and then send the file over the network stream
	sprintf (buffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	dataToSendToServer = appendData(dataToSendToServer, buffer);
	sprintf (buffer, "<TWXSync version=\"%ld\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" >\n", twxVersionSupported);
	dataToSendToServer = appendData(dataToSendToServer, buffer);

	sprintf (buffer, "<Session id=\"%s\"/>\n", sessionId);
	dataToSendToServer = appendData(dataToSendToServer, buffer);
	dataToSendToServer = appendData(dataToSendToServer, "<Update>\n");
	/* twx xml v2 here */
	sprintf(tmp, "tmp_twxSync%d.xml", rand());
	result = export_twxv2(tmp, header.lastSyncTimeWithServer, time(&data), 1);
	
	filename =  strdup (tmp);
	fileData = getFile(filename);
  	#ifdef P_FREE
		pfree(&filename);
	#else
		free(filename);
	#endif	
	unlink(tmp);	//delete file

	dataToSendToServer = appendData(dataToSendToServer, fileData);		
  	#ifdef P_FREE
		pfree(&fileData);
	#else
		free(fileData);
	#endif	
	dataToSendToServer = appendData(dataToSendToServer, "\n</Update>\n");
	dataToSendToServer = appendData(dataToSendToServer, "</TWXSync>\n");

	return (dataToSendToServer);
}
/*

  //implementation of pfree  since it is not in this file
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

*/