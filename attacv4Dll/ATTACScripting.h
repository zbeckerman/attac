ifndef __MMThreads_h__
#define __MMThreads_h__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <process.h>    /* _beginthread, _endthread */


typedef void (__stdcall ATTACCallbackFunc (long command));
long ATTACScripting_Start();
void __stdcall ATTACScripting_Init();

extern ATTACCallbackFunc *ATTAC_Callback;

#define INCL_RXFUNC
#define INCL_RXSYSEXIT
#define INCL_RXSUBCOM
#define INCL_RXARI

#include "rexxsaa.h"

#endif