#ifndef __statusthread_h__
#define __statusthread_h__

#define TIMEOUT_CONSTANT 640
#define TIMEOUT_AMOUNT 5000
#define LIMPIT_COUNT_MAX 100

extern HANDLE  StatusWaitEvent;
extern int StatusWaitCount;
extern char StatusWaitArray[MAX_LENGTH][MAX_AMOUNT];
extern int StatusWaitThreadResult;
extern char StatusLastLine[MAX_LENGTH];
extern int waitForStatusEvent;
extern int statusTest[LIMPIT_COUNT_MAX];
extern int startParsing;
extern HANDLE statusThreadsMutex;

void StatusThreadProc( void *dummy );
long sSCRIPT_WAIT_MUX(long timeout, ...);
long sSCRIPT_WAIT(long timeout, char arg[]);
char* sSCRIPT_LAST_LINE(int deletion);
void DISPLAY_UNIVERSE_SIZE_ERROR(int s, char where[]);
void Update_LastLine_Status(long pos);
char * removeCRs(char *str);
long statusThreads_WaitForData(long timeout);
long statusThreads_ReleaseData();

#endif
