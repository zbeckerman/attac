#ifndef __parsedatathread_h__
#define __parsedatathread_h__

#define LIMPIT_COUNT_MAX 100
#define MAX_FIGS_TO_ATTACK 50

extern HANDLE tempCurrentSectorMutex;
extern HANDLE REXXThreadsMutex;
extern HANDLE ParseDataWaitEvent;
extern HANDLE parseThreadsMutex;
extern int parseDataWaitCount;
extern char parseDataWaitArray[MAX_LENGTH][MAX_AMOUNT];
extern int parseDataWaitThreadResult;
extern char parseDataLastLine[MAX_LENGTH];
extern int waitForParseDataMutex;
extern int debugMe;
extern int remote;
extern int parserTest[LIMPIT_COUNT_MAX];
extern char data[MAX_LENGTH];
extern char *dataMessage;

void ParseDataThreadProc( void *dummy );
long pSCRIPT_WAIT_MUX(long timeout, ...);
long pSCRIPT_WAIT(long timeout, char arg[]);
char* pSCRIPT_LAST_LINE(int deletion);
void DISPLAY_UNIVERSE_SIZE_ERROR(int s, char where[]);
char * UNCOMMA (char *tmp1);
char * CLEANLINE (char *tmp1);
void Update_LastLine_ParseData(long pos);
void outputData(char str[]);
long Parse_WaitForREXXData(long timeout);
long Parse_ReleaseREXXData();
long parseThreads_WaitForData(long timeout);
long parseThreads_ReleaseData();

#endif
