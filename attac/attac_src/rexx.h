#ifndef __rexx_h__
#define __rexx_h__

#define INCL_RXFUNC 1
#define INCL_RXSYSEXIT 1
#define INCL_RXSUBCOM 1
#define INCL_RXARI 1

#include "./3PP/regina/rexxsaa.h"
#include "util.h"

//#define S_IFDIR			0040000     /* directory */
//#define S_IFMT			0170000 
#define MAX_NON_REGISTERED 300

extern int NonRegisteredREXXCount;
extern int REXX_RunningScript;
extern int REXX_LockKeyboard;
extern int ScriptCounter;

extern char LastLine[MAX_LENGTH];

void Update_LastLine(long pos, int called_by_respond);
long Respond ();
void My_Sleep (long time);
char* Translate_String(char *Data, int Long_Return, int For_Wait);
long Instr (long startPos, char stringBeingSearched[] , char stringToFind[] );
char * replaceString(char *realString, char *replaceFrom, char *replaceWith);
char * replaceChars(char *realString, char *replaceFrom);

char* right (char tmp[], int len);
char* left (char tmp[], int len);

// regina implementation 

extern HANDLE Send_Data_Mutex;
extern HANDLE Data_Mutex;
extern HANDLE Wait_Event;
extern HANDLE Wait_Input_Event;
extern long error_code;

extern long Respond_Cnt;
extern char *Respond_To[MAX_LENGTH];
extern char *Respond_With[MAX_LENGTH];
extern long Respond_Start[MAX_AMOUNT];

extern long Still_Waiting;
extern long Script_Running;
extern double REXXtimeout;
extern int encryptedScript;

long ATTACScripting_WaitForZocRespond(long timeout);
long ATTACScripting_ReleaseZocRespond();
void StartRexxThread(void *dummy);
void REXX_KillScript();
void REXX_ProcessInbound (unsigned char c, int logmode);
long ATTACScripting_WaitForSendData(long timeout);
long ATTACScripting_ReleaseSendData();
void REXX_Start(char *FileName, int encrypted, int extraArgs, char args[100][100]);
long ATTACScripting_WaitForData(long timeout);
long ATTACScripting_ReleaseData();
long ATTACScripting_ScriptRunning();
long ATTACScripting_WaitForATTAC(long timeout);
long ATTACScripting_WaitForATTACWithREXXWaiting(long timeout);
void ATTACScripting_SignalATTACComplete();
void ATTACScripting_Cleanup();
long ATTACScripting_StillWaiting();
long ATTACScripting_WaitForMemory(long timeout);
long ATTACScripting_ReleaseMemory();
long ATTAC_Time();
long ATTAC_XOR(unsigned long a, unsigned long b);

LONG APIENTRY ATTACScripting_ExitHandler(LONG exNum, LONG subfun, PEXIT pBlock);
APIRET APIENTRY ATTACScripting_FuncHandler(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_Hook(PRXSTRING command, PUSHORT flags, PRXSTRING returnstring);
APIRET APIENTRY ATTACScripting_ZocSend(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocSendRaw(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocTimeout(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocGetScreen(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_Message(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocBeep(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocCls(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocDelay(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocLockKeyboard(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocMsgBox(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocNotify(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocOs(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocWrite(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocWriteln(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_dosfname(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_dosisdir(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocAsk(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocAskP(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocRequest(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocRequestList(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocClearBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_Get_Sector_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_Get_Port_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_Get_ZTM_Sector_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_UpdateCounter(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_GetTotalSectors(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_GetStardockSector(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_GetBestPairPort(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_GetBestEvilPairPort(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_HasSectorFig(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_HasSectorPlanet(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_SectorFigAmount(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_SectorFigOwner(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_SectorFigType(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_IsSectorBusted(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_IsSectorAvoided(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_IsSectorExplored(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_SetCounterMax(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocWait(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocWaitMux(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocLastLine(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocRespond(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocLogname(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocLogging(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocString(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocWaitForSeq(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocExec(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_Lower(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_Upper(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocGetLine(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_StartBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_StopBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_GetBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_ClearBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocGetFilename(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocReceiveBuf(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocSetGlobalOption(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocGetGlobalOption(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocGetInfo(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ZocListFiles(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
APIRET APIENTRY ATTACScripting_ATTAC_Get_Ally_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr);
// end regina implementation

extern char *rexxAskPrompt;
extern char *rexxAskDefault;
extern char *rexxAskResult;

extern long rexxRequestArgumentCount;
extern char *rexxRequestArguments[MAX_AMOUNT];
extern int rexxRequestList;
extern char *rexxRequestResult;

extern char *ZocWaitResult;

extern char REXX_Buffer[MAX_LENGTH];
extern char REXX_Buffer_RegularCase[MAX_LENGTH];
extern int REXX_Waiting;
extern int REXX_WaitNextLine;
extern char *REXX_Waiting_For[MAX_AMOUNT];
extern long REXX_Waiting_For_Cnt;
extern int ZocLogging;
extern FILE *fpLogFile;
extern char ZocLogName[MAX_LENGTH];
extern char ZocBuffer[MAX_LENGTH];
extern int ZocBuffering;

extern char *ZocReceiveBuf;
extern long ZocReceiveBufMax;
extern long ZocReceiveBufCurr;
extern int ZocBufferStatus;
extern char *ZocReceiveBufExtra;
extern long ZocReceiveBufExtraCurr;
extern int ZocWaitForSeq;


typedef void (* my_Sleep)(long); 
my_Sleep Sleepy;


int getXCursorPos();
int getYCursorPos();

#endif

