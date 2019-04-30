// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <math.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "attacGUI.h"
#include "rexx.h"
#include "win_res.h"
#include "util.h"
#include "encrypt.h"
#include "info.h"
#include "parsedatathread.h"
#include "../PUTTY.H"
#include "../LDISC.H"
#include "../WINSTUFF.H"
#include "../TERMINAL.H"

// for Microsoft VC++
#ifndef strcasecmp
#define strcasecmp stricmp 
#endif

int encryptedScript=0;
int REXX_LockKeyboard=0;
int ScriptCounter=0;
int NonRegisteredREXXCount=0;
int REXX_RunningScript=0;
double REXXtimeout=20.0;

char LastLine[MAX_LENGTH];

long Respond_Cnt=0;
char *Respond_To[MAX_LENGTH];
char *Respond_With[MAX_LENGTH];
long Respond_Start[MAX_AMOUNT];
long killedScriptWithEsc=0;

// regina implementation 

HANDLE Zoc_Respond_Mutex;
HANDLE Send_Data_Mutex;
HANDLE Memory_Mutex;
HANDLE Data_Mutex;
HANDLE Wait_Event;
HANDLE Wait_Input_Event;

int ZocLogging=0;
FILE *fpLogFile=NULL;
char ZocLogName[MAX_LENGTH];
char ZocBuffer[MAX_LENGTH];
int ZocBuffering=0;

char *ZocReceiveBuf=NULL;
long ZocReceiveBufMax=0;
long ZocReceiveBufCurr=0;
int ZocBufferStatus=0;
int ZocWaitForSeq=0;


long error_code;
long Still_Waiting;
long Script_Running;

char *rexxAskPrompt=NULL;
char *rexxAskDefault=NULL;
char *rexxAskResult=NULL;

long rexxRequestArgumentCount=0;
char *rexxRequestArguments[MAX_AMOUNT];
int rexxRequestList=0;
char *rexxRequestResult=NULL;

char *ZocWaitResult=NULL; 

char REXX_Buffer[MAX_LENGTH];
char REXX_Buffer_RegularCase[MAX_LENGTH];
int REXX_Waiting=0;
int REXX_WaitNextLine=0;
char *REXX_Waiting_For[MAX_AMOUNT];
long REXX_Waiting_For_Cnt=0;

struct RXFETCHLINE
{
	RXSTRING	*retstr;
	char		*title;
};

struct rexxScript
{
	char		*filename;
	char		*data;
};


// end regina implementation


void My_Sleep (long time)
{
	HMODULE hLibraryKernel;
	hLibraryKernel = LoadLibrary("kernel32.dll");

	Sleepy = (my_Sleep) GetProcAddress(hLibraryKernel,"Sleep");

    Sleepy(time);

	FreeLibrary (hLibraryKernel); 
    hLibraryKernel = 0;
}


void REXX_Init()
{
	int i;

    strcpy(REXX_Buffer,"");
    strcpy(REXX_Buffer_RegularCase,"");
    strcpy (LastLine, "");
    REXX_LockKeyboard = 0;

	putenv("REGINA_OPTIONS=CALLS_AS_FUNCS");
	
	ATTACScripting_WaitForZocRespond(0);
    Respond_Cnt = 0;
	for (i=0; i< MAX_AMOUNT; i++)
	{
		REXX_Waiting_For[i]=NULL;
		Respond_To[i]=NULL;
		Respond_With[i]=NULL;
	}
	ATTACScripting_ReleaseZocRespond();
}

void REXX_KillScript()
{   
	int j;

    strcpy (REXX_Buffer, "");
    strcpy (REXX_Buffer_RegularCase, "");
    strcpy (ZocBuffer, "");

	ATTACScripting_WaitForZocRespond(0);
	for (j=0; j <Respond_Cnt; j++)
	{
		pfree(&Respond_To[j]);
		pfree(&Respond_With[j]);
	}
    Respond_Cnt = 0;
	ATTACScripting_ReleaseZocRespond();

    ZocBuffering = 0;
	ZocBufferStatus=0;
	ZocWaitForSeq=0;


	REXX_RunningScript=0;
    countMax=0;
	ScriptCounter=-1;
    REXX_WaitNextLine = 0;

	ATTACScripting_WaitForMemory(0);
	for (j=0; j<REXX_Waiting_For_Cnt; j++)
		pfree(&REXX_Waiting_For[j]);
    REXX_Waiting_For_Cnt = 0;
	ATTACScripting_ReleaseMemory();

	if (ZocLogging)
	{
        ZocLogging = 0;
		if (fpLogFile != NULL)
			fclose(fpLogFile);
	}

	if (ZocReceiveBuf != NULL)
	{
		pfree (&ZocReceiveBuf);

		ZocReceiveBufCurr=0;
		ZocReceiveBufMax=0;
		ZocReceiveBuf=NULL;
	}

	setScriptMessage("Script Not Running");

    if ((int) ATTACScripting_StillWaiting() == 1) 
        ATTACScripting_SignalATTACComplete();

    REXX_Waiting = 0;
	Still_Waiting =0;
}


void REXX_ProcessInbound (unsigned char c, int logmode)
{
	long I, j;
	unsigned char orig_inchar;
	char *tmp;
	char buffer[50], tChar[3];

	if (logmode==LGTYP_REXX && gettingTWXData==0 && passToREXX==1)
	{

		ATTACScripting_WaitForData(0);

		printf ("%c - %d\n", c, logmode);

		orig_inchar = c;
//		c = toupper(c);
    
		if (fpLogFile != NULL)
			fprintf (fpLogFile, "%c", c);

		if (c == 10)
		{
			if (REXX_WaitNextLine)
			{
				Update_LastLine(strlen(REXX_Buffer), 0);
				if (strcmp(LastLine,"") != 0)
				{
					REXX_WaitNextLine = 0;
					ATTACScripting_SignalATTACComplete();
				}
			}
		}

		if (ZocBufferStatus==1)
		{
			if (ZocReceiveBuf != NULL && ZocReceiveBufCurr<ZocReceiveBufMax)
			{
				sprintf (tChar, "%c", (unsigned char) orig_inchar);
				strcat (ZocReceiveBuf, tChar);
				ZocReceiveBuf[++ZocReceiveBufCurr]=0;
			}
		}

		if (c != 13)
		{
			sprintf (tChar, "%c", (unsigned char) toupper(c));
			strcat (REXX_Buffer, tChar);

			sprintf (tChar, "%c", (unsigned char) c);
			strcat (REXX_Buffer_RegularCase, tChar);

			if (strlen (REXX_Buffer) > (MAX_LENGTH-10))
			{
				tmp = right (REXX_Buffer, strlen (REXX_Buffer)-(MAX_LENGTH-200));
				strcpy (REXX_Buffer, tmp);
				pfree(&tmp);

				for(I = 0; I < Respond_Cnt; I++)
					Respond_Start[I] = Respond_Start[I] - strlen (REXX_Buffer)-(MAX_LENGTH-200);
			}

			if (strlen (REXX_Buffer_RegularCase) > (MAX_LENGTH-10))
			{
				tmp = right (REXX_Buffer_RegularCase, strlen (REXX_Buffer_RegularCase)-(MAX_LENGTH-200));
				strcpy (REXX_Buffer_RegularCase, tmp);
				pfree(&tmp);
			}

			if (ZocBuffering && strlen (ZocBuffer)<MAX_LENGTH)
			{
				sprintf (tChar, "%c", (unsigned char) orig_inchar);
				strcat (ZocBuffer, tChar);
			}

			while (Respond() == 1);

			if (REXX_Waiting)
			{
				ATTACScripting_WaitForMemory(0);

				for(I = 0; I < REXX_Waiting_For_Cnt; I++)
				{
					if (strlen(REXX_Buffer) >= strlen(REXX_Waiting_For[I]))
					{
						tmp = right (REXX_Buffer, strlen(REXX_Waiting_For[I]));

						if (strcmp(tmp, REXX_Waiting_For[I]) ==0) 
						{
							Update_LastLine(strlen(REXX_Buffer), 0);
							sprintf (buffer, "%ld", I);

							ZocWaitResult = (char *) strdup (buffer);

//							printf ("FOUND %ld: %s - %s\n", I, tmp, REXX_Waiting_For[I]);
//							outputData(tmp);

							I = REXX_Waiting_For_Cnt;

							REXX_Waiting=0;

							for (j=0; j<REXX_Waiting_For_Cnt; j++)
								pfree(&REXX_Waiting_For[j]);
							REXX_Waiting_For_Cnt = 0;

							ATTACScripting_SignalATTACComplete();
						}
						pfree(&tmp);
					}
				}

				ATTACScripting_ReleaseMemory();
			}

		}
		ATTACScripting_ReleaseData();
	}
}

void Update_LastLine(long pos, int called_by_respond)
{
	
    long I;
	char *tmp;

	if (called_by_respond)
	{
        for(I=pos; I< (long) strlen(REXX_Buffer); I++)
		{
            if (REXX_Buffer[I] == 10)
			{
                pos = I;
                I = strlen(REXX_Buffer);
            }
        }
    }

	if (pos>=MAX_LENGTH-1)
		tmp = left(REXX_Buffer_RegularCase, MAX_LENGTH);
	else
		tmp = left(REXX_Buffer_RegularCase, pos);

	strcpy (LastLine, tmp);
	pfree(&tmp);

    
	if (strlen(REXX_Buffer) - pos<0)
		tmp = right(REXX_Buffer, 0);
	else
		tmp = right(REXX_Buffer, strlen(REXX_Buffer) - pos);

	strcpy (REXX_Buffer, tmp);
	pfree(&tmp);

	if (strlen(REXX_Buffer_RegularCase) - pos<0)
		tmp = right(REXX_Buffer_RegularCase, 0);
	else
		tmp = right(REXX_Buffer_RegularCase, strlen(REXX_Buffer_RegularCase) - pos);

	strcpy (REXX_Buffer_RegularCase, tmp);
	pfree(&tmp);

    for (I = 0; I < Respond_Cnt; I++)
	{
        if (Respond_Start[I] < pos)
            Respond_Start[I] = 1;
        else
            Respond_Start[I] = Respond_Start[I] + 1 - pos;
    }

	//might need >=
    for (I = strlen(LastLine) - 1; I > 0; I--)
	{
        if (LastLine[I] == 13)
		{
			tmp = right(LastLine, (strlen(LastLine) - I));
			strcpy (LastLine, tmp);
			pfree(&tmp);
            I = 0;
        }
    }
}
 


long Respond ()
{ 
	long I;
    long J;
    long pos;
	char *tmp;

	ATTACScripting_WaitForZocRespond(0);

    for(I = 0; I< Respond_Cnt; I++)
	{
        if (Respond_To[I] != NULL && strlen(REXX_Buffer) - Respond_Start[I] >= strlen(Respond_To[I]))
		{
            pos = Instr(Respond_Start[I], REXX_Buffer, Respond_To[I]);
            if (pos != 0)
			{
				tmp = Translate_String(Respond_With[I], 0, 0);
				for (J = 0; J < (long) strlen(tmp); J++)
				{
					ldisc_send (ldisc_extern, &tmp[J], 1, 1);
				}
				pfree(&tmp);
                
				Respond_Start[I] = pos + strlen(Respond_To[I]) - 1;
				ATTACScripting_ReleaseZocRespond();
                return(1);
            }
        }
    }
	
	ATTACScripting_ReleaseZocRespond();
	return(0);   //i added this.
}



void REXX_Start(char *FileName, int encrypted, int extraArgs, char args[100][100])
{
	FILE *fp; /* declare a FILE pointer  */
    char *string = NULL;
    char *string2 = NULL;
	char *library = NULL;
    char *line = NULL;
	char *tmp1=NULL;
	char *tmp2=NULL;
	char *tmp3=NULL;
	char buffer[50];
	struct rexxScript *pRexxScript;

    int total_size;
    int read_cnt;	
    int size=0, i;
	ScriptCounter = 0;

    if (ATTACScripting_ScriptRunning() == 1)
	{
        MessageBox(NULL, "Script already running!", "REXX Error", MB_OK | MB_ICONEXCLAMATION);
        return;
	}

	encryptedScript=encrypted;

	if (encrypted==0)
	{
		if ( (fp = fopen(FileName,"r")) == NULL )
		{
			MessageBox(NULL, "Couldn't open file", "REXX Error", MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		else
		{
		 if (size==0)
			{
			  size=256;
			  string = (char *) malloc(11);
			  strcpy(string, "/*attac*/\n");
			  total_size = 10;
			  line = (char *) malloc(size);
			  while ((read_cnt = fread(line, 1, size, fp)) != 0)
				{
				  string2 = (char *) malloc(total_size + read_cnt + 1);
				  if (total_size != 0)
				  {
					strncpy(string2, string, total_size);
					pfree(&string);
				  }
				  strncpy(&string2[total_size], line, read_cnt);
				  total_size = total_size + read_cnt;
				  string2[total_size] = 0;
				  string = string2;
				  pfree(&line);
				  line = (char *) malloc(size);
				}
			  pfree(&line);
			}
		}
		fclose(fp);
	}
	else
	{
		string = (char *) getFileAndDecrypt(FileName);

		if (strcmp(string, "")==0)
		{
			sprintf (buffer, "Error opening file %s", FileName);
			attacErrorMessage(buffer);
			return;
		}
	}


	//append ATTAC = 1 onto the string
	size=0;
	tmp1 = (char *) strdup ("ATTAC = 1\n");
	size = strlen(tmp1);
	size += strlen(string);
	tmp3 = (char *) malloc (size + 1);
	strncpy (&tmp3[0], tmp1, strlen (tmp1));
	strncpy (&tmp3[strlen(tmp1)], string, strlen(string));
	tmp3[size]=0;
	pfree(&string);
	pfree(&tmp1);
	string = tmp3;


	if (extraArgs>0)
	{
		for (i=0; i<extraArgs; i++)
		{
			//append arg1 onto the string
			size=0;
			tmp3 = (char *) strdup ("\"\n");
			sprintf (buffer, "attac_arg%d = \"", i+1);
			tmp2 = (char *) strdup (buffer);
			tmp1 = (char *) malloc (strlen (tmp2) + strlen(args[i]) + strlen(tmp3) + 1);
			size = strlen(tmp2);
			strncpy (&tmp1[0], tmp2, size);
			strncpy (&tmp1[size], args[i], strlen (args[i]));
			size += strlen(args[i]);
			strncpy (&tmp1[size], tmp3, 2);
			size += strlen(tmp3);
			tmp1[size]=0;
			pfree(&tmp2);
			pfree(&tmp3);

			size = strlen(tmp1);
			size += strlen(string);
			tmp3 = (char *) malloc (size + 1);
			strncpy (&tmp3[0], tmp1, strlen (tmp1));
			strncpy (&tmp3[strlen(tmp1)], string, strlen(string));
			tmp3[size]=0;
			pfree(&string);
			pfree(&tmp1);
			string = tmp3;
		}

	}



	library = (char *) strdup("\nexit\n");
	size = strlen(library);
	size += strlen(string);
	tmp3 = (char *) malloc (size + 1);
	strncpy (&tmp3[0], string, strlen (string));
	strncpy (&tmp3[strlen(string)], library, strlen(library));
	tmp3[size]=0;
	pfree(&string);
	pfree(&library);
	string = tmp3;
	

	// add the ATTAC library onto the scripts
	if (encrypted==0) {
		strcpy (fullFilePath, scriptDir);
		strcat (fullFilePath, "library.zrx");
		library = (char *) getFileWithoutDecrypt(fullFilePath);
	}
	else {
		strcpy (fullFilePath, scriptDir);
		strcat (fullFilePath, "library.erx");
		library = (char *) getFileAndDecrypt(fullFilePath);
	}


	size = strlen(library);
	size += strlen(string);
	tmp3 = (char *) malloc (size + 1);
	strncpy (&tmp3[0], string, strlen (string));
	strncpy (&tmp3[strlen(string)], library, strlen(library));
	tmp3[size]=0;
	pfree(&string);
	pfree(&library);
	string = tmp3;

	REXX_LockKeyboard = 0;
	
	REXX_RunningScript=1;

	if (NonRegisteredREXXCount != -1)
		NonRegisteredREXXCount=0;

	//create a struct to pass the string and filename
	pRexxScript = (struct rexxScript*) malloc(sizeof(struct rexxScript));

	//populate the struct
	pRexxScript->filename = (char *) strdup(FileName);
	pRexxScript->data = string;

	// string is freed in thread
	_beginthread(StartRexxThread, 0, pRexxScript);

	setScriptMessage("Running ...");

}


int Hex_Convert(char Data)
{
  switch (Data)
  {
    case 'A': return 10;
    case 'B': return 11;
    case 'C': return 12;
    case 'D': return 13;
    case 'E': return 14;
    case 'F': return 15;
  }
 return (int) (Data - '0');
}


char* right (char tmp[], int len)
{
	char *newtmp;
	int i, lenTemp=0;

	lenTemp = (int) strlen(tmp);

	if (len >= lenTemp)
		newtmp = (char *) strdup (tmp);
	else
	{
		newtmp = (char*) malloc (len+1);
		for (i=0; i<(int)len && i <(int)strlen (tmp); i++)
			newtmp[(len-1)-i] = tmp[(lenTemp-1)-i];
		newtmp[i]='\x0';
	}
	return (newtmp);
}

char* left (char tmp[], int len)
{
	char *newtmp;
	int i, lenTemp=0;

	lenTemp = (int) strlen(tmp);

	if (len >= lenTemp)
		newtmp = (char *) strdup (tmp);
	else
	{
		newtmp = (char*) malloc (len+1);
		for (i=0; i<len && i <lenTemp; i++)
			newtmp[i] = tmp[i];
		newtmp[i]='\x0';
	}
	return (newtmp);
}


char* Translate_String(char *Data, int Long_Return, int For_Wait)
{
  char *new_data, *t;
  int i;
  int pos;
  int num;

  if (Data[0] == 0)
  {
    new_data = (char *) malloc(2);
	new_data[0] = ' ';
    new_data[1] = 0;

    return new_data;
  }

  new_data = (char *) malloc(strlen(Data) * 3 + 10);
  pos = 0;

  for (i = 0; i <(int) strlen(Data) - 1; i++)
  {
    if (Data[i] == '^')
    {
      if (Data[i + 1] == '^')
      {
        new_data[pos++] = '^'; i++;
      }
      else if (Data[i + 1] == 'G' || Data[i + 1] == 'g')
      {
        new_data[pos++] = 7; i++;
      }
      else if (Data[i + 1] == 'H' || Data[i + 1] == 'h')
      {
        new_data[pos++] = 8; i++;
      }
      else if (Data[i + 1] == 'I' || Data[i + 1] == 'i')
      {
        new_data[pos++] = 9; i++;
      }
      else if (Data[i + 1] == '[')
      {
        new_data[pos++] = 27; i++;
      }
      else if (Data[i + 1] == 'M' || Data[i + 1] == 'm')
      {
        if (Long_Return)
        {
          new_data[pos++] = 10; i++;
          new_data[pos++] = 13; i++;
        }
        else if (For_Wait)
        {
          new_data[pos++] = 10; i++;
        }
        else
        {
          new_data[pos++] = 13; i++;
        }
      }
      else if (Data[i + 1] == 'J' || Data[i + 1] == 'j')
      {
         new_data[pos++] = 10; i++;
      }
      else if (Data[i + 1] == '~')
      {
        new_data[pos++] = 30; i++;
      }
      else if (i <= (int)strlen(Data) - 4 && Data[i + 1] == '(' && Data[i + 4] == ')')
      {
        num = (Hex_Convert(Data[i + 2]) * 16) + Hex_Convert(Data[i + 3]);
        sprintf(&new_data[pos++], "%d", num);
      }
      else
      {
        new_data[pos++] = Data[i];
      }
    }
    else
    {
      new_data[pos++] = Data[i];
    }
  }
  if (i == (int)strlen(Data) - 1)
  {
    new_data[pos++] = Data[i];
  }
  new_data[pos] = 0;

  t = (char *) strdup (new_data);
  pfree(&new_data);
  return t;
}

long Instr (long startPos, char stringBeingSearched[] , char stringToFind[] )
{
	int i, j;
	int firstMatch=0, match=0;
	int stringToFindLen;
	int stringBeingSearchedLen;

	stringBeingSearchedLen = strlen(stringBeingSearched);
	stringToFindLen = strlen(stringToFind);

	if (stringBeingSearchedLen==0 || stringToFindLen==0)
		return(0);

	for (i=(startPos-1); i<stringBeingSearchedLen; i++)
	{
		match=0;
		if (stringBeingSearched[i]==stringToFind[0])
		{
			match++;
			firstMatch = i+1;
			for (j=1; j<stringToFindLen && i+j<stringBeingSearchedLen; j++)
				if (stringToFind[j]==stringBeingSearched[i+j])
					match++;
		}
		if (match==stringToFindLen)
			return(firstMatch);
		firstMatch=0;
	}
	return(firstMatch);
}


char * replaceChars(char *realString, char *replaceFrom)
{
	int lenRealString, lenReplaceFrom, i, j, k=0, matched=0;
	char returnVal[MAX_LENGTH], *t;

	lenRealString = strlen(realString);
	lenReplaceFrom = strlen(replaceFrom);

	strcpy (returnVal, "");
	for (i=0; i<lenRealString; i++)
	{
		for (j=0; j<lenReplaceFrom && matched==0; j++)
		{
			if (realString[i]==replaceFrom[j])
				matched=1;
		}
		if (matched==0)
			returnVal[k++]=realString[i];
		returnVal[k]=0;
		matched=0;
	}

	t = (char *) strdup (returnVal);
	return (t);
}

char * replaceString(char *realString, char *replaceFrom, char *replaceWith)
{
	char returnVal[MAX_LENGTH], *t, *realT;
	int lenFound;
	long pos;

	realT = (char *) strdup (realString);

	pos = Instr (1, realT, replaceFrom);

	strcpy (returnVal, "");
	while (pos)
	{
		t = left (realT, pos-1);
		strcat (returnVal, t);
		strcat (returnVal, replaceWith);
		lenFound = strlen (replaceFrom);
		pfree(&t);
		t = right (realT, strlen (realT) - (pos-1)-lenFound);
		pfree (&realT);
		realT = (char *) strdup (t);
		pfree(&t);

		pos = Instr (pos+1, realT, replaceFrom);
	}
	strcat (returnVal, realT);
	pfree(&realT);
	returnVal[strlen(returnVal)]=0;
	t = (char *) strdup (returnVal);
	return (t);
}

int IsDirectory(char* FileName)
{
    struct stat my_stat;
	if (FileName[strlen(FileName)-1]=='\\')
		FileName[strlen(FileName)-1]=0;
    if (stat(FileName, &my_stat) != 0) return 0;
    return ((my_stat.st_mode & S_IFDIR) != 0);
}

void printErrorMessage(int type, char *val, long err)
{
	char buffer[MAX_LENGTH];
	char errStr[255];

	if (err==RXFUNC_DEFINED)
		strcpy (errStr, "There is some function registered with this name");
	else if (err==RXFUNC_NOMEM)
		strcpy (errStr, "Not enough memory to perform the operation");
	else if (err==RXFUNC_NOTREG)
		strcpy (errStr, "There is no function registered with this name");
	else if (err==RXFUNC_MODNOTFND)
		strcpy (errStr, "The specified DLL can't be found");
	else if (err==RXFUNC_ENTNOTFND)
		strcpy (errStr, "The specified function can't be found in the DLL");
	else if (err==RXFUNC_NOTINIT)
		strcpy (errStr, "Registering a function in a DLL is not supported");
	else if (err==RXFUNC_BADTYPE)
		strcpy (errStr, "An arg you passed to the REXXSAA API is incorrect");
	else 
		strcpy (errStr, "Unspecified REXX error");

	if (type==0)
		sprintf (buffer, "The following error occured registering %s: %s", val, errStr);
	else
		sprintf (buffer, "The following error occured deregistering %s: %s", val, errStr);

	attacErrorMessage(buffer);
}

void StartRexxThread(void *dummy)
{
	char *Script_Filename, *Script_Data, buffer[255];
	long REXX_Error_Code;
	struct rexxScript *pRexxScript;
	RXSTRING rexxstartRetStr;
	APIRET errRegister;

	RXSYSEXIT Exits[2];
	RXSTRING Instore[2];
	
	Zoc_Respond_Mutex = CreateMutex(NULL, 0, NULL);
	Memory_Mutex = CreateMutex(NULL, 0, NULL);
	Data_Mutex = CreateMutex(NULL, 0, NULL);
	Wait_Event = CreateEvent(NULL, 0, 0, NULL);
	Wait_Input_Event = CreateEvent(NULL, 0, 0, NULL);
	
	REXX_Init();



	//declare local struct
	pRexxScript = (struct rexxScript*) dummy;

	//get data in local memory
	Script_Data = (char *) strdup(pRexxScript->data);
	Script_Filename = (char *) strdup(pRexxScript->filename);
/*
	{
		FILE *s;
		s = fopen ("c:\\rexx.txt", "w");
		fprintf (s, "%s", Script_Data);
		fclose (s);
	}
	*/
	//free struct
	pfree(&pRexxScript->data);
	pfree(&pRexxScript->filename);
	pfree(&pRexxScript);

	Script_Running = 0;
	Still_Waiting = 0;
	REXXtimeout = 20.0;

	errRegister = RexxRegisterSubcomExe("ATTAC", ATTACScripting_Hook, NULL);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC", errRegister);
		return;
	}
	errRegister = RexxRegisterExitExe("ExitHandler", (PFN) ATTACScripting_ExitHandler, NULL);
	if (errRegister)
	{
		printErrorMessage(0, "ExitHandler", errRegister);
		return;
	}

	errRegister = RexxRegisterFunctionExe("ZOCASK", ATTACScripting_ZocAsk);
	if (errRegister)
	{
		printErrorMessage(0, "ZocAsk", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCASKP", ATTACScripting_ZocAskP);
	if (errRegister)
	{
		printErrorMessage(0, "ZocAskP", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCBEEP", ATTACScripting_ZocBeep);
	if (errRegister)
	{
		printErrorMessage(0, "ZocBeep", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCCLS", ATTACScripting_ZocCls);
	if (errRegister)
	{
		printErrorMessage(0, "ZocCls", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCDELAY", ATTACScripting_ZocDelay);
	if (errRegister)
	{
		printErrorMessage(0, "ZocDelay", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCGETLINE", ATTACScripting_ZocGetLine);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetLine", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCLASTLINE", ATTACScripting_ZocLastLine);
	if (errRegister)
	{
		printErrorMessage(0, "ZocLastLine", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCLOCKKEYBOARD", ATTACScripting_ZocLockKeyboard);
	if (errRegister)
	{
		printErrorMessage(0, "ZocLockKeyboard", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCKEYBOARD", ATTACScripting_ZocLockKeyboard);
	if (errRegister)
	{
		printErrorMessage(0, "ZocKeyboard", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCMSGBOX", ATTACScripting_ZocMsgBox);
	if (errRegister)
	{
		printErrorMessage(0, "ZocMsgBox", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCNOTIFY", ATTACScripting_ZocNotify);
	if (errRegister)
	{
		printErrorMessage(0, "ZocNotify", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCOS", ATTACScripting_ZocOs);
	if (errRegister)
	{
		printErrorMessage(0, "ZocOs", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCREQUEST", ATTACScripting_ZocRequest);
	if (errRegister)
	{
		printErrorMessage(0, "ZocRequest", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCREQUESTLIST", ATTACScripting_ZocRequestList);
	if (errRegister)
	{
		printErrorMessage(0, "ZocRequestList", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCRESPOND", ATTACScripting_ZocRespond);
	if (errRegister)
	{
		printErrorMessage(0, "ZocRespond", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCSCREEN", ATTACScripting_ZocGetScreen);
	if (errRegister)
	{
		printErrorMessage(0, "ZocScreen", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCGETSCREEN", ATTACScripting_ZocGetScreen);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetScreen", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCSEND", ATTACScripting_ZocSend);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSend", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCSENDRAW", ATTACScripting_ZocSendRaw);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSendRaw", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCTIMEOUT", ATTACScripting_ZocTimeout);
	if (errRegister)
	{
		printErrorMessage(0, "ZocTimeout", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCWAIT", ATTACScripting_ZocWait);
	if (errRegister)
	{
		printErrorMessage(0, "ZocWait", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCWAITMUX", ATTACScripting_ZocWaitMux);
	if (errRegister)
	{
		printErrorMessage(0, "ZocWaitMux", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCWRITE", ATTACScripting_ZocWrite);
	if (errRegister)
	{
		printErrorMessage(0, "ZocWrite", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCWRITELN", ATTACScripting_ZocWriteln);
	if (errRegister)
	{
		printErrorMessage(0, "ZocWriteln", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCLOGNAME", ATTACScripting_ZocLogname);
	if (errRegister)
	{
		printErrorMessage(0, "ZocLogname", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCLOGGING", ATTACScripting_ZocLogging);
	if (errRegister)
	{
		printErrorMessage(0, "ZocLogging", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCCLEARBUFFER", ATTACScripting_ZocClearBuffer);
	if (errRegister)
	{
		printErrorMessage(0, "ZocClearBuffer", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCSYNCTIME", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSynctime", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("LOWER", ATTACScripting_Lower);
	if (errRegister)
	{
		printErrorMessage(0, "Lower", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("UPPER", ATTACScripting_Upper);
	if (errRegister)
	{
		printErrorMessage(0, "Upper", errRegister);
		return;
	}

	errRegister = RexxRegisterFunctionExe("ZOCGETFILENAME", ATTACScripting_ZocGetFilename);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetFilename", errRegister);
		return;
	}

	errRegister = RexxRegisterFunctionExe("ZOCRECEIVEBUF", ATTACScripting_ZocReceiveBuf);
	if (errRegister)
	{
		printErrorMessage(0, "ZocReceiveBuf", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ZOCGETINFO", ATTACScripting_ZocGetInfo);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetInfo", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCGETOPTION", ATTACScripting_ZocGetGlobalOption);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetOption", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCGETGLOBALOPTION", ATTACScripting_ZocGetGlobalOption);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetGlobalOption", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCSETOPTION", ATTACScripting_ZocSetGlobalOption);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetOption", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCSETGLOBALOPTION", ATTACScripting_ZocSetGlobalOption);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetGlobalOption", errRegister);
		return;
	}

	errRegister = RexxRegisterFunctionExe("ZOCSTRING", ATTACScripting_ZocString);
	if (errRegister)
	{
		printErrorMessage(0, "ZocString", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCWAITFORSEQ", ATTACScripting_ZocWaitForSeq);
	if (errRegister)
	{
		printErrorMessage(0, "ZocWaitForSeq", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCEXEC", ATTACScripting_ZocExec);
	if (errRegister)
	{
		printErrorMessage(0, "ZocExec", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZOCLISTFILES", ATTACScripting_ZocListFiles);
	if (errRegister)
	{
		printErrorMessage(0, "ZocListFiles", errRegister);
		return;
	}

	// ATTAC Specific REXX Command
	errRegister = RexxRegisterFunctionExe("ATTAC_Get_Ally_Info", ATTACScripting_ATTAC_Get_Ally_Info);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_Get_Ally_Info", errRegister);
		return;
	}

	errRegister = RexxRegisterFunctionExe("ATTAC_Get_Sector_Info", ATTACScripting_ATTAC_Get_Sector_Info);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_Get_Sector_Info", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_Get_Port_Info", ATTACScripting_ATTAC_Get_Port_Info);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_Get_Port_Info", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_Get_ZTM_Sector_Info", ATTACScripting_ATTAC_Get_ZTM_Sector_Info);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_Get_ZTM_Sector_Info", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_UpdateCounter", ATTACScripting_ATTAC_UpdateCounter);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_UpdateCounter", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_GetTotalSectors", ATTACScripting_ATTAC_GetTotalSectors);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_GetTotalSectors", errRegister);
		return;
	}
    errRegister = RexxRegisterFunctionExe("ATTAC_Message", ATTACScripting_ATTAC_Message);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_Message", errRegister);
		return;
	}

	errRegister = RexxRegisterFunctionExe("ATTAC_GetBuffer", ATTACScripting_ATTAC_GetBuffer);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_GetBuffer", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_ClearBuffer", ATTACScripting_ATTAC_ClearBuffer);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_ClearBuffer", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_StartBuffer", ATTACScripting_ATTAC_StartBuffer);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_StartBuffer", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_StopBuffer", ATTACScripting_ATTAC_StopBuffer);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_StopBuffer", errRegister);
		return;
	}

    errRegister = RexxRegisterFunctionExe("ATTAC_GetStardockSector", ATTACScripting_ATTAC_GetStardockSector);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_GetStardockSector", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_GetBestPairPort", ATTACScripting_ATTAC_GetBestPairPort);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_GetBestPairPort", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_GetBestEvilPairPort", ATTACScripting_ATTAC_GetBestEvilPairPort);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_GetBestEvilPairPort", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_HasSectorFig", ATTACScripting_ATTAC_HasSectorFig);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_HasSectorFig", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_HasSectorPlanet", ATTACScripting_ATTAC_HasSectorPlanet);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_HasSectorPlanet", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_SectorFigAmount", ATTACScripting_ATTAC_SectorFigAmount);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_SectorFigAmount", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_SectorFigOwner", ATTACScripting_ATTAC_SectorFigOwner);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_SectorFigOwner", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_SectorFigType", ATTACScripting_ATTAC_SectorFigType);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_SectorFigType", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_IsSectorBusted", ATTACScripting_ATTAC_IsSectorBusted);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_IsSectorBusted", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ATTAC_IsSectorAvoided", ATTACScripting_ATTAC_IsSectorAvoided);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_IsSectorAvoided", errRegister);
		return;
	}

	errRegister = RexxRegisterFunctionExe("ATTAC_IsSectorExplored", ATTACScripting_ATTAC_IsSectorExplored);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_IsSectorAvoided", errRegister);
		return;
	}
	
	errRegister = RexxRegisterFunctionExe("ATTAC_SetCounterMax", ATTACScripting_ATTAC_SetCounterMax);
	if (errRegister)
	{
		printErrorMessage(0, "ATTAC_SetCounterMax", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("dosfname", ATTACScripting_dosfname);
	if (errRegister)
	{
		printErrorMessage(0, "dosfname", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("dosisdir", ATTACScripting_dosisdir);
	if (errRegister)
	{
		printErrorMessage(0, "dosisdir", errRegister);
		return;
	}
	
//	errRegister = RexxRegisterFunctionExe("ATTAC_", ATTACScripting_FuncHandler);
//	errRegister = RexxRegisterFunctionExe("ATTAC_", ATTACScripting_FuncHandler);

	// These are Zoc Command that aren't supported

	errRegister = RexxRegisterFunctionExe("ZocAutoConnect", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocAutoConnect", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocBaud", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocBaud", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocCaptClr", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocCaptClr", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocCarrier", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocCarrier", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocConnect", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocConnect", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocCtrlString", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocCtrlString", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocCursor", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocCursor", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocDisconnect", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocDisconnect", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocDownload", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocDownload", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocEndZoc", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocEndZoc", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocGetFilenames", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetFilenames", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocGetPhonebk", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocGetPhonebk", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocHfc", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocHfc", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocLoadOpts", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocLoadOpts", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocPID", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocPID", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocRestimer", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocRestimer", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSendBreak", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSendBreak", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSendEmulationKey", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSendEmulationKey", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSendKey", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSendKey", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetAutoAccept", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetAutoAccept", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetDIPath", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetDIPath", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetDevice", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetDevice", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetDevParm", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetDevParm", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetEmu", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetEmu", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetHost", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetHost", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetPhonebk", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetPhonebk", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetTimer", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetTimer", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetUnit", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetUnit", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSetUnattended", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSetUnattended", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocShell", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocShell", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocSuppressOutput", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocSuppressOutput", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocUpload", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocUpload", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocXferShell", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocXferShell", errRegister);
		return;
	}
	errRegister = RexxRegisterFunctionExe("ZocXferDosShell", ATTACScripting_FuncHandler);
	if (errRegister)
	{
		printErrorMessage(0, "ZocXferDosShell", errRegister);
		return;
	}
	
	Exits[0].sysexit_name = "ExitHandler";
	Exits[0].sysexit_code = RXSIO;
	Exits[1].sysexit_code = RXENDLST;

	Instore[0].strptr = Script_Data;
	Instore[0].strlength = strlen(Script_Data);
	Instore[1].strptr = NULL;
	Instore[1].strlength = 0;

	Script_Running = 1;
	killedScriptWithEsc=0;

	REXX_Error_Code = RexxStart((long)0, (PRXSTRING) NULL, (PSZ) Script_Filename, Instore, (PSZ) "ATTAC", (LONG) RXCOMMAND, Exits, (PUSHORT) NULL, (PRXSTRING) &rexxstartRetStr);

	if ((long)(rexxstartRetStr.strlength)>0)
	{
		RexxFreeMemory(rexxstartRetStr.strptr);
	}

	Script_Running = 0;
	Still_Waiting = 0;

	errRegister = RexxDeregisterSubcom("ATTAC", NULL);
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC", errRegister);
		return;
	}
	errRegister = RexxDeregisterExit("ExitHandler", NULL);
	if (errRegister)
	{
		printErrorMessage(1, "ExitHandler", errRegister);
		return;
	}

	errRegister = RexxDeregisterFunction("ZOCASK");
	if (errRegister)
	{
		printErrorMessage(1, "ZocAsk", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCASKP");
	if (errRegister)
	{
		printErrorMessage(1, "ZocAskP", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCBEEP");
	if (errRegister)
	{
		printErrorMessage(1, "ZocBeep", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCCLS");
	if (errRegister)
	{
		printErrorMessage(1, "ZocCls", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCDELAY");
	if (errRegister)
	{
		printErrorMessage(1, "ZocDelay", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCGETLINE");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetLine", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCLASTLINE");
	if (errRegister)
	{
		printErrorMessage(1, "ZocLastLine", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCLOCKKEYBOARD");
	if (errRegister)
	{
		printErrorMessage(1, "ZocLockKeyboard", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCKEYBOARD");
	if (errRegister)
	{
		printErrorMessage(1, "ZocKeyboard", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCMSGBOX");
	if (errRegister)
	{
		printErrorMessage(1, "ZocMsgBox", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCNOTIFY");
	if (errRegister)
	{
		printErrorMessage(1, "ZocNotify", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCOS");
	if (errRegister)
	{
		printErrorMessage(1, "ZocOs", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCREQUEST");
	if (errRegister)
	{
		printErrorMessage(1, "ZocRequest", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCREQUESTLIST");
	if (errRegister)
	{
		printErrorMessage(1, "ZocRequestList", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCRESPOND");
	if (errRegister)
	{
		printErrorMessage(1, "ZocRespond", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCSCREEN");
	if (errRegister)
	{
		printErrorMessage(1, "ZocScreen", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCGETSCREEN");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetScreen", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCSEND");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSend", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSENDRAW");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSendRaw", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCTIMEOUT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocTimeout", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCWAIT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocWait", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCWAITMUX");
	if (errRegister)
	{
		printErrorMessage(1, "ZocWaitMux", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCWRITE");
	if (errRegister)
	{
		printErrorMessage(1, "ZocWrite", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCWRITELN");
	if (errRegister)
	{
		printErrorMessage(1, "ZocWriteln", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCLOGNAME");
	if (errRegister)
	{
		printErrorMessage(1, "ZocLogname", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCLOGGING");
	if (errRegister)
	{
		printErrorMessage(1, "ZocLogging", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCCLEARBUFFER");
	if (errRegister)
	{
		printErrorMessage(1, "ZocClearBuffer", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCSYNCTIME");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSynctime", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("LOWER");
	if (errRegister)
	{
		printErrorMessage(1, "Lower", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("UPPER");
	if (errRegister)
	{
		printErrorMessage(1, "Upper", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCGETFILENAME");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetFilename", errRegister);
		return;
	}

	errRegister = RexxDeregisterFunction("ZOCRECEIVEBUF");
	if (errRegister)
	{
		printErrorMessage(1, "ZocReceiveBuf", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ZOCGETINFO");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetInfo", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCGETOPTION");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetOption", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCGETGLOBALOPTION");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetGlobalOption", errRegister);
		return;
	}

	errRegister = RexxDeregisterFunction("ZOCSETOPTION");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetOption", errRegister);
		return;
	}

	errRegister = RexxDeregisterFunction("ZOCSETGLOBALOPTION");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetGlobalOption", errRegister);
		return;
	}
	

	errRegister = RexxDeregisterFunction("ZOCSTRING");
	if (errRegister)
	{
		printErrorMessage(1, "ZocString", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCWAITFORSEQ");
	if (errRegister)
	{
		printErrorMessage(1, "ZocWaitForSeq", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCEXEC");
	if (errRegister)
	{
		printErrorMessage(1, "ZocExec", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCLISTFILES");
	if (errRegister)
	{
		printErrorMessage(1, "ZocListFiles", errRegister);
		return;
	}



	// ATTAC Specific REXX Command
	errRegister = RexxDeregisterFunction("ATTAC_GET_ALLY_INFO");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_Get_Ally_Info", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_GET_SECTOR_INFO");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_Get_Sector_Info", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_GET_PORT_INFO");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_Get_Port_Info", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_GET_ZTM_SECTOR_INFO");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_Get_ZTM_Sector_Info", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_UPDATECOUNTER");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_UpdateCounter", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_GETTOTALSECTORS");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_GetTotalSectors", errRegister);
		return;
	}
    errRegister = RexxDeregisterFunction("ATTAC_MESSAGE");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_Message", errRegister);
		return;
	}

	errRegister = RexxDeregisterFunction("ATTAC_GETBUFFER");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_GetBuffer", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_CLEARBUFFER");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_ClearBuffer", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_STARTBUFFER");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_StartBuffer", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_STOPBUFFER");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_StopBuffer", errRegister);
		return;
	}

    errRegister = RexxDeregisterFunction("ATTAC_GETSTARDOCKSECTOR");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_GetStardockSector", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_GETBESTPAIRPORT");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_GetBestPairPort", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_GETBESTEVILPAIRPORT");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_GetBestEvilPairPort", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_HASSECTORPLANET");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_HasSectorPlanet", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_HASSECTORFIG");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_HasSectorFig", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_SECTORFIGAMOUNT");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_SectorFigAmount", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_SECTORFIGOWNER");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_SectorFigOwner", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_SECTORFIGTYPE");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_SectorFigType", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_ISSECTORBUSTED");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_IsSectorBusted", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_ISSECTORAVOIDED");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_IsSectorAvoided", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ATTAC_ISSECTOREXPLORED");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_IsSectorExplored", errRegister);
		return;
	}

	errRegister = RexxDeregisterFunction("ATTAC_SETCOUNTERMAX");
	if (errRegister)
	{
		printErrorMessage(1, "ATTAC_SetCounterMax", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("DOSFNAME");
	if (errRegister)
	{
		printErrorMessage(1, "dosfname", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("DOSISDIR");
	if (errRegister)
	{
		printErrorMessage(1, "dosisdir", errRegister);
		return;
	}
	
//	errRegister = RexxDeregisterFunction("ATTAC_", ATTACScripting_FuncHandler);
//	errRegister = RexxDeregisterFunction("ATTAC_", ATTACScripting_FuncHandler);

	// These are Zoc Command that aren't supported

	errRegister = RexxDeregisterFunction("ZOCAUTOCONNECT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocAutoConnect", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCBAUD");
	if (errRegister)
	{
		printErrorMessage(1, "ZocBaud", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCCAPTCLR");
	if (errRegister)
	{
		printErrorMessage(1, "ZocCaptClr", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCCARRIER");
	if (errRegister)
	{
		printErrorMessage(1, "ZocCarrier", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCCONNECT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocConnect", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCCTRLSTRING");
	if (errRegister)
	{
		printErrorMessage(1, "ZocCtrlString", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCCURSOR");
	if (errRegister)
	{
		printErrorMessage(1, "ZocCursor", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCDISCONNECT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocDisconnect", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCDOWNLOAD");
	if (errRegister)
	{
		printErrorMessage(1, "ZocDownload", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCENDZOC");
	if (errRegister)
	{
		printErrorMessage(1, "ZocEndZoc", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCGETFILENAMES");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetFilenames", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCGETPHONEBK");
	if (errRegister)
	{
		printErrorMessage(1, "ZocGetPhonebk", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCHFC");
	if (errRegister)
	{
		printErrorMessage(1, "ZocHfc", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCLOADOPTS");
	if (errRegister)
	{
		printErrorMessage(1, "ZocLoadOpts", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCPID");
	if (errRegister)
	{
		printErrorMessage(1, "ZocPID", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCRESTIMER");
	if (errRegister)
	{
		printErrorMessage(1, "ZocRestimer", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSENDBREAK");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSendBreak", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSENDEMULATIONKEY");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSendEmulationKey", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSENDKEY");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSendKey", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETAUTOACCEPT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetAutoAccept", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETDIPATH");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetDIPath", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETDEVICE");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetDevice", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETDEVPARM");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetDevParm", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETEMU");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetEmu", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETHOST");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetHost", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETPHONEBK");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetPhonebk", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETTIMER");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetTimer", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETUNIT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetUnit", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSETUNATTENDED");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSetUnattended", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSHELL");
	if (errRegister)
	{
		printErrorMessage(1, "ZocShell", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCSUPPRESSOUTPUT");
	if (errRegister)
	{
		printErrorMessage(1, "ZocSuppressOutput", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCUPLOAD");
	if (errRegister)
	{
		printErrorMessage(1, "ZocUpload", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCXFERSHELL");
	if (errRegister)
	{
		printErrorMessage(1, "ZocXferShell", errRegister);
		return;
	}
	errRegister = RexxDeregisterFunction("ZOCXFERDOSSHELL");
	if (errRegister)
	{
		printErrorMessage(1, "ZocXferDosShell", errRegister);
		return;
	}
	
	ATTACScripting_Cleanup();

	pfree(&Script_Data );
	pfree(&Script_Filename );

	REXX_KillScript();

    if (NonRegisteredREXXCount > MAX_NON_REGISTERED)
	{
		sprintf (buffer, "Script Stopped. You must restart the script or you can register ATTAC and this message will disappear");
		if (header.popupOnRexxCompletion==1)
			MessageBox(NULL, buffer , "ATTAC Error", MB_ICONERROR | MB_OK);
		else
			attacScriptMessage("ATTAC SCRIPT", buffer, ANSI_CYAN, BLACK, TRUE);
	}
    else if (REXX_Error_Code != 0 && killedScriptWithEsc==0)
	{
		if (REXX_Error_Code!= -1)
			sprintf (buffer, "Script Done.  Error Code: %ld", REXX_Error_Code);
		else 
			sprintf (buffer, "Script Done.");

		if (header.popupOnRexxCompletion==1)
			MessageBox(NULL, buffer, "ATTAC Error", MB_ICONERROR | MB_OK);
		else
			attacScriptMessage("ATTAC SCRIPT", buffer, ANSI_CYAN, BLACK, TRUE);
	}   
    else if (killedScriptWithEsc==0)
	{
		if (header.popupOnRexxCompletion==1)
			MessageBox(NULL, "Script Done", "ATTAC Error", MB_ICONEXCLAMATION | MB_OK);
		else
			attacScriptMessage("ATTAC SCRIPT", "Script Done", ANSI_CYAN, BLACK, TRUE);
	}

	_endthread();

}

APIRET APIENTRY ATTACScripting_Hook(PRXSTRING cmd, PUSHORT flags, PRXSTRING returnstring)
{
/*
	char buffer[1024];

	if (cmd->strlength>0 && cmd->strlength<900)
		sprintf (buffer, "Unknown Function %s", cmd->strptr);
	else
		sprintf (buffer, "Unknown Function");

	attacErrorMessage(buffer);
*/
	return(0);
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

long ATTACScripting_WaitForSendData(long timeout)
{
	long result;

	if (timeout == 0)
		timeout = INFINITE;
	result = WaitForSingleObject(Send_Data_Mutex, timeout);

	return result;
}

long ATTACScripting_ReleaseSendData()
{
	long result;

	result = ReleaseMutex(Send_Data_Mutex);

	return result;
}

long ATTACScripting_WaitForZocRespond(long timeout)
{
	long result;

	if (timeout == 0)
		timeout = INFINITE;
	result = WaitForSingleObject(Zoc_Respond_Mutex, timeout);

	return result;
}

long ATTACScripting_ReleaseZocRespond()
{
	long result;

	result = ReleaseMutex(Zoc_Respond_Mutex);

	return result;
}




long ATTACScripting_WaitForMemory(long timeout)
{
	long result;

	if (timeout == 0)
		timeout = INFINITE;
	result = WaitForSingleObject(Memory_Mutex, timeout);

	return result;
}

long ATTACScripting_ReleaseMemory()
{
	long result;

	result = ReleaseMutex(Memory_Mutex);

	return result;
}

long ATTACScripting_StillWaiting()
{
	long result;
	
	result = Still_Waiting || REXX_Waiting || REXX_WaitNextLine;

	return result;
}


long ATTAC_Time()
{
	time_t data;

	WaitForSingleObject(SyncDataMutex, INFINITE);
	time(&data);
	ReleaseMutex(SyncDataMutex);

	return data;
}

long ATTAC_XOR(unsigned long a, unsigned long b)
{
	return (long) a ^ b;
}


long ATTACScripting_ScriptRunning()
{
	return REXX_RunningScript;
}

long ATTACScripting_WaitForATTACWithREXXWaiting(long timeout)
{

	ResetEvent(Wait_Event);
	if ((REXX_WaitNextLine || REXX_Waiting) && WaitForSingleObject(Wait_Event, timeout) == WAIT_TIMEOUT)
	{
		Still_Waiting = 0;
		return WAIT_TIMEOUT;
	}
	Still_Waiting = 0;
	return 0;
}

long ATTACScripting_WaitForATTAC(long timeout)
{

	ResetEvent(Wait_Event);
	if (WaitForSingleObject(Wait_Event, timeout) == WAIT_TIMEOUT)
	{
		Still_Waiting = 0;
		return WAIT_TIMEOUT;
	}
	Still_Waiting = 0;
	return 0;
}

void ATTACScripting_SignalATTACComplete()
{
//	ATTACScripting_WaitForData(0);
//	if (1 || Still_Waiting || REXX_Waiting)
//	{ 
		SetEvent(Wait_Event);
//	}
//	ATTACScripting_ReleaseData();
}

void ATTACScripting_Cleanup()
{
	ATTACScripting_ReleaseData();
	CloseHandle(Data_Mutex);
	CloseHandle(Wait_Event);
	CloseHandle(Zoc_Respond_Mutex);
	CloseHandle(Memory_Mutex);

}

APIRET APIENTRY ATTACScripting_ZocSend(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *Msg, *val;
	int i;

	if (NonRegisteredREXXCount != -1)
		NonRegisteredREXXCount = NonRegisteredREXXCount + 1;

    //ZocSend
    if (numargs != 1 || REXX_RunningScript==0 || NonRegisteredREXXCount > MAX_NON_REGISTERED)
	{
		return(RXFUNC_BADTYPE);
	}

	ATTACScripting_WaitForData(0);
    
	val = (char *) strdup (args[0].strptr);

    Msg = Translate_String(val, 0, 0);
    for (i = 0; i < (int)strlen(Msg); i++)
	{
		ldisc_send (ldisc_extern, &Msg[i], 1, 1);
    }

	pfree(&Msg);	
	pfree(&val);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocSendRaw(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *val;
	int i;

	if (NonRegisteredREXXCount != -1)
		NonRegisteredREXXCount = NonRegisteredREXXCount + 1;

	
    //ZocSendRaw
    if (numargs != 1 || REXX_RunningScript==0 || NonRegisteredREXXCount > MAX_NON_REGISTERED)
	{
		return(RXFUNC_BADTYPE);
	}

    ATTACScripting_WaitForData(0);

	val = (char *) strdup (args[0].strptr);

    for (i = 0; i < (int) strlen(val); i++)
	{
		ldisc_send (ldisc_extern, &val[i], 1, 1);
    }

	pfree(&val);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ZocTimeout(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{

    //ZocTimeout
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	REXXtimeout = atof(args[0].strptr);
	if (REXXtimeout==-1)
		REXXtimeout=INFINITE;

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocListFiles(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char tmp[4096], buffer[4102];
    int i=0;
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

	strcpy (tmp, "");

    //ZocListFiles
    if (numargs <1 || numargs >2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	hFind=FindFirstFile(args[0].strptr, &FindFileData);

    do {
		if( FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
		{
			strcat(tmp, FindFileData.cFileName);
			if (strcmp(tmp, "") !=0)
			{
				if (numargs==1)
					strcat(tmp, " ");
				else
					strcat(tmp, args[1].strptr);
				i++;
			}
		}
    }while ( FindNextFile(hFind, &FindFileData) );
    FindClose(hFind);

	sprintf (buffer, "%d ", i);
	strcat (buffer, tmp);

	buffer[strlen(buffer)-1]=0;

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_FuncHandler(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[255];

	ATTACScripting_WaitForData(0);

	sprintf (buffer, "Call to unsupported REXX function %s", name);
	attacErrorMessage(buffer);
	
	ATTACScripting_ReleaseData();
	
	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocGetScreen(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *val=NULL;

	// ZocScreen
    if (numargs != 3 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}
  
	val = screen_xy (term, atoi(args[0].strptr), atoi(args[1].strptr), atoi(args[2].strptr));
    

	if (val!=NULL)
	{
		if (strlen(val) >= retstr->strlength)
		{
			retstr->strptr = RexxAllocateMemory(strlen(val)+1);
		}
		strcpy(retstr->strptr, val);
		retstr->strlength = strlen(val);
	}
	pfree (&val);

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ATTAC_Message(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	int fore=0;
	char *Msg, *val;
	char temp[MAX_LENGTH];

    if (numargs < 1 || numargs > 3 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }
	
	ATTACScripting_WaitForData(0);
	
	fore = ANSI_RED;

    if (numargs >= 2)
    {
      if (strcasecmp(args[1].strptr, "BLACK") == 0)
        fore = ANSI_BLACK;
      else if (strcasecmp(args[1].strptr, "RED") == 0)
        fore = ANSI_RED;
      else if (strcasecmp(args[1].strptr, "GREEN") == 0)
        fore = ANSI_GREEN;
      else if (strcasecmp(args[1].strptr, "YELLOW") == 0)
        fore = ANSI_YELLOW;
      else if (strcasecmp(args[1].strptr, "BLUE") == 0)
        fore = ANSI_BLUE;
      else if (strcasecmp(args[1].strptr, "MAGENTA") == 0)
        fore = ANSI_MAGENTA;
      else if (strcasecmp(args[1].strptr, "CYAN") == 0)
        fore = ANSI_CYAN;
      else if (strcasecmp(args[1].strptr, "WHITE") == 0)
        fore = ANSI_WHITE;
    }

    if (numargs >= 3)
    {
      val = (char *)strdup(args[2].strptr);
    }
    else
    {
      val = (char *) strdup("ATTAC SCRIPT");
    }
	if (args[0].strlength>0)
		strcpy(temp, args[0].strptr);
	else
		strcpy(temp, "");

	//add a CR LF to the end of the Msg

    Msg = Translate_String(temp, 0, 0);

	attacScriptMessage(val, Msg, fore, BLACK, TRUE);

	pfree(&Msg);
	pfree(&val);
	
	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocBeep(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	int Cnt=1;

    if (numargs > 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

    if (numargs == 1)
	{
        Cnt = atol(args[0].strptr);
        if (Cnt < 1)
            Cnt = 1;
    }
    while (Cnt>0)
	{
		beep(NULL, BELL_DEFAULT);
		My_Sleep(500);
        Cnt--;
	}

	/* Return successfully */
	return(0);
}
APIRET APIENTRY ATTACScripting_ZocCls(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	term_pwron(term);

	/* Return successfully */
	return(0);
}
APIRET APIENTRY ATTACScripting_ZocDelay(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	float tmp_float;
	long tmp_long;

	if (numargs > 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	if (numargs == 0)
		tmp_float =(float)  0.2;
	else
	{
		tmp_float = (float) atof(args[0].strptr);
	}

	tmp_long = (long) floor(tmp_float * 1000);

	Still_Waiting = 1;
	WaitForSingleObject(Wait_Event, tmp_long);
	Still_Waiting = 0;

	if (!ATTACScripting_ScriptRunning())
	{
		return(RXEXIT_RAISE_ERROR);
	}

	/* Return successfully */
	return(0);
}

LONG APIENTRY ATTACScripting_ExitHandler(LONG exNum, LONG subfun, PEXIT pBlock)
{
	struct RXFETCHLINE	fetch;
	char *Msg;
	char buf[3];

	/* Determine which type of operation -- ie, is it a SAY output, is it
	 * a PULL input, or is it interactive TRACE input?
	 */
	if (exNum == RXSIO)
	{
		switch (subfun)
		{
			/* It's the SAY instruction wanting us to output a line */
			case RXSIOSAY:
			case RXSIOTRC:
			{
				RXSIOSAY_PARM * psiosay;

				/* Redefine the PEXIT struct as what it really is -- a RXSIOSAY_PARM struct */
				psiosay = (RXSIOSAY_PARM *)pBlock;

				/* Just print the line in a message box */
				if (killedScriptWithEsc==0) {
					if (header.popupOnRexxCompletion==1) {
						if (encryptedScript==0)
							MessageBox(NULL, psiosay->rxsio_string.strptr, "ATTAC Error", MB_ICONERROR | MB_OK);
						else {
							if (NonRegisteredREXXCount > MAX_NON_REGISTERED)
								MessageBox(NULL, "Script stopped since ATTAC is not registered. Please restart script or register ATTAC for this message to disappear", "ATTAC Error", MB_ICONERROR | MB_OK);
							else
								MessageBox(NULL, "User stopped script with ESC or a error has occured in encrypted script", "ATTAC Error", MB_ICONERROR | MB_OK);
						}
					}
					else
					{
						buf[0]=13;
						buf[1]=10;
						buf[2]=0;

						Msg = Translate_String(buf, 0, 0);
						c_write(ldisc_extern, Msg, strlen(Msg));
						pfree(&Msg);

						if (encryptedScript==0)
							Msg = Translate_String(psiosay->rxsio_string.strptr, 0, 0);
						else {
							if (NonRegisteredREXXCount > MAX_NON_REGISTERED)
								Msg = Translate_String("Script stopped since ATTAC is not registered. Please restart script or register ATTAC for this message to disappear", 0, 0);
							else
								Msg = Translate_String("User stopped script with ESC or a error has occured in encrypted script", 0, 0);
						}
						c_write(ldisc_extern, Msg, strlen(Msg));
						pfree(&Msg);
					}
				}
				killedScriptWithEsc=1;
				break;
			}

			/* It's the PULL instruction wanting us to get a line from the user */
			case RXSIOTRD:
			{
				printf ("TEST\n");
				//not implemented yet
	/*
				RXSIOTRD_PARM		*psiotrd;
				int					code;

				// Redefine the PEXIT struct as what it really is -- a RXSIOTRD_PARM struct
				psiotrd = (RXSIOTRD_PARM *)pBlock;

				// Assume no input returned
				psiotrd->rxsiotrd_retc.strlength = 0;

				// Set up our RXFETCHLINE struct to pass appropriate data to EnterALineProc()
				fetch.title = "Enter a line for PULL:";
				fetch.retstr = &psiotrd->rxsiotrd_retc;

				// Get user's input via a dialog box with an edit control and Ok/Cancel buttons 
	fetch_it:	if ((code = DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ENTER_A_LINE), 0, EnterALineProc, (LPARAM)&fetch)) == -1)
				{
	*/

					/* An error. Tell the interpreter to raise the SYNTAX condition
					 * (which will abort the script if it doesn't trap SYNTAX) and
					 * display a generic error message associated with ANSI REXX
					 * General error number RXERR_SYSTEM_FAILURE.
					 *
					 * NOTE: If the script doesn't trap SYNTAX, us telling the
					 * interpreter to raise SYNTAX will cause the interpreter to
					 * eventually call MyRxsio() again (after MyRxsio() returns here)
					 * with a RXSIOTRC operation to display an error message
					 * associated with RXERR_SYSTEM_FAILURE
					 */
	/*
					return(RXEXIT_RAISE_ERROR);
				}
	*/
				/* Did he click upon the Cancel button? */
	//			if (!code)
	//			{
					/* Yes. Let's raise a HALT condition in the interpreter. Reginald
					 * offers a proprietary function, RexxRaiseCondition(), for
					 * this purpose which allows us a lot more flexibility in this
					 * regard than the standard API RexxSetHalt(). Besides raising
					 * the HALT condition, we could alternately choose to raise the
					 * SYNTAX condition with one of the ANSI REXX General Error
					 * numbers. You'll find those numbers defined in REXXSAA.H
					 * under the "EXTERNAL FUNCTION HANDLERS" section (for example,
					 * RXERR_STORAGE_EXHAUSTED). That allows us to abort the script's
					 * execution (if it doesn't trap SYNTAX) with a specific,
					 * meaningful error message. Contrast that to the only option
					 * that other interpreters allow for raising SYNTAX -- returning
					 * RXEXIT_RAISE_ERROR -- which always reports a ANSI GE error of
					 * RXERR_SYSTEM_FAILURE. (ie, No choice of error message there)
					 *
					 * Or, we can raise the HALT condition which will also abort
					 * the script (if it doesn't trap HALT), but specify any
					 * signal number. Contrast that to the only option that other
					 * interpreters allow for raising HALT -- calling RexxSetHalt()
					 * -- which always specifies a signal of "SIGINT".
					 *
					 * In conclusion, Reginald's RexxRaiseCondition() offers the ability
					 * to report a much wider range of error messages/numbers to the
					 * script, and thereby report much more detailed, accurate error
					 * feedback to the user.
					 *
					 * NOTE: If the script doesn't trap HALT, our raising HALT
					 * condition below will cause the interpreter to eventually call
					 * MyRxsio() again (after MyRxsio() returns here) with a RXSIOTRC
					 * operation to display an error message that the script has been
					 * aborted
					 */
	//				if (RexxRaiseConditionA) (*RexxRaiseConditionA)(REXXRAISE_HALT, (ULONG)SIGINT, 0, 0);
	//				else (*RexxSetHaltA)((LONG)GetCurrentProcess(), (LONG)GetCurrentThread());

					/* Since we just raised the condition ourselves, we return
					 * RXEXIT_HANDLED so that the interpreter doesn't also
					 * raise a condition (RXEXIT_RAISE_ERROR), nor attempt
					 * to handle inputting a line itself (RXEXIT_NOT_HANDLED)
					 */
	//			}

				/* If he clicked upon Ok, then the string has already been copied to the
				 * supplied RXSTRING and its strlength set in EnterALineProc(). All that
				 * is left is to return RXEXIT_HANDLED
				 */

	//			break;
			}

			default:
			{
				/* Hmmmmm. Must be some new REXX thing implemented after our program was written,
				 * Tell the interpreter to take care of this operation since we don't know what
				 * to do here
				 */

				Still_Waiting = 1;
		
				if (ATTACScripting_WaitForATTAC((long)REXXtimeout) == WAIT_TIMEOUT)
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
	}
	/* We successfully handled this operation. Let the interpreter
	 * continue running the script with the next operation
	 */
	return(RXEXIT_HANDLED);
}

APIRET APIENTRY ATTACScripting_ZocLockKeyboard(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
    // ZocLockKeyboard
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }
    
	ATTACScripting_WaitForData(0);

    if (strcmp (args[0].strptr, "1")==0 || strcmp (args[0].strptr, "LOCK")==0)
        REXX_LockKeyboard = 1;
    else
        REXX_LockKeyboard = 0;

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocMsgBox(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
    long Mode=0;
    char *Msg=NULL;
    long MsgMode=0;
	int  Return_Value=0;
	char buffer[10];

	// ZocMsgBox
    if (numargs < 1 || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	Mode = 0;

    if (numargs == 2) 
        Mode = atol(args[1].strptr);
    
	Msg = (char *) Translate_String(args[0].strptr, 0, 0);

    if (Mode == 1)
        MsgMode = MB_OK | MB_ICONSTOP;
    else if (Mode == 2)
        MsgMode = MB_YESNO;
    else
        MsgMode = MB_OK;
    
	Return_Value = MessageBox(NULL, Msg, "ATTAC Script Message", MsgMode);
    
	pfree(&Msg);

	if (Return_Value == (int) IDOK)
        sprintf (buffer,"##OK##");
    else if (Return_Value == (int) IDYES)
		sprintf (buffer,"##YES##");
    else
		sprintf (buffer,"##NO##");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ZocNotify(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
    char *Msg=NULL;

	// ZocNotify
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}
	Msg = Translate_String(args[0].strptr, 0, 0);
    
	showrexxnotify(globalHWnd, Msg);

	pfree(&Msg);

	My_Sleep(3000);

	SetActiveWindow(GetParent(rexxnotify));
	DestroyWindow(rexxnotify);

	rexxnotify = NULL;

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocOs(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];

	ATTACScripting_WaitForData(0);

	strcpy (buffer, "WINNT 4.00");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocWrite(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *Msg;

    //ZocWrite
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}
    
	ATTACScripting_WaitForData(0);

	Msg = Translate_String(args[0].strptr, 0, 0);

	c_write(ldisc_extern, Msg, strlen(Msg));

	pfree(&Msg);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}
APIRET APIENTRY ATTACScripting_ZocWriteln(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *Msg, *val;

    //ZocWriteln
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

    ATTACScripting_WaitForData(0);

	Msg = Translate_String(args[0].strptr, 0, 0);

	//add a CR LF to the end of the Msg
	val = (char*) malloc(strlen(Msg)+3);
	strncpy(val, Msg, strlen(Msg));

	val[strlen(Msg)+0] = 13;
	val[strlen(Msg)+1] = 10;
	val[strlen(Msg)+2] = 0;

	c_write(ldisc_extern, val, strlen(val));

	pfree(&val);
	pfree(&Msg);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_dosfname(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[MAX_LENGTH];

    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	ATTACScripting_WaitForData(0);

	strcpy (buffer, "WINNT 4.00");

	sprintf (buffer, "%s\\%s", exeDir, args[0].strptr);

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();
	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_dosisdir(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];

    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	sprintf (buffer, "%d", IsDirectory(args[0].strptr));

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocAsk(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
    if (numargs < 1 || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

    rexxAskPrompt = (char *) Translate_String(args[0].strptr, 0, 0);
    if (numargs == 2)
        rexxAskDefault = (char *) strdup (args[1].strptr);
    else
        rexxAskDefault = (char *) strdup ("");

	SetTimer(globalHWnd, IDC_TIMER_REXXASK, 1, ShowRexxAskTimer);

	Still_Waiting=1;
	if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
	{
		return RXFUNC_BADTYPE;
	}
	Still_Waiting=0;

	/* We return a value */
	if (strlen(rexxAskResult) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(rexxAskResult)+1);
	}
	strcpy(retstr->strptr, rexxAskResult);
	retstr->strlength = strlen(rexxAskResult);

	//free the result malloc'ed in callback
	pfree(&rexxAskResult);

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocAskP(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
    if (numargs < 1 || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

    rexxAskPrompt = (char *) Translate_String(args[0].strptr, 0, 0);
    if (numargs == 2)
        rexxAskDefault = (char *) strdup (args[1].strptr);
    else
        rexxAskDefault = (char *) strdup ("");
    
	SetTimer(globalHWnd, IDC_TIMER_REXXASKPASS, 1, ShowRexxAskPassTimer);

	Still_Waiting=1;
	if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
	{
		return RXFUNC_BADTYPE;
	}
	Still_Waiting=0;

	/* We return a value */
	if (strlen(rexxAskResult) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(rexxAskResult)+1);
	}
	strcpy(retstr->strptr, rexxAskResult);
	retstr->strlength = strlen(rexxAskResult);

	//free the result malloc'ed in callback
	pfree(&rexxAskResult);
	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocRequest(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	int I;

    if (numargs < 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	rexxRequestArgumentCount = numargs;

	for (I = 0; I < (int) numargs; I++)
	{
		rexxRequestArguments[I] = (char *) strdup (args[I].strptr);
	}

	rexxRequestList=0;

	SetTimer(globalHWnd, IDC_TIMER_REXXREQUEST, 1, ShowRexxRequestTimer);

	Still_Waiting=1;
	if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
	{
		return RXFUNC_BADTYPE;
	}
	Still_Waiting=0;

	/* We return a value */
	if (strlen(rexxRequestResult) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(rexxRequestResult)+1);
	}
	strcpy(retstr->strptr, rexxRequestResult);
	retstr->strlength = strlen(rexxRequestResult);

	//free the result malloc'ed in callback
	pfree(&rexxRequestResult);

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocRequestList(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	int I;
	char *token;

    if (numargs < 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	rexxRequestArgumentCount = numargs;

	if (rexxRequestArgumentCount==1)
	{
		//reset your parameters
		I=0;
		rexxRequestArgumentCount=0;
		token = strtok(args[0].strptr, "|");
		while (token != NULL)
		{
			rexxRequestArguments[I] = (char *) strdup (token);
			token = strtok(NULL, "|");
			//now increment them properly
			I=I+1;
			rexxRequestArgumentCount=rexxRequestArgumentCount+1;
		}
	}
	else
	{
		for (I = 0; I < (int)numargs; I++)
		{
			rexxRequestArguments[I] = (char *) strdup (args[I].strptr);
		}
	}
	rexxRequestList=1;

	SetTimer(globalHWnd, IDC_TIMER_REXXREQUEST, 1, ShowRexxRequestTimer);

	Still_Waiting=1;
	if (ATTACScripting_WaitForATTAC(INFINITE) == WAIT_TIMEOUT)
	{
		return RXFUNC_BADTYPE;
	}
	Still_Waiting=0;

	/* We return a value */
	if (strlen(rexxRequestResult) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(rexxRequestResult)+1);
	}
	strcpy(retstr->strptr, rexxRequestResult);
	retstr->strlength = strlen(rexxRequestResult);

	//free the result malloc'ed in callback
	pfree(&rexxRequestResult);

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ZocClearBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	int I;

    // ZocClearBuffer
	ATTACScripting_WaitForData(0);

    strcpy (REXX_Buffer, "");
    strcpy (REXX_Buffer_RegularCase, "");
		
	for(I = 0; I < Respond_Cnt; I++)
		Respond_Start[I] = 1;

	Update_LastLine(strlen(REXX_Buffer), 0);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}	


APIRET APIENTRY ATTACScripting_ATTAC_Get_Ally_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *returnStr;
	char buffer[20];
	int i, figOwner=-1;

    if( numargs < 1 || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	ATTACScripting_WaitForData(0);

    returnStr = (char*) strdup(args[0].strptr);

	for (i=0; i<MAX_PLAYERS; i++) {
		if (players[i].autonum >=0 && (strcmp (players[i].name, returnStr)==0)) {
			figOwner = i;
			i = MAX_PLAYERS;
		}
	}
	pfree(&returnStr);

	sprintf (buffer, "%d", isPlayerAnEnemy(figOwner));
	
	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_Get_Sector_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	long sSector;
	int tmpNum;
	char buffer[MAX_LENGTH/4];

	strcpy (buffer, "");

    if( numargs < 1 || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	ATTACScripting_WaitForData(0);

    sSector = atol(args[0].strptr);

    if(sSector <= 0 || sSector > header.sector_cnt)
	{
		return(RXFUNC_BADTYPE);
	}

    if (numargs == 1)
	{
        if (sectors[sSector-1].warps < 0)
			sprintf (buffer, "0");
        else
		{
			sprintf (buffer, "%d", sectors[sSector-1].warps);
		}
	}
    else if (numargs == 2)
	{
        strcpy (buffer, "0");
        tmpNum = atoi(args[1].strptr);

		if (tmpNum >= 1 && tmpNum <= 6)
		{
			sprintf (buffer, "%ld", sectors[sSector-1].warp_sect[tmpNum-1]);
		}
    }

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_Get_Port_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	long sSector;
	char buffer[MAX_LENGTH/4];

	strcpy (buffer, "");

    if( numargs != 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	ATTACScripting_WaitForData(0);

    sSector = atol(args[0].strptr);

    if(sSector <= 0 || sSector > header.sector_cnt)
	{
		return(RXFUNC_BADTYPE);
	}

    if (sectors[sSector-1].info==-1)
        strcpy (buffer, "Unknown");
    else if (sectors[sSector-1].info==10)
        strcpy (buffer, "NoPort");
    else
	{
        if (strcmp (args[1].strptr,"TYPE")==0)
		{
			sprintf (buffer, "%d", sectors[sSector-1].info);
		}
        else if (strcmp (args[1].strptr, "FUEL AMOUNT")==0)
		{
			sprintf (buffer, "%ld", sectors[sSector-1].port_amt[0]);
		}
        else if (strcmp (args[1].strptr, "FUEL PERCENT")==0)
		{
			sprintf (buffer, "%d", sectors[sSector-1].port_per[0]);
		}
        else if (strcmp (args[1].strptr, "ORG AMOUNT")==0)
		{
			sprintf (buffer, "%ld", sectors[sSector-1].port_amt[1]);
		}
        else if (strcmp (args[1].strptr, "ORG PERCENT")==0)
		{
			sprintf (buffer, "%d", sectors[sSector-1].port_per[1]);
		}
        else if (strcmp (args[1].strptr, "EQUIP AMOUNT")==0)
		{
			sprintf (buffer, "%ld", sectors[sSector-1].port_amt[2]);
		}
        else if (strcmp (args[1].strptr, "EQUIP PERCENT")==0)
		{
			sprintf (buffer, "%d", sectors[sSector-1].port_per[2]);
		}
	}

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ATTAC_Get_ZTM_Sector_Info(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	long sSector;
	int tmpNum;
	char buffer[MAX_LENGTH/4];

	strcpy (buffer, "");

    if( numargs < 1 || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}

	ATTACScripting_WaitForData(0);

    sSector = atol(args[0].strptr);

    if(sSector <= 0 || sSector > header.sector_cnt)
	{
		return(RXFUNC_BADTYPE);
	}

    if (numargs == 1)
	{
        if (sectors[sSector-1].warps < 0)
            strcpy (buffer, "0");
        else
		{
			sprintf (buffer, "%d", sectors[sSector-1].warps);
		}
	}
    else if (numargs == 2)
	{
        strcpy (buffer, "0");
        tmpNum = atoi(args[1].strptr);

		if (tmpNum >= 1 && tmpNum <= 6)
		{
			sprintf (buffer, "%ld", sectors[sSector-1].warp_sect[tmpNum-1]);
		}
    }


	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_UpdateCounter(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	int I;
	char buffer[MAX_LENGTH];

    // ATTAC_UpdateCounter
    if (numargs > 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

    I = 1;
    
    if (numargs == 1)
        I = atoi(args[0].strptr);
            
    ScriptCounter = ScriptCounter + I;

	if (countMax==0)
		sprintf (buffer, "%d", ScriptCounter);
	else
		sprintf (buffer, "%d -- (%d%% Done)", ScriptCounter, (int) ((double)ScriptCounter*100/(double)countMax));

	setScriptMessage(buffer);

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_GetTotalSectors(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];

	//ATTAC_GetTotalSectors

	ATTACScripting_WaitForData(0);

	if (header.sector_cnt>0)
	{
		sprintf (buffer, "%ld", header.sector_cnt);
	}
	else
		sprintf (buffer, "0");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ATTAC_GetStardockSector(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];

	ATTACScripting_WaitForData(0);

	if (header.sector_cnt>0)
	{
		sprintf (buffer, "%ld", header.stardock);
	}
	else
		sprintf (buffer, "0");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();
	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_GetBestPairPort(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *tmp;

    // ATTAC_GetBestPairPort
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	if (atoi(args[0].strptr)<1 || atoi (args[0].strptr)>3)
	{
		attacErrorMessage ("Invalid type for ATTAC_GetBestPairPort");
		return(RXFUNC_BADTYPE);
	}

	tmp = generate_pp(-1, -1, 0, atoi(args[0].strptr), atoi(args[0].strptr));

	/* We return a value */
	if (strlen(tmp) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(tmp)+1);
	}
	strcpy(retstr->strptr, tmp);
	retstr->strlength = strlen(tmp);
	//we can free it here since the else clause exits the function
	pfree(&tmp);

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_GetBestEvilPairPort(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *tmp;

    // ATTAC_GetBestEvilPairPort
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	if (atoi (args[0].strptr) !=1)
	{
		attacErrorMessage ("Invalid type for ATTAC_GetBestEvilPairPort");
		return(RXFUNC_BADTYPE);
	}

	tmp = generate_evil_pp(-1, -1, 0, atoi(args[0].strptr));

	/* We return a value */
	if (strlen(tmp) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(tmp)+1);
	}
	strcpy(retstr->strptr, tmp);
	retstr->strlength = strlen(tmp);
	//we can free it here since the else clause exits the function
	pfree(&tmp);

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ATTAC_HasSectorFig(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_HasSectorFig
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	tempLong = atol(args[0].strptr);

	if (tempLong>0 && tempLong <= header.sector_cnt)
	{
		if (sectors[tempLong-1].ftr_owner==-1 || sectors[tempLong-1].ftrs==-1)
		{
			strcpy(buffer, "-1");
		}
		else if (sectors[tempLong-1].ftr_owner==0 || (sectors[tempLong-1].ftr_owner-CORP_CONSTANT)==players[0].corpNumber)
		{
			strcpy(buffer, "0");
		}
		else if (isPlayerAnEnemy(sectors[tempLong-1].ftr_owner)==0)
		{
			strcpy(buffer, "2");
		}
		else if (isPlayerAnEnemy(sectors[tempLong-1].ftr_owner)==1)
		{
			strcpy(buffer, "1");
		}
		else
		{
			strcpy(buffer, "-1");
		}
	}
	else
		strcpy(buffer, "-1");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();
	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_HasSectorPlanet(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_HasSectorPlanet
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	tempLong = atol(args[0].strptr);

	if (tempLong>0 && tempLong <= header.sector_cnt)
	{
		//-1 no owner
		//0  you /corp own it
		//1 enemy owned
		//2 allied owned
		/*
		int planetOwner=-1;
		if (listPlanets[tempLong] != NULL)
		{
			int tmpInt = get_link_list_length(i);
			for (k=0; k < tmpInt; k++)
			{
				tmpStr = (char *) get_link_list_nth_data (i, k);
				tp  = left (tmpStr, 127);
				strcpy(planets[j].name, tp);
				free(tmpStr);
				free(tp);
				printf ("planet %s in sector %ld\n", planets[j].name, i+1);
				j++;
			}
		}

		if (sectors[tempLong-1].ftr_owner==-1 || sectors[tempLong-1].ftrs==-1)
		{
			strcpy(buffer, "-1");
		}
		else if (sectors[tempLong-1].ftr_owner==0 || (sectors[tempLong-1].ftr_owner-CORP_CONSTANT)==players[0].corpNumber)
		{
			strcpy(buffer, "0");
		}
		else if (isPlayerAnEnemy(sectors[tempLong-1].ftr_owner)==0)
		{
			strcpy(buffer, "2");
		}
		else if (isPlayerAnEnemy(sectors[tempLong-1].ftr_owner)==1)
		{
			strcpy(buffer, "1");
		}
		else
		{
			strcpy(buffer, "-1");
		}
		*/
		strcpy(buffer, "-1");
	}
	else
		strcpy(buffer, "-1");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();
	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_SectorFigAmount(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_SectorFigAmount
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	tempLong = atol(args[0].strptr);
	if (tempLong>0 && tempLong <= header.sector_cnt)
		sprintf (buffer, "%ld", sectors[tempLong-1].ftrs);
	else
		strcpy (buffer, "-1");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();
	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_SectorFigOwner(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_SectorFigOwner
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	tempLong = atol(args[0].strptr);
	if (tempLong>0 && tempLong <= header.sector_cnt)
	{
		if (sectors[tempLong-1].ftrs ==-1)
			strcpy(buffer, "No Figs");
		else if (sectors[tempLong-1].ftr_owner ==-1)
			strcpy(buffer, "Unknown");
		else 
		{
			if (sectors[tempLong-1].ftr_owner>CORP_CONSTANT)
			{
				sprintf (buffer, "CORP #%d", sectors[tempLong-1].ftr_owner-CORP_CONSTANT);
			}
			else
			{
				strcpy (buffer, players[sectors[tempLong-1].ftr_owner].name);
			}
		}
	}
	else
		strcpy (buffer, "-1");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_SectorFigType(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_SectorFigType
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	tempLong = atol(args[0].strptr);
	
	if (tempLong>0 && tempLong <= header.sector_cnt)
		sprintf (buffer, "%d", (int)sectors[tempLong-1].ftr_type);
	else
		strcpy (buffer, "-1");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_IsSectorBusted(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_IsSectorBusted
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }
	
	ATTACScripting_WaitForData(0);

	tempLong = atol(args[0].strptr);

	sprintf (buffer, "%d", isBustedSector(tempLong));

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_IsSectorAvoided(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_IsSectorAvoided
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);
		
	tempLong = atol(args[0].strptr);

	if (tempLong>=0 && tempLong<header.sector_cnt)
		sprintf (buffer, "%d", sectors[tempLong-1].avoided);
	else
		strcpy(buffer, "-1");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ATTAC_IsSectorExplored(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	long tempLong;

	//ATTAC_IsSectorExplored
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);
		
	tempLong = atol(args[0].strptr);

	if (tempLong>=0 && tempLong<header.sector_cnt)
	{
		if (sectors[tempLong-1].info==-1 || sectors[tempLong-1].info==11)
			strcpy (buffer, "0");
	}
	else
		strcpy(buffer, "1");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_SetCounterMax(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//SetCounterMax
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	countMax = atol(args[0].strptr);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocWait(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];
	int j=0;

	strcpy(buffer, "");

	if (NonRegisteredREXXCount != -1)
		NonRegisteredREXXCount = NonRegisteredREXXCount + 1;


	//ZocWait
    if (numargs != 1 || REXX_RunningScript==0 || NonRegisteredREXXCount > MAX_NON_REGISTERED)
	{
		return(RXFUNC_BADTYPE);
    }

    ATTACScripting_WaitForData(0);
    
	ATTACScripting_WaitForMemory(0);
    REXX_Waiting_For_Cnt = 1;
	REXX_Waiting_For[0] = Translate_String((char *)strupr(args[0].strptr), 0, 1);
    ATTACScripting_ReleaseMemory();

    if (Instr(1, REXX_Buffer, REXX_Waiting_For[0]) != 0)
	{
        Update_LastLine(Instr(1, REXX_Buffer, REXX_Waiting_For[0]) + strlen(REXX_Waiting_For[0]) - 1, 0);

		sprintf (buffer, "0");

		/* We return a value */
		if (strlen(buffer) >= retstr->strlength)
		{
			retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
		}
		strcpy(retstr->strptr, buffer);
		retstr->strlength = strlen(buffer);

		ATTACScripting_WaitForMemory(0);
		for (j=0; j<REXX_Waiting_For_Cnt; j++)
			pfree(&REXX_Waiting_For[j]);
		REXX_Waiting_For_Cnt = 0;
		ATTACScripting_ReleaseMemory();

		ATTACScripting_ReleaseData();

		/* Return successfully */
		return(0);
		
    }
    
    REXX_Waiting = 1;


	ATTACScripting_ReleaseData();

	if (REXX_Waiting && ATTACScripting_WaitForATTACWithREXXWaiting((long)REXXtimeout*1000) == WAIT_TIMEOUT)
	{
		REXX_Waiting = 0;
		strcpy(buffer, "640");
	}
	else
	{
		REXX_Waiting = 0;

		if (REXX_RunningScript!=0)
		{
			strcpy (buffer, ZocWaitResult);
			pfree(&ZocWaitResult);
		}
	}

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}

	strcpy(retstr->strptr, buffer);

	retstr->strlength = strlen(buffer);

	ATTACScripting_WaitForMemory(0);
	for (j=0; j<REXX_Waiting_For_Cnt; j++)
		pfree(&REXX_Waiting_For[j]);
	REXX_Waiting_For_Cnt = 0;
	ATTACScripting_ReleaseMemory();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ZocWaitMux(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50], *token, tempStr[4096];
	long pos, Found, tempLong, I, exitLoop=1;
	int j=0; 

	strcpy(buffer, "");

	if (NonRegisteredREXXCount != -1)
		NonRegisteredREXXCount = NonRegisteredREXXCount + 1;

	//ZocWaitMux
    if (numargs < 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	ATTACScripting_WaitForMemory(0);
    REXX_Waiting_For_Cnt = numargs;

//    while (Respond() == 1);

    pos = strlen(REXX_Buffer);

    Found = -1;

	//check if we only have one item in zocwaitmux and it has a |
	if (numargs==1 && Instr (1, args[0].strptr, "|"))
	{
		strcpy (tempStr, args[0].strptr);
		token = strtok(tempStr, "|");
		I=0;
		while (token != NULL && exitLoop==1)
		{
			REXX_Waiting_For[I] = Translate_String((char *)strupr(token), 0, 1);

//			printf ("REXX_Waiting_For[I]: %s\n", REXX_Waiting_For[I]);

			tempLong = Instr(1, REXX_Buffer, REXX_Waiting_For[I]);

			if ((tempLong != 0) && (tempLong < pos))
			{
				pos = tempLong;
				Found = I;
				exitLoop=0;
			}
			else
				I++;
		token = strtok(NULL, "|");
		}
		REXX_Waiting_For_Cnt = I;
	}
	//otherwise
	else
	{
		for (I = 0; I< (int) numargs; I++)
		{
			REXX_Waiting_For[I] = Translate_String((char *)strupr(args[I].strptr), 0, 1);

			tempLong = Instr(1, REXX_Buffer, REXX_Waiting_For[I]);

			if ((tempLong != 0) && (tempLong < pos))
			{
				pos = tempLong;
				REXX_Waiting_For_Cnt = Found = I;
				I = numargs;
			}
		}
	}

	ATTACScripting_ReleaseMemory();

    if (Found != -1)
	{
		Update_LastLine(Instr(1, REXX_Buffer, REXX_Waiting_For[Found]) + strlen(REXX_Waiting_For[Found]) - 1, 0);

		sprintf (buffer, "%ld", Found);

		/* We return a value */
		if (strlen(buffer) >= retstr->strlength)
		{
			retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
		}
		strcpy(retstr->strptr, buffer);
		retstr->strlength = strlen(buffer);

		ATTACScripting_WaitForMemory(0);
		for (j=0; j<REXX_Waiting_For_Cnt; j++)
			pfree(&REXX_Waiting_For[j]);
		REXX_Waiting_For_Cnt = 0;
		ATTACScripting_ReleaseMemory();

		ATTACScripting_ReleaseData();

		/* Return successfully */
		return(0);
    }

    REXX_Waiting = 1;
    ATTACScripting_ReleaseData();

	if (REXX_Waiting && ATTACScripting_WaitForATTACWithREXXWaiting((long)REXXtimeout*1000) == WAIT_TIMEOUT)
	{
		REXX_Waiting = 0;
		strcpy(buffer, "640");
	}
	else
	{
		REXX_Waiting = 0;

		if (REXX_RunningScript !=0)
		{
			strcpy (buffer, ZocWaitResult);
			pfree(&ZocWaitResult);
		}
	}

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_WaitForMemory(0);
	for (j=0; j<REXX_Waiting_For_Cnt; j++)
		pfree(&REXX_Waiting_For[j]);
    REXX_Waiting_For_Cnt = 0;
	ATTACScripting_ReleaseMemory();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocGetLine(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[50];

	//ZocGetLine
    if (numargs != 0 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	REXX_WaitNextLine = 1;

	ATTACScripting_ReleaseData();

	if (REXX_WaitNextLine && ATTACScripting_WaitForATTACWithREXXWaiting((long)REXXtimeout*1000) == WAIT_TIMEOUT)
	{
		REXX_WaitNextLine = 0;
		strcpy(buffer, "640");
	}
	else
	{
		REXX_WaitNextLine = 0;
		strcpy(buffer, "0");
	}

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);


	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocLastLine(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ZocLastLine
    if (numargs != 0 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	/* We return a value */
	if (strlen(LastLine) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(LastLine)+1);
	}
	strcpy(retstr->strptr, LastLine);
	retstr->strlength = strlen(LastLine);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ZocRespond(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *Prompt;
	int breakLoop=0;
	long I, J;

    // ZocRespond
    if (numargs < 1 || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
	}
    
	ATTACScripting_WaitForZocRespond(0);
    ATTACScripting_WaitForData(0);

    if (numargs == 1)
	{
		Prompt = Translate_String((char *)strupr(args[0].strptr), 0, 0);

        for (I = 0; I< Respond_Cnt && breakLoop==0; I++)
		{
            if (strcmp (Respond_To[I], Prompt)==0)
			{
                if (I != Respond_Cnt-1)
				{
					pfree(&Respond_To[I]);
					pfree(&Respond_With[I]);
                    for (J = I + 1; J < Respond_Cnt; J++)
					{
                        Respond_To[J - 1] = Respond_To[J];		//copy pointers
                        Respond_With[J - 1] = Respond_To[J];	//copy pointers
                        Respond_Start[J - 1] = Respond_Start[J];//copy long
                    }
                }

                Respond_Cnt = Respond_Cnt - 1;

				Respond_To[Respond_Cnt]=NULL;
				Respond_With[Respond_Cnt]=NULL;
				Respond_Start[Respond_Cnt]=0;
				breakLoop = 1;
            }
        }
    }
	else
	{
		Prompt = Translate_String((char *)strupr(args[0].strptr), 0, 0);

        for (I = 0; I < Respond_Cnt && breakLoop == 0; I++)
		{
            if (strcmp(Respond_To[I], Prompt)==0)
			{
				pfree(&Respond_With[I]);
                Respond_With[I] = Translate_String((char *) args[1].strptr, 0, 0);
				breakLoop = 1;
            }
        }

		if (breakLoop == 0)
		{
			Respond_To[Respond_Cnt] = (char *) strdup(Prompt);
			Respond_With[Respond_Cnt] = Translate_String((char *) args[1].strptr, 0, 0);
			Respond_Start[Respond_Cnt] = 1;
			Respond_Cnt = Respond_Cnt + 1;
		}
    }

	pfree(&Prompt);

	ATTACScripting_ReleaseData();
	ATTACScripting_ReleaseZocRespond();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ZocLogname(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ZocLogging
    if (numargs != 1)
	{
		return(RXFUNC_BADTYPE);
    }

    ATTACScripting_WaitForData(0);

	strcpy (ZocLogName,args[0].strptr);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocLogging(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ZocLogging
    if (numargs <1 || numargs>2)
	{
		return(RXFUNC_BADTYPE);
    }

    ATTACScripting_WaitForData(0);
	
    if (atoi(args[0].strptr) == 0)
	{
        if (ZocLogging)
		{
            ZocLogging = 0;
            if (fpLogFile != NULL)
				fclose (fpLogFile);
			fpLogFile=NULL;
		}
	}   
    else
	{
        ZocLogging = 1;
        fpLogFile = fopen (ZocLogName, "w");
    }

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocString(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *t, *token, *finalResult, buffer[50];
	long pos=0;

	//ZocString
    if (numargs <3)
	{
		return(RXFUNC_BADTYPE);
    }

    ATTACScripting_WaitForData(0);

	if (strcmp(args[0].strptr, "PART")==0)
	{
		t = (char *) strdup (args[1].strptr);

		if (numargs>3)
			token = strtok(t, args[3].strptr);
		else
			token = strtok(t, " ");
		
		while (token != NULL)
		{
			pos++;
			if (atol(args[2].strptr)==pos)
				break;

			if (numargs>3)
				token = strtok(NULL, args[3].strptr);
			else
				token = strtok(NULL, " ");
		}
		pfree(&t);
		if (token==NULL)
		{
			finalResult = (char *) strdup ("");
		}
		else
			finalResult = (char *) strdup (token);
	}
	else if (strcmp(args[0].strptr, "PARTCOUNT")==0)
	{
		t = (char *) strdup (args[1].strptr);

		if (numargs>2)
			token = strtok(t, args[2].strptr);
		else
			token = strtok(t, " ");
		
		while (token != NULL)
		{
			pos++;

			if (numargs>2)
				token = strtok(NULL, args[2].strptr);
			else
				token = strtok(NULL, " ");
		}
		pfree(&t);

		sprintf (buffer, "%ld", pos);
		finalResult = (char *) strdup (buffer);

	}
	else if (strcmp(args[0].strptr, "REPLACE")==0)
	{
		t = replaceString(args[1].strptr, args[2].strptr, args[3].strptr);
		finalResult = (char *) strdup (t);
		pfree(&t);
	}
	else if (strcmp(args[0].strptr, "REMOVE")==0)
	{
		t = replaceString(args[1].strptr, args[2].strptr, "");
		finalResult = (char *) strdup (t);
		pfree(&t);
	}		
	else if (strcmp(args[0].strptr, "REMOVECHARS")==0)
	{
		t = replaceChars(args[1].strptr, args[2].strptr);
		finalResult = (char *) strdup (t);
		pfree(&t);
	}		
	else
		finalResult = (char *) strdup ("Invalid ZocString Option");

	/* We return a value */
	if (strlen(finalResult) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(finalResult)+1);
	}
	strcpy(retstr->strptr, finalResult);
	retstr->strlength = strlen(finalResult);

	pfree(&finalResult);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocWaitForSeq(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ZocWaitForSeq
    if (numargs != 1)
	{
		return(RXFUNC_BADTYPE);
    }

    ATTACScripting_WaitForData(0);
	
	ZocWaitForSeq=atoi(args[0].strptr);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocExec(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION procInfo;
    int success=0;
	char temp[MAX_LENGTH];

	//ZocExec
    if (numargs < 1 || numargs>2)
	{
		return(RXFUNC_BADTYPE);
    }
		
	ZeroMemory(&startupInfo, sizeof(startupInfo)); 
	GetStartupInfo(&startupInfo);
	startupInfo.cb = sizeof(startupInfo);

	if (numargs==2)
	{
		//0= normal, 1= hidden, 2= minimized, 3= maximized.
		if (atoi(args[1].strptr)==0)
			startupInfo.wShowWindow &= SW_NORMAL;
		else if (atoi(args[1].strptr)==1)
			startupInfo.wShowWindow &= SW_HIDE;
		else if (atoi(args[1].strptr)==2)
			startupInfo.wShowWindow &= SW_MINIMIZE;
		else if (atoi(args[1].strptr)==3)
			startupInfo.wShowWindow &= SW_MAXIMIZE;
		else
			startupInfo.wShowWindow &= SW_NORMAL;

	}
	else
		startupInfo.wShowWindow &= SW_NORMAL;

	success = CreateProcess(NULL,
			args[0].strptr,
			NULL, NULL, FALSE,
			NORMAL_PRIORITY_CLASS,
			NULL, NULL,
			&startupInfo,
			&procInfo);

	if (success)
		WaitForSingleObject(procInfo.hProcess, INFINITE);
	else
	{
		sprintf (temp, "Process %s was not properly created", args[0].strptr);
		attacScriptMessage("ATTAC SCRIPT", temp, ANSI_RED, BLACK, TRUE);
	}

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_Lower(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *buffer;

	//Lower
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);
		
	buffer = (char *) strdup((char *) strlwr(args[0].strptr));

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);
	
	pfree(&buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_Upper(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *buffer;

	//Upper
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);
		
	buffer = (char *) strdup((char *) strupr(args[0].strptr));

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);
	
	pfree(&buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_StartBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ATTAC_StartBuffer
    if (REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

    strcpy(ZocBuffer, REXX_Buffer);
    ZocBuffering = 1;

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ATTAC_StopBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ATTAC_StopBuffer
    if (REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

    ZocBuffering = 0;

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ATTAC_GetBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ATTAC_GetBuffer
    if (REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	/* We return a value */
	if (strlen(ZocBuffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(ZocBuffer)+1);
	}
	strcpy(retstr->strptr, ZocBuffer);
	retstr->strlength = strlen(ZocBuffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ATTAC_ClearBuffer(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	long pos;
	char *tmp;

	//ATTAC_ClearBuffer
    if (numargs > 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

    pos = strlen(ZocBuffer);
    
    if( numargs == 1)
	{
        pos = atol(args[0].strptr);
        if (pos > (int) strlen(ZocBuffer))
            pos = strlen(ZocBuffer);
    }
    
    tmp = right(ZocBuffer, strlen(ZocBuffer) - pos);
	strcpy (ZocBuffer, tmp);
	pfree(&tmp);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


APIRET APIENTRY ATTACScripting_ZocGetFilename(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	OPENFILENAME of;
	char filename[MAX_LENGTH], buffer[MAX_LENGTH], filter[MAX_LENGTH], nullChar;

	// ZocGetFilename

    if (numargs == 0  || numargs > 2 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	nullChar = '\x0';
	
	sprintf(filter, "REXX Files%c*.zrx%cAll Files%c*%c%c%c", nullChar, nullChar, nullChar, nullChar, nullChar, nullChar);

	memset(&of, 0, sizeof(of));
#ifdef OPENFILENAME_SIZE_VERSION_400
	of.lStructSize = OPENFILENAME_SIZE_VERSION_400;
#else
	of.lStructSize = sizeof(of);
#endif
	of.hwndOwner = NULL;
	
	of.lpstrFilter = filter;
	of.lpstrCustomFilter = NULL;
	of.nFilterIndex = 2;
	if (numargs==2)
		strcpy (filename, args[1].strptr);
	else
		strcpy (filename, "");
	of.lpstrFile = filename;
	of.nMaxFile = sizeof(filename);
	of.lpstrFileTitle = NULL;
	of.lpstrInitialDir = NULL;
	of.lpstrTitle = args[0].strptr;
	of.Flags = 0;
	if (GetOpenFileName(&of)) {
		strcpy(buffer, filename);
	}		
	else
		strcpy(buffer, "##CANCEL##");


	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocReceiveBuf(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	//ZocReceiveBuf
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	if (atoi(args[0].strptr)==0)
	{
		//pfree the receive buffer and set the result if there is one
		if (ZocReceiveBuf != NULL)
		{
			/* We return a value */
			if (strlen(ZocReceiveBuf) >= retstr->strlength)
			{
				retstr->strptr = RexxAllocateMemory(strlen(ZocReceiveBuf)+1);
			}
			strcpy(retstr->strptr, ZocReceiveBuf);
			retstr->strlength = strlen(ZocReceiveBuf);

			pfree (&ZocReceiveBuf);
			ZocReceiveBuf=NULL;
		}

		ZocReceiveBufCurr=0;
		ZocBufferStatus=0;
	}
	else
	{
		if (ZocReceiveBuf!=NULL)
		{
//				printf ("ZocReceiveBuf: %s\n",ZocReceiveBuf);
			/* We return a value */
			if (strlen(ZocReceiveBuf) >= retstr->strlength)
			{
				retstr->strptr = RexxAllocateMemory(strlen(ZocReceiveBuf)+1);
			}
			strcpy(retstr->strptr, ZocReceiveBuf);
			retstr->strlength = strlen(ZocReceiveBuf);
			
			pfree(&ZocReceiveBuf);
			ZocReceiveBuf=NULL;
		}

		ZocReceiveBufMax = atol(args[0].strptr);
		ZocReceiveBufCurr=0;
		ZocReceiveBuf = (char *) malloc (ZocReceiveBufMax+1);
		strcpy (ZocReceiveBuf, "");
		ZocBufferStatus=1;
	}

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocSetGlobalOption(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char *tmp;

	//ZocSetGlobalOption
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }


	ATTACScripting_WaitForData(0);

	if (Instr(1, "CaptDefaultName", args[0].strptr)==0)
	{
		tmp = right(args[0].strptr, strlen(args[0].strptr)-Instr(1, "=", args[0].strptr));
		cfg.logfilename = filename_from_str(tmp);
		pfree(&tmp);
	}


	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);

}


APIRET APIENTRY ATTACScripting_ZocGetGlobalOption(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[256];

	//ZocGetGlobalOption
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	if (strcmp(args[0].strptr, "VTStripSeq")==0)
		strcpy(buffer, "VTStripSeq=\"yes\"");
//	else if (strcmp(args[0].strptr, "CaptDefaultName")==0)
//		sprintf (buffer, "CaptDefaultName=\"%s\"", filename_to_str(cfg->logfilename));
	else if (strcmp(args[0].strptr, "ScriptPath")==0)
		strcpy(buffer, "ScriptPath=\"scripts\"");
	else if (strcmp(args[0].strptr, "CapturePath")==0)
		strcpy(buffer, "CapturePath=\"datafiles\"");
	else if (strcmp(args[0].strptr, "DownloadAltPath")==0)
		strcpy(buffer, "DownloadAltPath=\"\\datafiles\"");
	else if (strcmp(args[0].strptr, "ScriptPath")==0)
		strcpy(buffer, "ScriptPath=\"scripts\"");
	else if (strcmp(args[0].strptr, "WindowTitle")==0)
	{
		sprintf(buffer, "WindowTitle=\"%%ZOCORHOST%% %%OPTIONS%%\"");
	}
	else
		strcpy(buffer, "Invalid ZocGetOption");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}

APIRET APIENTRY ATTACScripting_ZocGetInfo(PUCHAR name, ULONG numargs, RXSTRING args[], PSZ queuename, PRXSTRING retstr)
{
	char buffer[MAX_LENGTH];

	//ZocGetInfo
    if (numargs != 1 || REXX_RunningScript==0)
	{
		return(RXFUNC_BADTYPE);
    }

	ATTACScripting_WaitForData(0);

	if (strcmp(args[0].strptr, "VERSION")==0)
		strcpy(buffer, ver);
	else if (strcmp(args[0].strptr, "CURSOR-X")==0)
	{
		sprintf (buffer, "%d", getXCursor(term));
	}
	else if (strcmp(args[0].strptr, "CURSOR-Y")==0)
	{
		sprintf (buffer, "%d", getYCursor(term));
	}
    else if (strcmp(args[0].strptr, "OSYS")==0)
		strcpy(buffer, "Windows");
	else if (strcmp(args[0].strptr, "COMHANDLE")==0)
		strcpy(buffer, "NULL");
	else if (strcmp(args[0].strptr, "DOWNLOADDIR")==0)
		strcpy(buffer, appDir);
	else if (strcmp(args[0].strptr, "EXEDIR")==0)
		strcpy(buffer, exeDir);
	else if (strcmp(args[0].strptr, "WORKDIR")==0)
		strcpy(buffer, exeDir);
	else if (strcmp(args[0].strptr, "CONNECTEDTO")==0)
		strcpy(buffer, gameName);
	else if (strcmp(args[0].strptr, "ONLINE")==0)
	{
		if (isConnected)
			strcpy(buffer, "##YES##");
		else
			strcpy(buffer, "##NO##");
	}
	else
		strcpy(buffer, "Invalid ZocGetInfo");

	/* We return a value */
	if (strlen(buffer) >= retstr->strlength)
	{
		retstr->strptr = RexxAllocateMemory(strlen(buffer)+1);
	}
	strcpy(retstr->strptr, buffer);
	retstr->strlength = strlen(buffer);

	ATTACScripting_ReleaseData();

	/* Return successfully */
	return(0);
}


