#ifndef __writeMemory_h__
#define __writeMemory_h__

#include "util.h"

#define MAX_RESTORE_ATTEMPTS 3

extern int restoreCount;

extern HANDLE WriteMemoryWaitEvent;
extern HANDLE MemoryData_Mutex;
/*
void update_game_statusbar(gboolean clear);
*/
void writeSetReady();
void writeMemory();
int do_writeMemory();
void WriteMemoryThreadProc(void *dummy);
void CALLBACK writeMemoryTimerProc(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );
int restoreDBFromBackup ();

#endif
