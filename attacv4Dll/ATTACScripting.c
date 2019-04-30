#include "ATTACScripting.h"

ATTACCallbackFunc *ATTAC_Callback=NULL;

HANDLE Data_Mutex;
HANDLE Wait_Event;

HWND ATTACWindow;
HANDLE hModule;

PRXSTRING return_string;
char *my_return_string=NULL;
PRXSTRING command;
PRXSTRING argv;
long argc;
double timeout;
long error_code;
long Still_Waiting;

long Script_Running;
char *Script_Filename=NULL;
char *Script_Data=NULL;

APIRET APIENTRY ATTACScripting_FuncHandler(PSZ name, ULONG ac, PRXSTRING av, PSZ queuename, PRXSTRING rs);
APIENTRY ATTACScripting_Hook(PRXSTRING command, PUSHORT flags, PRXSTRING returnstring);
long APIENTRY ATTACScripting_ExitHandler(long ExitNumber, long Subfunction, PEXIT ParmBlock);
void ATTACScripting_SetResult(char *result, long error);

int CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM cbf, LPARAM data)
{

	if (msg == WM_CLOSE)
	{
		DestroyWindow(hWnd);
	}
	else if (msg >= WM_USER && msg <= WM_USER + 300)
	{
		((ATTACCallbackFunc *) cbf)(msg - WM_USER);
	}
	return DefWindowProc(hWnd, msg, cbf, data);
}


long ATTACScripting_ScriptRunning()
{
	return Script_Running;
}

HWND CreateCallbackWindow()
{
	char WindowName[80];
	char WindowClass[80];
	HWND WinHandle;
	WNDCLASSEX wndClass;

	sprintf(WindowName, "ATTACWindow");
	sprintf(WindowClass, "ATTACWindow");

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = 0;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hModule;
	wndClass.hIcon = 0;
	wndClass.hCursor = 0;
	wndClass.hbrBackground = 0;
	wndClass.lpszClassName = WindowClass;
	wndClass.lpszMenuName = 0;
	wndClass.hIconSm = 0;

	RegisterClassEx(&wndClass);

	WinHandle = CreateWindowEx(0, WindowClass, WindowName, 0, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		HWND_DESKTOP, 0, hModule, NULL);

	return WinHandle;
}




long ATTACScripting_WaitForData(long timeout)
{
	long result;

	if (timeout == 0)
		timeout = INFINITE;
	result = WaitForSingleObject(Data_Mutex, timeout);

	return result;
}

long ATTACScripting_ReleaseData()
{
	long result;

	result = ReleaseMutex(Data_Mutex);

	return result;
}

void ATTACScripting_Cleanup()
{
	ATTACScripting_ReleaseData();
	CloseHandle(Data_Mutex);
	CloseHandle(Wait_Event);
	UnregisterClass("ATTACWindow", hModule);
	DestroyWindow(ATTACWindow);
	ATTACWindow = 0;
	ATTAC_Callback = NULL;
}

void ATTACScripting_SignalATTACComplete()
{
	ATTACScripting_WaitForData(0);

	if (Still_Waiting)
		SetEvent(Wait_Event);

	ATTACScripting_ReleaseData();
}

long ATTACScripting_WaitForATTAC(long timeout)
{
	if (WaitForSingleObject(Wait_Event, timeout) == WAIT_TIMEOUT)
	{
		ATTACScripting_WaitForData(0);

		Still_Waiting = 0;
		return_string = NULL;
		argc = 0;

		ATTACScripting_ReleaseData();

		SendMessage(ATTACWindow, WM_USER + 4, (unsigned int) ATTAC_Callback, 0);
//we dont want to wait forever if we timeout...
//		ATTACScripting_WaitForATTAC(INFINITE);


		return WAIT_TIMEOUT;
	}
	ATTACScripting_WaitForData(0);
	Still_Waiting = 0;
	ATTACScripting_ReleaseData();
	return 0;
}

long APIENTRY ATTACScripting_ExitHandler(long ExitNumber, 
							  long Subfunction, 
							  PEXIT ParmBlock)
{
	if (!ATTACScripting_ScriptRunning())
	{
		return RXEXIT_NOT_HANDLED;
	}

	if (ExitNumber == RXSIO)
	{
		if (Subfunction == RXSIOSAY || Subfunction == RXSIOTRC)
		{
			RXSIOSAY_PARM *say_parm = (RXSIOSAY_PARM *)ParmBlock;

			argc = 1;
			argv = &(say_parm->rxsio_string);

			Still_Waiting = 1;
			SendMessage(ATTACWindow, WM_USER + 2, (unsigned int) ATTAC_Callback, 0);
	
			if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
			{
				return RXEXIT_RAISE_ERROR;
			}

			if (!ATTACScripting_ScriptRunning())
			{
				return RXEXIT_NOT_HANDLED;
			}
			
			return RXEXIT_HANDLED;
		}
		else if (Subfunction == RXSIOTRD || Subfunction == RXSIODTR)
		{
			RXSIOTRD_PARM *trd_parm = (RXSIOTRD_PARM *)ParmBlock;

			argc = 0;
			argv = NULL;

			return_string = &(trd_parm->rxsiotrd_retc);
			return_string->strptr = my_return_string;

			Still_Waiting = 1;
			SendMessage(ATTACWindow, WM_USER + 3, (unsigned int) ATTAC_Callback, 0);
	
			if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
			{
				return RXEXIT_RAISE_ERROR;
			}

			if (!ATTACScripting_ScriptRunning())
			{
				return RXEXIT_NOT_HANDLED;
			}

			return RXEXIT_HANDLED;
		}
		else
		{
			RXSIOSAY_PARM *say_parm = (RXSIOSAY_PARM *)ParmBlock;

			argc = 1;
			argv = &(say_parm->rxsio_string);

			Still_Waiting = 1;
			SendMessage(ATTACWindow, WM_USER + 108, (unsigned int) ATTAC_Callback, 0);
	
			if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
			{
				return RXEXIT_RAISE_ERROR;
			}

			if (!ATTACScripting_ScriptRunning())
			{
				return RXEXIT_NOT_HANDLED;
			}

			return RXEXIT_HANDLED;
		}
	}
	
	return RXEXIT_NOT_HANDLED;
}
							  
APIENTRY ATTACScripting_Hook(PRXSTRING cmd, PUSHORT flags, PRXSTRING returnstring)
{
	ATTACScripting_WaitForData(0);
	
	return_string = returnstring;
	argc = 0;
	command = cmd;
	Still_Waiting = 1;
	
	ATTACScripting_ReleaseData();

	SendMessage(ATTACWindow, WM_USER + 1, (unsigned int) ATTAC_Callback, 0);

	if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
	{
		return 1;
	}
	return 0;
}

APIRET APIENTRY ATTACScripting_FuncHandler(PSZ name,
								 ULONG ac,
								 PRXSTRING av,
								 PSZ queuename,
								 PRXSTRING rs)
{
	long tmp_long;
	double tmp_float;
	int function_num = 0;
	long my_timeout;

	if (!ATTACScripting_ScriptRunning())
	{
		return 1;
	}

	my_timeout = INFINITE;

	return_string = rs;
	argc = ac;
	argv = av;

	if (!stricmp("ZocAsk", name))
		function_num = 100;
	else if (!stricmp("ZocAskP", name))
		function_num = 101;
	else if (!stricmp("ZocBeep", name))
		function_num = 102;
	else if (!stricmp("ZocCls", name))
		function_num = 103;
	else if (!stricmp("ZocDelay", name))
	{
		if (argc > 1)
		{
			return_string = NULL;
			argc = 0;
			argv = NULL;
			return 1;
		}
		if (argc == 0)
			tmp_float = 0.2;
		else
		{
			tmp_float = atof(argv[0].strptr);
		}
		tmp_long = (long) floor(tmp_float * 1000);
		return_string = NULL;
		argc = 0;
		argv = NULL;

		Still_Waiting = 1;
		WaitForSingleObject(Wait_Event, tmp_long);
		Still_Waiting = 0;

		if (!ATTACScripting_ScriptRunning())
		{
			return 1;
		}

		return 0;
	}
	else if (!stricmp("ZocGetLine", name))
	{
		function_num = 105;
		my_timeout = (long) (timeout * 1000);
	}
	else if (!stricmp("ZocLastLine", name))
		function_num = 106;
	else if (!stricmp("ZocLockKeyboard", name))
		function_num = 107;
	else if (!stricmp("ZocKeyboard", name))
		function_num = 107;
	else if (!stricmp("ZocMsgBox", name))
		function_num = 108;
	else if (!stricmp("ZocNotify", name))
		function_num = 109;
	else if (!stricmp("ZocOs", name))
		function_num = 110;
	else if (!stricmp("ZocRequest", name))
		function_num = 111;
	else if (!stricmp("ZocRespond", name))
		function_num = 112;
	else if (!stricmp("ZocScreen", name))
		function_num = 113;
	else if (!stricmp("ZocGetScreen", name))
		function_num = 113;
	else if (!stricmp("ZocSend", name))
		function_num = 114;
	else if (!stricmp("ZocSendRaw", name))
		function_num = 115;
	else if (!stricmp("ZocTimeout", name))
	{
		if (argc != 1)
		{
			return_string = NULL;
			argc = 0;
			argv = NULL;
			return 1;
		}
		timeout = atof(argv[0].strptr);
		if (timeout==-1)
			timeout=INFINITE;

		return_string = NULL;
		argc = 0;
		argv = NULL;
		return 0;
	}
	else if (!stricmp("ZocWait", name))
	{
		function_num = 117;
		my_timeout = (long) (timeout * 1000);
	}
	else if (!stricmp("ZocWaitMux", name))
	{
		function_num = 118;
		my_timeout = (long) (timeout * 1000);
	}
	else if (!stricmp("ZocWrite", name))
		function_num = 119;
	else if (!stricmp("ZocWriteln", name))
		function_num = 120;
	else if (!stricmp("ZocLogname", name))
		function_num = 121;
	else if (!stricmp("ZocLogging", name))
		function_num = 122;
	else if (!stricmp("ZocClearBuffer", name))
		function_num = 123;
	else if (!stricmp("ATTAC_Get_Sector_Info", name))
		function_num = 150;
	else if (!stricmp("ATTAC_Get_ZTM_Sector_Info", name))
		function_num = 151;
	else if (!stricmp("ATTAC_Get_Port_Info", name))
		function_num = 152;
	else if (!stricmp("ATTAC_UpdateCounter", name))
		function_num = 153;
	else if (!stricmp("ATTAC_GetTotalSectors", name))
		function_num = 154;
	else if (!stricmp("ATTAC_Message", name))
		function_num = 155;
	else if (!stricmp("ATTAC_GetBuffer", name))
		function_num = 156;
	else if (!stricmp("ATTAC_ClearBuffer", name))
		function_num = 157;
	else if (!stricmp("ATTAC_StartBuffer", name))
		function_num = 158;
	else if (!stricmp("ATTAC_StopBuffer", name))
		function_num = 159;
	else if (!stricmp("ATTAC_GetStardockSector", name))
		function_num = 160;
	else if (!stricmp("ATTAC_GetBestPairPort", name))
		function_num = 161;
	else if (!stricmp("ATTAC_GetBestEvilPairPort", name))
		function_num = 162;
	else if (!stricmp("ZocSynctime", name))
		function_num = 163;
	else if (!stricmp("ZocGetInfo", name))
		function_num = 164;
	else if (!stricmp("ZocReceiveBuf", name))
		function_num = 165;
	else if (!stricmp("Lower", name))
		function_num = 166;
	else if (!stricmp("ZocGetFilename", name))
		function_num = 167;
	else if (!stricmp("ZocGetOption", name))
		function_num = 168;
	else if (!stricmp("ZocGetGlobalOption", name))
		function_num = 168;
	else if (!stricmp("dosfname", name))
		function_num = 169;
	else if (!stricmp("dosisdir", name))
		function_num = 170;
	else if (!stricmp("ZocRequestList", name))
		function_num = 171;
	else if (!stricmp("ZocSetOption", name))
		function_num = 172;
	else if (!stricmp("ZocString", name))
		function_num = 173;
	else if (!stricmp("ZocWaitForSeq", name))
		function_num = 174;
	else if (!stricmp("ZocExec", name))
		function_num = 175;
	else if (!stricmp("ATTAC_HasSectorFig", name))
		function_num = 176;
	else if (!stricmp("ATTAC_SectorFigAmount", name))
		function_num = 177;
	else if (!stricmp("ATTAC_SectorFigOwner", name))
		function_num = 178;
	else if (!stricmp("ATTAC_SectorFigType", name))
		function_num = 179;
	else if (!stricmp("ATTAC_IsSectorBusted", name))
		function_num = 180;
	else if (!stricmp("ATTAC_IsSectorAvoided", name))
		function_num = 181;
	else if (!stricmp("ATTAC_SetCounterMax", name))
		function_num = 182;


	if (function_num == 0)
		function_num = 5;

	error_code = 1;

	Still_Waiting = 1;

	SendMessage(ATTACWindow, WM_USER + function_num, (unsigned int) ATTAC_Callback, 0);


	if (ATTACScripting_WaitForATTAC(my_timeout) == WAIT_TIMEOUT)
	{
		ATTACScripting_SetResult("640", 0);
//		return 0;
	}

	if (!ATTACScripting_ScriptRunning())
	{
		return 1;
	}

	return_string = NULL;
	argc = 0;
	argv = NULL;

	return error_code;
}

long ATTACScripting_StillWaiting()
{
	long result;
	
	ATTACScripting_WaitForData(0);
	result = Still_Waiting;
	ATTACScripting_ReleaseData();
	
	return result;
}

long __cdecl Start(void *data)
{
	long x;

	ATTACScripting_Init();
	x = ATTACScripting_Start ();

	return 0;
}


void ATTACScripting_Run(char *filename, char *data, ATTACCallbackFunc *callback)

{
	Data_Mutex = CreateMutex(NULL, 0, NULL);
	Wait_Event = CreateEvent(NULL, 0, 0, NULL);

	ATTACScripting_WaitForData(0);

	Script_Filename = (char *) RexxAllocateMemory(strlen(filename) + 1);
	strcpy(Script_Filename, filename);
	Script_Filename[strlen(filename)]=0;
	Script_Data = (char *) RexxAllocateMemory(strlen(data) + 1);
	strcpy(Script_Data, data);
	Script_Data[strlen(data)]=0;
	
	ATTAC_Callback = callback;

	ATTACScripting_ReleaseData();

	_beginthread(Start, 0, NULL );
}

long ATTACScripting_Start()
{
	long REXX_Error_Code;
	RXSYSEXIT Exits[2];
	RXSTRING Instore[2];

	Exits[0].sysexit_code = RXSIO;
	Exits[0].sysexit_name = "ATTAC";
	Exits[1].sysexit_code = RXENDLST;
	my_return_string = NULL;

	Instore[0].strlength = strlen(Script_Data);
	Instore[0].strptr = Script_Data;
	Instore[1].strptr = NULL;
	
	ATTACScripting_WaitForData(0);
	Script_Running = 1;
	ATTACScripting_ReleaseData();

	REXX_Error_Code = RexxStart((long)0, (PRXSTRING) NULL, (PSZ) Script_Filename, Instore, (PSZ) "ATTAC", (LONG) RXCOMMAND, Exits, (PUSHORT) NULL, (PRXSTRING) NULL);

	RexxFreeMemory(Script_Filename);
	RexxFreeMemory(Script_Data);
	if (my_return_string != NULL)
	{
		//free(my_return_string);
		//RexxFreeMemory(my_return_string);
		//my_return_string=NULL;
	}

	ATTACScripting_WaitForData(0);
	Script_Running = 0;

	return_string = NULL;
	argv = NULL;
	argc = 0;

	Still_Waiting = 1;
	ATTACScripting_ReleaseData();

	SendMessage(ATTACWindow, WM_USER + 0, (unsigned int) ATTAC_Callback, 0);


	if (ATTACScripting_WaitForATTAC(INFINITE) != WAIT_TIMEOUT)
	{
		ATTACScripting_Cleanup();
		return 1;
	}

	ATTACScripting_Cleanup();

	return 0;
}


void __stdcall ATTACScripting_Init()
{
	char WindowName[80];

	sprintf(WindowName, "ATTACWindow");

	ATTACWindow = FindWindow(NULL, WindowName);

	if (ATTACWindow == NULL)
	{
		ATTACWindow = CreateCallbackWindow();
	}

	Script_Running = 0;
	Still_Waiting = 0;
	timeout = 20.0;

	RexxRegisterSubcomExe("ATTAC", ATTACScripting_Hook, NULL);
	RexxRegisterExitExe("ATTAC", ATTACScripting_ExitHandler, NULL);
	RexxRegisterFunctionExe("ZocAsk", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocAskP", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocBeep", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocCls", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocDelay", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetLine", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocLastLine", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocLockKeyboard", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocKeyboard", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocMsgBox", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocNotify", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocOs", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocRequest", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocRequestList", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocRespond", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocScreen", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetScreen", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSend", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSendRaw", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocTimeout", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocWait", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocWaitMux", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocWrite", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocWriteln", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocLogname", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocLogging", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocClearBuffer", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSynctime", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetInfo", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocReceiveBuf", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("Lower", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetFilename", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetOption", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetGlobalOption", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetOption", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocString", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocWaitForSeq", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocExec", ATTACScripting_FuncHandler);


	// ATTAC Specific REXX Command
	RexxRegisterFunctionExe("ATTAC_Get_Sector_Info", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_Get_Port_Info", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_Get_ZTM_Sector_Info", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_UpdateCounter", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_GetTotalSectors", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_Message", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_GetBuffer", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_ClearBuffer", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_StartBuffer", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_StopBuffer", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_GetStardockSector", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_GetBestPairPort", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_GetBestEvilPairPort", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_HasSectorFig", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_SectorFigAmount", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_SectorFigOwner", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_SectorFigType", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_IsSectorBusted", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_IsSectorAvoided", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ATTAC_SetCounterMax", ATTACScripting_FuncHandler);
	
	RexxRegisterFunctionExe("dosfname", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("dosisdir", ATTACScripting_FuncHandler);
	
//	RexxRegisterFunctionExe("ATTAC_", ATTACScripting_FuncHandler);
//	RexxRegisterFunctionExe("ATTAC_", ATTACScripting_FuncHandler);

	// These are Zoc Command that aren't supported

	RexxRegisterFunctionExe("ZocAutoConnect", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocBaud", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocCaptClr", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocCarrier", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocConnect", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocCtrlString", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocCursor", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocDisconnect", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocDownload", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocEndZoc", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetFilenames", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocGetPhonebk", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocHfc", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocLoadOpts", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocPID", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocRestimer", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSendBreak", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSendEmulationKey", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSendKey", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetAutoAccept", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetDIPath", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetDevice", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetDevParm", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetEmu", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetHost", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetPhonebk", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetTimer", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetUnit", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSetUnattended", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocShell", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocSuppressOutput", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocUpload", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocWaitForSeq", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocXferShell", ATTACScripting_FuncHandler);
	RexxRegisterFunctionExe("ZocXferDosShell", ATTACScripting_FuncHandler);
}

long ATTACScripting_GetCommand(char *Command)
{
	long result;
	result = 0;

	strcpy(Command, "");

	if (ATTACScripting_WaitForData(0) != WAIT_TIMEOUT)
	{
		result = 1;
		strncpy(Command, command->strptr, command->strlength);

		ATTACScripting_ReleaseData();
	}
	return result;
}

long ATTACScripting_GetCommandSize()
{
	long size;
	size = 0;
	if (ATTACScripting_WaitForData(0) != WAIT_TIMEOUT)
	{
		size = command->strlength;

		ATTACScripting_ReleaseData();
	}
	return size;
}

long ATTACScripting_GetArg(char *Argument, long index)
{
	long result;

	result = 0;

	strcpy(Argument, "");

	if (ATTACScripting_WaitForData(0) != WAIT_TIMEOUT)
	{
		if (index < argc)
		{
			strncpy(Argument, argv[index].strptr, argv[index].strlength);
			result = 1;
		}

		ATTACScripting_ReleaseData();
	}
	return result;
}

long ATTACScripting_GetArgSize(long index)
{
	long size;
	size = 0;
	if (ATTACScripting_WaitForData(0) != WAIT_TIMEOUT)
	{
		if (index < argc)
			size = argv[index].strlength;

		ATTACScripting_ReleaseData();
	}
	return size;
}

long ATTACScripting_GetArgCount()
{
	long size;
	size = 0;
	if (ATTACScripting_WaitForData(0) != WAIT_TIMEOUT)
	{
		size = argc;
		ATTACScripting_ReleaseData();
	}
	return size;
}

long ATTACScripting_GetErrorCode()
{
	return error_code;
}

void ATTACScripting_SetResult(char *result, long error)
{
	ATTACScripting_WaitForData(0);
	if (return_string != NULL)
	{
		if (my_return_string != NULL)
		{
			RexxFreeMemory(my_return_string);
			//my_return_string=NULL;
		}
		my_return_string = (char *) RexxAllocateMemory(strlen(result) + 1);
		return_string->strlength = strlen(result);
		return_string->strptr = my_return_string;
		strncpy(return_string->strptr, result, strlen(result));
		return_string->strptr[strlen(result)] = 0;
		error_code = error;
	}
	ATTACScripting_ReleaseData();
}

void ATTACScripting_KillScript()
{
	ATTACScripting_WaitForData(0);
	Script_Running = 0;
	ATTACScripting_ReleaseData();
}
